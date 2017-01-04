// SearchLayoutDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Controller.h"
#include "SearchLayoutDlg.h"
#include "afxdialogex.h"
#include "DatetimeCheck.h"
#include "LayoutManagePage.h"
UINT LAYOUTDLG_SEARCH = ::RegisterWindowMessage (_T("LAYOUTDLG_SEARCH"));

// CSearchLayoutDlg 对话框

IMPLEMENT_DYNC_SINGLETON(LayoutSearchOptions)
IMPLEMENT_DYNAMIC(CSearchLayoutDlg, CDialogEx)

CSearchLayoutDlg::CSearchLayoutDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSearchLayoutDlg::IDD, pParent)
{

}

CSearchLayoutDlg::~CSearchLayoutDlg()
{
}

void CSearchLayoutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SEARCHDLG_LAYOUT_DESC, m_strDesc);
	DDX_Control(pDX, IDC_COMBO_SEARCHDLG_LAYOUT_GROUP, m_ctlGroup);
	DDX_Control(pDX, IDC_COMBO_SEARCHDLG_LAYOUT_OWNER, m_ctlowner);
	DDX_Control(pDX, IDC_COMBO_SEARCHDLG_LAYOUT_SIZE, m_ctlSize);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_START, m_timeStart);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_END, m_timeEnd);
	DDX_Control(pDX, IDC_COMBO_SEARCHDLG_LAYOUT_AUDIT, m_ctlAudit);
	DDX_Check(pDX, IDC_CHECK_SEARCHDLG_LAYOUT_DATE, m_bUseDate);
	DDX_Control(pDX, IDOK_LAYOUT_SEARCH, m_SearchBtn);
	if(pDX->m_bSaveAndValidate)
	{
        LayoutSearchOptions::Instance()->_AuditIDSel = m_ctlAudit.GetCurSel();
		LayoutSearchOptions::Instance()->_AuditID = m_ctlAudit.GetItemData(m_ctlAudit.GetCurSel());

        LayoutSearchOptions::Instance()->_GroupIDSel = m_ctlGroup.GetCurSel();
		LayoutSearchOptions::Instance()->_GroupID = m_ctlGroup.GetItemData(m_ctlGroup.GetCurSel());

        LayoutSearchOptions::Instance()->_SizeIDSel = m_ctlSize.GetCurSel();
		LayoutSearchOptions::Instance()->_SizeID     = m_ctlSize.GetItemData(m_ctlSize.GetCurSel());

        LayoutSearchOptions::Instance()->_OwnerIDSel = m_ctlowner.GetCurSel();
		LayoutSearchOptions::Instance()->_OwnerID    = m_ctlowner.GetItemData(m_ctlowner.GetCurSel());

		LayoutSearchOptions::Instance()->_StrDesc    = m_strDesc;
		LayoutSearchOptions::Instance()->_timeStart = m_timeStart;
		LayoutSearchOptions::Instance()->_timeEnd = m_timeEnd;
		LayoutSearchOptions::Instance()->_bUseDate = m_bUseDate;

	}
    else
    {
        m_strDesc = LayoutSearchOptions::Instance()->_StrDesc;
        m_timeStart = LayoutSearchOptions::Instance()->_timeStart;
        m_timeEnd = LayoutSearchOptions::Instance()->_timeEnd;
        m_bUseDate = LayoutSearchOptions::Instance()->_bUseDate;
    }

}
BEGIN_MESSAGE_MAP(CSearchLayoutDlg, CDialogEx)
	ON_BN_CLICKED(IDOK_LAYOUT_SEARCH, &CSearchLayoutDlg::OnClickedButtonSearch)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_START, &CSearchLayoutDlg::OnDtnDatetimechangeDatetimepickStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_END, &CSearchLayoutDlg::OnDtnDatetimechangeDatetimepickEnd)
    ON_BN_CLICKED(IDC_CHECK_SEARCHDLG_LAYOUT_DATE, &CSearchLayoutDlg::OnBnClickedCheckDate)
END_MESSAGE_MAP()


// CSearchLayoutDlg 消息处理程序
BOOL CSearchLayoutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_SearchBtn.SetImage(IDB_SEARCH);


	GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_END)->EnableWindow(TRUE);

#ifndef SERVER_EDITION
    GetDlgItem(IDC_COMBO_SEARCHDLG_LAYOUT_OWNER)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_COMBO_SEARCHDLG_LAYOUT_AUDIT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_COMBO_SEARCHDLG_LAYOUT_SIZE)->ShowWindow(SW_HIDE);
