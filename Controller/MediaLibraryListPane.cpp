// F:\p4\sw\Demo\ContentManagement\Controller\MediaLibrary Monitor\MediaLibraryListPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "MediaLibraryPage.h"
#include "MediaLibraryListPane.h"
#include "Localization/Localization.h"
#ifdef _MEDIALIBRARY2
#include "MediaLibrary 2/MediaLibrary.h"
#else
#include "MediaLibrary/MediaLibrary.h"
#endif
#include "Sortfuncs.h"
#include "MagicSortListCtrl.h"

#include "Utilities/Utils.h"

UINT MEDIA_UPLOAD_LIST              = ::RegisterWindowMessage (_T("MEDIA_UPLOAD_LIST"));
UINT MEDIA_DOWNLOAD_LIST            = ::RegisterWindowMessage (_T("MEDIA_DOWNLOAD_LIST"));
UINT MEDIA_SHOW_INFO                = ::RegisterWindowMessage (_T("MEDIA_SHOW_INFO"));
UINT MEDIA_CLEAR_INFO               = ::RegisterWindowMessage (_T("MEDIA_CLEAR_INFO"));
//UINT MEDIA_LIST_UPDATE				= ::RegisterWindowMessage (_T("MEDIA_LIST_UPDATE"));
// CMediaLibraryListPane

IMPLEMENT_DYNAMIC(CToolBarEx, CToolBar)

BEGIN_MESSAGE_MAP(CToolBarEx, CToolBar)
	ON_WM_CREATE()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()



CToolBarEx::CToolBarEx()
{
    sizeImage.cx = 32;
    sizeImage.cy = 32;
    img.Create(sizeImage.cx, sizeImage.cy, ILC_COLOR8|ILC_MASK, 1,1);
}

CToolBarEx::~CToolBarEx()
{

}

void CToolBarEx::SetDisableImageList(UINT nBitmapID)
{
	 CBitmap	bitmap;
	 static     CImageList disaImageList;
    
	 disaImageList.DeleteImageList();
    // Create and set the normal toolbar image list.
    bitmap.LoadBitmap(nBitmapID);
    //img.Create(nBitmapID, sizeImage.cx, 1, ILC_MASK);
    disaImageList.Create(sizeImage.cx, sizeImage.cy, ILC_COLOR8|ILC_MASK, 1,1);
    disaImageList.Add(&bitmap, RGB(0,255,0));
    bitmap.Detach();

	this->GetToolBarCtrl().SetDisabledImageList(&disaImageList);
}

void CToolBarEx::SetImageList(UINT nBitmapID)
{
    CBitmap		bitmap;

    img.DeleteImageList();

    // Create and set the normal toolbar image list.
    bitmap.LoadBitmap(nBitmapID);
    //img.Create(nBitmapID, sizeImage.cx, 1, ILC_MASK);
    img.Create(sizeImage.cx, sizeImage.cy, ILC_COLOR8|ILC_MASK, 1,1);
    img.Add(&bitmap, RGB(0,0,0));
    bitmap.Detach();

    SetImage();
}


void CToolBarEx::SetImage()
{
    this->GetToolBarCtrl().SetImageList (&img);
}
void CToolBarEx::SetButtonNumber(int n)
{
    SetButtons(NULL,n);
}
void CToolBarEx::SetButton(int nButton, int nID, int nImage,LPTSTR lpText)
{
    SetButtonInfo(nButton, nID, TBSTYLE_BUTTON , nImage);
    SetButtonText(nButton, lpText);
}

void CToolBarEx::SetCheckBox(int nButton,int nID, int nImage,LPTSTR lpText)
{
    SetButtonInfo(nButton, nID, TBSTYLE_CHECK, nImage);
    SetButtonText(nButton, lpText);
}

void CToolBarEx::SetButton(int nButton, int nID, int nImage)
{
    SetButtonInfo(nButton, nID, TBSTYLE_BUTTON, nImage);
}
void CToolBarEx::SetSize(int nWidth,int nHeight)
{
    CSize cs;
    cs.cx =nWidth;
    cs.cy =nHeight;
    SetSizes(cs, sizeImage);
}
void CToolBarEx::AddIcon(HICON hIcon)
{
    img.Add (hIcon);
    SetImage();
}
void CToolBarEx::AddSeparator(int nButton)
{
    SetButtonInfo(nButton, 0, TBBS_SEPARATOR, 0);
}
void CToolBarEx::SetDefaultSize()
{
    CRect rectToolBar;
    GetItemRect(0, &rectToolBar);
    SetSizes(rectToolBar.Size (), sizeImage);
}
void CToolBarEx::AddBitmap(CBitmap *pbmImage, CBitmap *pbmMask)
{
    img.Add( pbmImage, pbmMask);
    SetImage();
}
void CToolBarEx::AddBitmap(CBitmap *pbmImage, COLORREF crMask)
{
    img.Add( pbmImage, crMask);
    SetImage();
}
void CToolBarEx::SetImageSize(int nImageWidth,int nImageHeight)
{
    sizeImage.cx = nImageWidth;
    sizeImage.cy =nImageHeight;
}

BOOL CToolBarEx::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	 
	m_dwStyle &= ~CBRS_GRIPPER;
	m_dwStyle &= ~CBRS_HIDE_INPLACE;
	return  __super::PreCreateWindow(cs);  //CControlBar::PreCreateWindow(cs);
	
}

int nCurItem = -1;
BOOL CToolBarEx::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
	{
		CPoint point ;
		::GetCursorPos(&point);
		this->ScreenToClient(&point);

		CToolBarCtrl &toolBar = this->GetToolBarCtrl();
		int nBtnIndex = toolBar.HitTest(&point);

		if((nBtnIndex & 0x80000000) != 0x80000000)
		{
			if(nBtnIndex != nCurItem )
			{
				m_ToolTip.DelTool(this);

				CString strToolTip;
				strToolTip.LoadString(this->GetItemID(nBtnIndex));
				strToolTip = Translate(strToolTip);

				m_ToolTip.AddTool(this,strToolTip);
			}
		}else
			pMsg->hwnd = ::AfxGetMainWnd()->m_hWnd;

		nCurItem = nBtnIndex ;
		this->m_ToolTip.RelayEvent(pMsg);
	}
	return CToolBar::PreTranslateMessage(pMsg);
}

int CToolBarEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_ToolTip.Create(::AfxGetMainWnd());
	return 0;
}

LRESULT CToolBarEx::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	//TRACE(_T("Btn Index:%d \n"),nBtnIndex);
	return CToolBar::OnNcHitTest(point);
}


IMPLEMENT_DYNAMIC(CMediaLibraryListPane, CBaseControllerPane)

CMediaLibraryListPane::CMediaLibraryListPane()
    : m_nTimer (0)
{
}

CMediaLibraryListPane::~CMediaLibraryListPane()
{

}


BEGIN_MESSAGE_MAP(CMediaLibraryListPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_REGISTERED_MESSAGE(AFX_WM_CHANGE_ACTIVE_TAB, &CMediaLibraryListPane::OnChangeActiveTab)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF,  &CMediaLibraryListPane::OnToolTipText)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF,  &CMediaLibraryListPane::OnToolTipText)
    ON_WM_TIMER()
    ON_WM_SIZE()
END_MESSAGE_MAP()



