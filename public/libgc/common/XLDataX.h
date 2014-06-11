#ifndef XLDATAX_H
#define XLDATAX_H

#include "IDataX.h"
#include "FixedBuffer.h"
#include "data_id_const.h"

#include <map>
#include <vector>
#include <string>

using std::map;
using std::vector;
using std::string;

#define MAKE_DATAX_INDEX(xType, pos)	((xType << 16) + pos)
#define GET_TYPE_FROM_DX_INDEX(nIndex)	(nIndex >> 16)
#define GET_POS_FROM_DX_INDEX(nIndex)	(nIndex & 0xFFFF)

class XLDataX :
	public IDataX
{
	enum DataX_Types_Enum
	{
		DX_TYPE_MIN_VALUE = 1,
		DX_TYPE_SHORT = 1,
		DX_TYPE_INT	 = 2,
		DX_TYPE_INT64 = 3,
		DX_TYPE_BYTES = 4,
		DX_TYPE_WSTRING = 5,
		DX_TYPE_DATAX = 6,
		DX_TYPE_INTARRAY = 7,
		DX_TYPE_DATAXARRAY	= 8,
		DX_TYPE_UTF8BYTES = 9,
		DX_TYPE_FLOAT = 10,
		DX_TYPE_INT64ARRAY = 11,
		DX_TYPE_MAX_VALUE = 11
	};

public:
	XLDataX(void);
	~XLDataX(void);

	static IDataX* DecodeFrom(byte* pbBuffer, int& nBufferLen);
	virtual bool  Decode(byte* pbBuffer, int& nBufferLen);

	// 返回该命令编码为二进制流的流长度(单位：字节数)
	virtual unsigned int EncodedLength();

	/***********************************************************************************
	* 将命令编码为buffer所指向内存上的二进制流
	* buffer_size: [IN/OUT] 调用前为缓冲区的初始大小，调用后设置为编码后二进制流的长度
	**********************************************************************************/
	virtual void Encode(void * buffer, unsigned int &buffer_size);

	// 将自己的内容完全复制一份
	virtual IDataX* Clone();
	// 将已有的数据清空
	virtual void Clear();
	// 返回元素数量
	virtual int GetSize();

	// 添加数据操作接口
	// --------------------
	// 添加Short类型数据
	virtual bool PutShort(unsigned short nKeyID, short nData);
	// 添加Int类型数据
	virtual bool PutInt(unsigned short nKeyID, int nData);
	// 添加64位整型数据
	virtual bool PutInt64(unsigned short nKeyID, int64_t nData);
	// 添加字节数组的内容
	virtual bool PutBytes(unsigned short nKeyID, const byte* pbData, int nDataLen);
	// 添加宽字节字符串
	virtual bool PutWString(unsigned short nKeyID, LPCWSTR pwszData, int nStringLen);
	// 嵌入IDataX内容
	virtual bool PutDataX(unsigned short nKeyID, IDataX* pDataCmd, bool bGiveupOwnership = false);
	// 添加Int数组的内容
	virtual bool PutIntArray(unsigned short nKeyID, int* pnData, int nElements);
	// 添加IDataX数组
	virtual bool PutDataXArray(unsigned short nKeyID, IDataX** ppDataCmd, int nElements, bool bGiveupOwnership = false);
	//替换或添加IDataX数组元素
	virtual bool PutDataXArrayElement(unsigned short nKeyID, int& nIndex, IDataX* pDataCmd, bool bGiveupOwnership = false);

	//添加UTF-8字符串数组
	virtual bool PutUTF8String(short nKeyID, const byte* pbData, int nDataLen);
	// 添加float类型数据
	virtual bool PutFloat(unsigned short nKeyID, float fData);
    
	// 添加Int数组的内容
	virtual bool PutInt64Array(unsigned short nKeyID, int64_t* pnData, int nElements);
	
	// 获取数据操作接口
	// ------------------------
	// 获取Short类型数据
	virtual bool GetShort(unsigned short nKeyID, short& nData);
	// 获取Int类型数据
	virtual bool GetInt(unsigned short nKeyID, int& nData);
	// 获取64位整型数据
	virtual bool GetInt64(unsigned short nKeyID, int64_t& nData) ;
	// 获取字节数组(如果pbDataBuf为NULL, 则会在nBufferLen设置该字节数组内容的实际长度)
	virtual bool GetBytes(unsigned short nKeyID, byte* pbDataBuf, int& nBufferLen);
	// 获取宽字节字符串(如果pwszDataBuf为NULL, 则会在nStringLen设置该字符串的实际长度)
	virtual bool GetWString(unsigned short nKeyID, LPWSTR pwszDataBuf, int& nStringLen) ;
	// 获取嵌入在里面的IDataX
	virtual bool GetDataX(unsigned short nKeyID, IDataX** ppDataCmd);
	// 获取整型数组的元素数量
	virtual bool GetIntArraySize(unsigned short nKeyID, int& nSize);
	// 获取整型数组的某个元素（根据索引编号）
	virtual bool GetIntArrayElement(unsigned short nKeyID, int nIndex, int& nData);
	// 获取IDataX数组的元素数量
	virtual bool GetDataXArraySize(unsigned short nKeyID, int& nSize);
	// 获取IDataX数组的某个元素（根据索引编号）
	virtual bool GetDataXArrayElement(unsigned short nKeyID, int nIndex, IDataX** ppDataCmd);	
	//获取UTF-8字符串数组
	virtual bool GetUTF8String(short nKeyID, byte* pbDataBuf, int& nBufferLen);
	// 获取Float类型数据
	virtual bool GetFloat(unsigned short nKeyID, float& fData);
	// 获取64整型数组的元素数量
	virtual bool GetInt64ArraySize(unsigned short nKeyID, int& nSize);
	// 获取64整型数组的某个元素（根据索引编号）
	virtual bool GetInt64ArrayElement(unsigned short nKeyID, int nIndex, int64_t& nData);

	//以字符串形式输出相关内容，用于调测
	virtual string ToString();

private:
	void EncodeBytesItem(FixedBuffer& fixed_buffer, int nPos);
	void EncodeWStringItem(FixedBuffer& fixed_buffer, int nPos);
	void EncodeDataXItem(FixedBuffer& fixed_buffer, int nPos);
	void EncodeIntArrayItem(FixedBuffer& fixed_buffer, int nPos);
	void EncodeDataXArrayItem(FixedBuffer& fixed_buffer, int nPos);
	void EncodeFloatItem(FixedBuffer& fixed_buffer, int nPos);
	void EncodeUTF8BytesItem(FixedBuffer& fixed_buffer, int nPos);
	void EncodeInt64ArrayItem(FixedBuffer& fixed_buffer, int nPos);

    unsigned CalcUTF8BytesItemLen(int nPos) { return sizeof(int) + m_vecUTF8BytesItems[nPos].length(); } 
	unsigned CalcBytesItemLen(int nPos) { return sizeof(int) + m_vecBytesItems[nPos].length(); }
	unsigned CalcWStringItemLen(int nPos) { return sizeof(int) + m_vecWstrItems[nPos].length() * sizeof(WCHAR); }
	unsigned CalcDataXItemLen( int nPos) { return m_vecDataXItems[nPos]->EncodedLength(); }
	unsigned CalcIntArrayItemLen(int nPos);
	unsigned CalcDataXArrayItemLen( int nPos);
	unsigned CalcInt64ArrayItemLen(int nPos);
    

	BOOL PutBytesItem(unsigned short nKeyID, FixedBuffer& fixed_buffer);
	BOOL PutWStringItem(unsigned short nKeyID, FixedBuffer& fixed_buffer);
	BOOL PutDataXItem(unsigned short nKeyID, FixedBuffer& fixed_buffer);
	BOOL PutIntArrayItem(unsigned short nKeyID, FixedBuffer& fixed_buffer);
	BOOL PutDataXArrayItem(unsigned short nKeyID, FixedBuffer& fixed_buffer);
	BOOL PutUTF8BytesItem(unsigned short nKeyID, FixedBuffer& fixed_buffer);
	BOOL PutFloatItem(unsigned short nKeyID, FixedBuffer& fixed_buffer);
	BOOL PutInt64ArrayItem(unsigned short nKeyID, FixedBuffer& fixed_buffer);

private:
	map<unsigned short, int> m_mapIndexes;

	vector<int> m_vecIntItems;
	vector<int64_t> m_vecInt64Items;
	vector<float> m_vecFloatItems;
	vector<string> m_vecBytesItems;
	vector<string> m_vecUTF8BytesItems;
	vector<std::wstring> m_vecWstrItems;
	vector<IDataX*> m_vecDataXItems;
	vector< vector<int> > m_vecIntArrayItems;
	vector< vector<int64_t> > m_vecInt64ArrayItems;
	vector< vector<IDataX*> > m_vecDataXArrayItems;

};

#endif
