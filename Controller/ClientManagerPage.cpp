// F:\p4\sw\Demo\ContentManagement\Controller\Client Manager\ClientManagement.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ClientManagerPage.h"
#include "ClientManagerAddGroupDlg.h"
#include "EmergencyMessageDlg.h"
#include "LayoutSettings.h"
#include "Localization/Localization.h"
#include "GetStartIntervalDlg.h"
#include "Utilities/SysCall.h"
#include "ClientStatePane.h"

#include "ExportFilesDlg.h"
#include "SearchClientDlg.h"

#define CLIENT_FILTER_HEIGHT 100

// wParam not used, lParam - new schedule Group ID
UINT SCHEDULER_ADD_EVENT			= ::RegisterWindowMessage (_T("SCHEDULER_ADD_EVENT"));

CString CLIENT_MANAGER_CLIP_FMT_NAME = _T("S3SignageManagerClientManagerClpFmt");
CLIPFORMAT CLIENT_MANAGER_CLIP_FMT = ::RegisterClipboardFormat (_T("S3SignageManagerClientManagerClpFmt"));

// CClientManagerPage

IMPLEMENT_DYNAMIC(CClientManagerPage, CBaseControllerPage)

CClientManagerPage::CClientManagerPage()
: m_DefaultGroupID (0)
{

}

CClientManagerPage::~CClientManagerPage()
{

}


BEGIN_MESSAGE_MAP(CClientManagerPage, CBaseControllerPage)
    ON_WM_CREATE()
    //ON_WM_TIMER()
    ON_WM_DESTROY()


    ON_COMMAND(ID_RIBBON_BTN_CLTMNT_REFRESH, OnRibbonBtn_Refresh)
    ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_EDITSCHEDULE, OnRibbonBtn_EditSchedule)
    ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_OFFLINESCHEDULE, OnRibbonBtn_EditSchedule_Offline)
    ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_ADDGROUP, OnRibbonBtn_AddGroup)
    ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_DELGROUP, OnRibbonBtn_DeleteGroup)
    ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_EDITGROUP, OnRibbonBtn_EditGroup)

    ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_DELCLT, OnRibbonBtn_DeleteClient)
    ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_EDITCLT, OnRibbonBtn_EditClient)

	ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_RESTARTCLT, OnRibbonBtn_RestartClient)
	ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_SHUTDOWNCLT, OnRibbonBtn_ShutDownClient)
	ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_STARTCLT, OnRibbonBtn_StartClient)

	ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_VIEWHISTORYCLT, OnRibbonBtn_ViewHistoryStatus)
 	ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_UPDATECLT, OnRibbonBtn_UpdateClient)
    ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_SEARCH, OnRibbonBtn_SearchClient)

    ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_SAVE, OnRibbonBtn_Save)

    ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_EDITMSG, OnRibbonBtn_EditEmergencyMessage)


    ON_REGISTERED_MESSAGE(CLIENTMANAGER_GROUPINFO_SAVE, OnGroupEdit)
    ON_REGISTERED_MESSAGE(CLIENTMANAGER_GROUP_CLIENT_GROUP_CHANGE, OnClientEdit)
    ON_REGISTERED_MESSAGE(CLIENTMANAGER_GROUPINFO_CLIENT_GROUP_CHANGE, OnClientEdit)

    ON_REGISTERED_MESSAGE(CLIENTMANAGER_CLIENT_SEL_CHANGE, OnClientSelChanged)
    ON_REGISTERED_MESSAGE(CLIENTDLG_SEARCH, OnClientDlgSearch)


    ON_MESSAGE(USER_MSG_SERRCH_CLIENT, UpdateSearchClientList)
    ON_MESSAGE(USER_MSG_UPDATE_CLIENT_STATUS, UpdateSearchClientListStatus)

    ON_NOTIFY(NM_CLICK, IDW_LIST_GROUP ,OnGroupSelChange2)
    ON_NOTIFY(LVN_ITEMCHANGED, IDW_LIST_CLIENT_STATUS ,OnClientSelChange)
#if 0
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_DELCLT, OnUpdateClientRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_RESTARTCLT, OnUpdateClientRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_SHUTDOWNCLT, OnUpdateClientRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_STARTCLT, OnUpdateClientRibbonBtn)

    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_EDITGROUP, OnUpdateGroupSingleSelectRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_EDITCLT, OnUpdateClientRibbonBtn)

    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_DELGROUP, OnUpdateGroupRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_EDITMSG, OnUpdateGroupSingleSelectRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_EDITSCHEDULE, OnUpdateGroupSingleSelectRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_SAVE, OnUpdateSaveRibbonBtn)

    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_ADDGROUP, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_OFFLINESCHEDULE, OnUpdateRibbonBtn)
#else
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_DELCLT, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_RESTARTCLT, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_SHUTDOWNCLT, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_STARTCLT, OnUpdateRibbonBtn)

	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_VIEWHISTORYCLT, OnUpdateRibbonBtn)

	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_EDITGROUP, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_EDITCLT, OnUpdateRibbonBtn)

	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_DELGROUP, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_EDITMSG, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_EDITSCHEDULE, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_SAVE, OnUpdateRibbonBtn)

	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_ADDGROUP, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_OFFLINESCHEDULE, OnUpdateRibbonBtn)

	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_UPDATECLT, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMNT_REFRESH, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_SEARCH, OnUpdateRibbonBtn)

#endif
END_MESSAGE_MAP()


