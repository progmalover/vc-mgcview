#include "StdAfx.h"
#include "ScheduleViewStyleAlpha.h"
#include "Localization/Localization.h"
IMPLEMENT_DYNAMIC(ScheduleViewStyleAlpha, ScheduleView)

CString myGetDayName(int dayofweek);
CString myGetDayName(int dayofweek)
{
	CString szDayName;

	dayofweek %= 7;
	switch(dayofweek)
	{
	case 0:
		szDayName = Translate(_T("Sunday"));
		break;
	case 1:
		szDayName = Translate(_T("Monday"));
		break;
	case 2:
		szDayName = Translate(_T("Tuesday"));
		break;
	case 3:
		szDayName = Translate(_T("Wednesday"));
		break;
	case 4:
		szDayName = Translate(_T("Thursday"));
		break;
	case 5:
		szDayName = Translate(_T("Friday"));
		break;
	case 6:
		szDayName = Translate(_T("Saturday"));
		break;
	}
	return szDayName;
}

ScheduleViewStyleAlpha::ScheduleViewStyleAlpha()
    : ScheduleView         ()
    , m_Timer                  (NULL)
    , m_rectTimeBar            (0, 0, 0, 0)
    , m_nHeaderTodayHeight          (0)
    , m_nHeaderDayCaptionHeight    (0)
    , m_CurrentTime            ()
	,m_visualRate(1)
{
    m_nRowHeight = MIN_ROW_HEIGHT;
}

ScheduleViewStyleAlpha::~ScheduleViewStyleAlpha()
{

}

void ScheduleViewStyleAlpha::OnPaint (CDC* pDC, const CRect& rectClient)
{
    ASSERT_VALID (pDC);

    OnDrawBackgound (pDC, m_rect4Appointments);
    OnDrawTimeBar (pDC, m_rectTimeBar, TRUE);

    CRect rtHeader (rectClient);
    rtHeader.left   = m_rect4Appointments.left;
    rtHeader.bottom = rtHeader.top + m_nHeaderTodayHeight + m_nHeaderDayCaptionHeight;

    OnDrawHeader (pDC, rtHeader);

    OnDrawAppointments (pDC);

    GetOpLayer()->OnDraw (pDC, this);

    DrawSelection(pDC);

    DrawDefaultAppointment(pDC);

}

void ScheduleViewStyleAlpha::OnDrawBackgound (CDC* pDC, const CRect& rect)
{
    ASSERT_VALID (pDC);

    CRect rectFill (rect);

    //	const int nWeekStart = BORROW_PlannerManagerCtrl::GetFirstDayOfWeek () + 1;
    const int yOffset = m_fScrollOffset * m_nRowHeight;

    const int nDays = GetViewDays ();

    rectFill.OffsetRect (0, -yOffset);

    const int nSecs = GetTimeDeltaInSeconds (GetTimeDelta ());
    const int nCount = 60 * 60 / nSecs;

    CPen     penHour[2];
    CPen     penHalfHour[2];

    for (int i = 0; i < 2; i++)
    {
        penHour[i].CreatePen (PS_SOLID, 0,
            RGB(255, 255, 255));

        penHalfHour[i].CreatePen (PS_SOLID, 0, 
            RGB(255, 255, 255));
    }

    CBrush brBack (VisualManager->GetSmartDockingHighlightToneColor());

    COLORREF colorFill = CLR_DEFAULT;
    BOOL bIsDefaultColor = colorFill == CLR_DEFAULT;

    COLORREF clrTextOld = pDC->SetTextColor (colorFill);

    COLORREF clrBkOld = pDC->SetBkColor (1 ? 
        RGB (255, 255, 255) : RGB (128, 128, 128));

    VisualManager->OnFillPopupWindowBackground(pDC, rect);

    pDC->SetTextColor (clrTextOld);
    pDC->SetBkColor (clrBkOld);

    CBrush brHilite (VisualManager->GetToolbarHighlightColor());

    CPen penBlack (PS_SOLID, 0, VisualManager->GetToolbarDisabledTextColor());
    CPen* pOldPen = pDC->SelectObject (&penBlack);

    const int iStart = m_fScrollOffset;
    const int iEnd   = min (m_fScrollOffset + rect.Height () / m_nRowHeight, nCount * 24);

    //const int iWorkStart = nFirstWorkingHour * nCount + (int)(nFirstWorkingMinute / nMinuts);
    //const int iWorkEnd   = nLastWorkingHour * nCount + (int)(nLastWorkingMinute / nMinuts);

    COleDateTime dtStart (GetDateBegin ());

    BOOL bShowSelection = 0;

    COleDateTimeSpan SelectedDateDistance = (GetScheduler()->GetCurSelectedDate() - GetDateBegin());
    INT DateDistance = SelectedDateDistance.GetDays();

    CPoint PtSelected(0,0);
    PtSelected.y = m_nRowHeight * (GetScheduler()->GetCurSelectedDate().GetHour() * 60 
        + GetScheduler()->GetCurSelectedDate().GetMinute()) *60/ ScheduleView::GetTimeDeltaInSeconds (GetTimeDelta ())
        + m_nHeaderTodayHeight + m_nHeaderDayCaptionHeight - yOffset;
    
    if (DateDistance >=0 && DateDistance < nDays)
    {
        PtSelected.x = m_ViewRects[DateDistance].left + _SCHEDULE_APPOINTMENT_SPACE + 1;
    } 

    int nDay = 0;
    for (nDay = 0; nDay < nDays; nDay++)
    {
        int nWD = dtStart.GetDayOfWeek ();
        BOOL bWeekEnd = nWD == 1 || nWD == 7;

        rectFill = m_ViewRects [nDay];

        rectFill.left   += _SCHEDULE_APPOINTMENT_SPACE;
        rectFill.right  -= _SCHEDULE_APPOINTMENT_SPACE;
        rectFill.bottom = rectFill.top + m_nRowHeight - 1;

        for (int iStep = iStart; iStep < iEnd; iStep++)
        {
            // This rect is used for paste, tell user where will be paste to
            if (rectFill.PtInRect(PtSelected))
            {
                CBrush br (RGB(0, 0, 255)); 
                pDC->FillRect (rectFill, &br);
            }
            else
            {
                pDC->FillRect (rectFill, &brHilite);
                //VisualManager->OnFillPopupWindowBackground(pDC, rectFill);
            }

            int nPenIndex = 0;//bIsWork ? 0 : 1;

            //pDC->SelectObject (((iStep + 1) % nCount == 0) ? 
            //    &penHour [nPenIndex] : &penHalfHour [nPenIndex]);

            pDC->MoveTo (rectFill.left, rectFill.bottom);
            pDC->LineTo (rectFill.right, rectFill.bottom);

            rectFill.OffsetRect (0, m_nRowHeight);
        }

        dtStart += COleDateTimeSpan (1, 0, 0, 0);
    }

    pDC->SelectObject (&penBlack);

    for (nDay = 0; nDay < nDays; nDay++)
    {
        CRect rectDurBar (m_ViewRects [nDay]);

        rectDurBar.left   += _SCHEDULE_APPOINTMENT_SPACE;
        rectDurBar.right  -= _SCHEDULE_APPOINTMENT_SPACE;

        pDC->MoveTo (rectDurBar.right, rectDurBar.top);
        pDC->LineTo (rectDurBar.right, rectDurBar.bottom);
    }

    pDC->SelectObject (pOldPen);
}

