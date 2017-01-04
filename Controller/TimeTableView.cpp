#include "StdAfx.h"
#include "TimeTableView.h"

IMPLEMENT_DYNAMIC(TimeTableView, CObject)

TimeTableView::TimeTableView(void)
: m_rect4Appointments          (0, 0, 0, 0)
, m_nRowHeight        (0)
, m_nScrollOffset     (0)
, m_nScrollTotal      (0)
, m_nScrollPage       (0)
, m_pTimeTableCtrl     (NULL)
, m_pFocusedAppointment (NULL)
, m_Date   ()
, m_HitTest (_TVIEW_HITTEST_NOWHERE)

{

}


TimeTableView::~TimeTableView(void)
{
}

void TimeTableView::OnActivate(CTimeTableCtrl* pTimeTable, const TimeTableView* pOldView)
{
    m_pTimeTableCtrl = pTimeTable;
}
void TimeTableView::OnDeactivate(CTimeTableCtrl* pTimeTable)
{
}

long TimeTableView::GetTimeDeltaInSeconds (_TVIEW_TIME_DELTA delta)
{
    ASSERT(_TVIEW_TIME_DELTA_FIRST <= delta);
    ASSERT(delta <= _TVIEW_TIME_DELTA_LAST);

    long nRes = 60*60;

    switch (delta)
    {
    case _TVIEW_TIME_DELTA_60:
        nRes = 60*60;
        break;
    case _TVIEW_TIME_DELTA_30:
        nRes = 30*60;
        break;
    case _TVIEW_TIME_DELTA_20:
        nRes = 20*60;
        break;
    case _TVIEW_TIME_DELTA_15:
        nRes = 15*60;
        break;
    case _TVIEW_TIME_DELTA_10:
        nRes = 10*60;
        break;
    case _TVIEW_TIME_DELTA_6:
        nRes = 6*60;
        break;
    case _TVIEW_TIME_DELTA_5:
        nRes = 5*60;
        break;
    case _TVIEW_TIME_DELTA_4:
        nRes = 4*60;
        break;
    case _TVIEW_TIME_DELTA_3:
        nRes = 3*60;
        break;
    case _TVIEW_TIME_DELTA_2:
        nRes = 2*60;
        break;
    case _TVIEW_TIME_DELTA_1:
        nRes = 1*60;
		break;
    case _TVIEW_TIME_DELTA_30S:
        nRes = 30;
        break;
    case _TVIEW_TIME_DELTA_15S:
        nRes = 15;
        break;
    case _TVIEW_TIME_DELTA_5S:
        nRes = 5;
        break;
    case _TVIEW_TIME_DELTA_1S:
        nRes = 1;
		break;
    default:
        ASSERT (FALSE);
    }

    return nRes;
}

CString TimeTableView::GetTimeSeparator ()
{
    CString str;

    ::GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_STIME, str.GetBuffer (10), 10);

    str.ReleaseBuffer ();

    return str;
}

COleDateTime TimeTableView::GetDateFromPoint (const CPoint& point)
{
    COleDateTime date;

    int nDay = -1;

    CRect rt (m_rect4Appointments);
    rt.right++;
    rt.bottom++;

    if (rt.PtInRect (point))
    {
        nDay = 0;
    }

    if (nDay != -1)
    {
        date = COleDateTime(1983,3,17,0,0,0) + COleDateTimeSpan (nDay, 0, 0, 0);
    }

    return date;
}


Appointment* TimeTableView::GetItemFromPoint (const CPoint& point)const
{
    Appointment* pApp = NULL;

    const AppointmentList& ItemList = m_pTimeTableCtrl->GetItemList();

    POSITION pos = ItemList.GetHeadPosition ();
    Appointment* pItemNext;
    while (pos != NULL)
    {
        POSITION posNext = pos;

        pItemNext = ItemList.GetNext (pos);

        if (pItemNext->PointInRectDraw (point))
        {
            return pItemNext;
        }
    }

    return NULL;
}


