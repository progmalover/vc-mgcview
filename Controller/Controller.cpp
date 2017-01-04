// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface
// (the "Fluent UI") and is provided only as referential material to supplement the
// Microsoft Foundation Classes Reference and related electronic documentation
// included with the MFC C++ library software.
// License terms to copy, use or distribute the Fluent UI are available separately.
// To learn more about our Fluent UI licensing program, please visit
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// Controller.cpp : Defines the clas behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Controller.h"
#include "resource.h"
#include "ControllerFrame.h"
#include "LoginDialog.h"
#include "DemoTranslucentDlg.h"
#include "LoadingTranslucentDlg.h"
#include "Psapi.h"
#include "Utilities/CrashRptHelper.h"
#include "Utilities/SysCall.h"
#include "LayoutSettings.h"

#include "Client/ClientConfig.h"
#include "Client/ClientConfigFile.h"
#include "MediaServer/MediaServerConfig.h"
#include "ControllerServerConfiguer/ControllerServerConfig.h"

#include "SplashWnd.h"

#include "ControllerSettings.h"

#include "Localization/Localization.h"

#include "CommonLib/CommonLib.h"
#include "afxlinkctrl.h"

#include "AsyncGrid.h"

#include <MinHook.h>

ControllerServerLib the_ControllerServer;
MediaServerLib the_MediaServer;

std::shared_ptr<MagicView::CommonUI::S3MessageBoxManager>g_msgboxMgr;
std::tstring GetControllerDefaultDir()
{
    std::tstring ret;
#ifdef STARTER_EDITION
    ret = SysCall::GetCommonAppDataFolder() + _T("\\MagicView\\S3MagicViewStarterEdition\\");
#elif defined(STANDALONE_EDITION)
    ret = SysCall::GetCommonAppDataFolder() + _T("\\MagicView\\S3MagicViewStandaloneEdition\\");
#elif defined(PROFESSIONAL_EDITION)
    ret = SysCall::GetCommonAppDataFolder() + _T("\\MagicView\\S3MagicViewProfessionalEdition\\");
#else
    ret = SysCall::GetCommonAppDataFolder() + _T("\\MagicView\\S3MagicViewController\\");
#endif
    SysCall::NewFolder(ret);
    return ret;
}

int ShowXMessageBox( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	MagicView::CommonUI::XMSGBOXPARAMS xmb;
	xmb.bUseUserDefinedButtonCaptions = TRUE;
	return g_msgboxMgr->XMessageBox(NULL, lpText, lpCaption, uType, &xmb);
}

int ShowXMessageBoxTimeOut( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType,DWORD dwSeconds)
{
    MagicView::CommonUI::XMSGBOXPARAMS xmb;
    xmb.nTimeoutSeconds = dwSeconds;
    xmb.bUseUserDefinedButtonCaptions = TRUE;
    return g_msgboxMgr->XMessageBox(NULL, lpText, lpCaption, uType|MB_DONOTASKAGAIN, &xmb);
}


void GetSoft(LPCTSTR lpszMessageText, DWORD dwUserData)
{
	SysCall::BrowseURL((TCHAR*)dwUserData);
}

std::tstring GetMediaServerDefaultDir()
{
    return GetControllerDefaultDir();
}

std::tstring GetControllerServerDefaultDir()
{
    return GetControllerDefaultDir();
}

std::tstring GetMediaLibraryDefaultDir()
{
    return GetControllerDefaultDir();
}

std::tstring GetClientDefaultDir()
{
    return GetControllerDefaultDir();
}

CWnd *SearchWndObj(CWnd *pStartWnd ,CRuntimeClass *pRuntime)
{
    if(pStartWnd->IsKindOf(pRuntime))      
        return pStartWnd;

    CWnd *pChild = pStartWnd->GetWindow(GW_CHILD);
    while(pChild)
    {

        CWnd *pSubChild = pChild->GetNextWindow(GW_CHILD);
        if(pSubChild)
            pSubChild = SearchWndObj(pChild,pRuntime);

        if(pSubChild)
            return pSubChild;

        if(pChild->IsKindOf(pRuntime))
            return pChild;
        pChild = pChild->GetNextWindow();
    }

    return NULL;
}

CWnd *GetCurrentPageCtrl(CRuntimeClass *p)
{
	CWnd *pMainWnd = ::AfxGetMainWnd();
	CWnd *pChild = pMainWnd->GetWindow(GW_CHILD);
	while(pChild)
	{
		if(pChild->IsKindOf(p))
		{
		  if(pChild->IsWindowVisible())
		  return pChild;
		}
		pChild = pChild->GetWindow(GW_HWNDNEXT);
	}
	return NULL;
}

CWnd *GetRuntimePageCtrl(CRuntimeClass *p)
{
	CWnd *pMainWnd = ::AfxGetMainWnd();
	CWnd *pChild = pMainWnd->GetWindow(GW_CHILD);
	while(pChild)
	{
		if(pChild->IsKindOf(p))
		  return pChild;
		pChild = pChild->GetWindow(GW_HWNDNEXT);
	}
	return NULL;
}


ControllerServerLib& g_ControllerServer()
{
    return the_ControllerServer;
}

MediaServerLib& g_MediaServer()
{
    return the_MediaServer;
}


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define RETRY_TIMES  10
#define INPUT_RETRY_TIMES 3

// CControllerApp

BEGIN_MESSAGE_MAP(CControllerApp, CWinAppEx)
	ON_COMMAND(ID_CONTEXT_HELP, &CControllerApp::OnHelp)
    ON_COMMAND(ID_APP_ABOUT, &CControllerApp::OnAppAbout)
    ON_COMMAND(ID_SYS_SETTING, &CControllerFrame::OnOptions)
    ON_COMMAND(ID_SYS_RE_CONNECT, &CControllerFrame::OnReconnect)
    ON_UPDATE_COMMAND_UI(ID_SYS_RE_CONNECT, OnUpdateReconnectRinbbonBtn)

END_MESSAGE_MAP()


BOOL CControllerApp::ParseCommandLine()
{
    LPWSTR *szArgList;
    int argCount;

    m_bNoLang = FALSE;
    szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);

    for (int i = 0; i < argCount; ++i)
    {
        CString arg = szArgList[i];
        if (arg == _T("/nolang"))
        {
            m_bNoLang = TRUE;
        }
    }
#if 0
    LPWSTR *szArgList;
    int argCount;

    szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
    if (szArgList == NULL)
    {
        _MessageBox4P(NULL, L"Unable to parse command line", L"Error", MB_OK);
        exit(1);
    }

    CString ControllerServer, ControllerServerPort, ConfigFilePath;
    // "/Install [CONTROLLER_SERVER] [CONTROLLER_SERVER_PORT] "[ALLUSERDATA]S3Graphics\S3Signage\S3SignageController\"" when do install
    for(int i = 0; i < argCount; i++)
    {
        //_MessageBox4P(NULL, szArgList[i], L"Arglist contents", MB_OK);

        CString myArg = szArgList[i];
        if (myArg == _T("/Install") && (i + 2) < argCount)
        {
            ControllerServer = szArgList[i + 1];
            ControllerServerPort = szArgList[i + 2];
            ConfigFilePath = szArgList[i + 3];
            i += 3;
        }
    }

    if (!ConfigFilePath.IsEmpty())
    {
        ControllerConfig configFile;
        configFile.LoadDefaults();
        configFile.Load((GetControllerDefaultDir() + SETTING_FILE_NAME).c_str());
        configFile.Set(CONTROLLER_SERVER_IP, ControllerServer);
        configFile.Set(CONTROLLER_SERVER_PORT, ControllerServerPort);
        configFile.Set(CONFIG_PATH, ConfigFilePath);

        // Clear the previous installed config, for these config is changed on the installing process
        CString ConfigDir = configFile.Get(CONFIG_PATH);
        ConfigDir.Trim();
        if ((ConfigDir.ReverseFind(_T('\\')) + 1) != ConfigDir.GetLength())
        {
            ConfigDir += _T("\\");
        }
        if (!ConfigDir.IsEmpty())
        {
			ProcessCall::SetDirectoryWithFullControlACL(ConfigDir, DOMAIN_ALIAS_RID_USERS);
            ProcessCall::SetDirectoryWithFullControlACL(ConfigDir, DOMAIN_ALIAS_RID_POWER_USERS);
            ProcessCall::SetDirectoryWithFullControlACL(ProcessCall::GetAppPath().c_str(),
                DOMAIN_ALIAS_RID_USERS);
            ProcessCall::SetDirectoryWithFullControlACL(ProcessCall::GetAppPath().c_str(),
                DOMAIN_ALIAS_RID_POWER_USERS);

            if (PathFileExists(ConfigDir))
            {
                CString TargetConfigFile = ConfigDir + SETTING_FILE_NAME;
                if(PathFileExists(TargetConfigFile))
                {
                    configFile.LoadDefaults();
                    configFile.Load(TargetConfigFile);
                    configFile.Set(CONTROLLER_SERVER_IP, ControllerServer);
                    configFile.Set(CONTROLLER_SERVER_PORT, ControllerServerPort);
                    configFile.Set(CONFIG_PATH, ConfigFilePath);

                    configFile.ChangeFilePathName(GetAppPath() + SETTING_FILE_NAME);

                    if (!DeleteFile(TargetConfigFile))
                    {
                        MessageBox(Translate(_T("Failed to clear previous installed config file!")),Translate(_T("Error:Controller")), MB_OK|MB_ICONERROR);
                    }
                }
            }
        }

        if (configFile.Save())
        {
            exit(0);
        }

        exit(1);
    }

    LocalFree(szArgList);
