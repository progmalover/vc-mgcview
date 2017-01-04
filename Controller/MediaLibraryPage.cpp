
#include "StdAfx.h"
#include "Controller.h"
#include "MediaLibraryPage.h"
#include "Localization/Localization.h"
#ifdef _MEDIALIBRARY2
#include "MediaLibrary 2/MediaLibrary.h"
#else
#include "MediaLibrary/MediaLibrary.h"
#endif
#include "RenameCategory.h"
#include "RejectReason.h"
#include "UploadUrlDialog.h"
#include "MagicSortListCtrl.h"
#include <map>
#include <boost/filesystem.hpp>

#include  "LoadingTranslucentDlg.h"
#include "PreviewDownLoadDlg.h"

#define MEDIALIST_REFRESH_TIMER 0x1174
#define UPLIST_TIMER 0xFF000001
// CMediaLibraryPage

IMPLEMENT_DYNAMIC(CMediaLibraryPage, CBaseControllerPage)

CMediaLibraryPage::CMediaLibraryPage()
{
	m_bUploadingList = TRUE;
	m_bUploadedList = TRUE;
	m_bUploadFailList = TRUE;
	m_bDownloadingList = TRUE;
	m_bDownloadedList = TRUE;
	m_bDownloadFailList = TRUE;
    m_bIsUrl = TRUE;

}

CMediaLibraryPage::~CMediaLibraryPage()
{
	MediaSearchOptions::FreeInstance();
}

BEGIN_MESSAGE_MAP(CMediaLibraryPage, CBaseControllerPage)
    ON_WM_CREATE()
    ON_WM_DESTROY()

    ON_WM_TIMER()

	ON_COMMAND(ID_RIBBON_BTN_MEDIA_PREVIEW, OnRibbonBtnPreview)
    ON_COMMAND(ID_RIBBON_BTN_MEDIA_REFRESH, OnRefreshCurList)
	ON_COMMAND(ID_RIBBON_BTN_MEDIA_APPROVE, OnRibbonBtnApprove)
	ON_COMMAND(ID_RIBBON_BTN_MEDIA_REJECT, OnRibbonBtnReject)
	ON_COMMAND(ID_RIBBON_BTN_MEDIA_REQUEST, OnRibbonBtnRequest)
	ON_COMMAND(ID_RIBBON_BTN_MEDIA_UPLOADFILE, OnRibbonBtnUploadFile)
    ON_COMMAND(ID_RIBBON_BTN_MEDIA_UPLOADURL, OnRibbonBtnUploadURL)
	ON_COMMAND(ID_RIBBON_BTN_MEDIA_SYNCFILE, OnRibbonBtnSyncFile)
	ON_COMMAND(ID_RIBBON_BTN_MEDIA_FORCESYNCFILE, OnRibbonBtnForceSyncFile)
	ON_COMMAND(ID_RIBBON_BTN_MEDIA_DELFILE, OnRibbonBtnDeleteFile)
	ON_COMMAND(ID_RIBBON_BTN_MEDIA_RETIREFILE, OnRibbonBtnRetireFile)
	ON_COMMAND(ID_RIBBON_BTN_MEDIA_ADDWORKSPACE, OnRibbonBtnAddWorkspace)
	ON_COMMAND(ID_RIBBON_BTN_MEDIA_SYNCWORKSPACE, OnRibbonBtnSyncWorkspace)
	ON_COMMAND(ID_BTN_MEDIA_STARTUPLOAD, OnRibbonBtnStartUpload)
	ON_COMMAND(ID_BTN_MEDIA_PAUSEUPLOAD, OnRibbonBtnPauseUpload)
	ON_COMMAND(ID_BTN_MEDIA_CANCELUPLOAD, OnRibbonBtnDeleteUpload)
	ON_COMMAND(ID_BTN_MEDIA_STARTDOWNLOAD, OnRibbonBtnStartDownload)
	ON_COMMAND(ID_BTN_MEDIA_PAUSEDOWNLOAD, OnRibbonBtnPauseDownload)
	ON_COMMAND(ID_BTN_MEDIA_CANCELDOWNLOAD, OnRibbonBtnDeleteDownload)
	ON_COMMAND(ID_BTN_MEDIA_UPLOADING, OnUploadingList)
	ON_COMMAND(ID_BTN_MEDIA_UPLOADED, OnUploadedList)
	ON_COMMAND(ID_BTN_MEDIA_UPLOADFAILED, OnUploadFailList)
	ON_COMMAND(ID_BTN_MEDIA_DOWNLOADING, OnDownloadingList)
	ON_COMMAND(ID_BTN_MEDIA_DOWNLOADED, OnDownloadedList)
	ON_COMMAND(ID_BTN_MEDIA_DOWNLOADFAILED, OnDownloadFailList)
    ON_COMMAND(ID_RIBBON_BTN_MEDIA_RENAME_CATE, OnRenameCategoryList)

    //
  //  ON_REGISTERED_MESSAGE(MEDIA_CATEGORY_REFRESH, OnMediaLibraryCategoryRefresh)
	ON_REGISTERED_MESSAGE(MEDIALIB_SEARCH, OnMediaLibrarySearch)
  //  ON_REGISTERED_MESSAGE(MEDIA_LIST_CATEGORY, UpdateMediaCategoryList)
    ON_REGISTERED_MESSAGE(MEDIA_UPLOAD_LIST, OnMediaLibraryUploadList)
    ON_REGISTERED_MESSAGE(MEDIA_DOWNLOAD_LIST, OnMediaLibraryDownloadList)
    ON_REGISTERED_MESSAGE(MEDIA_SHOW_INFO, OnMediaInfoShow)
    ON_REGISTERED_MESSAGE(MEDIA_CLEAR_INFO, OnMediaInfoClear)
    ON_REGISTERED_MESSAGE(MEDIADLG_SEARCH, OnMediaDlgSearch)
    ON_REGISTERED_MESSAGE(MEDIA_SETACTIVE_TAB, OnMediaSetActiveTab)
    ON_MESSAGE(USER_MSG_MEDIALIBRARY_GET_MEDIALIST, &CMediaLibraryPage::OnGetMediaLibraryMediaList)
    ON_MESSAGE(USER_MSG_SERVER_MEDIALIST, &CMediaLibraryPage::OnGetServerMediaList)
    ON_MESSAGE(USER_MSG_UPLOAD_MEDIA, &OnMediaUpload)
	ON_MESSAGE(USER_MSG_UPDATE_SERVER_MEDIALIST, &OnUpdateMediaList )


	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_MEDIA_PREVIEW, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_MEDIA_REFRESH, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_MEDIA_APPROVE, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_MEDIA_REJECT, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_MEDIA_REQUEST, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_MEDIA_UPLOADFILE, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_MEDIA_UPLOADURL, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_MEDIA_SYNCFILE, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_MEDIA_FORCESYNCFILE, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_MEDIA_DELFILE, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_MEDIA_RETIREFILE, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_BTN_MEDIA_STARTUPLOAD, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_BTN_MEDIA_PAUSEUPLOAD, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_BTN_MEDIA_CANCELUPLOAD, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_BTN_MEDIA_UPLOADING, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_BTN_MEDIA_UPLOADED, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_BTN_MEDIA_UPLOADFAILED, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_BTN_MEDIA_STARTDOWNLOAD, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_BTN_MEDIA_PAUSEDOWNLOAD, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_BTN_MEDIA_CANCELDOWNLOAD, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_BTN_MEDIA_DOWNLOADING, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_BTN_MEDIA_DOWNLOADED, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_BTN_MEDIA_DOWNLOADFAILED, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_MEDIA_RENAME_CATE, OnUpdateRibbonBtn)


END_MESSAGE_MAP()


