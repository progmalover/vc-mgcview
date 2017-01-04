// F:\p4\sw\Demo\ContentManagement\Controller\Client Monitor\ClientStatePane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ClientStatePane.h"
#include "Localization/Localization.h"


extern CLIPFORMAT CLIENT_MANAGER_CLIP_FMT;

extern UINT CLIENTMANAGER_CLIENT_SEL_CHANGE = ::RegisterWindowMessage(_T("CLIENTMANAGER_CLIENT_SEL_CHANGE;"));

extern UINT BCGM_GRID_BEGINDRAG;
extern UINT BCGM_GRID_ITEM_DBLCLICK;

// CClientStatePane
 
IMPLEMENT_DYNAMIC(CClientStatePane, CBaseControllerPane)

CClientStatePane::CClientStatePane()
    : m_ClientGroupId(-1)
    //, m_nTimer (0)
{
}

CClientStatePane::~CClientStatePane()
{

}


BEGIN_MESSAGE_MAP(CClientStatePane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    //ON_WM_TIMER()
END_MESSAGE_MAP()



// CClientStatePane message handlers
int CClientStatePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    RECT rect;
    GetClientRect(&rect);

    m_wndStateTab.Create(CMFCTabCtrl::STYLE_3D_VS2005, rect, this, IDW_LIST_CLIENT_TAB);
    m_wndStateTab.SetLocation(CMFCBaseTabCtrl::LOCATION_TOP);
    m_wndStateTab.EnableActiveTabCloseButton(FALSE);
    m_wndStateTab.AutoDestroyWindow(TRUE);

    SetMainWnd(&m_wndStateTab);

    UpdateClientList(-1);

    return 0;
}


void CClientStatePane::OnDestroy()
{
    m_wndStateTab.RemoveAllTabs();
    m_wndStateTab.DestroyWindow();
    CBaseControllerPane::OnDestroy();
}

void CClientStatePane::UpdateClientList(const int ClientGroupId)
{
    //if (m_nTimer == 0)
    {
        GetConfig->Get(CLIENT_MONITOR_REFRESH, m_refreshDuration);

        //m_nTimer = SetTimer(0xFF000001, StringUtility::stoi(m_refreshDuration.GetString()) * 1000, 0);
    }

	m_wndStateTab.UpdateClientList(ClientGroupId, FALSE);  
	m_ClientGroupId = ClientGroupId;
}

/*
void CClientStatePane::OnTimer(UINT_PTR nIDEvent)
{
    if (IsWindowVisible())
    {
        CBaseControllerPane::OnTimer(nIDEvent);
        m_wndStateTab.UpdateClientList(m_ClientGroupId);
    }
}
*/

BOOL CClientStatePane::UpdateOptions()
{
    GetConfig->Get(CLIENT_MONITOR_REFRESH, m_refreshDuration);
	/*
    if (m_nTimer)
    {
        KillTimer(m_nTimer);

        m_nTimer = SetTimer(0xFF000001, StringUtility::stoi(m_refreshDuration.GetString()) * 1000, 0);
    }
	*/
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CClientStateTab, CMFCTabCtrl)

CClientStateTab::CClientStateTab()
{
}

CClientStateTab::~CClientStateTab()
{
    auto it = m_wndCharts.begin(), ite = m_wndCharts.end();
    for (; it != ite; ++it)
    {
        it->first->DestroyWindow();
        delete it->first;
    }

    m_wndCharts.clear();
}

BEGIN_MESSAGE_MAP(CClientStateTab, CMFCTabCtrl)
    ON_WM_CREATE()
    ON_NOTIFY(/*LVN_ITEMCHANGED*/NM_CLICK, IDW_LIST_CLIENT_STATUS, &CClientStateTab::OnLvnItemchangedListUser)
    //ON_WM_LBUTTONDBLCLK()

    ON_NOTIFY(LVN_GETINFOTIP, IDW_LIST_CLIENT_STATUS, OnGetToolTip)

	ON_REGISTERED_MESSAGE(BCGM_GRID_BEGINDRAG, OnLvnBeginDragNew )

    ON_NOTIFY(LVN_BEGINDRAG, IDW_LIST_CLIENT_STATUS, OnLvnBeginDrag)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGENTER, OnDragEnter)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGOVER, OnDragOver)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGLEAVE, OnDragLeave)
    ON_REGISTERED_MESSAGE(WM_DND_DROP, OnDrop)

	//ON_MESSAGE(USER_MSG_SERRCH_CLIENT, SearchClients)
    //ON_MESSAGE(USER_MSG_UPDATE_CLIENT_STATUS, RefreshClientListStatus)

	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_DBLCLICK, OnNmDblClickNew )
	ON_NOTIFY(NM_DBLCLK, IDW_LIST_CLIENT_STATUS, &CClientStateTab::OnNmDblClick)
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


int CClientStateTab::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMFCTabCtrl::OnCreate(lpCreateStruct) == -1)
        return -1;

    RECT rect;
    GetClientRect(&rect);
    UINT uStyle = WS_CHILD|WS_VISIBLE|LVS_REPORT;

	m_grid.SetBackgroundTask(g_backgroundTask);
	m_grid.Create(WS_CHILD|WS_VISIBLE|LVS_REPORT, rect, this, IDW_LIST_CLIENT_STATUS);

	m_grid.InsertColumn(NO, Translate(_T("ClientID")), 80);
	m_grid.InsertColumn(ONLINE_INDEX, Translate(_T("Status")), 150);
	m_grid.InsertColumn(NAME_INDEX, Translate(_T("Client Name")), 80);
	m_grid.InsertColumn(IP_INDEX, Translate(_T("IP Address")),  80);
	m_grid.InsertColumn(MAC_INDEX, Translate(_T("MAC Address")),  150);
	m_grid.InsertColumn(CURLAYOUT_INDEX, Translate(_T("Current Layout")),  300);
	m_grid.InsertColumn(FPS_INDEX, Translate(_T("FPS")),  60);
	m_grid.InsertColumn(HEARTBEAT_INDEX, Translate(_T("Heart Beat")),  150);
	m_grid.InsertColumn(NEXTLAYOUT_INDEX, Translate(_T("Next Layout")),  150);
	m_grid.InsertColumn(SCHEDULE_INDEX, Translate(_T("Schedule")),  150);
	m_grid.InsertColumn(SPEED_INDEX, Translate(_T("Speed")),  80);
	m_grid.InsertColumn(DISK_INDEX, Translate(_T("Disk")),  150);
	m_grid.InsertColumn(CPUUSAGE_INDEX, Translate(_T("CPU Usage")),  80);

	m_grid.InsertColumn(MEMUSAGE_INDEX, Translate(_T("Memory Usage")),  80);
	m_grid.InsertColumn(FANSPEED_INDEX, Translate(_T("Fan Speed")),  80);
	m_grid.InsertColumn(CPU_TEMPERATURE_INDEX, Translate(_T("CPU Temperature")),  80);
	m_grid.InsertColumn(MB_TEMPERATURE_INDEX, Translate(_T("MotherBoard Temperature")),  80);
	m_grid.InsertColumn(HDD_TEMPERATURE_INDEX, Translate(_T("HDD Temperature")),  80);
	m_grid.InsertColumn(GPU_TEMPERATURE_INDEX, Translate(_T("GPU Temperature")),  80);
	m_grid.InsertColumn(VERSION_INDEX, Translate(_T("Client Version")),  80);
	m_grid.InsertColumn(PLAYER_INDEX, Translate(_T("Player Status")),  500);

	m_grid.SetVirtualRows(m_grid.GetVirtualRowCount());
	m_grid.EnableDragSelection(TRUE);
	m_grid.EnableDragHeaderItems(TRUE);
	m_grid.SetWholeRowSel(TRUE);

	m_grid.GetColumnsInfo().RemoveAllSortColumns();
	m_grid.GetColumnsInfo().EnableMultipleSort(false);

	//g_backgroundTask->startTimer(boost::bind(&CClientGrid::handle_timeout, &m_grid, boost::asio::placeholders::error));
	g_backgroundTask->startTimer(boost::bind(&CClientGrid::handle_timeout, &m_grid, _1, _2));

	//m_TabList.push_back(m_grid);
	//m_wndCharts.insert(std::map<CWnd*, int>::value_type(&m_grid, -1));
	InsertTab((CWnd*)&m_grid, Translate(_T("main")), 0);

    uStyle = WS_CHILD|WS_VISIBLE|LVS_REPORT;
