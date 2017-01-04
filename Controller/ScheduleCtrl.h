#pragma once
#include "afxwin.h"

#include "ScheduleCtrlTypes.h"
#include "ScheduleView.h"
#include "AppointmentsManager.h"
#include "BaseDropTarget.h"
#include "BorrowSkin/BORROW_CalendarBar.h"
#include "ScheduleRangeManager.h"
#include "Controller.h"
#include "scheduleCtrlCmds.h"
// wParam - not used, lParam - points to the appointment
extern UINT SCHEDULE_ADD_APPOINTMENT;

// wParam - not used, lParam - points to the appointment
extern UINT SCHEDULE_BEFORE_UPDATE_APPOINTMENT;
// wParam - not used, lParam - points to the appointment
extern UINT SCHEDULE_UPDATE_APPOINTMENT;
// wParam - not used, lParam - points to the appointment
// Return value:
// -1L - cancel delete
//  0L - delete single appointment or only occurrence of the series
//  1L - delete full series
extern UINT SCHEDULE_REMOVE_APPOINTMENT;
// wParam, lParam - not used
extern UINT SCHEDULE_REMOVE_ALL_APPOINTMENTS;

// wParam - TRUE - selected, otherwise - not selected, lParam - points to the appointment
extern UINT SCHEDULE_SELECT_APPOINTMENT;

// wParam - not used, lParam - points to the appointment
extern UINT SCHEDULE_SET_DEFAULT_APPOINTMENT;

// wParam - old type, lParam - new style
extern UINT SCHEDULE_STYLE_CHANGED;
// wParam, lParam - not used
extern UINT SCHEDULE_DATE_CHANGED;

// wParam - new time delta, lParam - not used
extern UINT SCHEDULE_TIME_DELTA_CHANGED;

// wParam, lParam - see WM_LBUTTONDBLCLK
extern UINT SCHEDULE_LBUTTONDBLCLK;
// wParam, lParam - see WM_KEYDOWN
extern UINT SCHEDULE_KEYDOWN;

// wParam, lParam - not used
extern UINT SCHEDULE_DROP_APPOINTMENTS;
// wParam - drop effect code, lParam - not used
extern UINT SCHEDULE_DRAG_APPOINTMENTS;

// wParam - operation code, lParam - not used
// Return value:
// -1L - cancel operation
//  0L - allow operation
extern UINT SCHEDULE_BEGIN_CHANGE_OPERATION;
// wParam - operation code, lParam - result of operation
extern UINT SCHEDULE_END_CHANGE_OPERATION;

#define CALENDAR_CTRL_HEIGHT 30
#define _USEING_TOOLBAR
using namespace _SCHEDULECTRLCMDS;
class ScheduleCtrl : public CWnd
{
    DECLARE_DYNAMIC(ScheduleCtrl)
	DECLARE_SCHEULE_FRIEND_CMDS()

