#pragma once


//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的
//
//// 关闭 MFC 对某些常见但经常被安全忽略的警告消息的隐藏
//#define _AFX_ALL_WARNINGS
//
//#include <afxwin.h>         // MFC 核心和标准组件
//#include <afxext.h>         // MFC 扩展
//#include <afxdisp.h>        // MFC 自动化类
//
//#include <afxdtctl.h>		// Internet Explorer 4 公共控件的 MFC 支持
//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>			// Windows 公共控件的 MFC 支持
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
