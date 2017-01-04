#include "stdafx.h"
#include "Controller.h"
#include "ScheduleContinuousDeleteDlg.h"
#include "SchedulerPage.h"
#include "afxdialogex.h"
#include "ScheduleCtrl.h"
#include "Localization/Localization.h"

IMPLEMENT_DYNAMIC(CScheduleContinuousDeleteDlg, CDialogEx)

CScheduleContinuousDeleteDlg::CScheduleContinuousDeleteDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScheduleContinuousDeleteDlg::IDD, pParent)
{

}

CScheduleContinuousDeleteDlg::~CScheduleContinuousDeleteDlg()
{
}

void CScheduleContinuousDeleteDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_MULTIDEL_CNT, m_ReoccurencCnt);
}


BEGIN_MESSAGE_MAP(CScheduleContinuousDeleteDlg, CDialogEx)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_RADIO_HORIZONTALLY, &CScheduleContinuousDeleteDlg::OnBnClickedRadioHorizontally)
    ON_BN_CLICKED(IDC_RADIO_VERTICALLY, &CScheduleContinuousDeleteDlg::OnBnClickedRadioVertically)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CScheduleContinuousDeleteDlg)
	//EASYSIZE(IDC_STATIC_INFO_GROUP,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP


BOOL CScheduleContinuousDeleteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;

    m_bHorizontally = TRUE;
	m_ReoccurencCnt = 0;

    UpdateData(FALSE);

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CScheduleContinuousDeleteDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}


void CScheduleContinuousDeleteDlg::OnBnClickedRadioHorizontally()
{
    // TODO: Add your control notification handler code here
    m_bHorizontally = TRUE;
}


void CScheduleContinuousDeleteDlg::OnBnClickedRadioVertically()
{
    // TODO: Add your control notification handler code here
    m_bHorizontally = FALSE;
}