#ifdef _USE_SORT_LIST
    m_wndSearchList = new CMagicSortListCtrl;
#else
    m_wndSearchList = new CMFCListCtrl;
     uStyle |= LVS_NOSORTHEADER;
#endif
    m_wndSearchList->Create(uStyle, rect, this, IDW_LIST_CLIENT_STATUS);
    m_wndSearchList->SetExtendedStyle(m_wndSearchList->GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    m_wndSearchList->ShowScrollBar(SB_HORZ);

    m_wndSearchList->SetExtendedStyle(m_wndSearchList->GetExtendedStyle()|LVS_EX_DOUBLEBUFFER);

    m_wndSearchList->InsertColumn(NO, Translate(_T("NO.")), LVCFMT_LEFT, 30);
    m_wndSearchList->InsertColumn(ONLINE_INDEX, Translate(_T("Status")), LVCFMT_LEFT, 40);
    m_wndSearchList->InsertColumn(NAME_INDEX, Translate(_T("Name")), LVCFMT_LEFT, 80);
    m_wndSearchList->InsertColumn(IP_INDEX, Translate(_T("IP Address")), LVCFMT_LEFT, 80);
    m_wndSearchList->InsertColumn(MAC_INDEX, Translate(_T("MAC Address")), LVCFMT_LEFT, 80);
    m_wndSearchList->InsertColumn(CURLAYOUT_INDEX, Translate(_T("Current Layout")), LVCFMT_LEFT, 300);
    m_wndSearchList->InsertColumn(FPS_INDEX, Translate(_T("FPS")), LVCFMT_LEFT, 60);
    m_wndSearchList->InsertColumn(HEARTBEAT_INDEX, Translate(_T("Heart Beat")), LVCFMT_LEFT, 150);
    m_wndSearchList->InsertColumn(NEXTLAYOUT_INDEX, Translate(_T("Next Layout")), LVCFMT_LEFT, 150);
    m_wndSearchList->InsertColumn(SCHEDULE_INDEX, Translate(_T("Schedule")), LVCFMT_LEFT, 150);
    m_wndSearchList->InsertColumn(SPEED_INDEX, Translate(_T("Speed")), LVCFMT_LEFT, 80);
    m_wndSearchList->InsertColumn(DISK_INDEX, Translate(_T("Disk")), LVCFMT_LEFT, 150);
    m_wndSearchList->InsertColumn(CPUUSAGE_INDEX, Translate(_T("CPU Usage")), LVCFMT_LEFT, 80);
    m_wndSearchList->InsertColumn(MEMUSAGE_INDEX, Translate(_T("Memory Usage")), LVCFMT_LEFT, 80);
    m_wndSearchList->InsertColumn(FANSPEED_INDEX, Translate(_T("Fan Speed")), LVCFMT_LEFT, 80);
    m_wndSearchList->InsertColumn(CPU_TEMPERATURE_INDEX, Translate(_T("CPU Temperature")), LVCFMT_LEFT, 80);
    m_wndSearchList->InsertColumn(MB_TEMPERATURE_INDEX, Translate(_T("MotherBoard Temperature")), LVCFMT_LEFT, 80);
    m_wndSearchList->InsertColumn(HDD_TEMPERATURE_INDEX, Translate(_T("HDD Temperature")), LVCFMT_LEFT, 80);
    m_wndSearchList->InsertColumn(GPU_TEMPERATURE_INDEX, Translate(_T("GPU Temperature")), LVCFMT_LEFT, 80);
    m_wndSearchList->InsertColumn(VERSION_INDEX, Translate(_T("Client Version")), LVCFMT_LEFT, 80);
    m_wndSearchList->InsertColumn(PLAYER_INDEX, Translate(_T("Player Status")), LVCFMT_LEFT, 500);
#ifdef _USE_SORT_LIST
    m_wndSearchList->SetCompairFunPair(_T("MAC Address"),BaseStrComp);
    m_wndSearchList->SetCompairFunPair(_T("FPS"),BaseStrComp);
    m_wndSearchList->SetCompairFunPair(_T("Heart Beat"),BaseStrComp);
    m_wndSearchList->SetCompairFunPair(_T("Next Layout"),BaseStrComp);

    m_wndSearchList->SetCompairFunPair(_T("Schedule"),BaseStrComp);
    m_wndSearchList->SetCompairFunPair(_T("Speed"),MediaSizeComp);
    m_wndSearchList->SetCompairFunPair(_T("Disk"),MediaSizeComp);
    m_wndSearchList->SetCompairFunPair(_T("CPU Usage"),BaseStrComp);
    m_wndSearchList->SetCompairFunPair(_T("Memory Usage"),BaseStrComp);
    m_wndSearchList->SetCompairFunPair(_T("Fan Speed"),BaseStrComp);
    m_wndSearchList->SetCompairFunPair(_T("CPU Temperature"),BaseStrComp);
    m_wndSearchList->SetCompairFunPair(_T("MotherBoard Temperature"),BaseStrComp);
    m_wndSearchList->SetCompairFunPair(_T("HDD Temperature"),BaseStrComp);
    m_wndSearchList->SetCompairFunPair(_T("GPU Temperature"),BaseStrComp);
    m_wndSearchList->SetCompairFunPair(_T("Client Version"),BaseStrComp);
    m_wndSearchList->SetCompairFunPair(_T("Player Status"),BaseStrComp);
#endif
    m_TabList.push_back(m_wndSearchList);
    m_wndCharts.insert(std::map<CWnd*, int>::value_type(m_wndSearchList, 0));
    InsertTab(m_wndSearchList, Translate(_T("Search Result")), 1);
    m_wndSearchList->SetExtendedStyle(m_wndSearchList->GetExtendedStyle() | LVS_EX_INFOTIP);
    m_DropTarget.Register (this);
	GetConfig->Get(CLIENT_MONITOR_REFRESH, m_RefreshDuration);

    return 0;
}

