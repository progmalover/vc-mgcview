#if !defined(AFX_BCGPGROUP_H__917CCAFD_2ED2_44EC_8297_0C3A5C62F28F__INCLUDED_)
#define AFX_BCGPGROUP_H__917CCAFD_2ED2_44EC_8297_0C3A5C62F28F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BORROW_Pro.h"

/////////////////////////////////////////////////////////////////////////////
// CBORROW_Group window

class BORROW_DLLEXPORT CBORROW_Group : public CButton
{
	DECLARE_DYNAMIC(CBORROW_Group)

// Construction
public:
	CBORROW_Group();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBORROW_Group)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBORROW_Group();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBORROW_Group)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
	afx_msg LRESULT OnUpdateUIState (WPARAM, LPARAM);
	afx_msg LRESULT OnSetText (WPARAM, LPARAM lp);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPGROUP_H__917CCAFD_2ED2_44EC_8297_0C3A5C62F28F__INCLUDED_)