#endif
    return TRUE;
}

// CControllerApp construction

CControllerApp::CControllerApp() :
m_WorkThread(WorkThreadFun),
m_ConnectionObserverThread(ObserverThreadFun),
m_crashrptHelper(_T("S3 MagicView Controller")),
#ifdef STARTER_EDITION
m_logWatcher(_T("ControllerStarterLog.properties"), 5*1000)
#elif defined(STANDALONE_EDITION)
m_logWatcher(_T("ControllerStandaloneLog.properties"), 5*1000)
#elif defined(PROFESSIONAL_EDITION)
m_logWatcher(_T("ControllerProfessionalLog.properties"), 5*1000)
#else
m_logWatcher(_T("ControllerLog.properties"), 5*1000)
#endif
{
    // support Restart Manager
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#ifdef _MANAGED
    // If the application is built using Common Language Runtime support (/clr):
    //     1) This additional setting is needed for Restart Manager support to work properly.
    //     2) In your project, you must add a reference to System.Windows.Forms in order to build.
    System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

    // TODO: replace application ID string below with unique ID string; recommended
    // format for string is CompanyName.ProductName.SubProduct.VersionInformation
    SetAppID(_T("Controller.AppID.NoVersion"));

    // TODO: add construction code here,
    // Place all significant initialization in InitInstance

    m_bControllerServerOffline = TRUE;
	m_bShowReConnectDialog = TRUE;
}

CControllerApp::~CControllerApp()
{
	m_AlarmRecordNum = 0;
	//FreeLocalizationDict();
}
// The one and only CControllerApp object

CControllerApp theApp;
ULONG_PTR gdiplusToken;

#define CONTROLLER_SERVER_NAME (_T("ControllerServer.exe"))
#define MEDIA_SERVER_NAME (_T("MediaServer.exe"))

CString GetAppPath()
{
    CString strPathBuffer;
    TCHAR PathBuffer[255];

    GetModuleFileName(AfxGetInstanceHandle(),
        PathBuffer,
        255);

    strPathBuffer.Format ( _T("%s "),PathBuffer);

    CString strAppPath = strPathBuffer.Mid(0,strPathBuffer.ReverseFind (_T('\\'))+1);

    return strAppPath;

}

CString FormatSizeString(DWORD64 Size)
{
    CString FormatResult;
    if(Size < 1024)
    {
        FormatResult.Format(_T("%lldb"), Size);
    }else if(Size < 1024 * 1024)
    {
        FormatResult.Format(_T("%0.0fKb"), (float)Size/1024.0f);
    }else
    {
        FormatResult.Format(_T("%0.1fMb"), (float)Size/1024.0f/1024.0f);
    }
    return FormatResult;
}

void CControllerApp::OnControllerServerOffline(BOOL bLoginByOthers)
{
    TRACE0("Failed to connect Controller Server!\n");

    if (m_bControllerServerOffline)
    {
        return;
    }

    m_bControllerServerOffline = TRUE;

#ifndef SERVER_EDITION

	ShowXMessageBox(NULL,Translate(_T("Error happened, Please try to restart MagicView.")),Translate(_T("Error:Manager")), MB_OK|MB_ICONERROR);

	PostQuitMessage(100);

#endif

	if (FALSE == m_bShowReConnectDialog)
		return;

	m_bShowReConnectDialog     = FALSE;

    CControllerFrame* pFrame = (CControllerFrame*) m_pMainWnd;

    CString szInfo;
    if (bLoginByOthers)
    {
        szInfo = Translate(_T("Your account has logined in other Controller, or your account is deleted, are you going to re-connect?"));
    }
    else
    {
        szInfo = Translate(_T("Connection to server lost, are you going to re-connect?"));
    }
#ifdef SERVER_EDITION
    if (ShowXMessageBox(NULL,szInfo, Translate(_T("Tips:Controller")),MB_YESNO|MB_ICONINFORMATION) == IDYES)
#else
    if (ShowXMessageBox(NULL,szInfo, Translate(_T("Tips:Manager")),MB_YESNO|MB_ICONINFORMATION) == IDYES)
#endif
    {
        if (!GetControllerApp->Relogin())
        {
            if (pFrame)
            {
                pFrame->EnableCurPage(FALSE);

                pFrame->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
					(LPARAM)Translate(_T("Unable to connect controller server, Some functions are disabled.")).GetString());
            }

        }
    }
    else
    {
        if (pFrame)
        {
            pFrame->EnableCurPage(FALSE);

            pFrame->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
				(LPARAM)Translate(_T("Unable to connect controller server, Some functions are disabled.")).GetString());
        }

    }

	m_bShowReConnectDialog = TRUE;
}

BOOL CControllerApp::Relogin()
{
    CControllerFrame* pFrame = (CControllerFrame*) m_pMainWnd;

    if (pFrame)
    {
        pFrame->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
			(LPARAM)Translate(_T("Connecting to server......")).GetString());
	}

	CString ControllerServerIP = m_Config.Get(CONTROLLER_SERVER_IP);
	CString ControllerServerPort = m_Config.Get(CONTROLLER_SERVER_PORT);

    HRESULT hr = HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID);
    try {
        m_pControlerInterface->Restart();
        hr = m_pControlerInterface->Login();
    } catch (NetworkException&) {
        
    }
    if(FAILED(hr))
    {
        std::tstring ErrorMessage;
        unsigned int errID = m_pControlerInterface->GetLastError(ErrorMessage);

        if (S3NET_INVALID_ACCOUNT == errID || S3NET_LOGIN_PWD_NOT_MATCH == errID)
        {
            ShowXMessageBox(NULL,Translate(_T("Password invalid!")),Translate(_T("Warning:Controller")), MB_OK|MB_ICONEXCLAMATION);
        }
        else
        {
            ShowXMessageBox(NULL,Translate(_T("Unable to connect controller server!")),Translate(_T("Warning:Controller")), MB_OK|MB_ICONEXCLAMATION);
        }

        if (pFrame)
        {
            pFrame->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
                (LPARAM)Translate(_T("Re-Login Failed! Switch to Offline Mode!")).GetString());
        }

        return FALSE;
    }
    else
    {
        m_bControllerServerOffline = FALSE;
    }

//     m_Authority = m_pControlerInterface->GetPermissionType();

    if (pFrame)
    {
        pFrame->RefreshPages();

        pFrame->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)Translate(_T("Re-Login Succeeded!")).GetString());
    }

    return TRUE;
}

// CControllerApp initialization
//#include <math.h>

typedef LRESULT (__stdcall *AfxWndProc_t)(HWND, UINT, WPARAM, LPARAM);
typedef BOOL (PASCAL *WalkPreTranslateTree_t)(HWND hWndStop, MSG* pMsg);
static AfxWndProc_t AfxWndProcOld = NULL;
static WalkPreTranslateTree_t WalkPreTranslateTreeOld = NULL;

