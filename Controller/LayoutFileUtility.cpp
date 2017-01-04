

#include "StdAfx.h"
#include "PreviewDownLoadDlg.h"
#include "LayoutWarningCtrl.h"
#include "PlayerController.h"
#include "LayoutFileUtility.h"
#ifdef _MEDIALIBRARY2
#include "MediaLibrary 2/MediaLibrary.h"
#else
#include "MediaLibrary/MediaLibrary.h"
#endif
// LayoutFileUtility

LayoutFileUtility::LayoutFileUtility()
{

}

LayoutFileUtility::~LayoutFileUtility()
{


}


HRESULT LayoutFileUtility::PreviewLayout(const CString& szLayoutFile, const vector<int>& MediaIDs,
    const int nWidth, const int nHeight, CString szLayoutThumbnail)
{
    HRESULT hr = S_OK;

    MEDIA_LIST MediaList;
    MediaList.m_MediaIDs = MediaIDs;

#ifdef SERVER_EDITION
    if (MediaIDs.size() > 0)
    {
        //Need download the media files

        // copy layout to disc
        MEDIA_DETAIL_INFO MediaInfo;
        hr = GetMediaLibraryInterface->GetMedia(MediaList,MediaInfo);
        if (!SUCCEEDED(hr))
        {
            TRACE0("Error happen in GetMedia, PreviewDownLoadDlg");
            //AfxMessageBox(Translate(_T("Failed to get media info!")), MB_OK|MB_ICONINFORMATION);
            std::vector<Media> mediaList;
            CString msg = Translate(_T("Preview failed:")) + _T("\n");
            GetControllerInterface->GetMedias(MediaList.m_MediaIDs, mediaList);
            auto it = mediaList.begin(), ite = mediaList.end();
            for (; it != ite; ++it)
            {
                if (it->GetAudit().GetStatus() == Audit::DELETED)
                {
                    CString s;
                    s.Format(Translate(_T("Media '%s' is %s.")), it->GetName().c_str(), Translate(_T("deleted")));
                    msg += s + _T("\n");
                }
                else if (it->GetAudit().GetStatus() == Audit::CREATEBUTNOTAPPROVED)
                {
                    CString s;
                    s.Format(Translate(_T("Media '%s' is %s.")), it->GetName().c_str(), Translate(_T("not approved")));
                    msg += s + _T("\n");
                }
                else if (it->GetAudit().GetStatus() == Audit::REJECTED)
                {
                    CString s;
                    s.Format(Translate(_T("Media '%s' is %s.")), it->GetName().c_str(), Translate(_T("rejected")));
                    msg += s + _T("\n");
                }
            }
            ShowXMessageBox(NULL,msg, Translate(_T("Tips:Preview layout")), MB_OK|MB_ICONINFORMATION);
            return E_UNEXPECTED;
        }

        if (MediaInfo.m_FinishedCnt != MediaInfo.m_TotalCnt)
        {
            PreviewDownLoadDlg dlg;

            // collect exported layout list
            DWORD64 FreeRemain = GetMediaLibraryInterface->GetMediaLibraryRootFreeSpace();

            DWORD64 LayoutSize = MediaInfo.m_TotalSize;
            DWORD64 SizeStillNeedDownload = MediaInfo.m_TotalSize - MediaInfo.m_FinishedSize;
            if (LayoutSize >= FreeRemain)
            {
                CString cWarning, str1, str2, strSizeStillNeed;
                str1.Format(_T("%d MB"),LayoutSize>>20);
                str2.Format(_T("%d MB"),FreeRemain>>20);
                strSizeStillNeed.Format(_T("%d MB"),SizeStillNeedDownload>>20);
                cWarning =  Translate(_T("Insufficient disk space, Unable to download the layout!\n\nLayout total size: "))+ str1 +
                    /*Translate(_T(";\nMedia that need download: ")) + strSizeStillNeed + */
					Translate(_T(";\nController disk free size: ")) + str2 +
                     Translate(_T("; \n\nPlease make sure there are enough disk free space for downloading the media!"));
                ShowXMessageBox(NULL,cWarning, Translate(_T("Warning:Preview layout")), MB_OK|MB_ICONEXCLAMATION);
                return E_UNEXPECTED;
            }

            if (!dlg.AddMediaListJob(MediaList))
            {
                ShowXMessageBox(NULL,Translate(_T("Media Library failed to add download task, some media used in the layout may be not downloaded!")),Translate(_T("Error:Preview layout")), MB_OK|MB_ICONERROR);
            }

            if (dlg.DoModal() != IDOK || dlg.m_Progress< PROGRESS_SLICE)
            {
                return E_UNEXPECTED;
            }
        }

        // Losed media contents when open or preview the layout
        CStringArray  arrLosedFiles;
        // Suspicion media contents when open or preview the layout
        CStringArray arrSuspicionFiles;

        // copy layout to disc
        MEDIA_DETAIL_INFO LocalMediaInfo;
        if(GetMediaLibraryInterface->GetMedia(MediaList,LocalMediaInfo) != S_OK)
        {
            ShowXMessageBox(NULL,Translate(_T("Failed to get media info!")),Translate(_T("Error:Preview layout")), MB_OK|MB_ICONERROR);;
            return E_UNEXPECTED;
        }

        for (auto it = LocalMediaInfo.m_Medias.begin(); it != LocalMediaInfo.m_Medias.end(); ++it)
        {
            CString MediaLibraryFilePath = (GetMediaLibraryInterface->GetMediaLibraryRoot()
                + it->second.MediaServerPath).c_str();

            if (it->second.MediaType == MEDIA_FILE && !::PathFileExists(MediaLibraryFilePath))
            {
                CString MediaFileName = it->second.Name.c_str();

                if (::PathFileExists((GetMediaLibraryInterface->GetMediaLibraryRoot() +
                    MediaLibrary::GetSuspicionFileName(it->second.MediaServerPath)).c_str()))
                {
                    arrSuspicionFiles.Add(MediaFileName);
                }
                else
                {
                    arrLosedFiles.Add(MediaFileName);
                }
            }

        }

        if (szLayoutThumbnail.IsEmpty() && !(arrLosedFiles.IsEmpty() && arrSuspicionFiles.IsEmpty()))
        {
            CString szWarning;
            szWarning = _T("<html>\r\n");
            szWarning.AppendFormat(_T("<body>\r\n"));
            if (!arrLosedFiles.IsEmpty())
            {
                szWarning.AppendFormat(_T("<p align=\"center\"><font fac=\"Calibri\" color=\"#FF0000\" size=4>"));
                szWarning += Translate(_T("Failed to find following files:"));
                szWarning.AppendFormat(_T("</font></p>\r\n"));

                for (int i = 0; i < arrLosedFiles.GetCount(); i++)
                {
                    szWarning.AppendFormat(_T("<p><font fac=\"Calibri\" color=\"#330033\" size=3>    %s</font></p>\r\n"),
                        arrLosedFiles.GetAt(i));
                }
            }

            if (!arrSuspicionFiles.IsEmpty())
            {
                szWarning.AppendFormat(_T("<p align=\"center\"><font fac=\"Calibri\" color=\"#FF0000\" size=4>"));
                szWarning += Translate(_T("Following files could not be shown on the screen because they are not safely downloaded:"));
                szWarning.AppendFormat(_T("</font></p>\r\n"));

                for (int i = 0; i < arrSuspicionFiles.GetCount(); i++)
                {
                    szWarning.AppendFormat(_T("<p><font fac=\"Calibri\" color=\"#330033\" size=3>    %s</font></p>\r\n"),
                        arrSuspicionFiles.GetAt(i));
                }
            }

            szWarning.AppendFormat(_T("</body>\r\n"));
            szWarning.AppendFormat(_T("</html>"));

            CLayoutWarningCtrl warningCtrl;
            warningCtrl.SetWarning(szWarning);
            warningCtrl.DoModal();
            //return E_FAIL;
        }
    }
#endif

    PlayerController::GetInstance()->SetPipePostfix(StringUtility::GenerateGUID().c_str());
    PlayerController::GetInstance()->SetLogPath(GetControllerDefaultDir().c_str());
    hr = PlayerController::GetInstance()->StartPlayer(nWidth, nHeight);
    if (FAILED(hr))
    {
        ShowXMessageBox(NULL,Translate(_T("Failed to connect to player!")),Translate(_T("Error:Preview layout")), MB_OK|MB_ICONERROR);
        return hr;
    }
    hr = PlayerController::GetInstance()->Load(szLayoutFile);

    if (!szLayoutThumbnail.IsEmpty())
    {
        Sleep(5000);
        PlayerController::GetInstance()->Snapshot(szLayoutThumbnail);
        PlayerController::GetInstance()->StopPlayer();
    }

    return hr;
}