/// CMediaLibraryPage Message handle
int CMediaLibraryPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (!CreateRibbonButtons())
    {
        TRACE0("Failed to create MediaLibrary management ribbon buttons\n");
        return -1;      // fail to create
    }

    if (CBaseControllerPage::OnCreate(lpCreateStruct) == -1)
        return -1;

        // enable Visual Studio 2005 style docking window behavior
    CDockingManager::SetDockingMode(DT_SMART);
    // enable Visual Studio 2005 style docking window auto-hide behavior
    EnableAutoHidePanes(CBRS_ALIGN_ANY);

    CRect rect;
    GetClientRect(&rect);

    int left    = 0;
    int right   = 0;
    int top     = 0;
    int bottom  = 0;

    // Create and Install the Media List pane
    left    = rect.Width() / 4;
    right   = rect.right;
    bottom  = rect.bottom;

    if (!m_wndMediaLibraryList.Create(Translate(_T("Media List")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_MEDIALIBRARY_LIST, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create Media List pane\n");
        return -1;      // fail to create
    }

    SetMainWnd(&m_wndMediaLibraryList);

    // Create and Install the MediaLibrary info view pane
    left    = 0;
    right   = rect.Width() / 4;
    top     = 0;
    bottom  = rect.bottom;

    if (!m_wndMediaLibraryCategory.Create(Translate(_T("Media Library Category Tree")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_MEDIALIBRARY_CATEGORY, WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create MediaLibrary Category Tree\n");
        return FALSE;      // fail to create
    }
    m_wndMediaLibraryCategory.EnableDocking(CBRS_LEFT);
    DockPane(&m_wndMediaLibraryCategory);


    // Create and Install the Media Info pane
    left    = rect.Width() / 4;
    top     = rect.bottom - CLIENT_FILTER_HEIGHT ;
    bottom  = rect.bottom;
    right   = rect.right;

    if (!m_wndMediaInfo.Create(Translate(_T("Media Info")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_MEDIALIBRARY_INFO, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, NULL))
    {
        TRACE0("Failed to create Media info pane\n");
        return FALSE;      // fail to create
    }
    m_wndMediaInfo.EnableDocking(CBRS_BOTTOM);
    DockPane(&m_wndMediaInfo);
 
    SetTimer(UPLIST_TIMER, 500, 0);
	SetTimer(MEDIALIST_REFRESH_TIMER,2000,0);
    this->RecalcLayout();
    return 0;
}


void CMediaLibraryPage::OnDestroy()
{
    CBaseControllerPage::OnDestroy();

}

void CMediaLibraryPage::Close()
{
    m_wndMediaLibraryCategory.DestroyWindow();
    m_wndMediaLibraryList.DestroyWindow();
    m_wndMediaInfo.DestroyWindow();


}


BOOL CMediaLibraryPage::CreateRibbonButtons()
{
    if (!m_pCategory)
    {
        return FALSE;
    }

    /// Remove the dummy panel, and create the real panels
    m_pCategory->RemovePanel(0);

	CString strPanelPreview            = Translate(_T("Preview"));
	CString strPanelApproval			= Translate(_T("Approval"));
	CString strPanelFile			= Translate(_T("File"));
    CString strPanelView            = Translate(_T("View"));

    // AfxGetApp()->LoadIconW(IDI_EMPTYCONTENT)

    CMFCRibbonPanel* pPanelFile = m_pCategory->AddPanel(strPanelFile);

#ifdef PROFESSIONAL_EDITION

	CMFCRibbonButton* pBtnUploadFile = new CMFCRibbonButton(ID_RIBBON_BTN_MEDIA_UPLOADFILE,  Translate(_T("Add Media")), 5, 5);
	pBtnUploadFile->SetAlwaysLargeImage();
	pBtnUploadFile->SetToolTipText(Translate(_T("Add Media")));
    pBtnUploadFile->SetDescription(Translate(_T("Add media file to media library.")));
	pPanelFile->Add(pBtnUploadFile);

    CMFCRibbonButton* pBtnUploadURL = new CMFCRibbonButton(ID_RIBBON_BTN_MEDIA_UPLOADURL,  Translate(_T("Add URL")), 5, 5);
    pBtnUploadURL->SetAlwaysLargeImage();
    pBtnUploadURL->SetToolTipText(Translate(_T("Add URL")));
    pBtnUploadURL->SetDescription(Translate(_T("Add a url to media library.")));
    pPanelFile->Add(pBtnUploadURL);

#else

    CMFCRibbonButton* pBtnUploadFile = new CMFCRibbonButton(ID_RIBBON_BTN_MEDIA_UPLOADFILE,  Translate(_T("Upload Media")), 5, 5);
    pBtnUploadFile->SetAlwaysLargeImage();
    pBtnUploadFile->SetToolTipText(Translate(_T("Upload Media")));
    pBtnUploadFile->SetDescription(Translate(_T("Upload media file from local to media server.")));
    pPanelFile->Add(pBtnUploadFile);

    CMFCRibbonButton* pBtnUploadURL = new CMFCRibbonButton(ID_RIBBON_BTN_MEDIA_UPLOADURL,  Translate(_T("Add URL")), 5, 5);
    pBtnUploadURL->SetAlwaysLargeImage();
    pBtnUploadURL->SetToolTipText(Translate(_T("Add URL")));
    pBtnUploadURL->SetDescription(Translate(_T("Add a url from local to media server.")));
    pPanelFile->Add(pBtnUploadURL);

    CMFCRibbonButton* pBtnSync = new CMFCRibbonButton(ID_RIBBON_BTN_MEDIA_SYNCFILE,  Translate(_T("Download")),6, 6);
    pBtnSync->SetAlwaysLargeImage();
    pBtnSync->SetToolTipText(Translate(_T("Download")));
    pBtnSync->SetDescription(Translate(_T("Download file from media server to local.")));
    pPanelFile->Add(pBtnSync);

    CMFCRibbonButton* pBtnForceSync = new CMFCRibbonButton(ID_RIBBON_BTN_MEDIA_FORCESYNCFILE,  Translate(_T("Force Download")),7, 7);
    pBtnForceSync->SetAlwaysLargeImage();
    pBtnForceSync->SetToolTipText(Translate(_T("Force Download")));
    pBtnForceSync->SetDescription(Translate(_T("Force download file from media server to local.")));
    pPanelFile->Add(pBtnForceSync);

#endif

	CMFCRibbonButton* pBtnDel = new CMFCRibbonButton(ID_RIBBON_BTN_MEDIA_DELFILE,  Translate(_T("Delete")),8, 8);
	pBtnDel->SetAlwaysLargeImage();
	pBtnDel->SetToolTipText(Translate(_T("Delete")));
    pBtnDel->SetDescription(Translate(_T("Delete the selected file(s) from media server.")));
	pPanelFile->Add(pBtnDel);

    CMFCRibbonButton* pBtnRenameCategory = new CMFCRibbonButton(ID_RIBBON_BTN_MEDIA_RENAME_CATE,  Translate(_T("Rename")), 9, 9);
    pBtnRenameCategory->SetAlwaysLargeImage();
    pBtnRenameCategory->SetToolTipText(Translate(_T("Rename")));
    pBtnRenameCategory->SetDescription(Translate(_T("Rename the selected file.")));
    pPanelFile->Add(pBtnRenameCategory);


#ifndef PROFESSIONAL_EDITION

    CMFCRibbonPanel* pPanelApproval = m_pCategory->AddPanel(strPanelApproval);

    CMFCRibbonButton* pBtnApprove = new CMFCRibbonButton(ID_RIBBON_BTN_MEDIA_APPROVE,  Translate(_T("Approve")), 1, 1);
    pBtnApprove->SetAlwaysLargeImage();
    pBtnApprove->SetToolTipText(Translate(_T("Approve")));
    pBtnApprove->SetDescription(Translate(_T("Approve file to publish.")));
    pPanelApproval->Add(pBtnApprove);

    CMFCRibbonButton* pBtnReject = new CMFCRibbonButton(ID_RIBBON_BTN_MEDIA_REJECT,  Translate(_T("Reject")), 2, 2);
    pBtnReject->SetAlwaysLargeImage();
    pBtnReject->SetToolTipText(Translate(_T("Reject")));
    pBtnReject->SetDescription(Translate(_T("Reject file to publish.")));
    pPanelApproval->Add(pBtnReject);

    CMFCRibbonButton* pBtnRetire = new CMFCRibbonButton(ID_RIBBON_BTN_MEDIA_RETIREFILE,  Translate(_T("Retire")), 4, 4);
    pBtnRetire->SetAlwaysLargeImage();
    pBtnRetire->SetToolTipText(Translate(_T("Retire")));
    pBtnRetire->SetDescription(Translate(_T("Retire file to publish."))); 
    pPanelApproval->Add(pBtnRetire);

#endif

    CMFCRibbonPanel* pPanelPreview = m_pCategory->AddPanel(strPanelPreview);

  	CMFCRibbonButton* pBtnPreview = new CMFCRibbonButton(ID_RIBBON_BTN_MEDIA_PREVIEW,  Translate(_T("Preview")), 0, 0);
	pBtnPreview->SetAlwaysLargeImage();
	pBtnPreview->SetToolTipText(Translate(_T("Preview")));
    pBtnPreview->SetDescription(Translate(_T("Preview the selected file now.")));
	pPanelPreview->Add(pBtnPreview);

    CMFCRibbonPanel* pPanelView = m_pCategory->AddPanel(strPanelView);

    CMFCRibbonButton* pBtnView = new CMFCRibbonButton(ID_RIBBON_BTN_MEDIA_REFRESH,  Translate(_T("Refresh")), 13, 13);
    pBtnView->SetAlwaysLargeImage();
    pBtnView->SetToolTipText(Translate(_T("Refresh")));
    pBtnView->SetDescription(Translate(_T("Refresh media file list.")));
    pPanelView->Add(pBtnView);

    return TRUE;
}


void CMediaLibraryPage::Refresh()
{
    INT tabIndex = m_wndMediaLibraryList.m_wndStateTab.GetActiveTab();
    if (tabIndex == MEDIA_LIST_TAB1)
    {
        SendMessage(MEDIALIB_SEARCH, 0L, 0L);
    }
    else if(tabIndex == UPLOAD_LIST_TAB2)
    {
        SendMessage(MEDIA_UPLOAD_LIST, 0L, 0L);
    }
#ifndef PROFESSIONAL_EDITION
    else if(tabIndex == DOWNLOAD_LIST_TAB3)
    {
          SendMessage(MEDIA_DOWNLOAD_LIST, 0L, 0L);
    }
#endif
    else if (tabIndex == CATEGORY_LIST_TAB4)
    {
     //   SendMessage(MEDIA_CATEGORY_REFRESH, 0L,  0L);
		//refresh media list, count and max(id)
		m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.RefreshMediaGrid();
    }

}

void CMediaLibraryPage::GetSelectItem(std::vector<int>& mediaIDList)
{
    INT tabIndex = m_wndMediaLibraryList.m_wndStateTab.GetActiveTab();
	if (tabIndex == CATEGORY_LIST_TAB4)
	{
		m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.GetSelectedMultiID(mediaIDList);
	}
	else
	{
		CListCtrlEx* pList = m_wndMediaLibraryList.m_wndStateTab.m_MainList[tabIndex];
		POSITION pos = pList->GetFirstSelectedItemPosition();


		while(pos)
		{
			int nItem = pList->GetNextSelectedItem(pos);
			int mediaID = pList->GetItemData(nItem);
			mediaIDList.push_back(mediaID);
		}
	} 
}


void CMediaLibraryPage::OnRibbonBtnPreview()
{
	CString MediaLibraryRoot;
    vector<INT> MediaIDs;
    m_wndMediaLibraryList.m_wndStateTab.GetSelectedMediaIDList(MediaIDs);


    if (MediaIDs.size() < 1)
    {
        MessageBox(Translate(_T("Please select media firstly!")), Translate(_T("Warning:Preview media")) ,MB_OK|MB_ICONEXCLAMATION);
        return;
    }
    //else if (MediaIDs.size() > 3)
	else if (MediaIDs.size() > 1)
    {
        MessageBox(Translate(_T("It's better that not preview so many medias at one time!")), Translate(_T("Tips:Preview media")) ,MB_OK|MB_ICONINFORMATION);
        return;
    }
    else
    {
        MediaLibraryRoot = GetMediaLibraryInterface->GetMediaLibraryRoot().c_str();

#ifndef SERVER_EDITION
        MediaLibraryRoot = g_MediaServer().GetServerHomeDir().c_str();
#endif

        if (!::PathFileExists(MediaLibraryRoot))
        {
            MessageBox(Translate(_T("Can not find the media library root directory!")), Translate(_T("Tips:Preview media")) ,MB_OK|MB_ICONINFORMATION);
            return;
        }

        MEDIA_LIST mediaList;
        mediaList.m_MediaIDs = MediaIDs;

        MEDIA_DETAIL_INFO mediaInfo;
        HRESULT hr = GetMediaLibraryInterface->GetMedia(mediaList, mediaInfo);

        BOOL bAllSynced = TRUE;
        for (auto it = mediaInfo.m_Medias.begin(); it!= mediaInfo.m_Medias.end(); it++)
        {
            if (it->second.Percent < 100)
            {
                bAllSynced = FALSE;
                continue;
            }

            CString szMediaPath = MediaLibraryRoot + it->second.MediaServerPath.c_str();
            int err = 33;
            if (it->second.MediaType == MEDIA_URL)
            {
                err = reinterpret_cast<int>(ShellExecute(GetSafeHwnd(), _T("open"), it->second.Name.c_str(), NULL, NULL, SW_SHOWNORMAL));
            }
            else
            {
                err = reinterpret_cast<int>(ShellExecute(GetSafeHwnd(), _T("open"), szMediaPath, it->second.Name.c_str(), NULL, SW_SHOWNORMAL));
            }
            if (err <= 32)
            {
                MessageBox(Translate(_T("Can't preview media: ")) + it->second.Name.c_str(),Translate(_T("Tips:Preview media")), MB_ICONINFORMATION|MB_OK);
            }
        }

        if (!bAllSynced)
        {
#ifndef SERVER_EDITION
            MessageBox(Translate(_T("The media is not synced, not be able to be previewed!")),Translate(_T("Tips:Preview media")), MB_ICONINFORMATION|MB_OK);
#else
			PreviewDownLoadDlg dlg;

			// collect exported layout list
			DWORD64 FreeRemain = GetMediaLibraryInterface->GetMediaLibraryRootFreeSpace();

			DWORD64 LayoutSize = mediaInfo.m_TotalSize;
			DWORD64 SizeStillNeedDownload = mediaInfo.m_TotalSize - mediaInfo.m_FinishedSize;
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
				return;
			}

			if (!dlg.AddMediaListJob(mediaList))
			{
				ShowXMessageBox(NULL,Translate(_T("Media Library failed to add download task, some media used in the layout may be not downloaded!")),Translate(_T("Error:Preview layout")), MB_OK|MB_ICONERROR);
			}

			if (dlg.DoModal() != IDOK || dlg.m_Progress< PROGRESS_SLICE)
			{
				return;
			}

			//preview media
			auto it = mediaInfo.m_Medias.begin();
            CString szMediaPath = MediaLibraryRoot + it->second.MediaServerPath.c_str();

			int err = 33;
			err = reinterpret_cast<int>(ShellExecute(GetSafeHwnd(), _T("open"), szMediaPath, it->second.Name.c_str(), NULL, SW_SHOWNORMAL));
			if (err <= 32)
			{
				MessageBox(Translate(_T("Can't preview media: ")) + it->second.Name.c_str(),Translate(_T("Tips:Preview media")), MB_ICONINFORMATION|MB_OK);
			}
#endif
        }

    }
}

void CMediaLibraryPage::OnRibbonBtnApprove()
{
    std::vector<Media> mediaList;
    std::vector<int> mediaIDList;
    GetSelectItem(mediaIDList);

    HRESULT hr = GetControllerInterface->GetMedias(mediaIDList, mediaList);
    if (!SUCCEEDED(hr))
    {
        MessageBox(Translate(_T("Failed to get the Media!")), Translate(_T("Error:Media library page")), MB_ICONERROR);
        return;
    }
    auto it = mediaList.begin(), ite = mediaList.end();
    Audit tmpAudit;

    for (; it != ite; ++it)
    {
        tmpAudit = it->GetAudit();
        Audit tmp(tmpAudit.GetRequestUserID(), tmpAudit.GetRequestTime(), GetControllerInterface->GetUserId(),
            S3Time::Now(), tmpAudit.CovertStatus(Audit::APPROVED));

        it->SetAudit(tmp);
        // modify controller server data
        hr = GetControllerInterface->AuditMedia(it->GetMeidaID(), it->GetAudit());
        if (!SUCCEEDED(hr))
        {
            MessageBox(Translate(_T("Failed to Approve the media!")), Translate(_T("Error:Media library page")), MB_ICONERROR);
            return;
        }
    }
    // refresh tab list
    if (m_wndMediaLibraryList.m_wndStateTab.GetActiveTab() == CATEGORY_LIST_TAB4)
    {
		m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.RefreshPartGrid();
    }
	else
	{
		Refresh();
	}
    if(0x01 == mediaIDList.size())
    {
        SendMessage(MEDIA_SHOW_INFO, 0L, (LPARAM)((mediaList.begin())->GetMeidaID()));
    }
}

void CMediaLibraryPage::OnRibbonBtnReject()
{
    std::vector<Media> mediaList;
    std::vector<int> mediaIDList;
    GetSelectItem(mediaIDList);

	std::vector<int> layoutIDList;

	if(mediaIDList.size())
	{
		for(auto iter = mediaIDList.begin() ; iter != mediaIDList.end(); ++iter )
		{
			GetControllerInterface->GetMediaRefInLayout(*iter, layoutIDList);
			if(layoutIDList.size())
			{
				if(MessageBox(Translate(_T("The media is used, do you want to continue?")), Translate(_T("Warning:Media library page")), MB_YESNO|MB_ICONEXCLAMATION) != IDYES)
				{
					return;
				}
				else
				{
					break;
				}
			}
		}
	}


    CRejectReason dlg(this);

    if (IDOK == dlg.DoModal())
    {
        UpdateData(TRUE);   

        HRESULT hr = GetControllerInterface->GetMedias(mediaIDList, mediaList);
        if (!SUCCEEDED(hr))
        {
            MessageBox(Translate(_T("Failed to get the media!")), Translate(_T("Error:Media library page")), MB_ICONERROR);
            return;
        }
        auto it = mediaList.begin(), ite = mediaList.end();
        Audit tmpAudit;
        for (; it != ite; ++it)
        {
            tmpAudit = it->GetAudit();
            Audit tmp(tmpAudit.GetRequestUserID(), tmpAudit.GetRequestTime(), GetControllerInterface->GetUserId(),
                S3Time::Now(), tmpAudit.CovertStatus(Audit::REJECTED));

            tmp.SetComment(dlg.m_RejectReason.GetString());
            it->SetAudit(tmp);
            // modify controller server data
            hr = GetControllerInterface->AuditMedia(it->GetMeidaID(), it->GetAudit());
            if (!SUCCEEDED(hr))
            {
                MessageBox(Translate(_T("Failed to reject the media!")), Translate(_T("Error:Media library page")), MB_ICONERROR);
                return;
            }
        }

		if (m_wndMediaLibraryList.m_wndStateTab.GetActiveTab() == CATEGORY_LIST_TAB4)
		{
			m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.RefreshPartGrid();
		}
		else
		{
			Refresh();
		}
        if(0x01 == mediaIDList.size())
        {
            SendMessage(MEDIA_SHOW_INFO, 0L, (LPARAM)((mediaList.begin())->GetMeidaID()));
        }
    }
}

void CMediaLibraryPage::OnRibbonBtnRequest()
{
    std::vector<Media> mediaList;
    std::vector<int> mediaIDList;
    GetSelectItem(mediaIDList);

    HRESULT hr = GetControllerInterface->GetMedias(mediaIDList, mediaList);
    if (!SUCCEEDED(hr))
    {
         MessageBox(Translate(_T("Failed to get the media!")), Translate(_T("Error:Media library page")), MB_ICONERROR);
        return;
    }
    auto it = mediaList.begin(), ite = mediaList.end();
    Audit tmpAudit;
    for (; it != ite; ++it)
    {
        tmpAudit = it->GetAudit();
        Audit tmp(GetControllerInterface->GetUserId(), S3Time::Now(), tmpAudit.GetApproveUserID(),
            tmpAudit.GetApproveTime(), tmpAudit.CovertStatus(Audit::CREATEBUTNOTAPPROVED)); // request

        it->SetAudit(tmp);
        // modify controller server data
        hr = GetControllerInterface->EditMedia(*it);
        if (!SUCCEEDED(hr))
        {
            MessageBox(Translate(_T("Failed to request the media!")), Translate(_T("Error:Media library page")), MB_ICONERROR);
            return;
        }
    }
   Refresh();
   if(0x01 == mediaIDList.size())
    {
        SendMessage(MEDIA_SHOW_INFO, 0L, (LPARAM)((mediaList.begin())->GetMeidaID()));
    }
}

void CMediaLibraryPage::OnRibbonBtnUploadFile()
{
  CFileDialog uploadDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT ,
      L"All Support Files(*.avi;*.wmv;*.asf;*.mkv;*.mp4;*.flv;*.wma;*.mp3;*.wav;*.jpg;*.tif;*.tiff;*.bmp;*.gif;*.png;*.swf;*.htm;*.html;*.mht;*.ppt;*.pps)|"
                          L"*.avi;*.wmv;*.asf;*.mkv;*.mp4;*.flv;*.wma;*.mp3;*.wav;*.jpg;*.tif;*.tiff;*.bmp;*.gif;*.png;*.swf;*.htm;*.html;*.mht;*.ppt;*.pps|"
        L"Video Files(*.avi;*.wmv;*.asf;*.mkv;*.mp4;*.flv)|"
                    L"*.avi;*.wmv;*.asf;*.mkv;*.mp4;*.flv|"
        L"Audio Files(*.wma;*.mp3;*.wav)|*.wma;*.mp3;*.wav|"
        L"Picture Files(*.jpg;*.tif;*.tiff;*.bmp;*.gif;*.png)|"
                      L"*.jpg;*.tif;*.tiff;*.bmp;*.gif;*.png|"
        L"Flash Files(*.swf)|*.swf|"
        L"html Files(*.htm;*.html;*.mht)|*.htm;*.html;*.mht|"
        L"Power Point Files(*.ppt;*.pps)|*.ppt;*.pps|", this);

    CMediaLibraryCategoryCtrl::ITEM_TYPE ItemType;
    INT CategoryID = m_wndMediaLibraryCategory.m_MediaLibraryCategory.GetSelectedCategory(ItemType);

    vector<CString> LocalFiles;

    TCHAR psBuffer[60000];
    psBuffer[0]                     = _T('\0');
    uploadDialog.m_ofn.lpstrFile    = psBuffer;
    uploadDialog.m_ofn.nMaxFile     = 60000;

    if (uploadDialog.DoModal() == IDOK)
    {
        //get category path
        POSITION pos = uploadDialog.GetStartPosition();
        while (pos != NULL)
        {
            CString filePath = uploadDialog.GetNextPathName(pos);
            LocalFiles.push_back(filePath);
            boost::filesystem3::path path(filePath);
            boost::system::error_code ec;
            if (!boost::filesystem3::is_regular_file(path, ec) || ec)
            {
                MessageBox(Translate(_T("File not found!")), Translate(_T("Error:Upload file")), MB_ICONERROR|MB_OK);
                return;
            }
        }
    }
    else
    {
        return;
    }

    //How to new a buf and post it to work thread
    UINT nBufID;
    pair<INT, vector<CString> >* pInfo = WorkThreadDataManager< pair<INT, vector<CString> > >::NewDataBuf(nBufID); //create dynamic msg
    if (pInfo == NULL)
    {
        MessageBox(Translate(_T("Failed to init upload task!")), Translate(_T("Error:Upload file")), MB_ICONERROR|MB_OK);
        return;
    }

    pInfo->first = CategoryID;
    pInfo->second = LocalFiles;

    DWORD MessageID = USER_MSG_UPLOAD_MEDIA;

    if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,
        (WPARAM)this->GetSafeHwnd(), (LPARAM)nBufID))//post thread msg
    {
        TRACE1("post message failed,error:%d\n",::GetLastError());
    }

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("Initializing upload tasks......")).GetString());
}

