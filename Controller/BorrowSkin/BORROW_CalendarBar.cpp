#include "stdafx.h"
#include <math.h>
#include <locale.h>
#include "Localization/Localization.h"
#include "BORROW_Pro.h"
#include "BORROW_CalendarBar.h"
#include "BORROW_CalendarMenuButton.h"
#include "BORROW_DateTimeCtrl.h"
#include "ControllerVisualManager.h"

int hcurSelectRow = 0;

#pragma warning (disable : 4706)
#ifdef _AFXDLL
#define COMPILE_MULTIMON_STUBS
#endif // _AFXDLL

#include "multimon.h"
#pragma warning (default : 4706)
//#include "resource.h"


// #include "BCGPDateTimeCtrl.h"
// #include "BCGPLocalResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT BORROW_CALENDAR_ON_SELCHANGED = ::RegisterWindowMessage (_T("BCGM_CALENDAR_ON_SELCHANGED"));

#define DAYS_IN_WEEK		7
#define ID_TODAY_BUTTON		101

static const int iSlowTimerDuration = 300;
static const int iFastTimerDuration = 100;
static const int iNumberOfSlowTimes = 10;
static const int iIdClickButtonEvt = 1;

static const int iFirstAllowedYear = 100;	// COleDateTime limitation
static const int iLastAllowedYear = 9999;	// COleDateTime limitation

static void ClearTime (COleDateTime& date)
{
	int nYear = date.GetYear ();
	int nMonth = date.GetMonth ();
	int nDay = date.GetDay ();

	date = COleDateTime (nYear, nMonth, nDay, 0, 0, 0);
}

static CString myGetDayName(int dayofweek)
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
/////////////////////////////////////////////////////////////////////////////
// CBCGPCalendar

CBORROW_Calendar::CBORROW_Calendar()
{
	COleDateTime today = COleDateTime::GetCurrentTime ();
	m_nStartMonth = today.GetMonth ();
	m_nStartYear = today.GetYear ();

	m_bMultipleSelection = FALSE;

	SetDate (today);

	m_sizeCalendar = CSize (0, 0);
	m_bSingleMonth = FALSE;
	m_nCalendarsInRow = 0;
	m_nMonths = 0;
	m_sizeBox = CSize (0, 0);

	m_nFirstDayOfWeek = 0;
	m_arWeekDays.SetSize (DAYS_IN_WEEK);

	m_rectBtnPrev.SetRectEmpty ();
	m_rectBtnNext.SetRectEmpty ();

	m_bSlowTimerMode = FALSE;
	m_iSlowTimerCount = 0;

	m_bIsTimerNext = FALSE;
	m_rectTimer.SetRectEmpty ();

	m_bTodayButton = FALSE;
	m_bWeekNumbers = FALSE;
	m_bTruncateSelection = FALSE;
	m_nMaxSelDates = -1;

#ifndef BCGP_EXCLUDE_PLANNER
	m_pWndPlanner = NULL;
#endif // BCGP_EXCLUDE_PLANNER
	m_rectDrag.SetRectEmpty ();

	m_dateStartDrag = COleDateTime ();
	m_dateTrack = COleDateTime ();
	m_bSelectWeekMode = FALSE;
	m_bSelectTruncate = FALSE;

	m_bSelChanged = FALSE;

	m_nVertMargin = 2;
	m_nHorzMargin = 2;

	m_nDaysHorzMarginLeft = 0;
	m_nDaysHorzMarginRight = 0;

	m_pParentBtn = NULL;

	m_rectDays.SetRectEmpty ();
	m_hWndMonthPicker = NULL;

	m_bScrollSelection = FALSE;
	m_bIsPopup = FALSE;

	m_bGradientFillCaption = FALSE;
}

CBORROW_Calendar::~CBORROW_Calendar()
{
}


BEGIN_MESSAGE_MAP(CBORROW_Calendar, CWnd)
	//{{AFX_MSG_MAP(CBCGPCalendar)
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_CANCELMODE()
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_TODAY_BUTTON, OnToday)
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBCGPCalendar message handlers

void CBORROW_Calendar::OnNcPaint() 
{
	if (GetStyle () & WS_BORDER)
	{
		visualManager->OnDrawControlBorder (this);
	}
}

void CBORROW_Calendar::OnPaint() 
{
	CPaintDC dcPaint (this); // device context for painting
	OnDraw(&dcPaint);
}

void CBORROW_Calendar::OnDraw(CDC* pDCPaint) 
{
	visualManager->GetCalendarColors (this, m_Colors);

	CMemDC memDC (*pDCPaint, this);
	CDC* pDC = &memDC.GetDC ();

	CRect rectClip;
	pDC->GetClipBox (rectClip);

	CRect rectClient;
	GetClientRect (&rectClient);

	pDC->FillRect (rectClient, &afxGlobalData.brWindow);

	pDC->SetBkMode (TRANSPARENT);
	pDC->SetTextColor (afxGlobalData.clrWindowText);
	
	CFont* pOldFont = pDC->SelectObject (&afxGlobalData.fontRegular);
	ASSERT (pOldFont != NULL);

	CRect rectInter;

	for (int nMonth = 0; nMonth < m_nMonths; nMonth++)
	{
		CRect rectCalendar;
		GetMonthRect (nMonth, rectCalendar);

		if (rectInter.IntersectRect (rectCalendar, rectClip))
		{
			OnDrawMonth (pDC, rectCalendar, nMonth);
		}
	}

	//------------------------
	// Draw prev/text buttons:
	//------------------------
	CMenuImages::IMAGE_STATE imageState;

	if (GetRValue (m_Colors.clrCaptionText) > 192 &&
		GetGValue (m_Colors.clrCaptionText) > 192 &&
		GetBValue (m_Colors.clrCaptionText) > 192)
	{
		imageState = CMenuImages::ImageWhite;
	}
	else
	{
		imageState = CMenuImages::ImageBlack;
	}

	CMenuImages::Draw (pDC, CMenuImages::IdArrowLeftLarge, 
			m_rectBtnPrev, imageState);

	CMenuImages::Draw (pDC, CMenuImages::IdArrowRightLarge, 
			m_rectBtnNext, imageState);

	pDC->SelectObject (pOldFont);
}

void CBORROW_Calendar::PostNcDestroy() 
{
	CWnd::PostNcDestroy();

	if (m_bIsPopup)
	{
		delete this;
	}
}

void CBORROW_Calendar::GetDayRects (int nMonthIndex, CRect rects [42], 
									  int& xStart, int& nDaysInPrevMonth) const
{
	CRect rect;
	GetMonthRect (nMonthIndex, rect);

	rect.top += m_sizeBox.cy * 2 + 4;
	rect.DeflateRect (m_nDaysHorzMarginLeft, 0, m_nDaysHorzMarginRight, 0);

	const BOOL bDrawPrevMonth = (nMonthIndex == 0);
	const BOOL bDrawNextMonth = (nMonthIndex == m_nMonths - 1);

	const COleDateTime date = GetMonthDate (nMonthIndex);
	const int nDaysInMonth = GetMaxMonthDay (date);
	const COleDateTime today = COleDateTime::GetCurrentTime ();
	const int nFirstWeekDay = date.GetDayOfWeek ();
	
	nDaysInPrevMonth = 0;
	if (bDrawPrevMonth)
	{
		int nYear = date.GetYear ();

		int nMonth = date.GetMonth () - 1;
		if (nMonth == 0)
		{
			nMonth = 12;
			nYear--;
		}

		nDaysInPrevMonth = GetMaxMonthDay (nMonth, nYear);
	}

	xStart = (nFirstWeekDay - m_nFirstDayOfWeek + 6) % 7;

	int y = 0;
	int x = 0;

	for (int i = 0; i < 42; i++, x++)
	{
		int nDay = i + 1 - xStart;

		rects [i] = CRect (0, 0, 0, 0);

		BOOL bIsEmptyRect = FALSE;

		if (nDay < 1)
		{
			bIsEmptyRect = !bDrawPrevMonth;
		}
		else if (nDay > nDaysInMonth)
		{
			bIsEmptyRect = !bDrawNextMonth;
		}

		if (x == DAYS_IN_WEEK)
		{
			x = 0;
			y++;
		}

		if (!bIsEmptyRect)
		{
			rects [i] = CRect (
				CPoint (rect.left + x * m_sizeBox.cx,
						rect.top + y * m_sizeBox.cy),
				m_sizeBox);
		}
	}
}

