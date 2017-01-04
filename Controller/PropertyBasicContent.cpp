// PropertyBasicContent.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "PropertyBasicContent.h"
#include "afxdialogex.h"
#include "PropertyCtrl.h"
#include "FileDetail.h"
#ifdef STARTER_EDITION
#include "OldFileDetail.h"
#endif
#include "MediaInfo/MediaInfo.h"
#include "Localization/Localization.h"
#include "MediaLibraryInfoCtrl.h"

// wParam - no used, lParam - no used
UINT WM_ON_TIME_CHANGE = ::RegisterWindowMessage(_T("PROPERTY_LAYER_TIME_CHANGE"));

// CPropertyBasicContent dialog
extern UINT WM_MEDIASETTING_UPDATE_BEGIN ;
extern UINT WM_MEDIASETTING_UPDATE_END;
UINT WM_MEDIA_BASICINFO_UPDATE_BEGIN = ::RegisterWindowMessage(_T("WM_MEDIA_BASICINFO_UPDATE_BEGIN"));
UINT WM_MEDIA_BASICINFO_UPDATE_END = ::RegisterWindowMessage(_T("WM_MEDIA_BASICINFO_UPDATE_END"));
IMPLEMENT_DYNAMIC(CPropertyBasicContent, CDialogEx)

    CPropertyBasicContent::CPropertyBasicContent(CWnd* pParent /*=NULL*/)
    : CDialogEx(CPropertyBasicContent::IDD, pParent)
{
    m_MediaInfo.reset();
}

CPropertyBasicContent::~CPropertyBasicContent()
{
    m_MediaInfo.reset();
}

void CPropertyBasicContent::UpdateBegin()
{
    if(!m_MediaInfo)
        return;
      GetParent()->SendMessage(WM_MEDIA_BASICINFO_UPDATE_BEGIN,0,0);
}

void CPropertyBasicContent::UpdateEnd()
{
    if(!m_MediaInfo)
        return;
      GetParent()->SendMessage(WM_MEDIA_BASICINFO_UPDATE_END,0,0);
}

void CPropertyBasicContent::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON_BG, m_wndColorBG);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_time);
	DDX_Control(pDX, IDC_SLIDER_VOLUME, m_wndVolume);
	DDX_Control(pDX, IDC_SLIDER_TRANSPARENCY, m_ctlTransparency);
	DDX_DateTimeCtrl(pDX, IDC_REFRESH_DATETIMEPICKER, m_refreshTime);
}


BEGIN_MESSAGE_MAP(CPropertyBasicContent, CDialogEx)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_MFCCOLORBUTTON_BG, &CPropertyBasicContent::OnClickedMfccolorbuttonBg)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, &CPropertyBasicContent::OnDatetimechangeDatetimepicker1)
    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_VOLUME, &CPropertyBasicContent::OnNMReleasedcaptureSliderVolume)
    ON_BN_CLICKED(IDC_CHECK_KEEP_ASPECT_RATIO, &CPropertyBasicContent::OnBnClickedCheckKeepAspectRatio)
    ON_BN_CLICKED(IDC_BUTTON_DETAIL, &CPropertyBasicContent::OnBnClickedButtonDetail)
    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_TRANSPARENCY, &CPropertyBasicContent::OnNMReleasedcaptureSliderTransparency)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_REFRESH_DATETIMEPICKER, &CPropertyBasicContent::OnDtnDatetimechangeRefreshDatetimepicker)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CPropertyBasicContent)
    EASYSIZE(IDC_BUTTON_DETAIL, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
    EASYSIZE(IDC_STATIC_PLAY,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_DATETIMEPICKER1,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_MFCCOLORBUTTON_BG,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_SLIDER_VOLUME,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_CHECK_KEEP_ASPECT_RATIO,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_SLIDER_TRANSPARENCY,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_REFRESH_DATETIMEPICKER,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
END_EASYSIZE_MAP


// CPropertyBasicContent message handlers
BOOL CPropertyBasicContent::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    INIT_EASYSIZE;
    ((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER1))->SetFormat(_T("HH:mm:ss"));
    ((CDateTimeCtrl*)GetDlgItem(IDC_REFRESH_DATETIMEPICKER))->SetFormat(_T("HH:mm:ss"));

    CFont font;
    LOGFONT lf;
    CWnd* content = (CWnd*)GetDlgItem(IDC_STATIC_CONTENT);
    content->GetFont()->GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;
    font.CreateFontIndirect(&lf);
    content->SetFont(&font);
    font.Detach();

    //     ((CButton*)GetDlgItem(IDC_CHECK_KEEP_ASPECT_RATIO))->EnableWindow(FALSE);

    Translate(this);
    return FALSE;  // return TRUE  unless you set the focus to a control
}


void CPropertyBasicContent::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);
    UPDATE_EASYSIZE;
}

