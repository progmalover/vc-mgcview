// UserPrivilegeEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "Resource.h"
#include "UserPrivilegeEditDlg.h"
#include "SetPasswordDlg.h"
#include "UserManagerPage.h"
#include "Localization/Localization.h"
#include "ScrollHelper.h"

#include <boost/assign/std/set.hpp>

// CUserPrivilegeEditDlg dialog

static std::vector<std::pair<int, UserPrivilege> > UIPrivilege;
static std::map<int, std::set<int> > PrivRelation;


IMPLEMENT_DYNAMIC(CUserPrivilegeEditDlg, CDialogEx)

CUserPrivilegeEditDlg::CUserPrivilegeEditDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUserPrivilegeEditDlg::IDD, pParent)
{
    m_scrollHelper = new CScrollHelper;
    m_scrollHelper->AttachWnd(this);
//     m_scrollHelper->SetDialogUnitSize(357, 402);

}

CUserPrivilegeEditDlg::~CUserPrivilegeEditDlg()
{
    delete m_scrollHelper;
}

void CUserPrivilegeEditDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CUserPrivilegeEditDlg, CDialogEx)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BTN_SAVE, &CUserPrivilegeEditDlg::OnBnClickedButtonSave)
    ON_BN_CLICKED(IDC_BTN_SAVE_PRO, &CUserPrivilegeEditDlg::OnBnClickedButtonSave)
    ON_BN_CLICKED(IDC_BTN_SETPASS, &CUserPrivilegeEditDlg::OnBnClickedButtonSetPassword)
    ON_COMMAND_RANGE(IDC_CHK_MEDIA_FILTER_CREATE, IDC_CHK_LOGIN_CONTROLLER, &CUserPrivilegeEditDlg::OnCheckPrivilege)
    ON_BN_CLICKED(IDC_BUTTON_CHECKALL, &CUserPrivilegeEditDlg::OnBnClickedBtnCheckAll)
    ON_BN_CLICKED(IDC_BUTTON_UNCHECKALL, &CUserPrivilegeEditDlg::OnBnClickedBtnUncheckAll)
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
    ON_WM_MOUSEWHEEL()
	ON_EN_CHANGE(IDC_USER_DESC, &CUserPrivilegeEditDlg::OnEnChangeUserDesc)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CUserEditorCtrl message handlers
