#pragma once


#include "BorrowSkin/BORROW_GridCtrl.h"
#include <boost/icl/interval_set.hpp>
#include <boost/any.hpp>
#include <boost/system/error_code.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>

class CCacheItem : public CBCGPGridItem
{
    friend class CBCGPGridCtrl;
    friend class CBCGPGridRow;

    DECLARE_DYNCREATE(CCacheItem)
protected:
    CCacheItem ();
    virtual ~CCacheItem();

    virtual _variant_t GetValue () const;
    virtual void SetValue (const _variant_t& varValue, BOOL bRedraw = TRUE);
    virtual CString GetLabel ();
};

class CBackgroundTask
{
public:
    CBackgroundTask();
    ~CBackgroundTask();
    void Post(boost::function<void()> func);
 //	void handle_timeout();

//	unsigned long m_testInterval;

	void startTimer(boost::function<void(const boost::system::error_code&, boost::shared_ptr<boost::asio::deadline_timer>)> func);
protected:
    void BackgroundThread();
    volatile bool m_stopThread;
	boost::asio::io_service m_threadIoService;	
    boost::asio::io_service::work m_threadIoServiceWork;
    boost::thread m_thread;
	boost::asio::deadline_timer m_timer;
	unsigned int m_testInterval;
};


class CSortAndFilter
{
public:
    typedef std::vector<std::tuple<std::tstring, int> > sorts_t;
    void AddSort(const std::tstring& name, int sort)
    {
        m_sorts.push_back(std::make_tuple(name, sort));
    }
    void SetFilter(const boost::any& filter)
    {
        m_filter = filter;
    }

    sorts_t GetSorts() const
    {
        return m_sorts;
    }

    boost::any GetFilter() const
    {
        return m_filter;
    }

    CSortAndFilter& operator = (const CSortAndFilter& o)
    {
        if (this != &o)
        {
            m_sorts = o.m_sorts;
            m_filter = o.m_filter;
        }

        return *this;
    }
private:
    sorts_t m_sorts;
    boost::any m_filter;
};
typedef CSortAndFilter sort_and_filter_t;


class CAsyncGrid2 : public CBCGPGridCtrl
{
    DECLARE_DYNAMIC(CAsyncGrid2)
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnRefresh(WPARAM wp, LPARAM lp);

public:
    CAsyncGrid2();
    virtual ~CAsyncGrid2();

    virtual void GetData(int start, int end, const sort_and_filter_t& sql, std::map<int, std::vector<_variant_t> >& data) = 0;
    virtual int GetVirtualRowCount() = 0;
    virtual sort_and_filter_t BuildSort() = 0;

    virtual void SetBackgroundTask(const boost::shared_ptr<CBackgroundTask>& task);

    virtual void Sort(int nColumn, BOOL bAscending = TRUE, BOOL bAdd = FALSE);
	virtual void GetSelectedMultiID(std::vector<int>& IDList);
    virtual BOOL JudgeFirstSelAudit(CString audit, int idxStatus);
	virtual void RefreshPartGrid();
    static BOOL CALLBACK GridCallback(BCGPGRID_DISPINFO* pdi, LPARAM lp);
    _variant_t GetCache(const CBCGPGridItemID& id);
	int GetCachePageSize();
    void SetCachePageSize(int size);
    void UpdateAllData();

	void Refresh_TH(int count, unsigned long maxId);


	void ForceUpdateOneRecord(const CBCGPGridItemID& id);
    void ForceUpdateVector(vector<CBCGPGridItemID*>& lst);


protected:
    virtual void GetDataTask(int start, int end, int version, const sort_and_filter_t& sf);
	virtual void Refresh(int count, unsigned long maxId);

    void PostBackgroundTask(boost::function<void()> func);

    void AddGettingInterval(int start, int end);
    void RemoveGettingInterval(int start, int end);
    void AddCachedInterval(int start, int end);
    void RemoveCachedInterval(int start, int end);


    int m_version;
    int m_cachePageSize;

    boost::mutex m_mutex;
    std::map<int, std::vector<_variant_t> > m_cachedRows;

    boost::weak_ptr<CBackgroundTask> m_backgroundTask;

    boost::icl::interval_set<int> m_cachedIntervals;
    boost::icl::interval_set<int> m_gettingIntervals;
    typedef boost::icl::interval_set<int> cached_rows_t;

   
};
