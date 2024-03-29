#include "libdpf.h"

#include <omp.h>
#include <stdio.h>
#include <string.h>

#include "block.h"

#define max(a, b)                   \
    (                               \
        {                           \
            __typeof__(a) _a = (a); \
            __typeof__(b) _b = (b); \
            _a > _b ? _a : _b;      \
        })

block dpf_reverse_lsb(block input) {
    static long long b1 = 0;
    static long long b2 = 1;
    block _xor = dpf_make_block(b1, b2);
    return dpf_xor(input, _xor);
}

block dpf_set_lsb_zero(block input) {
    int lsb = dpf_lsb(input);

    if (lsb == 1) {
        return dpf_reverse_lsb(input);
    } else {
        return input;
    }
}

void PRG(AES_KEY *key, block input, block *output1, block *output2, int *bit1,
         int *bit2) {
    input = dpf_set_lsb_zero(input);

    block stash[2];
    stash[0] = input;
    stash[1] = dpf_reverse_lsb(input);

    AES_ecb_encrypt_blks(stash, 2, key);

    stash[0] = dpf_xor(stash[0], input);
    stash[1] = dpf_xor(stash[1], input);
    stash[1] = dpf_reverse_lsb(stash[1]);

    *bit1 = dpf_lsb(stash[0]);
    *bit2 = dpf_lsb(stash[1]);

    *output1 = dpf_set_lsb_zero(stash[0]);
    *output2 = dpf_set_lsb_zero(stash[1]);
}

static int getbit(long x, int n, int b) {
    return (x >> (n - b)) & 1;
}