_SVIEW_TIME_DELTA ScheduleViewStyleAlpha::GetTimeDelta () const
{
    ASSERT_VALID(GetScheduler ());
    return GetScheduler ()->GetTimeDelta ();
}

void ScheduleViewStyleAlpha::AdjustRects ()
{
    const int nDays =  GetViewDays ();
    const int dxColumn = ROUND(m_rect4Appointments.Width () / (double)nDays);

    CRect rect (m_rect4Appointments);
    rect.right = rect.left + dxColumn;

    m_ViewRects.SetSize (nDays);

    for (int nDay = 0; nDay < nDays; nDay++)
    {
        m_ViewRects[nDay] = rect;

        rect.OffsetRect (dxColumn, 0);

        if (nDay == (nDays - 2))
        {
            rect.right = m_rect4Appointments.right;
        }
    }
}


void ScheduleViewStyleAlpha::OnDrawTimeBar (CDC* pDC, const CRect& rectBar, 
    BOOL bDrawTimeLine)
{
    ASSERT_VALID (pDC);

    BOOL b24Hours = TRUE;

    CString strAM;
    CString strPM;

    COLORREF clrLine = RGB(0, 0, 0);

    CBrush TimebarBrush;
    TimebarBrush.CreateSolidBrush(RGB(210, 214, 239));

    VisualManager->OnFillPopupWindowBackground(pDC, rectBar);

    const int nHeaderHeight = m_nHeaderTodayHeight + m_nHeaderDayCaptionHeight;

    int y = rectBar.top + nHeaderHeight - m_fScrollOffset * m_nRowHeight - 1; 

    CPen pen (PS_SOLID, 0, VisualManager->GetToolbarDisabledTextColor());
    CPen* pOldPen = pDC->SelectObject (&pen);

    const long nCount = 60 *60/ ScheduleView::GetTimeDeltaInSeconds (GetTimeDelta ());

    UINT nFormat = DT_SINGLELINE | DT_RIGHT;

    if (nCount == 1)
    {
        nFormat |= DT_VCENTER;
    }

    CFont* pFont = &(const_cast<ScheduleCtrl*>(GetScheduler()))->GetSmallFont();
    ASSERT (pFont != NULL);

    LOGFONT lf;
    pFont->GetLogFont (&lf);

    lf.lfHeight *= 2;

    CFont* pOldFont = pDC->SelectObject (pFont);

    CFont fontBold;
    fontBold.CreateFontIndirect (&lf);

    if (bDrawTimeLine)
    {
        int nTimeHeight = ROUND(
            (m_CurrentTime.GetHour () + m_CurrentTime.GetMinute () / 60.0) * 
            nCount * m_nRowHeight);

        CRect rectTime  = rectBar;
        rectTime.bottom = y + nTimeHeight;
        rectTime.top    = rectTime.bottom - m_nRowHeight / (nCount > 1 ? 2 : 4);
        rectTime.left   += 5;

        OnDrawTimeLine (pDC, rectTime);
    }

    int bDrawFirstAM_PM = 0;

    int right = 0;
    int nStartHour = 0;

    int nMinDelta = max (pDC->GetTextExtent (_T("000")).cx, 18);
    CString strSeparator = ScheduleView::GetTimeSeparator ();

    for (int i = nStartHour; i < nStartHour + 24; i++)
    {
        CRect rectHour  = rectBar;
        rectHour.top    = y;
        rectHour.bottom = y + m_nRowHeight * nCount;
        rectHour.left   += 5;
        rectHour.right  -= 5;

        if(rectHour.bottom < nHeaderHeight)
        {
            y = rectHour.bottom;
            continue;
        }

        if (rectHour.top > rectBar.bottom)
        {
            break;
        }

        if (nCount > 2)
        {
            long nd = y + m_nRowHeight;

            for (int j = 0; j < nCount - 1; j++)
            {
                if (nd > rectBar.bottom)
                {
                    break;
                }

                if (nd >= nHeaderHeight)
                {
                    pDC->MoveTo (rectHour.right - nMinDelta, nd);
                    pDC->LineTo (rectHour.right, nd);

                    // draw some tiny time info
                    {
                        CRect TimeRect(rectHour.left, nd, rectHour.right, nd + m_nRowHeight);

                        int nHour = (i < 24) ? i : i - 24;

                        BOOL bAM = nHour < 12; 

                        if (!b24Hours)
                        {
                            if (nHour == 0 || nHour == 12)
                            {
                                nHour = 12;
                            }
                            else if (nHour > 12)
                            {
                                nHour -= 12;
                            }
                        }

                        CString str (_T("00"));
                       // str.Format (_T("%2d%s%02d"), nHour, strSeparator, (j+1)*60/nCount);
						 int nMinite = (j+1)*60/nCount;
						 if (nCount > 60)
                        {
                            int nSecond = (j+1)*60*60/(float)nCount - nMinite*60;
                            str.Format (_T("%2d%s%02d %02d"), nHour, strSeparator, nMinite, nSecond);
                        }
                        else
                        {
                            str.Format (_T("%2d%s%02d"), nHour, strSeparator, nMinite);
                        }

                        if (!b24Hours)
                        {
                            if (nHour == 12)
                            {
                                str.Format (_T("12 %s"), bAM ? strAM : strPM);
                            }
                        }

                        pDC->DrawText (str, TimeRect, nFormat);
                    }
                }

                nd += m_nRowHeight;
            }
        }

        if (rectHour.bottom >= nHeaderHeight)
        {
            y += m_nRowHeight * nCount;

            pDC->MoveTo (rectHour.left , y);
            pDC->LineTo (rectHour.right + right, y);
        }

        if (rectHour.top >= nHeaderHeight || rectHour.bottom > nHeaderHeight)
        {
            if (rectHour.top >= nHeaderHeight - 1)
            {
                bDrawFirstAM_PM++;
            }

            CString str (_T("00"));

            int nHour = (i < 24) ? i : i - 24;

            BOOL bAM = nHour < 12; 

            if (!b24Hours)
            {
                if (nHour == 0 || nHour == 12)
                {
                    nHour = 12;
                }
                else if (nHour > 12)
                {
                    nHour -= 12;
                }
            }

            if (nCount == 1)
            {
                str.Format (_T("%2d%s00"), nHour, strSeparator);

                if (!b24Hours)
                {
                    if (nHour == 12)
                    {
                        str.Format (_T("12 %s"), bAM ? strAM : strPM);
                    }
                }

                pDC->DrawText (str, rectHour, nFormat);
            }
            else
            {
                y = rectHour.bottom;

                rectHour.bottom = rectHour.top + m_nRowHeight;

                if (!b24Hours)
                {
                    if (nHour == 12 || bDrawFirstAM_PM == 1)
                    {
                        str = bAM ? strAM : strPM;
                    }
                }

                CRect rectMin (rectHour);
                rectMin.left   = rectMin.right - nMinDelta;

                pDC->DrawText (str, rectMin, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

                pDC->SelectObject (&fontBold);

                rectHour.bottom = y;
                rectHour.right -= nMinDelta;

                str.Format (_T("%d"), nHour);
                pDC->DrawText (str, rectHour, nFormat);

                pDC->SelectObject (pFont);
            }
        }

        y = rectHour.bottom;
    }

    pDC->SelectObject (pOldFont);

    CRect rt (rectBar);
    rt.bottom = rt.top + nHeaderHeight;

    pDC->MoveTo (rectBar.left, rt.bottom - 1);
    pDC->LineTo (rectBar.right - 5, rt.bottom - 1);

    pDC->MoveTo (rectBar.right - 1, rectBar.top);
    pDC->LineTo (rectBar.right - 1, rectBar.bottom);

    pDC->SelectObject (pOldPen);

    CRect DefaultLayoutRect(rectBar);
    DefaultLayoutRect.bottom = DefaultLayoutRect.top + m_nHeaderDayCaptionHeight + m_nHeaderTodayHeight;
    VisualManager->OnFillPopupWindowBackground(pDC, DefaultLayoutRect);

    pFont = &(const_cast<ScheduleCtrl*>(GetScheduler()))->GetRegularFont();

    pOldFont = pDC->SelectObject (pFont);
	CString strName = Translate(_T("Default")) + _T("\n") + Translate(_T("Layout"));
    pDC->DrawText (strName, DefaultLayoutRect, DT_VCENTER | DT_CENTER);

    pDC->SelectObject (pOldFont);
}

void ScheduleViewStyleAlpha::OnDrawTimeLine (CDC* pDC, const CRect& rect)
{
    ASSERT_VALID (pDC);

    pDC->MoveTo (rect.left, rect.bottom);
    pDC->LineTo (rect.right, rect.bottom);
}


void ScheduleViewStyleAlpha::OnDrawHeader (CDC* pDC, const CRect& rectHeader)
{
    ASSERT_VALID (pDC);

    const int nDays = GetViewDays ();

    COLORREF clrText = RGB(0, 0, 0);

    CFont* pFont = &(const_cast<ScheduleCtrl*>(GetScheduler()))->GetRegularFont();
    ASSERT (pFont != NULL);

    LOGFONT lf;
    pFont->GetLogFont (&lf);

    lf.lfHeight = static_cast<LONG>(m_nHeaderDayCaptionHeight * 0.45);

    CFont fontDays;
    fontDays.CreateFontIndirect (&lf);

    CFont* pOldFont = pDC->SelectObject (pFont);

    CPen penBlack (PS_SOLID, 0, VisualManager->GetToolbarDisabledTextColor());
    CPen* pOldPen = pDC->SelectObject (&penBlack);

    VisualManager->OnFillOutlookPageButton(pDC, rectHeader, FALSE, FALSE, clrText);

    CRect rectDayCaption (rectHeader);
    rectDayCaption.top = rectDayCaption.top + m_nHeaderTodayHeight;
    rectDayCaption.bottom = rectDayCaption.top + rectDayCaption.Height()/2;
    CRect rectToday (rectHeader);
    rectToday.bottom = rectToday.top + m_nHeaderTodayHeight;

    COleDateTime day (GetDateBegin ());
    COleDateTime dayCurrent = COleDateTime::GetCurrentTime ();
    dayCurrent.SetDateTime (dayCurrent.GetYear (), dayCurrent.GetMonth (), 
        dayCurrent.GetDay (), 0, 0, 0);

    
    CString strTemp;
    strTemp.Empty ();
    strTemp.GetBuffer (_MAX_PATH);

    SYSTEMTIME st;
    dayCurrent.GetAsSystemTime (st);
#if 0
    ::GetDateFormat (LOCALE_USER_DEFAULT, 0, &st, 
        _T("ddd',' MM/dd/yyyy"), (LPTSTR)(LPCTSTR)strText, _MAX_PATH);
#endif 
	::GetDateFormat (LOCALE_USER_DEFAULT, 0, &st, _T("MM/dd/yyyy"), (LPTSTR)(LPCTSTR)strTemp, _MAX_PATH);

    strTemp.ReleaseBuffer ();

	CString strText;
	strText = myGetDayName(st.wDayOfWeek);
	strText += _T(", ");
	strText += strTemp;

	VisualManager->OnFillOutlookPageButton(pDC, rectToday, m_bPtInToday, FALSE, clrText);

    DrawCaptionText (pDC, rectToday, strText, clrText, DT_CENTER);

    pDC->MoveTo (rectToday.left, rectToday.top);
    pDC->LineTo (rectToday.right, rectToday.top);
    pDC->MoveTo (rectToday.left, rectToday.bottom);
    pDC->LineTo (rectToday.right, rectToday.bottom);
    pDC->MoveTo (rectToday.left, rectToday.top);
    pDC->LineTo (rectToday.left, rectToday.bottom);
    pDC->MoveTo (rectToday.right, rectToday.top);
    pDC->LineTo (rectToday.right, rectToday.bottom);
    

    pDC->SelectObject (&fontDays);

    for (int nDay = 0; nDay < nDays; nDay++)
    {
        rectDayCaption.left = m_ViewRects [nDay].left;
        rectDayCaption.right = m_ViewRects [nDay].right;

        COLORREF clrText = RGB(0, 0, 0);

        CString strTemp;
        strTemp.Empty ();
        strTemp.GetBuffer (_MAX_PATH);

        SYSTEMTIME st;
        day.GetAsSystemTime (st);
#if 0
		::GetDateFormat (LOCALE_USER_DEFAULT, 0, &st, 
			_T("ddd',' MM/dd/yyyy"), (LPTSTR)(LPCTSTR)strText, _MAX_PATH);
#endif 
		::GetDateFormat (LOCALE_USER_DEFAULT, 0, &st, _T("MM/dd/yyyy"), (LPTSTR)(LPCTSTR)strTemp, _MAX_PATH);

		strTemp.ReleaseBuffer ();

		CString strText;
		strText = myGetDayName(st.wDayOfWeek);
		strText += _T(", ");
		strText += strTemp;


        if (strText.Find (TCHAR('\n')) != -1)
        {
            CString strDate;

            AfxExtractSubString (strDate, strText, 0, TCHAR('\n'));
            DrawCaptionText (pDC, rectDayCaption, strDate, clrText, DT_CENTER);

            rectDayCaption.OffsetRect(0, rectDayCaption.Height());
            AfxExtractSubString (strDate, strText, 1, TCHAR('\n'));
            DrawCaptionText (pDC, rectDayCaption, strDate, clrText, DT_CENTER);
            rectDayCaption.OffsetRect(0, -rectDayCaption.Height());
        }
        else
        {
            DrawCaptionText (pDC, rectDayCaption, strText, clrText, DT_LEFT);
        }
        
        rectDayCaption.bottom = rectDayCaption.bottom + rectDayCaption.Height();
        pDC->MoveTo (rectDayCaption.left, rectDayCaption.top);
        pDC->LineTo (rectDayCaption.right, rectDayCaption.top);
        pDC->MoveTo (rectDayCaption.left, rectDayCaption.bottom);
        pDC->LineTo (rectDayCaption.right, rectDayCaption.bottom);
        pDC->MoveTo (rectDayCaption.left, rectDayCaption.top);
        pDC->LineTo (rectDayCaption.left, rectDayCaption.bottom);
        pDC->MoveTo (rectDayCaption.right, rectDayCaption.top);
        pDC->LineTo (rectDayCaption.right, rectDayCaption.bottom);
        rectDayCaption.bottom = rectDayCaption.bottom - rectDayCaption.Height()/2;

        day += COleDateTimeSpan (1, 0, 0, 0);
    }


    pDC->SelectObject (pOldPen);

    pDC->SelectObject(pOldFont);
}


void  ScheduleViewStyleAlpha::AdjustLayout (CDC* /*pDC*/, const CRect& rectClient)
{
    m_nHeaderTodayHeight  = 1;
    m_nHeaderDayCaptionHeight = 1; //occupy 1 row space.

    m_rect4Appointments = rectClient;
    m_rect4DefaultAppointment  = rectClient;
    m_rect4Today = rectClient;


    const int nSecs = ScheduleView::GetTimeDeltaInSeconds (GetTimeDelta ());
    const int nCount = 24 * 60 *60 / nSecs;

  //
  //re-design method to caculate items ,and item height.
  // caculate one row hegiht .
    m_nRowHeight = 3*rectClient.Height() /((nCount + m_nHeaderTodayHeight + m_nHeaderDayCaptionHeight) *3 + 1) ; 
	
	// trying to hold all items in currrent page.
	if(m_nRowHeight < MIN_ROW_HEIGHT) //can't fill all item.
		m_nRowHeight = MIN_ROW_HEIGHT;

	m_nHeaderDayCaptionHeight  *= (m_nRowHeight + m_nRowHeight/3);
	m_nHeaderTodayHeight   *= m_nRowHeight;

	m_rect4Today.bottom = m_nHeaderTodayHeight;

	m_rect4Appointments.top += m_nHeaderDayCaptionHeight;

	m_rectTimeBar = rectClient;
	m_rectTimeBar.right = m_rectTimeBar.left + m_nRowHeight * 3 ;

	m_rect4Appointments.left = m_rectTimeBar.right;
	m_rect4Today.left = m_rectTimeBar.right;
    int delta = m_rect4Appointments.Height () - m_nHeaderTodayHeight;

    if (delta < 0)
    {
        m_nHeaderTodayHeight = m_rect4Appointments.Height ();
    }
    else
    {
        int nc = (int)(delta / m_nRowHeight);

        if (nc >= nCount)
        {
            m_nHeaderTodayHeight = m_rect4Appointments.Height () - nCount * m_nRowHeight;
        }
        else
        {
            m_nHeaderTodayHeight += delta - nc * m_nRowHeight;
        }
    }

    m_rect4Appointments.top += m_nHeaderTodayHeight;

    m_rect4DefaultAppointment.bottom = m_rect4Appointments.top;
    m_rect4DefaultAppointment.right = m_rect4Appointments.left;

	//m_rect4DefaultAppointment.OffsetRect(0,CALENDAR_CTRL_HEIGHT);
	 
    AdjustScrollSizes ();
}

void ScheduleViewStyleAlpha::DrawDefaultAppointment (CDC* pDC)
{
    if (!m_pDefaultAppointment)
    {
        return;
    }

    const int nHeaderHeight = m_nHeaderTodayHeight + m_nHeaderDayCaptionHeight;

    CRect rectDefaultApps = m_rectTimeBar;
    rectDefaultApps.bottom = m_rectTimeBar.top + nHeaderHeight - 1;

    AppointmentApperance DefaultApperance;
    DefaultApperance.m_Rect = rectDefaultApps;

    DrawAppointment (pDC, m_pDefaultAppointment, &DefaultApperance, FALSE); 

}

void ScheduleViewStyleAlpha::DrawAppointment (CDC* pDC, Appointment* pApp, AppointmentApperance* pDS, BOOL bClipClient)
{
    ASSERT_VALID (pDC);
    ASSERT_VALID (pApp);

    if (pDC == NULL || pDC->GetSafeHdc () == NULL || pApp == NULL || pDS == NULL)
    {
        return;
    }

    CRect rectDS = pDS->m_Rect;

    CFont* pFont = &(const_cast<ScheduleCtrl*>(GetScheduler()))->GetSmallBoldFont();
    ASSERT (pFont != NULL);

    CFont* pOldFont = pDC->SelectObject (pFont);

    if((bClipClient && OPERATION_DRAG == m_OpMode && pApp->IsSelected() &&
        m_HitTest == _SVIEW_HITTEST_APPOINTMENT_MOVE) || 
        (pApp == m_pDefaultAppointment && GetScheduler()->m_HitTest == _SVIEW_HITTEST_DEFAULT_APP))
    {
        rectDS.OffsetRect(GetScheduler()->m_ptOpCurrent - GetScheduler()->m_ptOpStart);
    }

    if (bClipClient)
    {
        CRect rectIntersection;
        rectIntersection.IntersectRect(rectDS, m_rect4Appointments);
        if (rectIntersection.IsRectEmpty())
        {
            return;
        }
    }

    CRect rect   (rectDS);

    COleDateTime dtStart  (pApp->GetStart());
    COleDateTime dtFinish (pApp->GetFinish());

    CString      strStart  (pApp->m_strStart);
    CString      strFinish (pApp->m_strFinish);

    BOOL bDrawTime     = bClipClient;
    bDrawTime = FALSE; // per phil's request, don't draw the time

    CRgn rgn;
    CRect rectClip (0, 0, 0, 0);
    pDC->GetClipBox (rectClip);

    CRgn RgnClient;
    if (bClipClient)
    {
        RgnClient.CreateRectRgn(m_rect4Appointments.left, m_rect4Appointments.top + 1, m_rect4Appointments.right, m_rect4Appointments.bottom);
        pDC->SelectClipRgn (&RgnClient, RGN_AND);
    }

    rgn.CreateRoundRectRgn (rect.left, rect.top, rect.right + 1, rect.bottom + 1, 5, 5);
    pDC->SelectClipRgn (&rgn, RGN_AND);

    COLORREF clrBack1   = RGB(220, 220, 220);
    COLORREF clrBack2   = clrBack1;
    COLORREF clrText    = RGB(66, 77, 88);
    COLORREF clrFrame1  = RGB(0, 0, 0);
    COLORREF clrFrame2  = clrFrame1;
    COLORREF clrTextOld = CLR_DEFAULT;

    //if (pApp->IsSelected())
    //{
    //    clrBack1   = RGB(0, 220, 0);
    //    clrBack2   = clrBack1;
    //    clrText    = RGB(255, 255, 255);
    //    clrFrame1  = RGB(0, 0, 0);
    //    clrFrame2  = clrFrame1;
    //    clrTextOld = CLR_DEFAULT;
    //} 

    CRgn rgn2;
    rgn2.CreateRoundRectRgn (rect.left + 1, rect.top + 1, rect.right, rect.bottom, 5, 5);
    pDC->SelectClipRgn (&rgn2, RGN_AND);

    // good for apps
    if (pApp->GetDetailType() == Appointment::APPOINTMENT_TYPE_TIMETABLE && !pApp->IsSelected())
    {
        VisualManager->OnFillTasksGroupInterior(pDC, rect, pApp->IsSelected());
    }
    else
    {
        VisualManager->OnFillOutlookPageButton(pDC, rect, pApp->IsSelected(), FALSE, clrFrame1);
    }
    

    if (pApp->GetImageList())
    {
        CRect IconRect(rectDS);

        IMAGEINFO info;
        if (pApp->GetImageList()->GetImageInfo(pApp->GetImageIndex(), &info))
        {
            //CSize sz(IconRect.Width(), IconRect.Height());
            //pApp->GetImageList()->DrawEx(pDC, pApp->GetImageIndex(), IconRect.TopLeft(), sz, RGB(0,0,0), RGB(0,0,0), 0);
            POINT pt;
            pt.x = IconRect.left + (IconRect.Width() - info.rcImage.right + info.rcImage.left) / 2;
            pt.y = IconRect.top + (IconRect.Height() - info.rcImage.bottom + info.rcImage.top) / 2;
            pApp->GetImageList()->Draw(pDC, pApp->GetImageIndex(), pt, 0);
        }
    }

    // draw description
    clrTextOld = pDC->SetTextColor (clrText);

    BOOL bCancelDraw = FALSE;
    BOOL bToolTipNeeded = FALSE;

    CSize szText (0, 0);

    szText = pDC->GetTextExtent (pApp->GetDescription());

    CRect rectText (rect);

    if (bDrawTime)
    {
        CSize szSpace (pDC->GetTextExtent (_T(" ")));

        CString str;

        if (!strStart.IsEmpty ())
        {
            str += strStart;

            if (!strFinish.IsEmpty ())
            {
                str += _T("-") + strFinish;
            }
        }

        if (!str.IsEmpty ())
        {
            COLORREF clrTime = clrText;
            if (clrTime != CLR_DEFAULT)
            {
                pDC->SetTextColor (clrTime);
            }

            CSize sz (pDC->GetTextExtent (str));

            rectText.DeflateRect (4, 1, 1, 0);

            pDC->DrawText (str, rectText, DT_NOPREFIX);	

            rect.left += sz.cx + szSpace.cx;

            if (clrTime != CLR_DEFAULT)
            {
                pDC->SetTextColor (clrText);
            }
        }

        // if time drawn and description not completely in view, then
        // move description down (if possible)

        //if ((szText.cx + 4) > rect.Width () && rect.Height () > szText.cy * 2)
        {
            bCancelDraw = FALSE;
            rectText.left = rectDS.left;
            rectText.OffsetRect (0, szText.cy);
        }
    }


    bool bDrawText = 1;
    if (bDrawText)
    {
        if (!bCancelDraw)
        {
            {
                rectText.DeflateRect (4, 1, 1, 0);
                pDC->DrawText (pApp->GetDescription(), rectText, DT_NOPREFIX);
            }
        }

        if (rectText.Width () < szText.cx)
        {
            bToolTipNeeded = TRUE;
        }
    }
    
    pDC->SetTextColor (clrTextOld);

    CRgn rgnClip;
    rgnClip.CreateRectRgn (rectClip.left, rectClip.top, rectClip.right, rectClip.bottom);

    pDC->SelectClipRgn (&rgnClip, RGN_COPY);
    if (bClipClient)
    {
        pDC->SelectClipRgn (&RgnClient, RGN_AND);
    }

    int nWidth = 1;

    CBrush br (VisualManager->GetToolbarDisabledTextColor());
    pDC->FrameRgn (&rgn, &br, nWidth, nWidth);


   if(bClipClient && OPERATION_COPY == m_OpMode && pApp->IsSelected() && m_HitTest == _SVIEW_HITTEST_APPOINTMENT_MOVE)
    {
        if (bClipClient)
        {
            pDC->SelectClipRgn (&RgnClient, RGN_OR);
        }

        int nWidth = 1;

        CRect CopyRect(rectDS);
        CPoint Offset = GetScheduler()->m_ptOpCurrent - GetScheduler()->m_ptOpStart;
        Offset.x -= rectDS.Width()/2;
        CopyRect.OffsetRect(Offset);
        //CopyRect.OffsetRect(0, m_ptOpCurrent.y - CopyRect.top);


        CRgn rgnCopy;
        rgnCopy.CreateRoundRectRgn (CopyRect.left, CopyRect.top, CopyRect.right + 1, CopyRect.bottom + 1, nWidth, nWidth);

        CBrush br (VisualManager->GetToolbarDisabledTextColor());
        pDC->FrameRgn (&rgnCopy, &br, nWidth, nWidth);
    }

   CRgn Rgn;
   Rgn.CreateRectRgn(rectClip.left, rectClip.top, rectClip.right, rectClip.bottom);
   pDC->SelectClipRgn (&Rgn, RGN_COPY);

    pDC->SelectObject(pOldFont);
}

void ScheduleViewStyleAlpha::AdjustAppointmentRectByDateTime (Appointment* pApp, CRect& rect, const COleDateTime& date)
{
    CRect rectFill = rect;
    COleDateTimeSpan spanDay (1, 0, 0, 0);
    const int nTimeDelta = GetTimeDeltaInSeconds (GetTimeDelta ());

    if (pApp->IsMultiDay())
    {
        if (pApp->GetStart() > date)
        {
            rect.top = rectFill.top + (long)(m_nRowHeight * 
                float((pApp->GetStart ().GetHour () * 60 + 
                (float)pApp->GetStart ().GetMinute ()) * 60/ nTimeDelta
                + (float)pApp->GetStart ().GetSecond() / nTimeDelta)) - 1;
        }

        if (pApp->GetStart () != pApp->GetFinish ())
        {
            if (pApp->GetFinish() < date+spanDay)
            {
                COleDateTime dtAF (pApp->GetFinish ());
                int seconds = dtAF.GetHour () * 60* 60 + dtAF.GetMinute () * 60 + dtAF.GetSecond();

                rect.bottom = rectFill.top + (long) (m_nRowHeight * 
                    (float)seconds / (float)(nTimeDelta)) - 1;

                if (rect.bottom == rect.top)
                {
                    rect.bottom = rect.top + 1;
                }
            }
        }
        else
        {
            rect.bottom = rect.top;
        }
    } 
    else
    {
        rect.top = rectFill.top + (long)(m_nRowHeight * 
            float((pApp->GetStart ().GetHour () * 60 + 
            (float)pApp->GetStart ().GetMinute ())*60 / nTimeDelta
            + (float)pApp->GetStart ().GetSecond() / (nTimeDelta))) - 1;


        if (pApp->GetStart () != pApp->GetFinish ())
        {
            COleDateTime dtAF (pApp->GetFinish ());
            int seconds = dtAF.GetHour () * 60* 60 + dtAF.GetMinute () * 60 + dtAF.GetSecond();

            rect.bottom = rectFill.top + (long) (m_nRowHeight * 
                (float)seconds / (float)(nTimeDelta)) - 1;

            if (rect.bottom == rect.top)
            {
                rect.bottom = rect.top + 1;
            }
        }
        else
        {
            rect.bottom = rect.top;
        }
    }

    pApp->SetRectDraw (rect, date);
}

void ScheduleViewStyleAlpha::AdjustAppointments ()
{
    const AppointmentArray& arQueryApps = GetScheduler()->GetQueryedAppointments ();

    const int nDays = GetViewDays ();

    if (//arQueryApps.GetSize () == 0|| 
        m_ViewRects.GetSize () != nDays)
    {
        return;
    }

    const int nTimeDelta = GetTimeDeltaInSeconds (GetTimeDelta ());
    const int yOffset = m_fScrollOffset * m_nRowHeight;

    COleDateTime date (m_DateBegin);

    const AppointmentArray& arApps = arQueryApps;

    for (int i = 0; i < arApps.GetSize (); i++)
    {
        Appointment* pApp = arApps[i];
        ASSERT_VALID (pApp);

        pApp->ResetDraw ();
    }

    COleDateTimeSpan spanDay (1, 0, 0, 0);

    int nDay = 0;

    for (nDay = 0; nDay < nDays; nDay ++)
    {

        const AppointmentArray& arApps = arQueryApps;

        int i;

        CRect rectFill (m_ViewRects [nDay]);
        rectFill.top -= yOffset;


        for (i = 0; i < arApps.GetSize (); i++)
        {
            Appointment* pApp = arApps[i];
            ASSERT_VALID (pApp);

            if (IsAppointmentInDate (*pApp, date))
            {
                CRect rtApp (rectFill);
                int width = rtApp.Width();
                rtApp.left  += width/6;
                rtApp.right -= width/6;

                AdjustAppointmentRectByDateTime (pApp, rtApp, date);
            }
        }

        date += spanDay;
    }

    // fill the other selected appointments rect

    CRect rectBegin (m_ViewRects [0]);
    rectBegin.top -= yOffset;

    POSITION pos = GetScheduler()->GetFirstSelectedAppointment();
    while (pos != NULL)
    {
        Appointment* pApp = GetScheduler()->GetNextSelectedAppointment(pos);
        ASSERT_VALID (pApp);

        if (pApp == NULL)
        {
            continue;
        }

        if (pApp->GetStart() < m_DateBegin || pApp->GetFinish() > m_DateEnd)
        {
            COleDateTime SelectDate = COleDateTime(pApp->GetStart().GetYear(), 
                pApp->GetStart().GetMonth(), pApp->GetStart().GetDay(), 0,0,0);
            COleDateTime EndTime = COleDateTime(pApp->GetFinish().GetYear(), 
                pApp->GetFinish().GetMonth(), pApp->GetFinish().GetDay(), 0,0,0);

            COleDateTimeSpan span = COleDateTimeSpan(1, 0, 0, 0);

            while (SelectDate <= EndTime)
            {
                CRect rtApp (rectBegin);
                int width = rtApp.Width();
                rtApp.left  += width/6;
                rtApp.right -= width/6;

                rtApp.OffsetRect((SelectDate - m_DateBegin).GetDays() * rectBegin.Width(), 0);

                AdjustAppointmentRectByDateTime (pApp, rtApp, SelectDate);

                SelectDate += span;
            }

        }
    }

    GetOpLayer()->SetRect(GetUnionRectOfSelectedAppointments());
}


void ScheduleViewStyleAlpha::DrawSelection(CDC* pDC)
{
    if(OPERATION_SELECT == m_OpMode)
    {
        CRect rect(GetScheduler()->m_ptOpStart, GetScheduler()->m_ptOpCurrent);

        CPen PenDot(PS_DOT, 1, RGB(0,0,0)); 
        CPen* pOldPen  = pDC->SelectObject(&PenDot);  

        // Draw lines here

        pDC->MoveTo (rect.left, rect.top);
        pDC->LineTo (rect.right, rect.top);
        pDC->MoveTo (rect.left, rect.bottom);
        pDC->LineTo (rect.right, rect.bottom);
        pDC->MoveTo (rect.left, rect.top);
        pDC->LineTo (rect.left, rect.bottom);
        pDC->MoveTo (rect.right, rect.top);
        pDC->LineTo (rect.right, rect.bottom);
        
        pDC->SelectObject(pOldPen);
    }
}


BOOL ScheduleViewStyleAlpha::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    int nPrevOffset = m_fScrollOffset;
	TRACE(_T("npos :%d \n"),nPos);
	 
    switch (nSBCode)
    {
    case SB_LINEUP:
        m_fScrollOffset -= m_visualRate;
        break;

    case SB_LINEDOWN:
        m_fScrollOffset += 1;
        break;

    case SB_TOP:
        m_fScrollOffset = 0;
        break;

    case SB_BOTTOM:
        m_fScrollOffset = m_fScrollTotal;
        break;

    case SB_PAGEUP:
        m_fScrollOffset -= m_fScrollPage;
        break;

    case SB_PAGEDOWN:
        m_fScrollOffset += m_fScrollPage;
        break;

    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        m_fScrollOffset = nPos ;
        break;

    default:
        return FALSE;
    }

    m_fScrollOffset = max (0, m_fScrollOffset);

    if (m_fScrollTotal == m_fScrollPage)
    {
        if (m_fScrollOffset > 0)
        {
            m_fScrollOffset = 1.0;
        }
    }
    else
    {
        m_fScrollOffset = min (m_fScrollOffset, m_fScrollTotal - m_fScrollPage + 1);
    }

    if (m_fScrollOffset == nPrevOffset)
    {
        return FALSE;
    }

    return ScheduleView::OnVScroll (nSBCode, nPos, pScrollBar);
}

