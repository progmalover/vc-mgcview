#if !defined(AFX_BCGPPROGRESSCTRL_H__23E0C43A_6498_403C_AB8F_7A395D9C2FA4__INCLUDED_)
#define AFX_BCGPPROGRESSCTRL_H__23E0C43A_6498_403C_AB8F_7A395D9C2FA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BORROW_Pro.h"

/////////////////////////////////////////////////////////////////////////////
// CBORROW_ProgressCtrl window

class BORROW_DLLEXPORT CBORROW_ProgressCtrl : public CProgressCtrl
{
	DECLARE_DYNAMIC(CBORROW_ProgressCtrl)

// Construction
public:
	CBORROW_ProgressCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBORROW_ProgressCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBORROW_ProgressCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBORROW_ProgressCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPPROGRESSCTRL_H__23E0C43A_6498_403C_AB8F_7A395D9C2FA4__INCLUDED_)
