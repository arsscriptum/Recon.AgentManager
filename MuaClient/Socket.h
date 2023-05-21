#pragma once


// ע�⣬������TCP_PUSHģʽ���������㷨ֻ�ܲ�����������
enum TCP_MODE { TCP_PACK, TCP_PUSH };


// ͨ��Socket�࣬��ͨ���ࡢԶ��SHELLͨ����ȶ������Դ��࣬�������ʵ��OnReceiveWithDec()
class CSocket : public CMyTcpPackClient, public CMyTcpPushClient {

protected:
	TCP_MODE	m_iTcpMode		= TCP_PACK;

	WCHAR		m_wszAddress[32];
	WORD		m_wPort			= 0;

	CEncrypt*	m_pEnc			= nullptr;
	CDecrypt*	m_pDec			= nullptr;
	BYTE		m_pbKey[MAX_KEY_LEN];
	BYTE		m_pbIv[MAX_IV_LEN];

	BOOL		m_bIsMainClientSocket = FALSE;
	BOOL		m_bRecvFirstPacketReturn = FALSE;

	QWORD		m_qwClientToken = 0;
	nsGeneralSocket::SERVICE_TYPE	m_wServiceType = nsGeneralSocket::NULL_SERVICE;
	WORD		m_wSocketType = 0;
	DWORD		m_dwParentSocketTag = 0;


public:

	CMyTcpPackClient* m_pTcpPackClient;
	CMyTcpPushClient* m_pTcpPushClient;


protected:

	EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual VOID OnReceiveWithDec(ITcpClient* pSender, CONNID dwConnID, QWORD qwClientToken, WORD wServiceType, WORD wSocketType, int iCommandID, DWORD dwParentSocketTag, MyBuffer mBuffer) = 0;

	virtual VOID SendLoginPacket();

	BOOL CreateSendKeyIv();


public:

	CSocket(TCP_MODE iTcpMode, nsGeneralSocket::SERVICE_TYPE wServiceType, WORD wSocketType, QWORD qwClientToken = 0, DWORD dwParentSocketTag = 0);
	~CSocket();

	VOID InitSocket(LPWSTR pwszAddress, WORD wPort);
	BOOL StartSocket();
	BOOL StopSocket();

	VOID SetCryptoAlg(CRYPTO_ALG iCryptoAlg);

	// ���ķ���������һ��ʼ����key��iv��һ�����ù�SendWithEnc��ǧ�����ٵ���Send�������ᵼ�¶Զ˽��ܳ�����
	BOOL Send(PBYTE pBuffer, DWORD dwLength);
	// ���ܺ󷢰�
	BOOL SendWithEnc(int iCommandID, PBYTE pBuffer = nullptr, DWORD dwLength = 0);

	BOOL IsConnected();

	VOID DebugPrintRecvParams(CONNID dwConnID, QWORD qwClientToken, WORD wServiceType, WORD wSocketType, int iCommandID, DWORD dwParentSocketTag, MyBuffer mBuffer);

	VOID SetParentSocketTag(DWORD dwParentSocketTag);
};