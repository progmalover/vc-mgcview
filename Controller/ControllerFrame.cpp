// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface
// (the "Fluent UI") and is provided only as referential material to supplement the
// Microsoft Foundation Classes Reference and related electronic documentation
// included with the MFC C++ library software.
// License terms to copy, use or distribute the Fluent UI are available separately.
// To learn more about our Fluent UI licensing program, please visit
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.cpp : implementation of the CControllerFrame class
//

#include "stdafx.h"
#include "Controller.h"
#include "ControllerFrame.h"
#include "BorrowSkin/ControllerVisualManager.h"
#include "ControllerSettings.h"

#include "MediaLibraryPage.h"
#include "LayoutManagePage.h"
#include "DesignerPage.h"
#include "ClientManagerPage.h"
#include "UserManagerPage.h"
#include "LogSystemPage.h"
#include "AlarmSystemPage.h"
#include "SchedulerPage.h"
#include "TimeTablePage.h"
#include "Localization/Localization.h"
#include "ReportSystemPage.h"
#include "ThumbnailLib/BaseRender.h"

#include "ImportLayoutDlg.h"

#include <memory>

#include "MsgWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern UINT MAIN_APP_ALARM_INFO;
// CControllerFrame

IMPLEMENT_DYNAMIC(CControllerFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CControllerFrame, CFrameWndEx)
    ON_WM_CREATE()
    ON_WM_TIMER()
    ON_COMMAND(ID_FILE_CLOSE, &CControllerFrame::OnFileClose)
    ON_WM_ERASEBKGND()
    ON_WM_CLOSE()
    ON_WM_GETMINMAXINFO()
    ON_REGISTERED_MESSAGE(AFX_WM_ON_CHANGE_RIBBON_CATEGORY, OnChangeRibbonCategory)
    ON_REGISTERED_MESSAGE(MAIN_APP_STATE_BAR_INFO, OnChangeState)
	ON_REGISTERED_MESSAGE(MAIN_APP_ALARM_INFO, OnAlarmInfo)
    ON_COMMAND(ID_SYS_SETTING, OnOptions)

    ON_REGISTERED_MESSAGE(LAYOUT_OPEN, OnOpenLayout)
    ON_REGISTERED_MESSAGE(LAYOUT_NEW, OnNewLayout)
    ON_REGISTERED_MESSAGE(LAYOUT_IMPORT, OnImportLayout)
    ON_REGISTERED_MESSAGE(CLOSE_CURRENT_PAGE, OnCloseCurrentPage)

    ON_MESSAGE(USER_MSG_CONTROLLER_OFFLINE, OnControllerOffline)
	ON_MESSAGE(WM_PROC_CALLBACK,OnThumbnailLibCallBack)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_PREVIEW_10X7, OnUpdateRibbonBtnPreviewLayout)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_PREVIEW_12X10, OnUpdateRibbonBtnPreviewLayout)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_PREVIEW_19X10, OnUpdateRibbonBtnPreviewLayout)

    ON_MESSAGE(WM_POWERBROADCAST, &OnPowerBroadcast)
    ON_WM_SYSCOLORCHANGE()
	ON_WM_SIZING()
END_MESSAGE_MAP()

// CControllerFrame construction/destruction

CControllerFrame::CControllerFrame()
    : m_bTimerSwitcher (FALSE)
{
    m_pActiveCategory =  NULL;


#ifdef STARTER_EDITION
    m_CategoryIndexMapper.DisableCategory(CLIENTMANAGE);
    m_CategoryIndexMapper.DisableCategory(MEDIALIBRARY);
    m_CategoryIndexMapper.DisableCategory(USERMANAGE);
    m_CategoryIndexMapper.DisableCategory(LOG);
    m_CategoryIndexMapper.DisableCategory(REPORT);
#else
#ifdef PROFESSIONAL_EDITION
    m_CategoryIndexMapper.DisableCategory(LOG);
    m_CategoryIndexMapper.DisableCategory(REPORT);
#else

#endif
#endif
}

CControllerFrame::~CControllerFrame()
{

}

int CControllerFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
        return -1;

    BOOL bNameValid;

    // set the visual style to be used the by the visual manager
    CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);

    // set the visual manager used to draw all user interface elements
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CControllerVisualManager));

    if (!CreateRibbonBar())
    {
        TRACE0("Failed to load ribbon bar\n");
        return -1;      // fail to create
    }

    if (!m_wndStatusBar.Create(this))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }

    CString strTitlePane1;
    CString strTitlePane2;
    bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
    ASSERT(bNameValid);
    bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
    ASSERT(bNameValid);
    m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
    m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

    CMFCRibbonBaseElement* pElement0 = m_wndStatusBar.GetElement(0);
    CMFCRibbonBaseElement* pExElement0 = m_wndStatusBar.GetExElement(0);
    pElement0->SetText(Translate(_T("Ready")));

#ifdef STARTER_EDITION
    pExElement0->SetText((_T("S3 MagicView Manager (Starter Edition) - ") + ProcessCall::GetMyVersion()).c_str());
#else
#ifdef STANDALONE_EDITION
    pExElement0->SetText((Translate(_T("S3 MagicView Manager (Standalone Edition) - ")).GetString() + ProcessCall::GetMyVersion()).c_str());
#elif defined(PROFESSIONAL_EDITION)
    pExElement0->SetText((Translate(_T("S3 MagicView Manager (Professional Edition) - ")).GetString() + ProcessCall::GetMyVersion()).c_str());
#else
    pExElement0->SetText((Translate(_T("S3 MagicView Controller (Server Edition) - ")).GetString() + ProcessCall::GetMyVersion()).c_str());
#endif
#endif

    // enable Visual Studio 2005 style docking window behavior
    CDockingManager::SetDockingMode(DT_SMART);
    // enable Visual Studio 2005 style docking window auto-hide behavior
    EnableAutoHidePanes(CBRS_ALIGN_ANY);

    HICON hIcon = theApp.LoadIcon(IDR_MAINFRAME);
    SetIcon(hIcon, TRUE);
    SetIcon(hIcon, FALSE);

    SetTimer(0xFF000001, 2000, 0);
    m_bTimerSwitcher = TRUE;

    // This is a patch, empty the clipboard on the startup. for the appintment's image is not in the clipboard.
    // by LeonXun, to be refined later.
    if (::OpenClipboard(GetSafeHwnd()))
    {
        ::EmptyClipboard();
        ::CloseClipboard();
    }

	Translate(this);
    return 0;
}

