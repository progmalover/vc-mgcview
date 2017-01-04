#include "afxwin.h"

class ScheduleRangeItem
{
public:
    ScheduleRangeItem();
    ~ScheduleRangeItem();

    COleDateTime& GetRangeStart();
    COleDateTime& GetRangeEnd();

    void SetRangeStart(const COleDateTime& start);
    void SetRangeEnd(const COleDateTime& end);

    // this is merged by days
    // if it->m_Start == 2011.1.13  iend == 2011.1.12
    // This 2 range will be merged.
    BOOL TryMergeIfOverlap(const COleDateTime& start, const COleDateTime& end);

private:
    COleDateTime m_Start;
    COleDateTime m_End;
};

class ScheduleRangeManager
{
public:
    ScheduleRangeManager();
    ~ScheduleRangeManager();

    void Clear();

    BOOL AppendNewQueriedRange(const COleDateTime& start,const COleDateTime& end, std::vector<COleDateTime>& TimeStamps);
    BOOL AppendNewModifiedRange(const COleDateTime& start,const COleDateTime& end);
    COleDateTime GetTimeStamp(COleDateTime Date);

    BOOL IsQueriedRange(const COleDateTime& start,const COleDateTime& end);

    BOOL GetQueriedRange(const UINT Index, COleDateTime& start, COleDateTime& end);
    INT  GetQueriedRangeCount();
    BOOL GetModifiedRange(const UINT Index, COleDateTime& start, COleDateTime& end);
    INT  GetModifiedRangeCount();

private:
    std::vector<ScheduleRangeItem>  m_QueriedRangePool;
    std::vector<ScheduleRangeItem>  m_ModifiedRangePool;


    std::map<COleDateTime, COleDateTime> m_TimeStampPool;
};