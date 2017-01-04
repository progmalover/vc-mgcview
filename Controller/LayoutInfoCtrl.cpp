// LayoutInfoCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "LayoutInfoCtrl.h"
#include "Localization/Localization.h"
#include "LayoutSettings.h"
#include "afxdialogex.h"
#include "RejectReason.h"


// CLayoutInfoCtrl dialog

IMPLEMENT_DYNAMIC(CLayoutInfoCtrl, CDialogEx)

CLayoutInfoCtrl::CLayoutInfoCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLayoutInfoCtrl::IDD, pParent)
{

}

CLayoutInfoCtrl::~CLayoutInfoCtrl()
{
}

// CLayoutInfoCtrl message handlers


void CLayoutInfoCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MEDIA_INFO_DESC, m_mediaInfo);
    DDX_Control(pDX, IDC_MEDIA_THUMBNAIL, m_thumbnail);
    DDX_Control(pDX, IDC_BTN_REJECT_REASON, m_ctrlRejectReason);
}


BEGIN_MESSAGE_MAP(CLayoutInfoCtrl, CDialogEx)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BTN_REJECT_REASON, &OnBnClickedBtnRejectReason)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CLayoutInfoCtrl)

  EASYSIZE(IDC_MEDIA_THUMBNAIL, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE,0)
  //EASYSIZE(IDC_STATIC_GROUP_INFO, IDC_STATIC_THUMBNAIL, ES_KEEPSIZE, ES_BORDER, ES_BORDER, 0)
  EASYSIZE(IDC_MEDIA_INFO_DESC, IDC_MEDIA_THUMBNAIL, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP


// CMediaLibraryInfoCtrl message handlers
BOOL CLayoutInfoCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    CRect ThumbnailRect;
    m_thumbnail.GetWindowRect(&ThumbnailRect);
    m_ThumbnailImage.Create(ThumbnailRect.Width(), ThumbnailRect.Height(), 32);

    m_ctrlRejectReason.ShowWindow(SW_HIDE);

    INIT_EASYSIZE;

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CLayoutInfoCtrl::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;
    if (m_thumbnail.GetSafeHwnd())
    {
        m_thumbnail.GetWindowRect(m_thumbRect);
    }
}


BOOL CLayoutInfoCtrl::SetMediaInfo(const Layout& LayoutInfo)
{
    InfoDisplayItem InfoItem;
    m_mediaInfo.ClearInfo();

    InfoItem.InfoName = Translate(_T("Layout Name: "));
    InfoItem.InfoValue = LayoutInfo.GetLayoutName().c_str();
    m_mediaInfo.AddInfo(InfoItem);


    InfoItem.InfoName = Translate(_T("Width: "));
	InfoItem.InfoValue.Format(_T("%d"), LayoutInfo.GetWidth());
    m_mediaInfo.AddInfo(InfoItem);

    InfoItem.InfoName = Translate(_T("Height: "));
	InfoItem.InfoValue.Format(_T("%d"), LayoutInfo.GetHeight());
    m_mediaInfo.AddInfo(InfoItem);

    InfoItem.InfoName = Translate(_T("Description: "));
	InfoItem.InfoValue = LayoutInfo.GetDescription().c_str();
    m_mediaInfo.AddInfo(InfoItem);

#ifndef STARTER_EDITION
	InfoItem.InfoName = Translate(_T("Size: "));
	//InfoItem.InfoValue = LayoutInfo.GetCustomizedSetting((LPCTSTR)szLayoutSize).c_str();
	CString layoutsize;
	layoutsize.Format(_T("%lld"), LayoutInfo.GetLayoutSize());
	DWORD64 dwValue = 0;
	for(int ii = 0; ii < layoutsize.GetLength(); ii++)
	{
		dwValue *= 10;
		if(layoutsize[ii] >= 0x30 && layoutsize[ii] <= 0x39)
		{
			dwValue += (layoutsize[ii]-0x30);
		}
	}

	double num1 = dwValue * 1.0;
	if(dwValue > 0)
	{
		num1 /= (1024*1024);
	}
	num1 += 1;
	layoutsize.Format(_T("%.1f Mb"),num1);
	InfoItem.InfoValue = layoutsize;
	m_mediaInfo.AddInfo(InfoItem);
#endif

	CString tmp = _T("");
    InfoItem.InfoName = Translate(_T("Duration: "));
    COleDateTimeSpan tmpDur;
    tmpDur.SetDateTimeSpan(0,0,0,LayoutInfo.GetDuration());
    tmp = tmpDur.Format(_T("%H:%M:%S"));
    InfoItem.InfoValue = tmp;
    m_mediaInfo.AddInfo(InfoItem);

#ifdef SERVER_EDITION
    InfoItem.InfoName = Translate(_T("Owner user: "));
    InfoItem.InfoValue = LayoutInfo.GetOwner().c_str();
    m_mediaInfo.AddInfo(InfoItem);
#endif

    InfoItem.InfoName = Translate(_T("Create Time: "));
    InfoItem.InfoValue = LayoutInfo.GetCreateTime().c_str();
    m_mediaInfo.AddInfo(InfoItem);

    m_mediaInfo.AddAuditInfo(LayoutInfo.GetAudit());

    if (LayoutInfo.GetAudit().GetStatus() == Audit::REJECTED)
    {
        CRect ThumbnailRect = m_thumbRect;
        ScreenToClient(&ThumbnailRect);

        CRect BtnRect = ThumbnailRect;
        BtnRect.DeflateRect(0, ThumbnailRect.Height() * 3 / 4, 0, 0);
        ThumbnailRect.DeflateRect(0, 0, 0, ThumbnailRect.Height() / 4);
        m_thumbnail.MoveWindow(ThumbnailRect);
        m_ctrlRejectReason.MoveWindow(BtnRect);

        m_ctrlRejectReason.ShowWindow(SW_SHOW);

        m_szRejectReason = LayoutInfo.GetAudit().GetComment().c_str();
    }
    else
    {
        CRect ThumbnailRect = m_thumbRect;
        ScreenToClient(&ThumbnailRect);

        CRect InfoRect;
        m_mediaInfo.GetWindowRect(&InfoRect);
        ScreenToClient(&InfoRect);

        ThumbnailRect.top = InfoRect.top;
        ThumbnailRect.bottom = InfoRect.bottom;

        m_thumbnail.MoveWindow(ThumbnailRect);
        m_ctrlRejectReason.MoveWindow(ThumbnailRect);

        m_ctrlRejectReason.ShowWindow(SW_HIDE);

        m_szRejectReason = _T("");
    }

    UpdateData(FALSE);

    return TRUE;
}