/// CClientManagerPage Message handle
int CClientManagerPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (!CreateRibbonButtons())
    {
        TRACE0("Failed to create client management ribbon buttons\n");
        return -1;      // fail to create
    }

    if (CBaseControllerPage::OnCreate(lpCreateStruct) == -1)
        return -1;

        // enable Visual Studio 2005 style docking window behavior
    CDockingManager::SetDockingMode(DT_SMART);
    // enable Visual Studio 2005 style docking window auto-hide behavior
    EnableAutoHidePanes(CBRS_ALIGN_ANY);


    CRect rect;
    GetClientRect(&rect);

    int left    = 0;
    int right   = 0;
    int top     = 0;
    int bottom  = 0;

    // Create and Install the group list pane
    right   = rect.Width() / 4;
    bottom  = rect.Height();

    if (!m_wndGroupList.Create(Translate(_T("Group List")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_GROUPLIST, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_LEFT | CBRS_LEFT | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create group list pane\n");
        return -1;      // fail to create
    }
    m_wndGroupList.EnableDocking(CBRS_LEFT);
    DockPane(&m_wndGroupList);


    left    = rect.Width() / 4;
    right   = rect.right;
    top     = 0;
    bottom  = rect.Height();

    if (!m_wndClientState.Create(Translate(_T("Client State View")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_CLIENTSTATELIST, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create client state pane\n");
        return -1;      // fail to create
    }

    SetMainWnd(&m_wndClientState);


    // Create and Install the Media Info pane
    left    = rect.Width() / 4;
    top     = rect.bottom - CLIENT_FILTER_HEIGHT;
    bottom  = rect.bottom;
    right   = rect.right;

    if (!m_wndClientInfo.Create(Translate(_T("Client Info")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_MEDIALIBRARY_INFO, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create Client info pane\n");
        return FALSE;      // fail to create
    }
    m_wndClientInfo.EnableDocking(CBRS_BOTTOM);
    DockPane(&m_wndClientInfo);

    InitPage();

    return 0;
}

BOOL CClientManagerPage::InitPage()
{

    //Refresh();

    //m_nTimer = SetTimer(0xFF000001, StringUtility::stoi(GetConfig->Get(CLIENT_MONITOR_REFRESH).GetString()) * 1000, 0);

    CImage* pTempImage = new CImage;
    pTempImage->LoadFromResource(AfxGetInstanceHandle(), IDB_CLIENT_STATUS_WELL);
    m_ClientStatusImage[CLIENT_WELL] = pTempImage;
    pTempImage = new CImage;
    pTempImage->LoadFromResource(AfxGetInstanceHandle(), IDB_CLIENT_CRITICAL);
    m_ClientStatusImage[CLIENT_CRITICAL] = pTempImage;
    pTempImage = new CImage;
    pTempImage->LoadFromResource(AfxGetInstanceHandle(), IDB_CLIENT_DISCONNECTED);
    m_ClientStatusImage[CLIENT_DISCONNECT] = pTempImage;
    pTempImage = new CImage;
    pTempImage->LoadFromResource(AfxGetInstanceHandle(), IDB_CLIENT_NEED_ATTENTION);
    m_ClientStatusImage[CLIENT_NEED_ATTENTION] = pTempImage;

	Translate(this);
    return TRUE;
}


void CClientManagerPage::OnDestroy()
{
    CBaseControllerPage::OnDestroy();

}

void CClientManagerPage::Close()
{
    m_wndGroupList.DestroyWindow();
    m_wndClientState.DestroyWindow();
    m_wndClientInfo.DestroyWindow();

    for (auto it = m_ClientStatusImage.begin(); it != m_ClientStatusImage.end(); it++)
    {
        delete it->second;
    }

    m_ClientStatusImage.clear();
}


BOOL CClientManagerPage::CreateRibbonButtons()
{
    if (!m_pCategory)
    {
        return FALSE;
    }

    /// Remove the dummy panel, and create the real panels
    m_pCategory->RemovePanel(0);

    CString strPanelPage        = Translate(_T("View"));
    CString strPanelGroup       = Translate(_T("Group Edit"));
    CString strPanelClient      = Translate(_T("Client Edit"));
    CString strPanelEdit        = Translate(_T("Edit"));
	CString strPanelRestart     = Translate(_T("Restart"));
	CString strPanelShutDown    = Translate(_T("Shutdown"));
	CString strPanelStart       = Translate(_T("Start"));
    CString strPanelMsg         = Translate(_T("Emergency Message"));
	CString strPanelUpdate      = Translate(_T("Upgrade Client"));
    CString strPanelSearch         = Translate(_T("Search"));

	CMFCRibbonPanel* pPanelPage = m_pCategory->AddPanel(strPanelPage);

	CMFCRibbonButton* pBtnRefresh = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMNT_REFRESH, Translate(_T("Refresh")), 15, 15);
	pBtnRefresh->SetAlwaysLargeImage();
	pBtnRefresh->SetToolTipText(Translate(_T("Refresh")));
    pBtnRefresh->SetDescription(Translate(_T("Refresh the whole page.")));
	pPanelPage->Add(pBtnRefresh);

    CMFCRibbonPanel* pPanelGroup = m_pCategory->AddPanel(strPanelGroup);

    CMFCRibbonButton* pBtnAddGroup = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_ADDGROUP, Translate(_T("Add Group")), 2, 2);
	pBtnAddGroup->SetToolTipText(Translate(_T("Add Group")));
    pBtnAddGroup->SetDescription(Translate(_T("Add a new group.")));
    pPanelGroup->Add(pBtnAddGroup);

    CMFCRibbonButton* pBtnDelGroup = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_DELGROUP, Translate(_T("Delete Group")), 3, 3);
	pBtnDelGroup->SetToolTipText(Translate(_T("Delete Group")));
    pBtnDelGroup->SetDescription(Translate(_T("Delete the selected group.")));
    pPanelGroup->Add(pBtnDelGroup);

    CMFCRibbonButton* pBtnEditGroup = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_EDITGROUP, Translate(_T("Edit Group")), 17, 17);
    pBtnEditGroup->SetToolTipText(Translate(_T("Edit Group")));
    pBtnEditGroup->SetDescription(Translate(_T("Edit the selected group.")));
    pPanelGroup->Add(pBtnEditGroup);

    CMFCRibbonPanel* pPanelClient = m_pCategory->AddPanel(strPanelClient);

    //CMFCRibbonButton* pBtnDisableClt = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_DISABLECLT, _T("Disable Client"), 2, 2);
    //pPanelClient->Add(pBtnDisableClt);

    //CMFCRibbonButton* pBtnEnableClt = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_ENABLECLT, _T("Enable Client"), 3, 3);
    //pPanelClient->Add(pBtnEnableClt);

    CMFCRibbonButton* pBtnDelClt = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_DELCLT, Translate(_T("Delete Client")), 4, 4);
	pBtnDelClt->SetToolTipText(Translate(_T("Delete Client")));
    pBtnDelClt->SetDescription(Translate(_T("Delete all the information of the selected client.")));
    pPanelClient->Add(pBtnDelClt);

    CMFCRibbonButton* pBtnEditClt = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_EDITCLT, Translate(_T("Edit Client")), 16, 16);
    pBtnEditClt->SetToolTipText(Translate(_T("Edit Client")));
    pBtnEditClt->SetDescription(Translate(_T("Edit all the information of the selected client.")));
    pPanelClient->Add(pBtnEditClt);

#ifndef PROFESSIONAL_EDITION

	CMFCRibbonButton* pBtnRestartClt = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_RESTARTCLT, Translate(_T("Restart Client")), 7, 7);
	pBtnRestartClt->SetToolTipText(Translate(_T("Restart Client")));
    pBtnRestartClt->SetDescription(Translate(_T("Restart the selected client.")));
    pPanelClient->Add(pBtnRestartClt);

	CMFCRibbonButton* pBtnShutDownClt = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_SHUTDOWNCLT, Translate(_T("Shutdown Client")), 8, 8);
	pBtnShutDownClt->SetToolTipText(Translate(_T("Shutdown Client")));
    pBtnShutDownClt->SetDescription(Translate(_T("Shut down the selected client.")));
    pPanelClient->Add(pBtnShutDownClt);

	CMFCRibbonButton* pBtnStartClt = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_STARTCLT, Translate(_T("Start Client")), 9, 9);
	pBtnStartClt->SetToolTipText(Translate(_T("Start Client")));
    pBtnStartClt->SetDescription(Translate(_T("Start the selected client.")));
    pPanelClient->Add(pBtnStartClt);

#endif
	CMFCRibbonButton* pBtnViewHistoryClt = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_VIEWHISTORYCLT, Translate(_T("History Info")), 12, 12);
	pBtnViewHistoryClt->SetToolTipText(Translate(_T("History Info")));
    pBtnViewHistoryClt->SetDescription(Translate(_T("View the client's History Info.")));
	pPanelClient->Add(pBtnViewHistoryClt);

#if 0
	CMFCRibbonButton* pBtnUploadClt = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_UPLOADCLT, Translate(_T("upload file")), 10, 10);
	pBtnUploadClt->SetToolTipText(Translate(_T("upload file")));
    pBtnUploadClt->SetDescription(Translate(_T("upload file.")));
	pPanelClient->Add(pBtnUploadClt);

	CMFCRibbonButton* pBtnUpdateClt = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_UPDATECLT, Translate(_T("update Client")), 11, 11);
	pBtnUpdateClt->SetToolTipText(Translate(_T("update Client")));
    pBtnUpdateClt->SetDescription(Translate(_T("update the client.")));
	pPanelClient->Add(pBtnUpdateClt);
#endif

    CMFCRibbonPanel* pPanelMsg = m_pCategory->AddPanel(strPanelMsg);

    CMFCRibbonButton* pBtnEditMsg = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_EDITMSG, Translate(_T("Edit Emergency Message")), 6, 6);
	pBtnEditMsg->SetToolTipText(Translate(_T("Edit Emergency")));
    pBtnEditMsg->SetDescription(Translate(_T("Edit the emergency message of the selected group.")));
    pPanelMsg->Add(pBtnEditMsg);

#ifndef PROFESSIONAL_EDITION

	CMFCRibbonPanel* pPanelUpdate = m_pCategory->AddPanel(strPanelUpdate);

	CMFCRibbonButton* pBtnEditUpdate = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_UPDATECLT, Translate(_T("Upgrade Client")), 11, 11);
	pBtnEditUpdate->SetToolTipText(Translate(_T("Upgrade Client")));
    pBtnEditUpdate->SetDescription(Translate(_T("Upload file and Upgrade client.")));
	pPanelUpdate->Add(pBtnEditUpdate);
#endif

    CMFCRibbonPanel* pPanelSearch = m_pCategory->AddPanel(strPanelSearch);

    CMFCRibbonButton* pBtnSearch = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_SEARCH, Translate(_T("Search Client")), 14, 14);
    pBtnSearch->SetToolTipText(Translate(_T("Search Client")) + _T("\n") + Translate(_T("Search client by User")));
    pPanelSearch->Add(pBtnSearch);
    return TRUE;
}

