#include "stdafx.h"
#include "AsyncGrid.h"


//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CAsyncGrid2, CBCGPGridCtrl)

UINT WM_GRID_REFRESH_ALL = RegisterWindowMessage(_T("Grid refresh all"));

BEGIN_MESSAGE_MAP(CAsyncGrid2, CBCGPGridCtrl)
	ON_REGISTERED_MESSAGE(WM_GRID_REFRESH_ALL, OnRefresh)
END_MESSAGE_MAP()

CAsyncGrid2::CAsyncGrid2()
{
    m_version = 0;
    m_cachePageSize = 100;

    SetReadOnly();
    EnableVirtualMode(CAsyncGrid2::GridCallback, (LPARAM)this);
    SetSingleSel(FALSE);
    EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS | BCGP_GRID_HEADER_SORT);
    EnableLineNumbers();
    EnableInvertSelOnCtrl();
    //EnableMultipleSort();

    SetDefaultItemRTC(RUNTIME_CLASS(CCacheItem));
}

CAsyncGrid2::~CAsyncGrid2()
{
}

BOOL CALLBACK CAsyncGrid2::GridCallback(BCGPGRID_DISPINFO* pdi, LPARAM lp)
{
    ASSERT (pdi != NULL);

    int nRow = pdi->item.nRow;	// Row of an item
    int nCol = pdi->item.nCol;	// Column of an item

    pdi->item.varValue = _T("---");
    return TRUE;
}

void CAsyncGrid2::PostBackgroundTask(boost::function<void()> func)
{
    boost::shared_ptr<CBackgroundTask> task(m_backgroundTask.lock());
    if (task)
    {
        task->Post(func);
    }
}

_variant_t CAsyncGrid2::GetCache(const CBCGPGridItemID& id)
{
    boost::mutex::scoped_lock lock(m_mutex);
    if (m_cachedRows.find(id.m_nRow) != m_cachedRows.end() && id.m_nColumn >= 0 && id.m_nColumn < m_cachedRows[id.m_nRow].size())
    {
        return m_cachedRows[id.m_nRow][id.m_nColumn];
    }
    else
    {
        int nPage = id.m_nRow / m_cachePageSize;
        int start = max(0, (nPage - 1) * m_cachePageSize);
        int end = min(m_nVirtualRows, (nPage + 2) * m_cachePageSize);

        boost::icl::interval_set<int> total = m_cachedIntervals + m_gettingIntervals;
        boost::icl::interval_set<int> current;
        current += boost::icl::discrete_interval<int>(start, end);
        current -= total;


        for (auto it = current.begin(); it != current.end(); ++it)
        {
            //std::tstring sql = BuildSQL();
            AddGettingInterval(it->lower(), it->upper());
            PostBackgroundTask(boost::bind(&CAsyncGrid2::GetDataTask, this, it->lower(), it->upper(), m_version, BuildSort()));
        }

        return (LPCTSTR)_T("---");
    }
}

void CAsyncGrid2::SetCachePageSize(int size)
{
    m_cachePageSize = size;
}

int CAsyncGrid2::GetCachePageSize()
{
	return m_cachePageSize;
}

void CAsyncGrid2::GetDataTask(int start, int end, int version, const sort_and_filter_t& sf)
{
    try {
        // check before get data
        if (m_version > version)
        {
            return;
        }

        std::map<int, std::vector<_variant_t> > rows;
        GetData(start, end, sf, rows);

        boost::mutex::scoped_lock lock(m_mutex);
        // check after get data
        if (m_version > version)
        {
            return;
        }

        for (auto it = rows.begin(); it != rows.end(); ++it)
        {
            ASSERT(it->second.size() == m_Columns.GetColumnCount());
            m_cachedRows[it->first] = it->second;

            m_cachedIntervals += boost::icl::discrete_interval<int>(it->first, it->first + 1);
        }

        RemoveGettingInterval(start, end);

        ::InvalidateRect(m_hWnd, NULL, TRUE);
        ::PostMessage(m_hWnd, WM_PAINT, 0, 0);
    } catch (NetworkException&) {

    }
}

void CAsyncGrid2::AddGettingInterval(int start, int end)
{
    boost::icl::discrete_interval<int> itvl(start, end);
    m_gettingIntervals += itvl;
}

void CAsyncGrid2::RemoveGettingInterval(int start, int end)
{
    boost::icl::discrete_interval<int> itvl(start, end);
    m_gettingIntervals -= itvl;
}