static LRESULT __stdcall AfxWndProc_Hook(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    try {
        if (AfxWndProcOld != NULL)
        {
            return AfxWndProcOld(hWnd, nMsg, wParam, lParam);
        }
        else
        {
            ASSERT(!"AfxWndProc not hooked");
            return DefWindowProc(hWnd, nMsg, wParam, lParam);
        }
    }
    catch (NetworkException&) {
        GetControllerApp->OnControllerServerOffline(FALSE);
        return DefWindowProc(hWnd, nMsg, wParam, lParam);
    }
}

static BOOL PASCAL WalkPreTranslateTree_Hook(HWND hWndStop, MSG* pMsg)
{
    ASSERT(hWndStop == NULL || ::IsWindow(hWndStop));
    ASSERT(pMsg != NULL);

    // walk from the target window up to the hWndStop window checking
    //  if any window wants to translate this message

    for (HWND hWnd = pMsg->hwnd; hWnd != NULL; hWnd = ::GetParent(hWnd))
    {
        CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);
        if (pWnd != NULL)
        {
            try {
                // target window is a C++ window
                if (pWnd->PreTranslateMessage(pMsg))
                    return TRUE; // trapped by target window (eg: accelerators)
            } catch (NetworkException&) {
                GetControllerApp->OnControllerServerOffline(FALSE);
            }
        }

        // got to hWndStop window without interest
        if (hWnd == hWndStop)
            break;
    }
    return FALSE;       // no special processing
}

BOOL CControllerApp::InitInstance()
{
	g_msgboxMgr.reset(new MagicView::CommonUI::S3MessageBoxManager);


#ifdef _DEBUG
    const static TCHAR* dllname = _T("mfc100ud.dll");
    const WORD AfxInternalPreTranslateMessageOrdinal = 2331;
    const WORD CWnd_WalkPreTranslateTreeOrdinal = 15948;
#else
    const static TCHAR* dllname = _T("mfc100u.dll");
    const WORD AfxInternalPreTranslateMessageOrdinal = 1960;
    const WORD CWnd_WalkPreTranslateTreeOrdinal = 13367;
#endif

    HMODULE m = LoadLibrary(dllname);
    if (!m)
        return FALSE;

    void* CWnd_WalkPreTranslateTreeOrdinal_Addr = GetProcAddress(m, (LPCSTR)CWnd_WalkPreTranslateTreeOrdinal);
    if (!CWnd_WalkPreTranslateTreeOrdinal_Addr)
    {
        return FALSE;
    }

    // hook AfxWndProc
    MH_Initialize();
    MH_CreateHook(&AfxWndProc, &AfxWndProc_Hook, (void**)&AfxWndProcOld);
    MH_EnableHook(&AfxWndProc);
    MH_CreateHook(CWnd_WalkPreTranslateTreeOrdinal_Addr, &WalkPreTranslateTree_Hook, (void**)&WalkPreTranslateTreeOld);
    MH_EnableHook(CWnd_WalkPreTranslateTreeOrdinal_Addr);

    ParseCommandLine();

    PrepareWorkThread();

    g_backgroundTask.reset(new CBackgroundTask);

#if 0
	// the current folder's config just provide real config file's path
	// if the real config file's path is empty, we use the current folder's config
	CString SettingFile = GetAppPath() + SETTING_FILE_NAME;
	if (PathFileExists(SettingFile))
	{
		m_Config.Load(SettingFile);
	}
	else
	{
		m_Config.Save(SettingFile);
	}

	//{ //this a tool to  translate bmp to gray.
	//CStdioFile cf(_T(".\\res\\MediaLibaryToolBarLarge.bmp"),CStdioFile::modeRead |CStdioFile::typeBinary);
	//int len = cf.GetLength();
	//PBYTE pData = new BYTE[len];
	//int nRead = cf.Read(pData,len);
	//cf.Close();

	////BI_JPEG
	//PBITMAPFILEHEADER pBitmap = (PBITMAPFILEHEADER )pData;
	//DWORD *pPiexel = (DWORD*)( pData + pBitmap->bfOffBits );

	//for( int i = 0;i < (nRead - sizeof(BITMAPINFO) - sizeof(BITMAPFILEHEADER))/4;i++)
	//{
	//
	//	DWORD pixel = pPiexel[i];

	//	BYTE r = GetRValue(pixel);
	//	BYTE g = GetGValue(pixel);
	//	BYTE b = GetBValue(pixel);

	//
	//	int d = (int)sqrt(((float)r*r + g*g + b*b))/3 ;
	//	if(d) d+=30;
	//
	//	pPiexel[i] =RGB(d,d,d);
	//	if( 0 == d)
	//		pPiexel[i] = RGB(0,255,0);//bk color
	//}
	//CStdioFile cf2(_T("d:\\ttt.bmp"),CStdioFile::modeCreate|CStdioFile::modeReadWrite | CStdioFile::typeBinary);
	//cf2.Write(pData,nRead);
	//cf2.Close();

	//delete pData;
	//}

	CString ConfigPath = m_Config.Get(CONFIG_PATH);
	ConfigPath.Trim();
	if (!ConfigPath.IsEmpty() && PathFileExists(ConfigPath))
	{
		if ((ConfigPath.ReverseFind(_T('\\')) + 1) != ConfigPath.GetLength())
		{
			ConfigPath += _T("\\");
		}

		CString Target = ConfigPath + SETTING_FILE_NAME;
		if(!PathFileExists(Target))
		{
			CString Orig = GetAppPath() + SETTING_FILE_NAME;
			if (!PathFileExists(Orig))
			{
				CString ErrorMsg, str;
				str.Format(_T("%s!"), Orig);
				ErrorMsg = Translate(_T("Failed to load config file: ")) + str;
				MessageBox(ErrorMsg,Translate(_T("Error:Controller")), MB_OK|MB_ICONERROR);
			}

			BOOL bCopy =  CopyFile(Orig,    //pointer   to   name   of   an   existing   file
				Target,  //pointer   to   filename   to   copy   to
				FALSE);  //bFailIfExists flag   for   operation   if   file   exists
		}

		m_Config.Load(Target);
	}
#endif
    //ControllerConfig m_Config;
    if (!m_Config.Load((GetControllerDefaultDir() + SETTING_FILE_NAME).c_str()))
    {
        m_Config.Save();
    }
	CControllerSettings controllerSetting(&m_Config);
	/*if (controllerSetting.m_LanguageSetting.m_SelLanguage == _T("Chinese"))
	{
		InitLocalzation(GetAppPath() + _T("Languages\\Chinese.xml"));
	}*/
	CString path = GetControllerDefaultDir().c_str();
	if (path.IsEmpty())
	{
		TCHAR modulePath[MAX_PATH];
		::GetModuleFileName(NULL, modulePath, MAX_PATH);
		path = StringUtility::GetFolder(modulePath).c_str();
	}
    (*g_msgboxMgr).SetIniPath((path + _T("\\msgbox.ini")).GetString());
	map<CString,CString> & LanMap = GetLanguageNameMap();
	if(LanMap.size())
	{
		CString lanLocName = ChangeEngToLocLanName(controllerSetting.m_LanguageSetting.m_SelLanguage);
		map<CString,CString>::iterator it = LanMap.find( lanLocName);
		if(it != LanMap.end())
			if(::PathFileExists(it->second))
				InitLocalzation(it->second);

	}


    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinAppEx::InitInstance();

    // Initialize OLE libraries
    if (!AfxOleInit())
    {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }

    if (!m_plugMgr.Init())
        return FALSE;

    m_plugMgr.SetHost(this);
    CreatePluginObjs(controllerSetting.m_LanguageSetting.m_SelLanguage.GetString());

    

    HANDLE hMutex = CreateMutex(NULL, FALSE, _T("MagicView Controller"));

	if (hMutex == INVALID_HANDLE_VALUE ||
		GetLastError() == ERROR_ALREADY_EXISTS)
	{
#if defined(STARTER_EDITION) || defined(PROFESSIONAL_EDITION)
        ShowXMessageBox(NULL, Translate(_T("MagicView Manager is already running")), Translate(_T("MagicView Manager")), MB_ICONINFORMATION);
#elif defined(SERVER_EDITION)
		ShowXMessageBox(NULL, Translate(_T("MagicView Controller is already running")), Translate(_T("MagicView Controller")), MB_ICONINFORMATION);
#endif
        return FALSE;
	}

    AfxEnableControlContainer();

    EnableTaskbarInteraction(FALSE);

    // AfxInitRichEdit2() is required to use RichEdit control
    // AfxInitRichEdit2();

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need
    // Change the registry key under which our settings are stored
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization
    SetRegistryKey(_T("S3 Signage Controller"));

    InitContextMenuManager();

    InitKeyboardManager();

    InitTooltipManager();
    CMFCToolTipInfo ttParams;
    ttParams.m_bVislManagerTheme = TRUE;
    theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
        RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);



    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


    // Set color button tool tips
