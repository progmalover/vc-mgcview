// TimeTableCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TimeTableCtrl.h"
#include "TimeTableViewStyleAlpha.h"
#include "LayoutCategoryCtrl.h"
#include "Controller.h"
#include "timetableCtrlcmds.h"
// CTimeTableCtrl

// wParam - not used, lParam - points to the appointment
UINT TIMETABLE_ADD_APPOINTMENT			= ::RegisterWindowMessage (_T("TIMETABLE_ADD_APPOINTMENT"));
// wParam - not used, lParam - points to the appointment
// Return value:
// -1L - cancel update
//  0L - update
UINT TIMETABLE_BEFORE_UPDATE_APPOINTMENT	= ::RegisterWindowMessage (_T("TIMETABLE_BEFORE_UPDATE_APPOINTMENT"));
// wParam - not used, lParam - points to the appointment
UINT TIMETABLE_UPDATE_APPOINTMENT		= ::RegisterWindowMessage (_T("TIMETABLE_UPDATE_APPOINTMENT"));
// wParam - not used, lParam - points to the appointment
// Return value:
// -1L - cancel delete
//	0L - delete single appointment or only occurrence of the series
//	1L - delete full series
UINT TIMETABLE_REMOVE_APPOINTMENT		= ::RegisterWindowMessage (_T("TIMETABLE_REMOVE_APPOINTMENT"));
// wParam, lParam - not used
UINT TIMETABLE_REMOVE_ALL_APPOINTMENTS	= ::RegisterWindowMessage (_T("TIMETABLE_REMOVE_ALL_APPOINTMENTS"));

// wParam - TRUE - select, otherwise - unselect, lParam - points to the appointment
// Return value:
// -1L - cancel operation
//  0L - allow operation
UINT TIMETABLE_BEFORE_SELECT_APPOINTMENT = ::RegisterWindowMessage (_T("TIMETABLE_BEFORE_SELECT_APPOINTMENT"));
// wParam - TRUE - selected, otherwise - not selected, lParam - points to the appointment
UINT TIMETABLE_SELECT_APPOINTMENT        = ::RegisterWindowMessage (_T("TIMETABLE_SELECT_APPOINTMENT"));

// wParam - not used, lParam - points to the appointment
UINT TIMETABLE_SET_DEFAULT_APPOINTMENT = ::RegisterWindowMessage (_T("TIMETABLE_SET_DEFAULT_APPOINTMENT"));

// wParam - old style, lParam - new style
UINT TIMETABLE_STYLE_CHANGED				= ::RegisterWindowMessage (_T("TIMETABLE_STYLE_CHANGED"));
// wParam, lParam - not used
UINT TIMETABLE_DATE_CHANGED				= ::RegisterWindowMessage (_T("TIMETABLE_DATE_CHANGED"));

// wParam - new time delta, lParam - not used
UINT TIMETABLE_TIME_DELTA_CHANGED				= ::RegisterWindowMessage (_T("TIMETABLE_TIME_DELTA_CHANGED"));

// wParam, lParam - see WM_LBUTTONDBLCLK
UINT TIMETABLE_LBUTTONDBLCLK 			= ::RegisterWindowMessage (_T("TIMETABLE_LBUTTONDBLCLK"));
// wParam, lParam - see WM_KEYDOWN
UINT TIMETABLE_KEYDOWN					= ::RegisterWindowMessage (_T("TIMETABLE_KEYDOWN"));

// wParam, lParam - not used
UINT TIMETABLE_DROP_APPOINTMENTS         = ::RegisterWindowMessage (_T("TIMETABLE_DROP_APPOINTMENTS"));
// wParam - drop effect code, lParam - not used
UINT TIMETABLE_DRAG_APPOINTMENTS         = ::RegisterWindowMessage (_T("TIMETABLE_DRAG_APPOINTMENTS"));

// wParam - operation code, lParam - not used
// Return value:
// -1L - cancel operation
//  0L - allow operation
UINT TIMETABLE_BEGIN_CHANGE_OPERATION    = ::RegisterWindowMessage (_T("TIMETABLE_BEGIN_CHANGE_OPERATION"));
// wParam - operation code, lParam - result of operation
UINT TIMETABLE_END_CHANGE_OPERATION      = ::RegisterWindowMessage (_T("TIMETABLE_END_CHANGE_OPERATION"));
UINT TIMETABLE_AFTERUNITDELETED = ::RegisterWindowMessage(_T("TIMETABLE_AFTERUNITDELETED"));
 
IMPLEMENT_DYNAMIC(CTimeTableCtrl, CWnd)

BEGIN_MESSAGE_MAP(CTimeTableCtrl, CWnd)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_KEYDOWN()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_MOUSEWHEEL()
    ON_WM_VSCROLL()
    ON_WM_SETCURSOR()

    ON_REGISTERED_MESSAGE(WM_DND_DRAGENTER, OnDragEnter)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGOVER, OnDragOver)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGLEAVE, OnDragLeave)
    ON_REGISTERED_MESSAGE(WM_DND_DROP, OnDrop)
	ON_COMMAND(ID_EDIT_REDO,OnRedo)
	ON_COMMAND(ID_EDIT_UNDO,OnUndo)
END_MESSAGE_MAP()

CTimeTableCtrl::CTimeTableCtrl()
	: m_TimeTableID        (-1)
	, m_clrBackground	  (CLR_DEFAULT)
	, m_Style			  (_TVIEW_STYLE_ALPHA)
	, m_TimeDelta		  (_TVIEW_TIME_DELTA_1)
	, m_pCurrentView (NULL)
	, m_OpMode (_TOPERATION_NONE)  
    , m_ItemManager ()
    , m_SelectedItemList ()
	, m_HitTest (_TVIEW_HITTEST_NOWHERE) 
	, m_bCaptured (FALSE)
	, m_pWndLastCapture (NULL)
	, m_ptOpStart	      (0, 0)
	, m_ptOpCurrent        (0, 0)
	, m_CurSelectedDate    ()
	, m_bNotifyParent	  (TRUE)
    , m_bRelocate		  (FALSE)
	, m_DropTarget ()
	, m_bDragDrop   (FALSE)
	, m_dragEffect		  (DROPEFFECT_NONE)
	, m_dtDrop()
	, m_bRedrawAfterDrop  (FALSE)
	, m_RegularFont ()
	, m_RegularBoldFont()
	, m_SmallFont()
	, m_SmallBoldFont()
	, m_AppointmentIconList()
	, m_IconDetailIDMapper ()
    , m_bModified (FALSE)
    , m_bPreparing (FALSE)
    , m_bReadOnly (FALSE)
{
	for (int i = 0; i < sizeof (m_pViews) / sizeof (TimeTableView*); i++)
	{
		m_pViews [i] = NULL;
	}

	m_AppointmentIconList.Create(THUMBNAIL_W, THUMBNAIL_H, ILC_COLOR32|ILC_MASK, 1, 1);

	m_AppointmentIconList.Add(AfxGetApp()->LoadIconW(IDI_EMPTYCONTENT));
}

CTimeTableCtrl::~CTimeTableCtrl()
{
	for (int i = 0; i < sizeof (m_pViews) / sizeof (TimeTableView*); i++)
	{
		if (m_pViews [i])
		{
			delete m_pViews [i];
		}
	}

}

