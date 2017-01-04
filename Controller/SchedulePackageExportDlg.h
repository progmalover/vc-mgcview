#pragma once
#include "Controller.h"
#include "afxcmn.h"

// CScheduleItemCtrl dialog

class CSchedulePackageExportDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CSchedulePackageExportDlg)
    DECLARE_EASYSIZE
public:
    CSchedulePackageExportDlg(CWnd* pParent);   // standard constructor
    virtual ~CSchedulePackageExportDlg();


// Dialog Data
    enum { IDD = IDD_SCHEDULEPACKAGE_EXPORT };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);

    DECLARE_MESSAGE_MAP()
public:

private:

public:
    CString m_SchedulePackagePath;
    CString m_SchedulePackageName;
};