BOOL CUserPrivilegeEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    m_nUserId = -1;
#define UI_PRIVILEGE_ENTRY(id, prv) UIPrivilege.push_back(std::make_pair(id, prv))
    if (UIPrivilege.empty())
    {
        UI_PRIVILEGE_ENTRY(IDC_CHK_MEDIA_CREATE,            UserPrivilege_CreateMedia);
        UI_PRIVILEGE_ENTRY(IDC_CHK_MEDIA_DEL,               UserPrivilege_DeleteMedia);
        UI_PRIVILEGE_ENTRY(IDC_CHK_MEDIA_EDIT,              UserPrivilege_EditMedia);
        UI_PRIVILEGE_ENTRY(IDC_CHK_MEDIA_GET,               UserPrivilege_GetMedia);
        UI_PRIVILEGE_ENTRY(IDC_CHK_MEDIA_AUDIT,             UserPrivilege_ApproveMedia);
        UI_PRIVILEGE_ENTRY(IDC_CHK_MEDIA_RETIRE,            UserPrivilege_RetireMedia);

        UI_PRIVILEGE_ENTRY(IDC_CHK_MEDIA_FILTER_CREATE,     UserPrivilege_CreateMediaFilter);
        UI_PRIVILEGE_ENTRY(IDC_CHK_MEDIA_FILTER_DEL,        UserPrivilege_DeleteMediaFilter);
        UI_PRIVILEGE_ENTRY(IDC_CHK_MEDIA_FILTER_EDIT,       UserPrivilege_EditMediaFilter);
        UI_PRIVILEGE_ENTRY(IDC_CHK_MEDIA_FILTER_LIST,       UserPrivilege_ListMediaFilter);

        UI_PRIVILEGE_ENTRY(IDC_CHK_LAYOUT_CREATE,           UserPrivilege_PublishLayout);
        UI_PRIVILEGE_ENTRY(IDC_CHK_LAYOUT_DEL,              UserPrivilege_DeleteLayout);
//         UI_PRIVILEGE_ENTRY(IDC_CHK_LAYOUT_EDIT,             UserPrivilege_EditLayout);
        UI_PRIVILEGE_ENTRY(IDC_CHK_LAYOUT_GET,              UserPrivilege_GetLayout);
        UI_PRIVILEGE_ENTRY(IDC_CHK_LAYOUT_AUDIT,            UserPrivilege_ApproveLayoutOperation);

        UI_PRIVILEGE_ENTRY(IDC_CHK_LAYOUT_FILTER_CREATE,    UserPrivilege_CreateLayoutFilter);
        UI_PRIVILEGE_ENTRY(IDC_CHK_LAYOUT_FILTER_DEL,       UserPrivilege_DeleteLayoutFilter);
        UI_PRIVILEGE_ENTRY(IDC_CHK_LAYOUT_FILTER_EDIT,      UserPrivilege_EditLayoutFilter);
        UI_PRIVILEGE_ENTRY(IDC_CHK_LAYOUT_FILTER_LIST,      UserPrivilege_ListLayoutFilter);

        UI_PRIVILEGE_ENTRY(IDC_CHK_TIMETABLE_CREATE,        UserPrivilege_PublishTimetable);
        UI_PRIVILEGE_ENTRY(IDC_CHK_TIMETABLE_DEL,           UserPrivilege_DeleteTimetable);
//         UI_PRIVILEGE_ENTRY(IDC_CHK_TIMETABLE_EDIT,          UserPrivilege_EditTimetableFilter);
        UI_PRIVILEGE_ENTRY(IDC_CHK_TIMETABLE_GET,           UserPrivilege_GetTimetable);
        UI_PRIVILEGE_ENTRY(IDC_CHK_TIMETABLE_AUDIT,         UserPrivilege_ApproveTimetableOperation);

        UI_PRIVILEGE_ENTRY(IDC_CHK_TIMETABLE_FILTER_CREATE, UserPrivilege_CreateTimetableFilter);
        UI_PRIVILEGE_ENTRY(IDC_CHK_TIMETABLE_FILTER_DEL,    UserPrivilege_DeleteTimetableFilter);
        UI_PRIVILEGE_ENTRY(IDC_CHK_TIMETABLE_FILTER_EDIT,   UserPrivilege_EditTimetableFilter);
        UI_PRIVILEGE_ENTRY(IDC_CHK_TIMETABLE_FILTER_LIST,   UserPrivilege_ListTimetableFilter);

        UI_PRIVILEGE_ENTRY(IDC_CHK_SCHEDULE_CREATE,         UserPrivilege_PublishSchedule);
//         UI_PRIVILEGE_ENTRY(IDC_CHK_SCHEDULE_DEL,            UserPrivilege_DeleteSchedule);
//         UI_PRIVILEGE_ENTRY(IDC_CHK_SCHEDULE_EDIT,           UserPrivilege_EditSchedule);
        UI_PRIVILEGE_ENTRY(IDC_CHK_SCHEDULE_GET,            UserPrivilege_GetSchedule);
        UI_PRIVILEGE_ENTRY(IDC_CHK_SCHEDULE_AUDIT,          UserPrivilege_ApproveScheduleOperation);

//         UI_PRIVILEGE_ENTRY(IDC_CHK_SCHEDULE_FILTER_CREATE,  UserPrivilege_CreateScheduleFilter);
//         UI_PRIVILEGE_ENTRY(IDC_CHK_SCHEDULE_FILTER_DEL,     UserPrivilege_DeleteScheduleFilter);
//         UI_PRIVILEGE_ENTRY(IDC_CHK_SCHEDULE_FILTER_EDIT,    UserPrivilege_EditScheduleFilter);
//         UI_PRIVILEGE_ENTRY(IDC_CHK_SCHEDULE_FILTER_LIST,    UserPrivilege_ListScheduleFilter);

        UI_PRIVILEGE_ENTRY(IDC_CHK_EM_CREATE,               UserPrivilege_EmergencyMessageOperation);
//         UI_PRIVILEGE_ENTRY(IDC_CHK_EM_DEL,                  UserPrivilege_DeleteEmergencyMessage);
//         UI_PRIVILEGE_ENTRY(IDC_CHK_EM_EDIT,                 UserPrivilege_EditEmergencyMessage);
//         UI_PRIVILEGE_ENTRY(IDC_CHK_EM_GET,                  UserPrivilege_GetEmergencyMessage);
//         UI_PRIVILEGE_ENTRY(IDC_CHK_EM_AUDIT,                UserPrivilege_ApproveEmergencyMessageOperation);

        UI_PRIVILEGE_ENTRY(IDC_CHK_USER_CREATE,             UserPrivilege_CreateUser);
        UI_PRIVILEGE_ENTRY(IDC_CHK_USER_DEL,                UserPrivilege_DeleteUser);
        UI_PRIVILEGE_ENTRY(IDC_CHK_USER_EDIT,               UserPrivilege_EditUser);
        UI_PRIVILEGE_ENTRY(IDC_CHK_USER_LIST,               UserPrivilege_GetUser);

        UI_PRIVILEGE_ENTRY(IDC_CHK_CLIENT_CREATE,           UserPrivilege_CreateClient);
        UI_PRIVILEGE_ENTRY(IDC_CHK_CLIENT_DEL,              UserPrivilege_DeleteClient);
        UI_PRIVILEGE_ENTRY(IDC_CHK_CLIENT_EDIT,             UserPrivilege_EditClient);
        UI_PRIVILEGE_ENTRY(IDC_CHK_CLIENT_LIST,             UserPrivilege_GetClient);

        UI_PRIVILEGE_ENTRY(IDC_CHK_CLIENTGROUP_CREATE,      UserPrivilege_CreateClientGroup);
        UI_PRIVILEGE_ENTRY(IDC_CHK_CLIENTGROUP_DEL,         UserPrivilege_DeleteClientGroup);
        UI_PRIVILEGE_ENTRY(IDC_CHK_CLIENTGROUP_EDIT,        UserPrivilege_EditClientGroup);
        UI_PRIVILEGE_ENTRY(IDC_CHK_CLIENTGROUP_GET,         UserPrivilege_ListClientGroup);

        UI_PRIVILEGE_ENTRY(IDC_CHK_PRIVILEGE_EDIT,          UserPrivilege_EditPrivilege);
        UI_PRIVILEGE_ENTRY(IDC_CHK_REPORT_VIEW,             UserPrivilege_ListReport);
        UI_PRIVILEGE_ENTRY(IDC_CHK_LOG_VIEW,                UserPrivilege_ListLog);

        UI_PRIVILEGE_ENTRY(IDC_CHK_LOGIN_CLIENT,            UserPrivilege_StartClient);
        UI_PRIVILEGE_ENTRY(IDC_CHK_REG_CLIENT,              UserPrivilege_CreateClient);
        UI_PRIVILEGE_ENTRY(IDC_CHK_LOGIN_CONTROLLER,        UserPrivilege_StartController);
#undef UI_PRIVILEGE_ENTRY
    }

    if (PrivRelation.empty())
    {
        using namespace boost::assign;
#define SET_PRIV_RELATION_MAP(id, ...) PrivRelation[id] = std::set<int>(); PrivRelation[id] += __VA_ARGS__

        SET_PRIV_RELATION_MAP(IDC_CHK_MEDIA_CREATE,             IDC_CHK_MEDIA_GET, IDC_CHK_MEDIA_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_MEDIA_DEL,                IDC_CHK_MEDIA_GET, IDC_CHK_MEDIA_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_MEDIA_EDIT,               IDC_CHK_MEDIA_GET, IDC_CHK_MEDIA_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_MEDIA_GET,                IDC_CHK_MEDIA_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_MEDIA_AUDIT,              IDC_CHK_MEDIA_GET, IDC_CHK_MEDIA_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_MEDIA_RETIRE,             IDC_CHK_MEDIA_GET, IDC_CHK_MEDIA_FILTER_LIST);

        SET_PRIV_RELATION_MAP(IDC_CHK_MEDIA_FILTER_CREATE,      IDC_CHK_MEDIA_GET, IDC_CHK_MEDIA_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_MEDIA_FILTER_DEL,         IDC_CHK_MEDIA_EDIT, IDC_CHK_MEDIA_GET, IDC_CHK_MEDIA_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_MEDIA_FILTER_EDIT,        IDC_CHK_MEDIA_GET, IDC_CHK_MEDIA_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_MEDIA_FILTER_LIST,        IDC_CHK_MEDIA_GET);

        SET_PRIV_RELATION_MAP(IDC_CHK_LAYOUT_CREATE,            IDC_CHK_MEDIA_GET, IDC_CHK_MEDIA_FILTER_LIST, IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_LAYOUT_DEL,               IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_LAYOUT_GET,               IDC_CHK_LAYOUT_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_LAYOUT_AUDIT,             IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST);

        SET_PRIV_RELATION_MAP(IDC_CHK_LAYOUT_FILTER_CREATE,     IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_LAYOUT_FILTER_DEL,        IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_LAYOUT_FILTER_EDIT,       IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_LAYOUT_FILTER_LIST,       IDC_CHK_LAYOUT_GET);

        SET_PRIV_RELATION_MAP(IDC_CHK_TIMETABLE_CREATE,         IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST, IDC_CHK_TIMETABLE_GET, IDC_CHK_TIMETABLE_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_TIMETABLE_DEL,            IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST, IDC_CHK_TIMETABLE_GET, IDC_CHK_TIMETABLE_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_TIMETABLE_GET,            IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST, IDC_CHK_TIMETABLE_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_TIMETABLE_AUDIT,          IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST, IDC_CHK_TIMETABLE_GET, IDC_CHK_TIMETABLE_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_TIMETABLE_FILTER_CREATE,  IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST, IDC_CHK_TIMETABLE_GET, IDC_CHK_TIMETABLE_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_TIMETABLE_FILTER_DEL,     IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST, IDC_CHK_TIMETABLE_GET, IDC_CHK_TIMETABLE_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_TIMETABLE_FILTER_EDIT,    IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST, IDC_CHK_TIMETABLE_GET, IDC_CHK_TIMETABLE_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_TIMETABLE_FILTER_LIST,    IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST, IDC_CHK_TIMETABLE_GET);

        SET_PRIV_RELATION_MAP(IDC_CHK_SCHEDULE_CREATE,          IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST, IDC_CHK_TIMETABLE_GET, IDC_CHK_TIMETABLE_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_SCHEDULE_GET,             IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST, IDC_CHK_TIMETABLE_GET, IDC_CHK_TIMETABLE_FILTER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_SCHEDULE_AUDIT,           IDC_CHK_LAYOUT_GET, IDC_CHK_LAYOUT_FILTER_LIST, IDC_CHK_TIMETABLE_GET, IDC_CHK_TIMETABLE_FILTER_LIST);

        SET_PRIV_RELATION_MAP(IDC_CHK_EM_CREATE,                IDC_CHK_CLIENT_LIST, IDC_CHK_CLIENTGROUP_GET);

        SET_PRIV_RELATION_MAP(IDC_CHK_USER_CREATE,              IDC_CHK_USER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_USER_DEL,                 IDC_CHK_USER_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_USER_EDIT,                IDC_CHK_USER_LIST);
        //SET_PRIV_RELATION_MAP(IDC_CHK_USER_LIST,                );

        SET_PRIV_RELATION_MAP(IDC_CHK_CLIENT_CREATE,            IDC_CHK_CLIENT_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_CLIENT_DEL,               IDC_CHK_CLIENT_LIST);
        SET_PRIV_RELATION_MAP(IDC_CHK_CLIENT_EDIT,              IDC_CHK_CLIENT_LIST);
        //SET_PRIV_RELATION_MAP(IDC_CHK_CLIENT_LIST,              );

        SET_PRIV_RELATION_MAP(IDC_CHK_CLIENTGROUP_CREATE,       IDC_CHK_CLIENT_LIST, IDC_CHK_CLIENTGROUP_GET);
        SET_PRIV_RELATION_MAP(IDC_CHK_CLIENTGROUP_DEL,          IDC_CHK_CLIENT_LIST, IDC_CHK_CLIENTGROUP_GET);
        SET_PRIV_RELATION_MAP(IDC_CHK_CLIENTGROUP_EDIT,         IDC_CHK_CLIENT_LIST, IDC_CHK_CLIENTGROUP_GET);
        SET_PRIV_RELATION_MAP(IDC_CHK_CLIENTGROUP_GET,          IDC_CHK_CLIENT_LIST);

        //SET_PRIV_RELATION_MAP(IDC_CHK_PRIVILEGE_EDIT,           );
        //SET_PRIV_RELATION_MAP(IDC_CHK_REPORT_VIEW,              );
        //SET_PRIV_RELATION_MAP(IDC_CHK_LOG_VIEW,                 );
        //SET_PRIV_RELATION_MAP(IDC_CHK_LOGIN_CLIENT,             );
        //SET_PRIV_RELATION_MAP(IDC_CHK_REG_CLIENT,               );
        //SET_PRIV_RELATION_MAP(IDC_CHK_LOGIN_CONTROLLER,         );

#undef SET_PRIV_RELATION_MAP
    }

    DisableAllControls();

