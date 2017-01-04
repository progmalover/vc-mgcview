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

// Controller.h : main header file for the Controller application
//
#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "Resource.h"
#include "Utilities/StringUtility.h"
#include "Utilities/ProcessCall.h"
#include "Utilities/CrashRptHelper.h"
#include "Localization/Localization.h"

#include "ControllerClient/ControllerClientDefine.h"
#include "ControllerClient/ControllerClientInterface.h"

#ifdef _MEDIALIBRARY2
#include "MediaLibrary 2/MediaLibrary.h"
#include "MediaLibrary 2/MediaClient.h"
#else
#include "MediaLibrary/MediaLibrary.h"
#include "MediaClient/MediaClient.h"
#endif


#include "ControllerServerLib/ControllerServerLib.h"
#include "MediaServerLib/MediaServerLib.h"

#include "CommonLib/PluginMgr.h"
#include "CommonUI/XMessageBox.h"

#include "ControllerSettings.h"
#include "ControllerFrame.h"
#include "LoadingTranslucentDlg.h"
#include "LayoutSettings.h"
#include "WorkThread.h"
#include <vector>
#include <map>

ControllerServerLib& g_ControllerServer();
MediaServerLib& g_MediaServer();
std::tstring GetControllerDefaultDir();
std::tstring GetMediaServerDefaultDir();
std::tstring GetControllerServerDefaultDir();
std::tstring GetMediaLibraryDefaultDir();
std::tstring GetClientDefaultDir();

// CControllerApp:
// See Controller.cpp for the implementation of this class
//
extern CWnd *GetRuntimePageCtrl(CRuntimeClass *p) ;
extern CWnd *GetCurrentPageCtrl(CRuntimeClass *p) ;
extern CWnd *SearchWndObj(CWnd *pStartWnd ,CRuntimeClass *pRuntime);

class WORK_THREAD
{
public:

    typedef unsigned (__stdcall *fun_ptr)(void *);

    WORK_THREAD(fun_ptr threadfun):
      m_hThread(0),
      m_nThreadID(-1),
      m_hThreadNotifyEvent(0),
      m_hThreadExitEvent(0),
      ThreadFun(threadfun){}

    BOOL PrepareThread();
    BOOL CleanThread();

    HANDLE              m_hThread;
    unsigned            m_nThreadID;
    HANDLE              m_hThreadNotifyEvent; // thread start event, end event
    HANDLE              m_hThreadExitEvent;

    fun_ptr             ThreadFun;
};

class IS3RenderableObject;
struct PLUGIN_INFO
{
    //CComPtr<IS3RenderableObject>    pRenderableObj;
    std::shared_ptr<IS3RenderableObject> pRenderableObj;
    CString                         szPluginName;
};

extern 	shared_ptr<MagicView::CommonUI::S3MessageBoxManager>g_msgboxMgr;
class CControllerApp : public CWinAppEx, public IS3ROHost
{
public:
    CControllerApp();
    virtual ~CControllerApp();

	int m_AlarmRecordNum;

    CArray<HWND, HWND> m_aryFrames;
    CString            m_Username;
    CString            m_Password;
    int                m_UserId;

    ConnectionManager m_connManager;
    boost::shared_ptr<ControllerClientInterface> m_pControlerInterface;

    boost::shared_ptr<MediaLibrary> m_pMediaLibraryInterface;

    ControllerConfig    m_Config;

//     User::PermissionType  m_Authority;

    BOOL                m_bControllerServerOffline;
	BOOL                m_bShowReConnectDialog;

    MagicView::CommonLib::CPluginManager2      m_plugMgr;
    virtual std::tstring STDMETHODCALLTYPE HostTranslate(const std::tstring& str);

    //CComPtr<IS3RenderableObject> FindObj(const CString& pluginName);
    std::shared_ptr<IS3RenderableObject> FindObj(const CString& pluginName);
    std::vector<PLUGIN_INFO*> GetPluginInfo();
    bool FindPluginSupportFile(const CString& name, PLUGIN_SUPPORTFILES* &f);
    bool FindPluginByExt(const CString& ext, CString& pluginName);

    //std::map<CString, IID> m_PluginObj;
    std::map<CString, PLUGIN_SUPPORTFILES*> m_PluginType;
    std::vector<PLUGIN_INFO*> m_plugins;

    BOOL                m_bNoLang;

    //////////////////////////////////////////////////////////////////////////
    // Multi Thread Message related, used for async data process
    // Post message like below, and remember release the dynamic msg in the WorkThread
    //
    ////////////////////////////////////////////////////////////////////////////
    //  //How to new a buf and post it to work thread
    //  UINT BufSize = 20;
    //  UINT nBufID;
    //  char* pInfo = WorkThreadDataManager<char>::NewDataBuf(nBufID, BufSize); //create dynamic msg
    //  if (pInfo == NULL)
    //  {
    //      break;
    //  }
    //  //Do any thing with your new buf
    //  pInfo[0] = 'S';
    //
    // if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID,USER_MSG,(WPARAM)hWnd,(LPARAM)nBufID))//post thread msg
    // {
    //     printf("post message failed,errno:%d\n",::GetLastError());
    // }
    //////////////////////////////////////////////////////////////////////////