BOOL CClientManagerPage::Refresh()
{
	HRESULT hr = E_FAIL;

	int nActiveTab = m_wndClientState.m_wndStateTab.GetActiveTab();

	//if (m_wndClientState.m_wndStateTab.GetActiveTab() == 0)
    {
		INT CurGroupID = m_wndGroupList.GetGroupList().GetCurGroupID();
		CString CurClientIP;
		//INT CurClientID = m_wndClientList.GetClientList().GetCurClientID(CurClientIP);

		std::vector<ClientGroup> groupList;
		hr = GetControllerInterface->GetAllClientGroup(groupList);
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
			return FALSE;
		}

		m_CurGroupList.clear();
		m_CurGroupList = groupList;

		m_wndGroupList.m_wndGroupList.UpdateGroupsStatus();

		if (CurGroupID >= 0)
		{
			m_wndClientState.UpdateClientList(CurGroupID);
		}
		else
		{
			m_wndClientState.UpdateClientList(-1);
		}

	}

    //else if (m_wndClientState.m_wndStateTab.GetActiveTab() == 1) //search client tab
    {
        //get list and refresh status
        UINT nBufID;
        std::map<int, ClientInfo>* pInfo = WorkThreadDataManager< std::map<int, ClientInfo> >::NewDataBuf(nBufID); //create dynamic msg
        if (pInfo == NULL)
        {
            return E_FAIL;
        }
        int nCount = m_wndClientState.m_wndStateTab.m_wndSearchList->GetItemCount();
        if(nCount > 0)
        {
            INT VisibleEnd = m_wndClientState.m_wndStateTab.m_wndSearchList->GetTopIndex() + m_wndClientState.m_wndStateTab.m_wndSearchList->GetCountPerPage() + 1;
            for(int i = m_wndClientState.m_wndStateTab.m_wndSearchList->GetTopIndex(); i < VisibleEnd; ++i)
            {
                if (m_wndClientState.m_wndStateTab.m_wndSearchList->IsItemVisible(i))
                {
                    INT ID = m_wndClientState.m_wndStateTab.m_wndSearchList->GetItemData(i);

                    std::map<int, ClientInfo>::iterator it = m_wndClientState.m_wndStateTab.m_ClientIDInfoSearchMap.find(ID);
                    if (it != m_wndClientState.m_wndStateTab.m_ClientIDInfoSearchMap.end())
                    {
                        pInfo[0].insert(*it);
                    }

                }
            }
        }

        if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID,USER_MSG_UPDATE_CLIENT_STATUS,
            (WPARAM)this->GetSafeHwnd(),(LPARAM)nBufID))//post thread msg
        {
            printf("post message failed,errno:%d\n",::GetLastError());
        }
        //return TRUE;
    }

	m_wndClientState.m_wndStateTab.SetActiveTab(nActiveTab);
	return TRUE;
	
}
void CClientManagerPage::OnGroupSelChange2 (NMHDR *pNMHDR, LRESULT *pResult)
{
    HRESULT hr = E_FAIL;
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    INT CurGroupID = m_wndGroupList.GetGroupList().GetCurGroupID();
    if (m_wndClientState.m_wndStateTab.GetActiveTab() != 0)
    {
        m_wndClientState.m_wndStateTab.SetActiveTab(0);
    }

    if (CurGroupID >= 0)
    {
		m_wndClientState.UpdateClientList(CurGroupID);
		
    }
    else
    {
        m_wndClientState.UpdateClientList(-1);
    }
}

void CClientManagerPage::OnClientSelChange (NMHDR *pNMHDR, LRESULT *pResult)
{
    HRESULT hr = E_FAIL;
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
 
}

LRESULT CClientManagerPage::OnGroupEdit (WPARAM, LPARAM)
{
/*
    HRESULT hr = GetControllerInterface->EditClientGroup(m_wndGroupInfo.GetGroupInfo().GetCurGroup());
    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
        }
        else
        {
            TRACE0("Failed to Edit Client Group!\n");
        }
        return 1;
    }

    Refresh();*/

    return 0;
}

LRESULT CClientManagerPage::OnClientEdit (WPARAM wp, LPARAM lp)
{
    INT ClientID = wp;
    ClientInfo* clientInfo = (ClientInfo*)lp;
    HRESULT hr = GetControllerInterface->EditClientInfo(ClientID,
        *clientInfo);
    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
        }
        else
        {
            TRACE0("Failed to Edit Client Info!\n");
        }
        return 1;
    }

    Refresh();
    //m_wndClientList.GetClientList().UpdateClient(*clientInfo, ClientID);
    //m_wndGroupInfo.GetGroupInfo().UpdateClient(*clientInfo, ClientID);

    return 0;
}


LRESULT CClientManagerPage::OnClientSelChanged (WPARAM wp, LPARAM lp)
{
    ClientInfo* pInfo = (ClientInfo*)wp;
    ClientRTStatistics* pStatics = (ClientRTStatistics*)lp;
    if (pInfo)
    {
        CString szIP;
        if (pStatics)
        {
            //m_wndClientInfo.m_InfoCtrl.SetThumbnail(*m_ClientStatusImage[GetClientStatus(*pStatics, szIP)]);
			CLIENT_STATUS status = GetClientStatus(*pStatics, szIP);
			CImage * pcimage= m_ClientStatusImage[status];
			m_wndClientInfo.m_InfoCtrl.SetThumbnail(*pcimage);
            if (!szIP.IsEmpty())
            {
                szIP += _T("\t");
            }
        }
        else
        {
            m_wndClientInfo.m_InfoCtrl.SetThumbnail(*m_ClientStatusImage[CLIENT_NEED_ATTENTION]);
            szIP = _T("");
        }

        m_wndClientInfo.m_InfoCtrl.SetInfo(*pInfo, szIP);
    }
    else
    {
        m_wndClientInfo.m_InfoCtrl.ClearInfo();
    }

	GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, (LPARAM)Translate(_T("Client(s) can be dragged to other client group !")).GetString());
    return 0;
}

LRESULT CClientManagerPage::OnClientDlgSearch(WPARAM wp, LPARAM lp)
{
    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
        (LPARAM)Translate(_T("Search Client List Updating!")).GetString());

    TCHAR buf[16];
    CString strID;
    CString Conditons(_T(""));

    CSearchClientDlg* pDlg = (CSearchClientDlg*)lp;

    CString clientName = pDlg->m_strName;
    CString company = pDlg->m_strCompany;
    CString email = pDlg->m_strEmail;
    CString hardware = pDlg->m_strHardware;
    CString location = pDlg->m_strLocation;
    CString ip = pDlg->m_strIp;
    CString admin = pDlg->m_strAdmin;
    CString tel = pDlg->m_strTel;

    CComboBox *pCB = &pDlg->m_ctlGroup;
    int GroupID = pCB->GetItemData(pCB->GetCurSel());

    if (clientName)
    {
        Conditons = _T("lower(ClientName) like lower('%") + clientName + _T("%') ");
    }
    if (GroupID >= 0)
    {
        _itot_s(GroupID, buf, 10);
        strID = buf;
        Conditons += _T("and GroupId = ") + strID;
    }
    if(company)
    {
        Conditons += _T(" and lower(Company) like lower('%") + company + _T("%') ");
    }
    if (email)
    {
        Conditons += _T("and Email like '%") + email + _T("%' ");
    }
    if (admin)
    {
        Conditons += _T("and Admin like '%") + admin + _T("%' ");
    }
    if (hardware)
    {
        Conditons += _T("and lower(HardwareInfo) like lower('%") + hardware + _T("%') ");
    }
    if (location)
    {
        Conditons += _T("and lower(Location) like lower('%") + location + _T("%') ");
    }
    if (tel)
    {
        Conditons += _T("and Phone like '%") + tel + _T("%' ");
    }
    if (ip)
    {
        Conditons += _T("and LastIP like '%") + ip + _T("%'");
    }
    Condition cond(Conditons.GetString());
    UINT nBufID;
    Condition* pInfo = WorkThreadDataManager< Condition >::NewDataBuf(nBufID);
    if (pInfo == NULL)
    {
        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)Translate(_T("Failed to search client!")).GetString());

        return -1;
    }

    *pInfo = cond;

    if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID,USER_MSG_SERRCH_CLIENT,
        (WPARAM)this->GetSafeHwnd(),(LPARAM)nBufID))//post thread msg
    {
        printf("post message failed,errno:%d\n",::GetLastError());

        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)Translate(_T("Failed to search client!")).GetString());

        return -1;
    }

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("Searching......")).GetString());

    return 0;
}

