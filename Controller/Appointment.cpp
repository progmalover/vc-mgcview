#include "StdAfx.h"
#include "Appointment.h"
#include "ScheduleView.h"
#include "AppointmentsManager.h"
#include "TimeTableView.h"

IMPLEMENT_SERIAL(Appointment, CObject, 0/*VERSIONABLE_SCHEMA | 1*/)

const CRect* MultiDayDrawRects::Get(const COleDateTime& Day)  const
{
    COleDateTime InternalDay = COleDateTime(Day.GetYear(), Day.GetMonth(), Day.GetDay(),0,0,0);

    for (int i = 0; i < m_Rects.GetSize (); i++)
    {
        if (m_Rects[i].date == InternalDay)
        {
            return &m_Rects[i].rect;
        }
    }

    return NULL;
}

int MultiDayDrawRects::GetRectCount() const
{
    return m_Rects.GetSize ();
}

const CRect* MultiDayDrawRects::Get(int Index)  const
{
    if (Index < m_Rects.GetSize ())
    {
        return &m_Rects[Index].rect;
    }

    return NULL;
}

void MultiDayDrawRects::Add(CRect rect, const COleDateTime& Day)
{
    Remove(Day);

    COleDateTime InternalDay = COleDateTime(Day.GetYear(), Day.GetMonth(), Day.GetDay(),0,0,0);

    MultiDayRect MyRect;
    MyRect.rect = rect;
    MyRect.date = InternalDay;

    m_Rects.Add(MyRect);
}
void MultiDayDrawRects::Remove(const COleDateTime& Day)
{
    COleDateTime InternalDay = COleDateTime(Day.GetYear(), Day.GetMonth(), Day.GetDay(),0,0,0);

    for (int i = 0; i < m_Rects.GetSize (); i++)
    {
        if (m_Rects[i].date == InternalDay)
        {
            m_Rects.RemoveAt(i, 1);
            break;
        }
    }
}
void MultiDayDrawRects::RemoveAll()
{
    m_Rects.RemoveAll();
}


Appointment::Appointment(void)
: m_strFinish (_T(""))
, m_strStart (_T(""))
, m_dtStart ()
, m_dtFinish ()
, m_strDescription  (_T(""))
, m_clrBackgroung (RGB(0,0,0))
, m_bSelected (FALSE)
, m_rectDraw ()
, m_rectMultiDay ()
, m_DetailID (-1)
, m_pImageList (NULL)
, m_ImageIndex (0)
, m_pBitmap (NULL)
, m_DetailType(APPOINTMENT_TYPE_LAYOUT)
{

}


Appointment::~Appointment(void)
{
}

// TIME_NOMINUTESORSECONDS	- Does not use minutes or seconds. 
// TIME_NOSECONDS			- Does not use seconds. 
// TIME_NOTIMEMARKER		- Does not use a time marker. 
// TIME_FORCE24HOURFORMAT	- Always uses a 24-hour time format.
CString GetFormattedTimeString (const COleDateTime& time, BOOL dwFlags, BOOL bSystem)
{
    CString strFormat;

    if (!bSystem)
    {
        const int c_Size = 10;
        TCHAR szLocaleData[c_Size];

        BOOL b24Hours = (dwFlags & TIME_FORCE24HOURFORMAT) != NULL;

        if (!b24Hours)
        {
            // Time format specifier.
            ::GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_ITIME, szLocaleData, c_Size);

            switch (szLocaleData [0])
            {
            case '0':	// AM / PM 12-hour format.
                b24Hours = FALSE;
                break;
            case '1':	// 24-hour format.
                b24Hours = TRUE;
                break;
            }
        }

        strFormat = b24Hours ? _T("H") : _T("h");

        // Specifier for leading zeros in time fields.
        ::GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_ITLZERO, szLocaleData, c_Size);
        switch (szLocaleData [0])
        {
        case '0':	// No leading zeros for hours.
            break;
        case '1':	// Leading zeros for hours.
            strFormat += strFormat[0];
            break;
        }

        CString strTemp;

        // Character(s) for the time separator.
        ::GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_STIME, szLocaleData, c_Size);
        strTemp = szLocaleData;
        strTemp = _T("'") + strTemp + _T("'");

        if ((dwFlags & TIME_NOMINUTESORSECONDS) == 0)
        {
            strFormat += strTemp + _T("mm");

            if ((dwFlags & TIME_NOSECONDS) == 0)
            {
                strFormat += strTemp + _T("ss");
            }
        }

        if (!b24Hours || (dwFlags & TIME_NOTIMEMARKER) != 0)
        {
            szLocaleData[0] = (TCHAR)0;

            if (time.GetHour () < 12)
            {
                // String for the AM designator.
                ::GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_S1159, szLocaleData, c_Size);
            }
            else
            {
                // String for the PM designator.
                ::GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_S2359, szLocaleData, c_Size);
            }

            strTemp = szLocaleData;
            strTemp = _T("'") + strTemp + _T("'");

            if (!strTemp.IsEmpty ())
            {
                // Specifier indicating whether the time marker string (AM or PM) precedes or follows the time string.
                ::GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_ITIMEMARKPOSN, szLocaleData, c_Size);
                switch (szLocaleData [0])
                {
                case '0':	// Use as suffix.
                    strFormat += strTemp;
                    break;
                case '1':	// Use as prefix.
                    strFormat = strTemp + strFormat;
                    break;
                }
            }
        }
    }

    CString strResult;

    SYSTEMTIME st;
    time.GetAsSystemTime (st);

    ::GetTimeFormat (LOCALE_USER_DEFAULT, dwFlags, &st, bSystem ? NULL : (LPCTSTR)strFormat, 
        strResult.GetBuffer (_MAX_PATH), _MAX_PATH);

    strResult.ReleaseBuffer ();

    return strResult;
}

