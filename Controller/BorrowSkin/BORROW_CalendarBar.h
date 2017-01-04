#if !defined(AFX_BCGPCALENDARBAR_H__9B759C19_6B1C_48C4_A884_C0DEDD60DD77__INCLUDED_)
#define AFX_BCGPCALENDARBAR_H__9B759C19_6B1C_48C4_A884_C0DEDD60DD77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BCGPCalendarBar.h : header file
//

#include "BORROW_Pro.h"

#define BCGP_EXCLUDE_PLANNER 1

class CBORROW_DateTimeCtrl;

/////////////////////////////////////////////////////////////////////////////
// CBCGPCalendar window

class BORROW_DLLEXPORT CBORROW_CalendarColors
{
public:
   COLORREF clrCaption;
   COLORREF clrCaptionText;
   COLORREF clrSelected;
   COLORREF clrSelectedText;
   COLORREF clrTodayBorder;
};

class BORROW_DLLEXPORT CBORROW_Calendar : public CWnd
{
	friend class CBCGPPlannerDropTarget;
	friend class CBORROW_CalendarBar;
	friend class CBORROW_MonthPickerWnd;
	friend class CBCGPCalendarXCtrl;
// Construction
public:
	CBORROW_Calendar();

// Operations
public:
	void RecalcLayout (BOOL bRedraw = TRUE);

	//-------------------
	// Selection methods:
	//-------------------
	void SelectDate (	const COleDateTime& date, BOOL bAdd = FALSE, 
						BOOL bNotify = TRUE, BOOL bRedraw = TRUE);
	void UnselectDate (	const COleDateTime& date, 
						BOOL bNotify = TRUE, BOOL bRedraw = TRUE);

	void SetSelectedDates (	const CList<DATE, DATE&>& lstDates,
							BOOL bNotify = TRUE, BOOL bRedraw = TRUE);

	void ClearSelectedDates (BOOL bNotify = TRUE, BOOL bRedraw = TRUE);

	void GetSelectedDates (CList<DATE, DATE&>& lstDates) const;

	BOOL IsDateSelected (int nYear, int nMonth, int nDay) const;
	BOOL IsDateSelected (COleDateTime date) const;

	//-----------------
	// Marking methods:
	//-----------------
	void MarkDates (const CArray<DATE, DATE&>& arDates, BOOL bRedraw = TRUE);

	BOOL IsDateMarked (int nYear, int nMonth, int nDay) const;
	BOOL IsDateMarked (COleDateTime date) const;

	//------------------
	// Coloring methods:
	//------------------
	void SetDateColor (COleDateTime date, COLORREF color, BOOL bRedraw = TRUE);
	COLORREF GetDateColor (COleDateTime date);

	static int GetMaxMonthDay (int nMonth, int nYear);
	static int GetMaxMonthDay (COleDateTime date)
	{
		return GetMaxMonthDay (date.GetMonth (), date.GetYear ());
	}

	static BOOL GetMinMaxSelection (const CList<DATE, DATE&>& lstDates, COleDateTime& date1, COleDateTime& date2);

// Attributes
public:
	void SetDate (const COleDateTime& date);
	COleDateTime GetDate () const;

	void SetSingleMonthMode (BOOL bSingleMonth = TRUE);
	BOOL IsSingleMonthMode () const
	{
		return m_bSingleMonth;
	}

	void EnableMutipleSelection (BOOL bEnable = TRUE, int nMaxSelDates = -1, BOOL bTruncate = FALSE);
	BOOL IsMiltipleSelection () const
	{
		return m_bMultipleSelection;
	}

	int GetMaxSelDates () const
	{
		return m_nMaxSelDates;
	}

	void EnableTodayButton (BOOL bEnable = TRUE);
	BOOL IsTodayButton () const
	{
		return m_bTodayButton;
	}

	void EnableWeekNumbers (BOOL bWeekNumbers = TRUE);
	BOOL IsWeekNumbers () const
	{
		return m_bWeekNumbers;
	}

	void EnableGradientFillCaption (BOOL bEnable = TRUE);
	BOOL IsGradientFillCaption () const
	{
		return m_bGradientFillCaption;
	}

	COleDateTime GetFirstDate () const
	{
		return m_dateFirst;
	}

	COleDateTime GetLastDate () const
	{
		return m_dateLast;
	}

#ifndef BCGP_EXCLUDE_PLANNER
	void SetPlanner (CBCGPPlannerManagerCtrl* pPlanner);
#endif // BCGP_EXCLUDE_PLANNER

	void SetFirstDayOfWeek (int nDay);	// 0-6

	int GetFirstDayOfWeek () const
	{
		return m_nFirstDayOfWeek;
	}

	COleDateTime GetFirstWeekDay (const COleDateTime& day)
	{
		return day - COleDateTimeSpan((day.GetDayOfWeek () - GetFirstDayOfWeek() + 6) % 7, 0, 0, 0);
	}