LRESULT CClientManagerPage::UpdateSearchClientList (WPARAM wp, LPARAM lp)
{
    ////////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf< std::map<int, ClientInfo> > MessageBuf;
    if (!WorkThreadDataManager< std::map<int, ClientInfo> >::GetDataBuf(nMessageBufID, MessageBuf))
    {
        return 1;
    }
    m_wndClientState.m_wndStateTab.m_ClientIDInfoSearchMap.clear();

    m_wndClientState.m_wndStateTab.m_ClientIDInfoSearchMap = *MessageBuf.Get();
    ////////////////////////////////////////////////////////////////////////////
    int visindex = m_wndClientState.m_wndStateTab.m_wndSearchList->GetTopIndex();
    int posx  = m_wndClientState.m_wndStateTab.m_wndSearchList->GetScrollPos(SB_HORZ);
    m_wndClientState.m_wndStateTab.m_wndSearchList->SetRedraw(FALSE);
    m_wndClientState.m_wndStateTab.m_wndSearchList->DeleteAllItems();
    INT insertIndex = -1;
    int i = 0;
    if (m_wndClientState.m_wndStateTab.GetActiveTab() != 1)
    {
        m_wndClientState.m_wndStateTab.SetActiveTab(1);
    }
    for ( std::map<int, ClientInfo>::const_iterator it = MessageBuf.Get()->begin(); it != MessageBuf.Get()->end(); ++it, ++i)
    {
        ClientInfo clientInfo = it->second;

        INT tempIndex = m_wndClientState.m_wndStateTab.m_wndSearchList->InsertItem(i, clientInfo.GetClientDesc().c_str());
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemData(i, it->first);

        std::tstring strName;

        strName = clientInfo.GetClientDesc();


        CString szNotAvailable = _T("N\\A");

        CString Number;
        Number.Format(_T("%d"), i);

        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::NO, Number);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::ONLINE_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::NAME_INDEX, strName.c_str());
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::IP_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::MAC_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::CURLAYOUT_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::FPS_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::HEARTBEAT_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::NEXTLAYOUT_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::SCHEDULE_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::SPEED_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::DISK_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::CPUUSAGE_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::MEMUSAGE_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::FANSPEED_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::CPU_TEMPERATURE_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::MB_TEMPERATURE_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::HDD_TEMPERATURE_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::GPU_TEMPERATURE_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::VERSION_INDEX, szNotAvailable);
        m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::PLAYER_INDEX, szNotAvailable);
    }

    m_wndClientState.m_wndStateTab.m_wndSearchList->SetRedraw(TRUE);

    RECT ys;
    if(m_wndClientState.m_wndStateTab.m_wndSearchList->GetItemRect(visindex,&ys,LVIR_BOUNDS) != 0)
    {
        CSize tmpsize;
        tmpsize.cx = posx;
        tmpsize.cy = (ys.bottom-ys.top)*(visindex);
        m_wndClientState.m_wndStateTab.m_wndSearchList->Scroll(tmpsize);
    }

    //refresh status
    UINT nBufID;
    std::map<int, ClientInfo>* pInfo = WorkThreadDataManager< std::map<int, ClientInfo> >::NewDataBuf(nBufID); //create dynamic msg
    if (pInfo == NULL)
    {
        return E_FAIL;
    }
    int nCount = m_wndClientState.m_wndStateTab.m_wndSearchList->GetItemCount();
    if(nCount > 0)
    {
        INT VisibleEnd = m_wndClientState.m_wndStateTab.m_wndSearchList->GetTopIndex() + m_wndClientState.m_wndStateTab.m_wndSearchList->GetCountPerPage() + 1;
        for(int i = m_wndClientState.m_wndStateTab.m_wndSearchList->GetTopIndex(); i < VisibleEnd; ++i)
        {
            if (m_wndClientState.m_wndStateTab.m_wndSearchList->IsItemVisible(i))
            {
                INT ID = m_wndClientState.m_wndStateTab.m_wndSearchList->GetItemData(i);

                std::map<int, ClientInfo>::iterator it = m_wndClientState.m_wndStateTab.m_ClientIDInfoSearchMap.find(ID);
                if (it != m_wndClientState.m_wndStateTab.m_ClientIDInfoSearchMap.end())
                {
                    pInfo[0].insert(*it);
                }

            }
        }
    }

    if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID,USER_MSG_UPDATE_CLIENT_STATUS,
        (WPARAM)this->GetSafeHwnd(),(LPARAM)nBufID))//post thread msg
    {
        printf("post message failed,errno:%d\n",::GetLastError());
    }

    ///////////////////
    CString InfoStatus("");
    InfoStatus.Format(Translate(_T("Search completed, the result records: %d")), i);
    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)(InfoStatus.GetBuffer()));

    return 0;
}

