#pragma once


//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯��������ʽ��
//
//// �ر� MFC ��ĳЩ��������������ȫ���Եľ�����Ϣ������
//#define _AFX_ALL_WARNINGS
//
//#include <afxwin.h>         // MFC ���ĺͱ�׼���
//#include <afxext.h>         // MFC ��չ
//#include <afxdisp.h>        // MFC �Զ�����
//
//#include <afxdtctl.h>		// Internet Explorer 4 �����ؼ��� MFC ֧��
//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>			// Windows �����ؼ��� MFC ֧��
//#endif // _AFX_NO_AFXCMN_SUPPORT
//
//#include <gdiplus.h>
//#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

class CUtility
{
public:
	static CString GetModulePath(HMODULE hModule = NULL);

	static BOOL IsFileExist(LPCTSTR lpszFilePath);

	static BOOL ExtractResourceToFile( LPCTSTR lpszType
									, UINT nResID
									, LPCTSTR lpszFilename
									, HMODULE hModule
									);

	static Image * LoadImage( UINT nID, LPCTSTR lpszType, HINSTANCE hInstance = NULL);

	static HRGN CreateRegionFromBitmap(Bitmap* bitmap, BYTE alphaValve = 0);

	static HRGN CreateRegion(LPCTSTR lpszFile, BYTE alphaValve = 0);

	static HRGN CreateRegion(UINT nID, LPCTSTR lpszType, HINSTANCE hInstance = NULL, BYTE alphaValve = 0); 
};
