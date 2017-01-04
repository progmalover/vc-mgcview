// F:\p4\sw\Demo\ContentManagement\Controller\Common UI\MediaLibraryInfoPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ClientInfoPane.h"
#include "ClientInfoCtrl.h"


// CClientInfoPane

IMPLEMENT_DYNAMIC(CClientInfoPane, CBaseControllerPane)

CClientInfoPane::CClientInfoPane()
{

}

CClientInfoPane::~CClientInfoPane()
{
}


BEGIN_MESSAGE_MAP(CClientInfoPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CClientInfoPane message handlers
int CClientInfoPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;


    m_InfoCtrl.Create(CClientInfoCtrl::IDD,this);
    m_InfoCtrl.ShowWindow(SW_SHOW);

    SetMainWnd(&m_InfoCtrl);
    SetMinSize(CSize(250,90));
    return 0;
}



void CClientInfoPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

