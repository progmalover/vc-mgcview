#pragma once
                             
#include "Appointment.h"
#include "AppointmentsManager.h"

class TimeTableItemManager : public BaseManager
{
public:
    TimeTableItemManager(void);
    virtual ~TimeTableItemManager(void);

    BOOL IsAutoDelete () const
    {
        return m_bAutoDelete;
    }

    void SetAutoDelete (BOOL bDelete)
    {
        m_bAutoDelete = bDelete;
    }

    virtual BOOL Add (Appointment* pItem, const COleDateTime& dtTarget);
    virtual BOOL Update (Appointment* pItem, const COleDateTime& dtOldStart, const COleDateTime& dtOldEnd, BOOL bForceAdd = FALSE);
    virtual BOOL Remove (Appointment* pItem);
    virtual BOOL RemoveAll ();
    virtual BOOL IsEmpty () const;
    COleDateTimeSpan GetTimeSpan() const;

    BOOL			    m_bAutoDelete;
    AppointmentList   m_ItemList;
};

extern void SortAppointmentList (AppointmentList& ItemList);