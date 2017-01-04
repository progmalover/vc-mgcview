#include "StdAfx.h"
#include "AppointmentsManager.h"


AppointmentsManager::AppointmentsManager(void)
{
    m_bAutoDelete = TRUE;
}


AppointmentsManager::~AppointmentsManager(void)
{
    RemoveAll ();
}

BOOL IsOneDay (const COleDateTime& date1, const COleDateTime& date2)
{
    return date1.GetYear () == date2.GetYear () &&
        date1.GetDayOfYear () == date2.GetDayOfYear ();
}

void SortAppointments (AppointmentArray& a, int size)
{
    int i, j;
    int lb, ub;
    int lbstack[c_MAXSTACK], ubstack[c_MAXSTACK];

    int stackpos = 1;
    int ppos;
    Appointment* pivot = NULL;

    lbstack[1] = 0;
    ubstack[1] = size-1;

    do
    {
        lb = lbstack[stackpos];
        ub = ubstack[stackpos];
        stackpos--;

        do
        {
            ppos = (lb + ub) >> 1;
            i = lb;
            j = ub;
            pivot = a[ppos];

            do
            {
                while (CompareAppointments (a[i], pivot) == -1) i++;
                while (CompareAppointments (pivot, a[j]) == -1) j--;

                if (i <= j)
                {
                    if (i < j)
                    {
                        Appointment* temp = a[i]; 
                        a[i] = a[j];
                        a[j] = temp;
                    }

                    i++;
                    j--;
                }
            }
            while (i <= j);

            if (i < ppos)
            {
                if (i < ub)
                {
                    stackpos++;
                    lbstack[ stackpos ] = i;
                    ubstack[ stackpos ] = ub;
                }

                ub = j;
            }
            else
            {
                if (j > lb)
                { 
                    stackpos++;
                    lbstack[ stackpos ] = lb;
                    ubstack[ stackpos ] = j;
                }

                lb = i;
            }

        }
        while (lb < ub);

    }
    while ( stackpos != 0 );
}


BOOL AppointmentsManager::Add (Appointment* pApp)
{
    ASSERT_VALID (pApp);

    if (pApp == NULL)
    {
        return FALSE;
    }

    COleDateTime dtS (pApp->GetStart ());
    COleDateTime dtF (pApp->GetFinish ());

    int nYear = dtS.GetYear ();
    int nDay  = dtS.GetDayOfYear ();

    DayPool* yearVal = NULL;
    m_Pool.Lookup (nYear, yearVal);

    if (yearVal == NULL)
    {
        yearVal = new DayPool;
        m_Pool.SetAt (nYear, yearVal);
    }

    AppointmentList* dayVal = NULL;
    yearVal->Lookup (nDay, dayVal);

    if (dayVal == NULL)
    {
        dayVal = new AppointmentList;
        yearVal->SetAt (nDay, dayVal);
    }

    BOOL bAdd = FALSE;

    POSITION pos = dayVal->GetTailPosition ();

    while (pos != NULL)
    {
        POSITION posPrev = pos;

        Appointment* pAppPrev = dayVal->GetPrev (pos);

        COleDateTime dtPrevS (pAppPrev->GetStart ());
        COleDateTime dtPrevF (pAppPrev->GetFinish ());

        if (dtS.GetHour () == dtPrevS.GetHour () && dtS.GetMinute () == dtPrevS.GetMinute () &&
            dtF.GetHour () == dtPrevF.GetHour () && dtF.GetMinute () == dtPrevF.GetMinute ())
        {
            dayVal->InsertAfter (posPrev, pApp);
            bAdd = TRUE;
            break;
        }
    }

    if (!bAdd)
    {
        pos = dayVal->GetHeadPosition ();

        while (pos != NULL)
        {
            POSITION posPrev = pos;

            Appointment* pAppNext = dayVal->GetNext (pos);

            COleDateTime dtNextS (pAppNext->GetStart ());

            if (dtS.GetHour () < dtNextS.GetHour () ||
                (dtS.GetHour () == dtNextS.GetHour () && 
                dtS.GetMinute () < dtNextS.GetMinute ())
                )
            {
                dayVal->InsertBefore (posPrev, pApp);
                bAdd = TRUE;
                break;
            }
        }
    }

    if (!bAdd)
    {
        dayVal->AddTail (pApp);
    }

    if (!IsOneDay (pApp->GetStart (), pApp->GetFinish ()))
    {
        m_PoolMulti.Add (pApp);
        SortAppointments (m_PoolMulti, (int) m_PoolMulti.GetSize ());
    }

    return TRUE;
}

BOOL AppointmentsManager::Update (Appointment* pApp, const COleDateTime& dtOldStart, 
    const COleDateTime& dtOldEnd, BOOL bForceAdd)
{
    ASSERT_VALID (pApp);

    const COleDateTime& dtOld = dtOldStart;

    if (pApp == NULL)
    {
        return FALSE;
    }

    BOOL bDelete = IsAutoDelete ();

    SetAutoDelete (FALSE);

    COleDateTime dtS (pApp->GetStart ());
    COleDateTime dtF (pApp->GetFinish ());

    if (dtOld != COleDateTime ())
    {
        pApp->SetInterval (dtOld, dtOld);
    }

    BOOL bFound = Remove (pApp);
    if (!bForceAdd && !bFound)
    {
        SetAutoDelete (bDelete);
        return FALSE;
    }

    pApp->SetInterval (dtS, dtF);

    Add (pApp);
    SetAutoDelete (bDelete);

    return TRUE;
}

