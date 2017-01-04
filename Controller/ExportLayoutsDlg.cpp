#include "stdafx.h"
#include "Controller.h"
#include "ExportLayoutsDlg.h"
#include "SchedulerPage.h"
#include "afxdialogex.h"
#include "ScheduleCtrl.h"
#include "Localization/Localization.h"

#define  CLIP_CNT 100

IMPLEMENT_DYNAMIC(ExportLayoutsDlg, CDialogEx)

ExportLayoutsDlg::ExportLayoutsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ExportLayoutsDlg::IDD, pParent)
    , m_bInited (FALSE)
    , m_Status (MC_STATUS_OFFLINE)
    , m_bStarted (FALSE)
	, m_bJobInited(FALSE)
	, m_JobFileCnt(0)
{
	m_JobDesc = Translate(_T("Initializing Jobs"));
	m_JobDesc += _T("......0.0%");
}

ExportLayoutsDlg::~ExportLayoutsDlg()
{
}

void ExportLayoutsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_UPLOAD_INFO, m_JobDesc);
    DDX_Control(pDX, IDC_UPLOAD_PROGRESS, m_ctrlProgress);
}


BEGIN_MESSAGE_MAP(ExportLayoutsDlg, CDialogEx)
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_BN_CLICKED(ID_UPLOAD_START, &ExportLayoutsDlg::OnBnClickedUploadStart)
    ON_BN_CLICKED(IDC_UPLOAD_PAUSE, &ExportLayoutsDlg::OnBnClickedUploadPause)
    ON_BN_CLICKED(IDC_UPLOAD_CANCEL, &ExportLayoutsDlg::OnBnClickedUploadCancel)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(ExportLayoutsDlg)
	//EASYSIZE(IDC_STATIC_INFO_GROUP,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP


// CScheduleItemCtrl message handlers
BOOL ExportLayoutsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;

    SetTimer(1,1000,NULL);

    m_ctrlProgress.SetRange(0, CLIP_CNT);

    SetWindowText(Translate(_T("Exporting Layouts...")));

    GetDlgItem(IDOK)->EnableWindow(FALSE);

    OnBnClickedUploadStart();

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void ExportLayoutsDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

void ExportLayoutsDlg::OnBnClickedUploadStart()
{
    // TODO: Add your control notification handler code here
    m_bStarted = TRUE;


    GetDlgItem(ID_UPLOAD_START)->EnableWindow(FALSE);
    GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(TRUE);
}


void ExportLayoutsDlg::OnBnClickedUploadPause()
{
    // TODO: Add your control notification handler code here
    m_bStarted = FALSE;


    GetDlgItem(ID_UPLOAD_START)->EnableWindow(TRUE);
    GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(FALSE);
}


void ExportLayoutsDlg::OnBnClickedUploadCancel()
{
    // TODO: Add your control notification handler code here
    if (MessageBox(Translate(_T("Are you sure to stop exporting?")), Translate(_T("Tips:Export layout")),MB_YESNO|MB_ICONQUESTION) != IDYES)
    {
        return;
    }

    KillTimer(1);
	std::vector<HANDLE>::iterator it;
	for(it = m_hFileTask.begin();it != m_hFileTask.end(); ++it)
	{
		HRESULT hr = GetMediaLibraryInterface->DeleteFileTask(*it);
		if (!SUCCEEDED(hr))
		{
			//TRACE0("Error happen in CheckMediaListTask, PreviewDownLoadDlg");
		}
	}
	m_hFileTask.empty();
    CDialogEx::OnCancel();
}

void ExportLayoutsDlg::OnTimer(UINT_PTR)
{
	if (!m_bJobInited)
	{
		if (S_OK != InitJob(m_bJobInited))
		{
            KillTimer(1);
			MessageBox(Translate(_T("Media Library Issue, Failed to download!")), Translate(_T("Error:Export file")), MB_ICONERROR);

			std::vector<HANDLE>::iterator it;
			for(it = m_hFileTask.begin();it != m_hFileTask.end(); ++it)
			{
				HRESULT hr = GetMediaLibraryInterface->DeleteFileTask(*it);
				if (!SUCCEEDED(hr))
				{
					//TRACE0("Error happen in CheckMediaListTask, PreviewDownLoadDlg");
				}
			}
			m_hFileTask.empty();
            CDialogEx::OnCancel();
		}		
	}
	if(m_bJobInited && m_bStarted)
    {
        RefreshStatus();
    }
}


