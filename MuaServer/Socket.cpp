#include "pch.h"
#include "Socket.h"

#include "MuaServer.h"				// theApp
#include "MuaServerDlg.h"			// ClientManager

#include <inttypes.h>				// PRIx64


// ��ȡ�ͻ��˹�����
CClientManager* GetClientManager()
{
	return ((CMuaServerDlg*)(theApp.m_pMainWnd))->m_pClientManager;
}


CSocket::CSocket(TCP_MODE iTcpMode) : m_pTcpPackServer(this), m_pTcpPushServer(this){
	m_iTcpMode = iTcpMode;
}


CSocket::~CSocket() {
	StopSocket();
}


VOID CSocket::InitSocket(LPWSTR pwszAddress, WORD wPort) {
	wcscpy_s(m_wszAddress, 32, pwszAddress);
	m_wPort = wPort;

	if (m_iTcpMode == TCP_PACK) {
		m_pTcpPackServer->SetLocalAddress(m_wszAddress, m_wPort);
	}
	else if (m_iTcpMode == TCP_PUSH) {
		m_pTcpPushServer->SetLocalAddress(m_wszAddress, m_wPort);
	}

	if (m_iTcpMode == TCP_PACK) {
		m_pTcpPackServer->InitSocket();
	}
	else if (m_iTcpMode == TCP_PUSH) {
		m_pTcpPushServer->InitSocket();
	}
}


BOOL CSocket::StartSocket() {
	BOOL bRet = FALSE;
	if (m_iTcpMode == TCP_PACK) {
		bRet = m_pTcpPackServer->StartSocket();
	}
	else if (m_iTcpMode == TCP_PUSH) {
		bRet = m_pTcpPushServer->StartSocket();
	}
	return bRet;
}


BOOL CSocket::StopSocket() {
	BOOL bRet = FALSE;
	if (m_iTcpMode == TCP_PACK) {
		bRet = m_pTcpPackServer->StopSocket();
	}
	else if (m_iTcpMode == TCP_PUSH) {
		bRet = m_pTcpPushServer->StopSocket();
	}
	return bRet;
}


BOOL CSocket::Disconnect(CONNID dwConnID)
{
	// �ȼ���Ƿ���
	BOOL bAlive = TRUE;
	if (m_iTcpMode == TCP_PACK) {
		bAlive = m_pTcpPackServer->m_pTcpPackServer->IsConnected(dwConnID);
	}
	else if (m_iTcpMode == TCP_PUSH) {
		bAlive = m_pTcpPushServer->m_pTcpPushServer->IsConnected(dwConnID);
	}
	
	// ���ԭ����û���򷵻��棬��ʾ�����ѶϿ�
	if (!bAlive) {
		return TRUE;
	}

	// �������Ͽ�����
	BOOL bRet = FALSE;
	if (m_iTcpMode == TCP_PACK) {
		bRet = m_pTcpPackServer->m_pTcpPackServer->Disconnect(dwConnID);
	}
	else if (m_iTcpMode == TCP_PUSH) {
		bRet = m_pTcpPushServer->m_pTcpPushServer->Disconnect(dwConnID);
	}
	return bRet;
}


BOOL CSocket::Send(CONNID dwConnID, PBYTE pBuffer, DWORD dwBufLen) {
	BOOL bRet = FALSE;
	if (m_iTcpMode == TCP_PACK) {
		bRet = m_pTcpPackServer->Send(dwConnID, pBuffer, dwBufLen);
	}
	else if (m_iTcpMode == TCP_PUSH) {
		bRet = m_pTcpPushServer->Send(dwConnID, pBuffer, dwBufLen);
	}
	return bRet;
}


