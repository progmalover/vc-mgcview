// F:\p4\sw\Demo\ContentManagement\Controller\BaseDockablePane.cpp : implementation file
//

#include "stdafx.h"
#include "BaseControllerPane.h"


IMPLEMENT_DYNAMIC(CBaseControllerPane, CDockablePane)

CBaseControllerPane::CBaseControllerPane()
{
    m_pMainWnd = NULL;
}


BEGIN_MESSAGE_MAP(CBaseControllerPane, CDockablePane)
    ON_WM_SIZE()
    ON_WM_SIZING()
    ON_WM_ERASEBKGND()
    ON_WM_SETFOCUS()
    ON_WM_GETMINMAXINFO()
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CBaseControllerPane message handlers
void CBaseControllerPane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);
    if(m_pMainWnd)
    {
        CRect rect;
        GetClientRect(rect);
        m_pMainWnd->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

BOOL CBaseControllerPane::OnEraseBkgnd(CDC* pDC)
{
    CDockablePane::OnEraseBkgnd(pDC);

    if(!m_pMainWnd)
    {
        CWnd::OnEraseBkgnd(pDC);
    }
    return TRUE;
}

void CBaseControllerPane::SetMainWnd(CWnd *pWnd)
{
    m_pMainWnd = pWnd;
}

void CBaseControllerPane::DrawCaption(CDC* pDC, CRect rectCaption)
{
    static int g_nCaptionHorzMargin = 2;
    ASSERT_VALID(pDC);

    CRect rcbtnRight = CRect(rectCaption.BottomRight(), CSize(0, 0));
    int i = 0;

    for (i = (int) m_arrButtons.GetUpperBound(); i >= 0; i --)
    {
        if (!m_arrButtons [i]->m_bLeftAlign && !m_arrButtons [i]->m_bHidden)
        {
            rcbtnRight = m_arrButtons [i]->GetRect();
            break;
        }
    }

    CRect rcbtnLeft = CRect(rectCaption.TopLeft(), CSize(0, 0));
    for (i = (int) m_arrButtons.GetUpperBound(); i >= 0; i --)
    {
        if (m_arrButtons [i]->m_bLeftAlign && !m_arrButtons [i]->m_bHidden)
        {
            rcbtnLeft = m_arrButtons [i]->GetRect();
            break;
        }
    }

    COLORREF clrCptnText = CMFCVisualManager::GetInstance()->OnDrawPaneCaption(pDC, this, m_bActive, rectCaption, rcbtnRight);

    for (i = 0; i < m_arrButtons.GetSize(); i ++)
    {
        CMFCCaptionButton* pbtn = m_arrButtons [i];
        ASSERT_VALID(pbtn);

        pbtn->m_clrForeground = clrCptnText;
    }

    int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
    COLORREF clrOldText = pDC->SetTextColor(clrCptnText);

    CFont* pOldFont = pDC->SelectObject(&afxGlobalData.fontRegular);
    ENSURE(pOldFont != NULL);

    CString strTitle;
    GetWindowText(strTitle);

    rectCaption.right = rcbtnRight.left;
    rectCaption.left = rcbtnLeft.right;
    rectCaption.top++;
    rectCaption.DeflateRect(g_nCaptionHorzMargin * 2, 0);

    LOGFONT lf;
    pDC->GetCurrentFont()->GetLogFont(&lf);
    CFont font;
    lf.lfWeight = FW_BOLD;
    font.CreateFontIndirect(&lf);
    pOldFont = pDC->SelectObject(&font);

    pDC->DrawText(strTitle, rectCaption, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

    pDC->SelectObject(pOldFont);
    pDC->SetBkMode(nOldBkMode);
    pDC->SetTextColor(clrOldText);

    font.DeleteObject(); 

}

void CBaseControllerPane::OnSetFocus(CWnd* pOldWnd)
{
    CDockablePane::OnSetFocus(pOldWnd);

    if(m_pMainWnd)
    {
        m_pMainWnd->SetFocus();
    }
}

void CBaseControllerPane::OnSizing(UINT fwSide, LPRECT pRect)
{
    CDockablePane::OnSizing(fwSide, pRect);
}


void CBaseControllerPane::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
    CDockablePane::OnGetMinMaxInfo(lpMMI);
    return;
}


LRESULT CBaseControllerPane::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID(this);

	LRESULT lResult;
    if(message >= 0xC000)
	{
		if (OnWndMsg(message, wParam, lParam, &lResult))
			return lResult;
        else
        {
            this->GetOwner()->SendMessage(message, wParam, lParam);
        }
	}

    return CDockablePane::WindowProc(message, wParam, lParam);
}

void CBaseControllerPane::OnContextMenu(CWnd* pWnd, CPoint point)
{

}
