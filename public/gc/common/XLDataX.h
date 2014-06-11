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

	// ���ظ��������Ϊ����������������(��λ���ֽ���)
	virtual unsigned int EncodedLength();

	/***********************************************************************************
	* ���������Ϊbuffer��ָ���ڴ��ϵĶ�������
	* buffer_size: [IN/OUT] ����ǰΪ�������ĳ�ʼ��С�����ú�����Ϊ�������������ĳ���
	**********************************************************************************/
	virtual void Encode(void * buffer, unsigned int &buffer_size);

	// ���Լ���������ȫ����һ��
	virtual IDataX* Clone();
	// �����е��������
	virtual void Clear();
	// ����Ԫ������
	virtual int GetSize();

	// ������ݲ����ӿ�
	// --------------------
	// ���Short��������
	virtual bool PutShort(unsigned short nKeyID, short nData);
	// ���Int��������
	virtual bool PutInt(unsigned short nKeyID, int nData);
	// ���64λ��������
	virtual bool PutInt64(unsigned short nKeyID, int64_t nData);
	// ����ֽ����������
	virtual bool PutBytes(unsigned short nKeyID, const byte* pbData, int nDataLen);
	// ��ӿ��ֽ��ַ���
	virtual bool PutWString(unsigned short nKeyID, LPCWSTR pwszData, int nStringLen);
	// Ƕ��IDataX����
	virtual bool PutDataX(unsigned short nKeyID, IDataX* pDataCmd, bool bGiveupOwnership = false);
	// ���Int���������
	virtual bool PutIntArray(unsigned short nKeyID, int* pnData, int nElements);
	// ���IDataX����
	virtual bool PutDataXArray(unsigned short nKeyID, IDataX** ppDataCmd, int nElements, bool bGiveupOwnership = false);
	//�滻�����IDataX����Ԫ��
	virtual bool PutDataXArrayElement(unsigned short nKeyID, int& nIndex, IDataX* pDataCmd, bool bGiveupOwnership = false);

	//���UTF-8�ַ�������
	virtual bool PutUTF8String(short nKeyID, const byte* pbData, int nDataLen);
	// ���float��������
	virtual bool PutFloat(unsigned short nKeyID, float fData);
    
	// ���Int���������
	virtual bool PutInt64Array(unsigned short nKeyID, int64_t* pnData, int nElements);
	
	// ��ȡ���ݲ����ӿ�
	// ------------------------
	// ��ȡShort��������
	virtual bool GetShort(unsigned short nKeyID, short& nData);
	// ��ȡInt��������
	virtual bool GetInt(unsigned short nKeyID, int& nData);
	// ��ȡ64λ��������
	virtual bool GetInt64(unsigned short nKeyID, int64_t& nData) ;
	// ��ȡ�ֽ�����(���pbDataBufΪNULL, �����nBufferLen���ø��ֽ��������ݵ�ʵ�ʳ���)
	virtual bool GetBytes(unsigned short nKeyID, byte* pbDataBuf, int& nBufferLen);
	// ��ȡ���ֽ��ַ���(���pwszDataBufΪNULL, �����nStringLen���ø��ַ�����ʵ�ʳ���)
	virtual bool GetWString(unsigned short nKeyID, LPWSTR pwszDataBuf, int& nStringLen) ;
	// ��ȡǶ���������IDataX
	virtual bool GetDataX(unsigned short nKeyID, IDataX** ppDataCmd);
	// ��ȡ���������Ԫ������
	virtual bool GetIntArraySize(unsigned short nKeyID, int& nSize);
	// ��ȡ���������ĳ��Ԫ�أ�����������ţ�
	virtual bool GetIntArrayElement(unsigned short nKeyID, int nIndex, int& nData);
	// ��ȡIDataX�����Ԫ������
	virtual bool GetDataXArraySize(unsigned short nKeyID, int& nSize);
	// ��ȡIDataX�����ĳ��Ԫ�أ�����������ţ�
	virtual bool GetDataXArrayElement(unsigned short nKeyID, int nIndex, IDataX** ppDataCmd);	
	//��ȡUTF-8�ַ�������
	virtual bool GetUTF8String(short nKeyID, byte* pbDataBuf, int& nBufferLen);
	// ��ȡFloat��������
	virtual bool GetFloat(unsigned short nKeyID, float& fData);
	// ��ȡ64���������Ԫ������
	virtual bool GetInt64ArraySize(unsigned short nKeyID, int& nSize);
	// ��ȡ64���������ĳ��Ԫ�أ�����������ţ�
	virtual bool GetInt64ArrayElement(unsigned short nKeyID, int nIndex, int64_t& nData);

	//���ַ�����ʽ���������ݣ����ڵ���
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