LRESULT CMediaLibraryPage::OnMediaUpload(WPARAM wp, LPARAM lp)
{
    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf< pair<BOOL, CString> > MessageBuf;
    if (!WorkThreadDataManager<pair<BOOL, CString> >::GetDataBuf(nMessageBufID, MessageBuf))
    {
          return 1;
    }
    //////////////////////////////////////////////////////////////////////////

    const CString& InfoMsg = MessageBuf.Get()->second;
    if (MessageBuf.Get()->first)
    {
        //set active upload tab list
        if (!m_wndMediaLibraryList.m_wndStateTab.SetActiveTab(UPLOAD_LIST_TAB2))
        {
            TRACE0("Set Upload list as active tab fail!\n");
        }
        SendMessage(MEDIA_UPLOAD_LIST, 0L,  0L);
    }

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)InfoMsg.GetString());

    //AfxMessageBox(InfoMsg);

    return 0;

}

void CMediaLibraryPage::OnRibbonBtnSyncFile()
{
	//download media file from media server
     MEDIA_LIST syncMedia;

     GetSelectItem(syncMedia.m_MediaIDs);

     HRESULT hr = GetMediaLibraryInterface->DownloadMedia(syncMedia);
     if (!SUCCEEDED(hr))
     {
         TRACE0("Failed to sync media!\n");
         MessageBox(Translate(_T("Could not connect to media server, Failed to add download task!")), Translate(_T("Error:Download task")), MB_ICONERROR);
         return;
     }

#ifndef PROFESSIONAL_EDITION
     if (!m_wndMediaLibraryList.m_wndStateTab.SetActiveTab(DOWNLOAD_LIST_TAB3))
     {
         TRACE0("Set Download list as active tab fail!\n");
     }
     SendMessage(MEDIA_DOWNLOAD_LIST, 0L,  0L);
#endif
}

void CMediaLibraryPage::OnRibbonBtnForceSyncFile()
{
    //download media file force
    MEDIA_LIST syncMedia;

    GetSelectItem(syncMedia.m_MediaIDs);

    HRESULT hr = GetMediaLibraryInterface->DownloadMedia(syncMedia, MediaLibrary::TASK_FORECE);
    if (!SUCCEEDED(hr))
    {
        TRACE0("Failed to sync media!\n");
        MessageBox(Translate(_T("Could not connect to media server, Failed to add download task!")), Translate(_T("Error:Download task")), MB_ICONERROR);
        return;
    }
#ifndef PROFESSIONAL_EDITION
    if (!m_wndMediaLibraryList.m_wndStateTab.SetActiveTab(DOWNLOAD_LIST_TAB3))
    {
        TRACE0("Set Download list as active tab fail!\n");
    }
     SendMessage(MEDIA_DOWNLOAD_LIST, 0L, 0L);
#endif
}

void CMediaLibraryPage::OnRibbonBtnDeleteFile()
{
	//delete the selected files on media_list_tab1
    //get mediaDelTask
    if (MessageBox(Translate(_T("Are you sure to delete the selected files?")),Translate(_T("Tips:Delete file")) ,MB_YESNO|MB_ICONQUESTION) != IDYES)
    {
        return;
    }

    CWaitCursor wait;

    MEDIA_LIST mediaDelTask;

    GetSelectItem(mediaDelTask.m_MediaIDs);

    MEDIA_LIST VarifiedMediaDelTask;
    auto it = mediaDelTask.m_MediaIDs.begin(), ite = mediaDelTask.m_MediaIDs.end();
    for (; it != ite; it++)
    {
        vector<INT> LayoutIDs;
        HRESULT hr = GetControllerInterface->GetMediaRefInLayout(*it, LayoutIDs);
        if (LayoutIDs.empty())
        {
            VarifiedMediaDelTask.AddMedia(*it);
        }
    }

    HRESULT hr = GetMediaLibraryInterface->ClearLocalMedia(VarifiedMediaDelTask);
    if (!SUCCEEDED(hr))
    {
        TRACE0("Failed to clear local media!\n");
    }
    hr = GetMediaLibraryInterface->DeleteServerMedia(VarifiedMediaDelTask);
    if (!SUCCEEDED(hr))
    {
        TRACE0("Failed to delete server media!\n");
        MessageBox(Translate(_T("Failed to delete server media data")), Translate(_T("Error:Delete file")) ,MB_OK|MB_ICONERROR);
    }

    INT nGap = mediaDelTask.m_MediaIDs.size() - VarifiedMediaDelTask.m_MediaIDs.size();
    if (nGap > 0)
    {
        CString Info;
        Info.Format(Translate(_T("Failed to delete %d media(s), make sure it is not used in any layout!")), nGap);
        MessageBox(Info,Translate(_T("Error:Delete file")) ,MB_OK|MB_ICONERROR);
    }

    Refresh();
}

void CMediaLibraryPage::OnRibbonBtnRetireFile()
{
	//
    std::vector<Media> mediaList;
    std::vector<int> mediaIDList;
    GetSelectItem(mediaIDList);

    HRESULT hr = GetControllerInterface->GetMedias(mediaIDList, mediaList);
    if (!SUCCEEDED(hr))
    {
        MessageBox(Translate(_T("Failed to get the media!")), Translate(_T("Error:Media library page")), MB_ICONERROR);
        return;
    }
    auto it = mediaList.begin(), ite = mediaList.end();
    Audit tmpAudit;
    for (; it != ite; ++it)
    {
        tmpAudit = it->GetAudit();
        Audit tmp(tmpAudit.GetRequestUserID(), tmpAudit.GetRequestTime(), tmpAudit.GetApproveUserID(),
            tmpAudit.GetApproveTime(), tmpAudit.CovertStatus(Audit::RETIRED));
        it->SetAudit(tmp);
        // modify controllerserver data
        hr = GetControllerInterface->EditMedia(*it);
        if (!SUCCEEDED(hr))
        {
            MessageBox(Translate(_T("Failed to reject the layout!")), Translate(_T("Error:Media library page")), MB_ICONERROR);
            return;
        }
    }
	if (m_wndMediaLibraryList.m_wndStateTab.GetActiveTab() == CATEGORY_LIST_TAB4)
	{
		m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.RefreshPartGrid();
	}
	else
	{
		Refresh();
	}
    if(0x01 == mediaIDList.size())
    {
        SendMessage(MEDIA_SHOW_INFO, 0L, (LPARAM)((mediaList.begin())->GetMeidaID()));
    }
}

void CMediaLibraryPage::OnRibbonBtnAddWorkspace()
{
	MessageBox(Translate(_T("yes")),Translate(_T("Workspace")) ,MB_OK);
}

void CMediaLibraryPage::OnRibbonBtnSyncWorkspace()
{
	MessageBox(Translate(_T("yes")),Translate(_T("Workspace")) ,MB_OK);
}

void CMediaLibraryPage::OnRibbonBtnStartUpload()
{
	//start upload the select media

    MEDIA_LIST Task;

    GetSelectItem(Task.m_MediaIDs);

    HRESULT hr = GetMediaLibraryInterface->StartTask(Task);
    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {

        }
        else
        {
            TRACE0("Failed to StartTask Upload!\n");
        }
    }
}

void CMediaLibraryPage::OnRibbonBtnPauseUpload()
{

    MEDIA_LIST Task;

    GetSelectItem(Task.m_MediaIDs);

    HRESULT hr = GetMediaLibraryInterface->PauseTask(Task);
    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {

        }
        else
        {
            TRACE0("Failed to PauseTask Upload!\n");
        }
    }
}

void CMediaLibraryPage::OnRibbonBtnDeleteUpload()
{
    if (MessageBox(Translate(_T("Are you sure to delete the selected files from upload task?")), Translate(_T("Tips:Delete upload list")) ,MB_YESNO|MB_ICONQUESTION) != IDYES)
    {
        return;
    }

    CWaitCursor waitcursor;

    MEDIA_LIST Task;

    GetSelectItem(Task.m_MediaIDs);

    HRESULT hr = GetMediaLibraryInterface->DeleteTask(Task);
    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {

        }
        else
        {
            TRACE0("Failed to DeleteTask Upload!\n");
        }
    }

    OnMediaLibraryUploadList(NULL, NULL);
}

void CMediaLibraryPage::OnRibbonBtnStartDownload()
{
    MEDIA_LIST Task;

    GetSelectItem(Task.m_MediaIDs);

    HRESULT hr = GetMediaLibraryInterface->StartTask(Task);
    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {

        }
        else
        {
            TRACE0("Failed to StartTask Download!\n");
        }
    }

}

void CMediaLibraryPage::OnRibbonBtnPauseDownload()
{
    MEDIA_LIST Task;

    GetSelectItem(Task.m_MediaIDs);

    HRESULT hr = GetMediaLibraryInterface->PauseTask(Task);
    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {

        }
        else
        {
            TRACE0("Failed to PauseTask Download!\n");
        }
    }
}

void CMediaLibraryPage::OnRibbonBtnDeleteDownload()
{
    if (MessageBox(Translate(_T("Are you sure to delete the selected files from download tasks?")),Translate(_T("Tips:Delete download list")), MB_YESNO|MB_ICONQUESTION) != IDYES)
    {
        return;
    }

    CWaitCursor waitcursor;

    MEDIA_LIST Task;

    GetSelectItem(Task.m_MediaIDs);

    HRESULT hr = GetMediaLibraryInterface->DeleteTask(Task);
    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {

        }
        else
        {
            TRACE0("Failed to DeleteTask Download!\n");
        }
    }

    OnMediaLibraryDownloadList(NULL, NULL);
}

void CMediaLibraryPage::OnUploadingList()
{
	m_bUploadingList = !m_bUploadingList;
    SendMessage(MEDIA_UPLOAD_LIST, 0L,  0L);
}

void CMediaLibraryPage::OnUploadedList()
{
	m_bUploadedList = !m_bUploadedList;
    SendMessage(MEDIA_UPLOAD_LIST, 0L,  0L);
}

void CMediaLibraryPage::OnUploadFailList()
{
	m_bUploadFailList = !m_bUploadFailList;
    SendMessage(MEDIA_UPLOAD_LIST, 0L,  0L);
}

void CMediaLibraryPage::OnDownloadingList()
{
	m_bDownloadingList = !m_bDownloadingList;
    SendMessage(MEDIA_DOWNLOAD_LIST, 0L,  0L);
}

void CMediaLibraryPage::OnDownloadedList()
{
	m_bDownloadedList = !m_bDownloadedList;
    SendMessage(MEDIA_DOWNLOAD_LIST, 0L,  0L);
}

void CMediaLibraryPage::OnDownloadFailList()
{
	m_bDownloadFailList = !m_bDownloadFailList;
    SendMessage(MEDIA_DOWNLOAD_LIST, 0L,  0L);
}

void CMediaLibraryPage::OnRenameCategoryList()
{
     std::tstring  newName;
     int mediaID;

     CString szExt;
     int n = -1;

      int indexList = m_wndMediaLibraryList.m_wndStateTab.GetActiveTab();
	  if (indexList == CATEGORY_LIST_TAB4)
	  {
		  mediaID = m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.GetSelectRowMediaID();
	  }
	  else
	  {
		  CListCtrlEx* pList = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList];
		  POSITION pos = pList->GetFirstSelectedItemPosition();
		  if(pos)
		  {
			  int nItem = pList->GetNextSelectedItem(pos);
			  mediaID = pList->GetItemData(nItem);
		  }
	  }
     //get selected item media name
     
     std::vector<int> mediaIDList;
     std::vector<Media> mediaList;
     mediaIDList.push_back(mediaID);

     HRESULT hr = GetControllerInterface->GetMedias(mediaIDList, mediaList);

     if (!SUCCEEDED(hr))
     {
         TRACE0("Failed to get media\n");
         return;
     }
     auto it = mediaList.begin();
     std::tstring tmp = it->GetName();
     CString mediaNameExt(tmp.c_str());
     CString mediaName;
     if (mediaList[0].GetMediaType() != _T("URL"))
     {
         m_bIsUrl = FALSE;
     }
     else
     {
        m_bIsUrl = TRUE;
     }
     n = mediaNameExt.ReverseFind('.');
     if (n != -1)
     {
		  if(!m_bIsUrl)
			mediaName = mediaNameExt.Left(n);
		  else
			mediaName = mediaNameExt;
          szExt = mediaNameExt.Mid(n + 1);
     }
     if (mediaName.GetLength() == 0)
     {
         mediaName = tmp.c_str();
     }
     CRenameCategory dlg(m_bIsUrl, this);
     dlg.InitMedia(mediaName,mediaList[0].GetDesc().c_str());
     UpdateData(FALSE);

     if (IDOK == dlg.DoModal())
     {
         //edit media
		 if(!m_bIsUrl)
         {
			 newName = dlg.m_NewMediaName +  _T(".") + szExt;
         }
		 else
         {
             newName = dlg.m_header + dlg.m_NewUrlName;
             it->SetMediaServerPath(newName);
#ifdef SERVER_EDITION
             Audit audit = it->GetAudit();
             Audit newAudit(audit.GetRequestUserID(), audit.GetRequestTime(), audit.GetApproveUserID(),
                 audit.GetApproveTime(), Audit::CREATEBUTNOTAPPROVED);
             it->SetAudit(newAudit);
#endif
         }
         it->SetName(newName);

         hr = GetControllerInterface->EditMedia(*it);
         if (!SUCCEEDED(hr))
         {
             TRACE0("Failed to Edit Media\n");
             std::tstring desc;
             int err = GetControllerInterface->GetLastError(desc);
             MessageBox(Translate(_T("Failed to Edit Media.")) + _T(" ") +Translate(desc.c_str()), Translate(_T("Error:Rename category")), MB_OK|MB_ICONERROR);
             return;
         }
        if (indexList == CATEGORY_LIST_TAB4)
		{
			m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.RefreshPartGrid();
		}
        else if (indexList == MEDIA_LIST_TAB1)
            SendMessage(MEDIALIB_SEARCH, 0L,  0L); 

        SendMessage(MEDIA_SHOW_INFO, 0L, (LPARAM)(mediaID));
     }
}


