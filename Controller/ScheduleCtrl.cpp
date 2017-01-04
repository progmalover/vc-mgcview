#include "StdAfx.h"
#include "resource.h"
#include "ScheduleCtrl.h"
#include "ScheduleViewStyleAlpha.h"
#include "Controller.h"
#include "Localization/Localization.h"
#include "LayoutCategoryCtrl.h"
#include "TimeTableCategoryCtrl.h"

#include ".\BorrowSkin\BORROW_DateTimeCtrl.h"

#include "DatetimeCheck.h"

#define ID_PREV_BTN  33333
#define ID_NEXT_BTN  33334
#define ID_MOVE_PREV_BTN 33335
#define ID_MOVE_NEXT_BTN 33336
#define BTN_WIDTH 50
#define CANLARDA_WIDTH 150
UINT _UPDATETOOLTIPS = ::RegisterWindowMessage (_T("_UPDATETOOLTIPS"));

// wParam - not used, lParam - points to the appointment
UINT SCHEDULE_ADD_APPOINTMENT			= ::RegisterWindowMessage (_T("SCHEDULE_ADD_APPOINTMENT"));
// wParam - not used, lParam - points to the appointment
// Return value:
// -1L - cancel update
//  0L - update
UINT SCHEDULE_BEFORE_UPDATE_APPOINTMENT	= ::RegisterWindowMessage (_T("SCHEDULE_BEFORE_UPDATE_APPOINTMENT"));
// wParam - not used, lParam - points to the appointment
UINT SCHEDULE_UPDATE_APPOINTMENT		= ::RegisterWindowMessage (_T("SCHEDULE_UPDATE_APPOINTMENT"));
// wParam - not used, lParam - points to the appointment
// Return value:
// -1L - cancel delete
//	0L - delete single appointment or only occurrence of the series
//	1L - delete full series
UINT SCHEDULE_REMOVE_APPOINTMENT		= ::RegisterWindowMessage (_T("SCHEDULE_REMOVE_APPOINTMENT"));
// wParam, lParam - not used
UINT SCHEDULE_REMOVE_ALL_APPOINTMENTS	= ::RegisterWindowMessage (_T("SCHEDULE_REMOVE_ALL_APPOINTMENTS"));

// wParam - TRUE - select, otherwise - unselect, lParam - points to the appointment
// Return value:
// -1L - cancel operation
//  0L - allow operation
UINT SCHEDULE_BEFORE_SELECT_APPOINTMENT = ::RegisterWindowMessage (_T("SCHEDULE_BEFORE_SELECT_APPOINTMENT"));
// wParam - TRUE - selected, otherwise - not selected, lParam - points to the appointment
UINT SCHEDULE_SELECT_APPOINTMENT        = ::RegisterWindowMessage (_T("SCHEDULE_SELECT_APPOINTMENT"));

// wParam - not used, lParam - points to the appointment
UINT SCHEDULE_SET_DEFAULT_APPOINTMENT = ::RegisterWindowMessage (_T("SCHEDULE_SET_DEFAULT_APPOINTMENT"));

// wParam - old style, lParam - new style
UINT SCHEDULE_STYLE_CHANGED				= ::RegisterWindowMessage (_T("SCHEDULE_STYLE_CHANGED"));
// wParam, lParam - not used
UINT SCHEDULE_DATE_CHANGED				= ::RegisterWindowMessage (_T("SCHEDULE_DATE_CHANGED"));

// wParam - new time delta, lParam - not used
UINT SCHEDULE_TIME_DELTA_CHANGED				= ::RegisterWindowMessage (_T("SCHEDULE_TIME_DELTA_CHANGED"));

// wParam, lParam - see WM_LBUTTONDBLCLK
UINT SCHEDULE_LBUTTONDBLCLK 			= ::RegisterWindowMessage (_T("SCHEDULE_LBUTTONDBLCLK"));
// wParam, lParam - see WM_KEYDOWN
UINT SCHEDULE_KEYDOWN					= ::RegisterWindowMessage (_T("SCHEDULE_KEYDOWN"));

// wParam, lParam - not used
UINT SCHEDULE_DROP_APPOINTMENTS         = ::RegisterWindowMessage (_T("SCHEDULE_DROP_APPOINTMENTS"));
// wParam - drop effect code, lParam - not used
UINT SCHEDULE_DRAG_APPOINTMENTS         = ::RegisterWindowMessage (_T("SCHEDULE_DRAG_APPOINTMENTS"));

// wParam - operation code, lParam - not used
// Return value:
// -1L - cancel operation
//  0L - allow operation
UINT SCHEDULE_BEGIN_CHANGE_OPERATION    = ::RegisterWindowMessage (_T("SCHEDULE_BEGIN_CHANGE_OPERATION"));
// wParam - operation code, lParam - result of operation
UINT SCHEDULE_END_CHANGE_OPERATION      = ::RegisterWindowMessage (_T("SCHEDULE_END_CHANGE_OPERATION"));

CLIPFORMAT ScheduleCtrl::s_ClpFormat = ::RegisterClipboardFormat (_T("S3SignageManagerScheduleClpFmt"));

template <class style>
void CopyList (const style& lstSrc, style& lstDst)
{
    lstDst.RemoveAll ();

    POSITION pos = lstSrc.GetHeadPosition ();

    while (pos != NULL)
    {
        lstDst.AddTail (lstSrc.GetNext (pos));
    }
}

int ROUND(double val)
{
    const double c_Median = 0.5;
    return int((val - int(val - c_Median)) >= c_Median ?
        val + c_Median : val - c_Median);
}


IMPLEMENT_DYNAMIC(ScheduleCtrl, CWnd)

BEGIN_MESSAGE_MAP(ScheduleCtrl, CWnd)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_DESTROY()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_MOUSEWHEEL()
    ON_WM_VSCROLL()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_SETCURSOR()
    ON_WM_KEYDOWN()
    ON_WM_TIMER()
    ON_WM_TIMECHANGE()
    ON_WM_CANCELMODE()
    ON_WM_LBUTTONDBLCLK()
    ON_MESSAGE(WM_SETFONT, OnSetFont)
    ON_MESSAGE(WM_GETFONT, OnGetFont)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnNeedToolTipText)
    ON_REGISTERED_MESSAGE(_UPDATETOOLTIPS, OnUpdateToolTips)

    ON_REGISTERED_MESSAGE(WM_DND_DRAGENTER, OnDragEnter)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGOVER, OnDragOver)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGLEAVE, OnDragLeave)
    ON_REGISTERED_MESSAGE(WM_DND_DROP, OnDrop)

	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TIMETABLE_STARTTIME, &ScheduleCtrl::OnDtnDatetimechangeDatetimepicker1)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TIMETABLE_ENDTIME, &ScheduleCtrl::OnDtnDatetimechangeDatetimepicker2)

	ON_BN_CLICKED(IDC_TIMETABLE_STARTTIME,&ScheduleCtrl::OnDatetimechangeDatetimepicker1)
	ON_BN_CLICKED(IDC_TIMETABLE_ENDTIME,&ScheduleCtrl::OnDatetimechangeDatetimepicker2)
	ON_COMMAND(ID_EDIT_UNDO,OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO,OnEditRedo)
	ON_BN_CLICKED(ID_PREV_BTN,OnSchedulePrevBtnClicked)
	ON_BN_CLICKED(ID_NEXT_BTN,OnScheduleNextBtnClicked)

	ON_BN_CLICKED(ID_MOVE_PREV_BTN,OnScheduleMovePrevBtnClicked)
	ON_BN_CLICKED(ID_MOVE_NEXT_BTN,OnScheduleMoveNextBtnClicked)
END_MESSAGE_MAP()

ScheduleCtrl::ScheduleCtrl(void)
    : m_ScheduleID        (-1)
    , m_clrBackground	  (CLR_DEFAULT)
    , m_nFirstWorkingHour ( 9.0)
    , m_nLastWorkingHour  (18.0)
    , m_nWeekStart        (1)
    , m_Style			  (_SVIEW_STYLE_ALPHA)
    , m_Operation (_SVIEW_OPERATION_NONE)
    , m_OpMode (OPERATION_NONE)
    , m_AppsManager ()
    , m_DefaultAppointment ()
    , m_QueryAppsArray ()
    , m_SelectedAppsList ()
    , m_TimeDelta		  (_SVIEW_TIME_DELTA_1)
    , m_pCurrentView (NULL)
    , m_OpLayer ()
    , m_HitTest (_SVIEW_HITTEST_NOWHERE)
    , m_bCaptured (FALSE)
    , m_pWndLastCapture (NULL)
    , m_ptOpStart	      (0, 0)
    , m_ptOpCurrent        (0, 0)
    , m_CurSelectedDate    ()
    , m_pCalendar           (NULL)
    , m_bRelocate		  (FALSE)
    , m_DropTarget ()
    , m_DropSource ()
    , m_bDragDrop   (FALSE)
    , m_dragEffect		  (DROPEFFECT_NONE)
    , m_dtDrop()
    , m_bRedrawAfterDrop  (FALSE)
    , m_bNotifyParent	  (TRUE)
    , m_RegularFont ()
    , m_RegularBoldFont()
    , m_SmallFont()
    , m_SmallBoldFont()
    , m_ScheduleItemIconList()
    , m_IconDetailIDMapper ()
{

    for (int i = 0; i < sizeof (m_pViews) / sizeof (ScheduleView*); i++)
    {
        m_pViews [i] = NULL;
    }

    m_ScheduleItemIconList.Create(THUMBNAIL_W, THUMBNAIL_H, ILC_COLOR32|ILC_MASK, 1, 1);

    m_ScheduleItemIconList.Add(AfxGetApp()->LoadIconW(IDI_EMPTYCONTENT));

	
}

ScheduleCtrl::~ScheduleCtrl(void)
{
    for (int i = 0; i < sizeof (m_pViews) / sizeof (ScheduleView*); i++)
    {
        if (m_pViews [i])
        {
            delete m_pViews [i];
        }
    }

	delete pStartTimeCtrl;
	delete pEndTimeCtrl;
	delete pStatic;
}

void ScheduleCtrl::DoDataExchange(CDataExchange* pDX)
{
	CWnd::DoDataExchange(pDX);
	//DDX_DateTimeCtrl(pDX, IDC_TIMETABLE_STARTTIME, m_datatimectrl1);
	//DDX_DateTimeCtrl(pDX, IDC_TIMETABLE_ENDTIME, m_datatimectrl2);
}


int ScheduleCtrl::CreateButton()
{
	pStatic =  new CStatic();
	RECT destRect = { 10, 10, 20, 20 };
	pStatic->Create(_T("--"), WS_CHILD|WS_VISIBLE|SS_CENTER , destRect, this, IDC_STATIC);
	pStatic->ShowWindow(TRUE);

	pStartTimeCtrl = new CBORROW_DateTimeCtrl();
	//CMonthCalCtrl *pButton = new CMonthCalCtrl();
	ASSERT_VALID(pStartTimeCtrl); 
	pStartTimeCtrl->Create( _T("DateTimeCtrl"),
		WS_CHILD | WS_VISIBLE , 
		CRect(10, 10,20, 20), 
		this, IDC_TIMETABLE_STARTTIME); 
	pStartTimeCtrl->SetAutoResize(TRUE);
	pStartTimeCtrl->EnableCheckBox(FALSE);
	pStartTimeCtrl->ShowWindow(TRUE);

	/*
	pStatic2 =  new CStatic();
	RECT destRect2 = { 200, 10, 250, 30 };
	pStatic2->Create(_T("Start Time:"), WS_CHILD|WS_VISIBLE, destRect2, this, IDC_STATIC);
	pStatic2->ShowWindow(TRUE);
	*/

	pEndTimeCtrl = new CBORROW_DateTimeCtrl();
	//CMonthCalCtrl *pButton = new CMonthCalCtrl();
	ASSERT_VALID(pEndTimeCtrl);
	pEndTimeCtrl->Create(_T("DateTimeCtrl"), 
		WS_CHILD | WS_VISIBLE , 
		CRect(10, 10,20, 20), 
		this, IDC_TIMETABLE_ENDTIME); 
	pEndTimeCtrl->SetAutoResize(TRUE);
	pEndTimeCtrl->EnableCheckBox(FALSE);
	pEndTimeCtrl->ShowWindow(TRUE);

	m_datatimectrl1 = COleDateTime::GetCurrentTime ();
	m_datatimectrl2 = COleDateTime::GetCurrentTime ()+ COleDateTimeSpan(7,0,0,0);
	 
	pStartTimeCtrl->SetDate(m_datatimectrl1.m_dt);
	pEndTimeCtrl->SetDate(m_datatimectrl2.m_dt);
	
	m_PrevBtn.Create(_T("<<"),WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,ID_PREV_BTN);
	m_NextBtn.Create(_T(">>"),WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,ID_NEXT_BTN);

	m_MovePrevBtn.Create(_T("<"),WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,ID_MOVE_PREV_BTN);
	m_MoveNextBtn.Create(_T(">"),WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,ID_MOVE_NEXT_BTN);
	
	SetDateInterval(m_datatimectrl1,m_datatimectrl2);

	return true;
}


void ScheduleCtrl::MovetoPrevInterval(int nVal)
{
	COleDateTime dtStart;
	COleDateTime dtEnd;
	this->GetDateInterval(dtStart,dtEnd);
	COleDateTimeSpan dateSpan;
	dateSpan.SetDateTimeSpan(nVal,0,0,0);
	dtStart -= dateSpan;
	dtEnd -= dateSpan;

	pStartTimeCtrl->SetDate(dtStart);
	pEndTimeCtrl->SetDate(dtEnd);
	this->SetDateInterval(dtStart,dtEnd);
}

void ScheduleCtrl::MovetoNextInterval(int nVal)
{
	COleDateTime dtStart;
	COleDateTime dtEnd;
	this->GetDateInterval(dtStart,dtEnd);

	COleDateTimeSpan  dateSpan;
	dateSpan.SetDateTimeSpan(nVal,0,0,0);
	dtStart += dateSpan;
	dtEnd += dateSpan;

	pStartTimeCtrl->SetDate(dtStart);
	pEndTimeCtrl->SetDate(dtEnd);

	this->SetDateInterval(dtStart,dtEnd);
}

void  ScheduleCtrl::OnScheduleMovePrevBtnClicked()
{
	MovetoPrevInterval(1); 
}

void  ScheduleCtrl::OnScheduleMoveNextBtnClicked()
{
	MovetoNextInterval(1);
}

void ScheduleCtrl::OnSchedulePrevBtnClicked()
{

	MovetoPrevInterval(this->GetDisplayRange());
}

int	 ScheduleCtrl::GetDisplayRange()
{
	COleDateTime dtStart;
	COleDateTime dtEnd;
	this->GetDateInterval(dtStart,dtEnd);
	COleDateTimeSpan dateSpan = dtEnd - dtStart; 
	return (dateSpan.GetDays());
}

void ScheduleCtrl::OnScheduleNextBtnClicked()
{

	MovetoNextInterval(this->GetDisplayRange());
}

void ScheduleCtrl::OnEditUndo()
{
	this->m_CmdEngine.undo();

}

void ScheduleCtrl::OnEditRedo()
{
	this->m_CmdEngine.redo();
}

