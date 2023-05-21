#include "pch.h"
#include "MainSocket.h"


typedef struct _REBORN_THREAD_PARAM {
	WCHAR m_pwszAddress[32];
	WORD m_wPort;
	_REBORN_THREAD_PARAM(wchar_t* pwszAddress, wchar_t* pwszPort) {
		wcscpy_s(m_pwszAddress, pwszAddress);
		char pszPort[10];
		WideCharToMultiByte(CP_ACP, NULL, pwszPort, -1, pszPort, 10, NULL, NULL);
		m_wPort = atoi(pszPort);
	}
}REBORN_THREAD_PARAM;


void WINAPI StartClientThreadFunc(REBORN_THREAD_PARAM* pThreadParam);
CMainSocket* StartClientFuncBody(CMainSocket* pMainSocket, LPWSTR pszAddress, WORD wPort);


// ��VS���ԵĻ����� "MuaClient -> �������� -> ���� -> �������" ���޸�IP��ַ�Ͷ˿�
int wmain(int argc, wchar_t* argv[]) {
	if (argc < 3)
	{
		CHAR pszPath[MAX_PATH];
		WideCharToMultiByte(CP_ACP, NULL, argv[0], -1, pszPath, MAX_PATH, NULL, NULL);
		printf("Usage:\n %s <Host> <Port>\n", pszPath);
		system("pause");
		return -1;
	}

	REBORN_THREAD_PARAM* pThreadParam = new REBORN_THREAD_PARAM(argv[1], argv[2]);
	HANDLE hRebornThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartClientThreadFunc, pThreadParam, 0, NULL);

	// ֮ǰhRebornThread���������new CSocketClient()һֱ����ֵ�ʧ�ܣ��һ���Ϊ��CSocketClient�Ĺ��캯�������⣬
	// û�뵽ֻ���ҵ����߳��˳��˶��ѡ�С�������Լ���
	WaitForSingleObject(hRebornThread, INFINITE);
}


void WINAPI StartClientThreadFunc(REBORN_THREAD_PARAM* pThreadParam) {
	WCHAR wszAddress[32];
	wcscpy_s(wszAddress, pThreadParam->m_pwszAddress);		// TODO ��� pThreadParam->m_pwszAddress ����
	WORD wPort = pThreadParam->m_wPort;
	delete pThreadParam;

	CMainSocket* pMainSocket = nullptr;
	while (true) {
		__try {
			pMainSocket = StartClientFuncBody(pMainSocket, wszAddress, wPort);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			pMainSocket = nullptr;
			DebugPrint("�����쳣�����������\n");
		}
	}
}


// SEH���ں��������ж���չ�������Ե����ŵ�һ��������
CMainSocket* StartClientFuncBody(CMainSocket* pMainSocketTemp, LPWSTR pwszAddress, WORD wPort) {
	CMainSocket* pMainSocket = pMainSocketTemp;

	if (pMainSocket == nullptr) {
		pMainSocket = new CMainSocket(TCP_PACK);
		pMainSocket->InitSocket(pwszAddress, wPort);
		pMainSocket->StartSocket();
	}

	// δ����ʱ�½�һ������
	if (!pMainSocket->IsConnected()) {
		delete pMainSocket;
		pMainSocket = new CMainSocket(TCP_PACK);
		pMainSocket->InitSocket(pwszAddress, wPort);
		DebugPrint("�������������.....\n");
		pMainSocket->StartSocket();
	}

	// 5������һ��
	Sleep(5000);

	return pMainSocket;
}

