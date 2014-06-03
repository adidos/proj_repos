#include "XLDataX.h"
#include <sstream>
#include <iterator>


using namespace std;

#ifdef LOGGER
IMPL_LOGGER(XLDataX, logger);
#endif


XLDataX::XLDataX(void)
{
    
}

XLDataX::~XLDataX(void)
{
    Clear();
}


unsigned int XLDataX::EncodedLength()
{
    unsigned int nLength = sizeof(short) + sizeof(int); // magic_num + total_remain_bytes

    for (map<unsigned short, int>::iterator it = m_mapIndexes.begin(); it != m_mapIndexes.end(); it++)
    {
        unsigned short nKey = it->first;
        int nIndex = it->second;

        int nType = GET_TYPE_FROM_DX_INDEX(nIndex);
        int nPos = GET_POS_FROM_DX_INDEX(nIndex);

        nLength += (sizeof(byte) + sizeof(nKey));  // nKey + nType

        switch (nType)
        {
        case DX_TYPE_SHORT:
        {
            nLength += sizeof(short);

            break;
        }
        case DX_TYPE_INT:
        {
            nLength += sizeof(int);

            break;
        }
        case DX_TYPE_INT64:
        {
            nLength += sizeof(__int64);

            break;
        }
        case DX_TYPE_BYTES:
        {
            nLength += CalcBytesItemLen(nPos);

            break;
        }
        case DX_TYPE_WSTRING:
        {
            nLength += CalcWStringItemLen(nPos);

            break;
        }
        case DX_TYPE_DATAX:
        {
            nLength += CalcDataXItemLen(nPos);

            break;
        }
        case DX_TYPE_INTARRAY:
        {
            nLength += CalcIntArrayItemLen(nPos);

            break;
        }
        case DX_TYPE_DATAXARRAY:
        {
            nLength += CalcDataXArrayItemLen(nPos);

            break;
        }
        case DX_TYPE_FLOAT:
        {
            nLength += sizeof(float);

            break;
        }
        case DX_TYPE_UTF8BYTES:
        {
            nLength += CalcUTF8BytesItemLen(nPos);

            break;
        }
        default:
        {
            ;

            break;
        }
        }
    }

    return nLength;
}

/***********************************************************************************
* 将命令编码为buffer所指向内存上的二进制流
* buffer_size: [IN/OUT] 调用前为缓冲区的初始大小，调用后设置为编码后二进制流的长度
***********************************************************************************/
void XLDataX::Encode(void * buffer, unsigned int &buffer_size)
{
    FixedBuffer fixed_buffer((char*)buffer, buffer_size, true);

    fixed_buffer.put_short(DX_MAGIC_NUM);

    // remain bytes
    int nRemainBytesPos = fixed_buffer.position();
    fixed_buffer.skip(sizeof(int));

    int nStartPos = fixed_buffer.position();
    for (map<unsigned short, int>::iterator it = m_mapIndexes.begin(); it != m_mapIndexes.end(); it++)
    {
        unsigned short nKey = it->first;
        int nIndex = it->second;

        int nType = GET_TYPE_FROM_DX_INDEX(nIndex);
        int nPos = GET_POS_FROM_DX_INDEX(nIndex);

        fixed_buffer.put_short(nKey);
        fixed_buffer.put_byte((byte)nType);
        switch (nType)
        {
        case DX_TYPE_SHORT:
        {
            fixed_buffer.put_short((short)m_vecIntItems[nPos]);

            break;
        }
        case DX_TYPE_INT:
        {
            fixed_buffer.put_int(m_vecIntItems[nPos]);

            break;
        }
        case DX_TYPE_INT64:
        {
            fixed_buffer.put_int64(m_vecInt64Items[nPos]);

            break;
        }
        case DX_TYPE_BYTES:
        {
            EncodeBytesItem(fixed_buffer, nPos);

            break;
        }
        case DX_TYPE_WSTRING:
        {
            EncodeWStringItem(fixed_buffer, nPos);

            break;
        }
        case DX_TYPE_DATAX:
        {
            EncodeDataXItem(fixed_buffer, nPos);

            break;
        }
        case DX_TYPE_INTARRAY:
        {
            EncodeIntArrayItem(fixed_buffer, nPos);

            break;
        }
        case DX_TYPE_DATAXARRAY:
        {
            EncodeDataXArrayItem(fixed_buffer, nPos);

            break;
        }
        case DX_TYPE_UTF8BYTES:
        {
            EncodeUTF8BytesItem(fixed_buffer, nPos);

            break;
        }
        case DX_TYPE_FLOAT:
        {
            EncodeFloatItem(fixed_buffer, nPos);

            break;
        }
        default:
        {
           ;

            break;
        }
        }
    }

    int nEndPos = fixed_buffer.position();
    int nTotalBytes = nEndPos - nStartPos;
    fixed_buffer.set_position(nRemainBytesPos);
    fixed_buffer.put_int(nTotalBytes);

    fixed_buffer.set_position(nEndPos);

    buffer_size = nTotalBytes + sizeof(short) + sizeof(int);
}

