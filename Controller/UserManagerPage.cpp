// F:\p4\sw\Demo\ContentManagement\Controller\User Management\UserManagement.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "UserManagerPage.h"
#include "DialogNewUser.h"
#include "Localization/Localization.h"

// CUserManagerPage

IMPLEMENT_DYNAMIC(CUserManagerPage, CBaseControllerPage)

CUserManagerPage::CUserManagerPage()
: m_nCurUserId(-1)
{

}

CUserManagerPage::~CUserManagerPage()
{
    Close();
}

BEGIN_MESSAGE_MAP(CUserManagerPage, CBaseControllerPage)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_NOTIFY(LVN_ITEMCHANGED, IDW_LIST_USER, &CUserManagerPage::OnLvnItemchangedListUser)
    ON_COMMAND(ID_RIBBON_BTN_USRMGMT_CREATE, &CUserManagerPage::OnRibbonBtnCreate)
    ON_COMMAND(ID_RIBBON_BTN_USRMGMT_DEL, &CUserManagerPage::OnRibbonBtnDel)
    ON_COMMAND(ID_RIBBON_BTN_USRMGMT_REFRESH, &CUserManagerPage::OnRibbonBtnRefresh)

    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_USRMGMT_DEL, &CUserManagerPage::OnUpdate)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_USRMGMT_CREATE, &CUserManagerPage::OnUpdate)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_USRMGMT_REFRESH, &CUserManagerPage::OnUpdate)
END_MESSAGE_MAP()

/// CUserManagerPage Message Handle

int CUserManagerPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (!CreateRibbonButtons())
    {
        TRACE0("Failed to create user management ribbon buttons\n");
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

    // Create and Install the user list view pane
    right   = rect.Width() / 4;
    bottom  = rect.bottom;

    if (!m_wndUserList.Create(Translate(_T("User List View")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_USERLIST, WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create user list view pane\n");
        return -1;      // fail to create
    }
    m_wndUserList.EnableDocking(CBRS_LEFT);
    DockPane(&m_wndUserList);
    //m_wndUserList.DockToFrameWindow(CBRS_LEFT, CRect(left, top, right, bottom), DT_DOCK_LAST, this);

    // Create and Install the user editor pane
    left    = right;
    right   = rect.right;

    if (!m_wndUserEditor.Create(Translate(_T("User Editor")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_USEREDITOR, WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create user editor pane\n");
        return -1;      // fail to create
    }
    SetMainWnd(&m_wndUserEditor);

    return 0;
}


void CUserManagerPage::OnDestroy()
{
    CBaseControllerPage::OnDestroy();

}

void CUserManagerPage::Close()
{
    m_wndUserEditor.DestroyWindow();
    m_wndUserList.DestroyWindow();
}


BOOL CUserManagerPage::CreateRibbonButtons()
{
    if (!m_pCategory)
    {
        return FALSE;
    }

    /// Remove the dummy panel, and create the real panels
    m_pCategory->RemovePanel(0);
#ifndef PROFESSIONAL_EDITION
    CString strPanel            = Translate(_T("User"));

    CMFCRibbonPanel* pPanel = m_pCategory->AddPanel(strPanel);

    CMFCRibbonButton* pBtnCreate = new CMFCRibbonButton(ID_RIBBON_BTN_USRMGMT_CREATE, Translate(_T("Create")), 0, 0);
    pBtnCreate->SetAlwaysLargeImage();
	pBtnCreate->SetToolTipText(Translate(_T("Create")));
    pBtnCreate->SetDescription(Translate(_T("Create a new user.")));
    pPanel->Add(pBtnCreate);


/*    CMFCRibbonButton* pBtnEdit = new CMFCRibbonButton(ID_RIBBON_BTN_USRMGMT_EDIT, _T("Edit"), 1, 1);
    pBtnEdit->SetAlwaysLargeImage();
    pPanel->Add(pBtnEdit);

    CMFCRibbonButton* pBtnSave = new CMFCRibbonButton(ID_RIBBON_BTN_USRMGMT_SAVE, _T("Save"), 2, 2);
    pBtnSave->SetAlwaysLargeImage();
    pPanel->Add(pBtnSave);
    */
    CMFCRibbonButton* pBtnDel = new CMFCRibbonButton(ID_RIBBON_BTN_USRMGMT_DEL, Translate(_T("Delete")), 1, 1);
    pBtnDel->SetAlwaysLargeImage();
	pBtnDel->SetToolTipText(Translate(_T("Delete")));
    pBtnDel->SetDescription(Translate(_T("Delete the selected user.")));
    pPanel->Add(pBtnDel);
#endif

    CString strView = Translate(_T("View"));
    CMFCRibbonPanel* pRefreshPanel = m_pCategory->AddPanel(strView);

    CMFCRibbonButton* pBtnRefresh = new CMFCRibbonButton(ID_RIBBON_BTN_USRMGMT_REFRESH, Translate(_T("Refresh")), 2, 2);
    pBtnRefresh->SetAlwaysLargeImage();
    pBtnRefresh->SetToolTipText(Translate(_T("Refresh")));
    pBtnRefresh->SetDescription(Translate(_T("Refresh user list.")));
    pRefreshPanel->Add(pBtnRefresh);

    return TRUE;
}


void CUserManagerPage::OnLvnItemchangedListUser(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
    int iCurrent = pNMLV->iItem;

    TCHAR pContent[42] ;



    if(pNMLV->uNewState&LVIS_SELECTED)
    {
        m_wndUserList.m_wndComponentList.GetItemText(iCurrent,1,pContent,255);
        m_nCurUserId = _tstoi(pContent);
        m_wndUserEditor.SetCurrentUser(m_nCurUserId);
    }
    else
    {
        m_wndUserEditor.SetCurrentUser(-1);
        m_nCurUserId = -1;
    }
}

void CUserManagerPage::OnRibbonBtnCreate()
{
    CDialogNewUser dlg;
    if (IDOK  == dlg.DoModal())
    {
        m_wndUserList.RefreshUsers();
    }
}

void CUserManagerPage::OnRibbonBtnDel()
{
    // no user selected
    if (m_nCurUserId == -1)
        return;

    User user;
    GetControllerInterface->GetUserInfo(m_nCurUserId, user);

    if (MessageBox(Translate(_T("Are you sure to delete user?")), Translate(_T("Delete User")), MB_YESNO) != IDYES)
        return;

    if (S_OK == GetControllerInterface->DeleteUser(m_nCurUserId))
    {
        m_wndUserList.RefreshUsers();
        m_wndUserEditor.SetCurrentUser(-1);

        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)(Translate(_T("User Deleted:")).GetString() + user.GetAccount().GetUserName()).c_str());
    }
}

void CUserManagerPage::OnUpdateDel( CCmdUI* pCmdUI )
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    // only selected and admin can del user,
    // but admin can't delete himself
    pCmdUI->Enable(m_wndUserList.m_wndComponentList.GetSelectedCount() && m_nCurUserId != -1 &&
        GetControllerInterface->CheckPrivilege(UserPrivilege_DeleteUser) &&
        GetControllerInterface->GetUserId() != m_nCurUserId);
}
void CUserManagerPage::OnUpdateCreate( CCmdUI* pCmdUI )
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }
    // only admin can create user
    pCmdUI->Enable(GetControllerInterface->CheckPrivilege(UserPrivilege_CreateUser));
}


BOOL CUserManagerPage::RefreshPage()
{
    m_wndUserList.RefreshUsers();
    m_wndUserEditor.SetCurrentUser(-1);

    return TRUE;
}

void CUserManagerPage::OnUpdate(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }
    if (m_nCurUserId == S3NET_ADMIN_USER_ID)
    {
        pCmdUI->Enable(FALSE);
        return;
    }


    static std::map<int, std::pair<std::function<void (CCmdUI*)>, std::function<bool ()> > > dispatchMap;
    // check privilege
    if (dispatchMap.empty())
    {
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_USRMGMT_DEL, &CUserManagerPage::OnUpdateDel, LAMBDA(SC(DeleteUser)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_USRMGMT_CREATE, &CUserManagerPage::OnUpdateCreate, LAMBDA(SC(CreateUser)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_USRMGMT_REFRESH, &CUserManagerPage::OnUpdateRefresh, RET_TRUE());
    }

    auto it = dispatchMap.find(pCmdUI->m_nID), ite = dispatchMap.end();
    // id exist and have privilege
    if (it != ite && it->second.second())
    {
        it->second.first(pCmdUI);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }

}

void CUserManagerPage::OnUpdateRefresh(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

void CUserManagerPage::OnRibbonBtnRefresh()
{
    RefreshPage();
}
