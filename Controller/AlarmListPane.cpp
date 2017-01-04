// F:\p4\sw\Demo\ContentManagement\Controller\Alarm\AlarmListPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "AlarmListPane.h"
#include "Localization/Localization.h"

// CAlarmListPane
IMPLEMENT_DYNAMIC(CAlarmListPane, CBaseControllerPane)

CAlarmListPane::CAlarmListPane()
{

}

CAlarmListPane::~CAlarmListPane()
{
}


BEGIN_MESSAGE_MAP(CAlarmListPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CAlarmListPane message handlers
int CAlarmListPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    RECT rect;
    GetClientRect(&rect);

	m_grid.SetBackgroundTask(g_backgroundTask);
    m_grid.Create(WS_CHILD|WS_VISIBLE|LVS_REPORT, rect, this, IDW_LIST_LOG);

	m_grid.InsertColumn(0, Translate(_T("Client ID")), 100);
	m_grid.InsertColumn(1, Translate(_T("Client Name")), 100);
    m_grid.InsertColumn(2, Translate(_T("Alarm Level")), 100);
    m_grid.InsertColumn(3, Translate(_T("Element")), 300);
    m_grid.InsertColumn(4, Translate(_T("Value")), 120);
    m_grid.InsertColumn(5, Translate(_T("Time")), 200);
      
	m_grid.SetVirtualRows(m_grid.GetVirtualRowCount());
    m_grid.EnableDragSelection(TRUE);
    m_grid.EnableDragHeaderItems(TRUE);
    m_grid.SetWholeRowSel(TRUE);

	g_backgroundTask->startTimer(boost::bind(&CAlarmGrid::handle_timeout, &m_grid, _1, _2));

    SetMainWnd(&m_grid);	
    return 0;
}


void CAlarmListPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

void CAlarmListPane::ClearList()
{
    //m_wndComponentList.DeleteAllItems();
}

/*
void CAlarmListPane::SetAlarmList( const std::vector<Alarm> & AlarmList )
{
    ClearList();
    auto it = AlarmList.begin(), ite = AlarmList.end();
    int i = 0;
	for (; it != ite; ++it, ++i)
    {
		if(it->GetAlarmLevel() >= AlarmLevelTypeCount || it->GetAlarmLevel() < 0)
			continue;

		if(it->GetAlarmElement() >= ElementTypeCount || it->GetAlarmElement() < 0)
			continue;

        //m_wndComponentList.InsertItem(i, StringUtility::itos(it->GetUserID()).c_str());
		m_wndComponentList.InsertItem(i, StringUtility::itos(it->GetUserID()).c_str());

		CString str1 = Translate(AlarmLevelArray[it->GetAlarmLevel()]);
		m_wndComponentList.SetItemText(i, 1,str1.GetString());
		
		CString str2 = Translate(ElementTypeArray[it->GetAlarmElement()]);
		m_wndComponentList.SetItemText(i, 2,str2.GetString());

		//m_wndComponentList.SetItemText(i, 1,StringUtility::itos(it->GetAlarmLevel()).c_str());
		//m_wndComponentList.SetItemText(i, 2,StringUtility::itos(it->GetAlarmElement()).c_str());
		m_wndComponentList.SetItemText(i, 3,StringUtility::itos(it->GetAlarmValue()).c_str());
        m_wndComponentList.SetItemText(i, 4, it->GetOperationTime().ToString().c_str());
        //m_wndComponentList.SetItemText(i, 4, it->GetDescription().c_str());
    }
}
*/


const wchar_t* pAlarmColFields[] = {
	_T("ClientId"),
	_T("ClientName"),
	_T("AlarmLevel"),
	_T("Element"),
	_T("Value"),
	_T("TimeStamp"),
};
//////////////////////////////////////////////////////////////////////////
sort_and_filter_t CAlarmGrid::BuildSort()
{
    sort_and_filter_t sf;
    int n = m_Columns.GetColumnCount();
    for (int i = 0; i < n; ++i)
    {
        int s = m_Columns.GetColumnState(i);
        if (s == 1)
        {
            sf.AddSort(/*m_Columns.GetColumnName(i)*/pAlarmColFields[i], 1);
        }
        else if (s == -1)
        {
            sf.AddSort(/*m_Columns.GetColumnName(i)*/pAlarmColFields[i], -1);
        }
        else
        {
            sf.AddSort(/*m_Columns.GetColumnName(i)*/pAlarmColFields[i], 0);
        }
    }

    sf.SetFilter(GetFilter());

    return sf;
}