void CMediaLibraryPage::OnRibbonBtnUploadURL()
{
    CString szURL;
	CString szUrlType;
    CUploadUrlDialog Dlg(this);
    if (Dlg.DoModal() == IDOK)
    {
        Dlg.GetLayoutNewName(szURL);
		Dlg.GetURLType(szUrlType);
    }
    else
    {
        return;
    }

    CMediaLibraryCategoryCtrl::ITEM_TYPE ItemType;
    INT CategoryID = m_wndMediaLibraryCategory.m_MediaLibraryCategory.GetSelectedCategory(ItemType);

    MediaFilter category;

    HRESULT hr = GetControllerInterface->GetMediaFilter(CategoryID, category);
    if (!SUCCEEDED(hr))
    {
        TRACE0("Failed to get media filter!\n");
        return;
    }
    else
    {
        MEDIA_UPLOAD_TASK uploadTask;
        MEDIA_DETAIL_INFO uploadTaskInfo;
        uploadTask.AddMedia(szURL.GetString(), category);

        GetMediaLibraryInterface->UploadURL(uploadTask,uploadTaskInfo,TRUE,szUrlType.GetString());
        SendMessage(MEDIA_UPLOAD_LIST, 0L,  0L);//notify ui to refresh task list-ctrl.

    }

}

LRESULT CMediaLibraryPage::OnMediaDlgSearch(WPARAM wp, LPARAM lp)
{
    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("MediaLibrary List Updating!")).GetString());

	return OnMediaLibrarySearch(wp,lp);
#if 0
    TCHAR buf[16];
    CString strID;
    //retrieve data from dialog
    CSearchFileDlg* pDlg = (CSearchFileDlg*)lp;

    CString Desc = pDlg->m_strDesc;

    CComboBox *pCB = &pDlg->m_ctlCategory;
    int CategoryID = pCB->GetItemData(pCB->GetCurSel());

    CComboBox *pCBowner = &pDlg->m_ctlOwner;
    int OwnerID = pCBowner->GetItemData(pCBowner->GetCurSel());

    CComboBox *pCBsize = &pDlg->m_ctlSize;
    int SizeId = pCBsize->GetItemData(pCBsize->GetCurSel());

    CComboBox *pCBapproval = &pDlg->m_ctlAudit;
    int ApprovalID = pCBapproval->GetItemData(pCBapproval->GetCurSel());

    CComboBox *pCBtype = &pDlg->m_ctlType;
    int TypeID = pCBtype->GetItemData(pCBtype->GetCurSel());

    CComboBox *pCBheight = &pDlg->m_ctlHeight;
    int HeightID = pCBheight->GetItemData(pCBheight->GetCurSel());

    CComboBox *pCBwidth = &pDlg->m_ctlWidth;
    int WidthID = pCBwidth->GetItemData(pCBwidth->GetCurSel());

    CComboBox *pCBdur = &pDlg->m_ctlDur;
    int DurID = pCBdur->GetItemData(pCBdur->GetCurSel());

    CString Conditons = _T(" Name like '%") + Desc + _T("%' ");

    if (CategoryID >=0)
    {
        _itot_s(CategoryID, buf, 10);
        strID = buf;
        Conditons += _T("and MediaFilterID = ") + strID;
    }
    if (OwnerID >= 0)
    {
        _itot_s(OwnerID, buf, 10);
        strID = buf;
        Conditons += _T(" and UploadUserID = ") + strID;
    }
    if (ApprovalID >= 0)
    {
        _itot_s(ApprovalID, buf, 10);
        strID = buf;
        Conditons += _T(" and Status = ") + strID;
    }
    if (SizeId >= 0)
    {
        switch(SizeId)
        {
        case 0:
            {
                Conditons += _T(" and FileSize = 0");
            }
            break;
        case 1:
            {
                Conditons += _T(" and FileSize > 0 and FileSize <= 10 * 1024 ");
            }
            break;
        case 2:
            {
                Conditons += _T(" and FileSize > 10*1024 and FileSize <= 100 * 1024 ");
            }
            break;
        case 3:
            {
                Conditons += _T(" and FileSize > 100 * 1024 and FileSize <= 1024 * 1024 ");
            }
            break;
        case 4:
            {
                Conditons += _T(" and FileSize > 1024 * 1024 and FileSize <= 16 * 1024 * 1024");
            }
            break;
        case 5:
            {
                Conditons += _T(" and FileSize > 16 * 1024 * 1024 and FileSize <= 128 * 1024 * 1024");
            }
            break;
        case 6:
            {
                Conditons += _T(" and FileSize > 128 * 1024 * 1024");
            }
            break;
        default:
            break;
        }
    }

    if (TypeID >= 0)
    {
        CString type;
        pCBtype->GetLBText(TypeID + 1, type);
       Conditons += _T(" and MediaType like '");
       Conditons += type;
       Conditons += ("'");
    }

    if (HeightID >= 0)
    {
        switch(HeightID)
        {
        case 0:
            {
                Conditons += _T(" and Height >= 0 and Height <= 128");
            }
            break;
        case 1:
            {
                Conditons += _T(" and Height > 128 and Height <= 256");
            }
            break;
        case 2:
            {
                Conditons += _T(" and Height > 256 and Height <= 512 ");
            }
            break;
        case 3:
            {
                Conditons += _T(" and Height > 512 and Height <= 1024 ");
            }
            break;
        case 4:
            {
                Conditons += _T(" and Height > 1024");
            }
            break;
        default:
            break;
        }
    }

    if (WidthID >= 0)
    {
        switch(WidthID)
        {
        case 0:
            {
                Conditons += _T(" and Width >= 0 and Width <= 128");
            }
            break;
        case 1:
            {
                Conditons += _T(" and Width > 128 and Width <= 256");
            }
            break;
        case 2:
            {
                Conditons += _T(" and Width > 256 and Width <= 512 ");
            }
            break;
        case 3:
            {
                Conditons += _T(" and Width > 512 and Width <= 1024 ");
            }
            break;
        case 4:
            {
                Conditons += _T(" and Width > 1024");
            }
            break;
        default:
            break;
        }
    }
    
    if (DurID >= 0)
    {
        switch(DurID)
        {
        case 0:
            {
                Conditons += _T(" and Duration >= 0 and Duration <= 60*1000");
            }
            break;
        case 1:
            {
                Conditons += _T(" and Duration > 60*1000 and Duration <= 300*1000");
            }
            break;
        case 2:
            {
                Conditons += _T(" and Duration > 300*1000 and Duration <= 1800*1000 ");
            }
            break;
        case 3:
            {
                Conditons += _T(" and Duration > 1800*1000 and Duration <= 3600*1000 ");
            }
            break;
        case 4:
            {
                Conditons += _T(" and Duration > 3600*1000");
            }
            break;
        default:
            break;
        }
    }

    if (pDlg->m_bUseDate)
    {
        S3Time startTime(S3Time::NULLTime), endTime(S3Time::OneDayAfter());
        ATL::COleDateTime t1, t2;
        COleDateTime TmpTimeStart =  pDlg->m_timeStart;
        COleDateTime TmpTimeEnd =  pDlg->m_timeEnd;

        t1.SetDateTime(TmpTimeStart.GetYear(),TmpTimeStart.GetMonth(), TmpTimeStart.GetDay(),0, 0,0);
        t2.SetDateTime(TmpTimeEnd.GetYear(), TmpTimeEnd.GetMonth(), TmpTimeEnd.GetDay(), 23,59,59);

        startTime = S3Time::CreateTime(t1);
        endTime = S3Time::CreateTime(t2);

        std::wstring tmpStart = startTime.ToString();
        std::wstring tmpEnd = endTime.ToString();
        CString tmp1(tmpStart.c_str());
        CString tmp2(tmpEnd.c_str());

        Conditons += _T(" and UploadTime between s3datetime('") + tmp1 + _T("') and s3datetime('") + tmp2 + _T("')");
    }

    Condition cond(Conditons.GetString());

    //How to new a buf and post it to work thread
    UINT nBufID;
    Condition* pInfo = WorkThreadDataManager< Condition >::NewDataBuf(nBufID); //create dynamic msg
    if (pInfo == NULL)
    {
        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)Translate(_T("Failed to search!")).GetString());

        return -1;
    }

    *pInfo = cond;

    DWORD MessageID = USER_MSG_SEARCH_MEDIA_LIST;

    if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,
        (WPARAM)this->GetSafeHwnd(), (LPARAM)nBufID))//post thread msg
    {
        TRACE1("post message failed,errno:%d\n",::GetLastError());

        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)Translate(_T("Failed to search!")).GetString());

        return -1;
    }

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("Searching......")).GetString());
#endif
    return 0;
}
LRESULT CMediaLibraryPage::OnMediaLibrarySearch (WPARAM, LPARAM)
{
    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("MediaLibrary List Updating!")).GetString());

    TCHAR buf[16];
    CString strID;
    //retrieve data from dialog
    MediaSearchOptions *pOptions = MediaSearchOptions::Instance();
    CString Desc = pOptions->_StrDesc;
    Desc.Replace(_T("%"), _T("\\%"));
    Desc.Replace(_T("_"), _T("\\_"));

    int CategoryID = pOptions->_CategoryID;
    int OwnerID =  pOptions->_OwnerID;
    int SizeId = pOptions->_SizeId;
    int TypeID = pOptions->_TypeID;
    int HeightID = pOptions->_HeightID;
    int WidthID = pOptions->_WidthID;
    int DurID = pOptions->_DurID;
    int ApprovalID =  pOptions->_ApprovalID;



    CString Conditons = _T(" lower(Name) like lower('%") + Desc + _T("%') ");

    if (OwnerID >= 0)
    {
        _itot_s(OwnerID, buf, 10);
        strID = buf;
        Conditons += _T("and UploadUserID = ") + strID;
    }
    if (ApprovalID >= 0)
    {
        _itot_s(ApprovalID, buf, 10);
        strID = buf;
        Conditons += _T(" and Status = ") + strID;
    }

    if (SizeId >= 0)
    {
        switch(SizeId)
        {
        case 0:
            {
                Conditons += _T(" and FileSize = 0");
            }
            break;
        case 1:
            {
                Conditons += _T(" and FileSize > 0 and FileSize <= 1024 * 1024 ");
            }
            break;
        case 2:
            {
                Conditons += _T(" and FileSize > 1024*1024 and FileSize <= 128 * 1024 * 1024 ");
            }
            break;
        case 3:
            {
                Conditons += _T(" and FileSize > 128 * 1024 * 1024 and FileSize <= 512 * 1024 * 1024 ");
            }
            break;
        case 4:
            {
                Conditons += _T(" and FileSize > 512 * 1024 * 1024 and FileSize <= 1024 * 1024 * 1024");
            }
            break;
        case 5:
            {
                Conditons += _T(" and FileSize > 1024 * 1024 * 1024 and FileSize <= 4 * 1024 * 1024 * 1024");
            }
            break;
        case 6:
            {
                Conditons += _T(" and FileSize > 4 * 1024 * 1024 * 1024");
            }
            break;
        default:
            break;
        }
    }


    if (TypeID >= 0)
    {
        CString type;
        type = MediaSearchOptions::Instance()->_strMediaType;
        Conditons += _T(" and lower(MediaType) like ");
        Conditons += _T("lower('");
        Conditons += type;
        Conditons += ("') ");
    }

    if (HeightID >= 0)
    {
        switch(HeightID)
        {
        case 0:
            {
                Conditons += _T(" and Height >= 0 and Height <= 128");
            }
            break;
        case 1:
            {
                Conditons += _T(" and Height > 128 and Height <= 256");
            }
            break;
        case 2:
            {
                Conditons += _T(" and Height > 256 and Height <= 512 ");
            }
            break;
        case 3:
            {
                Conditons += _T(" and Height > 512 and Height <= 1024 ");
            }
            break;
        case 4:
            {
                Conditons += _T(" and Height > 1024");
            }
            break;
        default:
            break;
        }
    }

    if (WidthID >= 0)
    {
        switch(WidthID)
        {
        case 0:
            {
                Conditons += _T(" and Width >= 0 and Width <= 128");
            }
            break;
        case 1:
            {
                Conditons += _T(" and Width > 128 and Width <= 256");
            }
            break;
        case 2:
            {
                Conditons += _T(" and Width > 256 and Width <= 512 ");
            }
            break;
        case 3:
            {
                Conditons += _T(" and Width > 512 and Width <= 1024 ");
            }
            break;
        case 4:
            {
                Conditons += _T(" and Width > 1024");
            }
            break;
        default:
            break;
        }
    }

    if (DurID >= 0)
    {
        switch(DurID)
        {
        case 0:
            {
                Conditons += _T(" and Duration >= 0 and Duration <= 60*1000");
            }
            break;
        case 1:
            {
                Conditons += _T(" and Duration > 60*1000 and Duration <= 300*1000");
            }
            break;
        case 2:
            {
                Conditons += _T(" and Duration > 300*1000 and Duration <= 1800*1000 ");
            }
            break;
        case 3:
            {
                Conditons += _T(" and Duration > 1800*1000 and Duration <= 3600*1000 ");
            }
            break;
        case 4:
            {
                Conditons += _T(" and Duration > 3600*1000");
            }
            break;
        default:
            break;
        }
    }
    if (MediaSearchOptions::Instance()->_bUseDate)
    {
        S3Time startTime(S3Time::NULLTime), endTime(S3Time::OneDayAfter());
        ATL::COleDateTime t1, t2;
        COleDateTime TmpTimeStart =  MediaSearchOptions::Instance()->_timeStart;
        COleDateTime TmpTimeEnd =  MediaSearchOptions::Instance()->_timeEnd;

        t1.SetDateTime(TmpTimeStart.GetYear(),TmpTimeStart.GetMonth(), TmpTimeStart.GetDay(),0, 0,0);
        t2.SetDateTime(TmpTimeEnd.GetYear(), TmpTimeEnd.GetMonth(), TmpTimeEnd.GetDay(), 23,59,59);

        startTime = S3Time::CreateTime(t1);
        endTime = S3Time::CreateTime(t2);

        std::wstring tmpStart = startTime.ToString();
        std::wstring tmpEnd = endTime.ToString();
        CString tmp1(tmpStart.c_str());
        CString tmp2(tmpEnd.c_str());

        Conditons += _T(" and UploadTime between s3datetime('") + tmp1 + _T("') and s3datetime('") + tmp2 + _T("')");
    }

    if (CategoryID >= 0)
    {
        _itot_s(CategoryID, buf, 10);
        strID = buf;
        Conditons += _T(" and (MediaFilterId = ") + strID;
    }


    // do search now
    //  Condition cond(Conditons.GetString());

    //How to new a buf and post it to work thread
    UINT nBufID;
    std::map<int, CString>* pInfo = WorkThreadDataManager< std::map<int, CString> >::NewDataBuf(nBufID); //create dynamic ms
    if (pInfo == NULL)
    {
        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)Translate(_T("Failed to search!")).GetString());

        return -1;
    }

    pInfo[0].insert(std::map<int, CString>::value_type(CategoryID, Conditons));

    DWORD MessageID = USER_MSG_SEARCH_MEDIA_LIST;

    if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,
        (WPARAM)this->GetSafeHwnd(), (LPARAM)nBufID))//post thread msg
    {
        TRACE1("post message failed,error:%d\n",::GetLastError());

        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)Translate(_T("Failed to search!")).GetString());

        return -1;
    }

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("Searching......")).GetString());

    return 0;
}

