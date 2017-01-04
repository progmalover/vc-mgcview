
#include "stdafx.h"
#include "ClientListCtrl.h"
#include "Controller.h"

extern CLIPFORMAT CLIENT_MANAGER_CLIP_FMT;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ClientListCtrl

IMPLEMENT_DYNCREATE(ClientListCtrl, CListCtrl)

BEGIN_MESSAGE_MAP(ClientListCtrl, CListCtrl)
    //{{AFX_MSG_MAP(ClientListCtrl)
    ON_WM_CREATE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
    ON_NOTIFY_REFLECT(LVN_GETINFOTIP, OnGetToolTip)

    ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnLvnBeginDrag)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGENTER, OnDragEnter)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGOVER, OnDragOver)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGLEAVE, OnDragLeave)
    ON_REGISTERED_MESSAGE(WM_DND_DROP, OnDrop)

    ON_MESSAGE(USER_MSG_ID_UPDATE_CLIENT_STATUS, RefreshClientsStatus)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ClientListCtrl construction/destruction

ClientListCtrl::ClientListCtrl()
{

}

ClientListCtrl::~ClientListCtrl()
{

}

BOOL ClientListCtrl::PreCreateWindow(CREATESTRUCT& cs)
{

    cs.style |= LVS_ICON | LVS_AUTOARRANGE | LVS_SHOWSELALWAYS;

    return CListCtrl::PreCreateWindow(cs);
}

int ClientListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CListCtrl::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    SetExtendedStyle(GetExtendedStyle() | LVS_EX_INFOTIP);

    m_DropTarget.Register (this);

    return 1;
}

/////////////////////////////////////////////////////////////////////////////
// data for the list view control
#define NUM_ITEMS   7

/////////////////////////////////////////////////////////////////////////////
// ClientListCtrl initialization

void ClientListCtrl::OnInitialUpdate()
{

    // set image lists

    m_LargeImageList.Create(THUMBNAIL_W, THUMBNAIL_H, ILC_COLOR32, 3, 1);
    m_LargeImageList.SetImageCount(CLIENT_STATUS_CNT);

    CBitmap bitmap;
    bitmap.LoadBitmap(IDB_CLIENT_STATUS_WELL);
    m_LargeImageList.Replace(CLIENT_WELL, &bitmap, NULL);
    bitmap.DeleteObject();
    bitmap.LoadBitmap(IDB_CLIENT_CRITICAL);
    m_LargeImageList.Replace(CLIENT_CRITICAL, &bitmap, NULL);
    bitmap.DeleteObject();
    bitmap.LoadBitmap(IDB_CLIENT_DISCONNECTED);
    m_LargeImageList.Replace(CLIENT_DISCONNECT, &bitmap, NULL);
    bitmap.DeleteObject();
    bitmap.LoadBitmap(IDB_CLIENT_NEED_ATTENTION);
    m_LargeImageList.Replace(CLIENT_NEED_ATTENTION, &bitmap, NULL);
    bitmap.DeleteObject();
    
    m_LargeImageList.SetOverlayImage(NUM_ITEMS, 1);

    SetImageList(&m_LargeImageList, LVSIL_NORMAL);


    // insert columns

    int i;
    LV_COLUMN lvc;

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM;

    for(i = 0; i<3; i++)
    {
        lvc.iSubItem = i;
        lvc.cx = 100;
        lvc.fmt = LVCFMT_RIGHT;
        InsertColumn(i,&lvc);
    }

    OnViewLargeIcons();
}

void ClientListCtrl::ClearList()
{
    m_ClientsPool.RemoveAll();

    DeleteAllItems();
}

BOOL ClientListCtrl::InitGroupsInfo(const std::vector<ClientGroup>& groups)
{
    m_GroupsPool.RemoveAll();
    INT key;
    for (auto it = groups.begin(); it != groups.end(); it++)
    {
        key = it->GetGroupID();
        m_GroupsPool.SetAt(key, const_cast<ClientGroup&>(*it));
    }

    return TRUE;
}

BOOL ClientListCtrl::AddClient(const ClientInfo& client, INT clientID)
{
    // insert items
    ClientInfo info; 
    INT key = clientID;

    if (m_ClientsPool.Lookup(key, info))
    {
        return FALSE;
    }

    info = client;
    m_ClientsPool.SetAt(key, info);

    LV_ITEM lvi;

    CString ClientIP;

    CString name = client.GetClientDesc().c_str();
    lvi.mask = LVIF_TEXT | LVIF_IMAGE;
    lvi.iItem = this->GetItemCount();
    lvi.iSubItem = 0;
    lvi.pszText = (LPWSTR)name.GetString();
    lvi.iImage = CLIENT_DISCONNECT;

    int index = InsertItem(&lvi);
    TCHAR buf[16];
    _itot_s(key, buf, 10);
    SetItemText(index,1,buf);

    SetItemText(index,2,ClientIP);

    return TRUE;
}

