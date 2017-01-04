// PublishLayoutCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "PublishLayoutCtrl.h"
#include "Localization/Localization.h"

// CPublishLayoutCtrl dialog

IMPLEMENT_DYNAMIC(CPublishLayoutCtrl, CDialogEx)

CPublishLayoutCtrl::CPublishLayoutCtrl(CWnd* pParent /*=NULL*/)
    : CDialogEx(CPublishLayoutCtrl::IDD, pParent)
{
    m_pParent       = pParent;
    m_nLayoutSize   = 0;
    m_nContents     = 0;
    m_LayoutInfo    = _T("");
}

CPublishLayoutCtrl::~CPublishLayoutCtrl()
{
    m_pParent = NULL;
    m_MediaFiles.RemoveAll();
    m_arrErrorFiles.RemoveAll();
}

void CPublishLayoutCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

}


BEGIN_MESSAGE_MAP(CPublishLayoutCtrl, CDialogEx)
    ON_WM_SIZE()
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CPublishLayoutCtrl)

END_EASYSIZE_MAP


// COpenLayoutCtrl message handlers
BOOL CPublishLayoutCtrl::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    INIT_EASYSIZE;

    m_Browser.CreateFromControl(this, IDC_STATIC_WEBBROWSER);

    m_Browser.Write(m_LayoutInfo);

	Translate(this);
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPublishLayoutCtrl::OnSize(UINT nType, int cx, int cy) 
{
    CDialogEx::OnSize(nType, cx, cy);
    UPDATE_EASYSIZE;
}

#if 0
HRESULT CPublishLayoutCtrl::GenerateLayoutInfo(LayoutElement* pLayout, CString& szInfo)
{
    HRESULT hr = S_OK;

    m_nContents = 0;
    CS3DrawObj* pObj;
    LAYERINFO* pLayerInfo;
    CString szFile;
    CString szFileName;
    LARGE_INTEGER fileSize;
    DWORD64 nFileSize = 0;
    HANDLE hFile = NULL;
    POSITION pos = pLayout->Layers.GetStartPosition();
    while (pos != NULL)
    {
        pLayout->Layers.GetNextAssoc( pos, pObj, pLayerInfo );

        POSITION posLayer= pLayerInfo->GetHeadPosition();
        while (posLayer != NULL)
        {
            MEDIASETTING* pMediaSetting  = pLayerInfo->GetNext(posLayer);

            if (!pMediaSetting->MediaFile.IsEmpty())
            {
                szFile = pMediaSetting->MediaFile;
            }

            if (!pMediaSetting->MessageFile.IsEmpty())
            {
                szFile = pMediaSetting->MessageFile;
            }

            hFile = INVALID_HANDLE_VALUE;
            if (szFile.GetLength() > 0 && ::PathFileExists(szFile))
            {
                m_nContents += 1;

                hFile = ::CreateFile(szFile, FILE_GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hFile == INVALID_HANDLE_VALUE)
                {
                    m_arrErrorFiles.Add(szFile);
                }
                else
                {
                    if (::GetFileSizeEx(hFile, &fileSize))
                    {
                        nFileSize = fileSize.QuadPart;
                    }

                    CloseHandle(hFile);

                    m_nLayoutSize += nFileSize;
                    szFileName = ::PathFindFileName(szFile);

                    CString szSize;
                    if(nFileSize < 1024)
                    {
                        szSize.Format(_T("%lld bytes"), nFileSize);
                    }
                    else if(nFileSize < 1024 * 1024)
                    {
                        szSize.Format(_T("%0.0f KB"), (float)nFileSize/1024.0f);
                    }
                    else
                    {
                        szSize.Format(_T("%0.0f MB"), (float)nFileSize/1024.0f/1024.0f);
                    }
                    m_MediaFiles.SetAt(szFileName, szSize);
                    m_SummryInfo.AppendFormat(_T("<p><font fac=\"Calibri\" color=\"#330033\" size=3>    %s: %s</font></p>\r\n"), szFileName, szSize);
                }
            }
        }
    }

    return hr;
}

