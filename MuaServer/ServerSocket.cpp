#include "pch.h"
#include "ServerSocket.h"
#include "Service.h"
#include "RemoteShellDlg.h"
#include "FileTransferDlg.h"
#include "ImageCaptureDlg.h"
#include "DesktopMonitorDlg.h"


CServerSocket::CServerSocket(TCP_MODE iTcpMode) : CSocket(iTcpMode) {
	;
}


VOID CServerSocket::OnReceiveWithDec(ITcpServer* pSender, CONNID dwConnID, QWORD qwClientToken, WORD wServiceType, WORD wSocketType, int iCommandID, DWORD dwParentSocketTag, MyBuffer mBuffer) {
	DebugPrintRecvParams(dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag, mBuffer);

	CClientSocket* pClientSocket = ((CMuaServerDlg*)(theApp.m_pMainWnd))->m_pClientManager->GetClientSocketByConnId(dwConnID);

	// ��socket
	if (wServiceType == nsGeneralSocket::MAIN_SOCKET_SERVICE) {
		switch(iCommandID) {
		case nsMainSocket::LOGIN_PACKET_C2S:
			nsMainSocket::_LoginPacket_C2S mData = MsgUnpack<nsMainSocket::_LoginPacket_C2S>((PBYTE)mBuffer.ptr(), mBuffer.size());
			
			((CMuaServerDlg*)(theApp.m_pMainWnd))->AddClient2List(qwClientToken, mData);

			SendWithEnc(dwConnID, nsMainSocket::LOGIN_PACKET_S2C);
			break;
		}
	}

	// ������socket
	else {

		// �ŵ������ɹ�
		if (iCommandID == nsGeneralSocket::CHANNEL_SUCCESS_C2S) {
			PostMessage(theApp.m_pMainWnd->m_hWnd, WM_RECV_CHANNEL_SUCCESS_C2S, 0, (LPARAM)pClientSocket);

			// ��Ҫ��������Client����CHANNEL_SUCCESS_S2C����Ϊ������ĶԻ��򴰿���Ҫһ����ʱ������ʼ����
			// ������ٴ��ڶԻ����ʼ�����ٶȣ��ᵼ�¿ͻ��˷������ݰ���ʱ�򣬶Ի�����null�����´���
			//SendWithEnc(dwConnID, nsGeneralSocket::CHANNEL_SUCCESS_S2C);
		}

		// �����Ľ��յ������ݶ�����������
		else {

			switch (wServiceType) {

			// *********************************************************** Զ��SHELL ***********************************************************

			case nsGeneralSocket::REMOTE_SHELL_SERVICE: {
				((CRemoteShellDlg*)(pClientSocket->m_pServiceDialog))->OnReceiveWithDec(pSender, dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag, mBuffer);
				break;
			}

		
			// *********************************************************** �ļ����� ***********************************************************

			case nsGeneralSocket::FILE_TRANSFER_SERVICE: {
				switch (wSocketType) {
				case (nsGeneralSocket::ROOT_SOCKET_WITH_DIALOG_TYPE): {
					((CFileTransferDlg*)(pClientSocket->m_pServiceDialog))->OnReceiveWithDec(pSender, dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag, mBuffer);
					break;
				}
				case (nsGeneralSocket::ROOT_SOCKET_TYPE): {
					((CFileTransfer*)(pClientSocket->m_pServiceObject))->OnReceiveWithDec(pSender, dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag, mBuffer);
					break;
				}

				case (nsFileTransfer::LIST_FILES_SOCKET): {
					CClientUnit* pClientUnit = ((CMuaServerDlg*)(theApp.m_pMainWnd))->m_pClientManager->GetClientUnitByToken(qwClientToken);
					CClientSocket* pParentSocket = pClientUnit->GetClientSocketByTag(pClientSocket->m_dwParentSocketTag);
					if (pParentSocket == nullptr) {
						DebugPrint("[ERROR] LIST_FILES_SOCKET�Ҳ�����Socket���Ͽ���Socket����\n");
						Disconnect(dwConnID);
					}
					else {
						if (pParentSocket->m_pServiceDialog != nullptr) {
							((CFileTransferDlg*)(pParentSocket->m_pServiceDialog))->OnReceiveWithDec(pSender, dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag, mBuffer);
						}
						else {
							((CFileTransfer*)(pParentSocket->m_pServiceObject))->OnReceiveWithDec(pSender, dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag, mBuffer);
						}
					}
					break;
				}

				case (nsFileTransfer::FILE_TRANSFER_SOCKET): {
					CClientUnit* pClientUnit = ((CMuaServerDlg*)(theApp.m_pMainWnd))->m_pClientManager->GetClientUnitByToken(qwClientToken);
					CClientSocket* pParentSocket = pClientUnit->GetClientSocketByTag(pClientSocket->m_dwParentSocketTag);
					if (pParentSocket == nullptr) {
						DebugPrint("[ERROR] FILE_TRANSFER_SOCKET�Ҳ�����Socket���Ͽ���Socket����\n");
						Disconnect(dwConnID);
					}
					else {
						if (pParentSocket->m_pServiceDialog != nullptr) {
							((CFileTransferDlg*)(pParentSocket->m_pServiceDialog))->OnReceiveWithDec(pSender, dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag, mBuffer);
						}
						else {
							((CFileTransfer*)(pParentSocket->m_pServiceObject))->OnReceiveWithDec(pSender, dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag, mBuffer);
						}
					}
					break;
				}
				
				}
				
				break;
			}


			// *********************************************************** ͼ�񲶻� ***********************************************************

			case nsGeneralSocket::IMAGE_CAPTURE_SERVICE: {
				((CImageCaptureDlg*)(pClientSocket->m_pServiceDialog))->OnReceiveWithDec(pSender, dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag, mBuffer);
				break;
			}

														
			// *********************************************************** ������ ***********************************************************

			case nsGeneralSocket::DESKTOP_MONITOR_SERVICE: {
				switch (wSocketType) {
				case (nsGeneralSocket::ROOT_SOCKET_WITH_DIALOG_TYPE): {
					((CDesktopMonitorDlg*)(pClientSocket->m_pServiceDialog))->OnReceiveWithDec(pSender, dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag, mBuffer);
					break;
				}

				case (nsDesktopMonitor::IMAGE_SOCKET): {
					CClientUnit* pClientUnit = ((CMuaServerDlg*)(theApp.m_pMainWnd))->m_pClientManager->GetClientUnitByToken(qwClientToken);
					CClientSocket* pParentSocket = pClientUnit->GetClientSocketByTag(pClientSocket->m_dwParentSocketTag);
					if (pParentSocket == nullptr) {
						DebugPrint("[ERROR] IMAGE_SOCKET�Ҳ�����Socket���Ͽ���Socket����\n");
						Disconnect(dwConnID);
					}
					else {
						((CDesktopMonitorDlg*)(pParentSocket->m_pServiceDialog))->OnReceiveWithDec(pSender, dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag, mBuffer);
					}
					break;
				}

				/*case (nsFileTransfer::FILE_TRANSFER_SOCKET): {
					CClientUnit* pClientUnit = ((CMuaServerDlg*)(theApp.m_pMainWnd))->m_pClientManager->GetClientUnitByToken(qwClientToken);
					CClientSocket* pParentSocket = pClientUnit->GetClientSocketByTag(pClientSocket->m_dwParentSocketTag);
					if (pParentSocket == nullptr) {
						DebugPrint("[ERROR] FILE_TRANSFER_SOCKET�Ҳ�����Socket���Ͽ���Socket����\n");
						Disconnect(dwConnID);
					}
					else {
						if (pParentSocket->m_pServiceDialog != nullptr) {
							((CFileTransferDlg*)(pParentSocket->m_pServiceDialog))->OnReceiveWithDec(pSender, dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag, sData);
						}
						else {
							((CFileTransfer*)(pParentSocket->m_pServiceObject))->OnReceiveWithDec(pSender, dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, dwParentSocketTag, sData);
						}
					}
					break;
				}*/

				}

				break;
			}
			}

		}
	}
}