	int GetVertMargin () const
	{
		return m_nVertMargin;
	}

	int GetHorzMargin () const
	{
		return m_nHorzMargin;
	}

	void SetVertMargin (int nVertMargin);
	void SetHorzMargin (int nHorzMargin);

protected:
	CMap<DATE, DATE&, BOOL, BOOL> 	
								m_SelectedDates;
	CMap<DATE, DATE&, BOOL, BOOL> 	
								m_MarkedDates;
	CMap<DATE, DATE&, COLORREF, COLORREF> 	
								m_DateColors;

	BOOL						m_bSingleMonth;
	BOOL						m_bWeekNumbers;
	BOOL						m_bGradientFillCaption;
	CSize						m_sizeCalendar;	// Month size
	int							m_nMonths;
	int							m_nCalendarsInRow;
	CSize						m_sizeBox;

	int							m_nStartMonth;	// 1-12
	int							m_nStartYear;

	int							m_nFirstDayOfWeek;

	CStringArray				m_arWeekDays;

	CRect						m_rectBtnPrev;
	CRect						m_rectBtnNext;

	BOOL						m_bSlowTimerMode;
	int							m_iSlowTimerCount;

	BOOL						m_bIsTimerNext;
	CRect						m_rectTimer;

	BOOL						m_bTodayButton;
	CButton						m_btnToday;

	COleDateTime				m_dateFirst;
	COleDateTime				m_dateLast;

	BOOL						m_bMultipleSelection;
	BOOL						m_bTruncateSelection;
	int							m_nMaxSelDates;

#ifndef BCGP_EXCLUDE_PLANNER
	CBCGPPlannerManagerCtrl*	m_pWndPlanner;
	CBCGPPlannerDropTarget		m_DropTarget;
#endif // BCGP_EXCLUDE_PLANNER

	CRect						m_rectDrag;

	CBORROW_CalendarColors			m_Colors;

	COleDateTime				m_dateTrack;
	COleDateTime				m_dateStartDrag;
	BOOL						m_bSelectWeekMode;
	BOOL						m_bSelectTruncate;

	BOOL						m_bSelChanged;

	int							m_nVertMargin;
	int							m_nHorzMargin;

	int							m_nDaysHorzMarginLeft;
	int							m_nDaysHorzMarginRight;

	CArray<int, int&>			m_arStartWeekInMonth;

	CBORROW_DateTimeCtrl*			m_pParentBtn;

	CRect						m_rectDays;
	BOOL						m_bIsMonthPicker;
	HWND						m_hWndMonthPicker;

	BOOL						m_bScrollSelection;
	BOOL						m_bIsPopup;

protected:
	BOOL GetMonthRect (int nMonthIndex, CRect& rect) const;
	int GetMonthRect (int nYear, int nMonth, CRect& rect) const;

	BOOL GetDateRect (int nYear, int nMonth, int nDay, CRect& rect) const;
	BOOL GetDateRect (COleDateTime date, CRect& rect) const
	{
		return GetDateRect (date.GetYear (), date.GetMonth (), date.GetDay (), rect);
	}

	COleDateTime GetMonthDate (int nMonthIndex) const;
	int GetMonthIndex (int nYear, int nMonth) const;

	void MovePrev ();
	void MoveNext ();

	void GetDayRects (int nMonthIndex, CRect rects [42], int& xStart, int& nDaysInPrevMonth) const;

	COleDateTime HitTest (CPoint point);
	COleDateTime HitTest (CPoint point, int& nDir, LPRECT lpRect = NULL);
	COleDateTime HitTestWeekNum (CPoint point, BOOL bStart = FALSE);
	int HitTestMonthName (CPoint point, CRect& rectMonthName);

	void RedrawDate (const COleDateTime& date);
	void ShiftSelection (COleDateTimeSpan delta);

// Overrides
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);

	virtual void OnDraw(CDC* pDCPaint);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGPCalendar)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:
	virtual void OnDrawMonth (CDC* pDC, CRect rect, int nMonthIndex);
	virtual void OnSelectionChanged ();

