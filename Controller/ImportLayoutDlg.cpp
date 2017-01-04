#include "stdafx.h"
#include "Controller.h"
#include "LayoutElement.h"
#include "ImportLayoutDlg.h"
#include "SchedulerPage.h"
#include "afxdialogex.h"
#include "ScheduleCtrl.h"
#include "Localization/Localization.h"


#define  CLIP_CNT 100

IMPLEMENT_DYNAMIC(ImportLayoutDlg, CDialogEx)

ImportLayoutDlg::ImportLayoutDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ImportLayoutDlg::IDD, pParent)
    , m_bInited (FALSE)
    , m_Status (MC_STATUS_OFFLINE)
    , m_bStarted (FALSE)
{
	m_LayoutElement = NULL;
}

ImportLayoutDlg::~ImportLayoutDlg()
{
	if (m_LayoutElement)
	{
		delete m_LayoutElement;
		m_LayoutElement = NULL;
	}
}

void ImportLayoutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_UPLOAD_INFO, m_JobDesc);
	DDX_Control(pDX, IDC_UPLOAD_PROGRESS, m_ctrlProgress);
	DDX_Control(pDX, IDC_ROOT_DIR, m_RootWorkdirectory);
}


BEGIN_MESSAGE_MAP(ImportLayoutDlg, CDialogEx)
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_BN_CLICKED(ID_UPLOAD_START, &ImportLayoutDlg::OnBnClickedUploadStart)
    ON_BN_CLICKED(IDC_UPLOAD_PAUSE, &ImportLayoutDlg::OnBnClickedUploadPause)
    ON_BN_CLICKED(IDC_UPLOAD_CANCEL, &ImportLayoutDlg::OnBnClickedUploadCancel)
	ON_EN_CHANGE(IDC_ROOT_DIR, &ImportLayoutDlg::OnEnChangeRootDir)
	ON_BN_CLICKED(IDOK, &ImportLayoutDlg::OnBnClickedOk)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(ImportLayoutDlg)
	//EASYSIZE(IDC_STATIC_INFO_GROUP,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP


// CScheduleItemCtrl message handlers
BOOL ImportLayoutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;

    SetTimer(1,1000,NULL);

    m_ctrlProgress.SetRange(0, CLIP_CNT);

    //SetWindowText(Translate(_T("Import Layout")));

	GetDlgItem(ID_UPLOAD_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_UPLOAD_CANCEL)->EnableWindow(TRUE);
    GetDlgItem(IDOK)->EnableWindow(FALSE);

    //OnBnClickedUploadStart();

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void ImportLayoutDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

