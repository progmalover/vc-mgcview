#pragma once

//#include "MagicSortListCtrl.h"
// CAlarmListPane

#include "AsyncGrid.h"
#include <boost/format.hpp>

struct SAlarmFilter {
    COleDateTime startTime;
    COleDateTime endTime;
    std::vector<int> userIds;
	unsigned long count;
	unsigned long maxId;

    SAlarmFilter() :
        endTime(2999, 12, 31, 23, 59, 59),count(0),maxId(0)
    {

    }

	void setCount(unsigned long cnt)
	{
		count = cnt;
	}

	void setMaxId(int md)
	{
		maxId = md;
	}

    SAlarmFilter& operator = (const SAlarmFilter& o)
    {
        if (&o != this)
        {
            startTime = o.startTime;
            endTime = o.endTime;
            userIds = o.userIds;
        }
        return *this;
    }

    std::tstring ToString() const
    {
        std::tstringstream ss;
        std::tstring str;
        ss << boost::wformat(_T(" where  TimeStamp >= s3datetime('%s') and TimeStamp < s3datetime('%s')")) %
            S3Time::CreateTime(startTime).ToString().c_str() %
            S3Time::CreateTime(endTime).ToString().c_str();

        str = ss.str();
        /*if (!userIds.empty())
        {
            str += _T(" and userid = ");
            str += StringUtility::itos(userIds[0]).c_str();
        }*/
        return str;
    }

	std::tstring ToString2() const
    {
        std::tstringstream ss;
        std::tstring str;
        ss << boost::wformat(_T(" and  a.TimeStamp >= s3datetime('%s') and a.TimeStamp < s3datetime('%s')")) %
            S3Time::CreateTime(startTime).ToString().c_str() %
            S3Time::CreateTime(endTime).ToString().c_str();

        str = ss.str();
        /*if (!userIds.empty())
        {
            str += _T(" and userid = ");
            str += StringUtility::itos(userIds[0]).c_str();
        }*/
        return str;
    }

};

class CAlarmGrid : public CAsyncGrid2
{
public:
    virtual ~CAlarmGrid();

    virtual void GetData(int start, int end, const sort_and_filter_t& sf, std::map<int, std::vector<_variant_t> >& data);
    virtual int GetVirtualRowCount();
    virtual sort_and_filter_t BuildSort();

	unsigned long GetMaxRowId();

	void          OnTimerRefresh();
	void          handle_timeout(const boost::system::error_code& ec, boost::shared_ptr<boost::asio::deadline_timer> timer);

	void          RefreshAlarmGrid();

	SAlarmFilter m_alarmFilter;
	void          SetFilter(const SAlarmFilter& filter);
	SAlarmFilter GetFilter();
private:
	virtual void Refresh(int count, unsigned long maxId);
	boost::recursive_mutex	m_filterMutex;
};

class CAlarmListPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CAlarmListPane)

public:
    CAlarmListPane();
    virtual ~CAlarmListPane();

    void ClearList();

	CAlarmGrid m_grid;
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


