#pragma once

#include "../Include/msgpack.hpp"
#include "../../MyCrypto/MyCrypto.h"

#ifndef QWORD
#define QWORD unsigned long long
#endif


namespace nsFileTransfer {

	#define FILES_NUM_PER_GROUP 100									// һ��Ŀ¼�µ��ļ���ϢҪ��n�δ��䣬��ֵ��ʾ�����ļ���ϢΪһ��

	#define FILE_SECTION_MAX_LENGTH (PACKET_MAX_LENGTH - 512)		// �ļ���һ����Ƭ�Ĵ�С

	#define FILE_CONCURRENT_DOWNLOAD_NUMBER	3						// �ļ�������������


	enum CHILD_SOCKET_TYPE {
		LIST_FILES_SOCKET = 0x0400,
		FILE_TRANSFER_SOCKET
	};


	// �ļ�����
	enum FILE_TYPE {
		TYPE_UNKNOWN,						// δ֪
		TYPE_POINT,							// ��ָ . �� ..
		TYPE_HARD_DISK,						// Ӳ�̣�����ʾWindowsϵͳ���̷�
		TYPE_FOLDER,						// �ļ���
		TYPE_FILE,							// �ļ�
		TYPE_LINK,							// ����
		// TODO: some type else
	};


	// �����
	enum COMMAND_ID {

		// **************** ��socket ****************

		FILE_TRANSFER_S2C = 0x04000000,
		FILE_TRANSFER_C2S,
		
		FILE_TRANSFER_WITHOUT_DIALOG_S2C,
		FILE_TRANSFER_WITHOUT_DIALOG_C2S,


		// ************** �г�Ӳ��/�ļ� **************

		CREATE_LIST_SOCKET_S2C,
		CREATE_LIST_SOCKET_C2S,
		
		LIST_HARD_DISKS_S2C,
		LIST_HARD_DISKS_C2S,

		LIST_FILES_S2C,
		LIST_FILES_C2S,
		LIST_FILES_FAILED_C2S,


		// ************** �ļ��ϴ����� **************

		CREATE_FILE_SOCKET_S2C,
		CREATE_FILE_SOCKET_C2S,

		FILE_UPLOAD_CMD_S2C,					// �ϴ���ָS2C
		FILE_UPLOAD_CMD_C2S,
		FILE_UPLOAD_C2S,
		FILE_UPLOAD_S2C,
		FILE_UPLOAD_FAILED_C2S,

		FILE_DOWNLOAD_CMD_S2C,					// ������ָC2S
		FILE_DOWNLOAD_CMD_C2S,
		FILE_DOWNLOAD_S2C,
		FILE_DOWNLOAD_C2S,
		FILE_DOWNLOAD_FAILED_C2S,
	};


	// ***************************** �г�Ӳ�� *****************************
	struct _ListHardDisk_WIN_C2S {
		DWORD						dwHardDiskNum;
		msgpack::type::raw_ref		msData;				// �ں���� _HardDiskUnit_WIN_C2S ����
		MSGPACK_DEFINE(dwHardDiskNum, msData)
	};

	struct _HardDiskUnit_WIN_C2S {
		string						sHardDiskName;
		string						sHardDiskLabel;
		QWORD						qwTotalBytes;
		QWORD						qwFreeBytes;
		MSGPACK_DEFINE(sHardDiskName, sHardDiskLabel, qwTotalBytes, qwFreeBytes)
	};


	// ***************************** �г��ļ� *****************************

	struct _ListFiles_S2C {
		string						sPath;
		MSGPACK_DEFINE(sPath)
	};

	struct _ListFiles_WIN_C2S {
		DWORD						dwSectionIndex;		// ÿ���г����ļ����������޵ģ���һ��Ŀ¼Ҫ��װ���_ListFiles_WIN_C2S��dwIndex��1��ʼ���������һ��_ListFiles_WIN_C2S��dwIndex = 0
		DWORD						dwSectionNum;		// �ܹ�Ҫ���ٸ�_ListFiles_WIN_C2S
		DWORD						dwFileNum;
		string						sPath;
		msgpack::type::raw_ref		msData;				// �ں���� _FileUnit_WIN_C2S ����
		MSGPACK_DEFINE(dwSectionIndex, dwSectionNum, dwFileNum, sPath, msData)
	};

