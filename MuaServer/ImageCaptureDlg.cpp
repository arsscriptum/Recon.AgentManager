﻿#include "pch.h"
#include "ImageCaptureDlg.h"

#include "afxdialogex.h"

#include <ImageHlp.h>  
#pragma comment(lib,"imagehlp.lib")			// MakeSureDirectoryPathExists


using namespace nsImageCapture;


// CImageCaptureDlg 对话框

IMPLEMENT_DYNAMIC(CImageCaptureDlg, CDialogEx)

CImageCaptureDlg::CImageCaptureDlg(CClientSocket* pClientSocket, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IMAGE_CAPTURE_DIALOG, pParent)
{
	m_pClientSocket = pClientSocket;

	this->Create(IDD_IMAGE_CAPTURE_DIALOG, GetDesktopWindow());
	this->ShowWindow(SW_SHOW);

	WCHAR pszTitle[64];
	wsprintf(pszTitle, L"image capture    %s:%d\n", m_pClientSocket->m_wszIpAddress, m_pClientSocket->m_wPort);
	this->SetWindowText(pszTitle);
}


CImageCaptureDlg::~CImageCaptureDlg()
{
	if (m_pClientSocket != nullptr) {
		GetClientManager()->DeleteClientSocket(m_pClientSocket);
		m_pClientSocket = nullptr;
	}
}


void CImageCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


// 解决回车键 ESC 默认关闭窗口
BOOL CImageCaptureDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)     return   TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)   return   TRUE;
	else
		return CDialog::PreTranslateMessage(pMsg);
}


BEGIN_MESSAGE_MAP(CImageCaptureDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_PRINT_SCREEN_, &CImageCaptureDlg::OnBnClickedPrintScreen)
END_MESSAGE_MAP()


// CImageCaptureDlg 消息处理程序


void CImageCaptureDlg::OnClose()
{
	if (m_pClientSocket != nullptr) {
		GetClientManager()->DeleteClientSocket(m_pClientSocket);
		m_pClientSocket = nullptr;
	}

	CDialogEx::OnClose();
}


VOID CImageCaptureDlg::OnReceiveWithDec(ITcpServer* pSender, CONNID dwConnID, QWORD qwClientToken, WORD wServiceType, WORD wSocketType, int iCommandID, DWORD dwParentSocketTag, MyBuffer mBuffer) {
	switch (iCommandID) {
	case PRINT_SCREEN_C2S:
		DisplayPrintScreen(dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, mBuffer);
		break;
	}
}


// 屏幕截图
void CImageCaptureDlg::OnBnClickedPrintScreen()
{
	GetServerSocket()->SendWithEnc(m_pClientSocket->m_dwConnID, PRINT_SCREEN_S2C);
}


void CImageCaptureDlg::DisplayPrintScreen(CONNID dwConnID, QWORD qwClientToken, WORD wServiceType, WORD wSocketType, int iCommandID, MyBuffer mBuffer) {
	
	RecvImage(dwConnID, qwClientToken, wServiceType, wSocketType, iCommandID, mBuffer);
}


