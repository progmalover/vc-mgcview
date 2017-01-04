// UserEditorCtrl.cpp : implementation file
// This file is deprecated by UserPrivilegeEditDlg.h/cpp

#include "stdafx.h"
#include "Controller.h"
#include "UserEditorCtrl.h"
#include "SetPasswordDlg.h"
#include "afxdialogex.h"
#include "UserManagerPage.h"
#include "Localization/Localization.h"

// CUserEditorCtrl dialog

IMPLEMENT_DYNAMIC(CUserEditorCtrl, CDialogEx)

CUserEditorCtrl::CUserEditorCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUserEditorCtrl::IDD, pParent)
{
    m_UserID = -1;
}

CUserEditorCtrl::~CUserEditorCtrl()
{
}

void CUserEditorCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_USERNAME, m_Username);
    DDX_Text(pDX, IDC_EDIT_USERDESC, m_UserDesc);
    DDX_Radio(pDX, IDC_RADIO_EDIT_USER_PRIVILAGE, m_nPrivilage);
}


BEGIN_MESSAGE_MAP(CUserEditorCtrl, CDialogEx)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &CUserEditorCtrl::OnBnClickedButtonSave)
    ON_BN_CLICKED(IDC_BUTTON_SETPASS, &CUserEditorCtrl::OnBnClickedButtonSetpass)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CUserEditorCtrl)
	EASYSIZE(IDC_EDIT_USERNAME,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_EDIT_USERDESC,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_SAVE,IDC_BUTTON_SETPASS,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_HCENTER)
    EASYSIZE(IDC_BUTTON_SETPASS,IDC_STATIC_USER_PRIV,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
END_EASYSIZE_MAP


// CUserEditorCtrl message handlers
BOOL CUserEditorCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    INIT_EASYSIZE;

    //GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
    //GetDlgItem(IDC_BUTTON_SETPASS)->EnableWindow(FALSE);
    //GetDlgItem(IDC_EDIT_USERDESC)->EnableWindow(FALSE);

    GetDlgItem(IDC_RADIO_EDIT_USER_PRIVILAGE)->EnableWindow(FALSE);
    GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
    GetDlgItem(IDC_RADIO3)->EnableWindow(FALSE);
    GetDlgItem(IDC_RADIO4)->EnableWindow(FALSE);

#ifndef SERVER_EDITION
    GetDlgItem(IDC_RADIO2)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_RADIO3)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_RADIO4)->ShowWindow(SW_HIDE);
    CWnd* theFrame = GetDlgItem(IDC_FRAME);
    CRect rect;
    theFrame->GetWindowRect(rect);
    ScreenToClient(rect);
    rect.bottom = rect.top + rect.Height()/2;
    rect.right = rect.left + rect.Width()/2;
    theFrame->MoveWindow(rect);

#endif

    SetCurrentUser(-1);

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CUserEditorCtrl::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

void CUserEditorCtrl::SetCurrentUser(int UserID)
{
    // only admin can change user's permission type,
    // and everyone can't change permission type of himself

    if (UserID >= 0 &&
//         GetControllerInterface->GetPermissionType() == User::ADMINACCOUNT &&
        GetControllerInterface->GetUserId() != UserID)
    {
        GetDlgItem(IDC_RADIO_EDIT_USER_PRIVILAGE)->EnableWindow(TRUE);
        GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
        GetDlgItem(IDC_RADIO3)->EnableWindow(TRUE);
        GetDlgItem(IDC_RADIO4)->EnableWindow(TRUE);
    }
    else
    {
        GetDlgItem(IDC_RADIO_EDIT_USER_PRIVILAGE)->EnableWindow(FALSE);
        GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
        GetDlgItem(IDC_RADIO3)->EnableWindow(FALSE);
        GetDlgItem(IDC_RADIO4)->EnableWindow(FALSE);
    }


    //
    if (UserID >= 0)
    {
        m_UserID = UserID;

        GetDlgItem(IDC_EDIT_USERDESC)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_SETPASS)->EnableWindow(TRUE);

        User UserInfo;
        GetControllerInterface->GetUserInfo(m_UserID, UserInfo);

        m_Username = UserInfo.GetAccount().GetUserName().c_str();
        m_UserDesc = UserInfo.GetDesc().c_str();
        m_nPrivilage = UserInfo.GetPermissionType() - 1;

        UpdateData(FALSE);
    }
    // no user selected
    else
    {

        GetDlgItem(IDC_EDIT_USERDESC)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_SETPASS)->EnableWindow(FALSE);

        m_Username.Empty();
        m_UserDesc.Empty();
        m_nPrivilage = - 1;

        UpdateData(FALSE);
    }
}

void CUserEditorCtrl::OnBnClickedButtonSave()
{
    User UserInfo;
    GetControllerInterface->GetUserInfo(m_UserID, UserInfo);

    UpdateData();

//     UserInfo.SetPermissionType(static_cast<User::PermissionType>(m_nPrivilage+1));
    UserInfo.SetDesc(m_UserDesc.GetString());
    HRESULT hr = GetControllerInterface->EditUser(UserInfo);

    if (hr == S_OK)
    {
        MessageBox(Translate(_T("User Information updated")), Translate(_T("Information")), MB_OK);
    }
    else
    {
        std::tstring ErrorMessage;
        GetControllerInterface->GetLastError(ErrorMessage);

        MessageBox(Translate(ErrorMessage.c_str()), Translate(_T("Error")), MB_OK);
    }
    ((CUserManagerPage*)(GetParent()->GetParent()))->m_wndUserList.RefreshUsers();
}


void CUserEditorCtrl::OnBnClickedButtonSetpass()
{
    CSetPasswordDlg dlg;

    if(dlg.DoModal() == IDOK)
    {
        User UserInfo;
        GetControllerInterface->GetUserInfo(m_UserID, UserInfo);
        Account UserAccout;
        UserAccout = UserInfo.GetAccount();

        Account NewUserAccount(UserAccout.GetUserName(), dlg.m_Password.GetString());
        UserInfo.SetNewAccount(NewUserAccount);

        HRESULT hr = GetControllerInterface->EditUser(UserInfo);

        if(hr == S_OK)
        {
            CString ThisUser = UserAccout.GetUserName().c_str();
            ThisUser.MakeLower();
            CString CurUser = GetControllerApp->m_Username;
            CurUser.MakeLower();
            if (CurUser == ThisUser)
            {
                GetControllerApp->m_Password = dlg.m_Password;

                GetControllerApp->Relogin();
            }

            MessageBox(Translate(_T("User Password Changed")), Translate(_T("Information")), MB_OK);
        }else
        {
            std::tstring ErrorMessage;
            GetControllerInterface->GetLastError(ErrorMessage);

            MessageBox(Translate(ErrorMessage.c_str()), Translate(_T("Error")), MB_OK);
        }
    }
}


void CUserEditorCtrl::OnOK()
{
    // NOTICE: can't call dialog OnOK
    // if CDialogEx::OnOK been called, this dialog will be destroyed
}


void CUserEditorCtrl::OnCancel()
{
    // NOTICE: can't call dialog OnCancel
    // if CDialogEx::OnCancel been called, this dialog will be destroyed
}