void CAsyncGrid2::AddCachedInterval(int start, int end)
{
    boost::icl::discrete_interval<int> itvl(start, end);
    m_cachedIntervals += itvl;
}

void CAsyncGrid2::RemoveCachedInterval(int start, int end)
{
    boost::icl::discrete_interval<int> itvl(start, end);
    m_cachedIntervals -= itvl;
}

void CAsyncGrid2::Sort(int nColumn, BOOL bAscending /*= TRUE*/, BOOL bAdd /*= FALSE*/)
{
    CWaitCursor cursor;
    __super::Sort(nColumn, bAscending, bAdd);

    UpdateAllData();
}


void CAsyncGrid2::UpdateAllData()
{
    {
        boost::mutex::scoped_lock lock(m_mutex);
        ++m_version;

        m_gettingIntervals.clear();
        m_cachedIntervals.clear();
        m_cachedRows.clear();
    }
    SetVirtualRows(GetVirtualRowCount());
    InvalidateRect(NULL, TRUE);
    PostMessage(WM_PAINT);
}

void CAsyncGrid2::SetBackgroundTask(const boost::shared_ptr<CBackgroundTask>& task)
{
    m_backgroundTask = task;
}

void CAsyncGrid2::ForceUpdateOneRecord(const CBCGPGridItemID& id)
{
	if (m_gettingIntervals.find(id.m_nRow) != m_gettingIntervals.end())
	{
		return;
	}

	int nPage = id.m_nRow / m_cachePageSize;
	int start = max(0, (nPage - 1) * m_cachePageSize);
	int end = min(m_nVirtualRows, (nPage + 2) * m_cachePageSize);

	boost::icl::interval_set<int> total = m_gettingIntervals;
	boost::icl::interval_set<int> current;
	current += boost::icl::discrete_interval<int>(start, end);
	current -= total;


	for (auto it = current.begin(); it != current.end(); ++it)
	{
		AddGettingInterval(it->lower(), it->upper());
		PostBackgroundTask(boost::bind(&CAsyncGrid2::GetDataTask, this, it->lower(), it->upper(), m_version, BuildSort()));
	}
}

void CAsyncGrid2::ForceUpdateVector(vector<CBCGPGridItemID*>& lst)
{
	auto it = lst.begin(), ite = lst.end();
	for (; it != ite; ++it)
	{
		CBCGPGridItemID* pid = *it;
		
		if (m_gettingIntervals.find(pid->m_nRow) != m_gettingIntervals.end())
		{
			return;
		}
		
		int nPage = pid->m_nRow / m_cachePageSize;
		int start = max(0, (nPage - 1) * m_cachePageSize);
		int end = min(m_nVirtualRows, (nPage + 2) * m_cachePageSize);

		boost::icl::interval_set<int> total = m_gettingIntervals;
		boost::icl::interval_set<int> current;
		current += boost::icl::discrete_interval<int>(start, end);
		current -= total;


		for (auto it = current.begin(); it != current.end(); ++it)
		{
			AddGettingInterval(it->lower(), it->upper());
			PostBackgroundTask(boost::bind(&CAsyncGrid2::GetDataTask, this, it->lower(), it->upper(), m_version, BuildSort()));
		}
	}
}

LRESULT CAsyncGrid2::OnRefresh( WPARAM wp, LPARAM lp )
{
	Refresh((int)wp, (unsigned long)lp);
	return 0;
}


void CAsyncGrid2::Refresh( int count, unsigned long maxId )
{
	
}

void CAsyncGrid2::Refresh_TH( int count, unsigned long maxId )
{
	PostMessage(WM_GRID_REFRESH_ALL, count, maxId);
}

void CAsyncGrid2::RefreshPartGrid()
{
	vector<CBCGPGridItemID*> lstSel;
	if (!m_lstSel.IsEmpty())
	{
		for (POSITION pos = m_lstSel.GetHeadPosition (); pos != NULL;)
		{
			CBCGPGridRange* pSelRange = m_lstSel.GetNext (pos);
			ASSERT (pSelRange != NULL);
			CBCGPGridRow* pTopRow = (pSelRange->m_nTop != -1) ? GetRow (pSelRange->m_nTop) : NULL;
			int nRow = pSelRange->m_nTop;
			while(nRow <= pSelRange->m_nBottom)
			{	
				CBCGPGridItemID id = pTopRow->GetItem(0)->GetGridItemID();
				lstSel.push_back(&id);
				nRow += 1;
				pTopRow = GetRow(nRow);
			}
		}
		ForceUpdateVector(lstSel);
	}
}

