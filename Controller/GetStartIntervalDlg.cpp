// CGetStartIntervalDlg.cpp : 
//

#include "stdafx.h"
#include "Controller.h"
#include "GetStartIntervalDlg.h"
#include "afxdialogex.h"


// CGetStartIntervalDlg 

IMPLEMENT_DYNAMIC(CGetStartIntervalDlg, CDialogEx)

CGetStartIntervalDlg::CGetStartIntervalDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGetStartIntervalDlg::IDD, pParent)
	, m_IntervalTime(_T(""))
{

}

CGetStartIntervalDlg::~CGetStartIntervalDlg()
{
}

void CGetStartIntervalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	 DDX_Text(pDX, IDC_EDT_START_INTERVAL, m_IntervalTime);
}

BOOL CGetStartIntervalDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_EDT_START_INTERVAL)->SetFocus();

	//Translate(this);
	return FALSE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CGetStartIntervalDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CGetStartIntervalDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CGetStartIntervalDlg 


void CGetStartIntervalDlg::OnBnClickedOk()
{
	// TODO: 
	UpdateData(TRUE);

	std::wstring str = m_IntervalTime.GetString();

	if( StringUtility::stoi(str) > 3600)
	{
		//AfxMessageBox(Translate(_T("m_IntervalTime must less than 3600 second !")), MB_ICONQUESTION);
		MessageBox(Translate(_T("Interval time must less than 3600 seconds !")),Translate(_T("Warning")), MB_OK);
		return;
	}
	
	CDialogEx::OnOK();
}
