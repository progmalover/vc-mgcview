// F:\p4\sw\Demo\ContentManagement\Controller\Alarm\AlarmSearchPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "AlarmSearchPane.h"



// CAlarmSearchPane

IMPLEMENT_DYNAMIC(CAlarmSearchPane, CBaseControllerPane)

CAlarmSearchPane::CAlarmSearchPane()
{

}

CAlarmSearchPane::~CAlarmSearchPane()
{
}


BEGIN_MESSAGE_MAP(CAlarmSearchPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CAlarmSearchPane message handlers
int CAlarmSearchPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_AlarmFilter.Create(CAlarmFilterCtrl::IDD,this);
    m_AlarmFilter.ShowWindow(SW_SHOW);

    SetMainWnd(&m_AlarmFilter);
    SetMinSize(CSize(150,150));
    return 0;
}


void CAlarmSearchPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

