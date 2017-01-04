#pragma once
#include "afxcmn.h"
#include "SimpleBrowser.h"


// CLayoutWarningCtrl dialog

class CLayoutWarningCtrl : public CDialogEx
{
    DECLARE_DYNAMIC(CLayoutWarningCtrl)
    DECLARE_EASYSIZE
public:
    CLayoutWarningCtrl(CWnd* pParent = NULL);   // standard constructor
    virtual ~CLayoutWarningCtrl();

    // Dialog Data
    enum { IDD = IDD_LAYOUT_WARNING };

    HRESULT SetWarning(CString szInfo);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);

    DECLARE_MESSAGE_MAP()

public:

    CWnd*                       m_pParent;
    SimpleBrowser               m_Browser;
    CString                     m_szWarning;

};