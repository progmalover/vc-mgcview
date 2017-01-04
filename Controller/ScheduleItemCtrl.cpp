// ScheduleItemCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "ScheduleItemCtrl.h"
#include "SchedulerPage.h"
#include "afxdialogex.h"
#include "ScheduleCtrl.h"
#include "Localization/Localization.h"
#include "DurationTimeCtrl.h"
UINT SCHEDULE_ITEM_UPDATE			= ::RegisterWindowMessage (_T("SCHEDULE_ITEM_UPDATE"));

UINT SCHEDULE_ITEM_UPDATE_DURATION  = ::RegisterWindowMessage (_T("SCHEDULE_ITEM_UPDATE_DURATION"));

// CScheduleItemCtrl dialog

IMPLEMENT_DYNAMIC(CScheduleItemCtrl, CDialogEx)

CScheduleItemCtrl::CScheduleItemCtrl(CWnd* pParent /*=NULL*/)
    : CDialogEx(CScheduleItemCtrl::IDD, pParent)
    , m_AppList ()
{

}

CScheduleItemCtrl::~CScheduleItemCtrl()
{
}

void CScheduleItemCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

    DDX_DateTimeCtrl(pDX,   IDC_DATETIMEPICKER_START,   m_tStartDate);
    DDX_DateTimeCtrl(pDX,   IDC_DATETIMEPICKER_END,   m_tEndDate);
	DDX_DurationDateTimeCtrl(pDX,   IDC_DATETIMEPICKER_DURATION,   m_tDuration);   

    DDX_Control(pDX, IDC_STATIC_INFO, m_strStaticInfo);
    DDX_Control(pDX, IDC_DATETIMEPICKER_START, m_StartTime);
    DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_EndTime);
   // DDX_Control(pDX, IDC_DATETIMEPICKER_DURATION, m_Duration);
}