/*
LRESULT CMediaLibraryPage::UpdateMediaCategoryList(WPARAM wp, LPARAM lp)
{
    INT categoryID = wp;

    if (!m_wndMediaLibraryList.m_wndStateTab.SetActiveTab(CATEGORY_LIST_TAB4))
    {
        TRACE0("Failed to set CATEGORY_LIST_TAB4 as active tab");
    }

    DWORD MessageID = USER_MSG_MEDIA_CATEGORY_LIST;

    if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,
        (WPARAM)this->GetSafeHwnd(), (LPARAM)categoryID))//post thread msg
    {
        TRACE1("post message failed,errno:%d\n",::GetLastError());
    }

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("Refreshing category media list......")).GetString());

    return 0;
}
}*/
/*
LRESULT CMediaLibraryPage::OnMediaLibraryCategoryRefresh(WPARAM wp, LPARAM lp)
{
   CMediaLibraryCategoryCtrl::ITEM_TYPE ItemType;
   INT categoryID = m_wndMediaLibraryCategory.m_MediaLibraryCategory.GetSelectedCategory(ItemType);
   SendMessage(MEDIA_LIST_CATEGORY, (WPARAM)categoryID, 0L);  
   return 0;
}
*/

LRESULT CMediaLibraryPage::OnGetMediaLibraryMediaList(WPARAM wp, LPARAM lp)
{
    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf< pair<CString, MEDIA_BASIC_INFO> > MessageBuf;
    if (!WorkThreadDataManager< pair<CString, MEDIA_BASIC_INFO> >
        ::GetDataBuf(nMessageBufID, MessageBuf))
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////

    const MEDIA_BASIC_INFO& mediaBaseInfo = MessageBuf.Get()->second;

    const CString& MessageType = MessageBuf.Get()->first;

    CListCtrlEx* pMediaList = NULL;
#ifndef PROFESSIONAL_EDITION
    if (MessageType == _T("DOWNLOAD_MEDIA_LIST") && m_wndMediaLibraryList.m_wndStateTab.GetActiveTab() == DOWNLOAD_LIST_TAB3)
    {
        pMediaList = m_wndMediaLibraryList.m_wndStateTab.m_MainList[DOWNLOAD_LIST_TAB3];
    }
    else
#endif
    if (MessageType == _T("UPLOAD_MEDIA_LIST") && m_wndMediaLibraryList.m_wndStateTab.GetActiveTab() == UPLOAD_LIST_TAB2)
    {
        pMediaList = m_wndMediaLibraryList.m_wndStateTab.m_MainList[UPLOAD_LIST_TAB2];
    }
    else
    {
        return 1;
    }

    if (pMediaList)
    {
        //insert item for download list

        std::map<INT, INT> SelectedMediaIDMap;
        POSITION pos = pMediaList->GetFirstSelectedItemPosition();
        while(pos)
        {
            int nItem = pMediaList->GetNextSelectedItem(pos);
            int MediaID = pMediaList->GetItemData(nItem);

            SelectedMediaIDMap[MediaID] = MediaID;
        }

        int visindex = pMediaList->GetTopIndex();
        int posx  = pMediaList->GetScrollPos(SB_HORZ);
        pMediaList->SetRedraw(FALSE);
        pMediaList->DeleteAllItems();

        auto it = mediaBaseInfo.m_Medias.begin(), ite = mediaBaseInfo.m_Medias.end();
        int i = 0;

		CMagicSortListCtrl *pSortList = CMagicSortListCtrl::ConverPtr(pMediaList);
        for (; it != ite; it++,++i)
        {
            TCHAR szMediaID[20];
            TCHAR sPercent[10];
            CString szPercent, szPercentGenerateInfo, szPercentVerify;
            _stprintf(szMediaID, _T("%d"), it->first);

            if (it->second.MediaInfoGeneratePercent < 100.0)
            {
                _itot_s(it->second.MediaInfoGeneratePercent, sPercent, 10);
                szPercentGenerateInfo = sPercent;
                szPercentGenerateInfo  += _T("%");
            }
            else
            {
                 szPercentGenerateInfo = _T("100%");
            }

            if (it->second.Percent < 100.0)
            {
                _itot_s(it->second.Percent, sPercent, 10);
                szPercent = sPercent;
                szPercent  += _T("%");
            }
            else
            {
                szPercent = _T("100%");
            }

            if (it->second.SecurityVerifyPercent < 100.0)
            {
                _itot_s(it->second.SecurityVerifyPercent, sPercent, 10);
                szPercentVerify = sPercent;
                szPercentVerify  += _T("%");
            }
            else
            {
                szPercentVerify = _T("100%");
            }

            CString timeDur;
            CString timeApprove;
            CString status;

            CString csDimension;
            if (it->second.Width || it->second.Height)
            {
                csDimension.Format(_T("%d * %d"), it->second.Width, it->second.Height);
            }

            if (it->second.Duration.GetTotalSeconds() > 0)
            {
                timeDur = it->second.Duration.Format(_T("%H:%M:%S"));
                _tprintf_s(_T("%s\n"), (LPCTSTR)timeDur);
            }

			if(pSortList)
            {
                if (it->first < 0)
                {
                    pSortList->InsertItem(i, _T(""));
                }
                else
                {
                    pSortList->InsertItem(i, (LPCTSTR)szMediaID);
                }
            }
			else
            {
                if (it->first < 0)
                {
                    pMediaList->InsertItem(i, _T(""));
                }
                else
                {
                    pMediaList->InsertItem(i, (LPCTSTR)szMediaID);
                }
            }

            pMediaList->SetItemData(i, it->first);

			CString strStat = ConvertStats2String(it->second.TaskStatus).c_str();
#if defined(PROFESSIONAL_EDITION) || defined(STANDALONE_EDITION)
			if(strStat == _T("Server insufficient space"))
			{
				strStat =  _T("Low Disk Space");
			}
#endif

            pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_TASK_STATS,Translate(strStat));

            pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_TASK_NAME, it->second.Name.c_str());
            pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_TASK_TYPE, it->second.Type.c_str());
            //  pMediaList->SetItemText(i, CMediaLibraryLissetTab::MEDIA_TASK_CATEGORY, it->second.Category.c_str());
           // CString fullPath("");
            //GetCategoryFullPath(it->second.CategoryID, fullPath);
            //pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_TASK_CATEGORY, fullPath);
            pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_TASK_PERCENT, (LPCTSTR)szPercent+szPercentGenerateInfo+szPercentVerify);
            CString szFinishedSize;
            szFinishedSize.Format(_T("%s/%s"), FormatSizeString(it->second.Size * it->second.Percent / 100.0),
                FormatSizeString(it->second.Size));
            pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_TASK_SIZE, szFinishedSize);
            pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_TASK_DIMENSION, (LPCTSTR)csDimension);
            pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_TASK_DURATION, (LPCTSTR)timeDur);
            pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_TASK_AVALIABLE, it->second.bAvailable ? _T("True") : _T("False"));

            auto  itfind = SelectedMediaIDMap.find(it->first);
            if (itfind != SelectedMediaIDMap.end())
            {
                pMediaList->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
            }
        }
		if(pSortList)
			pSortList->Resort();
        pMediaList->SetRedraw(TRUE);

        RECT ys;
        if(pMediaList->GetItemRect(visindex,&ys,LVIR_BOUNDS) != 0)
        {
            CSize tmpsize;
            tmpsize.cx = posx;
            tmpsize.cy = (ys.bottom-ys.top)*(visindex);
            pMediaList->Scroll(tmpsize);
        }

        if (MessageType == _T("DOWNLOAD_MEDIA_LIST"))
        {
            GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
                (LPARAM)Translate(_T("Download media list refreshed!")).GetString());
        }
        else if (MessageType == _T("UPLOAD_MEDIA_LIST"))
        {
            GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
                (LPARAM)Translate(_T("Upload category media list refreshed!")).GetString());
        }
    }

    return 0;
}

int CMediaLibraryPage::IsCategoryCanBeDel(int categoryID)
{
	CListCtrlEx* pMediaList = NULL;
	pMediaList = m_wndMediaLibraryList.m_wndStateTab.m_MainList[UPLOAD_LIST_TAB2];

	if (!pMediaList)
	{
		return 0x01;
	}

	CString categoryPath;
	GetCategoryFullPath(categoryID, categoryPath);

	int num = pMediaList->GetItemCount();

	for(int ii = 0; ii < num ; ii++)
	{

		{
#if 0
			CString path = pMediaList->GetItemText(ii, CMediaLibraryListTab::MEDIA_TASK_INDEX);
			int MediaID = 0;
			for(int ii = 0 ; ii < path.GetLength(); ii++)
			{
				if(path[ii]>= 0x30 && path[ii]<= 0x39)
				{
					MediaID *= 10;
					MediaID += (path[ii] - 0x30);
				}
			}
			TASK_BASIC_INFO BasicInfo;
			HRESULT hr = GetMediaLibraryInterface->CheckTaskStatus(MediaID, BasicInfo);
			if (SUCCEEDED(hr))
			{
				if (BasicInfo.MediaInfoGeneratePercent < 100.0 || BasicInfo.Percent < 100.0 || BasicInfo.SecurityVerifyPercent < 100.0)
				{
					return -1;
				}
			}
#else
			CString szTaskStatus = pMediaList->GetItemText(ii, CMediaLibraryListTab::MEDIA_TASK_STATS);
			CString szTaskFinished = Translate(ConvertStats2String(ML_FINISHED).c_str());
			if(szTaskStatus != szTaskFinished)
			{
				CString path = pMediaList->GetItemText(ii, CMediaLibraryListTab::MEDIA_TASK_INDEX);
				int MediaID = 0;
				for(int ii = 0 ; ii < path.GetLength(); ii++)
				{
					if(path[ii]>= 0x30 && path[ii]<= 0x39)
					{
						MediaID *= 10;
						MediaID += (path[ii] - 0x30);
					}
				}
				TASK_BASIC_INFO BasicInfo;
				HRESULT hr = GetMediaLibraryInterface->CheckTaskStatus(MediaID, BasicInfo);
				if (SUCCEEDED(hr))
				{
					if (BasicInfo.CategoryID == categoryID)
					{
						return -1;
					}
				}
			}
#endif
		}
	}
	return 0x01;
}

BOOL CMediaLibraryPage::GetIsUrl()
{
    return m_bIsUrl;
}