BOOL ScheduleCtrl::PreCreateWindow (CREATESTRUCT& cs)
{
    CRuntimeClass* viewClass[] =
    {
        RUNTIME_CLASS(ScheduleViewStyleAlpha),
    };

    for (int i = 0; i < sizeof (m_pViews) / sizeof (ScheduleView*); i++)
    {
        m_pViews [i] = new ScheduleViewStyleAlpha();

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
    m_pCurrentView->SetDate (COleDateTime::GetCurrentTime ());

    InitFonts();

    SetDateInterval(COleDateTime::GetCurrentTime (), COleDateTime::GetCurrentTime ()+ COleDateTimeSpan(7,0,0,0));

    return CWnd::PreCreateWindow (cs);
}

void ScheduleCtrl::SetDefaultAppointment(const Appointment* pApp, BOOL bRedraw)
{
    if (pApp)
    {
        m_DefaultAppointment = *pApp;

        GetCurrentView()->SetDefaultAppointment(&m_DefaultAppointment);
    }
    else
    {
        m_DefaultAppointment.SetDetailID(-1);

        GetCurrentView()->SetDefaultAppointment(NULL);
    }

    if (bRedraw)
    {
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

const Appointment* ScheduleCtrl::GetDefaultAppointment()
{
    if (m_DefaultAppointment.GetDetailID() == -1)
    {
        return NULL;
    }
    return &m_DefaultAppointment;
}

void ScheduleCtrl::InitFonts()
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

void ScheduleCtrl::GetListImage(CImageList &ImageList,CBitmap &Bitmap ,int nImageIdx)
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

BOOL ScheduleCtrl::AddAppointment (Appointment* pApp, BOOL bQuery, BOOL bRedraw, BOOL bNotify)
{
    ASSERT_VALID (pApp);

    if (pApp->GetDuration().m_span < (COleDateTimeSpan(0,0,0,9).m_span + 0.5/(24*60*60)))
    {
        CString ErrorMessage, layoutStr;
		//layoutStr.Format(_T("(%s)"), pApp->GetDescription());
		 ErrorMessage = Translate(_T("Illegal layout, the duration should big than 10 seconds!"));//Translate(_T("Bad Drop, Layout")) + layoutStr + Translate(_T("'s duration should not less than 10 seconds!"));
        MessageBox(ErrorMessage,Translate(_T("Warning")), MB_OK);

        return FALSE;
    }

    if (!m_AppsManager.Add(pApp))
    {
        return FALSE;
    }

    LRESULT lRes = 0L;

    if (m_bNotifyParent && bNotify && GetSafeHwnd () != NULL)
    {
        lRes = GetParent ()->SendMessage (SCHEDULE_ADD_APPOINTMENT, 0, (LPARAM) pApp);
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
            Index = m_ScheduleItemIconList.Add(&bitmap, RGB(255,255,255));

            m_IconDetailIDMapper.SetAt(pApp->GetDetailID(), Index);
        }

        pApp->SetImage(&m_ScheduleItemIconList, Index);
    }
    else
    {
        pApp->SetImage(&m_ScheduleItemIconList, 0);
    }

    if (bQuery)
    {
        QueryAppointments(FALSE);
    }

    if (bRedraw && GetSafeHwnd () != NULL)
    {
        GetCurrentView ()->AdjustLayout (FALSE);

        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

    return TRUE;
}

BOOL ScheduleCtrl::UpdateAppointment (Appointment* pApp, const COleDateTime& dtNewStart,
    const COleDateTime& dtNewEnd,  BOOL bQuery, BOOL bRedraw)
{
    ASSERT_VALID (pApp);

    LRESULT lRes = 0L;

    if (IsAppointmentOverlapWithOthers(pApp, dtNewStart - pApp->GetStart(), dtNewEnd - pApp->GetFinish()))
    {
		MessageBox(Translate(_T("Input time can't be accepted,because the next time range is occupied!")),Translate(_T("Warning:Schedule control")), MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (m_bNotifyParent && GetSafeHwnd () != NULL)
    {
        lRes = GetParent ()->SendMessage (SCHEDULE_BEFORE_UPDATE_APPOINTMENT, 0, (LPARAM) pApp);
    }

    pApp->SetInterval(dtNewStart, dtNewEnd);

    // UpdateIcon
    if (pApp->GetImageList())
    {
        UINT Index;
        if (!m_IconDetailIDMapper.Lookup(pApp->GetDetailID(), Index))
        {
            CBitmap bitmap;
            GetListImage(*pApp->GetImageList(), bitmap, pApp->GetImageIndex());
            Index = m_ScheduleItemIconList.Add(&bitmap, RGB(255,255,255));

            m_IconDetailIDMapper.SetAt(pApp->GetDetailID(), Index);
        }

        pApp->SetImage(&m_ScheduleItemIconList, Index);
    }

    if (m_bNotifyParent && GetSafeHwnd () != NULL)
    {
        lRes = GetParent ()->SendMessage (SCHEDULE_UPDATE_APPOINTMENT, 0, (LPARAM) pApp);
    }

    if (bQuery)
    {
        QueryAppointments(FALSE);
    }

    if (bRedraw && GetSafeHwnd () != NULL)
    {
        GetCurrentView ()->AdjustLayout (FALSE);

        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

    if (m_bNotifyParent && GetSafeHwnd () != NULL)
    {
        lRes = GetParent ()->SendMessage (SCHEDULE_UPDATE_APPOINTMENT, 0, (LPARAM) pApp);
    }

    if (lRes == LRESULT(-1L))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL ScheduleCtrl::RemoveAppointment (Appointment* pApp, BOOL bQuery, BOOL bRedraw)
{
    ASSERT_VALID (pApp);

    if (pApp == NULL)
    {
        return FALSE;
    }

    LRESULT lRes = 0L;

    if (m_bNotifyParent && GetSafeHwnd () != NULL)
    {
        lRes = GetParent ()->SendMessage (SCHEDULE_REMOVE_APPOINTMENT, 0, (LPARAM) pApp);
    }

    if (lRes == LRESULT(-1L))
    {
        return FALSE;
    }

    // remove appointment from query
    for (int i = 0; i < m_QueryAppsArray.GetSize (); i++)
    {
        if (m_QueryAppsArray[i] == pApp)
        {
            m_QueryAppsArray[i] = NULL;
            break;
        }
    }

    // remove appointment from selection
    POSITION pos = m_SelectedAppsList.Find (pApp);
    if (pos != NULL)
    {
        m_SelectedAppsList.RemoveAt (pos);
    }

    m_AppsManager.Remove(pApp);

    if (bQuery)
    {
        QueryAppointments ();
    }

    if (bRedraw && GetSafeHwnd () != NULL)
    {
        GetCurrentView ()->AdjustLayout (FALSE);

        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

    return TRUE;
}

//
// interface for cmds system -zxy
//
void  ScheduleCtrl::RemoveSelections(void)
{
	ScheduleCmdFactory *pFactory = this->m_CmdEngine.getFactory();
	_baseCmd *pCmd = pFactory->newCmd(CMD_SCHEDULE_DELETE);
	if(!m_CmdEngine.execCmd(pCmd))
		pFactory->delCmd(pCmd);
}

void ScheduleCtrl::RemoveSelectedAppointments (BOOL bQuery, BOOL bRedraw)
{
    ScheduleView* pView = GetCurrentView ();
    ASSERT_VALID (pView);

    POSITION pos = m_SelectedAppsList.GetHeadPosition ();
    while (pos != NULL)
    {
        POSITION prePos = pos;
        Appointment* pApp = m_SelectedAppsList.GetNext (pos);
        ASSERT_VALID (pApp);

        if (pApp == NULL)
        {
            continue;
        }

        LRESULT lRes = 0L;

        BOOL bNotify = TRUE;

        if (m_bNotifyParent && bNotify && GetSafeHwnd () != NULL)
        {
            lRes = GetParent ()->SendMessage (SCHEDULE_REMOVE_APPOINTMENT, 0, (LPARAM) pApp);
        }

        if (lRes != LRESULT(-1L))
        {
            m_AppsManager.Remove (pApp);

            for (int i = 0; i < m_QueryAppsArray.GetSize (); i++)
            {
                if (m_QueryAppsArray[i] == pApp)
                {
                    m_QueryAppsArray[i] = NULL;
                    break;
                }
            }

            m_SelectedAppsList.RemoveAt(prePos);
        }
    }

    if (bQuery)
    {
        QueryAppointments ();
    }

    if (bRedraw && GetSafeHwnd () != NULL)
    {
        GetCurrentView ()->AdjustLayout (FALSE);

        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

BOOL ScheduleCtrl::RemoveAllAppointments (BOOL bRedraw)
{
    BOOL bIsEmpty = m_AppsManager.IsEmpty ();

    m_QueryAppsArray.RemoveAll();

    m_SelectedAppsList.RemoveAll ();

    if (!m_AppsManager.RemoveAll ())
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
        GetParent ()->SendMessage (SCHEDULE_REMOVE_ALL_APPOINTMENTS, 0, 0);
    }

    return TRUE;
}

int ScheduleCtrl::GetQueryedAppointments (AppointmentArray& ar) const
{
    ar.RemoveAll ();
    ar.Copy (m_QueryAppsArray);

    return (int) ar.GetSize ();
}

int ScheduleCtrl::GetSelectedAppointmentsCount () const
{
    return (int) m_SelectedAppsList.GetCount ();
}

POSITION ScheduleCtrl::GetFirstSelectedAppointment () const
{
    return m_SelectedAppsList.GetHeadPosition ();
}

POSITION ScheduleCtrl::GetLastSelectedAppointment () const
{
    return m_SelectedAppsList.GetTailPosition ();
}

/*const*/ Appointment* ScheduleCtrl::GetNextSelectedAppointment (POSITION& pos) const
{
    return m_SelectedAppsList.GetNext (pos);
}

/*const*/ Appointment* ScheduleCtrl::GetPrevSelectedAppointment (POSITION& pos) const
{
    return m_SelectedAppsList.GetPrev (pos);
}

_SVIEW_HITTEST ScheduleCtrl::HitTest (const CPoint& point) const
{
	 
	//CPoint tpPoint = point;
	//tpPoint.Offset(0,-CALENDAR_CTRL_HEIGHT);
    return GetCurrentView()->HitTest (point);
}

_SVIEW_HITTEST ScheduleCtrl::HitTestArea (const CPoint& point) const
{
    return GetCurrentView()->HitTestArea (point);
}
_SVIEW_HITTEST ScheduleCtrl::HitTestAppointment (const CPoint& point) const
{
    return GetCurrentView()->HitTestAppointment (point);
}

void ScheduleCtrl::QueryAppointments (BOOL bRedraw)
{
    m_AppsManager.Query(m_QueryAppsArray, GetCurrentView()->GetDateBegin(), GetCurrentView()->GetDateEnd());

    if (GetSafeHwnd () == NULL)
    {
        return;
    }

    if(bRedraw)
    {
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

void ScheduleCtrl::QueryAppointments (const COleDateTime& date1, const COleDateTime& date2, BOOL bRedraw)
{
    m_AppsManager.Query(m_QueryAppsArray, date1, date2);

    if (GetSafeHwnd () == NULL)
    {
        return;
    }

    if(bRedraw)
    {
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

void ScheduleCtrl::QueryAll (BOOL bRedraw)
{
    m_AppsManager.QueryAll(m_QueryAppsArray);

    if (GetSafeHwnd () == NULL)
    {
        return;
    }

    if(bRedraw)
    {
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

void ScheduleCtrl::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    ProcessPaint(&dc);
}
 
void ScheduleCtrl::ProcessHeaderCtrls( CDC *pDC ,CRect ctrlsRect)
{
	CDrawingManager dm(*pDC);
	dm.FillGradient(ctrlsRect,afxGlobalData.clrBarShadow,afxGlobalData.clrBarLight);
	 

	CFont *pFont = pStatic->GetFont();
	CRect ctrlRect(ctrlsRect.Width()/2,ctrlsRect.Height()/2,ctrlsRect.Width()/2,ctrlsRect.Height()/2) ;

	//ctrlRect.OffsetRect(GetCurrentView()->GetHeaderCaptionHeight(),0);
	pDC->SaveDC();
	pDC->SelectObject(pFont);
	TEXTMETRIC metric;
    pDC->GetOutputTextMetrics( &metric);
	ctrlRect.InflateRect(metric.tmMaxCharWidth,10);

	pStatic->MoveWindow(ctrlRect);
	
	CRect ctrlRect1 = ctrlRect;
	ctrlRect1.InflateRect(CANLARDA_WIDTH,0);
	ctrlRect1.right = ctrlRect.left;
	pStartTimeCtrl->MoveWindow(ctrlRect1);
	CRect ctrlRect2 = ctrlRect;
	ctrlRect2.InflateRect(CANLARDA_WIDTH,0);
	ctrlRect2.left = ctrlRect.right;
	pEndTimeCtrl->MoveWindow(ctrlRect2);

	pDC->RestoreDC(-1);

	//ribbon button.


	CRect btnRect1,btnRect2;
	btnRect1 = ctrlRect;
	btnRect1.InflateRect(CANLARDA_WIDTH + BTN_WIDTH,0);
	btnRect1.right = ctrlRect1.left - 2;
	m_MovePrevBtn.MoveWindow(btnRect1);
	btnRect2 = ctrlRect;
	btnRect2.InflateRect(CANLARDA_WIDTH + BTN_WIDTH,0);
	btnRect2.left = ctrlRect2.right + 2;
	m_MoveNextBtn.MoveWindow(btnRect2);

	CRect btnRect3,btnRect4;

	btnRect3 = ctrlRect;
	btnRect3.InflateRect(CANLARDA_WIDTH + BTN_WIDTH *2,0);
	btnRect3.right = btnRect1.left - 2;
	m_PrevBtn.MoveWindow(btnRect3);
	btnRect4 = ctrlRect;
	btnRect4.InflateRect(CANLARDA_WIDTH + BTN_WIDTH*2,0);
	btnRect4.left = btnRect2.right + 2;
	m_NextBtn.MoveWindow(btnRect4);

}
void ScheduleCtrl::ProcessPaint(CDC* pDCPaint)
{
    ASSERT_VALID(pDCPaint);

    ScheduleView* pView = GetCurrentView ();

    CRect rectClient;
    GetClientRect (rectClient);
	rectClient.bottom -= CALENDAR_CTRL_HEIGHT;
	//rectClient.top += CALENDAR_CTRL_HEIGHT;
	
	//pDCPaint->FillSolidRect(rectClient.left,rectClient.top,rectClient.Width(),CALENDAR_CTRL_HEIGHT+rectClient.top ,afxGlobalData.GetColor(COLOR_BTNFACE));
	int height = CALENDAR_CTRL_HEIGHT;

	CRect ctrlsRect = rectClient;
	ctrlsRect.bottom = CALENDAR_CTRL_HEIGHT;
	ProcessHeaderCtrls(pDCPaint ,ctrlsRect);

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

    //HFONT hfontOld = pView->SetCurrFont (pDC);

    pView->OnPaint (pDC, rectClient);

    if (bMemDC)
    {
        pDCPaint->BitBlt (rectClient.left, rectClient.top+CALENDAR_CTRL_HEIGHT, rectClient.Width (), rectClient.Height (),
            &memDC, 0, 0, SRCCOPY);
    }

    //::SelectObject (pDC->GetSafeHdc (), hfontOld);

}

BOOL ScheduleCtrl::OnEraseBkgnd(CDC* pDC)
{

    return TRUE;
}

int ScheduleCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }
	this->m_CmdEngine.bindTarget(this);

    m_DropTarget.Register (this);


	m_TodayBtnToolTip.Create(AfxGetMainWnd());

	CMFCToolTipInfo tipInf;
	tipInf.m_bDrawIcon = FALSE;
	tipInf.m_bVislManagerTheme = TRUE;
	tipInf.m_bDrawDescription =TRUE;

	m_TodayBtnToolTip.SetParams(&tipInf);
	//m_TodayBtnToolTip.SetDescription(_T("Click it and set to currente date"));
	m_TodayBtnToolTip.AddTool(this,Translate(_T("Today Button")));

	return 1;
}

void ScheduleCtrl::OnDestroy()
{

}

void ScheduleCtrl::OnSize(UINT nStyle, int cx, int cy)
{
    GetCurrentView()->AdjustLayout(FALSE);

    RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

BOOL ScheduleCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	//by RaymondLiu
	if(pt.y < CALENDAR_CTRL_HEIGHT)
	{
		return true;
	}
	pt.y -= CALENDAR_CTRL_HEIGHT;
	
    BOOL bRes = GetCurrentView ()->OnMouseWheel (nFlags, zDelta, pt);

    return bRes;
}

void ScheduleCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    //COleDateTime dtOld (GetCurrentView ()->GetDate ());
    GetCurrentView ()->OnVScroll (nSBCode, nPos, pScrollBar);
}


void ScheduleCtrl::StartCapture ()
{
    if (!m_bCaptured)
    {
        CPoint pt;
        ::GetCursorPos (&pt);
        ScreenToClient (&pt);
		if(pt.y >= CALENDAR_CTRL_HEIGHT)
		{
			pt.y -= CALENDAR_CTRL_HEIGHT;
		}
		
		m_ptOpStart = pt;
        m_ptOpCurrent = m_ptOpStart;


        m_pWndLastCapture = SetCapture ();

        m_bCaptured = TRUE;

        m_HitTest = GetCurrentView ()->HitTest (pt);

    }
}

void ScheduleCtrl::StopCapture ()
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

        m_HitTest = _SVIEW_HITTEST_NOWHERE;
    }
}

void  ScheduleCtrl::BeginDragTest()
{
	// be in "drag" status.
	if(OPERATION_FIRST == m_OpMode  &&
		m_HitTest == _SVIEW_HITTEST_APPOINTMENT_BOTTOM )
	{
		ScheduleCmdFactory *pFactory = m_CmdEngine.getFactory();
		scheduleDurationChangeCmd *pCmd = (scheduleDurationChangeCmd *)pFactory->newCmdBegin(CMD_SCHEDULE_DURATION_CHANGE);
		if(NULL == pCmd)
		{
			_baseCmd *pCmd = pFactory->newCmdEnd();
			if(!m_CmdEngine.execCmd(pCmd))
			{
				pFactory->delCmd(pCmd);
			}
			return;
		}
		pCmd->SaveBeforeDrag();
	}

	if(OPERATION_FIRST == m_OpMode  &&
		m_HitTest == _SVIEW_HITTEST_APPOINTMENT_MOVE)
	{
		ScheduleCmdFactory *pFactory = m_CmdEngine.getFactory();
		scheduleDragCmd *pCmd = (scheduleDragCmd *)pFactory->newCmdBegin(CMD_SCHEDULE_DRAG);
		if(NULL == pCmd)
		{
			_baseCmd *pCmd = pFactory->newCmdEnd();
			if(!m_CmdEngine.execCmd(pCmd))
				pFactory->delCmd(pCmd);
			return;
		}
		pCmd->SaveBeforeDrag();
	}
}

void  ScheduleCtrl::EndDragTest()
{
	ScheduleCmdFactory *pFactory = m_CmdEngine.getFactory();
	_baseCmd *pCmd = pFactory->newCmdEnd();
	if(NULL == pCmd)
		return;
	ASSERT(pCmd->isObjOf(CMD_SCHEDULE_DURATION_CHANGE) || pCmd->isObjOf(CMD_SCHEDULE_DRAG));
	if(!m_CmdEngine.execCmd(pCmd))
		pFactory->delCmd(pCmd);
}

void ScheduleCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	//by RaymondLiu
	if(point.y < CALENDAR_CTRL_HEIGHT)
	{
		return ;
	}
	point.y -= CALENDAR_CTRL_HEIGHT;

	SetFocus ();

    StartCapture ();

    // If view onlbuttondown() return ture, means hit appointments
    if (GetCurrentView ()->OnLButtonDown (nFlags, point))
    {
		BeginDragTest();
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
            ClearAppointmentSelection (TRUE);
        }

        m_CurSelectedDate = GetCurrentView()->GetDateFromPoint(point);
    }
    /*
    if(m_HitTest == _SVIEW_HITTEST_OPLAYER_RIGHT)
    {
    HorizontallyCopyAndPaste(1, COPY_RIGHT);
    }
    else if (m_HitTest == _SVIEW_HITTEST_OPLAYER_LEFT)
    {
    HorizontallyCopyAndPaste(1, COPY_LEFT);
    }
    else if (m_HitTest == _SVIEW_HITTEST_OPLAYER_TOP)
    {
    VerticallyCopyAndPaste(1, COPY_UP);
    }
    else if (m_HitTest == _SVIEW_HITTEST_OPLAYER_BOTTOM)
    {
    VerticallyCopyAndPaste(1, COPY_DOWN);
    }*/

    if (GetCurrentView()->IsPtInToday(point))
    {
        SetToday();
    }
}

void ScheduleCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	//by RaymondLiu
	if(point.y < CALENDAR_CTRL_HEIGHT)
	{
		return ;
	}
	point.y -= CALENDAR_CTRL_HEIGHT;

    StopDragDrop ();

    SetRedraw(FALSE);

    if (!GetCurrentView ()->OnLButtonUp (nFlags, point))
    {
        CWnd::OnLButtonUp(nFlags, point);

        StopRelocate (TRUE);
    }
    else
    {
        StopRelocate (FALSE);
    }

    StopSelect ();

    StopCapture();

    if (m_HitTest == _SVIEW_HITTEST_DEFAULT_APP && HitTest(point) != _SVIEW_HITTEST_DEFAULT_APP)
    {
        SetDefaultAppointment(NULL);
    }

	EndDragTest();

    GetCurrentView()->UpdateOpMode(OPERATION_NONE);

    SetRedraw(TRUE);
    RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

}

void ScheduleCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	//by RaymondLiu
	if(point.y < CALENDAR_CTRL_HEIGHT)
	{
		return ;
	}
	point.y -= CALENDAR_CTRL_HEIGHT;

    if (IsCaptured ())
    {
        m_ptOpCurrent = point;
    }

	//
	//if last mouse operation(LButtonDown) is click on any appointment object.
	//
    if (IsCaptured ()&& m_OpMode == OPERATION_NONE)
    {
        if((nFlags & MK_CONTROL) && (nFlags & MK_LBUTTON))
        {
            GetCurrentView()->UpdateOpMode(OPERATION_COPY);
            StartDragDrop();
        }
        else if ((nFlags & MK_LBUTTON) != 0) //if LButton is keep pressing,and set to "Drag" mode.
        {
            StartRelocate();
        }
    }

    BOOL bHandled = GetCurrentView ()->OnMouseMove (nFlags, point);

    if (!bHandled)
    {
        CWnd::OnMouseMove (nFlags, point);
    }
    else if(m_bRelocate)// In "Drag" Mode.
    {
        if(m_HitTest == _SVIEW_HITTEST_APPOINTMENT_TOP)
        {
            COleDateTime DateCur (GetCurrentView()->GetDateFromPoint (m_ptOpCurrent));// Mouse Cursor position to "datetime" translation.

            Appointment* pApp;

            if (GetCurrentView()->GetFoucedAppointment())
            {
                pApp = GetCurrentView()->GetFoucedAppointment();

		
                COleDateTime datecur (DateCur);
                COleDateTime dateAppStart (pApp->GetStart());
				COleDateTime dateAppEnd (pApp->GetFinish());
				
                datecur = min(datecur, dateAppEnd - COleDateTimeSpan(0,0,0, 10));

                COleDateTimeSpan Movedate = datecur - dateAppStart;

                COleDateTime StartTime (pApp->GetStart() + Movedate);

				//if have no appointments to block "drag",set new date to change appointment.
                if(!IsAppointmentOverlapWithOthers(pApp, Movedate, COleDateTimeSpan())) 
                {
                    if (m_bNotifyParent && GetSafeHwnd () != NULL)
                    {
                        GetParent ()->SendMessage (SCHEDULE_BEFORE_UPDATE_APPOINTMENT, 0, (LPARAM) pApp);
                    }

                    pApp->SetInterval(StartTime, pApp->GetFinish(), &m_AppsManager);//update appointment data.

                    if (m_bNotifyParent)
                    {
                        GetParent()->SendMessage
                            (SCHEDULE_UPDATE_APPOINTMENT, (WPARAM) FALSE, (LPARAM) pApp);
                    }
                }
            }
            else
            {
                /*POSITION pos = m_SelectedAppsList.GetHeadPosition();
                while(pos != NULL)
                {
                    pApp = m_SelectedAppsList.GetNext(pos);

                    COleDateTime StartTime (pApp->GetStart() + Movedate);
                    COleDateTime EndTime (pApp->GetFinish() + Movedate);

                    if(m_HitTest == _SVIEW_HITTEST_APPOINTMENT_TOP && StartTime < pApp->GetFinish())
                    {
                        if(!IsAppointmentOverlapWithOthers(pApp, Movedate, COleDateTimeSpan()))
                        {
                            if (m_bNotifyParent && GetSafeHwnd () != NULL)
                            {
                                GetParent ()->SendMessage (SCHEDULE_BEFORE_UPDATE_APPOINTMENT, 0, (LPARAM) pApp);
                            }

                            pApp->SetInterval(StartTime, pApp->GetFinish(), &m_AppsManager);

                            if (m_bNotifyParent)
                            {
                                GetParent()->SendMessage
                                    (SCHEDULE_UPDATE_APPOINTMENT, (WPARAM) FALSE, (LPARAM) pApp);
                            }
                        }
                    }
                }   */
            }
        }
        else if(m_HitTest == _SVIEW_HITTEST_APPOINTMENT_BOTTOM)
        {
            COleDateTime DateCur (GetCurrentView()->GetDateFromPoint (m_ptOpCurrent));

            Appointment* pApp;

            if (GetCurrentView()->GetFoucedAppointment())
            {
                pApp = GetCurrentView()->GetFoucedAppointment();

                COleDateTime datecur (DateCur);
                COleDateTime dateAppStart (pApp->GetStart());
                COleDateTime dateAppEnd (pApp->GetFinish());

                datecur = max(datecur, dateAppStart + COleDateTimeSpan(0,0,0, 10));

                COleDateTimeSpan Movedate = datecur - dateAppEnd;

                COleDateTime EndTime (pApp->GetFinish() + Movedate);

			/*	int nDays = EndTime.GetDay();
				int nHours = EndTime.GetHour();
				if(nDays)
					nHours += 24 * nDays;
				if(nHours > 1000)
				{
					AfxMessageBox(Translate(_T("Can's support Duration large than 1000 hours!")));
					return;
				}*/
                if(!IsAppointmentOverlapWithOthers(pApp, COleDateTimeSpan(), Movedate))
                {
                    if (m_bNotifyParent && GetSafeHwnd () != NULL)
                    {
                        GetParent ()->SendMessage (SCHEDULE_BEFORE_UPDATE_APPOINTMENT, 0, (LPARAM) pApp);
                    }

                    pApp->SetInterval(pApp->GetStart(), EndTime, &m_AppsManager);

                    if (m_bNotifyParent)
                    {
                        GetParent()->SendMessage
                            (SCHEDULE_UPDATE_APPOINTMENT, (WPARAM) FALSE, (LPARAM) pApp);
                    }
                }
            }
            else
            {
                /*POSITION pos = m_SelectedAppsList.GetHeadPosition();
                while(pos != NULL)
                {
                    pApp = m_SelectedAppsList.GetNext(pos);

                    COleDateTime StartTime (pApp->GetStart() + Movedate);
                    COleDateTime EndTime (pApp->GetFinish() + Movedate);

                    if (m_bNotifyParent && GetSafeHwnd () != NULL)
                    {
                        GetParent ()->SendMessage (SCHEDULE_BEFORE_UPDATE_APPOINTMENT, 0, (LPARAM) pApp);
                    }

                    if(m_HitTest == _SVIEW_HITTEST_APPOINTMENT_TOP && StartTime < pApp->GetFinish())
                    {
                        if(!IsAppointmentOverlapWithOthers(pApp, Movedate, COleDateTimeSpan()))
                        {
                            pApp->SetInterval(StartTime, pApp->GetFinish(), &m_AppsManager);
                        }
                    }
                    else if(pApp->GetStart() < EndTime)
                    {
                        if(!IsAppointmentOverlapWithOthers(pApp, COleDateTimeSpan(), Movedate))
                        {
                            pApp->SetInterval(pApp->GetStart(), EndTime, &m_AppsManager);
                        }
                    }

                    if (m_bNotifyParent)
                    {
                        GetParent()->SendMessage
                            (SCHEDULE_UPDATE_APPOINTMENT, (WPARAM) FALSE, (LPARAM) pApp);
                    }
                } */
            }

            m_ptOpStart = m_ptOpCurrent;
        }
    }

    if (m_bDragDrop || m_bRelocate && (nFlags & MK_LBUTTON) != 0)
    {
        GetCurrentView()->AdjustLayout(FALSE);
    }

    if ((nFlags & MK_LBUTTON) != 0)
    {
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

void ScheduleCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{

}
void ScheduleCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{

}

BOOL ScheduleCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (GetCurrentView ()->OnSetCursor (pWnd, nHitTest, message))
    {
        return TRUE;
    }

    return CWnd::OnSetCursor (pWnd, nHitTest, message);
}
void ScheduleCtrl::OnSetFocus(CWnd* pOldWnd)
{

}
void ScheduleCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (m_bNotifyParent && GetSafeHwnd () != NULL)
    {
        GetParent ()->SendMessage (SCHEDULE_KEYDOWN, nChar, MAKELPARAM(nRepCnt, nFlags));
    }
}
void ScheduleCtrl::OnTimer(UINT_PTR nIDEvent)
{

}
void ScheduleCtrl::OnTimeChange()
{

}
void ScheduleCtrl::OnCancelMode()
{

}
LRESULT ScheduleCtrl::OnSetFont (WPARAM, LPARAM)
{
    return 0;
}
LRESULT ScheduleCtrl::OnGetFont (WPARAM, LPARAM)
{
    return 0;
}
BOOL ScheduleCtrl::OnNeedToolTipText(UINT id, NMHDR* pNMH, LRESULT* pResult)
{
    return TRUE;
}
LRESULT ScheduleCtrl::OnUpdateToolTips (WPARAM, LPARAM)
{
    return 0;
}

void ScheduleCtrl::SetStyle (_SVIEW_STYLE style, BOOL bRedraw /*= TRUE*/)
{
    ASSERT (_SVIEW_STYLE_FIRST <= style);
    ASSERT (style <= _SVIEW_STYLE_LAST);

    if (m_Style == style)
    {
        return;
    }

    _SVIEW_STYLE oldStyle = m_Style;

    const BOOL bIsWndCreated = GetSafeHwnd () != NULL;

    if (bIsWndCreated)
    {
        SetRedraw (FALSE);
    }

    AppointmentList lst;
    CopyList<AppointmentList>(m_SelectedAppsList, lst);

    //COleDateTime date = GetCurrentView ()->GetDate ();
    ScheduleView* pOldView = m_pCurrentView;

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

    UpdateCalendarsSelection ();
    //UpdateCalendarsState ();

    if (m_bNotifyParent && GetSafeHwnd () != NULL)
    {
        GetParent ()->SendMessage (SCHEDULE_STYLE_CHANGED, (WPARAM) oldStyle, (LPARAM) m_Style);
    }
}

ScheduleView* ScheduleCtrl::GetView (_SVIEW_STYLE style)
{
    ASSERT (_SVIEW_STYLE_FIRST <= style);
    ASSERT (style <= _SVIEW_STYLE_LAST);

    switch (style)
    {
    case _SVIEW_STYLE_ALPHA:
        return m_pViews [0];
    }

    return NULL;
}



void ScheduleCtrl::StartSelect ()
{
    CPoint pt;
    ::GetCursorPos (&pt);
    ScreenToClient (&pt);

    GetCurrentView ()->StartSelect ();

    m_OpMode = OPERATION_SELECT;
}

void ScheduleCtrl::StopSelect ()
{
    if(m_OpMode == OPERATION_SELECT && m_ptOpStart != m_ptOpCurrent)
    SelectArea(CRect(min(m_ptOpStart.x,m_ptOpCurrent.x),min(m_ptOpStart.y,m_ptOpCurrent.y),
        max(m_ptOpStart.x,m_ptOpCurrent.x),max(m_ptOpStart.y,m_ptOpCurrent.y)));

    GetCurrentView ()->StopSelect();

    m_OpMode = OPERATION_NONE;
}

BOOL ScheduleCtrl::UpdateChangeOperation (_SVIEW_OPERATION operation, BOOL bResult /*= FALSE*/)
{
    BOOL bRes = FALSE;

    if (m_Operation != operation)
    {
        if (m_Operation == _SVIEW_OPERATION_NONE)
        {
            m_Operation = operation;
            if(m_bNotifyParent && GetSafeHwnd () != NULL && GetParent ()->GetSafeHwnd () != NULL)
            {
                bRes = GetParent ()->SendMessage (SCHEDULE_BEGIN_CHANGE_OPERATION,
                    (WPARAM)m_Operation, (LPARAM)FALSE) == 0L;
            }

            if(!bRes)
            {
                m_Operation = _SVIEW_OPERATION_NONE;
            }
        }
        else if (operation == _SVIEW_OPERATION_NONE)
        {
            if(m_bNotifyParent && GetSafeHwnd () != NULL && GetParent ()->GetSafeHwnd () != NULL)
            {
                GetParent ()->PostMessage (SCHEDULE_END_CHANGE_OPERATION,
                    (WPARAM)m_Operation, (LPARAM)bResult);
            }
            m_Operation = operation;
        }
    }

    return bRes;
}

void ScheduleCtrl::StartDragDrop()
{
    if (GetCurrentView () == NULL)
    {
        return;
    }

    if (!m_bDragDrop)
    {
        if (!UpdateChangeOperation(_SVIEW_OPERATION_DRAG))
        {
            return;
        }

        COleDataSource srcItem;

        try
        {
            CSharedFile globFile;

            m_bDragDrop =  SerializeTo(globFile);

            if (m_bDragDrop)
            {
                srcItem.CacheGlobalData (GetClipboardFormat (),
                    globFile.Detach());
            }

        }
        catch (COleException* pEx)
        {
            TRACE(_T("ScheduleCtrl DragDrop. OLE exception: %x\r\n"),
                pEx->m_sc);
            pEx->Delete ();

            m_bDragDrop = FALSE;
        }
        catch (CNotSupportedException *pEx)
        {
            TRACE(_T("ScheduleCtrl DragDrop. \"Not Supported\" exception\r\n"));
            pEx->Delete ();

            m_bDragDrop = FALSE;
        }

        if (!m_bDragDrop)
        {
            return;
        }
        m_dragEffect = DROPEFFECT_NONE;

        DWORD dwEffects = DROPEFFECT_COPY | DROPEFFECT_SCROLL;

        dwEffects |= DROPEFFECT_MOVE;

        DROPEFFECT dropEffect = srcItem.DoDragDrop (dwEffects, NULL, &m_DropSource);

        BOOL bRes = dropEffect == DROPEFFECT_MOVE || dropEffect == DROPEFFECT_COPY;

        if (m_bNotifyParent && bRes)
        {
            GetParent ()->SendMessage (SCHEDULE_DRAG_APPOINTMENTS, (WPARAM)dropEffect, 0);
        }

        UpdateChangeOperation(_SVIEW_OPERATION_NONE, bRes);
    }
}


void ScheduleCtrl::StopDragDrop()
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


void ScheduleCtrl::StartRelocate ()
{
    if (GetCurrentView () == NULL)
    {
        return;
    }

    m_bRelocate = TRUE;

    GetCurrentView ()->StartRelocate();

    m_OpMode = OPERATION_DRAG;
}

void ScheduleCtrl::StopRelocate (BOOL bCancel)
{
    if (GetCurrentView () == NULL)
    {
        return;
    }

    GetCurrentView ()->StopRelocate();

    if (!bCancel && m_bRelocate && m_HitTest == _SVIEW_HITTEST_APPOINTMENT_MOVE)
    {
        m_bRelocate = FALSE;

        CRect RectClient;
        GetClientRect(RectClient);
		//RectClient.top += CALENDAR_CTRL_HEIGHT;
		RectClient.bottom -= CALENDAR_CTRL_HEIGHT;

        CPoint pt;
        ::GetCursorPos (&pt);
        ScreenToClient (&pt);
		//by RaymondLiu
		if(pt.y < CALENDAR_CTRL_HEIGHT ) 
		{	
			pt.y = -1;
		}
		else 
		{
			pt.y -= CALENDAR_CTRL_HEIGHT;
		}

        if (!RectClient.PtInRect(pt))
        {
            //RemoveSelectedAppointments();
        }
        else
        {
            RelocateDragedAppointments();
        }

        m_OpMode = OPERATION_NONE;
    }

}

void ScheduleCtrl::SelectAppointment (Appointment* pApp, BOOL bSelect, BOOL bRedraw)
{
    ASSERT_VALID (pApp);

    int nOldCount = GetSelectedAppointmentsCount ();

    POSITION pos = m_SelectedAppsList.Find (pApp);
    BOOL bOperation = (bSelect && (pos == NULL)) || (!bSelect && (pos != NULL));

    if (m_bNotifyParent && GetSafeHwnd () != NULL && bOperation)
    {
        bOperation = GetParent ()->SendMessage (SCHEDULE_BEFORE_SELECT_APPOINTMENT,
            (WPARAM) bSelect, (LPARAM) pApp) == 0L;
    }

    if (bOperation)
    {
        pApp->SetSelected (bSelect);

        if (bSelect)
        {
            m_SelectedAppsList.AddTail (pApp);
        }
        else
        {
            m_SelectedAppsList.RemoveAt (pos);
        }

        if (m_bNotifyParent && GetSafeHwnd () != NULL)
        {
            GetParent ()->SendMessage (SCHEDULE_SELECT_APPOINTMENT, (WPARAM) bSelect, (LPARAM) pApp);
        }
    }

    GetOpLayer()->SetRect(GetCurrentView()->GetUnionRectOfSelectedAppointments());

    if (GetSelectedAppointmentsCount () != nOldCount && bRedraw)
    {

        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

void ScheduleCtrl::SelectAppointments (const AppointmentArray& Apps, BOOL bSelect, BOOL bRedraw)
{
    for (int i = 0; i < Apps.GetSize (); i++)
    {
        SelectAppointment(Apps.GetAt(i), bSelect, FALSE);
    }

    if (bRedraw)
    {

        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

void ScheduleCtrl::SelectArea (const CRect& rect, BOOL bRedraw)
{
    CRect TestRect;
    ClearAppointmentSelection (FALSE);

    const AppointmentArray& arApps = GetQueryedAppointments ();

    int nDay = -1;
    COleDateTime date = GetCurrentView()->GetDateBegin();
    COleDateTimeSpan span = COleDateTimeSpan(1, 0, 0, 0);
    const int nDuration = GetCurrentView()->GetViewDays ();
    for (int i = 0; i < nDuration; i++)
    {

        for (int i = 0; i < arApps.GetSize (); i++)
        {
            TestRect.IntersectRect(arApps[i]->GetRectDraw(date), rect);
            if (!TestRect.IsRectEmpty())
            {
                SelectAppointment(arApps[i], TRUE, FALSE);
            }
        }

        date += span;
    }

    if (bRedraw)
    {
        GetOpLayer()->SetRect(GetCurrentView()->GetUnionRectOfSelectedAppointments());

        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

void ScheduleCtrl::ClearAppointmentSelection (BOOL bRedraw)
{
    int nOldCount = GetSelectedAppointmentsCount ();

    if (nOldCount > 0)
    {
        BOOL bNotify = m_bNotifyParent && GetSafeHwnd () != NULL;

        while (m_SelectedAppsList.GetTailPosition () != NULL)
        {
            Appointment* pApp = m_SelectedAppsList.GetTail ();

            pApp->SetSelected (FALSE);
            m_SelectedAppsList.RemoveTail ();

            if (bNotify)
            {
                GetParent ()->SendMessage
                    (SCHEDULE_SELECT_APPOINTMENT, (WPARAM) FALSE, (LPARAM) pApp);
            }
        }

        if (bRedraw)
        {
            RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        }
    }
}

BOOL ScheduleCtrl::IsAppointmentsOverlap(const Appointment& App1, const Appointment& App2)
{
    if (App1.GetStart() < App2.GetStart())
    {
        return App1.GetFinish() > App2.GetStart();
    }
    else
    {
        return App1.GetStart() < App2.GetFinish();
    }
}

BOOL ScheduleCtrl::IsAppointmentOverlapWithOthers(const Appointment* pApp,
    const COleDateTimeSpan& offsetStart, const COleDateTimeSpan& offsetEnd)
{
    COleDateTime DateBegin = pApp->GetStart() - COleDateTimeSpan(1, 0, 0, 0);
    COleDateTime DateEnd = pApp->GetFinish() + COleDateTimeSpan(1, 0, 0, 0);
    QueryAppointments(DateBegin + offsetStart, DateEnd + offsetEnd, FALSE);

    const AppointmentArray& arApps = GetQueryedAppointments ();

    Appointment TempApp;

    COleDateTime StartTime (pApp->GetStart() + offsetStart);
    COleDateTime EndTime (pApp->GetFinish() + offsetEnd);

    TempApp.SetInterval(StartTime, EndTime);

    //if (m_bNotifyParent && GetSafeHwnd () != NULL)
    //{
    //    GetParent ()->SendMessage (SCHEDULE_BEFORE_UPDATE_APPOINTMENT, 0, (LPARAM) &TempApp);
    //}

    for (int i = 0; i < arApps.GetSize (); i++)
    {
        if (pApp != arApps[i] && IsAppointmentsOverlap(*arApps[i], TempApp))
        {
            QueryAppointments(FALSE);
            return TRUE;
        }
    }

    QueryAppointments(FALSE);
    return FALSE;
}

BOOL ScheduleCtrl::IsNewAppointmentOverlapWithExists(const Appointment* pApp)
{
    COleDateTime DateBegin = pApp->GetStart() - COleDateTimeSpan(1, 0, 0, 0);
    COleDateTime DateEnd = pApp->GetFinish() + COleDateTimeSpan(1, 0, 0, 0);

    if (m_bNotifyParent && GetSafeHwnd () != NULL)
    {
        Appointment TempApp;
        TempApp.SetInterval(DateBegin, DateEnd);
        GetParent ()->SendMessage (SCHEDULE_BEFORE_UPDATE_APPOINTMENT, 0, (LPARAM) &TempApp);
    }

    QueryAppointments(DateBegin, DateEnd, FALSE);

    const AppointmentArray& arApps = GetQueryedAppointments ();

    for (int i = 0; i < arApps.GetSize (); i++)
    {
        if (IsAppointmentsOverlap(*arApps[i], *pApp))
        {
            QueryAppointments(FALSE);
            return TRUE;
        }
    }

    QueryAppointments(FALSE);
    return FALSE;
}

BOOL ScheduleCtrl::IsAppointmentsOverlapWithExsits(const AppointmentArray& pApps, const COleDateTimeSpan& offset)
{

    Appointment TempApp;

    int nOldCount = pApps.GetSize();

    if (nOldCount > 0)
    {
        COleDateTime minStartTime = pApps[0]->GetStart() + offset;
        COleDateTime maxEndTime = pApps[0]->GetFinish() + offset;

        for (int i = 0; i < pApps.GetSize (); i++)
        {
            minStartTime = min(minStartTime, pApps[i]->GetStart() + offset);
            maxEndTime = max(maxEndTime, pApps[i]->GetFinish() + offset);
        }

        minStartTime -= COleDateTimeSpan(1, 0, 0, 0);
        maxEndTime += COleDateTimeSpan(1, 0, 0, 0);

        if (m_bNotifyParent && GetSafeHwnd () != NULL)
        {
            TempApp.SetInterval(minStartTime, maxEndTime);
            GetParent ()->SendMessage (SCHEDULE_BEFORE_UPDATE_APPOINTMENT, 0, (LPARAM) &TempApp);
        }

        QueryAppointments(minStartTime, maxEndTime, FALSE);

        const AppointmentArray& arApps = GetQueryedAppointments ();

        for (int i = 0; i < pApps.GetSize (); i++)
        {

            COleDateTime StartTime (pApps[i]->GetStart() + offset);
            COleDateTime EndTime (pApps[i]->GetFinish() + offset);

            TempApp.SetInterval(StartTime, EndTime);

            for (int i = 0; i < arApps.GetSize (); i++)
            {
                if (IsAppointmentsOverlap(*arApps[i], TempApp))
                {
                    QueryAppointments(FALSE);
                    return TRUE;
                }
            }
        }

        QueryAppointments(FALSE);
    }

    return FALSE;
}

BOOL ScheduleCtrl::IsSelectedAppointmentOverlapWithOthers(
    const COleDateTimeSpan& offsetStart, const COleDateTimeSpan& offsetEnd, DWORD DaysInWeek)
{

    Appointment TempApp;

    int nOldCount = GetSelectedAppointmentsCount ();

    if (nOldCount > 0)
    {
        COleDateTime minStartTime = m_SelectedAppsList.GetHead()->GetStart() + offsetStart;
        COleDateTime maxEndTime = m_SelectedAppsList.GetHead()->GetFinish() + offsetEnd;

        POSITION pos = m_SelectedAppsList.GetHeadPosition();
        while(pos != NULL)
        {
            Appointment* pApp = m_SelectedAppsList.GetNext(pos);

            minStartTime = min(minStartTime, COleDateTime(pApp->GetStart() + offsetStart));
            maxEndTime = max(maxEndTime, COleDateTime(pApp->GetFinish() + offsetEnd));
        }

        minStartTime -= COleDateTimeSpan(1, 0, 0, 0);
        maxEndTime += COleDateTimeSpan(1, 0, 0, 0);

        if (m_bNotifyParent && GetSafeHwnd () != NULL)
        {
            TempApp.SetInterval(minStartTime - offsetStart, maxEndTime - offsetEnd);
            GetParent ()->SendMessage (SCHEDULE_BEFORE_UPDATE_APPOINTMENT, 0, (LPARAM) &TempApp);
        }

        QueryAppointments(minStartTime, maxEndTime, FALSE);

        const AppointmentArray& arApps = GetQueryedAppointments ();

        pos = m_SelectedAppsList.GetHeadPosition();
        while(pos != NULL)
        {
            Appointment* pApp = m_SelectedAppsList.GetNext(pos);

            COleDateTime StartTime (pApp->GetStart() + offsetStart);
            COleDateTime EndTime (pApp->GetFinish() + offsetEnd);

            if (!(DaysInWeek & 1 << (StartTime.GetDayOfWeek()-1)) ||
                !(DaysInWeek & 1 << (EndTime.GetDayOfWeek()-1)))
            {
                continue;
            }

            TempApp.SetInterval(StartTime, EndTime);

            for (int i = 0; i < arApps.GetSize (); i++)
            {
                if (!arApps[i]->IsSelected() && IsAppointmentsOverlap(*arApps[i], TempApp))
                {
                    QueryAppointments(FALSE);
                    return TRUE;
                }
            }
        }

        QueryAppointments(FALSE);
    }

    return FALSE;
}

BOOL ScheduleCtrl::GetSelectedAppointmentOverlapWithOthers(
	const COleDateTimeSpan& offsetStart, const COleDateTimeSpan& offsetEnd,  AppointmentArray &arOverlapApps, DWORD DaysInWeek)
{

	Appointment TempApp;

	int  nOldCount = GetSelectedAppointmentsCount ();
	BOOL bOverlap = FALSE;

	if (nOldCount > 0)
	{
		COleDateTime minStartTime = m_SelectedAppsList.GetHead()->GetStart() + offsetStart;
		COleDateTime maxEndTime = m_SelectedAppsList.GetHead()->GetFinish() + offsetEnd;

		POSITION pos = m_SelectedAppsList.GetHeadPosition();
		while(pos != NULL)
		{
			Appointment* pApp = m_SelectedAppsList.GetNext(pos);

			minStartTime = min(minStartTime, COleDateTime(pApp->GetStart() + offsetStart));
			maxEndTime = max(maxEndTime, COleDateTime(pApp->GetFinish() + offsetEnd));
		}

		minStartTime -= COleDateTimeSpan(1, 0, 0, 0);
		maxEndTime += COleDateTimeSpan(1, 0, 0, 0);

		if (m_bNotifyParent && GetSafeHwnd () != NULL)
		{
			TempApp.SetInterval(minStartTime - offsetStart, maxEndTime - offsetEnd);
			GetParent ()->SendMessage (SCHEDULE_BEFORE_UPDATE_APPOINTMENT, 0, (LPARAM) &TempApp);
		}

		QueryAppointments(minStartTime, maxEndTime, FALSE);

		const AppointmentArray& arApps = GetQueryedAppointments ();

		pos = m_SelectedAppsList.GetHeadPosition();

		while(pos != NULL)
		{
			Appointment* pApp = m_SelectedAppsList.GetNext(pos);

			COleDateTime StartTime (pApp->GetStart() + offsetStart);
			COleDateTime EndTime (pApp->GetFinish() + offsetEnd);

			if (!(DaysInWeek & 1 << (StartTime.GetDayOfWeek()-1)) ||
				!(DaysInWeek & 1 << (EndTime.GetDayOfWeek()-1)))
			{
				continue;
			}

			TempApp.SetInterval(StartTime, EndTime);

			for (int i = 0; i < arApps.GetSize (); i++)
			{
				if (!arApps[i]->IsSelected() && IsAppointmentsOverlap(*arApps[i], TempApp))
				{
					arOverlapApps.Add(arApps[i]);
					//QueryAppointments(FALSE);
					bOverlap = TRUE;
					//return TRUE;
				}
			}
		}

		QueryAppointments(FALSE);
	}

	return bOverlap;
}

void ScheduleCtrl::RelocateDragedAppointments()
{
    int nOldCount = GetSelectedAppointmentsCount ();

    if (nOldCount > 0)
    {
        BOOL bNotify = m_bNotifyParent && GetSafeHwnd () != NULL;

        COleDateTime datecur (GetCurrentView()->GetDateFromPoint (m_ptOpCurrent));
        COleDateTime datestart (GetCurrentView()->GetDateFromPoint (m_ptOpStart));

        COleDateTimeSpan Movedate = datecur - datestart;

        Appointment* pApp;
        BOOL bOverlap = FALSE;

        if(IsSelectedAppointmentOverlapWithOthers(Movedate, Movedate))
        {
            bOverlap = TRUE;
        }

        if (!bOverlap)
        {
            POSITION pos = m_SelectedAppsList.GetHeadPosition();
            while(pos != NULL)
            {
                pApp = m_SelectedAppsList.GetNext(pos);

                COleDateTime StartTime (pApp->GetStart() + Movedate);
                COleDateTime EndTime (pApp->GetFinish() + Movedate);

                //if (m_bNotifyParent && GetSafeHwnd () != NULL)
                //{
                //    GetParent ()->SendMessage (SCHEDULE_BEFORE_UPDATE_APPOINTMENT, 0, (LPARAM) pApp);
                //}

                pApp->SetInterval(StartTime, EndTime, &m_AppsManager);

                if (bNotify)
                {
                    GetParent()->SendMessage
                        (SCHEDULE_UPDATE_APPOINTMENT, (WPARAM) FALSE, (LPARAM) pApp);
                }
            }

            QueryAppointments(FALSE);

            GetCurrentView()->AdjustLayout(FALSE);

            RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        }

    }
}

void ScheduleCtrl::SetDate (const COleDateTime& date, BOOL bRedraw)
{
    GetCurrentView()->SetDate(date);

    BOOL bNotify = m_bNotifyParent && GetSafeHwnd () != NULL;
    if (bNotify)
    {
        GetParent()->SendMessage
            (SCHEDULE_DATE_CHANGED, (WPARAM) FALSE, (LPARAM) NULL);
    }

    UpdateCalendarsSelection ();

    if (bRedraw && GetSafeHwnd()!=NULL)
    {
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

//
//Geting display range of ctrl. -zxy
//
void ScheduleCtrl::GetDateInterval(COleDateTime &dBegin ,COleDateTime &dEnd)
{
	 dBegin = GetCurrentView()->GetDateBegin();
	 dEnd = GetCurrentView()->GetDateEnd();
}

void ScheduleCtrl::SetDateInterval (const COleDateTime& date1, const COleDateTime& date2, BOOL bRedraw)
{

    GetCurrentView()->SetDateInterval(date1, date2);

    BOOL bNotify = m_bNotifyParent && GetSafeHwnd () != NULL;
    if (bNotify)
    {
        GetParent()->SendMessage
            (SCHEDULE_DATE_CHANGED, (WPARAM) FALSE, (LPARAM) NULL);
    }

    UpdateCalendarsSelection ();

    if (bRedraw && GetSafeHwnd()!=NULL)
    {
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

void ScheduleCtrl::SetToday (BOOL bRedraw)
{
	COleDateTime  today;
	today = COleDateTime::GetCurrentTime() ;
	int nPage = this->GetDisplayRange();

	COleDateTime EndDay ;
	COleDateTimeSpan  span;
	span.SetDateTimeSpan(nPage,0,0,0);

	EndDay = today + span;
	pStartTimeCtrl->SetDate(today.m_dt);
	pEndTimeCtrl->SetDate(EndDay.m_dt);

    GetCurrentView()->SetDate(today);

    UpdateCalendarsSelection ();
}

void ScheduleCtrl::UpdateCalendarsSelection ()
{
    if (!m_pCalendar)
    {
        return;
    }

    m_pCalendar->ClearSelectedDates (FALSE, FALSE);
    COleDateTime StartDate = GetCurrentView()->GetDateBegin();
    COleDateTime EndDate = GetCurrentView()->GetDateEnd();
    for (; StartDate <= EndDate; StartDate += COleDateTimeSpan(1,0,0,0))
    {
        m_pCalendar->SelectDate(StartDate, TRUE, FALSE, FALSE);
    }

    m_pCalendar->RecalcLayout();
}

void ScheduleCtrl::SetTimeDelta (_SVIEW_TIME_DELTA delta, BOOL bRedraw)
{
    m_TimeDelta = delta;

    if (bRedraw && GetSafeHwnd()!=NULL)
    {
        GetCurrentView()->AdjustLayout(FALSE);
		GetCurrentView()->RecauclateEnableVisibleOffset();
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

COleDateTimeSpan ScheduleCtrl::GetSelectedAppointmentsRange()
{
    int nOldCount = GetSelectedAppointmentsCount ();

    COleDateTimeSpan span(1, 0, 0, 0);
    if (nOldCount > 0)
    {
        POSITION pos = m_SelectedAppsList.GetHeadPosition();

        Appointment* pApp = m_SelectedAppsList.GetNext(pos);

        COleDateTime MinTime = pApp->GetStart();
        COleDateTime MaxTime = pApp->GetFinish();

        pos = m_SelectedAppsList.GetHeadPosition();
        while(pos != NULL)
        {
            pApp = m_SelectedAppsList.GetNext(pos);

            MinTime = min(MinTime, pApp->GetStart());
            MaxTime = max(MaxTime, pApp->GetFinish());
        }

        span = MaxTime - MinTime;
        LONG nDays = (LONG)span.GetDays() + 1;

        span.SetDateTimeSpan(nDays,0, 0, 0);
    }

    return span;
}

BOOL ScheduleCtrl::GetContinuerousDeleteRanageDate(const int nDels, COPY_DIRECTION Direction,
    COleDateTime& S, COleDateTime& E)
{
    int nCount = GetSelectedAppointmentsCount ();

    if (nCount <= 0)
    {
        return FALSE;
    }

    if (nDels <= 0)
    {
        return FALSE;
    }
    // anyway, I will delete the selected ones.
    int DelRepeats = nDels - 1;

    COleDateTime MinTime;
    COleDateTime MaxTime;
    COleDateTimeSpan span;

    if (Direction > COPY_RIGHT)
    {
        // Vertically
        // check layout of the selected appointments
        POSITION pos = m_SelectedAppsList.GetHeadPosition();

        Appointment* pApp = m_SelectedAppsList.GetNext(pos);

        int minHour = 24;
        int maxHour = 0;
        int minMinite = 60;
        int maxMinite = 0;
        int minSecond = 60;
        int maxSecond = 0;

        MinTime = pApp->GetStart();
        MaxTime = pApp->GetFinish();

        pos = m_SelectedAppsList.GetHeadPosition();
        while(pos != NULL)
        {
            pApp = m_SelectedAppsList.GetNext(pos);

            minHour = min(minHour, pApp->GetStart().GetHour());
            maxHour = max(maxHour, pApp->GetFinish().GetHour());
            minMinite = min(minMinite, pApp->GetStart().GetMinute());
            maxMinite = max(maxMinite, pApp->GetFinish().GetMinute());
            minSecond = min(minSecond, pApp->GetStart().GetSecond());
            maxSecond = max(maxSecond, pApp->GetFinish().GetSecond());

            MinTime = min(MinTime, pApp->GetStart());
            MaxTime = max(MaxTime, pApp->GetFinish());
        }
        span = COleDateTimeSpan(0, maxHour - minHour, maxMinite - minMinite, maxSecond - minSecond);
        if (Direction == COPY_UP)
        {
            span = -span;
        }

        // delete
        MaxTime = COleDateTime(MaxTime.GetYear(), MaxTime.GetMonth(), MaxTime.GetDay(), 23, 59, 59);

        COleDateTimeSpan endspan = (DelRepeats * span);
        COleDateTime EndTime = MinTime + endspan;

        S = MinTime;
        E = EndTime;
    }
    else
    {
        // Horizontally
        // check layout of the selected appointments
        POSITION pos = m_SelectedAppsList.GetHeadPosition();

        Appointment* pApp = m_SelectedAppsList.GetNext(pos);

        MinTime = pApp->GetStart();
        MaxTime = pApp->GetFinish();

        pos = m_SelectedAppsList.GetHeadPosition();
        while(pos != NULL)
        {
            pApp = m_SelectedAppsList.GetNext(pos);

            MinTime = min(MinTime, pApp->GetStart());
            MaxTime = max(MaxTime, pApp->GetFinish());
        }
        span = MaxTime - MinTime;
        LONG nDays = (LONG)span.GetDays() + 1;

        if (Direction == COPY_LEFT)
        {
            nDays = -nDays;
        }

        MaxTime = COleDateTime(MaxTime.GetYear(), MaxTime.GetMonth(), MaxTime.GetDay(), 23, 59, 59);
        MaxTime += COleDateTimeSpan(max(0, (DelRepeats - 1))*nDays, 0, 0, 0);

        S = MinTime;
        E = MaxTime;
    }

    return TRUE;
}

BOOL ScheduleCtrl::ContinuerousDelete(const int nDels, COPY_DIRECTION Direction)
{
    int nCount = GetSelectedAppointmentsCount ();

    if (nCount <= 0)
    {
        return FALSE;
    }

    if (nDels <= 0)
    {
        return FALSE;
    }
    // anyway, I will delete the selected ones.
    int DelRepeats = nDels - 1;

    AppointmentArray ToDelArray;
    COleDateTime MinTime;
    COleDateTime MaxTime;
    COleDateTimeSpan span;
    COleDateTimeSpan step;
    COleDateTimeSpan spanstep;

    if (Direction > COPY_RIGHT)
    {
        // Vertically
        // check layout of the selected appointments
        POSITION pos = m_SelectedAppsList.GetHeadPosition();

        Appointment* pApp = m_SelectedAppsList.GetNext(pos);

        int minHour = 24;
        int maxHour = 0;
        int minMinite = 60;
        int maxMinite = 0;
        int minSecond = 60;
        int maxSecond = 0;

        MinTime = pApp->GetStart();
        MaxTime = pApp->GetFinish();

        pos = m_SelectedAppsList.GetHeadPosition();
        while(pos != NULL)
        {
            pApp = m_SelectedAppsList.GetNext(pos);

            minHour = min(minHour, pApp->GetStart().GetHour());
            maxHour = max(maxHour, pApp->GetFinish().GetHour());
            minMinite = min(minMinite, pApp->GetStart().GetMinute());
            maxMinite = max(maxMinite, pApp->GetFinish().GetMinute());
            minSecond = min(minSecond, pApp->GetStart().GetSecond());
            maxSecond = max(maxSecond, pApp->GetFinish().GetSecond());

            MinTime = min(MinTime, pApp->GetStart());
            MaxTime = max(MaxTime, pApp->GetFinish());
        }
        span = COleDateTimeSpan(0, maxHour - minHour, maxMinite - minMinite, maxSecond - minSecond);
        if (Direction == COPY_UP)
        {
            span = -span;
        }

        // delete
        step = span;
        spanstep = span;
        MaxTime = COleDateTime(MaxTime.GetYear(), MaxTime.GetMonth(), MaxTime.GetDay(), 23, 59, 59);

        QueryAppointments(MinTime, MaxTime, FALSE);
        COleDateTimeSpan endspan = (DelRepeats * spanstep);
        COleDateTime EndTime = MinTime + endspan;
        for (COleDateTime date = (MinTime + span); date <= EndTime; date += step)
        {
            if (date > (MinTime + span))
            {
                span += spanstep;
            }

            POSITION pos = m_SelectedAppsList.GetHeadPosition();
            while(pos != NULL)
            {
                Appointment* pApp = m_SelectedAppsList.GetNext(pos);

                COleDateTime iSTime (pApp->GetStart() + span);
                COleDateTime iETime (pApp->GetFinish() + span);

                for (int i = 0; i < m_QueryAppsArray.GetSize(); i++)
                {
                    Appointment* pQApp = m_QueryAppsArray[i];

                    if (pQApp->GetDetailID() == pApp->GetDetailID() &&
                        (pQApp->GetStart() == iSTime && pQApp->GetFinish() == iETime))
                    {
                        ToDelArray.Add(pQApp);
                    }
                }
            }
        }
    }
    else
    {
        // Horizontally
        // check layout of the selected appointments
        POSITION pos = m_SelectedAppsList.GetHeadPosition();

        Appointment* pApp = m_SelectedAppsList.GetNext(pos);

        MinTime = pApp->GetStart();
        MaxTime = pApp->GetFinish();

        pos = m_SelectedAppsList.GetHeadPosition();
        while(pos != NULL)
        {
            pApp = m_SelectedAppsList.GetNext(pos);

            MinTime = min(MinTime, pApp->GetStart());
            MaxTime = max(MaxTime, pApp->GetFinish());
        }
        span = MaxTime - MinTime;
        LONG nDays = (LONG)span.GetDays() + 1;

        if (Direction == COPY_LEFT)
        {
            nDays = -nDays;
        }

        // delete
        step = COleDateTimeSpan(1,0,0,0);
        MaxTime = COleDateTime(MaxTime.GetYear(), MaxTime.GetMonth(), MaxTime.GetDay(), 23, 59, 59);
        MaxTime += COleDateTimeSpan(max(0, (DelRepeats - 1))*nDays, 0, 0, 0);
        span = COleDateTimeSpan(nDays, 0, 0, 0);
        spanstep = span;

        for (COleDateTime date = (MinTime + span); date <= MaxTime; date += step)
        {
            if (date > (MinTime + span))
            {
                span += spanstep;
            }
            QueryAppointments(date, date, FALSE);

            POSITION pos = m_SelectedAppsList.GetHeadPosition();
            while(pos != NULL)
            {
                Appointment* pApp = m_SelectedAppsList.GetNext(pos);

                COleDateTime iSTime (pApp->GetStart() + span);
                COleDateTime iETime (pApp->GetFinish() + span);

                for (int i = 0; i < m_QueryAppsArray.GetSize(); i++)
                {
                    Appointment* pQApp = m_QueryAppsArray[i];
                    if (pQApp->GetStart().GetDayOfYear() > date.GetDayOfYear() ||
                        pQApp->GetFinish().GetDayOfYear() < date.GetDayOfYear())
                    {
                        continue;
                    }

                    if (pQApp->GetDetailID() == pApp->GetDetailID() &&
                        (pQApp->GetStart() == iSTime && pQApp->GetFinish() == iETime))
                    {
                        ToDelArray.Add(pQApp);
                    }
                }
            }
        }
    }

    for (int i = 0; i < ToDelArray.GetSize(); i++)
    {
        RemoveAppointment(ToDelArray[i], FALSE, FALSE);
    }

    if (nDels > 0)
    {
        RemoveSelectedAppointments(FALSE, FALSE);
    }

    QueryAppointments(FALSE);
    GetCurrentView()->AdjustLayout(FALSE);
    RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

    return TRUE;
}

void ScheduleCtrl::BeginHCopyPaste()
{
	ScheduleCmdFactory *pFactory = m_CmdEngine.getFactory();
	_baseCmd *pCmd = pFactory->newCmdBegin(CMD_SCHEDULE_HPASTE);
	ASSERT(pCmd);
}
void ScheduleCtrl::RemoveAppointInHCopyPaste(Appointment *pApp)
{
	ScheduleCmdFactory *pFactory = m_CmdEngine.getFactory();
	_baseCmd *pCmd = pFactory->getBuffedCmd();
	ASSERT(pCmd);
	ASSERT(pCmd->isObjOf(CMD_SCHEDULE_HPASTE));
	scheduleSubDeleteCmd *pDelCmd = (scheduleSubDeleteCmd*)pFactory->newCmd(CMD_SCHEDULE_SUB_DELETE);
	ASSERT(pDelCmd);
	pDelCmd->_Appointments.AddTail(pApp);
	if(IS_FAILURE(pDelCmd->ondo()))
		pFactory->delCmd(pDelCmd);
	else
		pCmd->addSubCmd(pDelCmd);
}

void ScheduleCtrl::OnHCopyPasteDone(Appointment *pApp)
{
	ScheduleCmdFactory *pFactory = m_CmdEngine.getFactory();
	schedulePasteCmd *pCmd = (schedulePasteCmd*)pFactory->getBuffedCmd();
	ASSERT(pCmd);
	ASSERT(pCmd->isObjOf(CMD_SCHEDULE_HPASTE));
	pCmd->_PasteList.AddTail(pApp);
}

void ScheduleCtrl::EndHCopyPaste()
{
	ScheduleCmdFactory *pFactory = m_CmdEngine.getFactory();
	schedulePasteCmd *pCmd = (schedulePasteCmd*)pFactory->newCmdEnd();
	ASSERT(pCmd);
	ASSERT(pCmd->isObjOf(CMD_SCHEDULE_HPASTE));
	if(!m_CmdEngine.execCmd(pCmd))
		pFactory->delCmd(pCmd);
	
}

BOOL ScheduleCtrl::HorizontallyCopyAndPaste(const COleDateTime& RangeStart, const COleDateTime& RangeEnd, DWORD DaysInWeek)
{
    int nOldCount = GetSelectedAppointmentsCount ();

    if (nOldCount > 0)
    {
        // check layout of the selected appointments
        POSITION pos = m_SelectedAppsList.GetHeadPosition();

        Appointment* pApp = m_SelectedAppsList.GetNext(pos);

        COleDateTime MinTime = pApp->GetStart();
        COleDateTime MaxTime = pApp->GetFinish();

        pos = m_SelectedAppsList.GetHeadPosition();
        while(pos != NULL)
        {
            pApp = m_SelectedAppsList.GetNext(pos);

            MinTime = min(MinTime, pApp->GetStart());
            MaxTime = max(MaxTime, pApp->GetFinish());
        }

        COleDateTimeSpan span = MaxTime - MinTime;
		LONG nDays = (LONG)span.GetDays()  + 1;
		COleDateTime tsTime = MaxTime - COleDateTimeSpan(0,0,0,1);
		if(tsTime.GetDay() == MinTime.GetDay())
			nDays -= 1;
		if(nDays <= 0)
			nDays = 1;
        // check if it is empty in the destination time span.
        COleDateTime STime(RangeStart.GetYear(), RangeStart.GetMonth(), RangeStart.GetDay(), 0, 0, 0);
        COleDateTime ETime(RangeEnd.GetYear(), RangeEnd.GetMonth(), RangeEnd.GetDay(), 0, 0, 0);
        ETime += COleDateTimeSpan(1,0,0,0);
        COleDateTime TStart(MinTime.GetYear(), MinTime.GetMonth(), MinTime.GetDay(), 0, 0, 0);
        INT nCopies = static_cast<INT>((ETime - STime)/COleDateTimeSpan(nDays,0,0,0));

        // paste the selected appointments
		static bool bOverlapPromt = false;
		int RetCode = 0;
		AppointmentArray arOverlapApps;

		BeginHCopyPaste();
        for (int i = 0; i < nCopies; i++)
        {
            span.SetDateTimeSpan(i*nDays,0, 0, 0);
            span +=  STime - TStart;
			arOverlapApps.RemoveAll();
			if (GetSelectedAppointmentOverlapWithOthers(span,span, arOverlapApps))
			{
				if (RetCode != IDYESTOALL)
				{				
					RetCode = MessageBox(Translate(_T("Some time-range is occupied, do you want cover it?")),Translate(_T("Warning")),MB_YESNO|MB_YESTOALL|MB_ICONWARNING);
					if (RetCode == IDYES || RetCode == IDYESTOALL)
					{
						for (int i = 0; i < arOverlapApps.GetSize() && arOverlapApps[i] != NULL; i++)
						{
							RemoveAppointInHCopyPaste(arOverlapApps[i]);
							//RemoveAppointment(arOverlapApps[i],  TRUE, TRUE);
						}
						
					}
					else
					{
						continue;
					}
				}
				else
				{
					for (int i = 0; i < arOverlapApps.GetSize() && arOverlapApps[i] != NULL; i++)
					{
						RemoveAppointInHCopyPaste(arOverlapApps[i]);
						//RemoveAppointment(arOverlapApps[i],  TRUE, TRUE);
					}
				}
			}

            POSITION pos = m_SelectedAppsList.GetHeadPosition();
            while(pos != NULL)
            {
                Appointment* pApp = m_SelectedAppsList.GetNext(pos);

                COleDateTime StartTime (pApp->GetStart() + span);
                COleDateTime EndTime (pApp->GetFinish() + span);

                if (!(DaysInWeek & 1 << (StartTime.GetDayOfWeek()-1)) ||
                    !(DaysInWeek & 1 << (EndTime.GetDayOfWeek()-1)))
                {
                    continue;
                }

                if (StartTime <  STime||
                   EndTime > ETime ||
                   (StartTime >= MinTime&& EndTime <= MaxTime))
                {
                    continue;
                }


                Appointment* pnewApp = new Appointment();

                if (!pnewApp)
                {
					EndHCopyPaste();
                    return FALSE;
                }

                *pnewApp = *pApp;

                pnewApp->SetInterval(StartTime, EndTime);

                if (!AddAppointment(pnewApp, FALSE, FALSE))
                {
                    delete pnewApp;
					EndHCopyPaste();
                    return FALSE;
                }else
				{
					OnHCopyPasteDone(pnewApp);
				}

                SelectAppointment(pnewApp, FALSE, FALSE);
            }
        }

		EndHCopyPaste();

        QueryAppointments();
        GetCurrentView()->AdjustLayout(FALSE);
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

		/*if(bOverlapPromt)
		{
			MessageBox(Translate(_T("Ignore some occupied time-range!")), Translate(_T("Horizontal Copy")), MB_OK|MB_ICONEXCLAMATION);
			}*/
		bOverlapPromt = false;
        return TRUE;
    }

    return FALSE;
}

BOOL ScheduleCtrl::HorizontallyCopyAndPaste(const int nCopies, COPY_DIRECTION Direction, DWORD DaysInWeek)
{
    int nOldCount = GetSelectedAppointmentsCount ();

    if (nOldCount > 0)
    {
        // check layout of the selected appointments
        POSITION pos = m_SelectedAppsList.GetHeadPosition();

        Appointment* pApp = m_SelectedAppsList.GetNext(pos);

        COleDateTime MinTime = pApp->GetStart();
        COleDateTime MaxTime = pApp->GetFinish();

        pos = m_SelectedAppsList.GetHeadPosition();
        while(pos != NULL)
        {
            pApp = m_SelectedAppsList.GetNext(pos);

            MinTime = min(MinTime, pApp->GetStart());
            MaxTime = max(MaxTime, pApp->GetFinish());
        }
        COleDateTimeSpan span = MaxTime - MinTime;
        LONG nDays = (LONG)span.GetDays() + 1;

        if (Direction == COPY_LEFT)
        {
            nDays = -nDays;
        }

        // check if it is empty in the destination time span.
        for (int i = 1; i <= nCopies; i++)
        {
            span.SetDateTimeSpan(i*nDays,0, 0, 0);
            if (IsSelectedAppointmentOverlapWithOthers(span,span))
            {
                return FALSE;
            }
        }

        // paste the selected appointments

        for (int i = 1; i <= nCopies; i++)
        {
            span.SetDateTimeSpan(i*nDays,0, 0, 0);

            POSITION pos = m_SelectedAppsList.GetHeadPosition();
            while(pos != NULL)
            {
                Appointment* pApp = m_SelectedAppsList.GetNext(pos);


                COleDateTime StartTime (pApp->GetStart() + span);
                COleDateTime EndTime (pApp->GetFinish() + span);

                if (!(DaysInWeek & 1 << (StartTime.GetDayOfWeek()-1)) ||
                    !(DaysInWeek & 1 << (EndTime.GetDayOfWeek()-1)))
                {
                    continue;
                }

                 Appointment* pnewApp = new Appointment();

                if (!pnewApp)
                {
                    return FALSE;
                }

                *pnewApp = *pApp;


                pnewApp->SetInterval(StartTime, EndTime);

                if (!AddAppointment(pnewApp, FALSE, FALSE))
                {
                    delete pApp;
                    return FALSE;
                }

                SelectAppointment(pnewApp, FALSE, FALSE);
            }
        }

        QueryAppointments();
        GetCurrentView()->AdjustLayout(FALSE);
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

        return TRUE;
    }

    return FALSE;
}


void  ScheduleCtrl::BeginVCopyPaste()
{
	ScheduleCmdFactory *pFactory = m_CmdEngine.getFactory();
	_baseCmd *pCmd = pFactory->newCmdBegin(CMD_SCHEDULE_PASTE);
	ASSERT(pCmd);
}

void  ScheduleCtrl::RemoveAppointInVCopyPaste(Appointment *pApp)
{

	ScheduleCmdFactory *pFactory = m_CmdEngine.getFactory();
	_baseCmd *pCmd = pFactory->getBuffedCmd();
	ASSERT(pCmd);
	ASSERT(pCmd->isObjOf(CMD_SCHEDULE_PASTE));
	scheduleSubDeleteCmd *pDelCmd = (scheduleSubDeleteCmd*)pFactory->newCmd(CMD_SCHEDULE_SUB_DELETE);
	ASSERT(pDelCmd);
	pDelCmd->_Appointments.AddTail(pApp);
	if(IS_FAILURE(pDelCmd->ondo()))
		pFactory->delCmd(pDelCmd);
	else
		pCmd->addSubCmd(pDelCmd);
}

void  ScheduleCtrl::OnVCopyPasteDone(Appointment *pApp)
{
	ScheduleCmdFactory *pFactory = m_CmdEngine.getFactory();
	schedulePasteCmd *pCmd = (schedulePasteCmd*)pFactory->getBuffedCmd();
	ASSERT(pCmd);
	ASSERT(pCmd->isObjOf(CMD_SCHEDULE_PASTE));
	pCmd->_PasteList.AddTail(pApp);
}

void  ScheduleCtrl::EndVCopyPaste()
{
	ScheduleCmdFactory *pFactory = m_CmdEngine.getFactory();
	schedulePasteCmd *pCmd = (schedulePasteCmd*)pFactory->newCmdEnd();
	ASSERT(pCmd);
	ASSERT(pCmd->isObjOf(CMD_SCHEDULE_PASTE));
	if(!m_CmdEngine.execCmd(pCmd))
		pFactory->delCmd(pCmd);
}
void ScheduleCtrl::AddJustCurrentViewLayout(BOOL bRedraw){ GetCurrentView()->AdjustLayout(bRedraw);}
BOOL ScheduleCtrl::VerticallyCopyAndPaste(const int nCopies, COPY_DIRECTION Direction)
{
    int nOldCount = GetSelectedAppointmentsCount ();

    if (nOldCount > 0)
    {
        // check layout of the selected appointments
        POSITION pos = m_SelectedAppsList.GetHeadPosition();
        Appointment* pApp = m_SelectedAppsList.GetNext(pos);

		Appointment* pStartApp  = pApp;
		Appointment* pEndApp  = pApp;
		int	minDay = 0;
		int maxDay = 0;
        pos = m_SelectedAppsList.GetHeadPosition();
        while(pos != NULL)
        {
            pApp = m_SelectedAppsList.GetNext(pos);
			minDay = pApp->GetStart().GetDay();
			maxDay = pApp->GetFinish().GetDay();
			if(maxDay != minDay)//in same day can copy.
				return FALSE;
			if (pStartApp->GetStart() > pApp->GetStart())
			{
				pStartApp = pApp;
			}
			if (pEndApp->GetStart() < pApp->GetStart())
			{
				pEndApp = pApp;
			}   
        }
		COleDateTimeSpan span = pEndApp->GetFinish() - pStartApp->GetStart();
		span += COleDateTimeSpan(0,0,0,1); //seperate by one second 
        if (Direction == COPY_UP)
        {
            span = -span;
        }

        // check if it is empty in the destination time span.
        for (int i = 1; i <= nCopies; i++)
        {
            COleDateTimeSpan spanTest = i * span;

            Appointment TempApp;

            int nOldCount = GetSelectedAppointmentsCount ();

            if (nOldCount > 0)
            {
                POSITION pos = m_SelectedAppsList.GetHeadPosition();
                while(pos != NULL)
                {
                    Appointment* pApp = m_SelectedAppsList.GetNext(pos);

                    COleDateTime StartTime (pApp->GetStart() + spanTest);
                    COleDateTime EndTime (pApp->GetFinish() + spanTest);

					//确认为同一天
                    if (!(StartTime.GetDay() == pApp->GetStart().GetDay()
                        && EndTime.GetDay() == pApp->GetFinish().GetDay()))
                    {
                        return FALSE;
                    }

                    TempApp.SetInterval(StartTime, EndTime);

                    if (IsNewAppointmentOverlapWithExists(&TempApp))
                    {
                        return FALSE;
                    }
                }
            }

        }

        // paste the selected appointments
		BeginVCopyPaste();
        for (int i = 1; i <= nCopies; i++)
        {
            POSITION pos = m_SelectedAppsList.GetHeadPosition();
            while(pos != NULL)
            {
                Appointment* pApp = m_SelectedAppsList.GetNext(pos);

                Appointment* pnewApp = new Appointment();

                if (!pnewApp)
                {
                    return FALSE;
                }

                *pnewApp = *pApp;

                COleDateTimeSpan spanNew = i * span;
                COleDateTime StartTime;
                COleDateTime EndTime;
                StartTime  = pnewApp->GetStart() + spanNew;
                EndTime = pnewApp->GetFinish() + spanNew;

                pnewApp->SetInterval(StartTime, EndTime);

                if (!AddAppointment(pnewApp, FALSE, FALSE))
                {
                    delete pnewApp;
                    return FALSE;
                }else
					OnVCopyPasteDone(pnewApp);
            }
        }

		EndVCopyPaste();
        QueryAppointments();
        GetCurrentView()->AdjustLayout(FALSE);
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

        return TRUE;
    }

    return FALSE;
}

//BOOL ScheduleCtrl::Copy()
//{
//    if (!(GetSelectedAppointmentsCount() > 0))
//    {
//        return FALSE;
//    }
//
//    // clear the copied apps
//    for (int i = 0; i < m_CopiedAppsArray.GetSize(); i++)
//    {
//        delete m_CopiedAppsArray[i];
//    }
//    m_CopiedAppsArray.RemoveAll();
//
//    // check layout of the selected appointments
//    POSITION pos = m_SelectedAppsList.GetHeadPosition();
//
//    pos = m_SelectedAppsList.GetHeadPosition();
//    while(pos != NULL)
//    {
//        Appointment* pApp = new Appointment;
//        *pApp = *(m_SelectedAppsList.GetNext(pos));
//        m_CopiedAppsArray.Add(pApp);
//
//    }
//
//    SortAppointments(m_CopiedAppsArray, m_CopiedAppsArray.GetSize());
//
//    return TRUE;
//}

void ScheduleCtrl::OnBeginPaste()
{
	ScheduleCmdFactory *pFactory = this->m_CmdEngine.getFactory();
	schedulePasteCmd *pCmd = (schedulePasteCmd *)pFactory->newCmdBegin(CMD_SCHEDULE_PASTE);
	ASSERT(pCmd);
}

void ScheduleCtrl::OnPasteDone( Appointment *pApp)
{
	ScheduleCmdFactory *pFactory = this->m_CmdEngine.getFactory();
	schedulePasteCmd *pCmd = (schedulePasteCmd *)pFactory->getBuffedCmd();
	ASSERT(pApp);
	ASSERT(pCmd);
	ASSERT(pCmd->isObjOf(CMD_SCHEDULE_PASTE));
	pCmd->_PasteList.AddTail(pApp);

}

void ScheduleCtrl::OnEndPaste()
{
	ScheduleCmdFactory *pFactory = this->m_CmdEngine.getFactory();
	schedulePasteCmd *pCmd = (schedulePasteCmd *)pFactory->newCmdEnd();
	ASSERT(pCmd);
	if(!m_CmdEngine.execCmd(pCmd))
		pFactory->delCmd(pCmd);
}

BOOL ScheduleCtrl::Paste(const AppointmentArray& ar)
{
    if (ar.GetSize() > 0)
    {

		OnBeginPaste();
        // check if it is empty in the destination time span.
        COleDateTimeSpan Span = m_CurSelectedDate - ar[0]->GetStart();

        if (IsAppointmentsOverlapWithExsits(ar, Span))
        {
            goto _EXIT;
        }

        // paste the selected appointments

        for (int i = 0; i < ar.GetSize (); i++)
        {
            Appointment* pnewApp = new Appointment();

            if (!pnewApp)
            {
                 goto _EXIT;
            }

            *pnewApp = *ar[i];

            COleDateTime StartTime (pnewApp->GetStart() + Span);
            COleDateTime EndTime (pnewApp->GetFinish() + Span);

            pnewApp->SetInterval(StartTime, EndTime);

            if (!AddAppointment(pnewApp, FALSE, FALSE))
            {
                delete pnewApp;
                goto _EXIT;
            }else
			{
				OnPasteDone( pnewApp);
			}

        }

        QueryAppointments();
        GetCurrentView()->AdjustLayout(FALSE);
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		OnEndPaste();
        return TRUE;

_EXIT:
		OnEndPaste();
		return FALSE;
    }
    return FALSE;
}

LRESULT ScheduleCtrl::OnDragLeave(WPARAM wParam, LPARAM lParam)
{
    return 0;

}

LRESULT ScheduleCtrl::OnDragEnter(WPARAM wParam, LPARAM lParam)
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


    if (pInfo->m_pDataObject->IsDataAvailable(CTimeTableCategoryCtrl::s_TimeTableFormat))
    {
        CPoint point = pInfo->m_point;

        _SVIEW_HITTEST hit = HitTest(point);

        if (hit == _SVIEW_HITTEST_DEFAULT_APP)
        {
            return DROPEFFECT_NONE;
        }

        return DROPEFFECT_COPY;
    }
    return DROPEFFECT_NONE;
}

LRESULT ScheduleCtrl::OnDragOver(WPARAM wParam, LPARAM lParam)
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

    if (pInfo->m_pDataObject->IsDataAvailable(CTimeTableCategoryCtrl::s_TimeTableFormat))
    {


		CPoint point = pInfo->m_point;
		point.y -= CALENDAR_CTRL_HEIGHT;

		_SVIEW_HITTEST hit = HitTest(point);

		if (hit == _SVIEW_HITTEST_DEFAULT_APP)
		{
			return DROPEFFECT_NONE;
		}

		return DROPEFFECT_COPY;
    }
    return DROPEFFECT_NONE;
}


LRESULT ScheduleCtrl::OnDrop(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1L;

    HGLOBAL hGlobal = NULL;
    HDROP hDrop = NULL;

    CS3OleDropInfo* pInfo = (CS3OleDropInfo*)wParam;
    BOOL bInvalidData = FALSE;

    CPoint point = pInfo->m_point;
	//by RaymondLiu
	if(point.y < CALENDAR_CTRL_HEIGHT)
	{
		return FALSE;
	}
	point.y -= CALENDAR_CTRL_HEIGHT;
	
    if(m_OpMode == OPERATION_DRAG)
    {
        OnLButtonUp(0, point);

        return FALSE;
    }

    OnLButtonUp(0, point);

    //if (dropEffect != DROPEFFECT_MOVE && dropEffect != DROPEFFECT_COPY)
    //{
    //    return FALSE;
    //}

    //if (GetCurrentView ()->IsCaptureMatched () && dropEffect != DROPEFFECT_COPY)
    //{
    //    return FALSE;
    //}

    ASSERT (pInfo->m_pDataObject != NULL);

    _SVIEW_HITTEST hit = HitTest(point);

    BOOL bRes = FALSE;

    if (hit == _SVIEW_HITTEST_DEFAULT_APP)
    {
        try
        {
            m_dtDrop = COleDateTime ();

            CFile* pFile;
            CLIPFORMAT DataFormat = GetClipboardFormat ();
            if(pInfo->m_pDataObject->IsDataAvailable(CLayoutCategoryCtrl::s_LayoutFormat))
            {
                pFile = pInfo->m_pDataObject->GetFileData (CLayoutCategoryCtrl::s_LayoutFormat);

                DataFormat = CLayoutCategoryCtrl::s_LayoutFormat;
            }

            if(pInfo->m_pDataObject->IsDataAvailable(CTimeTableCategoryCtrl::s_TimeTableFormat))
            {
                pFile = pInfo->m_pDataObject->GetFileData (CTimeTableCategoryCtrl::s_TimeTableFormat);

                DataFormat = CTimeTableCategoryCtrl::s_TimeTableFormat;
            }

            if(pInfo->m_pDataObject->IsDataAvailable(GetClipboardFormat ()))
            {
                pFile = pInfo->m_pDataObject->GetFileData (GetClipboardFormat ());

                DataFormat = GetClipboardFormat ();
            }

            if (pFile == NULL)
            {
                return FALSE;
            }

            AppointmentArray ar;

            BOOL bResult = FALSE;

            if(DataFormat == GetClipboardFormat ())
            {
                bResult = ScheduleCtrl::SerializeFromAppointment (*pFile, ar, m_Style, m_dtDrop);
            }

            if(DataFormat == CTimeTableCategoryCtrl::s_TimeTableFormat)
            {
                bResult = ScheduleCtrl::SerializeFromTimeTable (*pFile, ar, m_Style, m_dtDrop);
            }

            if(DataFormat == CLayoutCategoryCtrl::s_LayoutFormat)
            {
                bResult = ScheduleCtrl::SerializeFromLayout (*pFile, ar, m_Style, m_dtDrop);
            }
            if (!bResult)
            {
                return FALSE;
            }

            if (ar.GetSize() > 1)
            {
                ASSERT(0);
            }

            for (int i = 0; i < ar.GetSize (); i++)
            {
                SetDefaultAppointment(ar[i]);
                if (m_bNotifyParent)
                {
                    GetParent ()->SendMessage (SCHEDULE_SET_DEFAULT_APPOINTMENT, 0, reinterpret_cast<LPARAM>(ar[0]));
                }
                delete ar[i];
            }

            delete pFile;

            GetCurrentView()->AdjustLayout(FALSE);
            RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

        }
        catch (COleException* pEx)
        {
            TRACE(_T("ScheduleCtrl::OnDrop. OLE exception: %x\r\n"),
                pEx->m_sc);
            pEx->Delete ();
        }
    }
    else
    {
        m_dtDrop = GetCurrentView ()->GetDateFromPoint(m_ptOpCurrent);

        try
        {
            CFile* pFile;
            CLIPFORMAT DataFormat = GetClipboardFormat ();
            if(pInfo->m_pDataObject->IsDataAvailable(CLayoutCategoryCtrl::s_LayoutFormat))
            {
                pFile = pInfo->m_pDataObject->GetFileData (CLayoutCategoryCtrl::s_LayoutFormat);

                DataFormat = CLayoutCategoryCtrl::s_LayoutFormat;
            }

            if(pInfo->m_pDataObject->IsDataAvailable(CTimeTableCategoryCtrl::s_TimeTableFormat))
            {
                pFile = pInfo->m_pDataObject->GetFileData (CTimeTableCategoryCtrl::s_TimeTableFormat);

                DataFormat = CTimeTableCategoryCtrl::s_TimeTableFormat;
            }

            if(pInfo->m_pDataObject->IsDataAvailable(GetClipboardFormat ()))
            {
                pFile = pInfo->m_pDataObject->GetFileData (GetClipboardFormat ());

                DataFormat = GetClipboardFormat ();
            }

            if (pFile == NULL)
            {
                m_dtDrop = COleDateTime ();

                return FALSE;
            }

            AppointmentArray ar;

            bRes = ScheduleCtrl::SerializeFrom (*pFile, GetCurrentView ()->GetDateFromPoint (point), DataFormat);

            delete pFile;

            if (m_bNotifyParent)
            {
                GetParent ()->SendMessage (SCHEDULE_DROP_APPOINTMENTS, 0, 0);
            }

        }
        catch (COleException* pEx)
        {
            TRACE(_T("ScheduleCtrl::OnDrop. OLE exception: %x\r\n"),
                pEx->m_sc);
            pEx->Delete ();
        }

        m_dtDrop = COleDateTime ();
    }

    return bRes;
}



CLIPFORMAT ScheduleCtrl::GetClipboardFormat ()
{
    return s_ClpFormat;
}

BOOL ScheduleCtrl::SerializeTo (CFile& file)
{
    AppointmentArray ar;

    POSITION pos = m_SelectedAppsList.GetHeadPosition ();
    while (pos != NULL)
    {
        Appointment* pApp = m_SelectedAppsList.GetNext (pos);
        ASSERT_VALID (pApp);

        ar.Add (pApp);
    }

    return ScheduleCtrl::SerializeTo (file, ar, GetCurrentView()->GetDateFromPoint(m_ptOpCurrent));
}

BOOL ScheduleCtrl::SerializeTo (CFile& file, AppointmentArray& ar, COleDateTime& dateFrom)
{
    const int nCount = (int) ar.GetSize ();

    if (nCount == 0)
    {
        return FALSE;
    }

    CArchive Archive (&file, CArchive::store);

    BOOL bResult = ScheduleCtrl::SerializeTo (Archive, ar, dateFrom);

    Archive.Close();

    return bResult;
}

BOOL ScheduleCtrl::SerializeTo (CArchive& Archive, AppointmentArray& ar, COleDateTime& dateFrom)
{
    const int nCount = (int) ar.GetSize ();

    if (nCount == 0)
    {
        return FALSE;
    }

    BOOL bStored = FALSE;

    try
    {
        Archive << nCount;
        Archive << dateFrom;

        for (int i = 0; i < nCount; i++)
        {
            Appointment* pApp = ar[i];
            ASSERT_VALID (pApp);

            if (pApp != NULL)
            {
                Archive << pApp;
                bStored = TRUE;
            }
        }
    }
    catch (CArchiveException* pEx)
    {
        TRACE(_T("ScheduleCtrl::SerializeTo. Archive exception\r\n"));
        pEx->Delete ();
        return FALSE;
    }

    return bStored;
}

//for supporting cmd system.
//create "create obj" cmd here.
void ScheduleCtrl::BeginAddAppointments(void)
{
	ScheduleCmdFactory *pFactory = m_CmdEngine.getFactory(); 
	_baseCmd *pCmd = pFactory->newCmdBegin(CMD_SCHEDULE_CREATE);
	ASSERT(pCmd);
	 
}
//Add obj handler to cmd system.
void ScheduleCtrl::OnAddAppointment(Appointment *pApp)
{   
	ScheduleCmdFactory *pFactory = m_CmdEngine.getFactory(); 
	scheduleCreateCmd *pCmd = (scheduleCreateCmd *)pFactory->getBuffedCmd();
	ASSERT(pCmd);
	pCmd->_Appointments.AddTail(pApp);
}
//end 
void ScheduleCtrl::EndAddAppointments(void)
{
	ScheduleCmdFactory *pFactory = m_CmdEngine.getFactory(); 
	_baseCmd *pCmd =pFactory->newCmdEnd();
	ASSERT(pCmd);
	if(!m_CmdEngine.execCmd(pCmd))
		pFactory->delCmd(pCmd);
	 
}

BOOL ScheduleCtrl::SerializeFrom (CFile& file, const COleDateTime& dtTo, CLIPFORMAT DataFormat)
{
    AppointmentArray ar;

    BOOL bResult = FALSE;

    if(DataFormat == GetClipboardFormat ())
    {
        bResult = ScheduleCtrl::SerializeFromAppointment (file, ar, m_Style, dtTo);
    }

    if(DataFormat == CTimeTableCategoryCtrl::s_TimeTableFormat)
    {
        bResult = ScheduleCtrl::SerializeFromTimeTable (file, ar, m_Style, dtTo);
    }

    if(DataFormat == CLayoutCategoryCtrl::s_LayoutFormat)
    {
        bResult = ScheduleCtrl::SerializeFromLayout (file, ar, m_Style, dtTo);
    }

    if (!bResult)
    {
        return FALSE;
    }

    ScheduleView* pView = GetCurrentView ();
    ASSERT_VALID(pView);

    BOOL bAllDay = FALSE;
    BOOL bNeedAdjust = m_Style == _SVIEW_STYLE_ALPHA;

    for (int i = 0; i < ar.GetSize (); i++)
    {
        if (IsAppointmentOverlapWithOthers(ar[i], COleDateTimeSpan(),  COleDateTimeSpan()))
        {
            for (int j = 0; j < ar.GetSize (); j++)
            {
                delete ar[j];
            }

            return FALSE;
        }
    }


    COleDateTimeSpan MinSpan = COleDateTimeSpan(1, 0, 0, 0);
	this->BeginAddAppointments();
    for (int i = 0; i < ar.GetSize (); i++)
    {
        Appointment* pApp = ar[i];

        if (AddAppointment (pApp, FALSE, FALSE)) 
			OnAddAppointment(pApp);//for cmds system - zxy
		else //avoid memory leak ! -zxy
		{
			ar.RemoveAt(i);
			this->m_AppsManager.Remove(pApp);
			delete pApp;
			if(i>= 1)i--;
			continue;
		}
        MinSpan = min(MinSpan, pApp->GetDuration());
    }
	//
	//for cmds system -zxy
	//
	this->EndAddAppointments();
#if 0  //zxy dont change delta ,except select it by hand.
    BOOL bTimeDeltaChanged = FALSE;
    _SVIEW_TIME_DELTA TimeDelta = _SVIEW_TIME_DELTA_60;
    _SVIEW_TIME_DELTA OldTimeDelta = GetTimeDelta();
    if (MinSpan < COleDateTimeSpan(0, 0, 5, 0))
    {
        TimeDelta = _SVIEW_TIME_DELTA_1;
        if (OldTimeDelta != TimeDelta)
        {
            SetTimeDelta(TimeDelta);
            bTimeDeltaChanged = TRUE;
        }
    }
    else if (MinSpan < COleDateTimeSpan(0, 0, 10, 0))
    {
        TimeDelta = _SVIEW_TIME_DELTA_10;
        if (OldTimeDelta != TimeDelta)
        {
            SetTimeDelta(TimeDelta);
            bTimeDeltaChanged = TRUE;
        }
    }
    else if (MinSpan < COleDateTimeSpan(0, 0, 15, 0))
    {
        TimeDelta = _SVIEW_TIME_DELTA_15;
        if (OldTimeDelta != TimeDelta)
        {
            SetTimeDelta(TimeDelta);
            bTimeDeltaChanged = TRUE;
        }
    }

    if (bTimeDeltaChanged)
    {
        GetCurrentView()->AdjustScrollSizes();
        GetCurrentView()->ScrollToDate(dtTo);

        if (m_bNotifyParent && GetSafeHwnd () != NULL)
        {
            GetParent ()->SendMessage (SCHEDULE_TIME_DELTA_CHANGED, (WPARAM)TimeDelta, 0);
        }
    }
#endif

    if (ar.GetSize () > 0)
    {
        ClearAppointmentSelection (FALSE);
        QueryAppointments ();
        SelectAppointments(ar, TRUE, FALSE);

        GetCurrentView()->AdjustLayout(FALSE);
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

    return bResult;
}

BOOL ScheduleCtrl::SerializeFromAppointment (CFile& file,
    AppointmentArray& ar, _SVIEW_STYLE style, const COleDateTime& dtTo)
{
    CArchive Archive (&file, CArchive::load);

    BOOL bResult = ScheduleCtrl::SerializeFromAppointment (Archive, ar, style, dtTo);

    Archive.Close();

    return bResult;
}

BOOL ScheduleCtrl::SerializeFromLayout (CFile& file, AppointmentArray& ar,
        _SVIEW_STYLE style, const COleDateTime& dtTo)
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

                        Index = m_ScheduleItemIconList.Add(&bmp, RGB(123,456,789));
                    }
                    pStream -> Release();
                }
                GlobalFree(hGlobal);

                m_IconDetailIDMapper.SetAt(layout.GetLayoutID(), Index);
            }
        }
        pApp->SetImage(&m_ScheduleItemIconList, Index);

        COleDateTime LeadingDate = COleDateTime();

        pApp->SetInterval(dtTo + pApp->GetStart() - LeadingDate,
                dtTo + pApp->GetStart() - LeadingDate + pApp->GetDuration());

        ar.Add (pApp);
    }

    Archive.Close();

    delete pDoc;

    return TRUE;
}

BOOL ScheduleCtrl::SerializeFromTimeTable (CFile& file, AppointmentArray& ar,
        _SVIEW_STYLE style, const COleDateTime& dtTo)
{
    CArchive Archive (&file, CArchive::load);
    CString XMLString;
    Archive >> XMLString;


	TiXmlDocument pDoc;// = new TiXmlDocument();

	std::string xml = StringUtility::ConvertUnicodeToUTF8(XMLString.GetBuffer(0));
	pDoc.Parse(reinterpret_cast<LPCSTR>(xml.c_str()), NULL, TIXML_ENCODING_UTF8);

    TiXmlElement* pLayoutElement = pDoc.RootElement();

	if(pLayoutElement == NULL) return FALSE;

    Timetable timetable;

    if(timetable.LoadFromXML(pLayoutElement))
    {
#ifdef SERVER_EDITION
        if (timetable.GetAudit().GetStatus() != Audit::APPROVED)
        {
             AfxMessageBox(Translate(_T("Only approved item could be dragged to use!")));
            return FALSE;
        }
#endif
        Appointment *pApp = new Appointment;

        pApp->SetDescription(timetable.GetTimetableName().c_str());

        int timeTableDuration  = 0;


        for (auto it = timetable.LayoutEntryList.begin();
            it != timetable.LayoutEntryList.end(); it++)
        {
            timeTableDuration += it->GetDuration();
        }

		timeTableDuration = max(30, timeTableDuration);

        pApp->SetInterval(COleDateTime(), COleDateTime() + COleDateTimeSpan(timeTableDuration / (24*3600),
            (timeTableDuration % (24*3600)) / 3600,(timeTableDuration % 3600) / 60, (timeTableDuration % 60)));
        pApp->SetBackgroundColor(0xFF001234);

        pApp->SetDetailID(timetable.GetTimetableID());
        pApp->SetDetailType(Appointment::APPOINTMENT_TYPE_TIMETABLE);

        COleDateTime LeadingDate = COleDateTime();

        pApp->SetInterval(dtTo + pApp->GetStart() - LeadingDate,
                dtTo + pApp->GetStart() - LeadingDate + pApp->GetDuration());

        ar.Add(pApp);

    }

    Archive.Close();

//     delete pDoc;

    return TRUE;
}



BOOL ScheduleCtrl::SerializeFromAppointment (CArchive& Archive,
    AppointmentArray& ar, _SVIEW_STYLE style, const COleDateTime& dtTo)
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

            pApp->SetInterval(dtTo + pApp->GetStart() - LeadingDate,
                dtTo + pApp->GetStart() - LeadingDate + pApp->GetDuration());

            if (pApp != NULL)
            {
                ar.Add (pApp);
            }
        }

    }
    catch (CArchiveException* pEx)
    {
        TRACE(_T("ScheduleCtrl::SerializeTo. Archive exception\r\n"));
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

void ScheduleCtrl::AlignUp (Appointment* pApp, BOOL bRedraw)
{
    ASSERT_VALID (pApp);

    if (pApp == NULL)
    {
        return;
    }

    COleDateTimeSpan span (1, 0, 0, 0);
    QueryAppointments(pApp->GetStart() - span,pApp->GetFinish() + span,FALSE);

    COleDateTime StartTime = COleDateTime(pApp->GetStart().GetYear(),
        pApp->GetStart().GetMonth(), pApp->GetStart().GetDay(), 0, 0, 0);

    AppointmentArray & arApps = GetQueryedAppointments();

    for (int i = 0; i < arApps.GetSize (); i++)
    {
        if (arApps[i]->GetFinish() <= pApp->GetStart())
        {
            StartTime = max(arApps[i]->GetFinish(), StartTime);
        }
    }

    if (m_bNotifyParent && GetSafeHwnd () != NULL)
    {
        GetParent ()->SendMessage (SCHEDULE_BEFORE_UPDATE_APPOINTMENT, 0, (LPARAM) pApp);
    }

    pApp->SetInterval(StartTime, StartTime + pApp->GetDuration(), &m_AppsManager);

    BOOL bNotify = m_bNotifyParent && GetSafeHwnd () != NULL;
    if (bNotify)
    {
        GetParent()->SendMessage
            (SCHEDULE_UPDATE_APPOINTMENT, (WPARAM) FALSE, (LPARAM) pApp);
    }

    QueryAppointments();

    if (bRedraw)
    {
        GetCurrentView()->AdjustLayout(FALSE);
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

void ScheduleCtrl::AlignDown (Appointment* pApp, BOOL bRedraw)
{
    ASSERT_VALID (pApp);

    if (pApp == NULL)
    {
        return;
    }

    COleDateTimeSpan span (1, 0, 0, 0);
    QueryAppointments(pApp->GetStart() - span, pApp->GetFinish() + span, FALSE);

    COleDateTime EndTime = COleDateTime(pApp->GetFinish().GetYear(),
        pApp->GetFinish().GetMonth(), pApp->GetFinish().GetDay(), 23, 59, 59);

    AppointmentArray & arApps = GetQueryedAppointments();

   // for (int i = 0; i < arApps.GetSize (); i++)
	int i= arApps.GetSize();

	while( -- i >= 0)
    {
        if (arApps[i]->GetStart() >= pApp->GetFinish())
        {
            EndTime = min(arApps[i]->GetStart(), EndTime);
        }
    }

    if (m_bNotifyParent && GetSafeHwnd () != NULL)
    {
        GetParent ()->SendMessage (SCHEDULE_BEFORE_UPDATE_APPOINTMENT, 0, (LPARAM) pApp);
    }

    pApp->SetInterval(EndTime - pApp->GetDuration(), EndTime, &m_AppsManager);

    BOOL bNotify = m_bNotifyParent && GetSafeHwnd () != NULL;
    if (bNotify)
    {
        GetParent()->SendMessage
            (SCHEDULE_UPDATE_APPOINTMENT, (WPARAM) FALSE, (LPARAM) pApp);
    }

    QueryAppointments(FALSE);

    if (bRedraw)
    {
        GetCurrentView()->AdjustLayout(FALSE);
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

void ScheduleCtrl::AlignUpSelectedAppointments (BOOL bRedraw)
{
    SetRedraw(FALSE);

    POSITION pos = m_SelectedAppsList.GetHeadPosition();
    while(pos != NULL)
    {
        Appointment* pApp = m_SelectedAppsList.GetNext(pos);

        AlignUp(pApp, FALSE);
    }

    SetRedraw(TRUE);

    if (bRedraw)
    {
        GetCurrentView()->AdjustLayout(FALSE);
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}
void ScheduleCtrl::AlignDownSelectedAppointments (BOOL bRedraw)
{
    SetRedraw(FALSE);


    POSITION pos = m_SelectedAppsList.GetTailPosition();
    while(pos != NULL)
    {
        Appointment* pApp = m_SelectedAppsList.GetPrev(pos);

        AlignDown(pApp, FALSE);
    }

    SetRedraw(TRUE);

    if (bRedraw)
    {
        GetCurrentView()->AdjustLayout(FALSE);
        RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

void ScheduleCtrl::AlignSelections(BOOL bUp)
{
	ScheduleCmdFactory *pFactory = m_CmdEngine.getFactory();
	scheduleAlignCmd *pCmd = (scheduleAlignCmd *)pFactory->newCmd(CMD_SCHEDULE_ALIGN);
	if(bUp <= 0)
		pCmd->_AlignDirection = ALIGN_DOWN;

	if(!m_CmdEngine.execCmd(pCmd))
		pFactory->delCmd(pCmd);


}
BOOL ScheduleCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message >= WM_MOUSEFIRST &&
		pMsg->message <= WM_MOUSELAST)
	{
		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		//by RaymondLiu
		if(pt.y >= CALENDAR_CTRL_HEIGHT) 
		{
			pt.y -= CALENDAR_CTRL_HEIGHT;

			if (GetCurrentView()->IsPtInToday(pt))
			{
				m_TodayBtnToolTip.SetDescription( CString(Translate(_T("Click it to reset calender and date of today"))));
				m_TodayBtnToolTip.RelayEvent(pMsg);
			}else
			{
				HWND hWnd = pMsg->hwnd ;
				pMsg->hwnd = AfxGetMainWnd()->m_hWnd;
				m_TodayBtnToolTip.RelayEvent(pMsg);
				pMsg->hwnd = hWnd;
			}
		}
		
	}
	this->m_CmdEngine.TransMessage(pMsg);
	return CWnd::PreTranslateMessage(pMsg);
}

void ScheduleCtrl::OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult)
{
/*	
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	COleDateTime starttime,endtime;
	((CDateTimeCtrl *)GetDlgItem(IDC_TIMETABLE_STARTTIME))->GetTime(starttime);
	((CDateTimeCtrl *)GetDlgItem(IDC_TIMETABLE_ENDTIME))->GetTime(endtime);

	COleDateTimeSpan timespan;         // days,hours,minutes,seconds
	timespan.SetDateTimeSpan(90,0,0,0); //retrieve 3-months data
	if(endtime > starttime + timespan)
	{
		AfxMessageBox(Translate(_T("The period between start time and end time must less than 90 days!")));
		UpdateData(FALSE);
		return;
	}
	else if (endtime <= starttime)
	{
		AfxMessageBox(Translate(_T("start time must less than end time!")));
		UpdateData(FALSE);
		return;
	}
	else
	{

		UpdateData(TRUE);
		SetDateInterval(m_datatimectrl1,m_datatimectrl2);
	}
*/
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here

	COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 0, 0, 0);
	CheckDateTime(this, IDC_TIMETABLE_STARTTIME, IDC_TIMETABLE_ENDTIME, date, true);
	*pResult = 0;

	((CDateTimeCtrl *)GetDlgItem(IDC_TIMETABLE_STARTTIME))->GetTime(m_datatimectrl1);
	((CDateTimeCtrl *)GetDlgItem(IDC_TIMETABLE_ENDTIME))->GetTime(m_datatimectrl2);
	m_datatimectrl1.SetDate(m_datatimectrl1.GetYear(),m_datatimectrl1.GetMonth(),m_datatimectrl1.GetDay());
	m_datatimectrl2.SetDate(m_datatimectrl2.GetYear(),m_datatimectrl2.GetMonth(),m_datatimectrl2.GetDay());
	if(m_datatimectrl1 == date)
	{
		SetDateInterval(m_datatimectrl1,m_datatimectrl2);
	}
	
}


void ScheduleCtrl::OnDtnDatetimechangeDatetimepicker2(NMHDR *pNMHDR, LRESULT *pResult)
{
	/*
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	COleDateTime starttime,endtime;
	((CDateTimeCtrl *)GetDlgItem(IDC_TIMETABLE_STARTTIME))->GetTime(starttime);
	((CDateTimeCtrl *)GetDlgItem(IDC_TIMETABLE_ENDTIME))->GetTime(endtime);

	COleDateTimeSpan timespan;         // days,hours,minutes,seconds
	timespan.SetDateTimeSpan(90,0,0,0); //retrieve 3-months data
	if(endtime > starttime + timespan)
	{
		AfxMessageBox(Translate(_T("The period between start time and end time must less than 90 days!")));
		UpdateData(FALSE);
		return;
	}
	else if (endtime <= starttime)
	{
		AfxMessageBox(Translate(_T("start time must less than end time!")));
		UpdateData(FALSE);
		return;
	}
	else
	{
		UpdateData(TRUE);
		SetDateInterval(m_datatimectrl1,m_datatimectrl2);
	}
	*/
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here

	COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 0, 0, 0);
	CheckDateTime(this, IDC_TIMETABLE_STARTTIME, IDC_TIMETABLE_ENDTIME, date, false);
	*pResult = 0;

	((CDateTimeCtrl *)GetDlgItem(IDC_TIMETABLE_STARTTIME))->GetTime(m_datatimectrl1);
	((CDateTimeCtrl *)GetDlgItem(IDC_TIMETABLE_ENDTIME))->GetTime(m_datatimectrl2);
	m_datatimectrl1.SetDate(m_datatimectrl1.GetYear(),m_datatimectrl1.GetMonth(),m_datatimectrl1.GetDay());
	m_datatimectrl2.SetDate(m_datatimectrl2.GetYear(),m_datatimectrl2.GetMonth(),m_datatimectrl2.GetDay());
	if(m_datatimectrl2 == date)
	{
		SetDateInterval(m_datatimectrl1,m_datatimectrl2);
	}
}

static void myCheckDateTime(CWnd* pWnd, int startId, int endId, COleDateTime newDate, bool isStart)
{
	if (isStart)
	{
		if (LOBYTE(LOWORD(GetVersion())) < 6) // Win XP and below only
		{
			static bool isSecondTime = false;
			// We are changing the date in the Month Control, we only handle the second notification
			if (NULL != DateTime_GetMonthCal(pWnd->GetDlgItem(startId)->GetSafeHwnd()))
			{
				if (!isSecondTime)
				{
					isSecondTime = !isSecondTime;
					return;
				}
				else
				{
					isSecondTime = !isSecondTime;
				}
			}
			else
			{
				isSecondTime = false;
			}
		}

		COleDateTime end;
		end = ((CBORROW_DateTimeCtrl*)pWnd->GetDlgItem(endId))->GetDate();

		end.SetDateTime(end.GetYear(), end.GetMonth(), end.GetDay(),23, 59, 59);
		if (end < newDate)
		{
			ShowXMessageBox(pWnd->GetSafeHwnd(),Translate(_T("Can't set start date after end date")), Translate(_T("Error: Check datetime")), MB_OK|MB_ICONERROR);
			//pWnd->MessageBox(Translate(_T("Can't set start date after end date")), Translate(_T("Error: Check datetime")), MB_OK|MB_ICONERROR);
			newDate.SetDateTime(end.GetYear(), end.GetMonth(), end.GetDay(), 0, 0, 0);
			((CBORROW_DateTimeCtrl*)pWnd->GetDlgItem(startId))->SetDate(newDate);
		}
		else
		{
			((CBORROW_DateTimeCtrl*)pWnd->GetDlgItem(startId))->SetDate(newDate);
		}
	}
	else
	{
		if (LOBYTE(LOWORD(GetVersion())) < 6) // Win XP and below only
		{
			static bool isSecondTime = false;
			// We are changing the date in the Month Control, we only handle the second notification
			if (NULL != DateTime_GetMonthCal(pWnd->GetDlgItem(endId)->GetSafeHwnd()))
			{
				if (!isSecondTime)
				{
					isSecondTime = !isSecondTime;
					return;
				}
				else
				{
					isSecondTime = !isSecondTime;
				}
			}
			else
			{
				isSecondTime = false;
			}
		}

		//         LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
		//         COleDateTime end(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
		COleDateTime start;
		start = ((CBORROW_DateTimeCtrl*)pWnd->GetDlgItem(startId))->GetDate();

		start.SetDateTime(start.GetYear(), start.GetMonth(), start.GetDay(), 0, 0, 0);

		if (newDate < start)
		{
			ShowXMessageBox(pWnd->GetSafeHwnd(), Translate(_T("Can't set end date before start date")), Translate(_T("Error:Check datetime")), MB_OK|MB_ICONERROR);
			//pWnd->MessageBox(Translate(_T("Can't set end date before start date")), Translate(_T("Error:Check datetime")), MB_OK|MB_ICONERROR);
			newDate.SetDateTime(start.GetYear(), start.GetMonth(), start.GetDay(), 23, 59, 59);
			((CBORROW_DateTimeCtrl*)pWnd->GetDlgItem(endId))->SetDate(newDate);
		}
		else
		{
			((CBORROW_DateTimeCtrl*)pWnd->GetDlgItem(endId))->SetDate(newDate);
		}
	}
}

void ScheduleCtrl::OnDatetimechangeDatetimepicker1()
{
	m_datatimectrl1 = ((CBORROW_DateTimeCtrl *)GetDlgItem(IDC_TIMETABLE_STARTTIME))->GetDate();
	//m_datatimectrl2 = ((CBORROW_DateTimeCtrl *)GetDlgItem(IDC_TIMETABLE_ENDTIME))->GetDate();

	myCheckDateTime(this, IDC_TIMETABLE_STARTTIME, IDC_TIMETABLE_ENDTIME, m_datatimectrl1, true);

	m_datatimectrl1 = ((CBORROW_DateTimeCtrl *)GetDlgItem(IDC_TIMETABLE_STARTTIME))->GetDate();
	m_datatimectrl2 = ((CBORROW_DateTimeCtrl *)GetDlgItem(IDC_TIMETABLE_ENDTIME))->GetDate();

	m_datatimectrl1.SetDate(m_datatimectrl1.GetYear(),m_datatimectrl1.GetMonth(),m_datatimectrl1.GetDay());
	m_datatimectrl2.SetDate(m_datatimectrl2.GetYear(),m_datatimectrl2.GetMonth(),m_datatimectrl2.GetDay());
	//if(m_datatimectrl1 == date)
	{
		SetDateInterval(m_datatimectrl1,m_datatimectrl2);
	}
	
}


void ScheduleCtrl::OnDatetimechangeDatetimepicker2()
{
	//m_datatimectrl1 = ((CBORROW_DateTimeCtrl *)GetDlgItem(IDC_TIMETABLE_STARTTIME))->GetDate();
	m_datatimectrl2 = ((CBORROW_DateTimeCtrl *)GetDlgItem(IDC_TIMETABLE_ENDTIME))->GetDate(); 

	myCheckDateTime(this, IDC_TIMETABLE_STARTTIME, IDC_TIMETABLE_ENDTIME, m_datatimectrl2, false);

	m_datatimectrl1 = ((CBORROW_DateTimeCtrl *)GetDlgItem(IDC_TIMETABLE_STARTTIME))->GetDate();
	m_datatimectrl2 = ((CBORROW_DateTimeCtrl *)GetDlgItem(IDC_TIMETABLE_ENDTIME))->GetDate(); 

	m_datatimectrl1.SetDate(m_datatimectrl1.GetYear(),m_datatimectrl1.GetMonth(),m_datatimectrl1.GetDay());
	m_datatimectrl2.SetDate(m_datatimectrl2.GetYear(),m_datatimectrl2.GetMonth(),m_datatimectrl2.GetDay());
	//if(m_datatimectrl2 == date)
	{	
		SetDateInterval(m_datatimectrl1,m_datatimectrl2);
	}
}

COleDateTime ScheduleCtrl::GetStartTime()
{
	return 	m_datatimectrl1;
}

COleDateTime ScheduleCtrl::GetEndTime()
{

	return m_datatimectrl2;
}

