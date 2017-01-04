// LayoutRenameCtrl.cpp
//

#include "stdafx.h"
#include "Controller.h"
#include "LayoutRenameCtrl.h"
#include "Localization/Localization.h"

// CLayoutRenameCtrl

IMPLEMENT_DYNAMIC(CLayoutRenameCtrl, CDialogEx)

CLayoutRenameCtrl::CLayoutRenameCtrl(CWnd* pParent /*=NULL*/, CString WindowName, CString defaultName)
    : CDialogEx(CLayoutRenameCtrl::IDD, pParent)
{
    m_szLayoutName = defaultName;
    m_szWindowName = WindowName;
}

CLayoutRenameCtrl::~CLayoutRenameCtrl()
{
}

void CLayoutRenameCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_NAME, m_szLayoutName);
}


BEGIN_MESSAGE_MAP(CLayoutRenameCtrl, CDialogEx)

    ON_BN_CLICKED(IDOK, &CLayoutRenameCtrl::OnBnClickedOk)
END_MESSAGE_MAP()


BOOL CLayoutRenameCtrl::OnInitDialog()
{
    CDialogEx::OnInitDialog();

	Translate(this);

    SetWindowTextW(m_szWindowName);

    return TRUE;
}


// CLayoutRenameCtrl Message Handle


void CLayoutRenameCtrl::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
	//check layout name is invalid
	const CString::PCXSTR interpunction = _T("\\/:*?\"<>|");
	int i;
	CString szTmpLayoutName;
    GetDlgItem(IDC_EDIT_NAME)->GetWindowText(szTmpLayoutName);
    szTmpLayoutName.Trim();
    if (szTmpLayoutName.IsEmpty())
    {
        MessageBox(Translate(_T("New Name should not Null!")), Translate(_T("Warning:Rename layout")), MB_OK|MB_ICONERROR);
        return;
    }
	while ((i = szTmpLayoutName.FindOneOf(interpunction)) != -1)
	{
		MessageBox(Translate(_T("Layout name contains invalid character(s).")),Translate(_T("Warning:Rename layout")), MB_OK|MB_ICONINFORMATION);
		return;
	}

    GetDlgItem(IDC_EDIT_NAME)->GetWindowText(m_szLayoutName);

    CDialogEx::OnOK();
}
