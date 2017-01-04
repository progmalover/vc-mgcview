#pragma once

#include "ScheduleCtrlTypes.h"
#include "Appointment.h"
#include "OperationLayer.h"
#include "ScheduleCtrl.h"

class ScheduleCtrl;
#define MIN_ROW_HEIGHT  20
class ScheduleView : public CObject
{
    DECLARE_DYNAMIC(ScheduleView)
    friend class ScheduleCtrl;

public:
    ScheduleView(void);
    virtual ~ScheduleView(void);

    virtual void OnActivate(ScheduleCtrl* pScheduler, const ScheduleView* pOldView);
    virtual void OnDeactivate(ScheduleCtrl* pScheduler);
public:

    virtual void OnPaint (CDC* /*pDC*/, const CRect& /*rectClient*/) = 0;
    virtual void OnDrawBackgound (CDC* /*pDC*/, const CRect& /*rect*/) = 0;

    // 0 - all appointments in view; 
    // 1 - upper appointments not in view; 
    // 2 - lower appointments not in view
    // 3 - upper and lower not in view
    // if date = COleDateTime () - draws all appointments 
    //    in this case rect ignored, return value - always 0
    virtual BYTE OnDrawAppointments (CDC* pDC, const CRect& rect = CRect (0, 0, 0, 0), 
        const COleDateTime& date = COleDateTime ());

    virtual void DrawAppointment (CDC* pDC, Appointment* pApp, AppointmentApperance* pDS, BOOL bClipClient = TRUE){}

    virtual void DrawOperationLayer (CDC* pDC, OperationLayer* pApp, OperationLayerApperance* pDS){}

    virtual void DrawHeader (CDC* pDC, const CRect& rect, int dxColumn){}

    //Hit tests
    virtual _SVIEW_HITTEST HitTest (const CPoint& point) const;
    virtual _SVIEW_HITTEST HitTestArea (const CPoint& point) const;
    virtual _SVIEW_HITTEST HitTestOperationLayer (const CPoint& point) const;
    virtual _SVIEW_HITTEST HitTestAppointment (const CPoint& point) const;


    //Handle mouse keyboard message
    virtual BOOL OnLButtonDown(UINT nFlags, CPoint point);
    virtual BOOL OnLButtonUp(UINT nFlags, CPoint point);
    virtual BOOL OnMouseMove(UINT nFlags, CPoint point);
    virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    virtual BOOL OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    virtual BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) = 0;

    virtual void StartSelect ();
    virtual void StopSelect ();
    virtual void StartRelocate ();
    virtual void StopRelocate ();
    virtual void UpdateOpMode(OPRATION_MODE mode){m_OpMode = mode;}

    virtual void ScrollToDate(COleDateTime Date){}

public:
    Appointment* GetAppointmentFromPoint (const CPoint& point);
	void SelectAppointmetRange(Appointment *pStartApp ,Appointment *pEndApp);
    virtual COleDateTime GetDateFromPoint (const CPoint& point);
    virtual BOOL IsPtInDefaultArea(const CPoint& point) const{return FALSE;}
    virtual BOOL IsPtInToday(const CPoint& point) const{return FALSE;}
	virtual BOOL ResetCenterPointTime(int nDetal = 0);
	virtual void RecauclateEnableVisibleOffset(){};
    inline OPRATION_MODE GetOPMode(){return m_OpMode;}
    void SetDefaultAppointment(Appointment* pApp){m_pDefaultAppointment = pApp;}
	void ReArrangeAppointments(void);
protected:

    const ScheduleCtrl* GetScheduler () const
    {
        return m_pScheduleCtrl;
    }

    long GetTimeDeltaInSeconds (_SVIEW_TIME_DELTA delta);
    const COleDateTime& GetDateBegin () const
    {
        return m_DateBegin;
    }

    const COleDateTime& GetDateEnd () const
    {
        return m_DateEnd;
    }

    void SetDate (const COleDateTime& date);
    void SetDateInterval (const COleDateTime& date1, const COleDateTime& date2);

	int GetViewDays () const	// Days
	{
		return (int)(m_DateEnd - m_DateBegin).GetTotalDays () + 1;
	}

    BOOL IsAppointmentInDate (const Appointment& rApp, const COleDateTime& date);

    CString GetTimeSeparator ();
    void DrawCaptionText (CDC* pDC, CRect rect, 
        const CString& strText, COLORREF clrText, int nAlign = DT_CENTER, BOOL bHighlight = FALSE);

    CRect GetUnionRectOfSelectedAppointments();

    OperationLayer* GetOpLayer();
    Appointment* GetFoucedAppointment(){return m_pFocusedAppointment;}


    virtual void AdjustLayout (CDC* /*pDC*/, const CRect& /*rectClient*/) = 0;
    virtual void AdjustRects () = 0;
    virtual void AdjustAppointments () = 0;

    virtual void AdjustScrollSizes ();
    virtual void AdjustLayout (BOOL bRedraw = TRUE);
	virtual int GetHeaderCaptionHeight(){return 0;};
protected:
	Appointment *		m_pPreHitAppointment ;
    CRect				m_rect4Appointments;
    CRect				m_rect4DefaultAppointment;
    CRect				m_rect4Today;
    int					m_nRowHeight;

    float				m_fScrollOffset;
    float				m_fScrollTotal;
    float				m_fScrollPage;
	float				m_CenterTime;
    ScheduleCtrl*       m_pScheduleCtrl;

    COleDateTime		m_DateBegin;	// start date
    COleDateTime		m_DateEnd;		// end date

    OPRATION_MODE       m_OpMode;
    Appointment*        m_pFocusedAppointment;

    Appointment*        m_pDefaultAppointment;

    _SVIEW_HITTEST      m_HitTest;

    BOOL                m_bPtInToday;

    CArray<CRect, CRect&> m_ViewRects;
};

