#pragma once

//#include "MagicSortListCtrl.h"
// CLogListPane

#include "AsyncGrid.h"
#include <boost/format.hpp>

struct SLogFilter {
    COleDateTime startTime;
    COleDateTime endTime;
    std::vector<int> userIds;

    SLogFilter() :
        endTime(2999, 12, 31, 23, 59, 59)
    {

    }
    SLogFilter& operator = (const SLogFilter& o)
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
#ifdef SERVER_EDITION
        ss << boost::wformat(_T(" where TimeStamp >= cast('%s' as datetime) and TimeStamp < cast('%s' as datetime)")) %
            S3Time::CreateTime(startTime).ToString().c_str() %
            S3Time::CreateTime(endTime).ToString().c_str();
#else
        ss << boost::wformat(_T(" where TimeStamp >= #%s# and TimeStamp < #%s#")) %
            S3Time::CreateTime(startTime).ToString().c_str() %
            S3Time::CreateTime(endTime).ToString().c_str();
#endif

        str = ss.str();
        if (!userIds.empty())
        {
            str += _T(" and userid = ");
            str += StringUtility::itos(userIds[0]).c_str();
        }
        return str;
    }
};

class CLogGrid : public CAsyncGrid2
{
public:
    virtual ~CLogGrid();

    virtual void GetData(int start, int end, const sort_and_filter_t& sf, std::map<int, std::vector<_variant_t> >& data);
    virtual int GetVirtualRowCount();
    virtual sort_and_filter_t BuildSort();

};

class CLogListPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CLogListPane)

public:
    CLogListPane();
    virtual ~CLogListPane();

    void ClearList();
    void SetFilter(const SLogFilter& filter);
    SLogFilter GetFilter();
    //void SetLogList(const std::vector<Log> & logList);
    //CMagicSortListCtrl        m_wndComponentList;
    CLogGrid m_grid;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()
    SLogFilter m_filter;
public:
    afx_msg void OnDestroy();

};