BOOL CAsyncGrid2::JudgeFirstSelAudit(CString audit, int idxStatus)
{
	if (!m_lstSel.IsEmpty())
	{
		POSITION pos = m_lstSel.GetHeadPosition ();
		CBCGPGridRange* pSelRange = m_lstSel.GetNext (pos);
		ASSERT (pSelRange != NULL);
		CBCGPGridRow* pTopRow = (pSelRange->m_nTop != -1) ? GetRow (pSelRange->m_nTop) : NULL;
		if (m_pSelRow)
		{
			CBCGPGridItem *pIt = m_pSelRow->GetItem(idxStatus);
			_variant_t v = pIt->GetValue();
			if (v.vt == VT_BSTR && (v.bstrVal == audit))
			{
				return TRUE;
			}
		}	
	}
	return FALSE;
}

void CAsyncGrid2::GetSelectedMultiID(std::vector<int>& IDList)
{
	if (!m_lstSel.IsEmpty())
	{
		for (POSITION pos = m_lstSel.GetHeadPosition (); pos != NULL;)
		{
			CBCGPGridRange* pSelRange = m_lstSel.GetNext (pos);
			ASSERT (pSelRange != NULL);
			CBCGPGridRow* pTopRow = (pSelRange->m_nTop != -1) ? GetRow (pSelRange->m_nTop) : NULL;
			int nRow = pSelRange->m_nTop;
			while(nRow <= pSelRange->m_nBottom)
			{
				CBCGPGridItem *pIt = pTopRow->GetItem(0);
				_variant_t v = pIt->GetValue();
				if (v.vt != VT_BSTR)
				{
					IDList.push_back(v);
					nRow += 1;
					pTopRow = GetRow(nRow);
				}
				else
				{
					nRow += 1;
				}
			}
		}		
	}
}

IMPLEMENT_DYNCREATE(CCacheItem, CBCGPGridItem)
    CCacheItem::CCacheItem()
{

}

CCacheItem::~CCacheItem()
{

}

_variant_t CCacheItem::GetValue() const
{
    CAsyncGrid2* owner = dynamic_cast<CAsyncGrid2*>(GetOwnerList());
    if (!owner)
    {
        return m_varValue;
    }
    else
    {
        return owner->GetCache(GetGridItemID());
    }
}

void CCacheItem::SetValue(const _variant_t& varValue, BOOL bRedraw /*= TRUE*/)
{
    __super::SetValue(varValue, bRedraw);
}

CString CCacheItem::GetLabel()
{
    return FormatItem ();
}

CBackgroundTask::CBackgroundTask() :
    m_threadIoServiceWork(m_threadIoService),
    m_thread(boost::bind(&CBackgroundTask::BackgroundThread, this)),
    m_stopThread(false),
	m_timer(m_threadIoService),
	m_testInterval(30)
{

}


CBackgroundTask::~CBackgroundTask()
{
    m_stopThread = true;
    m_threadIoService.stop();
    m_thread.join();
}

void CBackgroundTask::BackgroundThread()
{
    while (true)
    {
        try
        {
            boost::system::error_code ec;
            CoInitialize(NULL);
            m_threadIoService.run(ec);
            CoUninitialize();
            break;
        }
        catch (NetworkException&)
        {
            LogController(WARN, _T("NetworkException throw in background thread."));
        }
    }
}

void CBackgroundTask::Post(boost::function<void()> func)
{
    m_threadIoService.post(func);
}

void CBackgroundTask::startTimer(boost::function<void(const boost::system::error_code&, boost::shared_ptr<boost::asio::deadline_timer>) > func)
{
    boost::shared_ptr<boost::asio::deadline_timer> timer(new boost::asio::deadline_timer(m_threadIoService));
    boost::system::error_code ec;
    func(ec, timer);
}

/*
void CBackgroundTask::handle_timeout()
{
	//
	CWnd *pWnd = SearchWndObj(AfxGetMainWnd(),RUNTIME_CLASS(CMediaLibraryListTab));
	CMediaLibraryListTab* pTab= (CMediaLibraryListTab*)pWnd;
	pTab->m_mediaGrid.OnTimerRefresh();
	m_timer.expires_from_now(boost::posix_time::seconds(30));
	m_timer.async_wait(boost::bind(&CBackgroundTask::handle_timeout, this));
}
*/