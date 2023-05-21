#include "pch.h"
#include "MyTcpPackClient.h"

CMyTcpPackClient::CMyTcpPackClient() : m_pTcpPackClient(this) {
	;
}


VOID CMyTcpPackClient::InitSocket() {
	m_bIsRunning = FALSE;

	m_dwConnectId = 0;

	//m_dwConnectId = m_pTcpPackClient->GetConnectionID();	// ����Ҫstart֮����ܻ�ȡ

	// �������ݰ���󳤶ȣ���Ч���ݰ���󳤶Ȳ��ܳ���0x3FFFFF�ֽ�(4MB-1B)��Ĭ�ϣ�262144/0x40000 (256KB)
	m_pTcpPackClient->SetMaxPackSize(PACKET_MAX_LENGTH);
	// ���������������ͼ��
	m_pTcpPackClient->SetKeepAliveTime(60 * 1000);
	// ��������������԰����ͼ��
	m_pTcpPackClient->SetKeepAliveInterval(20 * 1000);
}


VOID CMyTcpPackClient::SetRemoteAddress(PWCHAR pwszAddress, WORD wPort) {
	wcscpy_s(m_wszAddress, pwszAddress);
	m_wPort = wPort;
}


BOOL CMyTcpPackClient::StartSocket() {

	BOOL bRet = TRUE;

	if (!(m_pTcpPackClient->IsConnected())) {
		// Ĭ�����첽connect��bRet����true��һ������ɹ����ӡ��������ˣ��ָĳ�ͬ��connect
		bRet = m_pTcpPackClient->Start(m_wszAddress, m_wPort, 0);
		if (!bRet) {
			return FALSE;
		}
	}

	m_bIsRunning = TRUE;

	// �������ڹ��캯����GetConnectionID��һֱ����0.����start֮����е�CONNID��
	m_dwConnectId = m_pTcpPackClient->GetConnectionID();
	return bRet;
}


BOOL CMyTcpPackClient::StopSocket() {
	BOOL bRet = m_pTcpPackClient->Stop();
	if (bRet) {
		m_bIsRunning = FALSE;
		return TRUE;
	}
	else {
		return FALSE;
	}
}


BOOL CMyTcpPackClient::Send(BYTE* pBuffer, int iLength, int iOffset) {
	if (!m_pTcpPackClient->IsConnected()) {
		return FALSE;
	}

	BOOL bRet;

	if (m_bIsRunning == TRUE) {
		bRet = m_pTcpPackClient->Send(pBuffer, iLength, iOffset);
	}
	else {
		bRet = FALSE;
	}

	return bRet;
}


BOOL CMyTcpPackClient::IsConnected() {
	return m_pTcpPackClient->IsConnected();
}


// �ص�����

EnHandleResult CMyTcpPackClient::OnPrepareConnect(ITcpClient* pSender, CONNID dwConnID, SOCKET socket) {
	DebugPrint("[Client %d] OnPrepareConnect: \n", dwConnID);
	return HR_OK;
}


EnHandleResult CMyTcpPackClient::OnConnect(ITcpClient* pSender, CONNID dwConnID) {
	DebugPrint("[Client %d] OnConnect: \n", dwConnID);
	return HR_OK;
}


EnHandleResult CMyTcpPackClient::OnHandShake(ITcpClient* pSender, CONNID dwConnID) {
	DebugPrint("[Client %d] OnHandShake: \n", dwConnID);
	return HR_OK;
}


EnHandleResult CMyTcpPackClient::OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength) {
	DebugPrint("[Client %d] OnSend: %d Bytes \n", dwConnID, iLength);
	return HR_OK;
}


EnHandleResult CMyTcpPackClient::OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength) {
	DebugPrint("[Client %d] OnReceive: %d Bytes \n", dwConnID, iLength);
	return HR_OK;
}


EnHandleResult CMyTcpPackClient::OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) {
	DebugPrint("[Client %d] OnClose: \n", dwConnID);
	return HR_OK;
}