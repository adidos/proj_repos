
#ifndef __MYTEA_H_20080220__
#define __MYTEA_H_20080220__
#include "tea.h"
#include "common.h"

class mytea
{
public:
	mytea(const byte *key, int round = 32, bool isNetByte = false);
	~mytea();
	
	int encrypt_buf(const byte *src,int src_len,byte *enc,int enc_len);
	int decrypt_buf(const byte *enc,int enc_len,byte *dec,int dec_len);
private:
	TEA m_tea;
};

#endif //__MYTEA_H_20080220__
