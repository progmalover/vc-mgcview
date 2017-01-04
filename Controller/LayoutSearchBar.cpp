// MediaLibrarySearchBarCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "LayoutSearchBar.h"
#include "Localization/Localization.h"
#include "DatetimeCheck.h"
UINT LAYOUT_LIST_SEARCH = ::RegisterWindowMessage (_T("LAYOUT_LIST_SEARCH"));
UINT LAYOUTSEARCH_BUTTON = ::RegisterWindowMessage(_T("LAYOUTSEARCH_BUTTON"));

// CMediaLibrarySearchBarCtrl dialog

IMPLEMENT_DYNAMIC(CLayoutSearchBarCtrl, CDialogEx)

CLayoutSearchBarCtrl::CLayoutSearchBarCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLayoutSearchBarCtrl::IDD, pParent)
{

}

CLayoutSearchBarCtrl::~CLayoutSearchBarCtrl()
{
}

void CLayoutSearchBarCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DESC, m_strDesc);
	DDX_Control(pDX, IDC_COMBO_GROUP, m_ctlCategory);
	DDX_Control(pDX, IDC_COMBO_OWNER, m_ctlowner);
	DDX_Control(pDX, IDC_COMBO_SIZE, m_ctlSize);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_MEDIA_START, m_timeStart);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_MEDIA_END, m_timeEnd);
	DDX_Control(pDX, IDC_COMBO_APPROVAL, m_CtlApproval);
	DDX_Check(pDX, IDC_CHECK_DATE, m_bUseDate);
	DDX_Control(pDX, IDC_BUTTON_SEARCH, m_SearchBtn);
}


BEGIN_MESSAGE_MAP(CLayoutSearchBarCtrl, CDialogEx)
	ON_WM_SIZE()
    ON_WM_WINDOWPOSCHANGED()
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CLayoutSearchBarCtrl::OnClickedButtonSearch)
	ON_EN_CHANGE(IDC_EDIT_DESC, &CLayoutSearchBarCtrl::OnEnChangeEditDesc)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CLayoutSearchBarCtrl::OnCbnSelchangeCombo2)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_MEDIA_START, &CLayoutSearchBarCtrl::OnDtnDatetimechangeDatetimepickStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_MEDIA_END, &CLayoutSearchBarCtrl::OnDtnDatetimechangeDatetimepickEnd)
    ON_BN_CLICKED(IDC_CHECK_DATE, &CLayoutSearchBarCtrl::OnBnClickedCheckDate)
    ON_CBN_DROPDOWN(IDC_COMBO_GROUP, &CLayoutSearchBarCtrl::OnCbnDropdownComboGroup)
    ON_CBN_CLOSEUP(IDC_COMBO_GROUP, &CLayoutSearchBarCtrl::OnCbnCloseupComboGroup)
    ON_REGISTERED_MESSAGE(LAYOUTSEARCH_BUTTON, &CLayoutSearchBarCtrl::OnLayoutSearchButton)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CLayoutSearchBarCtrl)
	EASYSIZE(IDC_COMBO_GROUP,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_COMBO_OWNER,IDC_COMBO_GROUP,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_COMBO_SIZE,IDC_COMBO_OWNER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_COMBO_APPROVAL,IDC_COMBO_SIZE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_FRAME,IDC_COMBO_APPROVAL,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_CHECK_DATE,IDC_COMBO_APPROVAL,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_START,IDC_CHECK_DATE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_DATETIMEPICKER_MEDIA_START,IDC_STATIC_START,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_END,IDC_DATETIMEPICKER_MEDIA_START,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_DATETIMEPICKER_MEDIA_END,IDC_STATIC_END,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_EDIT_DESC,IDC_STATIC_FRAME,ES_BORDER,IDC_BUTTON_SEARCH,ES_KEEPSIZE,0)
    EASYSIZE(IDC_BUTTON_SEARCH,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
END_EASYSIZE_MAP


// CMediaLibrarySearchBarCtrl message handlers
BOOL CLayoutSearchBarCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_SearchBtn.SetImage(IDB_SEARCH);

    m_dpi.Attach(AfxFindResourceHandle(MAKEINTRESOURCE(IDD), RT_DIALOG), m_hWnd, IDD);

    

    INIT_EASYSIZE;

	GetDlgItem(IDC_DATETIMEPICKER_MEDIA_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_DATETIMEPICKER_MEDIA_END)->EnableWindow(TRUE);
    GetDlgItem(IDC_COMBO_MEDIATYPE)->ShowWindow(SW_HIDE);

#ifndef SERVER_EDITION
    GetDlgItem(IDC_COMBO_OWNER)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_APPROVAL)->EnableWindow(FALSE);
#endif

#ifdef STARTER_EDITION
    GetDlgItem(IDC_COMBO_SIZE)->EnableWindow(FALSE);
#endif
	m_timeStart = COleDateTime::GetCurrentTime();
	m_timeEnd = COleDateTime::GetCurrentTime();


    UpdateCategory();
	UpdateOwner();
	UpdateSize();
	UpdateApproval();

    OnBnClickedCheckDate();

	// sync data with GUI
	UpdateData(FALSE); 
	 
	
	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CLayoutSearchBarCtrl::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;
}


