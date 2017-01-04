// SearchFileDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Controller.h"
#include "SearchFileDlg.h"
#include "afxdialogex.h"
#include "DatetimeCheck.h"
#include "MediaLibraryPage.h"
#include "LayoutManagePage.h"
#include "MediaLibraryCategoryCtrl.h"
#include "LayoutCategoryCtrl.h"

// CSearchFileDlg 对话框
UINT MEDIADLG_SEARCH = ::RegisterWindowMessage (_T("MEDIADLG_SEARCH"));
IMPLEMENT_DYNC_SINGLETON(MediaSearchOptions)
IMPLEMENT_DYNAMIC(CSearchFileDlg, CDialogEx)

CSearchFileDlg::CSearchFileDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSearchFileDlg::IDD, pParent)
{

}

CSearchFileDlg::~CSearchFileDlg()
{
}



void CSearchFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_SEARCHDLG_DESC, m_strDesc);
    DDX_Control(pDX, IDC_COMBO_SEARCHDLG_GROUP, m_ctlCategory);
    DDX_Control(pDX, IDC_COMBO_SEARCHDLG_OWNER, m_ctlOwner);
    DDX_Control(pDX, IDC_COMBO_SEARCHDLG_SIZE, m_ctlSize);
    DDX_Control(pDX, IDC_COMBO_SEARCHDLG_AUDIT, m_ctlAudit);
    DDX_Control(pDX, IDC_COMBO_SEARCHDLG_TYPE, m_ctlType);
    DDX_Control(pDX, IDC_COMBO_SEARCHDLG_DUR, m_ctlDur);
    DDX_Control(pDX, IDC_COMBO_SEARCHDLG_HEIGHT, m_ctlHeight);
    DDX_Control(pDX, IDC_COMBO_SEARCHDLG_WIDTH, m_ctlWidth);
    DDX_Control(pDX, IDOK, m_SearchBtn);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_SEARCHDLG_START,m_timeStart);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_SEARCHDLG_END, m_timeEnd);
    DDX_Check(pDX, IDC_CHECK_SEARCHDLG_DATE, m_bUseDate);
    if(pDX->m_bSaveAndValidate)
    {
        MediaSearchOptions::Instance()->_ApprovalIDSel = m_ctlAudit.GetCurSel();
        MediaSearchOptions::Instance()->_ApprovalID = m_ctlAudit.GetItemData(m_ctlAudit.GetCurSel());
        
        MediaSearchOptions::Instance()->_CategoryIDSel = m_ctlCategory.GetCurSel();
        MediaSearchOptions::Instance()->_CategoryID = m_ctlCategory.GetItemData(m_ctlCategory.GetCurSel());

        MediaSearchOptions::Instance()->_DurIDSel = m_ctlDur.GetCurSel();
        MediaSearchOptions::Instance()->_DurID		 = m_ctlDur.GetItemData(m_ctlDur.GetCurSel());

        MediaSearchOptions::Instance()->_HeightIDSel = m_ctlHeight.GetCurSel();
        MediaSearchOptions::Instance()->_HeightID	 = m_ctlHeight.GetItemData(m_ctlHeight.GetCurSel());

        MediaSearchOptions::Instance()->_WidthIDSel = m_ctlWidth.GetCurSel();
        MediaSearchOptions::Instance()->_WidthID    = m_ctlWidth.GetItemData(m_ctlWidth.GetCurSel());

        MediaSearchOptions::Instance()->_SizeIdSel = m_ctlSize.GetCurSel();
        MediaSearchOptions::Instance()->_SizeId     = m_ctlSize.GetItemData(m_ctlSize.GetCurSel());

        MediaSearchOptions::Instance()->_TypeIDSel = m_ctlType.GetCurSel();
        MediaSearchOptions::Instance()->_TypeID     = m_ctlType.GetItemData(m_ctlType.GetCurSel());

        MediaSearchOptions::Instance()->_OwnerIDSel = m_ctlOwner.GetCurSel();
        MediaSearchOptions::Instance()->_OwnerID    = m_ctlOwner.GetItemData(m_ctlOwner.GetCurSel());

        MediaSearchOptions::Instance()->_StrDesc    = m_strDesc;
        MediaSearchOptions::Instance()->_timeStart = m_timeStart;
        MediaSearchOptions::Instance()->_timeEnd = m_timeEnd;
        MediaSearchOptions::Instance()->_bUseDate = m_bUseDate;

        if(m_ctlType.GetCount())
            m_ctlType.GetLBText(MediaSearchOptions::Instance()->_TypeID + 1, MediaSearchOptions::Instance()->_strMediaType); 
    }
    else
    {
        m_strDesc = MediaSearchOptions::Instance()->_StrDesc;
        m_timeStart = MediaSearchOptions::Instance()->_timeStart;
        m_timeEnd = MediaSearchOptions::Instance()->_timeEnd;
        m_bUseDate = MediaSearchOptions::Instance()->_bUseDate;
    }
}