BOOL CControllerFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CFrameWndEx::PreCreateWindow(cs) )
        return FALSE;


    cs.style |= WS_MAXIMIZE;
    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
    return TRUE;
}

    // CControllerFrame diagnostics

#ifdef _DEBUG
void CControllerFrame::AssertValid() const
{
    CFrameWndEx::AssertValid();
}

void CControllerFrame::Dump(CDumpContext& dc) const
{
    CFrameWndEx::Dump(dc);
}
#endif //_DEBUG

void CControllerFrame::OnTimer(UINT_PTR nIDEvent)
{
    if (!m_bTimerSwitcher)
    {
        return;
    }

    m_bTimerSwitcher = FALSE;

    m_bTimerSwitcher = TRUE;
}

// CControllerFrame message handlers

void CControllerFrame::OnFileClose()
{
    DestroyWindow();
}

void CControllerFrame::OnClose()
{
#if defined(STARTER_EDITION) || defined(PROFESSIONAL_EDITION)
    if (MessageBox(Translate(_T("Are you sure to exit Manager?")), Translate(_T("Tips:Manager")),MB_YESNO|MB_ICONQUESTION) != IDYES)
#elif defined(SERVER_EDITION)
    if (MessageBox(Translate(_T("Are you sure to exit Controller?")), Translate(_T("Tips:Controller")),MB_YESNO|MB_ICONQUESTION) != IDYES)
#else
#error unknown edition
#endif
    {
        return;
    }

    g_backgroundTask.reset();

    CControllerApp *pApp = reinterpret_cast<CControllerApp*>(AfxGetApp());
    // main window is being closed so cleanup
    if (pApp->m_pMainWnd == this)
    {
        for (int iCntr=0; iCntr < pApp->m_aryFrames.GetSize(); iCntr++)
        {
            HWND hFrame = pApp->m_aryFrames.GetAt(iCntr);
            if (::IsWindow(hFrame))
                ::SendMessage(hFrame, WM_CLOSE, 0, 0);
        }
    }

    for (INT i = 0; i < m_PageInfo.GetSize(); i++)
    {
        if (m_PageInfo[i].pPage != NULL)
        {
            m_PageInfo[i].pPage->Close();
            m_PageInfo[i].pPage->DestroyWindow();
            //delete m_pMainWnd[i];
            m_PageInfo[i].pPage = NULL;
        }
    }

    CFrameWnd::OnClose();
}

BOOL CControllerFrame::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}


CBaseControllerPage* CControllerFrame::GetCurActivePage()
{

    INT PageIndex = FindPage(m_pActiveCategory);
    if(PageIndex >= 0 )    return m_PageInfo[PageIndex].pPage;

    return NULL;
}

