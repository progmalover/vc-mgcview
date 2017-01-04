#pragma once
#include "afxwin.h"

// CScheduleItemCtrl dialog

class ClientManagerAddGroupDlg : public CDialogEx
{
    DECLARE_DYNAMIC(ClientManagerAddGroupDlg)
    DECLARE_EASYSIZE
public:
    ClientManagerAddGroupDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~ClientManagerAddGroupDlg();

// Dialog Data
    enum { IDD = IDD_ADD_GROUP };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);

    DECLARE_MESSAGE_MAP()
public:

private:

public:
    CString m_GroupName;
    CString m_GroupDesc;
};