BOOL CSearchFileDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_SearchBtn.SetImage(IDB_SEARCH);
    GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_START)->EnableWindow(TRUE);
    GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_END)->EnableWindow(TRUE);

#ifndef SERVER_EDITION
     GetDlgItem(IDC_COMBO_SEARCHDLG_OWNER)->ShowWindow(SW_HIDE);
     GetDlgItem(IDC_COMBO_SEARCHDLG_AUDIT)->ShowWindow(SW_HIDE);
#endif

    m_timeStart = MediaSearchOptions::Instance()->_timeStart;
    m_timeEnd   = MediaSearchOptions::Instance()->_timeEnd;

     UpdateCategory();
     UpdateOwner();
     UpdateAudit();
     UpdateSize();
     UpdateFileType();
     UpdateHeight();
     UpdateWidth();
     UpdateDur();

     OnBnClickedCheckSearchdlgDate();
     if (m_bUseDate)
     {
         GetDlgItem(IDC_STATIC_SEARCHDLG_START)->EnableWindow(TRUE);
         GetDlgItem(IDC_STATIC_SEARCHDLG_END)->EnableWindow(TRUE);
         GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_START)->EnableWindow(TRUE);
         GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_END)->EnableWindow(TRUE);
     }
     else
     {
         GetDlgItem(IDC_STATIC_SEARCHDLG_START)->EnableWindow(FALSE);
         GetDlgItem(IDC_STATIC_SEARCHDLG_END)->EnableWindow(FALSE);
         GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_START)->EnableWindow(FALSE);
         GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_END)->EnableWindow(FALSE);
     }
    UpdateData(FALSE);

    GetDlgItem(IDC_EDIT_SEARCHDLG_DESC)->SetFocus();
    UntranslateWnd(m_ctlType.GetSafeHwnd());
    Translate(this);

    if (!CP(UserPrivilege_GetMedia) || !CP(UserPrivilege_ListMediaFilter))
    {
        GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_START)->EnableWindow(FALSE);
        GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_END)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_SEARCHDLG_SEARCH)->EnableWindow(FALSE);
    }

    return TRUE;
}


BEGIN_MESSAGE_MAP(CSearchFileDlg, CDialogEx)
    ON_BN_CLICKED(IDC_CHECK_SEARCHDLG_DATE, &CSearchFileDlg::OnBnClickedCheckSearchdlgDate)
    ON_BN_CLICKED(IDOK, &CSearchFileDlg::OnBnClickedButtonSearchdlgSearch)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_SEARCHDLG_START, &CSearchFileDlg::OnDtnDatetimechangeDatetimepickStart)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_SEARCHDLG_END, &CSearchFileDlg::OnDtnDatetimechangeDatetimepickEnd)
END_MESSAGE_MAP()


// CSearchFileDlg 消息处理程序

void CSearchFileDlg::GetAllCategory(MediaFilter* pRoot,std::vector< pair<CString, MediaFilter> > &category, const CString& PreFix)
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

void CSearchFileDlg::RecalcDropWidth(CComboBox* pComboBox)
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

void CSearchFileDlg::UpdateCategory()
{
    m_ctlCategory.ResetContent();
    std::vector< pair<CString, MediaFilter> > category;
    std::map<int, int> insertTip;
    MediaFilter root;
    
    HRESULT hr = GetControllerInterface->GetRootMediaFilter(root);
    MediaFilter *pRoot = &root;
    if (hr == HRESULT_FROM_SETUPAPI(ERROR_CONNECTION_INVALID))
    {
        GetControllerApp->OnControllerServerOffline();
        return;
    }
    category.clear();
    GetAllCategory(pRoot, category);

    auto it = category.begin(), ite = category.end();

    for (int i = 0; it != ite; ++it, ++i)
    {
        it->first.Replace(_T("root"), Translate(_T("Media Root Category")));
        m_ctlCategory.InsertString(i, it->first);
        int filterID = it->second.GetMediaFilterID();
        m_ctlCategory.SetItemData(i, filterID);
        insertTip.insert(map<int, int>::value_type(filterID, i));
    }

    CMediaLibraryCategoryCtrl::ITEM_TYPE ItemType;
    CWnd *pWnd = SearchWndObj(AfxGetMainWnd(),RUNTIME_CLASS(CMediaLibraryCategoryCtrl));
    CMediaLibraryCategoryCtrl* pCategory = (CMediaLibraryCategoryCtrl*)pWnd;
    INT ID = pCategory->GetSelectedCategory(ItemType);

    if (ItemType == CMediaLibraryCategoryCtrl::ITEM_CATEGORY)
    {
        MediaFilter Category;
        HRESULT hr = GetControllerInterface->GetMediaFilter(ID, Category);
        if (!SUCCEEDED(hr))
        {
            TRACE1("Failed to Get Media Filter %d!\n", ID);
        }
        map<int, int>::iterator it = insertTip.find(ID);
        m_ctlCategory.SetCurSel(it->second);
    }
    else
    {
        m_ctlCategory.SetCurSel(MediaSearchOptions::Instance()->_CategoryIDSel);
    }
   
    RecalcDropWidth(&m_ctlCategory);
}

