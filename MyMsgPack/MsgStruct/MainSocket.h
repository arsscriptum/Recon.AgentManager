#pragma once

#include "../Include/msgpack.hpp"
#include "../../MyCrypto/MyCrypto.h"

#ifndef QWORD
#define QWORD unsigned long long
#endif


// Ϊ��δ�����Լ���Linuxϵͳ��Client���漰�ַ��������ݲ�Ҫʹ��Unicode���д��䣬�����string��

namespace nsMainSocket {

	enum CHILD_SOCKET_TYPE {
		NOPE
	};

	// �����
	enum COMMAND_ID {
		LOGIN_PACKET_C2S = 0x02000000,
		LOGIN_PACKET_S2C
	};
	
	struct _LoginPacket_C2S {
		string						sHostName;		// ������
		string						sOsVersion;		// ϵͳ�汾
		string						sCpuType;		// CPU����
		string						sMemoryInfo;	// �ڴ���Ϣ
		DWORD						dwCameraNum;	// ����ͷ����
		MSGPACK_DEFINE(sHostName, sOsVersion, sCpuType, sMemoryInfo, dwCameraNum)
	};
}