BOOL AppointmentsManager::Remove (Appointment* pApp)
{
    ASSERT_VALID (pApp);

    if (pApp == NULL)
    {
        return FALSE;
    }

    COleDateTime dt (pApp->GetStart ());

    int nYear = dt.GetYear ();
    int nDay = dt.GetDayOfYear ();

    DayPool* yearVal = NULL;
    m_Pool.Lookup (nYear, yearVal);

    if (yearVal == NULL)
    {
        return FALSE;
    }

    AppointmentList* dayVal = NULL;
    yearVal->Lookup (nDay, dayVal);

    if (dayVal == NULL)
    {
        return FALSE;
    }

    POSITION pos = dayVal->Find (pApp);

    if (pos == NULL)
    {
        return FALSE;
    }

    dayVal->RemoveAt (pos);

    if (dayVal->GetCount () == 0)
    {
        delete dayVal;
        yearVal->RemoveKey (nDay);
    }

    if (yearVal->GetCount () == 0)
    {
        delete yearVal;
        m_Pool.RemoveKey (nYear);
    }

    for (int i = 0; i < m_PoolMulti.GetSize (); i++)
    {
        if (m_PoolMulti [i] == pApp)
        {
            m_PoolMulti.RemoveAt (i);
            break;
        }
    }

    if (IsAutoDelete ())
    {
        delete pApp;
    }

    return TRUE;
}


BOOL AppointmentsManager::RemoveAll ()
{

    POSITION yearPos = m_Pool.GetStartPosition ();
    int yearKey = 0;
    DayPool* yearVal = NULL;

    while (yearPos != NULL)
    {
        m_Pool.GetNextAssoc (yearPos, yearKey, yearVal);

        if (yearVal != NULL)
        {
            POSITION dayPos = yearVal->GetStartPosition ();
            int dayKey = 0;
            AppointmentList* dayVal = NULL;

            while (dayPos != NULL)
            {
                yearVal->GetNextAssoc (dayPos, dayKey, dayVal);

                if (dayVal != NULL)
                {
                    if (IsAutoDelete ())
                    {
                        POSITION pos = dayVal->GetHeadPosition ();

                        while (pos != NULL)
                        {
                            Appointment* pApp = dayVal->GetNext (pos);

                            if (pApp != NULL)
                            {
                                delete pApp;
                            }
                        }
                    }

                    dayVal->RemoveAll ();
                    delete dayVal;
                }
            }


            yearVal->RemoveAll ();
            delete yearVal;
        }
    }

    m_Pool.RemoveAll ();
    m_PoolMulti.RemoveAll ();

    return TRUE;
}

void AppointmentsManager::Query (AppointmentArray& ar, const COleDateTime& date1, const COleDateTime& date2)
{
    ar.RemoveAll ();

    int nCount = (int)(date2 - date1).GetTotalDays () + 1;

    COleDateTime dt (date1);
    COleDateTimeSpan span (1, 0, 0, 0);

    int i = 0;

    for (i = 0; i < nCount; i++)
    {
        int nYear = dt.GetYear ();
        int nDay = dt.GetDayOfYear ();

        DayPool* yearVal = NULL;
        m_Pool.Lookup (nYear, yearVal);

        if (yearVal != NULL)
        {
            AppointmentList* dayVal = NULL;
            yearVal->Lookup (nDay, dayVal);

            if (dayVal != NULL)
            {
                POSITION pos = dayVal->GetHeadPosition ();

                while (pos != NULL)
                {
                    Appointment* pApp = dayVal->GetNext (pos);

                    if (pApp != NULL)
                    {
                        ar.Add (pApp);
                    }
                }
            }
        }

        dt += span;
    }

    for (i = 0; i < m_PoolMulti.GetSize (); i++)
    {
        Appointment* pApp = m_PoolMulti [i];
        if (pApp->GetStart () < date1)
        {
            if ((date1 <= pApp->GetFinish () && pApp->GetFinish () <= date2) ||
                date2 < pApp->GetFinish ())
            {
                if (pApp->GetFinish () != COleDateTime (date1.GetYear (), 
                    date1.GetMonth (), date1.GetDay (), 0, 0, 0))
                {
                    ar.Add (pApp);
                }
            }
        }
        else
        {
            break;
        }
    }

    if (ar.GetSize () > 0)
    {
        SortAppointments (ar, (int) ar.GetSize ());
    }
}

void AppointmentsManager::QueryAll (AppointmentArray& ar)
{
    ar.RemoveAll();

    for (POSITION posYear = m_Pool.GetStartPosition (); posYear != NULL;)
    {
        int nYear = -1;
        DayPool* yearVal = NULL;

        m_Pool.GetNextAssoc (posYear, nYear, yearVal);

        if (yearVal != NULL)
        {
            for (POSITION posDay = yearVal->GetStartPosition (); posDay != NULL;)
            {
                int nDay = -1;
                AppointmentList* dayVal = NULL;

                yearVal->GetNextAssoc (posDay, nDay, dayVal);

                if (dayVal != NULL)
                {
                    for (POSITION pos = dayVal->GetHeadPosition (); pos != NULL;)
                    {
                        Appointment* pApp = dayVal->GetNext (pos);
                        if (pApp != NULL)
                        {
                            ASSERT_VALID (pApp);
                            ar.Add (pApp);
                        }
                    }
                }
            }
        }
    }

}

BOOL AppointmentsManager::IsEmpty () const
{
    return m_Pool.IsEmpty ();
}
