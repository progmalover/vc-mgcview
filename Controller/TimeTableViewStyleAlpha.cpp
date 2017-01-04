#include "StdAfx.h"
#include "TimeTableViewStyleAlpha.h"
#include "Localization/Localization.h"
IMPLEMENT_DYNAMIC(TimeTableViewStyleAlpha, TimeTableView)

	TimeTableViewStyleAlpha::TimeTableViewStyleAlpha()
	: TimeTableView         ()
	, m_rectTimeBar            (0, 0, 0, 0)
	, m_CurrentTime            ()
{
	m_nRowHeight = 20;
}

TimeTableViewStyleAlpha::~TimeTableViewStyleAlpha()
{

}

void TimeTableViewStyleAlpha::OnPaint (CDC* pDC, const CRect& rectClient)
{
	ASSERT_VALID (pDC);

	VisualManager->OnFillPopupWindowBackground(pDC, rectClient);

    if (!m_pTimeTableCtrl->IsTimeTableOpened())
    {
        return;
    }

	OnDrawBackgound (pDC, m_rect4Appointments);
	OnDrawTimeBar (pDC, m_rectTimeBar, TRUE);

    OnDrawItems (pDC);

}

void TimeTableViewStyleAlpha::OnDrawBackgound (CDC* pDC, const CRect& rect)
{
	ASSERT_VALID (pDC);

	CRect rectFill (rect);

	//	const int nWeekStart = BORROW_PlannerManagerCtrl::GetFirstDayOfWeek () + 1;
	const int yOffset = m_nScrollOffset * m_nRowHeight;


	rectFill.OffsetRect (0, -yOffset);

	const int nSeconds = GetTimeDeltaInSeconds (GetTimeDelta ());
	const int nCount = 60*60 / nSeconds;

	CPen     penHour[2];
	CPen     penHalfHour[2];

	for (int i = 0; i < 2; i++)
	{
		penHour[i].CreatePen (PS_SOLID, 0,
			RGB(255, 255, 255));

		penHalfHour[i].CreatePen (PS_SOLID, 0, 
			RGB(255, 255, 255));
	}

	COLORREF colorFill = CLR_DEFAULT;
	BOOL bIsDefaultColor = colorFill == CLR_DEFAULT;

	COLORREF clrTextOld = pDC->SetTextColor (colorFill);

	COLORREF clrBkOld = pDC->SetBkColor (1 ? 
		RGB (255, 255, 255) : RGB (128, 128, 128));

	//VisualManager->OnFillPopupWindowBackground(pDC, rect);

	pDC->SetTextColor (clrTextOld);
	pDC->SetBkColor (clrBkOld);

	CBrush brHilite (VisualManager->GetToolbarHighlightColor());

	CPen penBlack (PS_SOLID, 0, VisualManager->GetToolbarDisabledTextColor());
	CPen* pOldPen = pDC->SelectObject (&penBlack);

	const int iStart = m_nScrollOffset;
	const int iEnd   = min (m_nScrollOffset + rect.Height () / m_nRowHeight + 
		(((rect.Height() % m_nRowHeight) / float(m_nRowHeight)) > 0.5 ? 1 : 0), nCount * 24);

	CPoint PtSelected(0,0);
	PtSelected.x = m_rect4Appointments.left + m_rect4Appointments.Width() / 2;
	PtSelected.y = m_nRowHeight * (GetTimeTable()->GetCurSelectedDate().GetHour() * 60 * 60
		+ GetTimeTable()->GetCurSelectedDate().GetMinute() * 60 
        + GetTimeTable()->GetCurSelectedDate().GetSecond())
        / TimeTableView::GetTimeDeltaInSeconds (GetTimeDelta ())
		- yOffset;

	rectFill = m_rect4Appointments;

	rectFill.left   += _TIMETABLE_APPOINTMENT_SPACE;
	rectFill.right  -= _TIMETABLE_APPOINTMENT_SPACE;
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
		}


		int nPenIndex = 0;//bIsWork ? 0 : 1;


		pDC->MoveTo (rectFill.left, rectFill.bottom);
		pDC->LineTo (rectFill.right, rectFill.bottom);

		rectFill.OffsetRect (0, m_nRowHeight);
	}


	pDC->SelectObject (&penBlack);

	
	/*CRect rectDurBar (m_rect4Appointments);

	rectDurBar.left   += _TIMETABLE_APPOINTMENT_SPACE;
	rectDurBar.right  -= _TIMETABLE_APPOINTMENT_SPACE;

	pDC->MoveTo (rectDurBar.right, rectDurBar.top);
	pDC->LineTo (rectDurBar.right, rectDurBar.bottom);


	pDC->SelectObject (pOldPen);*/
}

_TVIEW_TIME_DELTA TimeTableViewStyleAlpha::GetTimeDelta () const
{
	ASSERT_VALID(GetTimeTable ());
	return GetTimeTable ()->GetTimeDelta ();
}

