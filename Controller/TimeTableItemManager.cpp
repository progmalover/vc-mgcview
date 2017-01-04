#include "StdAfx.h"
#include "TimeTableItemManager.h"


TimeTableItemManager::TimeTableItemManager(void)
{
    m_bAutoDelete = TRUE;
}


TimeTableItemManager::~TimeTableItemManager(void)
{
    RemoveAll ();
}


void SortAppointmentList (AppointmentList& ItemList)
{
    AppointmentList TempList;

    POSITION pos = ItemList.GetHeadPosition ();
    while (pos != NULL)
    {
        POSITION posNext = pos;

        Appointment* pItemNext = TempList.GetNext (pos);

        TempList.AddTail(pItemNext);
    }

    ItemList.RemoveAll();
    COleDateTime TailTime = COleDateTime(1983,3,17,0,0,0);
    while (TempList.GetSize() > 0)
    {
        POSITION pos = TempList.GetHeadPosition ();
        POSITION MinPos = pos;
        COleDateTime MinTime = COleDateTime::GetCurrentTime();
        while (pos != NULL)
        {
            POSITION posNext = pos;

            Appointment* pItemNext = TempList.GetNext (pos);

            if (pItemNext->GetStart() < MinTime)
            {
                MinTime = pItemNext->GetStart();
                MinPos = posNext;
            }
        }

        Appointment* pItemMin = TempList.GetAt(MinPos);
        pItemMin->SetInterval(TailTime, TailTime + pItemMin->GetDuration());
        ItemList.AddTail(pItemMin);
        TempList.RemoveAt(MinPos);
        TailTime = pItemMin->GetFinish();
    }
}

BOOL TimeTableItemManager::Add (Appointment* pItem, const COleDateTime& dtTarget)
{
    ASSERT_VALID (pItem);

    if (pItem == NULL)
    {
        return FALSE;
    }

    if (pItem->GetDuration() < COleDateTimeSpan(0,0,0,10))
    {
        return FALSE;
    }

    if (GetTimeSpan() + pItem->GetDuration() > COleDateTimeSpan(0,24,00,00))
    {
        return FALSE;
    }

    COleDateTime myTarget(1983, 3, 17, dtTarget.GetHour(), dtTarget.GetMinute(), dtTarget.GetSecond());

    if (m_ItemList.IsEmpty())
    {
        pItem->SetInterval(COleDateTime(1983,3,17,0,0,0), COleDateTime(1983,3,17,0,0,0) + pItem->GetDuration());
        m_ItemList.AddHead(pItem);
        return TRUE;
    }

    BOOL bAdd = FALSE;
    COleDateTimeSpan PushSpan = pItem->GetDuration();

    POSITION pos = m_ItemList.GetHeadPosition ();
    Appointment* pItemNext;
    while (pos != NULL)
    {
        POSITION posNext = pos;

        pItemNext = m_ItemList.GetNext (pos);

        if (bAdd)
        {
            pItemNext->SetInterval(pItemNext->GetStart() + PushSpan, pItemNext->GetFinish() + PushSpan);

            continue;
        }

        if (myTarget + COleDateTimeSpan(0,0,0,1) < pItemNext->GetFinish() && myTarget >= pItemNext->GetStart())
        {
            bAdd = TRUE;
            COleDateTime midTime = pItemNext->GetStart();
            midTime += pItemNext->GetDuration()/2;
            if(myTarget > midTime)
            {
                pItem->SetInterval(pItemNext->GetFinish(), pItemNext->GetFinish() + pItem->GetDuration());
                m_ItemList.InsertAfter(posNext, pItem);
            }else
            {
                pItem->SetInterval(pItemNext->GetStart(), pItemNext->GetStart() + pItem->GetDuration());
                m_ItemList.InsertBefore(posNext, pItem);
                pItemNext->SetInterval(pItemNext->GetStart() + PushSpan, pItemNext->GetFinish() + PushSpan);
            }
        }
    }

    if (!bAdd)
    {
        pItem->SetInterval(pItemNext->GetFinish(), pItemNext->GetFinish() + pItem->GetDuration());
        m_ItemList.AddTail(pItem);
        bAdd = TRUE;
    }

    return bAdd;
}

