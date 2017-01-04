// F:\p4\sw\Demo\ContentManagement\Controller\Designer\LayoutTemplatesPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "LayoutTemplatesPane.h"


// CLayoutTemplatesPane

IMPLEMENT_DYNAMIC(CLayoutTemplatesPane, CBaseControllerPane)

CLayoutTemplatesPane::CLayoutTemplatesPane()
{

}

CLayoutTemplatesPane::~CLayoutTemplatesPane()
{
}


BEGIN_MESSAGE_MAP(CLayoutTemplatesPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CLayoutTemplatesPane message handlers
int CLayoutTemplatesPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_wndTemplateDlg.Create(CTemplateCtrl::IDD, this);
    m_wndTemplateDlg.ShowWindow(SW_SHOW);

    SetMainWnd(&m_wndTemplateDlg);
    SetMinSize(CSize(0, 0));

    return 0;
}


void CLayoutTemplatesPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();
}

