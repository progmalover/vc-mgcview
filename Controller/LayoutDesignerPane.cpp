// F:\p4\sw\Demo\ContentManagement\Controller\Designer\LayoutDesignerPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "LayoutDesignerPane.h"



// CLayoutDesignerPane

IMPLEMENT_DYNAMIC(CLayoutDesignerPane, CBaseControllerPane)

CLayoutDesignerPane::CLayoutDesignerPane()
{
		m_bEnableCaption = FALSE;
}

CLayoutDesignerPane::~CLayoutDesignerPane()
{
}


BEGIN_MESSAGE_MAP(CLayoutDesignerPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_NCCALCSIZE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// CLayoutDesignerPane message handlers
int CLayoutDesignerPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect layoutRect;
    GetClientRect(layoutRect);

    CString szCanvasClass = AfxRegisterWndClass( CS_VREDRAW | CS_HREDRAW , 
        ::LoadCursor(NULL, IDC_CROSS),
        (HBRUSH) ::GetStockObject(BLACK_BRUSH),
        NULL);

    if (!m_wndDesign.Create(NULL, NULL, WS_CHILD | WS_VISIBLE |
		  WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
        layoutRect, this, ID_DESIGNEVIEW, NULL))
    {
        TRACE0("Failed to create window\n");
        return -1;
    }

	if(!m_Ruler.Create(this,&m_wndDesign))
		return 0;
    SetMainWnd(&m_Ruler);
    return 0;
}

void CLayoutDesignerPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

void CLayoutDesignerPane::OnSize(UINT nType, int cx, int cy)
{
	//CBaseControllerPane::OnSize(nType, cx, cy);
	if(this->m_pMainWnd)
	{
		MSG msg;
		msg.message = WM_SIZE;
		m_Ruler.DelayEvent(&msg);
	}
	// TODO: Add your message handler code here
}


BOOL CLayoutDesignerPane::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	/*if( m_Ruler.DelayEvent(pMsg))
		return TRUE;*/
	return CBaseControllerPane::PreTranslateMessage(pMsg);
	
}


void CLayoutDesignerPane::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: Add your message handler code here and/or call default
	if(this->m_bEnableCaption)
		CBaseControllerPane::OnNcCalcSize(bCalcValidRects, lpncsp);
}


void CLayoutDesignerPane::OnSetFocus(CWnd* pOldWnd)
{
	CBaseControllerPane::OnSetFocus(pOldWnd);

	//validating "bound rect" of "draw object",ensure them can be previewed properly.
	m_wndDesign.UpDataDrawObject();
	// TODO: Add your message handler code here
}
