// F:\p4\sw\Demo\ContentManagement\Controller\MediaLibrary Monitor\MediaLibraryCategoryPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "MediaLibraryCategoryPane.h"


// CMediaLibraryCategoryPane

IMPLEMENT_DYNAMIC(CMediaLibraryCategoryPane, CBaseControllerPane)

CMediaLibraryCategoryPane::CMediaLibraryCategoryPane()
{

}

CMediaLibraryCategoryPane::~CMediaLibraryCategoryPane()
{
}


BEGIN_MESSAGE_MAP(CMediaLibraryCategoryPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CMediaLibraryCategoryPane message handlers
int CMediaLibraryCategoryPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    SetMinSize(CSize(250,400));

    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_MediaLibraryCategory.Create(CMediaLibraryCategoryCtrl::IDD,this);
    m_MediaLibraryCategory.ShowWindow(SW_SHOW);

    SetMainWnd(&m_MediaLibraryCategory);

    return 0;
}


void CMediaLibraryCategoryPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}



