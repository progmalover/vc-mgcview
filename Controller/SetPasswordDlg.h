#pragma once


// CSetPasswordDlg dialog

class CSetPasswordDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetPasswordDlg)

public:
	CSetPasswordDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetPasswordDlg();

// Dialog Data
	enum { IDD = IDD_SET_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
    virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()

public:
    CString m_Password;
    CString m_Password2;
    afx_msg void OnBnClickedOk();
};
