#pragma once


// CGetStartIntervalDlg 

class CGetStartIntervalDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGetStartIntervalDlg)

public:
	CGetStartIntervalDlg(CWnd* pParent = NULL);   // 
	virtual ~CGetStartIntervalDlg();

// 
	enum { IDD = IDD_GET_START_INTERVAL };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	 CString m_IntervalTime;

	afx_msg void OnBnClickedOk();
};
