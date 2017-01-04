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

class ExportLayoutsDlg : public CDialogEx
{
    DECLARE_DYNAMIC(ExportLayoutsDlg)
    DECLARE_EASYSIZE
public:
    ExportLayoutsDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~ExportLayoutsDlg();

// Dialog Data
    enum { IDD = IDD_UPLOAD_LAYOUT_SHORT };

    enum MC_STATUS{
        MC_STATUS_OFFLINE = 0,
        MC_STATUS_CONNECT = 1,
        MC_STATUS_LOGIN = 2,
    };


    BOOL AddDownloadLayoutJob(CString szServerFolder, CString szLocalFolder);

    BOOL AddMediaListJob(const MEDIA_LIST& Medialist, CString szLocalFolder);

    void GetStatus(FLOAT& Percent, CString& Desc, BOOL &bCompleted);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTimer(UINT_PTR);

    DECLARE_MESSAGE_MAP()

private:

    BOOL m_bStarted;
    BOOL m_bInited;
	BOOL m_bJobInited;
    MC_STATUS m_Status;

    // File tasks and media tasks
    vector<HANDLE>  m_hFileTask;
    MEDIA_LIST      m_MediaList;

	CString m_szLocalFolder;
	MEDIA_LIST m_AddMediaListJob;
	INT     m_JobFileCnt;
private:

    void RefreshStatus();
	HRESULT InitJob(BOOL &bJobInited);
public:
    CString m_JobDesc;
    CProgressCtrl m_ctrlProgress;
    afx_msg void OnBnClickedUploadStart();
    afx_msg void OnBnClickedUploadPause();
    afx_msg void OnBnClickedUploadCancel();

    INT  m_Progress;
};
