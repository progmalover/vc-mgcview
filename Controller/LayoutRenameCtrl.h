#pragma once
#include "afxcmn.h"

// CLayoutRenameCtrl

class CLayoutRenameCtrl : public CDialogEx
{
    DECLARE_DYNAMIC(CLayoutRenameCtrl)

public:
    CLayoutRenameCtrl(CWnd* pParent = NULL, CString WindowName = Translate(_T("Rename Layout")), CString defaultName = Translate(_T("")));
    virtual ~CLayoutRenameCtrl();

    // Dialog Data
    enum { IDD = IDD_RENAME_LAYOUT };

    void GetLayoutNewName(CString& szName) { szName = m_szLayoutName; };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

private:
    CString m_szLayoutName;
    CString m_szWindowName;

public:
    afx_msg void OnBnClickedOk();
};