// ���ܲ����ͣ����pBuffer = nullptr, dwBufLen = 0���Ǿ��൱��������ֻ���а�ͷ�������а��壩
BOOL CSocket::SendWithEnc(CONNID dwConnID, int iCommandID, PBYTE pBuffer, DWORD dwBufLen) {
	// �ͻ��˹�����
	CClientManager* pClientManager = GetClientManager();

	// �ҵ�ConnID��Ӧ��ClientSocket
	CClientSocket* pClientSocket = pClientManager->GetClientSocketByConnId(dwConnID);
	if (pClientSocket == nullptr) {
		return FALSE;
	}

	// pBuffer�ǰ��壬�˴���װ�ϰ�ͷ
	nsGeneralSocket::_GeneralDataPacket mData;
	mData.qwClientToken = pClientSocket->m_qwClientToken;
	mData.wServiceType = pClientSocket->m_wServiceType;
	mData.wSocketType = pClientSocket->m_wSocketType;
	mData.iCommandID = iCommandID;
	mData.dwParentSocketTag = pClientSocket->m_dwParentSocketTag;
	mData.msData.ptr = (PCHAR)pBuffer;	
	mData.msData.size = dwBufLen;

	MyBuffer mBuffer2 = MsgPack<nsGeneralSocket::_GeneralDataPacket>(mData);

	// �������ݲ�����
	string sCipherText = pClientSocket->m_pEnc->Encrypt(mBuffer2.ptr(), mBuffer2.size());

	BOOL bRet = FALSE;
	if (m_iTcpMode == TCP_PACK) {
		bRet = m_pTcpPackServer->Send(dwConnID, (PBYTE)sCipherText.c_str(), sCipherText.length());
	}
	else if (m_iTcpMode == TCP_PUSH) {
		bRet = m_pTcpPushServer->Send(dwConnID, (PBYTE)sCipherText.c_str(), sCipherText.length());
	}
	return bRet;
}


// ���dwParentSocketTag = 0�Ļ�������Ҫ���ô˺�����ֱ�ӵ���SendWithEnc����
BOOL CSocket::SendCreateChildSocket(CONNID dwConnID, int iCommandID, DWORD dwParentSocketTag) {
	// �ͻ��˹�����
	CClientManager* pClientManager = GetClientManager();

	// �ҵ�ConnID��Ӧ��ClientSocket
	CClientSocket* pClientSocket = pClientManager->GetClientSocketByConnId(dwConnID);
	if (pClientSocket == nullptr) {
		return FALSE;
	}

	nsGeneralSocket::_CreateChildSocket mData;
	mData.dwParentSocketTag = dwParentSocketTag;

	MyBuffer mBuffer = MsgPack<nsGeneralSocket::_CreateChildSocket>(mData);
	BOOL bRet = SendWithEnc(dwConnID, iCommandID, mBuffer.ptr(), mBuffer.size());

	return bRet;
}




VOID CSocket::DebugPrintRecvParams(CONNID dwConnID, QWORD qwClientToken, WORD wServiceType, WORD wSocketType, int iCommandID, DWORD dwParentSocketTag, MyBuffer mBuffer) {
	if (mBuffer.size() != 0) {
		DebugPrint("[Client %d] OnReceiveWithDec: TOKEN: 0x%" PRIx64 ", SERVICE_TYPE: 0x%x, SOCKET_TYPE: 0x%x, COMMAND_ID: 0x%x, PARENT_SOCKET_TAG: 0x%x, ����:\n\n", dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag);
		PrintData((PBYTE)mBuffer.ptr(), mBuffer.size());
	}
	else {
		DebugPrint("[Client %d] OnReceiveWithDec: TOKEN: 0x%" PRIx64 ", SERVICE_TYPE: 0x%x, SOCKET_TYPE: 0x%x, COMMAND_ID: 0x%x, PARENT_SOCKET_TAG: 0x%x, ����Ϊ��\n\n", dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag);
	}
}


BOOL _IsNewClientSocket(CONNID dwConnID, const BYTE* pData, int iLength) {
	BOOL bRet = FALSE;
	nsGeneralSocket::_SetCryptoAlg_C2S mData = MsgUnpack<nsGeneralSocket::_SetCryptoAlg_C2S>((PBYTE)pData, iLength);

	bRet = (
		(mData.sCipherText0.length() == iRsaKeyBits / 8) &&
		(mData.sCipherText1.length() == iRsaKeyBits / 8) &&
		(mData.sCipherText2.length() == iRsaKeyBits / 8) &&
		(mData.sCipherText3.length() == iRsaKeyBits / 8) &&
		(mData.sCipherText4.length() == iRsaKeyBits / 8)
		);

	if (bRet) {
		string sDataPart1 = RsaDecryptUsePrivateKey(sRsaPrivateKey, mData.sCipherText0);
		nsGeneralSocket::_SetCryptoAlg_C2S_Part1 mDataPart1 = MsgUnpack<nsGeneralSocket::_SetCryptoAlg_C2S_Part1>((PBYTE)sDataPart1.c_str(), sDataPart1.length());
		bRet = (mDataPart1.iCommandID == nsGeneralSocket::SET_CRYPTP_ALG_C2S);
	}

	return bRet;
}