#ifndef SERVER_EDITION
    auto it = UIPrivilege.begin(), ite = UIPrivilege.end();
    for (; it != ite; ++it)
    {
        GetDlgItem(it->first)->ShowWindow(SW_HIDE);
    }

    GetDlgItem(IDC_BUTTON_CHECKALL)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BUTTON_UNCHECKALL)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GROUP_PRIVILAGES)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GROUP_MEDIA)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GROUP_LAYOUT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GROUP_TIMETABLE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GROUP_SCHEDULE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_MEDIA_CATEGORY)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_LAYOUT_CATEGORY)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_TIMETABLE_CATEGORY)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_SCHEDULE_CATEGORY)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GROUP_EDIT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GROUP_CLIENT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GROUP_CLIENT_GROUP)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GROUP_EMMERGENCY)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GROUP_VIEW)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GROUP_PRI)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GROUP_LOG)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BTN_SAVE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GROUP_LOG2)->ShowWindow(SW_HIDE);
#else
    GetDlgItem(IDC_BTN_SAVE_PRO)->ShowWindow(SW_HIDE);
#endif

    CRect rect;
    GetWindowRect(rect);
    m_scrollHelper->SetDisplaySize(rect.Width(), rect.Height());
	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CUserPrivilegeEditDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
    m_scrollHelper->OnSize(nType, cx, cy);
}