IDataX* XLDataX::DecodeFrom(byte* pbBuffer, int& nBufferLen)
{
    if (nBufferLen < (int)(sizeof(short) + sizeof(int)))
        return NULL;

    FixedBuffer fixed_buf_1((char*)pbBuffer, nBufferLen, true);
    short nMagicNum = fixed_buf_1.get_short();
    if (nMagicNum != XLDataX::DX_MAGIC_NUM)
        return NULL;

    int nRemainBytes = fixed_buf_1.get_int();
    if (fixed_buf_1.remain_len() < nRemainBytes)
        return NULL;

    XLDataX* pDataX = new XLDataX();
    FixedBuffer fixed_buffer((char*)(pbBuffer + sizeof(short) + sizeof(int)), nRemainBytes, true);

    const int MIN_ITEM_LENGTH = sizeof(short) + sizeof(byte) + sizeof(short);
    BOOL bDecodeOK = TRUE;
    while (fixed_buffer.remain_len() >= MIN_ITEM_LENGTH && bDecodeOK)
    {
        unsigned short nKey = (unsigned short)fixed_buffer.get_short();
        int nType = fixed_buffer.get_byte();

        BOOL bRet = TRUE;
        switch (nType)
        {
        case DX_TYPE_SHORT:
        {
            pDataX->PutShort(nKey, fixed_buffer.get_short());

            break;
        }
        case DX_TYPE_INT:
        {
            pDataX->PutInt(nKey, fixed_buffer.get_int());

            break;
        }
        case DX_TYPE_INT64:
        {
            pDataX->PutInt64(nKey, fixed_buffer.get_int64());

            break;
        }
        case DX_TYPE_BYTES:
        {
            pDataX->PutBytesItem(nKey, fixed_buffer);

            break;
        }
        case DX_TYPE_WSTRING:
        {
            pDataX->PutWStringItem(nKey, fixed_buffer);

            break;
        }
        case DX_TYPE_DATAX:
        {
            bRet = pDataX->PutDataXItem(nKey, fixed_buffer);
            if (bRet == -2)
                bDecodeOK = FALSE;

            break;
        }
        case DX_TYPE_INTARRAY:
        {
            pDataX->PutIntArrayItem(nKey, fixed_buffer);

            break;
        }
        case DX_TYPE_DATAXARRAY:
        {
            bRet = pDataX->PutDataXArrayItem(nKey, fixed_buffer);
            if (bRet == -2)
                bDecodeOK = FALSE;

            break;
        }
        case DX_TYPE_UTF8BYTES:
        {
            pDataX->PutUTF8BytesItem(nKey, fixed_buffer);

            break;
        }
        case DX_TYPE_FLOAT:
        {
            pDataX->PutFloatItem(nKey, fixed_buffer);

            break;
        }
        default:
        {
            ;
            delete pDataX;
            return NULL;
        }
        }

    }

    nBufferLen = nRemainBytes + sizeof(short) + sizeof(int);

    return pDataX;
}

bool XLDataX::Decode(byte* pbBuffer, int& nBufferLen)
{
    if (nBufferLen < (int)(sizeof(short) + sizeof(int)))
        return false;

    FixedBuffer fixed_buf_1((char*)pbBuffer, nBufferLen, true);
    short nMagicNum = fixed_buf_1.get_short();
    if (nMagicNum != XLDataX::DX_MAGIC_NUM)
        return false;

    int nRemainBytes = fixed_buf_1.get_int();
    if (fixed_buf_1.remain_len() < nRemainBytes)
        return false;

    XLDataX* pDataX = this;
    FixedBuffer fixed_buffer((char*)(pbBuffer + sizeof(short) + sizeof(int)), nRemainBytes, true);

    const int MIN_ITEM_LENGTH = sizeof(short) + sizeof(byte) + sizeof(short);
    bool bDecodeOK = true;
    while (fixed_buffer.remain_len() >= MIN_ITEM_LENGTH && bDecodeOK)
    {
        unsigned short nKey = (unsigned short)fixed_buffer.get_short();
        int nType = fixed_buffer.get_byte();

        BOOL bRet = TRUE;
        switch (nType)
        {
        case DX_TYPE_SHORT:
        {
            pDataX->PutShort(nKey, fixed_buffer.get_short());

            break;
        }
        case DX_TYPE_INT:
        {
            pDataX->PutInt(nKey, fixed_buffer.get_int());

            break;
        }
        case DX_TYPE_INT64:
        {
            pDataX->PutInt64(nKey, fixed_buffer.get_int64());

            break;
        }
        case DX_TYPE_BYTES:
        {
            pDataX->PutBytesItem(nKey, fixed_buffer);

            break;
        }
        case DX_TYPE_WSTRING:
        {
            pDataX->PutWStringItem(nKey, fixed_buffer);

            break;
        }
        case DX_TYPE_DATAX:
        {
            bRet = pDataX->PutDataXItem(nKey, fixed_buffer);
            if (bRet == -2)
                bDecodeOK = false;

            break;
        }
        case DX_TYPE_INTARRAY:
        {
            pDataX->PutIntArrayItem(nKey, fixed_buffer);

            break;
        }
        case DX_TYPE_DATAXARRAY:
        {
            bRet = pDataX->PutDataXArrayItem(nKey, fixed_buffer);
            if (bRet == -2)
                bDecodeOK = false;

            break;
        }
        case DX_TYPE_UTF8BYTES:
        {
            pDataX->PutUTF8BytesItem(nKey, fixed_buffer);

            break;
        }
        case DX_TYPE_FLOAT:
        {
            pDataX->PutFloatItem(nKey, fixed_buffer);

            break;
        }
        default:
        {
            ;
            return false;
        }
        }

    }

    nBufferLen = nRemainBytes + sizeof(short) + sizeof(int);
    return bDecodeOK;
}

// 将自己的内容完全复制一份
IDataX* XLDataX::Clone()
{
    XLDataX* pDataX = new XLDataX();

    pDataX->m_mapIndexes = this->m_mapIndexes;
    pDataX->m_vecFloatItems = this->m_vecFloatItems;
    pDataX->m_vecIntItems = this->m_vecIntItems;
    pDataX->m_vecInt64Items = this->m_vecInt64Items;
    pDataX->m_vecBytesItems = this->m_vecBytesItems;
    pDataX->m_vecWstrItems = this->m_vecWstrItems;
    pDataX->m_vecUTF8BytesItems = this->m_vecUTF8BytesItems;
    pDataX->m_vecInt64ArrayItems = this->m_vecInt64ArrayItems;


    for (int i = 0, size = m_vecDataXItems.size(); i < size; i++)
    {
        IDataX* pCopied = m_vecDataXItems[i]->Clone();
        pDataX->m_vecDataXItems.push_back(pCopied);
    }

    pDataX->m_vecIntArrayItems = this->m_vecIntArrayItems;

    for (int i = 0, size = m_vecDataXArrayItems.size(); i < size; i++)
    {
        vector<IDataX*> vecTemp;
        vector<IDataX*>& vecSrc = m_vecDataXArrayItems[i];
        for (int j = 0, nSize = vecSrc.size(); j < nSize; j++)
        {
            IDataX* pCopied = vecSrc[j]->Clone();
            vecTemp.push_back(pCopied);
        }
        pDataX->m_vecDataXArrayItems.push_back(vecTemp);
    }


    return pDataX;
}