void ImportLayoutDlg::OnBnClickedUploadStart()
{
	CString filePath;

	m_RootWorkdirectory.GetWindowText(filePath);
    if(filePath.GetLength() == 0 ) 
	{
		MessageBox(Translate(_T("Please Select directory !")), Translate(_T("Warning:Import Layout")), MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	if(filePath.Right(1) != _T("\\"))
	{
		filePath += "\\";
	}
	CString dirPath = filePath;
	filePath +=  _T("index.xml");

	if( _taccess( filePath, 0 ) == -1 )
	{
#ifdef SERVER_EDITION
		MessageBox(Translate(_T("File not found!")),Translate(_T("Error:Controller")), MB_OK|MB_ICONERROR);
#else
		MessageBox(Translate(_T("File not found!")),Translate(_T("Error:Manager")), MB_OK|MB_ICONERROR);
#endif
		return ;
	}

	if (m_LayoutElement)
	{
		delete m_LayoutElement;
		m_LayoutElement = NULL;
	}
	m_LayoutElement = new LayoutElement;
	m_LayoutElement->SetLayoutFile(filePath);

	HRESULT hr = m_LayoutElement->LoadLayout(filePath, GetControllerApp->m_plugMgr, _T(""), _T(""));
	if (FAILED(hr))
	{
		GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,	(LPARAM)(Translate(_T("Can not load the layout, try to open it again!"))).GetString());
		return ;
	}

	{
		DWORD64 LayoutSize = 0;
		std::vector<int> mediaIDList;
		mediaIDList.empty();
		list<std::shared_ptr<LayerElement>> layers = m_LayoutElement->GetAllLayers();
		list<std::shared_ptr<LayerElement>> :: iterator itLayerElement;
		for(itLayerElement = layers.begin(); itLayerElement != layers.end(); itLayerElement++)
		{
			vector<std::shared_ptr<MediaElement>> medias = (*itLayerElement)->GetMediaList();
			vector<std::shared_ptr<MediaElement>> :: iterator itm;
			for(itm = medias.begin(); itm != medias.end(); itm++)
			{
				int nMediaID = (*itm)->GetMediaID();
				if (nMediaID >= 0)
				{
					vector<int>::iterator result = find( mediaIDList.begin(), mediaIDList.end(), nMediaID ); //find nMediaID
					if  ( result == mediaIDList.end() ) //not find
					{
						mediaIDList.push_back(nMediaID);
						LayoutSize += (*itm)->GetMediaSize();
					}
				}
			}
		}
	
		// collect exported layout list
		DWORD64 nMediaServerFreeSize = 0;
		INT FreeRemainMB = 0;
		hr = GetMediaLibraryInterface->GetServerDiskUnuseSpace(nMediaServerFreeSize);
		if(hr == S3_SUCCESS)
		{
			FreeRemainMB = static_cast<INT>(nMediaServerFreeSize>>20);
		}
		else 
		{
			FreeRemainMB = 0;
		}
		//INT FreeRemainMB = SysCall::GetFreeSpaceMB(dirPath.GetString());

		// copy layout to disc
		INT LayoutSizeMB = static_cast<INT>(LayoutSize>>20);
		if (LayoutSizeMB > (FreeRemainMB - 1))
		{
			CString cWarning, str1, str2;
			str1.Format(_T("%d MB"),LayoutSizeMB);
			str2.Format(_T("%d MB"),FreeRemainMB);
			cWarning = m_LayoutElement->GetLayoutName() + _T("(") + str1 +
				Translate(_T(")may be too big for this disk(Free size: ")) +
				str2 + Translate(_T("), Can't export to this disk!"));
			MessageBox(cWarning, Translate(_T("Tips:Export file")),MB_OK|MB_ICONEXCLAMATION);
			return;
		}
	}

	//--begin by RaymondLiu
	COleDateTime dtS = COleDateTime::GetCurrentTime();
	MediaFilter Category;
	INT iMediaCategoryID = -1;
	CString cstrMediaCategoryName;
	cstrMediaCategoryName.Format(_T("%s-%2d%2d%2d"), m_LayoutElement->GetLayoutName().GetString(), dtS.GetHour(), dtS.GetMinute(), dtS.GetSecond());
	//create a new mediafilter named cstrLayoutName
	Category.ChangeParentFilterID(-1);
	Category.Rename(cstrMediaCategoryName.GetString());
	HRESULT hr2 = GetControllerInterface->AddMediaFilter(Category, iMediaCategoryID);
	if (!SUCCEEDED(hr2))
	{
		if (hr2 == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
		{
			TRACE0("Failed to add media filter\n");
			GetControllerApp->OnControllerServerOffline();
			return;
			//return E_UNEXPECTED;
		}
	}

	hr2 = GetControllerInterface->GetMediaFilter(iMediaCategoryID, Category);
	if (!SUCCEEDED(hr2))
	{
		TRACE0("Failed to get media filter\n");
		GetControllerApp->OnControllerServerOffline();
		return;
		//return E_UNEXPECTED;
	}

	list<std::shared_ptr<LayerElement>> layers = m_LayoutElement->GetAllLayers();
	list<std::shared_ptr<LayerElement>> :: iterator it;
	for(it = layers.begin(); it != layers.end(); it++)
	{
		vector<std::shared_ptr<MediaElement>> medias = (*it)->GetMediaList();
		vector<std::shared_ptr<MediaElement>> :: iterator itm;
		for(itm = medias.begin(); itm != medias.end(); itm++)
		{
			CString File;
			if (!(*itm)->GetMediaFile().IsEmpty())
			{
				File = (*itm)->GetMediaFile();
			}
			/* i do not know why --by RaymondLiu 
			if (!(*itm)->GetMessageFile().IsEmpty())
			{
				File = (*itm)->GetMessageFile();
			}
			*/

			if (File.IsEmpty() )
			{
				continue;
			}

			int nMediaID = (*itm)->GetMediaID();
			if (nMediaID >= 0)
			{
				std::map<int, HANDLE>::iterator it = m_hMediaFileTask.find(nMediaID);
				if (it != m_hMediaFileTask.end())
				{
					//exist in m_hMediaFileTask, means it has been added to m_hMediaFileTask
					continue;
				}
				
				HANDLE hTaskHandle;
				CString strTemp = dirPath + File;
				hr = GetMediaLibraryInterface->UploadSingleMedia(strTemp.GetString(), Category, hTaskHandle);
				if (!SUCCEEDED(hr) || hTaskHandle == NULL)
				{
				}
				else 
				{	
					m_hMediaFileTask.insert(pair<int,HANDLE>(nMediaID, hTaskHandle));
				}
			}
		}
	}

	// TODO: Add your control notification handler code here
    m_bStarted = TRUE;
	
    GetDlgItem(ID_UPLOAD_START)->EnableWindow(FALSE);
    GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(TRUE);
}


void ImportLayoutDlg::OnBnClickedUploadPause()
{
    // TODO: Add your control notification handler code here
    m_bStarted = FALSE;


    GetDlgItem(ID_UPLOAD_START)->EnableWindow(TRUE);
    GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(FALSE);
}


void ImportLayoutDlg::OnBnClickedUploadCancel()
{
    // TODO: Add your control notification handler code here
	if(TRUE == m_bStarted || m_hMediaFileTask.size())
	{
		if (MessageBox(Translate(_T("Are you sure to stop uploading?")), Translate(_T("Tips:Upload file")),MB_YESNO|MB_ICONQUESTION) != IDYES)
		{
			return;
		}
	}
	//KillTimer(1);
	std::map<INT,HANDLE>::iterator it;
	for(it = m_hMediaFileTask.begin();it != m_hMediaFileTask.end(); ++it)
	{
		HRESULT hr = GetMediaLibraryInterface->DeleteFileTask(it->second);
		if (!SUCCEEDED(hr))
		{
			//TRACE0("Error happen in GetMediaLibraryInterface->DeleteFileTask, ImportLayoutDlg");
		}
	}
	m_hMediaFileTask.clear();
    CDialogEx::OnCancel();
}

void ImportLayoutDlg::OnTimer(UINT_PTR)
{
    if(m_bStarted)
    {
        RefreshStatus();
    }
}


void ImportLayoutDlg::GetStatus(FLOAT& Percent, CString& Desc)
{
    DWORD64 FinishedSize = 0;
    DWORD64 TotalSize = 0;
    DWORD   FinishedCnt = 0;
    DWORD   TotalCnt = 0;
 
    for (auto it = m_hMediaFileTask.begin(); it != m_hMediaFileTask.end(); it++)
    {
        FILE_STATUS FileInfo;
        HRESULT hr = GetMediaLibraryInterface->GetFileStatus(it->second, FileInfo);
        if (!SUCCEEDED(hr))
        {
            TRACE0("Error happen in GetFileStatus, PreviewDownLoadDlg");
        }

        FinishedCnt += FileInfo.m_FinishedCnt;
        FinishedSize += FileInfo.m_FinishedSize;
        TotalCnt += FileInfo.m_TotalCnt;
        TotalSize += FileInfo.m_TotalSize;
    }

	if(TotalSize)
	{
		Percent = FLOAT( FLOAT((10000 * FinishedSize) / TotalSize) / 100.0);
	}
	else
	{
		Percent = 0;
	}

    Desc.Format(_T("Files: %d/%d    Size: %s/%s"), FinishedCnt, TotalCnt,
        FormatSizeString(FinishedSize), FormatSizeString(TotalSize));

}
void ImportLayoutDlg::RefreshStatus()
{
    // TODO: Add your control notification handler code here

    FLOAT nProgress = 0;
    BOOL bFinished = FALSE;
    GetStatus(nProgress, m_JobDesc);

    m_Progress = INT(nProgress);

    if (m_Progress >= CLIP_CNT)
    {
        m_ctrlProgress.SetPos(CLIP_CNT);
		GetDlgItem(ID_UPLOAD_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(FALSE);
		GetDlgItem(IDC_UPLOAD_CANCEL)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
    }
    else
    {
        m_ctrlProgress.SetPos(m_Progress);

		if (m_bStarted)
		{
			GetDlgItem(ID_UPLOAD_START)->EnableWindow(FALSE);
			GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(ID_UPLOAD_START)->EnableWindow(TRUE);
			GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(FALSE);
		}

        if (bFinished)
        {
#define Translate(x) x
            m_JobDesc += Translate(_T(" - Error happened, Unable to Upload!"));
#undef Translate

			GetDlgItem(ID_UPLOAD_START)->EnableWindow(FALSE);
			GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(FALSE);
			GetDlgItem(IDC_UPLOAD_CANCEL)->EnableWindow(FALSE);
			GetDlgItem(IDOK)->EnableWindow(TRUE);
        }
    }

    UpdateData(FALSE);
}

void ImportLayoutDlg::OnEnChangeRootDir()
{
	// TODO:
	
}



void ImportLayoutDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	std::vector<int> mediaIDList;
	mediaIDList.clear();

	list<std::shared_ptr<LayerElement>> layers = m_LayoutElement->GetAllLayers();
	list<std::shared_ptr<LayerElement>> :: iterator it;
	for(it = layers.begin(); it != layers.end(); it++)
	{
		vector<std::shared_ptr<MediaElement>> medias = (*it)->GetMediaList();
		vector<std::shared_ptr<MediaElement>> :: iterator itm;
		for(itm = medias.begin(); itm != medias.end(); itm++)
		{
			INT nMediaID = (*itm)->GetMediaID();
			if ( nMediaID >= 0)
			{
				std::map<int, HANDLE>::iterator it = m_hMediaFileTask.find(nMediaID);
				if (it != m_hMediaFileTask.end())
				{
					BaseTask *pTask = (BaseTask *)(it->second);
					INT nNewMediaID = pTask->GetMediaID();
					std::tstring szServerFilePath = pTask->GetServerFilePath();
					//set new MediaID
					(*itm)->SetMediaID(nNewMediaID);
					(*itm)->SetMediaFile(szServerFilePath.c_str());

					mediaIDList.push_back(nNewMediaID);
				}
			}
		}
	}

#ifdef SERVER_EDITION
	std::vector<Media> mediaList;
	//std::vector<int> mediaIDList;
	//GetSelectItem(mediaIDList);

	HRESULT hr = GetControllerInterface->GetMedias(mediaIDList, mediaList);
	if (!SUCCEEDED(hr))
	{
		MessageBox(Translate(_T("Failed to get the Media!")), Translate(_T("Error:Media library page")), MB_ICONERROR);
		return;
	}
	auto it2 = mediaList.begin(), ite = mediaList.end();
	Audit tmpAudit;

	for (; it2 != ite; ++it2)
	{
		tmpAudit = it2->GetAudit();
		Audit tmp(tmpAudit.GetRequestUserID(), tmpAudit.GetRequestTime(), GetControllerInterface->GetUserId(),
			S3Time::Now(), tmpAudit.CovertStatus(Audit::APPROVED));

		it2->SetAudit(tmp);
		// modify controller server data
		hr = GetControllerInterface->AuditMedia(it2->GetMeidaID(), it2->GetAudit());
		if (!SUCCEEDED(hr))
		{
			MessageBox(Translate(_T("Failed to Approve the media!")), Translate(_T("Error:Media library page")), MB_ICONERROR);
			return;
		}
	}
#endif
	int nStrLen;
	TCHAR szTmpPath[MAX_PATH];
	nStrLen = GetTempPath(MAX_PATH, szTmpPath);
	szTmpPath[nStrLen] = 0x00;
	
	CString xmlFilename = szTmpPath;
	xmlFilename += _T("\\index.xml");
	CString Contentxml;
	m_LayoutElement->Save(xmlFilename, FALSE,Contentxml);

	CDialogEx::OnOK();
}