void ScheduleViewStyleAlpha::ScrollToDate(COleDateTime Date)
{
    long Target = (Date.GetHour() * 60 + Date.GetMinute())*60 / ScheduleView::GetTimeDeltaInSeconds (GetTimeDelta ());

    m_fScrollOffset = max(0, Target - m_fScrollPage / 2);
}

void ScheduleViewStyleAlpha::RecauclateEnableVisibleOffset()
{
	//ScheduleView::GetTimeDeltaInSeconds (GetTimeDelta ())
	//According center point to caculate invisible offset val,to display appointment in right place.
	return;
	const ScheduleCtrl *pCtrl = GetScheduler();
	float fcenter = this->m_CenterTime ;
	
	const AppointmentArray &appArray = pCtrl->m_QueryAppsArray;
	if(appArray.GetCount() <= 0)
		return;
	//cauclate time display range.
	float fmin = 0xffffffff,fmax = 0.0;
	for(int i = 0;i < appArray.GetSize();i++)
	{
		Appointment *pApp = appArray.GetAt(i);
		float ftime = pApp->GetStart().GetHour() *3600 + pApp->GetStart().GetMinute() *60 + pApp->GetStart().GetSecond();

		if(fmin >ftime)
			fmin = ftime;
		
		ftime = pApp->GetFinish().GetHour() *3600 + pApp->GetFinish().GetMinute() *60 + pApp->GetFinish().GetSecond();
		if(fmax < ftime)
			fmax = ftime;
	}

	if((fcenter - fmin) < (fmax - fcenter))
		this->m_fScrollOffset = fmax;
	else
		this->m_fScrollOffset = fmin;

	ScheduleView::AdjustScrollSizes ();
}