extern CString myGetMonthName(int month);
void CBORROW_Calendar::OnDrawMonth (CDC* pDC, CRect rect, int nMonthIndex)
{
	ASSERT_VALID (pDC);

	const BOOL bCtrl = ::GetAsyncKeyState (VK_CONTROL) & 0x8000;

	UINT dtFlags = DT_VCENTER | DT_SINGLELINE;

	if (m_bSingleMonth)
	{
		dtFlags |= DT_CENTER;
	}
	else
	{
		dtFlags |= DT_RIGHT;
	}

	CBrush brSelected (m_Colors.clrSelected);

	CRect rectClip;
	pDC->GetClipBox (rectClip);

	int i = 0;

	const COleDateTime date = GetMonthDate (nMonthIndex);
	const int nDaysInMonth = GetMaxMonthDay (date);
	const COleDateTime today = COleDateTime::GetCurrentTime ();

	const BOOL bDrawNextMonth = (nMonthIndex == m_nMonths - 1);

	//---------------------------------------
	// Draw month + year + prev/next buttons:
	//---------------------------------------
	CRect rectCaption = rect;
	rectCaption.bottom = rectCaption.top + m_sizeBox.cy + 2;

	if (!m_bSingleMonth)
	{
		rectCaption.DeflateRect (1, 0);
		rectCaption.top++;
	}
	else
	{
		CRect rectClient;
		GetClientRect (rectClient);

		rectCaption.left = rectClient.left + m_nHorzMargin;
		rectCaption.right = rectClient.right - m_nHorzMargin;
		rectCaption.top += m_nVertMargin;
	}

	if (m_bGradientFillCaption)
	{
		COLORREF clrLight;
		COLORREF clrDark;
		
		if (GetRValue (m_Colors.clrCaption) > 192 &&
			GetGValue (m_Colors.clrCaption) > 192 &&
			GetBValue (m_Colors.clrCaption) > 192)
		{
			clrLight = m_Colors.clrCaption;
			clrDark = CDrawingManager::PixelAlpha (clrLight, 80);
		}
		else
		{
			clrDark = m_Colors.clrCaption;
			clrLight = CDrawingManager::PixelAlpha (clrDark, 120);
		}

		CDrawingManager dm (*pDC);
		dm.FillGradient (rectCaption, clrDark, clrLight, TRUE);
	}
	else
	{
		CBrush br (m_Colors.clrCaption);
		pDC->FillRect (rectCaption, &br);
	}

	rectCaption.right -= 2;

	pDC->SetTextColor (m_Colors.clrCaptionText);

	{
		CBORROW_DefaultLocale dl;
		//CString strYearMonth = date.Format(_T("%B %Y"));
		//CString strMonth = date.Format(_T("%B"));
		//CString strYearMonth = Translate(strMonth);
		
		CString strYearMonth = myGetMonthName(date.GetMonth());
		strYearMonth += _T(" ");
		strYearMonth += date.Format(_T("%Y"));


		pDC->DrawText (strYearMonth, rectCaption, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}

	rect.DeflateRect (m_nDaysHorzMarginLeft, 0, m_nDaysHorzMarginRight, 0);
	rect.top = rectCaption.bottom;

	//----------------
	// Draw week days:
	//----------------
	CRect rectWeeks = rect;
	rectWeeks.bottom = rectWeeks.top + m_sizeBox.cy + 1;

	for (i = 0; i < DAYS_IN_WEEK; i++)
	{
		CRect rectWeekDay = rectWeeks; 
		rectWeekDay.left = rectWeeks.left + m_sizeBox.cx * i - 1;
		rectWeekDay.right = rectWeekDay.left + m_sizeBox.cx;

		pDC->DrawText (m_arWeekDays [i], rectWeekDay, dtFlags);
	}

	CPen pen (PS_SOLID, 1, afxGlobalData.clrBtnShadow);
	CPen* pOldPen = pDC->SelectObject (&pen);

	int yLine = rectWeeks.bottom - 1;
	const int xLineLeft = rect.left;
	const int xLineRight = rect.right + 2;

	pDC->MoveTo (xLineLeft, yLine);
	pDC->LineTo (xLineRight, yLine);

	rect.top = rectWeeks.bottom + 2;

	BOOL bPrevMarked = FALSE;
	CFont* pOldFont = NULL;

	CRect rectInter;

	CRect rectWeekNums;
	rectWeekNums.SetRectEmpty ();

	//-------------------
	// Draw week numbers:
	//-------------------
	if (m_bWeekNumbers)
	{
		rectWeekNums = rect;
		rectWeekNums.right = rect.left;
		rectWeekNums.left = rectWeekNums.right - m_sizeBox.cx;
	}

	//-----------
	// Draw days:
	//-----------
	CRect rects [42];
	int xStart = 0;
	int nDaysInPrevMonth = 0;

	GetDayRects (nMonthIndex, rects, xStart, nDaysInPrevMonth);

	int nDayOffset = 1 - xStart;
	int yLineBottom = -1;
	int nWeeks = 0;

	COleDateTime date1;
	COleDateTime date2;
	if (m_dateStartDrag != COleDateTime ())
	{
		date1 = min (m_dateTrack, m_dateStartDrag);
		date2 = max (m_dateTrack, m_dateStartDrag);

		BOOL bSelectTruncate = m_bSelectWeekMode ||
							(m_bTruncateSelection && (date2 - date1) > COleDateTimeSpan(6, 0, 0, 0));

		if (bSelectTruncate)
		{
			date1 = GetFirstWeekDay (date1);
			date2 = GetFirstWeekDay (date2) + COleDateTimeSpan (6, 0, 0, 0);
		}
	}

	for (i = 0; i < 42; i++)
	{
		CRect rectDay = rects [i];
		if (rectDay.IsRectEmpty ())
		{
			continue;
		}

		if (!rectInter.IntersectRect (rectDay, rectClip))
		{
			continue;
		}

		pDC->SetTextColor (afxGlobalData.clrWindowText);

		int nDay = nDayOffset + i;
		BOOL bIsCurrMonth = TRUE;

		if (nDay < 1)
		{
			bIsCurrMonth = FALSE;
			nDay = nDaysInPrevMonth + nDay;
		}
		else if (nDay > nDaysInMonth)
		{
			if (!bDrawNextMonth)
			{
				break;
			}

			nDay -= nDaysInMonth;
			bIsCurrMonth = FALSE;
		}

		BOOL bIsSelected = FALSE;

		if (bIsCurrMonth)
		{
			COleDateTime dateCurr (date.GetYear (), date.GetMonth (), nDay, 0, 0, 0);
			
			bIsSelected = IsDateSelected (dateCurr);

			if (m_dateStartDrag != COleDateTime ())
			{
				if (date1 <= dateCurr && dateCurr <= date2)
				{
					if (bCtrl && bIsSelected)
					{
						bIsSelected = FALSE;
					}
					else
					{
						bIsSelected = TRUE;
					}
				}
			}

			COLORREF clrDay = GetDateColor (dateCurr);
			if (clrDay != (COLORREF)-1)
			{
				pDC->SetTextColor (clrDay);
			}
		}

		if (!bIsCurrMonth && !bIsSelected)
		{
			pDC->SetTextColor (afxGlobalData.clrGrayedText);
		}

		CRect rectText = rectDay;

		if (!m_bSingleMonth)
		{
			rectText.OffsetRect (-1, 0);
		}

		if (bIsSelected)
		{
			pDC->FillRect (rectDay, &brSelected);

			if (m_Colors.clrSelectedText != afxGlobalData.clrWindowText)
			{
				pDC->SetTextColor (m_Colors.clrSelectedText);
			}
		}

		CString strDay;
		strDay.Format (_T("%d"), nDay);

		BOOL bIsMarked = 
			bIsCurrMonth && 
			IsDateMarked (date.GetYear (), date.GetMonth (), nDay);

		if (bIsMarked)
		{
			if (!bPrevMarked)
			{
				pOldFont = pDC->SelectObject (&afxGlobalData.fontBold);
			}
		}
		else
		{
			if (bPrevMarked)
			{
				ASSERT_VALID (pOldFont);
				pDC->SelectObject (pOldFont);
				pOldFont = NULL;
			}
		}

		pDC->DrawText (strDay, rectText, dtFlags);

		bPrevMarked = bIsMarked;

		if (bIsCurrMonth && date.GetYear () == today.GetYear () &&
			date.GetMonth () == today.GetMonth () &&
			nDay == today.GetDay ())
		{
			pDC->Draw3dRect (rectDay, m_Colors.clrTodayBorder, m_Colors.clrTodayBorder);
		}

		if (rectDay.bottom > yLineBottom)
		{
			nWeeks++;
		}

		yLineBottom = rectDay.bottom;
	}

	if (pOldFont != NULL)
	{
		pDC->SelectObject (pOldFont);
	}

	//-------------------
	// Draw week numbers:
	//-------------------
	if (m_bWeekNumbers)
	{
		pOldFont = pDC->SelectObject (&afxGlobalData.fontSmall);

		pDC->MoveTo (xLineLeft - 1, yLine);

		pDC->SetTextColor (afxGlobalData.clrGrayedText);

		for (i = 0; i < nWeeks; i++)
		{
			CRect rectNum = rectWeekNums;
			rectNum.top += m_sizeBox.cy * i;
			rectNum.bottom = rectNum.top + m_sizeBox.cy;

			int nWeekNum = m_arStartWeekInMonth [nMonthIndex] + i;
			if (nWeekNum == 54)
			{
				nWeekNum = 1;
			}

			CString strNum;
			strNum.Format (_T("%d"), nWeekNum);

			if (!m_bSingleMonth)
			{
				rectNum.right -= 4;
			}

			pDC->DrawText (strNum, rectNum, dtFlags);

			if (!m_bSingleMonth)
			{
				pDC->LineTo (xLineLeft - 1, rectNum.bottom);
			}
		}

		pDC->SelectObject (pOldFont);
	}

	if (m_bTodayButton && yLineBottom >= 0 && 
		(m_bSingleMonth || GetStyle () & WS_POPUP))
	{
		pDC->MoveTo (xLineLeft, yLineBottom);
		pDC->LineTo (xLineRight, yLineBottom);
	}

	pDC->SelectObject (pOldPen);
}

void CBORROW_Calendar::OnSelectionChanged ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

#ifndef _BCGPCALENDAR_STANDALONE
	CBORROW_CalendarBar* pParentBar = DYNAMIC_DOWNCAST (CBORROW_CalendarBar, GetParent ());
	if (pParentBar != NULL)
	{
		ASSERT_VALID (pParentBar);
		pParentBar->OnSelectionChanged ();
		return;
	}
#endif

	CWnd* pOwner = GetOwner ();
	if (pOwner == NULL)
	{
		return;
	}

	ASSERT_VALID (pOwner);

	pOwner->SendMessage (BORROW_CALENDAR_ON_SELCHANGED,
		0, (LPARAM) GetSafeHwnd ());
}

void CBORROW_Calendar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);

	if (GetStyle() & WS_BORDER) 
	{
		lpncsp->rgrc[0].left++; 
		lpncsp->rgrc[0].top++ ;
		lpncsp->rgrc[0].right--;
		lpncsp->rgrc[0].bottom--;
	}
}

BOOL CBORROW_Calendar::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	CString strClassName = afxGlobalData.RegisterWindowClass (_T("BCGPCalendar"));

	if (dwStyle & WS_POPUP)
	{
		dwStyle |= WS_BORDER;
		m_bIsPopup = TRUE;

		m_pParentBtn = DYNAMIC_DOWNCAST (CBORROW_DateTimeCtrl, pParentWnd);

		return CWnd::CreateEx (WS_EX_PALETTEWINDOW, 
								strClassName, 
								_T (""),
								dwStyle, 
								rect,
								NULL,
								0);
	}

	return CWnd::Create (strClassName, _T (""),
							dwStyle, 
							rect,
							pParentWnd,
							nID);
}

BOOL CBORROW_Calendar::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}

void CBORROW_Calendar::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	RecalcLayout ();
}

BOOL CBORROW_Calendar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
#ifndef _BCGSUITE_
	if (m_bWeekNumbers && m_bMultipleSelection)
	{
		CPoint ptCursor;
		::GetCursorPos (&ptCursor);
		ScreenToClient (&ptCursor);

		if (HitTestWeekNum (ptCursor, TRUE) != COleDateTime ())
		{
			if (hcurSelectRow == NULL)
			{
				CBORROW_LocalResource locaRes;
				// TODO: add resource
                // hcurSelectRow = AfxGetApp ()->LoadCursor (IDC_BCGBARRES_SELECT_ROW);
			}

			//::SetCursor (hcurSelectRow);
			return TRUE;
		}
	}
#endif

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CBORROW_Calendar::SetSingleMonthMode (BOOL bSingleMonth)
{
	m_bSingleMonth = bSingleMonth;
	RecalcLayout ();
}

void CBORROW_Calendar::SetDate (const COleDateTime& date)
{
	SelectDate (date);
}

COleDateTime CBORROW_Calendar::GetDate () const
{
	if (m_SelectedDates.IsEmpty ())
	{
		return COleDateTime ();
	}

	POSITION pos = m_SelectedDates.GetStartPosition ();
	
	DATE date;
	BOOL bFlag;

	m_SelectedDates.GetNextAssoc (pos, date, bFlag);

	return (COleDateTime) date;
}

