#pragma once

#include "GroupInfoCtrl.h"
#include "ClientEditorCtrl.h"
#include "GroupListPane.h"
//include "ClientFilterPane.h"
#include "ClientStatePane.h"
#include "ClientInfoPane.h"

// wParam not used, lParam - new schedule Group ID
extern UINT SCHEDULER_ADD_EVENT;

extern CString CLIENT_MANAGER_CLIP_FMT_NAME;
extern CLIPFORMAT CLIENT_MANAGER_CLIP_FMT;
// CClientManagerPage

class CClientManagerPage : public CBaseControllerPage
{

protected:
    DECLARE_DYNAMIC(CClientManagerPage)

public:
    CClientManagerPage();
    virtual ~CClientManagerPage();

public:

    // Close the main client management window.
    void Close();

    /// Create the ribbon buttons
    BOOL CreateRibbonButtons();

    BOOL Refresh();

    virtual BOOL RefreshPage();

    BOOL InitPage();

    BOOL UpdateOptions();
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    
    afx_msg void OnRibbonBtn_EditSchedule();
    afx_msg void OnRibbonBtn_EditSchedule_Offline();

    afx_msg void OnRibbonBtn_AddGroup();
    afx_msg void OnRibbonBtn_DeleteGroup();
    afx_msg void OnRibbonBtn_EditGroup();
    //afx_msg void OnRibbonBtn_DisableClient();
    //afx_msg void OnRibbonBtn_EnableClient();
    afx_msg void OnRibbonBtn_EditClient();
    afx_msg void OnRibbonBtn_DeleteClient();
	afx_msg void OnRibbonBtn_RestartClient();
	afx_msg void OnRibbonBtn_ShutDownClient();
	afx_msg void OnRibbonBtn_StartClient();

	afx_msg void OnRibbonBtn_ViewHistoryStatus();

	afx_msg void OnRibbonBtn_UploadClient();
	afx_msg void OnRibbonBtn_UpdateClient();
    afx_msg void OnRibbonBtn_SearchClient();
    afx_msg void OnRibbonBtn_Save();
    afx_msg void OnRibbonBtn_EditEmergencyMessage();

    afx_msg void OnRibbonBtn_Refresh();


    afx_msg void OnUpdateClientRibbonBtn(CCmdUI* pCmdUI);
    afx_msg void OnUpdateGroupRibbonBtn(CCmdUI* pCmdUI);
    afx_msg void OnUpdateSaveRibbonBtn(CCmdUI* pCmdUI);
    afx_msg void OnUpdateGroupSingleSelectRibbonBtn(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnNormal(CCmdUI* pCmdUI);

    afx_msg void OnUpdateRibbonBtn(CCmdUI* pCmdUI);

    afx_msg void OnGroupSelChange2(NMHDR *pNMHDR, LRESULT *pResult);

    afx_msg LRESULT UpdateGroupInfo (WPARAM wp, LPARAM lp);
    afx_msg LRESULT UpdateSearchClientList (WPARAM wp, LPARAM lp);
    afx_msg LRESULT UpdateSearchClientListStatus(WPARAM wp, LPARAM lp);

    afx_msg void OnClientSelChange(NMHDR *pNMHDR, LRESULT *pResult);
    

    afx_msg LRESULT OnGroupEdit (WPARAM, LPARAM);
    afx_msg LRESULT OnClientEdit (WPARAM, LPARAM);

    
    afx_msg LRESULT OnClientSelChanged (WPARAM, LPARAM);
    afx_msg LRESULT OnClientDlgSearch(WPARAM wp, LPARAM lp);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

protected:
    // Pane
    CClientStatePane    m_wndClientState;
    CGroupListPane      m_wndGroupList;
    CClientInfoPane     m_wndClientInfo;


    INT                 m_DefaultGroupID;
    

    std::vector<ClientGroup> m_CurGroupList;

    std::map<INT, CImage*> m_ClientStatusImage;
};


