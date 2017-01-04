//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of the BCGControlBar Library
// Copyright (C) 1998-2010 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
 //*******************************************************************************

#include "stdafx.h"

#include "BORROW_Dialog.h"
#include "Localization/Localization.h"
/*
#ifndef _BCGSUITE_
#include "BCGPPopupMenu.h"
#include "BCGPToolbarMenuButton.h"
#include "BCGPPngImage.h"
#endif
*/

#include "ControllerVisualManager.h"
// #include "BCGPLocalResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CBORROW_Dialog, CDialog)

/////////////////////////////////////////////////////////////////////////////
// CBORROW_Dialog dialog

#pragma warning (disable : 4355)

CBORROW_Dialog::CBORROW_Dialog() :
	m_Impl (*this)
{
	CommonConstruct ();
}
//*************************************************************************************
CBORROW_Dialog::CBORROW_Dialog (UINT nIDTemplate, CWnd *pParent/*= NULL*/) : 
				CDialog (nIDTemplate, pParent),
				m_Impl (*this)
{
	CommonConstruct ();
}
//*************************************************************************************
CBORROW_Dialog::CBORROW_Dialog (LPCTSTR lpszTemplateName, CWnd *pParentWnd/*= NULL*/) : 
				CDialog(lpszTemplateName, pParentWnd),
				m_Impl (*this)
{
	CommonConstruct ();
}

#pragma warning (default : 4355)

//*************************************************************************************
void CBORROW_Dialog::CommonConstruct ()
{
	m_hBkgrBitmap = NULL;
	m_sizeBkgrBitmap = CSize (0, 0);
	m_BkgrLocation = (BackgroundLocation) -1;
	m_bAutoDestroyBmp = FALSE;
	m_bIsLocal = FALSE;
	m_pLocaRes = NULL;
	m_bWasMaximized = FALSE;
	m_rectBackstageWatermark.SetRectEmpty();
	m_pBackstageWatermark = NULL;
}
//*************************************************************************************
void CBORROW_Dialog::SetBackgroundColor (COLORREF color, BOOL bRepaint)
{
	if (m_brBkgr.GetSafeHandle () != NULL)
	{
		m_brBkgr.DeleteObject ();
	}

	if (color != (COLORREF)-1)
	{
		m_brBkgr.CreateSolidBrush (color);
	}

	if (bRepaint && GetSafeHwnd () != NULL)
	{
		Invalidate ();
		UpdateWindow ();
	}
}
//*************************************************************************************
void CBORROW_Dialog::SetBackgroundImage (HBITMAP hBitmap, 
								BackgroundLocation location,
								BOOL bAutoDestroy,
								BOOL bRepaint)
{
	if (m_bAutoDestroyBmp && m_hBkgrBitmap != NULL)
	{
		::DeleteObject (m_hBkgrBitmap);
	}

	m_hBkgrBitmap = hBitmap;
	m_BkgrLocation = location;
	m_bAutoDestroyBmp = bAutoDestroy;

	if (hBitmap != NULL)
	{
		BITMAP bmp;
		::GetObject (hBitmap, sizeof (BITMAP), (LPVOID) &bmp);

		m_sizeBkgrBitmap = CSize (bmp.bmWidth, bmp.bmHeight);
	}
	else
	{
		m_sizeBkgrBitmap = CSize (0, 0);
	}

	if (bRepaint && GetSafeHwnd () != NULL)
	{
		Invalidate ();
		UpdateWindow ();
	}
}
//*************************************************************************************
BOOL CBORROW_Dialog::SetBackgroundImage (UINT uiBmpResId,
									BackgroundLocation location,
									BOOL bRepaint)
{
	HBITMAP hBitmap = NULL;

	if (uiBmpResId != 0)
	{
		//-----------------------------
		// Try to load PNG image first:
		//-----------------------------
/*
		CBCGPPngImage pngImage;
		if (pngImage.Load (MAKEINTRESOURCE (uiBmpResId)))
		{
			hBitmap = (HBITMAP) pngImage.Detach ();
		}
		else
*/
		{
			hBitmap = ::LoadBitmap (AfxGetResourceHandle (), MAKEINTRESOURCE (uiBmpResId));
		}

		if (hBitmap == NULL)
		{
			ASSERT (FALSE);
			return FALSE;
		}
	}

	SetBackgroundImage (hBitmap, location, TRUE /* Autodestroy */, bRepaint);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CBORROW_Dialog, CDialog)
	//{{AFX_MSG_MAP(CBORROW_Dialog)
	ON_WM_ACTIVATE()
	ON_WM_NCACTIVATE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SETTINGCHANGE()
	ON_WM_SIZE()
	ON_WM_NCPAINT()
	ON_WM_NCMOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_NCHITTEST()
	ON_WM_NCCALCSIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_GETMINMAXINFO()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_DWMCOMPOSITIONCHANGED, OnDWMCompositionChanged)
