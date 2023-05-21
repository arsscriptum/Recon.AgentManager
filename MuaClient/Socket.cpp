#include "pch.h"
#include "Socket.h"


CSocket::CSocket(TCP_MODE iTcpMode, nsGeneralSocket::SERVICE_TYPE wServiceType, WORD wSocketType, QWORD qwClientToken, DWORD dwParentSocketTag) : m_pTcpPackClient(this), m_pTcpPushClient(this) {
	m_iTcpMode = iTcpMode;
	m_wServiceType = wServiceType;
	m_wSocketType = wSocketType;
	m_qwClientToken = qwClientToken;
	m_dwParentSocketTag = dwParentSocketTag;

	if (m_iTcpMode == TCP_PACK) {
		m_pTcpPushClient = nullptr;
	}
	else if (m_iTcpMode == TCP_PUSH) {
		m_pTcpPackClient = nullptr;
	}
}


CSocket::~CSocket() {
	if (m_pEnc != nullptr) {
		delete m_pEnc;
		m_pEnc = nullptr;
	}
	if (m_pDec != nullptr) {
		delete m_pDec;
		m_pDec = nullptr;
	}
}


VOID CSocket::InitSocket(LPWSTR pwszAddress, WORD wPort) {
	wcscpy_s(m_wszAddress, pwszAddress);
	m_wPort = wPort;

	if (m_iTcpMode == TCP_PACK) {
		m_pTcpPackClient->SetRemoteAddress(m_wszAddress, m_wPort);
	}else if(m_iTcpMode == TCP_PUSH) {
		m_pTcpPushClient->SetRemoteAddress(m_wszAddress, m_wPort);
	}

	if (m_iTcpMode == TCP_PACK) {
		m_pTcpPackClient->InitSocket();
	}
	else if (m_iTcpMode == TCP_PUSH) {
		m_pTcpPushClient->InitSocket();
	}
}


BOOL CSocket::StartSocket() {
	BOOL bRet = FALSE;
	if (m_iTcpMode == TCP_PACK) {
		bRet = m_pTcpPackClient->StartSocket();
	}
	else if (m_iTcpMode == TCP_PUSH) {
		bRet = m_pTcpPushClient->StartSocket();
	}

	if (bRet) {
		// ����Key��Iv�����͸�Server
		bRet = CreateSendKeyIv();
	}

	return bRet;
}


BOOL CSocket::StopSocket() {
	BOOL bRet = FALSE;
	if (m_iTcpMode == TCP_PACK) {
		bRet = m_pTcpPackClient->StopSocket();
	}
	else if (m_iTcpMode == TCP_PUSH) {
		bRet = m_pTcpPushClient->StopSocket();
	}
	return bRet;
}


VOID CSocket::SetCryptoAlg(CRYPTO_ALG iCryptoAlg) {
	m_pEnc = new CEncrypt();
	m_pEnc->Init(iCryptoAlg, m_pbKey, m_pbIv);

	m_pDec = new CDecrypt();
	m_pDec->Init(iCryptoAlg, m_pbKey, m_pbIv);
}


BOOL CSocket::Send(PBYTE pBuffer, DWORD dwBufLen) {
	BOOL bRet = FALSE;
	if (m_iTcpMode == TCP_PACK) {
		bRet = m_pTcpPackClient->Send(pBuffer, dwBufLen);
	}
	else if (m_iTcpMode == TCP_PUSH) {
		bRet = m_pTcpPushClient->Send(pBuffer, dwBufLen);
	}
	return bRet;
}


