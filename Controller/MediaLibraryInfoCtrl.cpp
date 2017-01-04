// MediaLibraryInfoCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "MediaLibraryInfoCtrl.h"
#include "Localization/Localization.h"
#include "RejectReason.h"
#include "MediaLibraryPage.h"
// CMediaLibraryInfoCtrl dialog

IMPLEMENT_DYNAMIC(CMediaLibraryInfoCtrl, CDialogEx)

CMediaLibraryInfoCtrl::CMediaLibraryInfoCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMediaLibraryInfoCtrl::IDD, pParent)
{

}

CMediaLibraryInfoCtrl::~CMediaLibraryInfoCtrl()
{
}

void CMediaLibraryInfoCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MEDIA_INFO_DESC, m_mediaInfo);
    DDX_Control(pDX, IDC_MEDIA_THUMBNAIL, m_thumbnail);
    DDX_Control(pDX, IDC_BTN_REJECT_REASON, m_ctrlRejectReason);
}


BEGIN_MESSAGE_MAP(CMediaLibraryInfoCtrl, CDialogEx)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BTN_REJECT_REASON, &CMediaLibraryInfoCtrl::OnBnClickedBtnRejectReason)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CMediaLibraryInfoCtrl)

  EASYSIZE(IDC_MEDIA_THUMBNAIL, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE,0)
  //EASYSIZE(IDC_STATIC_GROUP_INFO, IDC_STATIC_THUMBNAIL, ES_KEEPSIZE, ES_BORDER, ES_BORDER, 0)
  EASYSIZE(IDC_MEDIA_INFO_DESC, IDC_MEDIA_THUMBNAIL, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP


// CMediaLibraryInfoCtrl message handlers
BOOL CMediaLibraryInfoCtrl::OnInitDialog()
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


void CMediaLibraryInfoCtrl::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;
    if (m_thumbnail.GetSafeHwnd())
    {
        m_thumbnail.GetWindowRect(&m_thumbRect);
    }
}



BOOL CMediaLibraryInfoCtrl::SetMediaInfo(const Media& MediaInfo)
{
    InfoDisplayItem InfoItem;
    m_mediaInfo.ClearInfo();

    InfoItem.InfoName = Translate(_T("Media Name: "));
    InfoItem.InfoValue = MediaInfo.GetName().c_str();
    m_mediaInfo.AddInfo(InfoItem);

    /*
    InfoItem.InfoName = Translate(_T("Size: "));
    InfoItem.InfoValue = FormatSizeString(MediaInfo.GetFileSize());
    m_mediaInfo.AddInfo(InfoItem);
    */
    InfoItem.InfoName = Translate(_T("Type: "));
    InfoItem.InfoValue = MediaInfo.GetMediaType().c_str();
    m_mediaInfo.AddInfo(InfoItem);


    InfoItem.InfoName = Translate(_T("Width: "));
    InfoItem.InfoValue.Format(_T("%d "), MediaInfo.GetWidth());
    m_mediaInfo.AddInfo(InfoItem);


    InfoItem.InfoName = Translate(_T("Height: "));
    InfoItem.InfoValue.Format(_T("%d"), MediaInfo.GetHeight()); 
    m_mediaInfo.AddInfo(InfoItem);


    //InfoItem.InfoName = Translate(_T("Category: "));
    //InfoItem.InfoValue = MediaInfo.GetMediaFilter().GetMediaFilterName().c_str();
    //m_mediaInfo.AddInfo(InfoItem);

	CMediaLibraryPage *pMediaLibPage = (CMediaLibraryPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CMediaLibraryPage));
	InfoItem.InfoName = Translate(_T("Category: "));
	InfoItem.InfoValue.Empty();
	pMediaLibPage->GetCategoryFullPath(MediaInfo.GetMediaFilter().GetMediaFilterID(),InfoItem.InfoValue);//MediaInfo.GetMediaFilter().GetMediaFilterName().c_str();
	m_mediaInfo.AddInfo(InfoItem);

