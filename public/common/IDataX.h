#ifndef __XL_IDATAX_HEADER_20090328_
#define __XL_IDATAX_HEADER_20090328_

#include <string>

#ifndef WIN32
typedef wchar_t* LPCWSTR; 
typedef wchar_t* LPWSTR;
typedef bool BOOL;
#define ASSERT assert
#endif

class IDataX;

class DataXFactory
{
	DataXFactory() { }
public:
	static IDataX* CreateIDataX();
	static IDataX* DecodeFrom(const byte* pbData, int nLen);
};


/*******************************************************************
**  "万能"数据交换接口，客户端和大厅交换数据可通过此接口
**
**  使用方法：将数据通过 PutXXX设置好后，调用Encode()方法将命令序列化,
**            将序列化的二进制数据放入VARIANT里面作为SafeArray传递给对方
**********************************************************************/
class IDataX
{
public:
	virtual ~IDataX() { }

	enum { DX_MAGIC_NUM = 0x385A };

	// 返回该命令编码为二进制流的流长度(单位：字节数)
	virtual unsigned int EncodedLength() = 0;

	/***********************************************************************************
	* 将命令编码为buffer所指向内存上的二进制流
	* buffer_size: [IN/OUT] 调用前为缓冲区的初始大小，调用后设置为编码后二进制流的长度
	***********************************************************************************/
	virtual void Encode(void * buffer, unsigned int &buffer_size) = 0;

	virtual bool Decode(byte* pbBuffer, int& nBufferLen) = 0;

	// 将自己的内容完全复制一份
	virtual IDataX* Clone() = 0;
	// 将已有的数据清空
	virtual void Clear() = 0;
	// 返回元素数量
	virtual int GetSize() = 0;

	// 添加数据操作接口
	// --------------------
	// 添加Short类型数据
	virtual bool PutShort(unsigned short nKeyID, short nData) = 0;
	// 添加Int类型数据
	virtual bool PutInt(unsigned short nKeyID, int nData) = 0;
	// 添加64位整型数据
	virtual bool PutInt64(unsigned short nKeyID, __int64 nData) = 0;
	// 添加字节数组的内容
	virtual bool PutBytes(unsigned short nKeyID, const byte* pbData, int nDataLen) = 0;
	// 添加宽字节字符串
	virtual bool PutWString(unsigned short nKeyID, LPCWSTR pwszData, int nStringLen) = 0;
	// 嵌入IDataX内容
	//  参数: bGiveupOwnership值为true，则表示调用者将pDataCmd所有权转移给该函数，调用者后面不负责释放
	//        bGiveupOwnership值为false, 则表示调用者在执行此函数后要负责释放pDataCmd，该函数会在内部复制一份pDataCmd内容
	virtual bool PutDataX(unsigned short nKeyID, IDataX* pDataCmd, bool bGiveupOwnership = false) = 0;
	// 添加Int数组的内容
	virtual bool PutIntArray(unsigned short nKeyID, int* pnData, int nElements) = 0;
	// 添加IDataX数组
	//  参数: bGiveupOwnership说明参见PutDataX()
	virtual bool PutDataXArray(unsigned short nKeyID, IDataX** ppDataCmd, int nElements, bool bGiveupOwnership = false) = 0;
	//替换或添加IDataX数组元素
	//  参数: bGiveupOwnership说明参见PutDataX()
	//        nIndex 放入位置，如果大于等于0并且小于当前元素数，则替换现有元素，如果大于现有元素数则在后面添加，此时nIndex为实际位置
	virtual bool PutDataXArrayElement(unsigned short nKeyID, int& nIndex, IDataX* pDataCmd, bool bGiveupOwnership = false) = 0;

	//添加UTF-8字符串数组
	virtual bool PutUTF8String(short nKeyID, const byte* pbData, int nDataLen) = 0;
	// 添加float类型数据
	virtual bool PutFloat(unsigned short nKeyID, float fData) = 0;
	// 添加Int数组的内容
	virtual bool PutInt64Array(unsigned short nKeyID, __int64* pnData, int nElements) = 0;

	// 获取数据操作接口
	// ------------------------
	// 获取Short类型数据
	virtual bool GetShort(unsigned short nKeyID, short& nData) = 0;
	// 获取Int类型数据
	virtual bool GetInt(unsigned short nKeyID, int& nData) = 0;
	// 获取64位整型数据
	virtual bool GetInt64(unsigned short nKeyID, __int64& nData) = 0;
	// 获取字节数组(如果pbDataBuf为NULL, 则会在nBufferLen设置该字节数组内容的实际长度)
	virtual bool GetBytes(unsigned short nKeyID, byte* pbDataBuf, int& nBufferLen) = 0;
	// 获取宽字节字符串(如果pwszDataBuf为NULL, 则会在nStringLen设置该字符串的实际长度)
	virtual bool GetWString(unsigned short nKeyID, LPWSTR pwszDataBuf, int& nStringLen) = 0;
	// 获取嵌入在里面的IDataX
	virtual bool GetDataX(unsigned short nKeyID, IDataX** ppDataCmd) = 0;
	// 获取整型数组的元素数量
	virtual bool GetIntArraySize(unsigned short nKeyID, int& nSize) = 0;
	// 获取整型数组的某个元素（根据索引编号）
	virtual bool GetIntArrayElement(unsigned short nKeyID, int nIndex, int& nData) = 0;
	// 获取IDataX数组的元素数量
	virtual bool GetDataXArraySize(unsigned short nKeyID, int& nSize) = 0;
	// 获取IDataX数组的某个元素（根据索引编号）
	virtual bool GetDataXArrayElement(unsigned short nKeyID, int nIndex, IDataX** ppDataCmd) = 0;
	//获取UTF-8字符串数组
	virtual bool GetUTF8String(short nKeyID, byte* pbDataBuf, int& nBufferLen) = 0;
	// 获取Float类型数据
	virtual bool GetFloat(unsigned short nKeyID, float& fData) = 0;
	// 获取64整型数组的元素数量
	virtual bool GetInt64ArraySize(unsigned short nKeyID, int& nSize) = 0;
	// 获取64整型数组的某个元素（根据索引编号）
	virtual bool GetInt64ArrayElement(unsigned short nKeyID, int nIndex, __int64& nData) = 0;

	//以字符串形式输出相关内容，用于调测
	virtual std::string ToString() = 0;
};

#endif //  #define __XL_IDATAX_HEADER_20090328_