void CUserPrivilegeEditDlg::SetCurrentUser(int UserID)
{
    if (UserID < 0)
    {
        DisableAllControls();
        return;
    }
    else
    {
        GetDlgItem(IDC_USER_DESC)->EnableWindow(TRUE);
        //GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
        GetDlgItem(IDC_BTN_SETPASS)->EnableWindow(TRUE);
        GetDlgItem(IDC_BTN_SAVE_PRO)->EnableWindow(TRUE);

        auto it = UIPrivilege.begin(), ite = UIPrivilege.end();
        for (; it != ite; ++it)
        {
            ((CButton*)GetDlgItem(it->first))->SetCheck(FALSE);
        }
        if (UserID != S3NET_ADMIN_USER_ID && GetControllerInterface->CheckPrivilege(UserPrivilege_EditPrivilege))
        {
            for (it = UIPrivilege.begin(); it != ite; ++it)
            {
                GetDlgItem(it->first)->EnableWindow(TRUE);
            }
            GetDlgItem(IDC_BUTTON_CHECKALL)->EnableWindow(TRUE);
            GetDlgItem(IDC_BUTTON_UNCHECKALL)->EnableWindow(TRUE);
        }
    }
    m_nUserId = UserID;
    User u;
    HRESULT hr = GetControllerInterface->GetUserInfo(UserID, u);
    if (FAILED(hr))
    {
        MessageBox(Translate(_T("Failed to get user info, please try later.")),Translate(_T("Error:User privilege")), MB_OK|MB_ICONERROR);
        return;
    }
	m_description = u.GetDesc();
    GetDlgItem(IDC_USER_DESC)->SetWindowText(u.GetDesc().c_str());


    std::tstring errDesc;
    hr = GetControllerInterface->ListUserPrivilege(UserID, m_privileges);
    if (FAILED(hr))
    {
        int err = GetControllerInterface->GetLastError(errDesc);
        if (err == S3NET_HAVE_NO_PERMISSION)
        {
            MessageBox(Translate(_T("You don't have privilege to list user's privilege.")),Translate(_T("Warning:User privilege")), MB_OK|MB_ICONEXCLAMATION);
            return;
        }
    }

    m_unSavedPrivileges = m_privileges;
    // Set privilege
    SetPrivilege(m_privileges);
}

