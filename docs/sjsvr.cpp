// sjsvr.cpp : ���� DLL Ӧ�ó������ڵ㡣
//
#include <assert.h>
#include <new>
#include <string.h>
#include "sjsvr.h"
#include "DdzLogic.h"
#include "igame.h"
#include "player.h"
#include "sjsvr.h"
#include "DDZ_SC_DEF.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>


#define LOG_DLLPLAYERINFO_ERROR(pPlayerInfo,strFunName) {if(pPlayerInfo == NULL) {m_pITable->WriteLog("[ERROR] pPlayerInfo == NULL!! function=%s",##strFunName);}}

void CDdzServer::Create( ITable* pTable ,int iInitMode, const char *szCfgFile)
{
	m_objPlayerMng.Create();
	//m_objPlayerMng.set_itable(pTable);
	m_pITable = pTable;
	m_pITable->WriteLog( "DDZSvr created, this=0x%x", this );
	m_objPlayerMng.Initialize(pTable);

	m_logic.Create();
	m_Setting.bEnableBombDouble = 1;
	m_Setting.ucSetOfCards = 1;
	m_Setting.nRule =	CARD_TYPE_SINGLE |
		CARD_TYPE_CONTINUE_SINGLE |
		CARD_TYPE_2_TUPLE |
		CARD_TYPE_CONTINUE_2_TUPLE |
		CARD_TYPE_3_TUPLE |
		CARD_TYPE_CONTINUE_3_TUPLE |
		CARD_TYPE_3_TUPLE_PLUS_SINGLE |
		CARD_TYPE_3_TUPLE_PLUS_2_TUPLE |
		CARD_TYPE_CONTINUE_3_TUPLE_PLUS_SINGLE |
		CARD_TYPE_CONTINUE_3_TUPLE_PLUS_2_TUPLE |
		CARD_TYPE_4_TUPLE_PLUS_2_SINGLE |
		CARD_TYPE_4_TUPLE_PLUS_2_2_TUPLE |
		CARD_TYPE_4_BOMB |
		CARD_TYPE_2_JOKER;
	
	m_RoomSetting.nShareRate=10;
	m_RoomSetting.nMaxGameChip=-100;
	m_RoomSetting.nIsTestRoom=1;
    m_RoomSetting.nScoreMulti = 2;
    m_RoomSetting.nScoreRadix = 50; //TODO read the value from configure file

	int room_id = m_pITable->GetRoomID();
	pTable->WriteLog("so get room id is %d", room_id);
	switch(room_id)
	{
		case 1:
		{
			m_RoomSetting.nScoreRadix = 50;
			break;
		}
		case 2:
		{
			m_RoomSetting.nScoreRadix = 100;
			break;
		}
		case 3:
		{
			m_RoomSetting.nScoreRadix = 200;
			break;
		}
		case 4:
		{
			m_RoomSetting.nScoreRadix = 500;
			break;
		}
		default:
            m_RoomSetting.nScoreRadix = 50; 
			break;
	}

	m_pITable->WriteLog( "Score Radix %d", m_RoomSetting.nScoreRadix );
	initTOBeforeGame();
}

void CDdzServer::Restore( ITable* pTable ,int iInitMode, const char *szCfgFile)
{
	//����ⲿ�ڴ�������¸�ֵ
	m_objPlayerMng.set_itable(pTable);
	m_pITable = pTable;
	pTable->WriteLog("succ restore in CCddServer");
}

CDdzServer::CDdzServer()
{
	//m_pITable->WriteLog( "DDZ server created" );
	m_new_game = true;
}


int CDdzServer::OnGameMessage(__uint64 nPlayerID, int nChair, void * pcGameData, short nDataLen)
//֪ͨͬ�������,��DLL�����ITable��֪ͨ�������Ϸ�����ͻص�SetGameScore��SetGameEnd
{
	m_pITable->WriteLog("DDZSvr::OnGameMessage--playerId=" LLU_FMT ", chair=%d len=%d", nPlayerID, nChair,nDataLen );  

	if (m_gsGameStatus == gsNoStart) 
	{
		m_pITable->WriteLog("DDZSvr::OnGameMessage--game not start");	
		return 0;
	}

	char* pMsg = (char*)pcGameData;

    m_pITable->WriteLog("DDZSvr::OnGameMessage pcGameData[0]=%d pcGameData[0]=%d pcGameData[0]=%d pcGameData[0]=%d pcGameData[0]=%d pcGameData[0]=%d ", pMsg[0], pMsg[1], pMsg[2], pMsg[3], pMsg[4]);

	char cmd = pMsg[0];
	m_pITable->WriteLog( "���յ��ͻ��˵�������: %d", (int)cmd );
	
	switch(cmd)
	{
		case CS_ORDER:
			return do_Order( nPlayerID, nChair, pMsg );
			//break;

		case CS_GIVE_CARDS:
			return do_GiveCard( nPlayerID, nChair, pcGameData );
			//break;
        case CS_NOTIFY_TRUST:
            {
                m_pITable->WriteLog("DDZSvr::OnGameMessage SC_NOTIFY_TRUST playerId=" LLU_FMT ", chair=%d", nPlayerID, nChair );

				char pData[8] = {'0'};
				pData[0] = nChair;
				pData[1] = pMsg[1];
                m_cTrust[nChair] = pData[1];

                SendData( nPlayerID,SC_NOTIFY_TRUST,nChair,pData, 2);
                return 0;
            }
		default:
			m_pITable->WriteLog("DDZSvr::OnGameMessage--unknown command = %d\n",cmd);
			return 0;
	}
}

// ��ʼ��������Ϸ��һЩ���� ״̬��ֵ��Ҫ�޸�
void CDdzServer::initTOBeforeGame()
{
 	m_pITable->WriteLog( "DDZSvr::initTOBeforeGame--" );
	m_gsGameStatus = gsNoStart;
	m_BeneathCards.ReleaseAll();
	m_CurrentOrderValue	= 0;
	m_nMulity = 0;
	m_orderRound = 0;
    m_curPassNum = 0;
    m_WhoIsBanker = 10;
	m_FirstBanker = 10;
	m_WhoIsFisrtToOrder = -1;
	m_WhoIsOnTurn = -1;

	for( int i = 0 ; i < 3 ; i ++ )
	{
		m_CurrentGiveCards[i].ReleaseAll();
		m_PlayerCards[i].ReleaseAll();
		m_PlayerGivedCount[i] = 0;
		m_PlayerGivedTime[i] = 0;
	}

    for(int i = 0;i<3;i++)
    {
        PlayerPass[i] = PASS_INIT;
    }

	//srand (( unsigned ) time ( NULL ));
	//m_WhoIsFisrtToOrder = rand()%3;
}



//�û�������Ϸ
int CDdzServer::OnGameUserEnter(TABLE_USER_INFO *pUser)
{
	//m_pITable->WriteLog("DDZSvr::OnGameUserEnter, userID=" LLU_FMT, pUser->m_iUserid);
	////ֻ���û����벻��DLL����Ϣ�������������DLL����TABLE��Ϣ����Ϊ�û������ʱ����Ҫ�õ������û����б�
	//int nRet = m_objPlayerMng.AddPlayer(pUser);
	//return nRet;

    m_pITable->WriteLog("OnGameUserEnter() called, Chair=%d cState=%d, UserID="LLU_FMT,pUser->m_cChair,pUser->m_cState, pUser->m_iUserid);

    // �ж��Ƿ�Ϊ�Թ��� ��������Ϸ����
    if (sLookOn == pUser->m_cState)
    {
        m_pITable->WriteLog("OnGameUserEnter() LookOnUser! cState=%d",pUser->m_cState);
        SendCurrentDataTo(pUser->m_iUserid,pUser->m_cChair,SC_WATCHER_MSG,true);
        return 0;
    }

    int nRet = m_objPlayerMng.AddPlayer(pUser);
    if( 0 != nRet)
    {
        m_pITable->WriteLog("OnGameUserEnter() AddPlayer Error! nRet=%d",nRet);
    }

    return nRet;
}

