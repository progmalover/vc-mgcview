#pragma once
#include "resource.h"
#include "afxwin.h"
#include "TimeTableCtrlType.h"
#include "TimeTableView.h"
#include "BaseDropTarget.h"
#include "TimeTableItemManager.h"
#include "ScheduleCtrl.h"
#include "timetablectrlcmds.h"

// wParam - not used, lParam - points to the appointment
extern UINT TIMETABLE_ADD_APPOINTMENT;

// wParam - not used, lParam - points to the appointment
extern UINT TIMETABLE_BEFORE_UPDATE_APPOINTMENT;
// wParam - not used, lParam - points to the appointment
extern UINT TIMETABLE_UPDATE_APPOINTMENT;
// wParam - not used, lParam - points to the appointment
// Return value:
// -1L - cancel delete
//  0L - delete single appointment or only occurrence of the series
//  1L - delete full series
extern UINT TIMETABLE_REMOVE_APPOINTMENT;
// wParam, lParam - not used
extern UINT TIMETABLE_REMOVE_ALL_APPOINTMENTS;

// wParam - TRUE - selected, otherwise - not selected, lParam - points to the appointment
extern UINT TIMETABLE_BEFORE_SELECT_APPOINTMENT;
// wParam - TRUE - selected, otherwise - not selected, lParam - points to the appointment
extern UINT TIMETABLE_SELECT_APPOINTMENT;

// wParam - not used, lParam - points to the appointment
extern UINT TIMETABLE_SET_DEFAULT_APPOINTMENT;

// wParam - old type, lParam - new style
extern UINT TIMETABLE_STYLE_CHANGED;
// wParam, lParam - not used
extern UINT TIMETABLE_DATE_CHANGED;

// wParam - new time delta, lParam - not used
extern UINT TIMETABLE_TIME_DELTA_CHANGED;

// wParam, lParam - see WM_LBUTTONDBLCLK
extern UINT TIMETABLE_LBUTTONDBLCLK;
// wParam, lParam - see WM_KEYDOWN
extern UINT TIMETABLE_KEYDOWN;

// wParam, lParam - not used
extern UINT TIMETABLE_DROP_APPOINTMENTS;
// wParam - drop effect code, lParam - not used
extern UINT TIMETABLE_DRAG_APPOINTMENTS;

// wParam - operation code, lParam - not used
// Return value:
// -1L - cancel operation
//  0L - allow operation
extern UINT TIMETABLE_BEGIN_CHANGE_OPERATION;
// wParam - operation code, lParam - result of operation
extern UINT TIMETABLE_END_CHANGE_OPERATION;
extern UINT TIMETABLE_AFTERUNITDELETED ;

#define TIMETABLE_SELECTNO_APPOINTMENTS TIMETABLE_AFTERUNITDELETED
// CTimeTableCtrl
using namespace _TIMETABLECTRLCMDS;
class TimeTableView;

class CTimeTableCtrl : public CWnd
{
	DECLARE_DYNAMIC(CTimeTableCtrl)
	DECLARE_TIMETABLE_FRIEND_CMDS()
	friend class TimeTableiew;
	friend class TimeTableViewStyleAlpha;
public:
	CTimeTableCtrl();
	virtual ~CTimeTableCtrl();

    void CloseTimeTable (BOOL bRedraw = TRUE);
	void BeginPreparingTimeTable(INT ID){m_TimeTableID = ID; m_bPreparing = TRUE;}
    void EndPreparingTimeTable(){m_bPreparing = FALSE;RedrawWindow();}
    BOOL IsTimeTableOpened(){return m_TimeTableID >= -2;}
	INT  GetTimeTableID(){return m_TimeTableID;}

    BOOL IsModified(){ return m_bModified; }
    void SetModified(){if(!m_bPreparing)m_bModified = TRUE;}
    void ClearModified(){m_bModified = FALSE;}

    void SetReadOnly(BOOL bReadOnly = TRUE){m_bReadOnly = bReadOnly;}

	void SetStyle (_TVIEW_STYLE	Style, BOOL bRedraw = TRUE);

	//
	//appending for cmds system - zxy
	//
	virtual BOOL BeginAddAppointments();
	virtual BOOL OnAddAppointment(Appointment *pAdd);
	virtual BOOL EndAddAppointments();
	TimeTableItemManager & GetItemManager(){return m_ItemManager;};
	_TVIEW_STYLE GetStyle () const
	{
		return m_Style;
	} 

	void SetTimeDelta (_TVIEW_TIME_DELTA delta, BOOL bRedraw = TRUE);

	_TVIEW_TIME_DELTA GetTimeDelta () const
	{
		return m_TimeDelta;
	}

    ////////

    // appointments
    BOOL AddAppointment (Appointment* pApp,BOOL bRedraw = TRUE, BOOL bNotify = TRUE);
    BOOL UpdateAppointment (Appointment* pApp, const COleDateTime& dtNewStart, const COleDateTime& dtNewEnd, 
        BOOL bRedraw = TRUE, BOOL bNotify = TRUE);
    BOOL RemoveAppointment (Appointment* pApp, BOOL bRedraw = TRUE);
	
