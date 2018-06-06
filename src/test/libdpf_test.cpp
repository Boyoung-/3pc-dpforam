#include "../libdpf/libdpf.h"

int main() {
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

	int j;
	for (j = 0; j < 1; j++) {
		printf("Group %d\n", j);

		dpf_cb(resf0[j]);
		dpf_cb(resf1[j]);
		dpf_cb(dpf_xor(resf0[j], resf1[j]));
	}

	return 0;
}
