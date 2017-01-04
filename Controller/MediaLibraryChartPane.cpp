// F:\p4\sw\Demo\ContentManagement\Controller\MediaLibrary Monitor\MediaLibraryCategoryPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "MediaLibraryChartPane.h"


// CMediaLibraryCategoryPane

IMPLEMENT_DYNAMIC(CMediaLibraryChartPane, CBaseControllerPane)

CMediaLibraryChartPane::CMediaLibraryChartPane()
{

}

CMediaLibraryChartPane::~CMediaLibraryChartPane()
{
}


BEGIN_MESSAGE_MAP(CMediaLibraryChartPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CMediaLibraryCategoryPane message handlers
int CMediaLibraryChartPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    SetMinSize(CSize(250,400));

    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_wndMediaLibraryChart.Create(CMediaLibraryChartDlg::IDD,this);
    m_wndMediaLibraryChart.ShowWindow(SW_SHOW);

    SetMainWnd(&m_wndMediaLibraryChart);

    return 0;
}


void CMediaLibraryChartPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