BOOL CClientStateTab::GetSelectedClients(vector<INT>& ClientIDs)
{
    if (GetActiveTab() == 0)
    {
		INT selectCount = m_grid.GetSelectionCount();
		for(int i = 0 ;i < selectCount; i++)
		{
			CBCGPGridRange range;
			if (m_grid.GetSelection (i, range))
			{
				const int nRowOffset = range.m_nTop;
				const int nRowCount = range.m_nBottom - range.m_nTop + 1;

				for (int nRow = 0; nRow < nRowCount; nRow++)
				{
					int nRowIndex = nRow + nRowOffset;
					ASSERT(nRowIndex >= 0);

					CBCGPGridRow * currow = m_grid.GetRow(nRowIndex);

					CBCGPGridItem* curitem = currow->GetItem(0);
					std::tstring szValue = (TCHAR*)(_bstr_t)curitem->GetValue();
					int nValue = StringUtility::stoi(szValue);

					ClientIDs.push_back(nValue);
				}
			}
		}
		
    }
    else if (GetActiveTab() == 1)
    {
		POSITION pos = m_wndSearchList->GetFirstSelectedItemPosition();
		while(pos)
		{
			INT index = m_wndSearchList->GetNextSelectedItem(pos);
			INT SelectedID = -1;
			if (index >= 0)
			{
				SelectedID = m_wndSearchList->GetItemData(index);
				ClientIDs.push_back(SelectedID);
			}
		}
    }
    else
    {
        return FALSE;
    }

	if (ClientIDs.size() > 0)
	{
		return TRUE;
	}

    return FALSE;
}

void CClientStateTab::UpdateClientList(const int nClientGroupId, BOOL bJustRefreashIfConditionTheSame)
{
    if (!GetControllerApp->IsLogin())
    {
        return;
    }

	/*
	SClientFilter filter;
	filter.ClientGroupId = nClientGroupId;
	SetFilter(filter);
	*/
	
	m_grid.m_clientFilter.setClientGroupId(nClientGroupId);
	m_grid.OnTimerRefresh();

	/*
    POSITION pos = m_wndMainList->GetFirstSelectedItemPosition();
    INT index = -1;
	INT SelectedID = -1;
	if(pos) 
	{
		index = m_wndMainList->GetNextSelectedItem(pos);
    	if (index >= 0)
		{
			SelectedID = m_wndMainList->GetItemData(index);
		}
	}
    if (condition.ToXML() != m_CurCondition.ToXML() || !bJustRefreashIfConditionTheSame)
    {
        m_CurCondition = condition;

        //How to new a buf and post it to work thread
        UINT nBufID;
        Condition* pInfo = WorkThreadDataManager< Condition >::NewDataBuf(nBufID); //create dynamic msg
        if (pInfo == NULL)
        {
            return;
        }
        //Do any thing with your new buf
        pInfo[0] = m_CurCondition;

        if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID,USER_MSG_SERRCH_CLIENT,
            (WPARAM)this->GetSafeHwnd(),(LPARAM)nBufID))//post thread msg
        {
            printf("post message failed,errno:%d\n",::GetLastError());
        }

        return;
    }

    //How to new a buf and post it to work thread
    UINT nBufID;
    std::map<int, ClientInfo>* pInfo = WorkThreadDataManager< std::map<int, ClientInfo> >::NewDataBuf(nBufID); //create dynamic msg
    if (pInfo == NULL)
    {
        return;
    }
    //Do any thing with your new buf

    int nCount = m_wndMainList->GetItemCount();
    if(nCount > 0)
    {
        INT VisibleEnd = m_wndMainList->GetTopIndex() + m_wndMainList->GetCountPerPage() + 1;
        for(int i = m_wndMainList->GetTopIndex(); i < VisibleEnd; ++i)
        {
            if (m_wndMainList->IsItemVisible(i))
            {
                INT ID = m_wndMainList->GetItemData(i);

                std::map<int, ClientInfo>::iterator it = m_ClientIDInfoMap.find(ID);
                if (it != m_ClientIDInfoMap.end())
                {
                    pInfo[0].insert(*it);
                }

            }
        }
    }

    if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID,USER_MSG_UPDATE_CLIENT_STATUS,
        (WPARAM)this->GetSafeHwnd(),(LPARAM)nBufID))//post thread msg
    {
        printf("post message failed,errno:%d\n",::GetLastError());
    }
	*/

}
#if 0
LRESULT CClientStateTab::SearchClients(WPARAM wp,LPARAM lp)
{
    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf< std::map<int, ClientInfo> > MessageBuf;
    if (!WorkThreadDataManager< std::map<int, ClientInfo> >::GetDataBuf(nMessageBufID, MessageBuf))
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////

    m_ClientIDInfoMap.clear();

    m_ClientIDInfoMap = *MessageBuf.Get();


    int visindex = m_wndMainList->GetTopIndex();
    int posx  = m_wndMainList->GetScrollPos(SB_HORZ);
    m_wndMainList->SetRedraw(FALSE);
    m_wndMainList->DeleteAllItems();
    INT insertIndex = -1;
    auto it = m_ClientIDInfoMap.begin(), ite = m_ClientIDInfoMap.end();
    for (int i = 0; it != ite; ++it, ++i)
    {
        ClientInfo clientInfo = it->second;
		/*
		LV_ITEM lvi;
		lvi.mask = LVIF_IMAGE;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.pszText = (LPWSTR)clientInfo.GetClientDesc().c_str();
		lvi.iImage = 0;
		INT tempIndex = m_wndMainList->InsertItem(&lvi);
		*/
		//INT tempIndex = m_wndMainList->InsertItem(i, clientInfo.GetClientDesc().c_str(),1);
		INT tempIndex = m_wndMainList->InsertItem(i, clientInfo.GetClientDesc().c_str());
        m_wndMainList->SetItemData(i, it->first);

        std::tstring strName;

        strName = clientInfo.GetClientDesc();


        CString szNotAvailable = _T("N\\A");

        CString szClientID;
        szClientID.Format(_T("%d"), clientInfo.GetClientID());
        m_wndMainList->SetItemText(i, NO, szClientID);
        m_wndMainList->SetItemText(i, ONLINE_INDEX, szNotAvailable);
        m_wndMainList->SetItemText(i, NAME_INDEX, strName.c_str());
        m_wndMainList->SetItemText(i, IP_INDEX, szNotAvailable);
		m_wndMainList->SetItemText(i, MAC_INDEX, szNotAvailable);
        m_wndMainList->SetItemText(i, CURLAYOUT_INDEX, szNotAvailable);
        m_wndMainList->SetItemText(i, FPS_INDEX, szNotAvailable);
        m_wndMainList->SetItemText(i, HEARTBEAT_INDEX, szNotAvailable);
        m_wndMainList->SetItemText(i, NEXTLAYOUT_INDEX, szNotAvailable);
        m_wndMainList->SetItemText(i, SCHEDULE_INDEX, szNotAvailable);
        m_wndMainList->SetItemText(i, SPEED_INDEX, szNotAvailable);
        m_wndMainList->SetItemText(i, DISK_INDEX, szNotAvailable);
        m_wndMainList->SetItemText(i, CPUUSAGE_INDEX, szNotAvailable);
        m_wndMainList->SetItemText(i, MEMUSAGE_INDEX, szNotAvailable);
		m_wndMainList->SetItemText(i, FANSPEED_INDEX, szNotAvailable);
		m_wndMainList->SetItemText(i, CPU_TEMPERATURE_INDEX, szNotAvailable);
		m_wndMainList->SetItemText(i, MB_TEMPERATURE_INDEX, szNotAvailable);
		m_wndMainList->SetItemText(i, HDD_TEMPERATURE_INDEX, szNotAvailable);
		m_wndMainList->SetItemText(i, GPU_TEMPERATURE_INDEX, szNotAvailable);
		m_wndMainList->SetItemText(i, VERSION_INDEX, szNotAvailable);
        m_wndMainList->SetItemText(i, PLAYER_INDEX, szNotAvailable);

    }

    m_wndMainList->SetRedraw(TRUE);

    RECT ys;
    if(m_wndMainList->GetItemRect(visindex,&ys,LVIR_BOUNDS) != 0)
    {
        CSize tmpsize;
        tmpsize.cx = posx;
        tmpsize.cy = (ys.bottom-ys.top)*(visindex);
        m_wndMainList->Scroll(tmpsize);
    }

    UpdateClientList(m_CurCondition);
    return 0;
}