    friend class ScheduleView;
    friend class ScheduleViewStyleAlpha;
public:
    ScheduleCtrl(void);
    virtual ~ScheduleCtrl(void);
	int CreateButton();
	afx_msg void OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepicker2(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnDatetimechangeDatetimepicker1();
	afx_msg void OnDatetimechangeDatetimepicker2();


    void SetScheduleID(INT ID){m_ScheduleID = ID;}
    INT  GetScheduleID(){return m_ScheduleID;}
    void SetScheduleGroup(ClientGroup &clientGroup){m_Group = clientGroup;}
    ClientGroup  &GetScheduleGroup(){return m_Group;}

    void SetDefaultAppointment(const Appointment* pApp, BOOL bRedraw = FALSE);
    const Appointment* GetDefaultAppointment();
	AppointmentList & GetSelectedAppointments() { return m_SelectedAppsList ;}

    // appointments
    BOOL AddAppointment (Appointment* pApp, BOOL bQuery = TRUE, BOOL bRedraw = TRUE, BOOL bNotify = TRUE);
    BOOL UpdateAppointment (Appointment* pApp, const COleDateTime& dtNewStart, const COleDateTime& dtNewEnd, 
        BOOL bQuery = TRUE, BOOL bRedraw = TRUE);
    BOOL RemoveAppointment (Appointment* pApp, BOOL bQuery = TRUE, BOOL bRedraw = TRUE);
	void RemoveSelections(void);
    void RemoveSelectedAppointments (BOOL bQuery = TRUE, BOOL bRedraw = TRUE);
    BOOL RemoveAllAppointments (BOOL bRedraw = TRUE);

    void SelectAppointment (Appointment* pApp, BOOL bSelect, BOOL bRedraw = TRUE);
    void SelectAppointments (const AppointmentArray& Apps, BOOL bSelect, BOOL bRedraw = TRUE);
    void SelectArea (const CRect& rect, BOOL bRedraw = TRUE);

	
    void AlignUp (Appointment* pApp, BOOL bRedraw = TRUE);
    void AlignDown (Appointment* pApp, BOOL bRedraw = TRUE);
    void AlignUpSelectedAppointments (BOOL bRedraw = TRUE);
    void AlignDownSelectedAppointments (BOOL bRedraw = TRUE);
	int	 GetDisplayRange();
	// for cmd system - zxy
	void AlignSelections(BOOL bUp);

    BOOL IsAppointmentInSelection (const Appointment* pApp) const;

    void ClearAppointmentSelection (BOOL bRedraw = TRUE);

    int GetSelectedAppointmentsCount () const;
    int GetCopiedAppointmentsCount () const;

    POSITION GetFirstSelectedAppointment () const;
    POSITION GetLastSelectedAppointment () const;
    /*const*/ Appointment* GetNextSelectedAppointment (POSITION& pos) const;
    /*const*/ Appointment* GetPrevSelectedAppointment (POSITION& pos) const;

    void QueryAppointments (BOOL bRedraw = TRUE);
    void QueryAppointments (const COleDateTime& date1, const COleDateTime& date2, BOOL bRedraw = TRUE);
    void QueryAll (BOOL bRedraw = TRUE);

    int GetQueryedAppointmentsCount () const;
    int GetQueryedAppointments (AppointmentArray& ar) const;

    AppointmentArray& GetQueryedAppointments ()
    {
        return m_QueryAppsArray;
    }

    const AppointmentArray& GetQueryedAppointments () const
    {
        return m_QueryAppsArray;
    }

    // Copy paste
    COleDateTimeSpan GetSelectedAppointmentsRange();
    BOOL HorizontallyCopyAndPaste(const int nCopies, COPY_DIRECTION Direction = COPY_RIGHT, DWORD DaysInWeek = MULTICOPY_ALLWEEK);
    BOOL HorizontallyCopyAndPaste(const COleDateTime& RangeStart, const COleDateTime& RangeEnd, DWORD DaysInWeek = MULTICOPY_ALLWEEK);
    BOOL VerticallyCopyAndPaste(const int nCopies, COPY_DIRECTION Direction = COPY_DOWN);
    //BOOL Copy();
    BOOL Paste(const AppointmentArray& ar);
    BOOL GetContinuerousDeleteRanageDate(const int nCopies, COPY_DIRECTION Direction,
        COleDateTime& S, COleDateTime& E);
    BOOL ContinuerousDelete(const int nCopies, COPY_DIRECTION Direction = COPY_RIGHT);

    // Attributes

    void SetDate (const COleDateTime& date, BOOL bRedraw = TRUE);
    void SetDateInterval (const COleDateTime& date1, const COleDateTime& date2, BOOL bRedraw = TRUE);
    void SetToday (BOOL bRedraw = TRUE);

    void UpdateCalendarsSelection ();
    void SetCalendar(CBORROW_Calendar* cal){m_pCalendar = cal;}
    CBORROW_Calendar* GetCalendar(){return m_pCalendar;}

    void SetStyle (_SVIEW_STYLE	Style, BOOL bRedraw = TRUE);

    _SVIEW_STYLE GetStyle () const
    {
        return m_Style;
    }

    void SetTimeDelta (_SVIEW_TIME_DELTA delta, 
        BOOL bRedraw = TRUE);

    _SVIEW_TIME_DELTA GetTimeDelta () const
    {
        return m_TimeDelta;
    }

    // drag and drop
    afx_msg LRESULT OnDragLeave(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDrop(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragEnter(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragOver(WPARAM wParam, LPARAM lParam);
    // clipboard
    static CLIPFORMAT GetClipboardFormat ();

	void GetDateInterval(COleDateTime &dBegin ,COleDateTime &dEnd);
    // serializing
    static BOOL SerializeTo (CFile& file, AppointmentArray& ar, COleDateTime& dateFrom = COleDateTime());
    static BOOL SerializeTo (CArchive& Archive, AppointmentArray& ar, COleDateTime& dateFrom);

    BOOL SerializeFromLayout (CFile& file, AppointmentArray& ar, 
        _SVIEW_STYLE style, const COleDateTime& dtTo);
    static BOOL SerializeFromTimeTable (CFile& file, AppointmentArray& ar, 
        _SVIEW_STYLE style, const COleDateTime& dtTo);
    static BOOL SerializeFromAppointment (CFile& file, AppointmentArray& ar, 
        _SVIEW_STYLE style, const COleDateTime& dtTo);
    static BOOL SerializeFromAppointment (CArchive& Archive, AppointmentArray& ar, 
        _SVIEW_STYLE style, const COleDateTime& dtTo);
    // clipboard
    static CLIPFORMAT s_ClpFormat;
    static CString s_ClpFormatName;

    // serializing
    BOOL SerializeTo (CFile& file);
    BOOL SerializeFrom (CFile& file, const COleDateTime& dtTo, CLIPFORMAT DataFormat);

    CFont& GetRegularFont() {return m_RegularFont;}
    CFont& GetRegularBoldFont() {return m_RegularBoldFont;}
    CFont& GetSmallFont() {return m_SmallFont;}
    CFont& GetSmallBoldFont() {return m_SmallBoldFont;}

    static void GetListImage(CImageList &ImageList,CBitmap &Bitmap ,int nImageIdx);

    const COleDateTime& GetCurSelectedDate() const {return m_CurSelectedDate;}

    ScheduleRangeManager        m_ScheduleRangeHolder;
protected:

    //Hit tests
    virtual _SVIEW_HITTEST HitTest (const CPoint& point) const;
    virtual _SVIEW_HITTEST HitTestArea (const CPoint& point) const;
    virtual _SVIEW_HITTEST HitTestAppointment (const CPoint& point) const;

    ScheduleView* GetCurrentView () const
    {
        //ASSERT_VALID (m_pCurrentView);
        return m_pCurrentView;
    }

    HFONT SetCurrFont (CDC* pDC);
    virtual BOOL PreCreateWindow (CREATESTRUCT& cs);

    OperationLayer* GetOpLayer (){return &m_OpLayer;}

    virtual void ProcessPaint(CDC* pDCPaint);
	void ProcessHeaderCtrls( CDC *pDC ,CRect rectClient);

    _SVIEW_OPERATION GetChangeOperation() const
    {
        return m_Operation;
    }

    BOOL UpdateChangeOperation (_SVIEW_OPERATION operation, BOOL bResult = FALSE);

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 
	COleDateTime m_datatimectrl1;
	COleDateTime m_datatimectrl2;
	void PostNcDestroy(){delete this;};
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nStyle, int cx, int cy);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

    
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnTimeChange();
    afx_msg void OnCancelMode();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
    afx_msg LRESULT OnSetFont (WPARAM, LPARAM);
    afx_msg LRESULT OnGetFont (WPARAM, LPARAM);
    afx_msg BOOL OnNeedToolTipText(UINT id, NMHDR* pNMH, LRESULT* pResult);
    afx_msg LRESULT OnUpdateToolTips (WPARAM, LPARAM);

    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSchedulePrevBtnClicked();
	afx_msg void OnScheduleNextBtnClicked();
	afx_msg void OnScheduleMovePrevBtnClicked();
	afx_msg void OnScheduleMoveNextBtnClicked();
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();


private:
    ScheduleView* GetView (_SVIEW_STYLE style);

    void StartSelect ();
    void StopSelect ();
    void StartRelocate ();
    void StopRelocate (BOOL bCancel);

    void StartDragDrop ();
    void StopDragDrop ();


    void StartCapture ();
    void StopCapture ();
    BOOL IsCaptured(){return m_bCaptured;}

    void RelocateDragedAppointments();
    BOOL IsAppointmentsOverlap(const Appointment& App1, const Appointment& App2);
    BOOL IsAppointmentOverlapWithOthers(const Appointment* pApp,
        const COleDateTimeSpan& offsetStart, const COleDateTimeSpan& offsetEnd);
    BOOL IsNewAppointmentOverlapWithExists(const Appointment* pApp);
    BOOL IsSelectedAppointmentOverlapWithOthers(
        const COleDateTimeSpan& offsetStart, const COleDateTimeSpan& offsetEnd, DWORD DaysInWeek = MULTICOPY_ALLWEEK);

	BOOL GetSelectedAppointmentOverlapWithOthers(
		const COleDateTimeSpan& offsetStart, const COleDateTimeSpan& offsetEnd,  AppointmentArray &arOverlapApps, DWORD DaysInWeek = MULTICOPY_ALLWEEK);
    BOOL IsAppointmentsOverlapWithExsits(const AppointmentArray& pApps, const COleDateTimeSpan& offset);

    void InitFonts();
	void MovetoPrevInterval(int nVal);
	void MovetoNextInterval(int nVal);
protected:
	//////////////////////////////////////////////////////////////////////
	ScheduleCmdEngine *GetCmdEngine(){ return &m_CmdEngine;};
	//cmd system -zxy.
	// for undo/redo paste cmd. 
	void OnBeginPaste();
	void OnPasteDone( Appointment *pApp);
	void OnEndPaste();
	void BeginDragTest();
	void EndDragTest();
	// for undo/redo drag cmd.-zxt	
	void BeginAddAppointments(void);
	void OnAddAppointment(Appointment *pApp);
	void EndAddAppointments(void);
	// for undo/redo hcopypaste
	void BeginHCopyPaste();
	void RemoveAppointInHCopyPaste(Appointment *pApp);
	void OnHCopyPasteDone(Appointment *pApp);
	void EndHCopyPaste();
	// for undo/redo  vcopypaste
	void BeginVCopyPaste();
	void RemoveAppointInVCopyPaste(Appointment *pApp);
	void OnVCopyPasteDone(Appointment *pApp);
	void EndVCopyPaste();

	void AddJustCurrentViewLayout(BOOL bRedraw);
	/////////////////////////////////////////////////////////////////////
protected:
    INT                         m_ScheduleID;
    ClientGroup                 m_Group;
    COLORREF					m_clrBackground;

    // first & last - double representation
    // [0 - 23].[0 - 59]
    double						m_nFirstWorkingHour;
    double						m_nLastWorkingHour;

    int					        m_nWeekStart;

    _SVIEW_STYLE			    m_Style;			// view Style
    _SVIEW_OPERATION            m_Operation;
    OPRATION_MODE               m_OpMode;

    AppointmentsManager         m_AppsManager;
    Appointment                 m_DefaultAppointment;

    AppointmentArray		    m_QueryAppsArray;
    AppointmentList		        m_SelectedAppsList;

    _SVIEW_TIME_DELTA           m_TimeDelta;

    ScheduleView*			    m_pViews [1];
    ScheduleView*			    m_pCurrentView;

    OperationLayer              m_OpLayer;

    _SVIEW_HITTEST              m_HitTest;
    BOOL						m_bCaptured;
    CWnd*						m_pWndLastCapture;
    CPoint						m_ptOpStart;
    CPoint						m_ptOpCurrent;

    COleDateTime                m_CurSelectedDate;
    CBORROW_Calendar*           m_pCalendar;

    BOOL						m_bRelocate;

    CBaseDropTarget		        m_DropTarget;
    COleDropSource		        m_DropSource;
    BOOL						m_bDragDrop;
    DROPEFFECT					m_dragEffect;
    COleDateTime				m_dtDrop;
    BOOL						m_bRedrawAfterDrop;

    BOOL						m_bNotifyParent;

    CFont                       m_RegularFont;
    CFont                       m_RegularBoldFont;
    CFont                       m_SmallFont;
    CFont                       m_SmallBoldFont;
	CMFCToolTipCtrl				m_TodayBtnToolTip;
    CImageList                  m_ScheduleItemIconList;
    typedef CMap<UINT, UINT, UINT, UINT>     CMapImageIndex;
    CMapImageIndex              m_IconDetailIDMapper;

	CStatic *pStatic;
	CStatic *pStatic2;
	CBORROW_DateTimeCtrl *pStartTimeCtrl;
	CBORROW_DateTimeCtrl *pEndTimeCtrl; 
	CMFCButton		m_PrevBtn;
	CMFCButton		m_NextBtn;
	CMFCButton		m_MovePrevBtn;
	CMFCButton		m_MoveNextBtn;
	ScheduleCmdEngine m_CmdEngine;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	COleDateTime GetStartTime();
	COleDateTime GetEndTime();

	CImageList * GetScheduleItemIconList() { return &m_ScheduleItemIconList;}
	CMapImageIndex * GetIconDetailIDMapper() { return &m_IconDetailIDMapper;}
};

