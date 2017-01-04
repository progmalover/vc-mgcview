#pragma once
#include "afx.h"

#include "ScheduleCtrlTypes.h"
class ScheduleView;
class AppointmentsManager;
class TimeTableView;
class BaseManager;

CString GetFormattedTimeString (const COleDateTime& time, BOOL dwFlags, BOOL bSystem);

class MultiDayDrawRects
{
public:
    MultiDayDrawRects(){};

    typedef struct MultiDayRect
    {
        CRect           rect;
        COleDateTime	date;
    };

    typedef CArray<MultiDayRect, MultiDayRect&> CRectArray;

    const CRect* Get(const COleDateTime& Day)  const;
    int GetRectCount() const;
    const CRect* Get(int Index)  const;
    void Add(CRect rect, const COleDateTime& Day);
    void Remove(const COleDateTime& Day);
    void RemoveAll();

    CRectArray m_Rects;
};

typedef struct AppointmentApperance
{
    CRect	        m_Rect;
    BOOL            m_Visible;
}AppointmentApperance;

typedef struct AppointmentPorperty
{
    COleDateTime	dtStart;
    COleDateTime	dtFinish;

    CString			strDescription;

    COLORREF		clrBackgroung;
    COLORREF		clrDuration;
    COLORREF		clrForeground;

    BOOL			bSelected;

    CRect			rectDraw;

    BOOL			bVisibleDraw;

}AppointmentPorperty;

class Appointment :
    public CObject
{
public:
    Appointment(void);
    virtual ~Appointment(void);

    DECLARE_SERIAL(Appointment)

    enum APPOINTMENT_HITTEST
    {
        APPOINTMENT_HITTEST_FIRST   = 0,
        APPOINTMENT_HITTEST_NOWHERE = APPOINTMENT_HITTEST_FIRST,
        APPOINTMENT_HITTEST_INSIDE  = 1, // appointment area
        APPOINTMENT_HITTEST_MOVE    = 2, // appointment move area (IN side)
        APPOINTMENT_HITTEST_TOP     = 3, // appointment top area (top side)
        APPOINTMENT_HITTEST_BOTTOM  = 4, // appointment bottom area (bottom side)
        APPOINTMENT_HITTEST_LEFT    = 5, // appointment left area (left side)
        APPOINTMENT_HITTEST_RIGHT   = 6, // appointment right area (right side)
        APPOINTMENT_HITTEST_LAST    = APPOINTMENT_HITTEST_RIGHT
    };

    enum APPOINTMENT_TYPE
    {
        APPOINTMENT_TYPE_LAYOUT     = 0,
        APPOINTMENT_TYPE_TIMETABLE
    };

    virtual void Serialize (CArchive& ar);

    void _UpdateTimeStrings ();

    virtual void OnDraw (CDC* pDC, ScheduleView* pView);
    virtual void OnDraw (CDC* pDC, ScheduleView* pView, const COleDateTime& date);

    virtual void OnDraw (CDC* pDC, TimeTableView* pView);

    const CRect& GetRectDraw (const COleDateTime& date = COleDateTime ()) const;
    void SetRectDraw (const CRect& rect, const COleDateTime& date = COleDateTime ());
    void ResetDraw ();

    BOOL PointInRectDraw (const CPoint& point) const;
    Appointment::APPOINTMENT_HITTEST HitTest (const CPoint& point) const;

    virtual void GetProperties (AppointmentPorperty& props) const{};
    virtual void SetProperties (const AppointmentPorperty& props){};


    void SetInterval (const COleDateTime& dtStart, const COleDateTime& dtFinish, BaseManager* pAM = NULL);
    void SetDescription (const CString& strText);
    void SetBackgroundColor (COLORREF clr);


    const COleDateTime& GetStart () const
    {
        return m_dtStart;
    }

    const COleDateTime& GetFinish () const
    {
        return m_dtFinish;
    }
	void SetStart(COleDateTime &start)
	{
		m_dtStart = start;
	}
	void SetEnd (COleDateTime &end) 
    {
         m_dtFinish = end;
    }

    COleDateTimeSpan GetDuration () const
    {
        COleDateTimeSpan span (m_dtFinish - m_dtStart);

        return span;
    }

    const CString& GetDescription () const
    {
        return m_strDescription;
    }

    COLORREF GetBackgroundColor () const
    {
        return m_clrBackgroung;
    }

    void SetImage(CImageList* pImageList, UINT index)
    {
        m_pImageList = reinterpret_cast<DWORD>(pImageList);
        m_ImageIndex = index;
    }

    CImageList* GetImageList() const
    {
        return reinterpret_cast<CImageList*>(m_pImageList);
    }

    UINT GetImageIndex() const
    {
        return m_ImageIndex;
    }

    void SetBitmap(CBitmap* pbitmap)
    {
        m_pBitmap = reinterpret_cast<DWORD>(pbitmap);
    }

    CBitmap* GetBitmap() const
    {
        return reinterpret_cast<CBitmap*>(m_pBitmap);
    }

    void SetDetailID(INT ID)
    {
        m_DetailID = ID;
    }

    void SetDetailType(APPOINTMENT_TYPE Type)
    {
        m_DetailType = Type;
    }

    INT GetDetailID() const {return m_DetailID;}

    APPOINTMENT_TYPE GetDetailType() const {return m_DetailType;}

    BOOL IsMultiDay () const
    {
        int d1 = m_dtStart.GetDay();
        int d2 = m_dtFinish.GetDay();
        int y1 = m_dtStart.GetYear();
        int y2 = m_dtFinish.GetYear();
        int m1 = m_dtStart.GetMonth();
        int m2 = m_dtFinish.GetMonth();

        return !((d1 == d2) && (y1 == y2) && (m1 == m2)) ;
    }

    BOOL IsSelected () const
    {
        return m_bSelected;
    }
    void SetSelected (BOOL selected)
    {
        m_bSelected = selected;
    }

    void operator=(const Appointment& Src);

public:
    CString			m_strFinish;
    CString			m_strStart;

private:

    COleDateTime	m_dtStart;
    COleDateTime	m_dtFinish;

    CString			m_strDescription;

    COLORREF		m_clrBackgroung;

    BOOL			m_bSelected;

    CRect			m_rectDraw;

    MultiDayDrawRects m_rectMultiDay;

    INT              m_DetailID;
    APPOINTMENT_TYPE m_DetailType;

    DWORD           m_pImageList;
    UINT            m_ImageIndex;

    DWORD           m_pBitmap;
};

typedef CArray<Appointment*, Appointment*> AppointmentArray;
typedef CList<Appointment*, Appointment*> AppointmentList;
