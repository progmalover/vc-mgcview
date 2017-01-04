// F:\p4\sw\Demo\ContentManagement\Controller\User Management\UserListPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "UserListPane.h"
#include "UserManagerPage.h"

// CUserListPane

IMPLEMENT_DYNAMIC(CUserListPane, CBaseControllerPane)

CUserListPane::CUserListPane()
{

}

CUserListPane::~CUserListPane()
{
}


BEGIN_MESSAGE_MAP(CUserListPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CUserListPane message handlers
int CUserListPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMinSize(CSize(150, 365));

    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    RECT rect;
    GetClientRect(&rect);

    m_wndComponentList.Create(WS_CHILD|WS_VISIBLE|LVS_ICON|LVS_SINGLESEL|LVS_AUTOARRANGE, rect, this, IDW_LIST_USER);
    m_wndComponentList.SetExtendedStyle(m_wndComponentList.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

    // set image lists
    //m_LargeImageList.Create(IDB_USER_LARGE, 64, ILC_COLOR24, RGB(0, 0, 0));
    //m_LargeImageList.SetOverlayImage(2, 1);
    m_LargeImageList.Create(64, 64, ILC_COLOR32, 2, 1);
    m_LargeImageList.SetImageCount(2);

    CBitmap bitmap;
    bitmap.LoadBitmap(IDB_USER_ADMIN_LARGE);
    m_LargeImageList.Replace(0, &bitmap, NULL);
    bitmap.Detach();
    bitmap.LoadBitmap(IDB_USER_NORMAL_LARGE);
    m_LargeImageList.Replace(1, &bitmap, NULL);

    m_wndComponentList.SetImageList(&m_LargeImageList, LVSIL_NORMAL);

    SetMainWnd(&m_wndComponentList);

    m_wndComponentList.InsertColumn(0,_T("Name"));
    m_wndComponentList.InsertColumn(1,_T("ID"));
    m_wndComponentList.InsertColumn(2,_T("Description"));

    m_wndComponentList.SetColumnWidth(0,70);
    m_wndComponentList.SetColumnWidth(1,30);
    m_wndComponentList.SetColumnWidth(2,100);



    RefreshUsers();

    return 0;
}

void CUserListPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

void CUserListPane::RefreshUsers()
{
    HRESULT hr;

    m_wndComponentList.SetItemCount(65535);
    m_wndComponentList.DeleteAllItems();

    m_wndComponentList.m_item = -1;

    if (/*GetControllerInterface->CheckPrivilege(UserPrivilege_ListUser) &&*/
        GetControllerInterface->CheckPrivilege(UserPrivilege_GetUser))
    {
        std::vector<User> UserList;

        hr = GetControllerInterface->GetAllUsers(UserList);
        if (FAILED(hr))
        {
            return;
        }

        auto it = UserList.begin(), ite = UserList.end();
        int i = 0;
        for(; it != ite; ++it )
        {
            if (it->GetUserID() < 0)
            {
                continue;
            }
            LV_ITEM lvi;
            CString name = it->GetAccount().GetUserName().c_str();

#ifndef SERVER_EDITION
            if (name == _T("admin"))
            {

            }
            else
#endif
            {
                lvi.mask = LVIF_IMAGE | LVIF_TEXT;
                lvi.iItem = i;
                lvi.iSubItem = 0;
                lvi.pszText = (LPWSTR)name.GetString();

                if (it->GetUserID() == S3NET_ADMIN_USER_ID)
                {
                    lvi.iImage = 0;
                }
                else
                {
                    lvi.iImage = 1;
                }

                m_wndComponentList.InsertItem(&lvi);

                CString sID;

                sID.Format(_T("%d"), it->GetUserID());
                m_wndComponentList.SetItemText(i, 1, sID);
                m_wndComponentList.SetItemText(i, 2, it->GetDesc().c_str());

                ++i;
            }
        }
    }
    else
    {
        User user;
        hr = GetControllerInterface->GetUserInfo(GetControllerInterface->GetUserId(), user);
        if (FAILED(hr))
        {
            return;
        }

        CString name = user.GetAccount().GetUserName().c_str();

#ifndef SERVER_EDITION
        if (name == _T("admin"))
        {

        }
        else
#endif
        {

            LV_ITEM lvi;
            lvi.mask = LVIF_IMAGE | LVIF_TEXT;
            lvi.iItem = m_wndComponentList.GetItemCount();
            lvi.iSubItem = 0;
            lvi.pszText = (LPWSTR)name.GetString();
            lvi.iImage = 1;
            m_wndComponentList.InsertItem(&lvi);

            CString sID;

            sID.Format(_T("%d"), user.GetUserID());
            m_wndComponentList.SetItemText(0, 1, sID);
            m_wndComponentList.SetItemText(0, 2, user.GetDesc().c_str());

        }
    }
}

BEGIN_MESSAGE_MAP(CUserListCtrl, CMFCListCtrl)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CUserListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifdef SERVER_EDITION
    int item = HitTest(point);
    CUserPrivilegeEditDlg& privilegeDlg = ((CUserManagerPage*)(GetParent()->GetParent()))->m_wndUserEditor.m_UserEditor;
    if (item != m_item)
    {
        if (m_item != -1 && (privilegeDlg.NeedSave() || privilegeDlg.IsUserDescChanged()))
        {
            if (MessageBox(Translate(_T("You have changed user privilege, do you want to discard the changes?")),Translate(_T("Tips:UserList pane")), MB_YESNO|MB_TOPMOST) == IDYES)
            {
                m_item = item;
                CMFCListCtrl::OnLButtonDown(nFlags, point);
            }
        }
        else
        {
            m_item = item;
            CMFCListCtrl::OnLButtonDown(nFlags, point);
        }
    }
#else
    CMFCListCtrl::OnLButtonDown(nFlags, point);
#endif
}

CUserListCtrl::CUserListCtrl()
{
    m_item = -1;
}

CUserListCtrl::~CUserListCtrl()
{

}
