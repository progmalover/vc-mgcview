#pragma once

#include "SkinDlg/StandardDialog.h"
#include "afxwin.h"
#include "setdpi.h"

// CDemoStandardDlg dialog

class CDemoStandardDlg : public CStandardDialog
{
	DECLARE_DYNAMIC(CDemoStandardDlg)

public:
	CDemoStandardDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDemoStandardDlg();

// Dialog Data
	enum { IDD = IDD_LOGIN_SKIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();

    CString m_Username;
    CString m_Password;
    CString m_strIP;
    CString m_Port;
    BOOL    m_bCancel;
    afx_msg void OnBnClickedBtnSettingLogin();
    CSetDPI m_dpi;
};
