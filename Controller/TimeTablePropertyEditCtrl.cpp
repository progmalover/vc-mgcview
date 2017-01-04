// TimeTablePropertyEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "TimeTablePropertyEditCtrl.h"
#include "afxdialogex.h"
#include "Localization/Localization.h"
#include "RejectReason.h"


// CTimeTablePropertyEditCtrl dialog

IMPLEMENT_DYNAMIC(CTimeTablePropertyEditCtrl, CDialogEx)

CTimeTablePropertyEditCtrl::CTimeTablePropertyEditCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTimeTablePropertyEditCtrl::IDD, pParent)
{

}

CTimeTablePropertyEditCtrl::~CTimeTablePropertyEditCtrl()
{
}

void CTimeTablePropertyEditCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TIMETABLE_INFO, m_Info);
    DDX_Control(pDX, IDC_BTN_REJECT_REASON, m_ctrlRejectReason);
}


BEGIN_MESSAGE_MAP(CTimeTablePropertyEditCtrl, CDialogEx)
	 ON_WM_SIZE()
     ON_BN_CLICKED(IDC_BTN_REJECT_REASON, &CTimeTablePropertyEditCtrl::OnBnClickedBtnRejectReason)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CTimeTablePropertyEditCtrl)
    EASYSIZE(IDC_TIMETABLE_INFO,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
END_EASYSIZE_MAP


// CTimeTablePropertyEditCtrl message handlers
BOOL CTimeTablePropertyEditCtrl::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_ctrlRejectReason.ShowWindow(SW_HIDE);

	INIT_EASYSIZE;

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CTimeTablePropertyEditCtrl::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;

    if (m_ctrlRejectReason.GetSafeHwnd())
    {
        if (m_ctrlRejectReason.IsWindowVisible())
        {
            CRect ThumbnailRect;
            m_Info.GetWindowRect(&ThumbnailRect);
            ScreenToClient(&ThumbnailRect);

            CRect BtnRect;
            m_ctrlRejectReason.GetWindowRect(&BtnRect);
            ScreenToClient(&BtnRect);
            ThumbnailRect.top = BtnRect.bottom;

            m_Info.MoveWindow(ThumbnailRect);
        }
        else
        {
            CRect ThumbnailRect;
            m_Info.GetWindowRect(&ThumbnailRect);
            ScreenToClient(&ThumbnailRect);

            CRect BtnRect;
            m_ctrlRejectReason.GetWindowRect(&BtnRect);
            ScreenToClient(&BtnRect);
            ThumbnailRect.top = BtnRect.top;

            m_Info.MoveWindow(ThumbnailRect);
        }
    }
}



void CTimeTablePropertyEditCtrl::SetTimeTable(const Timetable& Table)
{
    InfoDisplayItem InfoItem;
    m_Info.ClearInfo();

    InfoItem.InfoName = Translate(_T("Timetable Name: "));
    InfoItem.InfoValue = Table.GetTimetableName().c_str();
    m_Info.AddInfo(InfoItem);


    int UserID = Table.GetCreateUserID();


    User Creator;
    HRESULT hr = GetControllerInterface->GetUserInfo(UserID, Creator, true);
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to get user info %d", UserID);
        return;
    }
#ifdef SERVER_EDITION
    InfoItem.InfoName = Translate(_T("Creator: "));
    InfoItem.InfoValue = Creator.GetAccount().GetUserNameW().c_str();
    m_Info.AddInfo(InfoItem);
#endif
    m_Info.AddAuditInfo(Table.GetAudit());

    if (Table.GetAudit().GetStatus() == Audit::REJECTED)
    {
        CRect ThumbnailRect;
        m_Info.GetWindowRect(&ThumbnailRect);
        ScreenToClient(&ThumbnailRect);

        CRect BtnRect;
        m_ctrlRejectReason.GetWindowRect(&BtnRect);
        ScreenToClient(&BtnRect);
        ThumbnailRect.top = BtnRect.bottom;

        m_Info.MoveWindow(ThumbnailRect);

        m_ctrlRejectReason.ShowWindow(SW_SHOW);

        m_szRejectReason = Table.GetAudit().GetComment().c_str();
    }
    else
    {
        CRect ThumbnailRect;
        m_Info.GetWindowRect(&ThumbnailRect);
        ScreenToClient(&ThumbnailRect);

        CRect BtnRect;
        m_ctrlRejectReason.GetWindowRect(&BtnRect);
        ScreenToClient(&BtnRect);
        ThumbnailRect.top = BtnRect.top;

        m_Info.MoveWindow(ThumbnailRect);

        m_ctrlRejectReason.ShowWindow(SW_HIDE);

        m_szRejectReason = _T("");
    }

    UpdateData(FALSE);

}

void CTimeTablePropertyEditCtrl::ClearTimeTable()
{
    m_Info.ClearInfo();
    m_ctrlRejectReason.ShowWindow(SW_HIDE);
    UpdateData(FALSE);
}

void CTimeTablePropertyEditCtrl::OnCancel()
{
}


void CTimeTablePropertyEditCtrl::OnOK()
{
}


void CTimeTablePropertyEditCtrl::OnBnClickedBtnRejectReason()
{
    // TODO: Add your control notification handler code here
    CRejectReason dlg(this, TRUE, m_szRejectReason);

    if (IDOK == dlg.DoModal())
    {
    }
}
