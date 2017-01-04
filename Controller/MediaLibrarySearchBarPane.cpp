// F:\p4\sw\Demo\ContentManagement\Controller\Common UI\MediaLibrarySearchBarPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "MediaLibrarySearchBarPane.h"
#include "MediaLibrarySearchBar.h"


// CMediaLibrarySearchBarPane

IMPLEMENT_DYNAMIC(CMediaLibrarySearchBarPane, CBaseControllerPane)

CMediaLibrarySearchBarPane::CMediaLibrarySearchBarPane()
{

}

CMediaLibrarySearchBarPane::~CMediaLibrarySearchBarPane()
{
}


BEGIN_MESSAGE_MAP(CMediaLibrarySearchBarPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CMediaLibrarySearchBarPane message handlers
int CMediaLibrarySearchBarPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;


    m_MediaLibrarySearchBarCtrl.Create(CMediaLibrarySearchBarCtrl::IDD,this);
    m_MediaLibrarySearchBarCtrl.ShowWindow(SW_SHOW);

    SetMainWnd(&m_MediaLibrarySearchBarCtrl);
    SetMinSize(CSize(250,90));
    return 0;
}



void CMediaLibrarySearchBarPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