BOOL CLayoutInfoCtrl::SetMediaThumbnail(const CImage& Thumbnail)
{
    if (Thumbnail.IsNull())
    {
        m_thumbnail.SetBitmap(NULL);
        return TRUE;
    }


    HDC hDstDC = NULL;

    CRect rc;
    // Obtain a DC to the destination image
    hDstDC = m_ThumbnailImage.GetDC();

    rc.left = 0;
    rc.top = 0;
    rc.right = m_ThumbnailImage.GetWidth();
    rc.bottom = m_ThumbnailImage.GetHeight();

    HBRUSH hSolidBrush = CreateSolidBrush(RGB(255,255,255));
    FillRect(hDstDC, &rc, hSolidBrush);
    DeleteObject(hSolidBrush);
    int OldStretchMode = GetStretchBltMode (hDstDC);
    SetStretchBltMode( hDstDC,  HALFTONE );

    FLOAT W2H = ((FLOAT)Thumbnail.GetWidth() / (FLOAT)Thumbnail.GetHeight());
    FLOAT w2h = ((FLOAT)rc.Width() / (FLOAT)rc.Height());
    if ( W2H > w2h)
    {
        INT DeflactY = (rc.Height() - rc.Width()/W2H) / 2;
        rc.DeflateRect(0, DeflactY, 0, DeflactY);
    }
    else
    {
        INT DeflactX = (rc.Width() - rc.Height()*W2H) / 2;
        rc.DeflateRect(DeflactX, 0, DeflactX, 0);
    }

    //Thumbnail.SetTransparentColor(RGB(123,456,789));
    Thumbnail.StretchBlt(hDstDC, rc);
    SetStretchBltMode(hDstDC, OldStretchMode);

    // Release the destination DC
    m_ThumbnailImage.ReleaseDC();
    m_ThumbnailImage.SetHasAlphaChannel(TRUE);
    m_ThumbnailImage.SetTransparentColor(RGB(0,0,0));

    m_thumbnail.SetBitmap(m_ThumbnailImage);

    m_thumbnail.ShowWindow(SW_SHOW);
    UpdateData(FALSE);

    return TRUE;
}

BOOL CLayoutInfoCtrl::SetCategoryInfo(const LayoutGroup& CategoryInfo)
{
    m_thumbnail.ShowWindow(SW_HIDE);

    CString szInfo;

    szInfo = CategoryInfo.GetGroupName().c_str();

    m_mediaInfo.SetWindowText(szInfo);
    UpdateData(FALSE);

    return TRUE;
}

BOOL  CLayoutInfoCtrl::ClearInfo()
{
    m_thumbnail.SetBitmap(NULL);

    m_mediaInfo.ClearInfo();
    m_thumbnail.ShowWindow(SW_HIDE);
    m_ctrlRejectReason.ShowWindow(SW_HIDE);
    UpdateData(FALSE);

    return TRUE;
}

void CLayoutInfoCtrl::OnBnClickedBtnRejectReason()
{
    // TODO: Add your control notification handler code here

    CRejectReason dlg(this, TRUE, m_szRejectReason);

    if (IDOK == dlg.DoModal())
    {
    }
}