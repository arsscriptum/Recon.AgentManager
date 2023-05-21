#include "pch.h"
#include "ClientUnit.h"
#include "ClientManager.h"
#include "../MuaServer.h"
#include "../MuaServerDlg.h"


CClientUnit::CClientUnit(CONNID dwConnID)
{
	// ��ȡ�Զ�IP�Ͷ˿�
	m_dwMainSocketConnID = dwConnID;
	int iAddressLen = 32;
	((CMuaServerDlg*)(theApp.m_pMainWnd))->m_pServerSocket->m_pTcpPackServer->m_pTcpPackServer->GetRemoteAddress(dwConnID, m_wszIpAddress, iAddressLen, m_wPort);

	// ����TOKEN
	AutoSeededRandomPool  rng;
	rng.GenerateBlock((PBYTE)&m_qwClientToken, sizeof(QWORD));

	// ���TOKEN��ͻ�˾���������
	while (((CMuaServerDlg*)(theApp.m_pMainWnd))->m_pClientManager->IsTokenConflict(m_qwClientToken)) {
		rng.GenerateBlock((PBYTE)&m_qwClientToken, sizeof(QWORD));
	}

	InitializeCriticalSection(&m_cs);				// ��ʼ����������DeleteClientUnit
}


VOID CClientUnit::AddClientSocket(CONNID dwConnID, CClientSocket* pClientSocket)
{
	m_mapClientSocketUseConnID.insert({ dwConnID, pClientSocket });
	m_mapClientSocketUseTag.insert({ pClientSocket->m_dwSocketTag, pClientSocket });
}


CClientSocket* CClientUnit::GetClientSocketByConnId(CONNID dwConnID)
{
	if (m_mapClientSocketUseConnID.find(dwConnID) == m_mapClientSocketUseConnID.end()) {
		return nullptr;
	}
	else {
		return m_mapClientSocketUseConnID[dwConnID];
	}
}


CClientSocket* CClientUnit::GetClientSocketByTag(DWORD dwSocketTag)
{
	if (m_mapClientSocketUseTag.find(dwSocketTag) == m_mapClientSocketUseTag.end()) {
		return nullptr;
	}
	else {
		return m_mapClientSocketUseTag[dwSocketTag];
	}
}


BOOL CClientUnit::IsSocketTagConflict(DWORD dwSocketTag)
{
	BOOL bRet = m_mapClientSocketUseTag.find(dwSocketTag) != m_mapClientSocketUseTag.end();
	return bRet;
}