void CBORROW_Calendar::RecalcLayout (BOOL bRedraw)
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	int i;

	CClientDC dc (this);
	CFont* pOldFont = dc.SelectObject (&afxGlobalData.fontRegular);
	ASSERT (pOldFont != NULL);

	CRect rectClient;
	GetClientRect (&rectClient);

	if (m_bTodayButton)
	{
		m_btnToday.SetFont (&afxGlobalData.fontRegular);

		CString strToday;
		m_btnToday.GetWindowText (strToday);

		CSize sizeToday = dc.GetTextExtent (strToday) + CSize (10, 8);
		
		int xToday = rectClient.CenterPoint ().x - sizeToday.cx / 2;
		int yToday = rectClient.bottom - sizeToday.cy - 4;

		m_btnToday.SetWindowPos (NULL, xToday, yToday, sizeToday.cx, sizeToday.cy,
			SWP_NOZORDER | SWP_NOACTIVATE);

		rectClient.bottom = yToday - 4;

		m_btnToday.EnableWindow ();
	}
	else
	{
		m_btnToday.ShowWindow (SW_HIDE);
		m_btnToday.EnableWindow (FALSE);
	}

	m_rectDays = rectClient;

	int iMaxDigitWidth = 0;
	int iMaxDigitHeight = 0;

	for (i = 0; i < 10; i ++)
	{
		CString strDigit;
		strDigit.Format (_T("%d"), i);

		CSize sizeDigit = dc.GetTextExtent (strDigit);

		iMaxDigitWidth = max (iMaxDigitWidth, sizeDigit.cx);
		iMaxDigitHeight = max (iMaxDigitHeight, sizeDigit.cy);
	}

	dc.SelectObject (&afxGlobalData.fontSmall);

	CString strWeekNum = _T("99");
	m_nDaysHorzMarginLeft = dc.GetTextExtent (strWeekNum).cx + iMaxDigitWidth;
	
	m_nDaysHorzMarginRight = (iMaxDigitWidth + 1) * 2;

	m_sizeBox = CSize (iMaxDigitWidth * 2 + 5, iMaxDigitHeight + 2);

	if (m_bSingleMonth)
	{
		m_sizeBox.cx = min (3 * m_sizeBox.cx / 2,
			(rectClient.Width () - 2 * m_nHorzMargin - m_nDaysHorzMarginLeft - m_nDaysHorzMarginRight) / DAYS_IN_WEEK);

		m_sizeBox.cy = min (3 * m_sizeBox.cy / 2,
			(rectClient.Height () - 2 * m_nVertMargin) / 8);
	}

	m_sizeCalendar = CSize (
		DAYS_IN_WEEK * m_sizeBox.cx + m_nDaysHorzMarginLeft + m_nDaysHorzMarginRight,
		8 * m_sizeBox.cy + 5);

	m_nCalendarsInRow = m_bSingleMonth ? 1 : max (1, rectClient.Width () / m_sizeCalendar.cx);

	int nCalendarsInColumn = m_bSingleMonth ? 1 : max (1, rectClient.Height () / m_sizeCalendar.cy);
	m_nMonths = m_nCalendarsInRow * nCalendarsInColumn;

	dc.SelectObject (pOldFont);

	if (m_bTodayButton)
	{
		if (m_sizeCalendar.cy > rectClient.Height ())
		{
			m_btnToday.ShowWindow (SW_HIDE);
			m_btnToday.EnableWindow (FALSE);
		}
		else
		{
			m_btnToday.ShowWindow (SW_SHOWNOACTIVATE);
		}
	}

	//----------------
	// Fill week days:
	//----------------
	{
		COleDateTimeSpan day (1, 0, 0, 0);
		CBORROW_DefaultLocale dl;


		for (int iWeekDay = 0; iWeekDay < DAYS_IN_WEEK; iWeekDay ++)
		{
			/*COleDateTime dateTmp = COleDateTime::GetCurrentTime ();
			int iDay = (iWeekDay + m_nFirstDayOfWeek) % DAYS_IN_WEEK;

			while (dateTmp.GetDayOfWeek () != iDay + 1)
			{
				dateTmp += day;
			}

			CString strWeekDay = dateTmp.Format (_T ("%a"));

			m_arWeekDays.SetAt (iWeekDay, strWeekDay);*/
			m_arWeekDays.SetAt (iWeekDay, myGetDayName(iWeekDay));
		}

        int iDifferenceChar = 0;

        for(int i=0; i<m_arWeekDays[0].GetLength() && i<m_arWeekDays[1].GetLength(); i++)
        {
            if(m_arWeekDays[0].GetAt(i) != m_arWeekDays[1].GetAt(i))
            {
                iDifferenceChar = i;
                break;
            }
        }

        for (int iWeekDay = 0; iWeekDay < DAYS_IN_WEEK; iWeekDay ++)
		{
            m_arWeekDays[iWeekDay] = m_arWeekDays[iWeekDay].Mid(iDifferenceChar, 1);
		}
	}

	//--------------------------------
	// Set prev/text button locations:
	//--------------------------------
	GetMonthRect (0, m_rectBtnPrev);

	m_rectBtnPrev.right = m_rectBtnPrev.left + m_sizeBox.cx;
	m_rectBtnPrev.bottom = m_rectBtnPrev.top + m_sizeBox.cy + 2;

	GetMonthRect (m_nCalendarsInRow - 1, m_rectBtnNext);

	m_rectBtnNext.left = m_rectBtnNext.right - m_sizeBox.cx;
	m_rectBtnNext.bottom = m_rectBtnNext.top + m_sizeBox.cy + 2;

	m_dateFirst = COleDateTime (m_nStartYear, m_nStartMonth, 1, 0, 0, 0);
	
	COleDateTime dateLast = GetMonthDate (m_nMonths - 1);
	m_dateLast = COleDateTime (dateLast.GetYear (), dateLast.GetMonth (), 
		GetMaxMonthDay (dateLast), 23, 59, 59);

	if (!m_SelectedDates.IsEmpty () && m_bScrollSelection)
	{
		COleDateTime date = GetDate ();

		BOOL bSelectTruncate = FALSE;
		BOOL bSelectWeek     = FALSE;
		BOOL bNeedShift      = FALSE;
		if (m_bTruncateSelection && m_bMultipleSelection)
		{
			CList<DATE, DATE&> lstDates;
			GetSelectedDates (lstDates);

			COleDateTime date1;
			COleDateTime date2;

			GetMinMaxSelection (lstDates, date1, date2);

			COleDateTimeSpan delta (date2 - date1);
			bSelectTruncate = GetFirstWeekDay (date1) == date1 &&
				(delta >= COleDateTimeSpan (6, 0, 0, 0) ||
				 delta == COleDateTimeSpan (4, 0, 0, 0));

			if (bSelectTruncate)
			{
				bSelectWeek = delta == COleDateTimeSpan (6, 0, 0, 0) ||
							  delta == COleDateTimeSpan (4, 0, 0, 0);
				date = date1;

				bNeedShift = date1 < m_dateFirst || date2 > m_dateLast;
			}
		}

		COleDateTime dateOld = date;

		int nYear  = date.GetYear ();
		int nMonth = date.GetMonth ();
		int nDay   = date.GetDay ();

		if (date < m_dateFirst)
		{
			if (bSelectTruncate)
			{
				date = GetFirstWeekDay (m_dateFirst);
				
				if (bSelectWeek)
				{
					dateOld = GetFirstWeekDay (COleDateTime(nYear, nMonth, 1, 0, 0, 0));
				}
			}
			else
			{
				while (date < m_dateFirst)
				{
					nMonth++;
					if (nMonth > 12)
					{
						nMonth = 1;
						nYear++;
					}

					date = COleDateTime (nYear, nMonth, 
						min (nDay, GetMaxMonthDay (nMonth, nYear)),
						0, 0, 0);
				}
			}

			ShiftSelection (date - dateOld);
		}
		else if (date > m_dateLast || bNeedShift)
		{
			if (bSelectTruncate)
			{
				date = GetFirstWeekDay (
						COleDateTime (m_dateLast.GetYear (), m_dateLast.GetMonth (), 1, 0, 0, 0));
		
				if (bSelectWeek)
				{
					dateOld = GetFirstWeekDay (COleDateTime(nYear, nMonth, 1, 0, 0, 0));
				}
			}
			else
			{
				while (date > m_dateLast)
				{
					nMonth--;
					if (nMonth == 0)
					{
						nMonth = 12;
						nYear--;
					}

					date = COleDateTime (nYear, nMonth, 
						min (nDay, GetMaxMonthDay (nMonth, nYear)),
						0, 0, 0);
				}
			}

			ShiftSelection (date - dateOld);
		}
	}

	if (m_bWeekNumbers)
	{
		m_arStartWeekInMonth.SetSize (m_nMonths);
		
		for (i = 0; i < m_nMonths; i++)
		{
			COleDateTime date = GetMonthDate (i);
			COleDateTime jan1 (date.GetYear(), 1, 1, 0, 0, 0);

			int nOffset1 = (jan1.GetDayOfWeek () + 6 - m_nFirstDayOfWeek) % 7 + 1;
			int nOffset2 = (date.GetDayOfWeek () + 6 - m_nFirstDayOfWeek) % 7 + 1;

			m_arStartWeekInMonth [i] = (date.GetDayOfYear () - (nOffset2 - nOffset1)) / 7 + 1;
		}
	}
	else
	{
		m_arStartWeekInMonth.SetSize (0);
	}

	if (bRedraw)
	{
		RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}
}

BOOL CBORROW_Calendar::GetMonthRect (int nMonthIndex, CRect& rect) const
{
	if (nMonthIndex < 0 || nMonthIndex >= m_nMonths)
	{
		rect.SetRectEmpty ();
		return FALSE;
	}

	CRect rectClient;
	GetClientRect (&rectClient);

	const int xMargin = max (0, 
		(rectClient.Width () - m_sizeCalendar.cx * m_nCalendarsInRow) / 2);

	const int x = nMonthIndex % m_nCalendarsInRow;
	const int y = nMonthIndex / m_nCalendarsInRow;

	rect = CRect (
		CPoint (rectClient.left + x * m_sizeCalendar.cx + xMargin,
				rectClient.top + y * m_sizeCalendar.cy),
		m_sizeCalendar);

	return TRUE;
}

int CBORROW_Calendar::GetMonthIndex (int nYear, int nMonth) const
{
	int nCurrMonth = m_nStartMonth;
	int nCurrYear = m_nStartYear;

	for (int i = 0; i < m_nMonths; i++)
	{
		if (nMonth == nCurrMonth && nYear == nCurrYear)
		{
			return i;
		}

		nCurrMonth++;
		if (nCurrMonth > 12)
		{
			nCurrMonth = 1;
			nCurrYear++;
		}
	}

	return -1;
}

int CBORROW_Calendar::GetMonthRect (int nYear, int nMonth, CRect& rect) const
{
	rect.SetRectEmpty ();

	int nMonthIndex = GetMonthIndex (nYear, nMonth);
	if (nMonthIndex < 0 || !GetMonthRect (nMonthIndex, rect))
	{
		return -1;
	}

	return nMonthIndex;
}

BOOL CBORROW_Calendar::GetDateRect (int nYear, int nMonth, int nDay, CRect& rect) const
{
	rect.SetRectEmpty ();

	int nMonthIndex = GetMonthIndex (nYear, nMonth);
	if (nMonthIndex < 0)
	{
		return FALSE;
	}

	CRect rects [42];
	int xStart = 0;
	int nDaysInPrevMonth = 0;

	GetDayRects (nMonthIndex, rects, xStart, nDaysInPrevMonth);

	int i = nDay - 1 + xStart;
	if (i >= 0 && i < 42)
	{
		rect = rects [i];
		return TRUE;
	}

	return FALSE;
}