int TROUND(double val)
{
    const double c_Median = 0.5;
    return int((val - int(val - c_Median)) >= c_Median ? 
        val + c_Median : val - c_Median);
}



void CTimeTableCtrl::OnUndo()
{
	m_CmdEngine.undo();
}

void CTimeTableCtrl::OnRedo()
{
	m_CmdEngine.redo();

}

BOOL CTimeTableCtrl::PreCreateWindow (CREATESTRUCT& cs)
{
	CRuntimeClass* viewClass[] =
	{
		RUNTIME_CLASS(TimeTableViewStyleAlpha),
	};

	for (int i = 0; i < sizeof (m_pViews) / sizeof (TimeTableView*); i++)
	{
		m_pViews [i] = new TimeTableViewStyleAlpha();

		ASSERT_VALID (m_pViews [i]);
		if (m_pViews [i] == NULL)
		{
			return FALSE;
		}

		if (!m_pViews [i]->IsKindOf (viewClass[i]))
		{
			ASSERT (FALSE);
			return FALSE;
		}
	}

	m_pCurrentView = m_pViews[0];
	m_pCurrentView->OnActivate (this, NULL);
	m_pCurrentView->SetDate(COleDateTime::GetCurrentTime ());
	InitFonts();

	return CWnd::PreCreateWindow (cs);
}

void CTimeTableCtrl::InitFonts()
{
	CWindowDC dc (NULL);

	if (m_RegularFont.GetSafeHandle () != NULL)
	{
		::DeleteObject (m_RegularFont.Detach ());
	}

	if (m_SmallFont.GetSafeHandle () != NULL)
	{
		::DeleteObject (m_SmallFont.Detach ());
	}

	if (m_RegularBoldFont.GetSafeHandle () != NULL)
	{
		::DeleteObject (m_RegularFont.Detach ());
	}

	if (m_SmallBoldFont.GetSafeHandle () != NULL)
	{
		::DeleteObject (m_SmallFont.Detach ());
	}
	//------------------
	// Initialize fonts:
	//------------------
	NONCLIENTMETRICS info;
	const UINT cbProperSize = sizeof(NONCLIENTMETRICS);

	ZeroMemory (&info, sizeof (NONCLIENTMETRICS));
	info.cbSize = cbProperSize;

	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, cbProperSize, &info, 0);

	LOGFONT lf;
	memset (&lf, 0, sizeof (LOGFONT));

	lf.lfCharSet = (BYTE) GetTextCharsetInfo (dc.GetSafeHdc (), NULL, 0);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	//------------------
	// Adjust font size:
	//------------------
	int nFontHeight = lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight;
	if (nFontHeight <= 14)
	{
		nFontHeight = 14;
	}
	else
	{
		nFontHeight--;
	}

	lf.lfHeight = (lf.lfHeight < 0) ? -nFontHeight : nFontHeight;

	lstrcpy (lf.lfFaceName, info.lfMenuFont.lfFaceName);

	m_RegularFont.CreateFontIndirect (&lf);

	//-------------------
	// Create regular bold font:
	//-------------------
	LONG lfWeightSaved = lf.lfWeight;

	lf.lfWeight = FW_BOLD;
	m_RegularBoldFont.CreateFontIndirect (&lf);
	lf.lfWeight = lfWeightSaved;

	//-------------------
	// Create small font:
	//-------------------
	LONG lfHeightSaved = lf.lfHeight;

	lf.lfHeight = (long) ((3. + abs (lf.lfHeight)) * 2 / 3);
	if (lfHeightSaved < 0)
	{
		lf.lfHeight = -lf.lfHeight;
	}

	m_SmallFont.CreateFontIndirect (&lf);

	lf.lfHeight = lfHeightSaved;

	//-------------------
	// Create small font:
	//-------------------
	lfHeightSaved = lf.lfHeight;
	lfWeightSaved = lf.lfWeight;

	lf.lfWeight = FW_BOLD;
	lf.lfHeight = (long) ((1. + abs (lf.lfHeight)) * 2 / 3);
	if (lfHeightSaved < 0)
	{
		lf.lfHeight = -lf.lfHeight;
	}

	m_SmallBoldFont.CreateFontIndirect (&lf);

	lf.lfHeight = lfHeightSaved;
	lf.lfWeight = lfWeightSaved;
}
// CTimeTableCtrl message handlers

void CTimeTableCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	ProcessPaint(&dc);	
}

void CTimeTableCtrl::ProcessPaint(CDC* pDCPaint)
{
	ASSERT_VALID(pDCPaint);
	TimeTableView* pView = GetCurrentView ();	

	CRect rectClient;
	GetClientRect (rectClient);

	CDC memDC;
	if (memDC.CreateCompatibleDC (pDCPaint))
	{
		CBitmap memBitmap;
		if (memBitmap.CreateCompatibleBitmap (pDCPaint, rectClient.Width (), rectClient.Height ()))
		{
			memDC.SelectObject (&memBitmap);
		}
		else
		{
			memDC.DeleteDC ();
		}
	}

	BOOL bMemDC = memDC.GetSafeHdc () != NULL;

	CDC* pDC = bMemDC ? &memDC : pDCPaint;

	pDC->SetBkMode (TRANSPARENT);

	pView->OnPaint (pDC, rectClient);
	if (bMemDC)
	{
		pDCPaint->BitBlt (rectClient.left, rectClient.top, rectClient.Width (), rectClient.Height (),
			&memDC, 0, 0, SRCCOPY);
	}

}

void CTimeTableCtrl::SetStyle (_TVIEW_STYLE style, BOOL bRedraw /*= TRUE*/)
{
	ASSERT (_TVIEW_STYLE_FIRST <= style);
	ASSERT (style <= _TVIEW_STYLE_LAST);

	if (m_Style == style)
	{
		return;
	}

	_TVIEW_STYLE oldStyle = m_Style;

	const BOOL bIsWndCreated = GetSafeHwnd () != NULL;

	if (bIsWndCreated)
	{
		SetRedraw (FALSE);
	}


	//COleDateTime date = GetCurrentView ()->GetDate ();
	TimeTableView* pOldView = m_pCurrentView;

	if (pOldView != NULL)
	{
		pOldView->OnDeactivate (this);
	}

	m_Style = style;

	m_pCurrentView = GetView (m_Style);

	m_pCurrentView->OnActivate (this, pOldView);

	//RestoreAppointmentSelection (lst, FALSE);

	if (bIsWndCreated)
	{
		SetRedraw (TRUE);


		if (bRedraw)
		{
			GetCurrentView()->AdjustLayout(FALSE);

			RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
	}

}

void CTimeTableCtrl::SetTimeDelta (_TVIEW_TIME_DELTA delta, BOOL bRedraw)
{
	m_TimeDelta = delta;

    if (m_bNotifyParent && GetSafeHwnd () != NULL)
    {
        GetParent ()->SendMessage (TIMETABLE_TIME_DELTA_CHANGED, 0, 0);
    }

	if (bRedraw && GetSafeHwnd()!=NULL)
	{
		GetCurrentView()->AdjustLayout(FALSE);
		RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}
}


TimeTableView* CTimeTableCtrl::GetView (_TVIEW_STYLE style)
{
	ASSERT (_TVIEW_STYLE_FIRST <= style);
	ASSERT (style <= _TVIEW_STYLE_LAST);

	switch (style)
	{
	case _TVIEW_STYLE_ALPHA:
		return m_pViews [0];
	}

	return NULL;
}


BOOL CTimeTableCtrl::OnEraseBkgnd(CDC* pDC)
{

	return TRUE;
}

int CTimeTableCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

    // register drop target
    m_DropTarget.Register( this );
	m_CmdEngine.bindTarget(this);
	return 1;
}

