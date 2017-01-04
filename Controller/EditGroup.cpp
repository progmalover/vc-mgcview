// EditGroup.cpp : implementation file
//

#include "stdafx.h"
#include "ControllerClient/Models.h"
#include "EditGroup.h"
#include "afxdialogex.h"

// CEditGroup dialog

IMPLEMENT_DYNAMIC(CEditGroup, CDialogEx)

CEditGroup::CEditGroup(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEditGroup::IDD, pParent)
{

	m_GroupName = _T("");
	m_GroupDesc = _T("");
}

CEditGroup::~CEditGroup()
{
}

void CEditGroup::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SCHGROUP_DESC, m_GroupDesc);
	DDX_Text(pDX, IDC_SCHGROUP_NAME, m_GroupName);
}


BEGIN_MESSAGE_MAP(CEditGroup, CDialogEx)
	ON_BN_CLICKED(IDOK, &CEditGroup::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CEditGroup::InitGroup(INT GroupID)
{
	m_InitGroupID = GroupID;
	return TRUE;
}
// CEditGroup message handlers


BOOL CEditGroup::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	if (GetGroupInfo(m_CurGroup))
	{
		m_GroupName = Translate(m_CurGroup.GetGroupName().c_str());
		m_GroupDesc = Translate(m_CurGroup.GetGroupDesc().c_str());
		UpdateData(FALSE);
		if(m_CurGroup.GetGroupID() == 1)
		{
			((CEdit *)GetDlgItem(IDC_SCHGROUP_NAME))->SetReadOnly(TRUE);
		}
	}
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CEditGroup::GetGroupInfo(ClientGroup &GroupInfo)
{
	std::vector<ClientGroup> groups;
	HRESULT hr = GetControllerInterface->GetAllClientGroup(groups);
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

	for (std::vector<ClientGroup>::const_iterator it = groups.begin(); it != groups.end(); ++it)
	{
		if (it->GetGroupID() == m_InitGroupID)
		{
			GroupInfo = *it;
			break;
		}
	}
	return TRUE;
}

void CEditGroup::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_CurGroup.SetGroupDesc(m_GroupDesc.GetString());
	m_CurGroup.SetGroupName(m_GroupName.GetString());

	CDialogEx::OnOK();
}