BEGIN_MESSAGE_MAP(CScheduleItemCtrl, CDialogEx)
    ON_WM_SIZE()
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_START, &CScheduleItemCtrl::OnDtnDatetimechangeDatetimepickerStart)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_END, &CScheduleItemCtrl::OnDtnDatetimechangeDatetimepickerEnd)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_DURATION, &CScheduleItemCtrl::OnDtnDatetimechangeDatetimepickerDuration)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CScheduleItemCtrl)
    EASYSIZE(IDC_STATIC_INFO_GROUP,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDC_STATIC_INFO,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDC_STATIC_START_TIME,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
    EASYSIZE(IDC_STATIC_END_TIME,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
    EASYSIZE(IDC_STATIC_PLAY_DURATION,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
    EASYSIZE(IDC_DATETIMEPICKER_START,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDC_DATETIMEPICKER_END,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDC_DATETIMEPICKER_DURATION,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP


// CScheduleItemCtrl message handlers
BOOL CScheduleItemCtrl::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;

   // ((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_DURATION))->SetFormat(_T("HH:mm:ss"));
	m_Duration.CreateFromStatic(this,IDC_DATETIMEPICKER_DURATION);
    ((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_START))->SetFormat(_T("HH:mm:ss"));
    ((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_END))->SetFormat(_T("HH:mm:ss"));
	 
	Translate(this);
    return TRUE;  // return TRUE  unless you set the focus to a control
}


void CScheduleItemCtrl::OnSize(UINT nType, int cx, int cy) 
{
    CDialogEx::OnSize(nType, cx, cy);
    UPDATE_EASYSIZE;
}

void CScheduleItemCtrl::SetItemList(std::vector<Appointment*>& AppList)
{
    m_AppList.clear();
    m_AppList = AppList;

    CString Desc;

    if (m_AppList.size() > 1)
    {
        m_tStartDate = m_AppList[0]->GetStart();
        m_tEndDate = m_AppList[0]->GetFinish();
        m_tDuration = m_AppList[0]->GetDuration();

        m_StartTime.EnableWindow(FALSE);
        m_EndTime.EnableWindow(FALSE);
        m_Duration.EnableWindow(TRUE);

        for (std::vector<Appointment*>::iterator it = m_AppList.begin(); it != m_AppList.end(); it++)
        {
            Desc += Translate(_T("Name:")) + (*it)->GetDescription() + _T(";\n");
            //Layout layout;
            //GetControllerInterface->GetLayout((*it)->GetDetailID(), layout);
            //Desc += (_T("Layout Name:") + layout.GetLayoutName() + _T("\t")).c_str();
            //Desc += (_T("(Owner:") + layout.GetOwner() + _T(");\n")).c_str();
        }
    }
    else if (m_AppList.size() == 1)
    {
        m_tStartDate = m_AppList[0]->GetStart();
        m_tEndDate = m_AppList[0]->GetFinish();
        m_tDuration = m_AppList[0]->GetDuration();

		int nHours = m_tDuration.GetHours();
		int nDays = m_tDuration.GetDays();
		int nMin = m_tDuration.GetMinutes();
		int nSec = m_tDuration.GetSeconds();

		nHours = 24 * nDays;
		if(nHours >= 1000)
		{
			
			//AfxMessageBox(Translate(_T("Can't support Duration range large than 1000 hours!mmm")));
			//stDateTime = m_tDuration;
			
			m_tDuration.SetDateTimeSpan(999/24,999%24,nMin,nSec);
			m_tEndDate = m_tStartDate + m_tDuration;
			m_AppList[0]->SetEnd(m_tEndDate);
			//return;
		}

        m_StartTime.EnableWindow(TRUE);
        m_EndTime.EnableWindow(TRUE);
        m_Duration.EnableWindow(TRUE);

        Desc =  Translate(_T("Name:")) + m_AppList[0]->GetDescription();
        Desc += _T("\n\n");

        if(m_AppList[0]->GetDetailType() == Appointment::APPOINTMENT_TYPE_LAYOUT)
        {
            Desc += Translate(_T("Type:")) + Translate(_T("Layout"));
        }
        if(m_AppList[0]->GetDetailType() == Appointment::APPOINTMENT_TYPE_TIMETABLE)
        {
            Desc += Translate(_T("Type:")) + Translate(_T("TimeTable"));
        } 

        //Layout layout;
        //GetControllerInterface->GetLayout(m_AppList[0]->GetDetailID(), layout);
        //Desc += (_T("Layout Name:") + layout.GetLayoutName() + _T("\t")).c_str();
        //Desc += (_T("(Owner:") + layout.GetOwner() + _T(");\n")).c_str();
    }
    else
    {
        m_tStartDate = COleDateTime();
        m_tEndDate = COleDateTime();
        m_tDuration = COleDateTime();

        m_StartTime.EnableWindow(FALSE);
        m_EndTime.EnableWindow(FALSE);
        m_Duration.EnableWindow(FALSE);

        Desc =  Translate(_T("No Layout selected!"));
    }
    
    m_strStaticInfo.SetWindowTextW(Desc);

    UpdateData(FALSE);
}

void CScheduleItemCtrl::OnDtnDatetimechangeDatetimepickerStart(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
	COleDateTime   LastStartDate = m_tStartDate; 
    UpdateData(TRUE);

    if (m_tStartDate > m_tEndDate)
    {
        m_tStartDate = m_tEndDate - COleDateTimeSpan(0,0,1,0);   
    }
	
	if(m_tStartDate == m_tEndDate)
	{
		m_tStartDate = LastStartDate;
		MessageBox(Translate(_T("The schedule start time can not be same with end time.")), Translate(_T("Warning")), MB_OK | MB_ICONWARNING);
		UpdateData(FALSE);
		return;
	}
    GetParent()->SendMessage(SCHEDULE_ITEM_UPDATE);
}


void CScheduleItemCtrl::OnDtnDatetimechangeDatetimepickerEnd(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
	COleDateTime   LastEndDate = m_tEndDate; 
    UpdateData(TRUE);

    if (m_tEndDate < m_tStartDate)
    {
        m_tEndDate = m_tStartDate + COleDateTimeSpan(0,0,1,0);
    }
	if(m_tStartDate == m_tEndDate)
	{
		m_tEndDate = LastEndDate;
		MessageBox(Translate(_T("The schedule start time can not be same with end time.")), Translate(_T("Warning")), MB_OK | MB_ICONWARNING);
		UpdateData(FALSE);
		return;
	}
    GetParent()->SendMessage(SCHEDULE_ITEM_UPDATE);
}


void CScheduleItemCtrl::OnDtnDatetimechangeDatetimepickerDuration(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    UpdateData(TRUE);

    if (m_tDuration < COleDateTimeSpan(0,0,1,0))
    {
        m_tDuration = COleDateTimeSpan(0,0,1,0);
    }

    GetParent()->SendMessage(SCHEDULE_ITEM_UPDATE_DURATION);
}