void CUserPrivilegeEditDlg::OnBnClickedButtonSave()
{
    if (m_nUserId < 0)
    {
        return;
    }

	HRESULT hr;
	User user;
	hr = GetControllerInterface->GetUserInfo(m_nUserId, user);
	CString desc;
	GetDlgItem(IDC_USER_DESC)->GetWindowText(desc);
	user.SetDesc(desc.GetString());
	hr = GetControllerInterface->EditUser(user);
	if (FAILED(hr))
	{
		std::tstring errDesc;
		int err = GetControllerInterface->GetLastError(errDesc);
		errDesc = Translate(_T("Edit user description failed: ")) + Translate(errDesc.c_str());
		MessageBox(errDesc.c_str(),Translate(_T("Error:User privilege")), MB_OK|MB_ICONERROR);

		return;
	}
	m_description = user.GetDesc();

#ifdef SERVER_EDITION
	hr = GetControllerInterface->EditUserPrivileges(m_nUserId, m_privileges);
	if (FAILED(hr))
	{
		std::tstring errDesc;
		int err = GetControllerInterface->GetLastError(errDesc);
		errDesc = Translate(_T("Edit user privilege failed: ")) + Translate(errDesc.c_str());
		MessageBox(errDesc.c_str(),Translate(_T("Error:User privilege")), MB_OK|MB_ICONERROR);
	}

	m_unSavedPrivileges = m_privileges;
	MessageBox(Translate(_T("User information and privileges saved.")),Translate(_T("Tips:User privilege")),MB_OK);
#else
	MessageBox(Translate(_T("User description saved.")),Translate(_T("Tips:User privilege")),MB_OK);
#endif

    GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
}