// 	ON_REGISTERED_MESSAGE(BCGM_CHANGEVISUALMANAGER, OnChangeVisualManager)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBORROW_Dialog message handlers

void CBORROW_Dialog::OnActivate(UINT nState, CWnd *pWndOther, BOOL /*bMinimized*/) 
{
	m_Impl.OnActivate (nState, pWndOther);
}
//*************************************************************************************
BOOL CBORROW_Dialog::OnNcActivate(BOOL bActive) 
{
	//-----------------------------------------------------------
	// Do not call the base class because it will call Default()
	// and we may have changed bActive.
	//-----------------------------------------------------------
	BOOL bRes = (BOOL)DefWindowProc (WM_NCACTIVATE, bActive, 0L);

	if (bRes)
	{
		m_Impl.OnNcActivate (bActive);
	}

	return bRes;
}
//**************************************************************************************
BOOL CBORROW_Dialog::OnEraseBkgnd(CDC* pDC) 
{
	BOOL bRes = TRUE;

	if (m_brBkgr.GetSafeHandle () == NULL && m_hBkgrBitmap == NULL &&
		!IsVisualManagerStyle ())
	{
		bRes = CDialog::OnEraseBkgnd (pDC);
	}
	else
	{
		ASSERT_VALID (pDC);

		CRect rectClient;
		GetClientRect (rectClient);

		if (m_BkgrLocation != BACKGR_TILE || m_hBkgrBitmap == NULL)
		{
			if (m_brBkgr.GetSafeHandle () != NULL)
			{
				pDC->FillRect (rectClient, &m_brBkgr);
			}
			else if (IsVisualManagerStyle ())
			{
/*
#ifndef _BCGSUITE_
				if (IsBackstageMode())
				{
					CMemDC memDC(*pDC, this);
					CDC* pDCMem = &memDC.GetDC();

					visualManager->OnFillRibbonBackstageForm(pDCMem, this, rectClient);

					if (!m_rectBackstageWatermark.IsRectEmpty())
					{
						if (m_pBackstageWatermark != NULL)
						{
							ASSERT_VALID(m_pBackstageWatermark);
							m_pBackstageWatermark->DrawEx(pDCMem, m_rectBackstageWatermark, 0);
						}
						else
						{
							OnDrawBackstageWatermark(pDCMem, m_rectBackstageWatermark);
						}
					}

					bRes = TRUE;
				}
				else
#endif
*/
				{
					if (!visualManager->OnFillDialog (pDC, this, rectClient))
					{
						CDialog::OnEraseBkgnd (pDC);
					}
				}
			}
			else
			{
				CDialog::OnEraseBkgnd (pDC);
			}
		}

		if (m_hBkgrBitmap != NULL)
		{
			ASSERT (m_sizeBkgrBitmap != CSize (0, 0));

			if (m_BkgrLocation != BACKGR_TILE)
			{
				CPoint ptImage = rectClient.TopLeft ();

				switch (m_BkgrLocation)
				{
				case BACKGR_TOPLEFT:
					break;

				case BACKGR_TOPRIGHT:
					ptImage.x = rectClient.right - m_sizeBkgrBitmap.cx;
					break;

				case BACKGR_BOTTOMLEFT:
					ptImage.y = rectClient.bottom - m_sizeBkgrBitmap.cy;
					break;

				case BACKGR_BOTTOMRIGHT:
					ptImage.x = rectClient.right - m_sizeBkgrBitmap.cx;
					ptImage.y = rectClient.bottom - m_sizeBkgrBitmap.cy;
					break;
				}

				pDC->DrawState (ptImage, m_sizeBkgrBitmap, m_hBkgrBitmap, DSS_NORMAL);
			}
			else
			{
				// Tile background image:
				for (int x = rectClient.left; x < rectClient.Width (); x += m_sizeBkgrBitmap.cx)
				{
					for (int y = rectClient.top; y < rectClient.Height (); y += m_sizeBkgrBitmap.cy)
					{
						pDC->DrawState (CPoint (x, y), m_sizeBkgrBitmap, m_hBkgrBitmap, DSS_NORMAL);
					}
				}
			}
		}
	}

	m_Impl.DrawResizeBox(pDC);
	m_Impl.ClearAeroAreas (pDC);
	return bRes;
}
//**********************************************************************************
void CBORROW_Dialog::OnDestroy() 
{
	if (m_bAutoDestroyBmp && m_hBkgrBitmap != NULL)
	{
		::DeleteObject (m_hBkgrBitmap);
		m_hBkgrBitmap = NULL;
	}

	m_Impl.OnDestroy ();

	CDialog::OnDestroy();
}
//***************************************************************************************
HBRUSH CBORROW_Dialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if (m_brBkgr.GetSafeHandle () != NULL || m_hBkgrBitmap != NULL ||
		IsVisualManagerStyle ())
	{
		HBRUSH hbr = m_Impl.OnCtlColor (pDC, pWnd, nCtlColor);
		if (hbr != NULL)
		{
			return hbr;
		}
	}	

	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}
