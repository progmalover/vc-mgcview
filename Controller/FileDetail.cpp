// FileDetail.cpp : 实现文件
//

#include "stdafx.h"
#include "Controller.h"
#include "FileDetail.h"
#include "afxdialogex.h"
#include "MediaInfo/MediaInfo.h"
#include "Localization/Localization.h"

// CFileDetail 对话框

IMPLEMENT_DYNAMIC(CFileDetail, CDialogEx)

CFileDetail::CFileDetail(const Media& MediaInfo, CWnd* pParent /*=NULL*/)
	: CDialogEx(CFileDetail::IDD, pParent)
{
    m_MediaInfoStruct.push_back(MediaInfo);
}

CFileDetail::~CFileDetail()
{
}

void CFileDetail::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MFCPROPERTYGRID_DETAIL, m_mediaInfo);
    DDX_Control(pDX, IDC_MEDIA_TUMBNAIL, m_MediaTumbnailCtrl);
    DDX_Control(pDX, IDC_DETAIL_INFO, m_DetailInfoCtrl);
}


BEGIN_MESSAGE_MAP(CFileDetail, CDialogEx)
    ON_WM_SIZE()
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CFileDetail)
    EASYSIZE(IDC_MEDIA_TUMBNAIL,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_MFCPROPERTYGRID_DETAIL,IDC_MEDIA_TUMBNAIL,ES_BORDER,ES_KEEPSIZE,ES_BORDER,0)
    EASYSIZE(IDC_DETAIL_INFO,IDC_MFCPROPERTYGRID_DETAIL,ES_BORDER,ES_BORDER,ES_BORDER,0)
    
END_EASYSIZE_MAP

BOOL CFileDetail::OnInitDialog()
{
    CDialogEx::OnInitDialog();
                       
    INIT_EASYSIZE;

    UpdateData(FALSE);

	Translate(this);

    CRect ThumbnailRect;
    m_MediaTumbnailCtrl.GetWindowRect(&ThumbnailRect);
    m_ThumbnailImage.Create(ThumbnailRect.Width(), ThumbnailRect.Height(), 32);

    if (!m_MediaInfoStruct.empty())
    {
        int mediaID = m_MediaInfoStruct[0].GetMeidaID();
        std::vector<int> mediaSingleList;
        std::vector<Media> mediaSingleInfo;
        mediaSingleList.push_back(mediaID);
        //get media thumbnail
        MediaThumbnail thumbnail;
        HRESULT hr = GetControllerInterface->GetMediaThumbnail(mediaID, thumbnail);
        if (!SUCCEEDED(hr))
        {
            TRACE0("Failed to Get Media Thumbnail!\n");
        }
        //show media thumbnail in picture control


        vector<BYTE> data;
        thumbnail.GetThumbnail(data);
        MLThumbnail MLMediaThumbnail(data);

        SetMediaInfo(m_MediaInfoStruct[0]);

        SetMediaThumbnail(MLMediaThumbnail.GetImage());
    }
    return TRUE;
}

void CFileDetail::OnSize(UINT nType, int cx, int cy) 
{
    CDialogEx::OnSize(nType, cx, cy);
    UPDATE_EASYSIZE;
}

BOOL CFileDetail::SetMediaInfo(const Media& MediaInfo)
{
    InfoDisplayItem InfoItem;
    m_mediaInfo.ClearInfo();

    InfoItem.InfoName = Translate(_T("Media Name: "));
    InfoItem.InfoValue = MediaInfo.GetName().c_str();
    m_mediaInfo.AddInfo(InfoItem);


    InfoItem.InfoName = Translate(_T("Size: "));
    InfoItem.InfoValue = FormatSizeString(MediaInfo.GetFileSize());
    m_mediaInfo.AddInfo(InfoItem);

    InfoItem.InfoName = Translate(_T("Type: "));
    InfoItem.InfoValue = MediaInfo.GetMediaType().c_str();
    m_mediaInfo.AddInfo(InfoItem);


    InfoItem.InfoName = Translate(_T("Width: "));
    InfoItem.InfoValue.Format(_T("%d "), MediaInfo.GetWidth());
    m_mediaInfo.AddInfo(InfoItem);


    InfoItem.InfoName = Translate(_T("Height: "));
    InfoItem.InfoValue.Format(_T("%d"), MediaInfo.GetHeight());
    m_mediaInfo.AddInfo(InfoItem);

    InfoItem.InfoName = Translate(_T("Category: "));
    InfoItem.InfoValue = MediaInfo.GetMediaFilter().GetMediaFilterName().c_str();
    m_mediaInfo.AddInfo(InfoItem);
/*
    mediaInfo += Translate(_T("MediaServerPath: "));
    mediaInfo += MediaInfo.GetMediaServerPath().c_str();
    mediaInfo += _T("\t");

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
    InfoItem.InfoValue = MediaInfo.GetUploadTime().ToString().c_str();
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


    m_DetailInfoCtrl.SetWindowText(Translate(_T("Detail Information: ")) + MediaInfo.GetDesc().c_str());

    return TRUE;
}


BOOL CFileDetail::SetMediaThumbnail(const CImage& Thumbnail)
{

    if (Thumbnail.IsNull())
    {
        m_MediaTumbnailCtrl.SetBitmap(NULL);
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


    m_MediaTumbnailCtrl.SetBitmap(m_ThumbnailImage);

    m_MediaTumbnailCtrl.ShowWindow(SW_SHOW);

    UpdateData(FALSE);

    return TRUE;
}
