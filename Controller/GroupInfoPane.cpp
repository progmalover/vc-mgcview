// F:\p4\sw\Demo\ContentManagement\Controller\Client Manager\GroupInfoPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "GroupInfoPane.h"


// CGroupInfoPane

IMPLEMENT_DYNAMIC(CGroupInfoPane, CBaseControllerPane)

CGroupInfoPane::CGroupInfoPane()
{

}

CGroupInfoPane::~CGroupInfoPane()
{
}


BEGIN_MESSAGE_MAP(CGroupInfoPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CLayoutPropertiesMediaFileBrowser message handlers
int CGroupInfoPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;


    m_GroupInfo.Create(CGroupInfoCtrlNew::IDD,this);
    m_GroupInfo.ShowWindow(SW_SHOW);

    SetMainWnd(&m_GroupInfo);
    SetMinSize(CSize(250,90));

    //m_GroupInfo.OnInitialUpdate();

    return 0;
}


void CGroupInfoPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();
}