void ScheduleViewStyleAlpha::AdjustScrollSizes ()
{
	if(NULL == m_CenterTime)
		this->ResetCenterPointTime();


    long nCount = 24 * 60 * 60/ ScheduleView::GetTimeDeltaInSeconds (GetTimeDelta ());
	//m_visualRate = nCount / 0x7ffff;
	m_visualRate = 1;//1 point = 1 pixel
	if(nCount > 0x7ffff)
	{
		m_visualRate =  (float)nCount / 0x7ffff;
		nCount = 0x7ffff;
	}
	m_fScrollPage  = m_rect4Appointments.Height () / m_nRowHeight;
    m_fScrollTotal = nCount;

    if( GetTimeDeltaInSeconds(GetTimeDelta()) != 3600 ) //autoscal mode can't support center point align.
    {
	    float fCenterRaw = this->m_CenterTime / ScheduleView::GetTimeDeltaInSeconds (GetTimeDelta ());
	    m_fScrollOffset = max(0,fCenterRaw - m_fScrollPage/2);
    }

	m_fScrollOffset = (m_fScrollTotal - m_fScrollOffset) < m_fScrollPage ?m_fScrollTotal - m_fScrollPage :m_fScrollOffset;
	//RecauclateOffsetVal();

    ScheduleView::AdjustScrollSizes ();
}

