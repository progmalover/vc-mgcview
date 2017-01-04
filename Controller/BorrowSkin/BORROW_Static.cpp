#include "stdafx.h"
#include "ControllerVisualManager.h"
#include "BORROW_Static.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CBORROW_Static, CStatic)

/////////////////////////////////////////////////////////////////////////////
// CBORROW_Static

CBORROW_Static::CBORROW_Static()
{
	m_bOnGlass = FALSE;
	m_bVisualManagerStyle = FALSE;
	m_clrText = (COLORREF)-1;
	m_bBackstageMode = FALSE;
}

CBORROW_Static::~CBORROW_Static()
{
}


BEGIN_MESSAGE_MAP(CBORROW_Static, CStatic)
	//{{AFX_MSG_MAP(CBORROW_Static)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_ENABLE()
	ON_WM_NCPAINT()
	//}}AFX_MSG_MAP
// 	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLAERO, OnBCGSetControlAero)
// 	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLVMMODE, OnBCGSetControlVMMode)
// 	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLBACKSTAGEMODE, OnBCGSetControlBackStageMode)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBORROW_Static message handlers

BOOL CBORROW_Static::OnEraseBkgnd(CDC* pDC) 
{
	if (!m_bOnGlass)
	{
		return CStatic::OnEraseBkgnd (pDC);
	}

	return TRUE;
}
//*****************************************************************************
void CBORROW_Static::OnPaint() 
{
	const DWORD dwStyle = GetStyle ();

	if ((dwStyle & SS_ICON) == SS_ICON ||
		(dwStyle & SS_BLACKRECT) == SS_BLACKRECT ||
		(dwStyle & SS_GRAYRECT) == SS_GRAYRECT ||
		(dwStyle & SS_WHITERECT) == SS_WHITERECT ||
		(dwStyle & SS_BLACKFRAME) == SS_BLACKFRAME ||
		(dwStyle & SS_GRAYFRAME) == SS_GRAYFRAME ||
		(dwStyle & SS_WHITEFRAME) == SS_WHITEFRAME ||
		(dwStyle & SS_USERITEM) == SS_USERITEM ||
		(dwStyle & SS_ETCHEDHORZ) == SS_ETCHEDHORZ ||
		(dwStyle & SS_ETCHEDVERT) == SS_ETCHEDVERT)
	{
		Default ();
		return;
	}

	CPaintDC dc(this); // device context for painting

	CMemDC memDC (dc, this);
	CDC* pDC = &memDC.GetDC ();

	CRect rectText;
	GetClientRect (rectText);

	afxGlobalData.DrawParentBackground (this, pDC);

	CFont* pOldFont = (CFont*) pDC->SelectStockObject (DEFAULT_GUI_FONT);
	ASSERT(pOldFont != NULL);

	UINT uiDTFlags = DT_WORDBREAK;

	if (dwStyle & SS_CENTER)
	{
		uiDTFlags |= DT_CENTER;
	}
	else if (dwStyle & SS_RIGHT)
	{
		uiDTFlags |= DT_RIGHT;
	}

	if (dwStyle & SS_NOPREFIX)
	{
		uiDTFlags |= DT_NOPREFIX;
	}

	COLORREF clrText = m_clrText == (COLORREF)-1 ? (m_bBackstageMode ? RGB(0, 0, 0) : afxGlobalData.clrBarText) : m_clrText;
	if (!IsWindowEnabled ())
	{
        clrText = CMFCVisualManager::GetInstance()->GetToolbarDisabledTextColor ();
	}

	CString strText;
	GetWindowText (strText);

    COLORREF clrTextOld = pDC->SetTextColor (clrText);
    pDC->SetBkMode (TRANSPARENT);
    pDC->DrawText (strText, rectText, uiDTFlags);
    pDC->SetTextColor (clrTextOld);


	pDC->SelectObject (pOldFont);
}
//*****************************************************************************
LRESULT CBORROW_Static::OnSetText (WPARAM, LPARAM)
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
//*****************************************************************************
void CBORROW_Static::OnEnable(BOOL bEnable) 
{
	CStatic::OnEnable(bEnable);

	if (GetParent () != NULL)
	{
		CRect rect;
		GetWindowRect (rect);

		GetParent ()->ScreenToClient (&rect);
		GetParent ()->RedrawWindow (rect);
	}
}
//*****************************************************************************
void CBORROW_Static::OnNcPaint() 
{
	const DWORD dwStyle = GetStyle ();
	BOOL bIsSeparator = (dwStyle & SS_ETCHEDHORZ) == SS_ETCHEDHORZ || (dwStyle & SS_ETCHEDVERT) == SS_ETCHEDVERT;

    //if (!bIsSeparator || !CMFCVisualManager::GetInstance ()->IsOwnerDrawDlgSeparator(this))
	{
		Default();
		return;
	}

	CWindowDC dc(this);

	CRect rect;
	GetWindowRect (rect);

	rect.bottom -= rect.top;
	rect.right -= rect.left;
	rect.left = rect.top = 0;

	//CBORROW_VisualManager::GetInstance ()->OnDrawDlgSeparator(&dc, this, rect, (dwStyle & SS_ETCHEDVERT) != SS_ETCHEDVERT);
}