//�û��˳���Ϸ
int CDdzServer::OnGameUserExit(__uint64 nPlayerID)
//DLL��ֻ�ص�SetGameScore���÷���,SetGameEnd֪ͨ�����Ϸ������ά���Լ����û��б�
{
    m_pITable->WriteLog("DDZSvr::OnGameUserExit--userid="  LLU_FMT, nPlayerID);

    DllPlayerInfo* pstPlayer=m_objPlayerMng.GetPlayerInfo(nPlayerID);
    if (pstPlayer==NULL ) 
    {
        m_pITable->WriteLog("the user:" LLU_FMT " is not in the table: %d",nPlayerID,m_pITable->GetTableID());
        return 0;
    }

    if (m_gsGameStatus==gsNoStart )
    {
        m_pITable->WriteLog("the user:" LLU_FMT " exit the table: %d status:%d",nPlayerID,m_pITable->GetTableID(),pstPlayer->cStatus);
    }
    else
    {
        //�����ʱ���˳���Ҫ�۷ֺ�֪ͨ������Ϸ,ͬʱ������Ϸ���������û���״̬
        m_pITable->WriteLog("the user:" LLU_FMT " exit the table: %d status:%d seatID=%d",nPlayerID,m_pITable->GetTableID(),pstPlayer->cStatus,pstPlayer->cSeatid);

        char sendBuf[32] = {0};
        int *ptr = (int*)sendBuf;
        if (m_CurrentOrderValue == 0)
        {
            ptr[sc_EscNum] = 1;
            ptr[sc_Points] = -3;
            m_pITable->SetGameScore(pstPlayer->iUserid,32,(void*)sendBuf);
        }
        else
        {
            ptr[sc_EscNum] = 1;
            ptr[sc_Points] = m_CurrentOrderValue * m_nMulity * (-3);
            m_pITable->SetGameScore(pstPlayer->iUserid,32,(void*)sendBuf);
        }

        m_pITable->WriteLog("{ddz} OnGameUserExit() 001");
        GameRestart();
        m_objPlayerMng.ResetStatus();
        m_pITable->SetGameEnd( ); 
    }

    m_objPlayerMng.DeletePlayer(nPlayerID);
    m_pITable->WriteLog("{ddz} OnGameUserExit() 002 ");

    //����ҷǶ��������˳���ʱ�����˳� ������Ϸ��Ϣ
    m_objPlayerMng.ResetGameInfo();

    m_WhoIsBanker = 10;
    // ����й��û����
    ResetTrust();

    return 0;
}

//�û�����
int CDdzServer::OnGameUserOffLine(__uint64 nPlayerID)
{
	m_pITable->WriteLog("DDZSvr::OnGameUserOffLine--userid=%I64u", nPlayerID);
    if(gsPlaying == m_gsGameStatus || gsOrdering == m_gsGameStatus)
    {
        DllPlayerInfo* pstPlayer=m_objPlayerMng.GetPlayerInfo(nPlayerID);
        if (pstPlayer == NULL)
        {
            m_pITable->WriteLog("[error] can't find userid=%llu",nPlayerID);
            return -1;
        }
		
		//��������Ϊ�й�״̬
		m_cTrust[pstPlayer->cSeatid] = TRUST_SYS_SET;
		char data[8] = {'\0'};
		data[0] = pstPlayer->cSeatid;
		data[1] = TRUST_SYS_SET;

		m_pITable->WriteLog("AutoGiveCard:userid=%I64u,seatid = %d, set trust %d", 
					nPlayerID, pstPlayer->cSeatid, TRUST_SYS_SET);

        SendDataExcept(nPlayerID,SC_NOTIFY_TRUST,pstPlayer->cSeatid, data, 2);
		
		//����ֵ�����ҳ��ƣ����̳���
		if(pstPlayer->cSeatid == m_WhoIsOnTurn) {
			AutoAction();
			m_pITable->WriteLog("user %d is on turn ,auto give card.", nPlayerID);
		}
    }

	return 0;
}

//�û��������½�����Ϸ
int CDdzServer::OnGameUserReplay(__uint64 nPlayerID)
{
	m_pITable->WriteLog("DDZSvr::OnGameUserReplay--userid=%llu", nPlayerID);
	//�����ݲ�ʵ��
	DllPlayerInfo* pstPlayer=m_objPlayerMng.GetPlayerInfo(nPlayerID);
	if (pstPlayer==NULL ) 
	{
		m_pITable->WriteLog("the user:" LLU_FMT " is not in the table: %d",nPlayerID,m_pITable->GetTableID());
		return 0;
	}
	
	//add by chenjian on 2014-5-12 11:23:37
	//����������ʱ������0
	m_timeOutTimes[pstPlayer->cSeatid] = 0;
	//end add

    if (sPlaying == pstPlayer->cStatus)
    {
        m_pITable->WriteLog("OnGameUserReplay--userid=%llu is reconnected to so!!", nPlayerID);

        //SendDataExcept(nPlayerID,SC_NOTIFY_REPLAY,pstPlayer->cSeatid);

        SendCurrentDataTo(nPlayerID,pstPlayer->cSeatid,SC_REPLAY_MSG,false);

        return 0;
    }

    m_pITable->WriteLog("OnGameUserReplay--userid="LLU_FMT" is not reconnected to so!! status=%d", nPlayerID,pstPlayer->cStatus);

	return 0;
}

//�û�����ʼ
int CDdzServer::OnGameUserReady(__uint64 nPlayerID)
{
    m_pITable->WriteLog("DDZSvr::OnGameUserReady--userid=" LLU_FMT, nPlayerID);
    DllPlayerInfo* pPlayer = m_objPlayerMng.GetPlayerInfo(nPlayerID);
    if (pPlayer==NULL ) 
    {
        m_pITable->WriteLog("DDZSvr::OnGameUserReady--the user:" LLU_FMT " is not in the table: %d",nPlayerID,m_pITable->GetTableID());
        return 0;
    }

    pPlayer->cStatus=sReady;
    if (m_objPlayerMng.JudgeStart()<0 )
    {
        m_pITable->WriteLog("DDZSvr::OnGameUserReady--SendDataExcept(notify user ready(105):" LLU_FMT ",%s, chair:%d)", 
            pPlayer->iUserid, pPlayer->m_szNick, pPlayer->cSeatid );
        return 0;
    }

    m_pITable->WriteLog( "DDZSvr::OnGameUserReady--game start" );

    //m_gsGameStatus=gsOrdering;

    int iData=0;
   // SendData(nPlayerID,CMD_DLL_NOTIFY_GAME_BEGIN,iData);
    m_pITable->SetGameStart();

    initTOBeforeGame();
    ProcessDealCards(nPlayerID);//����

    return 0;
}

//��Ϸ��ʼ
int CDdzServer::OnGameBegin()
{
	m_pITable->WriteLog("DDZSvr::OnGameBegin--");
	//�����ݲ�ʵ��
	return 0;
}

//������������ǿ�ƽ�����Ϸ
int CDdzServer::OnGameSetGameEnd()
//��ʱ�ò������ڷ������޷�Ǩ��ʱ�õ���
{
	m_pITable->WriteLog("DDZSvr::OnGameSetGameEnd--");
	//�����ݲ�ʵ��
	return 0;
}

int CDdzServer::OnGameUserDel(__uint64 nPlayerID)
{
    m_pITable->WriteLog("{ddz} OnGameUserDel nPlayerID=%llu",nPlayerID);

	OnGameUserExit(nPlayerID);
	return 0;
}

int CDdzServer::OnGameTimeout(int nTimerID)
{
	int i = m_WhoIsOnTurn;
	AutoAction();	

	//add by chenjian on 2014-5-12 11:24:20
	//��ҷ��йܣ���ʱ2�ν���ϵͳ�й�
	if(TRUST_SYS_SET != m_cTrust[i])	
	{
		++m_timeOutTimes[i];
		if(m_timeOutTimes[i] >= 2)//���2�γ�ʱ�Զ���Ϊϵͳ�й�
		{
			m_cTrust[i] = TRUST_SYS_SET;
			m_timeOutTimes[i] = 0;
			
			DllPlayerInfo* pPlayer = m_objPlayerMng.GetPlayerInfoBySeat(i);
			if (pPlayer == NULL)
			{
				m_pITable->WriteLog("OnGameTimeout pPlayer == NULL chairID=%d",i);
				return 0;
			}

			char pData[8] = {'0'};
			pData[0] = i;
			pData[1] = TRUST_SYS_SET;

            SendData(pPlayer->iUserid, SC_NOTIFY_TRUST,i,pData, 2);
			m_pITable->WriteLog("OnGameTimeout: table %d timeout is more then 2 times, trust!",i);
		}
	}
	else
	{
		m_pITable->WriteLog("OnGameTimeout: table %d TRUST_SYS_SET is also timeout, wrong!",i);
	}
	//end add
	
	return 0;
}

//��Ϸ��ʱ��������û���ʱʱ��ϵͳ����
int CDdzServer::OnGameTimer ()
{
	m_pITable->WriteLog("DDZSvr::OnGameTimer--");
	
	time_t local;
	time(&local);
	//����Ҫ���û���ʱ���д���
	//switch(bjObject.game_status) {
	//case sDeal_ing:
	//	if (m_iDealCardTimer>0 && bjObject.m_iBeneathedFlag ==0 ) {
	//		if (local-m_iDealCardTimer>bjObject.game_timer) {
	//			m_iDealCardTimer=0;
	//			bjObject.ProcessShowBeneathCard();
	//		}
	//	}		
	//	break;
	//}

	return 0;
}

