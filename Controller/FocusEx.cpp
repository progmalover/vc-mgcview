// FocusEx.cpp: implementation of the CFocusEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FocusEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFocusEx::CFocusEx()
{
	m_pfnGetFocusableWindow = NULL;
	m_lParam = NULL;
	m_bInited = FALSE;
}

CFocusEx::~CFocusEx()
{
	m_pfnGetFocusableWindow = NULL;
	m_lParam = NULL;
	m_bInited = FALSE;
}

HWND CFocusEx::GetFirstFocusableWindow(HWND hWndDialog)
{
	HWND hWndCtrl = ::GetWindow( hWndDialog, GW_CHILD );
	hWndCtrl = ::GetWindow( hWndCtrl, GW_HWNDFIRST );

	while( hWndCtrl ) {
		if( IsFocusableWindow( hWndCtrl ) ) {
			break;
		}

		hWndCtrl = ::GetWindow( hWndCtrl, GW_HWNDNEXT );
	}

	CString s;
	s.Format(_T("%X"), hWndCtrl);
	OutputDebugString(s);

	return hWndCtrl;
}

HWND CFocusEx::GetLastFocusableWindow(HWND hWndDialog)
{
	HWND hWndCtrl = ::GetWindow( hWndDialog, GW_CHILD );
	hWndCtrl = ::GetWindow( hWndCtrl, GW_HWNDLAST );

	while( hWndCtrl ) {
		if( IsFocusableWindow( hWndCtrl ) ) {
			break;
		}

		hWndCtrl = ::GetWindow( hWndCtrl, GW_HWNDPREV );
	}

	return hWndCtrl;
}

BOOL CFocusEx::IsFocusableWindow(CWnd *pWndCtrl)
{
	if( pWndCtrl ) {
		return IsFocusableWindow( pWndCtrl->GetSafeHwnd() );		
	}

	return FALSE;
}

BOOL CFocusEx::IsFocusableWindow(HWND hWndCtrl)
{
	if( hWndCtrl ) {
		LONG lStyle = ::GetWindowLong( hWndCtrl, GWL_STYLE );

		if( (lStyle & WS_TABSTOP) == WS_TABSTOP &&
			::IsWindowEnabled( hWndCtrl ) &&
			::IsWindowVisible( hWndCtrl ) ) {
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFocusEx::ProcessKeyPressMessage(CWnd* pWndDialog, MSG* pMsg)
{
	if (pMsg)
    {
		switch (pMsg->message)
        {
		case WM_KEYDOWN:
			{
				if (pMsg->wParam == VK_TAB)
                {
                    if (GetKeyState(VK_SHIFT) < 0)
                        return InternalProcessKeyPressMessage(pWndDialog, TRUE);					
                    else
					    return InternalProcessKeyPressMessage(pWndDialog, FALSE);					
				}				
			}
			break;
		}
	}
	return FALSE;
}

BOOL CFocusEx::InternalProcessKeyPressMessage(CWnd *pWndDialog, BOOL shift)
{
	CWnd* pWndFocus = pWndDialog->GetFocus();
				
	if (pWndFocus)
    {
		if (m_pfnGetFocusableWindow)
        {
			HWND hWndFirst = m_pfnGetFocusableWindow(FOCUSABLEWINDOW_POSITION_FIRST, m_lParam, shift);
			HWND hWndFocusable = m_pfnGetFocusableWindow(FOCUSABLEWINDOW_POSITION_FOCUSABLE, m_lParam, shift);
			HWND hWndLast = m_pfnGetFocusableWindow(FOCUSABLEWINDOW_POSITION_LAST, m_lParam, shift);

            if (!shift)
            {
                if (hWndLast == pWndFocus->GetSafeHwnd())
                {
                    //::SetFocus( hWndFirst );
                    ::SetFocus( hWndFocusable );
                    return TRUE;
                }
            }
            else
            {
                if (hWndFirst == pWndFocus->GetSafeHwnd())
                {
                    //::SetFocus( hWndLast );
                    HWND hwnd = GetLastFocusableWindow(hWndFocusable);
                    if (hwnd)
                    {
                        ::SetFocus(hwnd);
                    }
                    else
                    {
                        ::SetFocus( hWndFocusable );
                    }
                    return TRUE;
                }
            }
		}
	}

	return FALSE;
}

void CFocusEx::InitFocusEx(PFNGETFOCUSABLEWINDOW pfnGetFocusableWindow, LPVOID lParam)
{
	m_bInited = TRUE;

	m_pfnGetFocusableWindow = pfnGetFocusableWindow;
	m_lParam = lParam;
}

BOOL CFocusEx::SetFocusToFirstWindow(HWND hWndDialog)
{
	HWND hWndFirst = GetFirstFocusableWindow(hWndDialog);
	if( hWndFirst ) {
		::SetFocus( hWndFirst );
		return TRUE;
	}

	return FALSE;
}