#endif

	m_timeStart = LayoutSearchOptions::Instance()->_timeStart;
	m_timeEnd = LayoutSearchOptions::Instance()->_timeEnd;


    UpdateGroup();
	UpdateOwner();
	UpdateSize();
	UpdateAudit();

    OnBnClickedCheckDate();
    if (m_bUseDate)
    {
        GetDlgItem(IDC_STATIC_SEARCHDLG_LAYOUT_START)->EnableWindow(TRUE);
        GetDlgItem(IDC_STATIC_SEARCHDLG_LAYOUT_END)->EnableWindow(TRUE);
        GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_START)->EnableWindow(TRUE);
        GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_END)->EnableWindow(TRUE);
    }
    else
    {
        GetDlgItem(IDC_STATIC_SEARCHDLG_LAYOUT_START)->EnableWindow(FALSE);
        GetDlgItem(IDC_STATIC_SEARCHDLG_LAYOUT_END)->EnableWindow(FALSE);
        GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_START)->EnableWindow(FALSE);
        GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_END)->EnableWindow(FALSE);
    }

	// sync data with GUI
	UpdateData(FALSE); 
	 
//	GetDlgItem(IDC_EDIT_SEARCHDLG_DESC)->SetFocus();
	
	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}
void CSearchLayoutDlg::OnClickedButtonSearch()
{
    UpdateData(TRUE);
    CWnd *pWnd = SearchWndObj(AfxGetMainWnd(),RUNTIME_CLASS(CLayoutManagePage));
    if (pWnd)
    {
         pWnd->SendMessage(LAYOUTDLG_SEARCH, 0, (LPARAM)this);
    }
    // search
    // It should be done in the page, not here
     CDialogEx::EndDialog(TRUE);
}
void CSearchLayoutDlg::RecalcDropWidth(CComboBox* pComboBox)
{
    // Reset the dropped width
    int nNumEntries = pComboBox->GetCount();
    int nWidth = 0;
    CString str;
    CClientDC dc(pComboBox);
    int nSave = dc.SaveDC();
    dc.SelectObject(pComboBox->GetFont());
    int nScrollWidth = ::GetSystemMetrics(SM_CXVSCROLL);
    for (int i = 0; i < nNumEntries; i++)
    {
        pComboBox->GetLBText(i, str);
        int nLength = dc.GetTextExtent(str).cx + nScrollWidth;
        nWidth = max(nWidth, nLength);
    }
    // Add margin space to the calculations
    nWidth += dc.GetTextExtent(_T("0")).cx;
    dc.RestoreDC(nSave);
    pComboBox->SetDroppedWidth(nWidth);
}
void CSearchLayoutDlg::UpdateGroup()
{
    m_ctlGroup.ResetContent();
    std::vector< pair<CString, LayoutGroup> > category;
    std::map<int, int> insertTip;
    LayoutGroup root;
    /////////////get all category
    HRESULT hr = GetControllerInterface->GetRootLayoutFilter(root);
    LayoutGroup *pRoot = &root;
    if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
    {
        GetControllerApp->OnControllerServerOffline();
        return;
    }
    category.clear();
    GetAllCategory(pRoot, category);
    ////////
    auto it = category.begin(), ite = category.end();

    for (int i = 0; it != ite; ++it, ++i)
    {
        it->first.Replace(_T("root"), Translate(_T("Layout Root Category")));
        m_ctlGroup.InsertString(i, it->first);
        int groupId = it->second.GetGroupID();
        m_ctlGroup.SetItemData(i, groupId);
        insertTip.insert(map<int, int>::value_type(groupId, i));
    }
    //Get select layoutfilterid

    CLayoutCategoryCtrl::ITEM_TYPE_LAYOUT ItemType;
    CWnd *pWnd = SearchWndObj(AfxGetMainWnd(),RUNTIME_CLASS(CLayoutCategoryCtrl));
    CLayoutCategoryCtrl* pCategory = (CLayoutCategoryCtrl*)pWnd;
    INT ID = pCategory->GetSelectedCategory(ItemType);

    if (ItemType == CLayoutCategoryCtrl::CATEGORY_INDEX)
    {
        LayoutGroup Category;
        HRESULT hr = GetControllerInterface->GetLayoutFilter(ID, Category);
        if (!SUCCEEDED(hr))
        {
            TRACE1("Failed to Get Layout Filter %d!\n", ID);
        }
        map<int, int>::iterator it = insertTip.find(ID);
        m_ctlGroup.SetCurSel(it->second);
    }
    else
    {
        m_ctlGroup.SetCurSel(LayoutSearchOptions::Instance()->_GroupIDSel);
    }

    RecalcDropWidth(&m_ctlGroup);
}

