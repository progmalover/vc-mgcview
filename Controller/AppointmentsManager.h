#pragma once

#include "Appointment.h"

const int c_MAXSTACK = 2048;
const int c_HT_PREC  = 5;

inline
    int CompareAppointments (const Appointment* pApp1, const Appointment* pApp2)
{
    if (pApp1 == pApp2)
    {
        return 0;
    }

    if (pApp1->GetStart () < pApp2->GetStart ())
    {
        return -1;
    }
    else if (pApp2->GetStart () < pApp1->GetStart ())
    {
        return 1;
    }
    else
    {
        COleDateTimeSpan span1 (pApp1->GetDuration ());
        COleDateTimeSpan span2 (pApp2->GetDuration ());

        if (span1 < span2)
        {
            return 1;
        }
        else if (span2 < span1)
        {
            return -1;
        }
    }

    const CString& str1 = pApp1->GetDescription();
    const CString& str2 = pApp2->GetDescription();

    if (!str1.IsEmpty () && !str2.IsEmpty ())
    {
        if (_totupper(str1[0]) == _totupper(str2[0]))
        {
            return str1.Compare (str2);
        }
    }

    return str1.CompareNoCase (str2);
}


void SortAppointments (AppointmentArray& a, int size);
BOOL IsOneDay (const COleDateTime& date1, const COleDateTime& date2);

class BaseManager
{
public:
    BaseManager(void){}
    virtual ~BaseManager(void){}

    virtual BOOL Update (Appointment* pItem, const COleDateTime& dtOldStart, const COleDateTime& dtOldEnd, BOOL bForceAdd = FALSE) = 0;

};

class AppointmentsManager : public BaseManager
{
public:
    AppointmentsManager(void);
    virtual ~AppointmentsManager(void);

    BOOL IsAutoDelete () const
    {
        return m_bAutoDelete;
    }

    void SetAutoDelete (BOOL bDelete)
    {
        m_bAutoDelete = bDelete;
    }

    virtual BOOL Add (Appointment* pApp);
    virtual BOOL Update (Appointment* pApp, const COleDateTime& dtOldStart, const COleDateTime& dtOldEnd, BOOL bForceAdd = FALSE);
    virtual BOOL Remove (Appointment* pApp);
    virtual BOOL RemoveAll ();
    virtual void Query (AppointmentArray& ar, const COleDateTime& date1, const COleDateTime& date2);
    virtual void QueryAll (AppointmentArray& ar);
    virtual BOOL IsEmpty () const;

    typedef CMap<int, int, AppointmentList*, AppointmentList*> DayPool;
    typedef CMap<int, int, DayPool*, DayPool*> YearPool;

    YearPool& GetCollection ()
    {
        return m_Pool;
    }

    BOOL			 m_bAutoDelete;
    YearPool	     m_Pool;
    AppointmentArray m_PoolMulti;
};

