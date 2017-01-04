
#include "stdafx.h"
#include "GroupListCtrl.h"
#include "Localization/Localization.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT CLIENTMANAGER_GROUP_SELECTED = ::RegisterWindowMessage (_T("CLIENTMANAGER_GROUP_SELECTED"));
UINT CLIENTMANAGER_GROUP_CLIENT_GROUP_CHANGE  = ::RegisterWindowMessage (_T("CLIENTMANAGER_GROUP_CLIENT_GROUP_CHANGE"));

extern CLIPFORMAT CLIENT_MANAGER_CLIP_FMT;

/////////////////////////////////////////////////////////////////////////////
// GroupListCtrl

IMPLEMENT_DYNCREATE(GroupListCtrl, CListCtrl)

BEGIN_MESSAGE_MAP(GroupListCtrl, CListCtrl)
    //{{AFX_MSG_MAP(GroupListCtrl)
    ON_WM_CREATE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()

    //ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnLvnBeginDrag)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGENTER, OnDragEnter)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGOVER, OnDragOver)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGLEAVE, OnDragLeave)
    ON_REGISTERED_MESSAGE(WM_DND_DROP, OnDrop)

    ON_MESSAGE(USER_MSG_UPDATE_GROUP_STATUS, RefreshGroupsStatus)
    //}}AFX_MSG_MAP
    ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GroupListCtrl construction/destruction

GroupListCtrl::GroupListCtrl()
    : m_CurID (-1)
{
	m_showOfflineGroup = false;
}

GroupListCtrl::~GroupListCtrl()
{

}

BOOL GroupListCtrl::PreCreateWindow(CREATESTRUCT& cs)
{

    cs.style |= LVS_ICON | LVS_AUTOARRANGE | LVS_SHOWSELALWAYS | LVS_SINGLESEL;

    return CListCtrl::PreCreateWindow(cs);
}

int GroupListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CListCtrl::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    m_DropTarget.Register (this);

    return 1;
}

/////////////////////////////////////////////////////////////////////////////
// data for the list view control
#define NUM_ITEMS   7

/////////////////////////////////////////////////////////////////////////////
// GroupListCtrl initialization

void GroupListCtrl::OnInitialUpdate()
{

    // set image lists

    m_LargeImageList.Create(THUMBNAIL_W, THUMBNAIL_H, ILC_COLOR32, 3, 1);
    m_LargeImageList.SetImageCount(GROUP_STATUS_CNT);

    CBitmap bitmap;
    bitmap.LoadBitmap(IDB_GROUP_STATUS_WELL);
    m_LargeImageList.Replace(GROUP_STATUS_WELL, &bitmap, NULL);
    bitmap.DeleteObject();
    bitmap.LoadBitmap(IDB_GROUP_NEED_ATTENTION);
    m_LargeImageList.Replace(GROUP_NEED_ATTENTION, &bitmap, NULL);
    bitmap.DeleteObject();
    bitmap.LoadBitmap(IDB_GROUP_EMPTY);
    m_LargeImageList.Replace(GROUP_EMPTY, &bitmap, NULL);
    bitmap.DeleteObject();
    bitmap.LoadBitmap(IDB_GROUP_CRITICAL);
    m_LargeImageList.Replace(GROUP_CRITICAL, &bitmap, NULL);
    bitmap.DeleteObject();
    
    m_LargeImageList.SetOverlayImage(NUM_ITEMS, 1);

    SetImageList(&m_LargeImageList, LVSIL_NORMAL);


    // insert columns

    int i;
    LV_COLUMN lvc;

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM;

    for(i = 0; i<2; i++)
    {
        lvc.iSubItem = i;
        lvc.cx = 100;
        lvc.fmt = LVCFMT_RIGHT;
        InsertColumn(i,&lvc);
    }

    OnViewLargeIcons();
#ifndef STANDALONE_EDITION
    SetTimer(0xFF000001, StringUtility::stoi(GetConfig->Get(CLIENT_MONITOR_REFRESH).GetString()) * 1000, 0);
#endif
    //UpdateGroupsStatus();
}


void GroupListCtrl::ShowOfflineGroup()
{
	m_showOfflineGroup = true;
	
}
void GroupListCtrl::UpdateGroupsStatus()
{
#ifndef STANDALONE_EDITION
	if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID,USER_MSG_UPDATE_GROUP_STATUS,
        (WPARAM)this->GetSafeHwnd(),(LPARAM)NULL))//post thread msg
    {
        printf("post message failed,errno:%d\n",::GetLastError());
    }