BOOL CPropertyBasicContent::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Add your specialized code here and/or call the base class


    CDialogEx::PreCreateWindow(cs);

    cs.style |= WS_CLIPCHILDREN;
    return TRUE;
}

VOID  CPropertyBasicContent::SetContent(std::shared_ptr<MediaElement> mediaInfo)
{
    if (mediaInfo.get())
    {
        m_MediaInfo = mediaInfo;
        COLORREF c = m_MediaInfo->GetBGColor() & 0xFFFFFF;
        c = RGB((c >> 16), (c & 0x00ff00) >> 8, (c & 0x0000ff));
        m_wndColorBG.SetColor(c);

        int h, m, s, dur;
        dur = m_MediaInfo->GetDuration();

        h = dur / 3600;
        m = (dur - h * 3600) / 60;
		s = dur % 60;

        m_time.SetTime(h, m, s);
        //m_pMedia->MediaType;

		dur = m_MediaInfo->GetRefreshInterval();
		h = dur / 3600;
		m = (dur - h * 3600) / 60;
		s = dur % 60;
		m_refreshTime.SetTime(h,m,s);

        m_wndVolume.SetRange(0, 100);
        m_wndVolume.SetPos(m_MediaInfo->GetVolumeCount());

#ifdef STARTER_EDITION_
		GetDlgItem(IDC_BUTTON_DETAIL)->ShowWindow(FALSE);
#endif
        GetDlgItem(IDC_BUTTON_DETAIL)->EnableWindow(FALSE);
		GetDlgItem(IDC_REFRESH_DATETIMEPICKER)->EnableWindow(FALSE);
        GetDlgItem(IDC_SLIDER_TRANSPARENCY)->EnableWindow(FALSE);
        GetDlgItem(IDC_SLIDER_VOLUME)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHECK_KEEP_ASPECT_RATIO)->EnableWindow(FALSE);
        ((CButton*)GetDlgItem(IDC_CHECK_KEEP_ASPECT_RATIO))->SetCheck(FALSE);
        GetDlgItem(IDC_MFCCOLORBUTTON_BG)->EnableWindow(FALSE);

        m_ctlTransparency.SetRange(0, 99);
        m_ctlTransparency.SetPos(50);
		CString szMediaType = m_MediaInfo->GetMediaType();
        if (szMediaType == szTypeName_Audio)
        {
            GetDlgItem(IDC_SLIDER_VOLUME)->EnableWindow(TRUE);
        }
        else if (szMediaType == szTypeName_Video)
        {
            GetDlgItem(IDC_MFCCOLORBUTTON_BG)->EnableWindow(TRUE);
            GetDlgItem(IDC_CHECK_KEEP_ASPECT_RATIO)->EnableWindow(TRUE);
            ((CButton*)GetDlgItem(IDC_CHECK_KEEP_ASPECT_RATIO))->SetCheck(m_MediaInfo->GetKeepAspect());
            GetDlgItem(IDC_SLIDER_TRANSPARENCY)->EnableWindow(TRUE);
            GetDlgItem(IDC_SLIDER_VOLUME)->EnableWindow(TRUE);
            m_nTransparency = (float)(m_MediaInfo->GetBGColor() >> 24) / 2.55;
            m_ctlTransparency.SetPos(m_nTransparency);
        }
        else if (szMediaType == _T("S3ImageViewer"))
        {
            GetDlgItem(IDC_MFCCOLORBUTTON_BG)->EnableWindow(TRUE);
            GetDlgItem(IDC_CHECK_KEEP_ASPECT_RATIO)->EnableWindow(TRUE);
            ((CButton*)GetDlgItem(IDC_CHECK_KEEP_ASPECT_RATIO))->SetCheck(m_MediaInfo->GetKeepAspect());
            GetDlgItem(IDC_SLIDER_TRANSPARENCY)->EnableWindow(TRUE);
            m_nTransparency = (float)(m_MediaInfo->GetBGColor() >> 24) / 2.55;
            m_ctlTransparency.SetPos(m_nTransparency);
        }
        else if (szMediaType == szTypeName_Text)
        {
            GetDlgItem(IDC_MFCCOLORBUTTON_BG)->EnableWindow(TRUE);
            GetDlgItem(IDC_SLIDER_TRANSPARENCY)->EnableWindow(TRUE);
			std::shared_ptr<S3SIGNAGE_TEXT_SETTING> pTextSetting = m_MediaInfo->GetTextSetting();
            m_nTransparency = pTextSetting->Transparency;
            m_ctlTransparency.SetPos(m_nTransparency);
        }
        else if (szMediaType == _T("S3WebBrowser") &&
                 (m_MediaInfo->GetMediaName().Left(7).CompareNoCase(_T("rtsp://")) == 0 ||
                 m_MediaInfo->GetMediaName().Left(6).CompareNoCase(_T("mms://")) == 0))
        {
            GetDlgItem(IDC_SLIDER_VOLUME)->EnableWindow(TRUE);
        }


        // all others can show detail
        if (!(szMediaType == _T("Clock") ||
			szMediaType == _T("S3Weather") ||
            szMediaType == _T("S3Weather2") ||
			szMediaType == _T("S3Clock") ||
            szMediaType == _T("S3WebBrowser") ||
            szMediaType == _T("S3Capture") ||
            szMediaType == _T("S3EXE") ||
            szMediaType == _T("S3PPTViewer") ||
            szMediaType == szTypeName_Message ||
            szMediaType == szTypeName_Text ||
            szMediaType == szTypeName_EmptyContent)
			&& (   -1 == m_MediaInfo->GetMediaFile().Find(_T("mms://")) 
			    && -1 == m_MediaInfo->GetMediaFile().Find(_T("rtsp://")))
		   )
        {
		
            GetDlgItem(IDC_BUTTON_DETAIL)->EnableWindow(TRUE);
        }

		if (szMediaType == _T("S3WebBrowser"))
		{
			GetDlgItem(IDC_REFRESH_DATETIMEPICKER)->EnableWindow(TRUE);
		}

        UpdateData(FALSE);
    }
}