int CDdzServer::SendData(__uint64 nPlayerID,int cDllCmd,char cChairid,char* szBuf ,short nLen)
{
	m_pITable->WriteLog( "DDZSvr::SendData--cmd=%d, data_len=%d", cDllCmd, nLen );

	char szMsg[500];
	szMsg[0]=cDllCmd;
	memcpy(szMsg+1,szBuf,nLen);
	nLen+=1;

	return m_pITable->SendGameData(szMsg,nLen,nPlayerID);
}

int CDdzServer::SendData(__uint64 nPlayerID, int cDllCmd,int iData)//�������û�������Ϣ
{
	m_pITable->WriteLog( "DDZSvr::SendData--cmd=%d, iData=%d", cDllCmd, iData );
	char pMsg[sizeof(int)+1];
	pMsg[0]=cDllCmd;	
	int iDataTemp=htonl(iData);
	memcpy(pMsg+1,&iDataTemp,sizeof(int));
	short nLen=sizeof(int)+1;
	m_pITable->SendGameDataToLookOnUsers(-1, (char*)pMsg, nLen, nPlayerID);
	return m_pITable->SendGameData((char*)pMsg,nLen,nPlayerID);
}
//��һ���û�������Ϣ
int CDdzServer::SendDataTo(__uint64 nPlayerID,__uint64 iUserid,int cDllCmd,char cChairid,char* szBuf ,short nLen)
{
	char szMsg[500];
	szMsg[0]=cDllCmd;
	memcpy(szMsg+1,szBuf,nLen);
	nLen+=1;
	m_pITable->SendGameDataToLookOnUsers(cChairid, (char*)szMsg, nLen, nPlayerID);
	return m_pITable->SendGameDataTo(iUserid,(char*)szMsg,nLen,nPlayerID);
}
int CDdzServer::SendDataTo(__uint64 nPlayerID,__uint64 iUserid,int cDllCmd,int iData)
{
	m_pITable->WriteLog( "DDZSvr::SendDataTo--userid=" LLU_FMT ", cmd=%d, data=%d", iUserid, cDllCmd, iData );
	char pMsg[sizeof(int)+1];
	pMsg[0]=cDllCmd;	
	int iDataTemp=htonl(iData);
	memcpy(pMsg+1,&iDataTemp,sizeof(int));
	short nLen=sizeof(int)+1;
	DllPlayerInfo* pPlayer = m_objPlayerMng.GetPlayerInfo(nPlayerID);
	if (pPlayer==NULL ) 
	{
		m_pITable->WriteLog("DDZSvr::OnGameUserReady--the user:" LLU_FMT " is not in the table: %d",nPlayerID,m_pITable->GetTableID());
		return 0;
	}
	m_pITable->SendGameDataToLookOnUsers(pPlayer->cSeatid, (char*)pMsg, nLen, nPlayerID);
	return m_pITable->SendGameDataTo(iUserid,(char*)pMsg,nLen,nPlayerID);
}
//���͸����Լ���������ˣ��磺׼����
int CDdzServer::SendDataExcept(__uint64 iUserid,int cDllCmd,char cChairid,char* szBuf ,short nLen)
{
	char szMsg[500];
	szMsg[0]=cDllCmd;
	memcpy(szMsg+1,szBuf,nLen);
	nLen+=1;
	//m_pITable->SendGameDataToLookOnUsers(-1, (char*)szMsg, nLen, iUserid);
	return m_pITable->SendGameDataExcept(iUserid,(char*)szMsg,nLen,iUserid);
}
int CDdzServer::SendDataExcept(__uint64 iUserid,int cDllCmd,int iData)
{
	m_pITable->WriteLog( "DDZSvr::SendDataExcept--userid=" LLU_FMT ", cmd=%d, data=%d", iUserid, cDllCmd, iData );
	char pMsg[sizeof(int)+1];
	pMsg[0]=cDllCmd;	
	int iDataTemp=htonl(iData);
	memcpy(pMsg+1,&iDataTemp,sizeof(int));
	short nLen=sizeof(int)+1;
	//m_pITable->SendGameDataToLookOnUsers(-1, (char*)pMsg, nLen, iUserid);
	return m_pITable->SendGameDataExcept(iUserid,(char*)pMsg,nLen,iUserid);
}

int CDdzServer::GameRestart()
{
    m_gsGameStatus = gsNoStart;
	m_pITable->WriteLog("DDZSvr::GameRestart--");
	//initTOBeforeGame();  //*** ������Ҫ?
	//�߼��ϵ�RestarҲҪ����
	m_objPlayerMng.GameRestart();
	return 0;
}

