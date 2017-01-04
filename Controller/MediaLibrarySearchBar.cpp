// MediaLibrarySearchBarCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "MediaLibrarySearchBar.h"
#include "Localization/Localization.h"
#include "DatetimeCheck.h"
UINT MEDIALIB_SEARCH = ::RegisterWindowMessage (_T("MEDIALIB_SEARCH"));
UINT MEDIASEARCH_BUTTON= ::RegisterWindowMessage (_T("MEDIASEARCH_BUTTON"));
// CMediaLibrarySearchBarCtrl dialog

IMPLEMENT_DYNAMIC(CMediaLibrarySearchBarCtrl, CDialogEx)

CMediaLibrarySearchBarCtrl::CMediaLibrarySearchBarCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMediaLibrarySearchBarCtrl::IDD, pParent)
{

}

CMediaLibrarySearchBarCtrl::~CMediaLibrarySearchBarCtrl()
{
}

void CMediaLibrarySearchBarCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_DESC, m_strDesc);
    DDX_Control(pDX, IDC_COMBO_GROUP, m_ctlCategory);
    DDX_Control(pDX, IDC_COMBO_OWNER, m_ctlowner);
    DDX_Control(pDX, IDC_COMBO_SIZE, m_ctlSize);
    DDX_Control(pDX, IDC_COMBO_APPROVAL, m_CtlApproval);
    DDX_Control(pDX, IDC_COMBO_MEDIATYPE, m_CtlMediaType);
	DDX_Control(pDX, IDC_BUTTON_SEARCH,m_SearchBtn);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_MEDIA_START, m_timeStart);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_MEDIA_END, m_timeEnd);
    DDX_Check(pDX, IDC_CHECK_DATE, m_bUseDate);
}


BEGIN_MESSAGE_MAP(CMediaLibrarySearchBarCtrl, CDialogEx)
    ON_WM_SIZE()
    ON_WM_WINDOWPOSCHANGED()
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CMediaLibrarySearchBarCtrl::OnClickedButtonSearch)
	ON_EN_CHANGE(IDC_EDIT_DESC, &CMediaLibrarySearchBarCtrl::OnEnChangeEditDesc)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CMediaLibrarySearchBarCtrl::OnCbnSelchangeCombo2)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_MEDIA_START, &CMediaLibrarySearchBarCtrl::OnDtnDatetimechangeDatetimepickStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_MEDIA_END, &CMediaLibrarySearchBarCtrl::OnDtnDatetimechangeDatetimepickEnd)
    ON_BN_CLICKED(IDC_CHECK_DATE, &CMediaLibrarySearchBarCtrl::OnBnClickedCheckDate)
    ON_CBN_DROPDOWN(IDC_COMBO_GROUP, &CMediaLibrarySearchBarCtrl::OnCbnDropdownComboGroup)
    ON_CBN_CLOSEUP(IDC_COMBO_GROUP, &CMediaLibrarySearchBarCtrl::OnCbnCloseupComboGroup)

    ON_REGISTERED_MESSAGE(MEDIASEARCH_BUTTON, &CMediaLibrarySearchBarCtrl::OnMediaLibSearchButton)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CMediaLibrarySearchBarCtrl)
	EASYSIZE(IDC_COMBO_GROUP,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_COMBO_OWNER,IDC_COMBO_GROUP,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_COMBO_SIZE,IDC_COMBO_OWNER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_COMBO_APPROVAL,IDC_COMBO_SIZE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_COMBO_MEDIATYPE, IDC_COMBO_APPROVAL,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_FRAME,IDC_COMBO_MEDIATYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_CHECK_DATE,IDC_COMBO_APPROVAL,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_START,IDC_CHECK_DATE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_DATETIMEPICKER_MEDIA_START,IDC_STATIC_START,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_END,IDC_DATETIMEPICKER_MEDIA_START,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_DATETIMEPICKER_MEDIA_END,IDC_STATIC_END,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_EDIT_DESC,IDC_STATIC_FRAME,ES_BORDER,IDC_BUTTON_SEARCH,ES_KEEPSIZE,0)
    EASYSIZE(IDC_BUTTON_SEARCH,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
END_EASYSIZE_MAP


// CMediaLibrarySearchBarCtrl message handlers
BOOL CMediaLibrarySearchBarCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_SearchBtn.SetImage(IDB_SEARCH);
    m_dpi.Attach(AfxFindResourceHandle(MAKEINTRESOURCE(IDD), RT_DIALOG), m_hWnd, IDD);

    INIT_EASYSIZE;

	GetDlgItem(IDC_DATETIMEPICKER_MEDIA_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_DATETIMEPICKER_MEDIA_END)->EnableWindow(TRUE);

#ifndef SERVER_EDITION
    GetDlgItem(IDC_COMBO_OWNER)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_APPROVAL)->EnableWindow(FALSE);
