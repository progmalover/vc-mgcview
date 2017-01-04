#include "stdafx.h"
#include "Controller.h"
#include "PreviewDownLoadDlg.h"
#include "SchedulerPage.h"
#include "afxdialogex.h"
#include "ScheduleCtrl.h"
#include "Localization/Localization.h"

#define  CLIP_CNT 100

IMPLEMENT_DYNAMIC(PreviewDownLoadDlg, CDialogEx)

PreviewDownLoadDlg::PreviewDownLoadDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(PreviewDownLoadDlg::IDD, pParent)
    , m_bInited (FALSE)
    , m_Status (MC_STATUS_OFFLINE)
    , m_Progress (0)
    , m_bStarted (FALSE)
    , m_nExistLocalFiles (0)
    , m_ExistLocalFilesSize (0)
    , m_bJobInited (FALSE)
    , m_JobFileCnt(0)
{
	m_JobDesc = Translate(_T("Initializing Jobs"));
	m_JobDesc += _T("......0.0%");
    //m_JobDesc = _T("Initializing Jobs......0.0%");
}

PreviewDownLoadDlg::~PreviewDownLoadDlg()
{
}

void PreviewDownLoadDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_UPLOAD_INFO, m_JobDesc);
    DDX_Control(pDX, IDC_UPLOAD_PROGRESS, m_ctrlProgress);
}


BEGIN_MESSAGE_MAP(PreviewDownLoadDlg, CDialogEx)
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_BN_CLICKED(ID_UPLOAD_START, &PreviewDownLoadDlg::OnBnClickedUploadStart)
    ON_BN_CLICKED(IDC_UPLOAD_PAUSE, &PreviewDownLoadDlg::OnBnClickedUploadPause)
    ON_BN_CLICKED(IDC_UPLOAD_CANCEL, &PreviewDownLoadDlg::OnBnClickedUploadCancel)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(PreviewDownLoadDlg)
	//EASYSIZE(IDC_STATIC_INFO_GROUP,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP


