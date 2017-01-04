// F:\p4\sw\Demo\ContentManagement\Controller\MainWnd.cpp : implementation file
//

#include "stdafx.h"
#include "BaseControllerPage.h"

UINT CLOSE_CURRENT_PAGE			= ::RegisterWindowMessage (_T("CLOSE_CURRENT_PAGE"));


// wParam - not used, lParam - points to string that use to be set to the state bar (TCHAR*)
UINT MAIN_APP_STATE_BAR_INFO			= ::RegisterWindowMessage (_T("MAIN_APP_STATE_BAR_INFO"));

UINT MAIN_APP_ALARM_INFO			= ::RegisterWindowMessage (_T("MAIN_APP_ALARM_INFO"));
// CBaseControllerPage

IMPLEMENT_DYNAMIC(CBaseControllerPage, CFrameWndEx)


BEGIN_MESSAGE_MAP(CBaseControllerPage, CFrameWndEx)
    ON_WM_NCPAINT()
END_MESSAGE_MAP()

CBaseControllerPage::CBaseControllerPage()
{
    m_pCategory = NULL;
    m_pMainWnd = NULL;
    m_bEnable = TRUE;
}

CBaseControllerPage::~CBaseControllerPage()
{
    m_pCategory = NULL;
}


void CBaseControllerPage::SetMainWnd(CWnd *pWnd)
{
    m_pMainWnd = pWnd;
}

void CBaseControllerPage::AdjustClientArea()
{
    if(m_pMainWnd)
    {
        CRect rect;
        m_dockManager.GetClientAreaBounds(rect);
        m_pMainWnd->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
    }
}


void CBaseControllerPage::OnSetFocus(CWnd* pOldWnd)
{
    CFrameWndEx::OnSetFocus(pOldWnd);
}

BOOL CBaseControllerPage::OnEraseBkgnd(CDC* pDC)
{

    return TRUE;
}

void  CBaseControllerPage::OnNcPaint()
{                  
    RecalcLayout(TRUE);

    CFrameWndEx::OnNcPaint();   
}

BOOL CBaseControllerPage::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Add your specialized code here and/or call the base class

    CFrameWndEx::PreCreateWindow(cs);

    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
    return TRUE;
}


BOOL CBaseControllerPage::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
#if 0
	if(pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
	{
			CPoint pt;
			::GetCursorPos(&pt);
			CWnd *pWnd = CWnd ::WindowFromPoint(pt);
			if(pWnd != CWnd ::GetFocus())
			{
				//pWnd->SetActiveWindow();
				if(!pWnd->IsKindOf(RUNTIME_CLASS(CMFCRibbonBar)) && 
					!pWnd->IsKindOf(RUNTIME_CLASS(CBasePane))
					)
				{
					pWnd->SetFocus();
				}
				 
			
			}
	}
#endif
	return CFrameWndEx::PreTranslateMessage(pMsg);
}
