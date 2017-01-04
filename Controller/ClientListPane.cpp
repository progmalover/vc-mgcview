// F:\p4\sw\Demo\ContentManagement\Controller\Client Manager\ClientListPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ClientListPane.h"


// CClientListPane

IMPLEMENT_DYNAMIC(CClientListPane, CBaseControllerPane)

CClientListPane::CClientListPane()
{

}

CClientListPane::~CClientListPane()
{
}


BEGIN_MESSAGE_MAP(CClientListPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CClientListPane message handlers
int CClientListPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    RECT rect;
    GetClientRect(&rect);

    m_ClientList.Create(WS_CHILD|WS_VISIBLE|LVS_ICON, rect, this, IDW_LIST_CLIENT);

    SetMainWnd(&m_ClientList);

    m_ClientList.OnInitialUpdate();

    return 0;
}


void CClientListPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