// CScheduleItemCtrl message handlers
BOOL PreviewDownLoadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    BOOL bEnable = FALSE;
    UINT menuf = bEnable ? (MF_BYCOMMAND) : (MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
    CMenu* pSM = GetSystemMenu(FALSE);
    if (pSM)
    {
        pSM->EnableMenuItem(SC_CLOSE, menuf);
    }

    INIT_EASYSIZE;

    SetTimer(1,1000,NULL);

    m_ctrlProgress.SetRange(0, CLIP_CNT);

    SetWindowText(Translate(_T("Loading Media Files......")));
    GetDlgItem(IDOK)->EnableWindow(FALSE);

    OnBnClickedUploadStart();

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void PreviewDownLoadDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

void PreviewDownLoadDlg::OnBnClickedUploadStart()
{
    // TODO: Add your control notification handler code here
    m_bStarted = TRUE;

    // TODO: Add your control notification handler code here


    GetDlgItem(ID_UPLOAD_START)->EnableWindow(FALSE);
    GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(TRUE);
}


void PreviewDownLoadDlg::OnBnClickedUploadPause()
{
    // TODO: Add your control notification handler code here
    m_bStarted = FALSE;

    // TODO: Add your control notification handler code here


    GetDlgItem(ID_UPLOAD_START)->EnableWindow(TRUE);
    GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(FALSE);
}


void PreviewDownLoadDlg::OnBnClickedUploadCancel()
{
    // TODO: Add your control notification handler code here
    if (MessageBox(Translate(_T("Are you sure to stop downloading?")),Translate(_T("Tips:Download file")), MB_YESNO|MB_ICONQUESTION) != IDYES)
    {
        return;
    }

    if (!m_MediaList.m_MediaIDs.empty())
    {
        MEDIA_BASIC_INFO MediaInfo;
        HRESULT hr = GetMediaLibraryInterface->PauseTask(m_MediaList);
        if (!SUCCEEDED(hr))
        {
            TRACE0("Error happen in OnBnClickedUploadCancel (PauseTask), PreviewDownLoadDlg");
        }
    }

    CDialogEx::OnCancel();

}

void PreviewDownLoadDlg::OnTimer(UINT_PTR)
{
    if (!m_bJobInited)
    {
/*
        if (InitJob())
        {
            m_bJobInited = TRUE;
        }
*/

        if (FAILED(InitJob(m_bJobInited)))
        {
            KillTimer(1);
            MessageBox(Translate(_T("Media Library Issue, Failed to download!")), Translate(_T("Tips:Download file")), MB_ICONERROR);
            CDialogEx::OnCancel();
        }
    }

    if(m_bJobInited && m_bStarted)
    {
        RefreshStatus();
    }
}

HRESULT PreviewDownLoadDlg::InitJob(BOOL &bAllJobsInited)
{
    HRESULT hr = S_OK;

    bAllJobsInited = FALSE;

    if (m_AddDownloadLayoutJob.empty() && m_AddMediaListJob.m_MediaIDs.empty())
    {
        bAllJobsInited = TRUE;
        return S_OK;
    }

    std::vector<std::pair<CString, CString> >::iterator itFS = m_AddDownloadLayoutJob.begin();
    std::vector<std::pair<CString, CString> >::iterator itFE = m_AddDownloadLayoutJob.end();
    if (itFS != itFE)
    {
        FILE_TASK FileTask;
        FileTask.AddFile(itFS->first.GetString(), itFS->second.GetString(), ML_FOLDER);

        HANDLE hTaskHandle;
        hr = GetMediaLibraryInterface->DownloadFile(FileTask,hTaskHandle);
        if (!SUCCEEDED(hr))
        {
            return hr;
        }

        m_hFileTask.push_back(hTaskHandle);

        FILE_STATUS BasicInfo;
        hr = GetMediaLibraryInterface->GetFileStatus(hTaskHandle, BasicInfo);
        if (SUCCEEDED(hr))
        {
            m_nExistLocalFiles += BasicInfo.m_FinishedCnt;
            m_ExistLocalFilesSize += BasicInfo.m_FinishedSize;
        }
        else
        {
            return hr;
        }
        m_AddDownloadLayoutJob.erase(itFS);
    }
    else
    {
        vector<int>::const_iterator itMS = m_AddMediaListJob.m_MediaIDs.begin();    
        vector<int>::const_iterator itME = m_AddMediaListJob.m_MediaIDs.end();

        if (itMS != itME)
        {        
            MEDIA_LIST medialist;
            medialist.AddMedia(*itMS);

            hr = GetMediaLibraryInterface->DownloadMedia(medialist, MediaLibrary::TASK_IMMEDIATE);
            if (!SUCCEEDED(hr))
            {
                return hr;
            }

            MEDIA_BASIC_INFO BasicInfo;
            hr = GetMediaLibraryInterface->CheckTaskListStatus(medialist, BasicInfo);
            if (SUCCEEDED(hr))
            {
                m_nExistLocalFiles += BasicInfo.m_FinishedCnt;
                m_ExistLocalFilesSize += BasicInfo.m_FinishedSize;
            }
            else
            {
                return hr;
            }
            m_AddMediaListJob.m_MediaIDs.erase(itMS);
        }
    }


    INT LeftJobFileCnt = 0;
    LeftJobFileCnt += m_AddDownloadLayoutJob.size();
    LeftJobFileCnt += m_AddMediaListJob.m_MediaIDs.size();

	
	m_JobDesc = Translate(_T("Initializing Jobs"));
	m_JobDesc += _T("......");
    CString tmp;
	tmp.Format(_T("%.2f%%"), 100*(1.0 - (FLOAT)LeftJobFileCnt/(FLOAT)m_JobFileCnt));
	m_JobDesc += tmp;

    UpdateData(FALSE);

    return hr;
}

void PreviewDownLoadDlg::GetStatus(FLOAT& Percent, CString& Desc)
{
    DWORD64 FinishedSize = 0;
    DWORD64 TotalSize = 0;
    DWORD   FinishedCnt = 0;
    DWORD   TotalCnt = 0;

    vector<INT> FailedMedia;
    TString szFailedMedia;
    TString szSuspisionMedia;
    INT nSuspisionMedia = 0;

    if (!m_MediaList.m_MediaIDs.empty())
    {
        MEDIA_BASIC_INFO MediaInfo;
        HRESULT hr = GetMediaLibraryInterface->CheckTaskListStatus(m_MediaList, MediaInfo);
        if (!SUCCEEDED(hr))
        {
            TRACE0("Error happen in CheckMediaListTask, PreviewDownLoadDlg");
            if (HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID) == hr)
            {
                KillTimer(1);
                MessageBox(Translate(_T("Media Library Issue, Failed to download!")), Translate(_T("Tips:Download file")), MB_ICONERROR);

                HRESULT hr = GetMediaLibraryInterface->PauseTask(m_MediaList);
                if (!SUCCEEDED(hr))
                {
                    TRACE0("Error happen in OnBnClickedUploadCancel (PauseTask), PreviewDownLoadDlg");
                }
                               
                CDialogEx::OnCancel();
            }
            
        }

        FinishedCnt += MediaInfo.m_FinishedCnt;
        FinishedSize += MediaInfo.m_FinishedSize;
        TotalCnt += MediaInfo.m_TotalCnt;
        TotalSize += MediaInfo.m_TotalSize;

        for (auto it = MediaInfo.m_Medias.begin(); it != MediaInfo.m_Medias.end(); ++it)
        {
            TASK_BASIC_INFO& TaskInfo = it->second;
            if (GetMediaLibraryInterface->bFAILEDTASK(TaskInfo.TaskStatus) &&
                TaskInfo.TaskStatus != ML_NOT_SECURE)
            {
                szFailedMedia += TaskInfo.Name + _T(";");
                FailedMedia.push_back(it->first);
            }

            if (TaskInfo.TaskStatus == ML_NOT_SECURE)
            {
                nSuspisionMedia ++;
                szSuspisionMedia += TaskInfo.Name + _T(";");
            }
        }

        if (FailedMedia.size()>0)
        {
            MEDIA_LIST MediaList;
            MediaList.m_MediaIDs = FailedMedia;
            HRESULT hr = GetMediaLibraryInterface->StartTask(MediaList);
            if (!SUCCEEDED(hr))
            {
                TRACE0("Error happen in StartTask, PreviewDownLoadDlg");
            }
        }
    }

    for (auto it = m_hFileTask.begin(); it != m_hFileTask.end(); it++)
    {
        FILE_STATUS FileInfo;
        HRESULT hr = GetMediaLibraryInterface->GetFileStatus(*it, FileInfo);
        if (!SUCCEEDED(hr))
        {
            TRACE0("Error happen in GetFileStatus, PreviewDownLoadDlg");
            KillTimer(1);
            MessageBox(Translate(_T("Media Library Issue, Failed to download!")), Translate(_T("Tips:Download file")), MB_ICONERROR);

            if (!m_MediaList.m_MediaIDs.empty())
            {
                HRESULT hr = GetMediaLibraryInterface->PauseTask(m_MediaList);
                if (!SUCCEEDED(hr))
                {
                    TRACE0("Error happen in OnBnClickedUploadCancel (PauseTask), PreviewDownLoadDlg");
                }
            }

            CDialogEx::OnCancel();
        }

        FinishedCnt += FileInfo.m_FinishedCnt;
        FinishedSize += FileInfo.m_FinishedSize;
        TotalCnt += FileInfo.m_TotalCnt;
        TotalSize += FileInfo.m_TotalSize;
    }

    Percent = (TotalSize - m_ExistLocalFilesSize) > 0 ?
        FLOAT( FLOAT((10000 * (FinishedSize - m_ExistLocalFilesSize)) /
        (TotalSize - m_ExistLocalFilesSize)) / 100.0) : 100;

#ifdef PROFESSIONAL_EDITION
    Desc.Format(_T("Files: %d/%d    Size: %s/%s"), FinishedCnt, TotalCnt,
        FormatSizeString(FinishedSize), FormatSizeString(TotalSize));
#else
    Desc.Format(Translate(_T("Total Files Count: %d    Total Files Size: %s\n\n")
                            _T("Local Exists Media Size:%s\n\n")
                            _T("This Time Downloaded\n")
                            _T("Size: %s/%s\n\n")
                            _T("All Finished Downloaded Files\n")
                            _T("Total Files Count: %d   Total Finished Size: %s")),
        TotalCnt, FormatSizeString(TotalSize), FormatSizeString(m_ExistLocalFilesSize),
        FormatSizeString(FinishedSize - m_ExistLocalFilesSize), FormatSizeString(TotalSize), TotalCnt, FormatSizeString(TotalSize));
#endif

    if (nSuspisionMedia > 0)
    {
        CString sSuspisionString;
        sSuspisionString.Format(Translate(_T("%d downloaded file(s) is not safe:%s")), nSuspisionMedia, szSuspisionMedia.c_str());

        Desc += sSuspisionString;
    }
}

