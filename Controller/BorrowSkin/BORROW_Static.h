#if !defined(AFX_BCGPSTATIC_H__A34CC2CA_E7ED_4D03_93F4_E2513841C22C__INCLUDED_)
#define AFX_BCGPSTATIC_H__A34CC2CA_E7ED_4D03_93F4_E2513841C22C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BORROW_Pro.h"

/////////////////////////////////////////////////////////////////////////////
// CBORROW_Static window

class BORROW_DLLEXPORT CBORROW_Static : public CStatic
{
	DECLARE_DYNAMIC(CBORROW_Static)

// Construction
public:
	CBORROW_Static();

// Attributes
public:
	BOOL		m_bOnGlass;
	BOOL		m_bVisualManagerStyle;
	BOOL		m_bBackstageMode;
	COLORREF	m_clrText;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBORROW_Static)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBORROW_Static();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBORROW_Static)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnNcPaint();
	//}}AFX_MSG
	afx_msg LRESULT OnSetText (WPARAM, LPARAM lp);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPSTATIC_H__A34CC2CA_E7ED_4D03_93F4_E2513841C22C__INCLUDED_)
