#include "pch.h"
#include "MyTcpPackServer.h"


CMyTcpPackServer::CMyTcpPackServer() : m_pTcpPackServer(this) {
	;
}

 
VOID CMyTcpPackServer::InitSocket() {
	m_bIsRunning = FALSE;

	// �������ݰ���󳤶ȣ���Ч���ݰ���󳤶Ȳ��ܳ���0x3FFFFF�ֽ�(4MB-1B)��Ĭ�ϣ�262144/0x40000 (256KB)
	m_pTcpPackServer->SetMaxPackSize(PACKET_MAX_LENGTH);
	// ���������������ͼ��
	m_pTcpPackServer->SetKeepAliveTime(60 * 1000);
	// ��������������԰����ͼ��
	m_pTcpPackServer->SetKeepAliveInterval(20 * 1000);
}


VOID CMyTcpPackServer::SetLocalAddress(PWCHAR pwszAddress, WORD wPort) {
	wcscpy_s(m_wszAddress, pwszAddress);	
	m_wPort = wPort;
}


// ����socket�����
BOOL CMyTcpPackServer::StartSocket() {

	BOOL bRet = m_pTcpPackServer->Start(m_wszAddress, m_wPort);
	if (!bRet) {
		return FALSE;
	}
	else {

#ifdef _DEBUG
		USES_CONVERSION;									// ʹ��A2W֮ǰ���������
		DebugPrint("Socket����������ɹ���ģʽTCP_PACK��IP=%s, PORT=%d\n", W2A(m_wszAddress), m_wPort);
#endif

		m_bIsRunning = TRUE;
		return TRUE;
	}
}


BOOL CMyTcpPackServer::StopSocket() {
	BOOL bRet = m_pTcpPackServer->Stop();
	if (bRet) {
		m_bIsRunning = FALSE;
		return TRUE;
	}
	else {
		return FALSE;
	}
}


BOOL CMyTcpPackServer::Send(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset) {
	if (!m_pTcpPackServer->IsConnected(dwConnID)) {
		return FALSE;
	}

	BOOL bRet = m_pTcpPackServer->Send(dwConnID, pBuffer, iLength, iOffset);
	return bRet;
}


// �ص�������ʵ��

EnHandleResult CMyTcpPackServer::OnPrepareListen(ITcpServer* pSender, SOCKET soListen) {
	DebugPrint("OnPrepareListen: \n");
	return HR_OK;
}


EnHandleResult CMyTcpPackServer::OnAccept(ITcpServer* pSender, CONNID dwConnID, SOCKET soClient) {
	DebugPrint("[Client %d] OnAccept: \n", dwConnID);
	return HR_OK;
}


EnHandleResult CMyTcpPackServer::OnHandShake(ITcpServer* pSender, CONNID dwConnID) {
	DebugPrint("[Client %d] OnHandShake: \n", dwConnID);
	return HR_OK;
}


EnHandleResult CMyTcpPackServer::OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength) {
	DebugPrint("[Client %d] OnSend: %d Bytes \n", dwConnID, iLength);
	return HR_OK;
}


EnHandleResult CMyTcpPackServer::OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength) {
	DebugPrint("[Client %d] OnReceive: %d Bytes \n", dwConnID, iLength);
	return HR_OK;
}


EnHandleResult CMyTcpPackServer::OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) {
	DebugPrint("[Client %d] OnClose: \n", dwConnID);
	return HR_OK;
}


EnHandleResult CMyTcpPackServer::OnShutdown(ITcpServer* pSender) {
	DebugPrint("OnShutdown: \n");
	DebugPrint("Socket����˹رճɹ�\n");
	return HR_OK;
}