#endif


	m_timeStart = COleDateTime::GetCurrentTime();
	m_timeEnd = COleDateTime::GetCurrentTime();


    UpdateCategory();
	UpdateOwner();
	UpdateSize();
	UpdateApproval();
    UpdateMediaType();

    OnBnClickedCheckDate();

	// sync data with GUI
	UpdateData(FALSE);
    UntranslateWnd(m_CtlMediaType.GetSafeHwnd());

	Translate(this);

    // privilege checker
    if (!CP(UserPrivilege_GetMedia) ||
        !CP(UserPrivilege_ListMediaFilter))
    {
        GetDlgItem(IDC_DATETIMEPICKER_MEDIA_START)->EnableWindow(FALSE);
        GetDlgItem(IDC_DATETIMEPICKER_MEDIA_END)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);
    }

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CMediaLibrarySearchBarCtrl::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;
}

void CMediaLibrarySearchBarCtrl::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
    CDialogEx::OnWindowPosChanged(lpwndpos);

    UPDATE_EASYSIZE;
}


void CMediaLibrarySearchBarCtrl::OnClickedButtonSearch()
{
    UpdateData(TRUE);
    GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);
    // search
    // It should be done in the page, not here

    GetParent()->SendMessage(MEDIALIB_SEARCH);
}

LRESULT CMediaLibrarySearchBarCtrl::OnMediaLibSearchButton(WPARAM, LPARAM)
{
    GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(TRUE);
    return S_OK;
}

void CMediaLibrarySearchBarCtrl::RecalcDropWidth(CComboBox* pComboBox)
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

void CMediaLibrarySearchBarCtrl::UpdateCategory()
{
    m_ctlCategory.ResetContent();
    std::vector< pair<CString, MediaFilter> > category;
    MediaFilter root;
    /////////////get all category
    HRESULT hr = GetControllerInterface->GetRootMediaFilter(root);
    MediaFilter *pRoot = &root;
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
        m_ctlCategory.SetItemData(i, it->second.GetMediaFilterID());
    }

    m_ctlCategory.SetCurSel(0);

    RecalcDropWidth(&m_ctlCategory);
}

void CMediaLibrarySearchBarCtrl::UpdateOwner()
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

void CMediaLibrarySearchBarCtrl::UpdateSize()
{
	m_ctlSize.ResetContent();
	m_ctlSize.InsertString(0, Translate(_T("All Sizes")));
	m_ctlSize.SetItemData(0, -1);
	m_ctlSize.InsertString(1, Translate(_T("Empty (0KB)")));
	m_ctlSize.SetItemData(1, 0);
	m_ctlSize.InsertString(2, Translate(_T("Tiny (0-1MB)")));
	m_ctlSize.SetItemData(2, 1);
	m_ctlSize.InsertString(3, Translate(_T("Small (1-128MB)")));
	m_ctlSize.SetItemData(3, 2);
	m_ctlSize.InsertString(4, Translate(_T("Medium (128-512MB)")));
	m_ctlSize.SetItemData(4, 3);
	m_ctlSize.InsertString(5, Translate(_T("Large (512MB-1GB)")));
	m_ctlSize.SetItemData(5, 4);
	m_ctlSize.InsertString(6, Translate(_T("Huge (1-4GB)")));
	m_ctlSize.SetItemData(6, 5);
    m_ctlSize.InsertString(7, Translate(_T("Gigantic (>4GB)")));
    m_ctlSize.SetItemData(7, 6);
	m_ctlSize.SetCurSel(0);

    RecalcDropWidth(&m_ctlSize);
}

