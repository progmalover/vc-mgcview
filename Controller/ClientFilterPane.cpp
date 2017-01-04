// F:\p4\sw\Demo\ContentManagement\Controller\Common UI\ClientFilterPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ClientFilterPane.h"
#include "ClientFilterCtrl.h"


// CClientFilterPane

IMPLEMENT_DYNAMIC(CClientFilterPane, CBaseControllerPane)

CClientFilterPane::CClientFilterPane()
{

}

CClientFilterPane::~CClientFilterPane()
{
}


BEGIN_MESSAGE_MAP(CClientFilterPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CClientFilterPane message handlers
int CClientFilterPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;


    m_ClientFilterCtrl.Create(CClientFilterCtrl::IDD,this);
    m_ClientFilterCtrl.ShowWindow(SW_SHOW);

    SetMainWnd(&m_ClientFilterCtrl);
    SetMinSize(CSize(250,90));
    return 0;
}



void CClientFilterPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

