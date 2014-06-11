//============================================================
// Command.h : abstract class Command
//                           (defines a common interface for all commands)
// Author: JeffLuo
// Created: 2007-09-29
//============================================================

#ifndef __COMMAND_H_20070929_15
#define  __COMMAND_H_20070929_15

#include <string>
#include "FixedBuffer.h"


using std::string;


class Command
{
public:	
	Command() : m_external_ip(0) { }
	virtual ~Command() { }
	
	// 去掉const修饰，参考body_length()方法
	int encoded_length() { return header_length() + body_length();  }  

	virtual string get_cmd_name() const  = 0;
	virtual int get_cmd_type_id() const  = 0;
	virtual string get_cmd_details() const = 0;	

	virtual int header_length() const = 0;	
	virtual bool decode_header(byte* buffer, int buffer_len) = 0;

	// body_length()实现代码可能会计算并内部缓存，因此去掉const修饰
	virtual int body_length() = 0;
	virtual bool decode_parameters(byte* buffer, int buffer_len) = 0;
	
	virtual bool encode(byte* buffer, int& buffer_len) = 0;

public:
	unsigned int m_external_ip;

private:
};


#endif //#ifndef __COMMAND_H_20070929_15