// �����߼�
int CDdzServer::do_Order( __uint64 nPlayerID, int nChair, char* pMsg )
{
	int score_multiple = m_RoomSetting.nScoreMulti;

	//����˳����
    if( nChair != m_WhoIsOnTurn )
    {
        m_pITable->WriteLog( "[ERROR] DDZSvr::do_Order--nChair != WhoIsOnTurn , Chair=%d playerID=" LLU_FMT, nChair, nPlayerID );
        return 0;
    }
	
	//�ƾ�״̬���
    if( gsOrdering != m_gsGameStatus )
    {
        m_pITable->WriteLog("[ERROR] DDZSvr::do_Order--gsOrdering != m_gsGameStatus , Chair=%d playerID=" LLU_FMT, nChair, nPlayerID );
        return 0;
    }

    ClearBuf();

    msgBuf[3] = nChair;

    //here the right time & person
    m_pITable->WriteLog("DDZSvr::do_Order--order value=%d, m_CurrentOrderValue=%d", pMsg[1], m_CurrentOrderValue );
    if ( CARD_ORDER_PASS== pMsg[1] ) //means pass
    { 
        m_cOrderPoint[nChair] = CARD_ORDER_PASS;
        msgBuf[4] = m_CurrentOrderValue;
		m_pITable->WriteLog("DDZSvr::do_Order--PASS: CurrentOrderValue = %d, Order =%d\n, playerID=" LLU_FMT " Chair=%d\n" ,
							m_CurrentOrderValue, pMsg[1], nPlayerID, nChair);
    }
    else if ( m_CurrentOrderValue == 0 && pMsg[1] == CARD_ORDER_CALL)
    {//m_CurrentOrderValue=0��ʾ��û���˽е����� ���3��(ֻ����3��)��ʾ�е���
    
        m_cOrderPoint[nChair] = CARD_ORDER_CALL;

        msgBuf[4] = pMsg[1];
        assert( pMsg[1] > 0 );

        m_WhoIsBanker = nChair;
		m_FirstBanker = nChair;
        m_CurrentOrderValue = pMsg[1];
		m_pITable->WriteLog("DDZSvr::do_Order--CALL: CurrentOrderValue = %d, Order =%d\n, playerID=" LLU_FMT " Chair=%d\n" , 
							m_CurrentOrderValue, pMsg[1], nPlayerID, nChair);
    }
	else if(m_CurrentOrderValue >= CARD_ORDER_CALL && pMsg[1] == CARD_ORDER_GRAB)
	{//�Ѿ����˽е����ˣ��͸���������
	
        m_cOrderPoint[nChair] = CARD_ORDER_GRAB;
		m_CurrentOrderValue *= score_multiple;
		m_WhoIsBanker = nChair;			//˭��/��������˭�ͱ��Ϊ����
		msgBuf[4] = m_CurrentOrderValue;

		m_pITable->WriteLog("DDZSvr::do_Order--GRAB: CurrentOrderValue = %d, Order =%d\n, playerID=" LLU_FMT " Chair=%d\n" , 
							m_CurrentOrderValue, pMsg[1], nPlayerID, nChair);
	}
    else
    { // order action illegal
        m_pITable->WriteLog("DDZSvr::do_Order--OrderValue Wrong: CurrentOrderValue = %d, Order =%d\n, playerID=" LLU_FMT " Chair=%d\n" , 
            m_CurrentOrderValue , pMsg[1], nPlayerID, nChair);
        return 0;
    }

	//δ��ʱ����ɾ����ʱ��, ������ʹ�����Ϣ����ʱ����ȡ���������Ը���ҳ�ʱ
	m_pITable->UnRegisterTimer(m_WhoIsOnTurn);

	//m_orderRound > 0��ʾ�Ѿ���ѯ��һȦ
    char nNext = GetNextOnTurn();
    if (nNext == m_WhoIsFisrtToOrder)
    {
    	++m_orderRound;
    	//��ѯ��һȦ������==3�֣�˵��2�˷�����һ���е���
    	//ֱ�ӿ�ʼ��Ϸ
    	if(m_CurrentOrderValue == CARD_ORDER_CALL)
		{
			msgBuf[4] = m_CurrentOrderValue;
	        msgBuf[5] = 0;//���ʱ��ʼ��Ϸ���ǲ��ܽеڶ�Ȧ��
	        msgBuf[6] =  nChair;
		}//��ѯ��һȦ����<3��˵��û���˽е��������·���
		else if(m_CurrentOrderValue < CARD_ORDER_CALL)
		{
			m_pITable->WriteLog("DDZSvr::do_Order--OrderValue ReSend cards!!");
            initTOBeforeGame();
	        ProcessDealCards(nPlayerID);

			return 0;
		}
		else	//����>3˵�����е������������˵�����������Ҫ����������
		{
	        msgBuf[5] = 1;//���ʱ���������
	        //�ҳ���һ�����ʸ����������ˣ���һ��pass��û���ʸ���
	        m_WhoIsOnTurn = nNext;
	        if(m_WhoIsFisrtToOrder != m_FirstBanker)
			{
				//���˶������������һ����û�не�����
				//��ô�ڷ���>3������µ�һ���е�����һ���ǵڶ�����
				nNext = GetNextOnTurn();
				m_WhoIsOnTurn = nNext;
			}

			msgBuf[6] = nNext;
		}
    }
	else
	{
		//����Ѿ���ѯ��һȦ�����ҵ�ǰΪ��һ���е���������������������������ʼ��Ϸ
		if(nChair == m_FirstBanker && m_orderRound > 0)
		{
			//���Ϊ�����������һ���е��������Ϊ����
			if(pMsg[1] == CARD_ORDER_GRAB)
			{
				m_WhoIsBanker = nChair;
				msgBuf[4] = m_CurrentOrderValue;
		        msgBuf[6] =  nChair;
			}
			//else ��һ���е�����pass�����Ե���Ϊ��һ������������

			msgBuf[5] = 0;//��ʼ��Ϸ���
		}
		else //��һȦ
		{
			msgBuf[4] = m_CurrentOrderValue;
			msgBuf[5] = 1;	//����������
			msgBuf[6] = nNext;
			m_WhoIsOnTurn = nNext;
		}
	}

	if(1 == msgBuf[5]) //����������
	{
		m_pITable->WriteLog("DDZSvr::do_Order--OrderValue Continue call socre!!");

        for (int i=0;i<3;i++)
        {
            m_cmdMsgBuffer[i].cmd =SC_ORDER;
            m_cmdMsgBuffer[i].nPlayerID = nPlayerID;
            m_cmdMsgBuffer[i].len = 7;
            memcpy(m_cmdMsgBuffer[i].msgBuf,msgBuf,7);
        }

        SendData(nPlayerID,SC_ORDER,0,msgBuf,7);
	}
	else if(0 == msgBuf[5])//�·����ƣ���ʼ��Ϸ
	{
		m_WhoIsOnTurn = m_WhoIsBanker;
		msgBuf[2] = SC_ORDER;
		msgBuf[4] = m_CurrentOrderValue;
		msgBuf[6] = m_WhoIsBanker;
		SendData(nPlayerID,SC_ORDER,0,msgBuf,7);

        /*  2	   3			4					  5             6 --			*/
        /*  MsgID  WhoIsBanker	NumberOfBeneathCards  score         BeneathCards	*/

        ClearBuf();

        msgBuf[2] = SC_SHOW_BENEATH;
        msgBuf[3] = m_WhoIsBanker;
        msgBuf[4] = 3; // for 1 set ddz only
        msgBuf[5] = m_CurrentOrderValue;

        //�·�����
        for( int i = 0 ; i < 3 ; i ++ )
        {
            msgBuf[6 + i] =  m_logic.Set_1[51 + i] ;
        }


        m_PlayerCards[m_WhoIsBanker].AddCards( m_BeneathCards );


        DllPlayerInfo* pPlayer = m_objPlayerMng.GetPlayerInfoBySeat(m_WhoIsBanker);

        if (pPlayer == NULL)
        {
            m_pITable->WriteLog("do_Order pPlayer == NULL chairID=%d",m_WhoIsBanker);
            return 0;
        }

        m_pITable->WriteLog( "DDZSvr::do_Order--�����Ƹ�" LLU_FMT ", %s, chair %d", 
            pPlayer->iUserid, pPlayer->m_szNick, m_WhoIsBanker );
        m_pITable->WriteLog( "DDZSvr::do_Order--SendData(SHOW BENEATH), %d,%d,%d,%d,%d,%d,%d,%d %d",
            msgBuf[0], msgBuf[1], msgBuf[2], msgBuf[3], msgBuf[4], msgBuf[5], msgBuf[6], msgBuf[7],msgBuf[8]);

        for (int i=0;i<3;i++)
        {
            m_cmdMsgBuffer[i].cmd = SC_SHOW_BENEATH;
            m_cmdMsgBuffer[i].nPlayerID = nPlayerID;
            m_cmdMsgBuffer[i].len = 9;
            memcpy(m_cmdMsgBuffer[i].msgBuf,msgBuf,9);
        }

        SendData(nPlayerID,SC_SHOW_BENEATH, 0, msgBuf, 9);

        m_gsGameStatus = gsPlaying;
	}

	if(TRUST_SYS_SET == m_cTrust[m_WhoIsOnTurn])
	{
		AutoAction();	
	}
	else
		m_pITable->RegisterTimer(m_WhoIsOnTurn, TIME_OUT_ORDER, false);

	return 0;
}

