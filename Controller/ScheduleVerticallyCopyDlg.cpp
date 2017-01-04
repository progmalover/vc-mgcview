#include "stdafx.h"
#include "Controller.h"
#include "ScheduleVerticallyCopyDlg.h"
#include "SchedulerPage.h"
#include "afxdialogex.h"
#include "ScheduleCtrl.h"
#include "Localization/Localization.h"

IMPLEMENT_DYNAMIC(CScheduleVerticallyCopyDlg, CDialogEx)

CScheduleVerticallyCopyDlg::CScheduleVerticallyCopyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScheduleVerticallyCopyDlg::IDD, pParent)
    , m_VerticallyCnt (0)
	, m_szVerticallyCnt (_T("0"))
{

}

CScheduleVerticallyCopyDlg::~CScheduleVerticallyCopyDlg()
{
}

void CScheduleVerticallyCopyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Text(pDX, IDC_VERTICALLYCOPY_CNT, m_VerticallyCnt);
	DDX_Control(pDX, IDC_VERTICALLYCOPY_CNT, m_CVerticallyCnt);
	DDX_Text(pDX, IDC_VERTICALLYCOPY_CNT, m_szVerticallyCnt);
}


BEGIN_MESSAGE_MAP(CScheduleVerticallyCopyDlg, CDialogEx)
    ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CScheduleVerticallyCopyDlg::OnBnClickedOk)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CScheduleVerticallyCopyDlg)
	//EASYSIZE(IDC_STATIC_INFO_GROUP,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

BOOL CScheduleVerticallyCopyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;
    //m_CVerticallyCnt.EnableMask(_T("ddd"), _T("   "), _T(' '));
	//m_CVerticallyCnt.SetLimitText(3);
    m_CVerticallyCnt.SetMaxWholeDigits(-3);
    m_CVerticallyCnt.SetRange(0.0f, 999.0f);
	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CScheduleVerticallyCopyDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}



void CScheduleVerticallyCopyDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(!StringUtility :: IsDigitalStr(m_szVerticallyCnt.GetBuffer()))
	{
        MessageBox(Translate(_T("An integer between 1 and 999 is required for this field.")),Translate(_T("Warning:Schedule vertically copy")), MB_OK|MB_ICONEXCLAMATION);
		return;
	}	
	m_VerticallyCnt = StringUtility :: stoi(m_szVerticallyCnt.GetBuffer());

	if(abs(m_VerticallyCnt) > 999)
	{
		MessageBox(Translate(_T("An integer between 1 and 999 is required for this field.")),Translate(_T("Warning:Schedule vertically copy")), MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	CDialogEx::OnOK();
}
