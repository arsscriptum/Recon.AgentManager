#pragma once

#include <unordered_map>

#include "ClientUnit.h"
#include "../ServerSocket.h"


class CServerSocket;


// �������е�Mua�ͻ���
class CClientManager{
private:
	unordered_map<QWORD, CClientUnit*> m_mapClientUnit;
	unordered_map<CONNID, CClientSocket*> m_mapClientSocket;

	CRITICAL_SECTION m_cs;

public:
	CClientManager();
	~CClientManager();

	// Ϊ��ClientUtil����TOKENǰ��Ҫ����TOKEN�Ƿ��������ClientUtil��TOKEN��ͻ
	BOOL IsTokenConflict(QWORD qwClientToken);

	VOID AddClientUtil(QWORD qwClientToken, CClientUnit* pClientUnit);
	VOID AddClientSocket(CONNID dwConnID, CClientSocket* pClientSocket);

	CClientUnit* GetClientUnitByConnId(CONNID dwConnID);
	CClientUnit* GetClientUnitByToken(QWORD qwClientToken);
	CClientSocket* GetClientSocketByConnId(CONNID dwConnID);

	CServerSocket* GetServerSocket();

	VOID DeleteClientSocket(CClientSocket* pClientSocket);
	VOID DeleteClientUnit(CClientUnit* pClientUnit);
};