LRESULT CClientManagerPage::UpdateSearchClientListStatus(WPARAM wp, LPARAM lp)
{
     UINT nMessageBufID = (UINT)wp;

    DataBuf< std::map< int, std::pair<ClientInfo, ClientRTStatistics> >  > MessageBuf;
    if (!WorkThreadDataManager< std::map< int, std::pair<ClientInfo, ClientRTStatistics> >  >::GetDataBuf(nMessageBufID, MessageBuf))
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////

    const std::map< int, std::pair<ClientInfo, ClientRTStatistics> >   * pConstInfo = MessageBuf.Get();

	if(pConstInfo->size())
	{
		for (auto it = pConstInfo->begin(); it != pConstInfo->end(); it++)
		{
			m_wndClientState.m_wndStateTab.m_ClientIDStaticsSearchMap[it->first] = it->second;
		}
	}

    POSITION pos = m_wndClientState.m_wndStateTab.m_wndSearchList->GetFirstSelectedItemPosition();
	INT SelectedID = -1;
	INT index = -1;
	if(pos != NULL)
	{
	    index = m_wndClientState.m_wndStateTab.m_wndSearchList->GetNextSelectedItem(pos);
	    if (index >= 0)
	    {
	        SelectedID = m_wndClientState.m_wndStateTab.m_wndSearchList->GetItemData(index);
	    }
	}
	int nCount = m_wndClientState.m_wndStateTab.m_wndSearchList->GetItemCount();
    if(nCount > 0)
    {
        INT VisibleEnd = m_wndClientState.m_wndStateTab.m_wndSearchList->GetTopIndex() + m_wndClientState.m_wndStateTab.m_wndSearchList->GetCountPerPage() + 1;
        for(int i = m_wndClientState.m_wndStateTab.m_wndSearchList->GetTopIndex(); i < VisibleEnd; ++i)
        {
            if (m_wndClientState.m_wndStateTab.m_wndSearchList->IsItemVisible(i))
            {
                INT ID = m_wndClientState.m_wndStateTab.m_wndSearchList->GetItemData(i);

                std::map< int, std::pair<ClientInfo, ClientRTStatistics> >::const_iterator it = pConstInfo->find(ID);
                if (it != pConstInfo->end())
                {
                    ClientInfo clientInfo = it->second.first;

                    std::tstring strName;

                    strName = clientInfo.GetClientDesc();

					CString strMACAddress;
					strMACAddress = clientInfo.GetMacAddress().c_str();

                    ClientRTStatistics Statistics = it->second.second;

                    LAYOUT_DESC CurLayoutDesc, NextLayoutDesc;
                    SCHEDULE_DESC ScheduleDesc;
                    PLAYER_STATUS_DESC PlayerStatus;
                    SYSTEM_STATUS_DESC SystemStatus;
                    S3Time LastHeartBeat(0,0,0,0,0,0);

                    Statistics.GetCurLayoutDesc(CurLayoutDesc);
                    Statistics.GetNextLayoutDesc(NextLayoutDesc);
                    Statistics.GetScheduleDesc(ScheduleDesc);
                    Statistics.GetPlayerStatusDesc(PlayerStatus);
                    Statistics.GetLastActiveTime(LastHeartBeat);
                    Statistics.GetSysStatusDesc(SystemStatus);

                    CString CstrCurLayout, CstrNextLayout, CstrSchedule, CstrSpeed, CstrOnline,
                        CstrPlayerStatus, CstrHeartBeat, CstrFps, CstrDisc, CstrCPU, CstrMem, CstrIP,
						CstrFanSpeed, CstrCPUTemperature, CstrMBTemperature, CstrHDDTemperature, CstrGPUTemperature, CstrVersion;

					CstrOnline = Statistics.IsOnline() ? Translate(_T("Online")) : Translate(_T("Offline"));

                    if (Statistics.IsOnline())
                    {
                        CstrIP = Statistics.GetIPAddr().c_str();

                        CString CurDownloadSize;
                        CurDownloadSize.Format( _T("%s/%s"),
                            FormatSizeString(CurLayoutDesc.DownloadedSize), FormatSizeString(CurLayoutDesc.LayoutSize));

                        CstrCurLayout.Format( _T("%s: %s, %s - %s"),
                            CurLayoutDesc.LayoutName.c_str(),
                            CurDownloadSize,
                            CurLayoutDesc.StartTime.ToString().c_str(), CurLayoutDesc.EndTime.ToString().c_str());

                        if (CurLayoutDesc.LayoutName.empty())
                        {
                            CstrCurLayout = Translate(_T("No Layout arranged in schedule!"));
                        }

                        CString NextDownloadSize;
                        NextDownloadSize.Format( _T("%s/%s"),
                            FormatSizeString(NextLayoutDesc.DownloadedSize), FormatSizeString(NextLayoutDesc.LayoutSize));

                        CstrNextLayout.Format( _T("%s: %s, %s - %s"),
                            NextLayoutDesc.LayoutName.c_str(),
                            NextDownloadSize,
                            NextLayoutDesc.StartTime.ToString().c_str(), NextLayoutDesc.EndTime.ToString().c_str());

                        if (NextLayoutDesc.LayoutName.empty())
                        {
                            CstrNextLayout =Translate(_T("No Layout arranged in schedule!"));
                        }

                        CString SDownloadSize;
                        SDownloadSize.Format( _T("%s/%s"),
                            FormatSizeString(ScheduleDesc.DownloadedSize), FormatSizeString(ScheduleDesc.TotalSize));

                        CstrSchedule.Format( Translate(_T("%s, To: %s, Layout count: %d")),
                            SDownloadSize,
                            ScheduleDesc.EndTime.ToString().c_str(), ScheduleDesc.LayoutCount);

                        CstrSpeed.Format( _T("%d KB/s"), SystemStatus.TransportRate);


                        CstrPlayerStatus.Format( Translate(_T("Device lost count: %d, Status: %s")),
                            PlayerStatus.DeviceLostCount, Translate(PlayerStatus.Status.c_str()));

                        CstrFps.Format( _T("%d"), PlayerStatus.FPS);

                        CstrHeartBeat = LastHeartBeat.ToString().c_str();
                        /*COleDateTimeSpan TimeGap = COleDateTime::GetCurrentTime() - LastHeartBeat.ToCOleDateTime();

                        if (TimeGap.GetDays() > 1)
                        {
                        CString str;
                        str.Format(_T("%d Days,"), TimeGap.GetDays());
                        CstrHeartBeat.Append(str);
                        }
                        if (TimeGap.GetHours() > 1)
                        {
                        CString str;
                        str.Format(_T("%d Hours,"), TimeGap.GetHours());
                        CstrHeartBeat.Append(str);
                        }
                        if (TimeGap.GetMinutes() > 1)
                        {
                        CString str;
                        str.Format(_T("%d Minutes,"), TimeGap.GetMinutes());
                        CstrHeartBeat.Append(str);
                        }

                        CString str;
                        str.Format(_T("%d Seconds Before"), TimeGap.GetSeconds());
                        CstrHeartBeat.Append(str);*/

                        CstrDisc.Format( Translate(_T("%d %%, Free Size: %d MB ")), SystemStatus.DiskUsage, SystemStatus.DiskFreeSpace);
                        
						if(SystemStatus.CPUUsage >= 0)
							CstrCPU.Format( _T("%d %%"), SystemStatus.CPUUsage);

                        if(SystemStatus.MemUsage >= 0)
							CstrMem.Format( _T("%d %%"), SystemStatus.MemUsage);
						
						if(SystemStatus.FanSpeed >= 0 )
							CstrFanSpeed.Format( _T("%d RPM"), SystemStatus.FanSpeed);
						
						if (SystemStatus.CPUTemperature > 0)
							CstrCPUTemperature.Format( _T("%d \x2103"), SystemStatus.CPUTemperature);

						if (SystemStatus.MotherBoardTemperature > 0)
							CstrMBTemperature.Format( _T("%d \x2103"), SystemStatus.MotherBoardTemperature);

						if (SystemStatus.HDDTemperature > 0)
							CstrHDDTemperature.Format( _T("%d \x2103"), SystemStatus.HDDTemperature);

						if (SystemStatus.S3GPUTemperature > 0)
							CstrGPUTemperature.Format( _T("%d \x2103"), SystemStatus.S3GPUTemperature);

						CstrVersion.Format( _T("%s"), SystemStatus.Version.c_str());
                    }
					else
					{
						CstrCurLayout = _T("");
						CstrNextLayout = _T("");
						CstrSchedule = _T("");
						CstrSpeed = _T("");
						CstrOnline = _T("");
						CstrPlayerStatus = _T("");
						CstrHeartBeat = _T("");
						CstrFps = _T("");
						CstrDisc = _T("");
						CstrCPU = _T("");
						CstrMem = _T("");
						CstrIP = _T("");
						CstrFanSpeed = _T("");
						CstrCPUTemperature = _T("");
						CstrMBTemperature = _T("");
						CstrHDDTemperature = _T("");
						CstrGPUTemperature = _T("");
						CstrVersion = _T("");
					}

                    m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::ONLINE_INDEX, CstrOnline);
                    m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::NAME_INDEX, strName.c_str());
                    m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::IP_INDEX, CstrIP);
					m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::MAC_INDEX, strMACAddress);
                    m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::CURLAYOUT_INDEX, CstrCurLayout);
                    m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::FPS_INDEX, CstrFps);
                    m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::HEARTBEAT_INDEX, CstrHeartBeat);
                    m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::NEXTLAYOUT_INDEX, CstrNextLayout);
                    m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::SCHEDULE_INDEX, CstrSchedule);
                    m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::SPEED_INDEX, CstrSpeed);
                    m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::DISK_INDEX, CstrDisc);
                    m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::CPUUSAGE_INDEX, CstrCPU);
                    m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::MEMUSAGE_INDEX, CstrMem);
					m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::FANSPEED_INDEX, CstrFanSpeed);
					m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::CPU_TEMPERATURE_INDEX, CstrCPUTemperature);
					m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::MB_TEMPERATURE_INDEX, CstrMBTemperature);
					m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::HDD_TEMPERATURE_INDEX, CstrHDDTemperature);
					m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::GPU_TEMPERATURE_INDEX, CstrGPUTemperature);
					m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::VERSION_INDEX, CstrVersion);
                    m_wndClientState.m_wndStateTab.m_wndSearchList->SetItemText(i, CClientStateTab::PLAYER_INDEX, CstrPlayerStatus);

					if(i ==  index)
					{
						 GetParent()->SendMessage(CLIENTMANAGER_CLIENT_SEL_CHANGE, (WPARAM)&clientInfo, (LPARAM)&Statistics);
					}
                }
            }
        }
    }

    return 0;
}


