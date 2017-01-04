// F:\p4\sw\Demo\ContentManagement\Controller\Scheduler\ScheduleItemPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ScheduleItemPane.h"


// CScheduleItemPane

IMPLEMENT_DYNAMIC(CScheduleItemPane, CBaseControllerPane)

CScheduleItemPane::CScheduleItemPane()
{

}

CScheduleItemPane::~CScheduleItemPane()
{
}


BEGIN_MESSAGE_MAP(CScheduleItemPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CScheduleItemPane message handlers
int CScheduleItemPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_ScheduleItem.Create(CScheduleItemCtrl::IDD,this);
    m_ScheduleItem.ShowWindow(SW_SHOW);

    SetMainWnd(&m_ScheduleItem);
    SetMinSize(CSize(150,150));

    return 0;
}


void CScheduleItemPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