#else
    RefreshGroupsList();
#endif
}

void GroupListCtrl::RefreshGroupsList()
{
	std::vector<ClientGroup> groupList;
	HRESULT hr = GetControllerInterface->GetAllClientGroup(groupList);
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
		return;
	}

	int pos = GetNextItem(-1,LVNI_ALL|LVNI_SELECTED);
	int SelectGroupID = -1;
	if(pos >= 0) 
	{
		SelectGroupID = GetItemData(pos);
	}

	DeleteAllItems();

	std::vector<ClientGroup>::iterator it = groupList.begin();
	for (; it != groupList.end(); it++)
	{
		ClientGroup LookUpGroup;
		int GroupID = it->GetGroupID();
		int GroupStatus = GROUP_STATUS_WELL;

		{
			LookUpGroup = *it;
			m_GroupsPool.SetAt(GroupID, *it);

			LV_ITEM lvi;

			CString name = LookUpGroup.GetGroupName().c_str();
			lvi.mask = LVIF_TEXT | LVIF_IMAGE;
			lvi.iItem = this->GetItemCount();
			lvi.iSubItem = 0;
			lvi.pszText = (LPWSTR)name.GetString();
			lvi.iImage = GROUP_STATUS_WELL;

			int index = InsertItem(&lvi);

			TCHAR buf[10];
			_itot_s(GroupID, buf, 10);
			SetItemText(index,1,buf);

			SetItemData(index, GroupID);
			if(GroupID == SelectGroupID)
			{
				SetItemState(index,LVIS_SELECTED,LVIS_SELECTED);
			}
		}
	}
	return ;
}

LRESULT GroupListCtrl::RefreshGroupsStatus(WPARAM wp, LPARAM lp)
{
#ifndef STANDALONE_EDITION
    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf<std::map<int, std::pair<ClientGroup, int>>> MessageBuf;
    if (!WorkThreadDataManager< std::map<int, std::pair<ClientGroup, int>> >
        ::GetDataBuf(nMessageBufID, MessageBuf))
    {
        return 1;
    }

    const std::map<int, std::pair<ClientGroup, int>> *pGroupStatus = MessageBuf.Get();
	
	int pos = GetNextItem(-1,LVNI_ALL|LVNI_SELECTED);
	int SelectGroupID = -1;
	if(pos >= 0) 
	{
		SelectGroupID = GetItemData(pos);
	}

	DeleteAllItems();
	//m_GroupsPool.RemoveAll();

	auto it = pGroupStatus->begin();
    while(it != pGroupStatus->end())
    {

		ClientGroup LookUpGroup;
		int GroupID = it->first;
		int GroupStatus = it->second.second;

		LookUpGroup = it->second.first;
		m_GroupsPool.SetAt(GroupID, LookUpGroup);

		LV_ITEM lvi;

		CString name = LookUpGroup.GetGroupName().c_str();
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		lvi.iItem = this->GetItemCount();
		lvi.iSubItem = 0;
		lvi.pszText = (LPWSTR)name.GetString();
		lvi.iImage = GroupStatus;//GROUP_EMPTY;

		int index = InsertItem(&lvi);

		TCHAR buf[10];
		_itot_s(GroupID, buf, 10);
		SetItemText(index,1,buf);

		SetItemData(index, GroupID);

		if(GroupID == SelectGroupID)
		{
			SetItemState(index,LVIS_SELECTED,LVIS_SELECTED);
		}

        it++;
    }
	if(m_showOfflineGroup)
	{
		ClientGroup offlineGroup(-1, Translate(_T("Offline Group")).GetString(), Translate(_T("For Offline")).GetBuffer());
		ClientGroup LookUpGroup;
		int GroupID = offlineGroup.GetGroupID();
		int GroupStatus = GROUP_EMPTY;
		//if(!m_GroupsPool.Lookup(GroupID, LookUpGroup))
		{
			LookUpGroup = offlineGroup;
			m_GroupsPool.SetAt(GroupID, offlineGroup);

			LV_ITEM lvi;

			CString name = LookUpGroup.GetGroupName().c_str();
			lvi.mask = LVIF_TEXT | LVIF_IMAGE;
			lvi.iItem = this->GetItemCount();
			lvi.iSubItem = 0;
			lvi.pszText = (LPWSTR)name.GetString();
			lvi.iImage = GROUP_EMPTY;

			int index = InsertItem(&lvi);

			TCHAR buf[10];
			_itot_s(GroupID, buf, 10);
			SetItemText(index,1,buf);

			SetItemData(index, GroupID);

			if(SelectGroupID == -1)
			{
				SetItemState(index,LVIS_SELECTED,LVIS_SELECTED);
			}

		}

	}

    std::vector<int> DeleteList;
    CMap<INT, INT, ClientGroup, ClientGroup&>::CPair *pPair = m_GroupsPool.PGetFirstAssoc();

    while(pPair)
    {
        int ClientID = pPair->key;
        pPair =  m_GroupsPool.PGetNextAssoc(pPair);

        if(ClientID < 0) continue;
        auto it = pGroupStatus->find(ClientID);
        if(it == pGroupStatus->end())
        {
            DeleteList.push_back(ClientID);
        }

    }


    auto itd = DeleteList.begin();
    while(itd != DeleteList.end())
    {
        m_GroupsPool.RemoveKey(*itd);

        int nCount = GetItemCount(); 
        if(nCount > 0) 
        { 
            LVITEM lvitem; 
            lvitem.mask = LVIF_IMAGE;

            for(int i = 0; i < nCount; ++i) 
            { 
                lvitem.iItem    = i; 
                lvitem.iSubItem = 0;
                GetItem(&lvitem); 

                CString keystr = GetItemText(i, 1);
                INT CurGroupID = _ttoi(keystr);

                if (CurGroupID == *itd)
                {
                    DeleteItem(i);
                    break;
                }
            } 
        } 
        itd++;
    }

    return 0;
#else
	  {

		  return 0;
	  }
#endif
}


