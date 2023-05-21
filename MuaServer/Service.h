#pragma once
#include "ServerSocket.h"
#include "MuaServer.h"
#include "MuaServerDlg.h"


// ������Ļ���
class CService
{
public:
	virtual VOID OnReceiveWithDec(ITcpServer* pSender, CONNID dwConnID, QWORD qwClientToken, WORD wServiceType, WORD wSocketType, int iCommandID, DWORD dwParengSocketTag, MyBuffer mBuffer) = 0;

	CClientManager* CService::GetClientManager();
	CServerSocket* CService::GetServerSocket();
};
