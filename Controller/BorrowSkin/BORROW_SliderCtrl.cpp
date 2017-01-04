//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of BCGControlBar Library Professional Edition
// Copyright (C) 1998-2010 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
// BCGPSliderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ControllerVisualManager.h"
#include "BORROW_SliderCtrl.h"
#include "BORROW_TrackMouse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBORROW_SliderCtrl

IMPLEMENT_DYNAMIC(CBORROW_SliderCtrl, CSliderCtrl)

CBORROW_SliderCtrl::CBORROW_SliderCtrl()
{
	m_bVisualManagerStyle = FALSE;
	m_bOnGlass = FALSE;
	m_bTracked = FALSE;
	m_bIsThumbHighligted = FALSE;
	m_bIsThumPressed = FALSE;
}

CBORROW_SliderCtrl::~CBORROW_SliderCtrl()
{
}

BEGIN_MESSAGE_MAP(CBORROW_SliderCtrl, CSliderCtrl)
	//{{AFX_MSG_MAP(CBORROW_SliderCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_CANCELMODE()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBORROW_SliderCtrl message handlers

BOOL CBORROW_SliderCtrl::OnEraseBkgnd(CDC* /*pDC*/)
{
	if (!m_bVisualManagerStyle)
	{
		return (BOOL) Default ();
	}

	return TRUE;
}
//**************************************************************************
LRESULT CBORROW_SliderCtrl::OnBCGSetControlVMMode (WPARAM wp, LPARAM)
{
	m_bVisualManagerStyle = (BOOL) wp;
	RedrawWindow ();
	return 0;
}
//**************************************************************************
LRESULT CBORROW_SliderCtrl::OnBCGSetControlAero (WPARAM wp, LPARAM)
{
	m_bOnGlass = (BOOL) wp;
	return 0;
}
//**************************************************************************
void CBORROW_SliderCtrl::OnCancelMode() 
{
	CSliderCtrl::OnCancelMode();

	m_bIsThumbHighligted = FALSE;
	m_bIsThumPressed = FALSE;

	RedrawWindow ();
}
//*****************************************************************************************
void CBORROW_SliderCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	BOOL bIsThumbHighligted = m_bIsThumbHighligted;

	CRect rectThumb;
	GetThumbRect (rectThumb);

	m_bIsThumbHighligted = rectThumb.PtInRect (point);

	CSliderCtrl::OnMouseMove(nFlags, point);

	if (bIsThumbHighligted != m_bIsThumbHighligted)
	{
		RedrawWindow ();
	}

	if (!m_bTracked)
	{
		m_bTracked = TRUE;
		
		TRACKMOUSEEVENT trackmouseevent;
		trackmouseevent.cbSize = sizeof(trackmouseevent);
		trackmouseevent.dwFlags = TME_LEAVE;
		trackmouseevent.hwndTrack = GetSafeHwnd();
		trackmouseevent.dwHoverTime = HOVER_DEFAULT;
		::BORROW_TrackMouse (&trackmouseevent);	
	}
}
//*****************************************************************************************
LRESULT CBORROW_SliderCtrl::OnMouseLeave(WPARAM,LPARAM)
{
	m_bTracked = FALSE;

	if (m_bIsThumbHighligted)
	{
		m_bIsThumbHighligted = FALSE;
		RedrawWindow ();
	}

	return 0;
}
//*****************************************************************************************
void CBORROW_SliderCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CMemDC memDC (dc, this);
	CDC* pDC = &memDC.GetDC ();

// 	if (!CMFCVisualManager::GetInstance ()->OnFillParentBarBackground (this, pDC))
	{
		afxGlobalData.DrawParentBackground (this, pDC, NULL);
	}

	DWORD dwStyle = GetStyle ();
	BOOL bVert = (dwStyle & TBS_VERT);
	BOOL bLeftTop = (dwStyle & TBS_BOTH) || (dwStyle & TBS_LEFT);
	BOOL bRightBottom = (dwStyle & TBS_BOTH) || ((dwStyle & TBS_LEFT) == 0);
	BOOL bIsFocused = GetSafeHwnd () == ::GetFocus ();

	CRect rectChannel;
	GetChannelRect (rectChannel);

	if (bVert)
	{
		CRect rect = rectChannel;

		rectChannel.left = rect.top;
		rectChannel.right = rect.bottom;
		rectChannel.top = rect.left;
		rectChannel.bottom = rect.right;
	}

    ((CControllerVisualManager*)CMFCVisualManager::GetInstance())->OnDrawSliderChannel (pDC, this, bVert, rectChannel, m_bOnGlass);

	CRect rectThumb;
	GetThumbRect (rectThumb);

	((CControllerVisualManager*)CMFCVisualManager::GetInstance())->OnDrawSliderThumb (
		pDC, this, rectThumb, m_bIsThumbHighligted || bIsFocused,
		m_bIsThumPressed, !IsWindowEnabled (),
		bVert, bLeftTop, bRightBottom, m_bOnGlass);

	if (bIsFocused && m_bDrawFocus)
	{
		CRect rectFocus;
		GetClientRect (rectFocus);
		pDC->DrawFocusRect (rectFocus);
	}
}
//*****************************************************************************************
void CBORROW_SliderCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bIsThumPressed)
	{
		m_bIsThumPressed = FALSE;
		RedrawWindow ();
	}
	
	CSliderCtrl::OnLButtonUp(nFlags, point);
}
//*****************************************************************************************
void CBORROW_SliderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rectThumb;
	GetThumbRect (rectThumb);

	m_bIsThumPressed = rectThumb.PtInRect (point);
	
	if (m_bIsThumPressed)
	{
		RedrawWindow ();
	}
	
	CSliderCtrl::OnLButtonDown(nFlags, point);
}