// CMediaLibraryListPane message handlers
int CMediaLibraryListPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    RECT rect;
    GetClientRect(&rect);

    m_wndStateTab.Create(CMFCTabCtrl::STYLE_3D_VS2005, rect, this, IDW_LIST_MEDIA_TAB);
    m_wndStateTab.SetLocation(CMFCBaseTabCtrl::LOCATION_TOP);
    m_wndStateTab.EnableActiveTabCloseButton(FALSE);
    m_wndStateTab.AutoDestroyWindow(TRUE);

    SetMainWnd(&m_wndStateTab);

    if (m_nTimer == 0)
    {
        GetConfig->Get(CLIENT_MONITOR_REFRESH, m_refreshDuration);

        m_nTimer = SetTimer(0xFF000001, StringUtility::stoi(m_refreshDuration.GetString()) * 1000, 0);
    }

    return 0;
}

BOOL CMediaLibraryListPane::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
    ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

    // if there is a top level routing frame then let it handle the message

    if (GetRoutingFrame() != NULL) return FALSE;

    // to be thorough we will need to handle UNICODE versions of the message also !!

    TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
    TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
    TCHAR szFullText[512];
    CString strTipText;
    UINT nID = pNMHDR->idFrom;

    if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
        pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
    {
        // idFrom is actually the HWND of the tool

        nID = ::GetDlgCtrlID((HWND)nID);
    }

    if (nID != 0) // will be zero on a separator

    {
        AfxLoadString(nID, szFullText);
        strTipText=szFullText;

#ifndef _UNICODE
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
        }
        else
        {
            _mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
        }
#else
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            _wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
        }
        else
        {
            lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
        }
#endif

        *pResult = 0;

        // bring the tooltip window above other popup windows

        ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
            SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);

        return TRUE;
    }

    return FALSE;
}

