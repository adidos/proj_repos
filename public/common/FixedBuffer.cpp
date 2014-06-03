//============================================================
// FixedBuffer.h : interface of class FixedBuffer
//                          
// Author: JeffLuo
// Created: 2006-11-08
//============================================================

#include "FixedBuffer.h"

const string FixedBuffer::EMPTY_STRING = string("");

FixedBuffer::FixedBuffer(char * buffer, int buffer_size, bool reverse_byteorder)
{
	m_buffer = buffer;
	m_buffer_size = buffer_size;
	m_offset = 0;
	m_reverse_byteorder = reverse_byteorder;

	m_limit = 0;
}

bool FixedBuffer::put_byte(byte x)
{
	if(remain_len() < (int)sizeof(byte))
	{
		char buffer[256];
		sprintf(buffer, "remain buffer length(%d) not enough in put_byte()", remain_len());
		
	}

	*(m_buffer + m_offset) = x;
	m_offset += sizeof(byte);
	if(m_limit < m_offset)
		m_limit = m_offset;

	return true;
}

bool FixedBuffer::put_int(int x)
{
	if(remain_len() < (int)sizeof(int))
	{
		char buffer[256];
		sprintf(buffer, "remain buffer length(%d) not enough in put_int()", remain_len());
		
	}

	copy_bytes((byte*)(m_buffer + m_offset), (byte*)&x, sizeof(int));
	m_offset += sizeof(int);
	if(m_limit < m_offset)
		m_limit = m_offset;	

	return true;
}

bool FixedBuffer::put_int64(uint64_t x)
{
	if(remain_len() < (int)sizeof(uint64_t))
	{
		char buffer[256];
		sprintf(buffer, "remain buffer length(%d) not enough in put_int64()", remain_len());
			
	}

	copy_bytes((byte*)(m_buffer+m_offset), (byte*)&x, sizeof(uint64_t));
	m_offset += sizeof(uint64_t);
	if(m_limit < m_offset)
		m_limit = m_offset;

	return true;
}

bool FixedBuffer::put_short(short x)
{
	if(remain_len() < (int)sizeof(short))
	{
		char buffer[256];
		sprintf(buffer, "remain buffer length(%d) not enough in put_short()", remain_len());
		
	}


	copy_bytes((byte*)(m_buffer+m_offset), (byte*)&x, sizeof(short));
	m_offset += sizeof(short);
	if(m_limit < m_offset)
		m_limit = m_offset;

	return true;
}

//NOTICE: put_bytesV2 do not put prefix length, but put_string do
bool FixedBuffer::put_bytesV2(const byte * buf, unsigned int  len){
	
	if(len<1)
		return false;

	if(remain_len() < len)
	{
		char buffer[256];
		sprintf(buffer, "remain buffer length(%d) not enough in put_bytesV2()", remain_len());
			
	}

	//copy_bytes((byte*)(m_buffer+m_offset), (byte*)&len, sizeof(int));
	//m_offset += sizeof(int);
	if(len > 0)
		memcpy(m_buffer + m_offset, buf, len);
	m_offset += len;

	if(m_limit < m_offset)
		m_limit = m_offset;

	return true;

}

bool FixedBuffer::put_string(const char * str, int str_len)
{
	int len = str_len;
	if(len == -1)
		len = (str != NULL ? strlen(str) : 0);

	if(remain_len() < (int)sizeof(int) + len)
	{
		char buffer[256];
		sprintf(buffer, "remain buffer length(%d) not enough in put_string()", remain_len());
		
	}

	copy_bytes((byte*)(m_buffer+m_offset), (byte*)&len, sizeof(int));
	m_offset += sizeof(int);
	if(len > 0)
		memcpy(m_buffer + m_offset, str, len);
	m_offset += len;

	if(m_limit < m_offset)
		m_limit = m_offset;

	return true;
}

bool FixedBuffer::put_string(const string & str)
{
	int len = str.length();

	if(remain_len() < (int)sizeof(int) + len)
	{
		char buffer[256];
		sprintf(buffer, "remain buffer length(%d) not enough in put_string()", remain_len());
		
	}

	copy_bytes((byte*)(m_buffer+m_offset), (byte*)&len, sizeof(int));
	m_offset += sizeof(int);
	if(len > 0)
		memcpy(m_buffer + m_offset, str.c_str(), len);
	m_offset +=  len;

	if(m_limit < m_offset)
		m_limit = m_offset;

	return true;
}

bool FixedBuffer::skip(int offset)
{
	int new_pos = m_offset + offset;
	if(new_pos > -1 && new_pos <= m_buffer_size)
	{
		m_offset += offset;
		if(m_limit < m_offset)
			m_limit = m_offset;
		
		return true;
	}
	else
	{
		return false;
	}
}

byte FixedBuffer::get_byte()
{
	if(remain_len() < (int)sizeof(byte))
	{
		
	}   
	byte b = *(m_buffer + m_offset);
	m_offset += sizeof(byte);

	if(m_limit < m_offset)
		m_limit = m_offset;

	return b;
}

int FixedBuffer::get_int()
{
	int x;
	if(remain_len() < (int)sizeof(int))
	{
		
	}   

	copy_bytes((byte*)&x, (byte*)(m_buffer+m_offset), sizeof(int));
	m_offset += sizeof(int);

	if(m_limit < m_offset)
		m_limit = m_offset;

	return x;
}

uint64_t FixedBuffer::get_int64()
{
	uint64_t x;
	if(remain_len() < (int)sizeof(uint64_t))
	{
		
	}   

	copy_bytes((byte*)&x, (byte*)(m_buffer+m_offset), sizeof(uint64_t));
	m_offset += sizeof(uint64_t);

	if(m_limit < m_offset)
		m_limit = m_offset;

	return x;
}


short FixedBuffer::get_short()
{
	short x;
	if(remain_len() < (int)sizeof(short))
	{
		
	}   

	copy_bytes((byte*)&x, (byte*)(m_buffer+m_offset), sizeof(short));
	m_offset += sizeof(short);

	if(m_limit < m_offset)
		m_limit = m_offset;

	return x;
}


string FixedBuffer::get_string()
{
	int str_len = get_int();
	if(str_len < 0)
	{
		
	}   
	else if(str_len == 0)
		return EMPTY_STRING;
    
	if(remain_len() < str_len)
	{
		char buffer[256];
		sprintf(buffer, "remain buffer length(%d) less than expected length(%d) in get_string()", remain_len(), str_len);
		
	}   

	string str(str_len, 0);
	memcpy(&str[0], m_buffer + m_offset, str_len);
	m_offset += str_len;

	if(m_limit < m_offset)
		m_limit = m_offset;

	return str;
}

// NOTICE: get_bytes do not read prefix length, but get_string() DO
bool FixedBuffer::get_bytes(byte * buf, int len)
{
	if(len < 0 || remain_len() < len)
	{
		char buffer[256];
		sprintf(buffer, "remain buffer length(%d) less than expected length(%d) in get_bytes()", remain_len(), len);
		
	}   

	memcpy(buf, m_buffer + m_offset, len);
	m_offset += len;

	if(m_limit < m_offset)
		m_limit = m_offset;

	return true;
}