#define ADD_COLOR(v, name) CMFCColorButton::SetColorName(RGB((v >> 16), (v & 0x00ff00) >>8, (v & 0x0000ff)), Translate(name))
    ADD_COLOR(0xff0000, Translate(_T("Red")));
    ADD_COLOR(0x00ff00, Translate(_T("Lime")));
    ADD_COLOR(0x0000ff, Translate(_T("Blue")));
    ADD_COLOR(0x800000, Translate(_T("Maroon")));
    ADD_COLOR(0x008000, Translate(_T("Green")));
    ADD_COLOR(0x000000, Translate(_T("Black")));
    ADD_COLOR(0x808000, Translate(_T("Olive")));
    ADD_COLOR(0x000080, Translate(_T("Navy")));
    ADD_COLOR(0x800080, Translate(_T("Purple")));
    ADD_COLOR(0x008080, Translate(_T("Teal")));
    ADD_COLOR(0xc0c0c0, Translate(_T("Silver")));
    ADD_COLOR(0x808080, Translate(_T("Gray")));
    ADD_COLOR(0xffff00, Translate(_T("Yellow")));
    ADD_COLOR(0xff00ff, Translate(_T("Magenta")));
    ADD_COLOR(0x00ffff, Translate(_T("Cyan")));
    ADD_COLOR(0xffffff, Translate(_T("White")));
    ADD_COLOR(0xc0dcc0, Translate(_T("Lightgreen")));
    ADD_COLOR(0xa6caf0, Translate(_T("Lightblue")));
    ADD_COLOR(0xfffbf0, Translate(_T("Lightyellow")));
    ADD_COLOR(0xa0a0a4, Translate(_T("Lightgray")));
#undef ADD_COLOR

#if defined(STARTER_EDITION) || defined(PROFESSIONAL_EDITION)
    // change windows and message title
    //First free the string allocated by MFC at CWinApp startup.
    //The string is allocated before InitInstance is called.
    free((void*)m_pszAppName);
    //Change the name of the application file.
    //The CWinApp destructor will free the memory.
    m_pszAppName = _tcsdup(Translate(_T("S3MagicView Manager")).GetString());
#elif defined(SERVER_EDITION)
    // change windows and message title
    //First free the string allocated by MFC at CWinApp startup.
    //The string is allocated before InitInstance is called.
    free((void*)m_pszAppName);
    //Change the name of the application file.
    //The CWinApp destructor will free the memory.
    m_pszAppName = _tcsdup(Translate(_T("S3MagicView Controller")).GetString());
#endif

    int imgId = 0;
#ifdef SERVER_EDITION
    imgId = IDB_NEW_LOADING;
    if (GetLocLanName().CompareNoCase(_T("CHS")) == 0)
    {
        imgId = IDB_NEW_LOADING_CHINESE;
    }
#else
    imgId = IDB_LOADING_STARTER;
    if (GetLocLanName().CompareNoCase(_T("CHS")) == 0)
    {
        imgId = IDB_LOADING_STARTER_CHINESE;
    }

#endif

   


#ifdef STARTER_EDITION
    ProcessCall::GetDebugPriv();

    OnStarterEditionStartup();

    m_Username = _T("admin");
    m_Password = _T("admin");

#else
#ifdef PROFESSIONAL_EDITION

    ProcessCall::GetDebugPriv();

    OnProfessionalEditionStartup();

    m_Username = _T("admin");
    m_Password = _T("admin");

#endif
#endif

    UINT AtpInputTimes = 0;
    CSplashWnd splashWnd(imgId);
    UINT AttemptTimes = 0;
    while(AtpInputTimes < INPUT_RETRY_TIMES)
    {
#ifdef SERVER_EDITION

        CDemoTranslucentDlg dlg(IDB_LOGIN_BG);

        dlg.DoModal();

        if(!dlg.GetStandardDialog()->m_bCancel)
        {
            m_Username = dlg.GetStandardDialog()->m_Username;
            m_Password = dlg.GetStandardDialog()->m_Password;
        }
        else
            return FALSE;
#endif
       
        std::tstring versionString = _T("Version ") + ProcessCall::GetMyVersion();
        splashWnd.SetString(versionString);
        splashWnd.Show();

        CString ControllerServerIP = m_Config.Get(CONTROLLER_SERVER_IP);
        CString ControllerServerPort = m_Config.Get(CONTROLLER_SERVER_PORT);

        m_pControlerInterface.reset(m_connManager.CreateControllerClientInterface(
            Account(m_Username.GetString(), m_Password.GetString()),
            ControllerServerIP.GetString(), ControllerServerPort.GetString()));

        int bias = 0;
        HRESULT hr = S_FALSE;
        
        while (hr != S_OK && AttemptTimes < RETRY_TIMES)
        {
            try {
                m_pControlerInterface->Restart();
                hr = m_pControlerInterface->GetServerTimeZone(bias);
            } catch (NetworkException&) {
                hr = HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID);
            }

/*
            if (hr != S_OK)
            {
                Sleep(1000);
            }
*/

            AttemptTimes++;
        }

        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            splashWnd.Hide();
#ifdef SERVER_EDITION
            ShowXMessageBox(NULL, Translate(_T("Failed to connect Controller Server!")), Translate(_T("Failed to connect Controller Server!")), MB_OK|MB_TOPMOST|MB_ICONEXCLAMATION);
#else
#ifndef STANDALONE_EDITION
            if (!g_ControllerServer().IsHardKeyConnected())
            {
                ShowXMessageBox(NULL, Translate(_T("No USB key detected, unable to initialize controller, please insert the USB key and restart!")), Translate(_T("Error:Manager")), MB_OK|MB_TOPMOST|MB_ICONEXCLAMATION );
            }
            else
#endif
            {
                ShowXMessageBox(NULL, Translate(_T("Failed to initialize controller!")), Translate(_T("Error:Controller")), MB_OK|MB_TOPMOST|MB_ICONEXCLAMATION);
            }
