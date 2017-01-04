// F:\p4\sw\Demo\ContentManagement\Controller\Common UI\MediaLibraryInfoPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "MediaLibraryInfoPane.h"
#include "MediaLibraryInfoCtrl.h"


// CMediaLibraryInfoPane

IMPLEMENT_DYNAMIC(CMediaLibraryInfoPane, CBaseControllerPane)

CMediaLibraryInfoPane::CMediaLibraryInfoPane()
{

}

CMediaLibraryInfoPane::~CMediaLibraryInfoPane()
{
}


BEGIN_MESSAGE_MAP(CMediaLibraryInfoPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CMediaLibraryInfoPane message handlers
int CMediaLibraryInfoPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;


    m_MediaLibraryInfoCtrl.Create(CMediaLibraryInfoCtrl::IDD,this);
    m_MediaLibraryInfoCtrl.ShowWindow(SW_SHOW);

    SetMainWnd(&m_MediaLibraryInfoCtrl);
    SetMinSize(CSize(250,90));
    return 0;
}



void CMediaLibraryInfoPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