LRESULT CClientStateTab::RefreshClientListStatus(WPARAM wp,LPARAM lp)
{
    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf< std::map< int, std::pair<ClientInfo, ClientRTStatistics> >  > MessageBuf;
    if (!WorkThreadDataManager< std::map< int, std::pair<ClientInfo, ClientRTStatistics> >  >::GetDataBuf(nMessageBufID, MessageBuf))
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////

    const std::map< int, std::pair<ClientInfo, ClientRTStatistics> >   * pConstInfo = MessageBuf.Get();

    for (auto it = pConstInfo->begin(); it != pConstInfo->end(); it++)
    {
        m_ClientIDStaticsMap[it->first] = it->second;
    }

    POSITION pos = m_wndMainList->GetFirstSelectedItemPosition();
	INT index = -1;
	INT SelectedID = -1;
	if(pos)
	{
		index = m_wndMainList->GetNextSelectedItem(pos);
		if (index >= 0)
		{
			SelectedID = m_wndMainList->GetItemData(index);
		}
	}
	

    int nCount = m_wndMainList->GetItemCount();
    int clientId;
    if(nCount > 0)
    {
        INT VisibleEnd = m_wndMainList->GetTopIndex() + m_wndMainList->GetCountPerPage() + 1;
        for(int i = m_wndMainList->GetTopIndex(); i < VisibleEnd; ++i)
        {
            if (m_wndMainList->IsItemVisible(i))
            {
                INT ID = m_wndMainList->GetItemData(i);

                std::map< int, std::pair<ClientInfo, ClientRTStatistics> >::const_iterator it = pConstInfo->find(ID);
                if (it != pConstInfo->end())
                {
                    ClientInfo clientInfo = it->second.first;
                    clientId = clientInfo.GetClientID();

                    std::tstring strName;

                    strName = clientInfo.GetClientDesc();

					CString strMACAddress;
					strMACAddress = clientInfo.GetMacAddress().c_str();

                    ClientRTStatistics Statistics = it->second.second;

                    LAYOUT_DESC CurLayoutDesc, NextLayoutDesc;
                    SCHEDULE_DESC ScheduleDesc;
                    PLAYER_STATUS_DESC PlayerStatus;
                    SYSTEM_STATUS_DESC SystemStatus;
                    S3Time LastHeartBeat(0,0,0,0,0,0);

                    Statistics.GetCurLayoutDesc(CurLayoutDesc);
                    Statistics.GetNextLayoutDesc(NextLayoutDesc);
                    Statistics.GetScheduleDesc(ScheduleDesc);
                    Statistics.GetPlayerStatusDesc(PlayerStatus);
                    Statistics.GetLastActiveTime(LastHeartBeat);
                    Statistics.GetSysStatusDesc(SystemStatus);

					/* by raymondLiu
					COleDateTime now = COleDateTime::GetCurrentTime();
					COleDateTimeSpan duration(0,0,0,StringUtility::stoi(m_RefreshDuration.GetString()));
					if (now > LastHeartBeat.ToCOleDateTime() + duration
						|| LastHeartBeat == S3Time::NULLTime)
					{
						Statistics.SetOffline();
					}*/


                    CString CstrClientID, CstrCurLayout, CstrNextLayout, CstrSchedule, CstrSpeed, CstrOnline,
                        CstrPlayerStatus, CstrHeartBeat, CstrFps, CstrDisc, CstrCPU, CstrMem, CstrIP,
						CstrFanSpeed, CstrCPUTemperature, CstrMBTemperature, CstrHDDTemperature, CstrGPUTemperature, CstrVersion;

                    CstrClientID.Format(_T("%d"), clientId);

                    //CstrOnline = Translate((Statistics.IsOnline() ? _T("Online") : _T("Offline")));

                    if (Statistics.IsOnline())
                    {
                        CstrIP = Statistics.GetIPAddr().c_str();

                        CString CurDownloadSize;

                        CurDownloadSize.Format( _T("%s/%s"),
                            FormatSizeString(CurLayoutDesc.DownloadedSize), FormatSizeString(CurLayoutDesc.LayoutSize));

						if (Statistics.IsDefaultLayout())
						{
							CstrCurLayout.Format( _T("%s: %s, %s"),
								CurLayoutDesc.LayoutName.c_str(),
								CurDownloadSize,
								Translate(_T("Default Layout")));
						}
						else
						{
							CstrCurLayout.Format( _T("%s: %s, %s - %s"),
								CurLayoutDesc.LayoutName.c_str(),
								CurDownloadSize,
								CurLayoutDesc.StartTime.ToString().c_str(), CurLayoutDesc.EndTime.ToString().c_str());
						}
                        if (CurLayoutDesc.LayoutName.empty())
                        {
                            CstrCurLayout = Translate(_T("No Layout arranged in schedule!"));
                        }

                        CString NextDownloadSize;
                        NextDownloadSize.Format( _T("%s/%s"),
                            FormatSizeString(NextLayoutDesc.DownloadedSize), FormatSizeString(NextLayoutDesc.LayoutSize));

                        CstrNextLayout.Format( _T("%s: %s, %s - %s"),
                            NextLayoutDesc.LayoutName.c_str(),
                            NextDownloadSize,
                            NextLayoutDesc.StartTime.ToString().c_str(), NextLayoutDesc.EndTime.ToString().c_str());

                        if (NextLayoutDesc.LayoutName.empty())
                        {
                            CstrNextLayout =Translate(_T("No Layout arranged in schedule!"));
                        }

                        CString SDownloadSize;
                        SDownloadSize.Format( _T("%s/%s"),
                            FormatSizeString(ScheduleDesc.DownloadedSize), FormatSizeString(ScheduleDesc.TotalSize));

                        CstrSchedule.Format( Translate(_T("%s, To: %s, Layout count: %d")),
                            SDownloadSize,
                            ScheduleDesc.EndTime.ToString().c_str(), ScheduleDesc.LayoutCount);

                        CstrSpeed.Format( _T("%d KB/s"), SystemStatus.TransportRate);


                        CstrPlayerStatus.Format( Translate(_T("Device lost count: %d, Status: %s")),
                            PlayerStatus.DeviceLostCount, Translate(PlayerStatus.Status.c_str()));

                        CstrFps.Format( _T("%d"), PlayerStatus.FPS);

                        CstrHeartBeat = LastHeartBeat.ToString().c_str();
                        /*COleDateTimeSpan TimeGap = COleDateTime::GetCurrentTime() - LastHeartBeat.ToCOleDateTime();

                        if (TimeGap.GetDays() > 1)
                        {
                        CString str;
                        str.Format(_T("%d Days,"), TimeGap.GetDays());
                        CstrHeartBeat.Append(str);
                        }
                        if (TimeGap.GetHours() > 1)
                        {
                        CString str;
                        str.Format(_T("%d Hours,"), TimeGap.GetHours());
                        CstrHeartBeat.Append(str);
                        }
                        if (TimeGap.GetMinutes() > 1)
                        {
                        CString str;
                        str.Format(_T("%d Minutes,"), TimeGap.GetMinutes());
                        CstrHeartBeat.Append(str);
                        }

                        CString str;
                        str.Format(_T("%d Seconds Before"), TimeGap.GetSeconds());
                        CstrHeartBeat.Append(str);*/

                        CstrDisc.Format( Translate(_T("%d %%, Free Size: %d MB ")), SystemStatus.DiskUsage, SystemStatus.DiskFreeSpace);
                       
						if(SystemStatus.CPUUsage >= 0)
							CstrCPU.Format( _T("%d %%"), SystemStatus.CPUUsage);

						if(SystemStatus.MemUsage >= 0)
							CstrMem.Format( _T("%d %%"), SystemStatus.MemUsage);

						if(SystemStatus.FanSpeed >= 0 )
							CstrFanSpeed.Format( _T("%d RPM"), SystemStatus.FanSpeed);
											
						if (SystemStatus.CPUTemperature > 0)
							CstrCPUTemperature.Format( _T("%d \x2103"), SystemStatus.CPUTemperature);

						if (SystemStatus.MotherBoardTemperature > 0)
							CstrMBTemperature.Format( _T("%d \x2103"), SystemStatus.MotherBoardTemperature);

						if (SystemStatus.HDDTemperature > 0)
							CstrHDDTemperature.Format( _T("%d \x2103"), SystemStatus.HDDTemperature);

						if (SystemStatus.S3GPUTemperature > 0)
							CstrGPUTemperature.Format( _T("%d \x2103"), SystemStatus.S3GPUTemperature);

						CstrVersion.Format( _T("%s"), SystemStatus.Version.c_str());
						
						/*
						if(SystemStatus.DiskFreeSpace < clientInfo.GetMinFreeDiskSize() || PlayerStatus.FPS < clientInfo.GetMinFps())
						//if(SystemStatus.DiskFreeSpace < 1000)
						{
							LVITEM lvitem;
							lvitem.mask = LVIF_IMAGE ;
							lvitem.iItem    = i; 
							lvitem.iSubItem = 0;
							m_wndMainList->GetItem(&lvitem); 
							lvitem.iImage = 1;
							m_wndMainList->SetItem(&lvitem);
						}*/
						CstrOnline = Translate(_T("Online"));
						if(SystemStatus.DiskFreeSpace < clientInfo.GetMinFreeDiskSize() )
						{
							CstrOnline += _T("-");
							CstrOnline += Translate(_T("Low Disk Space"));
						}
							
						if( PlayerStatus.FPS < clientInfo.GetMinFps())
						{
							CstrOnline += _T("-");
							CstrOnline += Translate(_T("Low Fps"));
						}
                    }
					else
					{
						/*
						LVITEM lvitem;
						lvitem.mask = LVIF_IMAGE ;
						lvitem.iItem    = i; 
						lvitem.iSubItem = 0;
						m_wndMainList->GetItem(&lvitem); 
						lvitem.iImage = 0;
						m_wndMainList->SetItem(&lvitem);
						*/
						CstrOnline = Translate(_T("Offline"));
					}
                   
					m_wndMainList->SetItemText(i, NO, CstrClientID);
                    m_wndMainList->SetItemText(i, ONLINE_INDEX, CstrOnline);
                    m_wndMainList->SetItemText(i, NAME_INDEX, strName.c_str());
                    m_wndMainList->SetItemText(i, IP_INDEX, CstrIP);
					m_wndMainList->SetItemText(i, MAC_INDEX, strMACAddress);
                    m_wndMainList->SetItemText(i, CURLAYOUT_INDEX, CstrCurLayout);
                    m_wndMainList->SetItemText(i, FPS_INDEX, CstrFps);
                    m_wndMainList->SetItemText(i, HEARTBEAT_INDEX, CstrHeartBeat);
                    m_wndMainList->SetItemText(i, NEXTLAYOUT_INDEX, CstrNextLayout);
                    m_wndMainList->SetItemText(i, SCHEDULE_INDEX, CstrSchedule);
                    m_wndMainList->SetItemText(i, SPEED_INDEX, CstrSpeed);
                    m_wndMainList->SetItemText(i, DISK_INDEX, CstrDisc);
                    m_wndMainList->SetItemText(i, CPUUSAGE_INDEX, CstrCPU);
                    m_wndMainList->SetItemText(i, MEMUSAGE_INDEX, CstrMem);
					m_wndMainList->SetItemText(i, FANSPEED_INDEX, CstrFanSpeed);
					m_wndMainList->SetItemText(i, CPU_TEMPERATURE_INDEX, CstrCPUTemperature);
					m_wndMainList->SetItemText(i, MB_TEMPERATURE_INDEX, CstrMBTemperature);
					m_wndMainList->SetItemText(i, HDD_TEMPERATURE_INDEX, CstrHDDTemperature);
					m_wndMainList->SetItemText(i, GPU_TEMPERATURE_INDEX, CstrGPUTemperature);
					m_wndMainList->SetItemText(i, VERSION_INDEX, CstrVersion);
                    m_wndMainList->SetItemText(i, PLAYER_INDEX, CstrPlayerStatus);

					if(i ==  index)
					{
						 GetParent()->SendMessage(CLIENTMANAGER_CLIENT_SEL_CHANGE, (WPARAM)&clientInfo, (LPARAM)&Statistics);
					}
                }
            }
        }
    }

    return 0;
}
#endif