#endif
            return FALSE;
        }

        if (hr == S_OK)
        {
            splashWnd.Hide();
            TIME_ZONE_INFORMATION timeZoneInfo;
            GetTimeZoneInformation(&timeZoneInfo);

            if (timeZoneInfo.Bias != bias)
            {
                if (ShowXMessageBox(NULL, Translate(_T("The time zone in client is different with controller server, it may cause schedule problem. Do you want to continue?")),
                    Translate(_T("Warning")), MB_OKCANCEL) == IDCANCEL)
                {
                    return FALSE;
                }
            }
        }

        hr = HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID);
        AttemptTimes = 0;
        while (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID) && AttemptTimes < RETRY_TIMES)
        {
            try {
                hr = m_pControlerInterface->Login();
            } catch (NetworkException&) {
                hr = HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID);
            }


            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                Sleep(1000);
            }

            AttemptTimes++;
        }

        if(FAILED(hr))
        {
            splashWnd.Hide();
            m_bControllerServerOffline = TRUE;
		    if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
		    {
    #ifdef SERVER_EDITION
                ShowXMessageBox(NULL, Translate(_T("Failed to connect Controller Server!")), Translate(_T("Error")), MB_OK);
    #else
			    #ifndef STANDALONE_EDITION
                if (!g_ControllerServer().IsHardKeyConnected())
                {
                    ShowXMessageBox(NULL, Translate(_T("No USB key detected, unable to initialize controller, please insert the USB key and restart!")), Translate(_T("Error")), MB_OK|MB_ICONEXCLAMATION);
                }
                else
			    #endif
                {
                    ShowXMessageBox(NULL, Translate(_T("Failed to initialize controller!")), Translate(_T("Error")), MB_OK);
                }
    #endif
		    }
		    else
		    {
			    std::tstring ErrorMessage;
			    unsigned int errID = m_pControlerInterface->GetLastError(ErrorMessage);

			    if (S3NET_INVALID_ACCOUNT == errID)
			    {
				    ShowXMessageBox(NULL, Translate(_T("Please use correct account! Only three times.")), Translate(_T("Error")), MB_OK|MB_ICONEXCLAMATION);
                    AtpInputTimes++;
			    }
			    else if (S3NET_LOGIN_PWD_NOT_MATCH == errID)
			    {
				    ShowXMessageBox(NULL, Translate(_T("Please input correct password! Only three times.")), Translate(_T("Error")), MB_OK|MB_ICONEXCLAMATION);
                    AtpInputTimes++;
			    }
			    else
			    {
				    CString ErrorMessageString;
				    ErrorMessageString.Format(_T("%s"), ErrorMessage.c_str());
				    ShowXMessageBox(NULL, Translate(ErrorMessageString), Translate(_T("Error")), MB_OK);
                    return FALSE;
			    }
		    } 
        }
        else
        {
            m_bControllerServerOffline = FALSE;
            break;
        }
    }
    if (AtpInputTimes >= INPUT_RETRY_TIMES)
    {
        ShowXMessageBox(NULL, Translate(_T("Attempt times has reached three times, the controller will exit.")), Translate(_T("Error")), MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }

//     m_Authority = m_pControlerInterface->GetPermissionType();

    m_pMediaLibraryInterface.reset(new MediaLibrary(_T("ControllerMediaLibrary.ini"), m_pControlerInterface, FALSE));
#ifndef SERVER_EDITION
    m_pMediaLibraryInterface->SetNewMediaInitStatus(Audit::APPROVED);
    m_pMediaLibraryInterface->SetCollectFileAfterUploaded(FALSE);
#endif

#ifndef _DEBUG
#ifndef SERVER_EDITION
	#ifndef STANDALONE_EDITION
	if (!g_ControllerServer().IsHardKeyConnected())
	{
        splashWnd.Hide();
		ShowXMessageBox(splashWnd.GetWindowHwnd(), Translate(_T("No USB key detected, unable to initialize controller, please insert the USB key and restart!")), Translate(_T("Error")), MB_OK|MB_TOPMOST|MB_ICONEXCLAMATION);
		return FALSE;
	}
	#endif
#endif
#endif
    m_pMediaLibraryInterface->Init();

	BOOL bConnect = m_pMediaLibraryInterface->Logon();
    AttemptTimes = 0;
    while(!bConnect && AttemptTimes < RETRY_TIMES)
    {
		MagicView::CommonUI::XMSGBOXPARAMS xmb;
		xmb.bUseUserDefinedButtonCaptions = TRUE;
		//_tcscpy(xmb.UserDefinedButtonCaptions.szReport, Translate(_T("Error")));
		xmb.nTimeoutSeconds = 5;
#define Translate(x) x
		int retCode = (*g_msgboxMgr).XMessageBox(NULL,
							Translate(_T("Connect Media Server failed, would you try it again?")),
							Translate(_T("Error")),
							MB_RETRYCANCEL|MB_ICONERROR|MB_DONOTASKAGAIN,
							&xmb);
#undef Translate
		if (   IDRETRY == (retCode & (~MB_DONOTASKAGAIN)) 
		    || IDRETRY == (retCode & (~MB_TIMEOUT)))
		{
			AttemptTimes++;
			bConnect = m_pMediaLibraryInterface->Logon();
			Sleep(1000);
		}
		else
		{
			return FALSE;
		}

    }

	if (!bConnect)	
	{
		splashWnd.Hide();
#ifdef SERVER_EDITION
		ShowXMessageBox(NULL, 
			Translate(_T("Failed to connect Media Server!")), 
			Translate(_T("Failed to connect Media Server!")), 
			MB_OK|MB_TOPMOST|MB_ICONEXCLAMATION);
#endif
		return FALSE;
	}


    CString AppTitle =  Translate(_T("S3 MagicView Controller (Server Edition) - ")) + ProcessCall::GetMyVersion().c_str();

#ifdef STARTER_EDITION

    OnStarterEditionPrepareSetting();

    AppTitle =  Translate(_T("S3 MagicView Manager (Starter Edition) - ")) + ProcessCall::GetMyVersion().c_str();

#endif

#ifdef PROFESSIONAL_EDITION

    OnProfessionalEditionPrepareSetting();
#ifdef STANDALONE_EDITION
    AppTitle = Translate(_T("S3 MagicView Manager (Standalone Edition) - ")) + ProcessCall::GetMyVersion().c_str();
#else
    AppTitle = Translate(_T("S3 MagicView Manager (Professional Edition) - ")) + ProcessCall::GetMyVersion().c_str();
#endif

#endif

    // To create the main window, this code creates a new frame window
    // object and then sets it as the application's main window object
    CControllerFrame* pFrame = new CControllerFrame;
    if (!pFrame)
        return FALSE;
    m_pMainWnd = pFrame;
    // create main MDI frame window
    //if (!pFrame->LoadFrame(IDR_MAINFRAME))

    if (!pFrame->Create(NULL,AppTitle, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CFrameWnd::rectDefault, NULL, NULL, NULL))
    {
        return FALSE;
    }
    // try to load shared MDI menus and accelerator table
    //TODO: add additional member variables and load calls for
    //	additional menu types your application may need
    HINSTANCE hInst = AfxGetResourceHandle();
    m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_ControllerTYPE));
    m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ControllerTYPE));

    // trigger init controller page
    pFrame->SendMessage(AFX_WM_ON_CHANGE_RIBBON_CATEGORY);

    // The one and only window has been initialized, so show and update it
    pFrame->ShowWindow(SW_SHOW);
    pFrame->UpdateWindow();

	Translate(pFrame);

    static CMFCToolBarFontComboBox unused(0, -1);

    splashWnd.Hide();

    OnStartupCheckingConfig();

	g_backgroundTask->startTimer(boost::bind(&CControllerApp::handle_timeout_app, &theApp, _1, _2));

    // call DragAcceptFiles only if there's a suffix
    //  In an SDI app, this should occur after ProcessShellCommand
    return TRUE;
}

int CControllerApp::ExitInstance()
{
    CleanWorkThread();

    //TODO: handle additional resources you may have added
    try {
	if (m_pControlerInterface)
		m_pControlerInterface->Logout();
    } catch (NetworkException&) {

    }

    m_pMediaLibraryInterface.reset();
    m_pControlerInterface.reset();

    for (auto it = m_plugins.begin(); it != m_plugins.end(); ++it)
    {
        delete *it;
    }
    for (auto it = m_PluginType.begin(); it != m_PluginType.end(); ++it)
    {
        if (it->second)
        {
            delete [] it->second->pSupportFile;
            delete it->second;
        }
    }
    //m_PluginObj.clear();
    m_PluginType.clear();

    m_plugins.clear();
    m_plugMgr.Shutdown();
    AfxOleTerm(FALSE);

    Gdiplus::GdiplusShutdown(gdiplusToken);

#ifdef STARTER_EDITION
    OnStarterEditionExit();
#else
#ifdef PROFESSIONAL_EDITION
    OnProfessionalEditionExit();
#endif
#endif

    MH_Uninitialize();
    return CWinAppEx::ExitInstance();
}

// CControllerApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    // Dialog Data
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd *pWnd,UINT nHitTest,UINT message);
    // Implementation
protected:
    DECLARE_MESSAGE_MAP()

public:
	CMFCLinkCtrl m_S3WebSite;
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect logoRect;
	GetDlgItem(IDC_LOGO)->GetWindowRect(&logoRect);
	this->ScreenToClient(&logoRect);

	if(logoRect.PtInRect(point))
	{
		//m_S3WebSite.SendMessage(WM_LBUTTONDOWN,0,0);
		CString strURL;
		m_S3WebSite.GetWindowText(strURL);
		if (::ShellExecute(NULL, NULL, strURL, NULL, NULL, SW_SHOWNORMAL) <(HINSTANCE) 32)
		{
			TRACE(_T("Can't open URL: %s\n"), strURL);
		}
	}
}

