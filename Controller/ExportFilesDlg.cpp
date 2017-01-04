#include "stdafx.h"
#include "Controller.h"
#include "ExportFilesDlg.h"
#include "SchedulerPage.h"
#include "afxdialogex.h"
#include "ScheduleCtrl.h"
#include "Localization/Localization.h"
#define  CLIP_CNT 100

#if 1

//#pragma comment( lib, "msi.lib" )
//#include <conio.h>
//#include <iostream>
#include <MsiQuery.h>

#include "WinTrust.h"
#include "SoftPub.h"
#include "Mscat.h"
#include <memory>
#include "strsafe.h"
//#include<Windows.h>

#define TYPE_STRING 's'
#define TYPE_INTEGER 'i'
#define TYPE_BINARY  'v'
/////////////////////////////////////////////////////////////////////////////
using namespace std;


int GetMsiVersionInfo(const TCHAR *msifilename, std::tstring &strversion)
{
	int nRetCode = 0;
	/*
	if(argc!=3)
	{
		cout<<"run the application with arguments <application name> <Msidatabase(with full path)> <table name in MSI database>"<<endl;
		getch();
		nRetCode=1;
		return nRetCode;
	}
	*/
	strversion = _T("");
	const TCHAR *szPathName = msifilename;
	const TCHAR *szTableName=_T("File");
	int uColumns;

	CStringArray *saColumnNames=new CStringArray();
	CWordArray *waTypes=new CWordArray();
	CObList *olRecords=new CObList();
	MSIHANDLE hDatabase, hView, hRecord;
	TCHAR szSelect[256], szTemp[256], szValue[32768];
	long lValue;
	BOOL bSuccess = TRUE;
	DWORD dwLength;

	//add by LiuSong
	int VersionColId = 0x00;
	int FileNameColId = 0x00;
	if (MsiOpenDatabase(szPathName,MSIDBOPEN_READONLY,&hDatabase)!= ERROR_SUCCESS)
	{
		//cout<<"error in opening the MSI database"<<"\t"<<GetLastError()<<endl;
		return FALSE;
	}
	// build query for all records in this table
	wsprintf(szSelect,_T("SELECT * from %s"),szTableName);
	if (MsiDatabaseOpenView(hDatabase,szSelect,&hView) != ERROR_SUCCESS)
	{
		//cout<<"error in Opening View"<<"\t"<<GetLastError()<<endl;
         MsiCloseHandle(hDatabase);		//before return ,must close handle created before. -zxy
         return FALSE;
	}
	// execute query - not a parameter query so second parameter is NULL.
	if (MsiViewExecute(hView,NULL) != ERROR_SUCCESS)
	{
		//cout<<"error in executing the View"<<"\t"<<GetLastError()<<endl;
        MsiCloseHandle(hView);
        MsiCloseHandle(hDatabase);	//before return ,must close handle created before. -zxy	
		return FALSE;
	}
	// read column names
	MsiViewGetColumnInfo(hView,MSICOLINFO_NAMES,&hRecord);
	// get total number of columns in table.
	uColumns = MsiRecordGetFieldCount(hRecord);
	saColumnNames->SetSize(uColumns);
	waTypes->SetSize(uColumns);
	// read in the column names from the record to our StringArray
	cout<<"The column Headers follow"<<endl;
	for (unsigned int i = 0; i < uColumns; ++i)
	{
		dwLength = 256;
		if (MsiRecordGetString(hRecord,i + 1,szTemp,&dwLength) != ERROR_SUCCESS)
		{
			return FALSE;
		}
		saColumnNames->SetAt(i,szTemp);
		
		if(_tcsicmp(szTemp,_T("version")) == 0 )
		{//add by LiuSong
			VersionColId = i;
		}
		if(_tcsicmp(szTemp,_T("FileName")) == 0 )
		{//add by LiuSong
			FileNameColId = i;
		}
		//cout<<szTemp<<" ";
	}
	cout<<endl;
	MsiCloseHandle(hRecord) ;

	// get the data types for each column

	if (MsiViewGetColumnInfo(hView,MSICOLINFO_TYPES,&hRecord)!=ERROR_SUCCESS)
	{
		cout<<"error in reading the record"<<"\t"<<GetLastError()<<endl;
        MsiCloseHandle(hView);
        MsiCloseHandle(hDatabase);//before return ,must close handle created before. -zxy
		return FALSE;
	}

	for (int i = 0; i < uColumns; i++)
	{
		long length;
		dwLength = 256;
		MsiRecordGetString(hRecord,i + 1,szTemp,&dwLength);
		//length = atol(&szTemp[1]);
		switch(tolower(szTemp[0]))
		{
		case('s'): // normal string type
		case('l'): // localizable string
				waTypes->SetAt(i,TYPE_STRING);
				break ;
		case('i') :
				waTypes->SetAt(i,TYPE_INTEGER);
				break;
		case('v') :
				waTypes->SetAt(i,TYPE_BINARY);
				break ;
		}
	}
	MsiCloseHandle(hRecord) ;
	cout<<endl;

	std::tstring strversionbak;
	// read records until there are no more records
	while (MsiViewFetch(hView,&hRecord) == ERROR_SUCCESS)
	{
		CStringArray *psaRecord = new CStringArray;
		psaRecord->SetSize(uColumns);
		olRecords->AddTail(psaRecord);
		
		//for (int i = 0; i < uColumns; i++)
		int i = VersionColId;
		int j = FileNameColId;
		{
			switch(waTypes->GetAt(i))
			{
			case(TYPE_INTEGER):
					lValue = MsiRecordGetInteger(hRecord,i + 1);
					(*psaRecord)[i].Format(_T("%d"),lValue);
					//cout<<lValue<<" ";
					break;
			case(TYPE_STRING):
					dwLength = 32768;
					MsiRecordGetString(hRecord,i + 1,szValue,&dwLength);
					(*psaRecord)[i] = szValue;
					//cout<<szValue<<" ";
					if(_tcsicmp(szValue,_T("")))
					{
						strversionbak = szValue;
						dwLength = 32768;
						MsiRecordGetString(hRecord,j + 1,szValue,&dwLength);
						if(0x00 == _tcsncicmp(szValue,_T("Client.exe"),10))
						{
							strversion = strversionbak;
						}
					}
					break;
			case(TYPE_BINARY):
					(*psaRecord)[i] = "{Binary Data}";
					cout<<"binary  ";
				/*
			don't read binary data into string, if you want to read it into
			a BYTE buffer, the code looks like:
			DWORD dwLen = MsiRecordDataSize(hRecord,i + 1);
			char *pBinary = new char[dwLen];
			MsiRecordReadStream(hRecord,i + 1,pBinary,&dwLen);
			*/
					break;
			default: break;
			}
		}
		cout<<endl;
        MsiCloseHandle(hRecord); //--after "MsiViewFetch" ,must close the return handle. -zxy
	}
    MsiViewClose(hView);
	MsiCloseHandle(hRecord);
	MsiCloseHandle(hView);
	MsiCloseHandle(hDatabase);
	return TRUE;
}
#endif 

