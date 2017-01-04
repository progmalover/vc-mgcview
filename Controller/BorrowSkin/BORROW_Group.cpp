#include "stdafx.h"
#include "BORROW_Group.h"
#include "ControllerVisualManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define visualManager ((CControllerVisualManager*)CMFCVisualManager::GetInstance())

/////////////////////////////////////////////////////////////////////////////
// CBORROW_Group

IMPLEMENT_DYNAMIC(CBORROW_Group, CButton)

CBORROW_Group::CBORROW_Group()
{
}

CBORROW_Group::~CBORROW_Group()
{
}

#ifndef WM_UPDATEUISTATE
#define	WM_UPDATEUISTATE 0x0128
#endif

BEGIN_MESSAGE_MAP(CBORROW_Group, CButton)
	//{{AFX_MSG_MAP(CBORROW_Group)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATEUISTATE, OnUpdateUIState)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBORROW_Group message handlers

BOOL CBORROW_Group::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}
//**************************************************************************
void CBORROW_Group::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CMemDC* pMemDC = NULL;
	CDC* pDC = &dc;

	CRect rectClient;
	GetClientRect (rectClient);

	CString strName;
	GetWindowText (strName);

	CFont* pOldFont = (CFont*) pDC->SelectStockObject (DEFAULT_GUI_FONT);
	ASSERT(pOldFont != NULL);

	pDC->SetBkMode (TRANSPARENT);
	pDC->SetTextColor (afxGlobalData.clrBarText);

	visualManager->OnDrawGroup (pDC, this, rectClient, strName);

	pDC->SelectObject (pOldFont);

	if (pMemDC != NULL)
	{
		delete pMemDC;
	}
}
//**************************************************************************
void CBORROW_Group::OnEnable(BOOL bEnable) 
{
	CButton::OnEnable(bEnable);

	if (GetParent () != NULL)
	{
		CRect rect;
		GetWindowRect (rect);

		GetParent ()->ScreenToClient (&rect);
		GetParent ()->RedrawWindow (rect);
	}
}
//**************************************************************************
LRESULT CBORROW_Group::OnUpdateUIState (WPARAM, LPARAM)
{
	return 0;
}
//*****************************************************************************
LRESULT CBORROW_Group::OnSetText (WPARAM, LPARAM)
{
	LRESULT lr = Default ();

	if (GetParent () != NULL)
	{
		CRect rect;
		GetWindowRect (rect);

		GetParent ()->ScreenToClient (&rect);
		GetParent ()->RedrawWindow (rect);
	}

	return lr;
}