HRESULT CPublishLayoutCtrl::UpdateLayoutFile(LAYOUT* pLayout, CString& szInfo)
{
    HRESULT hr = S_OK;

    int Hour, Minute, Second;
    Hour    = pLayout->LayoutDuration / (60 * 60);
    Minute  = (pLayout->LayoutDuration % (60 * 60)) / 60;
    Second  = (pLayout->LayoutDuration % (60 * 60)) % 60;

    CString szFileName;
    CString szSize;
    LARGE_INTEGER fileSize;
    DWORD64 nFileSize = 0;
    HANDLE hFile = NULL;
    BOOL bLayout = FALSE;
    if (pLayout->LayoutFile.GetLength() > 0)
    {
        m_nContents += 1;

        hFile = ::CreateFile(pLayout->LayoutFile, FILE_GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            m_arrErrorFiles.Add(pLayout->LayoutFile);
        }
        else
        {
            if (::GetFileSizeEx(hFile, &fileSize))
            {
                nFileSize = fileSize.QuadPart;
            }

            CloseHandle(hFile);

            m_nLayoutSize += nFileSize;
            szFileName = ::PathFindFileName(pLayout->LayoutFile);

            if(nFileSize < 1024)
            {
                szSize.Format(_T("%lld bytes"), nFileSize);
            }
            else if(nFileSize < 1024 * 1024)
            {
                szSize.Format(_T("%0.0f KB"), (float)nFileSize/1024.0f);
            }
            else
            {
                szSize.Format(_T("%0.0f MB"), (float)nFileSize/1024.0f/1024.0f);
            }
            m_MediaFiles.SetAt(szFileName, szSize);

            bLayout = TRUE;
        }
    }

    pLayout->MediaCount         = m_nContents;
    pLayout->MediaContentSize   = m_nLayoutSize;

    m_LayoutInfo = _T("<html>\r\n");
    m_LayoutInfo.AppendFormat(_T("<body>\r\n"));
    m_LayoutInfo.AppendFormat(_T("<p align=\"center\"><font fac=\"Calibri\" color=\"#FF0000\" size=4>"));
	m_LayoutInfo += Translate(_T("Try to Publish Layout:"));
	m_LayoutInfo.AppendFormat(_T("%s</font></p>\r\n"), pLayout->LayoutName);
    m_LayoutInfo.AppendFormat(_T("<p><font fac=\"Calibri\" color=\"#330033\" size=3>"));
	m_LayoutInfo += Translate(_T("Canvas size is (pixel): width - "));
	m_LayoutInfo.AppendFormat(_T("%d"), pLayout->LayoutSize.cx);
	m_LayoutInfo += Translate(_T(", height - "));
	m_LayoutInfo.AppendFormat(_T("%d</font></p>\r\n"),  pLayout->LayoutSize.cy);
    m_LayoutInfo.AppendFormat(_T("<p><font fac=\"Calibri\" color=\"#330033\" size=3>"));
	m_LayoutInfo += Translate(_T("Total duration is (hh:mm:ss): "));
	m_LayoutInfo.AppendFormat(_T("%02d:%02d:%02d</font></p>\r\n"), Hour, Minute, Second);
    m_LayoutInfo.AppendFormat(_T("<p><font fac=\"Calibri\" color=\"#330033\" size=3>"));
	m_LayoutInfo += Translate(_T("Include media contents: "));
	m_LayoutInfo.AppendFormat(_T("%d</font></p>\r\n"), m_nContents);

    if (m_nLayoutSize < 1024)
    {
        m_LayoutInfo.AppendFormat(_T("<p><font fac=\"Calibri\" color=\"#330033\" size=3>"));
		m_LayoutInfo += Translate(_T("Total contents size is: "));
		m_LayoutInfo.AppendFormat(_T("%lld bytes</font></p>\r\n"), m_nLayoutSize);
    }
    else if(m_nLayoutSize < 1024 * 1024)
    {
        m_LayoutInfo.AppendFormat(_T("<p><font fac=\"Calibri\" color=\"#330033\" size=3>"));
		m_LayoutInfo += Translate(_T("Total contents size is: "));
		m_LayoutInfo.AppendFormat(_T("%0.0f KB</font></p>\r\n"), (float)m_nLayoutSize/1024.0f);
    }
    else
    {
        m_LayoutInfo.AppendFormat(_T("<p><font fac=\"Calibri\" color=\"#330033\" size=3>"));
		m_LayoutInfo += Translate(_T("Total contents size is: "));
		m_LayoutInfo.AppendFormat(_T("%0.0f MB</font></p>\r\n"), (float)m_nLayoutSize/1024.0f/1024.0f);
    }

    m_LayoutInfo.AppendFormat(_T("<p><font fac=\"Calibri\" color=\"#330033\" size=3>"));
	m_LayoutInfo += Translate(_T("Detail media content list: "));
	m_LayoutInfo.AppendFormat(_T("</font></p>\r\n"));
    m_LayoutInfo.AppendFormat(m_SummryInfo);
    m_LayoutInfo.AppendFormat(_T("<p><font fac=\"Calibri\" color=\"#330033\" size=3>    %s: %s</font></p>\r\n"), szFileName, szSize);

    if (m_arrErrorFiles.GetCount() > 0)
    {
        m_LayoutInfo.AppendFormat(_T("<p></p>\r\n<p><font fac=\"Calibri\" color=\"#330033\" size=3>"));
		m_LayoutInfo += Translate(_T("Failed to get following contents size: "));
		m_LayoutInfo.AppendFormat(_T("</font></p>\r\n"));
        for (INT i = 0; i < m_arrErrorFiles.GetCount(); i++)
        {
            m_LayoutInfo.AppendFormat(_T("<p><font fac=\"Calibri\" color=\"#330033\" size=3>    %s</font></p>\r\n"), m_arrErrorFiles.GetAt(i));
        }
    }

    m_LayoutInfo.AppendFormat(_T("</body>\r\n"));
    m_LayoutInfo.AppendFormat(_T("</html>"));

    return hr;
}
#endif