#if 0 
BOOL CheckFileTrust( LPCWSTR lpFileName )
{
	BOOL bRet = FALSE;
	WINTRUST_DATA wd = { 0 };
	WINTRUST_FILE_INFO wfi = { 0 };
	WINTRUST_CATALOG_INFO wci = { 0 };
	CATALOG_INFO ci = { 0 };

	HCATADMIN hCatAdmin = NULL;
	if ( !CryptCATAdminAcquireContext( &hCatAdmin, NULL, 0 ) )
	{
		return FALSE;
	}

	HANDLE hFile = CreateFileW( lpFileName, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, 0, NULL );
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		CryptCATAdminReleaseContext( hCatAdmin, 0 );
		return FALSE;
	}

	DWORD dwCnt = 100;
	BYTE byHash[100];
	CryptCATAdminCalcHashFromFileHandle( hFile, &dwCnt, byHash, 0 );
	CloseHandle( hFile );

	LPWSTR pszMemberTag = new WCHAR[dwCnt * 2 + 1];
	for ( DWORD dw = 0; dw < dwCnt; ++dw )
	{
		wsprintfW( &pszMemberTag[dw * 2], L"%02X", byHash[dw] );
	}

	HCATINFO hCatInfo = CryptCATAdminEnumCatalogFromHash( hCatAdmin,
		byHash, dwCnt, 0, NULL );
	if ( NULL == hCatInfo )
	{
		wfi.cbStruct       = sizeof( WINTRUST_FILE_INFO );
		wfi.pcwszFilePath  = lpFileName;
		wfi.hFile          = NULL;
		wfi.pgKnownSubject = NULL;

		wd.cbStruct            = sizeof( WINTRUST_DATA );
		wd.dwUnionChoice       = WTD_CHOICE_FILE;
		wd.pFile               = &wfi;
		wd.dwUIChoice          = WTD_UI_NONE;
		wd.fdwRevocationChecks = WTD_REVOKE_NONE;
		wd.dwStateAction       = WTD_STATEACTION_IGNORE;
		wd.dwProvFlags         = WTD_SAFER_FLAG;
		wd.hWVTStateData       = NULL;
		wd.pwszURLReference    = NULL;
	}
	else
	{
		CryptCATCatalogInfoFromContext( hCatInfo, &ci, 0 );
		wci.cbStruct             = sizeof( WINTRUST_CATALOG_INFO );
		wci.pcwszCatalogFilePath = ci.wszCatalogFile;
		wci.pcwszMemberFilePath  = lpFileName;
		wci.pcwszMemberTag       = pszMemberTag;

		wd.cbStruct            = sizeof( WINTRUST_DATA );
		wd.dwUnionChoice       = WTD_CHOICE_CATALOG;
		wd.pCatalog            = &wci;
		wd.dwUIChoice          = WTD_UI_NONE;
		wd.fdwRevocationChecks = WTD_STATEACTION_VERIFY;
		wd.dwProvFlags         = 0;
		wd.hWVTStateData       = NULL;
		wd.pwszURLReference    = NULL;
	}
	GUID action = WINTRUST_ACTION_GENERIC_VERIFY_V2;
	HRESULT hr  = WinVerifyTrust( NULL, &action, &wd );
	bRet        = SUCCEEDED( hr );

	if ( NULL != hCatInfo )
	{
		CryptCATAdminReleaseCatalogContext( hCatAdmin, hCatInfo, 0 );
	}
	CryptCATAdminReleaseContext( hCatAdmin, 0 ); // 2007.4.10
	delete[] pszMemberTag;
	return bRet;
}