void CTimeTableCtrl::OnDestroy()
{

}

void CTimeTableCtrl::OnSize(UINT nStyle, int cx, int cy)
{
	GetCurrentView()->AdjustLayout(FALSE);

	RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

BOOL CTimeTableCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	BOOL bRes = GetCurrentView ()->OnMouseWheel (nFlags, zDelta, pt);

	return bRes;
}

void CTimeTableCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//COleDateTime dtOld (GetCurrentView ()->GetDate ());

	GetCurrentView ()->OnVScroll (nSBCode, nPos, pScrollBar);
}

BOOL CTimeTableCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (GetCurrentView ()->OnSetCursor (pWnd, nHitTest, message))
	{
		return TRUE;
	}

	return CWnd::OnSetCursor (pWnd, nHitTest, message);
}

void CTimeTableCtrl::StartSelect ()
{
	CPoint pt;
	::GetCursorPos (&pt);
	ScreenToClient (&pt);

	GetCurrentView ()->StartSelect ();

	m_OpMode = _TOPERATION_SELECT;
}

void CTimeTableCtrl::StopSelect ()
{
	if(m_OpMode == _TOPERATION_SELECT && m_ptOpStart != m_ptOpCurrent)
		SelectArea(CRect(min(m_ptOpStart.x,m_ptOpCurrent.x),min(m_ptOpStart.y,m_ptOpCurrent.y),
		max(m_ptOpStart.x,m_ptOpCurrent.x),max(m_ptOpStart.y,m_ptOpCurrent.y)));

	GetCurrentView ()->StopSelect();

	m_OpMode = _TOPERATION_NONE;
}

void CTimeTableCtrl::StartCapture ()
{
	if (!m_bCaptured)
	{
		CPoint pt;
		::GetCursorPos (&pt);
		ScreenToClient (&pt);

		m_ptOpStart = pt;
		m_ptOpCurrent = m_ptOpStart;


		m_pWndLastCapture = SetCapture ();

        m_bCaptured = TRUE;        

        m_HitTest = GetCurrentView ()->HitTest (pt);
	}
}

void CTimeTableCtrl::StopCapture ()
{
	if (m_bCaptured)
	{
		m_bCaptured = FALSE;

		::ReleaseCapture ();

		if (m_pWndLastCapture != NULL)
		{
			m_pWndLastCapture->SetCapture ();
			m_pWndLastCapture = NULL;
		}

		StopCapture ();

		m_ptOpStart   = CPoint (0, 0);
        m_ptOpCurrent = m_ptOpStart;     

        m_HitTest = _TVIEW_HITTEST_NOWHERE;
	}
}

void CTimeTableCtrl::StartDragDrop()
{
	if (GetCurrentView () == NULL)
	{
		return;
	}

	if (!m_bDragDrop)
	{
	
	}
}


void CTimeTableCtrl::StopDragDrop()
{
	if (GetCurrentView () == NULL)
	{
		return;
	}

	if (m_bDragDrop)
	{
		m_bDragDrop = FALSE;

		if (m_dragEffect != DROPEFFECT_NONE)
		{
			m_dragEffect = DROPEFFECT_NONE;
		}
	}

}


void CTimeTableCtrl::StartRelocate ()
{
	if (GetCurrentView () == NULL)
	{
		return;
	}

	m_bRelocate = TRUE;

	GetCurrentView ()->StartRelocate();

	m_OpMode = _TOPERATION_DRAG;
}

void CTimeTableCtrl::StopRelocate (BOOL bCancel)
{
	if (GetCurrentView () == NULL)
	{
		return;
	}

	GetCurrentView ()->StopRelocate();

	if (!bCancel && m_bRelocate && m_HitTest == _TVIEW_HITTEST_APPOINTMENT_MOVE)
	{
		m_bRelocate = FALSE;

		CRect RectClient;
		GetClientRect(RectClient);

		CPoint pt;
		::GetCursorPos (&pt);
		ScreenToClient (&pt);

		if (!RectClient.PtInRect(pt))
		{
			//RemoveSelectedAppointments();
		}
		else
        {
            RelocateDragedItems();
		}

		m_OpMode = _TOPERATION_NONE;
	}

}


void CTimeTableCtrl::RelocateDragedItems()
{
    int nOldCount = GetSelectedAppointmentsCount ();

    if (nOldCount > 0)
    {
        BOOL bNotify = m_bNotifyParent && GetSafeHwnd () != NULL;

        COleDateTime datecur (GetCurrentView()->GetDateFromPoint (m_ptOpCurrent));
        COleDateTime datestart (GetCurrentView()->GetDateFromPoint (m_ptOpStart));

        COleDateTimeSpan Movedate = datecur - datestart;

        POSITION pos = m_SelectedItemList.GetHeadPosition();
        while(pos != NULL) 
        { 
            Appointment* pApp = m_SelectedItemList.GetNext(pos);  

            COleDateTime StartTime (pApp->GetStart() + Movedate);
            COleDateTime EndTime (pApp->GetFinish() + Movedate);

            UpdateAppointment(pApp, StartTime, EndTime);
        } 

        GetCurrentView()->AdjustLayout(FALSE);

        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

    }
}

void CTimeTableCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{

}
void CTimeTableCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{

}

void CTimeTableCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)   
{
    if (m_bNotifyParent && GetSafeHwnd () != NULL)
    {
        GetParent ()->SendMessage (TIMETABLE_KEYDOWN, nChar, MAKELPARAM(nRepCnt, nFlags));
    }
}

void CTimeTableCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
    SetFocus ();

    StartCapture ();

    // If view onlbuttondown() return ture, means hit Appointments
	_TOPRATION_MODE  lastMode = this->m_OpMode;
   if (GetCurrentView ()->OnLButtonDown (nFlags, point))
    {
    }
    else
    {
        CWnd::OnLButtonDown(nFlags, point);

        if (m_HitTest != _SVIEW_HITTEST_DEFAULT_APP)
        {
            StartSelect ();
        }
        else
        {
            //ClearAppointmentSelection (TRUE);
        }

        m_CurSelectedDate = GetCurrentView()->GetDateFromPoint(point);
    }

    //For cmd system : zxy
	if(nFlags == 1 &&(m_HitTest == _TVIEW_HITTEST_APPOINTMENT_MOVE || 
		m_HitTest == _TVIEW_HITTEST_APPOINTMENT_BOTTOM)
		)
		{
			TimeTableCmdFactory *pFactory = m_CmdEngine.getFactory();
			ASSERT(pFactory);
			_baseCmd *pLastCmd = pFactory->getBuffedCmd();

			if(NULL != pLastCmd) //last cmd not finished for some reasons.
			{
				if(pLastCmd->isObjOf(CMD_TIEMEOBJ_DRAG))
				{
					((timeObjDragCmd *)pLastCmd)->SaveEndDragInfos();
				}

				if(pLastCmd->isObjOf(CMD_TIEMEOBJ_DURATION_CHANGE))
				{
					((timeObjDurationChangeCmd * )pLastCmd)->SaveEndInfo();
				}
				if(!m_CmdEngine.execCmd(pLastCmd))
					pFactory->delCmd(pLastCmd);
				pFactory->newCmdEnd();
			}

			switch(m_HitTest)
			{
				case _TVIEW_HITTEST_APPOINTMENT_MOVE:
					{
						timeObjDragCmd *pCmd = (timeObjDragCmd *)pFactory->newCmdBegin(CMD_TIEMEOBJ_DRAG);
						pCmd->SaveStartDragInfos();
					}
					break;
				case _TVIEW_HITTEST_APPOINTMENT_BOTTOM:
					{
						timeObjDurationChangeCmd * pCmd = (timeObjDurationChangeCmd *)pFactory->newCmdBegin(CMD_TIEMEOBJ_DURATION_CHANGE);
						pCmd->SaveStartInfo();
					}
					break;
				default:
					break;
			}

		}
}

void CTimeTableCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
    StopDragDrop ();

    SetRedraw(FALSE);
	_TOPRATION_MODE  lastMode = this->m_OpMode;
    if (!GetCurrentView ()->OnLButtonUp (nFlags, point))
    {
        CWnd::OnLButtonUp(nFlags, point);

        StopRelocate (TRUE);

    }
    else
    {
        StopRelocate (FALSE);
    }

	if((m_HitTest == _TVIEW_HITTEST_APPOINTMENT_MOVE || 
		m_HitTest == _TVIEW_HITTEST_APPOINTMENT_BOTTOM))
	{
		TimeTableCmdFactory *pFactory = m_CmdEngine.getFactory();
		ASSERT(pFactory);
		_baseCmd *pCmd = pFactory->newCmdEnd();
		if(pCmd)
		switch(this->m_HitTest )
		{
		case _TVIEW_HITTEST_APPOINTMENT_MOVE:
			{
				ASSERT(pCmd->isObjOf(CMD_TIEMEOBJ_DRAG));
				timeObjDragCmd *pDragCmd = (timeObjDragCmd * )pCmd;
				pDragCmd->SaveEndDragInfos();
				if(!m_CmdEngine.execCmd(pDragCmd))
					pFactory->delCmd(pDragCmd);
			}
			break;
		case _TVIEW_HITTEST_APPOINTMENT_BOTTOM:
			{
				ASSERT(pCmd->isObjOf(CMD_TIEMEOBJ_DURATION_CHANGE));
				timeObjDurationChangeCmd * pDurationCmd = (timeObjDurationChangeCmd *)pCmd;
				pDurationCmd->SaveEndInfo();
				if(!m_CmdEngine.execCmd(pDurationCmd))
					pFactory->delCmd(pDurationCmd);
			}
			break;
		default:
			break;
		}
	}
    StopSelect ();

    StopCapture();

    GetCurrentView()->UpdateOpMode(_TOPERATION_NONE);

	SetRedraw(TRUE);
	RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

	CPoint newpoint1,newpoint2;
	newpoint1.SetPoint(point.x, point.y - 2);
	newpoint2.SetPoint(point.x, point.y + 2);
	if(GetCurrentView()->GetItemFromPoint(newpoint1))
	{
		GetCurrentView ()->OnLButtonDown (nFlags, newpoint1);
	}
	else if(GetCurrentView()->GetItemFromPoint(newpoint2))
	{
		GetCurrentView ()->OnLButtonDown (nFlags, newpoint2);
	}
}

void CTimeTableCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
    if (IsCaptured ())
    {
        m_ptOpCurrent = point;
    }

    if (IsCaptured ()&& m_OpMode == _TOPERATION_NONE)
    {
        if((nFlags & MK_CONTROL) && (nFlags & MK_LBUTTON))
        {
            GetCurrentView()->UpdateOpMode(_TOPERATION_COPY);
            StartDragDrop();
        }
        else if ((nFlags & MK_LBUTTON) != 0)
        {
            StartRelocate();
        }
    }

    BOOL bHandled = GetCurrentView ()->OnMouseMove (nFlags, point);

    if (!bHandled)
    {
        CWnd::OnMouseMove (nFlags, point);
    }
    else if(m_bRelocate)
    {
        if(m_HitTest == _SVIEW_HITTEST_APPOINTMENT_BOTTOM)
        {
            COleDateTime DateCur (GetCurrentView()->GetDateFromPoint (m_ptOpCurrent));

            Appointment* pApp;

            if (GetCurrentView()->GetFoucedItem())
            {
                pApp = GetCurrentView()->GetFoucedItem(); 

                COleDateTime datecur (1900, 1, 1, DateCur.GetHour(), DateCur.GetMinute(), DateCur.GetSecond());
                COleDateTime dateAppStart (1900, 1, 1, 
                    pApp->GetStart().GetHour(), pApp->GetStart().GetMinute(), pApp->GetStart().GetSecond());
                COleDateTime dateAppEnd (1900, 1, 1, 
                    pApp->GetFinish().GetHour(), pApp->GetFinish().GetMinute(), pApp->GetFinish().GetSecond());

                datecur = max(datecur, dateAppStart + COleDateTimeSpan(0,0,0, 10));

                COleDateTimeSpan Movedate = datecur - dateAppEnd;

                COleDateTime EndTime (pApp->GetFinish() + Movedate);

                UpdateAppointment(pApp, pApp->GetStart(), EndTime, FALSE);

				 
				GetParent()->SendMessage(TIMETABLE_SELECT_APPOINTMENT,(WPARAM)TRUE,(LPARAM)pApp);
            } 
            

            m_ptOpStart = m_ptOpCurrent;
        }
    }

    if (m_bDragDrop || m_bRelocate && (nFlags & MK_LBUTTON) != 0)
    {
        //GetCurrentView()->AdjustLayout(FALSE);
    }

    if ((nFlags & MK_LBUTTON) != 0)
    {
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}




