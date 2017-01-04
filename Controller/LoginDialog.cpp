// LoginDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "LoginDialog.h"
#include "afxdialogex.h"
#include "Localization/Localization.h"

// CLoginDialog dialog

IMPLEMENT_DYNAMIC(CLoginDialog, CDialogEx)

CLoginDialog::CLoginDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDialog::IDD, pParent)
    , m_Username(_T(""))
    , m_Password(_T(""))
{

}

CLoginDialog::~CLoginDialog()
{
}

void CLoginDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_USERNAME, m_Username);
    DDX_Text(pDX, IDC_EDIT_PASSWORD, m_Password);
}

BOOL CLoginDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_EDIT_USERNAME)->SetFocus();

	Translate(this);
	return FALSE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CLoginDialog, CDialogEx)
END_MESSAGE_MAP()


// CLoginDialog message handlers