// Receive image slices and merge them into a complete image
VOID CImageCaptureDlg::RecvImage(CONNID dwConnID, QWORD qwClientToken, WORD wServiceType, WORD wSocketType, int iCommandID, MyBuffer mBuffer) {
	_ImageSection_C2S mData = MsgUnpack<_ImageSection_C2S>((PBYTE)mBuffer.ptr(), mBuffer.size());

	// The first slice of the image, concatenated into the filename where it will be stored
	if (mData.dwIndex == 1) {

		//If the folder does not exist, create it
		CHAR szPathName[MAX_PATH] = { 0 };
		sprintf_s(szPathName, "%s\\MuaData\\ImageCapture\\", ROOT_DIR);		// If the last component of the path is a directory rather than a filename, the string must end with \.
		if (_access(szPathName, 0) == -1) {
			MakeSureDirectoryPathExists(szPathName);
		}

		// 获取时间
		struct tm stime;
		time_t t = time(0);
		localtime_s(&stime, &t);
		char szTime[32]{ 0 };
		snprintf(szTime, sizeof(szTime), "%04d%02d%02d_%02d%02d%02d", 1900 + stime.tm_year, 1 + stime.tm_mon, stime.tm_mday, stime.tm_hour, stime.tm_min, stime.tm_sec);

		// 拼接文件名
		if (strcmp(szTime, m_szLastImageTime) == 0) {
			m_dwCurrentTimeImageNum++;
		}
		else {
			strcpy_s(m_szLastImageTime, 32, szTime);
			m_dwCurrentTimeImageNum = 1;
		}
		USES_CONVERSION;
		sprintf_s(m_szImagePath, "%s%s_%s_%04d.jpg", szPathName, W2A(m_pClientSocket->m_wszIpAddress), szTime, m_dwCurrentTimeImageNum);
	}

	// In this case, the client must be forging packets in an attempt to consume the resources of the server.
	else if (mData.dwIndex != 0 && mData.dwIndex != m_dwImageSectionIndex + 1) {
		// TODO: Disconnect
		return;
	}

	// write to file
	string sImagePath = m_szImagePath;
	std::ofstream of(sImagePath, std::ios_base::binary | std::ios_base::app);
	of.write(mData.msImageSection.ptr, mData.msImageSection.size);
	of.close();

	// 图像的最后一个切片
	if (mData.dwIndex == 0) {
		LoadImage(m_szImagePath);

		memset(m_szImagePath, 0, sizeof(m_szImagePath));
		m_dwImageSectionIndex = 0;
	}
	else {
		m_dwImageSectionIndex++;
	}
}


// Load image to UI
VOID CImageCaptureDlg::LoadImage(PCHAR pszFileName) {
	CString csImagePath;
	USES_CONVERSION;
	csImagePath.Format(L"%s", A2W(pszFileName));
 
	//Define variables to store image information
	//BITMAPINFO* pBmpInfo;				//记录图像细节  
	//BYTE* pBmpData;						//图像数据  
	BITMAPFILEHEADER bmpHeader;			//文件头  
	BITMAPINFOHEADER bmpInfo;			//信息头  
	CFile bmpFile;						//记录打开文件  

	//Open the file in a read-only mode and read each part of the BMP image. The bmp file header information data
	if (!bmpFile.Open(csImagePath, CFile::modeRead | CFile::typeBinary))
		return;
	if (bmpFile.Read(&bmpHeader, sizeof(BITMAPFILEHEADER)) != sizeof(BITMAPFILEHEADER))
		return;
	if (bmpFile.Read(&bmpInfo, sizeof(BITMAPINFOHEADER)) != sizeof(BITMAPINFOHEADER))
		return;

	MyBuffer mBmpInfo = MyBuffer(sizeof(BITMAPINFOHEADER));

	//为图像数据申请空间  
	memcpy(mBmpInfo.ptr(), &bmpInfo, sizeof(BITMAPINFOHEADER));
	DWORD dataBytes = bmpHeader.bfSize - bmpHeader.bfOffBits;
	MyBuffer mBmpData = MyBuffer(dataBytes);

	bmpFile.Read(mBmpData.ptr(), dataBytes);
	bmpFile.Close();

	//显示图像  
	CWnd* pWnd = GetDlgItem(IDC_PICTURE1);		//获得pictrue控件窗口的句柄  
	CRect rect;
	pWnd->GetClientRect(&rect);					//获得pictrue控件所在的矩形区域  
	CDC* pDC = pWnd->GetDC();					//获得pictrue控件的DC  
	pDC->SetStretchBltMode(COLORONCOLOR);
	StretchDIBits(pDC->GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, bmpInfo.biWidth, bmpInfo.biHeight, mBmpData.ptr(), (BITMAPINFO*)mBmpInfo.ptr(), DIB_RGB_COLORS, SRCCOPY);
}