void TimeTableViewStyleAlpha::OnDrawTimeBar (CDC* pDC, const CRect& rectBar, 
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

    const int nHeaderHeight = 0;

    int y = rectBar.top + nHeaderHeight - m_nScrollOffset * m_nRowHeight - 1; 

    CPen pen (PS_SOLID, 0, VisualManager->GetToolbarDisabledTextColor());
    CPen* pOldPen = pDC->SelectObject (&pen);

    const long nCount = 60*60 / TimeTableView::GetTimeDeltaInSeconds(GetTimeDelta ());

    UINT nFormat = DT_SINGLELINE | DT_RIGHT;

    if (nCount == 1)
    {
        nFormat |= DT_VCENTER;
    }

    CFont* pFont = &(const_cast<CTimeTableCtrl*>(GetTimeTable()))->GetSmallFont();
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
    CString strSeparator = TimeTableView::GetTimeSeparator ();

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
}

void TimeTableViewStyleAlpha::OnDrawTimeLine (CDC* pDC, const CRect& rect)
{
	ASSERT_VALID (pDC);

	pDC->MoveTo (rect.left, rect.bottom);
	pDC->LineTo (rect.right, rect.bottom);
}


void  TimeTableViewStyleAlpha::AdjustLayout (CDC* /*pDC*/, const CRect& rectClient)
{
	m_rect4Appointments = rectClient;

	const int nSeconds = TimeTableView::GetTimeDeltaInSeconds(GetTimeDelta ());
	const int nCount = 24 * 60 * 60 / nSeconds;

	m_rectTimeBar = rectClient;
	m_rectTimeBar.right = m_rectTimeBar.left + m_nRowHeight * (nCount == 24 ? 2 : 3) + 5;

	m_rect4Appointments.left = m_rectTimeBar.right;

	int nRow = rectClient.Height () / nCount;
	int nOldRowHeight = m_nRowHeight;

	if (nRow > m_nRowHeight)
	{
		m_nRowHeight = nRow;
	}


	nRow = m_rect4Appointments.Height ()/ nCount;

	if (nRow > nOldRowHeight)
	{
		m_nRowHeight = nRow;
	}

	AdjustScrollSizes ();
}



void TimeTableViewStyleAlpha::DrawSelection(CDC* pDC)
{
	if(_TOPERATION_SELECT == m_OpMode)
	{
		CRect rect(GetTimeTable()->m_ptOpStart, GetTimeTable()->m_ptOpCurrent);

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


BOOL TimeTableViewStyleAlpha::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nPrevOffset = m_nScrollOffset;

	switch (nSBCode)
	{
	case SB_LINEUP:
		m_nScrollOffset -= 1;
		break;

	case SB_LINEDOWN:
		m_nScrollOffset += 1;
		break;

	case SB_TOP:
		m_nScrollOffset = 0;
		break;

	case SB_BOTTOM:
		m_nScrollOffset = m_nScrollTotal;
		break;

	case SB_PAGEUP:
		m_nScrollOffset -= m_nScrollPage;
		break;

	case SB_PAGEDOWN:
		m_nScrollOffset += m_nScrollPage;
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_nScrollOffset = nPos;
		break;

	default:
		return FALSE;
	}

	m_nScrollOffset = max (0, m_nScrollOffset);

	if (m_nScrollTotal == m_nScrollPage)
	{
		if (m_nScrollOffset > 0)
		{
			m_nScrollOffset = 1;
		}
	}
	else
	{
		m_nScrollOffset = min (m_nScrollOffset, m_nScrollTotal - m_nScrollPage + 1);
	}

	if (m_nScrollOffset == nPrevOffset)
	{
		return FALSE;
	}

	return TimeTableView::OnVScroll (nSBCode, nPos, pScrollBar);
}

void TimeTableViewStyleAlpha::AdjustScrollSizes ()
{
	long nCount = 24 * 60 * 60 / TimeTableView::GetTimeDeltaInSeconds (GetTimeDelta ());

	m_nScrollPage  = m_rect4Appointments.Height () / m_nRowHeight  + 
		(((m_rect4Appointments.Height() % m_nRowHeight) / float(m_nRowHeight)) > 0.5 ? 1 : 0);
	m_nScrollTotal = nCount - 1;

	m_nScrollOffset = max(0, min (m_nScrollOffset, m_nScrollTotal - m_nScrollPage + 1));

	TimeTableView::AdjustScrollSizes ();
}

void TimeTableViewStyleAlpha::ScrollToDate(COleDateTime Date)
{
    long Target = (Date.GetHour() * 60 * 60 + Date.GetMinute() * 60 + Date.GetSecond()) 
        / TimeTableView::GetTimeDeltaInSeconds (GetTimeDelta ());

    m_nScrollOffset = max(0, Target - m_nScrollPage / 2);
}

BOOL TimeTableViewStyleAlpha::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL isShiftPressed = (0x8000 & GetKeyState(VK_SHIFT)) != 0;




	return FALSE;
}

