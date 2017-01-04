// F:\p4\sw\Demo\ContentManagement\Controller\Report System\ReportListPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ReportSystemPage.h"
#include "ReportListPane.h"
#include "Localization/Localization.h"

//CReportListTab
IMPLEMENT_DYNAMIC(CReportListTab, CMFCTabCtrl)

CReportListTab::CReportListTab()
{

}

CReportListTab::~CReportListTab()
{
 
}

BEGIN_MESSAGE_MAP(CReportListTab, CMFCTabCtrl)
    ON_WM_CREATE()
END_MESSAGE_MAP()

int CReportListTab::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMFCTabCtrl::OnCreate(lpCreateStruct) == -1)
        return -1;

    RECT rect;
    GetClientRect(&rect);

  
     m_ReportCltStatusCtrl.Create(CReportCltStatusDlg::IDD, this);

     
    m_ReportCltStatusCtrl.m_ReportCltStatusList.SetExtendedStyle(m_ReportCltStatusCtrl.m_ReportCltStatusList.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    m_ReportCltStatusCtrl.m_ReportCltStatusList.ShowScrollBar(SB_HORZ);
    // double buffer fix flash when do data update
    m_ReportCltStatusCtrl.m_ReportCltStatusList.SetExtendedStyle(m_ReportCltStatusCtrl.m_ReportCltStatusList.GetExtendedStyle()|LVS_EX_DOUBLEBUFFER);

    m_ReportCltStatusCtrl.m_ReportCltStatusList.InsertColumn(CLIENT_INDEX, Translate(_T("Client ID")), LVCFMT_LEFT, 100);
    m_ReportCltStatusCtrl.m_ReportCltStatusList.InsertColumn(CLIENT_NAME, Translate(_T("Client Name")), LVCFMT_LEFT, 150);
    m_ReportCltStatusCtrl.m_ReportCltStatusList.InsertColumn(GROUP_NAME, Translate(_T("Group Name")), LVCFMT_LEFT, 150);
    m_ReportCltStatusCtrl.m_ReportCltStatusList.InsertColumn(CLIENT_STATUS, Translate(_T("Client Status")), LVCFMT_LEFT, 180);
    m_ReportCltStatusCtrl.m_ReportCltStatusList.InsertColumn(CLIENT_REPORT_TIME, Translate(_T("Client Report Time")), LVCFMT_LEFT, 180);

    m_ReportCltStatusCtrl.m_ReportCltStatusList.ModifyStyle(LVS_SINGLESEL, 0);
   
    InsertTab(&m_ReportCltStatusCtrl, Translate(_T("Client Status Query")), CLIENT_STATUS_QUERY);

    m_ReportCltDldCtrl.Create(CReportCltDldDlg::IDD,this);

    
    m_ReportCltDldCtrl.m_ReportCltDldList.SetExtendedStyle(m_ReportCltDldCtrl.m_ReportCltDldList.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    m_ReportCltDldCtrl.m_ReportCltDldList.ShowScrollBar(SB_HORZ);

    // double buffer fix flash when do data update
    m_ReportCltDldCtrl.m_ReportCltDldList.SetExtendedStyle(m_ReportCltDldCtrl.m_ReportCltDldList.GetExtendedStyle()|LVS_EX_DOUBLEBUFFER);

    m_ReportCltDldCtrl.m_ReportCltDldList.InsertColumn(CLIENT_INDEX, Translate(_T("Client ID")), LVCFMT_LEFT, 100);
    m_ReportCltDldCtrl.m_ReportCltDldList.InsertColumn(CLIENT_NAME, Translate(_T("Client Name")), LVCFMT_LEFT, 150);
    m_ReportCltDldCtrl.m_ReportCltDldList.InsertColumn(GROUP_NAME, Translate(_T("Group Name")), LVCFMT_LEFT, 150);
    m_ReportCltDldCtrl.m_ReportCltDldList.InsertColumn(DOWN_FILE_NAME, Translate(_T("Download File Name")), LVCFMT_LEFT, 180);
    m_ReportCltDldCtrl.m_ReportCltDldList.InsertColumn(DOWN_RESULT_STATUS, Translate(_T("Download Result Status")), LVCFMT_LEFT, 180);
    m_ReportCltDldCtrl.m_ReportCltDldList.InsertColumn(CLIENT_REPORT_TIME, Translate(_T("Client Report Time")), LVCFMT_LEFT, 180);

    m_ReportCltDldCtrl.m_ReportCltDldList.ModifyStyle(LVS_SINGLESEL, 0);

    InsertTab(&m_ReportCltDldCtrl, Translate(_T("Client Download Query")), CLIENT_DOWNLOAD_QUERY);
    
    m_ReportCltPlayCtrl.Create(CReportCltPlayDlg::IDD,this);

    m_ReportCltPlayCtrl.m_ReportCltPlayList.SetExtendedStyle(m_ReportCltPlayCtrl.m_ReportCltPlayList.GetExtendedStyle() |LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    m_ReportCltPlayCtrl.m_ReportCltPlayList.ShowScrollBar(SB_HORZ);

    // double buffer fix flash when do data update
    m_ReportCltPlayCtrl.m_ReportCltPlayList.SetExtendedStyle(m_ReportCltPlayCtrl.m_ReportCltPlayList.GetExtendedStyle()|LVS_EX_DOUBLEBUFFER);

    m_ReportCltPlayCtrl.m_ReportCltPlayList.InsertColumn(CLIENT_INDEX, Translate(_T("Client ID")), LVCFMT_LEFT, 100);
    m_ReportCltPlayCtrl.m_ReportCltPlayList.InsertColumn(CLIENT_NAME, Translate(_T("Client Name")), LVCFMT_LEFT, 150);
    m_ReportCltPlayCtrl.m_ReportCltPlayList.InsertColumn(GROUP_NAME, Translate(_T("Group Name")), LVCFMT_LEFT, 150);
    m_ReportCltPlayCtrl.m_ReportCltPlayList.InsertColumn(PLAY_TASK_NAME, Translate(_T("Task Name")), LVCFMT_LEFT, 180);
//    m_ReportCltPlayCtrl.m_ReportCltPlayList.InsertColumn(PLAY_REGION_NAME, Translate(_T("PlayRegionName")), LVCFMT_LEFT, 180);
    m_ReportCltPlayCtrl.m_ReportCltPlayList.InsertColumn(PLAY_TIMES, Translate(_T("Play Times")), LVCFMT_LEFT, 180);
    m_ReportCltPlayCtrl.m_ReportCltPlayList.InsertColumn(PLAY_START_TIME, Translate(_T("Play Start Time")), LVCFMT_LEFT, 180);
    m_ReportCltPlayCtrl.m_ReportCltPlayList.InsertColumn(PLAY_END_TIME, Translate(_T("Play End Time")), LVCFMT_LEFT, 180);
//    m_ReportCltPlayCtrl.m_ReportCltPlayList.InsertColumn(PLAY_REPORT_TIME, Translate(_T("PlayReportTime")), LVCFMT_LEFT, 180);
   
    m_ReportCltPlayCtrl.m_ReportCltPlayList.ModifyStyle(LVS_SINGLESEL, 0);

    InsertTab(&m_ReportCltPlayCtrl, Translate(_T("Client Play Query")), CLIENT_PLAY_QUERY);

    m_ReportCltPlaySumCtrl.Create(CReportCltPlaySumDlg::IDD,this);

    m_ReportCltPlaySumCtrl.m_ReportCltPlaySumList.SetExtendedStyle(m_ReportCltPlaySumCtrl.m_ReportCltPlaySumList.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    m_ReportCltPlaySumCtrl.m_ReportCltPlaySumList.ShowScrollBar(SB_HORZ);

    // double buffer fix flash when do data update
    m_ReportCltPlaySumCtrl.m_ReportCltPlaySumList.SetExtendedStyle(m_ReportCltPlaySumCtrl.m_ReportCltPlaySumList.GetExtendedStyle()|LVS_EX_DOUBLEBUFFER);

    m_ReportCltPlaySumCtrl.m_ReportCltPlaySumList.InsertColumn(PLAY_TASK_NAME, Translate(_T("Task Name")), LVCFMT_LEFT, 120);
    m_ReportCltPlaySumCtrl.m_ReportCltPlaySumList.InsertColumn(PLAY_TASK_TYPE, Translate(_T("Task Type")), LVCFMT_LEFT, 120);
    m_ReportCltPlaySumCtrl.m_ReportCltPlaySumList.InsertColumn(PLAY_TIMES, Translate(_T("Play Times")), LVCFMT_LEFT,180);

    m_ReportCltPlaySumCtrl.m_ReportCltPlaySumList.ModifyStyle(LVS_SINGLESEL, 0);

    InsertTab(&m_ReportCltPlaySumCtrl, Translate(_T("Task Play Sum Query")), CLIENT_PLAY_SUM_QUERY);

    m_ReportCltDetailCtrl.Create(CReportCltDetailDlg::IDD,this);

    m_ReportCltDetailCtrl.m_ReportCltDetailList.SetExtendedStyle(m_ReportCltDetailCtrl.m_ReportCltDetailList.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    m_ReportCltDetailCtrl.m_ReportCltDetailList.ShowScrollBar(SB_HORZ);

    // double buffer fix flash when do data update
    m_ReportCltDetailCtrl.m_ReportCltDetailList.SetExtendedStyle(m_ReportCltDetailCtrl.m_ReportCltDetailList.GetExtendedStyle()|LVS_EX_DOUBLEBUFFER);

    m_ReportCltDetailCtrl.m_ReportCltDetailList.InsertColumn(CLIENT_INDEX, Translate(_T("Client ID")), LVCFMT_LEFT, 100);
    m_ReportCltDetailCtrl.m_ReportCltDetailList.InsertColumn(CLIENT_NAME, Translate(_T("Client Name")), LVCFMT_LEFT, 150);
    m_ReportCltDetailCtrl.m_ReportCltDetailList.InsertColumn(DISK_REMAIN_CAPACITY, Translate(_T("Hardware Info")), LVCFMT_LEFT, 500);
    m_ReportCltDetailCtrl.m_ReportCltDetailList.InsertColumn(RELATED_LED_NUM, Translate(_T("Location")), LVCFMT_LEFT, 120);
    m_ReportCltDetailCtrl.m_ReportCltDetailList.InsertColumn(LINE_NUM, Translate(_T("Phone")), LVCFMT_LEFT,180);
    //m_ReportCltDetailCtrl.m_ReportCltDetailList.InsertColumn(BUSINESS_PROPERTY, Translate(_T("DisplayLayout")), LVCFMT_LEFT, 120);
    m_ReportCltDetailCtrl.m_ReportCltDetailList.InsertColumn(CLIENT_GROUP_NAME, Translate(_T("Group Name")), LVCFMT_LEFT,180);

    m_ReportCltDetailCtrl.m_ReportCltDetailList.ModifyStyle(LVS_SINGLESEL, 0);

    InsertTab(&m_ReportCltDetailCtrl, Translate(_T("Client Details Query")), CLIENT_DETAILS_QUERY);


    m_ReportCltAvailCtrl.Create(CReportCltAvailDlg::IDD,this);

    m_ReportCltAvailCtrl.m_ReportCltAvailList.SetExtendedStyle(m_ReportCltAvailCtrl.m_ReportCltAvailList.GetExtendedStyle() |LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    m_ReportCltAvailCtrl.m_ReportCltAvailList.ShowScrollBar(SB_HORZ);

    // double buffer fix flash when do data update
    m_ReportCltAvailCtrl.m_ReportCltAvailList.SetExtendedStyle(m_ReportCltAvailCtrl.m_ReportCltAvailList.GetExtendedStyle()|LVS_EX_DOUBLEBUFFER);

    m_ReportCltAvailCtrl.m_ReportCltAvailList.InsertColumn(CLIENT_INDEX, Translate(_T("Client ID")), LVCFMT_LEFT, 100);
    m_ReportCltAvailCtrl.m_ReportCltAvailList.InsertColumn(CLIENT_NAME, Translate(_T("Client Name")), LVCFMT_LEFT, 150);
    m_ReportCltAvailCtrl.m_ReportCltAvailList.InsertColumn(GROUP_NAME, Translate(_T("Group Name")), LVCFMT_LEFT, 150);
    m_ReportCltAvailCtrl.m_ReportCltAvailList.InsertColumn(CLIENT_AVAILABILITY_QUERY, Translate(_T("Availability")), LVCFMT_LEFT, 180);
  

    m_ReportCltAvailCtrl.m_ReportCltAvailList.ModifyStyle(LVS_SINGLESEL, 0);

    InsertTab(&m_ReportCltAvailCtrl, Translate(_T("Client Availability Query")), CLIENT_AVAILABILITY_QUERY);

    m_ReportCltOnlineCtrl.Create(CReportCltOnlineDlg::IDD,this);

    m_ReportCltOnlineCtrl.m_ReportCltOnlineList.SetExtendedStyle(m_ReportCltOnlineCtrl.m_ReportCltOnlineList.GetExtendedStyle() |LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    m_ReportCltOnlineCtrl.m_ReportCltOnlineList.ShowScrollBar(SB_HORZ);

    // double buffer fix flash when do data update
    m_ReportCltOnlineCtrl.m_ReportCltOnlineList.SetExtendedStyle(m_ReportCltOnlineCtrl.m_ReportCltOnlineList.GetExtendedStyle()|LVS_EX_DOUBLEBUFFER);

    m_ReportCltOnlineCtrl.m_ReportCltOnlineList.InsertColumn(CLIENT_GROUP_ID,Translate(_T("Group ID")), LVCFMT_LEFT, 100 );
    m_ReportCltOnlineCtrl.m_ReportCltOnlineList.InsertColumn(CLIENT_GROUP_NAME, Translate(_T("Group Name")), LVCFMT_LEFT, 150);
    m_ReportCltOnlineCtrl.m_ReportCltOnlineList.InsertColumn(ONLINE_RATE, Translate(_T("Online Rate")), LVCFMT_LEFT, 180);

    m_ReportCltOnlineCtrl.m_ReportCltOnlineList.ModifyStyle(LVS_SINGLESEL, 0);

    InsertTab(&m_ReportCltOnlineCtrl, Translate(_T("Client Group Online Query")), CLINET_ONLINE_RATE);
    
    return 0;
}

CString CReportListTab::FormatSizeString(DWORD64 Size)
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


void CReportListTab::ClearList()
{
    //    m_wndComponentList.DeleteAllItems();
}
/////////////////////////////////////////////////////////////////////////////
// CReportListPane

IMPLEMENT_DYNAMIC(CReportListPane, CBaseControllerPane)

CReportListPane::CReportListPane()
{

}

CReportListPane::~CReportListPane()
{
}


BEGIN_MESSAGE_MAP(CReportListPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_REGISTERED_MESSAGE(AFX_WM_CHANGE_ACTIVE_TAB, &CReportListPane::OnChangeActiveTab)
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CReportListPane message handlers
int CReportListPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    RECT rect;
    GetClientRect(&rect);

   //create report tab pane
    m_wndStateTab.Create(CMFCTabCtrl::STYLE_3D_VS2005, rect, this, IDW_LIST_REPORT_TAB);
    m_wndStateTab.SetLocation(CMFCBaseTabCtrl::LOCATION_TOP);
    m_wndStateTab.EnableActiveTabCloseButton(FALSE);
    m_wndStateTab.AutoDestroyWindow(TRUE);

    SetMainWnd(&m_wndStateTab);
    Condition cond(_T("1"));
    //UpdateMediaLibraryList(cond);

    return 0;
}


void CReportListPane::OnDestroy()
{
    m_wndStateTab.m_ReportCltPlaySumCtrl.DestroyWindow();
    m_wndStateTab.m_ReportCltStatusCtrl.DestroyWindow();
    m_wndStateTab.m_ReportCltPlayCtrl.DestroyWindow();
    m_wndStateTab.m_ReportCltDldCtrl.DestroyWindow();
    m_wndStateTab.m_ReportCltOnlineCtrl.DestroyWindow();
    m_wndStateTab.m_ReportCltDetailCtrl.DestroyWindow();
    m_wndStateTab.m_ReportCltOnlineCtrl.DestroyWindow();
    m_wndStateTab.RemoveAllTabs();
    m_wndStateTab.DestroyWindow();

    CBaseControllerPane::OnDestroy();
}

LRESULT CReportListPane::OnChangeActiveTab(WPARAM wp, LPARAM lp)
{
    // wp is the active tab index
    int index = (int)wp;
    switch(index)
    {
    case  CReportListTab::CLIENT_STATUS_QUERY:
        {
  //          m_wndStateTab.m_ReportCltStatusCtrl.ShowWindow(SW_SHOW);
        }
        break;
    case CReportListTab::CLIENT_DOWNLOAD_QUERY:
        {
            
        }
        break;
    case CReportListTab::CLIENT_PLAY_QUERY:
        {

        }
        break;
    case CReportListTab::CLIENT_PLAY_SUM_QUERY:
        {

        }
        break;
    case CReportListTab::CLIENT_DETAILS_QUERY:
        {

        }
        break;
    case CReportListTab::CLIENT_AVAILABILITY_QUERY:
        {

        }
        break;
    case CReportListTab::CLINET_ONLINE_RATE:
        {

        }
        break;
    default:
        break;

    }
  
      
    return 0;
}