// �߼��д���
int CDdzServer::do_GiveCard( __uint64 nPlayerID, int nChair, void* pcGameData )
{
	if(m_new_game)
	{
		m_new_game = false;	
	}
	char* pTemp=(char*)pcGameData+1;
	/* 2	  3				4		   5				  6					 7--	60*/
	/* MsgID  bSetNextGive  WhoIsNext  WhoGiveTheseCards  NumberOfGiveCards	 Cards	nMulti*/
	ClearBuf();

	DllPlayerInfo* pPlayer = m_objPlayerMng.GetPlayerInfo( nPlayerID );
    if (pPlayer==NULL ) 
    {
        m_pITable->WriteLog("[error]DDZSvr::do_GiveCard Can't find the user:" LLU_FMT " Chair:%d",nPlayerID,nChair);
        return 0;
    }

    m_pITable->WriteLog( "DDZSvr::do_GiveCard--Give Cards ID: " LLU_FMT ", %s, Chair:%d m_WhoIsOnTurn:%d\n", nPlayerID, pPlayer->m_szNick, nChair,m_WhoIsOnTurn);

	if( gsPlaying != m_gsGameStatus )
	{
		ERRLOG( "[error]DDZSvr::do_GiveCard--Give Cards Wrong: Game no Start  ID:" LLU_FMT " Chair:%d WhoIsOnTurn:%d\n", nPlayerID, nChair,m_WhoIsOnTurn);
		return 0;
	}

	if( nChair != m_WhoIsOnTurn )
	{
        ERRLOG( "[error]DDZSvr::do_GiveCard--Give Cards Wrong: no OnTurn ID:" LLU_FMT " Chair:%d WhoIsOnTurn:%d\n", nPlayerID, nChair,m_WhoIsOnTurn);
		return 0;
	}

    m_pITable->WriteLog("do_GiveCard left[1] cards number chairId(0)=%d chairId(1)=%d chairId(2)=%d give cards Lenth=%d Chair=%d\n", m_PlayerCards[0].CurrentLength, m_PlayerCards[1].CurrentLength, m_PlayerCards[2].CurrentLength,pTemp[3],nChair);

    NEW_VARIABLE( DDZCards , LastGiveCards );
    GetLastGiveCards( LastGiveCards );

	BOOL bGiveCardOk = TRUE;
	if( 0 != pTemp[3] )
	{
		//				m_pITable->WriteLog( "		Cards Length = %d\n" , pTemp[3] );
		NEW_VARIABLE( DDZCards , GiveCards );
		CARD tempCard;

		int i;
		char play_flow_buf[1024] = {0};
		
		m_pITable->WriteLog( "give card num=%d", pTemp[3] );
		
		sprintf(play_flow_buf, "chair(%d)(%d:", nChair, pTemp[3]);
		m_play_flow += play_flow_buf;
		
		for( i = 0 ; i < pTemp[3] ; i ++ )
		{
			tempCard =  CDdzLogic::char2CARD( pTemp[4 + i] );

			if( 0 == tempCard.number && 0 == tempCard.shape )
			{
				bGiveCardOk = FALSE;
				m_pITable->WriteLog("[error] bad card: %d, %d", tempCard.shape, tempCard.number );
				break;
			}

			m_pITable->WriteLog("GIVECARD: %d card is %d, sharp %d", i + 1, tempCard.number, tempCard.shape);
			GiveCards.AddCard( tempCard );

			sprintf(play_flow_buf, "%d,", tempCard.number);
			m_play_flow += play_flow_buf;
		}

		m_play_flow += ")|";

		if( bGiveCardOk )
		{
			bGiveCardOk = GiveCards.IsSubSet( m_PlayerCards[nChair] );
		}

		if( !bGiveCardOk )
		{
			m_pITable->WriteLog("[error]error: cards not the subset of the playercards");
            return 0;
		}
		else
		{
			//NEW_VARIABLE( DDZCards , LastGiveCards );
			//GetLastGiveCards( LastGiveCards );

			if( 0 == LastGiveCards.CurrentLength )
			{
				m_pITable->WriteLog( "First Player to Give" );
				bGiveCardOk = SetCardsType( GiveCards , m_Setting.nRule );
                m_pITable->WriteLog("0 == LastGiveCards.CurrentLength GiveCards type:%d bGiveCardOk=%d",GiveCards.nType,bGiveCardOk);
			}
			else
			{
				m_pITable->WriteLog( "No First Player to Give" );
				SetCardsType( LastGiveCards , m_Setting.nRule );
				SetCardsType( GiveCards , m_Setting.nRule );
				bGiveCardOk = IsFirstCardsBig( GiveCards , LastGiveCards );
                m_pITable->WriteLog("0 != LastGiveCards.CurrentLength GiveCards type:%d LastGiveCards type:%d bGiveCardOk=%d",GiveCards.nType,LastGiveCards.nType,bGiveCardOk);
			}

			if( bGiveCardOk )
			{//can send now, but check if the giving cards player have more cards
				m_pITable->UnRegisterTimer(m_WhoIsOnTurn);

				//ͳ��ÿ���˳�����������
				m_PlayerGivedCount[nChair] += pTemp[3];
				m_PlayerGivedTime[nChair]++;

				if( m_Setting.bEnableBombDouble )
				{
					if(GiveCards.nType == CARD_TYPE_2_JOKER)
					{
						m_nMulity *= 2;
					} 
					if( GiveCards.nType == CARD_TYPE_4_BOMB )
					{
						m_nMulity *= 2;
					}
				}

				m_CurrentGiveCards[nChair].ReleaseAll();
				m_CurrentGiveCards[nChair].AddCards( GiveCards );

				m_PlayerCards[nChair].DelCards( GiveCards );

				msgBuf[2] = SC_GIVE_CARD;

				m_WhoIsOnTurn = GetNextOnTurn();
				msgBuf[3] = 1;

                if (m_PlayerCards[nChair].CurrentLength == 0)
                {
                    msgBuf[4] = 10;
                }
                else
                {
				    msgBuf[4] = m_WhoIsOnTurn;
                }

                msgBuf[5] = nChair;
                msgBuf[8] = GiveCards.CurrentLength;
                msgBuf[1] = GiveCards.nType;//�·���������


                for( i = 0 ; i < GiveCards.CurrentLength ; i ++ )
                {
                    msgBuf[9 + i] = CDdzLogic::CARD2char( GiveCards.cards[i] );
                }

                msgBuf[6] = m_nMulity;

                m_curPassNum = 0;
                msgBuf[7] = 0;  // ��ͷ�ҳ���
                m_pITable->WriteLog( "[givecards] Not Must Give Cards! m_curPassNum=%d",m_curPassNum );

                m_pITable->WriteLog("nChair %d, next Chair %d", nChair,m_WhoIsOnTurn);

                PlayerPass[nChair] = PASS_FALSE;

                for (int i=0;i<3;i++)
                {
                    m_cmdMsgBuffer[i].cmd = SC_GIVE_CARD;
                    m_cmdMsgBuffer[i].nPlayerID = nPlayerID;
                    m_cmdMsgBuffer[i].len = 9 + GiveCards.CurrentLength;
                    memcpy(m_cmdMsgBuffer[i].msgBuf,msgBuf, 9 + GiveCards.CurrentLength);

                }

                //����9Ϊ�̶����ȣ�ÿһλ����ͬ�ĺ���
                SendData( nPlayerID, SC_GIVE_CARD, 0, msgBuf , 9 + GiveCards.CurrentLength);          
				
			}
            else
            {
                m_pITable->WriteLog("[error]give the type of the last given cards or current given cards is wrong!");
                return 0;
            }
		}
	}
	//if( 0 == pTemp[3] || !bGiveCardOk )
    else 
	{//means pass
		//��ֹ����������ѭ��
		if(0 == LastGiveCards.CurrentLength && m_curPassNum == 3)
		{
			return 0;
		}

		m_pITable->UnRegisterTimer(m_WhoIsOnTurn);
        m_curPassNum++;

		m_CurrentGiveCards[nChair].ReleaseAll();
		m_WhoIsOnTurn = GetNextOnTurn();

        if( m_curPassNum >= 2 )
        {
            msgBuf[7] = 1;  // ͷ�ҳ���
            m_pITable->WriteLog( "[pass] Must Give Cards! m_curPassNum=%d",m_curPassNum );
            m_curPassNum = 0;
        }
        else
        {
            msgBuf[7] = 0;  // ��ͷ�ҳ���
            m_pITable->WriteLog( "[pass] Not Must Give Cards! m_curPassNum=%d",m_curPassNum );
        }

		msgBuf[2] = SC_GIVE_CARD;
		msgBuf[3] = 1;
		msgBuf[4] = m_WhoIsOnTurn;
		msgBuf[5] = nChair;
		msgBuf[8] = 0;
		msgBuf[6] = m_nMulity;  //�ƾ��еķ�����

		m_pITable->WriteLog( "DDZSvr::do_GiveCard--SendData  pass" );
        m_pITable->WriteLog("nChair %d, next Chair %d", nChair,m_WhoIsOnTurn);

        PlayerPass[nChair] = PASS_TRUE;

        for (int i=0;i<3;i++)
        {
            m_cmdMsgBuffer[i].cmd = SC_GIVE_CARD;
            m_cmdMsgBuffer[i].nPlayerID = nPlayerID;
            m_cmdMsgBuffer[i].len = 9;  //�����ƣ����ݾ�Ϊ�̶�����
            memcpy(m_cmdMsgBuffer[i].msgBuf,msgBuf,9);
        }

		SendData(nPlayerID, SC_GIVE_CARD, 0, msgBuf ,9);////????
		
	}

    m_pITable->WriteLog("do_GiveCard[2] left cards number chairId(0)=%d chairId(1)=%d chairId(2)=%d give cards Lenth=%d Chair=%d\n", m_PlayerCards[0].CurrentLength, m_PlayerCards[1].CurrentLength, m_PlayerCards[2].CurrentLength,pTemp[3],nChair);
    // �غϽ���
    if( 0 == m_PlayerCards[nChair].CurrentLength )
    {
    	m_new_game = true;
				
        m_pITable->WriteLog( "do_GiveCard UserID=" LLU_FMT ", %s, Chair:%d Win!\n", nPlayerID, pPlayer->m_szNick, nChair);

        m_gsGameStatus = gsNoStart;

		{
			m_game_result.clear();
			
			__uint64 iUserid_arr[3] = {0};

			for (int i=0; i<3; ++i)
			{
				DllPlayerInfo * pi = m_objPlayerMng.GetPlayerInfoBySeat(i);

				if ( pi != NULL ){
					iUserid_arr[i] = pi->iUserid;
				}
			}			
		
			char game_result_buff[32] = {0};
			sprintf(game_result_buff, "{\"Banker\":%d,\"0\":%d,\"1\":%d,\"2\":%d,", 
					m_WhoIsBanker, 
					iUserid_arr[0],
					iUserid_arr[1],
					iUserid_arr[2]);
			
			m_game_result = game_result_buff; 
    	}
		
        calc_scores( nPlayerID, nChair ); // ���						
		
        m_pITable->SetGameEnd();

        m_pITable->WriteLog("DDZSvr::do_GiveCard--GAME END: Normal\n");
        for(int i = 0;i<3;i++)
        {
            DllPlayerInfo   *pstPlayer = m_objPlayerMng.GetPlayerInfoBySeat(i);
            if (pstPlayer == NULL)
            {
                m_pITable->WriteLog("[error] can't find seat=%d",i);
                continue;
            }
            pstPlayer->cStatus = sSit;
        }

        m_WhoIsBanker = 10;
        // ����й��û����
        ResetTrust();
    }
	else //δ����������²ż����¼ҳ���
	{
		//����¼��������йܾͳ���
		if(TRUST_SYS_SET == m_cTrust[m_WhoIsOnTurn])
		{
			if(msgBuf[7] == 1)
			{
				m_pITable->WriteLog("AUTOGIVECARD: last 2 player pass, first to give card");
			}

			AutoAction();	
		}
		else //�Ƿ����й�״̬�����ö�ʱ��
			m_pITable->RegisterTimer(m_WhoIsOnTurn, TIME_OUT_GIVE_CARD, false); //Ϊ��һ�����ע�ᶨʱ��
	}

	return 0;
}

