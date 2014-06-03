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
**  "����"���ݽ����ӿڣ��ͻ��˺ʹ����������ݿ�ͨ���˽ӿ�
**
**  ʹ�÷�����������ͨ�� PutXXX���úú󣬵���Encode()�������������л�,
**            �����л��Ķ��������ݷ���VARIANT������ΪSafeArray���ݸ��Է�
**********************************************************************/
class IDataX
{
public:
	virtual ~IDataX() { }

	enum { DX_MAGIC_NUM = 0x385A };

	// ���ظ��������Ϊ����������������(��λ���ֽ���)
	virtual unsigned int EncodedLength() = 0;

	/***********************************************************************************
	* ���������Ϊbuffer��ָ���ڴ��ϵĶ�������
	* buffer_size: [IN/OUT] ����ǰΪ�������ĳ�ʼ��С�����ú�����Ϊ�������������ĳ���
	***********************************************************************************/
	virtual void Encode(void * buffer, unsigned int &buffer_size) = 0;

	virtual bool Decode(byte* pbBuffer, int& nBufferLen) = 0;

	// ���Լ���������ȫ����һ��
	virtual IDataX* Clone() = 0;
	// �����е��������
	virtual void Clear() = 0;
	// ����Ԫ������
	virtual int GetSize() = 0;

	// ������ݲ����ӿ�
	// --------------------
	// ���Short��������
	virtual bool PutShort(unsigned short nKeyID, short nData) = 0;
	// ���Int��������
	virtual bool PutInt(unsigned short nKeyID, int nData) = 0;
	// ���64λ��������
	virtual bool PutInt64(unsigned short nKeyID, __int64 nData) = 0;
	// ����ֽ����������
	virtual bool PutBytes(unsigned short nKeyID, const byte* pbData, int nDataLen) = 0;
	// ��ӿ��ֽ��ַ���
	virtual bool PutWString(unsigned short nKeyID, LPCWSTR pwszData, int nStringLen) = 0;
	// Ƕ��IDataX����
	//  ����: bGiveupOwnershipֵΪtrue�����ʾ�����߽�pDataCmd����Ȩת�Ƹ��ú����������ߺ��治�����ͷ�
	//        bGiveupOwnershipֵΪfalse, ���ʾ��������ִ�д˺�����Ҫ�����ͷ�pDataCmd���ú��������ڲ�����һ��pDataCmd����
	virtual bool PutDataX(unsigned short nKeyID, IDataX* pDataCmd, bool bGiveupOwnership = false) = 0;
	// ���Int���������
	virtual bool PutIntArray(unsigned short nKeyID, int* pnData, int nElements) = 0;
	// ���IDataX����
	//  ����: bGiveupOwnership˵���μ�PutDataX()
	virtual bool PutDataXArray(unsigned short nKeyID, IDataX** ppDataCmd, int nElements, bool bGiveupOwnership = false) = 0;
	//�滻�����IDataX����Ԫ��
	//  ����: bGiveupOwnership˵���μ�PutDataX()
	//        nIndex ����λ�ã�������ڵ���0����С�ڵ�ǰԪ���������滻����Ԫ�أ������������Ԫ�������ں�����ӣ���ʱnIndexΪʵ��λ��
	virtual bool PutDataXArrayElement(unsigned short nKeyID, int& nIndex, IDataX* pDataCmd, bool bGiveupOwnership = false) = 0;

	//���UTF-8�ַ�������
	virtual bool PutUTF8String(short nKeyID, const byte* pbData, int nDataLen) = 0;
	// ���float��������
	virtual bool PutFloat(unsigned short nKeyID, float fData) = 0;
	// ���Int���������
	virtual bool PutInt64Array(unsigned short nKeyID, __int64* pnData, int nElements) = 0;

	// ��ȡ���ݲ����ӿ�
	// ------------------------
	// ��ȡShort��������
	virtual bool GetShort(unsigned short nKeyID, short& nData) = 0;
	// ��ȡInt��������
	virtual bool GetInt(unsigned short nKeyID, int& nData) = 0;
	// ��ȡ64λ��������
	virtual bool GetInt64(unsigned short nKeyID, __int64& nData) = 0;
	// ��ȡ�ֽ�����(���pbDataBufΪNULL, �����nBufferLen���ø��ֽ��������ݵ�ʵ�ʳ���)
	virtual bool GetBytes(unsigned short nKeyID, byte* pbDataBuf, int& nBufferLen) = 0;
	// ��ȡ���ֽ��ַ���(���pwszDataBufΪNULL, �����nStringLen���ø��ַ�����ʵ�ʳ���)
	virtual bool GetWString(unsigned short nKeyID, LPWSTR pwszDataBuf, int& nStringLen) = 0;
	// ��ȡǶ���������IDataX
	virtual bool GetDataX(unsigned short nKeyID, IDataX** ppDataCmd) = 0;
	// ��ȡ���������Ԫ������
	virtual bool GetIntArraySize(unsigned short nKeyID, int& nSize) = 0;
	// ��ȡ���������ĳ��Ԫ�أ�����������ţ�
	virtual bool GetIntArrayElement(unsigned short nKeyID, int nIndex, int& nData) = 0;
	// ��ȡIDataX�����Ԫ������
	virtual bool GetDataXArraySize(unsigned short nKeyID, int& nSize) = 0;
	// ��ȡIDataX�����ĳ��Ԫ�أ�����������ţ�
	virtual bool GetDataXArrayElement(unsigned short nKeyID, int nIndex, IDataX** ppDataCmd) = 0;
	//��ȡUTF-8�ַ�������
	virtual bool GetUTF8String(short nKeyID, byte* pbDataBuf, int& nBufferLen) = 0;
	// ��ȡFloat��������
	virtual bool GetFloat(unsigned short nKeyID, float& fData) = 0;
	// ��ȡ64���������Ԫ������
	virtual bool GetInt64ArraySize(unsigned short nKeyID, int& nSize) = 0;
	// ��ȡ64���������ĳ��Ԫ�أ�����������ţ�
	virtual bool GetInt64ArrayElement(unsigned short nKeyID, int nIndex, __int64& nData) = 0;

	//���ַ�����ʽ���������ݣ����ڵ���
	virtual std::string ToString() = 0;
};

#endif //  #define __XL_IDATAX_HEADER_20090328_