void CSearchLayoutDlg::UpdateOwner()
{
	m_ctlowner.ResetContent();
	std::vector<User> owner;
	HRESULT hr = GetControllerInterface->GetAllUsers(owner);
	if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
	{
		GetControllerApp->OnControllerServerOffline();
		return;
	}

	auto it = owner.begin(), ite = owner.end();

	m_ctlowner.InsertString(0, Translate(_T("All Owners")));
	m_ctlowner.SetItemData(0, -1);

	for (int i = 1; it != ite; ++it)
	{
		if(it->GetUserID() >= 0)
		{
			Account tmp = it->GetAccount();
			m_ctlowner.InsertString(i, tmp.GetUserName().c_str());
			m_ctlowner.SetItemData(i, it->GetUserID());
			++i;
		}
	}
	m_ctlowner.SetCurSel(LayoutSearchOptions::Instance()->_OwnerIDSel);

    RecalcDropWidth(&m_ctlowner);
}

void CSearchLayoutDlg::UpdateSize()
{
	m_ctlSize.ResetContent();
	m_ctlSize.InsertString(0, Translate(_T("All Sizes")));
	m_ctlSize.SetItemData(0, -1);
	m_ctlSize.InsertString(1, Translate(_T("Small(1-10MB)")));
	m_ctlSize.SetItemData(1, 0);
	m_ctlSize.InsertString(2, Translate(_T("Mediaum(10-16MB)")));
	m_ctlSize.SetItemData(2, 1);
	m_ctlSize.InsertString(3, Translate(_T("Large(16MB-128MB)")));
	m_ctlSize.SetItemData(3, 2);
	m_ctlSize.InsertString(4, Translate(_T("Huge(>128MB)")));
	m_ctlSize.SetItemData(4, 3);
	m_ctlSize.SetCurSel(LayoutSearchOptions::Instance()->_SizeIDSel);

    RecalcDropWidth(&m_ctlSize);
}

void CSearchLayoutDlg::UpdateAudit()
{
	m_ctlAudit.ResetContent();
	m_ctlAudit.InsertString(0, Translate(_T("All Status")));
	m_ctlAudit.SetItemData(0, -1);
	m_ctlAudit.InsertString(1, Translate(_T("Approved")));
	m_ctlAudit.SetItemData(1, 2);
	m_ctlAudit.InsertString(2, Translate(_T("Rejected")));
	m_ctlAudit.SetItemData(2, 1);
	m_ctlAudit.InsertString(3, Translate(_T("Requested")));
	m_ctlAudit.SetItemData(3, 0);
	m_ctlAudit.SetCurSel(LayoutSearchOptions::Instance()->_AuditIDSel);

    RecalcDropWidth(&m_ctlAudit);
}
void CSearchLayoutDlg::GetAllCategory(LayoutGroup* pRoot,std::vector< pair<CString, LayoutGroup> > &category, const CString& PreFix)
{
    if(pRoot)
    {
        category.push_back(make_pair(PreFix + pRoot->GetGroupName().c_str(), *pRoot));
        std::vector<LayoutGroup> subCategory;
        HRESULT hr = GetControllerInterface->GetSubLayoutFilters(*pRoot, subCategory);

        auto it = subCategory.begin(), ite = subCategory.end();
        for (; it != ite; it++)
        {
            GetAllCategory(&(*it), category, PreFix + pRoot->GetGroupName().c_str() + _T("\\"));
        }
    }
}

void CSearchLayoutDlg::OnEnChangeEditDesc()
{
    CString str;
    GetDlgItem(IDC_EDIT_DESC)->GetWindowText(str);
/*
    if (RemoveInterpunction(str))
    {
        MessageBox(Translate(_T("Interpunctions are not allowed")), Translate(_T("Error")), MB_OK|MB_ICONERROR);
        GetDlgItem(IDC_EDIT_DESC)->SetWindowText(str);
    }
    */
}
void CSearchLayoutDlg::OnDtnDatetimechangeDatetimepickEnd(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_START, IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_END, date, false);
    *pResult = 0;
}

void CSearchLayoutDlg::OnDtnDatetimechangeDatetimepickStart(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_START, IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_END, date, true);
    *pResult = 0;
}
void CSearchLayoutDlg::OnBnClickedCheckDate()
{
    // TODO: Add your control notification handler code here

    if (!IsDlgButtonChecked(IDC_CHECK_SEARCHDLG_LAYOUT_DATE))
    {
        GetDlgItem(IDC_STATIC_SEARCHDLG_LAYOUT_START)->EnableWindow(FALSE);
        GetDlgItem(IDC_STATIC_SEARCHDLG_LAYOUT_END)->EnableWindow(FALSE);
        GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_START)->EnableWindow(FALSE);
        GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_END)->EnableWindow(FALSE);
    }
    else
    {
        GetDlgItem(IDC_STATIC_SEARCHDLG_LAYOUT_START)->EnableWindow(TRUE);
        GetDlgItem(IDC_STATIC_SEARCHDLG_LAYOUT_END)->EnableWindow(TRUE);
        GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_START)->EnableWindow(TRUE);
        GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_LAYOUT_END)->EnableWindow(TRUE);
    }
}
