// MediaListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "MagicSortListCtrl.h"
#include "Appointment.h"
#include "MediaLibraryListPane.h"
 
// CMagicSortListCtrl

IMPLEMENT_DYNAMIC(CMagicSortListCtrl, CListCtrlEx)
 CMagicSortListCtrl::CSortFunctions CMagicSortListCtrl::m_xFuns;
CMagicSortListCtrl::CMagicSortListCtrl()
{
}

CMagicSortListCtrl::~CMagicSortListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMagicSortListCtrl, CListCtrlEx)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, CMagicSortListCtrl::OnColumnClick)
	ON_WM_SHOWWINDOW()
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()


void CMagicSortListCtrl::Resort()
{
	if(m_iSortedColumn == -1)
	{
		m_iSortedColumn =  CMediaLibraryListTab::MEDIA_INDEX ;
	}

	Sort(m_iSortedColumn,this->m_bAscending);
}

int CMagicSortListCtrl::InsertItem( int iItem ,LPCTSTR strItem)
{
	LVITEM lvItem;
	lvItem.iItem = iItem;
	lvItem.iSubItem = 0;
	lvItem.mask =  LVIF_PARAM|LVIF_TEXT;
	lvItem.pszText = (LPTSTR)strItem;
	lvItem.cchTextMax = ::_tcslen(strItem);
	lvItem.lParam = iItem;

	return __super::InsertItem(&lvItem);

}

int CMagicSortListCtrl::FindParamItem(CMagicSortListCtrl *plist, LPARAM lparam )
{
	LVITEM lv;
	for( int i = 0;i < plist->GetItemCount();i++)
	{
		lv.iItem = i;
		lv.mask = LVIF_PARAM;
		plist->GetItem(&lv);

		if(lv.lParam == lparam)
			return i;
	}
	TRACE(_T("FindParamItem:not find item!"));
	return 0;
}
int CMagicSortListCtrl:: OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn)
{
	int ret = 0;
	TRACE(_T("lParam1 :%d\n"),lParam1);
	TRACE(_T("lParam2 :%d\n"),lParam2);

	CString strItem1,strItem2;

	strItem1 = GetItemText( FindParamItem(this,lParam1),iColumn);
	strItem2 = GetItemText( FindParamItem(this,lParam2),iColumn);

	LVCOLUMN colInfo;
	CString strColum;
	TCHAR   textBuf[255];

	colInfo.cchTextMax = 255;
	colInfo.pszText = textBuf;
	colInfo.mask = LVCF_TEXT;
	GetColumn(iColumn,&colInfo);

	strColum = colInfo.pszText ;

	CSortFunctions::COMPARE_LISTITEM_FUN pFunc = CMagicSortListCtrl::m_xFuns.GetSortFunc(strColum);
	if( NULL == pFunc)
		return 0;
	return (*pFunc)(strItem1,strItem2);
}

// CMagicSortListCtrl message handlers
void CMagicSortListCtrl::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	ENSURE(pNMListView != NULL);

	if(CMagicSortListCtrl::m_xFuns.GetFuncsNumber() <= 0)
		return;
	if(this->GetItemCount() <= 0)
		return;

	int iColumn = pNMListView->iSubItem;

	LVCOLUMN colInfo;
	CString strColum;
	TCHAR   textBuf[255];

	colInfo.mask = LVCF_TEXT;
	colInfo.cchTextMax = 255;
	colInfo.pszText = textBuf;

	GetColumn(iColumn,&colInfo);
	strColum = colInfo.pszText ;
	// No found item sort fun,direct return.
	if(NULL == CMagicSortListCtrl::m_xFuns.GetSortFunc(strColum) )
		return;


	BOOL bShiftIsPressed = (::GetAsyncKeyState(VK_SHIFT) & 0x8000);
	int nColumnState = GetHeaderCtrl().GetColumnState(iColumn);
	BOOL bAscending = TRUE;

	if (nColumnState != 0)
	{
		bAscending = nColumnState <= 0;
	}
	this->Invalidate();
	Sort(iColumn, bAscending, IsMultipleSort());
	*pResult = 0;
}


int CMagicSortListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrlEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	this->ModifyStyle(LVS_ALIGNMASK|LVS_NOSORTHEADER,LVS_REPORT);
	InitSortFuncs();
	return 0;
}


void CMagicSortListCtrl::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CListCtrlEx::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	//this->Resort();
}

