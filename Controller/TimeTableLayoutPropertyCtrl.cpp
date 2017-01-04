// TimeTableLayoutPropertyCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "TimeTableLayoutPropertyCtrl.h"
#include "afxdialogex.h"
#include "Localization/Localization.h"

UINT TIMETABLE_ITEM_PROPERTY_CHANGE = ::RegisterWindowMessage (_T("TIMETABLE_ITEM_PROPERTY_CHANGE"));


// CTimeTableLayoutPropertyCtrl dialog

IMPLEMENT_DYNAMIC(CTimeTableLayoutPropertyCtrl, CDialogEx)

CTimeTableLayoutPropertyCtrl::CTimeTableLayoutPropertyCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTimeTableLayoutPropertyCtrl::IDD, pParent)
	, m_fLoopTimes(1.0)
{
	
}

CTimeTableLayoutPropertyCtrl::~CTimeTableLayoutPropertyCtrl()
{
}

void CTimeTableLayoutPropertyCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_TIMETABLE_LAYOUT_NAME, m_LayoutName);
    DDX_Text(pDX, IDC_EDIT_TIMETABLE_LAYOUT_CATEGORY, m_Category);
    DDX_DateTimeCtrl(pDX, IDC_EDIT_TIMETABLE_LAYOUT_DURATION_TIME, m_DurationTime);
    DDX_Text(pDX, IDC_EDIT_TIMETABLE_LAYOUT_LOOPTIMES, m_fLoopTimes);
}


BEGIN_MESSAGE_MAP(CTimeTableLayoutPropertyCtrl, CDialogEx)
	 ON_WM_SIZE()
     ON_NOTIFY(DTN_DATETIMECHANGE, IDC_EDIT_TIMETABLE_LAYOUT_DURATION_TIME, &CTimeTableLayoutPropertyCtrl::OnDtnDatetimechangeEditTimetableLayoutDurationTime)
     ON_EN_CHANGE(IDC_EDIT_TIMETABLE_LAYOUT_LOOPTIMES, &CTimeTableLayoutPropertyCtrl::OnEnChangeEditTimetableLayoutLooptimes)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CTimeTableLayoutPropertyCtrl)
	EASYSIZE(IDC_EDIT_TIMETABLE_LAYOUT_NAME,IDC_STATIC_TIMETABLE_LAYOUT_NAME,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_EDIT_TIMETABLE_LAYOUT_CATEGORY,IDC_STATIC_TIMETABLE_LAYOUT_CATEGORY,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_EDIT_TIMETABLE_LAYOUT_DURATION_TIME,IDC_STATIC_TIMETABLE_LAYOUT_DURATION_TIME,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_EDIT_TIMETABLE_LAYOUT_LOOPTIMES,IDC_STATIC_TIMETABLE_LAYOUT_LOOPTIMES,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
END_EASYSIZE_MAP


// CTimeTableLayoutPropertyCtrl message handlers
BOOL CTimeTableLayoutPropertyCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    m_dpi.Attach(AfxFindResourceHandle(MAKEINTRESOURCE(IDD), RT_DIALOG), m_hWnd, IDD);

	GetDlgItem(IDC_EDIT_TIMETABLE_LAYOUT_DURATION_TIME)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_TIMETABLE_LAYOUT_LOOPTIMES)->EnableWindow(false);

    ((CDateTimeCtrl*)GetDlgItem(IDC_EDIT_TIMETABLE_LAYOUT_DURATION_TIME))->SetFormat(_T("HH:mm:ss"));

	INIT_EASYSIZE;
	m_LoopEdit.SubclassWindow(this->GetDlgItem(IDC_EDIT_TIMETABLE_LAYOUT_LOOPTIMES)->GetSafeHwnd());
	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CTimeTableLayoutPropertyCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}


void CTimeTableLayoutPropertyCtrl::OnDtnDatetimechangeEditTimetableLayoutDurationTime(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    UpdateData(TRUE);

    COleDateTimeSpan Span = m_DurationTime;

    m_fLoopTimes = Span.GetTotalSeconds()/(float)m_OrigDuration.GetTotalSeconds();

    UpdateData(FALSE);

    GetParent()->SendMessage(TIMETABLE_ITEM_PROPERTY_CHANGE);
}


void CTimeTableLayoutPropertyCtrl::OnEnChangeEditTimetableLayoutLooptimes()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here.

    UpdateData(TRUE);

    COleDateTimeSpan LoopSpan = m_fLoopTimes*m_OrigDuration;

    FLOAT CurLoopTimes = m_DurationTime / m_OrigDuration;

    if (CurLoopTimes != m_fLoopTimes)
    {         
        m_DurationTime = LoopSpan;  
        GetParent()->SendMessage(TIMETABLE_ITEM_PROPERTY_CHANGE);  

        UpdateData(FALSE);
    }
}



