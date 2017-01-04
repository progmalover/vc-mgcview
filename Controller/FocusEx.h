// FocusEx.h: interface for the CFocusEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FOCUSEX_H__215ABE97_EC9E_4704_AC09_FA169582168E__INCLUDED_)
#define AFX_FOCUSEX_H__215ABE97_EC9E_4704_AC09_FA169582168E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define FOCUSABLEWINDOW_POSITION_FIRST				0
#define FOCUSABLEWINDOW_POSITION_FOCUSABLE			1
#define FOCUSABLEWINDOW_POSITION_LAST				2


typedef HWND (* PFNGETFOCUSABLEWINDOW)(int nPosition, LPVOID lParam, BOOL shift);

class CFocusEx  
{
public:
	static BOOL SetFocusToFirstWindow(HWND hWndDialog);
	void InitFocusEx(PFNGETFOCUSABLEWINDOW pfnGetFocusableWindow, LPVOID lParam);
	BOOL ProcessKeyPressMessage(CWnd* pWndDialog, MSG* pMsg);

	static HWND GetLastFocusableWindow( HWND hWndDialog );
	static HWND GetFirstFocusableWindow( HWND hWndDialog );


	CFocusEx();
	~CFocusEx();

private:
	static BOOL IsFocusableWindow( HWND hWndCtrl );
	static BOOL IsFocusableWindow( CWnd* pWndCtrl );
	
private:
	BOOL InternalProcessKeyPressMessage(CWnd* pWndDialog, BOOL shift);

	PFNGETFOCUSABLEWINDOW		m_pfnGetFocusableWindow;
	LPVOID						m_lParam;

	BOOL						m_bInited;
};

#endif // !defined(AFX_FOCUSEX_H__215ABE97_EC9E_4704_AC09_FA169582168E__INCLUDED_)
