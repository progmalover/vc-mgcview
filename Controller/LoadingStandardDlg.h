#pragma once

#include "SkinDlg/StandardDialog.h"
#include "afxwin.h"

// CLoadingStandardDlg dialog

class CLoadingStandardDlg : public CStandardDialog
{
	DECLARE_DYNAMIC(CLoadingStandardDlg)

public:
	CLoadingStandardDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoadingStandardDlg();

// Dialog Data
	enum { IDD = IDD_LOADING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnTimer(UINT_PTR);
    virtual void OnOK();
    virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

    INT nCnt;
    CString m_LoadingStr;
};
