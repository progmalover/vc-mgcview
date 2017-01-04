#include "StdAfx.h"
#include "LayoutCategoryPane.h"


CLayoutCategoryPane::CLayoutCategoryPane(void)
{
}


CLayoutCategoryPane::~CLayoutCategoryPane(void)
{
}


BEGIN_MESSAGE_MAP(CLayoutCategoryPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CLayoutListPane message handlers
int CLayoutCategoryPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    RECT rect;
    GetClientRect(&rect);

    m_LayoutList.Create(CLayoutCategoryCtrl::IDD, this);
	m_LayoutList.ShowWindow(SW_SHOW);

	SetMainWnd(&m_LayoutList);
	SetMinSize(CSize(250,90));

    return 0;
}


void CLayoutCategoryPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}