int CAlarmGrid::GetVirtualRowCount()
{
    std::tstring sql = _T("select count(*) as cnt from Alarm");
    sql += GetFilter().ToString();

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
    return rs->Fields->Item["cnt"]->Value;
}

void CAlarmGrid::GetData(int start, int end, const sort_and_filter_t& sf, std::map<int, std::vector<_variant_t> >& data)
{
    std::map<int, std::vector<_variant_t> > temp;
    CString limit;
    std::tstring sql, clause;

    sql = _T("select a.ClientId, c.ClientName , a.AlarmLevel , a.Element ,a.Value ,a.TimeStamp from alarm a , client c where c.ClientId = a.ClientId");
    SAlarmFilter filter;
    filter = boost::any_cast<SAlarmFilter>(sf.GetFilter());
    sql += filter.ToString2();

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
        for (short i = 0; i < rs->Fields->Count; ++i)
        {
			int nVal;
			CString szStr;
            if(i == 2)
			{
				nVal = rs->Fields->Item["AlarmLevel"]->Value;
				szStr = Translate(AlarmLevelArray[nVal]);
				row.push_back((_variant_t)(_bstr_t)szStr.GetString());
			}
			else if(i ==3 )
			{
				nVal = rs->Fields->Item["Element"]->Value;;
				szStr = Translate(ElementTypeArray[nVal]);
				row.push_back((_variant_t)(_bstr_t)szStr.GetString());
			}
			else
				row.push_back(rs->Fields->Item[i]->Value);
        }
        temp[n] = row;
        ++n;
        rs->MoveNext();
    }

    data.swap(temp);
}

CAlarmGrid::~CAlarmGrid()
{

}

void CAlarmGrid::handle_timeout(const boost::system::error_code& ec, boost::shared_ptr<boost::asio::deadline_timer> timer)
{
	if (ec)
		return;

	if (IsWindowVisible())
	{
		OnTimerRefresh();
	}
	timer->expires_from_now(boost::posix_time::seconds( 60 )); //60 seconds
	timer->async_wait(boost::bind(&CAlarmGrid::handle_timeout, this, boost::asio::placeholders::error, timer));
}

void CAlarmGrid::OnTimerRefresh()
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
	SAlarmFilter f = GetFilter();
	if (maxId != f.maxId || count != f.count)
	{
		RefreshAlarmGrid();
	}
	else
	{
		ForceUpdateOneRecord(m_idActive);
	}
	*/
}

unsigned long CAlarmGrid::GetMaxRowId()
{
	std::tstring sql = _T("select count(*) as c, max(alarmid) as maxId from Alarm");
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

void CAlarmGrid::RefreshAlarmGrid()
{
	CWaitCursor cursor;

	m_alarmFilter.setCount(GetVirtualRowCount());
	m_alarmFilter.setMaxId(GetMaxRowId());
	UpdateAllData();
}

void CAlarmGrid::SetFilter(const SAlarmFilter& filter)
{
	boost::recursive_mutex::scoped_lock lock(m_filterMutex);
	CWaitCursor cursor;
	m_alarmFilter = filter;
	UpdateAllData();
}

SAlarmFilter CAlarmGrid::GetFilter()
{
	boost::recursive_mutex::scoped_lock lock(m_filterMutex);
	return m_alarmFilter;
}

void CAlarmGrid::Refresh( int count, unsigned long maxId )
{
	if (count != m_nVirtualRows || maxId != m_alarmFilter.maxId)
	{
		RefreshAlarmGrid();
	}
	else
	{
		ForceUpdateOneRecord(m_idActive);
	}
}