// Implementation
public:
	virtual ~CBORROW_Calendar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBCGPCalendar)
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCancelMode();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnToday();
	afx_msg LRESULT OnPrintClient(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPCalendarBar window

#define HMARGINS 2 // in pixels
#define VMARGINS 2 // in pixels

class BORROW_DLLEXPORT CBORROW_CalendarBar : public CMFCPopupMenuBar
{
//     friend class CCalendarPopup;
    friend class CBORROW_CalendarMenuButton;
//     friend class CBCGPCalendarProp;
#ifndef BCGP_EXCLUDE_PLANNER
	friend class CBCGPPlannerManagerCtrl;
	friend class CBCGPPlannerDropTarget;
#endif // BCGP_EXCLUDE_PLANNER
	friend class CBORROW_Calendar;

	DECLARE_SERIAL(CBORROW_CalendarBar)

// Construction
public:

	CBORROW_CalendarBar();

	virtual BOOL Create(
			CWnd*		pParentWnd,
			DWORD		dwStyle,
			UINT		nID,
			BOOL		enableSelection
			);

	virtual BOOL CreateControl (
			CWnd*			pParentWnd,
			const CRect&	rect,
			UINT			nID,
			BOOL			enableSelection = FALSE,
			DWORD			dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER
		);

protected:

	CBORROW_CalendarBar(const COleDateTime& month, UINT nCommandID);
	CBORROW_CalendarBar (CBORROW_CalendarBar& src, UINT uiCommandID, BOOL enableSelection);

// Attributes
public:
	void SetDate (const COleDateTime& date)
	{
		m_wndCalendar.SetDate (date);
	}

	COleDateTime GetDate () const
	{
		return m_wndCalendar.GetDate ();
	}

	void SetFirstDayOfWeek (int nDay)
	{
		m_wndCalendar.SetFirstDayOfWeek (nDay);
	}

	int GetFirstDayOfWeek () const
	{
		return m_wndCalendar.GetFirstDayOfWeek ();
	}

	const CList<int,int>& GetSelectedDays () const;
	void SelectDays (const CList<int,int>& lstDays, BOOL bRedraw = TRUE);

	const CList<int,int>& GetMarkedDays () const;
	void MarkDays (const CList<int,int>& lstDays, BOOL bRedraw = TRUE);

	UINT GetCommandID () const
	{
		return m_nCommandID;
	}

	void SetCommandID (UINT nCommandID)
	{
		m_nCommandID = nCommandID;
	}

	BOOL IsTearOff () const
	{
		return m_bIsTearOff;
	}

	UINT GetState() const
	{
		return m_styleFlags;
	}

	void SetState(UINT flags, UINT mask);

	void SetVertMargin (int nVertMargin);
	void SetHorzMargin (int nHorzMargin);

	int GetVertMargin () const
	{
		return m_wndCalendar.GetVertMargin ();
	}

	int GetHorzMargin () const
	{
		return m_wndCalendar.GetHorzMargin ();
	}

#if (!defined _BCGSUITE_) && !defined (_BCGSUITE_INC_)
	void EnableShowTooltips (BOOL bEnable = TRUE)
	{
		m_bShowTooltips = bEnable;
	}

	BOOL IsShowTooltips () const
	{
		return m_bShowTooltips;
	}
#endif

	virtual void OnSelectionChanged ();
	virtual BOOL OnClickDate (COleDateTime date);
	virtual CSize CalcSize (BOOL bVertDock);

	static const UINT CBR_WEEKDAYSEL;
	static const UINT CBR_WEEKNUMBER;
	static const UINT CBR_MULTISELECTION;
	static const UINT CBR_ENABLED;
	static const UINT CBR_NAVIGATION_BUTTONS;

	// Generated message map functions
protected:
	//{{AFX_MSG(CBCGPCalendarBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void CommonInit ();
	
	virtual void Serialize (CArchive& ar);
	virtual void ShowCommandMessageString (UINT uiCmdId);
	
	CBORROW_Calendar		m_wndCalendar;
	UINT				m_nCommandID;
	BOOL				m_bIsTearOff;
	UINT				m_styleFlags;
	BOOL				m_bIsCtrl;
};

BORROW_DLLEXPORT extern UINT BORROW_CALENDAR_ON_SELCHANGED;

class CBORROW_MonthPickerWnd : public CMiniFrameWnd
{
// Construction
public:
	CBORROW_MonthPickerWnd (CBORROW_Calendar* pCalendarWnd, int nMonthIndexInCalendar);

// Attributes
protected:
	CBORROW_Calendar*	m_pCalendarWnd;
	int				m_nFirstVisibleMonth;
	int				m_nFirstVisibleYear;
	int				m_nRowHeight;
	int				m_nSelRow;
	int				m_nMonthIndexInCalendar;

	CString			m_strMonthName [12];

	int				m_nElapseInterval;
	UINT_PTR		m_nIDTimerEvent;

// Operations
protected:
	void PrevMonth ();
	void NextMonth ();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGPMonthPickerWnd)
	public:
	virtual BOOL Create(CPoint ptCenter);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBORROW_MonthPickerWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBCGPMonthPickerWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CBORROW_DefaultLocale
{
public:
	CBORROW_DefaultLocale ();
	~CBORROW_DefaultLocale ();

protected:
	CString	m_strLanguage;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPCALENDARBAR_H__9B759C19_6B1C_48C4_A884_C0DEDD60DD77__INCLUDED_)
