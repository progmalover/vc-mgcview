#pragma once
#include "Controller.h"
#include "afxcmn.h"

// CScheduleItemCtrl dialog

class CSchedulePackageImportDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CSchedulePackageImportDlg)
    DECLARE_EASYSIZE
public:
    CSchedulePackageImportDlg(CWnd* pParent);   // standard constructor
    virtual ~CSchedulePackageImportDlg();


// Dialog Data
    enum { IDD = IDD_SCHEDULEPACKAGE_IMPORT };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);

    DECLARE_MESSAGE_MAP()
public:

private:

public:
    CString m_SchedulePackagePathName;
};