// �жϴ�Socket�Ƿ�Ϊ�µ�ClientSocket
// (���֪����IP:PORT, ���TCP˭������������ֻ����ѭ����MUA��ͨ��Э��Ĳ���ClientSocket)
BOOL CSocket::IsNewClientSocket(CONNID dwConnID, const BYTE* pData, int iLength)
{
	BOOL bRet = FALSE;

	// �����������ݷ����л���_SetCryptoAlg_C2S��ʽ����������5�����ĳ��ȶ���RSA��Կ���ȵ����ݰ���������ȷ�Ĵ���Key��IV�����ݰ�
	__try {
		bRet = _IsNewClientSocket(dwConnID, pData, iLength);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		bRet = FALSE;
	}

	return bRet;
}


// ��ʼ������µ�ClientSocket
BOOL CSocket::InitNewClientSocket(CONNID dwConnID, const BYTE* pData, int iLength)
{	
	nsGeneralSocket::_SetCryptoAlg_C2S mData = MsgUnpack<nsGeneralSocket::_SetCryptoAlg_C2S>((PBYTE)pData, iLength);

	string sKey1, sKey2, sIv1, sIv2;
	sKey1 = RsaDecryptUsePrivateKey(sRsaPrivateKey, mData.sCipherText1);
	sKey2 = RsaDecryptUsePrivateKey(sRsaPrivateKey, mData.sCipherText2);
	sIv1 = RsaDecryptUsePrivateKey(sRsaPrivateKey, mData.sCipherText3);
	sIv2 = RsaDecryptUsePrivateKey(sRsaPrivateKey, mData.sCipherText4);

	string sKey = sKey1 + sKey2;
	string sIv = sIv1 + sIv2;

	string sDataPart1 = RsaDecryptUsePrivateKey(sRsaPrivateKey, mData.sCipherText0);
	nsGeneralSocket::_SetCryptoAlg_C2S_Part1 mDataPart1 = MsgUnpack<nsGeneralSocket::_SetCryptoAlg_C2S_Part1>((PBYTE)sDataPart1.c_str(), sDataPart1.length());


	CClientUnit* pClientUnit;
	CClientSocket* pClientSocket;

	// ������ClientSocket���µģ�������ClientUnitҲ���µ�
	if (mDataPart1.qwClientToken == 0) {
		// �����µ�ClientUnit��������ClientUnit¼��ClientManager��map��
		pClientUnit = new CClientUnit(dwConnID);
		GetClientManager()->AddClientUtil(pClientUnit->m_qwClientToken, pClientUnit);
		DebugPrint("[Client %d] �����µ�ClientUnit, CLIENT_TOKEN: 0x%" PRIx64 "\n", dwConnID, pClientUnit->m_qwClientToken);

		// �����µ�ClientSocket��������ClientSocket�ֱ�¼��ClientManager��������ClientUnit��map��
		pClientSocket = new CClientSocket(pClientUnit->m_qwClientToken, mDataPart1.wServiceType, mDataPart1.wSocketType, dwConnID, mDataPart1.dwParentSocketTag);
		GetClientManager()->AddClientSocket(dwConnID, pClientSocket);
		pClientUnit->AddClientSocket(dwConnID, pClientSocket);
		DebugPrint("[Client %d] �����µ�ClientSocket, CONNID: %d\n", dwConnID, dwConnID);
	}

	// ��ClientSocket���µģ���������ClientUnit�Ѵ���
	else {
		// ��ȡ��ClientSocket������ClientUnit
		pClientUnit = GetClientManager()->GetClientUnitByToken(mDataPart1.qwClientToken);

		if (pClientUnit == nullptr) {
			DebugPrint("[Client %d] �ͻ���α��TOKEN���й������ж�����", dwConnID);
			Disconnect(dwConnID);
			return FALSE;
		}

		// �����µ�ClientSocket��������ClientSocket�ֱ�¼��ClientManager��������ClientUnit��map��
		pClientSocket = new CClientSocket(pClientUnit->m_qwClientToken, mDataPart1.wServiceType, mDataPart1.wSocketType, dwConnID, mDataPart1.dwParentSocketTag);
		GetClientManager()->AddClientSocket(dwConnID, pClientSocket);
		pClientUnit->AddClientSocket(dwConnID, pClientSocket);
		DebugPrint("[Client %d] �����µ�ClientSocket, CONNID: %d\n", dwConnID, dwConnID);
	}

	// ��Client����SET_CRYPTO_ALG���ݰ�
	nsGeneralSocket::_SetCryptoAlg_S2C mData2;
	mData2.qwClientToken = pClientUnit->m_qwClientToken;
	mData2.wServiceType = pClientSocket->m_wServiceType;
	mData2.wSocketType = pClientSocket->m_wSocketType;
	mData2.dwParentSocketTag = pClientSocket->m_dwParentSocketTag;
	mData2.iCommandID = nsGeneralSocket::SET_CRYPTP_ALG_S2C;
	mData2.iCryptoAlg = nsGeneralSocket::GetCryptoAlgByServiceType((nsGeneralSocket::SERVICE_TYPE)mDataPart1.wServiceType);

	MyBuffer mBuffer = MsgPack<nsGeneralSocket::_SetCryptoAlg_S2C>(mData2);
	//string sBuffer = Bytes2Str(pBuffer, dwBufLen);
	//delete[] pBuffer;

	string sCipherTextReturn = RsaEncryptUsePrivateKey(sRsaPrivateKey, Bytes2Str(mBuffer.ptr(), mBuffer.size()));
	Send(dwConnID, (PBYTE)sCipherTextReturn.c_str(), sCipherTextReturn.length());

	// ���ñ�Socket��ͨ��Key��IV
	pClientSocket->SetCrypto((CRYPTO_ALG)mData2.iCryptoAlg, (PBYTE)sKey.c_str(), (PBYTE)sIv.c_str());

	return TRUE;
}


