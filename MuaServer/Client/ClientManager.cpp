#include "pch.h"
#include "ClientManager.h"
#include "../MuaServer.h"
#include "../MuaServerDlg.h"


CClientManager::CClientManager(){
	InitializeCriticalSection(&m_cs);		// ��ʼ����
}


CClientManager::~CClientManager() {
	//EnterCriticalSection(&m_cs);			// ����

	for (auto iter = m_mapClientUnit.begin(); iter != m_mapClientUnit.end();) {
		QWORD qwClientToken = iter->first;
		CClientUnit* pClientUnit = iter->second;
		iter++;										// erase��ǰiter�󣬾�û��iter++�ˡ�����Ҫ��ɾ��ǰʵ�ֵ������ĵ�����
		DeleteClientUnit(pClientUnit);
	}

	//LeaveCriticalSection(&m_cs);			// ����
}


BOOL CClientManager::IsTokenConflict(QWORD qwClientToken)
{
	//EnterCriticalSection(&m_cs);			// ����

	BOOL bRet = m_mapClientUnit.find(qwClientToken) != m_mapClientUnit.end();
	
	//LeaveCriticalSection(&m_cs);			// ����

	return bRet;
}


VOID CClientManager::AddClientUtil(QWORD qwClientToken, CClientUnit* pClientUnit)
{
	//EnterCriticalSection(&m_cs);			// ����

	m_mapClientUnit.insert({ qwClientToken, pClientUnit });
	DebugPrint("m_mapClientSocket.size = %d\n", m_mapClientSocket.size());
	DebugPrint("m_mapClientUnit.size = %d\n", m_mapClientUnit.size());

	//LeaveCriticalSection(&m_cs);			// ����
}


VOID CClientManager::AddClientSocket(CONNID dwConnID, CClientSocket* pClientSocket)
{
	//EnterCriticalSection(&m_cs);			// ����
	
	m_mapClientSocket.insert({ dwConnID, pClientSocket });
	DebugPrint("m_mapClientSocket.size = %d\n", m_mapClientSocket.size());
	DebugPrint("m_mapClientUnit.size = %d\n", m_mapClientUnit.size());

	//LeaveCriticalSection(&m_cs);			// ����
}


CClientUnit* CClientManager::GetClientUnitByConnId(CONNID dwConnID)
{
//	EnterCriticalSection(&m_cs);			// ����

	CClientUnit* pRet = nullptr;
	CClientSocket* pClientSocket = GetClientSocketByConnId(dwConnID);
	if (pClientSocket != nullptr){
		QWORD qwClientToken = pClientSocket->m_qwClientToken;
		if (m_mapClientUnit.find(qwClientToken) == m_mapClientUnit.end()) {
			pRet = nullptr;
		}
		else {
			pRet= m_mapClientUnit[qwClientToken];
		}
	}
	else {
		pRet = nullptr;
	}

	//LeaveCriticalSection(&m_cs);			// ����
	return pRet;
}


CClientUnit* CClientManager::GetClientUnitByToken(QWORD qwClientToken) {
	//EnterCriticalSection(&m_cs);			// ����

	CClientUnit* pClientUnit = nullptr;
	if (m_mapClientUnit.find(qwClientToken) == m_mapClientUnit.end()) {
		pClientUnit = nullptr;				// EnterCriticalSection��LeaveCriticalSectionǶ�׵Ĵ�������һ�£���ǰreturn�ᵼ��δ֪�Ĵ���
	}
	else {
		pClientUnit = m_mapClientUnit[qwClientToken];
	}

	//LeaveCriticalSection(&m_cs);			// ����
	return pClientUnit;
}


CClientSocket* CClientManager::GetClientSocketByConnId(CONNID dwConnID)
{
	//EnterCriticalSection(&m_cs);			// ����

	CClientSocket* pClientSocket = nullptr;
	if (m_mapClientSocket.find(dwConnID) == m_mapClientSocket.end()) {
		pClientSocket = nullptr;
	}
	else {
		pClientSocket = m_mapClientSocket[dwConnID];
	}

	//LeaveCriticalSection(&m_cs);			// ����
	return pClientSocket;
}


CServerSocket* CClientManager::GetServerSocket() {
	return ((CMuaServerDlg*)(theApp.m_pMainWnd))->m_pServerSocket;
}


