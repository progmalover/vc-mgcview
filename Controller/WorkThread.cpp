
#include "stdafx.h"
#include "WorkThread.h"
#include "Controller.h"
#include "ClientStatus.h"
#include "LoadingTranslucentDlg.h"
#include "AsyncGrid.h"
#include <tuple>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////////
boost::shared_ptr<CBackgroundTask> g_backgroundTask;


//////////////////////////////////////////////////////////////////////////

char g_str[1024];

static std::vector<ManagerBase*> g_Managers;
static CMutex g_WorkThreadManagersMutex;
static UINT   g_nBufID;
static CMutex g_WorkThreadIDMutex;


void MyOutputDebugStringW(const char * str)
{
#if 0
 wostream f1(_T("e:\\me.txt"), ios::app) ;
 if(!f1) return;
 f1.write(str,size);
 f1.close();
#endif
#if 0
 FILE *fp = fopen("e:\\my.txt", "at");
 if(fp == NULL)
        return;
 fprintf(fp, str);
 fclose(fp);
#endif
}

// thread function
unsigned __stdcall WorkThreadFun(void *param)
{
    CCrashRptThreadHelper helper;
    printf("thread fun start\n");

    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    if(!SetEvent(GetControllerApp->m_WorkThread.m_hThreadNotifyEvent)) //set thread start event 
    {
        printf("set start event failed,errno:%d\n",::GetLastError());
        return 1;
    }

    while (TRUE)       
    {
        if(!::WaitForSingleObject(GetControllerApp->m_WorkThread.m_hThreadExitEvent,1))
        {
            break;
        }

        try {

        if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))        
        {
            //get msg from message queue
            switch(msg.message)
            {
            case USER_MSG_EXIT_THREAD:
                {
                    goto EXIT_THREAD;

                    break;
                }
            case USER_MSG:
                {
                    // this is a sample

                    //////////////////////////////////////////////////////////////////////////
                    // How to get a buf
                    UINT nMessageBufID = (UINT)msg.lParam;
                    HWND   hWnd  = (HWND)msg.wParam;

                    DataBuf<char> MessageBuf;
                    if (!WorkThreadDataManager<char>::GetDataBuf(nMessageBufID, MessageBuf))
                    {
                        break;
                    }
                    sprintf(g_str,"USER_MSG: %d \n",nMessageBufID);
                    MyOutputDebugStringW(g_str);
                    //////////////////////////////////////////////////////////////////////////

                    //////////////////////////////////////////////////////////////////////////
                    // do anything with your buf
                    const char * pConstInfo = MessageBuf.Get();
                    printf("recv %s\n",pConstInfo);
                    //////////////////////////////////////////////////////////////////////////

                    //////////////////////////////////////////////////////////////////////////
                    // How to new a buf
                    UINT BufSize = 20;
                    UINT nBufID;
                    char* pInfo = WorkThreadDataManager<char>::NewDataBuf(nBufID, BufSize); //create dynamic msg
                    sprintf(g_str,"USER_MSG char : %d \n",nBufID);
                    MyOutputDebugStringW(g_str);
                    if (pInfo == NULL)
                    {
                        break;
                    }
                    //////////////////////////////////////////////////////////////////////////

                    //////////////////////////////////////////////////////////////////////////
                    // Do any thing with your new buf
                    pInfo[0] = 'S';
                    //////////////////////////////////////////////////////////////////////////

                    //////////////////////////////////////////////////////////////////////////
                    // How to Post a message
                    ::PostMessage(hWnd/*AfxGetMainWnd()->m_hWnd*/,USER_MSG,(WPARAM)nBufID, NULL);// send back
                    //////////////////////////////////////////////////////////////////////////
                }
                break;
            case USER_MSG_UPDATE_CLIENT_STATUS:
                {
                    //////////////////////////////////////////////////////////////////////////
                    // How to get a buf
                    UINT nMessageBufID = (UINT)msg.lParam;
                    HWND   hWnd  = (HWND)msg.wParam;

                    //g_str.Format(_T("USER_MSG_UPDATE_CLIENT_STATUS: %d \n"),nMessageBufID);
                    sprintf(g_str,"USER_MSG_UPDATE_CLIENT_STATUS std::map<int, ClientInfo>: %d \n",nMessageBufID);
                    MyOutputDebugStringW(g_str);

                    DataBuf< std::map<int, ClientInfo> > MessageBuf;
                    if (!WorkThreadDataManager< std::map<int, ClientInfo> >::GetDataBuf(nMessageBufID, MessageBuf))
                    {
                        break;
                    }
                    //////////////////////////////////////////////////////////////////////////

                    if (!GetControllerApp->IsLogin())
                    {
                        break;
                    }

                    const  std::map<int, ClientInfo>  * pConstInfo = MessageBuf.Get();

                    //////////////////////////////////////////////////////////////////////////
                    // How to new a buf
                    UINT nBufID;
                    std::map< int, std::pair<ClientInfo, ClientRTStatistics> > * pInfo = 
                        WorkThreadDataManager< std::map< int, std::pair<ClientInfo, ClientRTStatistics> > >
                        ::NewDataBuf(nBufID); //create dynamic msg
                    //g_str.Format(_T("USER_MSG_UPDATE_CLIENT_STATUS: %d \n"),nBufID);
                     sprintf(g_str,"USER_MSG_UPDATE_CLIENT_STATUS std::map< int, std::pair<ClientInfo, ClientRTStatistics> >: %d \n",nBufID);
                    MyOutputDebugStringW(g_str);
                    if (pInfo == NULL)
                    {
                        break;
                    }
                    //////////////////////////////////////////////////////////////////////////

                    auto it = pConstInfo->begin(), ite = pConstInfo->end();
                    for (int i = 0; it != ite; ++it, ++i)
                    {
                        std::pair<ClientInfo, ClientRTStatistics> pp;
                        GetControllerInterface->GetClientRTStatistics(it->first, pp.second);
                        pp.first = it->second;
                        
                        (*pInfo)[it->first] = pp;
                    }
                    //////////////////////////////////////////////////////////////////////////

                    //////////////////////////////////////////////////////////////////////////
                    // How to Post a message
                    ::PostMessage(hWnd,USER_MSG_UPDATE_CLIENT_STATUS,(WPARAM)nBufID, NULL);// send back
                    //////////////////////////////////////////////////////////////////////////
                }
                break;
            case USER_MSG_ID_UPDATE_CLIENT_STATUS:
                {
                    //////////////////////////////////////////////////////////////////////////
                    // How to get a buf
                    UINT nMessageBufID = (UINT)msg.lParam;
                    HWND   hWnd  = (HWND)msg.wParam;

                    //g_str.Format(_T("USER_MSG_ID_UPDATE_CLIENT_STATUS: %d \n"),nMessageBufID);
                     sprintf(g_str,"USER_MSG_ID_UPDATE_CLIENT_STATUS std::vector< int > : %d \n",nMessageBufID);
                    MyOutputDebugStringW(g_str);

                    DataBuf< std::vector< int > > MessageBuf;
                    if (!WorkThreadDataManager< std::vector< int > >::GetDataBuf(nMessageBufID, MessageBuf))
                    {
                        break;
                    }
                    //////////////////////////////////////////////////////////////////////////

                    if (!GetControllerApp->IsLogin())
                    {
                        break;
                    }

                    const  std::vector< int >  * pConstInfo = MessageBuf.Get();

                    //////////////////////////////////////////////////////////////////////////
                    // How to new a buf
                    UINT nBufID;
                    std::map< int, ClientRTStatistics > * pInfo = 
                        WorkThreadDataManager< std::map< int, ClientRTStatistics> >
                        ::NewDataBuf(nBufID); //create dynamic msg
                    //g_str.Format(_T("USER_MSG_ID_UPDATE_CLIENT_STATUS: %d \n"),nBufID);
                    sprintf(g_str,"USER_MSG_ID_UPDATE_CLIENT_STATUS std::map< int, ClientRTStatistics >: %d \n",nBufID);
                    MyOutputDebugStringW(g_str);
                    if (pInfo == NULL)
                    {
                        break;
                    }
                   
                    //////////////////////////////////////////////////////////////////////////

                    auto it = pConstInfo->begin(), ite = pConstInfo->end();
                    for (int i = 0; it != ite; ++it, ++i)
                    {
                        ClientRTStatistics statistics;
                        GetControllerInterface->GetClientRTStatistics(*it, statistics);

                        (*pInfo)[*it] = statistics;
                    }
                    //////////////////////////////////////////////////////////////////////////

                    //////////////////////////////////////////////////////////////////////////
                    // How to Post a message
                    ::PostMessage(hWnd,USER_MSG_ID_UPDATE_CLIENT_STATUS,(WPARAM)nBufID, NULL);// send back
                    //////////////////////////////////////////////////////////////////////////
                }
                break;
            case USER_MSG_SERRCH_CLIENT: 
                {
                    //////////////////////////////////////////////////////////////////////////
                    // How to get a buf
                    UINT nMessageBufID = (UINT)msg.lParam;
                    HWND   hWnd  = (HWND)msg.wParam;
					// by RaymondLiu-LiuSong
                    DataBuf< Condition > MessageBuf;
                    if (!WorkThreadDataManager<Condition>::GetDataBuf(nMessageBufID, MessageBuf))
                    {
                        break;
                    }
                    //////////////////////////////////////////////////////////////////////////

                    if (!GetControllerApp->IsLogin())
                    {
                        break;
                    }

                    const  Condition  * pConstInfo = MessageBuf.Get();
					
                    //////////////////////////////////////////////////////////////////////////
                    // How to new a buf
                    UINT nBufID;
                    std::map<int, ClientInfo> * pInfo = 
                        WorkThreadDataManager< std::map<int, ClientInfo> >
                        ::NewDataBuf(nBufID); //create dynamic msg
                    //g_str.Format(_T("USER_MSG_SERRCH_CLIENT: %d \n"),nBufID);
                    sprintf(g_str,"USER_MSG_SEARCH_CLIENT std::map<int, ClientInfo>: %d \n",nBufID);
                    MyOutputDebugStringW(g_str);
                    if (pInfo == NULL)
                    {
                        break;
                    }
                    //////////////////////////////////////////////////////////////////////////

                    CWaitCursor waitdlg;

                    std::vector<int> clientIdList;
                    HRESULT hr = GetControllerInterface->SearchClient(*pConstInfo, clientIdList);
					//HRESULT hr = GetControllerInterface->SearchClient(g_condition, clientIdList);
                    if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                    {
                        ::PostMessage(GetControllerFrame->m_hWnd,USER_MSG_CONTROLLER_OFFLINE,FALSE, NULL);// send back
                        break;
                    }

                    std::vector<ClientInfo> ClientList;

                    hr = GetControllerInterface->GetClients(clientIdList, ClientList);
                    if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                    {
                        ::PostMessage(GetControllerFrame->m_hWnd,USER_MSG_CONTROLLER_OFFLINE,FALSE, NULL);// send back
                        break;
                    }
                    auto it = clientIdList.begin(), ite = clientIdList.end();
                    auto itC = ClientList.begin();

                    for (; it != ite; it++, itC++)
                    {

                        (*pInfo)[*it] = *itC;
                    }
                    //////////////////////////////////////////////////////////////////////////

                    //////////////////////////////////////////////////////////////////////////
                    // How to Post a message
                    ::PostMessage(hWnd,USER_MSG_SERRCH_CLIENT,(WPARAM)nBufID, NULL);// send back
                    //////////////////////////////////////////////////////////////////////////
                }
                break;
            case USER_MSG_UPDATE_GROUP_STATUS:
                {
                     HWND   hWnd  = (HWND)msg.wParam;
                   //////////////////////////////////////////////////////////////////////////
	                std::vector<ClientGroup> groupList;
	                HRESULT hr = GetControllerInterface->GetAllClientGroup(groupList);
	                if (!SUCCEEDED(hr))
	                {
		                if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
		                {
			                GetControllerApp->OnControllerServerOffline();
		                }
		                else
		                {
			                TRACE0("Failed to Get Client Group!\n");
		                }
		                break;
	                }

                    // How to new a buf
                    UINT nBufID;
                    std::map<int, std::pair<ClientGroup, int>> * pInfo = 
                        WorkThreadDataManager< std::map<int, std::pair<ClientGroup, int>>>
                        ::NewDataBuf(nBufID); //create dynamic msg
                    //g_str.Format(_T("USER_MSG_UPDATE_GROUP_STATUS: %d \n"),nBufID);
                    sprintf(g_str,"USER_MSG_UPDATE_GROUP_STATUS std::map<int, std::pair<ClientGroup, int>>: %d \n",nBufID);
                    MyOutputDebugStringW(g_str);
                    if (pInfo == NULL)
                    {
                        break;
                    }

                    auto itgroup = groupList.begin();
                    while(itgroup != groupList.end())
                    {
                        int GroupStatus;
                        CString condition; 
                        condition.Format(_T("GroupId = %d"), itgroup->GetGroupID());
                        Condition cond(condition.GetString());
                        std::vector<int> clientIDList;

                        HRESULT hr = GetControllerInterface->SearchClient(cond, clientIDList);
						if (!SUCCEEDED(hr))
						{
							if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
							{
								::PostMessage(GetControllerFrame->m_hWnd,USER_MSG_CONTROLLER_OFFLINE,FALSE, NULL);// send back
							}
							else
							{
								TRACE0("Failed to Search Client!\n");
							}
							break;
						}

                        if (clientIDList.size() < 1)
                        {
                            GroupStatus = GROUP_EMPTY;
                        }else
                        {
                            GroupStatus = GROUP_STATUS_WELL;
                        }

                        std::pair<ClientGroup, int> groupinfo;
                        groupinfo.first = *itgroup;
                        groupinfo.second = GroupStatus;

                        (*pInfo)[itgroup->GetGroupID()] = groupinfo;

                        itgroup++;
                    }

                    ::PostMessage(hWnd,USER_MSG_UPDATE_GROUP_STATUS,(WPARAM)nBufID, (LPARAM)NULL);// send back

                }
                break;
            case USER_MSG_GET_CLIENTS_IN_GROUP:
                {
                    CWaitDlg waitdlg;

                    //////////////////////////////////////////////////////////////////////////
                    // How to get a buf
                    INT CurGroupID = (INT)msg.lParam;
                    HWND   hWnd  = (HWND)msg.wParam;

                    if (CurGroupID >= 0)
                    {
                        // How to new a buf
                        UINT nBufID;
                        std::pair< std::pair<CString, ClientGroup >, std::pair<std::vector<int>,  
                            std::map<int, ClientInfo> > > * pInfo = 
                            WorkThreadDataManager< std::pair< std::pair<CString, ClientGroup>, 
                            std::pair<std::vector<int>,  std::map<int, ClientInfo> > > >
                            ::NewDataBuf(nBufID); //create dynamic msg
                        //g_str.Format(_T("USER_MSG_GET_CLIENTS_IN_GROUPS: %d \n"),nBufID);
                        sprintf(g_str,"USER_MSG_GET_CLIENTS_IN_GROUPS std::pair< std::pair<CString, ClientGroup >, std::pair<std::vector<int>, std::map<int, ClientInfo> > >: %d \n",nBufID);
                        MyOutputDebugStringW(g_str);
                        if (pInfo == NULL)
                        {
                            break;
                        }

                        CString condition; 
                        condition.Format(_T("GroupId = %d"), CurGroupID);

                        pInfo->first.first = condition;

                        std::vector<ClientGroup> groups;
                        HRESULT hr = GetControllerInterface->GetAllClientGroup(groups);
                        if (!SUCCEEDED(hr))
                        {
                            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                            {
                                ::PostMessage(GetControllerFrame->m_hWnd,USER_MSG_CONTROLLER_OFFLINE,FALSE, NULL);// send back
                            }
                            else
                            {
                                TRACE0("Failed to Get Client Group!\n");
                            }
                            break;
                        }
                        for (std::vector<ClientGroup>::const_iterator it = groups.begin(); it != groups.end(); ++it)
                        {
                            if ((*it).GetGroupID() == CurGroupID)
                            {
                                pInfo->first.second = (*it);
                            }
                        }


                        Condition cond(condition.GetString());
                        std::vector<int>& clientIDList = pInfo->second.first;

                        hr = GetControllerInterface->SearchClient(cond, clientIDList);
                        if (!SUCCEEDED(hr))
                        {
                            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                            {
                                ::PostMessage(GetControllerFrame->m_hWnd,USER_MSG_CONTROLLER_OFFLINE,FALSE, NULL);// send back
                            }
                            else
                            {
                                TRACE0("Failed to Search Client!\n");
                            }
                            break;
                        }

                        std::map<int, ClientInfo>& clientInfoMap = pInfo->second.second;
                        for (std::vector<int>::iterator it = clientIDList.begin(); it != clientIDList.end(); ++it)
                        {
                            ClientInfo info;
                            hr = GetControllerInterface->GetClient(*it, info);
                            if (SUCCEEDED(hr))
                            {
                                clientInfoMap[*it] = info;
                            }
                            else
                            {
                                if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                                {
                                    ::PostMessage(GetControllerFrame->m_hWnd,USER_MSG_CONTROLLER_OFFLINE,FALSE, NULL);// send back
                                }
                                else
                                {
                                    TRACE0("Failed to Get Client!\n");
                                }
                            }
                        }

                        //////////////////////////////////////////////////////////////////////////
                        // How to Post a message
                        ::PostMessage(hWnd,USER_MSG_GET_CLIENTS_IN_GROUP,(WPARAM)nBufID, NULL);// send back
                        //////////////////////////////////////////////////////////////////////////
                    }

                }
                break;
            case USER_MSG_GET_MEDIA_ROOT_CATEGORY:
                { 
                    HWND   hWnd  = (HWND)msg.wParam;

                    UINT nBufID;
                    MediaFilter * pInfo = 
                        WorkThreadDataManager< MediaFilter >
                        ::NewDataBuf(nBufID); //create dynamic msg
                    //g_str.Format(_T("USER_MSG_GET_MEDIA_ROOT_CATEGORY: %d \n"),nBufID);
                    sprintf(g_str,"USER_MSG_GET_MEDIA_ROOT_CATEGORY MediaFilter: %d \n",nBufID);
                    MyOutputDebugStringW(g_str);
                    if (pInfo == NULL)
                    {
                        break;
                    }

                    HRESULT hr = GetControllerInterface->GetRootMediaFilter( *pInfo);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE0("Failed to get root media filter\n");
                        break;
                    }

                    //////////////////////////////////////////////////////////////////////////
                    // How to Post a message
                    ::PostMessage(hWnd,USER_MSG_GET_MEDIA_ROOT_CATEGORY,(WPARAM)nBufID, NULL);// send back
                    //////////////////////////////////////////////////////////////////////////
                }
                break;
            case USER_MSG_GET_MEDIA_SUB_CATEGORY:
                {
                    HWND   hWnd  = (HWND)msg.wParam;
                    INT    CategoryID = (INT)msg.lParam;

                    UINT nBufID;
                    std::pair< INT, std::vector<MediaFilter> > * pInfo = 
                        WorkThreadDataManager< std::pair< INT, std::vector<MediaFilter> > >
                        ::NewDataBuf(nBufID); //create dynamic msg
                    sprintf(g_str,"USER_MSG_GET_MEDIA_SUB_CATEGORY std::pair< INT, std::vector<MediaFilter> >: %d \n",nBufID);
                    MyOutputDebugStringW(g_str);
                    if (pInfo == NULL)
                    {
                        break;
                    }

                    pInfo->first = CategoryID;

                    MediaFilter Category;
                    HRESULT hr = GetControllerInterface->GetMediaFilter(CategoryID, Category);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE1("Failed to get media filter %d\n", CategoryID);
                        break;
                    }

                    hr = GetControllerInterface->GetSubMediaFilters(Category, pInfo->second);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE0("Failed to get sub media filters\n");
                        break;
                    }

                    //////////////////////////////////////////////////////////////////////////
                    // How to Post a message
                    ::PostMessage(hWnd,USER_MSG_GET_MEDIA_SUB_CATEGORY,(WPARAM)nBufID, NULL);// send back
                    //////////////////////////////////////////////////////////////////////////

                }
                break;
            case USER_MSG_GET_MEDIA_SUB_CATEGORY_FILE:
                {
                    HWND   hWnd  = (HWND)msg.wParam;
                    INT    CategoryID = (INT)msg.lParam;

                    UINT nBufID;
                    pair< INT, pair< vector<MediaFilter>, vector<Media> > > * pInfo = 
                        WorkThreadDataManager< pair< INT, pair< vector<MediaFilter>, vector<Media> > > >
                        ::NewDataBuf(nBufID); //create dynamic msg
                    sprintf(g_str,"USER_MSG_GET_SUB_CATEGORY_FILE pair< INT, pair< vector<MediaFilter>, vector<Media> > >: %d \n",nBufID);
                    MyOutputDebugStringW(g_str);
                    if (pInfo == NULL)
                    {
                        break;
                    }

                    pInfo->first = CategoryID;

                    MediaFilter Category;
                    HRESULT hr = GetControllerInterface->GetMediaFilter(CategoryID, Category);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE1("Failed to get media filter %d\n", CategoryID);
                        break;
                    }

                    hr = GetControllerInterface->GetSubMediaFilters(Category, pInfo->second.first);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE0("Failed to get sub media filters\n");
                        break;
                    }

                    hr = GetControllerInterface->GetMediaInFoler(Category, pInfo->second.second);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE0("Failed to get root medias\n");
                        break;
                    }

                    //////////////////////////////////////////////////////////////////////////
                    // How to Post a message
                    ::PostMessage(hWnd,USER_MSG_GET_MEDIA_SUB_CATEGORY_FILE,(WPARAM)nBufID, NULL);// send back
                    //////////////////////////////////////////////////////////////////////////

                }
                break;
            case USER_MSG_GET_LAYOUT_ROOT_CATEGORY:
                {
                    HWND   hWnd  = (HWND)msg.wParam;
                    INT    CategoryID = (INT)msg.lParam;

                    UINT nBufID;
                    LayoutGroup* pInfo = 
                        WorkThreadDataManager< LayoutGroup >
                        ::NewDataBuf(nBufID); //create dynamic msg
                    sprintf(g_str,"USER_MSG_GET_LAYOUT_ROOT_CATEGORY LayoutGroup: %d \n",nBufID);
                    MyOutputDebugStringW(g_str);
                    if (pInfo == NULL)
                    {
                        break;
                    }

                    LayoutGroup& RootCategory = *pInfo;
                    HRESULT hr = GetControllerInterface->GetRootLayoutFilter(RootCategory);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE1("Failed to get layout filter %d\n", CategoryID);
                        break;
                    }

                    //////////////////////////////////////////////////////////////////////////
                    // How to Post a message
                    ::PostMessage(hWnd,USER_MSG_GET_LAYOUT_ROOT_CATEGORY,(WPARAM)nBufID, NULL);// send back
                    //////////////////////////////////////////////////////////////////////////
                }
                break;
            case USER_MSG_GET_LAYOUT_SUB_CATEGORY:
                {
                    HWND   hWnd  = (HWND)msg.wParam;
                    INT    CategoryID = (INT)msg.lParam;

                    UINT nBufID;
                    pair< INT,  vector<LayoutGroup> > * pInfo = 
                        WorkThreadDataManager< pair< INT,  vector<LayoutGroup> > >
                        ::NewDataBuf(nBufID); //create dynamic msg
                    sprintf(g_str,"USER_MSG_GET_LAYOUT_SUB_CATEGORY pair< INT,  vector<LayoutGroup> >: %d \n",nBufID);
                    MyOutputDebugStringW(g_str);
                    if (pInfo == NULL)
                    {
                        break;
                    }

                    LayoutGroup CategoryGroup;
                    HRESULT hr = GetControllerInterface->GetLayoutFilter(CategoryID, CategoryGroup);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE1("Failed to get layout filter %d\n", CategoryID);
                        break;
                    }


                    pInfo->first = CategoryID;

                    hr = GetControllerInterface->GetSubLayoutFilters(CategoryGroup, pInfo->second);


                    //////////////////////////////////////////////////////////////////////////
                    // How to Post a message
                    ::PostMessage(hWnd,USER_MSG_GET_LAYOUT_SUB_CATEGORY,(WPARAM)nBufID, NULL);// send back
                    //////////////////////////////////////////////////////////////////////////
                }
                break;
            case USER_MSG_GET_LAYOUT_SUB_CATEGORY_FILE:
                {
                    HWND   hWnd  = (HWND)msg.wParam;
                    INT    CategoryID = (INT)msg.lParam;

                    UINT nBufID;
                    pair< INT, pair< vector<LayoutGroup>, vector<tuple<Layout, vector<INT>, vector<INT> > > > > * pInfo = 
                        WorkThreadDataManager< pair< INT, pair< vector<LayoutGroup>, vector<tuple<Layout, vector<INT>, vector<INT> > > > > >
                        ::NewDataBuf(nBufID); //create dynamic msg
                    sprintf(g_str,"USER_MSG_GET_LAYOUT_SUB_CATEGORY_FILE pair< INT, pair< vector<LayoutGroup>, vector<tuple<Layout, vector<INT>, vector<INT> > > > > : %d \n",nBufID);
                    MyOutputDebugStringW(g_str);
                    if (pInfo == NULL)
                    {
                        break;
                    }

                    pInfo->first = CategoryID;
                    

                    LayoutGroup CategoryGroup;
                    HRESULT hr = GetControllerInterface->GetLayoutFilter(CategoryID, CategoryGroup);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE1("Failed to get layout filter %d\n", CategoryID);
                        break;
                    }


                    pInfo->first = CategoryID;

                    hr = GetControllerInterface->GetSubLayoutFilters(CategoryGroup, pInfo->second.first);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE1("Failed to get layout filter %d\n", CategoryID);
                        break;
                    }

                    vector<Layout> Layouts;
                    hr = GetControllerInterface->GetLayoutInFilter(CategoryGroup, Layouts);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE1("Failed to get layout files %d\n", CategoryID);
                        break;
                    }

                    for (auto it = Layouts.begin(); it != Layouts.end(); it++)
                    {
                        S3Time TempTime(1900,1,1,1,1,1);
                        vector<int> Groups; 
						std::vector<int> ClientGroupIDList;
						std::vector<int> refTimetableIDList;
                        hr = GetControllerInterface->CheckLayoutRef(it->GetLayoutID(), TempTime, Groups, ClientGroupIDList ,refTimetableIDList );
                        if (!SUCCEEDED(hr))
                        {
                            TRACE1("Failed to get layout ref %d\n", it->GetLayoutID());
                            break;
                        }

                        pInfo->second.second.push_back(std::make_tuple(*it, Groups, ClientGroupIDList));
                    }

                    //////////////////////////////////////////////////////////////////////////
                    // How to Post a message
                    ::PostMessage(hWnd,USER_MSG_GET_LAYOUT_SUB_CATEGORY_FILE,(WPARAM)nBufID, NULL);// send back
                    //////////////////////////////////////////////////////////////////////////
                }
                break;
            case USER_MSG_GET_TIMETABLE_ROOT_CATEGORY:
                {
                    HWND   hWnd  = (HWND)msg.wParam;
                    INT    CategoryID = (INT)msg.lParam;

                    UINT nBufID;
                    TimetableFilter* pInfo = 
                        WorkThreadDataManager< TimetableFilter >
                        ::NewDataBuf(nBufID); //create dynamic msg
                    sprintf(g_str,"USER_MSG_GET_TIMETABLE_ROOT_CATEGORY TimetableFilter: %d \n",nBufID);
                    MyOutputDebugStringW(g_str);
                    if (pInfo == NULL)
                    {
                        break;
                    }

                    TimetableFilter& RootCategory = *pInfo;
                    HRESULT hr = GetControllerInterface->GetRootTimetableFilter(RootCategory);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE1("Failed to get timetable filter %d\n", CategoryID);
                        break;
                    }

                    //////////////////////////////////////////////////////////////////////////
                    // How to Post a message
                    ::PostMessage(hWnd,USER_MSG_GET_TIMETABLE_ROOT_CATEGORY,(WPARAM)nBufID, NULL);// send back
                    //////////////////////////////////////////////////////////////////////////
                }
                break;
            case USER_MSG_GET_TIMETABLE_SUB_CATEGORY_FILE:
                {
                    HWND   hWnd  = (HWND)msg.wParam;
                    INT    CategoryID = (INT)msg.lParam;

                    UINT nBufID;
                    pair< TimetableFilter, pair< vector<TimetableFilter>, 
                        pair< vector<Timetable>, vector<Layout> > > > * pInfo = 
                        WorkThreadDataManager< pair< TimetableFilter, pair< vector<TimetableFilter>, 
                        pair< vector<Timetable>, vector<Layout> > > > >
                        ::NewDataBuf(nBufID); //create dynamic msg
                    sprintf(g_str,"USER_MSG_GET_TIMETABLE_SUB_CATEGORY_FILE pair< TimetableFilter, pair< vector<TimetableFilter>, pair< vector<Timetable>, vector<Layout> > > >: %d \n",nBufID);
                    MyOutputDebugStringW(g_str);
                    if (pInfo == NULL)
                    {
                        break;
                    }

                    TimetableFilter& Category = pInfo->first;
                    HRESULT hr = GetControllerInterface->GetTimetableFilter(CategoryID, Category);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE1("Failed to get timetable filter %d\n", CategoryID);
                        break;
                    }

                    hr = GetControllerInterface->GetSubTimetableFilters(Category, pInfo->second.first);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE1("Failed to get timetable filter %d\n", CategoryID);
                        break;
                    }

                    hr = GetControllerInterface->GetTimetablesInFilter(Category, pInfo->second.second.first);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE1("Failed to get timetable %d\n", CategoryID);
                        break;
                    }

                    const vector<Timetable>& SubItemList = pInfo->second.second.first;
                    vector<Layout>& LayoutList = pInfo->second.second.second;
                    std::map<INT, INT> LayoutIDMap;

                    for (auto it = SubItemList.begin(); it != SubItemList.end(); it++)
                    {
                        for (auto it2 = it->LayoutEntryList.begin(); it2 != it->LayoutEntryList.end(); it2++)
                        {
                            auto itTest = LayoutIDMap.find(it2->GetLayoutID());
                            if (itTest == LayoutIDMap.end())
                            {
                                Layout layout;
                                hr = GetControllerInterface->GetLayout(it2->GetLayoutID(), layout);
                                if (!SUCCEEDED(hr))
                                {
                                    TRACE1("Failed to get layout files %d\n", CategoryID);
                                    break;
                                }

                                LayoutList.push_back(layout);

                                LayoutIDMap[it2->GetLayoutID()] = 1;
                            }
                        }
                    }

                    //////////////////////////////////////////////////////////////////////////
                    // How to Post a message
                    ::PostMessage(hWnd,USER_MSG_GET_TIMETABLE_SUB_CATEGORY_FILE,(WPARAM)nBufID, NULL);// send back
                    //////////////////////////////////////////////////////////////////////////
                }
                break;
                
            case USER_MSG_SEARCH_MEDIA_LIST:
            {
                //////////////////////////////////////////////////////////////////////////
                // How to get a buf
                UINT nMessageBufID = (UINT)msg.lParam;
                HWND   hWnd  = (HWND)msg.wParam;

                sprintf(g_str,"USER_MSG_SEARCH_MEDIA_LIST std::map<int, CString> : %d \n",nMessageBufID);
                MyOutputDebugStringW(g_str);

                DataBuf< std::map<int, CString> > MessageBuf;
                if (!WorkThreadDataManager< std::map<int, CString> >::GetDataBuf(nMessageBufID, MessageBuf))
                {
                    break;
                }
                //////////////////////////////////////////////////////////////////////////

                if (!GetControllerApp->IsLogin())
                {
                    break;
                }

                const  std::map<int, CString>  * pConstInfo = MessageBuf.Get();
                CString conds;
                if (pConstInfo->begin()->first != -1)
                {
                    //////////////////////////////////////////////////////////////////////////
                    CString condition;
                    TCHAR buf[16];
                    CString strID;
                    vector<int> FilterIDList;
                    FilterIDList.push_back(pConstInfo->begin()->first);
                    while (!FilterIDList.empty())
                    {
                        int ID = FilterIDList.back();
                        FilterIDList.pop_back();
                        MediaFilter parent;
                        vector<MediaFilter> subMediaFilter;
                        HRESULT hr = GetControllerInterface->GetMediaFilter(ID, parent);
                        if (!SUCCEEDED(hr))
                        {
                            TRACE0("Failed to Get MediaFilter\n");
                            break;
                        }
                        hr = GetControllerInterface->GetSubMediaFilters(parent, subMediaFilter);
                        if (!SUCCEEDED(hr))
                        {
                            TRACE0("Failed to Get SubMediaFilter\n");
                            break;
                        }
                        while(!subMediaFilter.empty())
                        {
                            MediaFilter tmp = subMediaFilter.back();
                            subMediaFilter.pop_back();
                            int subID = tmp.GetMediaFilterID();
                            FilterIDList.push_back(subID);
                            _itot_s(subID, buf, 10);
                            strID = buf;
                            condition += _T(" or MediaFilterId = ") + strID;    
                        }
                    }
                    condition += _T(")");
                    conds = pConstInfo->begin()->second + condition;
                }
                else
                {
                   conds = pConstInfo->begin()->second;
                }
                Condition cond(conds.GetString());
                //////////////////////////////////////////////////////////////////////////
               UINT nBufID;
               pair< vector<Media>, MEDIA_DETAIL_INFO > * pInfo = 
                    WorkThreadDataManager<  pair<vector<Media>, MEDIA_DETAIL_INFO > >
                    ::NewDataBuf(nBufID); //create dynamic msg
               sprintf(g_str,"USER_MSG_SEARCH_MEDIA_LIST pair< vector<Media>, MEDIA_DETAIL_INFO > : %d \n",nBufID);
               MyOutputDebugStringW(g_str);
                if (pInfo == NULL)
                {
                    break;
                }

                CWaitDlg waitdlg;

                vector<INT> MediaIDs;
                HRESULT hr = GetControllerInterface->SearchMedia(cond, MediaIDs);//get mediaIDList
                if (!SUCCEEDED(hr))
                {
                    TRACE0("Failed to Search Media\n");
                    break;
                }

                vector<Media>& Medias = pInfo->first;
                hr = GetControllerInterface->GetMedias(MediaIDs, Medias);
                if (!SUCCEEDED(hr))
                {
                    TRACE0("Failed to Get Media Infos\n");
                    break;
                }

                MEDIA_DETAIL_INFO& mediaInfo = pInfo->second;
                CString syncStatus =_T("");
                
                MEDIA_LIST mediaTask;
                
                mediaTask.m_MediaIDs = MediaIDs;

                hr = GetMediaLibraryInterface->GetMedia(mediaTask, mediaInfo);
                if (!SUCCEEDED(hr))
                {
                    TRACE0("Failed to get media!\n");
                    break;
                }
                //////////////////////////////////////////////////////////////////////////
                // How to Post a message
                ::PostMessage(hWnd,USER_MSG_SERVER_MEDIALIST,(WPARAM)nBufID, MEDIA_SEARCH_LIST);// send back
                //////////////////////////////////////////////////////////////////////////
                }
                break;

			case USER_MSG_UPDATE_MEDIA_CATEGORY_LIST:
            case USER_MSG_MEDIA_CATEGORY_LIST:
                {
                    HWND   hWnd  = (HWND)msg.wParam;
                    INT    CategoryID = (INT)msg.lParam;

                    UINT nBufID;
                    pair< vector<Media>, MEDIA_DETAIL_INFO> * pInfo =
                        WorkThreadDataManager<pair< vector<Media>, MEDIA_DETAIL_INFO > >
                        ::NewDataBuf(nBufID);// INT = categoryID, vector<int> = mediaIDlist, vector<int> = layout, vector<Media> = mediaInfo

                    sprintf(g_str,"USER_MSG_UPDATE_MEDIA_CATEGORY_LIST&USER_MSG_MEDIA_CATEGORY_LIST  pair< vector<Media>, MEDIA_DETAIL_INFO>: %d \n",nBufID);
                    MyOutputDebugStringW(g_str);
                    if (pInfo == NULL)
                    {
                        break;
                    }

					std::vector<Media>& mediaList = pInfo->first;
					MediaFilter Category;
					HRESULT hr;

					std::auto_ptr<CWaitDlg> waitdlg;
					if(USER_MSG_MEDIA_CATEGORY_LIST == msg.message)
					{
						waitdlg.reset(new CWaitDlg);
					}

					hr = GetControllerInterface->GetMediaFilter(CategoryID, Category);
					if (!SUCCEEDED(hr))
					{
						TRACE0("Failed to get media filter\n");
					}
					hr = GetControllerInterface->GetMediaInFoler(Category, mediaList);
					if (!SUCCEEDED(hr))
					{
						TRACE0("Failed to get medialist by GetMediaInFolder!\n");
					}

                    MEDIA_DETAIL_INFO& mediaInfo = pInfo->second;
                    CString syncStatus =_T(""); 

                    MEDIA_LIST mediaTask;

                    for (auto it = mediaList.begin(); it != mediaList.end(); it++)
                    {
                        mediaTask.AddMedia(it->GetMeidaID());
                    }

                    hr = GetMediaLibraryInterface->GetMedia(mediaTask, mediaInfo);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE0("Failed to get media!\n");
                        break;
                    }
                    //////////////////////////////////////////////////////////////////////////
                    // How to Post a message
					if(USER_MSG_MEDIA_CATEGORY_LIST == msg.message)
						::PostMessage(hWnd,USER_MSG_SERVER_MEDIALIST,(WPARAM)nBufID, MEDIA_CATEGORY_LIST);// send back
					else 
						::SendMessage(hWnd,USER_MSG_UPDATE_SERVER_MEDIALIST,(WPARAM)nBufID, MEDIA_CATEGORY_LIST);// send back

                    //////////////////////////////////////////////////////////////////////////
                }
                break; 
            case USER_MSG_MEDIALIBRARY_GET_MEDIALIST:
                {
                    //////////////////////////////////////////////////////////////////////////
                    // How to get a buf
                    UINT nMessageBufID = (UINT)msg.lParam;
                    HWND   hWnd  = (HWND)msg.wParam;

                    sprintf(g_str,"USER_MSG_MEDIALIBRARY_GET_MEDIALIST pair<DWORD, CString>: %d \n",nMessageBufID);
                    MyOutputDebugStringW(g_str);
                    DataBuf< pair<DWORD, CString> > MessageBuf;
                    if (!WorkThreadDataManager<pair<DWORD, CString>>::GetDataBuf(nMessageBufID, MessageBuf))
                    {
                        break;
                    }

                    //////////////////////////////////////////////////////////////////////////

                    DWORD mediaType = (DWORD)MessageBuf.Get()->first;
                    UINT nBufID;

                   pair<CString, MEDIA_BASIC_INFO> * pInfo = 
                        WorkThreadDataManager< pair<CString, MEDIA_BASIC_INFO> > 
                        ::NewDataBuf(nBufID); //create dynamic msg
                   sprintf(g_str,"USER_MSG_MEDIALIBRARY_GET_MEDIALIST pair<CString, MEDIA_BASIC_INFO>: %d \n",nBufID);
                   MyOutputDebugStringW(g_str);
                    if (pInfo == NULL)
                    {
                        break;
                    }
                            
                    pInfo->first = MessageBuf.Get()->second;

                    MEDIA_BASIC_INFO& mediaBaseInfo = pInfo->second;
                    HRESULT hr = GetMediaLibraryInterface->GetMediaList(mediaType, mediaBaseInfo); 
                    if (!SUCCEEDED(hr))
                    {
                        TRACE0("Failed to GetMediaList!\n"); 
                        break;
                    }

                    auto it = mediaBaseInfo.m_Medias.begin(), ite = mediaBaseInfo.m_Medias.end();

                    //////////////////////////////////////////////////////////////////////////
                    // How to Post a message
                    ::PostMessage(hWnd,USER_MSG_MEDIALIBRARY_GET_MEDIALIST,(WPARAM)nBufID, NULL);// send back
                    //////////////////////////////////////////////////////////////////////////
                }
                break;
			case USER_MSG_GET_CLIENT_HISTORY://add by LiuSong
				{
					CWaitDlg waitdlg;

					HWND   hWnd  = (HWND)msg.wParam;
					ST_CLIENT_RT_STA_PARA *pCond = (ST_CLIENT_RT_STA_PARA *)msg.lParam;

					UINT nBufID;
					ClientStatus *pClientStatus = WorkThreadDataManager< ClientStatus >::NewDataBuf(nBufID); //create dynamic msg
                    sprintf(g_str,"USER_MSG_GET_CLIENT_HISTORY ClientStatus: %d \n",nBufID);
                    MyOutputDebugStringW(g_str);
					if (pClientStatus == NULL)
					{
						break;
					}

					const S3Time starttime = S3Time::CreateTime(pCond->starttime);
					const S3Time endtime = S3Time::CreateTime(pCond->endtime);
					HRESULT hr =GetControllerInterface->GetClientStatus(pCond->clientid, starttime,endtime,*pClientStatus);
					if (!SUCCEEDED(hr))
					{
						TRACE0("Failed to get client history \n");
						break;
					}

					delete pCond;
					
					/*
					HWND   hWnd  = (HWND)msg.wParam;
					ST_CLIENT_RT_STA_PARA *pCond = (ST_CLIENT_RT_STA_PARA *)msg.lParam;

					UINT nBufID;
					std::vector<ClientRTStatistics> *pInfo = WorkThreadDataManager< std::vector<ClientRTStatistics> >::NewDataBuf(nBufID); //create dynamic msg

					if (pInfo == NULL)
					{
					break;
					}

					HRESULT hr =GetControllerInterface->GetClientRTStatisticss(pCond->clientid, starttime,endtime,*pInfo);
					if (!SUCCEEDED(hr))
					{
						TRACE0("Failed to get client history \n");
						break;
					}

					delete pCond;
					*/

					//////////////////////////////////////////////////////////////////////////
					// How to Post a message
					::PostMessage(hWnd,USER_MSG_GET_CLIENT_HISTORY,(WPARAM)nBufID, NULL);// send back
					//////////////////////////////////////////////////////////////////////////

				}
				break;
            case USER_MSG_UPLOAD_MEDIA:
                {
                    //////////////////////////////////////////////////////////////////////////
                    // How to get a buf
                    UINT nMessageBufID = (UINT)msg.lParam;
                    HWND   hWnd  = (HWND)msg.wParam;

                    sprintf(g_str,"USER_MSG_UPLOAD_MEDIA pair<INT, vector<CString>: %d \n",nMessageBufID);
                    MyOutputDebugStringW(g_str);
                    DataBuf< pair<INT, vector<CString>> > MessageBuf;
                    if (!WorkThreadDataManager<pair<INT, vector<CString>>>::GetDataBuf(nMessageBufID, MessageBuf))
                    {
                        break;
                    }
                    //////////////////////////////////////////////////////////////////////////

                    UINT nBufID;

                    pair<BOOL, CString> * pInfo = 
                        WorkThreadDataManager<  pair<BOOL, CString> > 
                        ::NewDataBuf(nBufID); //create dynamic msg
                    sprintf(g_str,"USER_MSG_UPLOAD_MEDIA pair<BOOL, CString>: %d \n",nBufID);
                    MyOutputDebugStringW(g_str);
                    if (pInfo == NULL)
                    {
                        break;
                    }

                    INT  CategoryID = MessageBuf.Get()->first;
                    const vector<CString>& LocalFiles = MessageBuf.Get()->second;

                    MediaFilter category;

                    pInfo->first = TRUE;

                    CWaitDlg waitdlg;

                    HRESULT hr = GetControllerInterface->GetMediaFilter(CategoryID, category);
                    if (!SUCCEEDED(hr))
                    {
                        pInfo->first = FALSE;
                        TRACE0("Failed to get media filter!\n");
                    }
                    else
                    {
                        CString& InfoMsg = pInfo->second;

                        CString SucceededFiles, FailedFiles;
                        MEDIA_UPLOAD_TASK uploadTask;
                        MEDIA_DETAIL_INFO uploadTaskInfo;

                        //
                        //Add file to medialibrary -zxy
                        //
                        for (auto it = LocalFiles.begin(); it != LocalFiles.end(); it++)
                        {  
                            uploadTask.AddMedia(it->GetString(), category);
                        }

                        hr = GetMediaLibraryInterface->UploadMedia(uploadTask, uploadTaskInfo);
                     
                        
                    }

                    //////////////////////////////////////////////////////////////////////////
                    // How to Post a message
                    ::SendMessage(hWnd,USER_MSG_UPLOAD_MEDIA,(WPARAM)nBufID, NULL);// send back
                    //////////////////////////////////////////////////////////////////////////
                }
                break;
            case USER_MSG_SEARCH_LAYOUT_LIST:
            {
                //////////////////////////////////////////////////////////////////////////
                // How to get a buf
                UINT nMessageBufID = (UINT)msg.lParam;
                HWND   hWnd  = (HWND)msg.wParam;

                sprintf(g_str,"USER_MSG_SEARCH_LAYOUT_LIST std::map<int, CString> : %d \n",nMessageBufID);
                MyOutputDebugStringW(g_str);
                DataBuf< std::map<int, CString> > MessageBuf;
                if (!WorkThreadDataManager<std::map<int, CString>>::GetDataBuf(nMessageBufID, MessageBuf))
                {
                    break;
                }
                //////////////////////////////////////////////////////////////////////////

                if (!GetControllerApp->IsLogin())
                {
                    break;
                }

                const  std::map<int, CString>  * pConstInfo = MessageBuf.Get();
                CString conds;
                if (pConstInfo->begin()->first != -1)
                {
                    CString conditons;
                    TCHAR buf[16];
                    CString strID;
                    vector<int> LayoutFilterIDlist;
                    LayoutFilterIDlist.push_back(pConstInfo->begin()->first);
                    while(!LayoutFilterIDlist.empty())
                    {
                        int ID = LayoutFilterIDlist.back();
                        LayoutFilterIDlist.pop_back();
                        LayoutGroup parent;
                        vector<LayoutGroup> subLayoutFilter;
                        HRESULT hr = GetControllerInterface->GetLayoutFilter(ID, parent);
                        if (!SUCCEEDED(hr))
                        {
                            TRACE0("Failed to Get LayoutFilter\n");
                            break;
                        }
                        hr = GetControllerInterface->GetSubLayoutFilters(parent, subLayoutFilter);
                        if (!SUCCEEDED(hr))
                        {
                            TRACE0("Failed to Get SubLayoutFilter\n");
                            break;
                        }
                        while(!subLayoutFilter.empty())
                        {
                            LayoutGroup tmp = subLayoutFilter.back();
                            subLayoutFilter.pop_back();
                            int subID = tmp.GetGroupID();
                            LayoutFilterIDlist.push_back(subID);
                            _itot_s(subID, buf, 10);
                            strID = buf;
                            conditons += _T(" or LayoutFilterId = ") + strID;
                        }
                    }
                    conditons += _T(")");
                    conds = pConstInfo->begin()->second + conditons;
                }
                else
                {
                    conds = pConstInfo->begin()->second;
                }
                
                Condition cond(conds.GetString());
                //////////////////////////////////////////////////////////////////////////

               UINT nBufID;
               vector<pair<Layout, INT > > * pInfo = 
                    WorkThreadDataManager<  vector<pair<Layout, INT > > >
                    ::NewDataBuf(nBufID); //create dynamic msg
               sprintf(g_str,"USER_MSG_SEARCH_LAYOUT_LIST vector<pair<Layout, vector<INT> > >: %d \n",nBufID);
               MyOutputDebugStringW(g_str);
                if (pInfo == NULL)
                {
                    break;
                }

                CWaitDlg waitdlg;

                vector<INT> LayoutIDs;
                HRESULT hr = GetControllerInterface->SearchLayout(cond, LayoutIDs);//get mediaIDList
                if (!SUCCEEDED(hr))
                {
                    TRACE0("Failed to Search Media\n");
                    break;
                }

                vector<Layout> Layouts;
                hr = GetControllerInterface->GetLayouts(LayoutIDs, Layouts);
                if (!SUCCEEDED(hr))
                {
                    TRACE0("Failed to Get Media Infos\n");
                    break;
                }

                for (auto it = Layouts.begin(); it != Layouts.end(); it++)
                {
                    S3Time TempTime(1900,1,1,1,1,1);
                    vector<int> Groups; 
					std::vector<int> ClientGroupIDList;
					std::vector<int> refTimetableIDList;
                    hr = GetControllerInterface->CheckLayoutRef(it->GetLayoutID(), TempTime, Groups, ClientGroupIDList , refTimetableIDList);
                    if (!SUCCEEDED(hr))
                    {
                        TRACE1("Failed to get layout ref %d\n", it->GetLayoutID());
                        break;
                    }

                    pInfo->push_back(make_pair(*it, Groups.size() + ClientGroupIDList.size() + refTimetableIDList.size() ));
                }


                //////////////////////////////////////////////////////////////////////////
                // How to Post a message
                ::PostMessage(hWnd,USER_MSG_SEARCH_LAYOUT_LIST,(WPARAM)nBufID, 0);// send back
                //////////////////////////////////////////////////////////////////////////
                }
                break;
            default:
                {
                    
                }
                break;
            }    
        }
        }
        catch(NetworkException&)
        {
            Sleep(50);
        }
        
    };

