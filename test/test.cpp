#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/randpool.h>
#include <cryptopp/osrng.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/modes.h>
#include <iostream>

using namespace std;
using namespace CryptoPP;

int main() {
	cout << "test" << endl;

	unsigned char seed[48];
	for (int i=0; i<48; i++) {
		seed[i] = i;
	}
	CTR_Mode<AES>::Encryption prng1;
	prng1.SetKeyWithIV(seed, 32, seed + 32, 16);
	CTR_Mode<AES>::Encryption prng2;
	prng2.SetKeyWithIV(seed, 32, seed + 32, 16);

	//OS_GenerateRandomBlock(false, seed, seed.size());
	//prng1.IncorporateEntropy(seed, 32);
	//prng2.IncorporateEntropy(seed, 32);


//	string s;
//	HexEncoder hex(new StringSink(s));
//
//	hex.Put(seed, 32);
//	hex.MessageEnd();
//
//	cout << "Seed: " << s << endl;

	unsigned char block1[100];
	unsigned char block2[100];
	prng1.GenerateBlock(block1, 100);
	prng2.GenerateBlock(block2, 100);
	//prng1.GenerateWord32()

	cout << "compare " << memcmp(block1, block2, 100) << endl;

	return 0;
}