void CClientManagerPage::OnRibbonBtn_EditSchedule()
{
    INT CurGroupID = m_wndGroupList.GetGroupList().GetCurGroupID();

    if (CurGroupID >= 0)
    {
        GetParent()->SendMessage(SCHEDULER_ADD_EVENT, NULL, CurGroupID);
    }
}

void CClientManagerPage::OnRibbonBtn_EditSchedule_Offline()
{
    GetParent()->SendMessage(SCHEDULER_ADD_EVENT, NULL, -1);
}

void CClientManagerPage::OnRibbonBtn_AddGroup()
{
    HRESULT hr = E_FAIL;
    ClientManagerAddGroupDlg dlg(this);

    if (dlg.DoModal() == IDOK)
    {
        CString NewName, NewDesc;
        NewName = dlg.m_GroupName;
        NewDesc = dlg.m_GroupDesc;

        if (!NewName.IsEmpty())
        {
            ClientGroup group;
            hr = GetControllerInterface->NewClientGroup(NewName.GetString(), NewDesc.GetString(), group);
            if (!SUCCEEDED(hr))
            {
                if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                {
                    GetControllerApp->OnControllerServerOffline();
                }
                else
                {
                    std::tstring desc;
                    int err = GetControllerInterface->GetLastError(desc);
                    MessageBox(Translate(_T("Failed to Add Client Group: ")) + Translate(desc.c_str()), Translate(_T("Error:Add group")), MB_OK|MB_ICONERROR);
                    TRACE0("Failed to New Client Group!\n");
                }
                return;
            }

            GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
                (LPARAM)(Translate(_T("New Client Group Added:")) + NewName).GetString());

            Refresh();
        }
    }
}

void CClientManagerPage::OnRibbonBtn_DeleteGroup()
{
	if (MessageBox(Translate(_T("Are you sure to delete this Group?")), Translate(_T("Tips:Delete group")),MB_YESNO|MB_ICONQUESTION) != IDYES)
    {
        return;
    }

    INT CurGroupID = m_wndGroupList.GetGroupList().GetCurGroupID();
    if (CurGroupID >= 0)
    {
        HRESULT hr = GetControllerInterface->DeleteClientGroup(CurGroupID);
        if (!SUCCEEDED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                GetControllerApp->OnControllerServerOffline();
            }
            else
            {
                TRACE0("Failed to Delete Client Group!\n");
            }
            return;
        }

        POSITION pos = m_wndGroupList.GetGroupList().GetFirstSelectedItemPosition();
        INT index = m_wndGroupList.GetGroupList().GetNextSelectedItem(pos);

        CString strName = m_wndGroupList.GetGroupList().GetItemText(index, 0);

        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)(Translate(_T("Client Group Deleted:")) + strName).GetString());

        // Put all the clients to Default Group
        CString Conditons;
        Conditons.Format( _T("GroupId =  %d"), CurGroupID);
        Condition cond(Conditons.GetString());

        std::vector<int> clientIDList;

        hr = GetControllerInterface->SearchClient(cond, clientIDList);
        if (!SUCCEEDED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                GetControllerApp->OnControllerServerOffline();
            }
            else
            {
                TRACE0("Failed to Search Client!\n");
            }
            return;
        }
        for (std::vector<int>::iterator it = clientIDList.begin(); it != clientIDList.end(); ++it)
        {
            ClientInfo info;
            hr = GetControllerInterface->GetClient(*it, info);
            if (SUCCEEDED(hr))
            {
                info.AddGroupID(m_DefaultGroupID);
                hr = GetControllerInterface->EditClientInfo(*it, info);
            }
            else
            {
                if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                {
                    GetControllerApp->OnControllerServerOffline();
                }
                else
                {
                    TRACE0("Failed to Get Client!\n");
                }
            }
        }

        //m_wndGroupInfo.GetGroupInfo().Clear();

        Refresh();

    }
}


void CClientManagerPage::OnRibbonBtn_EditGroup()
{
    CGroupInfoCtrl dlg(this);
    dlg.InitGroup(m_wndGroupList.GetGroupList().GetCurGroupID());

    if (IDOK == dlg.DoModal())
    {
        HRESULT hr = GetControllerInterface->EditClientGroup(dlg.GetCurGroup());
        if (!SUCCEEDED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                GetControllerApp->OnControllerServerOffline();
            }
            else
            {
                std::tstring desc;
                int err = GetControllerInterface->GetLastError(desc);
                if (err == S3NET_CLIENT_GROUPNAME_CONFLICT)
                {  
					MessageBox(Translate(_T("Edit client group failed: group name already exists.")), Translate(_T("Error:Edit group")), MB_OK);
                }
                else if (err == S3NET_NAME_EMPTY)
                {
                    MessageBox(Translate(_T("Edit client group failed: group name can't be empty.")), Translate(_T("Error:Edit group")), MB_OK);
                }
                else if (err == S3NET_CLIENT_GROUP_ID_NOT_FOUND)
                {
                    MessageBox((Translate(_T("Edit client group failed: group not exists, maybe deleted by others."))), Translate(_T("Error:Edit group")), MB_OK);
                }
                TRACE0("Failed to Edit Client Group!\n");
            }
            return;
        }

        Refresh();

        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)Translate(_T("Group Information Changed!")).GetString());
    }
}

void CClientManagerPage::OnRibbonBtn_EditClient()
{
    CClientEditorCtrl dlg(this);

    vector<INT> ClientIDs;
    m_wndClientState.m_wndStateTab.GetSelectedClients(ClientIDs);
    if (ClientIDs.size() != 1)
    {
        MessageBox(Translate(_T("Please selecte a single client at first!")), Translate(_T("Tips:Edit client")),MB_OK|MB_ICONINFORMATION);
        return;
    }

    CString CurClientIP;
    INT CurClientID = ClientIDs[0];

    if (CurClientID >= 0)
    {
        ClientInfo client;
        HRESULT hr = GetControllerInterface->GetClient(CurClientID, client);
        if (!SUCCEEDED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                GetControllerApp->OnControllerServerOffline();
            }
            else
            {
                TRACE0("Failed to Get Client\n");
            }
            return;
        }

        dlg.SetCurClient(client, CurClientID, CurClientIP);

    }

    if (IDOK == dlg.DoModal())
    {
        INT ClientID = dlg.GetCurClientID();
        ClientInfo clientInfo = dlg.GetCurClient();
        HRESULT hr = GetControllerInterface->EditClientInfo(ClientID,
            clientInfo);
        if (!SUCCEEDED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                GetControllerApp->OnControllerServerOffline();
            }
            else
            {
                TRACE0("Failed to Edit Client Info!\n");
            }
            return;
        }

		if (MessageBox(Translate(_T("Do you want to restart the client to make the setting take effect?")),Translate(_T("Tips:Restart client")), MB_YESNO|MB_ICONQUESTION) == IDYES)
		{
			SystemTask task;
			task.SetTaskType(SystemTask::REBOOT);
			HRESULT hr = GetControllerInterface->AssignTaskToClients(task, ClientIDs);
			if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
			{
				GetControllerApp->OnControllerServerOffline();
				return;
			}
		}

        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)Translate(_T("Client Information Changed!")).GetString());
    }

	Refresh();
}

//
//void CClientManagerPage::OnRibbonBtn_DisableClient()
//{
//    INT ID = m_wndClientList.GetClientList().GetCurClientID();
//    if (ID >= 0)
//    {
//        ClientInfo client;
//        HRESULT hr;
//        hr = GetControllerInterface->GetClient(ID, client);
//
//        //if (SUCCEED(hr))
//        //{
//        //    GetControllerInterface->EditClientInfo(ID, );
//        //}
//    }
//    else
//    {
//
//    }
//}
//
//void CClientManagerPage::OnRibbonBtn_EnableClient()
//{
//
//}
void CClientManagerPage::OnRibbonBtn_RestartClient()
{
	std::vector<INT> ClientIDs;
	if (!m_wndClientState.m_wndStateTab.GetSelectedClients(ClientIDs))
	{
		MessageBox(Translate(_T("No client(s) selected.")),Translate(_T("Warning:Restart client")), MB_OK|MB_ICONEXCLAMATION);

		return;
	}

	if (MessageBox(Translate(_T("Are you sure to Restart the selected client(s)?")),Translate(_T("Tips:Restart client")), MB_YESNO|MB_ICONQUESTION) != IDYES)
	{
		return;
	}
#if 0
	std::vector<INT> GroupIDs;
	GroupIDs.push_back(m_wndGroupList.GetGroupList().GetCurGroupID());

	SystemTask task;
	task.SetTaskType(SystemTask::REBOOT);
	HRESULT hr = GetControllerInterface->AssignTaskToGroups(task, GroupIDs);
	if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
	{
		GetControllerApp->OnControllerServerOffline();
		return;
	}

	POSITION pos = m_wndGroupList.GetGroupList().GetFirstSelectedItemPosition();
	INT index = m_wndGroupList.GetGroupList().GetNextSelectedItem(pos);

	CString strName = m_wndGroupList.GetGroupList().GetItemText(index, 0);

	GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
		(LPARAM)(Translate(_T("Restart Command Sent to group:")) + strName).GetString());