CRect TimeTableViewStyleAlpha::GetItemRect(Appointment* pItem)
{
    CRect rectFill = m_rect4Appointments;
    rectFill.left = rectFill.left + m_rect4Appointments.Width()/6;
    rectFill.right = rectFill.right - m_rect4Appointments.Width()/6;

    COleDateTimeSpan spanDay (1, 0, 0, 0);
    const int nTimeDelta = GetTimeDeltaInSeconds (GetTimeDelta ());

    CRect rect(rectFill);

    rect.top = rectFill.top + (long)(m_nRowHeight * 
        float((pItem->GetStart ().GetHour () * 60 * 60 + 
        pItem->GetStart ().GetMinute () * 60 
        + pItem->GetStart ().GetSecond()) /  (float)nTimeDelta)) - 1;


    if (pItem->GetStart () != pItem->GetFinish ())
    {
        COleDateTime dtAF (pItem->GetFinish ());
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

    const int yOffset = m_nScrollOffset * m_nRowHeight;

    rect.OffsetRect(0, -yOffset);

    pItem->SetRectDraw (rect);

    return rect;
}

void TimeTableViewStyleAlpha::DrawAppointment (CDC* pDC, Appointment* pApp, BOOL bClipClient)
{
    ASSERT_VALID (pDC);
    ASSERT_VALID (pApp);

    if (pDC == NULL || pDC->GetSafeHdc () == NULL || pApp == NULL)
    {
        return;
    }

    CRect rectDS = GetItemRect(pApp);

    CFont* pFont = &(const_cast<CTimeTableCtrl*>(GetTimeTable()))->GetSmallBoldFont();
    ASSERT (pFont != NULL);

    CFont* pOldFont = pDC->SelectObject (pFont);

    if((bClipClient && _TOPERATION_DRAG == m_OpMode && pApp->IsSelected() &&
        m_HitTest == _TVIEW_HITTEST_APPOINTMENT_MOVE))
    {
        rectDS.OffsetRect(GetTimeTable()->m_ptOpCurrent - GetTimeTable()->m_ptOpStart);
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
    VisualManager->OnFillOutlookPageButton(pDC, rect, pApp->IsSelected(), FALSE, clrFrame1);

    if (pApp->GetImageList())
    {
        CRect IconRect(rectDS);

        IMAGEINFO info;
        if (pApp->GetImageList()->GetImageInfo(pApp->GetImageIndex(), &info))
        {
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
    if (pApp->IsSelected())
    {
        br.DeleteObject();
        br.CreateSolidBrush(RGB(255,0,0));
    }
    pDC->FrameRgn (&rgn, &br, nWidth, nWidth);


    if(bClipClient && OPERATION_COPY == m_OpMode && pApp->IsSelected() && m_HitTest == _TVIEW_HITTEST_APPOINTMENT_MOVE)
    {
        if (bClipClient)
        {
            pDC->SelectClipRgn (&RgnClient, RGN_OR);
        }

        int nWidth = 1;

        CRect CopyRect(rectDS);
        CPoint Offset = GetTimeTable()->m_ptOpCurrent - GetTimeTable()->m_ptOpStart;
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


void TimeTableViewStyleAlpha::OnDrawItems (CDC* pDC)
{
    const AppointmentList& ItemList = GetTimeTable()->m_ItemManager.m_ItemList;

    if (ItemList.GetSize () == 0)
    {
        return;
    }

    POSITION pos = ItemList.GetHeadPosition ();

    while (pos != NULL)
    {
        POSITION posNext = pos;

        Appointment* pItemNext = ItemList.GetNext (pos);

        if (!pItemNext->IsSelected())
        {
            pItemNext->OnDraw(pDC, this);
        }
    }

    pos = ItemList.GetHeadPosition ();

    while (pos != NULL)
    {
        POSITION posNext = pos;

        Appointment* pItemNext = ItemList.GetNext (pos);

        if (pItemNext->IsSelected())
        {
            pItemNext->OnDraw(pDC, this);
        }
    }
    
    return;
}


COleDateTime TimeTableViewStyleAlpha::GetDateFromPoint (const CPoint& point)
{
    COleDateTime date = COleDateTime(1983,3,17,0,0,0);

    CRect rectBar = m_rectTimeBar;
    const int nHeaderHeight = 0;
    int y = rectBar.top + nHeaderHeight - m_nScrollOffset * m_nRowHeight - 1; 

    int seconds = (int)((max(0,(point.y - y)) / (float)m_nRowHeight) * TimeTableView::GetTimeDeltaInSeconds (GetTimeDelta ()));

    date = date + COleDateTimeSpan (0, (int)(seconds) / 3600, (int)((seconds) % 3600)/60, (int)(seconds % 60));

    return date;
}