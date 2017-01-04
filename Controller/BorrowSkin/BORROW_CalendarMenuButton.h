#if !defined(AFX_BCGPCALENDARMENUBUTTON_H__B5E358A5_9FBF_40EF_94DA_983CEEECB6C1__INCLUDED_)
#define AFX_BCGPCALENDARMENUBUTTON_H__B5E358A5_9FBF_40EF_94DA_983CEEECB6C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BORROW_Pro.h"


class BORROW_DLLEXPORT CBORROW_CalendarMenuButton : public CMFCToolBarMenuButton  
{
	DECLARE_SERIAL(CBORROW_CalendarMenuButton)

public:
	CBORROW_CalendarMenuButton();
	CBORROW_CalendarMenuButton(UINT uiCmdID, LPCTSTR lpszText);
	virtual ~CBORROW_CalendarMenuButton();

	virtual void SetDate (const COleDateTime& date, BOOL bNotify = TRUE);
	COleDateTime GetDate () const
	{
		return m_Calendar;
	}

	static const COleDateTime GetCalendarByCmdID (UINT uiCmdID);
	static void SetCalendarByCmdID (UINT uiCmdID, const COleDateTime& date)
	{
		m_CalendarsByID.SetAt (uiCmdID, date);
	}

/*	void SetColumnsNumber (int nColumns)
	{
		m_nColumns = nColumns;
	}*/

	void EnableTearOff (UINT uiID, 
						int nVertDockColumns = -1,
						int nHorzDockRows = -1);

	virtual void OnChangeParentWnd (CWnd* pWndParent);

protected:
	virtual void OnDraw (CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages,
						BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
						BOOL bHighlight = FALSE,
						BOOL bDrawBorder = TRUE,
						BOOL bGrayDisabledButtons = TRUE);
	//virtual CMFCPopupMenu* CreatePopupMenu ();
	virtual BOOL IsEmptyMenuAllowed () const
	{
		return TRUE;
	}

	virtual void Serialize (CArchive& ar);
	virtual void CopyFrom (const CMFCToolBarButton& src);
	virtual int OnDrawOnCustomizeList (CDC* pDC, const CRect& rect, BOOL bSelected);
	virtual BOOL OnClick (CWnd* pWnd, BOOL bDelay);
	void Initialize ();

protected:
	COleDateTime				m_Calendar;	// Currently selected day
	int							m_nColumns;
	int							m_nVertDockColumns;
	int							m_nHorzDockRows;

	BOOL						m_bStdCalendarDlg;

	static CMap<UINT,UINT,COleDateTime, COleDateTime>	m_CalendarsByID;
};

#endif // !defined(AFX_BCGPCALENDARMENUBUTTON_H__B5E358A5_9FBF_40EF_94DA_983CEEECB6C1__INCLUDED_)
