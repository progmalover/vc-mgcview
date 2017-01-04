// GroupEditor.cpp : ????????
//

#include "stdafx.h"
#include "Controller.h"
#include "GroupEditor.h"
#include "afxdialogex.h"
#include "LayoutAddGroupCtrl.h"
#include "PropertyLayout.h"
#include "Localization/Localization.h"

// CGroupEditor ???бу?и░

IMPLEMENT_DYNAMIC(CGroupEditor, CDialogEx)

CGroupEditor::CGroupEditor(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGroupEditor::IDD, pParent)
	, m_szCategoryDesc(_T(""))
{
    m_pLayoutGroup = NULL;
    m_layoutInfo   = NULL;
    m_pPropertyLayout = NULL;
    m_nSelected = -1;
}

CGroupEditor::~CGroupEditor()
{
}

void CGroupEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GROUP, m_wndGroupList);
	DDX_Text(pDX, IDC_CATEGORY_DESC, m_szCategoryDesc);
}


BEGIN_MESSAGE_MAP(CGroupEditor, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_NEW_GROUP, &CGroupEditor::OnBnClickedButtonNewGroup)
    ON_BN_CLICKED(IDC_BUTTON_DELETE_GROUP, &CGroupEditor::OnBnClickedButtonDeleteGroup)
    ON_LBN_SELCHANGE(IDC_LIST_GROUP, &CGroupEditor::OnLbnSelchangeListGroup)
END_MESSAGE_MAP()

void CGroupEditor::SetContent(LayoutElement* layoutInfo, LAYOUTGROUP_LIST* groups, CPropertyLayout* p)
{
    m_pLayoutGroup    = groups;
    m_layoutInfo      = layoutInfo;
    m_pPropertyLayout = p;
}

BOOL CGroupEditor::OnInitDialog()
{
    CDialogEx::OnInitDialog();

	m_szCategoryDesc = _T("");

    UpdateGroupList();

	Translate(this);
    return TRUE;
}


// CGroupEditor


void CGroupEditor::OnBnClickedButtonNewGroup()
{
    std::vector<LayoutGroup>    LayoutGroupList;

    CLayoutAddGroupCtrl AddLayoutDlg;
    if (AddLayoutDlg.DoModal() == IDOK)
    {
        if (GetControllerInterface->GetAllLayoutGroup(LayoutGroupList) != S_OK)
        {
            MessageBox(Translate(_T("Get group name failed!")), Translate(_T("Error:Edit group")), MB_ICONERROR|MB_OK);
        }
        else
        {
            m_pLayoutGroup->RemoveAll();

            auto it = LayoutGroupList.begin(), ite = LayoutGroupList.end();
            for (; it != ite; ++it)
            {
                LAYOUTGROUP_SETTING* pGroup = new LAYOUTGROUP_SETTING();
                pGroup->GroupID     = it->GetGroupID();
                pGroup->GroupName   = it->GetGroupName().c_str();
				pGroup->GroupDesc   = it->GetGroupDesc().c_str();
                m_pLayoutGroup->AddTail(pGroup);
            }

            m_pPropertyLayout->SetContent(m_layoutInfo, m_pLayoutGroup);

            GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
                (LPARAM)Translate(_T("Layout Groups Added!")).GetString());
        }
    }

    UpdateGroupList();
}


void CGroupEditor::OnBnClickedButtonDeleteGroup()
{
    if (m_nSelected != -1)
    {
        LAYOUTGROUP_SETTING* layoutGroup = m_pLayoutGroup->GetAt(m_pLayoutGroup->FindIndex(m_nSelected));
       
		CString strName;
		strName.Format(_T("\"%s\"?"), layoutGroup->GroupName);
		 CString s = Translate(_T("Are you sure to delete group")) + strName;
       
        if (MessageBox(s, Translate(_T("Delete Group")), MB_YESNO) == IDYES)
        {
            m_pLayoutGroup->RemoveAt(m_pLayoutGroup->FindIndex(m_nSelected));
            HRESULT hr = GetControllerInterface->DeleteLayoutGroup(m_wndGroupList.GetItemData(m_nSelected));
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                GetControllerApp->OnControllerServerOffline();
                return;
            }

            UpdateGroupList();

			m_szCategoryDesc = _T("");
			UpdateData(FALSE);
        }
    }
}


void CGroupEditor::OnLbnSelchangeListGroup()
{
    m_nSelected = m_wndGroupList.GetCurSel();

	m_szCategoryDesc = m_GroupDescMap[m_wndGroupList.GetItemData(m_nSelected)];

	UpdateData(FALSE);
}

void CGroupEditor::UpdateGroupList()
{
    m_wndGroupList.ResetContent();
    CRect rect;
    m_wndGroupList.GetClientRect(&rect);
    m_wndGroupList.SetColumnWidth(rect.Width());
	m_GroupDescMap.clear();

    if (m_layoutInfo && m_pLayoutGroup)
    {
        int selectedGroupIndex;
        for (int i = 0; i < m_pLayoutGroup->GetCount(); ++i)
        {
            LAYOUTGROUP_SETTING* layoutSetting = m_pLayoutGroup->GetAt(m_pLayoutGroup->FindIndex(i));
            m_wndGroupList.InsertString(i, layoutSetting->GroupName.GetString());
            m_wndGroupList.SetItemData(i, layoutSetting->GroupID);
			m_GroupDescMap[layoutSetting->GroupID] = layoutSetting->GroupDesc;
            if (m_layoutInfo->GetGroupID() == layoutSetting->GroupID)
            {
                selectedGroupIndex = i;
            }
        }
    }
}