void CUserPrivilegeEditDlg::OnOK()
{
    // NOTICE: can't call dialog OnOK
    // if CDialogEx::OnOK been called, this dialog will be destroyed
}


void CUserPrivilegeEditDlg::OnCancel()
{
    // NOTICE: can't call dialog OnCancel
    // if CDialogEx::OnCancel been called, this dialog will be destroyed
}

void CUserPrivilegeEditDlg::SetPrivilege(const std::set<UserPrivilege>& prvs)
{
    auto it = prvs.begin(), ite = prvs.end();
    for (; it != ite; ++it)
    {
        auto it1 = UIPrivilege.begin(), ite1 = UIPrivilege.end();
        for (; it1 != ite1; ++it1)
        {
            if (it1->second == *it)
            {
                ((CButton*)GetDlgItem(it1->first))->SetCheck(TRUE);
            }
        }
    }
}

void CUserPrivilegeEditDlg::OnCheckPrivilege(UINT id)
{
    BOOL checked = ((CButton*)GetDlgItem(id))->GetCheck();
    if (checked)
    {
        auto it = UIPrivilege.begin(), ite = UIPrivilege.end();
        for (; it != ite; ++it)
        {
            if (it->first == id)
            {
                m_privileges.insert(it->second);
            }
        }

        // auto check related privilege
        const std::set<int>& relatedPriv = PrivRelation[id];
        for (auto it = relatedPriv.begin(); it != relatedPriv.end(); ++it)
        {
            // check all related check boxes
            ((CButton*)GetDlgItem(*it))->SetCheck(BST_CHECKED);
            // add all related privileges
            auto it1 = UIPrivilege.begin(), ite1 = UIPrivilege.end();
            for (; it1 != ite1; ++it1)
            {
                if (it1->first == *it)
                {
                    m_privileges.insert(it1->second);
                }
            }

        }
    }
    else
    {
        auto it = UIPrivilege.begin(), ite = UIPrivilege.end();
        for (; it != ite; ++it)
        {
            if (it->first == id)
            {
                m_privileges.erase(it->second);
            }
        }
    }
	if(NeedSave() || IsUserDescChanged())
	{
		GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
	}
}

