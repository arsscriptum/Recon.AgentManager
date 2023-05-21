#pragma once
#include <unordered_map>
#include "Socket.h"


// �������е�Mua�ͻ���
class CSocketManager {
private:
	unordered_map<CONNID, CClientSocket*> m_mapSocket;

	CRITICAL_SECTION m_cs;

public:
	CSocketManager();
	~CSocketManager();

	VOID AddSocket(, CSocket* pSocket);

	CClientUnit* GetClientUnitByConnId(CONNID dwConnID);
	CClientUnit* GetClientUnitByToken(QWORD qwClientToken);
	CClientSocket* GetClientSocketByConnId(CONNID dwConnID);

	VOID DeleteClientSocket(CClientSocket* pClientSocket);
};