BOOL ClientListCtrl::RemoveClient(const ClientInfo& client, INT clientID)
{
    ClientInfo info; 
    INT key = clientID;

    if (m_ClientsPool.Lookup(key, info))
    {
        m_ClientsPool.RemoveKey(key);

        TCHAR buf[16];
        _itot_s(key, buf, 10);

        int n = GetItemCount();
        for (int i=0;i<n;i++)
        {
            CString strText = GetItemText(i,1);
            if(strText == buf)
            {
                DeleteItem(i);
                break;
            }
        }

        return TRUE;
    }

    return FALSE;
}

BOOL ClientListCtrl::UpdateClient(const ClientInfo& client, INT clientID)
{
    ClientInfo info; 
    INT key = clientID;

    if (m_ClientsPool.Lookup(key, info))
    {
        info = client;
        m_ClientsPool.SetAt(key, info);
    }

    return TRUE;
}


INT ClientListCtrl::GetCurClientID(CString& IPAddress)
{
    POSITION pos = GetFirstSelectedItemPosition();
    INT index = GetNextSelectedItem(pos);

    if (index >= 0)
    {
        CString keystr = GetItemText(index, 1);
        m_CurClientID = _ttoi(keystr);

        IPAddress = GetItemText(index, 2);
    }
    else
    {
        m_CurClientID = -1;
    }

    return m_CurClientID;
}

void ClientListCtrl::UpdateClientsStatus()
{
    if (!GetControllerApp->IsLogin())
    {
        return;
    }

    //How to new a buf and post it to work thread
    UINT nBufID;
    std::vector<int>* pInfo = WorkThreadDataManager< std::vector<int> >::NewDataBuf(nBufID); //create dynamic msg
    if (pInfo == NULL)
    {
        return;
    }
    //Do any thing with your new buf
    int nCount = GetItemCount(); 
    if(nCount > 0) 
    { 
        LVITEM lvitem; 
        lvitem.mask = LVIF_IMAGE;

        INT VisibleEnd = GetTopIndex() + GetCountPerPage() + 1;
        for(int i = GetTopIndex(); i < VisibleEnd; ++i) 
        { 
            if (IsItemVisible(i))
            {
                lvitem.iItem    = i; 
                lvitem.iSubItem = 0;
                GetItem(&lvitem); 

                CString keystr = GetItemText(i, 1);
                INT ClientID = _ttoi(keystr);
                pInfo[0].push_back(ClientID);

            }
        } 
    }

    if(!PostThreadMessage(GetControllerApp->m_nWorkThreadID,USER_MSG_ID_UPDATE_CLIENT_STATUS,
        (WPARAM)this->GetSafeHwnd(),(LPARAM)nBufID))//post thread msg
    {
        printf("post message failed,errno:%d\n",::GetLastError());
    }
}