void CClientStateTab::OnLvnItemchangedListUser(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
    int iCurrent = pNMLV->iItem;
    int clientId;
    ClientRTStatistics* pStatics = NULL;
    if(iCurrent > -1)
    {
        ClientInfo* pInfo = NULL;
        if (GetActiveTab() == 0)
        {
				/*
                clientId = m_wndMainList->GetItemData(iCurrent);
 				auto it = m_ClientIDInfoMap.find(clientId);
                if (it != m_ClientIDInfoMap.end())
                {
                    pInfo = &(it->second);
                }
  
                std::map<int, std::pair<ClientInfo, ClientRTStatistics>>::iterator it2 = m_ClientIDStaticsMap.find(clientId);
                if (it2 != m_ClientIDStaticsMap.end())
                {
                    pStatics = &(it2->second.second);
                }*/
        }
        else if (GetActiveTab() == 1)
        {
                clientId = m_wndSearchList->GetItemData(iCurrent); 
                auto it = m_ClientIDInfoSearchMap.find(clientId);
                if (it != m_ClientIDInfoSearchMap.end())
                {
                    pInfo = &(it->second);
                }

                std::map<int, std::pair<ClientInfo, ClientRTStatistics>>::iterator it2 = m_ClientIDStaticsSearchMap.find(clientId);
                if (it2 != m_ClientIDStaticsSearchMap.end())
                {
                    pStatics = &(it2->second.second);
                }
        }
        else
            return;
				
				
				
		  GetParent()->SendMessage(CLIENTMANAGER_CLIENT_SEL_CHANGE, (WPARAM)pInfo, (LPARAM)pStatics);
    }
    else
    {
        GetParent()->SendMessage(CLIENTMANAGER_CLIENT_SEL_CHANGE, NULL);
    }
}


LRESULT CClientStateTab::OnNmDblClickNew(WPARAM wParam, LPARAM lParam)
{
	CBCGPGridItem* pClickedItem = reinterpret_cast<CBCGPGridItem *>(lParam);
    if(pClickedItem == NULL )
	{
		return 0;
	}
	
	int clientId;

	CBCGPGridRow* currow = pClickedItem->GetParentRow();
	CBCGPGridItem* curitem = currow->GetItem(0);
	std::tstring szValue = (TCHAR*)(_bstr_t)curitem->GetValue();
	clientId = StringUtility::stoi(szValue.c_str());
	AddChartPane(clientId);
	return 1;
}

