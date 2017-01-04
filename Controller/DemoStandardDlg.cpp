// MyContentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DemoStandardDlg.h"
#include "DemoTranslucentDlgSetting.h"
#include "Localization/Localization.h"
#include "Controller.h"
// CDemoStandardDlg dialog

IMPLEMENT_DYNAMIC(CDemoStandardDlg, CStandardDialog)
CDemoStandardDlg::CDemoStandardDlg(CWnd* pParent/* = NULL*/)
	: CStandardDialog(CDemoStandardDlg::IDD, RGB(0, 255, 0), pParent)
    , m_Username(_T(""))
    , m_Password(_T(""))
    , m_strIP(_T("127.0.0.1"))
    , m_Port(_T("2000"))
    , m_bCancel (TRUE)
{
}

CDemoStandardDlg::~CDemoStandardDlg()
{
}

void CDemoStandardDlg::DoDataExchange(CDataExchange* pDX)
{
	CStandardDialog::DoDataExchange(pDX);

    DDX_Text(pDX, IDC_EDIT_USERNAME, m_Username);
    DDX_Text(pDX, IDC_EDIT_PASSWORD, m_Password);
}


BEGIN_MESSAGE_MAP(CDemoStandardDlg, CStandardDialog)
    //ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BTN_SETTING_LOGIN, &CDemoStandardDlg::OnBnClickedBtnSettingLogin)
END_MESSAGE_MAP()


// CDemoStandardDlg message handlers

void CDemoStandardDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	ShowXMessageBox(NULL, Translate(_T("Hello")), Translate(_T("Hello World")), MB_OK);
}

void CDemoStandardDlg::OnOK()
{
    m_bCancel = FALSE;
	CStandardDialog::OnOK();
}

BOOL CDemoStandardDlg::OnInitDialog()
{
	CStandardDialog::OnInitDialog();
    m_dpi.Attach(AfxFindResourceHandle(MAKEINTRESOURCE(IDD), RT_DIALOG), m_hWnd, IDD);

    GetDlgItem(IDC_EDIT_USERNAME)->SetFocus();

	Translate(this);
	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDemoStandardDlg::OnBnClickedBtnSettingLogin()
{
    // TODO: Add your control notification handler code here

    CDemoTranslucentDlgSetting dlg(IDB_LOGIN_BG);

    dlg.DoModal();
}