BOOL TimeTableItemManager::Update (Appointment* pItem, 
    const COleDateTime& dtOldStart, const COleDateTime& dtOldEnd, BOOL bForceAdd/* = FALSE*/)
{
    ASSERT_VALID (pItem);

    if (pItem == NULL)
    {
        return FALSE;
    }

    if (pItem->GetDuration() < COleDateTimeSpan(0,0,0,10))
    {
        pItem->SetInterval(dtOldStart, dtOldEnd);
        return FALSE;
    }


	COleDateTimeSpan  span(GetTimeSpan().GetDays(),GetTimeSpan().GetHours(),
							GetTimeSpan().GetMinutes(),GetTimeSpan().GetSeconds());
    if (span > COleDateTimeSpan(0,23,59,59))
    {
		MessageBox(_T("Timetable duration could not be longer than 24h."), _T("Warning"), MB_OK|MB_ICONWARNING);
        pItem->SetInterval(dtOldStart, dtOldEnd);
        return FALSE;
    }
     
    BOOL bDelete = IsAutoDelete ();

    SetAutoDelete (FALSE);

    BOOL bUpdate = FALSE;
    COleDateTimeSpan PushSpan = dtOldEnd - dtOldStart;

    POSITION pos = m_ItemList.GetHeadPosition ();

    while (pos != NULL)
    {
        POSITION posNext = pos;

        Appointment* pItemNext = m_ItemList.GetNext (pos);

        if (bUpdate)
        {
            //pItemNext->SetInterval(pItemNext->GetStart() + PushSpan, pItemNext->GetFinish() + PushSpan);

            continue;
        }

        if (pItem == pItemNext)
        {
            bUpdate = TRUE;

            SortAppointmentList(m_ItemList);

            break;
        }
    }

    if (bForceAdd && !bUpdate)
    {
        Add(pItem, dtOldStart + COleDateTimeSpan(0,0,0,1) /*pItem->GetStart()*/);
    }

    SetAutoDelete (bDelete);

    return bUpdate;
}

BOOL TimeTableItemManager::Remove (Appointment* pItem)
{
    ASSERT_VALID (pItem);

    if (pItem == NULL)
    {
        return FALSE;
    }

    BOOL bDelete = FALSE;
    COleDateTimeSpan PushSpan = pItem->GetDuration();

    POSITION pos = m_ItemList.GetHeadPosition ();

    while (pos != NULL)
    {
        POSITION posNext = pos;

        Appointment* pItemNext = m_ItemList.GetNext (pos);

        if (bDelete)
        {
            if ((pItemNext->GetStart() - PushSpan).GetDayOfYear() != (pItemNext->GetFinish() - PushSpan).GetDayOfYear())
            {
                PushSpan = pItemNext->GetStart();
            }
            pItemNext->SetInterval(pItemNext->GetStart() - PushSpan, pItemNext->GetFinish() - PushSpan);

            continue;
        }

        if (pItem == pItemNext)
        {
            m_ItemList.RemoveAt(posNext);

            bDelete = TRUE;
        }
    }

    if (IsAutoDelete ())
    {
        delete pItem;
    }

    return bDelete;
}


BOOL TimeTableItemManager::RemoveAll ()
{
    if (IsAutoDelete ())
    {
        POSITION pos = m_ItemList.GetHeadPosition ();

        while (pos != NULL)
        {
            POSITION posNext = pos;

            Appointment* pItemNext = m_ItemList.GetNext (pos);

            delete pItemNext;
        }
    }

    m_ItemList.RemoveAll();

    return TRUE;
}

BOOL TimeTableItemManager::IsEmpty () const
{
    return m_ItemList.IsEmpty ();
}

COleDateTimeSpan TimeTableItemManager::GetTimeSpan() const
{
    COleDateTimeSpan Span(0,0,0,0);

    if (m_ItemList.GetSize() == 0)
    {
        return Span;
    }

    POSITION pos = m_ItemList.GetHeadPosition ();

    while (pos != NULL)
    {
        POSITION posNext = pos;

        Appointment* pItemNext = m_ItemList.GetNext (pos);

        Span += pItemNext->GetDuration();
    }
    return Span;
}