COleDateTime CBORROW_Calendar::GetMonthDate (int nMonthIndex) const
{

	COleDateTime date (m_nStartYear, m_nStartMonth, 1, 0, 0, 0);

	int nMonth = m_nStartMonth;
	int nYear = m_nStartYear;

	for (int nMonthCount = 0; nMonthCount < nMonthIndex + 1; nMonthCount++)
	{
		date = COleDateTime (nYear, nMonth++, 1, 0, 0, 0);

		if (nMonth > 12)
		{
			nMonth = 1;
			nYear++;
		}
	}

	return date;
}

BOOL CBORROW_Calendar::IsDateSelected (COleDateTime date) const
{
	BOOL bFlag;
	return m_SelectedDates.Lookup (date.m_dt, bFlag);
}

BOOL CBORROW_Calendar::IsDateSelected (int nYear, int nMonth, int nDay) const
{
	return IsDateSelected (COleDateTime (nYear, nMonth, nDay, 0, 0, 0));
}

BOOL CBORROW_Calendar::IsDateMarked (COleDateTime date) const
{
	BOOL bFlag = FALSE;
	return m_MarkedDates.Lookup (date.m_dt, bFlag);
}

BOOL CBORROW_Calendar::IsDateMarked (int nYear, int nMonth, int nDay) const
{
	COleDateTime date (nYear, nMonth, nDay, 0, 0, 0);
	BOOL bFlag = FALSE;

	return m_MarkedDates.Lookup (date.m_dt, bFlag);
}

void CBORROW_Calendar::SetDateColor (COleDateTime date, COLORREF color, BOOL bRedraw)
{
	if (color == (COLORREF)-1)
	{
		m_DateColors.RemoveKey (date.m_dt);
	}
	else
	{
		m_DateColors.SetAt (date.m_dt, color);
	}

	if (bRedraw)
	{
		RedrawDate (date);
	}
}

COLORREF CBORROW_Calendar::GetDateColor (COleDateTime date)
{
	COLORREF color = (COLORREF)-1;
	m_DateColors.Lookup (date.m_dt, color);

	return color;
}

void CBORROW_Calendar::SetVertMargin (int nVertMargin)
{
	m_nVertMargin = nVertMargin;
	RecalcLayout ();
}

void CBORROW_Calendar::SetHorzMargin (int nHorzMargin)
{
	m_nHorzMargin = nHorzMargin;
	RecalcLayout ();
}

int CBORROW_Calendar::GetMaxMonthDay (int nMonth, int nYear)
{ 
	static int nMonthLen [] = 
	{	
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 
	};

	int nRes = nMonthLen [nMonth - 1];
	if (nMonth == 2 && nYear % 4 == 0 && 
		(nYear % 100 != 0 || nYear % 400 == 0))
	{
		nRes = 29;
	}

	return nRes;
}

void CBORROW_Calendar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDown(nFlags, point);

	m_bSelChanged = FALSE;

	SetFocus ();
	SetCapture ();

	if (m_rectBtnPrev.PtInRect (point))
	{
		MovePrev ();

		m_bSlowTimerMode = TRUE;
		m_iSlowTimerCount = 0;
		m_bIsTimerNext = FALSE;
		m_rectTimer = m_rectBtnPrev;
		SetTimer (iIdClickButtonEvt, iSlowTimerDuration, NULL);

		return;
	}

	if (m_rectBtnNext.PtInRect (point))
	{
		MoveNext ();

		m_bSlowTimerMode = TRUE;
		m_iSlowTimerCount = 0;
		m_bIsTimerNext = TRUE;
		m_rectTimer = m_rectBtnNext;
		SetTimer (iIdClickButtonEvt, iSlowTimerDuration, NULL);

		return;
	}

	//---------------------
	// Hit test month name:
	//---------------------
	CRect rectMonthName;
	int nClickedMonthIndex = HitTestMonthName (point, rectMonthName);

	if (nClickedMonthIndex >= 0)
	{
		CBORROW_MonthPickerWnd* pMonthPicker = new 
			CBORROW_MonthPickerWnd (this, nClickedMonthIndex);

		CPoint ptMonthPicker = rectMonthName.CenterPoint ();
		ClientToScreen (&ptMonthPicker);

		pMonthPicker->Create (ptMonthPicker);
		return;
	}

	m_bSelectWeekMode = FALSE;
	m_bSelectTruncate = FALSE;

	if (m_bWeekNumbers && m_bMultipleSelection)
	{
		m_dateTrack = HitTestWeekNum (point, TRUE);
		
		if (m_dateTrack != COleDateTime ())
		{
			m_bSelectWeekMode = TRUE;
			m_dateStartDrag = m_dateTrack;

			if ((nFlags & MK_CONTROL) == 0)
			{
				ClearSelectedDates (FALSE);
			}

			OnMouseMove (nFlags, point);
			return;
		}
	}

	int nDir = 0;

	m_dateTrack = HitTest (point, nDir, NULL);

	if (m_dateTrack == COleDateTime ())
	{
		return;
	}

	m_dateStartDrag = m_dateTrack;

	if (!m_bMultipleSelection || nDir != 0)
	{
		COleDateTime date = m_dateTrack;

		switch (nDir)
		{
		case -1:
			MovePrev ();
			m_dateStartDrag = m_dateTrack = COleDateTime ();
			break;

		case 1:
			MoveNext ();
			m_dateStartDrag = m_dateTrack = COleDateTime ();
			break;
		}

		SelectDate (date);
		return;
	}

	if ((nFlags & MK_CONTROL) == 0)
	{
		m_bSelChanged = TRUE;
		ClearSelectedDates (FALSE);
	}

	OnMouseMove (nFlags, point);
}

void CBORROW_Calendar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonUp(nFlags, point);

	if (::GetCapture () == GetSafeHwnd ())
	{
		ReleaseCapture ();
	}

	KillTimer (iIdClickButtonEvt);

	if (m_dateStartDrag != COleDateTime ())
	{
		if (m_bMultipleSelection)
		{
			COleDateTime date1 = min (m_dateTrack, m_dateStartDrag);
			COleDateTime date2 = max (m_dateTrack, m_dateStartDrag);

			m_bSelectTruncate = m_bSelectWeekMode ||
								(m_bTruncateSelection && (date2 - date1) > COleDateTimeSpan(6, 0, 0, 0));

			if (m_bSelectTruncate)
			{
				date1 = GetFirstWeekDay (date1);
				date2 = GetFirstWeekDay (date2) + COleDateTimeSpan (6, 0, 0, 0);
			}

			const COleDateTimeSpan day (1, 0, 0, 0);
			for (COleDateTime date = date1; date <= date2; date += day)
			{
				if ((nFlags & MK_CONTROL) && IsDateSelected (date))
				{
					UnselectDate (date, FALSE, FALSE);
				}
				else
				{
					SelectDate (date, TRUE, FALSE, FALSE);
				}

				m_bSelChanged = TRUE;
			}
		}
		else
		{
			SelectDate (m_dateStartDrag, TRUE, FALSE, FALSE);
		}
	}

	COleDateTime dateClicked;
	if (m_dateStartDrag == m_dateTrack)
	{
		dateClicked = m_dateTrack;
	}

	m_dateStartDrag = COleDateTime ();
	m_dateTrack = COleDateTime ();
	m_bSelectWeekMode = FALSE;

	if (m_bSelChanged)
	{
		m_bSelChanged = FALSE;

		if (m_bMultipleSelection)
		{
			OnSelectionChanged ();
		}
	}

#ifndef _BCGPCALENDAR_STANDALONE
	CBORROW_CalendarBar* pParentBar = DYNAMIC_DOWNCAST (CBORROW_CalendarBar, GetParent ());
	if (pParentBar != NULL && dateClicked != COleDateTime ())
	{
		ASSERT_VALID (pParentBar);
		pParentBar->OnClickDate (dateClicked);
		return;
	}
#endif
	
	if (m_pParentBtn != NULL && dateClicked != COleDateTime ())
	{
		ASSERT_VALID (m_pParentBtn);
		m_pParentBtn->ClosePopupCalendar (dateClicked);
	}
}

void CBORROW_Calendar::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ((nFlags & MK_LBUTTON) == 0 || m_dateStartDrag == COleDateTime () ||
		!m_bMultipleSelection)
	{
		return;
	}

	COleDateTime datePrevTrack = m_dateTrack;
	COleDateTime dateTrack = 
		m_bSelectWeekMode ? HitTestWeekNum (point) : HitTest (point);

	if (dateTrack == COleDateTime ())
	{
		return;
	}

	m_dateTrack = dateTrack;

	if (!m_bSelectWeekMode &&
		m_dateTrack.GetMonth () == datePrevTrack.GetMonth () &&
		m_dateTrack.GetYear () == datePrevTrack.GetYear ())
	{
		CRect rectMonth;
		GetMonthRect (m_dateTrack.GetYear (), m_dateTrack.GetMonth (), rectMonth);

		if (m_bTruncateSelection)
		{
			COleDateTime date1 = min (datePrevTrack, min (m_dateTrack, m_dateStartDrag));
			COleDateTime date2 = max (datePrevTrack, max (m_dateTrack, m_dateStartDrag));

			BOOL bSelectTruncate = (date2 - date1) > COleDateTimeSpan(6, 0, 0, 0);

			if (bSelectTruncate)
			{
				date1 = GetFirstWeekDay (date1);
				date2 = GetFirstWeekDay (date2) + COleDateTimeSpan (6, 0, 0, 0);

				CRect rectMonth1;
				GetMonthRect (date1.GetYear (), date1.GetMonth (), rectMonth1);
				CRect rectMonth2;
				GetMonthRect (date2.GetYear (), date2.GetMonth (), rectMonth2);

				rectMonth.UnionRect (rectMonth, rectMonth1);
				rectMonth.UnionRect (rectMonth, rectMonth2);
			}
		}

		RedrawWindow (rectMonth, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}
	else
	{
		RedrawWindow (m_rectDays, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}
}

void CBORROW_Calendar::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent != iIdClickButtonEvt)
	{
		return;
	}

	CPoint ptCursor;
	::GetCursorPos (&ptCursor);
	ScreenToClient (&ptCursor);

	if (m_rectTimer.PtInRect (ptCursor))
	{
		m_bIsTimerNext ? MoveNext () : MovePrev ();

		if (m_bSlowTimerMode && m_iSlowTimerCount ++ >= iNumberOfSlowTimes)
		{
			m_bSlowTimerMode = FALSE;
			KillTimer (iIdClickButtonEvt);
			SetTimer (iIdClickButtonEvt, iFastTimerDuration, NULL);
		}
	}
}

void CBORROW_Calendar::MovePrev ()
{
	m_nStartMonth--;

	if (m_nStartMonth == 0)
	{
		if (m_nStartYear <= iFirstAllowedYear)
		{
			m_nStartYear = iFirstAllowedYear;
			m_nStartMonth = 1;
		}
		else
		{
			m_nStartYear--;
			m_nStartMonth = 12;
		}
	}

	m_bScrollSelection = TRUE;
	RecalcLayout ();
	m_bScrollSelection = FALSE;
}

