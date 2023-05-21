#pragma once

#include "pch.h"

class CMyTcpPackServer : public CTcpServerListener {
public:
	WCHAR					m_wszAddress[32];
	WORD					m_wPort = 0;

	CTcpPackServerPtr		m_pTcpPackServer;

	BOOL					m_bIsRunning = FALSE;


public:
	CMyTcpPackServer();

	VOID SetLocalAddress(PWCHAR pwszAddress, WORD wPort);
	VOID InitSocket();
	BOOL StartSocket();
	BOOL StopSocket();
	BOOL Send(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset = 0);


protected:
	// CTcpServerListener�ĳ����������ڻص���ȫ����ʵ�֣���Ȼ�ᱨ������ʵ����������
	virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen);
	virtual EnHandleResult OnAccept(ITcpServer* pSender, CONNID dwConnID, SOCKET soClient);
	virtual EnHandleResult OnHandShake(ITcpServer* pSender, CONNID dwConnID);
	virtual EnHandleResult OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult OnShutdown(ITcpServer* pSender);
};