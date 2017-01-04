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
#include "afxeditbrowsectrl.h"

class ImportLayoutDlg : public CDialogEx
{
    DECLARE_DYNAMIC(ImportLayoutDlg)
    DECLARE_EASYSIZE
public:
    ImportLayoutDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~ImportLayoutDlg();

// Dialog Data
    enum { IDD = IDD_IMPORT_LAYOUT };

    enum MC_STATUS{
        MC_STATUS_OFFLINE = 0,
        MC_STATUS_CONNECT = 1,
        MC_STATUS_LOGIN = 2,
    };

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
    //vector<HANDLE>  m_hFileTask;
	std::map<int,HANDLE> m_hMediaFileTask;
private:

    void RefreshStatus();

public:
    CString m_JobDesc;
    CProgressCtrl m_ctrlProgress;
    afx_msg void OnBnClickedUploadStart();
    afx_msg void OnBnClickedUploadPause();
    afx_msg void OnBnClickedUploadCancel();

	INT  m_Progress;

	CMFCEditBrowseCtrl m_RootWorkdirectory;
	afx_msg void OnEnChangeRootDir();

private:
	std::map<std::tstring,HANDLE> mapUploadMedia;
	LayoutElement                 *m_LayoutElement;
public:
	afx_msg void OnBnClickedOk();
};