void CTimeTableCtrl::SelectItem (Appointment* pApp, BOOL bSelect, BOOL bRedraw)
{
    ASSERT_VALID (pApp);

    int nOldCount = GetSelectedAppointmentsCount ();

    POSITION pos = m_SelectedItemList.Find (pApp);
    BOOL bOperation = (bSelect && (pos == NULL)) || (!bSelect && (pos != NULL));

    if (m_bNotifyParent && GetSafeHwnd () != NULL && bOperation)
    {
        bOperation = GetParent ()->SendMessage (TIMETABLE_BEFORE_SELECT_APPOINTMENT, 
            (WPARAM) bSelect, (LPARAM) pApp) == 0L;
    }

    if (bOperation)
    {
        pApp->SetSelected (bSelect);

        if (bSelect)
        {
			m_SelectedItemList.AddTail (pApp);
		}
        else
        {
            m_SelectedItemList.RemoveAt (pos);
        }

        if (m_bNotifyParent && GetSafeHwnd () != NULL)
        {
            GetParent ()->SendMessage (TIMETABLE_SELECT_APPOINTMENT, (WPARAM) bSelect, (LPARAM) pApp);
        }
    }

    //GetOpLayer()->SetRect(GetCurrentView()->GetUnionRectOfSelectedAppointments());

    //if (GetSelectedAppointmentsCount () != nOldCount && bRedraw)
    {

        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

void CTimeTableCtrl::SelectArea (const CRect& rect, BOOL bRedraw)
{
    //CRect TestRect;
    //ClearAppointmentSelection (FALSE);

    //const AppointmentArray& arApps = GetQueryedAppointments ();

    //int nDay = -1;
    //COleDateTime date = GetCurrentView()->GetDateBegin();
    //COleDateTimeSpan span = COleDateTimeSpan(1, 0, 0, 0);
    //const int nDuration = GetCurrentView()->GetViewDays ();
    //for (int i = 0; i < nDuration; i++)
    //{

    //    for (int i = 0; i < arApps.GetSize (); i++)
    //    {
    //        TestRect.IntersectRect(arApps[i]->GetRectDraw(date), rect);
    //        if (!TestRect.IsRectEmpty())
    //        {
    //            SelectItem(arApps[i], TRUE, FALSE);
    //        }
    //    }

    //    date += span;
    //}

    //if (bRedraw)
    //{
    //    GetOpLayer()->SetRect(GetCurrentView()->GetUnionRectOfSelectedAppointments());

    //    RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    //}
}

void CTimeTableCtrl::ClearAppointmentSelection (BOOL bRedraw)
{
    int nOldCount = GetSelectedAppointmentsCount ();

    if (nOldCount > 0)
    {
        BOOL bNotify = m_bNotifyParent && GetSafeHwnd () != NULL;

        while (m_SelectedItemList.GetTailPosition () != NULL)
        {
            Appointment* pApp = m_SelectedItemList.GetTail ();

            pApp->SetSelected (FALSE);
            m_SelectedItemList.RemoveTail ();

            if (bNotify)
            {
                GetParent ()->SendMessage 
                    (TIMETABLE_SELECT_APPOINTMENT, (WPARAM) FALSE, (LPARAM) pApp);
            }
        }

        if (bRedraw)
        {
            RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        }
    }
}


int CTimeTableCtrl::GetSelectedAppointmentsCount () const
{
    return (int) m_SelectedItemList.GetCount ();
}

POSITION CTimeTableCtrl::GetFirstSelectedAppointment () const
{
    return m_SelectedItemList.GetHeadPosition ();
}

POSITION CTimeTableCtrl::GetLastSelectedAppointment () const
{
    return m_SelectedItemList.GetTailPosition ();
}

/*const*/ Appointment* CTimeTableCtrl::GetNextSelectedAppointment (POSITION& pos) const
{
    return m_SelectedItemList.GetNext (pos);
}

/*const*/ Appointment* CTimeTableCtrl::GetPrevSelectedAppointment (POSITION& pos) const
{
    return m_SelectedItemList.GetPrev (pos);
}

void CTimeTableCtrl::CloseTimeTable (BOOL bRedraw)
{
    m_TimeTableID = -1;

    m_SelectedItemList.RemoveAll();
    m_ItemManager.RemoveAll();

    ClearModified();

    if (bRedraw)
    {
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

}


void CTimeTableCtrl::GetListImage(CImageList &ImageList,CBitmap &Bitmap ,int nImageIdx)
{   
    CClientDC   dcClient(NULL);

    IMAGEINFO   ImageInfo;
    ImageList.GetImageInfo(nImageIdx, &ImageInfo);

    int   nWidth  = abs(ImageInfo.rcImage.right - ImageInfo.rcImage.left);
    int   nHeight = abs(ImageInfo.rcImage.bottom - ImageInfo.rcImage.top);

    Bitmap.CreateCompatibleBitmap(&dcClient, nWidth, nHeight);

    CDC            dcMemory;
    dcMemory.CreateCompatibleDC((CDC*)NULL) ;

    CBitmap*  pBmpOld = dcMemory.SelectObject(&Bitmap);

    dcMemory.FillSolidRect(0, 0, nWidth, nHeight, RGB(255,255,255));

    ImageList.Draw(&dcMemory, nImageIdx, CPoint(0,0), ILD_NORMAL);

    dcMemory.SelectObject(pBmpOld);
    dcMemory.DeleteDC();

    ::DeleteObject( ImageInfo.hbmImage );
    ::DeleteObject( ImageInfo.hbmMask );
}

BOOL CTimeTableCtrl::AddAppointment (Appointment* pApp, BOOL bRedraw, BOOL bNotify)
{
    ASSERT_VALID (pApp);

    if (!m_bPreparing && m_bReadOnly)
    {
        CString ErrorMessage, layoutStr;
        layoutStr.Format(_T("(%s)"), pApp->GetDescription());
        ErrorMessage += Translate(_T("Unable to add ")) + layoutStr + 
            Translate(_T(", Readonly for this Timetable!"));
        MessageBox(ErrorMessage,Translate(_T("Warning:Timetable control")), MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (pApp->GetDuration().m_span < (COleDateTimeSpan(0,0,0,9).m_span + 0.5/(24*60*60)))
    {
        CString ErrorMessage, layoutStr;
        //layoutStr.Format(_T("(%s)"), pApp->GetDescription());
       // ErrorMessage += Translate(_T("Bad Drop, Layout")) + layoutStr + Translate(_T("'s duration should not less than 10 seconds!"));
        ErrorMessage = Translate(_T("Illegal layout, the duration should big than 10 seconds!"));
		MessageBox(ErrorMessage,Translate(_T("Warning:Timetable control")), MB_OK|MB_ICONEXCLAMATION);

        return FALSE;
    }

    if (!m_ItemManager.Add(pApp, pApp->GetStart()))
    {
        return FALSE;
    }

    LRESULT lRes = 0L;

    if (m_bNotifyParent && bNotify && GetSafeHwnd () != NULL)
    {
        lRes = GetParent ()->SendMessage (TIMETABLE_ADD_APPOINTMENT, 0, (LPARAM) pApp);
    }

    if (lRes == LRESULT(-1L))
    {
        return FALSE;
    }

    // UpdateIcon
    if (pApp->GetImageList())
    {
        UINT Index;
        if (!m_IconDetailIDMapper.Lookup(pApp->GetDetailID(), Index))
        {
            CBitmap bitmap;
            GetListImage(*pApp->GetImageList(), bitmap, pApp->GetImageIndex());
            Index = m_AppointmentIconList.Add(&bitmap, RGB(255,255,255));

            m_IconDetailIDMapper.SetAt(pApp->GetDetailID(), Index);
        }

        pApp->SetImage(&m_AppointmentIconList, Index);
    }
    else
    {
        pApp->SetImage(&m_AppointmentIconList, 0);
    }

    if (bRedraw && GetSafeHwnd () != NULL)
    {
        GetCurrentView ()->AdjustLayout (FALSE);

        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

    SetModified();

    return TRUE;
}

BOOL CTimeTableCtrl::UpdateAppointment (Appointment* pApp, const COleDateTime& dtNewStart, 
    const COleDateTime& dtNewEnd,  BOOL bRedraw, BOOL bNotify)
{
    ASSERT_VALID (pApp);

    if (!m_bPreparing && m_bReadOnly)
    {
        return FALSE;
    }

    LRESULT lRes = 0L;  


    if (m_bNotifyParent && GetSafeHwnd () != NULL && bNotify)
    {
        lRes = GetParent ()->SendMessage (TIMETABLE_BEFORE_UPDATE_APPOINTMENT, 0, (LPARAM) pApp);
    }

    pApp->SetInterval(dtNewStart, dtNewEnd, &m_ItemManager);

    SetModified();

    // UpdateIcon
    if (pApp->GetImageList())
    {
        UINT Index;
        if (!m_IconDetailIDMapper.Lookup(pApp->GetDetailID(), Index))
        {
            CBitmap bitmap;
            GetListImage(*pApp->GetImageList(), bitmap, pApp->GetImageIndex());
            Index = m_AppointmentIconList.Add(&bitmap, RGB(255,255,255));

            m_IconDetailIDMapper.SetAt(pApp->GetDetailID(), Index);
        }

        pApp->SetImage(&m_AppointmentIconList, Index);
    }

    if (m_bNotifyParent && GetSafeHwnd () != NULL && bNotify)
    {
        lRes = GetParent ()->SendMessage (TIMETABLE_UPDATE_APPOINTMENT, 0, (LPARAM) pApp);
    }


    if (bRedraw && GetSafeHwnd () != NULL)
    {
        GetCurrentView ()->AdjustLayout (FALSE);

        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

    if (lRes == LRESULT(-1L))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CTimeTableCtrl::RemoveAppointment (Appointment* pApp, BOOL bRedraw)
{
    ASSERT_VALID (pApp);

    if (pApp == NULL)
    {
        return FALSE;
    }

    LRESULT lRes = 0L;

    if (m_bNotifyParent && GetSafeHwnd () != NULL)
    {
        lRes = GetParent ()->SendMessage (TIMETABLE_REMOVE_APPOINTMENT, 0, (LPARAM) pApp);
    }

    if (lRes == LRESULT(-1L))
    {
        return FALSE;
    }


    // remove appointment from selection
    POSITION pos = m_SelectedItemList.Find (pApp);
    if (pos != NULL)
    {
        m_SelectedItemList.RemoveAt (pos);
    }

    m_ItemManager.Remove(pApp);

    if (bRedraw && GetSafeHwnd () != NULL)
    {
        GetCurrentView ()->AdjustLayout (FALSE);

        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

    SetModified();

    return TRUE;
}

void CTimeTableCtrl::DelSelections()
{
	TimeTableCmdFactory *pFactory = (TimeTableCmdFactory *)m_CmdEngine.getFactory();
	timeObjDeleteCmd *pCmd = (timeObjDeleteCmd*) pFactory->newCmd(CMD_TIEMEOBJ_DELETE);
	ASSERT(pFactory);
	ASSERT(pCmd);

	if(!m_CmdEngine.execCmd(pCmd))
		pFactory->delCmd(pCmd);
}

void CTimeTableCtrl::RemoveSelectedAppointments (BOOL bRedraw)
{
    TimeTableView* pView = GetCurrentView ();
    ASSERT_VALID (pView);

	POSITION pos = m_SelectedItemList.GetHeadPosition ();
    while (pos != NULL)
    {
        POSITION prePos = pos;
        Appointment* pApp = m_SelectedItemList.GetNext (pos);
        ASSERT_VALID (pApp);

        if (pApp == NULL)
        {
            continue;
        }

		if (m_bReadOnly)
		{
			CString ErrorMessage, layoutStr;
			layoutStr.Format(_T("(%s)"), pApp->GetDescription());
			ErrorMessage += Translate(_T("Unable to delete ")) + layoutStr + 
				Translate(_T(", Readonly for this Timetable!"));
			MessageBox(ErrorMessage,Translate(_T("Error:Timetable control")), MB_OK|MB_ICONERROR);
			break;
		}
        LRESULT lRes = 0L;

        BOOL bNotify = TRUE;

        if (m_bNotifyParent && bNotify && GetSafeHwnd () != NULL)
        {
            lRes = GetParent ()->SendMessage (TIMETABLE_REMOVE_APPOINTMENT, 0, (LPARAM) pApp);
        }

        if (lRes != LRESULT(-1L))
        {
            m_ItemManager.Remove (pApp);
            m_SelectedItemList.RemoveAt(prePos);
			GetOwner()->SendMessage(TIMETABLE_AFTERUNITDELETED,0,0);
        }

        SetModified();
    }


    if (bRedraw && GetSafeHwnd () != NULL)
    {
        GetCurrentView ()->AdjustLayout (FALSE);

        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

}

BOOL CTimeTableCtrl::RemoveAllAppointments (BOOL bRedraw)
{
    BOOL bIsEmpty = m_ItemManager.IsEmpty ();

    if (bIsEmpty)
    {
        return TRUE;
    }

    SetModified();

    m_SelectedItemList.RemoveAll ();

    if (!m_ItemManager.RemoveAll ())
    {
        return FALSE;
    }


    if (bRedraw && GetSafeHwnd () != NULL)
    {
        GetCurrentView()->AdjustLayout(FALSE);

        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

    if (m_bNotifyParent && !bIsEmpty && GetSafeHwnd () != NULL)
    {
        GetParent ()->SendMessage (TIMETABLE_REMOVE_ALL_APPOINTMENTS, 0, 0);
		GetOwner()->SendMessage(TIMETABLE_AFTERUNITDELETED,0,0);
    }

    return TRUE;
}


LRESULT CTimeTableCtrl::OnDragLeave(WPARAM wParam, LPARAM lParam)
{
    return 0;

}

LRESULT CTimeTableCtrl::OnDrop(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1L;

    HGLOBAL hGlobal = NULL;
    HDROP hDrop = NULL;

    CS3OleDropInfo* pInfo = (CS3OleDropInfo*)wParam;
    BOOL bInvalidData = FALSE;


    if (pInfo->m_pDataObject->IsDataAvailable(ScheduleCtrl::GetClipboardFormat()))
    {
        m_dtDrop = GetCurrentView ()->GetDateFromPoint(m_ptOpCurrent);

        try
        {
            CFile* pFile = pInfo->m_pDataObject->GetFileData (ScheduleCtrl::GetClipboardFormat ());
            if (pFile == NULL)
            {
                m_dtDrop = COleDateTime ();

                return FALSE;
            }


            SerializeFrom (*pFile, GetCurrentView ()->GetDateFromPoint (pInfo->m_point),ScheduleCtrl::GetClipboardFormat());

            delete pFile;

        }
        catch (COleException* pEx)
        {
            TRACE(_T("CTimeTableCtrl::OnDrop. OLE exception: %x\r\n"),
                pEx->m_sc);
            pEx->Delete ();
        }

        m_dtDrop = COleDateTime ();
    }
    else  if (pInfo->m_pDataObject->IsDataAvailable(CLayoutCategoryCtrl::s_LayoutFormat))
    {
        m_dtDrop = GetCurrentView ()->GetDateFromPoint(m_ptOpCurrent);

        try
        {
            CFile* pFile = pInfo->m_pDataObject->GetFileData (CLayoutCategoryCtrl::s_LayoutFormat);
            if (pFile == NULL)
            {
                m_dtDrop = COleDateTime ();

                return FALSE;
            }

            SerializeFrom (*pFile, GetCurrentView ()->GetDateFromPoint (pInfo->m_point), CLayoutCategoryCtrl::s_LayoutFormat);

            delete pFile;
        }
        catch (COleException* pEx)
        {
            TRACE(_T("CTimeTableCtrl::OnDrop. OLE exception: %x\r\n"),
                pEx->m_sc);
            pEx->Delete ();
        }

        m_dtDrop = COleDateTime ();
    }
    {
        bInvalidData = TRUE;
        lr = 0L;
    }

    if (!bInvalidData)
    {
        
    }

    return lr;

}
    
LRESULT CTimeTableCtrl::OnDragEnter(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1L;

    CS3OleDropInfo* pInfo = (CS3OleDropInfo*)wParam;

    if (pInfo->m_pDataObject->IsDataAvailable(ScheduleCtrl::GetClipboardFormat()))
    {
        return DROPEFFECT_COPY;
    }

    if (pInfo->m_pDataObject->IsDataAvailable(CLayoutCategoryCtrl::s_LayoutFormat))
    {
        return DROPEFFECT_COPY;
    }

    return DROPEFFECT_NONE;
}

LRESULT CTimeTableCtrl::OnDragOver(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1L;

    CS3OleDropInfo* pInfo = (CS3OleDropInfo*)wParam;

    if (pInfo->m_pDataObject->IsDataAvailable(ScheduleCtrl::GetClipboardFormat()))
    {
        return DROPEFFECT_COPY;
    }

    if (pInfo->m_pDataObject->IsDataAvailable(CLayoutCategoryCtrl::s_LayoutFormat))
    {
        return DROPEFFECT_COPY;
    }
    return DROPEFFECT_NONE;
}


BOOL CTimeTableCtrl::SerializeTo (CFile& file)
{
    AppointmentArray ar;

    POSITION pos = m_SelectedItemList.GetHeadPosition ();
    while (pos != NULL)
    {
        Appointment* pApp = m_SelectedItemList.GetNext (pos);
        ASSERT_VALID (pApp);

        ar.Add (pApp);
    }

    return ScheduleCtrl::SerializeTo (file, ar, GetCurrentView()->GetDateFromPoint(m_ptOpCurrent));
}

BOOL CTimeTableCtrl::SerializeFromAppointment (CFile& file, 
    AppointmentArray& ar, _TVIEW_STYLE style, const COleDateTime& dtTo)
{
    CArchive Archive (&file, CArchive::load);

    BOOL bResult = SerializeFromAppointment (Archive, ar, style, dtTo);

    Archive.Close();

    return bResult;
}

BOOL CTimeTableCtrl::SerializeFromAppointment (CArchive& Archive, 
    AppointmentArray& ar, _TVIEW_STYLE style, const COleDateTime& dtTo)
{
    try
    {
        int nCount;
        Archive >> nCount;

        if (nCount == 0)
        {
            return FALSE;
        }

        ar.RemoveAll ();
        int i = 0;

        COleDateTime  LeadingDate;

        Archive >> LeadingDate;

        for (i = 0; i < nCount; i++)
        {
            Appointment* pApp = NULL;
            Archive >> pApp;

            ASSERT_VALID (pApp);

            pApp->SetInterval(dtTo /*+ pApp->GetStart() - LeadingDate*/, 
                dtTo + /*pApp->GetStart() - LeadingDate + */pApp->GetDuration());

            if (pApp != NULL)
            {
                ar.Add (pApp);
            }
        }

    }
    catch (CArchiveException* pEx)
    {
        TRACE(_T("ScheduleCtrl::CTimeTableCtrl. Archive exception\r\n"));
        pEx->Delete ();

        if (ar.GetSize () > 0)
        {
            for (int i = 0; i < ar.GetSize (); i++)
            {
                if (ar[i] != NULL)
                {
                    delete ar[i];
                }
            }

            ar.RemoveAll ();
        }
    }

    return ar.GetSize () > 0;
}

BOOL CTimeTableCtrl::SerializeFromLayout (CFile& file, AppointmentArray& ar, 
        _TVIEW_STYLE style, const COleDateTime& dtTo)
{
    CArchive Archive (&file, CArchive::load);
    CString XMLString;
    Archive >> XMLString;


	TiXmlDocument *pDoc = new TiXmlDocument();

	std::string xml = StringUtility::ConvertUnicodeToUTF8(XMLString.GetBuffer(0));
	pDoc->Parse(reinterpret_cast<LPCSTR>(xml.c_str()), NULL, TIXML_ENCODING_UTF8);

    TiXmlElement* pLayoutElement = pDoc->RootElement(); 

	if(pLayoutElement == NULL) return FALSE;

    Layout layout;

    if(layout.LoadFromXML(pLayoutElement))
    {
        Appointment *pApp = new Appointment;

        pApp->SetDescription(layout.GetLayoutName().c_str());
        pApp->SetInterval(COleDateTime(), COleDateTime() + COleDateTimeSpan(layout.GetDuration()/(24*3600), 
            (layout.GetDuration() % (24*3600)) / 3600,(layout.GetDuration() % 3600) / 60, (layout.GetDuration()%60)));
        pApp->SetBackgroundColor(0xFF001234);

        pApp->SetDetailID(layout.GetLayoutID());
        pApp->SetDetailType(Appointment::APPOINTMENT_TYPE_LAYOUT);

        // UpdateIcon
        UINT Index = 0;
        if (!m_IconDetailIDMapper.Lookup(layout.GetLayoutID(), Index))
        {
            Layout::thumbnail_t thumbnail;

            GetControllerInterface->GetLayoutThumbnail(layout.GetLayoutID(), thumbnail);

            if (thumbnail.size() > 0)
            {
                HGLOBAL hGlobal  =  GlobalAlloc(GMEM_MOVEABLE, thumbnail.size() * sizeof(thumbnail[0]));
                void *  pData  =  GlobalLock(hGlobal);
                memcpy(pData, &thumbnail[0], thumbnail.size() * sizeof(thumbnail[0]));
                GlobalUnlock(hGlobal);

                IStream *  pStream  =  NULL;
                if  (CreateStreamOnHGlobal(hGlobal, TRUE,  & pStream)  ==  S_OK)
                {
                    CImage image;
                    if (SUCCEEDED(image.Load(pStream)))
                    {
                        CImage ThumbImage;
                        ThumbImage.Create(THUMBNAIL_W, THUMBNAIL_H, 32);

                        HDC hDstDC = NULL;
                        BOOL bResult;

                        // Obtain a DC to the destination image
                        hDstDC = ThumbImage.GetDC();

                        CDC  cDC;
                        cDC.Attach(hDstDC);
                        cDC.FloodFill(0, 0, RGB(123,456,789));
                        cDC.Detach();

                        // Perform the blit
                        bResult = image.BitBlt( hDstDC, max(0, (THUMBNAIL_W - image.GetWidth()))/2, 
                            max(0, (THUMBNAIL_H - image.GetHeight()))/2, 
                            image.GetWidth(), image.GetHeight(), 0, 0 );

                        // Release the destination DC
                        ThumbImage.ReleaseDC();

                        HBITMAP hBitmap=ThumbImage.Detach();
                        CBitmap bmp;
                        bmp.Attach(hBitmap); 

                        Index = m_AppointmentIconList.Add(&bmp, RGB(123,456,789));
                    } 
                    pStream -> Release();     
                } 
                GlobalFree(hGlobal);

                m_IconDetailIDMapper.SetAt(layout.GetLayoutID(), Index);
            }
        }
        pApp->SetImage(&m_AppointmentIconList, Index);

        COleDateTime LeadingDate = COleDateTime();

        pApp->SetInterval(dtTo /*+ pApp->GetStart() - LeadingDate*/, 
            dtTo + /*pApp->GetStart() - LeadingDate + */pApp->GetDuration());

        ar.Add (pApp);
    }

    Archive.Close();

    delete pDoc;

    return TRUE;
}

BOOL  CTimeTableCtrl::BeginAddAppointments()
{
	TimeTableCmdFactory *pFactory = (TimeTableCmdFactory *)m_CmdEngine.getFactory();
        _baseCmd *pbaseCmd = pFactory->newCmdEnd();
        ASSERT(pbaseCmd == NULL);//not allow un-processed command here!
	timeObjCreateCmd *pCmd = (timeObjCreateCmd*) pFactory->newCmdBegin(CMD_TIEMEOBJ_CREATE);
	ASSERT(pFactory);
	ASSERT(pCmd);

	return TRUE;
}
BOOL  CTimeTableCtrl::EndAddAppointments()
{
	TimeTableCmdFactory *pFactory = (TimeTableCmdFactory *)m_CmdEngine.getFactory();
	ASSERT(pFactory);
	_baseCmd *pCmd = pFactory->newCmdEnd();
	ASSERT(pCmd);
	if(!m_CmdEngine.execCmd(pCmd))
		pFactory->delCmd(pCmd);
	//pCmd->_Appointments.AddTail();
	return TRUE;
}

BOOL  CTimeTableCtrl:: OnAddAppointment(Appointment *pAdd)
{	
	TimeTableCmdFactory *pFactory = (TimeTableCmdFactory *)m_CmdEngine.getFactory();
	timeObjCreateCmd *pCmd = (timeObjCreateCmd*)pFactory->getBuffedCmd();
	ASSERT(pFactory);
	ASSERT(pCmd);
	pCmd->_Appointments.AddTail(pAdd);
	return TRUE;
}

BOOL CTimeTableCtrl::SerializeFrom (CFile& file, const COleDateTime& dtTo, CLIPFORMAT Format)
{
    AppointmentArray ar;

    BOOL bResult = FALSE;

    if(Format == ScheduleCtrl::GetClipboardFormat())
    {
        bResult = CTimeTableCtrl::SerializeFromAppointment (file, ar, m_Style, dtTo);
    }

    if(Format == CLayoutCategoryCtrl::s_LayoutFormat)
    {
        bResult = CTimeTableCtrl::SerializeFromLayout (file, ar, m_Style, dtTo);
    }

    if (!bResult)
    {
        return FALSE;
    } 

    TimeTableView* pView = GetCurrentView ();
    ASSERT_VALID(pView);

    BOOL bAllDay = FALSE;
    BOOL bNeedAdjust = m_Style == _SVIEW_STYLE_ALPHA;

    COleDateTimeSpan MinSpan = COleDateTimeSpan(1, 0, 0, 0);

    COleDateTime AppointmentsBegin(1983,3,17,23,59,59);

    ClearAppointmentSelection (FALSE);
	this->BeginAddAppointments();
    for (int i = 0; i < ar.GetSize (); i++)
    {
        Appointment* pApp = ar[i];
		AppointmentsBegin = min(AppointmentsBegin, pApp->GetStart());

        if (AddAppointment (pApp, FALSE, FALSE))
        {
            SelectItem(pApp, TRUE);
            MinSpan = min(MinSpan, pApp->GetDuration());
			//
			//for cmds system - zxy
			//
			OnAddAppointment(pApp);
        }
        else
        {
			if(!m_bPreparing && m_bReadOnly)
				return FALSE;
			if(pApp->GetDuration().GetHours() >= 24)
			{
				CString ErrorMessage, layoutStr;
				layoutStr.Format(_T("(%s)"), pApp->GetDescription());
				ErrorMessage += Translate(_T("Could not insert Layout")) + layoutStr
                + Translate(_T(", Timetable could not exceed 24 hours!"));
				MessageBox(ErrorMessage,Translate(_T("Warning:Timetable control")), MB_OK|MB_ICONEXCLAMATION);
			}
        }
    }
	 
	//
	//for cmds system -zxy
	//
	this->EndAddAppointments();
#if 0  //not auto set time delta.
    BOOL bTimeDeltaChanged = FALSE;
    _TVIEW_TIME_DELTA TimeDelta = _TVIEW_TIME_DELTA_60;
    _TVIEW_TIME_DELTA OldTimeDelta = GetTimeDelta();
    if (MinSpan < COleDateTimeSpan(0, 0, 5, 0))
    {
        TimeDelta = _TVIEW_TIME_DELTA_1;
        if (OldTimeDelta != TimeDelta)
        {
            SetTimeDelta(TimeDelta);
            bTimeDeltaChanged = TRUE;
        }
    }
    else if (MinSpan < COleDateTimeSpan(0, 0, 10, 0))
    {
        TimeDelta = _TVIEW_TIME_DELTA_10;
        if (OldTimeDelta != TimeDelta)
        {
            SetTimeDelta(TimeDelta);
            bTimeDeltaChanged = TRUE;
        }
    }
    else if (MinSpan < COleDateTimeSpan(0, 0, 15, 0))
    {
        TimeDelta = _TVIEW_TIME_DELTA_15;
        if (OldTimeDelta != TimeDelta)
        {
            SetTimeDelta(TimeDelta);
            bTimeDeltaChanged = TRUE;
        }
    }


    if (bTimeDeltaChanged)
    {
        GetCurrentView()->AdjustScrollSizes();
        if (m_bNotifyParent && GetSafeHwnd () != NULL)
        {
            GetParent ()->SendMessage (SCHEDULE_TIME_DELTA_CHANGED, (WPARAM)TimeDelta, 0);
        }
    }
#endif

    GetCurrentView()->ScrollToDate(AppointmentsBegin);

    if (ar.GetSize () > 0)
    {
        GetCurrentView()->AdjustLayout(FALSE);
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

    return bResult;
}

 BOOL CTimeTableCtrl::PreTranslateMessage(MSG* pMsg)
 {
	 m_CmdEngine.TransMessage(pMsg);
	 return __super::PreTranslateMessage(pMsg);
 }
