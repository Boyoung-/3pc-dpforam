#ifndef SSOT_H_
#define SSOT_H_

#include "protocol.h"

class ssot : public protocol {
public:
    ssot(const char* party, connection* cons[2],
         CryptoPP::AutoSeededRandomPool* rnd,
         CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption* prgs);
    void runE(uint b1, const uchar* const v01[2], uint mBytes, uchar* p1);
    void runD(uint mBytes);
    void runC(uint b0, const uchar* const u01[2], uint mBytes, uchar* p0);
    void test(uint iter);
};

#endif /* SSOT_H_ */
