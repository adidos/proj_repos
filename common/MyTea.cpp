#include "tea.h"
#include <cstring>
#include "MyTea.h"

mytea::mytea(const byte *key, int round /*= 32*/, bool isNetByte /*= false*/)
	:m_tea(key, round, isNetByte)
{
}

mytea::~mytea()
{

}

int mytea::encrypt_buf(const byte *src,int src_len,byte *enc,int enc_len)
{
	int bufno = src_len/8;
	int res = src_len%8;
	if(res >0)
	{
		bufno ++;
	}
	if(enc_len < bufno*8)
	{
		return -2;
	}
	int curpos = 0;
	for(curpos = 0;curpos < src_len;curpos += 8)
	{
		if(src_len - curpos < 8)
		{
			byte t_buf[9] ={0};
			memcpy((char *)t_buf,(char *)src+curpos,src_len - curpos);
			memcpy((char *)t_buf+src_len - curpos,"66666666",curpos+8-src_len);
			t_buf[8] = '\0';
			m_tea.encrypt(t_buf, enc + curpos);
		}
		else
		{
			m_tea.encrypt(src + curpos, enc + curpos);
		}
	}
	return curpos;
}
int mytea::decrypt_buf(const byte *enc,int enc_len,byte *dec,int dec_len)
{
	if(dec_len < enc_len)
	{
		return -1;
	}
	if(enc_len %8 != 0)
	{
		return -1;
	}
	int curpos = 0;
	for(curpos = 0;curpos < enc_len;curpos += 8)
	{
		m_tea.decrypt(enc + curpos,dec + curpos);
	}
	
	return curpos;
}