void ExportLayoutsDlg::GetStatus(FLOAT& Percent, CString& Desc, BOOL &bCompleted)
{
    DWORD64 FinishedSize = 0;
    DWORD64 TotalSize = 0;
    DWORD   FinishedCnt = 0;
    DWORD   TotalCnt = 0;

    if (!m_MediaList.m_MediaIDs.empty())
    {
        MEDIA_BASIC_INFO MediaInfo;
        HRESULT hr = GetMediaLibraryInterface->CheckTaskListStatus(m_MediaList, MediaInfo);
        if (!SUCCEEDED(hr))
        {
            if (HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID) == hr)
            {
                TRACE0("Error happen in CheckMediaListTask, PreviewDownLoadDlg");
                KillTimer(1);
			    MessageBox(Translate(_T("Media Library Issue, Failed to download!")), Translate(_T("Error:Export file")), MB_ICONERROR);

                std::vector<HANDLE>::iterator it;
                for(it = m_hFileTask.begin();it != m_hFileTask.end(); ++it)
                {
                    HRESULT hr = GetMediaLibraryInterface->DeleteFileTask(*it);
                    if (!SUCCEEDED(hr))
                    {
                        //TRACE0("Error happen in CheckMediaListTask, PreviewDownLoadDlg");
                    }
                }
                m_hFileTask.empty();
                CDialogEx::OnCancel();
            }
        }

        FinishedCnt += MediaInfo.m_FinishedCnt;
        FinishedSize += MediaInfo.m_FinishedSize;
        TotalCnt += MediaInfo.m_TotalCnt;
        TotalSize += MediaInfo.m_TotalSize;
    }

    for (auto it = m_hFileTask.begin(); it != m_hFileTask.end(); it++)
    {
        FILE_STATUS FileInfo;
        HRESULT hr = GetMediaLibraryInterface->GetFileStatus(*it, FileInfo);
        if (!SUCCEEDED(hr))
        {
            if (HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID) == hr)
            {
                TRACE0("Error happen in GetFileStatus, PreviewDownLoadDlg");
                KillTimer(1);
                MessageBox(Translate(_T("Media Library Issue, Failed to download!")), Translate(_T("Error:Export file")), MB_ICONERROR);

                std::vector<HANDLE>::iterator it;
                for(it = m_hFileTask.begin();it != m_hFileTask.end(); ++it)
                {
                    HRESULT hr = GetMediaLibraryInterface->DeleteFileTask(*it);
                    if (!SUCCEEDED(hr))
                    {
                        //TRACE0("Error happen in CheckMediaListTask, PreviewDownLoadDlg");
                    }
                }
                m_hFileTask.empty();
                CDialogEx::OnCancel();
            }
        }

        FinishedCnt += FileInfo.m_FinishedCnt;
        FinishedSize += FileInfo.m_FinishedSize;
        TotalCnt += FileInfo.m_TotalCnt;
        TotalSize += FileInfo.m_TotalSize;
    }

    if(TotalSize == 0)
    {
        Percent = 100;
    }
    else
    {
    Percent = FLOAT( FLOAT((10000 * FinishedSize) / TotalSize) / 100.0);
    }

	if(Percent == 100 && FinishedCnt == TotalCnt)
	{
		bCompleted = TRUE;
	}
	else
	{
		bCompleted = FALSE;
	}
    Desc.Format(Translate(_T("Files: %d/%d    Size: %s/%s")), FinishedCnt, TotalCnt,
        FormatSizeString(FinishedSize), FormatSizeString(TotalSize));

}

void ExportLayoutsDlg::RefreshStatus()
{
    // TODO: Add your control notification handler code here

    FLOAT nProgress = 0;
    BOOL bFinished = FALSE;
	BOOL bCompleted = FALSE;
    GetStatus(nProgress, m_JobDesc, bCompleted);

    m_Progress = INT(nProgress);

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

    if (m_Progress >= CLIP_CNT)
    {
        m_ctrlProgress.SetPos(CLIP_CNT);
		if(bCompleted)
		{
			GetDlgItem(ID_UPLOAD_START)->EnableWindow(FALSE);
			GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(FALSE);
			GetDlgItem(IDC_UPLOAD_CANCEL)->EnableWindow(FALSE);
			GetDlgItem(IDOK)->EnableWindow(TRUE);
		}
    }
    else
    {
        m_ctrlProgress.SetPos(m_Progress);

        if (bFinished)
        {
            m_JobDesc += Translate(_T("- Error happened, Unable to export!"));

            GetDlgItem(ID_UPLOAD_START)->EnableWindow(FALSE);
            GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(FALSE);
            GetDlgItem(IDC_UPLOAD_CANCEL)->EnableWindow(FALSE);
            GetDlgItem(IDOK)->EnableWindow(TRUE);
        }
    }

    UpdateData(FALSE);
}