COleDateTime ScheduleViewStyleAlpha::GetDateFromPoint (const CPoint& point)
{
    COleDateTime date;

    int nDay = -1;

    const int nDuration = GetViewDays ();

    for (int i = 0; i < nDuration; i++)
    {
        CRect rt (m_ViewRects[i]);
        rt.right++;
        rt.bottom++;

        CPoint MyPoint(point);
        MyPoint.y = max(max(point.y, rt.top), min(point.y, rt.bottom));

        if (rt.PtInRect (MyPoint))
        {
            nDay = i;
            break;
        }
    }

    if (nDay != -1)
    {
        date = GetDateBegin() + COleDateTimeSpan (nDay, 0, 0, 0);
    }
    else
    {
        if (point.x <= m_ViewRects[0].left)
        {       
            date = GetDateBegin();
        }
        else
        {
            date = GetDateBegin() + COleDateTimeSpan (nDuration, 0, 0, 0);
        }
    }

    CRect rectBar = m_rectTimeBar;
    const int nHeaderHeight = m_nHeaderTodayHeight + m_nHeaderDayCaptionHeight;
    int y = rectBar.top + nHeaderHeight - m_fScrollOffset * m_nRowHeight - 1; 
 
    float Seconds = (float)((max(0,(point.y - y)) / (float)m_nRowHeight) * ScheduleView::GetTimeDeltaInSeconds (GetTimeDelta ()));

    date = date + COleDateTimeSpan (0, (int)(Seconds) / (60*60), (int)((Seconds) /60)%60,Seconds - (Seconds/60) *60  /*(int)((minites - (int)minites) * 60)*/);

    return date;
}