#endif
	SystemTask task;
	task.SetTaskType(SystemTask::REBOOT);
	HRESULT hr = GetControllerInterface->AssignTaskToClients(task, ClientIDs);
	if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
	{
		GetControllerApp->OnControllerServerOffline();
		return;
	}
	GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
		(LPARAM)(Translate(_T("Restart Command Sent to client(s) !"))).GetString());

}

void CClientManagerPage::OnRibbonBtn_ShutDownClient()
{
	std::vector<INT> ClientIDs;
	if (!m_wndClientState.m_wndStateTab.GetSelectedClients(ClientIDs))
	{
		MessageBox(Translate(_T("No client(s) selected.")),Translate(_T("Tips:Shutdown client")), MB_OK|MB_ICONEXCLAMATION);

		return;
	}

	if (MessageBox(Translate(_T("Are you sure to shut down the selected client(s)?")), Translate(_T("Tips:Shutdown client")),MB_YESNO|MB_ICONQUESTION) != IDYES)
	{
		return;
	}

#if 0
	std::vector<INT> GroupIDs;
	GroupIDs.push_back(m_wndGroupList.GetGroupList().GetCurGroupID());

	SystemTask task;
	task.SetTaskType(SystemTask::SHUTDOWN);
	HRESULT hr = GetControllerInterface->AssignTaskToGroups(task, GroupIDs);

	if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
	{
		GetControllerApp->OnControllerServerOffline();
		return;
	}

	POSITION pos = m_wndGroupList.GetGroupList().GetFirstSelectedItemPosition();
	INT index = m_wndGroupList.GetGroupList().GetNextSelectedItem(pos);

	CString strName = m_wndGroupList.GetGroupList().GetItemText(index, 0);

	GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
		(LPARAM)(Translate(_T("ShutDown Command Sent to group:")) + strName).GetString());
#endif
	SystemTask task;
	task.SetTaskType(SystemTask::SHUTDOWN);
	HRESULT hr = GetControllerInterface->AssignTaskToClients(task, ClientIDs);

	if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
	{
		GetControllerApp->OnControllerServerOffline();
		return;
	}

	GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
		(LPARAM)(Translate(_T("ShutDown Command Sent to client(s) !"))).GetString());
}

void CClientManagerPage::OnRibbonBtn_StartClient()
{
	std::vector<INT> ClientIDs;
	if (!m_wndClientState.m_wndStateTab.GetSelectedClients(ClientIDs))
	{
		MessageBox(Translate(_T("No client(s) selected.")), Translate(_T("Warning:Start client")), MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	if (MessageBox(Translate(_T("Are you sure to start the selected client(s)?")), Translate(_T("Tips:Start client")),MB_YESNO|MB_ICONQUESTION) != IDYES)
	{
		return;
	}

	std::vector<ClientInfo> ClientInfoList;
	HRESULT hr = GetControllerInterface->GetClients(ClientIDs, ClientInfoList);
	if (HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID) == hr)
	{
		GetControllerApp->OnControllerServerOffline();
		return;
	}
	else if (FAILED(hr))
	{
		MessageBox(Translate(_T("Failed to get client information.")), Translate(_T("Error:Start client")), MB_OK|MB_ICONERROR);
		return;
	}

	std::vector<ClientInfo>::iterator it;
	bool bRet = false;
	for (it = ClientInfoList.begin(); it != ClientInfoList.end(); ++it)
	{
		std::tstring ip   = (*it).GetLastIpAddress();
		std::tstring mac  = (*it).GetMacAddress();

		if (_T("") != ip && _T("") != mac)
		{
			bRet = SysCall::Remotewakeup(ip, mac);
			if (!bRet)
			{
				break;
			}
		}
		else
		{
			bRet = false;
			break;
		}
	}

	if (!bRet)
	{
		MessageBox(Translate(_T("Fatal error occurs when start client(s).")), Translate(_T("Error:Start client")),MB_OK|MB_ICONERROR);
		return;
	}

	GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
		(LPARAM)(Translate(_T("Start command sent to client(s) !"))).GetString());
}

void CClientManagerPage::OnRibbonBtn_ViewHistoryStatus()
{
	vector<INT> ClientIDs;
	m_wndClientState.m_wndStateTab.GetSelectedClients(ClientIDs);
	if (ClientIDs.size() != 1)
	{
		MessageBox(Translate(_T("Please selecte a single client at first!")),Translate(_T("Warning:View histroy status")), MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	CString CurClientIP;
	INT CurClientID = ClientIDs[0];

	m_wndClientState.m_wndStateTab.AddChartPane(CurClientID);
}


void CClientManagerPage::OnRibbonBtn_UploadClient()
{


}


void CClientManagerPage::OnRibbonBtn_UpdateClient()
{
	/*
	std::vector<INT> ClientIDs;
	if (!m_wndClientState.m_wndStateTab.GetSelectedClients(ClientIDs))
	{
		AfxMessageBox(Translate(_T("No client(s) selected.")), MB_OK);

		return;
	}
	*/
	if(-100 == m_wndGroupList.GetGroupList().GetCurGroupID())
	{
		MessageBox(Translate(_T("No Group(s) selected.")), Translate(_T("Warning:Update client")),MB_OK|MB_ICONEXCLAMATION);

		return;
	}

	ExportFilesDlg dlg(this);

	if(IDOK == dlg.DoModal())
	{
		std::vector<INT> GroupIDs;
		GroupIDs.push_back(m_wndGroupList.GetGroupList().GetCurGroupID());

		SystemTask task;
		task.SetTaskType(SystemTask::UPDATE);
		task.SetArgs(_T("\\ClientSetup.msi"));
		task.SetMSIVersion(dlg.GetLastMSIVersion());

		HRESULT hr = GetControllerInterface->AssignTaskToGroups(task, GroupIDs);

		if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
		{
			GetControllerApp->OnControllerServerOffline();
			return;
		}

		POSITION pos = m_wndGroupList.GetGroupList().GetFirstSelectedItemPosition();
		INT index = m_wndGroupList.GetGroupList().GetNextSelectedItem(pos);

		CString strName = m_wndGroupList.GetGroupList().GetItemText(index, 0);

		GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
			(LPARAM)(Translate(_T("Upgrade Command Sent to group:")) + strName).GetString());
#if 0
		SystemTask task;
		task.SetTaskType(SystemTask::UPDATE);
		HRESULT hr = GetControllerInterface->AssignTaskToClients(task, ClientIDs);

		if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
		{
			GetControllerApp->OnControllerServerOffline();
			return;
		}

		GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
			(LPARAM)(Translate(_T("update Command Sent to client(s) ! ")) ).GetString());
#endif
	}
}

