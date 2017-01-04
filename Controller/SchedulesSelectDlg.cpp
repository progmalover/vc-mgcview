#include "stdafx.h"
#include "Controller.h"
#include "SchedulesSelectDlg.h"
#include "SchedulerPage.h"
#include "afxdialogex.h"
#include "ScheduleCtrl.h"
#include "Localization/Localization.h"

IMPLEMENT_DYNAMIC(CSchedulesSelectDlg, CDialogEx)

CSchedulesSelectDlg::CSchedulesSelectDlg(CWnd* pParent , const std::vector<ClientGroup>& Groups)
	: CDialogEx(CSchedulesSelectDlg::IDD, pParent)
    , m_Groups (Groups)
    , m_SelectedGroupID (-1)
{
    
}

CSchedulesSelectDlg::~CSchedulesSelectDlg()
{
}

void CSchedulesSelectDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_SCHEDULES, m_GroupsList);
}


BEGIN_MESSAGE_MAP(CSchedulesSelectDlg, CDialogEx)
    ON_WM_SIZE()
    ON_NOTIFY(NM_CLICK, IDC_LIST_SCHEDULES, &CSchedulesSelectDlg::OnNMClickListSchedules)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CSchedulesSelectDlg)
    EASYSIZE(IDC_LIST_SCHEDULES,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP


BOOL CSchedulesSelectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;

    InitClientGroups(m_Groups);
	
	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CSchedulesSelectDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

BOOL CSchedulesSelectDlg::InitClientGroups(const std::vector<ClientGroup>& Groups)
{
    m_SelectedGroupID = -1;
    m_GroupsList.DeleteAllItems();

    // insert columns
    LV_COLUMN lvc;

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    lvc.iSubItem = 0;
    lvc.pszText = _T("");
    lvc.cx = 1280;
    lvc.fmt = LVCFMT_CENTER;
    m_GroupsList.InsertColumn(0,&lvc);

    LV_ITEM lvi;

    for (std::vector<ClientGroup>::const_iterator it = Groups.begin(); it < Groups.end(); ++it)
    {
        CString name = it->GetGroupName().c_str();
        lvi.mask = LVIF_TEXT | LVIF_IMAGE;
        lvi.iItem = m_GroupsList.GetItemCount();
        lvi.iSubItem = 0;
        lvi.pszText = (LPWSTR)name.GetString();
        lvi.iGroupId = it->GetGroupID();

        INT nItem = m_GroupsList.InsertItem(&lvi);
        m_GroupsList.SetItemData(nItem, it->GetGroupID());
    }

    m_GroupsList.SetExtendedStyle(LVS_EX_GRIDLINES); 
    ::SendMessage(m_GroupsList.m_hWnd,   LVM_SETEXTENDEDLISTVIEWSTYLE, 
        LVS_EX_FULLROWSELECT,   LVS_EX_FULLROWSELECT);

    return TRUE;
}

INT CSchedulesSelectDlg::GetSelectGroupID()
{
    return m_SelectedGroupID;
}

void CSchedulesSelectDlg::OnNMClickListSchedules(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Add your control notification handler code here

    POSITION pos = m_GroupsList.GetFirstSelectedItemPosition();
    int nItem = m_GroupsList.GetNextSelectedItem(pos); 

    if (nItem >= 0)
    {
        m_SelectedGroupID =  m_GroupsList.GetItemData(nItem);
    }
    else
    {
        m_SelectedGroupID = -1;
    }

    *pResult = 0;
}
