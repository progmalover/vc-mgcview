#include "StdAfx.h"
#include "SchedulerItemSelectPane.h"
#include "LayoutCategoryCtrl.h"
#include "TimeTableCategoryCtrl.h"


IMPLEMENT_DYNAMIC(CSchedulerItemSelectTab, CMFCTabCtrl)

CSchedulerItemSelectTab::CSchedulerItemSelectTab()
{
}

CSchedulerItemSelectTab::~CSchedulerItemSelectTab()
{
}


BEGIN_MESSAGE_MAP(CSchedulerItemSelectTab, CMFCTabCtrl)
    ON_WM_CREATE()
END_MESSAGE_MAP()


int CSchedulerItemSelectTab::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMFCTabCtrl::OnCreate(lpCreateStruct) == -1)
        return -1;

    RECT rect;
    GetClientRect(&rect);

    CLayoutCategoryCtrl *pLayoutCategory = new CLayoutCategoryCtrl(this);
    pLayoutCategory->EnableFileMode();
    pLayoutCategory->ShowPowerOffLayout();
    pLayoutCategory->Create(CLayoutCategoryCtrl::IDD, this);

    InsertTab(pLayoutCategory, Translate(_T("Layout")), 0);

    CTimeTableCategoryCtrl *pTimeTableCategory = new CTimeTableCategoryCtrl(this, TRUE);
    //pTimeTableCategory->EnableFileMode();
    pTimeTableCategory->Create(CTimeTableCategoryCtrl::IDD, this);
    InsertTab(pTimeTableCategory, Translate(_T("TimeTable")), 0);

    return 0;
}


CSchedulerItemSelectPane::CSchedulerItemSelectPane(void)
{
}


CSchedulerItemSelectPane::~CSchedulerItemSelectPane(void)
{
}

BEGIN_MESSAGE_MAP(CSchedulerItemSelectPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CLayoutListPane message handlers
int CSchedulerItemSelectPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;


    RECT rect;
    GetClientRect(&rect);

    m_wndSchedulerSelectTab.Create(CMFCTabCtrl::STYLE_3D_VS2005, rect, this, IDW_LIST_LAYOUT_LIST);
    m_wndSchedulerSelectTab.SetLocation(CMFCBaseTabCtrl::LOCATION_TOP);
    m_wndSchedulerSelectTab.EnableActiveTabCloseButton(FALSE);
    m_wndSchedulerSelectTab.AutoDestroyWindow(TRUE);

    SetMainWnd(&m_wndSchedulerSelectTab);



    return 0;
}


void CSchedulerItemSelectPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}


