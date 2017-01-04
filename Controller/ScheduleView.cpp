#include "StdAfx.h"
#include "ScheduleView.h"

IMPLEMENT_DYNAMIC(ScheduleView, CObject)

ScheduleView::ScheduleView(void)
: m_rect4Appointments          (0, 0, 0, 0)
, m_rect4DefaultAppointment    (0, 0, 0, 0)
, m_rect4Today  (0, 0, 0, 0)
, m_nRowHeight        (0)
, m_fScrollOffset     (0)
, m_fScrollTotal      (0)
, m_fScrollPage       (0)
, m_pScheduleCtrl     (NULL)
, m_DateBegin   ()
, m_DateEnd ()
, m_OpMode           (OPERATION_NONE)
, m_pFocusedAppointment(NULL)
, m_pDefaultAppointment(NULL)
, m_HitTest (_SVIEW_HITTEST_NOWHERE)
, m_bPtInToday (FALSE)
, m_ViewRects ()
,m_CenterTime(0)
,m_pPreHitAppointment(0)
{

}


ScheduleView::~ScheduleView(void)
{
}

void ScheduleView::OnActivate(ScheduleCtrl* pScheduler, const ScheduleView* pOldView)
{
    m_pScheduleCtrl = pScheduler;
}
void ScheduleView::OnDeactivate(ScheduleCtrl* pScheduler)
{
}

long ScheduleView::GetTimeDeltaInSeconds (_SVIEW_TIME_DELTA delta)
{
    ASSERT(_SVIEW_TIME_DELTA_FIRST <= delta);
    ASSERT(delta <= _SVIEW_TIME_DELTA_LAST);

    long nRes = 60;

    switch (delta)
    {
    case _SVIEW_TIME_DELTA_60:
        nRes = 60 * 60;
        break;
    case _SVIEW_TIME_DELTA_30:
        nRes = 30 * 60;
        break;
    case _SVIEW_TIME_DELTA_20:
        nRes = 20 * 60;
        break;
    case _SVIEW_TIME_DELTA_15:
        nRes = 15 * 60;
        break;
    case _SVIEW_TIME_DELTA_10:
        nRes = 10 * 60;
        break;
    case _SVIEW_TIME_DELTA_6:
        nRes = 6 * 60;
        break;
    case _SVIEW_TIME_DELTA_5:
        nRes = 5 * 60;
        break;
    case _SVIEW_TIME_DELTA_4:
        nRes = 4 * 60;
        break;
    case _SVIEW_TIME_DELTA_3:
        nRes = 3 * 60;
        break;
    case _SVIEW_TIME_DELTA_2:
        nRes = 2 * 60;
        break;
    case _SVIEW_TIME_DELTA_1:
        nRes = 1*60;
        break;
	case _SVIEW_TIME_DELTA_30S :
		nRes = 30;
		break;
	case _SVIEW_TIME_DELTA_15S :
		nRes = 15;
		break;
	case _SVIEW_TIME_DELTA_5S :
		nRes = 5;
		break;
	case _SVIEW_TIME_DELTA_1S:
		nRes = 1;
		break;
    default:
        ASSERT (FALSE);
    }

    return nRes;
}

CString ScheduleView::GetTimeSeparator ()
{
    CString str;

    ::GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_STIME, str.GetBuffer (10), 10);

    str.ReleaseBuffer ();

    return str;
}

void ScheduleView::DrawCaptionText (CDC* pDC, CRect rect, 
    const CString& strText, COLORREF clrText, int nAlign, BOOL bHighlight)
{
    ASSERT_VALID (pDC);

    const int nTextMargin = 2;

    if (bHighlight)
    {
        const int nTextLen = min (rect.Width (), 
            pDC->GetTextExtent (strText).cx + 2 * nTextMargin);

        CRect rectHighlight = rect;
        rectHighlight.DeflateRect (1, 1);

        switch (nAlign)
        {
        case DT_LEFT:
            rectHighlight.right = rectHighlight.left + nTextLen;
            break;

        case DT_RIGHT:
            rectHighlight.left = rectHighlight.right - nTextLen;
            break;

        case DT_CENTER:
            rectHighlight.left = rectHighlight.CenterPoint ().x - nTextLen / 2;
            rectHighlight.right = rectHighlight.left + nTextLen;
            break;
        }

        CBrush brHilite (VisualManager->GetToolbarHighlightColor());
        pDC->FillRect (rectHighlight, &brHilite);

        clrText = VisualManager->GetToolbarHighlightColor();
    }

    rect.DeflateRect (nTextMargin, 0);

    COLORREF clrOld = pDC->SetTextColor (clrText);
    pDC->DrawText (strText, rect, DT_SINGLELINE | DT_VCENTER | nAlign);

    pDC->SetTextColor (clrOld);
}

