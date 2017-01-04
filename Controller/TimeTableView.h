#pragma once

#include "TimeTableCtrlType.h"
#include "TimeTableCtrl.h"
#include "Appointment.h"

class CTimeTableCtrl;

class TimeTableView : public CObject
{
	DECLARE_DYNAMIC(TimeTableView)
	friend class CTimeTableCtrl;


public:
	TimeTableView(void);
	virtual ~TimeTableView(void);

	virtual void OnActivate(CTimeTableCtrl* pTimeTable, const TimeTableView* pOldView);
	virtual void OnDeactivate(CTimeTableCtrl* pTimeTable);
public:

	virtual void OnPaint (CDC* /*pDC*/, const CRect& /*rectClient*/) = 0;
	virtual void OnDrawBackgound (CDC* /*pDC*/, const CRect& /*rect*/) = 0;
    virtual void DrawAppointment (CDC* pDC, Appointment* pApp, BOOL bClipClient = TRUE){}
    virtual void OnDrawItems (CDC* pDC){};

    virtual void ScrollToDate(COleDateTime Date){}

	//Hit tests
    virtual _TVIEW_HITTEST HitTest (const CPoint& point) const;
    virtual _TVIEW_HITTEST HitTestArea (const CPoint& point) const;
    virtual _TVIEW_HITTEST HitTestOperationLayer (const CPoint& point) const;
    virtual _TVIEW_HITTEST HitTestAppointment (const CPoint& point) const;

    Appointment* GetItemFromPoint (const CPoint& point) const;
    Appointment* GetFoucedItem(){return m_pFocusedAppointment;}

	//Handle mouse keyboard message
	virtual BOOL OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	virtual void StartSelect ();
	virtual void StopSelect ();
	virtual void StartRelocate ();
	virtual void StopRelocate ();
	virtual void UpdateOpMode(_TOPRATION_MODE mode){m_OpMode = mode;}
	void ReArrangeAppointments(void){AdjustLayout(TRUE);};
public:
	virtual COleDateTime GetDateFromPoint (const CPoint& point);

	inline _TOPRATION_MODE GetOPMode(){return m_OpMode;}

protected:

	const CTimeTableCtrl* GetTimeTable () const
	{
		return m_pTimeTableCtrl;
	}

	long GetTimeDeltaInSeconds (_TVIEW_TIME_DELTA delta);

	CString GetTimeSeparator ();

	const COleDateTime& GetDate () const
	{
		return m_Date;
	}

	void SetDate (const COleDateTime& date);


	virtual void AdjustLayout (CDC* /*pDC*/, const CRect& /*rectClient*/) = 0;

	virtual void AdjustScrollSizes ();
	virtual void AdjustLayout (BOOL bRedraw = TRUE);

protected:
	CRect				m_rect4Appointments;
	int					m_nRowHeight;

	int					m_nScrollOffset;
	int					m_nScrollTotal;
	int					m_nScrollPage;

	CTimeTableCtrl*     m_pTimeTableCtrl;

    Appointment*        m_pFocusedAppointment;

	COleDateTime		m_Date;	

	_TVIEW_HITTEST      m_HitTest;
	_TOPRATION_MODE     m_OpMode;
};

