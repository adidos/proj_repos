#ifndef DIRSVRCMD_H
#define DIRSVRCMD_H

#include "Command.h"
#include <string>
#include <map>
#include "IDataX.h"
#include "lite_auto_buf.h"

using namespace std;

const int CMD_MAGIC_NUM = 0x3C672E94;
const int CUR_PROTOCOL_VERSION = 10;

// cipher mode
const int CIPHER_MODE_NONE = 0;
const int CIPHER_MODE_TEA = 1;	

// 普通命令body编码后的最大长度(90%以上命令应该满足此条件)；
// 如果个别命令超过这个长度，就从heap里面分配动态内存
const int GENERAL_CMD_BODY_LEN = 1024; 

class IDataX;

class DataXCmd : public Command
{
	
public:
    DataXCmd(std::string cmd_name, byte cipher_flag=0);
	DataXCmd();
    virtual ~DataXCmd();

    //Command 接口
    virtual int body_length();
   /* virtual string get_cmd_details() const
    {
        return to_string();
    }*/
    virtual string get_cmd_name() const
    {
        return m_cmd_name; 
    }
    virtual int get_cmd_type_id() const
    {
        return 1; 
    }

	virtual string get_cmd_details() const
	{
		return NULL;
	}

    virtual int header_length() const { return m_header_len; }
    virtual bool decode_header(byte* buffer, int buffer_len);
    virtual bool decode_parameters(byte* buffer, int buffer_len);
    virtual bool encode(byte* buffer, int& buffer_len);

    //扩展接口
    //virtual string to_string() const;

	void set_datax(IDataX* dx);
	void attach_datax(IDataX* dx);
	IDataX* detach_datax();
	IDataX* get_datax() { return m_dx_ptr; }
	
    __int64 get_userid() const
    {
        return m_user_id;
    }
    void set_userid(const __int64 user_id)
    {
        m_user_id = user_id;
    }
	unsigned get_release_id() const { return m_release_id;}
    int get_cipher_flag() const  {  return m_cipher_flag;  };

    static void parse_release_id(int in_release_id, int& out_major_ver, int& out_minor_version);


private:
	void init(int cipher_flag);
    bool encrypt();
    bool decrypt(byte* pBuffer, int nBufferLen);	
    void get_cipher_key(unsigned char cipher_key[16]);

protected:
    int m_header_len;
    //消息头字段
    //_u32 m_magic_num;  // 固定常量，没必要单独作为一个字段
    _u16 m_protocol_ver;
    _u32 m_release_id;
    byte m_cipher_flag;
	
    _u32 m_encrypt_len;
    _u32 m_body_len;
    
    uint64_t m_user_id;
    std::string m_cmd_name;
    IDataX* m_dx_ptr;

	// 预留的buffer，避免在encode()和encode_length()调用时都需要再加密计算一次
	AutoByteBuf<GENERAL_CMD_BODY_LEN> m_body_buf;    
	

};

#endif

