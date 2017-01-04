#pragma once

#include "BaseDropTarget.h"
#include "ClientChartPane.h"
#include "MagicSortListCtrl.h"
#include <set>
//#define _USE_SORT_LIST

extern UINT CLIENTMANAGER_CLIENT_SEL_CHANGE;

#include "AsyncGrid.h"
#include <boost/format.hpp>

struct SClientFilter {

	int ClientGroupId;
	unsigned long count;
	unsigned long maxId;

	SClientFilter() :
	ClientGroupId(0),count(0),maxId(0)
	{

	}
	void setCount(unsigned long cnt)
	{
		count = cnt;
	}
	
	void setClientGroupId(int groupId)
	{
		ClientGroupId = groupId;
	}

	void setMaxId(int md)
	{
		maxId = md;
	}


	SClientFilter& operator = (const SClientFilter& o)
	{
		if (&o != this)
		{
			ClientGroupId = o.ClientGroupId;
		}
		return *this;
	}

	std::tstring ToString() const
	{
		std::tstring str;
		str = _T(" where GroupId = ");
		str += StringUtility::itos(ClientGroupId).c_str();
		return str;
	}
};


class CClientGrid : public CAsyncGrid2
{
public:
	virtual ~CClientGrid();

	virtual void GetData(int start, int end, const sort_and_filter_t& sf, std::map<int, std::vector<_variant_t> >& data);
	virtual int  GetVirtualRowCount();
	virtual sort_and_filter_t BuildSort();

	unsigned long GetMaxRowId();

	void          OnTimerRefresh();
	void          handle_timeout(const boost::system::error_code& ec, boost::shared_ptr<boost::asio::deadline_timer> timer);

	void          RefreshClientGrid();
	
	SClientFilter m_clientFilter;
	void          SetFilter(const SClientFilter& filter);
	SClientFilter GetFilter();

	
private:
	virtual void Refresh(int count, unsigned long maxId);
	boost::recursive_mutex	m_filterMutex;
	
};

class CClientStateTab : public CMFCTabCtrl
{
    DECLARE_DYNAMIC(CClientStateTab)

public:
    CClientStateTab();
    virtual ~CClientStateTab();

	CClientGrid m_grid;

	CImageList m_LargeImageList;
#ifdef _USE_SORT_LIST
    //CMagicSortListCtrl*       m_wndMainList;
    CMagicSortListCtrl*       m_wndSearchList;
#else
    //CMFCListCtrl*       m_wndMainList;
    CMFCListCtrl*       m_wndSearchList;
#endif
    std::vector<CMFCListCtrl*>  m_TabList;
//     UINT                m_MainListPos;
    std::map<CWnd*, int> m_wndCharts;
    std::set<int>       m_setClientChartTabIds;

    void UpdateClientList(const int nClientGroupId, BOOL bJustRefreashIfConditionTheSame = TRUE);
    virtual BOOL RemoveTab(int iTab, BOOL bRecalcLayout = TRUE);
    BOOL    GetSelectedClients(vector<INT>& ClientIDs);

    std::map<int, ClientInfo> m_ClientIDInfoMap;
    std::map<int, std::pair<ClientInfo, ClientRTStatistics>> m_ClientIDStaticsMap;

    std::map<int, ClientInfo> m_ClientIDInfoSearchMap;
    std::map<int, std::pair<ClientInfo, ClientRTStatistics>> m_ClientIDStaticsSearchMap;

    Condition    m_CurCondition;

    CBaseDropTarget             m_DropTarget;                   /// for drop target functionality
    COleDataSource              m_OleDataSource;

	void AddChartPane(int nClientId);

    enum{
        NO = 0,
        ONLINE_INDEX,
        NAME_INDEX,
        IP_INDEX,
		MAC_INDEX,
        CURLAYOUT_INDEX,
        FPS_INDEX,
        HEARTBEAT_INDEX,
        NEXTLAYOUT_INDEX,
        SCHEDULE_INDEX,
        SPEED_INDEX,
        DISK_INDEX,
        CPUUSAGE_INDEX,
        MEMUSAGE_INDEX,
		FANSPEED_INDEX,
		CPU_TEMPERATURE_INDEX,
		MB_TEMPERATURE_INDEX,
		HDD_TEMPERATURE_INDEX,
		GPU_TEMPERATURE_INDEX,
		VERSION_INDEX,
        PLAYER_INDEX
    };


protected:

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLvnItemchangedListUser(NMHDR *pNMHDR, LRESULT *pResult);
    
	afx_msg void OnNmDblClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBeginDrag(NMHDR *pNMHDR, LRESULT *pResult);

	LRESULT OnNmDblClickNew(WPARAM wParam, LPARAM lParam);
	LRESULT OnLvnBeginDragNew(WPARAM wParam, LPARAM lParam);

    afx_msg LRESULT OnDragEnter(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragOver(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragLeave(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDrop(WPARAM wParam, LPARAM lParam);

    afx_msg void OnGetToolTip(NMHDR* pNMHDR, LRESULT *pResult);

    virtual void FireChangeActiveTab(int nNewTab);

    //virtual void AddChartPane(int nClientId);

    afx_msg LRESULT SearchClients(WPARAM,LPARAM);
    afx_msg LRESULT RefreshClientListStatus(WPARAM,LPARAM);

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	
private:
	CString m_RefreshDuration;
};

// CClientStatePane

class CClientStatePane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CClientStatePane)

public:
    CClientStatePane();
    virtual ~CClientStatePane();

    CClientStateTab         m_wndStateTab;
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //afx_msg void OnTimer(UINT_PTR nIDEvent);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

    BOOL UpdateOptions();

    void UpdateClientList(const int ClientGroupId);

    //Condition               m_CurCondition;
	int                     m_ClientGroupId;
    //UINT_PTR                m_nTimer;
    CString                 m_refreshDuration;

};