void Appointment::_UpdateTimeStrings ()
{
    m_strStart.Empty ();
    m_strFinish.Empty ();

    m_strStart  = GetFormattedTimeString (m_dtStart, 0/*TIME_NOSECONDS*/, FALSE);
    m_strStart.MakeLower ();
    m_strFinish = GetFormattedTimeString (m_dtFinish, 0/*TIME_NOSECONDS*/, FALSE);
    m_strFinish.MakeLower ();
}

void Appointment::SetInterval (const COleDateTime& dtStart, const COleDateTime& dtFinish, BaseManager* pAM)
{
    ASSERT(dtStart <= dtFinish);

    COleDateTime OldDateStart = m_dtStart;
    COleDateTime OldDateEnd = m_dtFinish;

    if (m_dtStart != dtStart || m_dtFinish != dtFinish)
    {
        if (m_dtStart != dtStart)
        {
            m_dtStart = dtStart;
        }

        if (m_dtFinish != dtFinish)
        {
            m_dtFinish = dtFinish;
        }

        m_rectMultiDay.RemoveAll();

        _UpdateTimeStrings();
    }

    if (pAM)
    {
        pAM->Update(this, OldDateStart, OldDateEnd);
    }

    ASSERT(m_dtStart.GetStatus () == COleDateTime::valid);
    ASSERT(m_dtFinish.GetStatus () == COleDateTime::valid);
}

void Appointment::SetDescription (const CString& strText)
{
    if (m_strDescription != strText)
    {
        m_strDescription = strText;
    }
}

void Appointment::SetBackgroundColor (COLORREF clr)
{
    if (m_clrBackgroung != clr)
    {
        m_clrBackgroung = clr;
    }
}

void Appointment::OnDraw (CDC* pDC, ScheduleView* pView)
{
    ASSERT_VALID (pDC);
    //ASSERT_VALID (pView);

    if (pDC == NULL || pDC->GetSafeHdc () == NULL || pView == NULL)
    {
        return;
    }

}

void Appointment::OnDraw (CDC* pDC, ScheduleView* pView, const COleDateTime& date)
{
    ASSERT_VALID (pDC);
    //ASSERT_VALID (pView);

    if (pDC == NULL || pDC->GetSafeHdc () == NULL || pView == NULL)
    {
        return;
    }

    COleDateTime Thisday = COleDateTime(date.GetYear(),date.GetMonth(), date.GetDay(),0,0,0);
    COleDateTime Nextday = Thisday + COleDateTimeSpan(1,0,0,0);
    if ((GetStart() <= Thisday && GetFinish() >= Thisday) ||
        (GetStart() <= Nextday && GetFinish() >= Nextday) ||
        (GetStart() >= Thisday && GetFinish() <= Nextday)
        )
    {
        AppointmentApperance AA;

        AA.m_Rect = GetRectDraw(date);

        pView->DrawAppointment (pDC, this, &AA);
    }
}


void Appointment::OnDraw (CDC* pDC, TimeTableView* pView)
{
    ASSERT_VALID (pDC);
    //ASSERT_VALID (pView);

    if (pDC == NULL || pDC->GetSafeHdc () == NULL || pView == NULL)
    {
        return;
    }

    pView->DrawAppointment (pDC, this);

}

