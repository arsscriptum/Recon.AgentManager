#pragma once

#include "../Include/msgpack.hpp"
#include "../../MyCrypto/MyCrypto.h"

#ifndef QWORD
#define QWORD unsigned long long
#endif


namespace nsImageCapture {

	enum CHILD_SOCKET_TYPE {
		NOPE
	};

	// �����
	enum COMMAND_ID {
		IMAGE_CAPTURE_S2C = 0x05000000,
		IMAGE_CAPTURE_C2S,

		PRINT_SCREEN_S2C,		// ��Ļ��ͼ
		PRINT_SCREEN_C2S,

		CAMERA_PHOTO_S2C,		// �������
		CAMERA_PHOTO_C2S
	};


	struct _ImageSection_C2S {
		DWORD							dwIndex;			// ��i����Ƭ�Ĵ���Ϊi�������һ����Ƭ����Ϊ0
		msgpack::type::raw_ref			msImageSection;		// ͼ����Ƭ
		MSGPACK_DEFINE(dwIndex, msImageSection)
	};

	
	//struct _PrintScreen_C2S {
	//	msgpack::type::raw_ref			msImage;
	//	MSGPACK_DEFINE(msImage)
	//};


	//struct _CameraPhoto_C2S {
	//	msgpack::type::raw_ref			msImage;
	//	MSGPACK_DEFINE(msImage)
	//};
}