BOOL  CAboutDlg::OnSetCursor(CWnd *pWnd,UINT nHitTest,UINT message)
{
	CRect logoRect;
	CPoint pt;
	GetDlgItem(IDC_LOGO)->GetWindowRect(&logoRect);
	this->ScreenToClient(&logoRect);
	::GetCursorPos(&pt);
	this->ScreenToClient(&pt);

	if(logoRect.PtInRect(pt))
	{
		HCURSOR hCursor = ::LoadCursor(NULL,IDC_HAND);
		hCursor = ::SetCursor(hCursor);
		if(hCursor)
			::DestroyCursor(hCursor);
		return true; 
	}
	return __super ::OnSetCursor(pWnd,nHitTest,message);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_S3LINK, m_S3WebSite);
}

BOOL CAboutDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

#ifdef STARTER_EDITION
    SetDlgItemText(IDC_STATIC_VERSION, (Translate(_T("S3 MagicView Manager (Starter Edition) \n")).GetString() + ProcessCall::GetMyFullVersion()).c_str());
    SetWindowText(Translate(_T("About Manager")));
#else
#ifdef PROFESSIONAL_EDITION

    INT nClientLimit = g_ControllerServer().GetClientNumLimit();
    CString szInfo;
    szInfo.Format(Translate(_T("\n(USB key support %d clients)")), nClientLimit);
#ifdef STANDALONE_EDITION
    SetDlgItemText(IDC_STATIC_VERSION, (Translate(_T("S3 MagicView Manager (Standalone Edition) \n")).GetString() + ProcessCall::GetMyFullVersion()).c_str());
#else
    SetDlgItemText(IDC_STATIC_VERSION, (Translate(_T("S3 MagicView Manager (Professional Edition) \n")).GetString() + ProcessCall::GetMyFullVersion()/* + szInfo.GetString()*/).c_str());
#endif
    SetWindowText(Translate(_T("About Manager")));
#else
    SetDlgItemText(IDC_STATIC_VERSION, (Translate(_T("S3 MagicView Controller (Server Edition) \n")).GetString() + ProcessCall::GetMyFullVersion()).c_str());
    SetWindowText(Translate(_T("About Controller")));
#endif
#endif

	Translate(this);
    return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
ON_WM_LBUTTONDOWN()
ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void CControllerApp::OnHelp()
{
	int err = 33;
	CString strPath = SysCall::GetModuleDirectory().c_str();
	strPath += _T("\\Doc\\");

	CString languge = GetLocLanName();
	if (languge.CompareNoCase(_T("CHS")) == 0/*|| languge.CompareNoCase(_T("CHT")) == 0*/)
	{
		strPath += _T("S3MagicView3 1_UserManual_Chinese_v1.0.pdf");
	}
	else
	{
		strPath += _T("S3MagicView3 1_UserManual_English_v1.0.pdf");
	}

	err = reinterpret_cast<int>(ShellExecute(NULL, _T("open"), strPath.GetString(), NULL, NULL, SW_SHOWNORMAL));

	if (err <= 32)
	{
		if (err == SE_ERR_ACCESSDENIED || err == SE_ERR_NOASSOC)
		{
			MagicView::CommonUI::XMSGBOXPARAMS xmb;

			xmb.bUseUserDefinedButtonCaptions = TRUE;
			_tcscpy(xmb.UserDefinedButtonCaptions.szReport, Translate(_T("Get Adobe Reader")));
			xmb.dwReportUserData = (DWORD)_T("http://www.adobe.com/products/reader.html");
			xmb.lpReportFunc = GetSoft;
#define Translate(x) x
			g_msgboxMgr->XMessageBox(NULL,
				Translate(_T("Please install pdf file reader firstly.")),
				Translate(_T("Error:Controller")),
				MB_OK|MB_ICONERROR,
				&xmb);
#undef Translate

			//MessageBox(Translate(_T("Please install pdf file reader firstly.")),Translate(_T("Error:Controller")), MB_OK|MB_ICONERROR);
		}
		else
		{
			MessageBox(Translate(_T("Open help document failed.")),Translate(_T("Error:Controller")), MB_OK|MB_ICONERROR);
		}
	}
}

// App command to run the dialog
void CControllerApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

// CControllerApp customization load/save methods

void CControllerApp::PreLoadState()
{
    BOOL bNameValid;
    CString strName;
    bNameValid = strName.LoadString(IDS_EDIT_MENU);
    ASSERT(bNameValid);
    GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CControllerApp::LoadCustomState()
{
}

void CControllerApp::SaveCustomState()
{
}

// CControllerApp message handlers



void CControllerApp::OnUpdateReconnectRinbbonBtn(CCmdUI* pCmdUI)
{

    if (!IsLogin())
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }

    return;
}

void CControllerApp::OnStartupCheckingConfig()
{
    if (!::PathFileExists(GetConfig->Get(ROOT_DIR)))
    {
        CString szInfo;
        szInfo.Format(Translate(_T("The previous root temp directory '%s' is invalid, please select a new root temp directory!"))
            , GetConfig->Get(ROOT_DIR));
        ShowXMessageBox(NULL,szInfo,Translate(_T("Warning:Check configure")), MB_OK| MB_ICONEXCLAMATION);

        WCHAR szDir[MAX_PATH];
        BROWSEINFO bi;
        ITEMIDLIST *pidl;

        bi.hwndOwner = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
        bi.pidlRoot = NULL;
        bi.pszDisplayName = szDir;
        bi.lpszTitle = Translate(_T("Please select the root temp directory for controller:"));
        bi.ulFlags = BIF_RETURNONLYFSDIRS;
        bi.lpfn = NULL;
        bi.lParam = 0;
        bi.iImage = 0;

        pidl = SHBrowseForFolder(&bi);
        if(pidl == NULL)
        {
            CString szInfo;
            szInfo.Format(_T("Failed to set path, Change the root temp directory to \"C:\\ControllerTemp\\\""));
            ShowXMessageBox(NULL,Translate(szInfo),Translate(_T("Error:Check configure")), MB_OK|MB_ICONERROR);

            GetConfig->Set(ROOT_DIR, _T("C:\\ControllerTemp\\"));
        }
        else
        {
            CString strPath;
            if(SHGetPathFromIDList(pidl, szDir))
            {
                strPath.Format(L"%s", szDir);

                GetConfig->Set(ROOT_DIR, strPath);
            }
            else
            {
                CString szInfo;
                szInfo.Format(_T("Failed to set path \"%s\", Change the root temp directory to \"C:\\ControllerTemp\\\"")
                    , strPath);
#ifdef SERVER_EDITION
                ShowXMessageBox(NULL,Translate(szInfo),Translate(_T("Error:Controller")), MB_OK|MB_ICONERROR);
#else
                ShowXMessageBox(NULL,Translate(szInfo),Translate(_T("Error:Manager")), MB_OK|MB_ICONERROR);
#endif

                GetConfig->Set(ROOT_DIR, _T("C:\\ControllerTemp\\"));
            }
        }
    }

    GetConfig->Save();
}

#ifdef STARTER_EDITION

void CControllerApp::OnStarterEditionStartup()
{
    PROCESS_INFORMATION ServerProcess = {0};

    // start controller server
    if (ProcessCall::FindProcess(CONTROLLER_SERVER_NAME) > 0)
    {
        ProcessCall::KillProcess(CONTROLLER_SERVER_NAME);
    }

    if(!g_ControllerServer().StartServer(System::STARTER_VERTION))
    {
        // failed
        ShowXMessageBox(NULL, Translate(_T("Failed to start Controller Server!")),Translate(_T("Error:Manager")), MB_OK|MB_ICONERROR);
    }

    //start media server
    if (ProcessCall::FindProcess(MEDIA_SERVER_NAME) > 0)
    {
        ProcessCall::KillProcess(MEDIA_SERVER_NAME);
    }

    if(!g_MediaServer().StartServer())
    {
        // failed
        ShowXMessageBox(NULL, Translate(_T("Failed to start Media Server!")),Translate(_T("Error:Manager")), MB_OK|MB_ICONERROR);
    }
}