BYTE ScheduleView::OnDrawAppointments (CDC* pDC, const CRect& rect, const COleDateTime& date)
{
    BYTE res = 0;

    const AppointmentArray& arQueryApps = GetScheduler()->GetQueryedAppointments ();

    if (arQueryApps.GetSize () == 0)
    {
        return res;
    }

    Appointment* pApp;

    const int nDays = GetViewDays ();

    COleDateTime dateinternal (m_DateBegin);

    COleDateTimeSpan spanDay (1, 0, 0, 0);

    int nDay = 0;
    // draw all the unselected appointments in the visible area
    for (nDay = 0; nDay < nDays; nDay ++)
    {

        for (int i = 0; i < arQueryApps.GetSize (); i++)
        {
            pApp = arQueryApps[i];
            if (!pApp->IsSelected())
            {
                pApp->OnDraw (pDC, this, dateinternal);
            }
        }

        dateinternal += spanDay;
    }

    // draw all the selected appointments both visible and invisible
    POSITION pos = GetScheduler()->GetFirstSelectedAppointment();
    while (pos != NULL)
    {
        Appointment* pApp = GetScheduler()->GetNextSelectedAppointment(pos);
        ASSERT_VALID (pApp);

        if (pApp == NULL)
        {
            continue;
        }

        COleDateTime SelectDate = COleDateTime(pApp->GetStart().GetYear(), 
            pApp->GetStart().GetMonth(), pApp->GetStart().GetDay(), 0,0,0);
        COleDateTime EndTime = COleDateTime(pApp->GetFinish().GetYear(), 
            pApp->GetFinish().GetMonth(), pApp->GetFinish().GetDay(), 0,0,0);

        COleDateTimeSpan span = COleDateTimeSpan(1, 0, 0, 0);

        while (SelectDate <= EndTime)
        {
            pApp->OnDraw (pDC, this, SelectDate);

            SelectDate += span;
        }

    }

    return res;
}

BOOL ScheduleView::IsAppointmentInDate (const Appointment& rApp, const COleDateTime& date)
{
    COleDateTime dt (date);
    dt.SetDate (dt.GetYear (), dt.GetMonth (), dt.GetDay ());

    COleDateTime dtS (rApp.GetStart ());
    dtS.SetDate (dtS.GetYear (), dtS.GetMonth (), dtS.GetDay ());

    COleDateTime dtF (rApp.GetFinish ());

    return (dtS <= dt && dt < dtF) || (dtS == dt && dt == dtF);
}

CRect ScheduleView::GetUnionRectOfSelectedAppointments()
{
    CRect rect;

    POSITION pos = GetScheduler()->GetFirstSelectedAppointment();
    while (pos != NULL)
    {
        Appointment* pApp = GetScheduler()->GetNextSelectedAppointment(pos);
        COleDateTime Date = COleDateTime(pApp->GetStart().GetYear(), 
            pApp->GetStart().GetMonth(), pApp->GetStart().GetDay(), 0,0,0);
        COleDateTime EndTime = COleDateTime(pApp->GetFinish().GetYear(), 
            pApp->GetFinish().GetMonth(), pApp->GetFinish().GetDay(), 0,0,0);

        COleDateTimeSpan span = COleDateTimeSpan(1, 0, 0, 0);

        while (Date <= EndTime)
        {
            rect.UnionRect(rect, pApp->GetRectDraw(Date));

            Date += span;
        }
    }
    return rect;
}