void CMediaLibrarySearchBarCtrl::UpdateMediaType()
{
    m_CtlMediaType.ResetContent();
	m_CtlMediaType.InsertString(0, Translate(_T("All Media Types")));
	m_CtlMediaType.SetItemData(0, -1);
	m_CtlMediaType.InsertString(1, _T("AVI"));
	m_CtlMediaType.SetItemData(1, 0);
    m_CtlMediaType.InsertString(2, _T("Bitmap"));
    m_CtlMediaType.SetItemData(2, 1);
    m_CtlMediaType.InsertString(3, _T("bmp"));
    m_CtlMediaType.SetItemData(3, 2);
    m_CtlMediaType.InsertString(4, _T("GIF"));
    m_CtlMediaType.SetItemData(4, 3);
    m_CtlMediaType.InsertString(5, _T("JPEG"));
    m_CtlMediaType.SetItemData(5, 4);
    m_CtlMediaType.InsertString(6, _T("MPEG"));
    m_CtlMediaType.SetItemData(6, 5);
    m_CtlMediaType.InsertString(7, _T("PNG"));
    m_CtlMediaType.SetItemData(7, 6);
    m_CtlMediaType.InsertString(8, _T("PPS"));
    m_CtlMediaType.SetItemData(8, 7);
    m_CtlMediaType.InsertString(9, _T("PPT"));
    m_CtlMediaType.SetItemData(9, 8);
    m_CtlMediaType.InsertString(10, _T("TIFF"));
    m_CtlMediaType.SetItemData(10, 9);
    m_CtlMediaType.InsertString(11, _T("URL"));
    m_CtlMediaType.SetItemData(11, 10);
    m_CtlMediaType.InsertString(12, _T("Wave"));
    m_CtlMediaType.SetItemData(12, 11);
    m_CtlMediaType.InsertString(13, _T("Windows Media"));
    m_CtlMediaType.SetItemData(13, 12);
    m_CtlMediaType.SetCurSel(0);

    RecalcDropWidth(&m_CtlMediaType);
}

void CMediaLibrarySearchBarCtrl::UpdateApproval()
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
    m_CtlApproval.InsertString(4, Translate(_T("Retired")));
    m_CtlApproval.SetItemData(4, 3);
	m_CtlApproval.SetCurSel(0);

    RecalcDropWidth(&m_CtlApproval);
}

void CMediaLibrarySearchBarCtrl::GetAllCategory(MediaFilter* pRoot,std::vector< pair<CString, MediaFilter> > &category, const CString& PreFix)
{
    if(pRoot)
    {
        category.push_back(make_pair(PreFix + pRoot->GetMediaFilterName().c_str(), *pRoot));
        std::vector<MediaFilter> subCategory;
        HRESULT hr = GetControllerInterface->GetSubMediaFilters(*pRoot, subCategory);

        auto it = subCategory.begin(), ite = subCategory.end();
        for (; it != ite; it++)
        {
            GetAllCategory(&(*it), category, PreFix + pRoot->GetMediaFilterName().c_str() + _T("\\"));
        }
    }
}
/*
static bool RemoveInterpunction(CString &s)
{
    const CString::PCXSTR interpunction = _T("`!@#$%^&*()+-[]{}\\;:'\",./|=<>?~¡«£à£¡£À££¡ç£¥¦ä£¦£ª£¨£©£­£«£½£Û£Ý£û£ý£Ü£ü£º£»¡®¡¯¡°¡±¡¶¡·£¯£¿£¬¡£");
    int i;
    bool ret = false;
    while ((i = s.FindOneOf(interpunction)) != -1)
    {
        ret = true;
        s.Delete(i);
    }

    return ret;
}
*/


void CMediaLibrarySearchBarCtrl::OnEnChangeEditDesc()
{
    CString str;
    GetDlgItem(IDC_EDIT_DESC)->GetWindowText(str);
}


void CMediaLibrarySearchBarCtrl::OnDtnDatetimechangeDatetimepickEnd(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_MEDIA_START, IDC_DATETIMEPICKER_MEDIA_END, date, false);
    *pResult = 0;
}

void CMediaLibrarySearchBarCtrl::OnDtnDatetimechangeDatetimepickStart(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_MEDIA_START, IDC_DATETIMEPICKER_MEDIA_END, date, true);
    *pResult = 0;
}


void CMediaLibrarySearchBarCtrl::OnCbnSelchangeCombo2()
{
	// TODO: Add your control notification handler code here
}



void CMediaLibrarySearchBarCtrl::OnBnClickedCheckDate()
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


void CMediaLibrarySearchBarCtrl::OnCbnDropdownComboGroup()
{
    // TODO: Add your control notification handler code here
    UpdateCategory();
}


void CMediaLibrarySearchBarCtrl::OnCbnCloseupComboGroup()
{
    // TODO: Add your control notification handler code here

    m_ctlCategory.Clear();

    m_ctlCategory.InsertString(0, Translate(_T("Updating......")));

}
