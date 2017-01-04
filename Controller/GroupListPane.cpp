// F:\p4\sw\Demo\ContentManagement\Controller\Client Manager\GroupListPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "GroupListPane.h"


// CGroupListPane

IMPLEMENT_DYNAMIC(CGroupListPane, CBaseControllerPane)

CGroupListPane::CGroupListPane()
{

}

CGroupListPane::~CGroupListPane()
{
}


BEGIN_MESSAGE_MAP(CGroupListPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CGroupListPane message handlers
int CGroupListPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;
    SetMinSize(CSize(180,400));
    RECT rect;
    GetClientRect(&rect);

    m_wndGroupList.Create(WS_CHILD|WS_VISIBLE|LVS_ICON, rect, this, IDW_LIST_GROUP);

    SetMainWnd(&m_wndGroupList);

    m_wndGroupList.OnInitialUpdate();

    return 0;
}


void CGroupListPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