void CUserPrivilegeEditDlg::DisableAllControls()
{
    GetDlgItem(IDC_USER_DESC)->SetWindowText(_T(""));
    GetDlgItem(IDC_USER_DESC)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_SETPASS)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_CHECKALL)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_UNCHECKALL)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_SAVE_PRO)->EnableWindow(FALSE);

    auto it = UIPrivilege.begin(), ite = UIPrivilege.end();
    for (; it != ite; ++it)
    {
        ((CButton*)GetDlgItem(it->first))->SetCheck(FALSE);
        GetDlgItem(it->first)->EnableWindow(FALSE);
    }
}

void CUserPrivilegeEditDlg::OnBnClickedButtonSetPassword()
{
    CSetPasswordDlg dlg;

    if(dlg.DoModal() == IDOK)
    {
        User UserInfo;
        GetControllerInterface->GetUserInfo(m_nUserId, UserInfo);
        Account UserAccout;
        UserAccout = UserInfo.GetAccount();

        Account NewUserAccount(UserAccout.GetUserName(), dlg.m_Password.GetString());
        NewUserAccount.HashPassword();
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
                GetControllerInterface->SetPassword(dlg.m_Password.GetString());

                GetControllerApp->Relogin();
            }

            MessageBox(Translate(_T("User Password Changed")),Translate(_T("Tips:User privilege")),MB_OK);
        }
        else
        {
            std::tstring ErrorMessage;
            GetControllerInterface->GetLastError(ErrorMessage);

            MessageBox(Translate(ErrorMessage.c_str()),Translate(_T("Error:User privilege")), MB_OK|MB_ICONERROR);
        }
    }
}



void CUserPrivilegeEditDlg::OnBnClickedBtnCheckAll()
{
    auto it = UIPrivilege.begin(), ite = UIPrivilege.end();
    for (; it != ite; ++it)
    {
        ((CButton*)GetDlgItem(it->first))->SetCheck(TRUE);
        m_privileges.insert(it->second);
    }
	if(NeedSave() || IsUserDescChanged())
	{
		GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
	}
}


void CUserPrivilegeEditDlg::OnBnClickedBtnUncheckAll()
{
    auto it = UIPrivilege.begin(), ite = UIPrivilege.end();
    for (; it != ite; ++it)
    {
        ((CButton*)GetDlgItem(it->first))->SetCheck(FALSE);
        m_privileges.erase(it->second);
    }
	if(NeedSave() || IsUserDescChanged())
	{
		GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
	}
}

bool CUserPrivilegeEditDlg::NeedSave()
{
    return m_privileges != m_unSavedPrivileges;
}

bool CUserPrivilegeEditDlg::IsUserDescChanged()
{
	CString desc;
	GetDlgItem(IDC_USER_DESC)->GetWindowText(desc);
	CString tmp = m_description.c_str();

	if(desc != tmp)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void CUserPrivilegeEditDlg::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
    m_scrollHelper->OnHScroll(nSBCode, nPos, pScrollBar);
}

void CUserPrivilegeEditDlg::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
    m_scrollHelper->OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CUserPrivilegeEditDlg::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
    return m_scrollHelper->OnMouseWheel(nFlags, zDelta, pt);
}



void CUserPrivilegeEditDlg::OnEnChangeUserDesc()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if(NeedSave() || IsUserDescChanged())
	{
		GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
	}
}
