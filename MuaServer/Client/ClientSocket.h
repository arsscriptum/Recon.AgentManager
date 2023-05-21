#pragma once


// CClientSocket��ʾһ������Mua�ͻ��˵�Socket���ӣ�����Client��MainSocket�͸������Socket.
class CClientSocket{

private:
	PBYTE		m_pbKey			= nullptr;
	PBYTE		m_pbIv			= nullptr;

public:
	CONNID		m_dwConnID = 0;

	WCHAR		m_wszIpAddress[32] = { 0 };
	WORD		m_wPort = 0;

	// ��Socket����ʲô��������ͨ�ţ�Զ��SHELL���ļ�����ȣ�
	WORD		m_wServiceType = nsGeneralSocket::NULL_SERVICE;

	WORD		m_wSocketType = 0;

	// ��Socket�����Ŀͻ��˵�TOKEN.
	QWORD		m_qwClientToken = 0;

	// ��һ��ClientUnit�ڲ���ÿ��ClientSocket����Ψһ��Socket Tag�����ڱ�ʶClientUnit�ڲ��Ĳ�ͬSocket��
	DWORD		m_dwSocketTag = 0;

	// ��Socket��Tag�������ļ�����������г��ļ��б��socket�ĸ�socket��Tag
	DWORD		m_dwParentSocketTag = 0;

	CEncrypt*	m_pEnc = nullptr;
	CDecrypt*	m_pDec = nullptr;

	CDialogEx* m_pServiceDialog = nullptr;
	LPVOID	   m_pServiceObject = nullptr;

public:
	CClientSocket(QWORD qwClientToken, WORD wServiceType, WORD wSocketType, CONNID dwConnID, DWORD dwParentSocketTag);
	~CClientSocket();

	VOID SetCrypto(CRYPTO_ALG iCryptoAlg, PBYTE pbKey, PBYTE pbIv);
	
	// ��������жԻ�����Ӧ����SetServiceDialog�����Ի��������ClientSocket�����������������û�жԻ�����Ӧ����SetServiceObject�������������ClientSocket����������
	VOID SetServiceDialog(CDialogEx* pServiceDialog);

	// ��������жԻ�����Ӧ����SetServiceDialog�����Ի��������ClientSocket�����������������û�жԻ�����Ӧ����SetServiceObject�������������ClientSocket����������
	VOID SetServiceObject(LPVOID pServiceObject);
};