void CControllerApp::OnStarterEditionExit()
{
    // stop controller server
    g_ControllerServer().StopServer();

    //stop media server
    g_MediaServer().StopServer();
}

void CControllerApp::OnStarterEditionPrepareSetting()
{
    ClientConfigFile FileConfig;// = ClientConfigFile::GetConfigFile();
    FileConfig.Load(GetControllerDefaultDir() + CLIENT_CONFIG_FILE);
    // Server IP
    CString IP = _T("127.0.0.1");
    FileConfig.Set(CLIENT_SERVERIP, IP.GetString());

    // Server Port
    CString Port = _T("2000");
    FileConfig.Set(CLIENT_SERVERIP, IP.GetString());

    CString StarterEditionClient = _T("StarterEditionClient");
    // User Name
    CString UserNamePwd = _T("admin");
    FileConfig.Set(CLIENT_USERNAME, UserNamePwd.GetString());

    // Password
    FileConfig.Set(CLIENT_PASSWORD, UserNamePwd.GetString());

    // Client name
    FileConfig.Set(CLIENT_CLIENTNAME, StarterEditionClient.GetString());

    // Client location
    FileConfig.Set(CLIENT_LOCATION, StarterEditionClient.GetString());

    // Client company
    FileConfig.Set(CLIENT_COMPANY, StarterEditionClient.GetString());

    // Admin name
    FileConfig.Set(CLIENT_ADMINNAME, StarterEditionClient.GetString());

    // Telephone
    FileConfig.Set(CLIENT_TELEPHONE, StarterEditionClient.GetString());

    // Email
    FileConfig.Set(CLIENT_EMAIL, StarterEditionClient.GetString());

    // Local library
    FileConfig.Set(CLIENT_LOCAL_FOLDER,  (GetMediaLibraryDefaultDir() + _T("Temp\\")).c_str());

    // Player location
    CString PlayerLocation = GetAppPath() + _T("Player.exe");
    FileConfig.Set(CLIENT_PLAYER_LOCATION, PlayerLocation.GetString());

    // Schedule update time
    CString updateHour;
    updateHour.Format(_T("%d"), 0);
    FileConfig.Set(CLIENT_UPDATE_HOUR, updateHour.GetString());

    // Mode type
    CString modeType;
    modeType.Format(_T("%d"), ClientConfig::ONLINE);
    FileConfig.Set(CLIENT_MODE, modeType.GetString());

    // Free disk space alertline
    CString diskFreespaceAlertline;
    diskFreespaceAlertline.Format(_T("%d"), 0);
    FileConfig.Set(CLIENT_MIN_DISK_FREE_SIZE, diskFreespaceAlertline.GetString());

    // FPS alertline
    CString fpsAlertline;
    fpsAlertline.Format(_T("%d"), 0);
    FileConfig.Set(CLIENT_MIN_FPS, fpsAlertline.GetString());

    // hardware ID
    FileConfig.Set(CLIENT_HARDWARE_ID, StarterEditionClient.GetString());

    FileConfig.Save();

    ClientInfo clientInfo;
    clientInfo.AddAdminName(_T("StarterEditionClient"));
    clientInfo.AddClientDesc(_T("StarterEditionClient"));
    clientInfo.AddCompany(_T("StarterEditionClient"));
    clientInfo.AddEmail(_T("StarterEditionClient"));
    clientInfo.AddLocation(_T("StarterEditionClient"));
    clientInfo.AddTelPhone(_T("StarterEditionClient"));
    clientInfo.AddGroupID(S3NET_DEFAULT_CLIENTGROUP_ID);
    clientInfo.AddDisplayCardType(_T("StarterEditionClient"));
    clientInfo.AddHardwareInfo(_T("StarterEditionClient"));
    clientInfo.AddSoftwareInfo(_T("StarterEditionClient"));


    HardwareID hardwareID( _T("StarterEditionClient"));
    Account account(_T("admin"), _T("admin"));
    std::tstring serverName = IP.GetBuffer(0);
    std::tstring serverPort = Port.GetBuffer(0);

    ControllerClientInterface* pClientInterface =
        m_connManager.CreateClientInterface(
        hardwareID, account, serverName, serverPort);

    pClientInterface->SetTimeout(5000);
    pClientInterface->Restart();

    HRESULT hr = pClientInterface->RegClient(clientInfo);
    if (hr != S_OK)
    {
        ShowXMessageBox(NULL,Translate(_T("Register Client Failed!")),Translate(_T("Error:Manager")), MB_OK|MB_ICONERROR);
    }

    delete pClientInterface;
}

#endif

#ifdef PROFESSIONAL_EDITION

void CControllerApp::OnProfessionalEditionStartup()
{
#ifndef STANDALONE_EDITION
	if (FALSE == g_ControllerServer().IsHardKeyConnected())
	{
		ShowXMessageBox(NULL,Translate(_T("Please insert key then restart MagicView.")), Translate(_T("Warning:Manager")),MB_OK|MB_ICONEXCLAMATION);

		exit(1);
	}
#endif

    CString MediaServerIP = m_Config.Get(CONTROLLER_SERVER_IP).GetBuffer();
    CString Port = m_Config.Get(CONTROLLER_SERVER_PORT);
    CString ConfigFilesPath = m_Config.Get(CONFIG_PATH);
    ConfigFilesPath.Trim();
    if ((ConfigFilesPath.ReverseFind(_T('\\')) + 1) != ConfigFilesPath.GetLength())
    {
        ConfigFilesPath += _T("\\");
    }
    if (ConfigFilesPath.IsEmpty())
    {
        ConfigFilesPath = GetMediaServerDefaultDir().c_str();
    }

    ControllerServerConfig CSConfig;
    CSConfig.LoadDefaults();
    CSConfig.Load(GetControllerServerDefaultDir() + CS_CONFIG_FILE);
    CSConfig.Set(CS_MEDIA_SERVER_IP, MediaServerIP.GetString());
    CSConfig.Set(CS_PORT, Port.GetString());
    CSConfig.Save();

    CString ContrllorServerIP = m_Config.Get(CONTROLLER_SERVER_IP);
    CString  MediaServerRoot = (ConfigFilesPath + MEDIA_SERVR_ROOT_FOLDER);

    MediaServerConfig MSConfig;
    MSConfig.LoadDefaults();
    MSConfig.Load((GetMediaServerDefaultDir() + MS_CONFIG_FILE).c_str());
    MSConfig.Set(MS_CONTROLLER_SERVER_IP, ContrllorServerIP.GetString());
    MSConfig.Set(MS_HOME_DIR, MediaServerRoot.GetString());
    MSConfig.Save();

    PROCESS_INFORMATION ServerProcess = {0};

    // start controller server
    if (ProcessCall::FindProcess(CONTROLLER_SERVER_NAME) > 0)
    {
        ProcessCall::KillProcess(CONTROLLER_SERVER_NAME);
    }
#ifdef STANDALONE_EDITION
    if(!g_ControllerServer().StartServer(System::STANDALONE_VERTION))
#else
    if(!g_ControllerServer().StartServer(System::PROFESSIONAL_VERSION))
#endif
    {
        // failed
        ShowXMessageBox(NULL, Translate(_T("Failed to start Controller Server!")),Translate(_T("Error:Manager")), MB_OK|MB_ICONERROR);
    }

    //start media server
    if (ProcessCall::FindProcess(MEDIA_SERVER_NAME) > 0)
    {
        ProcessCall::KillProcess(MEDIA_SERVER_NAME);
    }

    if(!g_MediaServer().StartServer())
    {
        // failed
        ShowXMessageBox(NULL, Translate(_T("Failed to start Media Server!")),Translate(_T("Error:Manager")), MB_OK|MB_ICONERROR);
    }
}

void CControllerApp::OnProfessionalEditionExit()
{
    // stop controller server
    g_ControllerServer().StopServer();

    //stop media server
    g_MediaServer().StopServer();
}

void CControllerApp::OnProfessionalEditionPrepareSetting()
{


}

#endif

