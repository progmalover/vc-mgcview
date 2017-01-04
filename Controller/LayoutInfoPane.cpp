#include "StdAfx.h"
#include "Controller.h"
#include "LayoutInfoPane.h"



CLayoutInfoPane::CLayoutInfoPane(void)
{
}


CLayoutInfoPane::~CLayoutInfoPane(void)
{
}


BEGIN_MESSAGE_MAP(CLayoutInfoPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CLayoutListPane message handlers
int CLayoutInfoPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    RECT rect;
    GetClientRect(&rect);

    m_LayoutInfo.Create(CLayoutInfoCtrl::IDD, this);
    m_LayoutInfo.ShowWindow(SW_SHOW);

    SetMainWnd(&m_LayoutInfo);
    SetMinSize(CSize(250,90));

    return 0;
}


void CLayoutInfoPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}