#pragma once

class CScheduleContinuousDeleteDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CScheduleContinuousDeleteDlg)
    DECLARE_EASYSIZE
public:
    CScheduleContinuousDeleteDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CScheduleContinuousDeleteDlg();

// Dialog Data
    enum { IDD = IDD_CONTINUOUS_DEL };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedRadioHorizontally();
    afx_msg void OnBnClickedRadioVertically();

    DECLARE_MESSAGE_MAP()
public:
    int m_ReoccurencCnt;
    bool m_bHorizontally;
};