void CPropertyBasicContent::Clear()
{
    m_MediaInfo.reset();
}

void CPropertyBasicContent::UpdateContent()
{
    if (m_MediaInfo.get())
    {
        UpdateBegin();
        UpdateData(TRUE);

        m_MediaInfo->SetBGColor((m_MediaInfo->GetBGColor() & 0xff000000) | (GetRValue(m_wndColorBG.GetColor()) << 16) |
            (GetGValue(m_wndColorBG.GetColor()) << 8) | (GetBValue(m_wndColorBG.GetColor())) );
        m_MediaInfo->SetDuration( (m_time.GetHour() * 60 + m_time.GetMinute()) * 60 + m_time.GetSecond());
        m_MediaInfo->SetVolumeCount(m_wndVolume.GetPos());
		m_MediaInfo->SetRefreshInterval(((m_refreshTime.GetHour() * 60 + m_refreshTime.GetMinute()) * 60 + m_refreshTime.GetSecond()));

        CString szMediaType = m_MediaInfo->GetMediaType();

        m_nTransparency = m_ctlTransparency.GetPos();
        if (szMediaType == szTypeName_Text)
        {
			std::shared_ptr<S3SIGNAGE_TEXT_SETTING> pTextSetting = m_MediaInfo->GetTextSetting();
            pTextSetting->Transparency = m_nTransparency;
        }
        else if (szMediaType == szTypeName_Video || szMediaType == _T("S3ImageViewer"))
        {
            m_MediaInfo->SetBGColor( (m_MediaInfo->GetBGColor() & 0x00ffffff) | ((DWORD)((float)m_nTransparency * 2.55) << 24));
        }

        UpdateEnd();
    }
}


void CPropertyBasicContent::OnClickedMfccolorbuttonBg()
{
    UpdateContent();
}


void CPropertyBasicContent::OnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    CheckDuration();
    UpdateContent();

    GetParent()->SendMessage(WM_ON_TIME_CHANGE, 0, 0);

    *pResult = 0;
}

void CPropertyBasicContent::OnNMReleasedcaptureSliderVolume(NMHDR *pNMHDR, LRESULT *pResult)
{
    UpdateContent();
    *pResult = 0;
}

void CPropertyBasicContent::CheckDuration()
{
    if (!((CPropertyCtrl*)GetParent())->CheckTransitionTime())
	{
		MessageBox(Translate(_T("Transition duration needs to be shorter than media content's duration")),
			Translate(_T("Warning:Check duration")), MB_OK | MB_ICONEXCLAMATION);
		UpdateData(FALSE);
		return;
	}

	UpdateData(TRUE);
    DWORD duration = (m_time.GetHour() * 60 + m_time.GetMinute()) * 60 + m_time.GetSecond();
    if (duration == 0)
    {
        MessageBox(Translate(_T("Duration can't be set as 0")), Translate(_T("Warning:Check duration")), MB_OK|MB_ICONEXCLAMATION);
        m_time.SetTime(0, 0, 1);
        UpdateData(FALSE);
    }
}



void CPropertyBasicContent::OnBnClickedCheckKeepAspectRatio()
{
    if (m_MediaInfo.get())
    {
        m_MediaInfo->SetKeepAspect(((CButton*)GetDlgItem(IDC_CHECK_KEEP_ASPECT_RATIO))->GetCheck());
    }
}

