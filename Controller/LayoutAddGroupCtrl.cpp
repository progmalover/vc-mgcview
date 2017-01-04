#include "stdafx.h"
#include "Controller.h"
#include "LayoutAddGroupCtrl.h"
#include "afxdialogex.h"
#include "Localization/Localization.h"

IMPLEMENT_DYNAMIC(CLayoutAddGroupCtrl, CDialogEx)

CLayoutAddGroupCtrl::CLayoutAddGroupCtrl(CWnd* pParent /*=NULL*/)
    : CDialogEx(CLayoutAddGroupCtrl::IDD, pParent)
{
    m_szGroupName = _T("");
    m_szGroupDesc = _T("");
}

CLayoutAddGroupCtrl::~CLayoutAddGroupCtrl()
{
}

void CLayoutAddGroupCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

    DDX_Text(pDX, IDC_GROUP_NAME, m_szGroupName);
    DDX_Text(pDX, IDC_GROUP_DESC, m_szGroupDesc);
}


BEGIN_MESSAGE_MAP(CLayoutAddGroupCtrl, CDialogEx)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDOK, &CLayoutAddGroupCtrl::OnBnClickedOk)
    ON_EN_CHANGE(IDC_GROUP_NAME, &CLayoutAddGroupCtrl::OnEnChangeEditName)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CLayoutAddGroupCtrl)

END_EASYSIZE_MAP


// CScheduleItemCtrl message handlers
BOOL CLayoutAddGroupCtrl::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    INIT_EASYSIZE;

	Translate(this);
    return TRUE;  // return TRUE  unless you set the focus to a control
}


void CLayoutAddGroupCtrl::OnSize(UINT nType, int cx, int cy) 
{
    CDialogEx::OnSize(nType, cx, cy);
    UPDATE_EASYSIZE;
}

void CLayoutAddGroupCtrl::OnEnChangeEditName()
{
    HRESULT hr;
    int groupID = -1;

    GetDlgItem(IDC_GROUP_NAME)->GetWindowText(m_szGroupName);

    // Search the group name in the DB, if exists, do not insert it
    std::vector<LayoutGroup> LayoutGroupList;
    hr = GetControllerInterface->GetAllLayoutGroup(LayoutGroupList);
    if (hr != S_OK)
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
        }
        else
        {
            MessageBox(Translate(_T("Get group name failed!")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
        }
        return;
    }

    auto it = LayoutGroupList.begin(), ite = LayoutGroupList.end();
    for (; it != ite; ++it)
    {
        if (it->GetGroupName() == tstring(m_szGroupName.GetString()))
        {
            m_szGroupDesc = it->GetGroupDesc().c_str();
            GetDlgItem(IDC_GROUP_DESC)->SetWindowText(m_szGroupDesc);
        }
    }
}

void CLayoutAddGroupCtrl::OnBnClickedOk()
{
    HRESULT hr;
    UpdateData();
    int groupID = -1;

    // check if the group name exists
    if (m_szGroupName.IsEmpty())
    {
        MessageBox(Translate(_T("Group name can't be empty!")), Translate(_T("Error:Designer page")),  MB_OK|MB_ICONERROR);
        return;
    }

    // Search the group name in the DB, if exists, do not insert it
    std::vector<LayoutGroup> LayoutGroupList;
    hr = GetControllerInterface->GetAllLayoutGroup(LayoutGroupList);
    if (hr != S_OK)
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
        }
        else
        {
            MessageBox(Translate(_T("Get group name failed!")), Translate(_T("Error:Designer page")),  MB_OK|MB_ICONERROR);
        }
        return;
    }

    auto it = LayoutGroupList.begin(), ite = LayoutGroupList.end();
    for (; it != ite; ++it)
    {
        if (it->GetGroupName() == tstring(m_szGroupName.GetString()))
        {
            if (MessageBox(Translate(_T("The group name already exists, would you want to overwrite the existing group with current one?")),
                Translate(_T("Warning")), MB_OKCANCEL) == IDCANCEL)
            {
                return;
            }
            groupID = it->GetGroupID();
        }
    }

    LayoutGroup group(groupID, m_szGroupName.GetString(), m_szGroupDesc.GetString());
    if (groupID == -1)
    {
        hr = GetControllerInterface->NewLayoutGroup(m_szGroupName.GetString(), m_szGroupDesc.GetString(), -1, group);
    }
    else
    {
        hr = GetControllerInterface->EditLayoutGroup(group);
    }

    if (hr != S_OK)
    {
        tstring ErrorDesc;
        GetControllerInterface->GetLastError(ErrorDesc);
        MessageBox(Translate(ErrorDesc.c_str()), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
        return;
    }

    CDialogEx::OnOK();
}
