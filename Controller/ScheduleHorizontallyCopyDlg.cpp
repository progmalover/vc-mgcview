#include "stdafx.h"
#include "Controller.h"
#include "ScheduleHorizontallyCopyDlg.h"
#include "SchedulerPage.h"
#include "afxdialogex.h"
#include "ScheduleCtrl.h"
#include "Localization/Localization.h"

IMPLEMENT_DYNAMIC(CScheduleHorizontallyCopyDlg, CDialogEx)

CScheduleHorizontallyCopyDlg::CScheduleHorizontallyCopyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScheduleHorizontallyCopyDlg::IDD, pParent)
    , m_StartTime (COleDateTime::GetCurrentTime())
    , m_EndTime (COleDateTime::GetCurrentTime())
{

}

CScheduleHorizontallyCopyDlg::~CScheduleHorizontallyCopyDlg()
{
}

void CScheduleHorizontallyCopyDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_StartTime);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_EndTime);
    DDX_Check(pDX, IDC_HCOPY_SELECTALL, m_bSelectAll);
    DDX_Check(pDX, IDC_MONDAY, m_bMonday);
    DDX_Check(pDX, IDC_TUESDAY, m_bTuesday);
    DDX_Check(pDX, IDC_WEDNESDAY, m_bWednesday);
    DDX_Check(pDX, IDC_THURSDAY, m_bThursday);
    DDX_Check(pDX, IDC_FRIDAY, m_bFriday);
    DDX_Check(pDX, IDC_SATURDAY, m_bSaturday);
    DDX_Check(pDX, IDC_SUNDAY, m_bSunday);
    DDX_Control(pDX, IDOK, m_ctrlOK);
}


BEGIN_MESSAGE_MAP(CScheduleHorizontallyCopyDlg, CDialogEx)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_HCOPY_SELECTALL, &CScheduleHorizontallyCopyDlg::OnBnClickedHcopySelectall)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, &CScheduleHorizontallyCopyDlg::OnDtnDatetimechangeDatetimepicker1)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER2, &CScheduleHorizontallyCopyDlg::OnDtnDatetimechangeDatetimepicker2)
    ON_BN_CLICKED(IDC_MONDAY, &CScheduleHorizontallyCopyDlg::OnBnClickedMonday)
    ON_BN_CLICKED(IDC_TUESDAY, &CScheduleHorizontallyCopyDlg::OnBnClickedTuesday)
    ON_BN_CLICKED(IDC_WEDNESDAY, &CScheduleHorizontallyCopyDlg::OnBnClickedWednesday)
    ON_BN_CLICKED(IDC_THURSDAY, &CScheduleHorizontallyCopyDlg::OnBnClickedThursday)
    ON_BN_CLICKED(IDC_FRIDAY, &CScheduleHorizontallyCopyDlg::OnBnClickedFriday)
    ON_BN_CLICKED(IDC_SATURDAY, &CScheduleHorizontallyCopyDlg::OnBnClickedSaturday)
    ON_BN_CLICKED(IDC_SUNDAY, &CScheduleHorizontallyCopyDlg::OnBnClickedSunday)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CScheduleHorizontallyCopyDlg)
	//EASYSIZE(IDC_STATIC_INFO_GROUP,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

BOOL CScheduleHorizontallyCopyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;

    m_ctrlOK.EnableWindow(FALSE);
	
	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CScheduleHorizontallyCopyDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}


void CScheduleHorizontallyCopyDlg::OnBnClickedHcopySelectall()
{
    // TODO: Add your control notification handler code here

    UpdateData(TRUE);

    if (m_bSelectAll)
    {
        m_bMonday = TRUE;
        m_bTuesday = TRUE;
        m_bWednesday = TRUE;
        m_bThursday = TRUE;
        m_bFriday = TRUE;
        m_bSaturday = TRUE;
        m_bSunday = TRUE;
    } 
    else
    {
        m_bMonday = FALSE;
        m_bTuesday = FALSE;
        m_bWednesday = FALSE;
        m_bThursday = FALSE;
        m_bFriday = FALSE;
        m_bSaturday = FALSE;
        m_bSunday = FALSE;
    }

    UpdateData(FALSE);

    UpdateOKBtn();
}


void CScheduleHorizontallyCopyDlg::OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    // TODO: Add your control notification handler code here
    UpdateOKBtn();
    *pResult = 0;
}


void CScheduleHorizontallyCopyDlg::OnDtnDatetimechangeDatetimepicker2(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    // TODO: Add your control notification handler code here
    UpdateOKBtn();
    *pResult = 0;
}


void CScheduleHorizontallyCopyDlg::OnBnClickedMonday()
{
    // TODO: Add your control notification handler code here
    UpdateOKBtn();
}


void CScheduleHorizontallyCopyDlg::OnBnClickedTuesday()
{
    // TODO: Add your control notification handler code here
    UpdateOKBtn();
}


void CScheduleHorizontallyCopyDlg::OnBnClickedWednesday()
{
    // TODO: Add your control notification handler code here
    UpdateOKBtn();
}


void CScheduleHorizontallyCopyDlg::OnBnClickedThursday()
{
    // TODO: Add your control notification handler code here
    UpdateOKBtn();
}


void CScheduleHorizontallyCopyDlg::OnBnClickedFriday()
{
    // TODO: Add your control notification handler code here
    UpdateOKBtn();
}


void CScheduleHorizontallyCopyDlg::OnBnClickedSaturday()
{
    // TODO: Add your control notification handler code here
    UpdateOKBtn();
}


void CScheduleHorizontallyCopyDlg::OnBnClickedSunday()
{
    // TODO: Add your control notification handler code here
    UpdateOKBtn();
}

void CScheduleHorizontallyCopyDlg::UpdateOKBtn()
{
    UpdateData(TRUE);

    BOOL bEnable = FALSE;

    BOOL bEnabledDays[8];
    bEnabledDays[1] = m_bSunday;
    bEnabledDays[2] = m_bMonday;
    bEnabledDays[3] = m_bTuesday;
    bEnabledDays[4] = m_bWednesday;
    bEnabledDays[5] = m_bThursday;
    bEnabledDays[6] = m_bFriday;
    bEnabledDays[7] = m_bSaturday;

    COleDateTimeSpan TimeSpan = m_EndTime - m_StartTime;

    if (TimeSpan.m_status == COleDateTime::valid)
    {
        COleDateTimeSpan Oneday(1,0,0,0);
        COleDateTime e = m_EndTime + Oneday;
        for (COleDateTime t = m_StartTime; t < e; t+=Oneday)
        {
            if (bEnabledDays[t.GetDayOfWeek()])
            {
                bEnable = TRUE;
                break;
            }
        }
    }

    m_ctrlOK.EnableWindow(bEnable);

    UpdateData(FALSE);
}