VOID CClientManager::DeleteClientSocket(CClientSocket* pClientSocket) {
	// ����
	//EnterCriticalSection(&m_cs);

	CClientUnit* pClientUnit = GetClientUnitByToken(pClientSocket->m_qwClientToken);

	if (pClientUnit != nullptr) {
		// ��ClientUnit��map��ɾ����ClientSocket
		pClientUnit->m_mapClientSocketUseConnID.erase(pClientSocket->m_dwConnID);
		pClientUnit->m_mapClientSocketUseTag.erase(pClientSocket->m_dwSocketTag);

		// ��ClientManager��map��ɾ����ClientSocket
		this->m_mapClientSocket.erase(pClientSocket->m_dwConnID);

		// �Ͽ�socket���ӣ���һ��Ҫ�ڡ���ClientUnit��map��ɾ����ClientSocket�����棬����OnClose��ʱ��GetClientSocketByConnID����ֵ����nullptr��
		BOOL bRet = GetServerSocket()->Disconnect(pClientSocket->m_dwConnID);

		DebugPrint("m_mapClientSocket.size = %d\n", m_mapClientSocket.size());
		DebugPrint("m_mapClientUnit.size = %d\n", m_mapClientUnit.size());

		delete pClientSocket;
	}

	// ����
	//LeaveCriticalSection(&m_cs);					
}


VOID CClientManager::DeleteClientUnit(CClientUnit* pClientUnit) {
	// ����	
	//EnterCriticalSection(&m_cs);			

	// �Ͽ���ClientUnit���е�ClientSocket��socket����
	for (auto iter = pClientUnit->m_mapClientSocketUseConnID.begin(); iter != pClientUnit->m_mapClientSocketUseConnID.end();) {
		CONNID dwConnID = iter->first;
		CClientSocket* pClientSocket = iter->second;
		iter++;

		// ���ĳ����socket�ж�Ӧ�ĶԻ�����ر�
		// ע��ֻ��ɾ��ClientUnitʱ�ſ��ǹر�������socket��Ӧ�ĶԻ���
		// ���ֻ����socket�Ͽ������ˣ��򲻹رն�Ӧ�Ի��򣨷��򣬱���Զ��SHELL�ĶϿ�����ǰ�����ݽ��ᶪʧ��
		if (pClientSocket->m_wServiceType != nsGeneralSocket::MAIN_SOCKET_SERVICE) {
			if (pClientSocket->m_pServiceDialog != nullptr) {
				delete pClientSocket->m_pServiceDialog;			// �����������������ҪDeleteClientSocket
				pClientSocket->m_pServiceDialog = nullptr;
			}
			else {
				delete pClientSocket->m_pServiceObject;
				pClientSocket->m_pServiceObject = nullptr;
			}
		}
		else {
			DeleteClientSocket(pClientSocket);
		}
	}

	// ��ClientManager��map��ɾ����ClientUnit
	this->m_mapClientUnit.erase(pClientUnit->m_qwClientToken);
	DebugPrint("m_mapClientSocket.size = %d\n", m_mapClientSocket.size());
	DebugPrint("m_mapClientUnit.size = %d\n", m_mapClientUnit.size());

	// ��UI��ɾ����ClientUnit
	CListCtrl* pClientList = &((CMuaServerDlg*)(theApp.m_pMainWnd))->m_ClientList;
	for (DWORD dwIndex = 0; dwIndex < pClientList->GetItemCount(); dwIndex++) {
		// ��ȡ��ö�ٵ���һ�еĶ�����Ϣ��pClientUnit
		LV_ITEM  lvitemData = { 0 };
		lvitemData.mask = LVIF_PARAM;
		lvitemData.iItem = dwIndex;
		pClientList->GetItem(&lvitemData);
		CClientUnit* pClientUnitTemp = (CClientUnit*)lvitemData.lParam;

		// ȷ��Ҫɾ�������е���������ɾ����һ��
		if (pClientUnit == pClientUnitTemp) {
			pClientList->DeleteItem(dwIndex);
			break;
		}
	}

	// ����	
	//LeaveCriticalSection(&pClientUnit->m_cs);			

	delete pClientUnit;
}