void CLayoutSearchBarCtrl::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
    CDialogEx::OnWindowPosChanged(lpwndpos);

    UPDATE_EASYSIZE;
}

void CLayoutSearchBarCtrl::OnClickedButtonSearch()
{
    UpdateData(TRUE);
    GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);
    // search
    // It should be done in the page, not here

    GetParent()->SendMessage(LAYOUT_LIST_SEARCH);
}

LRESULT CLayoutSearchBarCtrl::OnLayoutSearchButton(WPARAM, LPARAM)
{
    GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(TRUE);
    return S_OK;
}

void CLayoutSearchBarCtrl::RecalcDropWidth(CComboBox* pComboBox)
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

void CLayoutSearchBarCtrl::UpdateCategory()
{
    m_ctlCategory.ResetContent();
    std::vector< pair<CString, LayoutGroup> > category;
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
        m_ctlCategory.InsertString(i, it->first);
        m_ctlCategory.SetItemData(i, it->second.GetGroupID());
    }

    m_ctlCategory.SetCurSel(0);

    RecalcDropWidth(&m_ctlCategory);
}

void CLayoutSearchBarCtrl::UpdateOwner()
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
	m_ctlowner.SetCurSel(0);

    RecalcDropWidth(&m_ctlowner);
}

void CLayoutSearchBarCtrl::UpdateSize()
{
    m_ctlSize.ResetContent();
    m_ctlSize.InsertString(0, Translate(_T("All Sizes")));
    m_ctlSize.SetItemData(0, -1);
    m_ctlSize.InsertString(1, Translate(_T("Small (1-10MB)")));
    m_ctlSize.SetItemData(1, 0);
    m_ctlSize.InsertString(2, Translate(_T("Medium (10-16MB)")));
    m_ctlSize.SetItemData(2, 1);
    m_ctlSize.InsertString(3, Translate(_T("Large (16-128MB)")));
    m_ctlSize.SetItemData(3, 2);
    m_ctlSize.InsertString(4, Translate(_T("Huge (>128MB)")));
    m_ctlSize.SetItemData(4, 3);
    m_ctlSize.SetCurSel(0);

    RecalcDropWidth(&m_ctlSize);
}

void CLayoutSearchBarCtrl::UpdateApproval()
{
	m_CtlApproval.ResetContent();
	m_CtlApproval.InsertString(0, Translate(_T("All Status")));
	m_CtlApproval.SetItemData(0, -1);
	m_CtlApproval.InsertString(1, Translate(_T("Approved")));
	m_CtlApproval.SetItemData(1, 2);
	m_CtlApproval.InsertString(2, Translate(_T("Rejected")));
	m_CtlApproval.SetItemData(2, 1);
	m_CtlApproval.InsertString(3, Translate(_T("Requested")));
	m_CtlApproval.SetItemData(3, 0);
	m_CtlApproval.SetCurSel(0);

    RecalcDropWidth(&m_CtlApproval);
}

void CLayoutSearchBarCtrl::GetAllCategory(LayoutGroup* pRoot,std::vector< pair<CString, LayoutGroup> > &category, const CString& PreFix)
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

void CLayoutSearchBarCtrl::OnEnChangeEditDesc()
{
    CString str;
    GetDlgItem(IDC_EDIT_DESC)->GetWindowText(str);
}


void CLayoutSearchBarCtrl::OnDtnDatetimechangeDatetimepickEnd(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_MEDIA_START, IDC_DATETIMEPICKER_MEDIA_END, date, false);
    *pResult = 0;
}

void CLayoutSearchBarCtrl::OnDtnDatetimechangeDatetimepickStart(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_MEDIA_START, IDC_DATETIMEPICKER_MEDIA_END, date, true);
    *pResult = 0;
}


void CLayoutSearchBarCtrl::OnCbnSelchangeCombo2()
{
	// TODO: Add your control notification handler code here
}



void CLayoutSearchBarCtrl::OnBnClickedCheckDate()
{
    // TODO: Add your control notification handler code here

    if (!IsDlgButtonChecked(IDC_CHECK_DATE))
    {
        GetDlgItem(IDC_STATIC_START)->EnableWindow(FALSE);
        GetDlgItem(IDC_STATIC_END)->EnableWindow(FALSE);
        GetDlgItem(IDC_DATETIMEPICKER_MEDIA_START)->EnableWindow(FALSE);
        GetDlgItem(IDC_DATETIMEPICKER_MEDIA_END)->EnableWindow(FALSE);
    }
    else
    {
        GetDlgItem(IDC_STATIC_START)->EnableWindow(TRUE);
        GetDlgItem(IDC_STATIC_END)->EnableWindow(TRUE);
        GetDlgItem(IDC_DATETIMEPICKER_MEDIA_START)->EnableWindow(TRUE);
        GetDlgItem(IDC_DATETIMEPICKER_MEDIA_END)->EnableWindow(TRUE);
    }
}


void CLayoutSearchBarCtrl::OnCbnDropdownComboGroup()
{
    // TODO: Add your control notification handler code here
    UpdateCategory();
}


void CLayoutSearchBarCtrl::OnCbnCloseupComboGroup()
{
    // TODO: Add your control notification handler code here

    m_ctlCategory.Clear();

    m_ctlCategory.InsertString(0, Translate(_T("Updating......")));

}
