// CSchedulerPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "SchedulerPane.h"
#include "SchedulerPage.h"


// CSchedulerPane

IMPLEMENT_DYNAMIC(CSchedulerPane, CBaseControllerPane)

CSchedulerPane::CSchedulerPane()
{

}

CSchedulerPane::~CSchedulerPane()
{
}


BEGIN_MESSAGE_MAP(CSchedulerPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
    ON_WM_ERASEBKGND()
    ON_WM_DESTROY()
END_MESSAGE_MAP()


int CSchedulerPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

	CRect rect;
	GetClientRect(&rect);

	m_scheduleListTab.Create(CMFCTabCtrl::STYLE_3D_VS2005, rect, this, IDW_LIST_MEDIA_TAB);
	m_scheduleListTab.SetLocation(CMFCBaseTabCtrl::LOCATION_TOP);
	m_scheduleListTab.EnableActiveTabCloseButton(FALSE);
	m_scheduleListTab.AutoDestroyWindow(TRUE);

	SetMainWnd(&m_scheduleListTab);   
    return 0;
}

void CSchedulerPane::InitializeWnd()
{
    
}


void CSchedulerPane::OnDestroy()
{
	m_scheduleListTab.RemoveAllTabs();
	m_scheduleListTab.DestroyWindow();
	CBaseControllerPane::OnDestroy();

}