void CBORROW_Calendar::MoveNext ()
{
	m_nStartMonth++;

	if (m_nStartMonth > 12)
	{
		if (m_nStartYear >= iLastAllowedYear)
		{
			m_nStartYear = iLastAllowedYear;
			m_nStartMonth = 12;
		}
		else
		{
			m_nStartYear++;
			m_nStartMonth = 1;
		}
	}

	m_bScrollSelection = TRUE;
	RecalcLayout ();
	m_bScrollSelection = FALSE;
}

void CBORROW_Calendar::OnCancelMode() 
{
	if (::GetCapture () == GetSafeHwnd ())
	{
		ReleaseCapture ();
	}

	KillTimer (iIdClickButtonEvt);
	CWnd::OnCancelMode();
}

void CBORROW_Calendar::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);

	if (pNewWnd->GetSafeHwnd () == m_btnToday.GetSafeHwnd () || 
		m_hWndMonthPicker != NULL)
	{
		return;
	}
	
	if (m_pParentBtn != NULL)
	{
		ASSERT_VALID (m_pParentBtn);
		m_pParentBtn->ClosePopupCalendar ();
	}
}

COleDateTime CBORROW_Calendar::HitTest (CPoint point)
{
	int nDir = 0;

	COleDateTime date = HitTest (point, nDir);
	if (nDir != 0)
	{
		return COleDateTime ();
	}

	return date;
}

COleDateTime CBORROW_Calendar::HitTest (CPoint point, int& nDir, LPRECT lpRect)
{
	nDir = 0;

	if (lpRect != NULL)
	{
		*lpRect = CRect (0, 0, 0, 0);
	}

	for (int nMonthIndex = 0; nMonthIndex < m_nMonths; nMonthIndex++)
	{
		CRect rect;
		GetMonthRect (nMonthIndex, rect);

		if (!rect.PtInRect (point))
		{
			continue;
		}

		const COleDateTime date = GetMonthDate (nMonthIndex);
		const int nDaysInMonth = GetMaxMonthDay (date);

		CRect rects [42];
		int xStart = 0;
		int nDaysInPrevMonth = 0;

		GetDayRects (nMonthIndex, rects, xStart, nDaysInPrevMonth);

		const int nDayOffset = 1 - xStart;

		for (int i = 0; i < 42; i++)
		{
			CRect rectDay = rects [i];
			if (!rectDay.PtInRect (point))
			{
				continue;
			}

			int nDay = nDayOffset + i;
			int nYear = date.GetYear ();
			int nMonth = date.GetMonth ();

			if (nDay < 1)
			{
				nDay += nDaysInPrevMonth;
				nMonth--;

				if (nMonth == 0)
				{
					nMonth = 12;
					nYear--;
				}

				nDir = -1;
			}
			else if (nDay > nDaysInMonth)
			{
				nDay -= nDaysInMonth;
				nMonth++;

				if (nMonth > 12)
				{
					nMonth = 1;
					nYear++;
				}

				nDir = 1;
			}

			if (lpRect != NULL)
			{
				*lpRect = rectDay;
			}

			return COleDateTime (nYear, nMonth, nDay, 0, 0, 0);
		}		
	}

	return COleDateTime ();
}

COleDateTime CBORROW_Calendar::HitTestWeekNum (CPoint point, BOOL bStart)
{
	for (int nMonthIndex = 0; nMonthIndex < m_nMonths; nMonthIndex++)
	{
		CRect rect;
		GetMonthRect (nMonthIndex, rect);

		if (!rect.PtInRect (point))
		{
			continue;
		}

		rect.top += m_sizeBox.cy * 2 + 4;
		rect.bottom = rect.top + m_sizeBox.cy;

		if (bStart)
		{
			rect.right = rect.left + m_nDaysHorzMarginLeft;
		}

		for (int i = 0; i < 6; i++)
		{
			if (rect.PtInRect (point))
			{
				COleDateTime date = GetMonthDate (nMonthIndex);
				int nMonth = date.GetMonth ();

				if (i > 0)
				{
					date += COleDateTimeSpan (i * 7, 0, 0, 0);
				}

				while (date.GetDayOfWeek () != m_nFirstDayOfWeek + 1)
				{
					date -= COleDateTimeSpan (1, 0, 0, 0);
				}

				if (i == 5 && date.GetMonth () != nMonth)
				{
					return COleDateTime ();
				}

				return date;
			}

			rect.OffsetRect (0, m_sizeBox.cy);
		}
	}

	return COleDateTime ();
}

int CBORROW_Calendar::HitTestMonthName (CPoint point, CRect& rectMonthName)
{
	rectMonthName.SetRectEmpty ();

	for (int nMonth = 0; nMonth < m_nMonths; nMonth++)
	{
		CRect rect;
		GetMonthRect (nMonth, rect);

		if (!rect.PtInRect (point))
		{
			continue;
		}

		CRect rectCaption = rect;
		rectCaption.bottom = rectCaption.top + m_sizeBox.cy + 2;

		rectCaption.DeflateRect (m_sizeBox.cx + 2, 0);
		
		if (rectCaption.PtInRect (point))
		{
			rectMonthName = rectCaption;
			return nMonth;
		}
	}

	return -1;
}

int CBORROW_Calendar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CString strToday = Translate(_T("Today"));

	{
		CBORROW_LocalResource localResource;
		//strToday.LoadString(IDS_BCGBARRES_CALENDAR_TODAY);
	}

	m_btnToday.Create (strToday, WS_CHILD | BS_PUSHBUTTON,
		CRect (0, 0, 0, 0), this, ID_TODAY_BUTTON);

	if (GetStyle () & WS_POPUP)
	{
		if (m_sizeCalendar == CSize (0, 0))
		{
			RecalcLayout (FALSE);
		}

		int nTodayHeight = 0;

		if (m_bTodayButton && m_btnToday.GetSafeHwnd () != NULL)
		{
			CRect rectToday (0, 0, 0, 0);
			m_btnToday.GetClientRect (rectToday);

			nTodayHeight = rectToday.Height () + 10;
		}

		CSize size = m_sizeCalendar + 
			CSize (2 * m_nHorzMargin, 2 * m_nVertMargin + nTodayHeight);

		CRect rectWindow;
		GetWindowRect (rectWindow);

		if (m_pParentBtn != NULL)
		{
			ASSERT_VALID (m_pParentBtn);
			
			CRect rectParent;
			m_pParentBtn->GetWindowRect (rectParent);

			rectWindow = CRect (
				CPoint (rectParent.right - size.cx, rectParent.bottom),
				size);
		}

		CRect rectScreen;

		MONITORINFO mi;
		mi.cbSize = sizeof (MONITORINFO);
		if (GetMonitorInfo (MonitorFromPoint (rectWindow.TopLeft (), MONITOR_DEFAULTTONEAREST),
			&mi))
		{
			rectScreen = mi.rcWork;
		}
		else
		{
			::SystemParametersInfo (SPI_GETWORKAREA, 0, &rectScreen, 0);
		}

		if (rectWindow.left < rectScreen.left)
		{
			rectWindow.OffsetRect (rectScreen.left - rectWindow.left, 0);
		}
		else if (rectWindow.right > rectScreen.right)
		{
			rectWindow.OffsetRect (rectScreen.right - rectWindow.right, 0);
		}

		if (rectWindow.top < rectScreen.top)
		{
			rectWindow.OffsetRect (0, rectScreen.top - rectWindow.top);
		}
		else if (rectWindow.bottom > rectScreen.bottom)
		{
			rectWindow.OffsetRect (0, rectScreen.bottom - rectWindow.bottom);
		}

		SetWindowPos (&wndTop, rectWindow.left, rectWindow.top, 
			size.cx, size.cy, SWP_SHOWWINDOW);
		SetFocus ();
	}

	return 0;
}

void CBORROW_Calendar::OnDestroy() 
{
	if (m_hWndMonthPicker != NULL)
	{
		::SendMessage(m_hWndMonthPicker, WM_CLOSE, 0, 0);
		m_hWndMonthPicker = NULL;
	}

	CWnd::OnDestroy();
}

void CBORROW_Calendar::EnableTodayButton (BOOL bEnable)
{
	m_bTodayButton = bEnable;
	RecalcLayout ();
}

void CBORROW_Calendar::EnableWeekNumbers (BOOL bWeekNumbers)
{
	m_bWeekNumbers = bWeekNumbers;
	RecalcLayout ();
}

void CBORROW_Calendar::EnableGradientFillCaption (BOOL bEnable)
{
	m_bGradientFillCaption = bEnable;

	if (GetSafeHwnd () != NULL)
	{
		RedrawWindow ();
	}
}

void CBORROW_Calendar::OnToday()
{
	COleDateTime today = COleDateTime::GetCurrentTime ();

	SetFocus ();

	if (today < m_dateFirst || today > m_dateLast)
	{
		m_nStartMonth = today.GetMonth ();
		m_nStartYear = today.GetYear ();

		RecalcLayout ();
	}

	if (m_pParentBtn != NULL)
	{
		ASSERT_VALID (m_pParentBtn);
		m_pParentBtn->ClosePopupCalendar (COleDateTime::GetCurrentTime ());
	}
}

void CBORROW_Calendar::MarkDates (const CArray<DATE, DATE&>& arDates, BOOL bRedraw/* = TRUE*/)
{
	m_MarkedDates.RemoveAll ();

	for (int i = 0; i < arDates.GetSize (); i++)
	{
		COleDateTime date = arDates [i];

		if (m_dateFirst <= date && m_dateLast >= date)
		{
			ClearTime (date);
			m_MarkedDates.SetAt (date.m_dt, TRUE);
		}
	}

	if (bRedraw && GetSafeHwnd () != NULL)
	{
		RedrawWindow (m_rectDays, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}
}

BOOL CBORROW_Calendar::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message)
    {
    case WM_KEYDOWN:
		{
			if (pMsg->wParam == VK_ESCAPE && m_pParentBtn != NULL)
			{
				ASSERT_VALID (m_pParentBtn);
				m_pParentBtn->ClosePopupCalendar ();
				return TRUE;
			}

			BOOL bIsMenuActive = FALSE;

	#ifndef _BCGPCALENDAR_STANDALONE
			bIsMenuActive = CMFCPopupMenu::GetActiveMenu () != NULL;
	#endif

	#ifndef BCGP_EXCLUDE_PLANNER
			if (m_pWndPlanner != NULL && !bIsMenuActive)
			{
				ASSERT_VALID (m_pWndPlanner);
				m_pWndPlanner->SendMessage (WM_KEYDOWN,
					pMsg->wParam, pMsg->lParam);
				return TRUE;
			}
	#endif
		}
		break;

	case WM_LBUTTONUP:
		if (pMsg->hwnd == m_btnToday.GetSafeHwnd () &&
			m_btnToday.GetSafeHwnd () != NULL)
		{
			CRect rectBtn;
			m_btnToday.GetWindowRect (rectBtn);

			CPoint point;
			::GetCursorPos (&point);

			if (!rectBtn.PtInRect (point))
			{
				SetFocus ();
			}
		}
    }
	
	return CWnd::PreTranslateMessage(pMsg);
}