// ���浱�ֺ��ֵܾķ��� ���ڶ��������� 
void CDdzServer::calc_scores( __uint64 nPlayerID,int nChair )
{
	int end_mulity = m_CurrentOrderValue * m_nMulity;
	
	int nNoBankerScore  = m_CurrentOrderValue * m_nMulity * m_RoomSetting.nScoreRadix;
	int nBankerScore	= m_CurrentOrderValue * m_nMulity * m_RoomSetting.nScoreRadix * ( 1 + m_Setting.ucSetOfCards );

    m_pITable->WriteLog("order value = %d, mulity value = %d, banker score = %d, no banker score = %d ",
                        m_CurrentOrderValue, m_nMulity,  nBankerScore, nNoBankerScore);

	if( nChair == m_WhoIsBanker )
	{//banker win
		nNoBankerScore = -nNoBankerScore;
	}
	else
	{//banker lose
		nBankerScore = -nBankerScore;
	}
	
	//���ׯ��ֻ����1�֣�����������Ҽ�����һ��û������������ӱ�
	m_pITable->WriteLog("calc_scores left cards number chairId(0)=%d chairId(1)=%d chairId(2)=%d\n", 
                m_PlayerCards[0].CurrentLength, m_PlayerCards[1].CurrentLength, m_PlayerCards[2].CurrentLength);

	m_pITable->WriteLog("calc_scores give cards number chairId(0)=%d chairId(1)=%d chairId(2)=%d banker give card = %d\n",
                m_PlayerGivedCount[0], m_PlayerGivedCount[1], m_PlayerGivedCount[2], m_PlayerGivedCount[m_WhoIsBanker] );

	if(m_PlayerGivedTime[m_WhoIsBanker] == 1 ||
	   0 == m_PlayerGivedCount[0]+m_PlayerGivedCount[1]+m_PlayerGivedCount[2] - m_PlayerGivedCount[m_WhoIsBanker])
	{
		end_mulity *= 2; // chun tian?
		
		nNoBankerScore *= 2;
		nBankerScore *= 2;
	    m_PlayerGivedCount[0]=m_PlayerGivedCount[1]=m_PlayerGivedCount[2]=0;
	}

	// ��ˮ
	int32_t fee = 0; 

	int nPlayerScores[4] = {0};
	USER_SCORE xScore[4];

	int32_t dizhu = m_RoomSetting.nScoreRadix;
	int nRate = m_RoomSetting.nShareRate;

	if(nRate <= 0 || nRate >= 100)
	{
		nRate = 10;
	}

	int i = 0;
	for( i = 0 ; i < 2 + m_Setting.ucSetOfCards ; i ++ )
	{				
		memset(&xScore[i], 0, sizeof(USER_SCORE));
		if( i != m_WhoIsBanker )
		{
			nPlayerScores[i] = nNoBankerScore;
		}
		else
		{
			nPlayerScores[i] = nBankerScore;
		}

		xScore[i].lScore = nPlayerScores[i];

		if( nPlayerScores[i] > 0 )
		{
			xScore[i].lWinCount = 1;
			xScore[i].lLostCount = 0;

			// cou shui only winner
			int sub_score = xScore[i].lScore* nRate / 100;
			xScore[i].lScore -= sub_score;
			fee += sub_score;
		}
		else
		{
			xScore[i].lWinCount = 0;
			xScore[i].lLostCount = 1;
		}
						        
	}				
			

    char sendBuf[32] = {0};
	for( i = 0 ; i < 2 + m_Setting.ucSetOfCards ; i ++ )
	{
		//m_objPlayerMng.SetUserScore(m_objPlayerMng.GetPlayerInfoBySeat(i),sc_WinNum,xScore[i].lWinCount,sc_LossNum,xScore[i].lLostCount);
		//m_objPlayerMng.SetUserScore(m_objPlayerMng.GetPlayerInfoBySeat(i),xScore[i].lScore);
		//���û���
        DllPlayerInfo* player=m_objPlayerMng.GetPlayerInfoBySeat(i);

        int tmpScore = 0;
        m_pITable->GetPlayerScore(player->iUserid,&tmpScore);
        LOG("%d �ĵ÷��� %d oldtotalsocre=%d\n",i,xScore[i].lScore,tmpScore);

        if (player!=NULL) {
            memset(sendBuf,0,32);
            int *ptr = (int*)sendBuf;
            ptr[sc_WinNum] = xScore[i].lWinCount;
            ptr[sc_LossNum] = xScore[i].lLostCount;
            ptr[sc_Points] = xScore[i].lScore;
            m_pITable->SetGameScore(player->iUserid,32,(void*)sendBuf);
        }
						
		// ToDo zz		
		m_pITable->report_game_flow_detail(m_game_id, player->iUserid, player->m_szNick, 
				tmpScore, xScore[i].lScore, m_pITable->GetRoomID());		 
		 
        int tmpScore2 = 0;
        m_pITable->GetPlayerScore(player->iUserid,&tmpScore2);
        m_objPlayerMng.m_nLastScore[i] = tmpScore2 - tmpScore;
        m_objPlayerMng.m_nTotalScore[i] += m_objPlayerMng.m_nLastScore[i];
        LOG("%d ��ʵ�ʵ÷��� %d newtotalscore=%d userid=%llu\n",i,m_objPlayerMng.m_nLastScore[i],tmpScore2,player->iUserid);
	}

	ClearBuf();

	/* 2	  3--			*/
	/* MsgID  PlayerScores	*/
	msgBuf[2] = SC_FINISH_ROUND;

	//ʣ�µ���
    msgBuf[3] = 3;  //3�����             
	msgBuf[4] = m_CurrentOrderValue; //������������
    char *ptr = &msgBuf[5];			//ը������
	short temp = htons(m_nMulity);
	memcpy((void*)ptr, (void*)&temp, 2);
	ptr+=2;

    int len = 7;    
    for(int i=0;i<3;i++)
    {
        *ptr++ = i;             // i���
        *ptr++ = m_PlayerCards[i].CurrentLength;    // i��ҵ�ʣ������
        len += 2;
        for(int j=0;j<m_PlayerCards[i].CurrentLength;j++)
        {
            *ptr++ = CDdzLogic::CARD2char( m_PlayerCards[i].cards[j]);
            len ++;
			LOG("SC_FINISH_ROUND: %d card is %d", i, CDdzLogic::CARD2char( m_PlayerCards[i].cards[j]));
        }
		
		LOG("SC_FINISH_ROUND: current length is %d", len);
    }

	m_pITable->WriteLog("try to send 'SC_FINISH_ROUND' to table players ...");

    for (int i=0;i<3;i++)
    {
        m_cmdMsgBuffer[i].cmd = SC_FINISH_ROUND;
        m_cmdMsgBuffer[i].nPlayerID = nPlayerID;
        m_cmdMsgBuffer[i].len = len;
        memcpy(m_cmdMsgBuffer[i].msgBuf,msgBuf,len);
    }

    LOG("SC_FINISH_ROUND: total length is %d", len);
	SendData(nPlayerID, SC_FINISH_ROUND,0,msgBuf, len);

	char game_result_buff[32] = {0};
	sprintf(game_result_buff, "\"mulity\":%d}", end_mulity);
	m_game_result += game_result_buff; 
					
	m_pITable->report_game_flow(m_game_id, m_game_result, m_play_flow, dizhu, fee, m_pITable->GetRoomID());

	m_game_result.clear();
	m_play_flow.clear();
	
}

string CDdzServer::gen_game_id()
{
	char strtime[20] = {0};
	time_t timep;
	struct tm *p_tm;
	timep = time(NULL);
	p_tm = localtime(&timep);
	strftime(strtime, sizeof(strtime), "%Y-%m-%d_%H:%M:%S", p_tm);
		

	char str_game_id[128] = {0};
	sprintf(str_game_id, "%s_%d_%d_%d", 
		strtime, m_pITable->GetGameID(), m_pITable->GetRoomID(), m_pITable->GetTableID());		

	m_pITable->WriteLog("gen_game_id: %s", str_game_id);
	//unsigned char md[MD5_DIGEST_LENGTH] = {0};
	//MD5((const unsigned char*)str_game_id, (unsigned long)strlen(str_game_id), md);

	//m_pITable->WriteLog("gen_game_id: %s -> %s", str_game_id, md);

	return str_game_id;
}