EXIT_THREAD:

    if(!SetEvent(GetControllerApp->m_WorkThread.m_hThreadNotifyEvent)) //set thread end event 
    {
        printf("set end event failed,errno:%d\n",::GetLastError());
        return 1;
    }

    return 0;
}



// thread function
unsigned __stdcall ObserverThreadFun(void *param)
{
    CCrashRptThreadHelper helper;
    printf("Observer thread fun start\n");

    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    if(!SetEvent(GetControllerApp->m_ConnectionObserverThread.m_hThreadNotifyEvent)) //set thread start event 
    {
        printf("set Observer start event failed,errno:%d\n",::GetLastError());
        return 1;
    }

    while (TRUE)       
    {
        try {
        if(!::WaitForSingleObject(GetControllerApp->m_ConnectionObserverThread.m_hThreadExitEvent,2000))
        {
            break;
        }

        if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))        
        {
            //get msg from message queue
            switch(msg.message)
            {
            case USER_MSG_EXIT_THREAD:
                {
                    goto EXIT_THREAD;

                    break;
                }
            case USER_MSG:
                {
                    // this is a sample

                    //////////////////////////////////////////////////////////////////////////
                    // How to get a buf
                    UINT nMessageBufID = (UINT)msg.lParam;
                    HWND   hWnd  = (HWND)msg.wParam;

                    DataBuf<char> MessageBuf;
                    if (!WorkThreadDataManager<char>::GetDataBuf(nMessageBufID, MessageBuf))
                    {
                        break;
                    }
                    //////////////////////////////////////////////////////////////////////////

                    //////////////////////////////////////////////////////////////////////////
                    // do anything with your buf
                    const char * pConstInfo = MessageBuf.Get();
                    printf("recv %s\n",pConstInfo);
                    //////////////////////////////////////////////////////////////////////////

                    //////////////////////////////////////////////////////////////////////////
                    // How to new a buf
                    UINT BufSize = 20;
                    UINT nBufID;
                    char* pInfo = WorkThreadDataManager<char>::NewDataBuf(nBufID, BufSize); //create dynamic msg
                    if (pInfo == NULL)
                    {
                        break;
                    }
                    //////////////////////////////////////////////////////////////////////////

                    //////////////////////////////////////////////////////////////////////////
                    // Do any thing with your new buf
                    pInfo[0] = 'S';
                    //////////////////////////////////////////////////////////////////////////

                    //////////////////////////////////////////////////////////////////////////
                    // How to Post a message
                    ::PostMessage(hWnd/*AfxGetMainWnd()->m_hWnd*/,USER_MSG,(WPARAM)nBufID, NULL);// send back
                    //////////////////////////////////////////////////////////////////////////
                }
                break;
            default:
                {
                    
                }
                break;
            }        
        }
        CControllerFrame *pControllerFrame = GetControllerFrame;
        if (GetControllerApp->IsLogin() && pControllerFrame)
        {
            //if (GetControllerInterface->IsconnectionDrop())
            //{
            //    ::PostMessage(pControllerFrame->m_hWnd,USER_MSG_CONTROLLER_OFFLINE,FALSE, NULL);// send back
            //}
            //else
            {
                bool isLogin = false;
                HRESULT hr = GetControllerInterface->CheckLoginStatus(isLogin);
                if (!SUCCEEDED(hr))
                {
                    if (HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID) == hr)
                    {
                        ::PostMessage(pControllerFrame->m_hWnd,USER_MSG_CONTROLLER_OFFLINE,FALSE, NULL);// send back
                    }
                    else
                    {
                        if (!isLogin)
                        {
                            ::PostMessage(pControllerFrame->m_hWnd,USER_MSG_CONTROLLER_OFFLINE,TRUE, NULL);// send back
                        }
                    }
                }
            }
        }
        } catch (NetworkException&)
        {
            ::PostMessage(GetControllerFrame->m_hWnd,USER_MSG_CONTROLLER_OFFLINE,FALSE, NULL);// send back
            Sleep(50);
        }
    };