void CBORROW_Calendar::SelectDate (const COleDateTime& date, BOOL bAdd,
									 BOOL bNotify, BOOL bRedraw)
{
	COleDateTime datePrev = GetDate ();

	if (!m_bMultipleSelection)
	{
		bAdd = FALSE;
	}

	if (m_SelectedDates.GetCount () == 1 && date == datePrev)
	{
		return;
	}

	if (bAdd && IsDateSelected (date))
	{
		return;
	}

	BOOL bRedrawAll = FALSE;

	if (!bAdd)
	{
		bRedrawAll = (m_SelectedDates.GetCount () > 1);
		m_SelectedDates.RemoveAll ();
	}

	COleDateTime dateSel = date;
	ClearTime (dateSel);

	m_SelectedDates.SetAt (dateSel.m_dt, TRUE);

	if (date < m_dateFirst || date > m_dateLast)
	{
		m_nStartMonth = date.GetMonth ();
		m_nStartYear = date.GetYear ();
		RecalcLayout ();
	}
	else if (bRedraw && GetSafeHwnd () != NULL)
	{
		if (bRedrawAll)
		{
			RedrawWindow (m_rectDays, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
		else
		{
			if (!bAdd)
			{
				if (datePrev != COleDateTime ())
				{
					RedrawDate (datePrev);
				}
			}

			RedrawDate (date);
		}
	}

	if (bNotify)
	{
		OnSelectionChanged ();
	}

	m_bSelChanged = TRUE;
}

void CBORROW_Calendar::UnselectDate (const COleDateTime& date, BOOL bNotify, BOOL bRedraw)
{
	COleDateTime dateDel = date;
	if (!m_SelectedDates.RemoveKey (dateDel.m_dt))
	{
		return;
	}

	m_bSelChanged = TRUE;

	if (bRedraw && GetSafeHwnd () != NULL)
	{
		RedrawDate (date);
	}

	if (bNotify)
	{
		OnSelectionChanged ();
	}
}

BOOL CBORROW_Calendar::GetMinMaxSelection (const CList<DATE, DATE&>& lstDates, COleDateTime& date1, COleDateTime& date2)
{
	BOOL bRes = FALSE;
	BOOL bFirst = TRUE;

	POSITION pos = lstDates.GetHeadPosition ();
	while (pos != NULL)
	{
		COleDateTime dt (lstDates.GetNext (pos));

		if (bFirst)
		{
			date1  = dt;
			date2  = dt;
			bFirst = FALSE;
			bRes   = TRUE;
		}
		else
		{
			if (dt < date1)
			{
				date1 = dt;
			}
			else if (date2 < dt)
			{
				date2 = dt;
			}

		}
	}

	return bRes;
}

void CBORROW_Calendar::SetSelectedDates (const CList<DATE, DATE&>& lstDates,
						BOOL bNotify, BOOL bRedraw)
{
	if (!m_bMultipleSelection)
	{
		ASSERT (FALSE);
		return;
	}

	POSITION pos = NULL;
	CList<int, int>	lstMonthsToRedraw;

	for (pos = m_SelectedDates.GetStartPosition (); pos != NULL;)
	{
		DATE date;
		BOOL bFlag;

		m_SelectedDates.GetNextAssoc (pos, date, bFlag);

		COleDateTime dateCurr = date;
		int nMonthIndex = GetMonthIndex (dateCurr.GetYear (), dateCurr.GetMonth ());

		if (lstMonthsToRedraw.Find (nMonthIndex) == NULL)
		{
			lstMonthsToRedraw.AddTail (nMonthIndex);
		}
	}

	m_SelectedDates.RemoveAll ();

	BOOL bIsFirst = TRUE;
	BOOL bRecalcLayout = FALSE;

	for (pos = lstDates.GetHeadPosition (); pos != NULL;)
	{
		COleDateTime date = lstDates.GetNext (pos);

		if (bIsFirst)
		{
			if (date < m_dateFirst || date > m_dateLast)
			{
				m_nStartMonth = date.GetMonth ();
				m_nStartYear = date.GetYear ();

				bRecalcLayout = TRUE;
			}

			bIsFirst = FALSE;
		}

		m_SelectedDates.SetAt (date.m_dt, TRUE);

		int nMonthIndex = GetMonthIndex (date.GetYear (), date.GetMonth ());

		if (lstMonthsToRedraw.Find (nMonthIndex) == NULL)
		{
			lstMonthsToRedraw.AddTail (nMonthIndex);
		}
	}

	m_bSelChanged = TRUE;

	if (bRecalcLayout)
	{
		RecalcLayout (bRedraw);
	}
	else if (bRedraw && GetSafeHwnd () != NULL)
	{
		if (lstMonthsToRedraw.GetCount () > m_nMonths / 2)
		{
			RedrawWindow (m_rectDays, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
		else
		{
			for (pos = lstMonthsToRedraw.GetHeadPosition (); pos != NULL;)
			{
				int nMonthIndex = lstMonthsToRedraw.GetNext (pos);

				CRect rect;
				if (GetMonthRect (nMonthIndex, rect))
				{
					InvalidateRect (rect);
				}
			}

			UpdateWindow ();
		}
	}

	if (bNotify)
	{
		OnSelectionChanged ();
	}
}

void CBORROW_Calendar::ClearSelectedDates (BOOL bNotify, BOOL bRedraw)
{
	if (m_SelectedDates.IsEmpty ())
	{
		return;
	}

	m_SelectedDates.RemoveAll ();
	m_bSelChanged = TRUE;

	if (bRedraw && GetSafeHwnd () != NULL)
	{
		RedrawWindow (m_rectDays, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}

	if (bNotify)
	{
		OnSelectionChanged ();
	}
}

void CBORROW_Calendar::GetSelectedDates (CList<DATE, DATE&>& lstDates) const
{
	for (POSITION pos = m_SelectedDates.GetStartPosition (); pos != NULL;)
	{
		DATE date;
		BOOL bFlag;

		m_SelectedDates.GetNextAssoc (pos, date, bFlag);
		lstDates.AddTail (date);
	}
}

void CBORROW_Calendar::EnableMutipleSelection (BOOL bEnable, int nMaxSelDays, BOOL bTruncate)
{
	m_bMultipleSelection = bEnable;
	m_bTruncateSelection = bTruncate;
	m_nMaxSelDates = nMaxSelDays;

	if (!bEnable && m_SelectedDates.GetCount () > 1)
	{
		m_SelectedDates.RemoveAll ();

		if (GetSafeHwnd () != NULL)
		{
			Invalidate ();
			UpdateWindow ();
		}
	}
}

void CBORROW_Calendar::SetFirstDayOfWeek (int nDay)
{
	ASSERT (nDay >= 0 && nDay < 7);

	m_nFirstDayOfWeek = nDay;
	RecalcLayout ();
}

#ifndef BCGP_EXCLUDE_PLANNER
void CBORROW_Calendar::SetPlanner (CBCGPPlannerManagerCtrl* pPlanner)
{
	m_pWndPlanner = pPlanner;
	m_DropTarget.Register (this);
}
#endif // BCGP_EXCLUDE_PLANNER

BOOL CBORROW_Calendar::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	OnDragLeave();

	if (dropEffect != DROPEFFECT_MOVE && dropEffect != DROPEFFECT_COPY)
	{
		return FALSE;
	}

	int nDir = 0;
	COleDateTime date = HitTest (point, nDir);

	if (date == COleDateTime () || nDir != 0)
	{
		return FALSE;
	}

#ifndef BCGP_EXCLUDE_PLANNER
	ASSERT_VALID (m_pWndPlanner);

	return m_pWndPlanner->OnDropAppointmentToCalendar (pDataObject,
		dropEffect, date);
#else
	return FALSE;
#endif
}
//********************************************************************************
DROPEFFECT CBORROW_Calendar::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	m_rectDrag.SetRectEmpty ();
	return OnDragOver(pDataObject, dwKeyState, point);
}
//********************************************************************************
void CBORROW_Calendar::OnDragLeave()
{
	if (!m_rectDrag.IsRectEmpty ())
	{
		CClientDC dc (this);

		CRect rectDragOld = m_rectDrag;
		m_rectDrag.SetRectEmpty ();

		dc.DrawDragRect (&m_rectDrag, CSize (2, 2), &rectDragOld, CSize (2, 2));
	}
}
//********************************************************************************
DROPEFFECT CBORROW_Calendar::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	DROPEFFECT dropEffect = DROPEFFECT_NONE;

	CRect rectDragOld = m_rectDrag;
	m_rectDrag.SetRectEmpty ();

#ifndef BCGP_EXCLUDE_PLANNER
	if (m_pWndPlanner != NULL && m_pWndPlanner->IsClipboardFormatAvailable (pDataObject))
	{
		if (!m_pWndPlanner->IsReadOnly ())
		{
			int nDir = 0;
			CRect rectDrag;

			COleDateTime date = HitTest (point, nDir, &rectDrag);

			if (date != COleDateTime () && nDir == 0)
			{
				dropEffect = (dwKeyState & MK_CONTROL) ? DROPEFFECT_COPY : DROPEFFECT_MOVE;
				m_rectDrag = rectDrag;
			}
		}
		else
		{
			dropEffect = DROPEFFECT_NONE;
		}
	}
#endif

	CClientDC dc (this);
	dc.DrawDragRect (
		m_rectDrag,
		CSize (2, 2), 
		rectDragOld.IsRectEmpty () ? NULL : &rectDragOld, 
		CSize (2, 2));

	return dropEffect;
}
//********************************************************************************
void CBORROW_Calendar::RedrawDate (const COleDateTime& date)
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rect;
	GetDateRect (date, rect);

	rect.InflateRect (3, 3);
	RedrawWindow (rect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}
//********************************************************************************
void CBORROW_Calendar::ShiftSelection (COleDateTimeSpan delta)
{
	if (m_SelectedDates.IsEmpty ())
	{
		return;
	}

	CArray<DATE, DATE> arDates;
	arDates.SetSize (m_SelectedDates.GetCount ());

	int i = 0;

	for (POSITION pos = m_SelectedDates.GetStartPosition (); pos != NULL; i++)
	{
		DATE date;
		BOOL bFlag;

		m_SelectedDates.GetNextAssoc (pos, date, bFlag);
		arDates [i] = date;
	}

	m_SelectedDates.RemoveAll ();

	for (i = 0; i < arDates.GetSize (); i++)
	{
		COleDateTime date = arDates [i];
		date += delta;
	
		m_SelectedDates.SetAt (date.m_dt, TRUE);
	}

	m_bSelChanged = TRUE;

	OnSelectionChanged ();
}
//****************************************************************************
LRESULT CBORROW_Calendar::OnPrintClient(WPARAM wp, LPARAM lp)
{
	DWORD dwFlags = (DWORD)lp;

	if (dwFlags & PRF_CLIENT)
	{
		CDC* pDC = CDC::FromHandle((HDC) wp);
		ASSERT_VALID(pDC);

		OnDraw(pDC);
	}

	return 0;
}

#ifndef _BCGPCALENDAR_STANDALONE

/////////////////////////////////////////////////////////////////////////////
// CBCGPCalendarBar

const UINT CBORROW_CalendarBar::CBR_WEEKDAYSEL = 0x1;
const UINT CBORROW_CalendarBar::CBR_WEEKNUMBER = 0x2;
const UINT CBORROW_CalendarBar::CBR_MULTISELECTION = 0x4;
const UINT CBORROW_CalendarBar::CBR_ENABLED = 0x8; // still has not effect - 6.2
const UINT CBORROW_CalendarBar::CBR_NAVIGATION_BUTTONS = 0x10;

IMPLEMENT_SERIAL(CBORROW_CalendarBar, CMFCPopupMenuBar, 1)

CBORROW_CalendarBar::CBORROW_CalendarBar()
{
	CommonInit ();

	m_nCommandID = 0;
	m_bIsTearOff = TRUE;
}
//**************************************************************************************
CBORROW_CalendarBar::CBORROW_CalendarBar (const COleDateTime& month, UINT nCommandID) :
	m_bIsTearOff (FALSE),
	m_nCommandID (nCommandID)
{
	CommonInit ();
	m_wndCalendar.SetDate (month);
}
//**************************************************************************************
CBORROW_CalendarBar::CBORROW_CalendarBar (CBORROW_CalendarBar& src, UINT uiCommandID, BOOL enableSelection) :
		m_bIsTearOff (TRUE),
		m_nCommandID (uiCommandID)

{
	CommonInit ();

	m_wndCalendar.SetDate (src.m_wndCalendar.GetDate ());

	if (enableSelection) 
	{
		m_styleFlags |= CBR_MULTISELECTION;
	}
	else				
	{
		m_styleFlags &= (~CBR_MULTISELECTION);
	}
}
//**************************************************************************************
void CBORROW_CalendarBar::CommonInit ()
{
	m_bLocked = TRUE;
    m_iButtonCapture = -1;
	m_styleFlags = (CBR_WEEKDAYSEL | CBR_WEEKNUMBER | CBR_ENABLED | CBR_NAVIGATION_BUTTONS)
					& (~CBR_MULTISELECTION);
	m_bIsCtrl = FALSE;
#ifndef _BCGSUITE_
	m_bShowTooltips = TRUE;
#endif
}
//*****************************************************************************************
void CBORROW_CalendarBar::SetState(UINT flags, UINT mask)
{
	m_styleFlags &= ~mask;
	m_styleFlags |= (flags & mask);

	if (!(m_styleFlags & CBR_MULTISELECTION))
	{
		m_wndCalendar.ClearSelectedDates ();
	}

	m_wndCalendar.EnableMutipleSelection (m_styleFlags & CBR_MULTISELECTION);
	m_wndCalendar.EnableWeekNumbers (m_styleFlags & CBR_WEEKNUMBER);
	m_wndCalendar.RecalcLayout ();
}
//***************************************************************************************
CSize CBORROW_CalendarBar::CalcSize (BOOL /*bVertDock*/)
{
	if (m_wndCalendar.m_sizeCalendar == CSize (0, 0))
	{
		m_wndCalendar.RecalcLayout (FALSE);
	}

	int nTodayHeight = 0;

	if (m_wndCalendar.m_bTodayButton && m_wndCalendar.m_btnToday.GetSafeHwnd () != NULL)
	{
		CRect rectToday (0, 0, 0, 0);
		m_wndCalendar.m_btnToday.GetClientRect (rectToday);

		nTodayHeight = rectToday.Height () + 10;
	}

	return m_wndCalendar.m_sizeCalendar + 
		CSize (2 * m_wndCalendar.m_nHorzMargin, 2 * m_wndCalendar.m_nVertMargin + nTodayHeight);
}

BEGIN_MESSAGE_MAP(CBORROW_CalendarBar, CMFCPopupMenuBar)
	//{{AFX_MSG_MAP(CBCGPCalendarBar)
	ON_WM_CREATE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGPCalendarBar message handlers

int CBORROW_CalendarBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMFCPopupMenuBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	DWORD dwStyle = WS_CHILD | WS_VISIBLE;
	if (m_bIsCtrl)
	{
		dwStyle |= WS_BORDER;
	}

	m_wndCalendar.EnableMutipleSelection (m_styleFlags & CBR_MULTISELECTION);
	m_wndCalendar.EnableWeekNumbers (m_styleFlags & CBR_WEEKNUMBER);

	if (!m_wndCalendar.Create (dwStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create calendar\n");
		return -1;      // fail to create
	}

	SetWindowPos (&wndTop, -1, -1, -1, -1,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	return 0;
}
//*************************************************************************************
void CBORROW_CalendarBar::OnDestroy() 
{
	CMFCPopupMenuBar::OnDestroy();
}
//*************************************************************************************
BOOL CBORROW_CalendarBar::OnClickDate (COleDateTime date)
{
	CBORROW_CalendarMenuButton* pCalendarMenuButton = NULL;

	CMFCPopupMenu* pParentMenu = DYNAMIC_DOWNCAST (CMFCPopupMenu, GetParent ());
	if (pParentMenu != NULL)
	{
		pCalendarMenuButton = DYNAMIC_DOWNCAST (CBORROW_CalendarMenuButton, pParentMenu->GetParentButton ());
	}

	if (pCalendarMenuButton != NULL)
	{
		pCalendarMenuButton->SetDate (date);
		InvokeMenuCommand (pCalendarMenuButton->m_nID, pCalendarMenuButton);
	}
	else
	{
		CObList listButtons;
		if (CMFCToolBar::GetCommandButtons (m_nCommandID, listButtons) > 0)
		{
			for (POSITION pos = listButtons.GetHeadPosition (); pos != NULL;)
			{
				CBORROW_CalendarMenuButton* pButton = 
					DYNAMIC_DOWNCAST (CBORROW_CalendarMenuButton, listButtons.GetNext (pos));
				if (pButton != NULL)
				{
					pButton->SetDate (date, FALSE);
				}
			}
		}

		CBORROW_CalendarMenuButton::SetCalendarByCmdID (m_nCommandID, date);
		GetOwner()->SendMessage (WM_COMMAND, m_nCommandID);
	}

	return TRUE;
}
//*************************************************************************************
BOOL CBORROW_CalendarBar::Create(
			CWnd*		pParentWnd,
			DWORD		dwStyle,
			UINT		nID,
			BOOL		enableSelection)
{
	if (enableSelection)
	{
		m_styleFlags |= CBR_MULTISELECTION;
	}
	else
	{
		m_styleFlags &= (~CBR_MULTISELECTION);
	}

	return CMFCPopupMenuBar::Create (pParentWnd, dwStyle, nID);
}
//************************************************************************************
BOOL CBORROW_CalendarBar::CreateControl (
				CWnd*			pParentWnd,
				const CRect&	rect,
				UINT			nID,
				BOOL			enableSelection,
				DWORD			dwStyle)
{
	ASSERT_VALID (pParentWnd);

	m_wndCalendar.m_bSingleMonth = TRUE;
	m_bIsCtrl = TRUE;

	if (!Create (pParentWnd, dwStyle | CBRS_ALIGN_TOP, nID, enableSelection))
	{
		return FALSE;
	}

	//----------------------------
	// Remove borders and gripper:
	//----------------------------
/*
	SetBarStyle (
		GetBarStyle ()
        & ~(CBRS_GRIPPER | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
*/

	CRect rectWnd = rect;
	MoveWindow (rectWnd);
	
	SetOwner (pParentWnd);
	SetCommandID (nID);

	// All commands will be routed via this dialog, not via the parent frame:
	SetRouteCommandsViaFrame (FALSE);

#ifndef _BCGSUITE_
	//SetControlVisualMode (pParentWnd);
#endif
	return TRUE;
}
//*************************************************************************************
void CBORROW_CalendarBar::Serialize (CArchive& ar)
{
	CMFCPopupMenuBar::Serialize (ar);

	if (ar.IsLoading ())
	{
		ar >> m_bIsTearOff;
		ar >> m_nCommandID;

		m_wndCalendar.RecalcLayout ();
	}
	else
	{
		ar << m_bIsTearOff;
		ar << m_nCommandID;
	}
}
//*************************************************************************************
void CBORROW_CalendarBar::ShowCommandMessageString (UINT /*uiCmdId*/)
{
	GetOwner()->SendMessage (WM_SETMESSAGESTRING,
		m_nCommandID == (UINT) -1 ? AFX_IDS_IDLEMESSAGE : (WPARAM) m_nCommandID);
}
//****************************************************************************************
void CBORROW_CalendarBar::SetVertMargin (int nVertMargin)
{
	m_wndCalendar.SetVertMargin (nVertMargin);
}
//*****************************************************************************************
void CBORROW_CalendarBar::SetHorzMargin (int nHorzMargin)
{
	m_wndCalendar.SetHorzMargin (nHorzMargin);
}
//****************************************************************************************
int CBORROW_CalendarBar::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	//return CControlBar::OnMouseActivate(pDesktopWnd, nHitTest, message);
    return MA_NOACTIVATE;
}
//*************************************************************************************
const CList<int,int>& CBORROW_CalendarBar::GetSelectedDays () const
{
	static CList<int, int> lstDays;

	for (POSITION pos = m_wndCalendar.m_SelectedDates.GetStartPosition (); pos != NULL;)
	{
		DATE date;
		BOOL bFlag;

		m_wndCalendar.m_SelectedDates.GetNextAssoc (pos, date, bFlag);

		COleDateTime dateCurr = date;
		lstDays.AddTail (dateCurr.GetDay ());
	}

	return lstDays;
}
//*************************************************************************************
void CBORROW_CalendarBar::SelectDays (const CList<int,int>& lstDays,
									 BOOL bRedraw)
{
	if (!(m_styleFlags & CBR_MULTISELECTION))
	{
		ASSERT (FALSE);
		return;
	}

	m_wndCalendar.ClearSelectedDates (FALSE, FALSE);
	
	for (POSITION pos = lstDays.GetHeadPosition (); pos != NULL;)
	{
		int nDay = lstDays.GetNext (pos);

		COleDateTime date (
			m_wndCalendar.m_dateFirst.GetYear (),
			m_wndCalendar.m_dateFirst.GetMonth (),
			nDay, 0, 0, 0);

		m_wndCalendar.SelectDate (date, TRUE, FALSE, FALSE);
	}

	if (bRedraw && m_wndCalendar.GetSafeHwnd () != NULL)
	{
		m_wndCalendar.RedrawWindow (m_wndCalendar.m_rectDays);
	}

	m_wndCalendar.OnSelectionChanged ();
}
//*************************************************************************************
void CBORROW_CalendarBar::MarkDays (const CList<int,int>& lstDays,
								 BOOL bRedraw)
{
	CArray<DATE, DATE&> arDates;
	arDates.SetSize (lstDays.GetCount ());

	int i = 0;

	for (POSITION pos = lstDays.GetHeadPosition (); pos != NULL; i++)
	{
		int nDay = lstDays.GetNext (pos);

		COleDateTime date (
			m_wndCalendar.m_dateFirst.GetYear (),
			m_wndCalendar.m_dateFirst.GetMonth (),
			nDay, 0, 0, 0);

		arDates [i] = date;
	}

	m_wndCalendar.MarkDates (arDates, bRedraw);
}
//***************************************************************************************
void CBORROW_CalendarBar::OnSelectionChanged ()
{
	if (!m_bIsTearOff)
	{
		return;
	}

	CWnd* pOwner = GetOwner ();
	if (pOwner != NULL)
	{
		ASSERT_VALID (pOwner);

		pOwner->SendMessage (BORROW_CALENDAR_ON_SELCHANGED,
			0, (LPARAM) GetSafeHwnd ());
	}
}
//********************************************************************************
void CBORROW_CalendarBar::OnSize(UINT nType, int cx, int cy) 
{
	CMFCPopupMenuBar::OnSize(nType, cx, cy);
	m_wndCalendar.SetWindowPos (NULL, -1, -1, cx, cy,
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGPDefaultLocale

CBORROW_DefaultLocale::CBORROW_DefaultLocale ()
{
	m_strLanguage = CString (_tsetlocale (LC_ALL, NULL));
	
	int nLen = GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE , NULL, 0);
	
	TCHAR* pzBuffer = new TCHAR [nLen + 1];
	pzBuffer[nLen] = NULL;

	nLen = GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, pzBuffer, nLen);
	CString strLocale = CString (pzBuffer) + _T("_");
	delete [] pzBuffer;
	
	nLen = GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_SENGCOUNTRY, NULL, 0);
	
	pzBuffer = new TCHAR [nLen + 1];
	pzBuffer [nLen] = NULL;

	nLen = GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_SENGCOUNTRY, pzBuffer, nLen);
	strLocale += pzBuffer;
	delete [] pzBuffer;
	
	_tsetlocale (LC_ALL, strLocale);
}
//**************************************************************************************
CBORROW_DefaultLocale::~CBORROW_DefaultLocale ()
{
	_tsetlocale(LC_ALL, m_strLanguage);
}