void CSearchFileDlg::UpdateOwner()
{
    m_ctlOwner.ResetContent();
    std::vector<User> owner;
    HRESULT hr = GetControllerInterface->GetAllUsers(owner);
    if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
    {
        GetControllerApp->OnControllerServerOffline();
        return;
    }

    auto it = owner.begin(), ite = owner.end();

    m_ctlOwner.InsertString(0, Translate(_T("All Owners")));
    m_ctlOwner.SetItemData(0, -1);

    for (int i = 1; it != ite; ++it)
    {
        if(it->GetUserID() >= 0)
        {
            Account tmp = it->GetAccount();
            m_ctlOwner.InsertString(i, tmp.GetUserName().c_str());
            m_ctlOwner.SetItemData(i, it->GetUserID());
            ++i;
        }
    }

    m_ctlOwner.SetCurSel(MediaSearchOptions::Instance()->_OwnerIDSel);

    RecalcDropWidth(&m_ctlOwner);
}

void CSearchFileDlg::UpdateSize()
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

    m_ctlSize.SetCurSel(MediaSearchOptions::Instance()->_SizeIdSel);

    RecalcDropWidth(&m_ctlSize);
}

void CSearchFileDlg::UpdateHeight()
{
    m_ctlHeight.ResetContent();
    m_ctlHeight.InsertString(0, Translate(_T("All Heights(width*height)")));
    m_ctlHeight.SetItemData(0, -1);
    m_ctlHeight.InsertString(1, Translate(_T("0-128 Pixels")));
    m_ctlHeight.SetItemData(1, 0);
    m_ctlHeight.InsertString(2, Translate(_T("128-256 Pixels")));
    m_ctlHeight.SetItemData(2, 1);
    m_ctlHeight.InsertString(3, Translate(_T("256-512 Pixels")));
    m_ctlHeight.SetItemData(3, 2);
    m_ctlHeight.InsertString(4, Translate(_T("512-1024 Pixels")));
    m_ctlHeight.SetItemData(4, 3);
    m_ctlHeight.InsertString(5, Translate(_T(">1024 Pixels")));
    m_ctlHeight.SetItemData(5, 4);

    m_ctlHeight.SetCurSel(MediaSearchOptions::Instance()->_HeightIDSel);

    RecalcDropWidth(&m_ctlHeight);

}

void CSearchFileDlg::UpdateWidth()
{
    m_ctlWidth.ResetContent();
    m_ctlWidth.InsertString(0, Translate(_T("All Widths(width*height)")));
    m_ctlWidth.SetItemData(0, -1);
    m_ctlWidth.InsertString(1, Translate(_T("0-128 Pixels")));
    m_ctlWidth.SetItemData(1, 0);
    m_ctlWidth.InsertString(2, Translate(_T("128-256 Pixels")));
    m_ctlWidth.SetItemData(2, 1);
    m_ctlWidth.InsertString(3, Translate(_T("256-512 Pixels")));
    m_ctlWidth.SetItemData(3, 2);
    m_ctlWidth.InsertString(4, Translate(_T("512-1024 Pixels")));
    m_ctlWidth.SetItemData(4, 3);
    m_ctlWidth.InsertString(5, Translate(_T(">1024 Pixels")));
    m_ctlWidth.SetItemData(5, 4);

    m_ctlWidth.SetCurSel(MediaSearchOptions::Instance()->_WidthIDSel);

    RecalcDropWidth(&m_ctlWidth);

}