EXIT_THREAD:

    if(!SetEvent(GetControllerApp->m_ConnectionObserverThread.m_hThreadNotifyEvent)) //set thread end event 
    {
        printf("set Observer end event failed,errno:%d\n",::GetLastError());
        return 1;
    }

    return 0;
}



ManagerBase::ManagerBase()
{
	g_WorkThreadManagersMutex.Lock();

	g_Managers.push_back(this);

	g_WorkThreadManagersMutex.Unlock();
}
 ManagerBase::~ManagerBase()
{
	g_WorkThreadManagersMutex.Lock();

	for (std::vector<ManagerBase*>::iterator it = g_Managers.begin(); it != g_Managers.end(); it++)
	{
		if (*it == this)
		{
			g_Managers.erase(it);
			break;
		}
	}

	g_WorkThreadManagersMutex.Unlock();
}

 void ManagerBase::CollectGarbage(DWORD Tick)
{

}

 void ManagerBase::DoGarbage(DWORD Tick)
{
	g_WorkThreadManagersMutex.Lock();

	for (std::vector<ManagerBase*>::iterator it = g_Managers.begin(); it != g_Managers.end(); it++)
	{
		(*it)->CollectGarbage(Tick);
	}

	g_WorkThreadManagersMutex.Unlock();
}

UINT ManagerBase::GetNewID()
{
	g_WorkThreadIDMutex.Lock();
	UINT newID = g_nBufID++;
	g_WorkThreadIDMutex.Unlock();
	return newID;
}
