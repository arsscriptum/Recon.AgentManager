#pragma once

#include "../MyHpSocket/MyHpSocket.h"
#include "../MyCrypto/MyCrypto.h"


// ע�⣬������TCP_PUSHģʽ���������㷨ֻ�ܲ�����������
enum TCP_MODE { TCP_PACK, TCP_PUSH };


// ͨ��Socket�࣬��ͨ���ࡢԶ��SHELLͨ����ȶ������Դ��࣬�������ʵ��OnReceiveWithDec()
class CSocket : public CMyTcpPackServer, public CMyTcpPushServer {

protected:
	TCP_MODE	m_iTcpMode		= TCP_PACK;

	WCHAR		m_wszAddress[32];
	WORD		m_wPort			= 0;


public:

	CMyTcpPackServer* m_pTcpPackServer;
	CMyTcpPushServer* m_pTcpPushServer;


private:
	BOOL IsNewClientSocket(CONNID dwConnID, const BYTE* pData, int iLength);
	BOOL InitNewClientSocket(CONNID dwConnID, const BYTE* pData, int iLength);

protected:
	EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual VOID OnReceiveWithDec(ITcpServer* pSender, CONNID dwConnID, QWORD qwClientToken, WORD wServiceType, WORD wSocketType, int iCommandID, DWORD dwParentSocketTag, MyBuffer mBuffer) = 0;

	EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);

public:
	CSocket(TCP_MODE iTcpMode);
	~CSocket();

	VOID InitSocket(LPWSTR pwszAddress, WORD wPort);
	BOOL StartSocket();
	BOOL StopSocket();
	BOOL Disconnect(CONNID dwConnID);

	// ���ķ���������һ��ʼ����key��iv��һ�����ù�SendWithEnc��ǧ�����ٵ���Send�������ᵼ�¶Զ˽��ܳ�����
	BOOL Send(CONNID dwConnID, PBYTE pBuffer, DWORD dwLength);
	// ���ܺ󷢰�
	BOOL SendWithEnc(CONNID dwConnID, int iCommandID, PBYTE pBuffer = nullptr, DWORD dwLength = 0);

	BOOL SendCreateChildSocket(CONNID dwConnID, int iCommandID, DWORD dwParentSocketTag);

	VOID CSocket::DebugPrintRecvParams(CONNID dwConnID, QWORD qwClientToken, WORD wServiceType, WORD wSocketType, int iCommandID, DWORD dwParentSocketTag, MyBuffer mBuffer);
};