#pragma once
#include "afxwin.h"


// CDesignerGridSetting dialog

class CDesignerGridSetting : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CDesignerGridSetting)

public:
	CDesignerGridSetting();   // standard constructor
	virtual ~CDesignerGridSetting();

// Dialog Data
	enum { IDD = IDD_DESIGNER_GRID };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:

    int m_XSpace;
    int m_YSpace;
    BOOL m_bShowGrid;
    BOOL m_bSnapToGrid;
    afx_msg void OnEnChangeEditX();
    afx_msg void OnEnChangeEditY();
    afx_msg void OnBnClickedCheckShow();
    afx_msg void OnBnClickedCheckSnap();
	afx_msg void OnEnKillfocusEditX();
	afx_msg void OnEnKillfocusEditY();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CEdit m_XEdit;
	CEdit m_YEdit;
};
extern UINT GRID_YSPACESETTINGINVALID;
extern UINT GRID_XSPACESETTINGINVALID;