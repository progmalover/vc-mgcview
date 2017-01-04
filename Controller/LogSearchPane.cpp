// F:\p4\sw\Demo\ContentManagement\Controller\Log\LogSearchPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "LogSearchPane.h"



// CLogSearchPane

IMPLEMENT_DYNAMIC(CLogSearchPane, CBaseControllerPane)

CLogSearchPane::CLogSearchPane()
{

}

CLogSearchPane::~CLogSearchPane()
{
}


BEGIN_MESSAGE_MAP(CLogSearchPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CLogSearchPane message handlers
int CLogSearchPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_LogFilter.Create(CLogFilterCtrl::IDD,this);
    m_LogFilter.ShowWindow(SW_SHOW);

    SetMainWnd(&m_LogFilter);
    SetMinSize(CSize(150,150));
    return 0;
}


void CLogSearchPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

