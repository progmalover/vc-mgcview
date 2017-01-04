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
// BCGPDlgImpl.h: interface for the CBORROW_DlgImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPDLGIMPL_H__18772215_4E74_4900_82E4_288CA46AB7E0__INCLUDED_)
#define AFX_BCGPDLGIMPL_H__18772215_4E74_4900_82E4_288CA46AB7E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BORROW_Pro.h"
#include "ControllerVisualManager.h"
// #include "VisualManager.h"
// #include "BCGPLayout.h"

class CMFCPopupMenu;
class CMFCCaptionButtonEx;

class BORROW_DLLEXPORT CBORROW_DlgImpl  
{
	friend class CBORROW_Dialog;
	friend class CMFCPropertyPage;
	friend class CMFCPropertySheet;
/*
	friend class CBCGPDialogBar;
	friend class CBCGPFormView;
*/

protected:
	CBORROW_DlgImpl(CWnd& dlg);
	virtual ~CBORROW_DlgImpl();

	static LRESULT CALLBACK BCGDlgMouseProc (int nCode, WPARAM wParam, LPARAM lParam);

	void SetActiveMenu (CMFCPopupMenu* pMenu);

	BOOL ProcessMouseClick (POINT pt);
	BOOL ProcessMouseMove (POINT pt);

	BOOL PreTranslateMessage(MSG* pMsg);
	BOOL OnCommand (WPARAM wParam, LPARAM lParam);
	void OnNcActivate (BOOL& bActive);
	void OnActivate(UINT nState, CWnd* pWndOther);

	void EnableVisualManagerStyle (BOOL bEnable, BOOL bNCArea = FALSE, const CList<UINT,UINT>* plstNonSubclassedItems = NULL);

	void OnDestroy ();
	void OnDWMCompositionChanged ();

	BOOL EnableAero (AFX_MARGINS& margins);
	void GetAeroMargins (AFX_MARGINS& margins) const;
	BOOL HasAeroMargins () const;
	void ClearAeroAreas (CDC* pDC);

	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	BOOL OnNcPaint();
	BOOL OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	void UpdateCaption ();
	void UpdateCaptionButtons ();
	UINT OnNcHitTest (CPoint point);
	void OnNcMouseMove(UINT nHitTest, CPoint point);
	void OnLButtonDown(CPoint point);
	void OnLButtonUp(CPoint point);
	void OnMouseMove(CPoint point);
	CMFCCaptionButtonEx* GetSysButton (UINT nHit);

	void SetHighlightedSysButton (UINT nHitTest);
	void OnTrackCaptionButtons (CPoint point);
	void StopCaptionButtonsTracking ();
	void RedrawCaptionButton (CMFCCaptionButtonEx* pBtn);

	BOOL IsOwnerDrawCaption ()
	{
/*
#if (!defined _BCGSUITE_) && (!defined _BCGSUITE_INC_)
		return m_bVisualManagerStyle && m_bVisualManagerNCArea && visualManager->IsOwnerDrawCaption ();
#else
*/
		return m_bVisualManagerStyle && m_bVisualManagerNCArea && visualManager->IsOwnerDrawCaption ();
// #endif
	}

	CRect GetCaptionRect ();
	void OnChangeVisualManager ();
	void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	void OnGetMinMaxInfo (MINMAXINFO FAR* lpMMI);

	int GetCaptionHeight ();
/*
	void EnableLayout(BOOL bEnable = TRUE, CRuntimeClass* pRTC = NULL, BOOL bResizeBox = TRUE);

	BOOL IsLayoutEnabled() const
	{
		return m_pLayout != NULL;
	}
*/

	int OnCreate();

	void DrawResizeBox(CDC* pDC);
// 	void AdjustControlsLayout();

	void EnableBackstageMode();

	CWnd&					m_Dlg;
	static HHOOK			m_hookMouse;
	static CBORROW_DlgImpl*	m_pMenuDlgImpl;
	BOOL					m_bVisualManagerStyle;
	BOOL					m_bVisualManagerNCArea;
	CArray<CWnd*, CWnd*>	m_arSubclassedCtrls;
	AFX_MARGINS				m_AeroMargins;
	BOOL					m_bTransparentStaticCtrls;
	CObList					m_lstCaptionSysButtons;
	UINT					m_nHotSysButton;
	UINT					m_nHitSysButton;
	CRect					m_rectRedraw;
	BOOL					m_bWindowPosChanging;
	BOOL					m_bIsWindowRgn;
	BOOL					m_bHasBorder;
	BOOL					m_bIsWhiteBackground;

	CList<UINT,UINT>		m_lstNonSubclassedItems;

// 	CBCGPControlsLayout*	m_pLayout;
	MINMAXINFO				m_LayoutMMI;
	CRect					m_rectResizeBox;
	BOOL					m_bResizeBox;
	BOOL					m_bBackstageMode;
};

extern BORROW_DLLEXPORT UINT BCGM_ONSETCONTROLAERO;
extern BORROW_DLLEXPORT UINT BCGM_ONSETCONTROLVMMODE;
extern BORROW_DLLEXPORT UINT BCGM_ONSETCONTROLBACKSTAGEMODE;

#endif // !defined(AFX_BCGPDLGIMPL_H__18772215_4E74_4900_82E4_288CA46AB7E0__INCLUDED_)