extern "C"
{ 
#ifdef WIN32 
	__declspec(dllexport) IGame* CreateGame(IGameAllocator* pAllocator, int iInitMode, const char *szCfgFile, ITable* pTable)
#else
	IGame*  (CreateGame)(IGameAllocator* pAllocator, int iInitMode, const char *szCfgFile, ITable* pTable)
#endif
	{
		char* pBuffer = (char*)pAllocator->Alloc(sizeof(CDdzServer),pTable->GetGameID(),pTable->GetZoneID(),pTable->GetRoomID(),pTable->GetTableID());
		CDdzServer* pChChess = new (pBuffer) CDdzServer;  
		pTable->WriteLog("Init RoomInfo: %d, %d, %d", pTable->GetGameID(), pTable->GetRoomID(), pTable->GetTableID());
		pChChess->Create( pTable ,iInitMode , szCfgFile );
		return pChChess;
	}
}
extern "C"
{ 
#ifdef WIN32 
	__declspec(dllexport) IGame* RestoreGame(void* pData, short nDataLen, int iInitMode,const char *szCfgFile, ITable* pTable)
#else
	IGame* RestoreGame(void* pData, short nDataLen, int iInitMode,const char *szCfgFile, ITable* pTable) 
#endif
	{
		if(nDataLen != (short)sizeof(CDdzServer))
		{
			pTable->WriteLog("error: find input ndatalen=%d,but sizeof(cserver)=%d",nDataLen,sizeof(CDdzServer));
			return NULL;
		}
		CDdzServer* pChChess = new (pData) CDdzServer;  
		pChChess->Restore( pTable ,iInitMode , szCfgFile );  
		return pChChess;
	}
}

void CDdzServer::ProcessDealCards(__uint64 nPlayerID)//
{
	m_game_id = gen_game_id();

	m_pITable->WriteLog("DDZSvr::ProcessDealCards--");
	srand (( unsigned ) time ( NULL ));
	m_WhoIsFisrtToOrder = rand()%3;
	if( 1 == m_Setting.ucSetOfCards )
	{
		m_logic.DealCards( m_PlayerCards[0] , m_PlayerCards[1] , m_PlayerCards[2] , m_BeneathCards );
	}

	int i=0, j=0;
	for( i = 0 ; i < 2 + m_Setting.ucSetOfCards ; i ++ )
	{
		//set cards
		/* 2      3				 4 --	*/
		/* MsgID  NumberOfCards	 Cards  */

		ClearBuf();

		msgBuf[2] = SC_DEAL_CARD;//������
		msgBuf[3] = m_PlayerCards[i].CurrentLength;//����Ƶĳ���
		msgBuf[4] = m_WhoIsFisrtToOrder;//˭��ʼ�Ƚ���

		char szTem[300];
		memset(szTem,0,300);
		for( j = 0 ; j < msgBuf[3] ; j ++ )
		{
			msgBuf[5 + j] = CDdzLogic::CARD2char( m_PlayerCards[i].cards[j] );
			sprintf(szTem+strlen(szTem),"%d,",msgBuf[5 + j]);
		}

		DllPlayerInfo* pPlayer = m_objPlayerMng.GetPlayerInfoBySeat(i);
        if (pPlayer == NULL)
        {
            m_pITable->WriteLog("ProcessDealCards pPlayer == NULL chairID=%d",i);
            return;
        }

        m_cmdMsgBuffer[i].cmd = SC_DEAL_CARD;
        m_cmdMsgBuffer[i].len = msgBuf[3] + 5;
        m_cmdMsgBuffer[i].nPlayerID = nPlayerID;
        memcpy(m_cmdMsgBuffer[i].msgBuf,msgBuf,m_cmdMsgBuffer[i].len);

		SendDataTo( nPlayerID,pPlayer->iUserid, SC_DEAL_CARD, pPlayer->cSeatid, msgBuf, msgBuf[3] + 5 );
	}

    ResetTrust();

	m_gsGameStatus = gsOrdering;
	m_nMulity = 1;
	m_CurrentOrderValue = 0;
	m_WhoIsOnTurn = m_WhoIsFisrtToOrder;

	//��Ӷ�ʱ��, ��λ����Ϊ��ʱ��id
	m_pITable->RegisterTimer(m_WhoIsOnTurn, TIME_OUT_ORDER, false);
}

void CDdzServer::ClearBuf()
{
	memset( msgBuf , 0 , 512 );
}



BOOL CDdzServer::SetCardsType( DDZCards& obCards , int nCardsType )
{
	obCards.nLen	= 0;
	obCards.nLevel	= 0;
	obCards.nType	= 0;

	if( ( nCardsType & CARD_TYPE_SINGLE ) && DDZCards::Check_Card_Type_SINGLE( obCards ) )
	{
		return TRUE;
	}	

	if( ( nCardsType & CARD_TYPE_CONTINUE_SINGLE ) && DDZCards::Check_Card_Type_CONTINUE_SINGLE( obCards ) )
	{
		return TRUE;
	}

	if( ( nCardsType & CARD_TYPE_2_TUPLE ) && DDZCards::Check_Card_Type_2_TUPLE( obCards ) )
	{
		return TRUE;
	}

	if( ( nCardsType & CARD_TYPE_CONTINUE_2_TUPLE ) && DDZCards::Check_Card_Type_CONTINUE_2_TUPLE( obCards ) )
	{
		return TRUE;
	}

	if( ( nCardsType & CARD_TYPE_3_TUPLE ) && DDZCards::Check_Card_Type_3_TUPLE( obCards ) )
	{
		return TRUE;
	}

	if( ( nCardsType & CARD_TYPE_CONTINUE_3_TUPLE ) && DDZCards::Check_Card_Type_CONTINUE_3_TUPLE( obCards ) )
	{
		return TRUE;
	}

	if( ( nCardsType & CARD_TYPE_3_TUPLE_PLUS_SINGLE ) && DDZCards::Check_Card_Type_3_TUPLE_PLUS_SINGLE( obCards ) )
	{
		return TRUE;
	}

	if( ( nCardsType & CARD_TYPE_3_TUPLE_PLUS_2_TUPLE ) && DDZCards::Check_Card_Type_3_TUPLE_PLUS_2_TUPLE( obCards ) )
	{
		return TRUE;
	}

	if( ( nCardsType & CARD_TYPE_CONTINUE_3_TUPLE_PLUS_SINGLE ) && DDZCards::Check_Card_Type_CONTINUE_3_TUPLE_PLUS_SINGLE( obCards ) )
	{
		return TRUE;
	}

	if( ( nCardsType & CARD_TYPE_CONTINUE_3_TUPLE_PLUS_2_TUPLE ) && DDZCards::Check_Card_Type_CONTINUE_3_TUPLE_PLUS_2_TUPLE( obCards ) )
	{
		return TRUE;
	}

//	if( ( nCardsType & CARD_TYPE_CONTINUE_3_TUPLE_PLUS_CONTINUE_SINGLE ) && DDZCards::Check_Card_Type_CONTINUE_3_TUPLE_PLUS_CONTINUE_SINGLE( obCards ) )
//	{
//		return TRUE;
//	}

//	if( ( nCardsType & CARD_TYPE_CONTINUE_3_TUPLE_PLUS_CONTINUE_2_TUPLE ) && DDZCards::Check_Card_Type_CONTINUE_3_TUPLE_PLUS_CONTINUE_2_TUPLE( obCards ) )
//	{
//		return TRUE;
//	}

	if( ( nCardsType & CARD_TYPE_4_TUPLE_PLUS_2_SINGLE ) && DDZCards::Check_Card_Type_4_TUPLE_PLUS_2_SINGLE( obCards ) )
	{
		return TRUE;
	}

	if( ( nCardsType & CARD_TYPE_4_TUPLE_PLUS_2_2_TUPLE ) && DDZCards::Check_Card_Type_4_TUPLE_PLUS_2_2_TUPLE( obCards ) )
	{
		return TRUE;
	}

	if( ( nCardsType & CARD_TYPE_4_BOMB ) && DDZCards::Check_Card_Type_4_BOMB( obCards ) )
	{
		return TRUE;
	}


	if( ( nCardsType & CARD_TYPE_2_JOKER ) && DDZCards::Check_Card_Type_2_JOKER( obCards ) )
	{
		return TRUE;
	}

	return FALSE;
}


char CDdzServer::GetNextOnTurn()
{
	//return ( m_WhoIsOnTurn + 1 + m_Setting.ucSetOfCards ) % ( 2 + m_Setting.ucSetOfCards );

    int old_i = ( m_WhoIsOnTurn + 1 + m_Setting.ucSetOfCards ) % ( 2 + m_Setting.ucSetOfCards );
    int i = m_WhoIsOnTurn;
    i = (i+2)%3;

    m_pITable->WriteLog("GetNextOnTurn old_i=%d new_i=%d",old_i,i);

    return old_i;
}