void CMediaLibraryListPane::InitUploadBar()
{
    //if (m_wndListToolBar.GetSafeHwnd())
    //{
    //    m_wndListToolBar.DestroyWindow();
    //}
	if(!IsWindow(m_wndListToolBar.m_hWnd))
    if (!m_wndListToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
        | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
    {
        TRACE0("Failed to create toolbar\n");
    }
	int nCount = m_wndListToolBar.GetCount();
	while(--nCount >= 0)
	{
		CToolBarCtrl &toolBarCtrl = m_wndListToolBar.GetToolBarCtrl();
		toolBarCtrl.DeleteButton(0);
	}

	m_wndListToolBar.SetButtonNumber(4);
    m_wndListToolBar.SetImageList(IDB_MEDIALIB_LARGE);
	m_wndListToolBar.SetDisableImageList(IDB_MEDIALIB_LARGE_DISABLE);
    m_wndListToolBar.AddSeparator (0);
    m_wndListToolBar.SetButton(1,ID_BTN_MEDIA_STARTUPLOAD,10);  
    m_wndListToolBar.SetButton(2,ID_BTN_MEDIA_PAUSEUPLOAD,11);
    m_wndListToolBar.SetButton(3,ID_BTN_MEDIA_CANCELUPLOAD,8);

    m_wndListToolBar.SetSize();

    //repostion     the   toolbar
    RecalToolBarPos();
}

void CMediaLibraryListPane::InitDownloadBar()
{
	if(!IsWindow(m_wndListToolBar.m_hWnd))
    if (!m_wndListToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
        | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
    {
        TRACE0("Failed to create toolbar\n");
    }
	int nCount = m_wndListToolBar.GetCount();
	while(--nCount >= 0)
	{
		CToolBarCtrl &toolBarCtrl = m_wndListToolBar.GetToolBarCtrl();
		toolBarCtrl.DeleteButton(0);
	}

	m_wndListToolBar.SetButtonNumber(4);
    m_wndListToolBar.SetImageList(IDB_MEDIALIB_LARGE);
	m_wndListToolBar.SetDisableImageList(IDB_MEDIALIB_LARGE_DISABLE);

    m_wndListToolBar.AddSeparator (0);
    m_wndListToolBar.SetButton(1,ID_BTN_MEDIA_STARTDOWNLOAD,10);
    m_wndListToolBar.SetButton(2,ID_BTN_MEDIA_PAUSEDOWNLOAD,11);
    m_wndListToolBar.SetButton(3,ID_BTN_MEDIA_CANCELDOWNLOAD,8);

    m_wndListToolBar.SetSize();

    m_wndListToolBar.SetOwner(this);

    //repostion     the   toolbar
    RecalToolBarPos();
}

void CMediaLibraryListPane::InitSearchResultBar()
{
	if(!IsWindow(m_wndListToolBar.m_hWnd))
    if (!m_wndListToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
        | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
    {
        TRACE0("Failed to create toolbar\n");
    }
	int nCount = m_wndListToolBar.GetCount();
	while(--nCount >= 0)
	{
		CToolBarCtrl &toolBarCtrl = m_wndListToolBar.GetToolBarCtrl();
		toolBarCtrl.DeleteButton(0);
	}

    m_wndListToolBar.SetButtonNumber(1);
    m_wndListToolBar.SetImageList(IDB_MEDIALIB_LARGE);
	m_wndListToolBar.SetDisableImageList(IDB_MEDIALIB_LARGE_DISABLE);
    m_wndListToolBar.AddSeparator (0);
    m_wndListToolBar.SetSize();

    m_wndListToolBar.SetOwner(this);

    //repostion     the   toolbar
    RecalToolBarPos();
}

void CMediaLibraryListPane::InitCategoryBar()
{
  	if(!IsWindow(m_wndListToolBar.m_hWnd))
    if (!m_wndListToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
        | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
    {
        TRACE0("Failed to create toolbar\n");
    }
	int nCount = m_wndListToolBar.GetCount();
	while(--nCount >= 0)
	{
		CToolBarCtrl &toolBarCtrl = m_wndListToolBar.GetToolBarCtrl();
		toolBarCtrl.DeleteButton(0);
	}
    m_wndListToolBar.SetButtonNumber(1);
    m_wndListToolBar.SetImageList(IDB_MEDIALIB_LARGE);
	m_wndListToolBar.SetDisableImageList(IDB_MEDIALIB_LARGE_DISABLE);
    m_wndListToolBar.AddSeparator (0);
    m_wndListToolBar.SetSize();

    m_wndListToolBar.SetOwner(this);

    //repostion     the   toolbar
    RecalToolBarPos();
}

void CMediaLibraryListPane::RecalToolBarPos()
{

    CRect rcClientStart;
    CRect rcClientNow;
    GetClientRect(rcClientStart);

    // To reposition and resize the control bar

    RepositionBars(AFX_IDW_CONTROLBAR_FIRST,
        AFX_IDW_CONTROLBAR_LAST,0, reposQuery, rcClientNow);

    m_wndStateTab.MoveWindow(rcClientNow);

    // And position the control bars

    RepositionBars(AFX_IDW_CONTROLBAR_FIRST,
        AFX_IDW_CONTROLBAR_LAST, 0);
	 

}

void CMediaLibraryListPane::OnSize(UINT nType, int cx, int cy)
{
   // CBaseControllerPane::OnSize(nType, cx, cy);

    //repostion     the   toolbar
    RecalToolBarPos();

}

void CMediaLibraryListPane::OnDestroy()
{
    m_wndStateTab.RemoveAllTabs();
    m_wndStateTab.DestroyWindow();
    CBaseControllerPane::OnDestroy();
}

void CMediaLibraryListPane::OnTimer(UINT_PTR nIDEvent)
{
    if (IsWindowVisible())
    {
        CBaseControllerPane::OnTimer(nIDEvent);
    }
}

BOOL CMediaLibraryListPane::UpdateOptions()
{
    GetConfig->Get(CLIENT_MONITOR_REFRESH, m_refreshDuration);

    if (m_nTimer)
    {
        KillTimer(m_nTimer);

        m_nTimer = SetTimer(0xFF000001, StringUtility::stoi(m_refreshDuration.GetString()) * 1000, 0);
    }

    return TRUE;
}

LRESULT CMediaLibraryListPane::OnChangeActiveTab(WPARAM wp, LPARAM lp)
{
    // wp is the active tab index
    int index = (int)wp;

    if (index == UPLOAD_LIST_TAB2)     //upload list tab
    {
        InitUploadBar();

        GetParent()->SendMessage(MEDIA_UPLOAD_LIST, 0L,  0L);
    }
#ifndef PROFESSIONAL_EDITION
    else if (index == DOWNLOAD_LIST_TAB3)    //download list tab
    {
        InitDownloadBar();

        GetParent()->SendMessage(MEDIA_DOWNLOAD_LIST, 0L,  0L);
    }
#endif
    else if (index == CATEGORY_LIST_TAB4)
    {
        InitCategoryBar();
		 
    }
    else if (m_wndStateTab.GetActiveTab() == MEDIA_LIST_TAB1)
    {
        InitSearchResultBar();
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMediaLibraryListTab, CMFCTabCtrl)

CMediaLibraryListTab::CMediaLibraryListTab()
{

}

CMediaLibraryListTab::~CMediaLibraryListTab()
{
    auto it = m_wndCharts.begin(), ite = m_wndCharts.end();
    for (; it != ite; ++it)
    {
        it->first->DestroyWindow();
        delete it->first;
    }

    m_wndCharts.clear();
}

BEGIN_MESSAGE_MAP(CMediaLibraryListTab, CMFCTabCtrl)
    ON_WM_CREATE()
    ON_NOTIFY(NM_CLICK, IDW_LIST_MEDIA_STATUS, &CMediaLibraryListTab::OnLvnMediaItemchangedList)
	//ON_NOTIFY(NM_KEYUP, IDW_LIST_MEDIA_STATUS, &CMediaLibraryListTab::OnLvnMediaItemchangedList)
	//ON_NOTIFY(LVN_ITEMCHANGED , IDW_LIST_MEDIA_STATUS, &CMediaLibraryListTab::OnLvnMediaItemchangedList)
    ON_NOTIFY(LVN_BEGINDRAG, IDW_LIST_MEDIA_STATUS, &CMediaLibraryListTab::OnLvnBeginDragMediaList)
    ON_NOTIFY(NM_DBLCLK, IDW_LIST_MEDIA_STATUS, &CMediaLibraryListTab::OnNmDblClick)
	ON_REGISTERED_MESSAGE(BCGM_GRID_BEGINDRAG, &CMediaLibraryListTab::OnBCGBeginDrag)
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


int CMediaLibraryListTab::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMFCTabCtrl::OnCreate(lpCreateStruct) == -1)
        return -1;

    RECT rect;
    GetClientRect(&rect);

	CMagicSortListCtrl* TmpMainList = new CMagicSortListCtrl;
    TmpMainList->Create(WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_SINGLESEL, rect, this, IDW_LIST_MEDIA_STATUS);
    TmpMainList->SetExtendedStyle(TmpMainList->GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    TmpMainList->ShowScrollBar(SB_HORZ);
    // double buffer fix flash when do data update
    TmpMainList->SetExtendedStyle(TmpMainList->GetExtendedStyle()|LVS_EX_DOUBLEBUFFER);

    TmpMainList->InsertColumn(MEDIA_INDEX, Translate(_T("File ID")), LVCFMT_LEFT, 80);
    TmpMainList->InsertColumn(MEDIA_NAME, Translate(_T("Name")), LVCFMT_LEFT, 150);
    TmpMainList->InsertColumn(MEDIA_TYPE, Translate(_T("Type")), LVCFMT_LEFT, 100);
    TmpMainList->InsertColumn(MEDIA_SIZE, Translate(_T("Size")), LVCFMT_LEFT, 100);
   // TmpMainList->InsertColumn(MEDIA_CATEGORY, Translate(_T("Category")), LVCFMT_LEFT, 120);
    TmpMainList->InsertColumn(MEDIA_DIMENSION, Translate(_T("Dimension")), LVCFMT_LEFT, 120);
    TmpMainList->InsertColumn(MEDIA_DURATION, Translate(_T("Duration")), LVCFMT_LEFT, 120);

	TmpMainList->SetCompairFunPair(Translate(_T("File ID")),NumberStrComp);
 #ifdef DB_MYSQL
    TmpMainList->InsertColumn(MEDIA_SYNCSTATUS, Translate(_T("Sync Status")), LVCFMT_LEFT, 140);
    TmpMainList->InsertColumn(MEDIA_AUDIT, Translate(_T("Audit")), LVCFMT_LEFT,100);
#endif
    TmpMainList->InsertColumn(MEDIA_REFERS, Translate(_T("Reference Count")), LVCFMT_LEFT, 110);
	TmpMainList->SetCompairFunPair(Translate(_T("Reference Count")),NumberStrComp);
    TmpMainList->ModifyStyle(LVS_SINGLESEL, 0);


	m_wndCharts.insert(std::map<CWnd*, int>::value_type(TmpMainList, -1));
	m_MainList.push_back(TmpMainList);

	InsertTab(TmpMainList, Translate(_T("Search Result")), MEDIA_LIST_TAB1);

	CMagicSortListCtrl* TmpUploadList  = new CMagicSortListCtrl;
	TmpUploadList->Create(WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_SINGLESEL, rect, this, IDW_LIST_MEDIA_STATUS);
	TmpUploadList->SetExtendedStyle(TmpUploadList->GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	TmpUploadList->ShowScrollBar(SB_HORZ);

	// double buffer fix flash when do data update
	TmpUploadList->SetExtendedStyle(TmpUploadList->GetExtendedStyle()|LVS_EX_DOUBLEBUFFER);

    TmpUploadList->InsertColumn(MEDIA_TASK_INDEX, Translate(_T("File ID")), LVCFMT_LEFT, 50);
	TmpUploadList->SetCompairFunPair(Translate(_T("File ID")),NumberStrComp);
    TmpUploadList->InsertColumn(MEDIA_TASK_STATS, Translate(_T("Status")), LVCFMT_LEFT, 60);
    TmpUploadList->InsertColumn(MEDIA_TASK_NAME, Translate(_T("Name")), LVCFMT_LEFT, 120);
    TmpUploadList->InsertColumn(MEDIA_TASK_TYPE, Translate(_T("Type")), LVCFMT_LEFT, 80);
    TmpUploadList->InsertColumn(MEDIA_TASK_SIZE, Translate(_T("Size")), LVCFMT_LEFT, 120);
    //TmpUploadList->InsertColumn(MEDIA_TASK_GENERATE_INFO_PERCENT, Translate(_T("Generate Info")), LVCFMT_LEFT, 100);
    TmpUploadList->InsertColumn(MEDIA_TASK_PERCENT, Translate(_T("Percent")), LVCFMT_LEFT, 100);
	TmpUploadList->SetCompairFunPair(Translate(_T("Percent")),NumberStrComp);
    //TmpUploadList->InsertColumn(MEDIA_TASK_VERIFY_PERCENT, Translate(_T("Verify")), LVCFMT_LEFT, 100);
   // TmpUploadList->InsertColumn(MEDIA_TASK_CATEGORY, Translate(_T("Category")), LVCFMT_LEFT, 80);

    TmpUploadList->InsertColumn(MEDIA_TASK_DIMENSION, Translate(_T("Dimension")), LVCFMT_LEFT, 80);
    TmpUploadList->InsertColumn(MEDIA_TASK_DURATION, Translate(_T("Duration")), LVCFMT_LEFT, 100);
    TmpUploadList->InsertColumn(MEDIA_TASK_AVALIABLE, Translate(_T("Available")), LVCFMT_LEFT, 80);
	TmpUploadList->SetCompairFunPair(Translate(_T("Available")),BaseStrComp);
    TmpUploadList->ModifyStyle(LVS_SINGLESEL, 0);

	m_wndCharts.insert(std::map<CWnd*, int>::value_type(TmpUploadList, 0));
	m_MainList.push_back(TmpUploadList);

#ifdef PROFESSIONAL_EDITION

    InsertTab(TmpUploadList, Translate(_T("Tasks")), UPLOAD_LIST_TAB2);

#else

    InsertTab(TmpUploadList, Translate(_T("Upload Tasks")), UPLOAD_LIST_TAB2);

	CMagicSortListCtrl* TmpDownloadList  = new CMagicSortListCtrl;
	TmpDownloadList->Create(WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_SINGLESEL, rect, this, IDW_LIST_MEDIA_STATUS);
	TmpDownloadList->SetExtendedStyle(TmpDownloadList->GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	TmpDownloadList->ShowScrollBar(SB_HORZ);

	// double buffer fix flash when do data update
	TmpDownloadList->SetExtendedStyle(TmpDownloadList->GetExtendedStyle()|LVS_EX_DOUBLEBUFFER);


	TmpDownloadList->InsertColumn(MEDIA_TASK_INDEX, Translate(_T("File ID")), LVCFMT_LEFT, 50);
	TmpDownloadList->SetCompairFunPair(Translate(_T("File ID")),NumberStrComp);
    TmpDownloadList->InsertColumn(MEDIA_TASK_STATS, Translate(_T("Status")), LVCFMT_LEFT, 60);
	TmpDownloadList->InsertColumn(MEDIA_TASK_NAME, Translate(_T("Name")), LVCFMT_LEFT, 120);
    TmpDownloadList->InsertColumn(MEDIA_TASK_TYPE, Translate(_T("Type")), LVCFMT_LEFT, 80);
    TmpDownloadList->InsertColumn(MEDIA_TASK_SIZE, Translate(_T("Size")), LVCFMT_LEFT, 120);
    //TmpDownloadList->InsertColumn(MEDIA_TASK_GENERATE_INFO_PERCENT, Translate(_T("Generate Info")), LVCFMT_LEFT, 100);
    TmpDownloadList->InsertColumn(MEDIA_TASK_PERCENT, Translate(_T("Percent")), LVCFMT_LEFT, 100);
	TmpDownloadList->SetCompairFunPair(Translate(_T("Percent")),NumberStrComp);
    //TmpDownloadList->InsertColumn(MEDIA_TASK_VERIFY_PERCENT, Translate(_T("Verify")), LVCFMT_LEFT, 100);
   // TmpDownloadList->InsertColumn(MEDIA_TASK_CATEGORY, Translate(_T("Category")), LVCFMT_LEFT, 80);

    TmpDownloadList->InsertColumn(MEDIA_TASK_DIMENSION, Translate(_T("Dimension")), LVCFMT_LEFT, 80);
    TmpDownloadList->InsertColumn(MEDIA_TASK_DURATION, Translate(_T("Duration")), LVCFMT_LEFT, 100);
    TmpDownloadList->InsertColumn(MEDIA_TASK_AVALIABLE, Translate(_T("Available")), LVCFMT_LEFT, 80);
	TmpDownloadList->SetCompairFunPair(Translate(_T("Available")),BaseStrComp);
    TmpDownloadList->ModifyStyle(LVS_SINGLESEL, 0);

	m_wndCharts.insert(std::map<CWnd*, int>::value_type(TmpDownloadList, 1));
    m_MainList.push_back(TmpDownloadList);
	InsertTab(TmpDownloadList, Translate(_T("Download Tasks")), DOWNLOAD_LIST_TAB3);

#endif

	
	m_mediaGrid.SetBackgroundTask(g_backgroundTask);
	m_mediaGrid.Create(WS_CHILD|WS_VISIBLE|LVS_REPORT, rect, this, IDW_LIST_MEDIA_STATUS);
#define Translate(x) x
	wchar_t* names[] = {
		Translate(_T("File ID")),
		Translate(_T("Name")),
		Translate(_T("Type")),
		Translate(_T("Size")),
		Translate(_T("Dimension")),
		Translate(_T("Duration")),
#ifdef SERVER_EDITION
		Translate(_T("Sync Status")),
		Translate(_T("Audit")),
#endif
		Translate(_T("Reference Count")),

	};
#undef Translate
	static const int kColumnCount = sizeof(names) / sizeof(*names);
	int idx = 10;
	for (int i = 0; i < kColumnCount; ++i)
	{
		if (names[i] == _T("Name"))
		{
			idx = 30;
		}
		else
		{
			idx = 10;
		}
		m_mediaGrid.InsertColumn(i, Translate(names[i]), idx * wcslen(names[i]));
        m_mediaGrid.GetColumnsInfo().SetColumnWidthAutoSize(i, true);
		//m_mediaGrid.GetColumnsInfo().SetColumnWidth(i, wcslen(names[i])*20);
	}
#ifdef SERVER_EDITION
    m_mediaGrid.SetColumnCanSort(MEDIA_SYNCSTATUS,FALSE);
#endif
	m_mediaGrid.SetCachePageSize(50);
	m_mediaGrid.SetVirtualRows(m_mediaGrid.GetVirtualRowCount());
	m_mediaGrid.EnableDragSelection(TRUE);
	m_mediaGrid.EnableDragHeaderItems(TRUE);
	m_mediaGrid.SetWholeRowSel(TRUE);
    InsertTab((CWnd*)&m_mediaGrid, Translate(_T("Media List")), CATEGORY_LIST_TAB4);

    g_backgroundTask->startTimer(boost::bind(&CMediaGrid::handle_timeout, &m_mediaGrid, _1, _2));

    EnableTabSwap(FALSE);

    return 0;
}


CString CMediaLibraryListTab::FormatSizeString(DWORD64 Size)
{
    CString FormatResult;
    if(Size < 1024)
    {
        FormatResult.Format(_T("%lldb"), Size);
    }else if(Size < 1024 * 1024)
    {
        FormatResult.Format(_T("%0.0fKb"), (float)Size/1024.0f);
    }else
    {
        FormatResult.Format(_T("%0.1fMb"), (float)Size/1024.0f/1024.0f);
    }
    return FormatResult;
}

HRESULT CMediaLibraryListTab::GetSelectedMediaIDList(std::vector<INT> &SelectedMediaIDList)
{
    int ActiveTabIndex = GetActiveTab();
	if (ActiveTabIndex == CATEGORY_LIST_TAB4)
	{
		m_mediaGrid.GetSelectedMultiID(SelectedMediaIDList);
	}
	else
	{
		POSITION pos = m_MainList[ActiveTabIndex]->GetFirstSelectedItemPosition();

		while(pos)
		{
			int nItem = m_MainList[ActiveTabIndex]->GetNextSelectedItem(pos);
			CString szMediaID = m_MainList[ActiveTabIndex]->GetItemText(nItem, 0);
			int MediaID = _tstoi(szMediaID);

			SelectedMediaIDList.push_back(MediaID);
		}
	}
    return S_OK;
}


void CMediaLibraryListTab::OnLvnMediaItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	int iCurrent = pNMLV->iItem;
	int tabIndex = GetActiveTab();
	INT selectCount = m_MainList[tabIndex]->GetSelectedCount();
	if (0x01 == selectCount)
	{
			// get selected media id
			CString mediaID = m_MainList[tabIndex]->GetItemText(iCurrent, 0);
			//send selected message id as a message param
			((GetParent()->GetParent()))->SendMessage(MEDIA_SHOW_INFO, 0L, (LPARAM)(_tstoi(mediaID)));
	}
	else
	{
		//send message to media info pane to clear media info desc
		GetParent()->SendMessage(MEDIA_CLEAR_INFO, 0L, 0L);
	}
}

void CMediaLibraryListTab::OnLvnBeginDragMediaList(NMHDR *pNMHDR, LRESULT *pResult)
{
    if (!GetControllerInterface->CheckPrivilege(UserPrivilege_PublishLayout))
    {
        MessageBox(Translate(_T("You don't have privilege to rename/move media")),Translate(_T("Warning:Drop media")) ,MB_OK|MB_ICONEXCLAMATION);
        return;
    }
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    int tabIndex = GetActiveTab();
    if (tabIndex != MEDIA_LIST_TAB1 && tabIndex != CATEGORY_LIST_TAB4)
    {
        return;
    }
    std::map<INT, Media>* pMediaMap = NULL;
    if (tabIndex == MEDIA_LIST_TAB1)
    {
        pMediaMap = &m_SearchMediaMap;
    }
    else if (tabIndex == CATEGORY_LIST_TAB4)
    {
        pMediaMap = &m_MediaMap;
    }

    int iCurrent = pNMLV->iItem;
#if 0
    CString szMediaID = m_MainList[CATEGORY_LIST_TAB4]->GetItemText(iCurrent, 0);
	DWORD MediaID = _tstoi(szMediaID);
#else
	POSITION pos = m_MainList[tabIndex]->GetFirstSelectedItemPosition();

	std::vector<int> selmediaList;
	while(pos)
	{
		int nItem = m_MainList[tabIndex]->GetNextSelectedItem(pos);
		CString szMediaID = m_MainList[tabIndex]->GetItemText(nItem, 0);
		int MediaID = _tstoi(szMediaID);

		auto  it = pMediaMap->find(MediaID);
		if (it == pMediaMap->end())
		{
			return ;
		}

		selmediaList.push_back(MediaID);
	}
#endif


    BOOL bDrag = FALSE;
    try
    {
        CSharedFile glogFile;

        bDrag = SerializeTo(glogFile, selmediaList, pMediaMap);

        if (bDrag)
        {
            m_OleDataSource.CacheGlobalData(CMediaLibraryCategoryCtrl::s_MediaFormat,
                glogFile.Detach());
        }

    }
    catch (COleException* pEx)
    {
        TRACE(_T(" DragDrop. OLE exception: %x\r\n"),
            pEx->m_sc);
        pEx->Delete ();

        bDrag = FALSE;
    }
    catch (CNotSupportedException *pEx)
    {
        TRACE(_T(" DragDrop. \"Not Supported\" exception\r\n"));
        pEx->Delete ();

        bDrag = FALSE;
    }

    if (!bDrag)
    {
        return;
    }

    DROPEFFECT dropEffect
        = m_OleDataSource.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE);

    m_OleDataSource.Empty();
}

void CMediaLibraryListTab::OnNmDblClick(NMHDR *pNMHDR, LRESULT *pResult)
{
    return;
#if 0
    LPNMITEMACTIVATE pNMLV = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    *pResult = 0;
    int iCurrent = pNMLV->iItem;
    if (iCurrent == -1) return;

//     TCHAR pContent[42] ;

    CMediaLibraryPage* MediaLibraryPage = (CMediaLibraryPage*)(GetParent()->GetParent());

    //m_wndMainList->GetItemText(iCurrent, 0, pContent, 255);
    //int MediaLibraryId = _tstoi(pContent);
    int MediaLibraryId = m_wndMainList->GetItemData(iCurrent);
    AddChartPane(MediaLibraryId);
#endif
}

void CMediaLibraryListTab::FireChangeActiveTab(int nNewTab)
{
    CMFCTabCtrl::FireChangeActiveTab(nNewTab);

    CWnd* pWnd = GetActiveWnd();

    BOOL bDisable = FALSE;
    for (auto it = m_MainList.begin(); it != m_MainList.end(); it++)
    {
        if (*it == pWnd)
        {
            EnableActiveTabCloseButton(FALSE);
            bDisable = TRUE;
            break;
        }
    }
	if (pWnd == &m_mediaGrid)
	{
		EnableActiveTabCloseButton(FALSE);
        m_mediaGrid.RefreshMediaGrid();
		bDisable = TRUE;
	}

	if (!bDisable)
	{
		EnableActiveTabCloseButton(TRUE);
	}
}

void CMediaLibraryListTab::AddChartPane(int nMediaLibraryId)
{
    if (m_setMediaLibraryChartTabIds.find(nMediaLibraryId) != m_setMediaLibraryChartTabIds.end())
    {
        CString label;
        //this->SetActiveTab();
        for (int i = 0; i < GetTabsNum(); ++i)
        {
            CWnd* wnd = GetTabWnd(i);
            auto it = m_wndCharts.find(wnd), ite = m_wndCharts.end();
            if (it != ite && it->second == nMediaLibraryId)
            {
                SetActiveTab(i);
            }
        }
    }
    else
    {
        RECT rect;
        GetClientRect(&rect);

        //CMediaLibraryChartPane* wndChart = new CMediaLibraryChartPane;
        //wndChart->Create(_T("Chart"), this, rect, TRUE, 999/*ID_VIEW_CLIENTListLIST*/, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_HIDE_INPLACE | WS_CAPTION,
        //    AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE);

        //m_wndCharts.insert(std::map<CWnd*, int>::value_type(wndChart, nMediaLibraryId));
        //m_setMediaLibraryChartTabIds.insert(nMediaLibraryId);
        //MediaLibraryInfo MediaLibraryInfo;
        //HRESULT hr = GetControllerInterface->GetMediaLibrary(nMediaLibraryId, MediaLibraryInfo);
        //if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        //{
        //    GetControllerApp->OnControllerServerOffline();
        //    return;
        //}
        //InsertTab(wndChart, MediaLibraryInfo.GetMediaLibraryDesc().c_str(), GetTabsNum());
        //SetActiveTab(GetTabsNum()-1);
    }
}

LRESULT CMediaLibraryListTab::OnBCGBeginDrag(WPARAM wp, LPARAM lp)
{
	if (!GetControllerInterface->CheckPrivilege(UserPrivilege_PublishLayout))
	{
		MessageBox(Translate(_T("You don't have privilege to rename/move media")),Translate(_T("Warning:Drop media")) ,MB_OK|MB_ICONEXCLAMATION);
		return S_OK;
	}
	vector<int> mediaIDList;
	m_mediaGrid.GetSelectedMultiID(mediaIDList);
	if (mediaIDList.size() > 0)
	{
		BOOL bDrag = FALSE;
		try
		{
			CSharedFile glogFile;
			std::map<INT, Media>* pMediaMap = NULL;
			pMediaMap = GetMediaMap();
			pMediaMap->clear();
			vector<Media>	medias;
			
			HRESULT hr = GetControllerInterface->GetMedias(mediaIDList, medias);
			if (hr != S_OK)
			{
				return E_FAIL;
			}
			auto itM = medias.begin(), itMEnd = medias.end();
			for (;itM != itMEnd;++itM)
			{
				(*pMediaMap)[itM->GetMeidaID()] = *itM;
			}
			
			bDrag = SerializeTo(glogFile, mediaIDList, pMediaMap);
			if (bDrag)
			{
				m_OleDataSource.CacheGlobalData(CMediaLibraryCategoryCtrl::s_MediaFormat,glogFile.Detach());
			}
		}
		catch(COleException* pEx)
		{
			TRACE(_T(" DragDrop. OLE exception: %x\r\n"),
				pEx->m_sc);
			pEx->Delete ();

			bDrag = FALSE;
		}
		catch (CNotSupportedException *pEx)
		{
			TRACE(_T(" DragDrop. \"Not Supported\" exception\r\n"));
			pEx->Delete ();

			bDrag = FALSE;
		}

		if (!bDrag)
		{
			return E_FAIL;
		}

		DROPEFFECT dropEffect
			= m_OleDataSource.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE);

		m_OleDataSource.Empty();
	}
	return S_OK;
}

BOOL CMediaLibraryListTab::RemoveTab(int iTab, BOOL bRecalcLayout /*= TRUE*/)
{
/*
    CString label;
    GetTabLabel(iTab, label);

    int MediaLibraryId = _tstoi(label.GetString());*/


    CWnd* wnd = GetTabWnd(iTab);
    CMFCTabCtrl::RemoveTab(iTab, bRecalcLayout);

    delete wnd;
    auto it = m_wndCharts.find(wnd), ite = m_wndCharts.end();
    if (it != ite)
    {
        m_setMediaLibraryChartTabIds.erase(it->second);
        m_wndCharts.erase(wnd);
    }

    return TRUE;
}



void CMediaLibraryListTab::OnLButtonUp(UINT nFlags, CPoint point)
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

BOOL CMediaLibraryListTab::SerializeTo(CFile& file, const std::vector<INT>& MediaIDList, const std::map<INT, Media>* mediaMap)
{
#if 1
	CString XMLString;

	for(auto iter=MediaIDList.begin();iter<MediaIDList.end();++iter)
	{
		auto it = mediaMap->find(*iter);
		if (it == mediaMap->end())
		{
			return FALSE;
		}
		XMLString += it->second.ToXML().c_str();
	}
	CArchive Archive (&file, CArchive::store);

    Archive << XMLString;

    return TRUE;
#else
	std::tstring XMLString = _T("");
	std::tstring IDList= _T("");;

	for(auto iter=MediaIDList.begin();iter<MediaIDList.end();++iter)
	{
		IDList += AddXMLTag(_T("MediaID"),StringUtility::itos(*iter));
	}
    XMLString = AddXMLTag(_T("MediaIDList"), IDList);

	CArchive Archive (&file, CArchive::store);

	CString tmp(XMLString.c_str());
    Archive << tmp;

    return TRUE;
#endif

}

BOOL CMediaLibraryListTab::PreTranslateMessage(MSG* pMsg)
{
	int tabIndex;
	int selectCount;

	BOOL rtn = CMFCTabCtrl::PreTranslateMessage(pMsg);
	//return rtn;

	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
			case VK_DOWN:
			case VK_UP:
				tabIndex = GetActiveTab();
				if (tabIndex == CATEGORY_LIST_TAB4)
				{
					return FALSE;
				}
				selectCount = m_MainList[tabIndex]->GetSelectedCount();
				if (1 == selectCount)
				{
					// get selected media id
					POSITION ps = m_MainList[tabIndex]->GetFirstSelectedItemPosition();
					int itemcount = m_MainList[tabIndex]->GetItemCount();
					int iCurrent = m_MainList[tabIndex]->GetNextSelectedItem(ps);

					if(pMsg->wParam == VK_DOWN)
					{
						if(iCurrent != itemcount - 1)
						{
							iCurrent++;
						}
						else
						{
							return rtn;//iCurrent = 0;
						}
					}
					else //if(pMsg->wParam == VK_UP)
					{
						if(iCurrent != 0x00)
						{
							iCurrent--;
						}
						else
						{
							return rtn;//iCurrent = itemcount - 1;
						}
					}

					CString mediaID = m_MainList[tabIndex]->GetItemText(iCurrent, 0);

					//send selected message id as a message param
					((GetParent()->GetParent()))->SendMessage(MEDIA_SHOW_INFO, 0L, (LPARAM)(_tstoi(mediaID)));

				}
				else
				{
					//send message to media info pane to clear media info desc
					GetParent()->SendMessage(MEDIA_CLEAR_INFO, 0L, 0L);
				}
				return rtn;
				//break;
			default:
				break;
		}
	}
	return rtn;
}

///////////////////////////////////////////////////////////////////
static wchar_t* colnames[] = {
	(_T("MediaId")),
	(_T("Name")),
	(_T("MediaType")),
	(_T("FileSize")),
	(_T("Dimension")),
	(_T("Duration")),
#ifdef SERVER_EDITION
	(_T("SyncStatus")), 
	(_T("Audit")),
#endif
	(_T("RefersCnt")),
};


sort_and_filter_t CMediaGrid::BuildSort()
{
	sort_and_filter_t sf;
	int n = m_Columns.GetColumnCount();

	std::tstring field;
	for (int i = 0; i < n; ++i)
    {
		std::tstring colName = colnames[i];
		int s = m_Columns.GetColumnState(i);

		field = colName;
		
		if (colName == _T("Dimension") && s == -1)
		{
			field = _T("width desc, height");
		}
		else if (colName == _T("Dimension"))
		{
			field = _T("Width, Height");
		}
#ifdef SERVER_EDITION
		if (colName == _T("Audit"))
		{
			field = _T("Status");
		}
#endif

        if (field != _T(""))
        {
			if (s == 1)
			{
				sf.AddSort(field, 1);
			}
			else if (s == -1)
			{
				sf.AddSort(field, -1);
			}
			else
			{
				sf.AddSort(field, 0);
			}
        }        
    }
	sf.SetFilter(GetFilter());
    return sf;
}

void CMediaGrid::RefreshMediaGrid()
{
	CWaitCursor cursor;
	SMediaFilter filter;
	filter.setCategoryId(m_mediaFilter.categoryId);
	filter.setCount(GetVirtualRowCount());
	filter.setMaxId(GetMaxRowId());
	SetFilter(filter);
}

int CMediaGrid::GetVirtualRowCount()
{
	std::tstring sql = _T("select count(*) as c from Media Where status <> @AUDIT_DELETED@ ");
	sql += GetFilter().ToString();


	ADODB::_RecordsetPtr rs;
	if(S_OK != GetControllerInterface->QuerySQL(sql, rs))
		return 0;

	rs->MoveFirst();
	return rs->Fields->Item["c"]->Value;

}

int CMediaGrid::GetSelectRowMediaID()
{
	m_pSelRow = GetCurSel();
	if (!m_pSelRow)
	{
		return -1;
	}
	CBCGPGridItem* pName = m_pSelRow->GetItem(0);
	_variant_t id;
	if (pName)
	{
		id = pName->GetValue();
	}
	if (id.vt == VT_BSTR)
	{
		return -1;
	}
	else
		return id;
}

void CMediaGrid::OnSelChanged(const CBCGPGridRange &range, BOOL bSelect)
{

	CBCGPGridCtrl::OnSelChanged(range, bSelect);
	CWnd * p = AfxGetMainWnd();
	if (p != NULL)
	{
		CWnd *pWnd = SearchWndObj(p,RUNTIME_CLASS(CMediaLibraryPage));
		CMediaLibraryPage* pPage= (CMediaLibraryPage*)pWnd;
		int id = GetSelectRowMediaID();
		if (id != -1)
		{
			pPage->SendMessage(MEDIA_SHOW_INFO, 0L, (LPARAM)(id));
		}	
	}
}

int CMediaGrid::JudgeReferCount()
{
	if (!m_lstSel.IsEmpty())
	{
		POSITION pos = m_lstSel.GetHeadPosition ();
		if (!pos)
		{
			return 1;
		}
		CBCGPGridRange* pSelRange = m_lstSel.GetNext (pos);
		ASSERT (pSelRange != NULL);
		CBCGPGridRow* pTopRow = (pSelRange->m_nTop != -1) ? GetRow (pSelRange->m_nTop) : NULL;
#ifdef SERVER_EDITION
		int idex = 8;
#else
		int idex = 6;
#endif
		if (m_pSelRow)
		{
			CBCGPGridItem *pIt = m_pSelRow->GetItem(idex);
			_variant_t v = pIt->GetValue();
			return v;
		}	
	}
	return 1;
}

void CMediaGrid::OnTimerRefresh()
{
    try
    {
        unsigned long maxId = GetMaxRowId();
        int count = GetVirtualRowCount();
        Refresh_TH(count, maxId);
    }
    catch (NetworkException& e)
    {
    }
/*
	SMediaFilter f = GetFilter();
	if (maxId != f.maxId || count != f.count)
	{
		//RefreshMediaGrid();
		RefreshAll_TH(count, maxId);
	}
	else
	{
		//ForceUpdateOneRecord(m_idActive);
		RefreshPartial_TH();
	}
*/
}

unsigned long CMediaGrid::GetMaxRowId()
{
	std::tstring sql = _T("select count(*) as c, max(mediaid) as maxId from Media Where status <> @AUDIT_DELETED@ ");
	sql += GetFilter().ToString();

	ADODB::_RecordsetPtr rs;
	if(S_OK != GetControllerInterface->QuerySQL(sql, rs))
		return 0;

	rs->MoveFirst();
	unsigned long ct = rs->Fields->Item["c"]->Value;
	if (ct == 0)
	{
		return 0;
	}
	else
		return rs->Fields->Item["maxId"]->Value;
}


TCHAR* SyncStatus[] = {
	_T("SYNCING"),
	_T("SYN_LOCAL_FILE_COLLECTING"),
	_T("NOT_AVAILABLE_IN_LOCAL"),
	_T("SYNC_WITH_SERVER"),
	_T("OUT_OF_DATE")
};

TCHAR* AuditStatus[] = {
	_T("REQUESTED"),
	_T("REJECTED"),
	_T("APPROVED"),
	_T("RETIRED"),
	_T("DELETED"),
    _T("NONE")
};


void CMediaGrid::GetData(int start, int end, const sort_and_filter_t& sf, std::map<int, std::vector<_variant_t> >& data)
{
	CWaitCursor cursor;
	std::map<int, std::vector<_variant_t> > temp;
	CString limit, range, desRange;
	std::tstring sql, clause;


    int idxSyncStatus = 6;
    int idxAuditStatus = 7;
    int idxSize = 3;
    int idxDur = 5;
	
	/*
	sql = _T("select m.*, f.MediaFilterId as fMediaFilterId, f.Name as fName, f.ParentFilterId as fParentFilterId,") 
		_T("f.RequestUserId as fRequestUserId, f.RequestTime as fRequestTime, f.ApproveUserId as fApproveUserId, f.ApproveTime as fApproveTime, f.Status as fStatus") 
		_T("from Media m, MediaFilter f where m.Status <> 4 and m.MediaFilterId = f.MediaFilterId ");
	*/

//	sql = _T("select m.MediaId, m.Name, m.MediaType, m.FileSize, m.duration from Media m, MediaFilter f where m.Status <> @AUDIT_DELETED@ and m.MediaFilterId = f.MediaFilterId ");
#if defined(DB_MYSQL)
    sql = _T("select m.MediaId, m.Name, m.MediaType, m.FileSize, s3concat{cast(m.width as char),'*', cast(m.height as char)} as Dimension, m.Duration,m.uploadtime,  m.Status ,(select count(distinct lm.layoutId) from LayoutMedia lm, Layout l where lm.LayoutId = l.LayoutId and l.Status <> @AUDIT_DELETED@ and lm.mediaid = m.mediaid) as RefersCnt from Media m where status <> @AUDIT_DELETED@ "); //CONCAT(CONV(m.width, 10, 10),'*', CONV(m.height, 10, 10)) //CONCAT(cast(m.width as char),'*', cast(m.height as char))
#elif defined(DB_SQLITE)
    sql = _T("select m.MediaId, m.Name, m.MediaType, m.FileSize, s3concat{cast(m.width as char), '*', cast(m.height as char)} as Dimension, m.Duration,(select count(distinct lm.layoutId) from LayoutMedia lm, Layout l where lm.LayoutId = l.LayoutId and l.Status <> @AUDIT_DELETED@ and lm.mediaid = m.mediaid) as RefersCnt from Media m where status <> @AUDIT_DELETED@ ");
#else
	sql = _T("select m.MediaId, m.Name, m.MediaType, m.FileSize,CONCAT(CONV(m.width, 10, 10),'*', CONV(m.height, 10, 10)) as Dimension, m.Duration,m.uploadtime,  m.Status ,(select count(distinct lm.layoutId) from LayoutMedia lm, Layout l where lm.LayoutId = l.LayoutId and l.Status <> @AUDIT_DELETED@ and lm.mediaid = m.mediaid) as RefersCnt from Media m where status <> @AUDIT_DELETED@ ");
	sql = _T("select top");
	range.Format(_T(" %d * from ( select * from ( select top"),end-start);
	sql += range;
	int total = boost::any_cast<SMediaFilter>(sf.GetFilter()).count;
	if (total == start)
	{
		return;
	}
	desRange.Format(_T(" %d * from ( select * from ( select * from ( select m.MediaId, m.Name, m.MediaType, m.FileSize, CStr(m.width)+'*'+CStr(m.height) as Dimension,")
		_T("m.Duration, iif(isnull(refcnt1), 0, refcnt1) as RefersCnt from media m left outer join (select mediaid, count(*) as refcnt1 from ")
		_T("(select * from layoutmedia lm inner join layout l on lm.layoutid = l.layoutid) group by mediaid) as cnt on m.mediaid = cnt.mediaid where m.status <> @AUDIT_DELETED@"),total-start);  

	sql += desRange;

#endif;

	SMediaFilter filter;
	filter = boost::any_cast<SMediaFilter>(sf.GetFilter());
	sql += filter.ToString();

#ifdef DB_ACCESS
	sql += _T(")");
#endif

	clause = _T(" order by ");


	// sort clause
	sort_and_filter_t::sorts_t sorts;
	sorts = sf.GetSorts();
	int n = sorts.size();
	int i = 0;
	bool bFirst = true;
	bool bSort = false;

#ifndef DB_ACCESS
	for (i = 0; i < n; ++i)
	{
		int s = std::get<1>(sorts[i]);
		std::wstring field = std::get<0>(sorts[i]);
		if (!bFirst && s != 0)
		{
			clause += _T(", ");
		}


		if (s == 1)
		{
			clause += field;
			clause += _T(" ASC");
			bSort = true;
			bFirst = false;
		}
		else if (s == -1)
		{
			clause += field;
			clause += _T(" DESC");
			bSort = true;
			bFirst = false;
		}
		else
		{
			continue;
		}

	}
	if (bSort > 0)
	{
		sql += clause;
	}


	limit.Format(_T(" limit %d, %d"), start, end - start);
	sql += limit.GetString();
#else
//	limit.Format(_T(" between %d and %d"), start, end - 1);

//	int Row_Per_Page = GetCachePageSize();

//	Select TOP Row_Per_Page MediaId From [ Select TOP (TotRows - ((Page_Number - 1) * Row_Per_Page)  From Media Order By ColumnName DESC] Order By ColumnName ASC 
	for (i = 0; i < n; ++i)
	{
		int s = std::get<1>(sorts[i]);
		std::wstring field = std::get<0>(sorts[i]);
		if (!bFirst && s != 0)
		{
			clause += _T(", ");
		}	
		if (s == 1)
		{
			clause += field;
			clause += _T(" DESC)))");
			bSort = true;
			bFirst = false;
			sql += clause;
			sql +=_T(" order by ");
			sql += field;
			sql += _T(" ASC)");
		}
		else if (s == -1)
		{
			clause += field;
			clause += _T(" ASC)))");
			bSort = true;
			bFirst = false;
			sql += clause;
			sql +=_T(" order by ");
			sql += field;
			sql += _T(" DESC)");
		}
		else
		{
			continue;
		}

	}
	
	if (bSort <= 0)
	{
		sql += _T("))))");
	}
	

#endif


	ADODB::_RecordsetPtr rs;
	if (S_OK != GetControllerInterface->QuerySQL(sql, rs))
	{
		return;
	}

	n = start;
#ifdef DB_MYSQL	
	std::map<int, S3Time> taskMedia;
	while (!rs->EndOfFile)
	{
		_variant_t v = rs->Fields->Item["uploadtime"]->Value;
		S3Time time = S3Time::CreateTime(v);
		taskMedia.insert(map<int, S3Time>::value_type(rs->Fields->Item["MediaId"]->Value,time));
		rs->MoveNext();
	}
	std::map<int, SYNC_STATUS> mediaSync;
	HRESULT hr = GetMediaLibraryInterface->GetMediaSyncStatus(taskMedia, mediaSync);
	if (hr != S_OK)
	{
		return;
	}
	rs->MoveFirst();
	while (!rs->EndOfFile)
	{
		std::vector<_variant_t> row;
		for (short i = 0; i < rs->Fields->Count; ++i)
		{
            CString szStr;
			if (i == idxSyncStatus) //upload time
			{
				map<int, SYNC_STATUS>::iterator t = mediaSync.find(long(row[0]));
                szStr = Translate(SyncStatus[t->second]);
				row.push_back((_variant_t)(_bstr_t)szStr.GetString());

			}
			else if(i == idxAuditStatus)
			{
                int idx = long(rs->Fields->Item[i]->Value);
                if (idx > 5)
                {
                    idx = 5;
                }
                szStr = Translate(AuditStatus[idx]);
				row.push_back((_variant_t)(_bstr_t)szStr.GetString());
			}
            else if (i == idxDur)
            {
                CString szDuration;
                CTimeSpan tmpDur(unsigned __int64(rs->Fields->Item[i]->Value)/1000);

                szDuration = tmpDur.Format(_T("%H:%M:%S"));
                row.push_back((_variant_t)(_bstr_t)szDuration.GetString());
            }
			else if (i == idxSize)
			{
				row.push_back((LPCTSTR)FormatSizeString(unsigned __int64(rs->Fields->Item[i]->Value)));
			}
			else
			{
				row.push_back(rs->Fields->Item[i]->Value);
			}		
		}
		temp[n] = row;
		++n;
		rs->MoveNext();
	}
#else
	while (!rs->EndOfFile)
	{
		std::vector<_variant_t> row;
		for (short i = 0; i < rs->Fields->Count; ++i)
		{
            if (i == idxSize)
            {
                row.push_back((LPCTSTR)FormatSizeString(unsigned __int64(rs->Fields->Item[i]->Value)));
            }
            else if (i == idxDur)
            {
                CString szDuration;
                CTimeSpan tmpDur(unsigned __int64(rs->Fields->Item[i]->Value)/1000);

                szDuration = tmpDur.Format(_T("%H:%M:%S"));
                row.push_back((_variant_t)(_bstr_t)szDuration.GetString());
            }
            else
            {
                row.push_back(rs->Fields->Item[i]->Value);
            }
		}
		temp[n] = row;
		++n;
		rs->MoveNext();
	}
#endif	
	data.swap(temp);
}

CMediaGrid::CMediaGrid()
{
	EnableDragSelection(TRUE);
}

void CMediaGrid::SetFilter(const SMediaFilter& filter)
{
	boost::recursive_mutex::scoped_lock lock(m_filterMutex);
	CWaitCursor cursor;
	m_mediaFilter = filter;
	UpdateAllData();
}

SMediaFilter CMediaGrid::GetFilter()
{
	boost::recursive_mutex::scoped_lock lock(m_filterMutex);
	return m_mediaFilter;
}

void CMediaGrid::handle_timeout(const boost::system::error_code& ec, boost::shared_ptr<boost::asio::deadline_timer> timer)
{
    if (ec)
        return;

	CString szRefreshDuration;
	GetConfig->Get(CLIENT_MONITOR_REFRESH, szRefreshDuration);

	if (IsWindowVisible())
	{
		OnTimerRefresh();
	}
    timer->expires_from_now(boost::posix_time::seconds( StringUtility::stoi(szRefreshDuration.GetString()) ));
    timer->async_wait(boost::bind(&CMediaGrid::handle_timeout, this, boost::asio::placeholders::error, timer));
	//g_backgroundTask->startTimer(boost::bind(&CMediaGrid::handle_timeout, this, boost::asio::placeholders::error, timer));	
}

CMediaGrid::~CMediaGrid()
{

}

void CMediaGrid::Refresh( int count, unsigned long maxId )
{
	if (count != m_nVirtualRows || maxId != m_mediaFilter.maxId)
	{
		RefreshMediaGrid();
	}
	else
	{
		ForceUpdateOneRecord(m_idActive);
	}
}
