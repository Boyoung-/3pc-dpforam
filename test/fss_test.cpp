#include "fss.h"

#include <iostream>

#include "util.h"

using namespace std;

int main() {
    fss1bit generator;
    fss1bit evaluators[2];

    for (uint m = 1; m <= 20; m++) {
        unsigned long range = 1ul << m;

        for (uint i = 0; i < 100; i++) {
            bool pass = true;
            unsigned long alpha = rand_long(range);
            uchar *keys[2];
            generator.gen(alpha, m, keys);
            uchar *share0 = new uchar[range];
            uchar *share1 = new uchar[range];
            evaluators[0].eval_all(keys[0], m, share0);
            evaluators[1].eval_all(keys[1], m, share1);

            for (unsigned long x = 0; x < range; x++) {
                uchar output = share0[x] ^ share1[x];
                if (x == alpha) {
                    if (output == 0) {
                        cout << "Failed: alpha=" << alpha << ", x=" << x
                             << ", outValue=" << output << endl;
                        pass = false;
                    }
                } else {
                    if (output != 0) {
                        cout << "Failed: alpha=" << alpha << ", x=" << x
                             << ", outValue=" << output << endl;
                        pass = false;
                    }
                }
            }

            if (pass)
                cout << "m=" << m << ", i=" << i << ": passed" << endl;
            else
                cout << "m=" << m << ", i=" << i << ": failed" << endl;

            delete[] keys[0];
            delete[] keys[1];
            delete[] share0;
            delete[] share1;
        }
        cout << endl;
    }

    return 0;
}
