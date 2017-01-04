// F:\p4\sw\Demo\ContentManagement\Controller\Log\LogSystem.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "LogSystemPage.h"
#include "Localization/Localization.h"

// CLogSystemPage

IMPLEMENT_DYNAMIC(CLogSystemPage, CBaseControllerPage)


CLogSystemPage::CLogSystemPage()
{
}

CLogSystemPage::~CLogSystemPage()
{

}

BEGIN_MESSAGE_MAP(CLogSystemPage, CBaseControllerPage)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_COMMAND(ID_RIBBON_BTN_LOG_REFRESH, OnRibbonBtnRefresh)
    ON_COMMAND(ID_RIBBON_BTN_LOG_PRINT, OnRibbonBtnPrint)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_LOG_REFRESH, OnUpdateRefresh)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_LOG_PRINT, OnUpdatePrint)
END_MESSAGE_MAP()

/// CLogSystemPage message handle
int CLogSystemPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (!CreateRibbonButtons())
    {
        TRACE0("Failed to create layout designer ribbon buttons\n");
        return -1;      // fail to create
    }

    if (CBaseControllerPage::OnCreate(lpCreateStruct) == -1)
        return -1;

        // enable Visual Studio 2005 style docking window behavior
    CDockingManager::SetDockingMode(DT_SMART);
    // enable Visual Studio 2005 style docking window auto-hide behavior
    EnableAutoHidePanes(CBRS_ALIGN_ANY);


    CRect rect;
    GetClientRect(rect);

    int left    = 0;
    int right   = 0;
    int top     = 0;
    int bottom  = 0;

    // Create and Install the log search pane
    right   = rect.Width() / 4;
    bottom  = rect.bottom;

    if (!m_wndLogSearch.Create(Translate(_T("Log Search")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_LOGSEARCH, WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create log search pane\n");
        return -1;      // fail to create
    }
    m_wndLogSearch.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndLogSearch);

    // Create and Install the log list view pane
    left    = right;
    right   = rect.right;

    if (!m_wndLogList.Create(Translate(_T("Log View")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_LOGLIST, WS_CHILD | WS_VISIBLE |  CBRS_LEFT | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create log list view pane\n");
        return -1;      // fail to create
    }

    SetMainWnd(&m_wndLogList);

	Translate(this);
    return 0;
}



void CLogSystemPage::OnDestroy()
{
    CBaseControllerPage::OnDestroy();


}

void CLogSystemPage::Close()
{
    m_wndLogSearch.DestroyWindow();
    m_wndLogList.DestroyWindow();
}

BOOL CLogSystemPage::CreateRibbonButtons()
{
    if (!m_pCategory)
    {
        return FALSE;
    }

    /// Remove the dummy panel, and create the real panels
    m_pCategory->RemovePanel(0);

    /*

    CString strPanel            = _T("Edit");
    CMFCRibbonPanel* pPanelEdit = m_pCategory->AddPanel(strPanel);*/

/*
    CMFCRibbonButton* pBtnPrint = new CMFCRibbonButton(ID_RIBBON_BTN_LOG_PRINT, _T("Print"), 0, 0);
    pBtnPrint->SetAlwaysLargeImage();
    pPanelEdit->Add(pBtnPrint);
*/

    /*CMFCRibbonButton* pBtnRefresh = new CMFCRibbonButton(ID_RIBBON_BTN_LOG_REFRESH, _T("Refresh"), 1, 1);
    pPanelEdit->Add(pBtnRefresh);*/

    return TRUE;
}

void CLogSystemPage::OnRibbonBtnRefresh()
{
    m_wndLogSearch.m_LogFilter.OnBnClickedButtonSearch();
}

void CLogSystemPage::OnRibbonBtnPrint()
{
    return;
    CDC dc;
    CPrintDialog printDlg(FALSE, PD_NOSELECTION|PD_PAGENUMS|PD_PRINTTOFILE);
    printDlg.m_pd.nMinPage = 1;
    printDlg.m_pd.nMaxPage = 2;
    printDlg.m_pd.nFromPage = 1;
    printDlg.m_pd.nToPage = 2;


    if (printDlg.DoModal() == IDCANCEL)     // Get printer settings from user
        return;

    dc.Attach(printDlg.GetPrinterDC());     // Get and attach a printer DC
    dc.m_bPrinting = TRUE;
    CString strTitle;                       // Get the application title
    strTitle.LoadString(AFX_IDS_APP_TITLE);
    Translate(strTitle);
	DOCINFO di;                             // Initialise print document details
    ::ZeroMemory (&di, sizeof (DOCINFO));
    di.cbSize = sizeof (DOCINFO);
    di.lpszDocName = strTitle;

    BOOL bPrintingOK = dc.StartDoc(&di);    // Begin a new print job


    // Get the printing extents and store in the m_rectDraw field of a

    // CPrintInfo object

    CPrintInfo Info;
    Info.m_pPD->m_pd.nMaxPage = 2;
    Info.m_rectDraw.SetRect(0,0,
        dc.GetDeviceCaps(HORZRES),
        dc.GetDeviceCaps(VERTRES));

    //OnBeginPrinting(&dc, &Info);            // Call your "Init printing" function

    for (UINT page = Info.GetMinPage();
        page <= Info.GetMaxPage() && bPrintingOK;
        page++)
    {
        dc.StartPage();                     // begin new page

        Info.m_nCurPage = page;
        //OnPrint(&dc, &Info);                // Call your "Print page" function

        bPrintingOK = (dc.EndPage() > 0);   // end page

    }
    //OnEndPrinting(&dc, &Info);              // Call your "Clean up" function

    if (bPrintingOK)
        dc.EndDoc();                        // end a print job
    else
        dc.AbortDoc();                      // abort job.

    dc.DeleteDC();                          // delete the printer DC
}

void CLogSystemPage::OnUpdateRefresh(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

/*
    if (GetControllerInterface->GetPermissionType() != User::ADMINACCOUNT)
    {
        pCmdUI->Enable(FALSE);
    }
    else
*/
    {
        pCmdUI->Enable(TRUE);
    }
}

void CLogSystemPage::OnUpdatePrint(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

/*
    if (GetControllerInterface->GetPermissionType() != User::ADMINACCOUNT)
    {
        pCmdUI->Enable(FALSE);
    }
    else
*/
    {
        pCmdUI->Enable(TRUE);
    }
}
