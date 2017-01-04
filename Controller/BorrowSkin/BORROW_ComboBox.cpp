#include "stdafx.h"
#include "BORROW_ComboBox.h"
#include "ControllerVisualManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BORROW_ComboBox

IMPLEMENT_DYNAMIC(CBORROW_ComboBox, CComboBox)

CBORROW_ComboBox::CBORROW_ComboBox()
{
	m_bIsDroppedDown = FALSE;
	m_bIsButtonHighlighted = FALSE;
	m_rectBtn.SetRectEmpty ();
	m_bTracked = FALSE;
}

CBORROW_ComboBox::~CBORROW_ComboBox()
{
}

BEGIN_MESSAGE_MAP(CBORROW_ComboBox, CComboBox)
	//{{AFX_MSG_MAP(BORROW_ComboBox)
	ON_WM_NCPAINT()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT_EX(CBN_CLOSEUP, OnCloseup)
	ON_CONTROL_REFLECT_EX(CBN_DROPDOWN, OnDropdown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BORROW_ComboBox message handlers
//**************************************************************************
void CBORROW_ComboBox::OnNcPaint() 
{
}
//**************************************************************************
void CBORROW_ComboBox::OnPaint() 
{
	if ((GetStyle () & 0x0003L) == CBS_SIMPLE)
	{
		Default ();
		return;
	}

	CPaintDC dc(this); // device context for painting

	CMemDC memDC (dc, this);
	CDC* pDC = &memDC.GetDC ();
	
	CRect rectClient;
	GetClientRect (rectClient);

	CDrawingManager dm (*pDC);
	dm.DrawRect (rectClient, afxGlobalData.clrWindow, (COLORREF)-1);

	SendMessage (WM_PRINTCLIENT, (WPARAM) pDC->GetSafeHdc (), (LPARAM) PRF_CLIENT);

	const int cxDropDown = ::GetSystemMetrics (SM_CXVSCROLL) + 4;

	m_rectBtn = rectClient;
	m_rectBtn.left = m_rectBtn.right - cxDropDown;

	m_rectBtn.DeflateRect (2, 2);


	CMFCToolBarComboBoxButton buttonDummy;
	CMFCVisualManager::GetInstance ()->OnDrawComboDropButton (
		pDC, m_rectBtn, !IsWindowEnabled (), m_bIsDroppedDown,
		m_bIsButtonHighlighted,
		&buttonDummy);

	dm.DrawRect (rectClient, (COLORREF)-1, afxGlobalData.clrBarShadow);
	rectClient.DeflateRect (1, 1);
	dm.DrawRect (rectClient, (COLORREF)-1, afxGlobalData.clrWindow);
}
//**************************************************************************
BOOL CBORROW_ComboBox::OnCloseup() 
{
	m_bIsDroppedDown = FALSE;
	m_bIsButtonHighlighted = FALSE;

	RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);

	return FALSE;
}
//**************************************************************************
BOOL CBORROW_ComboBox::OnDropdown() 
{
	if (m_bTracked)
	{
		ReleaseCapture ();
		m_bTracked = FALSE;
	}

	m_bIsDroppedDown = TRUE;
	m_bIsButtonHighlighted = FALSE;
	RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);

	return FALSE;
}
//**************************************************************************
void CBORROW_ComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ((nFlags & MK_LBUTTON) == 0)
	{
		BOOL bIsButtonHighlighted = m_bIsButtonHighlighted;
		m_bIsButtonHighlighted = m_rectBtn.PtInRect (point);

		if (bIsButtonHighlighted != m_bIsButtonHighlighted)
		{
			if (!m_bTracked)
			{
				if (m_bIsButtonHighlighted)
				{
					SetCapture ();
					m_bTracked = TRUE;
				}
			}
			else
			{
				if (!m_bIsButtonHighlighted)
				{
					ReleaseCapture ();
					m_bTracked = FALSE;
				}
			}
		
			RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
		}
	}
	
	CComboBox::OnMouseMove(nFlags, point);
}
//*****************************************************************************************
void CBORROW_ComboBox::OnCancelMode() 
{
	CComboBox::OnCancelMode();
	
	if (m_bTracked)
	{
		ReleaseCapture ();
		m_bIsButtonHighlighted = FALSE;
		m_bTracked = FALSE;

		RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
	}
}
//**************************************************************************
void CBORROW_ComboBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bTracked)
	{
		ReleaseCapture ();
		m_bIsButtonHighlighted = FALSE;
		m_bTracked = FALSE;

		RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
	}
	
	CComboBox::OnLButtonDown(nFlags, point);
}