BOOL TimeTableView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint pt;
    ::GetCursorPos (&pt);
    m_pTimeTableCtrl->ScreenToClient (&pt);

    _TVIEW_HITTEST hit = HitTest (point);

    BOOL bInAppointment = FALSE;

    if (hit == _TVIEW_HITTEST_CLIENT ||
        (hit >= _TVIEW_HITTEST_APPOINTMENT && hit < _TVIEW_HITTEST_OPLAYER ))
    {
        Appointment* pApp = GetItemFromPoint (point);
            
        if (pApp && hit >= _TVIEW_HITTEST_APPOINTMENT)
        {
            BOOL bControl = (nFlags & MK_CONTROL) != 0/* || (nFlags & MK_SHIFT) != 0*/;

            if (!bControl && !pApp->IsSelected())
            {
                m_pTimeTableCtrl->ClearAppointmentSelection (FALSE);
            }
 
            m_pTimeTableCtrl->SelectItem (pApp, TRUE/*!pApp->IsSelected ()*/, FALSE);

            bInAppointment = TRUE;
        }
        else if(hit < _TVIEW_HITTEST_OPLAYER)
        {
            m_pTimeTableCtrl->ClearAppointmentSelection (FALSE);
			m_pTimeTableCtrl->GetOwner()->SendMessage(TIMETABLE_SELECTNO_APPOINTMENTS,0,0);
        }
        else
        {
            bInAppointment = TRUE;
        }

        COleDateTime date (GetDateFromPoint (point));
        COleDateTime dt (date.GetYear(), date.GetMonth(), date.GetDay(), 0, 0, 0);

        
        if (bInAppointment)
        {
            m_pTimeTableCtrl->RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        }

    }

    m_HitTest = hit;

    if (bInAppointment)
    {
        m_pFocusedAppointment = GetItemFromPoint(point);
    }
    else
    {
        m_pFocusedAppointment = NULL;
    }

    return bInAppointment;
}

//return true if the app could be re-allocated
BOOL TimeTableView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_HitTest = _TVIEW_HITTEST_NOWHERE;

	CPoint pt;
	::GetCursorPos (&pt);
	m_pTimeTableCtrl->ScreenToClient (&pt);

	CRect RectClient;
	m_pTimeTableCtrl->GetClientRect(RectClient);

	if (!m_rect4Appointments.PtInRect(pt) && RectClient.PtInRect(pt))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL TimeTableView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(_TOPERATION_NONE < m_OpMode)
	{
		CPoint pt;
		::GetCursorPos (&pt);
		m_pTimeTableCtrl->ScreenToClient (&pt);
	}

	_TVIEW_HITTEST hit = HitTest (point);

	//m_pTimeTableCtrl->RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);	

	return TRUE;
}


BOOL TimeTableView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{

    if (m_nScrollTotal > 0)
    {
        const int nSteps = abs(zDelta) / WHEEL_DELTA;

        for (int i = 0; i < nSteps; i++)
        {
            OnVScroll (zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0, NULL);
        }
    }

    return TRUE;
}



BOOL TimeTableView::OnVScroll(UINT /*nSBCode*/, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
    ASSERT_VALID (m_pTimeTableCtrl);

    m_pTimeTableCtrl->SetScrollPos (SB_VERT, m_nScrollOffset);

    AdjustLayout ();

    m_pTimeTableCtrl->RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

    return TRUE;
}


BOOL TimeTableView::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
    ASSERT_VALID(m_pTimeTableCtrl);

    CPoint pt;
    ::GetCursorPos (&pt);
    m_pTimeTableCtrl->ScreenToClient (&pt);

    _TVIEW_HITTEST hit = HitTest (pt);

    if(m_OpMode == OPERATION_DRAG)
    {
        ::SetCursor (AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));

        return TRUE;
    }
    else if (hit == _TVIEW_HITTEST_APPOINTMENT_MOVE)
    {
        ::SetCursor (AfxGetApp()->LoadStandardCursor(IDC_HAND));

        return TRUE;
    }
 
    else if ((/*hit == _TVIEW_HITTEST_APPOINTMENT_TOP || */
        hit == _TVIEW_HITTEST_APPOINTMENT_BOTTOM) )
    {
        ::SetCursor (AfxGetApp()->LoadStandardCursor(IDC_SIZENS));

        return TRUE;
    }
 
    else if ((hit == _TVIEW_HITTEST_APPOINTMENT_LEFT || 
        hit == _TVIEW_HITTEST_APPOINTMENT_RIGHT))
    {
        ::SetCursor (AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));

        return TRUE;
    }

    return FALSE;
}

void TimeTableView::StartSelect ()
{
	ASSERT_VALID(m_pTimeTableCtrl);

	m_OpMode = _TOPERATION_SELECT;

}

void TimeTableView::StopSelect ()
{
	ASSERT_VALID(m_pTimeTableCtrl);

	m_OpMode = _TOPERATION_NONE;
}

void TimeTableView::StartRelocate ()
{
	ASSERT_VALID(m_pTimeTableCtrl);

    if(m_HitTest == _TVIEW_HITTEST_APPOINTMENT_MOVE)
    {
        m_OpMode = _TOPERATION_DRAG;
    }
}
void TimeTableView::StopRelocate ()
{
	ASSERT_VALID(m_pTimeTableCtrl);

	m_OpMode = _TOPERATION_NONE;
}

