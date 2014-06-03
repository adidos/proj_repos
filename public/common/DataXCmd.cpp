#include "DataXCmd.h"
#include "common.h"
#include "FixedBuffer.h"
#include "MyTea.h"
#include "mymd5.h"
#include <sstream>
#include "XLDataX.h"
#include "lite_auto_buf.h"
#include <exception>
#include <stdint.h>
using namespace std;



DataXCmd::DataXCmd(std::string cmd_name, byte cipher_flag)
	:m_cmd_name(cmd_name)
{
	init(cipher_flag);
}

DataXCmd::DataXCmd()
{
	init(CIPHER_MODE_NONE);
}

void DataXCmd::init(int cipher_flag)
{
    m_protocol_ver = CUR_PROTOCOL_VERSION;
    m_release_id = 0;
    m_cipher_flag = (byte)cipher_flag;

	m_dx_ptr = NULL;
    m_body_len = 0; 
    m_encrypt_len = 0;

    m_header_len = sizeof(int32_t)        // MagicNumber
                 + sizeof(m_protocol_ver)      // ProtocolVer
                 + sizeof(m_release_id)      // xReleaseID
                 + sizeof(m_cipher_flag)      // CipherFlag
                 + sizeof(m_encrypt_len)      // EncryptLen
                 + sizeof(m_body_len);        // BodyLen

}

DataXCmd::~DataXCmd()
{
	if(m_dx_ptr != NULL){
		delete m_dx_ptr;
		m_dx_ptr = NULL;
	}
}

void DataXCmd::set_datax(IDataX* dx)
{
	if(m_dx_ptr != NULL){
		delete m_dx_ptr;
		m_dx_ptr = NULL;
	}
	m_dx_ptr = dx;
}

void DataXCmd::attach_datax(IDataX* dx)
{
	if(dx != NULL){
		m_dx_ptr = dx;
	}
}

IDataX* DataXCmd::detach_datax()
{
	IDataX* dx = m_dx_ptr;
	m_dx_ptr = NULL;
	return dx;
}


bool DataXCmd::decode_header(byte * buffer, int buffer_len)
{
    if (!buffer)
    {
        //_WARN(logger, "buffer ptr is NULL in decode_header()!");
        return false;
    }
    if (buffer_len < m_header_len)
    {
        //_WARN(logger, "buffer length(" << buffer_len << ") is too small in decode_header()!");
        return false;
    }
    FixedBuffer clsBuf(reinterpret_cast<char*>(buffer), buffer_len, true);
    try
    {
        int magic_num = clsBuf.get_int();
        m_protocol_ver= clsBuf.get_short();
        m_release_id= clsBuf.get_int();
        m_cipher_flag= clsBuf.get_byte();
        m_encrypt_len = clsBuf.get_int();
        m_body_len = clsBuf.get_int();

        if ((CIPHER_MODE_NONE != m_cipher_flag) && (CIPHER_MODE_TEA != m_cipher_flag))
        {
            //_ERROR(logger, "invalid CipherFlag "<<(int)m_cipher_flag);
            return false;
        }

        if (CMD_MAGIC_NUM != magic_num)
        {
            //_ERROR(logger, "invalid magic num 0x"<<hex<<magic_num
                            //<<dec<<"!");
            return false;
        }
        if (CUR_PROTOCOL_VERSION != m_protocol_ver)
        {
            //_ERROR(logger, "invalid protocol version:"<<m_protocol_ver<<"!");
            return false;
        }
    }
    catch (exception &e)
    {
        //_ERROR(logger, "decode message header error! "<<e.what());
        return false;
    }
    return true;
}

bool DataXCmd::decode_parameters(byte * buffer, int buffer_len)
{
    if (!buffer)
    {
        //_ERROR(logger, "buffer is NULL in decode_parameters()!");
        return false;
    }
    m_encrypt_len = buffer_len;

    return decrypt(buffer, buffer_len);
}

