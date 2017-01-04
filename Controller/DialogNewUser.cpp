// DialogNewUser.cpp : 实现文件
//

#include "stdafx.h"
#include "Controller.h"
#include "DialogNewUser.h"
#include "afxdialogex.h"
#include "Localization/Localization.h"

#define  CLIENT_NAME (_T("Client"))
#define  ADMIN_NAME (_T("Administrator"))
#define  DESIGNER_NAME (_T("Designer"))
#define  SCHEDULER_NAME (_T("Scheduler"))
#define  AUDITOR_NAME (_T("Auditor"))
// CDialogNewUser 对话框

IMPLEMENT_DYNAMIC(CDialogNewUser, CDialogEx)

CDialogNewUser::CDialogNewUser(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogNewUser::IDD, pParent)
{

    m_strUserName = _T("");
    m_strPassword = _T("");
    m_strDescription = _T("");
}

CDialogNewUser::~CDialogNewUser()
{
}

void CDialogNewUser::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    //  DDX_Control(pDX, IDC_COMBO_NEW_USER_CHARACTER, m_ctlCharater);
    //  DDX_Control(pDX, IDC_EDIT_NEW_USER_NAME, m_strUserName);
    DDX_Text(pDX, IDC_EDIT_NEW_USER_NAME, m_strUserName);
    DDV_MaxChars(pDX, m_strUserName, 255);
    DDX_Text(pDX, IDC_EDIT_NEW_USER_PASSWORD, m_strPassword);
    DDV_MaxChars(pDX, m_strPassword, 255);
    DDX_Text(pDX, IDC_EDIT_NEW_USER_PASSWORD2, m_strPassword2);
    DDV_MaxChars(pDX, m_strPassword2, 255);
    DDX_Text(pDX, IDC_EDIT_NEW_USER_DESCRIPTION, m_strDescription);
    DDV_MaxChars(pDX, m_strDescription, 255);
    DDX_Control(pDX, IDC_COMBO_NEW_USER_CHARACTER, m_ctlCharater);
}


BEGIN_MESSAGE_MAP(CDialogNewUser, CDialogEx)

    ON_BN_CLICKED(IDOK, &CDialogNewUser::OnBnClickedOk)
    ON_WM_CREATE()

END_MESSAGE_MAP()


// CDialogNewUser 消息处理程序

void CDialogNewUser::OnBnClickedOk()
{
    HRESULT hr;
    int i = 0;
    UpdateData();

	m_strUserName.TrimLeft(_T(" "));
	m_strUserName.TrimRight(_T(" "));
     CString::PCXSTR interpunction;
     interpunction = _T("~!#$%^&()_+=-[]{};.'@*:\"/|<>?\\");
    // check user name and password exists
    if (m_strUserName.IsEmpty())
    {
        MessageBox(Translate(_T("User name can't be empty")), Translate(_T("Error:New user")), MB_ICONERROR);
        return;
    }
    while ((i = m_strUserName.FindOneOf(interpunction)) != -1)
    {
        MessageBox(Translate(_T("User name contains invalid character(s).")),Translate(_T("Warning:New user")),MB_OK|MB_ICONEXCLAMATION);
        return;
    }
    if (m_strPassword.IsEmpty())
    {
        MessageBox(Translate(_T("Password can't be empty")), Translate(_T("Error:New user")), MB_ICONERROR);
        return;
    }
    if (m_strPassword != m_strPassword2)
    {
        MessageBox(Translate(_T("Password and Confirm Password are not match")), Translate(_T("Error:New user")), MB_ICONERROR);
        return;
    }
    // check user name
    std::vector<User> UserList;
    hr = GetControllerInterface->GetAllUsers(UserList);
    if (hr != S_OK)
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
        }
        else
        {
            MessageBox(Translate(_T("Check user name failed")), Translate(_T("Error:New user")), MB_ICONERROR);
        }
        return;
    }
    auto it = UserList.begin(), ite = UserList.end();
    for (; it != ite; ++it)
    {
        if (it->GetAccount().GetUserName() == tstring(m_strUserName.GetString()))
        {
            MessageBox(Translate(_T("User already exists, choose another name please")), Translate(_T("Error:New user")), MB_OK|MB_ICONERROR);
            return;
        }
    }
    Account account(m_strUserName.GetString(), m_strPassword.GetString());


    User NewUser(-1, static_cast<User::PermissionType>(m_ctlCharater.GetItemData(m_ctlCharater.GetCurSel())),
        m_strDescription.GetString(), account);
    hr = GetControllerInterface->AddUser(NewUser, NewUser);
    if (hr != S_OK)
    {
        tstring ErrorDesc;
        GetControllerInterface->GetLastError(ErrorDesc);
        MessageBox(Translate(ErrorDesc.c_str()), Translate(_T("Error:New user")), MB_ICONERROR);
        return;
    }



    CDialogEx::OnOK();
}

int CDialogNewUser::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialogEx::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}


BOOL CDialogNewUser::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    int index;
    index = m_ctlCharater.AddString(CLIENT_NAME);
    m_ctlCharater.SetItemData(index, static_cast<DWORD_PTR>(User::CLIENTACCOUNT));

#ifdef SERVER_EDITION
    index = m_ctlCharater.AddString(ADMIN_NAME);
    m_ctlCharater.SetItemData(index, static_cast<DWORD_PTR>(User::ADMINACCOUNT));
    index = m_ctlCharater.AddString(DESIGNER_NAME);
    m_ctlCharater.SetItemData(index, static_cast<DWORD_PTR>(User::DESIGNERACCOUNT));
    index = m_ctlCharater.AddString(SCHEDULER_NAME);
    m_ctlCharater.SetItemData(index, static_cast<DWORD_PTR>(User::SCHEDULERACCOUNT));
    index = m_ctlCharater.AddString(AUDITOR_NAME);
    m_ctlCharater.SetItemData(index, static_cast<DWORD_PTR>(User::AUDITOR_ACCOUNT));
#endif

    m_ctlCharater.SetCurSel(0);

	Translate(this);
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}