CMagicSortListCtrl * CMagicSortListCtrl:: ConverPtr(CWnd *pWnd)
{
	if( pWnd->IsKindOf(RUNTIME_CLASS(CMagicSortListCtrl)) )
		return (CMagicSortListCtrl *)pWnd;
	return NULL;
}

//
// Some compare string items is predefined,the other need to call this function at app.
//
bool CMagicSortListCtrl::SetCompairFunPair(LPCTSTR strkey,BaseCompairFunc func)
{
	return m_xFuns.AddFuncPair(strkey, func);
}

BOOL CMagicSortListCtrl::InitSortFuncs()
{
	// Don't re-initlize
	if(m_xFuns.GetFuncsNumber() > 0)
		return TRUE;
	// Create relation between  column item name and sort function.
	//m_xFuns.AddFuncPair(make_pair(_T(), ));
	/* All Item String Section:
	No.| Name|Type|Size|Category|Dimention|Duration|SysncStatus|Audit|Refers|Status|
	Percent|Available|Desc|User Name|Login IP|Operation Time|Operation|IP Address|
	Current Layout|FPS|Heard Beat|Next Layout|Schedule|Speed|Disk|cpu usage|Memory Usage|
	Fan Speed|Temperature|Client Version |Player Status|ClientID|ClientName|ClientStatus|
	ClientRepeatTime|DownloadFileName|DownloadResultStatus|PlayTaskName|PlayRegionName|
	PlayTimes|PlayStartTime|PlayEndTime|PlayRepeatTime|PlayTaskID|DiskInfo|
	Location|Phone|DisplayLayOut|ClientGroupName|Availability|ClientGroupID|
	OnLine|AllStatus|OnlineRate

	For eg: m_xFuns.AddFuncPair(make_pair(_T("Name"), BaseStrComp));
	*/
	// Initlize Simple String Item
	m_xFuns.AddFuncPair(Translate(_T("Name")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Type")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Category")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Sync Status")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("SyncStatus")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Audit")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Refers")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Operation")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("OperationTime")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Status")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("User Name")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Current Layout")), BaseStrComp);
    m_xFuns.AddFuncPair(Translate(_T("ClientName")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Client Name")), BaseStrComp);
    m_xFuns.AddFuncPair(Translate(_T("Group Name")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Location")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Task Name")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("DisplayLayout")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("PlayRegionName")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Download File Name")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Download Result Status")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Client Group Name")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("AllStatus")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("OnLine")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Desc")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("DiskInfo")), BaseStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Availability")), BaseStrComp);
    m_xFuns.AddFuncPair(Translate(_T("Client Status")), BaseStrComp);
    m_xFuns.AddFuncPair(Translate(_T("Online Rate")), BaseStrComp);
    m_xFuns.AddFuncPair(Translate(_T("Task Type")), BaseStrComp);
    m_xFuns.AddFuncPair(Translate(_T("Play Times")), BaseStrComp);
	//m_xFuns.AddFuncPair(_T("ClientStatus"), BaseStrComp);



	// Initlize normal Number Item
	m_xFuns.AddFuncPair(Translate(_T("NO.")), NumberStrComp);
	m_xFuns.AddFuncPair(Translate(_T("No.")), NumberStrComp);
	m_xFuns.AddFuncPair(Translate(_T("ClientID")), NumberStrComp);
    m_xFuns.AddFuncPair(Translate(_T("Client ID")), NumberStrComp);
	m_xFuns.AddFuncPair(Translate(_T("ClientStatus")), NumberStrComp);
	m_xFuns.AddFuncPair(Translate(_T("ClientGroupID")), NumberStrComp);
    m_xFuns.AddFuncPair(Translate(_T("Group ID")), NumberStrComp);
	m_xFuns.AddFuncPair(Translate(_T("PlayRepeatTime")), NumberStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Fan Speed")), NumberStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Phone")), NumberStrComp);
	m_xFuns.AddFuncPair(Translate(_T("Temperature")), NumberStrComp);
	m_xFuns.AddFuncPair(Translate(_T("ClientRepeatTime")), NumberStrComp);
	m_xFuns.AddFuncPair(Translate(_T("PlayTaskID")), NumberStrComp);

	// Initlize Size String Item : 20kb
	m_xFuns.AddFuncPair(Translate(_T("Size")), MediaSizeComp);
	m_xFuns.AddFuncPair(Translate(_T("size")), MediaSizeComp);
	m_xFuns.AddFuncPair(Translate(_T("Memory Usage")), MediaSizeComp);
	// Initlize Dimention Item : 200 * 300
	m_xFuns.AddFuncPair(Translate(_T("Dimension")), MediaDimentionComp);

	// Initlize Duration Item :00:30:20
	m_xFuns.AddFuncPair(Translate(_T("Duration")), MediaDurationComp);
	m_xFuns.AddFuncPair(Translate(_T("Play Start Time")), MediaDurationComp);
	m_xFuns.AddFuncPair(Translate(_T("Play End Time")), MediaDurationComp);
	m_xFuns.AddFuncPair(Translate(_T("PlayReportTime")), MediaDurationComp);
	m_xFuns.AddFuncPair(Translate(_T("Client Report Time")), MediaDurationComp);

	// Initlize IP Item :192.168.1.1
	m_xFuns.AddFuncPair(Translate(_T("IP Address")), IPAdressComp);
	m_xFuns.AddFuncPair(Translate(_T("Login IP")), IPAdressComp);

	//Initlize Client Version Item :1.35/1.0.1
	m_xFuns.AddFuncPair(Translate(_T("Client Version")), VersionComp);
	return TRUE;
}

void CMagicSortListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    BOOL bControl = (nFlags & MK_CONTROL) != 0;
    if (bControl)
    {
        if (nChar == _T('a') || nChar == _T('A'))
        {
            int count = GetItemCount();
            for (int i = 0; i < count; ++i)
            {
                SetItemState(i, 0xffffffff, LVIS_SELECTED);
            }
            return;
        }
    }

    CListCtrlEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

// CSortFunctions
int	CMagicSortListCtrl::CSortFunctions::GetFuncsNumber()
{
	return this->m_SortFunsMap.size();
}
CMagicSortListCtrl::CSortFunctions::COMPARE_LISTITEM_FUN CMagicSortListCtrl::CSortFunctions::GetSortFunc(CString strColName)
{
	if(m_SortFunsMap.size() <= 0)
		return NULL;
	strColName.Remove(_T(' '));
	strColName.MakeLower();
	if(strColName.IsEmpty())
	{
		TRACE(_T("**Empty String Item isn't allowed! \n"));
		return (COMPARE_LISTITEM_FUN)NULL;
	}
	SORT_FUNS::iterator it  = m_SortFunsMap.find(strColName);
	if( it!= m_SortFunsMap.end())
		return (*it).second;
	return (COMPARE_LISTITEM_FUN)NULL;
}

bool CMagicSortListCtrl::CSortFunctions::AddFuncPair(CString strItemName,COMPARE_LISTITEM_FUN pFunc)
{
	strItemName.Remove(_T(' '));
	strItemName.MakeLower();
	SORT_FUNS::iterator it  = m_SortFunsMap.find(strItemName);
	if( it == m_SortFunsMap.end())
	{
		//TRACE(_T("**Duplicate Sort Item : %s \n"),strItemName );
		m_SortFunsMap.insert(make_pair(strItemName,pFunc));
		return true;
	}
	return false;
}

void CMagicSortListCtrl::OnCustomDraw(NMHDR *lpHdr,LRESULT *lpResult)
{
	__super::OnCustomDraw(lpHdr,lpResult);
	LPNMLVCUSTOMDRAW pCustom = (LPNMLVCUSTOMDRAW) lpHdr;
	//draw red row item  here.
	if(pCustom->nmcd.dwDrawStage ==  CDDS_ITEMPREPAINT|CDDS_SUBITEM)
	{
			int iCol ;
			int iRow ;
			CString sItem ;
			CRect rcItem;
			CDC *pDC;

			pDC =  CDC::FromHandle(pCustom->nmcd.hdc);
			iCol = pCustom->iSubItem;
			iRow = pCustom->nmcd.dwItemSpec;
			sItem = this->GetItemText(iRow,iCol);

			if(sItem == Translate(_T("REQUESTED"))
				||sItem == Translate(_T("REJECTED"))
				||sItem == Translate(_T("RETIRED")))
			{
				GetCellRect(iRow, iCol, LVIR_BOUNDS, rcItem);

				rcItem.OffsetRect(2,0); 
				pDC->SetTextColor(RGB(200,0,0));
				pDC->DrawText(sItem, -1, &rcItem, DT_LEFT | DT_VCENTER);
				*lpResult= CDRF_SKIPDEFAULT;
			}
	}
}