void CSearchFileDlg::UpdateFileType()
{
    m_ctlType.ResetContent();
    m_ctlType.InsertString(0, Translate(_T("All Types")));
    m_ctlType.SetItemData(0, -1);
    m_ctlType.InsertString(1, _T("AVC"));  
    m_ctlType.SetItemData(1, 0);
    m_ctlType.InsertString(2, _T("AVI"));  
    m_ctlType.SetItemData(2, 1);
    m_ctlType.InsertString(3, _T("BDAV")); 
    m_ctlType.SetItemData(3, 2);
    m_ctlType.InsertString(4, _T("Bitmap")); 
    m_ctlType.SetItemData(4, 3);
    m_ctlType.InsertString(5, _T("GIF")); 
    m_ctlType.SetItemData(5, 4);
    m_ctlType.InsertString(6, _T("JPEG")); 
    m_ctlType.SetItemData(6, 5);
    m_ctlType.InsertString(7, _T("MPEG-4"));  
    m_ctlType.SetItemData(7, 6);

    m_ctlType.InsertString(8, _T("MPEG-Audio"));
    m_ctlType.SetItemData(8, 7);
    m_ctlType.InsertString(9, _T("MPEG-PS"));
    m_ctlType.SetItemData(9, 8);
    m_ctlType.InsertString(10, _T("MPEG-TS"));
    m_ctlType.SetItemData(10, 9);
    m_ctlType.InsertString(11, _T("PNG"));
    m_ctlType.SetItemData(11, 10);

    m_ctlType.InsertString(12, _T("PPT"));
    m_ctlType.SetItemData(12, 11);
    m_ctlType.InsertString(13, _T("ShockWave"));
    m_ctlType.SetItemData(13, 12);
    m_ctlType.InsertString(14, _T("TIFF"));
    m_ctlType.SetItemData(14, 13);
    m_ctlType.InsertString(15, _T("URL"));
    m_ctlType.SetItemData(15, 14);

    m_ctlType.InsertString(16, _T("Wave"));
    m_ctlType.SetItemData(16, 15);
    m_ctlType.InsertString(17, _T("Windows Media"));
    m_ctlType.SetItemData(17, 16);

    m_ctlType.SetCurSel(MediaSearchOptions::Instance()->_TypeIDSel);

    RecalcDropWidth(&m_ctlType);
}

void CSearchFileDlg::UpdateDur()
{
    m_ctlDur.ResetContent();
    m_ctlDur.InsertString(0, Translate(_T("All Durations")));
    m_ctlDur.SetItemData(0, -1);
    m_ctlDur.InsertString(1, Translate(_T("0-1 Minute")));
    m_ctlDur.SetItemData(1, 0);
    m_ctlDur.InsertString(2, Translate(_T("1-5 Minutes")));
    m_ctlDur.SetItemData(2, 1);
    m_ctlDur.InsertString(3, Translate(_T("5-30 Minutes")));
    m_ctlDur.SetItemData(3, 2);
    m_ctlDur.InsertString(4, Translate(_T("30-60 Minutes")));
    m_ctlDur.SetItemData(4, 3);
    m_ctlDur.InsertString(5, Translate(_T(">1 Hour")));
    m_ctlDur.SetItemData(5, 4);

    m_ctlDur.SetCurSel(MediaSearchOptions::Instance()->_DurIDSel);

    RecalcDropWidth(&m_ctlDur);
}

void CSearchFileDlg::UpdateAudit()
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
    m_ctlAudit.InsertString(4, Translate(_T("Retired")));
    m_ctlAudit.SetItemData(4, 3);
   
    m_ctlAudit.SetCurSel(MediaSearchOptions::Instance()->_ApprovalIDSel);

    RecalcDropWidth(&m_ctlAudit);
}

void CSearchFileDlg::OnDtnDatetimechangeDatetimepickEnd(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_SEARCHDLG_START, IDC_DATETIMEPICKER_SEARCHDLG_END, date, false);
    *pResult = 0;
}

void CSearchFileDlg::OnDtnDatetimechangeDatetimepickStart(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_SEARCHDLG_START, IDC_DATETIMEPICKER_SEARCHDLG_END, date, true);
    *pResult = 0;
}

void CSearchFileDlg::OnEnChangeEditDesc()
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



void CSearchFileDlg::OnBnClickedButtonSearchdlgSearch()
{
    // TODO: 在此添加控件通知处理程序代码

    UpdateData(TRUE);

    CWnd *pWnd = SearchWndObj(AfxGetMainWnd(),RUNTIME_CLASS(CMediaLibraryPage));
    pWnd->SendMessage(MEDIADLG_SEARCH, 0, (LPARAM)this);

    CDialogEx::EndDialog(TRUE);
}


void CSearchFileDlg::OnBnClickedCheckSearchdlgDate()
{
    // TODO: 在此添加控件通知处理程序代码
    if (!IsDlgButtonChecked(IDC_CHECK_SEARCHDLG_DATE))
    {
        GetDlgItem(IDC_STATIC_SEARCHDLG_START)->EnableWindow(FALSE);
        GetDlgItem(IDC_STATIC_SEARCHDLG_END)->EnableWindow(FALSE);
        GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_START)->EnableWindow(FALSE);
        GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_END)->EnableWindow(FALSE);
    }
    else
    {
        GetDlgItem(IDC_STATIC_SEARCHDLG_START)->EnableWindow(TRUE);
        GetDlgItem(IDC_STATIC_SEARCHDLG_END)->EnableWindow(TRUE);
        GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_START)->EnableWindow(TRUE);
        GetDlgItem(IDC_DATETIMEPICKER_SEARCHDLG_END)->EnableWindow(TRUE);
    }
}
