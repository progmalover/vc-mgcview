// LayoutWarningCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "LayoutWarningCtrl.h"
#include "Localization/Localization.h"

// CLayoutWarningCtrl dialog

IMPLEMENT_DYNAMIC(CLayoutWarningCtrl, CDialogEx)

CLayoutWarningCtrl::CLayoutWarningCtrl(CWnd* pParent /*=NULL*/)
    : CDialogEx(CLayoutWarningCtrl::IDD, pParent)
{
    m_pParent       = pParent;
    m_szWarning     = _T("");
}

CLayoutWarningCtrl::~CLayoutWarningCtrl()
{
    m_pParent = NULL;

}

void CLayoutWarningCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

}


BEGIN_MESSAGE_MAP(CLayoutWarningCtrl, CDialogEx)
    ON_WM_SIZE()
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CLayoutWarningCtrl)

END_EASYSIZE_MAP


// COpenLayoutCtrl message handlers
BOOL CLayoutWarningCtrl::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    INIT_EASYSIZE;

    m_Browser.CreateFromControl(this, IDC_STATIC_WARNING);

    m_Browser.Write(m_szWarning);

	Translate(this);
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLayoutWarningCtrl::OnSize(UINT nType, int cx, int cy) 
{
    CDialogEx::OnSize(nType, cx, cy);
    UPDATE_EASYSIZE;
}

HRESULT CLayoutWarningCtrl::SetWarning(CString szInfo)
{
    HRESULT hr = S_OK;

    m_szWarning = szInfo;

    return hr;
}