	void DelSelections();
    void RemoveSelectedAppointments (BOOL bRedraw = TRUE);
    BOOL RemoveAllAppointments (BOOL bRedraw = TRUE);


	CFont& GetRegularFont() {return m_RegularFont;}
	CFont& GetRegularBoldFont() {return m_RegularBoldFont;}
	CFont& GetSmallFont() {return m_SmallFont;}
	CFont& GetSmallBoldFont() {return m_SmallBoldFont;}


    void RelocateDragedItems();

    int GetSelectedAppointmentsCount () const;

    POSITION GetFirstSelectedAppointment () const;
    POSITION GetLastSelectedAppointment () const;
    /*const*/ Appointment* GetNextSelectedAppointment (POSITION& pos) const;
    /*const*/ Appointment* GetPrevSelectedAppointment (POSITION& pos) const;

	void SelectArea (const CRect& rect, BOOL bRedraw = TRUE);

	const COleDateTime& GetCurSelectedDate() const {return m_CurSelectedDate;}

    const AppointmentList& GetItemList(){return m_ItemManager.m_ItemList; }

    void SelectItem (Appointment* pApp, BOOL bSelect, BOOL bRedraw = TRUE);

    BOOL IsAppointmentInSelection (const Appointment* pApp) const;

    void ClearAppointmentSelection (BOOL bRedraw = TRUE);

    static void GetListImage(CImageList &ImageList,CBitmap &Bitmap ,int nImageIdx);

    BOOL SerializeTo (CFile& file);

    static BOOL SerializeFromAppointment (CFile& file, AppointmentArray& ar, 
        _TVIEW_STYLE style, const COleDateTime& dtTo);
    BOOL SerializeFromLayout (CFile& file, AppointmentArray& ar, 
        _TVIEW_STYLE style, const COleDateTime& dtTo);
    static BOOL SerializeFromAppointment (CArchive& Archive, AppointmentArray& ar, 
        _TVIEW_STYLE style, const COleDateTime& dtTo);

    BOOL SerializeFrom (CFile& file, const COleDateTime& dtTo, CLIPFORMAT Format);
	TimeTableCmdEngine *GetCmdEngine(){return &m_CmdEngine;};
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nStyle, int cx, int cy);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

    afx_msg LRESULT OnDragLeave(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDrop(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragEnter(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragOver(WPARAM wParam, LPARAM lParam);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnUndo();
	afx_msg void OnRedo();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();

protected:
	virtual void ProcessPaint(CDC* pDCPaint);

	TimeTableView* GetCurrentView () const
	{
		//ASSERT_VALID (m_pCurrentView);
		return m_pCurrentView;
	}

	virtual BOOL PreCreateWindow (CREATESTRUCT& cs);

private:
	TimeTableView* GetView (_TVIEW_STYLE style);

	void StartSelect ();
	void StopSelect ();
	void StartRelocate ();
	void StopRelocate (BOOL bCancel);

	void StartDragDrop ();
	void StopDragDrop ();

	void StartCapture ();
	void StopCapture ();
	BOOL IsCaptured(){return m_bCaptured;}

	void InitFonts();

protected:
	INT                         m_TimeTableID;
	COLORREF					m_clrBackground;
	_TVIEW_STYLE			    m_Style;			// view Style
	_TVIEW_TIME_DELTA           m_TimeDelta;
	TimeTableView*			    m_pViews [1];
	TimeTableView*			    m_pCurrentView;
	_TOPRATION_MODE             m_OpMode;

    TimeTableItemManager        m_ItemManager;
    AppointmentList		        m_SelectedItemList;

	_TVIEW_HITTEST              m_HitTest;
	BOOL						m_bCaptured;
	CWnd*						m_pWndLastCapture;

	CPoint						m_ptOpStart;
	CPoint						m_ptOpCurrent;

	COleDateTime                m_CurSelectedDate;

	BOOL						m_bNotifyParent;


	BOOL						m_bRelocate;

	CBaseDropTarget 		    m_DropTarget;
    COleDataSource              m_OleDataSource;
	BOOL						m_bDragDrop;
	DROPEFFECT					m_dragEffect;
	COleDateTime				m_dtDrop;
	BOOL						m_bRedrawAfterDrop;

	CFont                       m_RegularFont;
	CFont                       m_RegularBoldFont;
	CFont                       m_SmallFont;
	CFont                       m_SmallBoldFont;

	CImageList                  m_AppointmentIconList;
	typedef CMap<UINT, UINT, UINT, UINT>     CMapImageIndex;
	CMapImageIndex              m_IconDetailIDMapper;

    BOOL                        m_bModified;
    BOOL                        m_bPreparing;
    BOOL                        m_bReadOnly;

	TimeTableCmdEngine			m_CmdEngine;
};