void ScheduleViewStyleAlpha::DrawOperationLayer (CDC* pDC, OperationLayer* pApp, OperationLayerApperance* pDS)
{
    if (m_pScheduleCtrl->GetSelectedAppointmentsCount()==0)
    {
        return;
    }

    CRect rectDs(pDS->m_Rect);

    if(OPERATION_DRAG == m_OpMode && m_HitTest == _SVIEW_HITTEST_APPOINTMENT_MOVE)
    {
        rectDs.OffsetRect(GetScheduler()->m_ptOpCurrent - GetScheduler()->m_ptOpStart);
    }

    CRect rectClip (0, 0, 0, 0);
    pDC->GetClipBox (rectClip);

    CRgn RgnClient;
    RgnClient.CreateRectRgn(m_rect4Appointments.left, m_rect4Appointments.top + 1, m_rect4Appointments.right, m_rect4Appointments.bottom);
    pDC->SelectClipRgn (&RgnClient, RGN_AND);

    CRect rect(rectDs);

    CPen PenDot(PS_DOT, 1, RGB(0,0,0)); 
    CPen* pOldPen  = pDC->SelectObject(&PenDot);  

    // Draw lines here

    pDC->MoveTo (rect.left, rect.top);
    pDC->LineTo (rect.right, rect.top);
    pDC->MoveTo (rect.left, rect.bottom);
    pDC->LineTo (rect.right, rect.bottom);
    pDC->MoveTo (rect.left, rect.top);
    pDC->LineTo (rect.left, rect.bottom);
    pDC->MoveTo (rect.right, rect.top);
    pDC->LineTo (rect.right, rect.bottom);

    pDC->SelectObject(pOldPen);

    // the below 4 operation will be implemented later, in professional version.

    //CRect MyButton(-c_HT_PREC,-c_HT_PREC,c_HT_PREC,c_HT_PREC);
    //MyButton.OffsetRect((rectDs.left + rectDs.right)/2, rectDs.top);
    //pDC->RoundRect(MyButton, CPoint(c_HT_PREC,c_HT_PREC));

    //MyButton.OffsetRect(0, rectDs.Height());
    //pDC->RoundRect(MyButton, CPoint(c_HT_PREC,c_HT_PREC));

    //MyButton.OffsetRect(-rectDs.Width()/2, -rectDs.Height()/2);
    //pDC->RoundRect(MyButton, CPoint(c_HT_PREC,c_HT_PREC));

    //MyButton.OffsetRect(rectDs.Width(), 0);
    //pDC->RoundRect(MyButton, CPoint(c_HT_PREC,c_HT_PREC));

    CRgn Rgn;
    Rgn.CreateRectRgn(rectClip.left, rectClip.top + 1, rectClip.right, rectClip.bottom);
    pDC->SelectClipRgn (&Rgn, RGN_COPY);


}

BOOL ScheduleViewStyleAlpha::IsPtInDefaultArea(const CPoint& point) const
{
    return m_rect4DefaultAppointment.PtInRect(point);
}

BOOL ScheduleViewStyleAlpha::IsPtInToday(const CPoint& point) const
{
    return m_rect4Today.PtInRect(point);
}


BOOL ScheduleViewStyleAlpha::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    BOOL isShiftPressed = (0x8000 & GetKeyState(VK_SHIFT)) != 0;

    const int nSecs = GetTimeDeltaInSeconds (GetTimeDelta ());

    
    return FALSE;
}
