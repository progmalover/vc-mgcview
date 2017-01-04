// F:\p4\sw\Demo\ContentManagement\Controller\Designer\LayoutProperties.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "LayoutPropertiesPane.h"



// CLayoutPropertiesPane

IMPLEMENT_DYNAMIC(CLayoutPropertiesPane, CBaseControllerPane)

CLayoutPropertiesPane::CLayoutPropertiesPane()
{

}

CLayoutPropertiesPane::~CLayoutPropertiesPane()
{
}


BEGIN_MESSAGE_MAP(CLayoutPropertiesPane, CBaseControllerPane)
    ON_WM_CREATE()
END_MESSAGE_MAP()



// CLayoutPropertiesPane message handlers
int CLayoutPropertiesPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;
	SetMinSize(CSize(280,400));
    CRect layoutRect;
    GetClientRect(layoutRect);
    
    CString szCanvasClass = AfxRegisterWndClass( CS_VREDRAW | CS_HREDRAW , 
    ::LoadCursor(NULL, IDC_ARROW),
    (HBRUSH)::CreateSolidBrush(GetSysColor(COLOR_BTNFACE)),
    NULL);

    if (!m_PropertyCtrl.Create(szCanvasClass, _T("Property Ctrl"), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
        layoutRect, this, ID_PROPERTY_CTRL, NULL))
    {
        TRACE0("Failed to create window\n");
        return -1;
    }

    SetMainWnd(&m_PropertyCtrl);
    return 0;
}

