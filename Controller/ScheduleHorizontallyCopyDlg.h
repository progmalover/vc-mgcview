#pragma once
#include "atlcomtime.h"
#include "afxwin.h"

class CScheduleHorizontallyCopyDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CScheduleHorizontallyCopyDlg)
    DECLARE_EASYSIZE
public:
    CScheduleHorizontallyCopyDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CScheduleHorizontallyCopyDlg();

// Dialog Data
    enum { IDD = IDD_HORIZONTALLYCOPY };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedHcopySelectall();

    void UpdateOKBtn();

    DECLARE_MESSAGE_MAP()
public:

private:

public:
    COleDateTime m_StartTime;
    COleDateTime m_EndTime;
    BOOL m_bSelectAll;
    BOOL m_bMonday;
    BOOL m_bTuesday;
    BOOL m_bWednesday;
    BOOL m_bThursday;
    BOOL m_bFriday;
    BOOL m_bSaturday;
    BOOL m_bSunday;
    CButton m_ctrlOK;
    afx_msg void OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDtnDatetimechangeDatetimepicker2(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedMonday();
    afx_msg void OnBnClickedTuesday();
    afx_msg void OnBnClickedWednesday();
    afx_msg void OnBnClickedThursday();
    afx_msg void OnBnClickedFriday();
    afx_msg void OnBnClickedSaturday();
    afx_msg void OnBnClickedSunday();
};