_SVIEW_HITTEST ScheduleView::HitTest (const CPoint& point) const
{
    _SVIEW_HITTEST hit = HitTestArea (point);

    if (hit == _SVIEW_HITTEST_CLIENT || hit == _SVIEW_HITTEST_NOWHERE)
    {
        //_SVIEW_HITTEST hitO = HitTestOperationLayer(point);

        //if (hitO != _SVIEW_HITTEST_NOWHERE)
        //{
        //    return hitO;
        //}

        _SVIEW_HITTEST hitA = HitTestAppointment (point);

        if (hitA != _SVIEW_HITTEST_NOWHERE)
        {
            hit = hitA;
        }
    }

    return hit;
}
_SVIEW_HITTEST ScheduleView::HitTestArea (const CPoint& point) const
{
    _SVIEW_HITTEST hit = _SVIEW_HITTEST_NOWHERE;

    if (m_rect4Appointments.PtInRect (point))
    {
        hit = _SVIEW_HITTEST_CLIENT;
    }
    else if(IsPtInDefaultArea(point))
    {
        hit = _SVIEW_HITTEST_DEFAULT_APP;
    }
    else if (IsPtInToday(point))
    {
        hit = _SVIEW_HITTEST_HEADER_TODAY;
    }

    return hit;
}

_SVIEW_HITTEST ScheduleView::HitTestOperationLayer (const CPoint& point) const
{
    _SVIEW_HITTEST hit = _SVIEW_HITTEST_NOWHERE;

    if(m_pScheduleCtrl->GetSelectedAppointmentsCount() > 0)
    {
        switch(m_pScheduleCtrl->GetOpLayer()->HitTest(point))
        {
        case OperationLayer::OPLAYER_HITTEST_INSIDE:
            //hit = _SVIEW_HITTEST_OPLAYER;
            break;
        case OperationLayer::OPLAYER_HITTEST_MOVE:
            //hit = _SVIEW_HITTEST_OPLAYER_MOVE;
            break;
        case OperationLayer::OPLAYER_HITTEST_TOP:
            hit = _SVIEW_HITTEST_OPLAYER_TOP;
            break;
        case OperationLayer::OPLAYER_HITTEST_BOTTOM:
            hit = _SVIEW_HITTEST_OPLAYER_BOTTOM;
            break;
        case OperationLayer::OPLAYER_HITTEST_LEFT:
            hit = _SVIEW_HITTEST_OPLAYER_LEFT;
            break;
        case OperationLayer::OPLAYER_HITTEST_RIGHT:
            hit = _SVIEW_HITTEST_OPLAYER_RIGHT;
            break;
        default:
            break;
        }
    }

    return hit;
}

_SVIEW_HITTEST ScheduleView::HitTestAppointment (const CPoint& point) const
{
    _SVIEW_HITTEST hit = _SVIEW_HITTEST_NOWHERE;

    Appointment* pApp = ((ScheduleView*) this)->GetAppointmentFromPoint (point);

    if (pApp != NULL)
    {
        switch(pApp->HitTest (point))
        {
        case Appointment::APPOINTMENT_HITTEST_INSIDE:
            hit = _SVIEW_HITTEST_APPOINTMENT;
            break;
        case Appointment::APPOINTMENT_HITTEST_MOVE:
            hit = _SVIEW_HITTEST_APPOINTMENT_MOVE;
            break;
        case Appointment::APPOINTMENT_HITTEST_TOP:
            hit = _SVIEW_HITTEST_APPOINTMENT_TOP;
            break;
        case Appointment::APPOINTMENT_HITTEST_BOTTOM:
            hit = _SVIEW_HITTEST_APPOINTMENT_BOTTOM;
            break;
        case Appointment::APPOINTMENT_HITTEST_LEFT:
            hit = _SVIEW_HITTEST_APPOINTMENT_LEFT;
            break;
        case Appointment::APPOINTMENT_HITTEST_RIGHT:
            hit = _SVIEW_HITTEST_APPOINTMENT_RIGHT;
            break;
        default:
            ASSERT(FALSE);
        }
    }

    return hit;
}

Appointment* ScheduleView::GetAppointmentFromPoint (const CPoint& point)
{
    Appointment* pApp = NULL;

    const AppointmentArray& arApps = GetScheduler()->GetQueryedAppointments ();

    for (int i = 0; i < arApps.GetSize (); i++)
    {
        if (arApps[i]->PointInRectDraw (point))
        {
            pApp = arApps[i];
            break;
        }
    }

    return pApp;
}

COleDateTime ScheduleView::GetDateFromPoint (const CPoint& point)
{
    COleDateTime date;

    int nDay = -1;

    const int nDuration = GetViewDays ();

    for (int i = 0; i < nDuration; i++)
    {
        CRect rt (m_ViewRects[i]);
        rt.right++;
        rt.bottom++;

        if (rt.PtInRect (point))
        {
            nDay = i;
            break;
        }
    }

    if (nDay != -1)
    {
        date = GetDateBegin() + COleDateTimeSpan (nDay, 0, 0, 0);
    }

    return date;
}

