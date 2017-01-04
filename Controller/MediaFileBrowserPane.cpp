// F:\p4\sw\Demo\ContentManagement\Controller\Client Monitor\ClientStatePane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "MediaFileBrowserPane.h"
#include "Localization/Localization.h"


// CClientStatePane

IMPLEMENT_DYNAMIC(CMediaFileBrowserPane, CBaseControllerPane)

CMediaFileBrowserPane::CMediaFileBrowserPane()
{
}

CMediaFileBrowserPane::~CMediaFileBrowserPane()
{

}


BEGIN_MESSAGE_MAP(CMediaFileBrowserPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_TIMER()
END_MESSAGE_MAP()



// CClientStatePane message handlers
int CMediaFileBrowserPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

#ifndef STARTER_EDITION

    RECT rect;
    GetClientRect(&rect);

    m_wndStateTab.Create(CMFCTabCtrl::STYLE_3D_VS2005, rect, this, IDW_LIST_CLIENT_TAB);
    m_wndStateTab.SetLocation(CMFCBaseTabCtrl::LOCATION_TOP);
    m_wndStateTab.EnableActiveTabCloseButton(FALSE);
    m_wndStateTab.AutoDestroyWindow(TRUE);

    SetMainWnd(&m_wndStateTab);

#else

    CRect rectDummy (0, 0, 0, 0);
    const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | TVS_HASLINES | 
        TVS_LINESATROOT | TVS_HASBUTTONS;

    m_wndShellTree.Create(dwViewStyle, rectDummy, this, IDC_MEDIAFILESTREE);
	m_wndShellTree.SetFlags(m_wndShellTree.GetFlags() | SHCONTF_INCLUDEHIDDEN);
    LPITEMIDLIST ppidl;
    if (SHGetSpecialFolderLocation(GetSafeHwnd(),CSIDL_BITBUCKET, &ppidl) != NOERROR)
    {
        return 0;
    }

    SHChangeNotifyEntry scnIDL;
    scnIDL.pidl = ppidl;
    scnIDL.fRecursive = TRUE;

    SetMainWnd(&m_wndShellTree);

#endif

    return 0;
}


void CMediaFileBrowserPane::OnDestroy()
{

#ifndef STARTER_EDITION
    m_wndStateTab.RemoveAllTabs();
    m_wndStateTab.DestroyWindow();
#endif

    CBaseControllerPane::OnDestroy();
}


//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMediaFileBrowser, CMFCTabCtrl)

CMediaFileBrowser::CMediaFileBrowser()
{
    HideSingleTab(TRUE);
}

CMediaFileBrowser::~CMediaFileBrowser()
{
    auto it = m_wndCharts.begin(), ite = m_wndCharts.end();
    for (; it != ite; ++it)
    {
        it->first->DestroyWindow();
        delete it->first;
    }

    m_wndCharts.clear();
}

BEGIN_MESSAGE_MAP(CMediaFileBrowser, CMFCTabCtrl)
    ON_WM_CREATE()
    ON_WM_LBUTTONUP()
    ON_REGISTERED_MESSAGE(MEDIA_LIST_CATEGORY, MediaCategoryListSelChange)
END_MESSAGE_MAP()


int CMediaFileBrowser::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMFCTabCtrl::OnCreate(lpCreateStruct) == -1)
        return -1;

    RECT rect;
    GetClientRect(&rect);

    m_wndMediaLibrary = new CMediaLibraryCategoryCtrl(this, TRUE);
    m_wndMediaLibrary->Create(CMediaLibraryCategoryCtrl::IDD,this);
    m_wndMediaLibrary->ShowWindow(SW_SHOW);

    m_wndCharts.insert(std::map<CWnd*, int>::value_type(m_wndMediaLibrary, 1));
    InsertTab(m_wndMediaLibrary, Translate(_T("Media Library")), 0);

    /*
    GetClientRect(&rect);

    CClientChartPane* wndChart = new CClientChartPane;
    wndChart->Create(_T("Workspace"), this, rect, TRUE, ID_VIEW_CLIENTSTATELIST, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE);

    m_wndCharts.insert(std::map<CWnd*, int>::value_type(wndChart, 2));
    InsertTab(wndChart, Translate(_T("Workspace")), GetTabsNum());
    */

    //SetActiveTab(0);

    return 0;
}


void CMediaFileBrowser::FireChangeActiveTab(int nNewTab)
{
    CMFCTabCtrl::FireChangeActiveTab(nNewTab);

    if (GetActiveWnd() == m_wndMediaLibrary)
        EnableActiveTabCloseButton(FALSE);
    else
        EnableActiveTabCloseButton(TRUE);
}

BOOL CMediaFileBrowser::RemoveTab(int iTab, BOOL bRecalcLayout /*= TRUE*/)
{
/*
    CString label;
    GetTabLabel(iTab, label);

    int clientId = _tstoi(label.GetString());*/

    
    CWnd* wnd = GetTabWnd(iTab);
    CMFCTabCtrl::RemoveTab(iTab, bRecalcLayout);

    delete wnd;
    auto it = m_wndCharts.find(wnd), ite = m_wndCharts.end();
    if (it != ite)
    {
        m_wndCharts.erase(wnd);
    }
    
    return TRUE;
}

void CMediaFileBrowser::OnLButtonUp(UINT nFlags, CPoint point)
{
    CWnd* pWndActive = NULL;

    if (m_bTabCloseButtonPressed)
    {
        if (m_rectCloseButton.PtInRect(point))
        {
            pWndActive = GetActiveWnd();
        }
    }
    
    CMFCTabCtrl::OnLButtonUp(nFlags, point);
    if (pWndActive)
    {
        RemoveTab(GetActiveTab());
    }
}

LRESULT CMediaFileBrowser::MediaCategoryListSelChange(WPARAM wp, LPARAM lp)
{
    GetParent()->SendMessage(MEDIA_LIST_CATEGORY, wp, lp);
    return 0;
}