BOOL CPropertyBasicContent::PreTranslateMessage(MSG* pMsg)
{
    m_focus.InitFocusEx(GetFocusableWindow, this);

    if (m_focus.ProcessKeyPressMessage(this, pMsg))
    {
        return TRUE;
    }
    if (pMsg->message == WM_MOUSEWHEEL)
    {
        GetParent()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
        return TRUE;
    }
    if (pMsg->message == WM_LBUTTONDOWN)
    {
        GetParent()->SetFocus();
    }

    return __super::PreTranslateMessage(pMsg);
}

HWND CPropertyBasicContent::GetFocusableWindow(int nPoisition, LPVOID lParam, BOOL shift)
{
    CPropertyBasicContent* pThis = (CPropertyBasicContent*)lParam;
    switch (nPoisition)
    {
    case FOCUSABLEWINDOW_POSITION_FIRST:
        return CFocusEx::GetFirstFocusableWindow(pThis->GetSafeHwnd());
    case FOCUSABLEWINDOW_POSITION_LAST:
        return CFocusEx::GetLastFocusableWindow(pThis->GetSafeHwnd());
    case FOCUSABLEWINDOW_POSITION_FOCUSABLE:
        {
            CPropertyCtrl* pParent = (CPropertyCtrl*)(pThis->GetParent());
            return pParent->GetNextFocusWindow(pThis->GetSafeHwnd(), shift);
        }
    }
    return NULL;
}


void CPropertyBasicContent::OnBnClickedButtonDetail()
{
    if (m_MediaInfo.get())
    {
		CString szMediaType = m_MediaInfo->GetMediaType();
        if (szMediaType == szTypeName_EmptyContent)
        {
            MessageBox(Translate(_T("EmptyContent")), Translate(_T("File Detail")), MB_OK);
        }
        else if (szMediaType == szTypeName_Message || szMediaType == szTypeName_Text)
        {
            MessageBox(Translate(_T("Message")), Translate(_T("File Detail")), MB_OK);
        }
        // TODO: read from plugin manager
        else if (szMediaType == _T("Clock") ||
			szMediaType == _T("S3Clock") ||
            szMediaType == _T("S3WebBrowser") ||
            szMediaType == _T("S3Capture") ||
            szMediaType == _T("S3EXE") ||
            szMediaType == _T("S3PPTViewer"))
        {
            MessageBox(Translate(_T("Plugin")), Translate(_T("File Detail")), MB_OK);
        }
        else
        {
            INT MediaID = m_MediaInfo->GetMediaID();
            if (MediaID >= 0)
            {
                vector<int> Medias;
                Medias.push_back(MediaID);
                vector<Media> MediaInfos;
                HRESULT hr = GetControllerInterface->GetMedias(Medias, MediaInfos);
                if (SUCCEEDED(hr) && !MediaInfos.empty())
                {
                    CFileDetail dlg(MediaInfos[0]);
                    dlg.DoModal();
                }
                else
                {
                    MessageBox(Translate(_T("Failed to get media info from controller server")), Translate(_T("File Detail")), MB_OK);
                    TRACE1("Failed to get media info from controller server %d", MediaID);
                }
            }
#ifdef STARTER_EDITION
			else
			{
				MediaInfoLib::MediaInfo mi;
				if (!mi.Open(m_MediaInfo->GetMediaFile().GetString()))
				{
					MessageBox(Translate(_T("Unknown file type, or file is not exist (click preview to download it first)")), Translate(_T("File information")), MB_OK);
					return;
				}

				COldFileDetail dlg(mi);
				dlg.DoModal();
				mi.Close();
			}
#else
            else
            {
                MessageBox(Translate(_T("Failed to Get Media Detail Info")), Translate(_T("File Detail")), MB_OK);
            }
#endif
        }

    }
}

void CPropertyBasicContent::OnNMReleasedcaptureSliderTransparency(NMHDR *pNMHDR, LRESULT *pResult)
{
    if (m_MediaInfo.get())
    {
        UpdateContent();
    }
    *pResult = 0;
}


void CPropertyBasicContent::OnDtnDatetimechangeRefreshDatetimepicker(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	
	CheckRefreshInterval();
	UpdateContent();
	*pResult = 0;
}

void CPropertyBasicContent::CheckRefreshInterval()
{
	UpdateData(TRUE);
	DWORD dwInterval = (m_refreshTime.GetHour() * 60 + m_refreshTime.GetMinute()) * 60 + m_refreshTime.GetSecond();
	if (dwInterval == 0)
	{
		MessageBox(Translate(_T("Refresh Interval can't be set as 0")), Translate(_T("Warning:Check duration")), MB_OK|MB_ICONEXCLAMATION);
		m_refreshTime.SetTime(0, 0, 1);
		UpdateData(FALSE);
	}
}