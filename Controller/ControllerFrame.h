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

/// MainFrm.h : interface of the CControllerFrame class
//

/// The enum of the ribbon category index info:
/// Make sure the index increased 1 each time from start value 0, because we use it to create ribbon category and mark the categories,
/// and we use the (last value + 1) to size the main window container



#pragma once

typedef enum {
    MEDIALIBRARY    = 0,
    LAYOUTMANAGER   ,
    DESIGNER        ,
	TIMETABLE       ,
    SCHEDULER       ,
    CLIENTMANAGE    ,
    USERMANAGE      ,
    LOG             ,
	ALARM			,
    REPORT          ,
	HELP            ,
    CATEGORY_MAX
} CATEGORY_TYPE;

typedef struct CONTROLLER_PAGE_INFO
{
    CBaseControllerPage    *pPage;
    CMFCRibbonCategory     *pCategory;
    CATEGORY_TYPE           CategoryType;
    INT                     UserData;
}CONTROLLER_PAGE_INFO;


class CATEGORY_INDEX_MAPPER
{
public:
    CATEGORY_INDEX_MAPPER();
    ~CATEGORY_INDEX_MAPPER();

    void EnableCategory(CATEGORY_TYPE Category);
    void DisableCategory(CATEGORY_TYPE Category);

    BOOL IsEnable(CATEGORY_TYPE Category);
    INT  GetEnabledCategoryCnt();

private:
    BOOL m_bEnable[CATEGORY_MAX];
    INT  m_Index[CATEGORY_MAX];
    INT  m_EnabledCnt;
};


class CMyRibbonBar : public CMFCRibbonBar
{
public:
    virtual BOOL OnShowRibbonContextMenu(CWnd* pWnd, int x, int y, CMFCRibbonBaseElement* pHit)
    {
        return TRUE;
    }
};

class CControllerFrame : public CFrameWndEx
{
    friend class CControllerApp;

public:
    CControllerFrame();
protected:
    DECLARE_DYNAMIC(CControllerFrame)

/// Overrides
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

/// Implementation
public:
    virtual ~CControllerFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:  /// control bar embedded members
    CMyRibbonBar                m_wndRibbonBar;
    CMFCRibbonApplicationButton m_MainButton;
    CMFCToolBarImages           m_PanelImages;
    CMFCRibbonStatusBar         m_wndStatusBar;


    CBaseControllerPage*        GetCurActivePage();

    CArray<CONTROLLER_PAGE_INFO, CONTROLLER_PAGE_INFO &> m_PageInfo;

    BOOL                        m_bTimerSwitcher;
    CMFCRibbonCategory         *m_pActiveCategory;
public:
    CATEGORY_TYPE                GetCurActivePageType();

    CATEGORY_INDEX_MAPPER       m_CategoryIndexMapper;



/// Generated message map functions

/// Note:   We can not operate message map by CMFCRibbonCateory class (derived from CObject), so I put the ribbon buttons message map here,
///         the MainFrm do the message map, and distribute the messages to the corresponding main wnd class, and some special message only
///         affect special dock pane, like "copy" in layout designer edit ribbon panel, only affect the design operation, should do message
///         map in the design pane.
public:

    afx_msg void OnOptions();
    afx_msg void OnReconnect();

    void EnableCurPage(BOOL bEnable = TRUE);
    BOOL UpdateOptions(CControllerSettings::SETTING_PAGE nPage);
    BOOL RefreshPages();
    int  CreateNewPage(CATEGORY_TYPE  CategoryType, INT UserData);
    int  FindPage(CATEGORY_TYPE  CategoryType, INT UserData);
    int  FindPage(CMFCRibbonCategory *pCategory);
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnFileClose();
    afx_msg void OnClose();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg LRESULT OnChangeRibbonCategory(WPARAM,LPARAM);
    afx_msg LRESULT OnChangeState(WPARAM,LPARAM);
	afx_msg LRESULT OnAlarmInfo(WPARAM,LPARAM);
    afx_msg LRESULT OnPowerBroadcast(WPARAM wp, LPARAM);
    afx_msg void OnSysColorChange();
    afx_msg LRESULT OnCloseCurrentPage(WPARAM,LPARAM);

    afx_msg void OnUpdateRibbonBtnPreviewLayout(CCmdUI* pCmdUI);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);

    afx_msg LRESULT OnOpenLayout(WPARAM,LPARAM);
    afx_msg LRESULT OnNewLayout(WPARAM,LPARAM);
    afx_msg LRESULT OnImportLayout(WPARAM,LPARAM);
	afx_msg LRESULT OnThumbnailLibCallBack(WPARAM,LPARAM);
    afx_msg LRESULT OnControllerOffline(WPARAM,LPARAM);

    DECLARE_MESSAGE_MAP()

    virtual void AdjustClientArea();
	BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
    BOOL CreateRibbonBar();

    void Add_Ribbon_MainPanel();

    void Add_Ribbon_QAT();

    BOOL Add_Category_MediaLibrary();
    BOOL Add_Category_LayoutManage();
    BOOL Add_Category_Designer();
	BOOL Add_Category_TimeTable();
    BOOL Add_Category_Scheduler();
    BOOL Add_Category_ClientManage();
    BOOL Add_Category_UserManage();
    BOOL Add_Category_Log();
	BOOL Add_Category_Alarm();
    BOOL Add_Category_Report();

    BOOL LoadPage(CBaseControllerPage* pPage, CRect rect, UINT nID, CMFCRibbonCategory *pCategory);
    BOOL LoadPageByCategory(CATEGORY_TYPE Category, CRect rect, UINT nPageIndex);

public:
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
};