#endif 


IMPLEMENT_DYNAMIC(ExportFilesDlg, CDialogEx)

ExportFilesDlg::ExportFilesDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ExportFilesDlg::IDD, pParent)
    , m_bInited (FALSE)
    , m_Status (MC_STATUS_OFFLINE)
    , m_bStarted (FALSE)
	, m_msiVersion(_T(""))
{

}

MediaLibrary *pSubLibrary = NULL;
ExportFilesDlg::~ExportFilesDlg()
{
    delete pSubLibrary;
}

void ExportFilesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_UPLOAD_INFO, m_JobDesc);
	DDX_Control(pDX, IDC_UPLOAD_PROGRESS, m_ctrlProgress);
	DDX_Control(pDX, IDC_ROOT_DIR, m_RootWorkdirectory);
}


BEGIN_MESSAGE_MAP(ExportFilesDlg, CDialogEx)
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_BN_CLICKED(ID_UPLOAD_START, &ExportFilesDlg::OnBnClickedUploadStart)
    ON_BN_CLICKED(IDC_UPLOAD_PAUSE, &ExportFilesDlg::OnBnClickedUploadPause)
    ON_BN_CLICKED(IDC_UPLOAD_CANCEL, &ExportFilesDlg::OnBnClickedUploadCancel)
	ON_EN_CHANGE(IDC_ROOT_DIR, &ExportFilesDlg::OnEnChangeRootDir)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(ExportFilesDlg)
	//EASYSIZE(IDC_STATIC_INFO_GROUP,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

 