// �Ƿ�Ĺ�Ϊһ��DDZCards�������� ����ÿ�ζ�����
void CDdzServer::GetLastGiveCards( DDZCards& cards )
{
	cards.ReleaseAll();
	char cSeatID;

	for( int i = 0 ; i < 1 + m_Setting.ucSetOfCards ; i ++ )
	{
		cSeatID = ( m_WhoIsOnTurn + 1 + i ) % ( 2 + m_Setting.ucSetOfCards );
		if( m_CurrentGiveCards[cSeatID].CurrentLength )
		{
			cards.AddCards( m_CurrentGiveCards[cSeatID] );
			return;
		}
	}
}
BOOL CDdzServer::IsFirstCardsBig( DDZCards& firstCards , DDZCards& secondCards )
{
	if( firstCards.nType == secondCards.nType )
	{
		if( firstCards.nLen == secondCards.nLen && firstCards.nLevel > secondCards.nLevel )
		{
			return TRUE;
		}

		return FALSE;
	}

	if( firstCards.nType >= CARD_TYPE_4_BOMB && firstCards.nType > secondCards.nType )
	{
		return TRUE;
	}

	return FALSE;
}

int CDdzServer::SendCurrentDataTo(__uint64 nPlayerID,char cSeatid,char cDllCmd,bool bSendAdd)
{
    m_pITable->WriteLog("{ddz} SendCurrentDataTo  cSeatid=%d cDllCmd=%d nPlayerID=%llu",cSeatid,cDllCmd,nPlayerID);

    // ������Ϸ����
    ClearBuf();
    char *ptr = msgBuf;
    int nLen = 0;

    *ptr++ = m_WhoIsOnTurn;
    nLen++;
    *ptr++ = m_gsGameStatus;
    nLen++;
    *ptr++ = m_CurrentOrderValue;
    nLen++;
    *ptr++ = m_nMulity;
    nLen++;
    *ptr++ = m_WhoIsBanker;
    nLen++;

    // �й���Ϣ
    *ptr++ = m_cTrust[0];
    *ptr++ = m_cTrust[1];
    *ptr++ = m_cTrust[2];
    nLen += 3;

    // �з���Ϣ
    //*ptr++ = m_cOrderPoint[0];
    //*ptr++ = m_cOrderPoint[1];
    //*ptr++ = m_cOrderPoint[2];
    //nLen += 3;

    m_pITable->WriteLog("{ddz} SendCurrentDataTo m_cTrust[0]=%d,m_cTrust[1]=%d,m_cTrust[2]=%d",m_cTrust[0],m_cTrust[1],m_cTrust[2]);

	if(gsPlaying== m_gsGameStatus)
	{
		*ptr++ = CDdzLogic::CARD2char(m_BeneathCards.cards[0]);
		*ptr++ = CDdzLogic::CARD2char(m_BeneathCards.cards[1]);
		*ptr++ = CDdzLogic::CARD2char(m_BeneathCards.cards[2]);

	}
	else if(gsOrdering == m_gsGameStatus) //���������׶β�������
	{
		*ptr++ = m_cOrderPoint[0];
		*ptr++ = m_cOrderPoint[1];
		*ptr++ = m_cOrderPoint[2];
	}
	else
	{
		m_pITable->WriteLog("{ddz} wrong status,current status is %d", m_gsGameStatus);
	}
    nLen += 3;

    m_pITable->WriteLog("{ddz} SendCurrentDataTo m_BeneathCards[0]=%d m_BeneathCards[1]=%d m_BeneathCards[2]=%d",
				m_BeneathCards.cards[0].number,m_BeneathCards.cards[1].number,m_BeneathCards.cards[2].number);

    for(int i=0;i<3;i++)
    {
        *ptr++ = i;                         // �����λid
        //*pInt++ = m_objPlayerMng.m_nLastScore[i];          //��һ�ֵķ���
        //*pInt = m_objPlayerMng.m_nTotalScore[i];           //�ܷ���
        //ptr += sizeof(int)*2;
        //nLen += 1+sizeof(int)*2;
        nLen += 1;

        *ptr++ = m_PlayerCards[i].CurrentLength;                //ʣ������
        *ptr++ = m_CurrentGiveCards[i].CurrentLength;     //�����Ƶ�����
        nLen += 2;

        //*ptr++ = PlayerPass[i];             //�Ƿ���pass
        //nLen++;

        m_pITable->WriteLog("{ddz} SendCurrentDataTo seat=%d lastsocre==%d totalsocre=%d leftCardsNum=%d SendCardsNum=%d pass=%d", 
						i, m_objPlayerMng.m_nLastScore[i],m_objPlayerMng.m_nTotalScore[i],m_PlayerCards[i].CurrentLength,
						m_CurrentGiveCards[i].CurrentLength,PlayerPass[i]);

        for (int j=0;j<m_CurrentGiveCards[i].CurrentLength;j++)
        {
            *ptr++ = CDdzLogic::CARD2char(m_CurrentGiveCards[i].cards[j]);  //������
            nLen++;
            m_pITable->WriteLog("{ddz} SendCurrentDataTo send c=%d",m_CurrentGiveCards[i].cards[j].number);
        }

        //��Ϊ��ǰ����� ���͸���ҵ���
        if(i == cSeatid)
        {
            m_pITable->WriteLog("{ddz} SendCurrentDataTo replay seat=%d",i);

            for (int j=0;j<m_PlayerCards[i].CurrentLength;j++)
            {
                *ptr++ =  CDdzLogic::CARD2char(m_PlayerCards[i].cards[j]);
                nLen++;
                m_pITable->WriteLog("{ddz} SendCurrentDataTo my c=%d", m_PlayerCards[i].cards[j].number);
            }
        }

    }
	//��ǵ������ƴ��������ڷ�����
	*ptr++ = 1;
    nLen++;
	*ptr++ = m_WhoIsFisrtToOrder;
	nLen++;

    int nRet = SendDataTo(nPlayerID, nPlayerID, cDllCmd, cSeatid, msgBuf, nLen);

    m_pITable->WriteLog("{ddz} SendCurrentDataTo nLen=%d nRet=%d",nLen,nRet);

    return nRet;
}

void CDdzServer::ResetTrust()
{
    memset(m_cTrust,0,3);
    memset(m_cOrderPoint,10,3);
	memset(m_timeOutTimes, 0, 3);
}

void CDdzServer::AutoAction()
{
	DllPlayerInfo* pPlayer = m_objPlayerMng.GetPlayerInfoBySeat(m_WhoIsOnTurn);
	if(NULL == pPlayer)
	{
		m_pITable->WriteLog("{ddz} OnGameTimeout pPlayer == NULL chairID=%d",m_WhoIsOnTurn);
		return;
	}
	
	//1. �е���ֱ��pass
	if(gsOrdering == m_gsGameStatus)
	{
		m_pITable->WriteLog("AutoGiveCard: current status is gsOrdering");
		char msg[2] = {'\0'};
		msg[0] = CS_ORDER;
		msg[1] = CARD_ORDER_PASS;
		
		do_Order(pPlayer->iUserid, m_WhoIsOnTurn, msg);
	}
	else if(gsPlaying == m_gsGameStatus) //���Ƴ�ʱ��ֱ�ӳ���
	{
		char msg[32] = {'\0'};
		msg[0] = CS_GIVE_CARDS;

		NEW_VARIABLE( DDZCards , OutCards);

		NEW_VARIABLE( DDZCards , LastGiveCards );
		GetLastGiveCards( LastGiveCards ); 
		m_PlayerCards[m_WhoIsOnTurn].respond_last_player_cards(LastGiveCards, false, OutCards);

		//û���й�ֱ��pass,���Ҳ�Ϊ�׼�
		if(TRUST_CANCLE == m_cTrust[m_WhoIsOnTurn] &&
			0 != LastGiveCards.CurrentLength)  
		{
			m_pITable->WriteLog("AutoGiveCard: not trust, not first, pass it");
			OutCards.ReleaseAll();
		}

		msg[4] = OutCards.CurrentLength;
		m_pITable->WriteLog("AutoGiveCard: user %I64u", pPlayer->iUserid);
		for(int i = 0; i < OutCards.CurrentLength; ++i)
		{
			msg[5 + i] = CDdzLogic::CARD2char(OutCards.cards[i]);
			m_pITable->WriteLog("AutoGiveCard: %d card is %d, sharp %d", i + 1, OutCards.cards[i].number, OutCards.cards[i].shape);
		}

		do_GiveCard(pPlayer->iUserid, m_WhoIsOnTurn, msg);
	}
}