// ���ܲ�����
BOOL CSocket::SendWithEnc(int iCommandID, PBYTE pBuffer, DWORD dwBufLen) {
	nsGeneralSocket::_GeneralDataPacket mData;
	mData.qwClientToken = m_qwClientToken;
	mData.wServiceType = m_wServiceType;
	mData.wSocketType = m_wSocketType;
	mData.iCommandID = iCommandID;
	mData.dwParentSocketTag = m_dwParentSocketTag;
	mData.msData.ptr = (PCHAR)pBuffer;
	mData.msData.size = dwBufLen;
	
	MyBuffer mBuffer2 = MsgPack<nsGeneralSocket::_GeneralDataPacket>(mData);

	BOOL bRet = FALSE;
	string sCipherText = m_pEnc->Encrypt(mBuffer2.ptr(), mBuffer2.size());
	if (m_iTcpMode == TCP_PACK) {
		bRet = m_pTcpPackClient->Send((PBYTE)sCipherText.c_str(), sCipherText.length());
	}
	else if (m_iTcpMode == TCP_PUSH) {
		bRet = m_pTcpPushClient->Send((PBYTE)sCipherText.c_str(), sCipherText.length());
	}

	return bRet;
}


BOOL CSocket::IsConnected() {
	if (m_iTcpMode == TCP_PACK) {
		return m_pTcpPackClient->IsConnected();
	}
	else if (m_iTcpMode == TCP_PUSH) {
		return m_pTcpPushClient->IsConnected();
	}
}


VOID CSocket::DebugPrintRecvParams(CONNID dwConnID, QWORD qwToken, WORD wServiceType, WORD wSocketType, int iCommandID, DWORD dwParentSocketTag, MyBuffer mBuffer) {
	if (mBuffer.size() != 0) {
		DebugPrint("[Client %d] OnReceiveWithDec: TOKEN: 0x%" PRIx64 ", SERVICE_TYPE: 0x%x, SOCKET_TYPE: 0x%x, COMMAND_ID: 0x%x, PARENT_SOCKET_TAG: 0x%x, ����:\n\n", dwConnID, qwToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag);
		PrintData(mBuffer.ptr(), mBuffer.size());
	}
	else {
		DebugPrint("[Client %d] OnReceiveWithDec: TOKEN: 0x%" PRIx64 ", SERVICE_TYPE: 0x%x, SOCKET_TYPE: 0x%x, COMMAND_ID: 0x%x, PARENT_SOCKET_TAG: 0x%x, ����Ϊ��\n\n", dwConnID, qwToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag);
	}
}


EnHandleResult CSocket::OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength) {
	DebugPrint("[Client %d] OnReceive: %d Bytes \n", dwConnID, iLength);

	// ���յĵ�һ�����ݰ�����ʹ��RSA���ܳ�Serverָ�ɵı�Socket�������õ������㷨
	if (!m_bRecvFirstPacketReturn) {							
		string sCipherText = Bytes2Str((PBYTE)pData, iLength);
		string sPlainText = RsaDecryptUsePublicKey(sRsaPublicKey, sCipherText);
		
		nsGeneralSocket::_SetCryptoAlg_S2C mData = MsgUnpack<nsGeneralSocket::_SetCryptoAlg_S2C>((PBYTE)sPlainText.c_str(), sPlainText.length());
		if (mData.iCommandID == nsGeneralSocket::SET_CRYPTP_ALG_S2C) {

			// ��ʼ�������㷨
			SetCryptoAlg((CRYPTO_ALG)mData.iCryptoAlg);
			m_bRecvFirstPacketReturn = TRUE;

			// ��Socket����Socket����Ҫ��Server�������߰�
			if (m_bIsMainClientSocket) {
				m_qwClientToken = mData.qwClientToken;
				
				SendLoginPacket();
			}
			// ��Socket������Socket����Server����CHANNEL_SUCCESS����ʾ���ŵ����԰�CryptoAlg�㷨�������������
			else {
				SendWithEnc(nsGeneralSocket::CHANNEL_SUCCESS_C2S);
			}
		}
	}

	// ���ǽ��յĵ�һ�����ݰ������������ܺ󽻸���OnReceiveWithDec����һ������
	else {														
		string sPlainText = m_pDec->Decrypt((PBYTE)pData, iLength);

		if (sPlainText == "") {
			DebugPrint("[Client %d] ���ܽ��յ�������ʱ�����쳣\n", dwConnID);
		}
		else {
			nsGeneralSocket::_GeneralDataPacket mData = MsgUnpack<nsGeneralSocket::_GeneralDataPacket>((PBYTE)sPlainText.c_str(), sPlainText.length());
			MyBuffer mBuffer = MyBuffer((PBYTE)mData.msData.ptr, mData.msData.size);
			OnReceiveWithDec(pSender, dwConnID, mData.qwClientToken, mData.wServiceType, mData.wSocketType, mData.iCommandID, mData.dwParentSocketTag, mBuffer);
		}
	}

	
	return HR_OK;
}