// CScheduleItemCtrl message handlers
BOOL ExportFilesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;

    SetTimer(1,1000,NULL);

    m_ctrlProgress.SetRange(0, CLIP_CNT);

    SetWindowText(Translate(_T("Upload File")));

	GetDlgItem(ID_UPLOAD_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_UPLOAD_CANCEL)->EnableWindow(TRUE);
    GetDlgItem(IDOK)->EnableWindow(FALSE);

    //OnBnClickedUploadStart();
    pSubLibrary = GetMediaLibraryInterface->CreateColneMDLIB();
	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void ExportFilesDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

void ExportFilesDlg::OnBnClickedUploadStart()
{
	CString filepath;
	CString strDir,strName;
	m_RootWorkdirectory.GetWindowText(filepath);
    
	int idx;
	int len;
	len = filepath.GetLength();

	if(len == 0 ) 
	{
		MessageBox(Translate(_T("Please Select Upload File !")), Translate(_T("Warning:Export file")), MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	if(CheckMsiVersion() <=0 )
	{
		return;
	}

	if(CheckSignature() <= 0)
	{
		return;
	}

	idx = filepath.ReverseFind(_T('\\'));
	strDir = filepath.Left(idx+1);
	strName = filepath.Right(len-idx-1);

	if(strName != _T("ClientSetup.msi"))
	{
		MessageBox(Translate(_T("filename must be ClientSetup.msi !")), NULL, MB_OK);
	}	

	AddUploadFileJob(_T(""), filepath);

	// TODO: Add your control notification handler code here
    m_bStarted = TRUE;
	
    GetDlgItem(ID_UPLOAD_START)->EnableWindow(FALSE);
    GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(TRUE);
}


void ExportFilesDlg::OnBnClickedUploadPause()
{
    // TODO: Add your control notification handler code here
    m_bStarted = FALSE;


    GetDlgItem(ID_UPLOAD_START)->EnableWindow(TRUE);
    GetDlgItem(IDC_UPLOAD_PAUSE)->EnableWindow(FALSE);
}


void ExportFilesDlg::OnBnClickedUploadCancel()
{
    // TODO: Add your control notification handler code here
	if(TRUE == m_bStarted || m_hFileTask.size())
	{
		if (MessageBox(Translate(_T("Are you sure to stop uploading?")), Translate(_T("Tips:Upload file")),MB_YESNO|MB_ICONQUESTION) != IDYES)
		{
			return;
		}
	}
    CDialogEx::OnCancel();
}

void ExportFilesDlg::OnTimer(UINT_PTR)
{
    if(m_bStarted)
    {
        RefreshStatus();
    }
}


void ExportFilesDlg::GetStatus(FLOAT& Percent, CString& Desc)
{
    DWORD64 FinishedSize = 0;
    DWORD64 TotalSize = 0;
    DWORD   FinishedCnt = 0;
    DWORD   TotalCnt = 0;
#if 0
    if (!m_MediaList.m_MediaIDs.empty())
    {
        MEDIA_BASIC_INFO MediaInfo;
        HRESULT hr = GetMediaLibraryInterface->CheckTaskListStatus(m_MediaList, MediaInfo);
        if (!SUCCEEDED(hr))
        {
            TRACE0("Error happen in CheckMediaListTask, PreviewDownLoadDlg");
        }

        FinishedCnt += MediaInfo.m_FinishedCnt;
        FinishedSize += MediaInfo.m_FinishedSize;
        TotalCnt += MediaInfo.m_TotalCnt;
        TotalSize += MediaInfo.m_TotalSize;
    }
#endif 
    for (auto it = m_hFileTask.begin(); it != m_hFileTask.end(); it++)
    {
        FILE_STATUS FileInfo;
        HRESULT hr = pSubLibrary->GetFileStatus(*it, FileInfo);
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
void ExportFilesDlg::RefreshStatus()
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

#if 0
BOOL ExportFilesDlg::AddDownloadLayoutJob(CString szServerFolder, CString szLocalFolder)
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
#endif

BOOL ExportFilesDlg::AddUploadFileJob(CString szServerFolder, CString szLocalFolder)
{
	FILE_TASK FileTask;
	FileTask.AddFile(szLocalFolder.GetString(), szServerFolder.GetString(), ML_FILE);

	HANDLE hTaskHandle;
	HRESULT hr = pSubLibrary->UploadMSIFile(FileTask,hTaskHandle);
	if (!SUCCEEDED(hr) || hTaskHandle == NULL)
	{
		return FALSE;
	}

	m_hFileTask.push_back(hTaskHandle);

	return TRUE;
}
#if 0
BOOL ExportFilesDlg::SetMediaListJob(const MEDIA_LIST& Medialist)
{
    m_MediaList = Medialist;

    HRESULT hr = GetMediaLibraryInterface->DownloadMedia(Medialist, MediaLibrary::TASK_IMMEDIATE);
    if (!SUCCEEDED(hr))
    {
        return FALSE;
    }

    return TRUE;

}
#endif

extern int S3_VerifyDigitalSignature(TCHAR * szFullFilePath);

void ExportFilesDlg::OnEnChangeRootDir()
{
	// TODO:
	//MessageBox(_T("00"),_T("11"),0);
	CheckMsiVersion();
}


int ExportFilesDlg::CheckMsiVersion()
{
	int rtn = -1;

	CString filepath;
	m_RootWorkdirectory.GetWindowText(filepath);
	int len = filepath.GetLength();
	if(len == 0 ) 
	{
		return rtn;
	}


	std::tstring strversion;

	if(-1 != filepath.Find(_T(".msi")))
	{
		GetMsiVersionInfo(filepath.GetBuffer(), strversion);
		if(strversion != _T(""))
		{
			CString tmp;
			tmp.Format(_T("Product Version: %s"),strversion.c_str());
			GetDlgItem(IDC_INFORMATION)->SetWindowText(tmp.GetBuffer());

			m_msiVersion = strversion;
		}
	}
	else
	{
		MessageBox(Translate(_T("it is not a msi file!")),Translate(_T("Warning")),0);
		GetDlgItem(IDC_INFORMATION)->SetWindowText(_T(""));
		return rtn;
	}

	filepath.ReleaseBuffer();

	/* by RaymondLiu
	if(0x00 == S3_VerifyDigitalSignature(filepath.GetBuffer()) )
	{
		//MessageBox(_T("valid file signature!"),_T("Information"),0);
	}
	else
	{
		MessageBox(Translate(_T("Invalid file signature!")),Translate(_T("Warning")),0);
		GetDlgItem(IDC_INFORMATION)->SetWindowText(_T(""));
		return rtn;
	}
	filepath.ReleaseBuffer();
	*/
	return 0x01;
}

int ExportFilesDlg::CheckSignature()
{
	int rtn = -1;

	CString filepath;
	m_RootWorkdirectory.GetWindowText(filepath);
	int len = filepath.GetLength();
	if(len == 0 ) 
	{
		return rtn;
	}

	/* By RaymondLiu
	std::tstring strversion;

	if(-1 != filepath.Find(_T(".msi")))
	{
		GetMsiVersionInfo(filepath.GetBuffer(), strversion);
		if(strversion != _T(""))
		{
			CString tmp;
			tmp.Format(_T("Product Version: %s"),strversion.c_str());
			GetDlgItem(IDC_INFORMATION)->SetWindowText(tmp.GetBuffer());

			m_msiVersion = strversion;
		}
	}
	else
	{
		MessageBox(Translate(_T("it is not a msi file!")),Translate(_T("Warning")),0);
		GetDlgItem(IDC_INFORMATION)->SetWindowText(_T(""));
		return rtn;
	}

	filepath.ReleaseBuffer();
	*/
	CWaitDlg waitdlg;
	if(0x00 == S3_VerifyDigitalSignature(filepath.GetBuffer()) )
	{
		//MessageBox(_T("valid file signature!"),_T("Information"),0);
	}
	else
	{
		MessageBox(Translate(_T("Invalid file signature!")),Translate(_T("Warning")),0);
		GetDlgItem(IDC_INFORMATION)->SetWindowText(_T(""));
		return rtn;
	}
	filepath.ReleaseBuffer();

	return 0x01;
}

const std::tstring& ExportFilesDlg::GetLastMSIVersion()const
{
	return m_msiVersion;
}