LRESULT CMediaLibraryPage::OnGetServerMediaList(WPARAM wp,LPARAM lp)
{
    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;
    DWORD MessageType  = (DWORD)lp;

    DataBuf< pair< vector<Media>, MEDIA_DETAIL_INFO>  > MessageBuf;
    if (!WorkThreadDataManager< pair< vector<Media>, MEDIA_DETAIL_INFO > >
        ::GetDataBuf(nMessageBufID, MessageBuf))
    {
        return 1;

    }
    //////////////////////////////////////////////////////////////////////////

    CListCtrlEx* pMediaList = NULL;
    std::map<INT, Media>* pMediaMap = NULL;
    if (MessageType == MEDIA_SEARCH_LIST && m_wndMediaLibraryList.m_wndStateTab.GetActiveTab() == MEDIA_LIST_TAB1)
    {
        pMediaList = m_wndMediaLibraryList.m_wndStateTab.m_MainList[MEDIA_LIST_TAB1];
        pMediaMap = &m_wndMediaLibraryList.m_wndStateTab.m_SearchMediaMap;
    }
    else if (MessageType == MEDIA_SEARCH_LIST && m_wndMediaLibraryList.m_wndStateTab.GetActiveTab() != MEDIA_LIST_TAB1)
    {
        m_wndMediaLibraryList.m_wndStateTab.SetActiveTab(MEDIA_LIST_TAB1);
        pMediaList = m_wndMediaLibraryList.m_wndStateTab.m_MainList[MEDIA_LIST_TAB1];
        pMediaMap = &m_wndMediaLibraryList.m_wndStateTab.m_SearchMediaMap;
    }
    else if (MessageType == MEDIA_CATEGORY_LIST && m_wndMediaLibraryList.m_wndStateTab.GetActiveTab() == CATEGORY_LIST_TAB4)
    {
     //   pMediaList = m_wndMediaLibraryList.m_wndStateTab.m_MainList[CATEGORY_LIST_TAB4];
    //    pMediaMap = m_wndMediaLibraryList.m_wndStateTab.GetMediaMap();
		return 1;
    }
    else
    {
        return 1;
    }

    if (pMediaList)
    {
        std::map<INT, INT> SelectedMediaIDMap;
        POSITION pos = pMediaList->GetFirstSelectedItemPosition();
        while(pos)
        {
            int nItem = pMediaList->GetNextSelectedItem(pos);
            int MediaID = pMediaList->GetItemData(nItem);

            SelectedMediaIDMap[MediaID] = MediaID;
        }

        int visindex = pMediaList->GetTopIndex();
        int posx  = pMediaList->GetScrollPos(SB_HORZ);
        pMediaList->SetRedraw(FALSE);
        pMediaList->DeleteAllItems();

        ASSERT(pMediaList->GetItemCount() == 0);

        const vector<Media>& Medias = MessageBuf.Get()->first;
        const MEDIA_DETAIL_INFO& MediaSyncInfo = MessageBuf.Get()->second;

        auto it = Medias.begin(), ite = Medias.end();
        int i = 0;
        //m_wndMediaLibraryList.m_wndStateTab.m_MediaMap.clear();
        pMediaMap->clear();

		CMagicSortListCtrl *pSortList = CMagicSortListCtrl::ConverPtr(pMediaList);

        for (; it != ite; ++it, ++i)
        {
            //m_wndMediaLibraryList.m_wndStateTab.m_MediaMap[it->GetMeidaID()] = *it;
            (*pMediaMap)[it->GetMeidaID()] = *it;

            TCHAR szMediaID[20];
            _stprintf(szMediaID, _T("%d"), it->GetMeidaID());

            CString dur;

            if (it->GetDuration() > 0)
            {
                COleDateTimeSpan tmpDur;
                tmpDur.SetDateTimeSpan(0,0,0,it->GetDuration() / 1000);
                dur = tmpDur.Format(_T("%H:%M:%S"));
            }

            CString szSize = FormatSizeString(it->GetFileSize());

			if(pSortList)
				pSortList->InsertItem(i, (LPCTSTR)szMediaID);
			else
				pMediaList->InsertItem(i, (LPCTSTR)szMediaID);


            pMediaList->SetItemData(i, it->GetMeidaID());

            pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_NAME, it->GetName().c_str());
            pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_TYPE, it->GetMediaType().c_str());
            pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_SIZE, szSize);
        //  pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_CATEGORY, it->GetMediaFilter().GetMediaFilterName().c_str());
          //  CString fullPath("");
          //  GetCategoryFullPath(it->GetMediaFilter().GetMediaFilterID(), fullPath);
          //  pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_CATEGORY, fullPath);

            //5 is dimension
            CString csDimension;
            if (it->GetWidth() || it->GetHeight())
            {
                csDimension.Format(_T("%d * %d"), it->GetWidth(), it->GetHeight());
            }

            pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_DIMENSION, csDimension);

            pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_DURATION, (LPCTSTR)dur);

            CString syncStatus =_T("");
            CString szRefCnt = _T("");
            auto itFindSync = MediaSyncInfo.m_Medias.begin();
            for (;itFindSync != MediaSyncInfo.m_Medias.end(); itFindSync++)
            {
                if (itFindSync->first == it->GetMeidaID())
                {
                    break;
                }
            }
            if (itFindSync == MediaSyncInfo.m_Medias.end())
            {
                syncStatus = Translate(_T("NOT_AVAILABLE_IN_LOCAL"));
                szRefCnt = Translate(_T("UNKNOWN"));
            }
            else
            {
                const TASK_DETAIL_INFO& TaskInfo = itFindSync->second;

                if (TaskInfo.SyncStatus == SYNCING)
                {
                    syncStatus =  Translate(_T("SYNCING"));
                }
                else if (TaskInfo.SyncStatus == SYNC_WITH_SERVER)
                {
                    syncStatus = Translate(_T("SYNC_WITH_SERVER"));
                }
                else if (TaskInfo.SyncStatus == OUT_OF_DATE)
                {
                    syncStatus = Translate(_T("OUT_OF_DATE"));
                }
                else
                {
                    syncStatus = Translate(_T("NOT_AVAILABLE_IN_LOCAL"));
                }

                INT RefCnt = itFindSync->second.RefCntInLayouts;

                TCHAR refers[20];
                _stprintf(refers, _T("%d"), RefCnt);

                szRefCnt = refers;
            }
#ifdef SERVER_EDITION
            pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_SYNCSTATUS, syncStatus);
            pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_AUDIT, Translate(it->GetAudit().GetStatusStr().c_str()));
#endif
            pMediaList->SetItemText(i, CMediaLibraryListTab::MEDIA_REFERS, szRefCnt);


            auto  itfind = SelectedMediaIDMap.find(it->GetMeidaID());
            if (itfind != SelectedMediaIDMap.end())
            {
                pMediaList->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
            }
        }

		if(pSortList)
			pSortList->Resort();
        pMediaList->SetRedraw(TRUE);

        RECT ys;
        if(pMediaList->GetItemRect(visindex,&ys,LVIR_BOUNDS) != 0)
        {
            CSize tmpsize;
            tmpsize.cx = posx;
            tmpsize.cy = (ys.bottom-ys.top)*(visindex);
            pMediaList->Scroll(tmpsize);
        }

        if (MessageType == MEDIA_SEARCH_LIST)
        {
            CString InfoStatus("");
            InfoStatus.Format(Translate(_T("Search completed, the result records: %d")), i);
            GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
                (LPARAM)(InfoStatus.GetBuffer()));
        }
        else if (MessageType == MEDIA_CATEGORY_LIST)
        {
            GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
                (LPARAM)Translate(_T("Category media list refreshed!")).GetString());
        } 

		if(MessageType == MEDIA_CATEGORY_LIST )
		 m_wndMediaLibraryList.m_wndStateTab.ReleaseMediaMap(NULL);
    }

    return 0;
}


LRESULT CMediaLibraryPage::OnMediaLibraryUploadList(WPARAM wp, LPARAM lp)
{

    DWORD mediaType = 0;

    if (m_bUploadingList)
    {
        mediaType = MediaLibrary::MT_UPLOADING;
    }
    if (m_bUploadedList)
    {
        mediaType = mediaType | MediaLibrary::MT_UPLOADED;
    }
    if (m_bUploadFailList)
    {
        mediaType = mediaType | MediaLibrary::MT_UPLOADFAILED;
    }

    // async mode to get username of approveuserid and rejectuserid: mediaInfo
    DWORD MessageID = USER_MSG_MEDIALIBRARY_GET_MEDIALIST;

    //How to new a buf and post it to work thread
    UINT nBufID;
    pair<DWORD, CString>* pInfo = WorkThreadDataManager< pair<DWORD, CString> >::NewDataBuf(nBufID); //create dynamic msg
    if (pInfo == NULL)
    {
        return -1;
    }

    pInfo->first = mediaType;
    pInfo->second = _T("UPLOAD_MEDIA_LIST");

    if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,
        (WPARAM)this->GetSafeHwnd(), (LPARAM)nBufID))//post thread msg
    {
        TRACE1("post message failed,error:%d\n",::GetLastError());
    }

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("Refreshing upload media list......")).GetString());

     return 0;
}

LRESULT CMediaLibraryPage::OnMediaLibraryDownloadList(WPARAM wp, LPARAM lp)
{
    DWORD mediaType;

    if (m_bDownloadingList)
    {
        mediaType = MediaLibrary::MT_DOWNLOADING;
    }
    if (m_bDownloadedList)
    {
        mediaType = mediaType | MediaLibrary::MT_DOWNLOADED;
    }
    if (m_bDownloadFailList)
    {
        mediaType = mediaType | MediaLibrary::MT_DOWNLOADFAILED;
    }

    // async mode to get username of approveuserid and rejectuserid: mediaInfo
    DWORD MessageID = USER_MSG_MEDIALIBRARY_GET_MEDIALIST;

    //How to new a buf and post it to work thread
    UINT nBufID;
    pair<DWORD, CString>* pInfo = WorkThreadDataManager< pair<DWORD, CString> >::NewDataBuf(nBufID); //create dynamic msg
    if (pInfo == NULL)
    {
        return -1;
    }

    pInfo->first = mediaType;
    pInfo->second = _T("DOWNLOAD_MEDIA_LIST");

    if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,
        (WPARAM)this->GetSafeHwnd(), (LPARAM)nBufID))//post thread msg
    {
        TRACE1("post message failed,error:%d\n",::GetLastError());
    }

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("Refreshing download media list......")).GetString());

     return 0;
}


LRESULT CMediaLibraryPage::OnMediaInfoShow(WPARAM wp, LPARAM lp)
{
    int mediaID = (int)lp;
    std::vector<int> mediaSingleList;
    std::vector<Media> mediaSingleInfo;
    mediaSingleList.push_back(mediaID);
     //get media thumbnail
    MediaThumbnail thumbnail;
    HRESULT hr = GetMediaLibraryInterface->GetMediaThumbnail(mediaID,thumbnail);
    if(!SUCCEEDED(hr))
    {
        hr = GetControllerInterface->GetMediaThumbnail(mediaID, thumbnail);
        if (!SUCCEEDED(hr))//if failure here,to be continue,will product an "Empty" thumbnail.
        {
            TRACE0("Failed to Get Media Thumbnail!\n");
        }
    }
     //show media thumbnail in picture control

    vector<BYTE> &data = thumbnail.GetThumbnailDataRef();
    //thumbnail.GetThumbnail(data);
    MLThumbnail thumbnailImage;
    thumbnailImage.InitFromData(data);

    m_wndMediaInfo.m_MediaLibraryInfoCtrl.SetMediaThumbnail(thumbnailImage.GetImage());

    //get media detail info
    hr = GetControllerInterface->GetMedias(mediaSingleList, mediaSingleInfo);
    if (!SUCCEEDED(hr) || mediaSingleInfo.size() < 1)
    {
        TRACE0("Get media detail info fail!\n");
        return E_FAIL;
    }

    m_wndMediaInfo.m_MediaLibraryInfoCtrl.SetMediaInfo(*mediaSingleInfo.begin());

    return 0;
}

LRESULT CMediaLibraryPage::OnMediaInfoClear(WPARAM wp, LPARAM lp)
{
    m_wndMediaInfo.m_MediaLibraryInfoCtrl.ClearInfo();

    UpdateData(FALSE);
    return 0;
}

LRESULT CMediaLibraryPage::OnMediaSetActiveTab(WPARAM wp, LPARAM lp)
{
    int indexTab = (int)wp;
    if(m_wndMediaLibraryList.m_wndStateTab.GetActiveTab() != indexTab)
        m_wndMediaLibraryList.m_wndStateTab.SetActiveTab(indexTab);
    return 0;
}

BOOL CMediaLibraryPage::UpdateOptions()
{
    return m_wndMediaLibraryList.UpdateOptions();
}

void CMediaLibraryPage::OnUpdateRibbonBtn(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    static std::map<int, std::pair<std::function<void (CCmdUI*)>, std::function<bool ()> > > dispatchMap;
    // check privilege
    if (dispatchMap.empty())
    {

        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_MEDIA_PREVIEW, &CMediaLibraryPage::OnUpdateRibbonBtnPreview, RET_TRUE());
#ifdef SERVER_EDITION
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_MEDIA_APPROVE, &CMediaLibraryPage::OnUpdateRibbonBtnApprove, LAMBDA(SC(ApproveMedia)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_MEDIA_REJECT, &CMediaLibraryPage::OnUpdateRibbonBtnReject, LAMBDA(SC(ApproveMedia)));
		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_MEDIA_RETIREFILE, &CMediaLibraryPage::OnUpdateRibbonBtnRetire, LAMBDA(SC(RetireMedia)));