//
//select all appointments between "pStartApp" and "pEndApp"
//
void ScheduleView::SelectAppointmetRange(Appointment *pStartApp ,Appointment *pEndApp)
{
	if(NULL == pEndApp ) return;
	if(pStartApp == pEndApp)
	{
			m_pScheduleCtrl->SelectAppointment (pStartApp, TRUE/*!pApp->IsSelected ()*/, FALSE);
			return;
	}

	const ScheduleCtrl *pCtrl = this->GetScheduler();
	const AppointmentArray &Days = pCtrl->m_QueryAppsArray;
	//no start appointment is selected when doing "shift selection"
	if(NULL == pStartApp)
	{
		/*from left to right to find first appointment obj*/
		
		if(Days.GetSize() <= 0)
			return;

		pStartApp = Days[0];
	}

	if(pStartApp->GetStart() > pEndApp->GetStart())
	{
		Appointment *p = pStartApp;
		pStartApp = pEndApp;
		pEndApp = p;
	}

	int i = 0;
	while(1)
	{
		if(Days[i]->GetStart() >= pStartApp->GetStart())
			break;
		i++;
	}

	while(1)
	{
		if(Days[i]->GetStart()<=  pEndApp->GetStart())
		{
			if(Days[i]->GetStart().GetDayOfYear()< pEndApp->GetStart().GetDayOfYear())
				m_pScheduleCtrl->SelectAppointment (Days[i], TRUE/*!pApp->IsSelected ()*/, FALSE);
			else
			{
				if(Days[i]->GetStart() <= pEndApp->GetStart())
					m_pScheduleCtrl->SelectAppointment (Days[i], TRUE/*!pApp->IsSelected ()*/, FALSE);
			}
		}
		i++;
		if( i >= Days.GetSize())
			break;
	}
}

BOOL ScheduleView::OnLButtonDown(UINT nFlags, CPoint point)
{
/*  by RaymonLiu
    CPoint pt;
    ::GetCursorPos (&pt);
    m_pScheduleCtrl->ScreenToClient (&pt);
*/	

	Appointment* pApp = NULL;
    _SVIEW_HITTEST hit = HitTest (point);

    BOOL bInAppointment = FALSE;

    if (hit == _SVIEW_HITTEST_CLIENT ||
        (hit >= _SVIEW_HITTEST_APPOINTMENT && hit < _SVIEW_HITTEST_OPLAYER ))
    {
        pApp = GetAppointmentFromPoint (point);
            
        if (pApp && hit >= _SVIEW_HITTEST_APPOINTMENT)
        {
            BOOL bControl = (nFlags & MK_CONTROL) != 0 || (nFlags & MK_SHIFT) != 0;

            if (!bControl && !pApp->IsSelected())
            {
                m_pScheduleCtrl->ClearAppointmentSelection (FALSE);
				m_pPreHitAppointment = NULL;
            }
 
			if(nFlags & MK_CONTROL)
			{
				m_pScheduleCtrl->SelectAppointment (pApp, TRUE/*!pApp->IsSelected ()*/, FALSE);
				m_pPreHitAppointment = pApp;
			}
			//zxy  "shift" selection
			else if(nFlags & MK_SHIFT)
			{
				AppointmentList &selList = m_pScheduleCtrl->GetSelectedAppointments();
				if(NULL == m_pPreHitAppointment && selList.GetCount() >= 1)
				{
					m_pPreHitAppointment = selList.GetHead();
				}
				m_pScheduleCtrl->ClearAppointmentSelection (FALSE);
				SelectAppointmetRange(m_pPreHitAppointment,pApp);
			}else
			{
				m_pScheduleCtrl->SelectAppointment (pApp, TRUE/*!pApp->IsSelected ()*/, FALSE);
				m_pPreHitAppointment = pApp;
			}

            bInAppointment = TRUE;
        }
        else if(hit < _SVIEW_HITTEST_OPLAYER)
        {
            m_pScheduleCtrl->ClearAppointmentSelection (FALSE);
			m_pPreHitAppointment = NULL;
        }
        else
        {
            bInAppointment = TRUE;
        }

        COleDateTime date (GetDateFromPoint (point));
        COleDateTime dt (date.GetYear(), date.GetMonth(), date.GetDay(), 0, 0, 0);

        
        if (bInAppointment)
        {
            m_pScheduleCtrl->RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        }

    }

    m_HitTest = hit;

    if (bInAppointment)
    {
        m_pFocusedAppointment = GetAppointmentFromPoint(point);
    }
    else
    {
        m_pFocusedAppointment = NULL;
    }

    return bInAppointment;
}