LRESULT ClientListCtrl::RefreshClientsStatus(WPARAM wp, LPARAM lp)
{
    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf< std::map< int,ClientRTStatistics> > MessageBuf;
    if (!WorkThreadDataManager< std::map< int,ClientRTStatistics>  >::GetDataBuf(nMessageBufID, MessageBuf))
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////

    const std::map< int,ClientRTStatistics>* pConstInfo = MessageBuf.Get();

    int nCount = GetItemCount(); 
    if(nCount > 0) 
    { 
        LVITEM lvitem; 
        lvitem.mask = LVIF_IMAGE;

        INT VisibleEnd = GetTopIndex() + GetCountPerPage() + 1;
        for(int i = GetTopIndex(); i < VisibleEnd; ++i) 
        { 
            if (IsItemVisible(i))
            {
                lvitem.iItem    = i; 
                lvitem.iSubItem = 0;
                GetItem(&lvitem); 

                CString keystr = GetItemText(i, 1);
                INT ClientID = _ttoi(keystr);

                std::map< int,ClientRTStatistics>::const_iterator it = pConstInfo->find(ClientID);
                if (it != pConstInfo->end())
                {
                    CString ClientIP;
                    lvitem.iImage = GetClientStatus(ClientID, ClientIP, *pConstInfo);

                    SetItem(&lvitem);
                    SetItemText(i, 2, ClientIP);
                }
            }
            

        } 
    } 

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ClientListCtrl diagnostics

#ifdef _DEBUG
void ClientListCtrl::AssertValid() const
{
    CListCtrl::AssertValid();
}

void ClientListCtrl::Dump(CDumpContext& dc) const
{
    CListCtrl::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// ClientListCtrl helpers

BOOL ClientListCtrl::SetViewType(DWORD dwViewType)
{
    return(ModifyStyle(LVS_TYPEMASK,dwViewType & LVS_TYPEMASK));
}

DWORD ClientListCtrl::GetViewType()
{
    return(GetStyle() & LVS_TYPEMASK);
}

/////////////////////////////////////////////////////////////////////////////
// ClientListCtrl message handlers

void ClientListCtrl::OnViewSmallIcons()
{
    if (GetViewType() != LVS_SMALLICON)
        SetViewType(LVS_SMALLICON);
}

void ClientListCtrl::OnViewLargeIcons()
{
    if (GetViewType() != LVS_ICON)
        SetViewType(LVS_ICON);
}

void ClientListCtrl::OnViewList()
{
    if (GetViewType() != LVS_LIST)
        SetViewType(LVS_LIST);
}

void ClientListCtrl::OnViewDetails()
{
    if ((GetViewType() != LVS_REPORT))
    {
        if (GetViewType() != LVS_REPORT)
            SetViewType(LVS_REPORT);
    }
}

void ClientListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
    CListCtrl::OnLButtonUp(nFlags, point);
}

void ClientListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    CListCtrl::OnLButtonDown(nFlags, point);
}


void ClientListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
    CListCtrl::OnMouseMove (nFlags, point);
}

void ClientListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
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

void ClientListCtrl::OnLvnBeginDrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    UNREFERENCED_PARAMETER(pResult);

    LPNMITEMACTIVATE pNMItemActive = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    INT Cnt = GetSelectedCount();
    INT indexCnt = 0;
    INT* indexBuf = new INT[Cnt];

    POSITION pos = GetFirstSelectedItemPosition();
    INT index = GetNextSelectedItem(pos);

    while (index >= 0)
    {
        CString keystr = GetItemText(index, 1);
        indexBuf[indexCnt] = _ttoi(keystr);
        indexCnt++;
        index = GetNextSelectedItem(pos);
    }

    INT Len = sizeof(INT) + indexCnt*sizeof(INT);

    HGLOBAL hgData = GlobalAlloc(GHND|GMEM_SHARE, Len);
    ASSERT(hgData!=NULL);

    INT* lpData = (INT*)::GlobalLock(hgData);
    ASSERT(lpData!=NULL);

    lpData[0] = indexCnt;
    memcpy(lpData+1, indexBuf, indexCnt*sizeof(INT));
    delete indexBuf;

    ::GlobalUnlock(hgData);

    // Cache the data, and initiate DragDrop
    m_OleDataSource.CacheGlobalData(CLIENT_MANAGER_CLIP_FMT, hgData);

    DROPEFFECT dropEffect
        = m_OleDataSource.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE);

    m_OleDataSource.Empty();

}

LRESULT ClientListCtrl::OnDrop(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1;

    return lr;
}

LRESULT ClientListCtrl::OnDragEnter(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = DROPEFFECT_MOVE;

    return lr;
}

LRESULT ClientListCtrl::OnDragOver(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = DROPEFFECT_MOVE;

    return lr;
}

LRESULT ClientListCtrl::OnDragLeave(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0;

    return lr;
}

void ClientListCtrl::OnGetToolTip(NMHDR* pNMHDR, LRESULT *pResult)
{
    LPNMLVGETINFOTIPW pToolTip = (LPNMLVGETINFOTIPW)pNMHDR;

    CString szTip;

    INT key = -1;

    if (pToolTip->iItem >= 0)
    {
        CString keystr = GetItemText(pToolTip->iItem, 1);
        key = _ttoi(keystr);
    }

    CString ClientIP = GetItemText(pToolTip->iItem, 2);

    ClientInfo info; 

    if (m_ClientsPool.Lookup(key, info))
    {
        INT GroupID = info.GetGroupID();
        ClientGroup group;
        if (m_GroupsPool.Lookup(GroupID, group))
        {
            CString GroupTip;
            GroupTip.Format( _T("This client %s belongs to group:"), ClientIP);
            GroupTip += group.GetGroupName().c_str();
            _tcscpy_s(pToolTip->pszText, INFOTIPSIZE, (LPCWSTR)GroupTip);
        }
    }

    pResult = 0;

}