void CClientStateTab::OnNmDblClick(NMHDR *pNMHDR, LRESULT *pResult)
{
#if 0
    return;
#else
    LPNMITEMACTIVATE pNMLV = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    *pResult = 0;

	
    int iCurrent = pNMLV->iItem;
    if (iCurrent == -1) return;

	int clientId;
	/*
	int clientId = m_wndMainList->GetItemData(iCurrent);
	*/
	int tabIndex = GetActiveTab();
	if(0 == tabIndex)
	{
		INT selectCount = m_grid.GetSelectionCount();
		if (1 == selectCount)
		{
			// get selected media id
			CBCGPGridRow* currow = m_grid.GetRow(iCurrent);
			CBCGPGridItem* curitem = currow->GetItem(0);
			std::tstring szValue = (TCHAR*)(_bstr_t)curitem->GetValue();
			clientId = StringUtility::stoi(szValue.c_str());
			AddChartPane(clientId);
		}
	}
	else if(1 == tabIndex)
	{
		clientId = m_wndSearchList->GetItemData(iCurrent);
		AddChartPane(clientId);
	}
    
#endif
}

void CClientStateTab::FireChangeActiveTab(int nNewTab)
{
    CMFCTabCtrl::FireChangeActiveTab(nNewTab);

    CWnd* pWnd = GetActiveWnd();
    BOOL bDisable = FALSE;
    for (auto it = m_TabList.begin(); it != m_TabList.end(); it++)
    {
        if (*it == pWnd)
        {
            EnableActiveTabCloseButton(FALSE);
            bDisable = TRUE;
            break;
        }
    }

    if (!bDisable)
    {
        EnableActiveTabCloseButton(TRUE);
    }

}

void CClientStateTab::AddChartPane(int nClientId)
{
    if (m_setClientChartTabIds.find(nClientId) != m_setClientChartTabIds.end())
    {
        CString label;
        //this->SetActiveTab();
        for (int i = 0; i < GetTabsNum(); ++i)
        {
            CWnd* wnd = GetTabWnd(i);
            auto it = m_wndCharts.find(wnd), ite = m_wndCharts.end();
            if (it != ite && it->second == nClientId)
            {
                SetActiveTab(i);
            }
        }
    }
    else
    {
        RECT rect;
        GetClientRect(&rect);

        CClientChartPane* wndChart = new CClientChartPane(nClientId);
        wndChart->Create(Translate(_T("Chart")), this, rect, TRUE, ID_VIEW_CLIENTSTATELIST, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_HIDE_INPLACE | WS_CAPTION,
            AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE);

        m_wndCharts.insert(std::map<CWnd*, int>::value_type(wndChart, nClientId));
        m_setClientChartTabIds.insert(nClientId);
        ClientInfo clientInfo;
        HRESULT hr = GetControllerInterface->GetClient(nClientId, clientInfo);
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
            return;
        }
        InsertTab(wndChart, clientInfo.GetClientDesc().c_str(), GetTabsNum());
        SetActiveTab(GetTabsNum()-1);
    }
}

BOOL CClientStateTab::RemoveTab(int iTab, BOOL bRecalcLayout /*= TRUE*/)
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
        m_setClientChartTabIds.erase(it->second);
        m_wndCharts.erase(wnd);
    }

    return TRUE;
}



void CClientStateTab::OnLButtonUp(UINT nFlags, CPoint point)
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

LRESULT CClientStateTab::OnLvnBeginDragNew(WPARAM wParam, LPARAM lParam)
{
	vector<INT> ClientIDs;
	if (GetSelectedClients(ClientIDs))
	{
		INT Len = sizeof(INT) + ClientIDs.size()*sizeof(INT);

		HGLOBAL hgData = GlobalAlloc(GHND|GMEM_SHARE, Len);
		ASSERT(hgData!=NULL);

		INT* lpData = (INT*)::GlobalLock(hgData);
		ASSERT(lpData!=NULL);

		lpData[0] = ClientIDs.size();
		memcpy(lpData+1, ClientIDs.data(), ClientIDs.size()*sizeof(INT));

		::GlobalUnlock(hgData);

		// Cache the data, and initiate DragDrop
		m_OleDataSource.CacheGlobalData(CLIENT_MANAGER_CLIP_FMT, hgData);

		DROPEFFECT dropEffect
			= m_OleDataSource.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE);

		m_OleDataSource.Empty();
	}
	return 1;
}

void CClientStateTab::OnLvnBeginDrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    UNREFERENCED_PARAMETER(pResult);

    LPNMITEMACTIVATE pNMItemActive = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    vector<INT> ClientIDs;
    if (GetSelectedClients(ClientIDs))
    {
        INT Len = sizeof(INT) + ClientIDs.size()*sizeof(INT);

        HGLOBAL hgData = GlobalAlloc(GHND|GMEM_SHARE, Len);
        ASSERT(hgData!=NULL);

        INT* lpData = (INT*)::GlobalLock(hgData);
        ASSERT(lpData!=NULL);

        lpData[0] = ClientIDs.size();
        memcpy(lpData+1, ClientIDs.data(), ClientIDs.size()*sizeof(INT));

        ::GlobalUnlock(hgData);

        // Cache the data, and initiate DragDrop
        m_OleDataSource.CacheGlobalData(CLIENT_MANAGER_CLIP_FMT, hgData);

        DROPEFFECT dropEffect
            = m_OleDataSource.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE);

        m_OleDataSource.Empty();
    }
}

LRESULT CClientStateTab::OnDrop(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1;

    return lr;
}

LRESULT CClientStateTab::OnDragEnter(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = DROPEFFECT_MOVE;

    return lr;
}

LRESULT CClientStateTab::OnDragOver(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = DROPEFFECT_MOVE;

    return lr;
}

LRESULT CClientStateTab::OnDragLeave(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0;

    return lr;
}

void CClientStateTab::OnGetToolTip(NMHDR* pNMHDR, LRESULT *pResult)
{
    LPNMLVGETINFOTIPW pToolTip = (LPNMLVGETINFOTIPW)pNMHDR;

    pResult = 0;
}