/////////////////////////////////////////////////////////////////////////////
// GroupListCtrl diagnostics

#ifdef _DEBUG
void GroupListCtrl::AssertValid() const
{
    CListCtrl::AssertValid();
}

void GroupListCtrl::Dump(CDumpContext& dc) const
{
    CListCtrl::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// GroupListCtrl helpers

BOOL GroupListCtrl::SetViewType(DWORD dwViewType)
{
    return(ModifyStyle(LVS_TYPEMASK,dwViewType & LVS_TYPEMASK));
}

DWORD GroupListCtrl::GetViewType()
{
    return(GetStyle() & LVS_TYPEMASK);
}

/////////////////////////////////////////////////////////////////////////////
// GroupListCtrl message handlers

void GroupListCtrl::OnViewSmallIcons()
{
    if (GetViewType() != LVS_SMALLICON)
        SetViewType(LVS_SMALLICON);
}

void GroupListCtrl::OnViewLargeIcons()
{
    if (GetViewType() != LVS_ICON)
        SetViewType(LVS_ICON);
}

void GroupListCtrl::OnViewList()
{
    if (GetViewType() != LVS_LIST)
        SetViewType(LVS_LIST);
}

void GroupListCtrl::OnViewDetails()
{
    if ((GetViewType() != LVS_REPORT))
    {
        if (GetViewType() != LVS_REPORT)
            SetViewType(LVS_REPORT);
    }
}

void GroupListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
    CListCtrl::OnLButtonUp(nFlags, point);
}

void GroupListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    UINT uFlags = 0;
    int nHitItem = HitTest(point, &uFlags);

    if (nHitItem >= 0)
    {
        CString keystr = GetItemText(nHitItem, 1);
        m_CurID = _ttoi(keystr);

        //GetParent()->SendMessage(CLIENTMANAGER_GROUP_SELECTED);
    }
    else
    {
        m_CurID = -1;
    }

    CListCtrl::OnLButtonDown(nFlags, point);
}


void GroupListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
    CListCtrl::OnMouseMove (nFlags, point);
}

void GroupListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    UINT uFlags = 0;
    int nHitItem = HitTest(point, &uFlags);

    if (uFlags & LVHT_ONITEM)
    {
        // double click works only if we don't have state icons,
        // or if we are in icon or small icon view
        if (GetViewType() == LVS_ICON ||
            GetViewType()==LVS_SMALLICON)
        {
            //CheckItem(nHitItem);
        }
    }

    CListCtrl::OnLButtonDblClk(nFlags, point);
}