void TimeTableView::AdjustScrollSizes ()
{
    ASSERT_VALID(m_pTimeTableCtrl);

    SCROLLINFO si;

    ZeroMemory (&si, sizeof (SCROLLINFO));
    si.cbSize = sizeof (SCROLLINFO);

    si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
    si.nMin  = 0;
    si.nMax  = m_nScrollTotal;
    si.nPage = m_nScrollPage;
    si.nPos  = m_nScrollOffset;

    m_pTimeTableCtrl->SetScrollInfo (SB_VERT, &si, TRUE);
}

void TimeTableView::SetDate (const COleDateTime& date)
{	
	m_Date = date;
	AdjustLayout ();
}

void TimeTableView::AdjustLayout (BOOL bRedraw)
{

    if (m_pTimeTableCtrl->GetSafeHwnd () == NULL)
    {
        return;
    }

    CClientDC dc (m_pTimeTableCtrl);

    CRect rectClient;
    m_pTimeTableCtrl->GetClientRect (rectClient);

    AdjustLayout (&dc, rectClient);
}

_TVIEW_HITTEST TimeTableView::HitTest (const CPoint& point) const
{
    _TVIEW_HITTEST hit = HitTestArea (point);

    if (hit == _TVIEW_HITTEST_CLIENT || hit == _TVIEW_HITTEST_NOWHERE)
    {
        _TVIEW_HITTEST hitO = HitTestOperationLayer(point);

        if (hitO != _TVIEW_HITTEST_NOWHERE)
        {
            return hitO;
        }

        _TVIEW_HITTEST hitA = HitTestAppointment (point);

        if (hitA != _TVIEW_HITTEST_NOWHERE)
        {
            hit = hitA;
        }
    }

    return hit;
}

_TVIEW_HITTEST TimeTableView::HitTestArea (const CPoint& point) const
{
    _TVIEW_HITTEST hit = _TVIEW_HITTEST_NOWHERE;

    if (m_rect4Appointments.PtInRect (point))
    {
        hit = _TVIEW_HITTEST_CLIENT;
    }

    return hit;
}

_TVIEW_HITTEST TimeTableView::HitTestOperationLayer (const CPoint& point) const
{
    _TVIEW_HITTEST hit = _TVIEW_HITTEST_NOWHERE;

    if(m_pTimeTableCtrl->GetSelectedAppointmentsCount() > 0)
    {
        //switch(m_pTimeTableCtrl->GetOpLayer()->HitTest(point))
        //{
        //case OperationLayer::OPLAYER_HITTEST_INSIDE:
        //    //hit = _TVIEW_HITTEST_OPLAYER;
        //    break;
        //case OperationLayer::OPLAYER_HITTEST_MOVE:
        //    //hit = _TVIEW_HITTEST_OPLAYER_MOVE;
        //    break;
        //case OperationLayer::OPLAYER_HITTEST_TOP:
        //    hit = _TVIEW_HITTEST_OPLAYER_TOP;
        //    break;
        //case OperationLayer::OPLAYER_HITTEST_BOTTOM:
        //    hit = _TVIEW_HITTEST_OPLAYER_BOTTOM;
        //    break;
        //case OperationLayer::OPLAYER_HITTEST_LEFT:
        //    hit = _TVIEW_HITTEST_OPLAYER_LEFT;
        //    break;
        //case OperationLayer::OPLAYER_HITTEST_RIGHT:
        //    hit = _TVIEW_HITTEST_OPLAYER_RIGHT;
        //    break;
        //default:
        //    break;
        //}
    }

    return hit;
}

_TVIEW_HITTEST TimeTableView::HitTestAppointment (const CPoint& point) const
{
    _TVIEW_HITTEST hit = _TVIEW_HITTEST_NOWHERE;

    Appointment* pApp = GetItemFromPoint (point);

    if (pApp != NULL)
    {
        switch(pApp->HitTest (point))
        {
        case Appointment::APPOINTMENT_HITTEST_INSIDE:
            hit = _TVIEW_HITTEST_APPOINTMENT;
            break;
        case Appointment::APPOINTMENT_HITTEST_MOVE:
            hit = _TVIEW_HITTEST_APPOINTMENT_MOVE;
            break;
        case Appointment::APPOINTMENT_HITTEST_TOP:
            hit = _TVIEW_HITTEST_APPOINTMENT_TOP;
            break;
        case Appointment::APPOINTMENT_HITTEST_BOTTOM:
            hit = _TVIEW_HITTEST_APPOINTMENT_BOTTOM;
            break;
        case Appointment::APPOINTMENT_HITTEST_LEFT:
            hit = _TVIEW_HITTEST_APPOINTMENT_LEFT;
            break;
        case Appointment::APPOINTMENT_HITTEST_RIGHT:
            hit = _TVIEW_HITTEST_APPOINTMENT_RIGHT;
            break;
        default:
            ASSERT(FALSE);
        }
    }

    return hit;
}