bool DataXCmd::encode(byte * buffer, int & buffer_len)
{
    if (!buffer)
    {
        //_ERROR(logger, "buffer is NULL in encode()!");
        return false;
    }
    //_DEBUG(logger, "encode message into buffer of length " <<buffer_len);
    FixedBuffer clsBuf(reinterpret_cast<char*>(buffer), buffer_len, true);
    try
    {
        clsBuf.put_int(CMD_MAGIC_NUM);
        clsBuf.put_short(m_protocol_ver);
        clsBuf.put_int(m_release_id);
        clsBuf.put_byte(m_cipher_flag);

		if(m_encrypt_len == 0)
			encrypt();
		if(m_encrypt_len <= 0)
		{
			// 加密失败
			return false;
		}
		
        clsBuf.put_int(m_encrypt_len);
        clsBuf.put_int(m_body_len);
		
        //body已经序列化到m_body_buf里面，需要拷贝到buffer里面
		if(clsBuf.remain_len() != (int)m_encrypt_len)
		{
			return false;
		}
		memcpy(buffer + clsBuf.position(), m_body_buf.current_buffer(), m_encrypt_len);
    }
    catch (exception &e)
    {
        //_ERROR(logger, "encode message error: "<<e.what());
        return false;
    }
    return true;
}



void DataXCmd::parse_release_id(int in_nReleaseId, int& out_nMajorVer, int& out_nMinorVer)
{
    out_nMajorVer = 0;
    out_nMinorVer = 0;
    out_nMajorVer = (in_nReleaseId >> 24) & 0xFF;
    out_nMinorVer = (in_nReleaseId & 0xFFFF);
}

bool DataXCmd::encrypt()
{
	AutoByteBuf<GENERAL_CMD_BODY_LEN> auto_buf;

	int expected_buf_len = sizeof(int64_t) + sizeof(int32_t) + m_cmd_name.length();
	if (m_dx_ptr != NULL){
		expected_buf_len += m_dx_ptr->EncodedLength();
	}
	byte* tmp_buf = NULL; 
	if (CIPHER_MODE_NONE == m_cipher_flag){
		// 不加密，直接使用m_body_buf,避免memcpy开销
		tmp_buf = m_body_buf.get_buffer(expected_buf_len);
	}else{
		// 加密，先使用临时buffer作为源数据缓冲
		tmp_buf = auto_buf.get_buffer(expected_buf_len);
	}
    FixedBuffer clsBuf(reinterpret_cast<char*>(tmp_buf), expected_buf_len, true);
    try
    {
        clsBuf.put_int64(m_user_id);
        clsBuf.put_string(m_cmd_name);
		m_body_len = clsBuf.position();
		if(m_dx_ptr != NULL) {
			unsigned dx_data_len = (unsigned)clsBuf.remain_len();
			m_dx_ptr->Encode(tmp_buf + clsBuf.position(), dx_data_len);
			m_body_len += dx_data_len;
		}
    }
    catch (exception &e)
    {
        //_ERROR(logger, "copy message body to temp buffer error when call encrypt():"<<e.what());
        return false;
    }
    
    //_DEBUG(logger, "do encrypt message,m_cipher_flag="<<(int)m_cipher_flag);

    if (CIPHER_MODE_NONE == m_cipher_flag)  
    {
	    //不加密,数据已序列化到m_body_buf里面，只需要再设置m_encrypt_len
        m_encrypt_len = m_body_len;
    }
    else if (CIPHER_MODE_TEA == m_cipher_flag)  //加密
    {
        unsigned char cipher_key[16];
        get_cipher_key(cipher_key);
        //_DEBUG(logger, "encrypt key:["<<Utility::bytes_to_hex(cipher_key, sizeof(cipher_key))
                        //<<"], raw data:["<<Utility::bytes_to_hex(tmp_buf, m_body_len)<<"]");
        mytea clsTea(cipher_key);
		int expected_encrypt_len = ((m_body_len + 15)/16) * 16;
    	byte* buf1 = m_body_buf.get_buffer(expected_encrypt_len);		
		
        m_encrypt_len= clsTea.encrypt_buf(tmp_buf, m_body_len, buf1, expected_encrypt_len);
        if (m_encrypt_len < 0)
        {
            //_ERROR(logger, "encryt message body to buffer failed!");
            m_encrypt_len= 0;
            return false;
        }
        //_DEBUG(logger, "encrypted data:["<<Utility::bytes_to_hex(buf1, m_encrypt_len)<<"]");
    }
    else
    {
        //_ERROR(logger, "invalid m_cipher_flag="<<(int)m_cipher_flag<<"!");
        m_encrypt_len = 0;
        return false;
    }
    return true;
}

