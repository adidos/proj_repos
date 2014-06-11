#ifndef _DATAX_NET_WRAPPER_H_
#define _DATAX_NET_WRAPPER_H_

#include <string>
#include <stdint.h>
#include "IDataX.h"

class DxWrapper
{
public:
	DxWrapper(IDataX* pDataX) : m_pDataX(pDataX) { }

	short GetShort(short nKeyID, short nDefault = 0);
	// 获取Int类型数据
	int GetInt(short nKeyID, int nDefault = 0);
	// 获取浮点数据值
	float GetFloat(short nKeyID, float fData = 0.0);
	// 获取64位整型数据
	int64_t GetInt64(short nKeyID, int64_t nDefault = 0);
	// 获取字节数组(如果pbDataBuf为NULL, 则会在nBufferLen设置该字节数组内容的实际长度)
	std::string GetBytes(short nKeyID, const std::string& strDefault = "");
	// 获取宽字节字符串(如果pwszDataBuf为NULL, 则会在nStringLen设置该字符串的实际长度)
	std::wstring GetWString(short nKeyID, const std::wstring& strDefault= L"");
	// 获取嵌入在里面的IDataX
	IDataX* GetDataX(short nKeyID);
	// 获取整型数组的元素数量
	int GetIntArraySize(short nKeyID) { return 0;}
	// 获取整型数组的某个元素（根据索引编号）
	int GetIntArrayElement(short nKeyID, int nIndex) { return 0; }
	// 获取IDataX数组的元素数量
	int GetDataXArraySize(short nKeyID);
	// 获取IDataX数组的某个元素（根据索引编号）
	IDataX* GetDataXArrayElement(short nKeyID, int nIndex) ;
	// 获取UTF8编码的字节数组(如果pbDataBuf为NULL, 则会在nBufferLen设置该字节数组内容的实际长度)
	std::string GetUTF8String(short nKeyID, const std::string& strDefault = "");

private:
	IDataX* m_pDataX;
};

inline short DxWrapper::GetShort(short nKeyID, short nDefault )
{
	if(m_pDataX == NULL)
		return nDefault;
	else
	{
		short nData = nDefault;
		bool bExists = m_pDataX->GetShort(nKeyID, nData);
		return bExists ? nData : nDefault;
	}
}

inline int DxWrapper::GetInt(short nKeyID, int nDefault )
{
	if(m_pDataX == NULL)
		return nDefault;
	else
	{
		int nData = nDefault;
		bool bExists = m_pDataX->GetInt(nKeyID, nData);
		return bExists ? nData : nDefault;
	}
}

inline float DxWrapper::GetFloat(short nKeyID, float fDefault)
{
	if(m_pDataX == NULL)
		return fDefault;
	else
	{
		float fData = fDefault;
		bool bExists = m_pDataX->GetFloat(nKeyID, fData);
		return bExists ? fData : fDefault;
	}
}

inline int64_t DxWrapper::GetInt64(short nKeyID, int64_t nDefault)
{
	if(m_pDataX == NULL)
		return nDefault;
	else
	{
		int64_t nData = nDefault;
		bool bExists = m_pDataX->GetInt64(nKeyID, nData);
		return bExists ? nData : nDefault;
	}
}

inline std::string DxWrapper::GetBytes(short nKeyID, const std::string& strDefault)
{
	if(m_pDataX == NULL)
		return strDefault;
	else
	{
		int nBufferLen = 0;
		bool bExists = m_pDataX->GetBytes(nKeyID, NULL, nBufferLen);
		if(!bExists)
			return strDefault;

		std::string str(nBufferLen, 0);
		bExists = m_pDataX->GetBytes(nKeyID, (byte*)str.c_str(), nBufferLen);
		return str;
	}
}

inline std::wstring DxWrapper::GetWString(short nKeyID, const std::wstring& strDefault)
{
	if(m_pDataX == NULL)
		return strDefault;
	else
	{
		int nBufferLen = 0;
		bool bExists = m_pDataX->GetWString(nKeyID, NULL, nBufferLen);
		if(!bExists)
			return strDefault;

		std::wstring str(nBufferLen, 0);
		bExists = m_pDataX->GetWString(nKeyID, (LPWSTR)str.c_str(), nBufferLen);
		return str;
	}
}

inline	IDataX* DxWrapper::GetDataX(short nKeyID) 
{
	if(m_pDataX == NULL)
		return NULL;
	else
	{
		IDataX* pDx = NULL;
		bool bExists = m_pDataX->GetDataX(nKeyID, &pDx);
		return bExists ? pDx : NULL;
	}
}

inline std::string DxWrapper::GetUTF8String(short nKeyID, const std::string& strDefault) 
{
	if(m_pDataX == NULL)
		return strDefault;
	else
	{
		int nBufferLen = 0;
		bool bExists = m_pDataX->GetUTF8String(nKeyID, NULL, nBufferLen);
		if(!bExists)
			return strDefault;

		std::string str(nBufferLen, 0);
		bExists = m_pDataX->GetUTF8String(nKeyID, (byte*)str.c_str(), nBufferLen);
		return str;
	}
}

inline
int DxWrapper::GetDataXArraySize(short nKeyID)
{
	int size = 0;
	m_pDataX->GetDataXArraySize(nKeyID, size);
	return size;
}

inline
IDataX* DxWrapper::GetDataXArrayElement(short nKeyID, int nIndex)
{
	IDataX* dx;
	if(m_pDataX->GetDataXArrayElement(nKeyID, nIndex, &dx))
	{
		return dx;
	}
	else
	{
		return NULL;
	}
}
	
#endif // #ifndef _DATAX_NET_WRAPPER_H_

