#pragma once
#include "amsEdit.h"
class CScheduleVerticallyCopyDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CScheduleVerticallyCopyDlg)
    DECLARE_EASYSIZE
public:
    CScheduleVerticallyCopyDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CScheduleVerticallyCopyDlg();

// Dialog Data
    enum { IDD = IDD_VERTICALLYCOPY };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);

    DECLARE_MESSAGE_MAP()
public:

private:

public:
    int m_VerticallyCnt;
	CAMSIntegerEdit m_CVerticallyCnt;
	CString m_szVerticallyCnt;
	afx_msg void OnBnClickedOk();
};
