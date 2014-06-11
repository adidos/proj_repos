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
	// ��ȡInt��������
	int GetInt(short nKeyID, int nDefault = 0);
	// ��ȡ��������ֵ
	float GetFloat(short nKeyID, float fData = 0.0);
	// ��ȡ64λ��������
	int64_t GetInt64(short nKeyID, int64_t nDefault = 0);
	// ��ȡ�ֽ�����(���pbDataBufΪNULL, �����nBufferLen���ø��ֽ��������ݵ�ʵ�ʳ���)
	std::string GetBytes(short nKeyID, const std::string& strDefault = "");
	// ��ȡ���ֽ��ַ���(���pwszDataBufΪNULL, �����nStringLen���ø��ַ�����ʵ�ʳ���)
	std::wstring GetWString(short nKeyID, const std::wstring& strDefault= L"");
	// ��ȡǶ���������IDataX
	IDataX* GetDataX(short nKeyID);
	// ��ȡ���������Ԫ������
	int GetIntArraySize(short nKeyID) { return 0;}
	// ��ȡ���������ĳ��Ԫ�أ�����������ţ�
	int GetIntArrayElement(short nKeyID, int nIndex) { return 0; }
	// ��ȡIDataX�����Ԫ������
	int GetDataXArraySize(short nKeyID);
	// ��ȡIDataX�����ĳ��Ԫ�أ�����������ţ�
	IDataX* GetDataXArrayElement(short nKeyID, int nIndex) ;
	// ��ȡUTF8������ֽ�����(���pbDataBufΪNULL, �����nBufferLen���ø��ֽ��������ݵ�ʵ�ʳ���)
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

