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
// BCGPProgressCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ControllerVisualManager.h"
#include "BORROW_ProgressCtrl.h"

#define visualManagerMFC	CMFCVisualManager::GetInstance ()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBORROW_ProgressCtrl

IMPLEMENT_DYNAMIC(CBORROW_ProgressCtrl, CProgressCtrl)

CBORROW_ProgressCtrl::CBORROW_ProgressCtrl()
{
}

CBORROW_ProgressCtrl::~CBORROW_ProgressCtrl()
{
}

BEGIN_MESSAGE_MAP(CBORROW_ProgressCtrl, CProgressCtrl)
	//{{AFX_MSG_MAP(CBORROW_ProgressCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBORROW_ProgressCtrl message handlers

BOOL CBORROW_ProgressCtrl::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}
//**************************************************************************
void CBORROW_ProgressCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CMemDC memDC (dc, this);
	CDC* pDC = &memDC.GetDC ();

    //if (!CMFCVisualManager::GetInstance()->OnFillParentBarBackground (this, pDC))
	{
		afxGlobalData.DrawParentBackground (this, pDC);
	}

	CRect rect;
	GetClientRect (rect);

	CRect rectProgress = rect;
	rectProgress.DeflateRect (1, 1);

	int nMin = 0;
	int nMax = 0;
	GetRange (nMin, nMax);

	CRect rectChunk (0, 0, 0, 0);
	
	if (nMax > nMin)
	{
		rectChunk = rectProgress;
		rectChunk.right = rectChunk.left + 
			GetPos () * rectChunk.Width () / (nMax - nMin);

		rectChunk.DeflateRect (1, 1);
	}
    CMFCRibbonProgressBar dummy;
	visualManagerMFC->OnDrawRibbonProgressBar (
		pDC, &dummy, rectProgress, rectChunk, FALSE);
}
//**************************************************************************
void CBORROW_ProgressCtrl::OnNcCalcSize(BOOL /*bCalcValidRects*/, NCCALCSIZE_PARAMS FAR* /*lpncsp*/) 
{
}
//**************************************************************************
void CBORROW_ProgressCtrl::OnNcPaint() 
{
}