void CControllerFrame::AdjustClientArea()
{
    CBaseControllerPage* pPage = GetCurActivePage();
    if (pPage)
    {
        CRect rect;
        m_dockManager.GetClientAreaBounds(rect);
        pPage->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

LRESULT CControllerFrame::OnChangeState(WPARAM wp,LPARAM lp)
{
    CString strState = (TCHAR*)lp;
    CMFCRibbonBaseElement* pElement0 = m_wndStatusBar.GetElement(0);
    CMFCRibbonBaseElement* pExElement0 = m_wndStatusBar.GetExElement(0);

    if (strState)
    {
        pElement0->SetText(strState);
    }

    m_wndStatusBar.RecalcLayout();
    m_wndStatusBar.RedrawWindow();

    return 0;
}

LRESULT CControllerFrame::OnAlarmInfo(WPARAM wp,LPARAM lp)
{
	CMsgWnd *pMsgWindow=new CMsgWnd;
	CMsgWnd& msgWindow=*pMsgWindow;
	msgWindow.CreateMsgWindow();
	msgWindow.ShowWindow(SW_SHOW);
	msgWindow.UpdateWindow();

	return 0;
}

BOOL CControllerFrame::LoadPageByCategory(CATEGORY_TYPE Category, CRect rect, UINT nPageIndex)
{
    BOOL bLoad = FALSE;
    CWaitCursor wait;
    switch (Category)
    {
    case MEDIALIBRARY:
        {
            if (m_PageInfo[nPageIndex].pPage == NULL)
            {
                m_PageInfo[nPageIndex].pPage = new CMediaLibraryPage;
            }
            bLoad = LoadPage(m_PageInfo[nPageIndex].pPage,
                rect, ID_MEDIALIBRARY, m_PageInfo[nPageIndex].pCategory);
        }
        break;
    case LAYOUTMANAGER:
        {
            if (m_PageInfo[nPageIndex].pPage == NULL)
            {
                m_PageInfo[nPageIndex].pPage = new CLayoutManagePage;
            }
            bLoad = LoadPage(m_PageInfo[nPageIndex].pPage,
                rect, ID_LAYOUTMANAGE, m_PageInfo[nPageIndex].pCategory);

        }
        break;
    case DESIGNER:
        {
            if (m_PageInfo[nPageIndex].pPage == NULL)
            {
                m_PageInfo[nPageIndex].pPage = new CDesignerPage;
            }
            bLoad = LoadPage(m_PageInfo[nPageIndex].pPage,
                rect, ID_DESIGNER, m_PageInfo[nPageIndex].pCategory);

        }
        break;
	case TIMETABLE:
		{
            if (m_PageInfo[nPageIndex].pPage == NULL)
            {
                m_PageInfo[nPageIndex].pPage = new CTimeTablePage;
            }
            bLoad = LoadPage(m_PageInfo[nPageIndex].pPage,
                rect, ID_TIMETABLE, m_PageInfo[nPageIndex].pCategory);
		}
		break;
    case SCHEDULER:
        {
            if (m_PageInfo[nPageIndex].pPage == NULL)
            {
                m_PageInfo[nPageIndex].pPage = new CSchedulerPage();
            }
            bLoad = LoadPage(m_PageInfo[nPageIndex].pPage,
                rect, ID_SCHEDULER, m_PageInfo[nPageIndex].pCategory);

        }
        break;
    case CLIENTMANAGE:
        {
            if (m_PageInfo[nPageIndex].pPage == NULL)
            {
                m_PageInfo[nPageIndex].pPage = new CClientManagerPage;
            }
            bLoad = LoadPage(m_PageInfo[nPageIndex].pPage,
                rect, ID_CLIENTMANAGE, m_PageInfo[nPageIndex].pCategory);

        }
        break;
    case USERMANAGE:
        {
            if (m_PageInfo[nPageIndex].pPage == NULL)
            {
                m_PageInfo[nPageIndex].pPage = new CUserManagerPage;
            }
            bLoad = LoadPage(m_PageInfo[nPageIndex].pPage,
                rect, ID_USERMANAGE, m_PageInfo[nPageIndex].pCategory);

        }
        break;
    case LOG:
        {
            if (m_PageInfo[nPageIndex].pPage == NULL)
            {
                m_PageInfo[nPageIndex].pPage = new CLogSystemPage;
            }
            bLoad = LoadPage(m_PageInfo[nPageIndex].pPage,
                rect, ID_LOGVIEW, m_PageInfo[nPageIndex].pCategory);

        }
        break;
	case ALARM:
		{
			if (m_PageInfo[nPageIndex].pPage == NULL)
			{
				m_PageInfo[nPageIndex].pPage = new CAlarmSystemPage;
			}
			bLoad = LoadPage(m_PageInfo[nPageIndex].pPage,
				rect, ID_ALARMVIEW, m_PageInfo[nPageIndex].pCategory);

		}
		break;
    case REPORT:
        {
            if (m_PageInfo[nPageIndex].pPage == NULL)
            {
                m_PageInfo[nPageIndex].pPage = new CReportSystemPage;
            }
            bLoad = LoadPage(m_PageInfo[nPageIndex].pPage,
                rect, ID_REPORTVIEW, m_PageInfo[nPageIndex].pCategory);
        }
        break;
    default:
        break;
    }

	Translate(this);
    return bLoad;
}

LRESULT CControllerFrame::OnChangeRibbonCategory(WPARAM,LPARAM)
{
    BOOL bChanged = FALSE;
    BOOL bLoad = FALSE;
    CRect rect;

    CMFCRibbonCategory* pCategory = m_wndRibbonBar.GetActiveCategory ();
    if (pCategory != NULL)
    {
        ASSERT_VALID (pCategory);

        if(pCategory != m_pActiveCategory)
        {
            bChanged = TRUE;
        }
    }

    if (!bChanged)
    {
        return 0;
    }
    CBaseControllerPage* pPage = GetCurActivePage();
    if(pPage)
    {
        pPage->ShowWindow(SW_HIDE);
    }

    m_dockManager.GetClientAreaBounds(rect);

    UINT PageIndex = FindPage(pCategory);


    bLoad = LoadPageByCategory(m_PageInfo[PageIndex].CategoryType, rect, PageIndex);

    if (!bLoad)
    {
        return -1;
    }
    m_pActiveCategory = pCategory;
    m_wndRibbonBar.RecalcLayout();

    return 0;
}

LRESULT CControllerFrame::OnCloseCurrentPage(WPARAM,LPARAM)
{
    INT PageIndex = FindPage(m_pActiveCategory);
    CBaseControllerPage *pPage = m_PageInfo[PageIndex].pPage;
    CMFCRibbonCategory  *pCategory = m_PageInfo[PageIndex].pCategory;

    if(pPage)
    {
        pPage->ShowWindow(SW_HIDE);

        BOOL bChanged = FALSE;
        BOOL bLoad = FALSE;
        CRect rect;

        m_dockManager.GetClientAreaBounds(rect);

        bLoad = LoadPageByCategory(m_PageInfo[PageIndex - 1].CategoryType, rect, PageIndex - 1);

        m_wndRibbonBar.ShowCategory(PageIndex - 1);

        m_wndRibbonBar.SetActiveCategory(m_PageInfo[PageIndex - 1].pCategory);

        m_wndRibbonBar.RemoveCategory(PageIndex);

        m_wndRibbonBar.RecalcLayout();

        // issue 81782
		// CDesignerPage::OnDestroy is not called, so we remove it from Frame list manually
        if (dynamic_cast<CDesignerPage*>(pPage))
        {
            CFrameImpl::RemoveFrame(pPage);
        }
        pPage->Close();
        pPage->DestroyWindow();

        m_PageInfo.RemoveAt(PageIndex);

    }
    return 0;
}

BOOL CControllerFrame::CreateRibbonBar()
{
    if (!m_wndRibbonBar.Create(this))
    {
        TRACE0("Failed to create ribbon bar\n");
        return FALSE;      // fail to create
    }

    // Dynamic init the ribbon bar and categories
    if (GetControllerInterface->CheckPrivilege(UserPrivilege_GetMedia) &&
        GetControllerInterface->CheckPrivilege(UserPrivilege_ListMediaFilter))
    {
        Add_Category_MediaLibrary();
    }
    if (GetControllerInterface->CheckPrivilege(UserPrivilege_GetLayout) &&
        GetControllerInterface->CheckPrivilege(UserPrivilege_ListLayoutFilter))
    {
        Add_Category_LayoutManage();
    }
    if (GetControllerInterface->CheckPrivilege(UserPrivilege_GetMedia) &&
        GetControllerInterface->CheckPrivilege(UserPrivilege_ListMediaFilter) &&
        GetControllerInterface->CheckPrivilege(UserPrivilege_GetLayout) &&
        GetControllerInterface->CheckPrivilege(UserPrivilege_ListLayoutFilter) &&
        GetControllerInterface->CheckPrivilege(UserPrivilege_GetTimetable) &&
        GetControllerInterface->CheckPrivilege(UserPrivilege_ListTimetableFilter))
    {
        Add_Category_TimeTable();
    }
    if (GetControllerInterface->CheckPrivilege(UserPrivilege_GetLayout) &&
        GetControllerInterface->CheckPrivilege(UserPrivilege_ListLayoutFilter) &&
        GetControllerInterface->CheckPrivilege(UserPrivilege_GetTimetable) &&
        GetControllerInterface->CheckPrivilege(UserPrivilege_ListTimetableFilter) &&
        GetControllerInterface->CheckPrivilege(UserPrivilege_GetSchedule) /*&&
        GetControllerInterface->CheckPrivilege(UserPrivilege_ListScheduleFilter)*/)
    {
        Add_Category_Scheduler();
    }
#ifndef STANDALONE_EDITION
    if (GetControllerInterface->CheckPrivilege(UserPrivilege_GetClient) &&
        GetControllerInterface->CheckPrivilege(UserPrivilege_ListClientGroup))
    {
        Add_Category_ClientManage();
    }
#endif
#if defined(SERVER_EDITION) || (defined(PROFESSIONAL_EDITION) && !defined(STANDALONE_EDITION))
	if (GetControllerInterface->CheckPrivilege(UserPrivilege_ListReport))
	{
		Add_Category_Alarm();
	}
#endif
    if (GetControllerInterface->CheckPrivilege(UserPrivilege_ListReport))
    {
        Add_Category_Report();
    }
	
#ifndef STANDALONE_EDITION
    Add_Category_UserManage();
#endif
    if (GetControllerInterface->CheckPrivilege(UserPrivilege_ListLog))
    {
        Add_Category_Log();
    }

    // Add "Options" button to the right of tabs:
    m_wndRibbonBar.AddToTabs (new CMFCRibbonButton (ID_SYS_SETTING, Translate(_T("Options")), m_PanelImages.ExtractIcon (0)));


	// Add "Help" button to the right of tabs:
	CMFCRibbonButton* pBtnHelp = new CMFCRibbonButton (ID_CONTEXT_HELP, Translate(_T("Help")), m_PanelImages.ExtractIcon (0));
	pBtnHelp->SetToolTipText(Translate(_T("Help")) + _T("\n") + Translate(_T("Display help document")));
	m_wndRibbonBar.AddToTabs (pBtnHelp);

    // TODO: hide this button until we have some thing to show.
    // Add "Help" button to the right of tabs:
    //m_wndRibbonBar.AddToTabs (new CMFCRibbonButton (ID_HELP_USING, Translate(_T("Help")), m_PanelImages.ExtractIcon (0)));

    // Add "About" button to the right of tabs:
	CMFCRibbonButton* pBtnAbout = new CMFCRibbonButton (ID_APP_ABOUT, Translate(_T("About")), m_PanelImages.ExtractIcon (0));
   	pBtnAbout->SetToolTipText(Translate(_T("About")) + _T("\n") + Translate(_T("Display program information, version number and copyright")));
	m_wndRibbonBar.AddToTabs (pBtnAbout);
#ifdef SERVER_EDITION
    m_wndRibbonBar.AddToTabs (new CMFCRibbonButton (ID_SYS_RE_CONNECT, Translate(_T("Reconnect")), m_PanelImages.ExtractIcon (0)));
#endif

    return TRUE;
}


BOOL CControllerFrame::LoadPage(CBaseControllerPage* pPage, CRect rect, UINT nID, CMFCRibbonCategory *pCategory)
{
    if (pPage->GetSafeHwnd() == NULL)
    {
        pPage->BindRibbonCategory(pCategory);
        if (!pPage->Create(NULL, NULL, WS_VISIBLE | WS_CHILDWINDOW |	WS_CLIPSIBLINGS | WS_CLIPCHILDREN, rect, this, NULL, NULL))
        {
            return FALSE;
        }
        Translate(pPage);
    }
    m_wndRibbonBar.SetOwner(pPage);

    pPage->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER | SWP_NOACTIVATE);

    pPage->ShowWindow(SW_SHOW);

    if (!GetControllerApp->IsLogin() && !pPage->IsKindOf( RUNTIME_CLASS( CDesignerPage )))
    {
        pPage->SetEnable(FALSE);
    }
    else
    {
        pPage->SetEnable(TRUE);
    }

    return TRUE;
}

int CControllerFrame::CreateNewPage(CATEGORY_TYPE  CategoryType, INT UserData)
{
    CONTROLLER_PAGE_INFO PageInfo;

    PageInfo.pCategory = NULL;
    PageInfo.pPage = NULL;
    PageInfo.CategoryType = CategoryType;
    PageInfo.UserData = UserData;

    m_PageInfo.Add(PageInfo);

    return m_PageInfo.GetSize() - 1;
}

int  CControllerFrame::FindPage(CATEGORY_TYPE  CategoryType, INT UserData)
{
    for(int i=0; i< m_PageInfo.GetSize(); i++)
    {
        if(m_PageInfo[i].CategoryType == CategoryType && m_PageInfo[i].UserData == UserData)
        {
            return i;
        }
    }

    return -1;
}

int  CControllerFrame::FindPage(CMFCRibbonCategory *pCategory)
{
    for(int i=0; i< m_PageInfo.GetSize(); i++)
    {
        if(m_PageInfo[i].pCategory == pCategory)
        {
            return i;
        }
    }

    return -1;
}


void CControllerFrame::Add_Ribbon_MainPanel()
{
    m_MainButton.SetImage (IDB_MAIN);
    m_MainButton.SetToolTipText (Translate(_T("Main Control")));
    m_MainButton.SetText (_T("F"));

    m_wndRibbonBar.SetApplicationButton (&m_MainButton, CSize (45, 45));

    CMFCRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory (Translate(_T("Main Control")), IDB_FILESMALL, IDB_FILELARGE);

    pMainPanel->Add (new CMFCRibbonButton (ID_FILE_SAVE, Translate(_T("Save\ns")), 0, 0));

    pMainPanel->Add (new CMFCRibbonButton (ID_FILE_SAVE_AS,Translate( _T("Save As...\nf")), 1, 1));

    //pMainPanel->AddRecentFilesList (_T("Recent List"));

    //pMainPanel->AddToBottom (new CMFCRibbonMainPanelButton (ID_SYS_SETTING, _T("Opt&ions"), 8));
    pMainPanel->AddToBottom (new CMFCRibbonMainPanelButton (ID_APP_EXIT, Translate(_T("Exit\nx")), 10));


}

void CControllerFrame::Add_Ribbon_QAT()
{
    // Add quick access toolbar commands:
    CMFCRibbonQuickAccessToolBarDefaultState qatState;

    qatState.AddCommand(ID_SYS_SETTING);

    // Set the state
    m_wndRibbonBar.SetQuickAccessDefaultState(qatState);

}

BOOL CControllerFrame::Add_Category_MediaLibrary()
{

    if (!m_CategoryIndexMapper.IsEnable(MEDIALIBRARY))
    {
        return FALSE;
    }

    CString strCategory     = Translate(_T("Media Library\nm"));

    INT CategotyIndex = CreateNewPage(MEDIALIBRARY, 0);

    CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(strCategory, IDB_MEDIALIB_SMALL, IDB_MEDIALIB_LARGE,
        CSize(16, 16), CSize(32, 32), CategotyIndex);

    m_PageInfo[CategotyIndex].pCategory = pCategory;

    pCategory->SetData(CategotyIndex);

    /// Create Dummy panel in main frame init, and then replace it with the real panel.
    /// If we don't create any panel in the main frame init, then later create panel just like menu, can not display the ribbon buttons
    CString strDummy     = Translate(_T("Loading"));
    CMFCRibbonPanel* pPanel = pCategory->AddPanel(strDummy);

    return TRUE;
}

BOOL CControllerFrame::Add_Category_LayoutManage()
{
    if (!m_CategoryIndexMapper.IsEnable(LAYOUTMANAGER))
    {
        return FALSE;
    }

    CString strCategory     = Translate(_T("Layout Manager\nl"));

    INT CategotyIndex = CreateNewPage(LAYOUTMANAGER, 0);

    CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(strCategory, IDB_LAYOUTMANAGE_SMALL, IDB_LAYOUTMANAGE_LARGE,
        CSize(16, 16), CSize(32, 32), CategotyIndex);

    m_PageInfo[CategotyIndex].pCategory = pCategory;

    pCategory->SetData(CategotyIndex);

    /// Create Dummy panel in main frame init, and then replace it with the real panel.
    /// If we don't create any panel in the main frame init, then later create panel just like menu, can not display the ribbon buttons
    CString strDummy     = Translate(_T("Loading"));
    CMFCRibbonPanel* pPanel = pCategory->AddPanel(strDummy);

    return TRUE;
}


BOOL CControllerFrame::Add_Category_Designer()
{
    if (!m_CategoryIndexMapper.IsEnable(DESIGNER))
    {
        return FALSE;
    }

    CString strCategory     = Translate(_T("Layout Designer\nd"));

    INT CategotyIndex = CreateNewPage(DESIGNER, 0);

    CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(strCategory, IDB_LAYOUT_SMALL, IDB_LAYOUT_LARGE,
        CSize(16, 16), CSize(32, 32), CategotyIndex);

    m_PageInfo[CategotyIndex].pCategory = pCategory;

    pCategory->SetData(CategotyIndex);

    /// Create Dummy panel in main frame init, and then replace it with the real panel.
    /// If we don't create any panel in the main frame init, then later create panel just like menu, can not display the ribbon buttons
    CString strDummy     = Translate(_T("Loading"));
    CMFCRibbonPanel* pPanel = pCategory->AddPanel(strDummy);

    return TRUE;
}

BOOL CControllerFrame::Add_Category_TimeTable()
{
    if (!m_CategoryIndexMapper.IsEnable(TIMETABLE))
    {
        return FALSE;
    }

	CString strCategory     = Translate(_T("Timetable Designer\nt"));

    INT CategotyIndex = CreateNewPage(TIMETABLE, 0);


	CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(strCategory, IDB_TIMETABLE_SMALL, IDB_TIMETABLE_LARGE,
		CSize(16, 16), CSize(32, 32), CategotyIndex);

    m_PageInfo[CategotyIndex].pCategory = pCategory;

    pCategory->SetData(CategotyIndex);

	/// Create Dummy panel in main frame init, and then replace it with the real panel.
	/// If we don't create any panel in the main frame init, then later create panel just like menu, can not display the ribbon buttons
	CString strDummy     = Translate(_T("Loading"));
	CMFCRibbonPanel* pPanel = pCategory->AddPanel(strDummy);

	return TRUE;
}

BOOL CControllerFrame::Add_Category_Scheduler()
{
    if (!m_CategoryIndexMapper.IsEnable(SCHEDULER))
    {
        return FALSE;
    }

    CString strCategory     = Translate(_T("Scheduler\ns"));

    INT CategotyIndex = CreateNewPage(SCHEDULER, 0);


    CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(strCategory, IDB_SCHEDULER_SMALL, IDB_SCHEDULER_LARGE,
        CSize(16, 16), CSize(32, 32), CategotyIndex);

    m_PageInfo[CategotyIndex].pCategory = pCategory;

    pCategory->SetData(CategotyIndex);

    /// Create Dummy panel in main frame init, and then replace it with the real panel.
    /// If we don't create any panel in the main frame init, then later create panel just like menu, can not display the ribbon buttons
    CString strDummy     = Translate(_T("Loading"));
    CMFCRibbonPanel* pPanel = pCategory->AddPanel(strDummy);

    return TRUE;
}

BOOL CControllerFrame::Add_Category_ClientManage()
{
    if (!m_CategoryIndexMapper.IsEnable(CLIENTMANAGE))
    {
        return FALSE;
    }

    CString strCategory         = Translate(_T("Client Manager\nc"));

    INT CategotyIndex = CreateNewPage(CLIENTMANAGE, 0);


    CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(strCategory, IDB_CLIENTMGR_SMALL, IDB_CLIENTMGR_LARGE,
        CSize(16, 16), CSize(32, 32), CategotyIndex);

    m_PageInfo[CategotyIndex].pCategory = pCategory;

    pCategory->SetData(CategotyIndex);

    /// Create Dummy panel in main frame init, and then replace it with the real panel.
    /// If we don't create any panel in the main frame init, then later create panel just like menu, can not display the ribbon buttons
    CString strDummy     = Translate(_T("Loading"));
    CMFCRibbonPanel* pPanel = pCategory->AddPanel(strDummy);

    return TRUE;
}

BOOL CControllerFrame::Add_Category_UserManage()
{
    if (!m_CategoryIndexMapper.IsEnable(USERMANAGE))
    {
        return FALSE;
    }

    CString strCategory  = Translate(_T("User Manager\nu"));

    INT CategotyIndex = CreateNewPage(USERMANAGE, 0);

    CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(strCategory, IDB_USERMGR_SMALL, IDB_USERMGR_LARGE,
        CSize(16, 16), CSize(32, 32), CategotyIndex);

    m_PageInfo[CategotyIndex].pCategory = pCategory;

    pCategory->SetData(CategotyIndex);

    /// Create Dummy panel in main frame init, and then replace it with the real panel.
    /// If we don't create any panel in the main frame init, then later create panel just like menu, can not display the ribbon buttons
    CString strDummy     = Translate(_T("Loading"));
    CMFCRibbonPanel* pPanel = pCategory->AddPanel(strDummy);

    return TRUE;
}

BOOL CControllerFrame::Add_Category_Log()
{
    if (!m_CategoryIndexMapper.IsEnable(LOG))
    {
        return FALSE;
    }

    CString strCategory         = Translate(_T("Log\no"));

    INT CategotyIndex = CreateNewPage(LOG, 0);


    CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(strCategory, IDB_LOG_SMALL, IDB_LOG_LARGE,
        CSize(16, 16), CSize(32, 32), CategotyIndex);

    m_PageInfo[CategotyIndex].pCategory = pCategory;

    pCategory->SetData(CategotyIndex);

    /// Create Dummy panel in main frame init, and then replace it with the real panel.
    /// If we don't create any panel in the main frame init, then later create panel just like menu, can not display the ribbon buttons
    CString strDummy     = Translate(_T("Loading"));
    CMFCRibbonPanel* pPanel = pCategory->AddPanel(strDummy);

    return TRUE;
}

BOOL CControllerFrame::Add_Category_Alarm()
{
	if (!m_CategoryIndexMapper.IsEnable(ALARM))
	{
		return FALSE;
	}

	CString strCategory         = Translate(_T("Client Alarm\na"));

	INT CategotyIndex = CreateNewPage(ALARM, 0);


	CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(strCategory, IDB_LOG_SMALL, IDB_LOG_LARGE, //need modify IDB
		CSize(16, 16), CSize(32, 32), CategotyIndex);

	m_PageInfo[CategotyIndex].pCategory = pCategory;

	pCategory->SetData(CategotyIndex);

	/// Create Dummy panel in main frame init, and then replace it with the real panel.
	/// If we don't create any panel in the main frame init, then later create panel just like menu, can not display the ribbon buttons
	CString strDummy     = Translate(_T("Loading"));
	CMFCRibbonPanel* pPanel = pCategory->AddPanel(strDummy);

	return TRUE;
}

BOOL CControllerFrame::Add_Category_Report()
{
    if (!m_CategoryIndexMapper.IsEnable(REPORT))
    {
        return FALSE;
    }

    CString strCategory         = Translate(_T("Client Report\nr"));

    INT CategotyIndex = CreateNewPage(REPORT, 0);


    CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(strCategory, IDB_LOG_SMALL, IDB_LOG_LARGE, //need modify IDB
        CSize(16, 16), CSize(32, 32), CategotyIndex);

    m_PageInfo[CategotyIndex].pCategory = pCategory;

    pCategory->SetData(CategotyIndex);

    /// Create Dummy panel in main frame init, and then replace it with the real panel.
    /// If we don't create any panel in the main frame init, then later create panel just like menu, can not display the ribbon buttons
    CString strDummy     = Translate(_T("Loading"));
    CMFCRibbonPanel* pPanel = pCategory->AddPanel(strDummy);

    return TRUE;
}

BOOL CControllerFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	class friendFrame :public CFrameWndEx
	{
		friend class CControllerFrame;   
	};
	if (! __super::OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
	{
		friendFrame *pFrame =  (friendFrame *)GetCurActivePage();
		return pFrame->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo);
	}
}

