#pragma once

#include "../Include/msgpack.hpp"
#include "../../MyCrypto/MyCrypto.h"

#ifndef QWORD
#define QWORD unsigned long long
#endif


namespace nsDesktopMonitor {

	enum CHILD_SOCKET_TYPE {
		IMAGE_SOCKET = 0x600,
		AUDIO_SOCKET,
	};

	// �����
	enum COMMAND_ID {
		DESKTOP_MONITOR_S2C = 0x06000000,
		DESKTOP_MONITOR_C2S,
	
		LIST_SCREEN_S2C,
		LIST_SCREEN_C2S,


		// *************** ͼ�� ***************

		CREATE_IMAGE_SOCKET_S2C,
		CREATE_IMAGE_SOCKET_C2S,
		
		DISPLAY_SCREEN_S2C,
		DISPLAY_SCREEN_C2S,
		DISPLAY_SCREEN_FAILED_C2S,


		// *************** ��Ƶ ***************

		CREATE_AUDIO_SOCKET_S2C,
		CREATE_AUDIO_SOCKET_C2S,


		
	};




	struct _ScreenUnit_WIN_C2S {
		string							sScreenName;
		DWORD							dwWidth;
		DWORD							dwHeight;
		DWORD							dwX;
		DWORD							dwY;
		BOOL							bIsMainScreen;
		MSGPACK_DEFINE(sScreenName, dwWidth, dwHeight, dwX, dwY, bIsMainScreen)
	};


	struct _ListScreen_C2S {
		DWORD							dwScreenNum;
		msgpack::type::raw_ref			msData;				// ��� _ScreenUnit
		MSGPACK_DEFINE(dwScreenNum, msData)
	};




	struct _DisplayScreen_S2C {
		string							sScreenName;
		MSGPACK_DEFINE(sScreenName)
	};


	struct _DisplayScreen_C2S {
		QWORD							qwTime;				// ���뼶ʱ���
		DWORD							dwImageSize;		// ��Ļ��ͼ�Ĵ�С
		DWORD							dwSectionNum;		// ��Ƭ��
		DWORD							dwSectionIndex;		// ��i����Ƭ�Ĵ���Ϊi�������һ����Ƭ����Ϊ0
		msgpack::type::raw_ref			msImageSection;		// ͼ����Ƭ����
		MSGPACK_DEFINE(qwTime, dwImageSize, dwSectionNum, dwSectionIndex, msImageSection)
	};

	struct _DisplayScreenFailed_C2S {
		string							sErrorInfo;
		MSGPACK_DEFINE(sErrorInfo)
	};

}