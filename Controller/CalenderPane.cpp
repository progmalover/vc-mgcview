// F:\p4\sw\Demo\ContentManagement\Controller\Scheduler\CalenderPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "CalenderPane.h"
#include "SchedulerPage.h"


// CCalenderPane

IMPLEMENT_DYNAMIC(CCalenderPane, CBaseControllerPane)

CCalenderPane::CCalenderPane()
{

}

CCalenderPane::~CCalenderPane()
{
}


BEGIN_MESSAGE_MAP(CCalenderPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CCalenderPane message handlers
int CCalenderPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;
    SetMinSize(CSize(180,400));
    CRect rectDummy;
    rectDummy.SetRectEmpty ();

    m_wndMonthCal.Create (WS_VISIBLE | WS_CHILD, rectDummy, this, IDC_SCHEDULER_CALENDAR);
    m_wndMonthCal.EnableMutipleSelection (TRUE, 7 /* Max sel days */, TRUE);
    m_wndMonthCal.EnableWeekNumbers ();
//    m_wndMonthCal.EnableTodayButton();

    SetMainWnd(&m_wndMonthCal);

    return 0;
}



void CCalenderPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}
