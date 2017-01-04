#pragma once


// CMediaFileBrowserPane

#include "ClientChartPane.h"
#include "MediaLibraryCategoryCtrl.h"
#include <set>

class CMediaFileBrowser : public CMFCTabCtrl
{
    DECLARE_DYNAMIC(CMediaFileBrowser)

public:
    CMediaFileBrowser();
    virtual ~CMediaFileBrowser();

    CMediaLibraryCategoryCtrl*       m_wndMediaLibrary;
    std::map<CWnd*, int> m_wndCharts;

    virtual BOOL RemoveTab(int iTab, BOOL bRecalcLayout = TRUE);

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    virtual void FireChangeActiveTab(int nNewTab);

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg LRESULT MediaCategoryListSelChange(WPARAM wp, LPARAM lp);
};

// CClientStatePane

class CMediaFileBrowserPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CMediaFileBrowserPane)

public:
    CMediaFileBrowserPane();
    virtual ~CMediaFileBrowserPane();

#ifndef STARTER_EDITION       
    CMediaFileBrowser         m_wndStateTab;

#else

    CMFCShellTreeCtrl         m_wndShellTree;

#endif

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();
};



