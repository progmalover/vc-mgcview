#pragma once
#include "afxcmn.h"
#include "LayoutSettings.h"

// CFileTransferDlg dialog

static MagicView::CommonLib::ThreadSection m_ThreadCS;

class CFileTransferDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CFileTransferDlg)

public:
    CFileTransferDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CFileTransferDlg();

// Dialog Data
    enum { IDD = IDD_UPLOAD_LAYOUT_SHORT };

    HRESULT InitTransferFiles(const CStringArray* pSrcFiles, const CString szDstFolder);


protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTimer(UINT_PTR);
    afx_msg void OnBnClickedUploadCancel();

    DECLARE_MESSAGE_MAP()


private:
    void RefreshStatus();

    HRESULT StopCopyFiles();
    HRESULT TransferFiles();

    static DWORD WINAPI CopyMediaFiles(LPVOID lpParam);

    static DWORD CALLBACK CopyFileProgress(
        LARGE_INTEGER TotalFileSize,
        LARGE_INTEGER TotalBytesTransferred,
        LARGE_INTEGER StreamSize,
        LARGE_INTEGER StreamBytesTransferred,
        DWORD dwStreamNumber,
        DWORD dwCallbackReason,
        HANDLE hSourceFile,
        HANDLE hDestinationFile,
        LPVOID lpData
        );

protected:
    CString             m_Desc;
    CProgressCtrl       m_ctrlProgress;
    CString             m_szDstFolder;
    CStringArray        m_arrMediaFiles;

    INT                 m_Progress;
    DWORD64             m_nTotalFilesSize;
    DWORD64             m_nCopyFileSize;
    DWORD64             m_nLastCopySize;
    DWORD               m_nCopyFileCount;
    DWORD               m_nFileCount;

    DWORD               m_dwThreadID;
    HANDLE              m_hThread;
    BOOL                m_bRunning;

};