BOOL WORK_THREAD::PrepareThread()
{
    m_hThreadNotifyEvent = ::CreateEvent(0,FALSE,FALSE,0); //create thread start event
    if(m_hThreadNotifyEvent == 0)
    {
        printf("create start event failed,errno:%d\n",::GetLastError());
        goto Fail_Exit;
    }

    m_hThreadExitEvent = ::CreateEvent(0,FALSE,FALSE,0); //create thread exit event
    if(m_hThreadExitEvent == 0)
    {
        printf("create exit event failed,errno:%d\n",::GetLastError());
        goto Fail_Exit;
    }

    //start thread
    m_hThread = (HANDLE)_beginthreadex( NULL, 0, ThreadFun, NULL, 0, &m_nThreadID );
    if(m_hThread == 0)
    {
        printf("start thread failed,errno:%d\n",::GetLastError());
        CloseHandle(m_hThreadNotifyEvent);
        CloseHandle(m_hThreadExitEvent);
        goto Fail_Exit;
    }

    //wait thread start event to avoid PostThreadMessage return errno:1444
    ::WaitForSingleObject(m_hThreadNotifyEvent,INFINITE);

    return TRUE;

Fail_Exit:
    m_hThread = 0;
    m_nThreadID = -1;
    m_hThreadNotifyEvent = 0;
    m_hThreadExitEvent = 0;

    return FALSE;
}

BOOL WORK_THREAD::CleanThread()
{
    BOOL bRes = FALSE;

    if (0 == m_hThreadExitEvent)
    {
        return bRes;
    }

    if(!SetEvent(m_hThreadExitEvent)) //set thread exit event
    {
        printf("set exit event failed,errno:%d\n",::GetLastError());

        _endthreadex(m_nThreadID);

        CloseHandle(m_hThreadNotifyEvent);
        CloseHandle(m_hThreadExitEvent);
        CloseHandle(m_hThread);

        goto Exit;
    }

    //wait thread end event

    if (WAIT_TIMEOUT ==::WaitForSingleObject(m_hThreadNotifyEvent, 1000*10))
    {
        TerminateThread(m_hThreadNotifyEvent, 1);
    }

    CloseHandle(m_hThreadNotifyEvent);
    CloseHandle(m_hThreadExitEvent);
    CloseHandle(m_hThread);

    bRes = TRUE;

Exit:
    m_hThread = 0;
    m_nThreadID = -1;
    m_hThreadNotifyEvent = 0;
    m_hThreadExitEvent = 0;

    return bRes;
}

BOOL CControllerApp::PrepareWorkThread()
{
    BOOL bRes = TRUE;

    if (!m_WorkThread.PrepareThread())
    {
        bRes = FALSE;
    }

    if (!m_ConnectionObserverThread.PrepareThread())
    {
        bRes = FALSE;
    }

    return bRes;
}

BOOL CControllerApp::CleanWorkThread()
{
    BOOL bRes = TRUE;

    if (!m_WorkThread.CleanThread())
    {
        bRes = FALSE;
    }

    if (!m_ConnectionObserverThread.CleanThread())
    {
        bRes = FALSE;
    }

    return bRes;
}

std::shared_ptr<IS3RenderableObject> CControllerApp::FindObj(const CString& pluginName)
{
    for (auto it = m_plugins.begin(); it != m_plugins.end(); ++it)
    {
        if (pluginName.CompareNoCase((*it)->szPluginName) == 0)
        {
            return (*it)->pRenderableObj;
        }
    }
    return NULL;
}

std::vector<PLUGIN_INFO*> CControllerApp::GetPluginInfo()
{
    return m_plugins;
}

// privilege related
bool SimpleCheck(UserPrivilege p)
{
    return GetControllerInterface->CheckPrivilege(p);
}

bool ReturnTrue()
{
    return true;
}

bool ReturnFalse()
{
    return false;
}


bool CControllerApp::FindPluginSupportFile(const CString& name, PLUGIN_SUPPORTFILES* &f)
{
    auto it = m_PluginType.find(name);
    if (it != m_PluginType.end())
    {
        f = it->second;
        return true;
    }
    else
    {
        return false;
    }
}

bool CControllerApp::FindPluginByExt(const CString& ext, CString& pluginName)
{
    auto it = m_PluginType.begin();
    for (; it != m_PluginType.end(); ++it)
    {
        PPLUGIN_SUPPORTFILES pSupportList = it->second;
        for (DWORD i = 0; i < pSupportList->nCount; i++)
        {
            if (ext.CompareNoCase(pSupportList->pSupportFile[i].FileExt) == 0)
            {
                pluginName = it->first;
                return true;
            }
        }
    }
    return false;
}

void CControllerApp::CreatePluginObjs(std::tstring selLanguage)
{
    std::tstring addWeather;
    if (selLanguage == _T("Automatic"))
    {
        selLanguage = GetLocalLanName();
    }
    if (selLanguage == _T("CHS"))
        addWeather = _T("S3Weather");
    else
        addWeather = _T("S3Weather2");

    std::vector<std::tstring> pluginNames = m_plugMgr.GetAllPluginName();
    for (auto it = pluginNames.begin(); it != pluginNames.end(); ++it)
    {
        std::tstring plgName = it->c_str();
        if (plgName == _T("S3Weather") && plgName != addWeather)
        {
            continue;
        }
        if (plgName == _T("S3Weather2") && plgName != addWeather)
        {
            continue;
        }
        std::shared_ptr<IS3RenderableObject> pPluginObj = m_plugMgr.CreateObject(*it);
        if (pPluginObj)
        {
            PLUGIN_INFO* pPlugin = new PLUGIN_INFO;
            pPlugin->szPluginName   = it->c_str();
            pPlugin->pRenderableObj = pPluginObj;

            m_plugins.push_back(pPlugin);

            // Query the plugin support files' list
            PLUGIN_SUPPORTFILES* pSupportList = new PLUGIN_SUPPORTFILES;
            pSupportList->nCount = 0;
            pSupportList->pSupportFile = NULL;
            HRESULT hr = pPluginObj->GetSupportedFiles(&pSupportList->nCount, pSupportList->pSupportFile);
            if (SUCCEEDED(hr) && pSupportList->nCount > 0)
            {
                pSupportList->pSupportFile = new RO_SUPPORTEDFILE[pSupportList->nCount];
                hr = pPluginObj->GetSupportedFiles(&pSupportList->nCount, pSupportList->pSupportFile);
                if (SUCCEEDED(hr))
                {
                    m_PluginType.insert(std::make_pair(it->c_str(), pSupportList));
                }
            }
            else
            {
                if (pSupportList)
                {
                    delete pSupportList;
                    pSupportList = NULL;
                }
            }
        }
    }
}

std::tstring STDMETHODCALLTYPE CControllerApp::HostTranslate(const std::tstring& str)
{
    return ::Translate(str.c_str()).GetString();
}

extern UINT MAIN_APP_ALARM_INFO;
void CControllerApp::handle_timeout_app(const boost::system::error_code& ec, boost::shared_ptr<boost::asio::deadline_timer> timer)
{
	if (ec)
		return;

	HRESULT hr = m_pControlerInterface->KeepAlive();
	if (hr != S_OK)
	{
		;//HandleError(hr, _T("ControllerApp::KeepAlive"), _T("Send Heart beat failed!"));
	}

	std::tstring sql = _T("select count(*) as cnt from Alarm");
	
	int alarmRecordNum = 0;
	ADODB::_RecordsetPtr rs;
	try
	{
		if(S_OK != m_pControlerInterface->QuerySQL(sql, rs))
		{
			return;
		}
		rs->MoveFirst();
		alarmRecordNum =  rs->Fields->Item["cnt"]->Value;

	}
	catch(...)
	{
		;
	}
	
	if(alarmRecordNum != 0x00 && alarmRecordNum != m_AlarmRecordNum	)
	{
		m_AlarmRecordNum = alarmRecordNum;
		Beep(200,5000);
		m_pMainWnd->SendMessage(MAIN_APP_ALARM_INFO, NULL, NULL);
	}
	timer->expires_from_now(boost::posix_time::seconds( 60 )); //60 seconds
	timer->async_wait(boost::bind(&CControllerApp::handle_timeout_app, &theApp, boost::asio::placeholders::error, timer));
}