void CTimeTableLayoutPropertyCtrl::ClearItem()
{
    m_App.SetDetailID(-1);
    m_LayoutName = _T("");
    m_Category   = _T("");
    m_DurationTime = COleDateTime();
    m_fLoopTimes = 1.0;

    UpdateData(FALSE);
}

 BOOL CTimeTableLayoutPropertyCtrl::PreTranslateMessage(MSG *pMsg)
 {
	 /* if(GetFocus() == GetDlgItem(IDC_EDIT_TIMETABLE_LAYOUT_LOOPTIMES))
	 {
	 if(pMsg->message == WM_KEYDOWN)  
	 {
	 if( isdigit(pMsg->wParam))
	 {
	 ;
	 }
	 else if( pMsg->wParam >= 0x00 && pMsg->wParam< 0x30)
	 {
	 ;
	 }
	 else if (pMsg->wParam >= VK_NUMPAD0 && pMsg->wParam <= VK_NUMPAD9 )
	 {
	 ;
	 }
	 else
	 {
	 CWnd * curFocus = GetFocus();
	 MessageBox(Translate(_T("Please input integer!")),Translate(_T("Warning:Check value")), MB_OK|MB_ICONEXCLAMATION);
	 curFocus->SetFocus();
	 return TRUE;
	 }
	 }
	 }*/
	 return __super::PreTranslateMessage(pMsg);
 }
void CTimeTableLayoutPropertyCtrl::SetItem(Appointment* pApp)
{
	if(pApp == NULL)
	{
		GetDlgItem(IDC_EDIT_TIMETABLE_LAYOUT_NAME)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_TIMETABLE_LAYOUT_CATEGORY)->SetWindowText(_T(""));

		GetDlgItem(IDC_EDIT_TIMETABLE_LAYOUT_DURATION_TIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_TIMETABLE_LAYOUT_LOOPTIMES)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_TIMETABLE_LAYOUT_LOOPTIMES)->SetWindowText(_T("0"));
		GetDlgItem(IDC_EDIT_TIMETABLE_LAYOUT_DURATION_TIME)->SetWindowText(_T("1"));
		return;
	}
	else
	{
		GetDlgItem(IDC_EDIT_TIMETABLE_LAYOUT_DURATION_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_TIMETABLE_LAYOUT_LOOPTIMES)->EnableWindow(TRUE);
	}
	

    m_App = *pApp;
    Layout layout;
    HRESULT hr = GetControllerInterface->GetLayout(m_App.GetDetailID(), layout);
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to get layout %d", m_App.GetDetailID());
        return;
    }

    int nSeconds = layout.GetDuration();
    m_OrigDuration = COleDateTimeSpan(0, nSeconds/3600, (nSeconds%3600)/60, nSeconds%60);
    m_LayoutName = layout.GetLayoutName().c_str();

    LayoutGroup group;
    hr = GetControllerInterface->GetLayoutFilter(layout.GetGroupID(), group);
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to get layout group %d", layout.GetGroupID());
        return;
    }

    m_Category   = group.GetGroupName().c_str();
    m_DurationTime = m_App.GetDuration();

    m_fLoopTimes = (float)( (int)(100 * m_App.GetDuration().GetTotalSeconds()/(float)nSeconds))/(float)100;
    UpdateData(FALSE);

    CPoint pos = m_LoopEdit.GetCaretPos();//save caret pos .
    CWnd *pFocusWnd = CWnd::GetFocus();//save focus wnd.
    CWnd *pCaptured = CWnd::GetCapture();//save captured window

    if(pFocusWnd && !pCaptured )//reset edit text and restore caret point.
    {
	        GetDlgItem(IDC_EDIT_TIMETABLE_LAYOUT_DURATION_TIME)->EnableWindow(true);//this option will change captured status of window.
	        GetDlgItem(IDC_EDIT_TIMETABLE_LAYOUT_LOOPTIMES)->EnableWindow(true);
            //simulate mouse click event,to restore keyboard input location.

            m_LoopEdit.SendMessage(WM_LBUTTONDOWN ,(WPARAM)MK_LBUTTON ,(LPARAM)MAKELPARAM(pos.x,pos.y));
            m_LoopEdit.SendMessage(WM_LBUTTONUP ,(WPARAM)MK_LBUTTON,(LPARAM)MAKELPARAM(pos.x,pos.y));

            pFocusWnd->SetFocus();//restore focus.
    }
    if(pCaptured)
       pCaptured->SetCapture();
}