const int nVisbleMonths = 7;

/////////////////////////////////////////////////////////////////////////////
// CBCGPMonthPickerWnd

CBORROW_MonthPickerWnd::CBORROW_MonthPickerWnd (CBORROW_Calendar* pCalendarWnd,
										  int nMonthIndexInCalendar)
{
	ASSERT_VALID (pCalendarWnd);

	m_pCalendarWnd = pCalendarWnd;
	m_nElapseInterval = 200;
	m_nIDTimerEvent = 1;

	m_nRowHeight = 0;

	m_nMonthIndexInCalendar = nMonthIndexInCalendar;
	m_nFirstVisibleMonth = m_pCalendarWnd->m_nStartMonth;
	m_nFirstVisibleYear = m_pCalendarWnd->m_nStartYear;

	int i = 0;

	for (i = 0; i < m_nMonthIndexInCalendar; i++)
	{
		NextMonth ();
	}

	for (i = 0; i < nVisbleMonths / 2; i++)
	{
		PrevMonth ();
	}

	m_nSelRow = nVisbleMonths / 2;
}

CBORROW_MonthPickerWnd::~CBORROW_MonthPickerWnd()
{
	ASSERT_VALID (m_pCalendarWnd);
	m_pCalendarWnd->m_hWndMonthPicker = NULL;
}