void GroupListCtrl::OnLvnBeginDrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    //UNREFERENCED_PARAMETER(pResult);

    //LPNMITEMACTIVATE pNMItemActive = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    //

    //HGLOBAL hgData = GlobalAlloc(GHND|GMEM_SHARE, 10);
    //ASSERT(hgData!=NULL);

    //TCHAR* lpData = (TCHAR*)::GlobalLock(hgData);
    //ASSERT(lpData!=NULL);

    ////_tcscpy_s(lpData, strCurDragedItemDesc.GetLength() + 1, strCurDragedItemDesc);

    //::GlobalUnlock(hgData);

    //// Cache the data, and initiate DragDrop
    //m_OleDataSource.CacheGlobalData(CLIENT_MANAGER_CLIP_FMT, hgData);

    //DROPEFFECT dropEffect
    //    = m_OleDataSource.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE);

    //m_OleDataSource.Empty();

}

LRESULT GroupListCtrl::OnDrop(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1;

    CS3OleDropInfo* pDropInfo = reinterpret_cast<CS3OleDropInfo*>(wParam);

    int nItem = HitTest(pDropInfo->m_point);
    if (nItem >= 0)
    {
        INT CurGroup =  -1;

        CString keystr = GetItemText(nItem, 1);
        CurGroup = _ttoi(keystr);

        if (CurGroup < 0)
        {
            lr = 0L;
            return lr;
        }

        HGLOBAL hGlobal = NULL;
        CS3OleDropInfo* pInfo = (CS3OleDropInfo*)wParam;

        if (pInfo->m_pDataObject->IsDataAvailable(CLIENT_MANAGER_CLIP_FMT))
        {
            hGlobal = pInfo->m_pDataObject->GetGlobalData(CLIENT_MANAGER_CLIP_FMT);

            // Get pointer to data
            INT* pData = (INT*)GlobalLock(hGlobal);
            INT ClientCnt = pData[0];

            for (INT i = 1; i <= ClientCnt; i++)
            {
                INT ClientID = pData[i];

                ClientInfo client;
                HRESULT hr = GetControllerInterface->GetClient(ClientID, client);
                if (!SUCCEEDED(hr))
                {
                    if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                    {
                        GetControllerApp->OnControllerServerOffline();
                    }
                    else
                    {
                        TRACE0("Failed to Get Client!\n");
                    }
                    return 1;
                }

                if (ClientID >= 0 && CurGroup != client.GetGroupID())
                {
                    client.AddGroupID(CurGroup);

                    hr = GetControllerInterface->EditClientInfo(ClientID, client);
                    if (!SUCCEEDED(hr))
                    {
                        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                        {
                            GetControllerApp->OnControllerServerOffline();
                        }
                        else
                        {
                            TRACE0("Failed to Edit Client Info!\n");
                        }
                        return 1;
                    }

                    GetParent()->SendMessage(CLIENTMANAGER_GROUP_CLIENT_GROUP_CHANGE, ClientID, (LPARAM)&client);
                }
                else
                {

                }
            }

            // Unlock memory
            GlobalUnlock(hGlobal);
        }

    }

    return lr;
}

LRESULT GroupListCtrl::OnDragEnter(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = DROPEFFECT_MOVE;
    CS3OleDropInfo* pDropInfo = reinterpret_cast<CS3OleDropInfo*>(wParam);

    int nItem = HitTest(pDropInfo->m_point);
    if (nItem < 0)
    {
        lr = DROPEFFECT_NONE;
    }

    return lr;
}

LRESULT GroupListCtrl::OnDragOver(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = DROPEFFECT_MOVE;
    CS3OleDropInfo* pDropInfo = reinterpret_cast<CS3OleDropInfo*>(wParam);

    int nItem = HitTest(pDropInfo->m_point);
    if (nItem < 0)
    {
        lr = DROPEFFECT_NONE;
    }

    return lr;
}

LRESULT GroupListCtrl::OnDragLeave(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0;

    return lr;
}

// return value
// -100: no group selected   
//-1: offlinegroup
//>=0 : current group id 
INT GroupListCtrl::GetCurGroupID()
{
    POSITION pos = GetFirstSelectedItemPosition();
    INT index = GetNextSelectedItem(pos);

    if (index >= 0)
    {
        CString keystr = GetItemText(index, 1);
        m_CurID = _ttoi(keystr);
    }
    else
    {
        m_CurID = -100;
    }

    return m_CurID;
}

ClientGroup GroupListCtrl::GetCurClientGroup()
{
	ClientGroup LookUpGroup;
    
	int curid = GetCurGroupID();
	if(curid != -100)
	{
		if(!m_GroupsPool.Lookup(curid, LookUpGroup))
		{

		}
		else
		{

		}
	}
	return LookUpGroup; 
}

void GroupListCtrl::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    UpdateGroupsStatus();
    CListCtrl::OnTimer(nIDEvent);
}