/*
    InfoItem.InfoName = Translate(_T("MediaServerPath: "));
    InfoItem.InfoValue = MediaInfo.GetMediaServerPath().c_str();
    m_mediaInfo.AddInfo(InfoItem);

    mediaInfo += Translate(_T("Desc: "));
    mediaInfo += MediaInfo.GetDesc().c_str();
    mediaInfo += _T("\t");
  */

    InfoItem.InfoName = Translate(_T("Duration: "));
    COleDateTimeSpan tmpDur;
    tmpDur.SetDateTimeSpan(0,0,0,MediaInfo.GetDuration() / 1000);
    InfoItem.InfoValue = tmpDur.Format(_T("%H:%M:%S"));
    m_mediaInfo.AddInfo(InfoItem);


    User Creator;
    HRESULT hr = GetControllerInterface->GetUserInfo(MediaInfo.GetUploadUserID(), Creator);


    InfoItem.InfoName = Translate(_T("Upload User: "));
    InfoItem.InfoValue =  Creator.GetAccount().GetUserNameW().c_str();
    m_mediaInfo.AddInfo(InfoItem);



    InfoItem.InfoName = Translate(_T("Upload Time: "));
    if (MediaInfo.GetUploadTime() == S3Time::NULLTime)
    {
        InfoItem.InfoName  = Translate(_T("Upload Status: "));
        InfoItem.InfoValue = Translate(_T("Uploading"));
    }
    else
    {
        InfoItem.InfoValue = MediaInfo.GetUploadTime().ToString().c_str();
    }
    m_mediaInfo.AddInfo(InfoItem);

/*
    InfoItem.InfoName = Translate(_T("DelUserID: "));
    InfoItem.InfoValue.Format(_T("%d"), MediaInfo.GetDelUserID());
    m_mediaInfo.AddInfo(InfoItem);


    InfoItem.InfoName = Translate(_T("Delete Time: "));
    InfoItem.InfoValue = MediaInfo.GetDelTime().ToString().c_str();
    m_mediaInfo.AddInfo(InfoItem);
*/

    m_mediaInfo.AddAuditInfo(MediaInfo.GetAudit());

    if (MediaInfo.GetAudit().GetStatus() == Audit::REJECTED)
    {
        CRect ThumbnailRect = m_thumbRect;
        ScreenToClient(&ThumbnailRect);

        CRect BtnRect = ThumbnailRect;
        BtnRect.DeflateRect(0, ThumbnailRect.Height() * 3 / 4, 0, 0);
        ThumbnailRect.DeflateRect(0, 0, 0, ThumbnailRect.Height() / 4);
     //   m_thumbnail.MoveWindow(ThumbnailRect);
        m_ctrlRejectReason.MoveWindow(BtnRect);

        m_ctrlRejectReason.ShowWindow(SW_SHOW);

        m_szRejectReason = MediaInfo.GetAudit().GetComment().c_str();
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

       // m_thumbnail.MoveWindow(ThumbnailRect);
        m_ctrlRejectReason.MoveWindow(ThumbnailRect);

        m_ctrlRejectReason.ShowWindow(SW_HIDE);

        m_szRejectReason = _T("");
    }


    return TRUE;
}

BOOL CMediaLibraryInfoCtrl::SetMediaInfo(const TASK_DETAIL_INFO& MediaInfo)
{
    m_thumbnail.ShowWindow(SW_SHOW);

    return FALSE;
}
BOOL CMediaLibraryInfoCtrl::SetMediaThumbnail(const CImage& Thumbnail)
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

    m_thumbnail.SetBitmap(m_ThumbnailImage);

    m_thumbnail.ShowWindow(SW_SHOW);
    UpdateData(FALSE);

    return TRUE;
}

BOOL CMediaLibraryInfoCtrl::SetCategoryInfo(const MediaFilter& CategoryInfo)
{
    m_thumbnail.ShowWindow(SW_HIDE);

    CString szInfo;

    szInfo = CategoryInfo.GetMediaFilterName().c_str();

    m_mediaInfo.SetWindowText(szInfo);
    UpdateData(FALSE);

    return TRUE;
}

BOOL  CMediaLibraryInfoCtrl::ClearInfo()
{
    m_thumbnail.SetBitmap(NULL);

    m_mediaInfo.ClearInfo();
    m_thumbnail.ShowWindow(SW_SHOW);
    m_ctrlRejectReason.ShowWindow(SW_HIDE);
    UpdateData(FALSE);


    return TRUE;
}


void CMediaLibraryInfoCtrl::OnBnClickedBtnRejectReason()
{
    // TODO: Add your control notification handler code here

    CRejectReason dlg(this, TRUE, m_szRejectReason);

    if (IDOK == dlg.DoModal())
    {
    }
}