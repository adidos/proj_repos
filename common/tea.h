#ifndef TEA_H
#define TEA_H

/*
*for htonl,htonl
*do remember link "ws2_32.lib"
*/
#ifdef WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include "common.h"

class TEA {
    //保持32/64位系统兼容
    typedef unsigned int ulong;
public:
	TEA(const byte *key, int round = 32, bool isNetByte = false);
	TEA(const TEA &rhs);
	TEA& operator=(const TEA &rhs);
	void encrypt(const byte *in, byte *out);
	void decrypt(const byte *in, byte *out);
private:
	void encrypt(const ulong *in, ulong *out);
	void decrypt(const ulong *in, ulong *out);
	ulong ntoh(ulong netlong) { return _isNetByte ? ntohl(netlong) : netlong; }
	ulong hton(ulong hostlong) { return _isNetByte ? htonl(hostlong) : hostlong; }
private:
	int _round; //iteration round to encrypt or decrypt
	bool _isNetByte; //whether input bytes come from network
	byte _key[16]; //encrypt or decrypt key
};

#endif/*TEA_H*/