BOOL CSocket::CreateSendKeyIv() {
	// ���л�Token���������͡������
	nsGeneralSocket::_SetCryptoAlg_C2S_Part1 mDataPart1;
	mDataPart1.qwClientToken = m_qwClientToken;			// MAIN_SOCKET_SERVICE����Ϊ0
	mDataPart1.wServiceType = m_wServiceType;
	mDataPart1.wSocketType = m_wSocketType;
	mDataPart1.iCommandID = nsGeneralSocket::SET_CRYPTP_ALG_C2S;
	mDataPart1.dwParentSocketTag = m_dwParentSocketTag;
	MyBuffer mBufferPart1 = MsgPack<nsGeneralSocket::_SetCryptoAlg_C2S_Part1>(mDataPart1);

	// �������key��iv
	AutoSeededRandomPool  rng;
	rng.GenerateBlock(m_pbKey, MAX_KEY_LEN);
	rng.GenerateBlock(m_pbIv, MAX_IV_LEN);

	// 3072bit��RSA��֧��384B���ݵļӽ���
	// ����384B������Ҫ������ֽڡ�
	// ����512B��key��iv��Ҫ�ȷָ��256B, �ټ���
	string sKey1, sKey2, sIv1, sIv2;
	sKey1 = Bytes2Str(m_pbKey, MAX_KEY_LEN / 2);
	sKey2 = Bytes2Str(m_pbKey, MAX_KEY_LEN / 2, MAX_KEY_LEN / 2);
	sIv1 = Bytes2Str(m_pbIv, MAX_IV_LEN / 2);
	sIv2 = Bytes2Str(m_pbIv, MAX_IV_LEN / 2, MAX_IV_LEN / 2);
	
	// RSA����
	string sCipherText0 = RsaEncryptUsePublicKey(sRsaPublicKey, Bytes2Str(mBufferPart1.ptr(), mBufferPart1.size()));
	string sCipherText1 = RsaEncryptUsePublicKey(sRsaPublicKey, sKey1);
	string sCipherText2 = RsaEncryptUsePublicKey(sRsaPublicKey, sKey2);
	string sCipherText3 = RsaEncryptUsePublicKey(sRsaPublicKey, sIv1);
	string sCipherText4 = RsaEncryptUsePublicKey(sRsaPublicKey, sIv2);

	// ���л�����	
	nsGeneralSocket::_SetCryptoAlg_C2S mData;
	mData.sCipherText0 = sCipherText0;
	mData.sCipherText1 = sCipherText1;
	mData.sCipherText2 = sCipherText2;
	mData.sCipherText3 = sCipherText3;
	mData.sCipherText4 = sCipherText4;

	MyBuffer mBuffer = MsgPack<nsGeneralSocket::_SetCryptoAlg_C2S>(mData);

	// ����
	BOOL bRet = Send(mBuffer.ptr(), mBuffer.size());
	
	return bRet;
}


// �麯����ֻ��MainSocket��Ҫ������������������ķ���Socket���մ˺������ɡ�
VOID CSocket::SendLoginPacket(){
	return;
}


VOID CSocket::SetParentSocketTag(DWORD dwParentSocketTag) {
	m_dwParentSocketTag = dwParentSocketTag;
}