//reture ture if the app could be re-allocated
BOOL ScheduleView::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_HitTest = _SVIEW_HITTEST_NOWHERE;
/* by RaymondLiu
    CPoint pt;
    ::GetCursorPos (&pt);
    m_pScheduleCtrl->ScreenToClient (&pt);
*/	
	CRect RectClient;
    m_pScheduleCtrl->GetClientRect(RectClient);
	if(!(nFlags & MK_SHIFT))
		m_pPreHitAppointment = NULL;
	//by RaymondLiu
	//if (!m_rect4Appointments.PtInRect(pt) && RectClient.PtInRect(pt))
	if (!m_rect4Appointments.PtInRect(point) && RectClient.PtInRect(point))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL ScheduleView::OnMouseMove(UINT nFlags, CPoint point)
{
    if(OPERATION_NONE < m_OpMode)
    {
/*
		CPoint pt;
        ::GetCursorPos (&pt);
        m_pScheduleCtrl->ScreenToClient (&pt);
*/
    }

    _SVIEW_HITTEST hit = HitTest (point);

    if (hit == _SVIEW_HITTEST_HEADER_TODAY && !m_bPtInToday)
    {
        m_bPtInToday = TRUE;
        m_pScheduleCtrl->RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
    else if(hit != _SVIEW_HITTEST_HEADER_TODAY && m_bPtInToday)
    {
        m_bPtInToday = FALSE;
        m_pScheduleCtrl->RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

    return TRUE;
}

BOOL ScheduleView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{

    if (m_fScrollTotal > 0)
    {
        const int nSteps = abs(zDelta) / WHEEL_DELTA;

        for (int i = 0; i < nSteps; i++)
        {
            OnVScroll (zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0, NULL);
        }
    }

    return TRUE;
}


BOOL ScheduleView::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
    ASSERT_VALID(m_pScheduleCtrl);

    CPoint pt;
    ::GetCursorPos (&pt);
    m_pScheduleCtrl->ScreenToClient (&pt);
	
	//By RaymondLiu
	if(pt.y < CALENDAR_CTRL_HEIGHT)
	{
		return TRUE;
	}
	else
	{
		pt.y -= CALENDAR_CTRL_HEIGHT;
	}

    _SVIEW_HITTEST hit = HitTest (pt);

    if(m_OpMode == OPERATION_DRAG)
    {
        ::SetCursor (AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));

        return TRUE;
    }
    else if (hit == _SVIEW_HITTEST_APPOINTMENT_MOVE)
    {
        ::SetCursor (AfxGetApp()->LoadStandardCursor(IDC_HAND));

        return TRUE;
    }
    /*else if (hit == _SVIEW_HITTEST_OPLAYER_TOP)
    {
    ::SetCursor (AfxGetApp()->LoadStandardCursor(IDC_CROSS));

    return TRUE;
    }  
    else if (hit == _SVIEW_HITTEST_OPLAYER_BOTTOM)
    {
    ::SetCursor (AfxGetApp()->LoadStandardCursor(IDC_CROSS));

    return TRUE;
    }
    else if (hit == _SVIEW_HITTEST_OPLAYER_LEFT)
    {
    ::SetCursor (AfxGetApp()->LoadStandardCursor(IDC_CROSS));

    return TRUE;
    }
    else if (hit == _SVIEW_HITTEST_OPLAYER_RIGHT)
    {
    ::SetCursor (AfxGetApp()->LoadStandardCursor(IDC_CROSS));

    return TRUE;
    }*/
    else if ((hit == _SVIEW_HITTEST_APPOINTMENT_TOP || 
        hit == _SVIEW_HITTEST_APPOINTMENT_BOTTOM) )
    {
        ::SetCursor (AfxGetApp()->LoadStandardCursor(IDC_SIZENS));

        return TRUE;
    }
    else if ((hit == _SVIEW_HITTEST_APPOINTMENT_LEFT || 
        hit == _SVIEW_HITTEST_APPOINTMENT_RIGHT))
    {
        ::SetCursor (AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));

        return TRUE;
    }

    return FALSE;
}

