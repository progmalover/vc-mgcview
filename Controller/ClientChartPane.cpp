// F:\p4\sw\Demo\ContentManagement\Controller\Client Monitor\ClientInfoViewPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ClientChartPane.h"


// CClientInfoViewPane

IMPLEMENT_DYNAMIC(CClientChartPane, CBaseControllerPane)

CClientChartPane::CClientChartPane(int id)
{
	m_clientid = id;
}

CClientChartPane::~CClientChartPane()
{
}


BEGIN_MESSAGE_MAP(CClientChartPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CClientInfoViewPane message handlers
int CClientChartPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    SetMinSize(CSize(250,400));

    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;
	m_wndClientChart.SetClientId(m_clientid);
    m_wndClientChart.Create(CClientChartDlg::IDD,this);
	m_wndClientChart.ShowWindow(SW_SHOW);

    SetMainWnd(&m_wndClientChart);

    return 0;
}


void CClientChartPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

