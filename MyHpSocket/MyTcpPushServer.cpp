#include "pch.h"
#include "MyTcpPushServer.h"


CMyTcpPushServer::CMyTcpPushServer() : m_pTcpPushServer(this) {
	;
}


VOID CMyTcpPushServer::InitSocket() {
	m_bIsRunning = FALSE;

	// ���������������ͼ��
	m_pTcpPushServer->SetKeepAliveTime(60 * 1000);
	// ��������������԰����ͼ��
	m_pTcpPushServer->SetKeepAliveInterval(20 * 1000);
}


VOID CMyTcpPushServer::SetLocalAddress(PWCHAR pwszAddress, WORD wPort) {
	wcscpy_s(m_wszAddress, pwszAddress);
	m_wPort = wPort;
}


// ����socket�����
BOOL CMyTcpPushServer::StartSocket() {

	BOOL bRet = m_pTcpPushServer->Start(m_wszAddress, m_wPort);
	if (!bRet) {
		return FALSE;
	}
	else {

#ifdef _DEBUG
		USES_CONVERSION;									// ʹ��A2W֮ǰ���������
		DebugPrint("Socket����������ɹ���ģʽTCP_PUSH��IP=%s, PORT=%d\n", W2A(m_wszAddress), m_wPort);
#endif

		m_bIsRunning = TRUE;
		return TRUE;
	}
}


BOOL CMyTcpPushServer::StopSocket() {
	BOOL bRet = m_pTcpPushServer->Stop();
	if (bRet) {
		m_bIsRunning = FALSE;
		return TRUE;
	}
	else {
		return FALSE;
	}
}


BOOL CMyTcpPushServer::Send(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset) {
	if (!m_pTcpPushServer->IsConnected(dwConnID)) {
		return FALSE;
	}

	BOOL bRet = m_pTcpPushServer->Send(dwConnID, pBuffer, iLength, iOffset);
	return bRet;
}


// �ص�������ʵ��

EnHandleResult CMyTcpPushServer::OnPrepareListen(ITcpServer* pSender, SOCKET soListen) {
	DebugPrint("OnPrepareListen: \n");
	return HR_OK;
}


EnHandleResult CMyTcpPushServer::OnAccept(ITcpServer* pSender, CONNID dwConnID, SOCKET soClient) {
	DebugPrint("[Client %d] OnAccept: \n", dwConnID);
	return HR_OK;
}


EnHandleResult CMyTcpPushServer::OnHandShake(ITcpServer* pSender, CONNID dwConnID) {
	DebugPrint("[Client %d] OnHandShake: \n", dwConnID);
	return HR_OK;
}


EnHandleResult CMyTcpPushServer::OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength) {
	DebugPrint("[Client %d] OnSend: %d Bytes \n", dwConnID, iLength);
	return HR_OK;
}


EnHandleResult CMyTcpPushServer::OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength) {
	DebugPrint("[Client %d] OnReceive: %d Bytes \n", dwConnID, iLength);
	return HR_OK;
}


EnHandleResult CMyTcpPushServer::OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) {
	DebugPrint("[Client %d] OnClose: \n", dwConnID);
	return HR_OK;
}


EnHandleResult CMyTcpPushServer::OnShutdown(ITcpServer* pSender) {
	DebugPrint("OnShutdown: \n");
	DebugPrint("Socket����˹رճɹ�\n");
	return HR_OK;
}