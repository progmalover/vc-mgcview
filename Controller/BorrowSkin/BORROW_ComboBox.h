#if !defined(AFX_BCGPCOMBOBOX_H__B809A10B_3085_419E_8ADA_6AA9A852CA73__INCLUDED_)
#define AFX_BCGPCOMBOBOX_H__B809A10B_3085_419E_8ADA_6AA9A852CA73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BORROW_Pro.h"
// #include "BCGPEdit.h"

/////////////////////////////////////////////////////////////////////////////
// BORROW_ComboBox window

class BORROW_DLLEXPORT CBORROW_ComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CBORROW_ComboBox)

// Construction
public:
	CBORROW_ComboBox();

// Attributes
public:

protected:
	CMFCEditBrowseCtrl	m_wndEdit;
	BOOL		m_bIsDroppedDown;
	CRect		m_rectBtn;
	BOOL		m_bIsButtonHighlighted;
	BOOL		m_bTracked;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BORROW_ComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBORROW_ComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(BORROW_ComboBox)
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg BOOL OnCloseup();
	afx_msg BOOL OnDropdown();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPCOMBOBOX_H__B809A10B_3085_419E_8ADA_6AA9A852CA73__INCLUDED_)
