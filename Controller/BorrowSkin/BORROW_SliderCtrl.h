#if !defined(AFX_BCGPSLIDERCTRL_H__D54F6105_98FA_4CD8_B021_C6862EEE5968__INCLUDED_)
#define AFX_BCGPSLIDERCTRL_H__D54F6105_98FA_4CD8_B021_C6862EEE5968__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BORROW_Pro.h"

/////////////////////////////////////////////////////////////////////////////
// CBORROW_SliderCtrl window

class BORROW_DLLEXPORT CBORROW_SliderCtrl : public CSliderCtrl
{
	DECLARE_DYNAMIC(CBORROW_SliderCtrl)

// Construction
public:
	CBORROW_SliderCtrl();

// Attributes
public:
	BOOL	m_bOnGlass;
	BOOL	m_bVisualManagerStyle;
	BOOL	m_bDrawFocus;

protected:
	BOOL	m_bIsThumbHighligted;
	BOOL	m_bIsThumPressed;
	BOOL	m_bTracked;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBORROW_SliderCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBORROW_SliderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBORROW_SliderCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCancelMode();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnBCGSetControlVMMode (WPARAM, LPARAM);
	afx_msg LRESULT OnBCGSetControlAero (WPARAM, LPARAM);
	afx_msg LRESULT OnMouseLeave(WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPSLIDERCTRL_H__D54F6105_98FA_4CD8_B021_C6862EEE5968__INCLUDED_)