void ScheduleView::StartSelect ()
{
    ASSERT_VALID(m_pScheduleCtrl);

    m_OpMode = OPERATION_SELECT;

}

void ScheduleView::StopSelect ()
{
    ASSERT_VALID(m_pScheduleCtrl);

    m_OpMode = OPERATION_NONE;
}

void ScheduleView::StartRelocate ()
{
    ASSERT_VALID(m_pScheduleCtrl);

    if(m_HitTest == _SVIEW_HITTEST_APPOINTMENT_MOVE)
    {
        m_OpMode = OPERATION_DRAG;
    }

}
void ScheduleView::StopRelocate ()
{
    ASSERT_VALID(m_pScheduleCtrl);

    m_OpMode = OPERATION_NONE;
}

BOOL ScheduleView::ResetCenterPointTime( int nDetal)
{
		float fScrollPage = this->m_fScrollPage ;
		float fOffset =	this->m_fScrollOffset;

		if( nDetal <= 0)
		 nDetal = this->GetTimeDeltaInSeconds(GetScheduler()->GetTimeDelta ());

		if(nDetal == 3600) //one hour,not caculate center point.
			return FALSE;
		this->m_CenterTime = (float)nDetal * (fOffset + (fScrollPage / 2) );
		return TRUE;
}

BOOL ScheduleView::OnVScroll(UINT /*nSBCode*/, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
    ASSERT_VALID (m_pScheduleCtrl);

    m_pScheduleCtrl->SetScrollPos (SB_VERT, m_fScrollOffset);

	ResetCenterPointTime();
    AdjustLayout ();

    m_pScheduleCtrl->RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

    return TRUE;
}


void ScheduleView::AdjustScrollSizes ()
{
    ASSERT_VALID(m_pScheduleCtrl);

	if(this->m_nRowHeight > MIN_ROW_HEIGHT)
	{
		//m_pScheduleCtrl->EnableScrollBar(SB_VERT,ESB_DISABLE_LEFT);
		m_pScheduleCtrl->ShowScrollBar(SB_VERT,0);
		return;
	}else
		m_pScheduleCtrl->ShowScrollBar(SB_VERT,TRUE);
    SCROLLINFO si;

    ZeroMemory (&si, sizeof (SCROLLINFO));
    si.cbSize = sizeof (SCROLLINFO);

    si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
    si.nMin  = 0;
    si.nMax  = m_fScrollTotal -1;
    si.nPage = m_fScrollPage;
    si.nPos  = m_fScrollOffset;

    m_pScheduleCtrl->SetScrollInfo (SB_VERT, &si, TRUE);
}

OperationLayer* ScheduleView::GetOpLayer()
{
    return m_pScheduleCtrl->GetOpLayer();
}


void ScheduleView::SetDate (const COleDateTime& date)
{
    COleDateTimeSpan span = COleDateTimeSpan(GetViewDays() - 1, 0, 0, 0);
    SetDateInterval(date, date+span);
}

void ScheduleView::SetDateInterval (const COleDateTime& date1, const COleDateTime& date2)
{
    m_DateBegin = COleDateTime(date1.GetYear (), date1.GetMonth (), date1.GetDay (), 0, 0, 0);
    m_DateEnd = COleDateTime(date2.GetYear (), date2.GetMonth (), date2.GetDay (), 0, 0, 0);

    m_pScheduleCtrl->QueryAppointments(m_DateBegin, m_DateEnd, FALSE);

    AdjustLayout ();
}

void ScheduleView::ReArrangeAppointments(void)
{
	AdjustLayout(TRUE);
}
void ScheduleView::AdjustLayout (BOOL bRedraw)
{

    if (m_pScheduleCtrl->GetSafeHwnd () == NULL)
    {
        return;
    }

    CClientDC dc (m_pScheduleCtrl);

    CRect rectClient;
    m_pScheduleCtrl->GetClientRect (rectClient);
	//rectClient.top -= CALENDAR_CTRL_HEIGHT;
	rectClient.bottom -= CALENDAR_CTRL_HEIGHT;

    AdjustLayout (&dc, rectClient);

    m_ViewRects.RemoveAll ();

    AdjustRects ();

    AdjustAppointments ();
}