BEGIN_MESSAGE_MAP(CBORROW_MonthPickerWnd, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CBCGPMonthPickerWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_CREATE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBCGPMonthPickerWnd message handlers

BOOL CBORROW_MonthPickerWnd::Create(CPoint ptCenter) 
{
	static CString strClassName;

	if (strClassName.IsEmpty ())
	{
		strClassName = AfxRegisterWndClass (0, AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}

	return CMiniFrameWnd::CreateEx (WS_EX_PALETTEWINDOW,
							strClassName, _T (""),
							WS_POPUP | MFS_SYNCACTIVE,
							CRect (ptCenter, CSize (0, 0)),
							NULL);
}
//****************************************************************************************
int CBORROW_MonthPickerWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pCalendarWnd->m_hWndMonthPicker = GetSafeHwnd();

	SetCapture ();

	// Set window size:
	int cx = 0;
	int cy = 0;
	
	COleDateTime day = COleDateTime::GetCurrentTime();

	CClientDC dc (this);
	CFont* pOldFont = dc.SelectObject (&afxGlobalData.fontRegular);

	{
		CBORROW_DefaultLocale dl;

		for (int i = 0; i < 12; i ++)
		{
			day.SetDate (day.GetYear(), i + 1, 1);
			//m_strMonthName [i] = day.Format (_T("%B"));
			m_strMonthName[i] = myGetMonthName(i+1);

			CSize sizeText = dc.GetTextExtent (m_strMonthName [i]);

			cx = max (cx, sizeText.cx);
			cy = max (cy, sizeText.cy);
		}
	}

	CString strYear = _T(" 9999");
	CSize sizeYear = dc.GetTextExtent (strYear);

	m_nRowHeight = cy + 4;

	CSize sizeWindow (cx + sizeYear.cx + 20, m_nRowHeight * nVisbleMonths + 2);

	dc.SelectObject (pOldFont);

	CRect rectWindow;
	GetWindowRect (rectWindow);

	SetWindowPos (NULL, 
		rectWindow.left - sizeWindow.cx / 2,
		rectWindow.top - sizeWindow.cy / 2,
		sizeWindow.cx, sizeWindow.cy,
		SWP_NOZORDER | SWP_NOACTIVATE);

	ShowWindow (SW_SHOW);

	SetTimer (m_nIDTimerEvent, m_nElapseInterval, NULL);
	return 0;
}
//****************************************************************************************
void CBORROW_MonthPickerWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CMemDC memDC (dc, this);
	CDC* pDC = &memDC.GetDC ();

	CFont* pOldFont = pDC->SelectObject (&afxGlobalData.fontRegular);
	pDC->SetBkMode (TRANSPARENT);

	CRect rectClient;
	GetClientRect (rectClient);

	pDC->FillRect (rectClient, &afxGlobalData.brWindow);
	pDC->Draw3dRect (rectClient, afxGlobalData.clrWindowText, afxGlobalData.clrWindowText);

	CRect rectMonth = rectClient;
	rectMonth.DeflateRect (1, 1);

	rectMonth.bottom = rectMonth.top + m_nRowHeight;

	int nYear = m_nFirstVisibleYear;
	int nMonth = m_nFirstVisibleMonth;

	for (int i = 0; i < nVisbleMonths; i++)
	{
		if (i == m_nSelRow)
		{
			CBrush br (afxGlobalData.clrWindowText);
			pDC->FillRect (rectMonth, &br);

			pDC->SetTextColor (afxGlobalData.clrWindow);
		}
		else
		{
			pDC->SetTextColor (afxGlobalData.clrWindowText);
		}

		CString str;
		str.Format (_T("%s %04d"), m_strMonthName [nMonth - 1], nYear);

		pDC->DrawText (str, rectMonth, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		rectMonth.OffsetRect (0, m_nRowHeight);

		nMonth++;
		if (nMonth > 12)
		{
			nMonth = 1;
			nYear++;
		}
	}

	pDC->SelectObject (pOldFont);
}
//****************************************************************************************
BOOL CBORROW_MonthPickerWnd::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}
//****************************************************************************************
void CBORROW_MonthPickerWnd::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/)
{
	ASSERT_VALID (m_pCalendarWnd);

	KillTimer (m_nIDTimerEvent);
	ReleaseCapture ();

	if (m_nSelRow != -1)
	{
		int i = 0;

		for (i = 0; i < m_nSelRow; i++)
		{
			NextMonth ();
		}

		for (i = 0; i < m_nMonthIndexInCalendar; i++)
		{
			PrevMonth ();
		}

		if (m_pCalendarWnd->m_nStartYear != m_nFirstVisibleYear ||
			m_pCalendarWnd->m_nStartMonth != m_nFirstVisibleMonth)
		{
			m_pCalendarWnd->m_nStartYear = m_nFirstVisibleYear;
			m_pCalendarWnd->m_nStartMonth = m_nFirstVisibleMonth;
			m_pCalendarWnd->m_bScrollSelection = TRUE;
			m_pCalendarWnd->RecalcLayout ();
		}
	}

	SendMessage (WM_CLOSE);
}
//****************************************************************************************
void CBORROW_MonthPickerWnd::OnMouseMove(UINT /*nFlags*/, CPoint point) 
{
	CRect rectClient;
	GetClientRect (rectClient);

	if (!rectClient.PtInRect (point))
	{
		if (m_nSelRow >= 0)
		{
			m_nSelRow = -1;
			SetTimer (m_nIDTimerEvent, m_nElapseInterval, NULL);
			RedrawWindow ();
		}
		return;
	}

	KillTimer (m_nIDTimerEvent);

	int nSelRow = (point.y - 1) / m_nRowHeight;

	if (nSelRow != m_nSelRow)
	{
		m_nSelRow = nSelRow;
		RedrawWindow ();
	}
}
//****************************************************************************************
void CBORROW_MonthPickerWnd::OnCancelMode() 
{
	CMiniFrameWnd::OnCancelMode();
	
	KillTimer (m_nIDTimerEvent);
	ReleaseCapture ();
	SendMessage (WM_CLOSE);
}
//****************************************************************************************
BOOL CBORROW_MonthPickerWnd::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		KillTimer (m_nIDTimerEvent);
		ReleaseCapture ();
		SendMessage (WM_CLOSE);
		return TRUE;
	}
	
	return CMiniFrameWnd::PreTranslateMessage(pMsg);
}
//****************************************************************************************
int CBORROW_MonthPickerWnd::OnMouseActivate(CWnd* /*pDesktopWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
	return MA_NOACTIVATE;
}
//****************************************************************************************
void CBORROW_MonthPickerWnd::OnTimer(UINT_PTR nIDEvent) 
{
	CMiniFrameWnd::OnTimer(nIDEvent);

	if (nIDEvent != 1)
	{
		return;
	}

	KillTimer (m_nIDTimerEvent);

	CPoint point;
	::GetCursorPos (&point);
	ScreenToClient (&point);


	CRect rectClient;
	GetClientRect (rectClient);

	if (point.y < rectClient.top)
	{
		PrevMonth ();
		RedrawWindow ();
		SetTimer (m_nIDTimerEvent, max (50, m_nElapseInterval * m_nRowHeight / (rectClient.top - point.y)), NULL); 
	}
	else if (point.y > rectClient.bottom)
	{
		NextMonth ();
		RedrawWindow ();
		SetTimer (m_nIDTimerEvent, max (50, m_nElapseInterval * m_nRowHeight / (point.y - rectClient.bottom)), NULL);
	}
	else
	{
		SetTimer (m_nIDTimerEvent, m_nElapseInterval, NULL);
	}
}
//****************************************************************************************
void CBORROW_MonthPickerWnd::PrevMonth ()
{
	m_nFirstVisibleMonth--;
	if (m_nFirstVisibleMonth == 0)
	{
		m_nFirstVisibleMonth = 12;
		m_nFirstVisibleYear--;
	}
}
//****************************************************************************************
void CBORROW_MonthPickerWnd::NextMonth ()
{
	m_nFirstVisibleMonth++;
	if (m_nFirstVisibleMonth > 12)
	{
		m_nFirstVisibleMonth = 1;
		m_nFirstVisibleYear++;
	}
}