    WORK_THREAD         m_WorkThread;
    WORK_THREAD         m_ConnectionObserverThread;
public:

    // Overrides
public:
    virtual BOOL InitInstance();

    void CreatePluginObjs(std::tstring selLanguage = _T("English"));

    virtual int ExitInstance();

    BOOL ParseCommandLine();

    BOOL IsLogin(){return !m_bControllerServerOffline;}
    BOOL Relogin();
    void OnControllerServerOffline(BOOL bLoginByOthers = FALSE);

    // Implementation
protected:
    HMENU  m_hMDIMenu;
    HACCEL m_hMDIAccel;

public:
    virtual void PreLoadState();
    virtual void LoadCustomState();
    virtual void SaveCustomState();

    afx_msg void OnAppAbout();
    afx_msg void OnUpdateReconnectRinbbonBtn(CCmdUI* pCmdUI);
    afx_msg void OnNope(UINT CmdID){}
    afx_msg void OnUpdateUINope(CCmdUI* pCmdUI){}
	afx_msg void OnHelp();
    DECLARE_MESSAGE_MAP()

private:
    CCrashRptHelper m_crashrptHelper;

    virtual void OnStartupCheckingConfig();

#ifdef STARTER_EDITION
	 virtual void OnStarterEditionStartup();
	 virtual void OnStarterEditionExit();
	 virtual void OnStarterEditionPrepareSetting();
#endif
   
    
#ifdef PROFESSIONAL_EDITION
    virtual void OnProfessionalEditionStartup();
    virtual void OnProfessionalEditionExit();
    virtual void OnProfessionalEditionPrepareSetting();
#endif

    virtual BOOL PrepareWorkThread();
    virtual BOOL CleanWorkThread();
	void handle_timeout_app(const boost::system::error_code& ec, boost::shared_ptr<boost::asio::deadline_timer> timer);

    log4cplus::ConfigureAndWatchThread m_logWatcher;
};

extern CControllerApp theApp;

#define GetControllerInterface   (((CControllerApp *)AfxGetApp())->m_pControlerInterface)

#define GetMediaLibraryInterface   (((CControllerApp *)AfxGetApp())->m_pMediaLibraryInterface)

//#define GetAuthority   (((CControllerApp *)AfxGetApp())->m_Authority)

#define GetConfig   (&(((CControllerApp *)AfxGetApp())->m_Config))

#define GetControllerFrame   static_cast<CControllerFrame*>((((CControllerApp *)AfxGetApp())->m_pMainWnd))

#define GetControllerApp ((CControllerApp *)AfxGetApp())

#define THUMBNAIL_W 64
#define THUMBNAIL_H 64

#define PROGRESS_SLICE  100.0

CString GetAppPath();

#define SETTING_FILE_NAME (_T("ControllerConfig.ini"))
#define MEDIA_SERVR_ROOT_FOLDER  (_T("Media"))

#define UPDATING  Translate(_T("Updating......"))

#ifdef _UNICODE
#define TString wstring
#else
#define TString string
#endif

#pragma comment(lib, "Version.lib ")

CString FormatSizeString(DWORD64 Size);


#define  DESIGNER_PATH  _T("DesignerRoot\\")

// privilege GUI related
bool SimpleCheck(UserPrivilege p);
bool ReturnTrue();
bool ReturnFalse();
// simple checker
//#define SC(p) std::bind<bool>(SimpleCheck, UserPrivilege_ ## p)
// always true
#define RET_TRUE() std::bind<bool>(ReturnTrue)
// always false
#define RET_FALSE() std::bind<bool>(ReturnFalse)

// combined checker, we use lambda to build the boolean express of checker
#define SC(p) SimpleCheck(UserPrivilege_ ## p)
#define LAMBDA(cond) []() -> bool {return (cond);}
#define UPDATE_COMMAND_ENTRY(id, fn, prvTest) \
    dispatchMap[id] = std::make_pair(std::bind(fn, this, std::placeholders::_1), \
                                     prvTest)
#define CP(p) GetControllerInterface->CheckPrivilege(p)

int ShowXMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption = NULL, UINT uType = MB_OK);
int ShowXMessageBoxTimeOut( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption = NULL, UINT uType = MB_OK,DWORD dwSeconds = 5);
#ifdef MessageBox
#undef MessageBox
#endif

#define MessageBox(lpText, lpCaption, uType) ShowXMessageBox(NULL, lpText, lpCaption, uType)