EnHandleResult CSocket::OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength) {
	DebugPrint("[Client %d] OnReceive: %d Bytes \n", dwConnID, iLength);
	
	// �ҵ�ConnID��Ӧ��ClientSocket
	CClientSocket* pClientSocket = GetClientManager()->GetClientSocketByConnId(dwConnID);

	// �µ�Socket
	if (pClientSocket == nullptr) {

		// �ж�����µ�Socket�Ƿ���ClientSocket(���֪����IP:PORT, ���TCP˭������������ֻ����ѭ����MUA��ͨ��Э��Ĳ���ClientSocket)
		if (IsNewClientSocket(dwConnID, pData, iLength)) {
			// ��ʼ������µ�ClientSocket
			InitNewClientSocket(dwConnID, pData, iLength);
		}

		// ����ClientSocket���Ͽ�����
		else {
			DebugPrint("[Client %d] �����ϱ���Ŀ��ͨ��Э�飬�Ͽ�����\n", dwConnID);
			Disconnect(dwConnID);
		}
	}

	// �����µ�Socket����������
	else {
		string sPlainText = pClientSocket->m_pDec->Decrypt((PBYTE)pData, iLength);

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


EnHandleResult CSocket::OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode){
	DebugPrint("[Client %d] OnClose: \n", dwConnID);

	CClientSocket* pClientSocket = GetClientManager()->GetClientSocketByConnId(dwConnID);
	
	// �������DeleteClientSocket�жϿ���socket���ӣ���GetClientSocketByConnId��Ϊnullptr���Ͳ����������ٴδ����ˣ���Ȼ������ѭ�������ˡ�
	if (pClientSocket != nullptr) {
		CClientUnit* pClientUnit = GetClientManager()->GetClientUnitByToken(pClientSocket->m_qwClientToken);

		// ��ǰ�Ͽ���socket������Client����socket����ɾ������ClientUnit
		if (pClientUnit->m_dwMainSocketConnID == dwConnID) {
			GetClientManager()->DeleteClientUnit(pClientUnit);
		}

		// ��ǰ�Ͽ���socket���Ӳ�����socket����ֻɾ�����ClientSocket
		else {
			GetClientManager()->DeleteClientSocket(pClientSocket);
		}
	}

	return HR_OK;
}