void CClientManagerPage::OnRibbonBtn_SearchClient()
{
    CSearchClientDlg dlg(this);

    dlg.DoModal();
}
void CClientManagerPage::OnRibbonBtn_DeleteClient()
{
    vector<INT> ClientIDs;
    if (!m_wndClientState.m_wndStateTab.GetSelectedClients(ClientIDs))
    {
        MessageBox(Translate(_T("No client(s) selected.")),Translate(_T("Warning:Delete client")), MB_OK|MB_ICONEXCLAMATION);

        return;
    }

    if (MessageBox(Translate(_T("Are you sure to delete the selected client(s)?")), Translate(_T("Tips:Delete client")),MB_YESNO|MB_ICONQUESTION) != IDYES)
    {
        return;
    }

    for (auto it = ClientIDs.begin(); it != ClientIDs.end(); it++)
    {
        INT ID = *it;

        if (ID >= 0)
        {
            ClientInfo client;
            HRESULT hr = GetControllerInterface->DeleteClient(ID);

            if (SUCCEEDED(hr))
            {


            }
            else
            {
                if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                {
                    GetControllerApp->OnControllerServerOffline();
                }
                else
                {
                    TRACE0("Failed to Delete Client!\n");
                }
            }
        }
        else
        {

        }
    }

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)(Translate(_T("Client(s) Deleted:"))).GetString());
    Refresh();

}

void CClientManagerPage::OnRibbonBtn_Save()
{
    /*if (m_wndGroupInfo.GetGroupInfo().m_bModified)
    {
        m_wndGroupInfo.GetGroupInfo().OnBnClickedButtonSave();
        m_wndGroupInfo.GetGroupInfo().m_bModified = FALSE;

        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)Translate(_T("Group Information Changed!")).GetString());
    }
    else if (m_wndClientInfo.GetClientInfo().m_bModified)
    {
        m_wndClientInfo.GetClientInfo().OnBnClickedButtonSave();
        m_wndClientInfo.GetClientInfo().m_bModified = FALSE;

        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)Translate(_T("Client Information Changed!")).GetString());
    }*/
}

void CClientManagerPage::OnRibbonBtn_Refresh()
{
    Refresh();
}

void CClientManagerPage::OnRibbonBtn_EditEmergencyMessage()
{
    EmergencyMessageDlg dlg(this);
    INT CurGroupID = m_wndGroupList.GetGroupList().GetCurGroupID();

    std::vector<EmergencyMessage> histroy;
    HRESULT hr;
    if (CurGroupID < 0)
    {
        hr = GetControllerInterface->GetEmergencyMSGHistory(
            S3Time::CreateTime(COleDateTime(COleDateTime::GetCurrentTime() - COleDateTimeSpan(365, 0, 0, 0))),
            S3Time::CreateTime(COleDateTime(COleDateTime::GetCurrentTime() + COleDateTimeSpan(365, 0, 0, 0))),
            histroy);
    }
    else
    {
        hr = GetControllerInterface->GetEmergencyMSGHistoryByGroup(
            S3Time::CreateTime(COleDateTime(COleDateTime::GetCurrentTime() - COleDateTimeSpan(365, 0, 0, 0))),
            S3Time::CreateTime(COleDateTime(COleDateTime::GetCurrentTime() + COleDateTimeSpan(365, 0, 0, 0))),
            histroy, CurGroupID);
    }

    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
        }
        else
        {
            TRACE0("Failed to Get Emergency Message History!\n");
        }
        return;
    }

    dlg.SetMsgHistory(histroy);

	if (dlg.DoModal() == IDOK)
    {
        EmergencyMessage Emessage;
        if (!dlg.GetNewMsg(Emessage))
        {
            MessageBox(Translate(_T("Failed to generate new emergency message!")), Translate(_T("Error:Edit emergency message")), MB_OK|MB_ICONERROR);
            return;
        }

        std::vector<INT> GroupIDs;
        GroupIDs.push_back(m_wndGroupList.GetGroupList().GetCurGroupID());
        HRESULT hr = GetControllerInterface->PublishEmergencyMessage(Emessage, GroupIDs);
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
            return;
        }


        POSITION pos = m_wndGroupList.GetGroupList().GetFirstSelectedItemPosition();
        INT index = m_wndGroupList.GetGroupList().GetNextSelectedItem(pos);

        CString strName = m_wndGroupList.GetGroupList().GetItemText(index, 0);

        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)(Translate(_T("Emergency message published to group:")) + strName).GetString());
    }
}

void CClientManagerPage::OnUpdateClientRibbonBtn( CCmdUI* pCmdUI )
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    vector<INT> ClientIDs;
    if (m_wndClientState.m_wndStateTab.GetSelectedClients(ClientIDs))
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}


void CClientManagerPage::OnUpdateGroupRibbonBtn( CCmdUI* pCmdUI )
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    POSITION pos = m_wndGroupList.GetGroupList().GetFirstSelectedItemPosition();
    INT index = m_wndGroupList.GetGroupList().GetNextSelectedItem(pos);


    UINT GroupID;
    CString GroupName;
    if (index >= 0 && m_wndGroupList.GetGroupList().GetSafeHwnd() != NULL)
    {
        GroupID = m_wndGroupList.GetGroupList().GetItemData(index);
        GroupName = m_wndGroupList.GetGroupList().GetItemText(index, 0);
    }

    if (index >= 0 && GroupID != S3NET_DEFAULT_CLIENTGROUP_ID)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CClientManagerPage::OnUpdateGroupSingleSelectRibbonBtn( CCmdUI* pCmdUI )
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    POSITION pos = m_wndGroupList.GetGroupList().GetFirstSelectedItemPosition();
    INT index = m_wndGroupList.GetGroupList().GetNextSelectedItem(pos);

    if (index >= 0)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CClientManagerPage::OnUpdateSaveRibbonBtn( CCmdUI* pCmdUI )
{
    /*if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    if (m_wndGroupInfo.GetGroupInfo().m_bModified || m_wndClientInfo.GetClientInfo().m_bModified)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }*/
}

void CClientManagerPage::OnUpdateRibbonBtnNormal(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }
}

void CClientManagerPage::OnUpdateRibbonBtn(CCmdUI* pCmdUI)
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	static std::map<int, std::pair<std::function<void (CCmdUI*)>, std::function<bool ()> > > dispatchMap;
	// check privilege
	if (dispatchMap.empty())
	{
		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_DELCLT, &CClientManagerPage::OnUpdateClientRibbonBtn, LAMBDA(SC(DeleteClient)));
		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_EDITCLT, &CClientManagerPage::OnUpdateClientRibbonBtn, LAMBDA(SC(EditClient)));

		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_RESTARTCLT, &CClientManagerPage::OnUpdateClientRibbonBtn, LAMBDA(SC(EditClient)));
		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_SHUTDOWNCLT, &CClientManagerPage::OnUpdateClientRibbonBtn,  LAMBDA(SC(EditClient)));
		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_STARTCLT, &CClientManagerPage::OnUpdateClientRibbonBtn, LAMBDA(SC(EditClient)));
		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_VIEWHISTORYCLT, &CClientManagerPage::OnUpdateClientRibbonBtn, LAMBDA(SC(EditClient)));

		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_ADDGROUP, &CClientManagerPage::OnUpdateRibbonBtnNormal, LAMBDA(SC(CreateClientGroup)));
		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_EDITGROUP, &CClientManagerPage::OnUpdateGroupSingleSelectRibbonBtn, LAMBDA(SC(EditClientGroup)));
		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_DELGROUP, &CClientManagerPage::OnUpdateGroupRibbonBtn, LAMBDA(SC(DeleteClientGroup)));

		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_EDITMSG, &CClientManagerPage::OnUpdateGroupSingleSelectRibbonBtn, LAMBDA(SC(EmergencyMessageOperation)));
		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_SAVE, &CClientManagerPage::OnUpdateSaveRibbonBtn, RET_TRUE());;

		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_EDITSCHEDULE, &CClientManagerPage::OnUpdateGroupSingleSelectRibbonBtn, RET_TRUE());
		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_OFFLINESCHEDULE, &CClientManagerPage::OnUpdateRibbonBtnNormal, RET_TRUE());

		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_UPDATECLT, &CClientManagerPage::OnUpdateRibbonBtnNormal, LAMBDA(SC(GetMediaServerInfo) && SC(AssignTaskToGroups)));
	}

	auto it = dispatchMap.find(pCmdUI->m_nID), ite = dispatchMap.end();
	// id exist and have privilege
	if (it != ite && it->second.second())
	{
		it->second.first(pCmdUI);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}

BOOL CClientManagerPage::UpdateOptions()
{
    return TRUE;
}


BOOL CClientManagerPage::RefreshPage()
{
    return Refresh();
}