void PreviewDownLoadDlg::RefreshStatus()
{
    // TODO: Add your control notification handler code here

    FLOAT nProgress = 0;
    BOOL bFinished = FALSE;
    GetStatus(nProgress, m_JobDesc);

    m_Progress = INT(nProgress);

    if (m_bStarted)
    {
        GetDlgItem(ID_UPLOAD_START)->EnableWindow(FALSE);
        GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(TRUE);
        GetDlgItem(IDOK)->EnableWindow(FALSE);
    }
    else
    {
        GetDlgItem(ID_UPLOAD_START)->EnableWindow(TRUE);
        GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(FALSE);
        GetDlgItem(IDOK)->EnableWindow(FALSE);
    }

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

        if (bFinished)
        {
#define Translate(x) x
            m_JobDesc += Translate(_T(" - Error happened, Unable to preview!"));

            GetDlgItem(ID_UPLOAD_START)->EnableWindow(FALSE);
            GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(FALSE);
            GetDlgItem(IDC_UPLOAD_CANCEL)->EnableWindow(FALSE);
            GetDlgItem(IDOK)->EnableWindow(TRUE);
#undef Translate
        }
    }

    UpdateData(FALSE);
}

BOOL PreviewDownLoadDlg::AddDownloadLayoutJob(const CString& szServerFolder, const CString& szLocalFolder)
{

    m_AddDownloadLayoutJob.push_back(make_pair(szServerFolder,  szLocalFolder));
    m_JobFileCnt++;

    return TRUE;
}

BOOL PreviewDownLoadDlg::AddMediaListJob(const MEDIA_LIST& Medialist)
{
    m_MediaList += Medialist;

    m_AddMediaListJob += Medialist;

    m_JobFileCnt += Medialist.m_MediaIDs.size();

    return TRUE;

}

void PreviewDownLoadDlg::Exit()
{
    m_Status = MC_STATUS_OFFLINE;
    KillTimer(1);
    PostMessage(WM_COMMAND, IDCANCEL);
}
