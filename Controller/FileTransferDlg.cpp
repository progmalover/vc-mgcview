#include "stdafx.h"
#include "Controller.h"
#include "FileTransferDlg.h"
#include "DesignerPage.h"
#include "Localization/Localization.h"
#include "Utilities/CrashRptHelper.h"
#include <set>
#define  CLIP_CNT 100

IMPLEMENT_DYNAMIC(CFileTransferDlg, CDialogEx)

CFileTransferDlg::CFileTransferDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CFileTransferDlg::IDD, pParent)
{
    m_Progress          = 0;
    m_nTotalFilesSize   = 0;
    m_nCopyFileSize     = 0;
    m_nLastCopySize     = 0;
    m_nCopyFileCount    = 0;
    m_nFileCount        = 0;
    m_hThread = INVALID_HANDLE_VALUE;
    m_bRunning          = FALSE;
}

CFileTransferDlg::~CFileTransferDlg()
{
    StopCopyFiles();
}

void CFileTransferDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_UPLOAD_INFO, m_Desc);
    DDX_Control(pDX, IDC_UPLOAD_PROGRESS, m_ctrlProgress);
}


BEGIN_MESSAGE_MAP(CFileTransferDlg, CDialogEx)
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_UPLOAD_CANCEL, &CFileTransferDlg::OnBnClickedUploadCancel)
END_MESSAGE_MAP()



// CFileTransferDlg message handlers
BOOL CFileTransferDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    BOOL bEnable = FALSE;
    UINT menuf = bEnable ? (MF_BYCOMMAND) : (MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
    CMenu* pSM = GetSystemMenu(FALSE);
    if (pSM)
    {
        pSM->EnableMenuItem(SC_CLOSE, menuf);
    }

    SetTimer(1, 1000, NULL);

    m_ctrlProgress.SetRange(0, CLIP_CNT);

    SetWindowText(Translate(_T("Copying Layout media files...")));
    GetDlgItem(IDOK)->EnableWindow(FALSE);
    GetDlgItem(ID_UPLOAD_START)->EnableWindow(FALSE);
    GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(FALSE);

	Translate(this);
    return TRUE;  // return TRUE  unless you set the focus to a control
}


void CFileTransferDlg::OnSize(UINT nType, int cx, int cy) 
{
    CDialogEx::OnSize(nType, cx, cy);

}


void CFileTransferDlg::OnBnClickedUploadCancel()
{
    // TODO: Add your control notification handler code here
    if (MessageBox(Translate(_T("Are you sure to stop uploading?")), Translate(_T("Tips:Upload file")),MB_YESNO|MB_ICONQUESTION) != IDYES)
    {
        return;
    }

    StopCopyFiles();

    CDialogEx::OnCancel();
}

void CFileTransferDlg::OnTimer(UINT_PTR)
{
    RefreshStatus();
}


void CFileTransferDlg::RefreshStatus()
{
    // TODO: Add your control notification handler code here
    if (m_nTotalFilesSize == m_nCopyFileSize && m_nCopyFileSize != 0)
    {
        m_Progress = CLIP_CNT;
    }
    else
    {
        m_Progress = (INT)(CLIP_CNT * m_nCopyFileSize / (m_nTotalFilesSize + 1));
    }
    CString sCopySize = FormatSizeString(m_nCopyFileSize);
    CString sTotalSize = FormatSizeString(m_nTotalFilesSize);

	CString fileStr, sizeStr;
	fileStr.Format(_T("%d/%d"), m_nCopyFileCount, m_nFileCount);
	sizeStr.Format(_T("%s/%s"), sCopySize, sTotalSize); 
    m_Desc = Translate(_T("Files:")) + fileStr +  Translate(_T("Size:")) + sizeStr;

    if (m_Progress >= CLIP_CNT)
    {
        m_ctrlProgress.SetPos(CLIP_CNT);

        GetDlgItem(IDC_UPLOAD_CANCEL)->EnableWindow(FALSE);
        GetDlgItem(IDOK)->EnableWindow(TRUE);
    }
    else
    {
        GetDlgItem(IDC_UPLOAD_CANCEL)->EnableWindow(TRUE);
        GetDlgItem(IDOK)->EnableWindow(FALSE);
        m_ctrlProgress.SetPos(m_Progress);
    }

    UpdateData(FALSE);
}