int GEN(AES_KEY *key, long alpha, int n, unsigned char **k0,
        unsigned char **k1) {
    int maxlayer = max(n - 7, 0);
    //int maxlayer = n;

    block s[maxlayer + 1][2];
    int t[maxlayer + 1][2];
    block sCW[maxlayer];
    int tCW[maxlayer][2];

    s[0][0] = dpf_random_block();
    s[0][1] = dpf_random_block();
    t[0][0] = dpf_lsb(s[0][0]);
    t[0][1] = t[0][0] ^ 1;
    s[0][0] = dpf_set_lsb_zero(s[0][0]);
    s[0][1] = dpf_set_lsb_zero(s[0][1]);

    int i;
    block s0[2], s1[2];  // 0=L,1=R
#define LEFT 0
#define RIGHT 1
    int t0[2], t1[2];
    for (i = 1; i <= maxlayer; i++) {
        PRG(key, s[i - 1][0], &s0[LEFT], &s0[RIGHT], &t0[LEFT], &t0[RIGHT]);
        PRG(key, s[i - 1][1], &s1[LEFT], &s1[RIGHT], &t1[LEFT], &t1[RIGHT]);

        int keep, lose;
        int alphabit = getbit(alpha, n, i);
        if (alphabit == 0) {
            keep = LEFT;
            lose = RIGHT;
        } else {
            keep = RIGHT;
            lose = LEFT;
        }

        sCW[i - 1] = dpf_xor(s0[lose], s1[lose]);

        tCW[i - 1][LEFT] = t0[LEFT] ^ t1[LEFT] ^ alphabit ^ 1;
        tCW[i - 1][RIGHT] = t0[RIGHT] ^ t1[RIGHT] ^ alphabit;

        if (t[i - 1][0] == 1) {
            s[i][0] = dpf_xor(s0[keep], sCW[i - 1]);
            t[i][0] = t0[keep] ^ tCW[i - 1][keep];
        } else {
            s[i][0] = s0[keep];
            t[i][0] = t0[keep];
        }

        if (t[i - 1][1] == 1) {
            s[i][1] = dpf_xor(s1[keep], sCW[i - 1]);
            t[i][1] = t1[keep] ^ tCW[i - 1][keep];
        } else {
            s[i][1] = s1[keep];
            t[i][1] = t1[keep];
        }
    }

    block finalblock;
    finalblock = dpf_zero_block();
    finalblock = dpf_reverse_lsb(finalblock);

    char shift = (alpha)&127;
    if (shift & 64) {
        finalblock = dpf_left_shirt(finalblock, 64);
    }
    if (shift & 32) {
        finalblock = dpf_left_shirt(finalblock, 32);
    }
    if (shift & 16) {
        finalblock = dpf_left_shirt(finalblock, 16);
    }
    if (shift & 8) {
        finalblock = dpf_left_shirt(finalblock, 8);
    }
    if (shift & 4) {
        finalblock = dpf_left_shirt(finalblock, 4);
    }
    if (shift & 2) {
        finalblock = dpf_left_shirt(finalblock, 2);
    }
    if (shift & 1) {
        finalblock = dpf_left_shirt(finalblock, 1);
    }
    //dpf_cb(finalblock);
    finalblock = dpf_reverse_lsb(finalblock);

    finalblock = dpf_xor(finalblock, s[maxlayer][0]);
    finalblock = dpf_xor(finalblock, s[maxlayer][1]);

    unsigned char *buff0;
    unsigned char *buff1;
    int size = 1 + 16 + 1 + 18 * maxlayer + 16;
    //	buff0 = (unsigned char*) malloc(size);
    //	buff1 = (unsigned char*) malloc(size);
    buff0 = new unsigned char[size];
    buff1 = new unsigned char[size];

    if (buff0 == NULL || buff1 == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    buff0[0] = n;
    memcpy(&buff0[1], &s[0][0], 16);
    buff0[17] = t[0][0];
    for (i = 1; i <= maxlayer; i++) {
        memcpy(&buff0[18 * i], &sCW[i - 1], 16);
        buff0[18 * i + 16] = tCW[i - 1][0];
        buff0[18 * i + 17] = tCW[i - 1][1];
    }
    memcpy(&buff0[18 * maxlayer + 18], &finalblock, 16);

    buff1[0] = n;
    memcpy(&buff1[18], &buff0[18], 18 * (maxlayer));
    memcpy(&buff1[1], &s[0][1], 16);
    buff1[17] = t[0][1];
    memcpy(&buff1[18 * maxlayer + 18], &finalblock, 16);

    *k0 = buff0;
    *k1 = buff1;

    return size;
}

block EVAL(AES_KEY *key, unsigned char *k, long x) {
    int n = k[0];
    int maxlayer = max(n - 7, 0);

    block s[maxlayer + 1];
    int t[maxlayer + 1];
    block sCW[maxlayer];
    int tCW[maxlayer][2];
    block finalblock;

    memcpy(&s[0], &k[1], 16);
    t[0] = k[17];

    int i;
    for (i = 1; i <= maxlayer; i++) {
        memcpy(&sCW[i - 1], &k[18 * i], 16);
        tCW[i - 1][0] = k[18 * i + 16];
        tCW[i - 1][1] = k[18 * i + 17];
    }

    memcpy(&finalblock, &k[18 * (maxlayer + 1)], 16);

    block sL, sR;
    int tL, tR;
    for (i = 1; i <= maxlayer; i++) {
        PRG(key, s[i - 1], &sL, &sR, &tL, &tR);

        if (t[i - 1] == 1) {
            sL = dpf_xor(sL, sCW[i - 1]);
            sR = dpf_xor(sR, sCW[i - 1]);
            tL = tL ^ tCW[i - 1][0];
            tR = tR ^ tCW[i - 1][1];
        }

        int xbit = getbit(x, n, i);
        if (xbit == 0) {
            s[i] = sL;
            t[i] = tL;
        } else {
            s[i] = sR;
            t[i] = tR;
        }
    }

    block res;
    res = s[maxlayer];
    if (t[maxlayer] == 1) {
        res = dpf_reverse_lsb(res);
    }

    if (t[maxlayer] == 1) {
        res = dpf_xor(res, finalblock);
    }

    return res;
}

block *EVALFULL(AES_KEY *key, const unsigned char *k) {
    int n = k[0];
    int maxlayer = max(n - 7, 0);
    long maxlayeritem = 1 << maxlayer;

    //block s[2][maxlayeritem];
    //int t[2][maxlayeritem];
    block *s[2];
    int *t[2];
    for (int ii = 0; ii < 2; ii++) {
        s[ii] = new block[maxlayeritem];
        t[ii] = new int[maxlayeritem];
    }

    int curlayer = 1;

    block sCW[maxlayer];
    int tCW[maxlayer][2];
    block finalblock;

    memcpy(&s[0][0], &k[1], 16);
    t[0][0] = k[17];

    int i;
    long j;
    for (i = 1; i <= maxlayer; i++) {
        memcpy(&sCW[i - 1], &k[18 * i], 16);
        tCW[i - 1][0] = k[18 * i + 16];
        tCW[i - 1][1] = k[18 * i + 17];
    }

    memcpy(&finalblock, &k[18 * (maxlayer + 1)], 16);

    //block sL, sR;
    //int tL, tR;
    for (i = 1; i <= maxlayer; i++) {
        long itemnumber = 1 << (i - 1);
        //#pragma omp parallel for
        for (j = 0; j < itemnumber; j++) {
            block sL, sR;
            int tL, tR;
            PRG(key, s[1 - curlayer][j], &sL, &sR, &tL, &tR);

            if (t[1 - curlayer][j] == 1) {
                sL = dpf_xor(sL, sCW[i - 1]);
                sR = dpf_xor(sR, sCW[i - 1]);
                tL = tL ^ tCW[i - 1][0];
                tR = tR ^ tCW[i - 1][1];
            }

            s[curlayer][2 * j] = sL;
            t[curlayer][2 * j] = tL;
            s[curlayer][2 * j + 1] = sR;
            t[curlayer][2 * j + 1] = tR;
        }
        curlayer = 1 - curlayer;
    }

    long itemnumber = 1 << maxlayer;
    block *res = (block *)malloc(sizeof(block) * itemnumber);

    //#pragma omp parallel for
    for (j = 0; j < itemnumber; j++) {
        res[j] = s[1 - curlayer][j];

        if (t[1 - curlayer][j] == 1) {
            res[j] = dpf_reverse_lsb(res[j]);
        }

        if (t[1 - curlayer][j] == 1) {
            res[j] = dpf_xor(res[j], finalblock);
        }
    }

    for (int ii = 0; ii < 2; ii++) {
        delete[] s[ii];
        delete[] t[ii];
    }

    return res;
}

void test_libdpf() {
    long long userkey1 = 597349;
    long long userkey2 = 121379;
    block userkey = dpf_make_block(userkey1, userkey2);

    dpf_seed(NULL);

    AES_KEY key;
    AES_set_encrypt_key(userkey, &key);

    unsigned char *k0;
    unsigned char *k1;

    GEN(&key, 1, 1, &k0, &k1);

    block res1;
    block res2;

    res1 = EVAL(&key, k0, 0);
    res2 = EVAL(&key, k1, 0);
    dpf_cb(res1);
    dpf_cb(res2);
    dpf_cb(dpf_xor(res1, res2));

    res1 = EVAL(&key, k0, 128);
    res2 = EVAL(&key, k1, 128);
    dpf_cb(res1);
    dpf_cb(res2);
    dpf_cb(dpf_xor(res1, res2));

    block *resf0, *resf1;
    resf0 = EVALFULL(&key, k0);
    resf1 = EVALFULL(&key, k1);

    long j;
    for (j = 0; j < 1; j++) {
        printf("Group %ld\n", j);

        dpf_cb(resf0[j]);
        dpf_cb(resf1[j]);
        dpf_cb(dpf_xor(resf0[j], resf1[j]));
    }
}
