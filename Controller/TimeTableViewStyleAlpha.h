#pragma once
#include "TimeTableView.h"
class TimeTableViewStyleAlpha :
	public TimeTableView
{
	DECLARE_DYNAMIC(TimeTableViewStyleAlpha)

public:
	TimeTableViewStyleAlpha(void);
	virtual ~TimeTableViewStyleAlpha(void);

	virtual void OnPaint (CDC* pDC, const CRect& rectClient);
	virtual void OnDrawBackgound (CDC* /*pDC*/, const CRect& /*rect*/);

	virtual void OnDrawTimeLine (CDC* pDC, const CRect& rectTime);
	virtual void OnDrawTimeBar (CDC* pDC, const CRect& rectBar, BOOL bDrawTimeLine);
    virtual void OnDrawItems (CDC* pDC);
	virtual void DrawSelection (CDC* pDC);

    virtual void DrawAppointment (CDC* pDC, Appointment* pApp, BOOL bClipClient = TRUE);

    CRect GetItemRect(Appointment* pItem);

    COleDateTime GetDateFromPoint (const CPoint& point);


	_TVIEW_TIME_DELTA GetTimeDelta () const;

	virtual BOOL OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	void AdjustLayout (CDC* /*pDC*/, const CRect& rectClient);
	virtual void AdjustScrollSizes ();

    virtual void ScrollToDate(COleDateTime Date);


protected:
	CRect				m_rectTimeBar;

	COleDateTime		m_CurrentTime;

};