HRESULT CFileTransferDlg::InitTransferFiles(const CStringArray* pSrcFilesOld, const CString szDstFolder)
{
    CString szSrcFile;
    CStringArray srcFiles;
    std::set<CString> setSrcFiles;
    for (int i = 0; i < pSrcFilesOld->GetCount(); i++)
    {
        szSrcFile = pSrcFilesOld->GetAt(i);
        setSrcFiles.insert(szSrcFile);
    }

    auto it = setSrcFiles.begin(), ite = setSrcFiles.end();
    for ( ; it != ite; ++it)
    {
        srcFiles.Add(*it);
    }

    HRESULT hr = S_OK;

    m_szDstFolder = szDstFolder;

    if ((m_szDstFolder.ReverseFind(_T('\\')) + 1) != m_szDstFolder.GetLength())
    {
        m_szDstFolder.Append(_T("\\"));
    }

    HANDLE fFile;

    for (int i = 0; i < srcFiles.GetCount(); i++)
    {
        szSrcFile = srcFiles.GetAt(i);

        fFile = ::CreateFile(szSrcFile, FILE_GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (fFile != INVALID_HANDLE_VALUE)
        {
            LARGE_INTEGER fileSize;
            if (GetFileSizeEx(fFile, &fileSize))
            {
                m_nFileCount += 1;
                m_nTotalFilesSize += fileSize.QuadPart;

                m_arrMediaFiles.Add(szSrcFile);
            }

            CloseHandle(fFile);
        }
    }

    if (m_hThread != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hThread);
    }

    m_hThread = INVALID_HANDLE_VALUE;

    m_hThread = CreateThread(NULL, 0, CopyMediaFiles, this, 0, &m_dwThreadID);
    if (m_hThread == INVALID_HANDLE_VALUE)
        return E_FAIL;

    return hr;
}

DWORD WINAPI CFileTransferDlg::CopyMediaFiles(LPVOID lpParam)
{
    CCrashRptThreadHelper helper;
    HRESULT hr = S_OK;
    CFileTransferDlg* pFileTransfer = (CFileTransferDlg*)lpParam;

    if (pFileTransfer)
    {
        hr = pFileTransfer->TransferFiles();
    }
    else
    {
        return E_UNEXPECTED;
    }

    return hr;

}

DWORD CALLBACK CFileTransferDlg::CopyFileProgress(
    LARGE_INTEGER TotalFileSize,
    LARGE_INTEGER TotalBytesTransferred,
    LARGE_INTEGER StreamSize,
    LARGE_INTEGER StreamBytesTransferred,
    DWORD dwStreamNumber,
    DWORD dwCallbackReason,
    HANDLE hSourceFile,
    HANDLE hDestinationFile,
    LPVOID lpData
    )
{
    m_ThreadCS.Lock();

    CFileTransferDlg* pFileTransfer = (CFileTransferDlg* )lpData;

    if (pFileTransfer)
    {
        if (!pFileTransfer->m_bRunning)
        {
            m_ThreadCS.Unlock();
            return PROGRESS_CANCEL;
        }

        pFileTransfer->m_nCopyFileSize = TotalBytesTransferred.QuadPart + pFileTransfer->m_nLastCopySize;

        if (pFileTransfer->m_nCopyFileSize == pFileTransfer->m_nTotalFilesSize || TotalFileSize.QuadPart == TotalBytesTransferred.QuadPart)
        {
            m_ThreadCS.Unlock();
            return PROGRESS_CONTINUE;
        }

        m_ThreadCS.Unlock();
        return PROGRESS_CONTINUE;
    }

    m_ThreadCS.Unlock();
    return PROGRESS_CANCEL;
}

HRESULT CFileTransferDlg::TransferFiles()
{
    m_bRunning = TRUE;

    Sleep(1000);

    HRESULT hr = S_OK;
    CString szSrcFile;
    CString szDstFile;
    CString szFileName;
    BOOL bCancel = FALSE;
    DWORD64 nLastFileSize = 0;

    for (int i = 0; i < m_arrMediaFiles.GetCount(); i++)
    {
        bCancel = FALSE;

        szSrcFile = m_arrMediaFiles.GetAt(i);
        szFileName = ::PathFindFileName(szSrcFile);
        szDstFile = m_szDstFolder;
        szDstFile.Append(szFileName);

        if (::PathFileExists(szDstFile))
        {
            ::DeleteFile(szDstFile);
        }

        if (::CopyFileEx(m_arrMediaFiles.GetAt(i), szDstFile, CopyFileProgress, this, &bCancel, COPY_FILE_FAIL_IF_EXISTS) != 0)
        {
            m_nCopyFileCount += 1;

            HANDLE fFile;
            fFile = ::CreateFile(szSrcFile, FILE_GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (fFile != INVALID_HANDLE_VALUE)
            {
                LARGE_INTEGER fileSize;
                if (GetFileSizeEx(fFile, &fileSize))
                {
                    m_nLastCopySize += fileSize.QuadPart;
                }

                CloseHandle(fFile);
            }
        }
        else
        {
            DWORD lastError = GetLastError();
            //return E_FAIL;
            break;
        }
    }

    m_bRunning = FALSE;

    return hr;
}

HRESULT CFileTransferDlg::StopCopyFiles()
{
    m_bRunning = FALSE;

    DWORD timeOut = 1 * 1000;
    if (NULL != m_hThread)
    {
        if (WAIT_TIMEOUT == WaitForSingleObject(m_hThread, timeOut))
        {
            TerminateThread(m_hThread, 1);
        }

        CloseHandle(m_hThread);
        m_hThread = NULL;
    }

    return S_OK;
}