// F:\p4\sw\Demo\ContentManagement\Controller\Common UI\MediaLibrarySearchBarPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "LayoutSearchBarPane.h"
#include "LayoutSearchBar.h"


// CMediaLibrarySearchBarPane

IMPLEMENT_DYNAMIC(CLayoutSearchBarPane, CBaseControllerPane)

CLayoutSearchBarPane::CLayoutSearchBarPane()
{

}

CLayoutSearchBarPane::~CLayoutSearchBarPane()
{
}


BEGIN_MESSAGE_MAP(CLayoutSearchBarPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CMediaLibrarySearchBarPane message handlers
int CLayoutSearchBarPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;


    m_LayoutSearchBarCtrl.Create(CLayoutSearchBarCtrl::IDD,this);
    m_LayoutSearchBarCtrl.ShowWindow(SW_SHOW);

    SetMainWnd(&m_LayoutSearchBarCtrl);
    SetMinSize(CSize(250,90));
    return 0;
}



void CLayoutSearchBarPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();
}