const CRect& Appointment::GetRectDraw (const COleDateTime& date /*= COleDateTime ()*/) const
{
    if (IsMultiDay ())
    {
        const CRect* pRect = m_rectMultiDay.Get(date);

        if (NULL != pRect)
        {
            return *pRect;
        }

        return m_rectDraw;
    }
    else
    {
        return m_rectDraw;
    }
}

void Appointment::SetRectDraw (const CRect& rect, const COleDateTime& date /*= COleDateTime ()*/)
{
    if (IsMultiDay ())
    {
        m_rectMultiDay.Add(rect, date);
    }
    else
    {
        m_rectDraw = rect;

        m_rectMultiDay.RemoveAll();
    }
}

void Appointment::ResetDraw ()
{
    m_rectDraw.SetRectEmpty ();
    m_rectMultiDay.RemoveAll();
}

BOOL Appointment::PointInRectDraw (const CPoint& point) const
{
    BOOL bHit = FALSE;

    if (IsMultiDay ())
    {
        int RectCount = m_rectMultiDay.GetRectCount();
        for (int i = 0; i < RectCount; i++)
        {
            const CRect* pRect = m_rectMultiDay.Get(i);
            if (pRect->PtInRect (point))
            {
                bHit = TRUE;
                break;
            }
        }
    }
    else
    {
        bHit = m_rectDraw.PtInRect (point);
    }

    return bHit;
}

Appointment::APPOINTMENT_HITTEST Appointment::HitTest (const CPoint& point) const
{
    APPOINTMENT_HITTEST hit = APPOINTMENT_HITTEST_NOWHERE;

    BOOL bHit = FALSE;
    CRect rt;

    if (IsMultiDay ())
    {
        int RectCount = m_rectMultiDay.GetRectCount();
        for (int i = 0; i < RectCount; i++)
        {
            const CRect* pRect = m_rectMultiDay.Get(i);
            if (pRect->PtInRect (point))
            {
                bHit = TRUE;

                rt = *pRect;

                break;
            }
        }
    }
    else
    {
        bHit = m_rectDraw.PtInRect (point);

        rt = m_rectDraw;
    }

    if(bHit)
    {
        hit = APPOINTMENT_HITTEST_INSIDE;

        if (point.y <= rt.bottom && (rt.bottom - c_HT_PREC) <= point.y)
        {
            hit = APPOINTMENT_HITTEST_BOTTOM;
        }
        else if (rt.top <= point.y && point.y <= (rt.top + c_HT_PREC))
        {
            hit = APPOINTMENT_HITTEST_TOP;
        }

        if (hit == APPOINTMENT_HITTEST_INSIDE)
        {
            hit = APPOINTMENT_HITTEST_MOVE;
        }
    }

    return hit;
}

void Appointment::operator=(const Appointment& Src)
{
    m_strFinish = Src.m_strFinish;
    m_strStart = Src.m_strStart;

    m_dtStart = Src.GetStart();
    m_dtFinish = Src.GetFinish();
    m_strDescription = Src.GetDescription();
    m_clrBackgroung = Src.GetBackgroundColor();
    m_bSelected = FALSE;
    m_pImageList = reinterpret_cast<DWORD>(Src.GetImageList());
    m_ImageIndex = Src.GetImageIndex();
    m_pBitmap = reinterpret_cast<DWORD>(Src.GetBitmap());
    m_DetailID = Src.GetDetailID();
	m_DetailType = Src.GetDetailType();
    //m_rectDraw = ;
    //m_rectMultiDay = ;
}

void Appointment::Serialize (CArchive& ar)
{
    CObject::Serialize (ar);

    if (ar.IsStoring ())
    {
        ar << m_dtStart;
        ar << m_dtFinish;
        ar << m_strDescription;
        ar << m_clrBackgroung;
        ar << m_pImageList;
        ar << m_ImageIndex;
        ar << m_pBitmap;
        ar << m_DetailID;
		ar << (int)m_DetailType;
	}
    else
    {
		int type;
        ar >> m_dtStart;
        ar >> m_dtFinish;
        ar >> m_strDescription;
        ar >> m_clrBackgroung;
        ar >> m_pImageList;
        ar >> m_ImageIndex;
        ar >> m_pBitmap;
        ar >> m_DetailID;
		ar >> type;
		m_DetailType = (APPOINTMENT_TYPE)type;
    }
}