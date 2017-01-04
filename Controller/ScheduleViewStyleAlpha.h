#pragma once
#include "scheduleview.h"
class ScheduleViewStyleAlpha :
    public ScheduleView
{
    DECLARE_DYNAMIC(ScheduleViewStyleAlpha)

public:
    ScheduleViewStyleAlpha(void);
    virtual ~ScheduleViewStyleAlpha(void);

    virtual void OnPaint (CDC* pDC, const CRect& rectClient);
    virtual void OnDrawBackgound (CDC* /*pDC*/, const CRect& /*rect*/);
    virtual void OnDrawHeader (CDC* pDC, const CRect& rectHeader);
    virtual void OnDrawTimeLine (CDC* pDC, const CRect& rectTime);
    virtual void OnDrawTimeBar (CDC* pDC, const CRect& rectBar, BOOL bDrawTimeLine);
    virtual void DrawAppointment (CDC* pDC, Appointment* pApp, AppointmentApperance* pDS, BOOL bClipClient = TRUE);
    virtual void DrawSelection (CDC* pDC);
    virtual void DrawOperationLayer (CDC* pDC, OperationLayer* pApp, OperationLayerApperance* pDS);
    virtual void DrawDefaultAppointment (CDC* pDC);

    virtual void ScrollToDate(COleDateTime Date);

    _SVIEW_TIME_DELTA GetTimeDelta () const;
    void AdjustRects ();
    void AdjustLayout (CDC* /*pDC*/, const CRect& rectClient);
    void AdjustAppointments ();
    void AdjustAppointmentRectByDateTime (Appointment* pApp, CRect& rect, const COleDateTime& date);
    void RecauclateEnableVisibleOffset();
    virtual BOOL OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

    virtual void AdjustScrollSizes ();

    virtual COleDateTime GetDateFromPoint (const CPoint& point);

    virtual BOOL IsPtInDefaultArea(const CPoint& point) const;
    virtual BOOL IsPtInToday(const CPoint& point) const;
	int GetHeaderCaptionHeight(){return m_nHeaderDayCaptionHeight;}
protected:
    CRect				m_rectTimeBar;
    int                 m_nHeaderTodayHeight;
    int					m_nHeaderDayCaptionHeight;

    COleDateTime		m_CurrentTime;
    UINT_PTR			m_Timer;
	float				m_visualRate;
    CArray<BOOL, BOOL>	m_SelectionAllDay;
};

