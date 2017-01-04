// SetPasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "SetPasswordDlg.h"
#include "afxdialogex.h"
#include "Localization/Localization.h"

// CSetPasswordDlg dialog

IMPLEMENT_DYNAMIC(CSetPasswordDlg, CDialogEx)

CSetPasswordDlg::CSetPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetPasswordDlg::IDD, pParent)
    , m_Password(_T(""))
    , m_Password2(_T(""))
{

}

CSetPasswordDlg::~CSetPasswordDlg()
{
}



void CSetPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_SETPASS, m_Password);
    DDX_Text(pDX, IDC_EDIT_SETPASS_CONFIRM, m_Password2);
}


BEGIN_MESSAGE_MAP(CSetPasswordDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CSetPasswordDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CSetPasswordDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//GetDlgItem(IDC_EDIT_USERNAME)->SetFocus();

	Translate(this);
	return FALSE;  
}

// CSetPasswordDlg message handlers


void CSetPasswordDlg::OnBnClickedOk()
{
    UpdateData();
    if(m_Password.Compare(m_Password2) != 0)
    {
        MessageBox(Translate(_T("Password doesn't match!")), Translate(_T("Error")), MB_OK);
        return;
    }
    if (m_Password.GetLength() == 0)
    {
        MessageBox(Translate(_T("Password can't be empty")), Translate(_T("Error")), MB_OK);
        return;
    }
    
    CDialogEx::OnOK();
}