LRESULT CControllerFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID(this);

	return __super::WindowProc(message,wParam,lParam);
}

VOID CControllerFrame::OnOptions()
{
    CControllerSettings ContollerOptionDlg(GetConfig, this);

    if(ContollerOptionDlg.DoModal() == IDOK)
    {

    }
}

CATEGORY_TYPE CControllerFrame::GetCurActivePageType()
{
    INT PageIndex = FindPage(m_pActiveCategory);

    if(PageIndex >=0) return m_PageInfo[PageIndex].CategoryType;

    return CATEGORY_MAX;
}

void CControllerFrame::OnReconnect()
{
    if (!GetControllerApp->IsLogin())
    {
        GetControllerApp->Relogin();
    }

}

BOOL CControllerFrame::UpdateOptions(CControllerSettings::SETTING_PAGE nPage)
{
    INT PageCnt = m_PageInfo.GetSize();

    BOOL bUpdate = TRUE;

    switch(nPage)
    {
	//update rotation on schedule
	case CControllerSettings::SETTING_PAGE_COMMON:
		for(int i = 0; i < PageCnt; i++)
		{
			if (m_PageInfo[i].pPage && m_PageInfo[i].pPage->IsKindOf(RUNTIME_CLASS(CSchedulerPage)))
			{
				CSchedulerPage* pPage = static_cast<CSchedulerPage*>(m_PageInfo[i].pPage);
				if (!pPage->UpdateOptions())
				{
					bUpdate = FALSE;
				}
			}
		}
		break;
    case CControllerSettings::SETTING_PAGE_DESIGNER:
        {
            for(int i = 0; i < PageCnt; i++)
            {
                if (m_PageInfo[i].pPage && m_PageInfo[i].pPage->IsKindOf(RUNTIME_CLASS(CDesignerPage)))
                {
                    CDesignerPage* pPage = static_cast<CDesignerPage*>(m_PageInfo[i].pPage);
                    if (!pPage->UpdateOptions())
                    {
                        bUpdate = FALSE;
                    }
                }
            }
        }
        break;
	case CControllerSettings::SETTING_PAGE_TIMETABLE:
		{
			for(int i = 0; i < PageCnt; i++)
			{
				if (m_PageInfo[i].pPage && m_PageInfo[i].pPage->IsKindOf(RUNTIME_CLASS(CTimeTablePage)))
				{
					CTimeTablePage* pPage = static_cast<CTimeTablePage*>(m_PageInfo[i].pPage);
					if (!pPage->UpdateOptions())
					{
						bUpdate = FALSE;
					}
				}
			}
		}
		break;
    case CControllerSettings::SETTING_PAGE_SCHEDULER:
        {

            for(int i = 0; i < PageCnt; i++)
            {
                if (m_PageInfo[i].pPage && m_PageInfo[i].pPage->IsKindOf(RUNTIME_CLASS(CSchedulerPage)))
                {
                    CSchedulerPage* pPage = static_cast<CSchedulerPage*>(m_PageInfo[i].pPage);
                    if (!pPage->UpdateOptions())
                    {
                        bUpdate = FALSE;
                    }
                }
            }



        }
        break;
    case CControllerSettings::SETTING_PAGE_CLIENT_MANAGER:
        {
            for(int i = 0; i < PageCnt; i++)
            {
                if (m_PageInfo[i].pPage && m_PageInfo[i].pPage->IsKindOf(RUNTIME_CLASS(CClientManagerPage)))
                {
                    CClientManagerPage* pPage = static_cast<CClientManagerPage*>(m_PageInfo[i].pPage);
                    if (!pPage->UpdateOptions())
                    {
                        bUpdate = FALSE;
                    }
                }
            }
        }
        break;
    }

    return bUpdate;
}

