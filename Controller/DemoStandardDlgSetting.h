#pragma once

#include "SkinDlg/StandardDialog.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "setdpi.h"

// CDemoStandardDlgSetting dialog

class CDemoStandardDlgSetting : public CStandardDialog
{
	DECLARE_DYNAMIC(CDemoStandardDlgSetting)

public:
	CDemoStandardDlgSetting(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDemoStandardDlgSetting();

// Dialog Data
	enum { IDD = IDD_LOGIN_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();

    CString m_strIP;
    CString m_Port;
    BOOL    m_bCancel;
    CIPAddressCtrl m_ctrlIP;
    CSetDPI m_dpi;
};
