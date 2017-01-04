#pragma once
#include "afxcmn.h"

#ifdef _MEDIALIBRARY2
#include "MediaLibrary 2/MediaClient.h"
//#include "MediaLibrary 2/MediaLibrary.h"
#else
#include "MediaClient/MediaClient.h"
//#include "MediaLibrary/MediaLibrary.h"
#endif

#include "LayoutSettings.h"


class PreviewDownLoadDlg : public CDialogEx
{
    DECLARE_DYNAMIC(PreviewDownLoadDlg)
    DECLARE_EASYSIZE
public:
    PreviewDownLoadDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~PreviewDownLoadDlg();

// Dialog Data
#ifdef PROFESSIONAL_EDITION
    enum { IDD = IDD_UPLOAD_LAYOUT_SHORT };
#else
    enum { IDD = IDD_UPLOAD_LAYOUT };
#endif

    enum MC_STATUS{
        MC_STATUS_OFFLINE = 0,
        MC_STATUS_CONNECT = 1,
        MC_STATUS_LOGIN = 2,
    };

    BOOL AddDownloadLayoutJob(const CString& szServerFolder, const CString& szLocalFolder);
    BOOL AddMediaListJob(const MEDIA_LIST& Medialist);

    void GetStatus(FLOAT& Percent, CString& Desc);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTimer(UINT_PTR);

    DECLARE_MESSAGE_MAP()

private:

    BOOL m_bStarted;
    BOOL m_bInited;  

    MC_STATUS m_Status;

    // File tasks and media tasks
    vector<HANDLE>  m_hFileTask;
    MEDIA_LIST      m_MediaList;

    INT     m_nExistLocalFiles;
    DWORD64 m_ExistLocalFilesSize;

    BOOL    m_bJobInited;

    MEDIA_LIST m_AddMediaListJob;
    std::vector<std::pair<CString, CString> >  m_AddDownloadLayoutJob;
    INT     m_JobFileCnt;

private:

    void Exit();

    void RefreshStatus();  

    // if all jobs are inited, reture ture, otherwise false.
    HRESULT InitJob(BOOL &bAllJobsInited);

public:
    CString m_JobDesc;
    CProgressCtrl m_ctrlProgress;
    afx_msg void OnBnClickedUploadStart();
    afx_msg void OnBnClickedUploadPause();
    afx_msg void OnBnClickedUploadCancel();

    INT m_Progress;


};
