// F:\p4\sw\Demo\ContentManagement\Controller\ReportSystem\ReportSystem.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ReportSystemPage.h"
#include "ReportListPane.h"
#include "Localization/Localization.h"

// CLogSystemPage

IMPLEMENT_DYNAMIC(CReportSystemPage, CBaseControllerPage)


CReportSystemPage::CReportSystemPage()
{
}

CReportSystemPage::~CReportSystemPage()
{

}

BEGIN_MESSAGE_MAP(CReportSystemPage, CBaseControllerPage)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

/// CReportSystemPage message handle
int CReportSystemPage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (!CreateRibbonButtons())
    {
        TRACE0("Failed to create Report System ribbon buttons\n");
        return -1;      // fail to create
    }

    if (CBaseControllerPage::OnCreate(lpCreateStruct) == -1)
        return -1;

    // enable Visual Studio 2005 style docking window behavior
    CDockingManager::SetDockingMode(DT_SMART);
    // enable Visual Studio 2005 style docking window auto-hide behavior
    EnableAutoHidePanes(CBRS_ALIGN_ANY);


    CRect rect;
    GetClientRect(&rect);

    int left    = 0;
    int right   = 0;
    int top     = 0;
    int bottom  = 0;

    // Create and Install the Report View List pane
    left    = rect.Width() / 4;
    right   = rect.right;
    bottom  = rect.bottom;


    if (!m_wndReportList.Create(Translate(_T("Report View")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_REPORT_LIST, WS_CHILD | WS_VISIBLE |  CBRS_LEFT | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create Report View pane\n");
        return -1;      // fail to create
    }


    SetMainWnd(&m_wndReportList);
  
    return 0;
}

void CReportSystemPage::OnDestroy()
{
    CBaseControllerPage::OnDestroy();
}

void CReportSystemPage::Close()
{
 //   m_wndReportOutlook.DestroyWindow();
    m_wndReportList.DestroyWindow();
}

BOOL CReportSystemPage::CreateRibbonButtons()
{
    if (!m_pCategory)
    {
        return FALSE;
    }

    /// Remove the dummy panel, and create the real panels
    m_pCategory->RemovePanel(0);
    //if you need, can add panels and ribbon buttons
 
    return TRUE;
}