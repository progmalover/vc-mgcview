// SearchClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Controller.h"
#include "SearchClientDlg.h"
#include "afxdialogex.h"
#include "GroupListCtrl.h"
#include "ClientManagerPage.h"

UINT CLIENTDLG_SEARCH = ::RegisterWindowMessage(_T("CLIENTDLG_SEARCH"));
// CSearchClientDlg 对话框

IMPLEMENT_DYNAMIC(CSearchClientDlg, CDialogEx)

CSearchClientDlg::CSearchClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSearchClientDlg::IDD, pParent)
{

}

CSearchClientDlg::~CSearchClientDlg()
{
}

void CSearchClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_SEARCH_CLIENT_NAME, m_strName);
    DDX_Text(pDX, IDC_EDIT_SEARCH_CLIENT_TEL, m_strTel);
    DDX_Text(pDX, IDC_EDIT_SEARCH_CLIENT_LOCATION, m_strLocation);
    DDX_Text(pDX, IDC_EDIT_SEARCH_CLIENT_IP, m_strIp);
    DDX_Text(pDX, IDC_EDIT_SEARCH_CLIENT_EMAIL, m_strEmail);
    DDX_Text(pDX, IDC_EDIT_SEARCH_CLIENT_HARDWARE, m_strHardware);
    DDX_Control(pDX, IDC_COMBO_SEARCH_CLIENT_GROUP, m_ctlGroup);
    DDX_Text(pDX, IDC_EDIT_SEARCH_CLIENT_ADMIN, m_strAdmin);
    DDX_Text(pDX, IDC_EDIT_SEARCH_CLIENT_COMPANY, m_strCompany);
}

BOOL CSearchClientDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_SearchBtn.SetImage(IDB_SEARCH);

    UpdateGroup();

    UpdateData(FALSE);

    Translate(this);
    return TRUE;
}

BEGIN_MESSAGE_MAP(CSearchClientDlg, CDialogEx)
       ON_BN_CLICKED(IDOK, &CSearchClientDlg::OnBnClickedButtonSearchdlgSearch)
END_MESSAGE_MAP()


// CSearchClientDlg 消息处理程序
void CSearchClientDlg::RecalcDropWidth(CComboBox* pComboBox)
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


void CSearchClientDlg::UpdateGroup()
{
    m_ctlGroup.ResetContent();
    std::vector<ClientGroup> groupList;
    std::map<int, int> insertTip;
    HRESULT hr = GetControllerInterface->GetAllClientGroup(groupList);
    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
            return;
        }
        else
        {
            TRACE0("Failed to Get Client Group!\n");
            return;
        }
    }
    auto it = groupList.begin(), ite = groupList.end();

    m_ctlGroup.InsertString(0, Translate(_T("All Groups")));
    m_ctlGroup.SetItemData(0, -1);
    insertTip.insert(map<int,int>::value_type(-1, 0));

    for (int i = 1; it != ite; ++it,++i)
    {
        m_ctlGroup.InsertString(i, it->GetGroupName().c_str());
        int tmpGroupId = it->GetGroupID();
        m_ctlGroup.SetItemData(i, tmpGroupId);
        insertTip.insert(map<int, int>::value_type(tmpGroupId, i));
    }

    CWnd* pWnd = SearchWndObj(AfxGetMainWnd(), RUNTIME_CLASS(GroupListCtrl));
    GroupListCtrl* pGroup = (GroupListCtrl*)pWnd;
    INT ID = pGroup->GetCurGroupID();



    m_ctlGroup.SetCurSel(0);
    RecalcDropWidth(&m_ctlGroup);
}

void CSearchClientDlg::OnBnClickedButtonSearchdlgSearch()
{
    UpdateData(TRUE);

    CWnd *pWnd = SearchWndObj(AfxGetMainWnd(), RUNTIME_CLASS(CClientManagerPage));
    pWnd->SendMessage(CLIENTDLG_SEARCH, 0, (LPARAM)this);

    CDialogEx::EndDialog(TRUE);
}