BOOL ExportLayoutsDlg::AddDownloadLayoutJob(CString szServerFolder, CString szLocalFolder)
{
    FILE_TASK FileTask;
    FileTask.AddFile(szLocalFolder.GetString(), szServerFolder.GetString(), ML_FOLDER);

    HANDLE hTaskHandle;
    HRESULT hr = GetMediaLibraryInterface->DownloadFile(FileTask,hTaskHandle);
    if (!SUCCEEDED(hr))
    {
        return FALSE;
    }

    m_hFileTask.push_back(hTaskHandle);

    return TRUE;
}

BOOL ExportLayoutsDlg::AddMediaListJob(const MEDIA_LIST& Medialist, CString szLocalFolder)
{
    if(Medialist.m_MediaIDs.size() == 0) return TRUE;

	m_AddMediaListJob +=Medialist;
	m_JobFileCnt += Medialist.m_MediaIDs.size();
	m_szLocalFolder = szLocalFolder;

	return TRUE;
#if 0
    std::vector<Media> mediaList;
    HRESULT hr = GetControllerInterface->GetMedias(Medialist.m_MediaIDs, mediaList);

    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
        }
        else
        {
            TRACE0("Failed to Get media!\n");
        }
        return FALSE;
    }

    for(int i=0; i< mediaList.size(); i++)
    {
        FILE_TASK FileTask;
        FileTask.AddFile((LPCTSTR)szLocalFolder, mediaList[i].GetMediaServerPath(), ML_FILE);

        HANDLE hTaskHandle;
        HRESULT hr = GetMediaLibraryInterface->DownloadFile(FileTask,hTaskHandle);
        if (!SUCCEEDED(hr))
        {
            return FALSE;
        }

        m_hFileTask.push_back(hTaskHandle);
    }

    return TRUE;
#endif

}


HRESULT ExportLayoutsDlg::InitJob(BOOL &bJobInited)
{
	if (m_AddMediaListJob.m_MediaIDs.empty())
	{
		bJobInited = TRUE;
		return S_OK;
	}

	vector<int>::const_iterator itMS = m_AddMediaListJob.m_MediaIDs.begin();    
	vector<int>::const_iterator itME = m_AddMediaListJob.m_MediaIDs.end();

	if (itMS != itME)
	{
		MEDIA_LIST Medialist;
		Medialist.AddMedia(*itMS);

		std::vector<Media> mediaList;
		HRESULT hr = GetControllerInterface->GetMedias(Medialist.m_MediaIDs, mediaList);

		if (!SUCCEEDED(hr))
		{
			if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
			{
				GetControllerApp->OnControllerServerOffline();
			}
			else
			{
				TRACE0("Failed to Get media!\n");
			}

			return hr;
			
		}

		for(int i=0; i< mediaList.size(); i++)
		{
            if (mediaList[i].GetMediaType() == _T("URL"))
            {
                continue;
            }
			
            FILE_TASK FileTask;
			FileTask.AddFile((LPCTSTR)m_szLocalFolder, mediaList[i].GetMediaServerPath(), ML_FILE);
/*

            MediaLibrary::TASK_STYLE taskStyle;
            taskStyle = (m_AddMediaListJob.m_MediaIDs.size() == m_JobFileCnt) ? MediaLibrary::TASK_IMMEDIATE : MediaLibrary::TASK_QUEUE;*/

			HANDLE hTaskHandle;
			HRESULT hr = GetMediaLibraryInterface->DownloadFile(FileTask, hTaskHandle);
			if (!SUCCEEDED(hr))
			{
				return hr;
			}

			m_hFileTask.push_back(hTaskHandle);
		}
		m_AddMediaListJob.m_MediaIDs.erase(itMS);
	}

	INT LeftJobFileCnt = 0;
	LeftJobFileCnt += m_AddMediaListJob.m_MediaIDs.size();


	m_JobDesc = Translate(_T("Initializing Jobs"));
	m_JobDesc += _T("......");
	CString tmp;
	tmp.Format(_T("%.2f%%"), 100*(1.0 - (FLOAT)LeftJobFileCnt/(FLOAT)m_JobFileCnt));
	m_JobDesc += tmp;

	UpdateData(FALSE);

	return FALSE;
}

