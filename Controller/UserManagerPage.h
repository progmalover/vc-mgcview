#pragma once

#include "UserEditorPane.h"
#include "UserListPane.h"


// CUserManagerPage

class CUserManagerPage : public CBaseControllerPage
{

protected:
    DECLARE_DYNAMIC(CUserManagerPage)

public:
    CUserManagerPage();
    virtual ~CUserManagerPage();

public:

    // Close the main user management window.
    void Close();

    virtual BOOL RefreshPage();

    // Create the ribbon buttons
    BOOL CreateRibbonButtons();

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLvnItemchangedListUser(NMHDR *pNMHDR, LRESULT *pResult);

    // ribbon button events
    afx_msg void OnRibbonBtnCreate();
    afx_msg void OnRibbonBtnDel();
    afx_msg void OnRibbonBtnRefresh();

    afx_msg void OnUpdate(CCmdUI* pCmdUI);
    afx_msg void OnUpdateDel(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCreate(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRefresh(CCmdUI* pCmdUI);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

    // Pane
    CUserListPane       m_wndUserList;
    CUserEditorPane     m_wndUserEditor;

    int                 m_nCurUserId;
};