LRESULT CControllerFrame::OnControllerOffline(WPARAM wp,LPARAM)
{
    BOOL bLoginByOthers = (BOOL)wp;
    GetControllerApp->OnControllerServerOffline(bLoginByOthers);
    return 0;
}

extern UINT WM_ON_LAYOUT_OPEN;

LRESULT CControllerFrame::OnOpenLayout(WPARAM wp,LPARAM lp)
{
    Layout* pLayout = (Layout*)lp;

    CWaitCursor wait;

    INT PageIndex = FindPage(DESIGNER, pLayout->GetLayoutID());
    // page exists
    if (PageIndex >= 0)
    {
        for (int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
        {
            CMFCRibbonCategory* category = m_wndRibbonBar.GetCategory(i);

            if (m_PageInfo[PageIndex].pCategory == category )
            {
                m_wndRibbonBar.ShowCategory(i);
                m_wndRibbonBar.SetActiveCategory(category);
            }
        }
    }
    else{

        CString strCategory;
        strCategory.Format(Translate(_T("Designer (%s)")), pLayout->GetLayoutName().c_str());

        INT CategotyIndex = CreateNewPage(DESIGNER, pLayout->GetLayoutID());

        CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(strCategory, IDB_LAYOUT_SMALL, IDB_LAYOUT_LARGE,
            CSize(16, 16), CSize(32, 32), CategotyIndex);

        pCategory->SetData(CategotyIndex);

        m_PageInfo[CategotyIndex].pCategory = pCategory;
        /// Create Dummy panel in main frame init, and then replace it with the real panel.
        /// If we don't create any panel in the main frame init, then later create panel just like menu, can not display the ribbon buttons
        CString strDummy     = Translate(_T("Loading"));
        CMFCRibbonPanel* pPanel = pCategory->AddPanel(strDummy);

        m_wndRibbonBar.ShowCategory(CategotyIndex);
        m_wndRibbonBar.SetActiveCategory(pCategory);

        m_PageInfo[CategotyIndex].pPage->SendMessage(WM_ON_LAYOUT_OPEN, wp, lp);
    }


    return 0;
}

LRESULT CControllerFrame::OnNewLayout(WPARAM wp,LPARAM  lp)
{
    CString strCategory  =Translate( _T("Designer (New Layout)"));
    UINT nTry = (UINT)wp;
    if (nTry > 1)
    {
        CString tmp;
        tmp.Format(_T(" (%d)"), nTry);
        strCategory += tmp;
    }

    CWaitCursor wait;

    INT CategotyIndex = CreateNewPage(DESIGNER, 0);

    CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(strCategory, IDB_LAYOUT_SMALL, IDB_LAYOUT_LARGE,
        CSize(16, 16), CSize(32, 32), CategotyIndex);

    pCategory->SetData(CategotyIndex);

    m_PageInfo[CategotyIndex].pCategory = pCategory;
    /// Create Dummy panel in main frame init, and then replace it with the real panel.
    /// If we don't create any panel in the main frame init, then later create panel just like menu, can not display the ribbon buttons
    CString strDummy     = Translate(_T("Loading"));
    CMFCRibbonPanel* pPanel = pCategory->AddPanel(strDummy);

    m_wndRibbonBar.ShowCategory(CategotyIndex);
    m_wndRibbonBar.SetActiveCategory(pCategory);


    CDesignerPage* pDesignerPage = static_cast<CDesignerPage*>(m_PageInfo[CategotyIndex].pPage);
    pDesignerPage->CreateLayout(lp);

    return 0;
}


LRESULT CControllerFrame::OnImportLayout(WPARAM,LPARAM lp)
{
#if defined(STANDALONE_EDITION) || defined(PROFESSIONAL_EDITION) || defined(SERVER_EDITION)

	#ifdef SERVER_EDITION
	if (GetControllerInterface->CheckPrivilege(UserPrivilege_CreateMedia) &&
		GetControllerInterface->CheckPrivilege(UserPrivilege_GetMedia) &&
		GetControllerInterface->CheckPrivilege(UserPrivilege_ApproveMedia) &&
		GetControllerInterface->CheckPrivilege(UserPrivilege_CreateMediaFilter) &&
		GetControllerInterface->CheckPrivilege(UserPrivilege_ListMediaFilter) 
		)
	{
		
	}
	else
	{
		MessageBox(Translate(_T("You don't have privilege to move/rename/edit layout.")), Translate(_T("Warning: Edit layout")), MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}
	#endif

	ImportLayoutDlg dlg(this);

	if(IDOK == dlg.DoModal())
	{
		CString strCategory  = Translate(_T("Designer (Imported Layout)"));

		//CWaitCursor wait;

		INT CategotyIndex = CreateNewPage(DESIGNER, 0);

		CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(strCategory, IDB_LAYOUT_SMALL, IDB_LAYOUT_LARGE,
			CSize(16, 16), CSize(32, 32), CategotyIndex);

		pCategory->SetData(CategotyIndex);

		m_PageInfo[CategotyIndex].pCategory = pCategory;
		/// Create Dummy panel in main frame init, and then replace it with the real panel.
		/// If we don't create any panel in the main frame init, then later create panel just like menu, can not display the ribbon buttons
		CString strDummy     = Translate(_T("Loading"));
		CMFCRibbonPanel* pPanel = pCategory->AddPanel(strDummy);

		m_wndRibbonBar.ShowCategory(CategotyIndex);
		m_wndRibbonBar.SetActiveCategory(pCategory);


		CDesignerPage* pDesignerPage = static_cast<CDesignerPage*>(m_PageInfo[CategotyIndex].pPage);

		int nStrLen;
		TCHAR szTmpPath[MAX_PATH];
		nStrLen = GetTempPath(MAX_PATH, szTmpPath);
		szTmpPath[nStrLen] = 0x00;

		CString xmlFilename = szTmpPath;
		xmlFilename += _T("\\index.xml");
		pDesignerPage->ImportLayout(lp, xmlFilename);
	}
	return 0;
#else
	/* By RaymondLiu
	CString filePath;
    CFileDialog fileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, L"XML (*.xml)|*.xml|", this);
    if (fileDialog.DoModal() == IDOK)
    {
        filePath = fileDialog.GetPathName();

		if( _taccess( filePath, 0 ) == -1 )
		{
#ifdef SERVER_EDITION
			MessageBox(Translate(_T("File not found!")),Translate(_T("Error:Controller")), MB_OK|MB_ICONERROR);
#else
            MessageBox(Translate(_T("File not found!")),Translate(_T("Error:Manager")), MB_OK|MB_ICONERROR);
#endif
			return 0;
		}

        CString strCategory  = Translate(_T("Designer (Imported Layout)"));

        CWaitCursor wait;

        INT CategotyIndex = CreateNewPage(DESIGNER, 0);

        CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(strCategory, IDB_LAYOUT_SMALL, IDB_LAYOUT_LARGE,
            CSize(16, 16), CSize(32, 32), CategotyIndex);

        pCategory->SetData(CategotyIndex);

        m_PageInfo[CategotyIndex].pCategory = pCategory;
        /// Create Dummy panel in main frame init, and then replace it with the real panel.
        /// If we don't create any panel in the main frame init, then later create panel just like menu, can not display the ribbon buttons
        CString strDummy     = Translate(_T("Loading"));
        CMFCRibbonPanel* pPanel = pCategory->AddPanel(strDummy);

        m_wndRibbonBar.ShowCategory(CategotyIndex);
        m_wndRibbonBar.SetActiveCategory(pCategory);


        CDesignerPage* pDesignerPage = static_cast<CDesignerPage*>(m_PageInfo[CategotyIndex].pPage);
        pDesignerPage->ImportLayout(lp, filePath);

    }*/
	return 0;
	
#endif
}



void CControllerFrame::OnUpdateRibbonBtnPreviewLayout(CCmdUI* pCmdUI)
{
    if (pCmdUI->m_nID >= ID_RIBBON_BTN_PREVIEW_10X7 && pCmdUI->m_nID <= ID_RIBBON_BTN_PREVIEW_19X10)
    {
        GetCurActivePage()->OnUpdateRibbonBtnPreview(pCmdUI);
    }
}


void CControllerFrame::EnableCurPage(BOOL bEnable)
{
    int nIdx = 0;

    CMFCRibbonCategory* pCategory = m_wndRibbonBar.GetActiveCategory ();
    if (pCategory != NULL)
    {
        ASSERT_VALID (pCategory);

        nIdx = m_wndRibbonBar.GetCategoryIndex(pCategory);
    }

    CBaseControllerPage* pPage = GetCurActivePage();
    if(pPage && !pPage->IsKindOf( RUNTIME_CLASS( CDesignerPage )))
    {
        pPage->SetEnable(bEnable);
    }
}


BOOL CControllerFrame::RefreshPages()
{
    INT PageCnt = m_PageInfo.GetSize();

    BOOL bRefreshSucceed = TRUE;
    for(int i = 0; i < PageCnt; i++)
    {
        if (m_PageInfo[i].pPage)
        {
            if (!m_PageInfo[i].pPage->RefreshPage())
            {
                bRefreshSucceed = FALSE;
            }

            if (!GetControllerApp->IsLogin() && !m_PageInfo[i].pPage->IsKindOf( RUNTIME_CLASS( CDesignerPage )))
            {
                m_PageInfo[i].pPage->SetEnable(FALSE);
            }
            else
            {
                m_PageInfo[i].pPage->SetEnable(TRUE);
            }
        }
    }



    return bRefreshSucceed;
}

void CControllerFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
    CFrameWndEx::OnGetMinMaxInfo(lpMMI); 

    lpMMI-> ptMinTrackSize.x   =   640; 
    lpMMI-> ptMinTrackSize.y   =   480;
}

LRESULT CControllerFrame::OnPowerBroadcast(WPARAM, LPARAM)
{
    return Default();
}

void CControllerFrame::OnSysColorChange()
{
    CFrameWnd::OnSysColorChange();
    //m_Impl.OnChangeVisualManager();
    SetWindowRgn(NULL, TRUE);
}


LRESULT  CControllerFrame::OnThumbnailLibCallBack(WPARAM wParam,LPARAM lParam)
{
	CallBack_Func  pFunc;
	pFunc = (CallBack_Func )wParam;

	int loParam = LOWORD(lParam);
	int hiParam = HIWORD(lParam);

	BOOL bRet = pFunc(this,loParam,hiParam);
	if(bRet)
		return TRUE;

	return FALSE;
}
CATEGORY_INDEX_MAPPER::CATEGORY_INDEX_MAPPER()
{
    m_EnabledCnt = 0;
    for (UINT i = 0; i < CATEGORY_MAX; i++)
    {
        m_bEnable[i] = TRUE;
        m_Index[i] = m_EnabledCnt;
        m_EnabledCnt++;
    }
}

CATEGORY_INDEX_MAPPER::~CATEGORY_INDEX_MAPPER()
{

}

void CATEGORY_INDEX_MAPPER::EnableCategory(CATEGORY_TYPE Category)
{
    m_bEnable[Category] = TRUE;
    m_EnabledCnt = 0;
    for (UINT i = 0; i < CATEGORY_MAX; i++)
    {
        if (m_bEnable[i])
        {
            m_Index[i] = m_EnabledCnt;
            m_EnabledCnt++;
        }
    }
}

void CATEGORY_INDEX_MAPPER::DisableCategory(CATEGORY_TYPE Category)
{
    m_bEnable[Category] = FALSE;
    m_EnabledCnt = 0;
    for (UINT i = 0; i < CATEGORY_MAX; i++)
    {
        if (m_bEnable[i])
        {
            m_Index[i] = m_EnabledCnt;
            m_EnabledCnt++;
        }
    }
}

BOOL CATEGORY_INDEX_MAPPER::IsEnable(CATEGORY_TYPE Category)
{
    return m_bEnable[Category];
}

INT CATEGORY_INDEX_MAPPER::GetEnabledCategoryCnt()
{
    return m_EnabledCnt;
}


void CControllerFrame::OnSizing(UINT fwSide, LPRECT pRect)
{
#define MINI_FRAME_WIDTH		800
#define MINI_FRAME_HEIGHT		600  

	if( (pRect->right - pRect->left) <= MINI_FRAME_WIDTH)
		 pRect->right = pRect->left + MINI_FRAME_WIDTH;
	if( (pRect->bottom - pRect->top) <= MINI_FRAME_HEIGHT)
		 pRect->bottom = pRect->top + MINI_FRAME_HEIGHT;

	CFrameWndEx::OnSizing(fwSide, pRect);
	// TODO: Add your message handler code here
}
 