bool DataXCmd::decrypt(byte* pBuffer, int nBufferLen)
{
    //_DEBUG(logger, "try decrypt message,m_cipher_flag="<<(int)m_cipher_flag);
	byte* buf1 = NULL;
    if (CIPHER_MODE_TEA == m_cipher_flag)  //解密
    {
        unsigned char szKey[16];
        get_cipher_key(szKey);
        //_DEBUG(logger, "decrypt key:["<<Utility::bytes_to_hex(szKey, sizeof(szKey))
                        //<<"], raw data:["<<Utility::bytes_to_hex(pBuffer, nBufferLen)<<"]");
		AutoByteBuf<GENERAL_CMD_BODY_LEN> auto_buf;
		buf1 = auto_buf.get_buffer(nBufferLen);
        mytea clsTea(szKey);
		int plain_data_len = clsTea.decrypt_buf(pBuffer, nBufferLen, buf1, nBufferLen);
        if (plain_data_len < 0)
        {
            //_ERROR(logger, "decrypt message body failed!");
            return false;
        }
		m_body_len = plain_data_len;
        pBuffer = buf1;
        nBufferLen = m_body_len;
        //_DEBUG(logger, "decrypted data:["<<Utility::bytes_to_hex(pBuffer, nBufferLen)<<"]");
    }
	else if (CIPHER_MODE_NONE == m_cipher_flag)
    {
        buf1 = pBuffer;
    }
    else if (CIPHER_MODE_NONE != m_cipher_flag)
    {
		//_ERROR(logger, "invalid m_cipher_flag="<<(int)m_cipher_flag<<"!");
		return false;
    }
	 byte* ptr = m_body_buf.get_buffer(nBufferLen);
    memcpy(ptr, buf1, nBufferLen);
    //_DEBUG(logger, "decode body fields from message body, len="<<nBufferLen);
    FixedBuffer clsBuf(reinterpret_cast<char*>(pBuffer), nBufferLen, true);
    try
    {
        m_user_id = clsBuf.get_int64();
		m_cmd_name = clsBuf.get_string();
		int remain_len = nBufferLen - clsBuf.position();
		m_dx_ptr = XLDataX::DecodeFrom(pBuffer + clsBuf.position(), remain_len);
    }
    catch (exception &e)
    {
        //_ERROR(logger, "decode message body error: "<<e.what());
        return false;
    }
    return true;
}

int DataXCmd::body_length()
{
	if(m_encrypt_len > 0)
		return m_encrypt_len;

	encrypt();
    return m_encrypt_len;
}

void DataXCmd::get_cipher_key(unsigned char szkey[16])
{
    //_DEBUG(logger, "get key from following fields: MagicNum="<<CMD_MAGIC_NUM
                    //<<", m_protocol_ver="<<m_protocol_ver
                    //<<", m_release_id="<<m_release_id
                    //<<", m_cipher_flag="<<(int)m_cipher_flag
                    //<<", m_body_len="<<m_body_len);

    MD5 myMD5;
    myMD5.update((void*)&CMD_MAGIC_NUM, sizeof(CMD_MAGIC_NUM));
    myMD5.update((void*)&m_protocol_ver, sizeof(m_protocol_ver));
    myMD5.update((void*)&m_release_id, sizeof(m_release_id));
    myMD5.update((void*)&m_cipher_flag, sizeof(m_cipher_flag));
    myMD5.update((void*)&m_body_len, sizeof(m_body_len));
    memcpy(szkey, myMD5.digest(), 16);
}