	struct _FileUnit_WIN_C2S {
		int							iFileType;
		string						sFileName;
		QWORD						qwFileBytes;
		string						sModifiedDate;
		MSGPACK_DEFINE(iFileType, sFileName, qwFileBytes, sModifiedDate)
	};


	struct _ListFilesFailed_C2S {
		string						sPath;
		string						sError;
		MSGPACK_DEFINE(sPath, sError)
	};


	// ***************************** �ļ����� *****************************

	struct _FileDownloadCmd_S2C {
		DWORD						dwFileTag;
		string						sClientPath;
		BOOL						bHaveFileSize;
		MSGPACK_DEFINE(dwFileTag, sClientPath, bHaveFileSize)
	};


	struct _FileDownloadCmd_C2S {
		DWORD						dwFileTag;
		BOOL						bOpenFile;
		QWORD						qwFileSize;
		DWORD						dwSectionNum;
		MSGPACK_DEFINE(dwFileTag, bOpenFile, qwFileSize, dwSectionNum)
	};

	
	struct _FileDownload_C2S {
		DWORD						dwFileTag;			// ÿ���ļ�����������Ψһ��tag���Ա�ʶ
		DWORD						dwSectionIndex;		// dwIndex��1��ʼ���������һ��_FileDownload_C2S��dwIndex = 0
		msgpack::type::raw_ref		msData;			
		MSGPACK_DEFINE(dwFileTag, dwSectionIndex, msData)
	};


	struct _FileDownload_S2C {
		DWORD						dwFileTag;
		DWORD						dwLastIndex;// �ѽ��յ���Ƭ������index
		MSGPACK_DEFINE(dwFileTag, dwLastIndex)
	};


	struct _FileDownloadFailed_C2S {
		DWORD						dwFileTag;
		string						sClientPath;
		string						sErrorInfo;
		MSGPACK_DEFINE(dwFileTag, sClientPath, sErrorInfo)
	};


	struct _FileDownloadStop_C2S {
		DWORD						dwFileTag;
		MSGPACK_DEFINE(dwFileTag)
	};


	// ***************************** �ļ��ϴ� *****************************

	struct _FileUploadCmd_S2C {
		DWORD						dwFileTag;
		string						sClientPath;
		QWORD						qwFileSize;
		DWORD						dwSectionNum;
		MSGPACK_DEFINE(dwFileTag, sClientPath, qwFileSize, dwSectionNum)
	};


	struct _FileUploadCmd_C2S {
		DWORD						dwFileTag;
		BOOL						bOpenFile;
		MSGPACK_DEFINE(dwFileTag, bOpenFile)
	};

	
	struct _FileUpload_S2C {
		DWORD						dwFileTag;			// ÿ���ļ�����������Ψһ��tag���Ա�ʶ
		DWORD						dwSectionIndex;		// dwIndex��1��ʼ���������һ��_FileUpload_S2C��dwIndex = 0
		msgpack::type::raw_ref		msData;
		MSGPACK_DEFINE(dwFileTag, dwSectionIndex, msData)
	};


	struct _FileUpload_C2S {
		DWORD						dwFileTag;
		DWORD						dwLastIndex;		// �ѽ��յ���Ƭ������index
		MSGPACK_DEFINE(dwFileTag, dwLastIndex)
	};


	struct _FileUploadFailed_C2S {
		DWORD						dwFileTag;
		string						sClientPath;
		string						sErrorInfo;
		MSGPACK_DEFINE(dwFileTag, sClientPath, sErrorInfo)
	};

	struct _FileUploadStop_S2C {
		DWORD						dwFileTag;
		MSGPACK_DEFINE(dwFileTag)
	};
}