LRESULT CClientStateTab::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    ASSERT_VALID(this);

    LRESULT lResult;
    if(message >= 0xC000)
    {
        if (OnWndMsg(message, wParam, lParam, &lResult))
            return lResult;
        else
        {
            this->GetOwner()->SendMessage(message, wParam, lParam);
        }
    }

    return CMFCTabCtrl::WindowProc(message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
sort_and_filter_t CClientGrid::BuildSort()
{
	sort_and_filter_t sf;
	/*
	int n = m_Columns.GetColumnCount();
	for (int i = 0; i < n; ++i)
	{
		std::tstring szColumnName = m_Columns.GetColumnName(i);
		if( _T("LayoutId") == szColumnName || _T("LayoutName") == szColumnName || _T("Duration") == szColumnName || _T("LayoutSize") == szColumnName )
		{
			int s = m_Columns.GetColumnState(i);
			if (s == 1)
			{
				sf.AddSort(m_Columns.GetColumnName(i), 1);
			}
			else if (s == -1)
			{
				sf.AddSort(m_Columns.GetColumnName(i), -1);
			}
			else
			{
				sf.AddSort(m_Columns.GetColumnName(i), 0);
			}
		}
	}
	*/
	sf.SetFilter(GetFilter());
	
	return sf;
}

int CClientGrid::GetVirtualRowCount()
{
	std::tstring sql = _T("select count(*) as c from Client");
	sql += GetFilter().ToString();
	sql += _T(" and Status <> @AUDIT_DELETED@");
	//sql += m_sortClause;

	ADODB::_RecordsetPtr rs;
	try
	{
		if(S_OK != GetControllerInterface->QuerySQL(sql, rs))
			return 0;
	}
	catch(...)
	{
		return 0;
	}

	rs->MoveFirst();
	return rs->Fields->Item["c"]->Value;
}

void CClientGrid::GetData(int start, int end, const sort_and_filter_t& sf, std::map<int, std::vector<_variant_t> >& data){
	std::map<int, std::vector<_variant_t> > temp;
	CString limit;
	std::tstring sql, clause;
	
	sql = _T("select ClientId, ClientName ,MacAddress from client");
	

	SClientFilter filter;
	filter = boost::any_cast<SClientFilter>(sf.GetFilter());
	sql += filter.ToString();
	sql += _T(" and Status <> @AUDIT_DELETED@");

	clause = _T(" order by ");

	// sort clause
	sort_and_filter_t::sorts_t sorts;
	sorts = sf.GetSorts();
	int n = sorts.size();
	int i = 0;
	bool bFirst = true;
	bool bSort = false;
	for (i = 0; i < n; ++i)
	{
		int s = std::get<1>(sorts[i]);
		if (!bFirst && s != 0)
		{
			clause += _T(", ");
		}


		if (s == 1)
		{
			clause += std::get<0>(sorts[i]);
			clause += _T(" ASC");
			bSort = true;
			bFirst = false;
		}
		else if (s == -1)
		{
			clause += std::get<0>(sorts[i]);
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

#if defined(DB_MYSQL) || defined(DB_SQLITE)
	limit.Format(_T(" limit %d, %d"), start, end - start);
#elif defined(DB_ACCESS)
	limit.Format(_T(" between %d and %d"), start, end - 1);
#endif
	sql += limit.GetString();

	ADODB::_RecordsetPtr rs;
	if (S_OK != GetControllerInterface->QuerySQL(sql, rs))
		return;

	n = start;
	while (!rs->EndOfFile)
	{
		std::vector<_variant_t> row;

		int clientId = rs->Fields->GetItem(_T("ClientId"))->GetValue();
		std::tstring clientName = (TCHAR*)(_bstr_t)rs->Fields->GetItem(_T("ClientName"))->GetValue();

		ClientRTStatistics Statistics;
		GetControllerInterface->GetClientRTStatistics(clientId, Statistics);

		LAYOUT_DESC CurLayoutDesc, NextLayoutDesc;
        SCHEDULE_DESC ScheduleDesc;
        PLAYER_STATUS_DESC PlayerStatus;
        SYSTEM_STATUS_DESC SystemStatus;
        S3Time LastHeartBeat(0,0,0,0,0,0);

        Statistics.GetCurLayoutDesc(CurLayoutDesc);
        Statistics.GetNextLayoutDesc(NextLayoutDesc);
        Statistics.GetScheduleDesc(ScheduleDesc);
        Statistics.GetPlayerStatusDesc(PlayerStatus);
        Statistics.GetLastActiveTime(LastHeartBeat);
        Statistics.GetSysStatusDesc(SystemStatus);

        CString CstrClientID, CstrCurLayout, CstrNextLayout, CstrSchedule, CstrSpeed, CstrOnline,
            CstrPlayerStatus, CstrHeartBeat, CstrFps, CstrDisc, CstrCPU, CstrMem, CstrIP,
			CstrFanSpeed, CstrCPUTemperature, CstrMBTemperature, CstrHDDTemperature, CstrGPUTemperature, CstrVersion;

        CstrClientID.Format(_T("%d"), clientId);

        //CstrOnline = Translate((Statistics.IsOnline() ? _T("Online") : _T("Offline")));

        if (Statistics.IsOnline())
        {
            CstrIP = Statistics.GetIPAddr().c_str();

            CString CurDownloadSize;

            CurDownloadSize.Format( _T("%s/%s"),
                FormatSizeString(CurLayoutDesc.DownloadedSize), FormatSizeString(CurLayoutDesc.LayoutSize));

			if (Statistics.IsDefaultLayout())
			{
				CstrCurLayout.Format( _T("%s: %s, %s"),
					CurLayoutDesc.LayoutName.c_str(),
					CurDownloadSize,
					Translate(_T("Default Layout")));
			}
			else
			{
				CstrCurLayout.Format( _T("%s: %s, %s - %s"),
					CurLayoutDesc.LayoutName.c_str(),
					CurDownloadSize,
					CurLayoutDesc.StartTime.ToString().c_str(), CurLayoutDesc.EndTime.ToString().c_str());
			}
            if (CurLayoutDesc.LayoutName.empty())
            {
                CstrCurLayout = Translate(_T("No Layout arranged in schedule!"));
            }

            CString NextDownloadSize;
            NextDownloadSize.Format( _T("%s/%s"),
                FormatSizeString(NextLayoutDesc.DownloadedSize), FormatSizeString(NextLayoutDesc.LayoutSize));

            CstrNextLayout.Format( _T("%s: %s, %s - %s"),
                NextLayoutDesc.LayoutName.c_str(),
                NextDownloadSize,
                NextLayoutDesc.StartTime.ToString().c_str(), NextLayoutDesc.EndTime.ToString().c_str());

            if (NextLayoutDesc.LayoutName.empty())
            {
                CstrNextLayout =Translate(_T("No Layout arranged in schedule!"));
            }

            CString SDownloadSize;
            SDownloadSize.Format( _T("%s/%s"),
                FormatSizeString(ScheduleDesc.DownloadedSize), FormatSizeString(ScheduleDesc.TotalSize));

            CstrSchedule.Format( Translate(_T("%s, To: %s, Layout count: %d")),
                SDownloadSize,
                ScheduleDesc.EndTime.ToString().c_str(), ScheduleDesc.LayoutCount);

            CstrSpeed.Format( _T("%d KB/s"), SystemStatus.TransportRate);


            CstrPlayerStatus.Format( Translate(_T("Device lost count: %d, Status: %s")),
                PlayerStatus.DeviceLostCount, Translate(PlayerStatus.Status.c_str()));

            CstrFps.Format( _T("%d"), PlayerStatus.FPS);

            CstrHeartBeat = LastHeartBeat.ToString().c_str();
            /*COleDateTimeSpan TimeGap = COleDateTime::GetCurrentTime() - LastHeartBeat.ToCOleDateTime();

            if (TimeGap.GetDays() > 1)
            {
            CString str;
            str.Format(_T("%d Days,"), TimeGap.GetDays());
            CstrHeartBeat.Append(str);
            }
            if (TimeGap.GetHours() > 1)
            {
            CString str;
            str.Format(_T("%d Hours,"), TimeGap.GetHours());
            CstrHeartBeat.Append(str);
            }
            if (TimeGap.GetMinutes() > 1)
            {
            CString str;
            str.Format(_T("%d Minutes,"), TimeGap.GetMinutes());
            CstrHeartBeat.Append(str);
            }

            CString str;
            str.Format(_T("%d Seconds Before"), TimeGap.GetSeconds());
            CstrHeartBeat.Append(str);*/

            CstrDisc.Format( Translate(_T("%d %%, Free Size: %d MB ")), SystemStatus.DiskUsage, SystemStatus.DiskFreeSpace);
                       
			if(SystemStatus.CPUUsage >= 0)
				CstrCPU.Format( _T("%d %%"), SystemStatus.CPUUsage);

			if(SystemStatus.MemUsage >= 0)
				CstrMem.Format( _T("%d %%"), SystemStatus.MemUsage);

			if(SystemStatus.FanSpeed >= 0 )
				CstrFanSpeed.Format( _T("%d RPM"), SystemStatus.FanSpeed);
											
			if (SystemStatus.CPUTemperature > 0)
				CstrCPUTemperature.Format( _T("%d \x2103"), SystemStatus.CPUTemperature);

			if (SystemStatus.MotherBoardTemperature > 0)
				CstrMBTemperature.Format( _T("%d \x2103"), SystemStatus.MotherBoardTemperature);

			if (SystemStatus.HDDTemperature > 0)
				CstrHDDTemperature.Format( _T("%d \x2103"), SystemStatus.HDDTemperature);

			if (SystemStatus.S3GPUTemperature > 0)
				CstrGPUTemperature.Format( _T("%d \x2103"), SystemStatus.S3GPUTemperature);

			CstrVersion.Format( _T("%s"), SystemStatus.Version.c_str());
						
			/*
			if(SystemStatus.DiskFreeSpace < clientInfo.GetMinFreeDiskSize() || PlayerStatus.FPS < clientInfo.GetMinFps())
			//if(SystemStatus.DiskFreeSpace < 1000)
			{
				LVITEM lvitem;
				lvitem.mask = LVIF_IMAGE ;
				lvitem.iItem    = i; 
				lvitem.iSubItem = 0;
				m_wndMainList->GetItem(&lvitem); 
				lvitem.iImage = 1;
				m_wndMainList->SetItem(&lvitem);
			}*/
			CstrOnline = Translate(_T("Online"));
			/*
			if(SystemStatus.DiskFreeSpace < clientInfo.GetMinFreeDiskSize() )
			{
				CstrOnline += _T("-");
				CstrOnline += Translate(_T("Low Disk Space"));
			}
							
			if( PlayerStatus.FPS < clientInfo.GetMinFps())
			{
				CstrOnline += _T("-");
				CstrOnline += Translate(_T("Low Fps"));
			}*/
        }
		else
		{
			/*
			LVITEM lvitem;
			lvitem.mask = LVIF_IMAGE ;
			lvitem.iItem    = i; 
			lvitem.iSubItem = 0;
			m_wndMainList->GetItem(&lvitem); 
			lvitem.iImage = 0;
			m_wndMainList->SetItem(&lvitem);
			*/
			CstrOnline = Translate(_T("Offline"));
		}

		row.push_back(rs->Fields->GetItem(_T("ClientId"))->GetValue());//layoutid
		row.push_back((_variant_t)(_bstr_t)CstrOnline.GetString());
		row.push_back(rs->Fields->GetItem(_T("ClientName"))->GetValue());//name
		row.push_back((_variant_t)(_bstr_t)CstrIP.GetString());
		row.push_back(rs->Fields->GetItem(_T("MacAddress"))->GetValue());
		row.push_back((_variant_t)(_bstr_t)CstrCurLayout.GetString());
		row.push_back((_variant_t)(_bstr_t)CstrFps.GetString());
		row.push_back((_variant_t)(_bstr_t)CstrHeartBeat.GetString());
		row.push_back((_variant_t)(_bstr_t)CstrNextLayout.GetString());
		row.push_back((_variant_t)(_bstr_t)CstrSchedule.GetString());
		row.push_back((_variant_t)(_bstr_t)CstrSpeed.GetString());
		row.push_back((_variant_t)(_bstr_t)CstrDisc.GetString());
		row.push_back((_variant_t)(_bstr_t)CstrCPU.GetString());
		row.push_back((_variant_t)(_bstr_t)CstrMem.GetString());
		row.push_back((_variant_t)(_bstr_t)CstrFanSpeed.GetString());
		row.push_back((_variant_t)(_bstr_t)CstrCPUTemperature.GetString());
		row.push_back((_variant_t)(_bstr_t)CstrMBTemperature.GetString());
		row.push_back((_variant_t)(_bstr_t)CstrHDDTemperature.GetString());
		row.push_back((_variant_t)(_bstr_t)CstrGPUTemperature.GetString());
		row.push_back((_variant_t)(_bstr_t)CstrVersion.GetString());
		row.push_back((_variant_t)(_bstr_t)CstrPlayerStatus.GetString());

		temp[n] = row;
		++n;
		rs->MoveNext();
	}

	data.swap(temp);
}

CClientGrid::~CClientGrid()
{

}
/*
void CClientGrid::handle_timeout(const boost::system::error_code& ec)
{
	if(IsWindowVisible())
	{
		OnTimerRefresh();
	}
	g_backgroundTask->startTimer(boost::bind(&CClientGrid::handle_timeout, this, boost::asio::placeholders::error));	
}
*/
void CClientGrid::handle_timeout(const boost::system::error_code& ec, boost::shared_ptr<boost::asio::deadline_timer> timer)
{
	if (ec)
		return;

	if (IsWindowVisible())
	{
		OnTimerRefresh();
	}
	CString  szRefreshDuration;
	GetConfig->Get(CLIENT_MONITOR_REFRESH, szRefreshDuration);
	timer->expires_from_now(boost::posix_time::seconds( StringUtility::stoi(szRefreshDuration.GetString()) ));
	timer->async_wait(boost::bind(&CClientGrid::handle_timeout, this, boost::asio::placeholders::error, timer));
	//g_backgroundTask->startTimer(boost::bind(&CMediaGrid::handle_timeout, this, boost::asio::placeholders::error, timer));	
}

void CClientGrid::OnTimerRefresh()
{
    try
    {
	    unsigned long maxId = GetMaxRowId();
	    int count = GetVirtualRowCount();
	    Refresh_TH(count, maxId);
    }
    catch (NetworkException&)
    {	
    } 
	/*
	int maxId = GetMaxRowId();
	unsigned long count = GetVirtualRowCount();
	SClientFilter f = GetFilter();
	if (maxId != f.maxId || count != f.count)
	{
		RefreshClientGrid();
	}
	else
	{
		ForceUpdateOneRecord(m_idActive);
	}
	*/
}

unsigned long CClientGrid::GetMaxRowId()
{
	std::tstring sql = _T("select count(*) as c, max(clientid) as maxId from client");
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

void CClientGrid::RefreshClientGrid()
{
	CWaitCursor cursor;

	m_clientFilter.setCount(GetVirtualRowCount());
	m_clientFilter.setMaxId(GetMaxRowId());
	UpdateAllData();
}

void CClientGrid::SetFilter(const SClientFilter& filter)
{
	boost::recursive_mutex::scoped_lock lock(m_filterMutex);
	CWaitCursor cursor;
	m_clientFilter = filter;
	UpdateAllData();
}

SClientFilter CClientGrid::GetFilter()
{
	boost::recursive_mutex::scoped_lock lock(m_filterMutex);
	return m_clientFilter;
}

void CClientGrid::Refresh( int count, unsigned long maxId )
{
	if (count != m_nVirtualRows || maxId != m_clientFilter.maxId)
	{
		RefreshClientGrid();
	}
	else
	{
		ForceUpdateOneRecord(m_idActive);
	}
}