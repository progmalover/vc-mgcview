#pragma once
#include "afxwin.h"

// CLayoutAddGroupCtrl dialog

class CLayoutAddGroupCtrl : public CDialogEx
{
    DECLARE_DYNAMIC(CLayoutAddGroupCtrl)
    DECLARE_EASYSIZE

public:
    CLayoutAddGroupCtrl(CWnd* pParent = NULL);   // standard constructor
    virtual ~CLayoutAddGroupCtrl();

// Dialog Data
    enum { IDD = IDD_ADD_GROUP };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedOk();
    afx_msg void OnEnChangeEditName();

    DECLARE_MESSAGE_MAP()


public:
    CString m_szGroupName;
    CString m_szGroupDesc;
};