//**************************************************************************************
BOOL CBORROW_Dialog::PreTranslateMessage(MSG* pMsg) 
{
	if (m_Impl.PreTranslateMessage (pMsg))
	{
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
//**************************************************************************************
void CBORROW_Dialog::SetActiveMenu (CMFCPopupMenu* pMenu)
{
	m_Impl.SetActiveMenu (pMenu);
}
//*************************************************************************************
BOOL CBORROW_Dialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (m_Impl.OnCommand (wParam, lParam))
	{
		return TRUE;
	}

	return CDialog::OnCommand(wParam, lParam);
}
//*************************************************************************************
INT_PTR CBORROW_Dialog::DoModal() 
{
	if (m_bIsLocal)
	{
		m_pLocaRes = new CBORROW_LocalResource ();
	}

	return CDialog::DoModal();
}
//*************************************************************************************
void CBORROW_Dialog::PreInitDialog()
{
	if (m_pLocaRes != NULL)
	{
		delete m_pLocaRes;
		m_pLocaRes = NULL;
	}
}
//*************************************************************************************
void CBORROW_Dialog::OnSysColorChange() 
{
	CDialog::OnSysColorChange();
	
	if (AfxGetMainWnd () == this)
	{
		afxGlobalData.UpdateSysColors ();
	}
}
//*************************************************************************************
void CBORROW_Dialog::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
	CDialog::OnSettingChange(uFlags, lpszSection);
	
	if (AfxGetMainWnd () == this)
	{
		afxGlobalData.OnSettingChange ();
	}
}
//*************************************************************************************
void CBORROW_Dialog::EnableVisualManagerStyle (BOOL bEnable, BOOL bNCArea, const CList<UINT,UINT>* plstNonSubclassedItems)
{
	ASSERT_VALID (this);

	m_Impl.EnableVisualManagerStyle (bEnable, bEnable && bNCArea, plstNonSubclassedItems);

	if (bEnable && bNCArea)
	{
		m_Impl.OnChangeVisualManager ();
	}
}
//*************************************************************************************
BOOL CBORROW_Dialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_Impl.m_bHasBorder = (GetStyle () & WS_BORDER) != 0;

	if (IsVisualManagerStyle ())
	{
		m_Impl.EnableVisualManagerStyle (TRUE, IsVisualManagerNCArea ());
	}

	if (m_Impl.HasAeroMargins ())
	{
		m_Impl.EnableAero (m_Impl.m_AeroMargins);
	}

	if (IsBackstageMode())
	{
		m_Impl.EnableBackstageMode();
	}
	
	Translate(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//*************************************************************************************
BOOL CBORROW_Dialog::EnableAero (AFX_MARGINS& margins)
{
	return m_Impl.EnableAero (margins);
}
//*************************************************************************************
void CBORROW_Dialog::GetAeroMargins (AFX_MARGINS& margins) const
{
	m_Impl.GetAeroMargins (margins);
}
//***************************************************************************
LRESULT CBORROW_Dialog::OnDWMCompositionChanged(WPARAM,LPARAM)
{
	m_Impl.OnDWMCompositionChanged ();
	return 0;
}
//***************************************************************************
void CBORROW_Dialog::OnSize(UINT nType, int cx, int cy) 
{
	BOOL bIsMinimized = (nType == SIZE_MINIMIZED);

	if (m_Impl.IsOwnerDrawCaption ())
	{
		CRect rectWindow;
		GetWindowRect (rectWindow);

		WINDOWPOS wndpos;
		wndpos.flags = SWP_FRAMECHANGED;
		wndpos.x     = rectWindow.left;
		wndpos.y     = rectWindow.top;
		wndpos.cx    = rectWindow.Width ();
		wndpos.cy    = rectWindow.Height ();

		m_Impl.OnWindowPosChanged (&wndpos);
	}

	m_Impl.UpdateCaption ();

	if (!bIsMinimized && nType != SIZE_MAXIMIZED && !m_bWasMaximized)
	{
// 		AdjustControlsLayout();
		return;
	}

	CDialog::OnSize(nType, cx, cy);

	m_bWasMaximized = (nType == SIZE_MAXIMIZED);

// 	AdjustControlsLayout();
}
//**************************************************************************
void CBORROW_Dialog::OnNcPaint() 
{
	if (!m_Impl.OnNcPaint ())
	{
		Default ();
	}
}
//**************************************************************************
void CBORROW_Dialog::OnNcMouseMove(UINT nHitTest, CPoint point) 
{
	m_Impl.OnNcMouseMove (nHitTest, point);
	CDialog::OnNcMouseMove(nHitTest, point);
}
//**************************************************************************
void CBORROW_Dialog::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_Impl.OnLButtonUp (point);
	CDialog::OnLButtonUp(nFlags, point);
}
//**************************************************************************
void CBORROW_Dialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_Impl.OnLButtonDown (point);
	CDialog::OnLButtonDown(nFlags, point);
}
//**************************************************************************
BCGNcHitTestType CBORROW_Dialog::OnNcHitTest(CPoint point) 
{
	BCGNcHitTestType nHit = m_Impl.OnNcHitTest (point);
	if (nHit != HTNOWHERE)
	{
		return nHit;
	}

	return CDialog::OnNcHitTest(point);
}
//**************************************************************************
void CBORROW_Dialog::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	if (!m_Impl.OnNcCalcSize (bCalcValidRects, lpncsp))
	{
		CDialog::OnNcCalcSize(bCalcValidRects, lpncsp);
	}
}
//**************************************************************************
void CBORROW_Dialog::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_Impl.OnMouseMove (point);
	CDialog::OnMouseMove(nFlags, point);
}
//**************************************************************************
void CBORROW_Dialog::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	if ((lpwndpos->flags & SWP_FRAMECHANGED) == SWP_FRAMECHANGED)
	{
		m_Impl.OnWindowPosChanged (lpwndpos);
	}

	CDialog::OnWindowPosChanged(lpwndpos);
}
//**************************************************************************
LRESULT CBORROW_Dialog::OnChangeVisualManager (WPARAM, LPARAM)
{
	m_Impl.OnChangeVisualManager ();
	return 0;
}
//**************************************************************************
void CBORROW_Dialog::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	m_Impl.OnGetMinMaxInfo (lpMMI);
	CDialog::OnGetMinMaxInfo(lpMMI);
}
//****************************************************************************
LRESULT CBORROW_Dialog::OnSetText(WPARAM, LPARAM) 
{
	LRESULT	lRes = Default();

	if (lRes && IsVisualManagerStyle () && IsVisualManagerNCArea ())
	{
		RedrawWindow (NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
	}

	return lRes;
}
//****************************************************************************
int CBORROW_Dialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

    return m_Impl.OnCreate();
}
//****************************************************************************
/*
void CBORROW_Dialog::EnableLayout(BOOL bEnable, CRuntimeClass* pRTC, BOOL bResizeBox)
{
	m_Impl.EnableLayout(bEnable, pRTC, bResizeBox);
}
*/
