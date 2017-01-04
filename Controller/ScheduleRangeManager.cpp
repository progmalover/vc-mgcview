#include "StdAfx.h"
#include "ScheduleRangeManager.h"

ScheduleRangeItem::ScheduleRangeItem()
{

}
ScheduleRangeItem::~ScheduleRangeItem()
{

}

COleDateTime& ScheduleRangeItem::GetRangeStart()
{
    return m_Start;
}

COleDateTime& ScheduleRangeItem::GetRangeEnd()
{
    return m_End;
}

void ScheduleRangeItem::SetRangeStart(const COleDateTime& start)
{
    m_Start = start;
}
void ScheduleRangeItem::SetRangeEnd(const COleDateTime& end)
{
    m_End = end;
}

// this is merged by days
// if it->m_Start == 2011.1.13  iend == 2011.1.12
// This 2 range will be merged.
BOOL ScheduleRangeItem::TryMergeIfOverlap(const COleDateTime& start, const COleDateTime& end)
{
    COleDateTimeSpan OneDay(1, 0, 0, 0);
    if ( ! (((start - OneDay) > m_End) || ((end + OneDay) < m_Start)))
    {
        m_Start = min(start, m_Start);
        m_End = max(end, m_End);

        return TRUE;
    }

    return FALSE;
}

ScheduleRangeManager::ScheduleRangeManager()
{

}
ScheduleRangeManager::~ScheduleRangeManager()
{
    m_QueriedRangePool.clear();
    m_ModifiedRangePool.clear();
}

void ScheduleRangeManager::Clear()
{
    m_QueriedRangePool.clear();
    m_ModifiedRangePool.clear();
}

BOOL ScheduleRangeManager::AppendNewQueriedRange(const COleDateTime& start,const COleDateTime& end, std::vector<COleDateTime>& TimeStamps)
{
    COleDateTime istart(start.GetYear(), start.GetMonth(), start.GetDay(), 0,0,0);
    COleDateTime iend(end.GetYear(), end.GetMonth(), end.GetDay(), 0,0,0);

    if ((COleDateTimeSpan(iend - istart).GetDays() + 1) > (LONG)TimeStamps.size())
    {
        return FALSE;
    }

    BOOL bMerged = FALSE;
    for (std::vector<ScheduleRangeItem>::iterator it = m_QueriedRangePool.begin(); it != m_QueriedRangePool.end(); ++it)
    {
        // this is merged by days
        // if it->m_Start == 2011.1.13  iend == 2011.1.12
        // This 2 range will be merged.
        if ((*it).TryMergeIfOverlap(istart, iend))
        {
            bMerged = TRUE;
            break;
        }
    }

    if (!bMerged)
    {
        ScheduleRangeItem item;
        item.SetRangeStart(istart);
        item.SetRangeEnd(iend);

        m_QueriedRangePool.push_back(item);
    }

    COleDateTimeSpan OneDay = COleDateTimeSpan(1, 0, 0, 0);
    INT i = 0;
    for (COleDateTime it = istart; it <= iend; it += OneDay)
    {
        m_TimeStampPool[it] = TimeStamps[i];
        i++;
    }

    return TRUE;
}

COleDateTime ScheduleRangeManager::GetTimeStamp(COleDateTime Date)
{
    COleDateTime Key(Date.GetYear(), Date.GetMonth(), Date.GetDay(), 0,0,0);
    auto pos = m_TimeStampPool.find(Key);
    if (pos != m_TimeStampPool.end())
    {
        return pos->second;
    }

    return COleDateTime();
}

BOOL ScheduleRangeManager::AppendNewModifiedRange(const COleDateTime& start,const COleDateTime& end)
{
    COleDateTime istart(start.GetYear(), start.GetMonth(), start.GetDay(), 0,0,0);
    COleDateTime iend(end.GetYear(), end.GetMonth(), end.GetDay(), 0,0,0);

    BOOL bMerged = FALSE;
    for (std::vector<ScheduleRangeItem>::iterator it = m_ModifiedRangePool.begin(); it != m_ModifiedRangePool.end(); ++it)
    {
        if ((*it).TryMergeIfOverlap(istart, iend))
        {
            bMerged = TRUE;
            break;
        }
    }

    if (!bMerged)
    {
        ScheduleRangeItem item;
        item.SetRangeStart(istart);
        item.SetRangeEnd(iend);

        m_ModifiedRangePool.push_back(item);
    }

    return TRUE;
}

BOOL ScheduleRangeManager::IsQueriedRange(const COleDateTime& start,const COleDateTime& end)
{
    COleDateTime istart(start.GetYear(), start.GetMonth(), start.GetDay(), 0,0,0);
    COleDateTime iend(end.GetYear(), end.GetMonth(), end.GetDay(), 0,0,0);

    for (std::vector<ScheduleRangeItem>::iterator it = m_QueriedRangePool.begin(); it != m_QueriedRangePool.end(); ++it)
    {
        if ((istart >= it->GetRangeStart() && iend <= it->GetRangeEnd()))
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL ScheduleRangeManager::GetQueriedRange(const UINT Index, COleDateTime& start, COleDateTime& end)
{
    if (Index >= m_QueriedRangePool.size())
    {
        return FALSE;
    }

    start = m_QueriedRangePool[Index].GetRangeStart();
    end = m_QueriedRangePool[Index].GetRangeEnd();

    return TRUE;
}

INT  ScheduleRangeManager::GetQueriedRangeCount()
{
    return m_QueriedRangePool.size();
}

BOOL ScheduleRangeManager::GetModifiedRange(const UINT Index, COleDateTime& start, COleDateTime& end)
{
    if (Index >= m_ModifiedRangePool.size())
    {
        return FALSE;
    }
    
    start = m_ModifiedRangePool[Index].GetRangeStart();
    end = m_ModifiedRangePool[Index].GetRangeEnd();

    return TRUE;
}

INT  ScheduleRangeManager::GetModifiedRangeCount()
{
    return m_ModifiedRangePool.size();
}