// 将已有的数据清空
void XLDataX::Clear()
{
    m_mapIndexes.clear();

    m_vecIntItems.clear();
    m_vecInt64Items.clear();
    m_vecBytesItems.clear();
    m_vecWstrItems.clear();

    for (int i = 0; i < (int)m_vecDataXItems.size(); i++)
    {
        IDataX* pDataX = m_vecDataXItems[i];
        delete pDataX;
    }
    m_vecDataXItems.clear();

    m_vecIntArrayItems.clear();

    for (int i = 0; i < (int)m_vecDataXArrayItems.size(); i++)
    {
        vector<IDataX*>& vecDataX = m_vecDataXArrayItems[i];
        for (int j = 0; j < (int)vecDataX.size(); j++)
        {
            IDataX* pDataX = vecDataX[j];
            delete pDataX;
        }
        vecDataX.clear();
    }
    m_vecDataXArrayItems.clear();
}

// 返回元素数量
int XLDataX::GetSize()
{
    return (int)m_mapIndexes.size();
}


// 添加Short类型数据
bool XLDataX::PutShort(unsigned short nKeyID, short nData)
{
    if (m_mapIndexes.find(nKeyID) != m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    m_vecIntItems.push_back(nData);
    int nSize = (int)m_vecIntItems.size();
    int nIndex = nSize - 1;

    m_mapIndexes[nKeyID] = MAKE_DATAX_INDEX(DX_TYPE_SHORT, nIndex);

    return true;
}

// 添加Int类型数据
bool XLDataX::PutInt(unsigned short nKeyID, int nData)
{
    if (m_mapIndexes.find(nKeyID) != m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    m_vecIntItems.push_back(nData);
    int nSize = (int)m_vecIntItems.size();
    int nIndex = nSize - 1;

    m_mapIndexes[nKeyID] = MAKE_DATAX_INDEX(DX_TYPE_INT, nIndex);

    return true;
}

// 添加64位整型数据
bool XLDataX::PutInt64(unsigned short nKeyID, __int64 nData)
{
    if (m_mapIndexes.find(nKeyID) != m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    m_vecInt64Items.push_back(nData);
    int nSize = (int)m_vecInt64Items.size();
    int nIndex = nSize - 1;

    m_mapIndexes[nKeyID] = MAKE_DATAX_INDEX(DX_TYPE_INT64, nIndex);

    return true;
}

// 添加字节数组的内容
bool XLDataX::PutBytes(unsigned short nKeyID, const byte* pbData, int nDataLen)
{
    if (m_mapIndexes.find(nKeyID) != m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    if (pbData == NULL)
    {
        // TODO: add log
        return false;
    }
    if (nDataLen < 0 || nDataLen > 65535)
    {
        // TODO: add log
        return false;
    }

    string strData;
    strData.assign((char*)pbData, nDataLen);

    m_vecBytesItems.push_back(strData);
    int nSize = (int)m_vecBytesItems.size();
    int nIndex = nSize - 1;

    m_mapIndexes[nKeyID] = MAKE_DATAX_INDEX(DX_TYPE_BYTES, nIndex);

    return true;
}

// 添加宽字节字符串
bool XLDataX::PutWString(unsigned short nKeyID, LPCWSTR pwszData, int nStringLen)
{
    if (m_mapIndexes.find(nKeyID) != m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    if (pwszData == NULL)
    {
        // TODO: add log
        return false;
    }
    if (nStringLen < 0 || nStringLen > 65535)
    {
        // TODO: add log
        return false;
    }

    std::wstring strData;
    strData.assign((wchar_t*)pwszData, nStringLen);

    m_vecWstrItems.push_back(strData);
    int nSize = (int)m_vecWstrItems.size();
    int nIndex = nSize - 1;

    m_mapIndexes[nKeyID] = MAKE_DATAX_INDEX(DX_TYPE_WSTRING, nIndex);

    return true;
}

// 嵌入IDataX内容
bool XLDataX::PutDataX(unsigned short nKeyID, IDataX* pDataCmd,bool bGiveupOwnership)
{
    if (m_mapIndexes.find(nKeyID) != m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    if (pDataCmd == NULL)
    {
        // TODO: add log
        return false;
    }

    IDataX* pDataXTemp = NULL;
    if (bGiveupOwnership)
    {
        // 调用方放弃控制权，直接使用
        pDataXTemp = pDataCmd;
    }
    else
    {
        // 调用方负责释放，则这里要拷贝一份
        pDataXTemp = pDataCmd->Clone();
    }

    m_vecDataXItems.push_back(pDataXTemp);
    int nSize = (int)m_vecDataXItems.size();
    int nIndex = nSize - 1;

    m_mapIndexes[nKeyID] = MAKE_DATAX_INDEX(DX_TYPE_DATAX, nIndex);

    return true;
}

// 添加Int数组的内容
bool XLDataX::PutIntArray(unsigned short nKeyID, int* pnData, int nElements)
{
    if (m_mapIndexes.find(nKeyID) != m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    if (pnData == NULL && nElements > 0)
    {
        // TODO: add log
        return false;
    }
    if (nElements < 0 || nElements > 65535)
    {
        // TODO: add log
        return false;
    }

    vector<int> vecDatas;
	if(pnData != NULL && nElements > 0)
		vecDatas.assign(pnData, pnData + nElements);

    m_vecIntArrayItems.push_back(vecDatas);
    int nSize = (int)m_vecIntArrayItems.size();
    int nIndex = nSize - 1;

    m_mapIndexes[nKeyID] = MAKE_DATAX_INDEX(DX_TYPE_INTARRAY, nIndex);

    return true;
}

// 添加IDataX数组
bool XLDataX::PutDataXArray(unsigned short nKeyID, IDataX** ppDataCmd, int nElements, bool bGiveupOwnership)
{
    if (m_mapIndexes.find(nKeyID) != m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    if (ppDataCmd == NULL && nElements > 0)
    {
        // TODO: add log
        return false;
    }
    if (nElements < 0 || nElements > 65535)
    {
        // TODO: add log
        return false;
    }

    vector<IDataX*> vecDataXs;
    vecDataXs.reserve(nElements);
    for (int i = 0; i < nElements; i++)
    {
        if (bGiveupOwnership)
            vecDataXs.push_back(ppDataCmd[i]);
        else
            vecDataXs.push_back(ppDataCmd[i]->Clone());
    }

    m_vecDataXArrayItems.push_back(vecDataXs);
    int nSize = (int)m_vecDataXArrayItems.size();
    int nIndex = nSize - 1;

    m_mapIndexes[nKeyID] = MAKE_DATAX_INDEX(DX_TYPE_DATAXARRAY, nIndex);

    return true;
}

// 插入/替换IDataX数组的某个元素（根据索引编号）
bool XLDataX::PutDataXArrayElement(unsigned short nKeyID, int& nIndex, IDataX* pDataCmd, bool bGiveupOwnership)
{
    if (pDataCmd == NULL)
    {
        return false;
    }

    map<unsigned short, int>::iterator it = m_mapIndexes.find(nKeyID);
    if (it == m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    int nMyIndex = it->second;
    int nType = GET_TYPE_FROM_DX_INDEX(nMyIndex);
    if (!(nType == DX_TYPE_DATAXARRAY))
    {
        // TODO: add log (type mismatch)
        return false;
    }
    int nPos = GET_POS_FROM_DX_INDEX(nMyIndex);
    if (nPos >= 0 && nPos < (int)m_vecDataXArrayItems.size())
    {
        vector<IDataX*>& vecTemp = m_vecDataXArrayItems[nPos];
        if (nIndex >= 0 && nIndex < (int)vecTemp.size())
        {
            delete vecTemp[nIndex];
            if (bGiveupOwnership)
            {
                vecTemp[nIndex] = pDataCmd;
            }
            else 
            {
                vecTemp[nIndex] = pDataCmd->Clone();
            }
        }
        else
        {
            if (bGiveupOwnership)
            {
                vecTemp.push_back(pDataCmd);
            }
            else 
            {
                vecTemp.push_back(pDataCmd->Clone());
            }
        }
    }
    else
    {
        //TODO: add log (index out of range)
        return false;
    }

    return true;
}


bool XLDataX::PutUTF8String(short nKeyID, const byte * pbData, int nDataLen)
{
    if (m_mapIndexes.find(nKeyID) != m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    if (pbData == NULL)
    {
        // TODO: add log
        return false;
    }
    if (nDataLen < 0 || nDataLen > 65535)
    {
        // TODO: add log
        return false;
    }

    string strData;
    strData.assign((char*)pbData, nDataLen);

    m_vecUTF8BytesItems.push_back(strData);
    int nSize = (int)m_vecUTF8BytesItems.size();
    int nIndex = nSize - 1;

    m_mapIndexes[nKeyID] = MAKE_DATAX_INDEX(DX_TYPE_UTF8BYTES, nIndex);

    return true;
}

bool XLDataX::PutFloat(unsigned short nKeyID, float fData)
{
    if (m_mapIndexes.find(nKeyID) != m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    m_vecFloatItems.push_back(fData);
    int nSize = (int)m_vecFloatItems.size();
    int nIndex = nSize - 1;

    m_mapIndexes[nKeyID] = MAKE_DATAX_INDEX(DX_TYPE_FLOAT, nIndex);

    return true;
}

// 添加Int64数组的内容
bool XLDataX::PutInt64Array(unsigned short nKeyID, __int64* pnData, int nElements)
{
    if (m_mapIndexes.find(nKeyID) != m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    if (pnData == NULL && nElements > 0)
    {
        // TODO: add log
        return false;
    }
    if (nElements < 0 || nElements > 65535)
    {
        // TODO: add log
        return false;
    }

    vector<__int64> vecDatas;
	if(pnData != NULL && nElements > 0)
		vecDatas.assign(pnData, pnData + nElements);

    m_vecInt64ArrayItems.push_back(vecDatas);
    int nSize = (int)m_vecInt64ArrayItems.size();
    int nIndex = nSize - 1;

    m_mapIndexes[nKeyID] = MAKE_DATAX_INDEX(DX_TYPE_INT64ARRAY, nIndex);

    return true;
}


// 获取Short类型数据
bool XLDataX::GetShort(unsigned short nKeyID, short& nData)
{
    nData = 0;
    map<unsigned short, int>::iterator it = m_mapIndexes.find(nKeyID);
    if (it == m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    int nIndex = it->second;
    int nType = GET_TYPE_FROM_DX_INDEX(nIndex);
    if (!(nType == DX_TYPE_SHORT || nType == DX_TYPE_INT))
    {
        // TODO: add log (type mismatch)
        return false;
    }
    int nPos = GET_POS_FROM_DX_INDEX(nIndex);
    if (nPos >= 0 && nPos < (int)m_vecIntItems.size())
    {
        nData = (short)m_vecIntItems[nPos];
    }
    else
    {
        //TODO: add log (index out of range)
        return false;
    }

    return true;
}

// 获取Int类型数据
bool XLDataX::GetInt(unsigned short nKeyID, int& nData)
{
    nData = 0;
    map<unsigned short, int>::iterator it = m_mapIndexes.find(nKeyID);
    if (it == m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    int nIndex = it->second;
    int nType = GET_TYPE_FROM_DX_INDEX(nIndex);
    if (!(nType == DX_TYPE_SHORT || nType == DX_TYPE_INT))
    {
        // TODO: add log (type mismatch)
        return false;
    }
    int nPos = GET_POS_FROM_DX_INDEX(nIndex);
    if (nPos >= 0 && nPos < (int)m_vecIntItems.size())
    {
        nData = m_vecIntItems[nPos];
    }
    else
    {
        //TODO: add log (index out of range)
        return false;
    }

    return true;
}

// 获取64位整型数据
bool XLDataX::GetInt64(unsigned short nKeyID, __int64& nData)
{
    nData = 0;
    map<unsigned short, int>::iterator it = m_mapIndexes.find(nKeyID);
    if (it == m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    int nIndex = it->second;
    int nType = GET_TYPE_FROM_DX_INDEX(nIndex);
    if (!(nType == DX_TYPE_INT64))
    {
        // TODO: add log (type mismatch)
        return false;
    }
    int nPos = GET_POS_FROM_DX_INDEX(nIndex);
    if (nPos >= 0 && nPos < (int)m_vecInt64Items.size())
    {
        nData = m_vecInt64Items[nPos];
    }
    else
    {
        //TODO: add log (index out of range)
        return false;
    }

    return true;
}

// 获取字节数组(如果pbDataBuf为NULL, 则会在nBufferLen设置该字节数组内容的实际长度)
bool XLDataX::GetBytes(unsigned short nKeyID, byte* pbDataBuf, int& nBufferLen)
{
    map<unsigned short, int>::iterator it = m_mapIndexes.find(nKeyID);
    if (it == m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    int nIndex = it->second;
    int nType = GET_TYPE_FROM_DX_INDEX(nIndex);
    if (!(nType == DX_TYPE_BYTES))
    {
        // TODO: add log (type mismatch)
        return false;
    }
    int nPos = GET_POS_FROM_DX_INDEX(nIndex);
    if (nPos >= 0 && nPos < (int)m_vecBytesItems.size())
    {
        string& strData = m_vecBytesItems[nPos];
        if (pbDataBuf == NULL)
        {
            nBufferLen = (int)strData.length();
        }
        else
        {
            if (nBufferLen < (int)strData.length())
            {
                //TODO: add log (buffer not enough)
                return false;
            }
            memcpy(pbDataBuf, strData.c_str(), strData.length());
            nBufferLen = (int)strData.length();
        }
    }
    else
    {
        //TODO: add log (index out of range)
        return false;
    }

    return true;
}

// 获取宽字节字符串(如果pwszDataBuf为NULL, 则会在nStringLen设置该字符串的实际长度)
bool XLDataX::GetWString(unsigned short nKeyID, LPWSTR pwszDataBuf, int& nStringLen)
{
    map<unsigned short, int>::iterator it = m_mapIndexes.find(nKeyID);
    if (it == m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    int nIndex = it->second;
    int nType = GET_TYPE_FROM_DX_INDEX(nIndex);
    if (!(nType == DX_TYPE_WSTRING))
    {
        // TODO: add log (type mismatch)
        return false;
    }
    int nPos = GET_POS_FROM_DX_INDEX(nIndex);
    if (nPos >= 0 && nPos < (int)m_vecWstrItems.size())
    {
        std::wstring& strData = m_vecWstrItems[nPos];
        if (pwszDataBuf == NULL)
        {
            nStringLen = (int)strData.length();
        }
        else
        {
            if (nStringLen < (int)strData.length())
            {
                //TODO: add log (buffer not enough)
                return false;
            }
            memcpy(pwszDataBuf, strData.c_str(), strData.length() * sizeof(wchar_t));
            nStringLen = (int)strData.length();
        }
    }
    else
    {
        //TODO: add log (index out of range)
        return false;
    }

    return true;
}

// 获取嵌入在里面的IDataX
bool XLDataX::GetDataX(unsigned short nKeyID, IDataX** ppDataCmd)
{
    if (ppDataCmd == NULL)
    {
        return false;
    }

    map<unsigned short, int>::iterator it = m_mapIndexes.find(nKeyID);
    if (it == m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    int nIndex = it->second;
    int nType = GET_TYPE_FROM_DX_INDEX(nIndex);
    if (!(nType == DX_TYPE_DATAX))
    {
        // TODO: add log (type mismatch)
        return false;
    }
    int nPos = GET_POS_FROM_DX_INDEX(nIndex);
    if (nPos >= 0 && nPos < (int)m_vecDataXItems.size())
    {
        *ppDataCmd = m_vecDataXItems[nPos]->Clone();
    }
    else
    {
        //TODO: add log (index out of range)
        return false;
    }

    return true;
}

// 获取整型数组的元素数量
bool XLDataX::GetIntArraySize(unsigned short nKeyID, int& nSize)
{
    nSize = 0;

    map<unsigned short, int>::iterator it = m_mapIndexes.find(nKeyID);
    if (it == m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    int nIndex = it->second;
    int nType = GET_TYPE_FROM_DX_INDEX(nIndex);
    if (!(nType == DX_TYPE_INTARRAY))
    {
        // TODO: add log (type mismatch)
        return false;
    }
    int nPos = GET_POS_FROM_DX_INDEX(nIndex);
    if (nPos >= 0 && nPos < (int)m_vecIntArrayItems.size())
    {
        vector<int>& vecTemp = m_vecIntArrayItems[nPos];
        nSize = (int)vecTemp.size();
    }
    else
    {
        //TODO: add log (index out of range)
        return false;
    }

    return true;
}

// 获取整型数组的某个元素（根据索引编号）
bool XLDataX::GetIntArrayElement(unsigned short nKeyID, int nIndex, int& nData)
{
    nData = 0;

    map<unsigned short, int>::iterator it = m_mapIndexes.find(nKeyID);
    if (it == m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    int nMyIndex = it->second;
    int nType = GET_TYPE_FROM_DX_INDEX(nMyIndex);
    if (!(nType == DX_TYPE_INTARRAY))
    {
        // TODO: add log (type mismatch)
        return false;
    }
    int nPos = GET_POS_FROM_DX_INDEX(nMyIndex);
    if (nPos >= 0 && nPos < (int)m_vecIntArrayItems.size())
    {
        vector<int>& vecTemp = m_vecIntArrayItems[nPos];
        if (nIndex >= 0 && nIndex < (int)vecTemp.size())
        {
            nData = vecTemp[nIndex];
        }
        else
        {
            return false;
        }
    }
    else
    {
        //TODO: add log (index out of range)
        return false;
    }

    return true;
}

// 获取IDataX数组的元素数量
bool XLDataX::GetDataXArraySize(unsigned short nKeyID, int& nSize)
{
    nSize = 0;

    map<unsigned short, int>::iterator it = m_mapIndexes.find(nKeyID);
    if (it == m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    int nIndex = it->second;
    int nType = GET_TYPE_FROM_DX_INDEX(nIndex);
    if (!(nType == DX_TYPE_DATAXARRAY))
    {
        // TODO: add log (type mismatch)
        return false;
    }
    int nPos = GET_POS_FROM_DX_INDEX(nIndex);
    if (nPos >= 0 && nPos < (int)m_vecDataXArrayItems.size())
    {
        vector<IDataX*>& vecTemp = m_vecDataXArrayItems[nPos];
        nSize = (int)vecTemp.size();
    }
    else
    {
        //TODO: add log (index out of range)
        return false;
    }

    return true;
}

// 获取IDataX数组的某个元素（根据索引编号）
bool XLDataX::GetDataXArrayElement(unsigned short nKeyID, int nIndex, IDataX** ppDataCmd)
{
    if (ppDataCmd == NULL)
    {
        return false;
    }

    map<unsigned short, int>::iterator it = m_mapIndexes.find(nKeyID);
    if (it == m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    int nMyIndex = it->second;
    int nType = GET_TYPE_FROM_DX_INDEX(nMyIndex);
    if (!(nType == DX_TYPE_DATAXARRAY))
    {
        // TODO: add log (type mismatch)
        return false;
    }
    int nPos = GET_POS_FROM_DX_INDEX(nMyIndex);
    if (nPos >= 0 && nPos < (int)m_vecDataXArrayItems.size())
    {
        vector<IDataX*>& vecTemp = m_vecDataXArrayItems[nPos];
        if (nIndex >= 0 && nIndex < (int)vecTemp.size())
        {
            *ppDataCmd = vecTemp[nIndex]->Clone();
        }
        else
        {
            return false;
        }
    }
    else
    {
        //TODO: add log (index out of range)
        return false;
    }

    return true;
}

bool XLDataX::GetUTF8String(short nKeyID, byte * pbDataBuf, int & nBufferLen)
{
    map<unsigned short, int>::iterator it = m_mapIndexes.find(nKeyID);
    if (it == m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    int nIndex = it->second;
    int nType = GET_TYPE_FROM_DX_INDEX(nIndex);
    if (!(nType == DX_TYPE_UTF8BYTES))
    {
        // TODO: add log (type mismatch)
        return false;
    }
    int nPos = GET_POS_FROM_DX_INDEX(nIndex);
    if (nPos >= 0 && nPos < (int)m_vecUTF8BytesItems.size())
    {
        string& strData = m_vecUTF8BytesItems[nPos];
        if (pbDataBuf == NULL)
        {
            nBufferLen = (int)strData.length();
        }
        else
        {
            if (nBufferLen < (int)strData.length())
            {
                //TODO: add log (buffer not enough)
                return false;
            }
            memcpy(pbDataBuf, strData.c_str(), strData.length());
            nBufferLen = (int)strData.length();
        }
    }
    else
    {
        //TODO: add log (index out of range)
        return false;
    }

    return true;
}

bool XLDataX::GetFloat(unsigned short nKeyID, float & fData)
{
    fData = 0;
    map<unsigned short, int>::iterator it = m_mapIndexes.find(nKeyID);
    if (it == m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    int nIndex = it->second;
    int nType = GET_TYPE_FROM_DX_INDEX(nIndex);
    if (!nType == DX_TYPE_FLOAT)
    {
        // TODO: add log (type mismatch)
        return false;
    }
    int nPos = GET_POS_FROM_DX_INDEX(nIndex);
    if (nPos >= 0 && nPos < (int)m_vecFloatItems.size())
    {
        fData = m_vecFloatItems[nPos];
    }
    else
    {
        //TODO: add log (index out of range)
        return false;
    }

    return true;
}

// 获取64整型数组的元素数量
bool XLDataX::GetInt64ArraySize(unsigned short nKeyID, int& nSize)
{
    nSize = 0;

    map<unsigned short, int>::iterator it = m_mapIndexes.find(nKeyID);
    if (it == m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    int nIndex = it->second;
    int nType = GET_TYPE_FROM_DX_INDEX(nIndex);
    if (!(nType == DX_TYPE_INT64ARRAY))
    {
        // TODO: add log (type mismatch)
        return false;
    }
    int nPos = GET_POS_FROM_DX_INDEX(nIndex);
    if (nPos >= 0 && nPos < (int)m_vecInt64ArrayItems.size())
    {
        vector<__int64>& vecTemp = m_vecInt64ArrayItems[nPos];
        nSize = (int)vecTemp.size();
    }
    else
    {
        //TODO: add log (index out of range)
        return false;
    }

    return true;
}

// 获取64整型数组的某个元素（根据索引编号）
bool XLDataX::GetInt64ArrayElement(unsigned short nKeyID, int nIndex, __int64& nData)
{
    nData = 0;

    map<unsigned short, int>::iterator it = m_mapIndexes.find(nKeyID);
    if (it == m_mapIndexes.end())
    {
        // TODO: add log
        return false;
    }

    int nMyIndex = it->second;
    int nType = GET_TYPE_FROM_DX_INDEX(nMyIndex);
    if (!(nType == DX_TYPE_INT64ARRAY))
    {
        // TODO: add log (type mismatch)
        return false;
    }
    int nPos = GET_POS_FROM_DX_INDEX(nMyIndex);
    if (nPos >= 0 && nPos < (int)m_vecInt64ArrayItems.size())
    {
        vector<__int64>& vecTemp = m_vecInt64ArrayItems[nPos];
        if (nIndex >= 0 && nIndex < (int)vecTemp.size())
        {
            nData = vecTemp[nIndex];
        }
        else
        {
            return false;
        }
    }
    else
    {
        //TODO: add log (index out of range)
        return false;
    }

    return true;
}


void XLDataX::EncodeBytesItem(FixedBuffer& fixed_buffer, int nPos)
{
    string& str = m_vecBytesItems[nPos];
    fixed_buffer.put_string(str);
}

void XLDataX::EncodeWStringItem(FixedBuffer& fixed_buffer, int nPos)
{
    std::wstring& str = m_vecWstrItems[nPos];

    fixed_buffer.put_int((int)str.length());
    fixed_buffer.put_bytes((byte*)str.c_str(), (int)(str.length() * sizeof(WCHAR)));
}

void XLDataX::EncodeDataXItem(FixedBuffer& fixed_buffer, int nPos)
{
    IDataX* pDataX = m_vecDataXItems[nPos];
    if (pDataX != NULL)
    {
        int nBufferPos = fixed_buffer.position();
        char* pBuffer = fixed_buffer.get_realbuffer() + nBufferPos;
        unsigned nBuffer = fixed_buffer.remain_len();
        pDataX->Encode(pBuffer, nBuffer);

        fixed_buffer.skip(nBuffer);
        //int nBufferPos2 = fixed_buffer.position();
    }
}

void XLDataX::EncodeIntArrayItem(FixedBuffer& fixed_buffer, int nPos)
{
    vector<int>& vecTemp = m_vecIntArrayItems[nPos];
    fixed_buffer.put_int((int)vecTemp.size());

    for (vector<int>::iterator it = vecTemp.begin(); it != vecTemp.end(); it++)
    {
        fixed_buffer.put_int(*it);
    }
}

void XLDataX::EncodeDataXArrayItem(FixedBuffer& fixed_buffer, int nPos)
{
    vector<IDataX*>& vecTemp = m_vecDataXArrayItems[nPos];
    fixed_buffer.put_int((int)vecTemp.size());

    for (vector<IDataX*>::iterator it = vecTemp.begin(); it != vecTemp.end(); it++)
    {
        IDataX* pDataX = *it;

        char* pBuffer = fixed_buffer.get_realbuffer() + fixed_buffer.position();
        unsigned nBuffer = fixed_buffer.remain_len();
        pDataX->Encode(pBuffer, nBuffer);

        fixed_buffer.skip(nBuffer);
    }
}

void XLDataX::EncodeFloatItem(FixedBuffer & fixed_buffer, int nPos)
{
    byte* p = reinterpret_cast<byte*>(&(m_vecFloatItems[nPos]));
    for (unsigned i=0; i<sizeof(float); i++)
    {
        fixed_buffer.put_byte(*(p+i));
    }
}

void XLDataX::EncodeUTF8BytesItem(FixedBuffer & fixed_buffer, int nPos)
{
    string& str = m_vecUTF8BytesItems[nPos];
    fixed_buffer.put_string(str);
}

void XLDataX::EncodeInt64ArrayItem(FixedBuffer& fixed_buffer, int nPos)
{
    vector<__int64>& vecTemp = m_vecInt64ArrayItems[nPos];
    fixed_buffer.put_int((int)vecTemp.size());

    for (vector<__int64>::iterator it = vecTemp.begin(); it != vecTemp.end(); it++)
    {
        fixed_buffer.put_int64(*it);
    }
}

unsigned XLDataX::CalcIntArrayItemLen(int nPos)
{
    unsigned nLen = 0;

    vector<int>& vecTemp = m_vecIntArrayItems[nPos];
    nLen += sizeof(int); // # of elements

    nLen += (sizeof(int) * vecTemp.size());

    return nLen;
}

unsigned XLDataX::CalcDataXArrayItemLen( int nPos)
{
    vector<IDataX*>& vecTemp = m_vecDataXArrayItems[nPos];
    unsigned nLen = sizeof(int);

    for (vector<IDataX*>::iterator it = vecTemp.begin(); it != vecTemp.end(); it++)
    {
        IDataX* pDataX = *it;

        nLen += pDataX->EncodedLength();
    }

    return nLen;
}

BOOL XLDataX::PutBytesItem(unsigned short nKeyID, FixedBuffer& fixed_buffer)
{
    string str = fixed_buffer.get_string();
    return PutBytes(nKeyID, (byte*)str.c_str(), (int)str.length());
}

BOOL XLDataX::PutWStringItem(unsigned short nKeyID, FixedBuffer& fixed_buffer)
{
    int nStringLen = fixed_buffer.get_int();
    WCHAR* pwszBuffer = new WCHAR[nStringLen];
    fixed_buffer.get_bytes((byte*)pwszBuffer, nStringLen * sizeof(WCHAR));

    bool bRet = PutWString(nKeyID, pwszBuffer, nStringLen);
    delete []pwszBuffer;

    return bRet;
}

BOOL XLDataX::PutDataXItem(unsigned short nKeyID, FixedBuffer& fixed_buffer)
{
    char* pBuffer = fixed_buffer.get_realbuffer() + fixed_buffer.position();
    int nBufferLen = fixed_buffer.remain_len();

    IDataX* pDataX = XLDataX::DecodeFrom((byte*)pBuffer, nBufferLen);
    if (pDataX)
    {
        fixed_buffer.skip(nBufferLen);
        return PutDataX(nKeyID, pDataX, true);
    }
    else
        return -2;  // Decode failure!!
}

BOOL XLDataX::PutIntArrayItem(unsigned short nKeyID, FixedBuffer& fixed_buffer)
{
    int nElements = fixed_buffer.get_int();
    vector<int> vecTemp;
    vecTemp.reserve(nElements+1);
    for (int i = 1; i <= nElements; i++)
    {
        int nTemp = fixed_buffer.get_int();
        vecTemp.push_back(nTemp);
    }

    return PutIntArray(nKeyID, (&vecTemp[0]), nElements);
}

BOOL XLDataX::PutDataXArrayItem(unsigned short nKeyID, FixedBuffer& fixed_buffer)
{

    vector<IDataX*> vecTemp;
    int nElements = fixed_buffer.get_int();
    vecTemp.reserve(nElements+1);

    for (int i = 1; i <= nElements; i++)
    {
        char* pBuffer = fixed_buffer.get_realbuffer() + fixed_buffer.position();
        int nBuffer = fixed_buffer.remain_len();
        IDataX* pDataX = XLDataX::DecodeFrom((byte*)pBuffer, nBuffer);
        if (pDataX == NULL)
            return -2;
        fixed_buffer.skip(nBuffer);

        vecTemp.push_back(pDataX);
    }

    return PutDataXArray(nKeyID, (&vecTemp[0]), nElements, true);
}

BOOL XLDataX::PutUTF8BytesItem(unsigned short nKeyID, FixedBuffer & fixed_buffer)
{
    string str = fixed_buffer.get_string();
    return PutUTF8String(nKeyID, (byte*)str.c_str(), (int)str.length());
}

BOOL XLDataX::PutFloatItem(unsigned short nKeyID, FixedBuffer & fixed_buffer)
{
    if (fixed_buffer.remain_len() < (int)sizeof(float))
    {
        return false;
    }
    float *pVal = reinterpret_cast<float*>(fixed_buffer.get_realbuffer() + fixed_buffer.position());
    fixed_buffer.skip(sizeof(float));
    return PutFloat(nKeyID, *pVal);
}

BOOL XLDataX::PutInt64ArrayItem(unsigned short nKeyID, FixedBuffer& fixed_buffer)
{
    int nElements = fixed_buffer.get_int();
    vector<__int64> vecTemp;
    vecTemp.reserve(nElements+1);
    for (int i = 1; i <= nElements; i++)
    {
        int nTemp = fixed_buffer.get_int();
        vecTemp.push_back(nTemp);
    }

    return PutInt64Array(nKeyID, (&vecTemp[0]), nElements);
}

string XLDataX::ToString()
{
    stringstream sTmp;
    sTmp<<"{";
    for (map<unsigned short, int>::iterator it = m_mapIndexes.begin(); it != m_mapIndexes.end(); it++)
    {
        //unsigned short nKey = it->first;
        short nKey = (short)it->first;
        int nIndex = it->second;

        int nType = GET_TYPE_FROM_DX_INDEX(nIndex);
        int nPos = GET_POS_FROM_DX_INDEX(nIndex);

        switch (nType)
        {
        case DX_TYPE_SHORT:
        {
            sTmp<<"(K:"<<nKey<<",T:DX_TYPE_SHORT,V:"<<((short)m_vecIntItems[nPos])<<"),";
            break;
        }
        case DX_TYPE_INT:
        {
            sTmp<<"(K:"<<nKey<<",T:DX_TYPE_INT,V:"<<m_vecIntItems[nPos]<<"),";
            break;
        }
        case DX_TYPE_INT64:
        {
            sTmp<<"(K:"<<nKey<<",T:DX_TYPE_INT64,V:"<<m_vecInt64Items[nPos]<<"),";
            break;
        }
        case DX_TYPE_BYTES:
        {
            //sTmp<<"(K:"<<nKey<<",T:DX_TYPE_BYTES,V:"
            //<<Utility::buffer_to_str(m_vecBytesItems[nPos].c_str(), m_vecBytesItems[nPos].length())<<"),";
            break;
        }
        case DX_TYPE_WSTRING:
        {
            //sTmp<<"(K:"<<nKey<<",T:DX_TYPE_WSTRING,V:"
            //<<Utility::buffer_to_str((unsigned char*)(m_vecWstrItems[nPos].c_str()), m_vecWstrItems[nPos].length()*sizeof(WCHAR))<<"),";
            break;
        }
        case DX_TYPE_DATAX:
        {
            sTmp<<"(K:"<<nKey<<",T:DX_TYPE_DATAX,V:"<<(m_vecDataXItems[nPos]->ToString())<<"),";
            break;
        }
        case DX_TYPE_INTARRAY:
        {
            sTmp<<"(K:"<<nKey<<",T:DX_TYPE_INTARRAY,V:[";
            vector<int>& vTmpInt = m_vecIntArrayItems[nPos];
            ostream_iterator<int, char> it_o(sTmp, ",");
            copy(vTmpInt.begin(), vTmpInt.end(), it_o);
            sTmp<<"]),";
            break;
        }
        case DX_TYPE_DATAXARRAY:
        {
            sTmp<<"(K:"<<nKey<<",T:DX_TYPE_DATAXARRAY,V:[";
            vector<IDataX*>& vTmpDataX =  m_vecDataXArrayItems[nPos];
            for (vector<IDataX*>::iterator it=vTmpDataX.begin();
                    it != vTmpDataX.end(); it++)
            {
                sTmp<<(*it)->ToString()<<",";
            }
            sTmp<<"])";
            break;
        }
        case DX_TYPE_FLOAT:
        {
            sTmp<<"(K:"<<nKey<<",T:DX_TYPE_FLOAT,V:"<<m_vecFloatItems[nPos]<<"),";
            break;
        }
        case DX_TYPE_UTF8BYTES:
        {
           // sTmp<<"(K:"<<nKey<<",T:DX_TYPE_UTF8BYTES,V:"
           // <<Utility::buffer_to_str(m_vecUTF8BytesItems[nPos].c_str(), m_vecUTF8BytesItems[nPos].length())<<"),";
            break;
        }
        default:
        {
           
            break;
        }
        }
    }
    sTmp<<"}";
    return sTmp.str();
}