#endif
   //     UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_MEDIA_REQUEST, &CMediaLibraryPage::OnUpdateRibbonBtnNeedSelect, LAMBDA(SC(ApproveMedia)));

        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_MEDIA_UPLOADFILE, &CMediaLibraryPage::OnUpdateRibbonBtnNeedSelectCategory, LAMBDA(SC(CreateMedia)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_MEDIA_UPLOADURL, &CMediaLibraryPage::OnUpdateRibbonBtnNeedSelectCategory, LAMBDA(SC(CreateMedia)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_MEDIA_SYNCFILE, &CMediaLibraryPage::OnUpdateRibbonBtnNeedSelectServerFile, LAMBDA(SC(GetMedia)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_MEDIA_FORCESYNCFILE, &CMediaLibraryPage::OnUpdateRibbonBtnNeedSelectServerFile, LAMBDA(SC(GetMedia)));
        
        
		UPDATE_COMMAND_ENTRY(ID_BTN_MEDIA_STARTUPLOAD, &CMediaLibraryPage::OnUpdateRibbonBtnUpload, LAMBDA(SC(CreateMedia)));
        UPDATE_COMMAND_ENTRY(ID_BTN_MEDIA_PAUSEUPLOAD, &CMediaLibraryPage::OnUpdateRibbonBtnUpload, LAMBDA(SC(CreateMedia)));
        UPDATE_COMMAND_ENTRY(ID_BTN_MEDIA_CANCELUPLOAD, &CMediaLibraryPage::OnUpdateRibbonBtnUpload, LAMBDA(SC(CreateMedia)));
        UPDATE_COMMAND_ENTRY(ID_BTN_MEDIA_UPLOADING, &CMediaLibraryPage::OnUpdateRibbonCheckBtnUploading, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_BTN_MEDIA_UPLOADED, &CMediaLibraryPage::OnUpdateRibbonCheckBtnUploaded, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_BTN_MEDIA_UPLOADFAILED, &CMediaLibraryPage::OnUpdateRibbonCheckBtnUploadFail, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_BTN_MEDIA_STARTDOWNLOAD, &CMediaLibraryPage::OnUpdateRibbonBtnDownload, LAMBDA(SC(GetMedia)));
        UPDATE_COMMAND_ENTRY(ID_BTN_MEDIA_PAUSEDOWNLOAD, &CMediaLibraryPage::OnUpdateRibbonBtnDownload, LAMBDA(SC(GetMedia)));
        UPDATE_COMMAND_ENTRY(ID_BTN_MEDIA_CANCELDOWNLOAD, &CMediaLibraryPage::OnUpdateRibbonBtnDownload, LAMBDA(SC(GetMedia)));
        UPDATE_COMMAND_ENTRY(ID_BTN_MEDIA_DOWNLOADING, &CMediaLibraryPage::OnUpdateRibbonCheckBtnDownloading, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_BTN_MEDIA_DOWNLOADED, &CMediaLibraryPage::OnUpdateRibbonCheckBtnDownloaded, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_BTN_MEDIA_DOWNLOADFAILED, &CMediaLibraryPage::OnUpdateRibbonCheckBtnDownloadFail, RET_TRUE());

		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_MEDIA_DELFILE, &CMediaLibraryPage::OnUpdateRibbonBtnDelSelectServerFile, LAMBDA(SC(DeleteMedia)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_MEDIA_RENAME_CATE, &CMediaLibraryPage::OnUpdateRibbonBtnNeedSelSigCate, LAMBDA(SC(EditMedia)));
    }

    auto it = dispatchMap.find(pCmdUI->m_nID), ite = dispatchMap.end();
     // id exist and have privilege
    if (it != ite && it->second.second())
    {
        it->second.first(pCmdUI);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CMediaLibraryPage::OnUpdateRibbonBtnPreview(CCmdUI* pCmdUI)
{

	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	vector<INT> MediaIDs;
	m_wndMediaLibraryList.m_wndStateTab.GetSelectedMediaIDList(MediaIDs);

	if (0x01 == MediaIDs.size())
	{
		int indexList = m_wndMediaLibraryList.m_wndStateTab.GetActiveTab();
		if (indexList == CATEGORY_LIST_TAB4 || indexList == MEDIA_LIST_TAB1)
		{		
			pCmdUI->Enable(TRUE);
			return;
		}

#ifndef PROFESSIONAL_EDITION
		if (UPLOAD_LIST_TAB2 == indexList || DOWNLOAD_LIST_TAB3 == indexList)
#else
		if (UPLOAD_LIST_TAB2 == indexList)
#endif
		{
			POSITION pos = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetFirstSelectedItemPosition();
			INT index = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetNextSelectedItem(pos);
			if (index >= 0)
			{
				CString szText = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetItemText(index, 1);
				if (szText.GetLength() != 0 && szText.CompareNoCase(Translate(_T("Finished"))) != 0)
				{
					pCmdUI->Enable(FALSE);
				}
				else
				{
					pCmdUI->Enable(TRUE);
				}
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMediaLibraryPage::OnUpdateRibbonBtnNeedSelectCategory(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }
    HTREEITEM hItem = m_wndMediaLibraryCategory.m_MediaLibraryCategory.m_ctrlCategoryTree.GetSelectedItem();
    if (hItem != NULL)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }

}

void CMediaLibraryPage::OnUpdateRibbonBtnNeedSelSigCate(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }
    int indexList = m_wndMediaLibraryList.m_wndStateTab.GetActiveTab();
	vector<int> mediaIDList;
	if (indexList == 3)
	{
		m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.GetSelectedMultiID(mediaIDList);
		if (mediaIDList.size() == 0x01)
		{
			//judge reference count
			if (!m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.JudgeReferCount())
			{
				pCmdUI->Enable(TRUE);
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}		
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else if (indexList == MEDIA_LIST_TAB1)
	{
		INT uSelectCount = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetSelectedCount();
		if (uSelectCount == 1)
		{
			POSITION pos = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetFirstSelectedItemPosition();
			INT sel = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetNextSelectedItem(pos);

			CString strRefCnt = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetItemText(sel, CMediaLibraryListTab::MEDIA_REFERS);
			int irefCnt = _ttoi(strRefCnt);
			if(0x00 == irefCnt)
			{
				pCmdUI->Enable(TRUE);
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

#ifdef SERVER_EDITION
void CMediaLibraryPage::OnUpdateRibbonBtnReject(CCmdUI* pCmdUI)
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if (m_wndMediaLibraryList.m_wndStateTab.m_MainList.size() == 0)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	int indexList = m_wndMediaLibraryList.m_wndStateTab.GetActiveTab();
	if (indexList == CATEGORY_LIST_TAB4)
	{
		vector<int> mediaIDList;
		m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.GetSelectedMultiID(mediaIDList);
		if (mediaIDList.size() > 0)
		{
			if (m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.JudgeFirstSelAudit(Translate(_T("REJECTED")), CMediaLibraryListTab::MEDIA_AUDIT))
			{
				// need add part of judge exist records audit status is not rejected
				pCmdUI->Enable(FALSE);
				return;
			}
			else
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
		else
			pCmdUI->Enable(FALSE);
	}
	else if (indexList == MEDIA_LIST_TAB1)
	{
		POSITION pos = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetFirstSelectedItemPosition();
		INT index = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetNextSelectedItem(pos);
		if(index >= 0)
		{	
			CString szText = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetItemText(index, CMediaLibraryListTab::MEDIA_AUDIT);

			if(szText == Translate(_T("REJECTED")) )
			{
				pCmdUI->Enable(FALSE);
			}
			else
			{
				pCmdUI->Enable(TRUE);
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}	
}

void CMediaLibraryPage::OnUpdateRibbonBtnApprove(CCmdUI* pCmdUI)
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if (m_wndMediaLibraryList.m_wndStateTab.m_MainList.size() == 0)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	int indexList = m_wndMediaLibraryList.m_wndStateTab.GetActiveTab();
	if (indexList == CATEGORY_LIST_TAB4)
	{
		vector<int> mediaIDList;
		m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.GetSelectedMultiID(mediaIDList);
		if (mediaIDList.size() > 0)
		{
			if (m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.JudgeFirstSelAudit(Translate(_T("APPROVED")), CMediaLibraryListTab::MEDIA_AUDIT))
			{
				pCmdUI->Enable(FALSE);
				return;
			}
			else
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}	
	}
	else if (MEDIA_LIST_TAB1 == indexList)
	{
		POSITION pos = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetFirstSelectedItemPosition();
		INT index = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetNextSelectedItem(pos);
		if(index >= 0)
		{
			CString szText = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetItemText(index, CMediaLibraryListTab::MEDIA_AUDIT);

			if(szText == Translate(_T("APPROVED")) )
			{
				pCmdUI->Enable(FALSE);
			}
			else
			{
				pCmdUI->Enable(TRUE);
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMediaLibraryPage::OnUpdateRibbonBtnRetire(CCmdUI* pCmdUI)
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if (m_wndMediaLibraryList.m_wndStateTab.m_MainList.size() == 0)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	int indexList = m_wndMediaLibraryList.m_wndStateTab.GetActiveTab();
	if (indexList == CATEGORY_LIST_TAB4)
	{
		vector<int> mediaIDList;
		m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.GetSelectedMultiID(mediaIDList);
		if (mediaIDList.size() > 0)
		{
			if (m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.JudgeFirstSelAudit(Translate(_T("RETIRED")), CMediaLibraryListTab::MEDIA_AUDIT))
			{
				pCmdUI->Enable(FALSE);
				return;
			}
			else
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else if(MEDIA_LIST_TAB1 == indexList)
	{
		POSITION pos = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetFirstSelectedItemPosition();
		INT index = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetNextSelectedItem(pos);

		if(index >= 0)
		{
			CString szText = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetItemText(index, CMediaLibraryListTab::MEDIA_AUDIT);

			if(szText == Translate(_T("RETIRED")) )
			{
				pCmdUI->Enable(FALSE);
			}
			else
			{
				pCmdUI->Enable(TRUE);
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
#endif
/*
void CMediaLibraryPage::OnUpdateRibbonBtnNeedSelect(CCmdUI* pCmdUI)
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	if (m_wndMediaLibraryList.m_wndStateTab.m_MainList.size() == 0)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	int indexList = m_wndMediaLibraryList.m_wndStateTab.GetActiveTab();
	if (indexList == CATEGORY_LIST_TAB4)
	{
		vector<int> mediaIDList;
		m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.GetSelectedMultiID(mediaIDList);
		if (mediaIDList.size() > 0)
		{
			// need add part of judge exist records audit status is not rejected
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else if (UPLOAD_LIST_TAB2 == indexList)
	{
		POSITION pos = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetFirstSelectedItemPosition();
		INT index = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetNextSelectedItem(pos);
		if(index >= 0)
		{
			CString szText = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetItemText(index, 1);
			if (szText.GetLength() != 0 && szText.CompareNoCase(Translate(_T("Finished"))) != 0)
			{
				pCmdUI->Enable(FALSE);
			}
			else
			{
				pCmdUI->Enable(TRUE);
			}
		}
		else
		{
			 pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		 pCmdUI->Enable(FALSE);
	}
}
}*/

void CMediaLibraryPage::OnUpdateRibbonBtnDelSelectServerFile(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }
    int indexList = m_wndMediaLibraryList.m_wndStateTab.GetActiveTab();

#ifndef PROFESSIONAL_EDITION
	if (UPLOAD_LIST_TAB2 == indexList || DOWNLOAD_LIST_TAB3 == indexList)
#else
	if (UPLOAD_LIST_TAB2 == indexList)
#endif
    {
        pCmdUI->Enable(FALSE);
        return;
    }
	if (indexList == CATEGORY_LIST_TAB4)
	{
		vector<int> mediaIDList;
		m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.GetSelectedMultiID(mediaIDList);
		if (mediaIDList.size() > 0)
		{
			if (!m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.JudgeReferCount())
			{
				pCmdUI->Enable(TRUE);
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
		
	}
	else
	{
		POSITION pos = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetFirstSelectedItemPosition();
		INT index = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetNextSelectedItem(pos);
		if(index >= 0)
		{
			CString strRefCnt = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetItemText(index, CMediaLibraryListTab::MEDIA_REFERS);
			int irefCnt = _ttoi(strRefCnt);
			if(0x00 == irefCnt)
			{
				pCmdUI->Enable(TRUE);
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
}

void CMediaLibraryPage::OnUpdateRibbonBtnNeedSelectServerFile(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }
    int indexList = m_wndMediaLibraryList.m_wndStateTab.GetActiveTab();

#ifndef PROFESSIONAL_EDITION
	if (UPLOAD_LIST_TAB2 == indexList || DOWNLOAD_LIST_TAB3 == indexList)
#else
	if (UPLOAD_LIST_TAB2 == indexList)
#endif
    {
        pCmdUI->Enable(FALSE);
        return;
    }

	if (indexList == CATEGORY_LIST_TAB4)
	{
		vector<int> mediaIDList;
		m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.GetSelectedMultiID(mediaIDList);
		if (mediaIDList.size() > 0)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		POSITION pos = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetFirstSelectedItemPosition();
		INT index = m_wndMediaLibraryList.m_wndStateTab.m_MainList[indexList]->GetNextSelectedItem(pos);
		if(index >= 0)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
}


void CMediaLibraryPage::OnUpdateRibbonBtnUpload(CCmdUI* pCmdUI)
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
 	if (UPLOAD_LIST_TAB2 == m_wndMediaLibraryList.m_wndStateTab.GetActiveTab())
 	{
        if (m_wndMediaLibraryList.m_wndStateTab.m_MainList[UPLOAD_LIST_TAB2]->GetSelectedCount() > 0)
        {
             pCmdUI->Enable(TRUE);
        }
        else
        {
            pCmdUI->Enable(FALSE);
        }
 	}
	else
	{
		pCmdUI->Enable(FALSE);
	}

}

void CMediaLibraryPage::OnUpdateRibbonCheckBtnUploading(CCmdUI* pCmdUI)
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if (UPLOAD_LIST_TAB2 == m_wndMediaLibraryList.m_wndStateTab.GetActiveTab())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
	pCmdUI->SetCheck(m_bUploadingList);
}

void CMediaLibraryPage::OnUpdateRibbonCheckBtnUploaded(CCmdUI* pCmdUI)
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if (UPLOAD_LIST_TAB2 == m_wndMediaLibraryList.m_wndStateTab.GetActiveTab())
	{

		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
	pCmdUI->SetCheck(m_bUploadedList);
}

void CMediaLibraryPage::OnUpdateRibbonCheckBtnUploadFail(CCmdUI* pCmdUI)
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if (UPLOAD_LIST_TAB2 == m_wndMediaLibraryList.m_wndStateTab.GetActiveTab())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
	pCmdUI->SetCheck(m_bUploadFailList);
}

void CMediaLibraryPage::OnUpdateRibbonBtnDownload(CCmdUI* pCmdUI)
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

#ifndef PROFESSIONAL_EDITION
	if (DOWNLOAD_LIST_TAB3 == m_wndMediaLibraryList.m_wndStateTab.GetActiveTab())
	{
        if (m_wndMediaLibraryList.m_wndStateTab.m_MainList[DOWNLOAD_LIST_TAB3]->GetSelectedCount() > 0)
        {
            pCmdUI->Enable(TRUE);
        }
        else
        {
            pCmdUI->Enable(FALSE);
        }
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
#endif
}

void CMediaLibraryPage::OnUpdateRibbonCheckBtnDownloading(CCmdUI* pCmdUI)
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

#ifndef PROFESSIONAL_EDITION
	if (DOWNLOAD_LIST_TAB3 == m_wndMediaLibraryList.m_wndStateTab.GetActiveTab())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
	pCmdUI->SetCheck(m_bDownloadingList);
#endif
}

void CMediaLibraryPage::OnUpdateRibbonCheckBtnDownloaded(CCmdUI* pCmdUI)
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

#ifndef PROFESSIONAL_EDITION
	if (DOWNLOAD_LIST_TAB3 == m_wndMediaLibraryList.m_wndStateTab.GetActiveTab())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
	pCmdUI->SetCheck(m_bDownloadedList);
#endif
}

void CMediaLibraryPage::OnUpdateRibbonCheckBtnDownloadFail(CCmdUI* pCmdUI)
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

#ifndef PROFESSIONAL_EDITION
	if (DOWNLOAD_LIST_TAB3 == m_wndMediaLibraryList.m_wndStateTab.GetActiveTab())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
	pCmdUI->SetCheck(m_bDownloadFailList);
#endif
}


/*
first fill ,Refresh it.
and then,Get Categrory medias number,and info to reset list item status.
zxy
*/

const pair< vector<Media>, MEDIA_DETAIL_INFO> & GetMediaInfosPair(int nMessageBufID,BOOL &bSucess)
{

	//must new buffer here for thread safely.
	UINT nBufID;
	DataBuf< pair< vector<Media>, MEDIA_DETAIL_INFO>  > *MessageBuf =
                        WorkThreadDataManager<DataBuf< pair< vector<Media>, MEDIA_DETAIL_INFO>  > >
                        ::NewDataBuf(nBufID);// I

	bSucess = TRUE;
    if (!WorkThreadDataManager< pair< vector<Media>, MEDIA_DETAIL_INFO > >
        ::GetDataBuf(nMessageBufID, *MessageBuf))
    {
		bSucess = FALSE;
    }
	 
    return   *MessageBuf->Get();
	 
}

void GetMediaSyncStatus(CStringList &itemList,int nMediaID,const MEDIA_DETAIL_INFO &MediaSyncInfo)
{
		auto itFindSync = MediaSyncInfo.m_Medias.begin();
		for (;itFindSync != MediaSyncInfo.m_Medias.end(); itFindSync++)
		{
			if (itFindSync->first == nMediaID)
				break;
		}
#define Translate(x) x
        if (itFindSync == MediaSyncInfo.m_Medias.end())
        {
            itemList.AddTail( Translate(_T("NOT_AVAILABLE_IN_LOCAL")));
            itemList.AddTail(Translate(_T("UNKNOWN")));
        }
        else
        {
            const TASK_DETAIL_INFO& TaskInfo = itFindSync->second;

            switch(TaskInfo.SyncStatus)
			{
			case SYNCING:
				itemList.AddTail( Translate(_T("SYNCING")));
				break;
			case SYNC_WITH_SERVER:
				itemList.AddTail( Translate(_T("SYNC_WITH_SERVER")));
				break;
			case OUT_OF_DATE:
				itemList.AddTail( Translate(_T("OUT_OF_DATE")));
				break;
			default:
				itemList.AddTail( Translate(_T("NOT_AVAILABLE_IN_LOCAL")));
				break;
			}
#undef Translate
            INT RefCnt = itFindSync->second.RefCntInLayouts;

            CString strRefCnt;
            strRefCnt.Format( _T("%d"), RefCnt);
            itemList.AddTail(strRefCnt);
        }
}

void ParseInfoToList(CStringList & itemList,Media * pInfo,const MEDIA_DETAIL_INFO &MediaSyncInfo)
{
	CString strItem;

	itemList.AddTail(pInfo->GetName().c_str());
	itemList.AddTail(pInfo->GetMediaType().c_str());
	itemList.AddTail(  FormatSizeString(pInfo->GetFileSize()));

	
    //CMediaLibraryPage::GetCategoryFullPath(pInfo->GetMediaFilter().GetMediaFilterID(), strItem);
	//itemList.AddTail(strItem);

	strItem.Format(_T("%d * %d"), pInfo->GetWidth(), pInfo->GetHeight());
	itemList.AddTail(strItem);

	    
	//get duration string 
	strItem.Empty();
    if (pInfo->GetDuration() > 0)
    {
        COleDateTimeSpan tmpDur;
        tmpDur.SetDateTimeSpan(0,0,0,pInfo->GetDuration() / 1000);
        strItem = tmpDur.Format(_T("%H:%M:%S"));
    }
	itemList.AddTail(strItem);

	CStringList tpList;
	GetMediaSyncStatus(tpList,pInfo->GetMeidaID(),MediaSyncInfo);
#ifdef SERVER_EDITION
     itemList.AddTail(Translate(tpList.GetHead()));
     itemList.AddTail(Translate(pInfo->GetAudit().GetStatusStr().c_str()));
#endif
     itemList.AddTail(Translate(tpList.GetTail()));

	
}


void UpdateToMediaList(CListCtrlEx *pList,Media info,const MEDIA_DETAIL_INFO& MediaSyncInfo,BOOL bAdd = 0)
{
	CStringList itemList;

	int nCurID = info.GetMeidaID();
	CString strMediaID;
	strMediaID.Format(_T("%d"),nCurID);
	if(bAdd)
	{
		 
		int nTail = pList->GetItemCount();
		nTail = pList->InsertItem(nTail, (LPCTSTR)strMediaID); 
		pList->SetItemData(nTail,info.GetMeidaID());

		ParseInfoToList(itemList,&info,MediaSyncInfo);
		POSITION pos = itemList.GetHeadPosition();
		int i = 0;
		while(pos)
		{
			i++;
			pList->SetItemText(nTail,i,itemList.GetAt(pos));
			itemList.GetNext(pos);

		}

		return;
	}

	//update item text
	int n = pList->GetTopIndex();
	int nLast = n + pList->GetCountPerPage();
	
	int nStartMediaID = StrToInt(pList->GetItemText(n,0));
	int nEndMediaID = StrToInt(pList->GetItemText(nLast,0));

	if( ((nCurID > nStartMediaID) && (nCurID < nEndMediaID))
		||((nCurID< nStartMediaID) && (nCurID > nEndMediaID)))
		return;
	 
	ParseInfoToList(itemList,&info,MediaSyncInfo);
	for (; n < nLast; n++) //just compare to visible items .
	{
	    CString strItem = pList->GetItemText(n,0);

		if(strItem == strMediaID)
		{
			POSITION pos = itemList.GetHeadPosition();
			int i = 0;
			while(pos)
			{
				i++;
				pList->SetItemText(n,i,itemList.GetAt(pos));
				itemList.GetNext(pos);
			}
			break;
		}

	}
}

void CMediaLibraryPage::DoMessage()
{
	MSG msg;
	while( PeekMessage( &msg, this->m_hWnd, WM_TIMER, WM_TIMER,PM_REMOVE) );

	//
	//these code occured an error :Message recursive occured!
	//
	/*  BOOL  bret = PeekMessage( &msg, this->m_hWnd, 0, 0xffffff, PM_REMOVE);

	if(bret != -1)
	{
	if(msg.message !=WM_TIMER && msg.message != USER_MSG_UPDATE_MEDIA_CATEGORY_LIST
	&& msg.message != USER_MSG_GET_LAYOUT_SUB_CATEGORY &&msg.message != USER_MSG_GET_LAYOUT_SUB_CATEGORY_FILE)
	{
	::TranslateMessage(&msg); 
	::DispatchMessage(&msg); 
	}
	}*/
}

LRESULT CMediaLibraryPage::OnUpdateMediaList(WPARAM wp, LPARAM lp)
{
	//get infos from data manager
	// MEDIA_DETAIL_INFO mediaInfos;
	//UINT nBufID;
	DataBuf< pair< vector<Media>, MEDIA_DETAIL_INFO>  > MessageBuf;

	if (!WorkThreadDataManager< pair< vector<Media>, MEDIA_DETAIL_INFO > >
		::GetDataBuf((UINT)wp, MessageBuf))
	{
		return 0;
	}

	 BOOL bRet;	 const pair< vector<Media>, MEDIA_DETAIL_INFO> & MediasInfoPair = *MessageBuf.Get(); 
	 const vector<Media> &Medias = MediasInfoPair.first;
	 const MEDIA_DETAIL_INFO& MediaSyncInfo = MediasInfoPair.second;
	 
	// DoMessage();

 
	 CListCtrlEx* pMediaList = m_wndMediaLibraryList.m_wndStateTab.m_MainList[CATEGORY_LIST_TAB4];
	 if(Medias.size() <= 0)
	 {
		 if(pMediaList->GetItemCount())
			pMediaList->DeleteAllItems();
		 return 0;
	 }

	 std::map<INT, Media> &midiasExist = *m_wndMediaLibraryList.m_wndStateTab.GetMediaMap();
	 //Add /modify  some medias info to list.
	 for( int i = 0;i < Medias.size();i++)
	 {
		DoMessage();
		Media  info = Medias[i];
		std::map<INT, Media>::iterator it = midiasExist.find(info.GetMeidaID());
		if(it == midiasExist.end()) // not in list,add it.
		{
			midiasExist[info.GetMeidaID()] = info;
			UpdateToMediaList(pMediaList,info,MediaSyncInfo,TRUE);
			continue;
		}
		UpdateToMediaList(pMediaList,info,MediaSyncInfo);
		
	 }
	 m_wndMediaLibraryList.m_wndStateTab.ReleaseMediaMap(NULL);
	return 0;
}

/*
void  CMediaLibraryPage::UpdateMediaList()
{
	CListCtrlEx* pMediaList = m_wndMediaLibraryList.m_wndStateTab.m_MainList[CATEGORY_LIST_TAB4];
	if(::IsWindow(pMediaList->m_hWnd))
	{
		    CMediaLibraryCategoryCtrl::ITEM_TYPE ItemType;
			INT categoryID = m_wndMediaLibraryCategory.m_MediaLibraryCategory.GetSelectedCategory(ItemType);

			DWORD MessageID = USER_MSG_UPDATE_MEDIA_CATEGORY_LIST;

			if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,
				(WPARAM)this->GetSafeHwnd(), (LPARAM)categoryID))//post thread msg
			{
				TRACE1("post message failed,errno:%d\n",::GetLastError());
			}

	}

}
*/

void CMediaLibraryPage::OnRefreshCurList()
{
    // wp is the active tab index
    int index = (int)m_wndMediaLibraryList.m_wndStateTab.GetActiveTab();

    if (index == UPLOAD_LIST_TAB2)     //upload list tab
    {
        SendMessage(MEDIA_UPLOAD_LIST, 0L,  0L);
    }
#ifndef PROFESSIONAL_EDITION
    else if (index == DOWNLOAD_LIST_TAB3)    //download list tab
    {
        SendMessage(MEDIA_DOWNLOAD_LIST, 0L,  0L);
    }
#endif
    else if (index == CATEGORY_LIST_TAB4)
    {
		m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.RefreshMediaGrid();
   //     SendMessage(MEDIA_CATEGORY_REFRESH, 0L,  0L);
    }
    else if (index == MEDIA_LIST_TAB1)
    {
        SendMessage(MEDIALIB_SEARCH, 0L,  0L);
    }
}

void CMediaLibraryPage::OnTimer(UINT_PTR nIDEvent)
{
    if (IsWindowVisible())
    {
        CBaseControllerPage::OnTimer(nIDEvent);

        CListCtrlEx* pMediaList = NULL;
#ifndef PROFESSIONAL_EDITION
        if (DOWNLOAD_LIST_TAB3 == m_wndMediaLibraryList.m_wndStateTab.GetActiveTab())
        {
            pMediaList = m_wndMediaLibraryList.m_wndStateTab.m_MainList[DOWNLOAD_LIST_TAB3];
        }
        else
#endif
        if (UPLOAD_LIST_TAB2 == m_wndMediaLibraryList.m_wndStateTab.GetActiveTab())
        {
            pMediaList = m_wndMediaLibraryList.m_wndStateTab.m_MainList[UPLOAD_LIST_TAB2];
        }

		if (CATEGORY_LIST_TAB4 == m_wndMediaLibraryList.m_wndStateTab.GetActiveTab() && nIDEvent == MEDIALIST_REFRESH_TIMER)
        {
           // pMediaList = m_wndMediaLibraryList.m_wndStateTab.m_MainList[CATEGORY_LIST_TAB4];
		//	UpdateMediaList();
	//		m_wndMediaLibraryList.m_wndStateTab.m_mediaGrid.OnTimerRefresh();
			return;
        }

        if (pMediaList)
        {
            MEDIA_LIST MediaList;
            int endIdx = pMediaList->GetTopIndex() + pMediaList->GetCountPerPage();
            for (int idx = pMediaList->GetTopIndex(); idx <= endIdx; idx++)
            {
                if (pMediaList->IsItemVisible(idx))
                {
                    INT MediaID = (INT)pMediaList->GetItemData(idx);
                    INT OrigMediaID = MediaID;

                    TASK_BASIC_INFO BasicInfo;
                    HRESULT hr = GetMediaLibraryInterface->CheckTaskStatus(MediaID, BasicInfo);
                    if (SUCCEEDED(hr))
                    {
                        TCHAR sPercent[10];
                        CString szPercent, szPercentGenerateInfo, szPercentVerify;

                        if (BasicInfo.MediaInfoGeneratePercent < 100.0)
                        {
                            _itot_s(BasicInfo.MediaInfoGeneratePercent, sPercent, 10);
                            szPercentGenerateInfo = sPercent;
                            szPercentGenerateInfo  += _T("%");
                        }
                        else
                        {
                            szPercentGenerateInfo = _T("100%");
                        }

                        if (BasicInfo.Percent < 100.0)
                        {
                            _itot_s(BasicInfo.Percent, sPercent, 10);
                            szPercent = sPercent;
                            szPercent  += _T("%");
                        }
                        else
                        {
                            szPercent = _T("100%");
                        }

                        if (BasicInfo.SecurityVerifyPercent < 100.0)
                        {
                            _itot_s(BasicInfo.SecurityVerifyPercent, sPercent, 10);
                            szPercentVerify = sPercent;
                            szPercentVerify  += _T("%");
                        }
                        else
                        {
                            szPercentVerify = _T("100%");
                        }

						CString strStat = ConvertStats2String(BasicInfo.TaskStatus).c_str();
#if defined(PROFESSIONAL_EDITION) || defined(STANDALONE_EDITION)
						if(strStat == _T("Server insufficient space"))
						{
							strStat =  _T("Low Disk Space");
						}
#endif

						pMediaList->SetItemText(idx, CMediaLibraryListTab::MEDIA_TASK_STATS,Translate(strStat));
                        pMediaList->SetItemText(idx, CMediaLibraryListTab::MEDIA_TASK_PERCENT, (LPCTSTR)szPercent+szPercentGenerateInfo+szPercentVerify);
                        CString szFinishedSize;
                        szFinishedSize.Format(_T("%s/%s"), FormatSizeString(BasicInfo.Size * BasicInfo.Percent / 100.0),
                            FormatSizeString(BasicInfo.Size));
                        pMediaList->SetItemText(idx, CMediaLibraryListTab::MEDIA_TASK_SIZE, szFinishedSize);
                        pMediaList->SetItemText(idx, CMediaLibraryListTab::MEDIA_TASK_AVALIABLE, Translate(BasicInfo.bAvailable ? _T("True") : _T("False")));

                        if (OrigMediaID != MediaID)
                        {
                            pMediaList->SetItemText(idx,
                                CMediaLibraryListTab::MEDIA_TASK_INDEX, StringUtility::itos(MediaID).c_str());
                            pMediaList->SetItemData(idx, MediaID);

                            CString timeDur;
                            CString timeApprove;
                            CString status;

                            CString csDimension;
                            if (BasicInfo.Width || BasicInfo.Height)
                            {
                                csDimension.Format(_T("%d * %d"), BasicInfo.Width, BasicInfo.Height);
                            }

                            if (BasicInfo.Duration.GetTotalSeconds() > 0)
                            {
                                timeDur = BasicInfo.Duration.Format(_T("%H:%M:%S"));
                                _tprintf_s(_T("%s\n"), (LPCTSTR)timeDur);
                            }

                            pMediaList->SetItemText(idx, CMediaLibraryListTab::MEDIA_TASK_NAME, BasicInfo.Name.c_str());
                            pMediaList->SetItemText(idx, CMediaLibraryListTab::MEDIA_TASK_TYPE, BasicInfo.Type.c_str());
							//  pMediaList->SetItemText(i, CMediaLibraryLissetTab::MEDIA_TASK_CATEGORY, it->second.Category.c_str());	
                          //  CString fullPath("");
                         //   GetCategoryFullPath(BasicInfo.CategoryID, fullPath);
                         //   pMediaList->SetItemText(idx, CMediaLibraryListTab::MEDIA_TASK_CATEGORY, fullPath);
                            pMediaList->SetItemText(idx, CMediaLibraryListTab::MEDIA_TASK_DIMENSION, (LPCTSTR)csDimension);
                            pMediaList->SetItemText(idx, CMediaLibraryListTab::MEDIA_TASK_DURATION, (LPCTSTR)timeDur);
                        }
                    }
                    else
                    {
                        pMediaList->DeleteItem(idx);
					}
				}
            }
        }
    }
}

void CMediaLibraryPage::GetCategoryFullPath( int CategoryID, CString& fullPath)
{
    vector<FilterPath> categoryName;
    HRESULT hr = GetControllerInterface->GetFilterFullPath(FilterType_Media, CategoryID, categoryName);
    if (!SUCCEEDED(hr))
    {
        TRACE0("Failed to get filter full path!");
        return;
    }
    auto ittmp = categoryName.begin(), ittmpe = categoryName.end();
    for (; ittmp != ittmpe; )
    {
        fullPath += (*ittmp).m_szFilterName.c_str();
        ittmp++;
        if (ittmp != ittmpe)
        {
            fullPath += _T("\\");
        }
    }
}
