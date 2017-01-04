// GroupInfo.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "GroupInfoCtrl.h"
#include "afxdialogex.h"
#include "Localization/Localization.h"
#include "ControllerClient/PowerOffSchedule.h"

UINT CLIENTMANAGER_GROUPINFO_SAVE = ::RegisterWindowMessage (_T("CLIENTMANAGER_GROUPINFO_SAVE"));
UINT CLIENTMANAGER_GROUPINFO_CLIENT_GROUP_CHANGE  = ::RegisterWindowMessage (_T("CLIENTMANAGER_GROUPINFO_CLIENT_GROUP_CHANGE"));

extern CLIPFORMAT CLIENT_MANAGER_CLIP_FMT;

// CGroupInfoCtrl dialog

IMPLEMENT_DYNAMIC(CGroupInfoCtrl, CDialogEx)

CGroupInfoCtrl::CGroupInfoCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGroupInfoCtrl::IDD, pParent)
    , m_CurGroup (-1, _T(""), _T(""))
    , m_InitGroupID (-1)
    , m_bModified (FALSE)
{
}

CGroupInfoCtrl::~CGroupInfoCtrl()
{
}

void CGroupInfoCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GROUP, m_ClientsInGroup);
	DDX_Text(pDX, IDC_EDIT_GNAME, m_SelectedGroupName);
	DDX_Text(pDX, IDC_EDIT_GDESC, m_SelectedGroupDesc);
	DDX_Slider(pDX, IDC_SLIDER_GROUP_VOLUME, m_volume);
	DDV_MinMaxInt(pDX, m_volume, 0, 100);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_FROM, m_startTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_TO, m_endTime);
	DDX_Check(pDX, IDC_CHECK_ALWAYS_WORK, m_bAlwaysWork);
}

BEGIN_MESSAGE_MAP(CGroupInfoCtrl, CDialogEx)

    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_MOUSEMOVE()

//    ON_NOTIFY_EX(TTN_NEEDTEXT,0,OnToolTipNotify)

    //{{AFX_MSG_MAP(CGroupInfoCtrl)

    ON_REGISTERED_MESSAGE(WM_DND_DRAGENTER, OnDragEnter)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGOVER, OnDragOver)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGLEAVE, OnDragLeave)
    ON_REGISTERED_MESSAGE(WM_DND_DROP, OnDrop)

    ON_MESSAGE(USER_MSG_ID_UPDATE_CLIENT_STATUS, RefreshClientsStatus)

    ON_MESSAGE(USER_MSG_GET_CLIENTS_IN_GROUP, UpdateGroupInfo)


    //}}AFX_MSG_MAP
    ON_NOTIFY(NM_CLICK, IDC_LIST_GROUP, &CGroupInfoCtrl::OnNMClickListGroup)
    ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_GROUP, &CGroupInfoCtrl::OnLvnBegindragListGroup)
    ON_EN_CHANGE(IDC_EDIT_GNAME, &CGroupInfoCtrl::OnEnChangeEditGname)
    ON_EN_CHANGE(IDC_EDIT_GDESC, &CGroupInfoCtrl::OnEnChangeEditGdesc)
    ON_BN_CLICKED(IDC_BTN_SAVE_GROUP_INFO, &CGroupInfoCtrl::OnBnClickedBtnSaveGroupInfo)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_FROM, &CGroupInfoCtrl::OnDtnDatetimechangeDatetimepickerFrom)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_TO, &CGroupInfoCtrl::OnDtnDatetimechangeDatetimepickerTo)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_GROUP_VOLUME, &CGroupInfoCtrl::OnNMReleasedcaptureSliderGroupVolume)
	ON_BN_CLICKED(IDC_CHECK_ALWAYS_WORK, &CGroupInfoCtrl::OnBnClickedCheckAlwaysWork)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CGroupInfoCtrl)
    EASYSIZE(IDC_LIST_GROUP,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_GNAME,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_GDESC,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_EDIT_GNAME,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_EDIT_GDESC,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDOK,IDC_EDIT_GDESC,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDCANCEL,IDOK,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP


// CGroupInfoCtrl message handlers

BOOL CGroupInfoCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;

    SetTimer(0xFF000001, StringUtility::stoi(GetConfig->Get(CLIENT_MONITOR_REFRESH).GetString()) * 1000, 0);

    OnInitialUpdate();

    if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID,USER_MSG_GET_CLIENTS_IN_GROUP,
        (WPARAM)this->GetSafeHwnd(),(LPARAM)m_InitGroupID))//post thread msg
    {
        TRACE1("post message failed,errno:%d\n", ::GetLastError());
        return FALSE;
    }

    m_DropTarget.Register(this);

    EnableToolTips(TRUE);
    m_tooltip.Create(this); 
    m_tooltip.Activate(true);

    pwnd =GetDlgItem(IDC_CHECK_ALWAYS_WORK);  
    m_tooltip.AddTool(pwnd, _T("always work "));

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CGroupInfoCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}


/////////////////////////////////////////////////////////////////////////////
// data for the list view control
#define NUM_ITEMS   7

/////////////////////////////////////////////////////////////////////////////
// CGroupInfoCtrl initialization

void CGroupInfoCtrl::OnInitialUpdate()
{

    // set image lists

    m_LargeImageList.Create(THUMBNAIL_W, THUMBNAIL_H, ILC_COLOR32, 1, 1);
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

    m_ClientsInGroup.SetImageList(&m_LargeImageList, LVSIL_NORMAL);


    // insert columns

    int i;
    LV_COLUMN lvc;

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM;

    for(i = 0; i<3; i++)
    {
        lvc.iSubItem = i;
        lvc.cx = 100;
        lvc.fmt = LVCFMT_RIGHT;
        m_ClientsInGroup.InsertColumn(i,&lvc);
    }

    OnViewLargeIcons();

    Clear();
}

void CGroupInfoCtrl::Clear()
{
    ClearList();

    m_CurGroup = ClientGroup(-1, _T(""), _T(""));

    m_SelectedGroupName = _T("");
    m_SelectedGroupDesc = _T("");

    m_bModified = FALSE;

    GetDlgItem( IDOK)->EnableWindow(FALSE);

    ((CEdit *)GetDlgItem( IDC_EDIT_GNAME))->SetReadOnly(TRUE);
    ((CEdit *)GetDlgItem( IDC_EDIT_GDESC))->SetReadOnly(TRUE);

    UpdateData(FALSE);
}

void CGroupInfoCtrl::ClearList()
{
    m_ClientsPool.RemoveAll();

    m_ClientsInGroup.DeleteAllItems();
}

void CGroupInfoCtrl::OnTimer(UINT_PTR nIDEvent)
{
    UpdateClientsStatus();
}

void CGroupInfoCtrl::UpdateClientsStatus()
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
    int nCount = m_ClientsInGroup.GetItemCount(); 
    if(nCount > 0) 
    { 
        LVITEM lvitem; 
        lvitem.mask = LVIF_IMAGE;

        INT VisibleEnd = m_ClientsInGroup.GetTopIndex() + m_ClientsInGroup.GetCountPerPage() + 1;
        for(int i = m_ClientsInGroup.GetTopIndex(); i < VisibleEnd; ++i) 
        { 
            if (m_ClientsInGroup.IsItemVisible(i))
            {
                lvitem.iItem    = i; 
                lvitem.iSubItem = 0;
                m_ClientsInGroup.GetItem(&lvitem); 

                CString keystr = m_ClientsInGroup.GetItemText(i, 1);
                INT ClientID = _ttoi(keystr);
                pInfo[0].push_back(ClientID);

            }
        } 
		if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID,USER_MSG_ID_UPDATE_CLIENT_STATUS,
			(WPARAM)this->GetSafeHwnd(),(LPARAM)nBufID))//post thread msg
		{
			TRACE1("post message failed,errno:%d\n", ::GetLastError());
		}
    }

    
}


LRESULT CGroupInfoCtrl::RefreshClientsStatus(WPARAM wp, LPARAM lp)
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

    int nCount = m_ClientsInGroup.GetItemCount(); 
    if(nCount > 0) 
    { 
        LVITEM lvitem; 
        lvitem.mask = LVIF_IMAGE;

        INT VisibleEnd = m_ClientsInGroup.GetTopIndex() + m_ClientsInGroup.GetCountPerPage() + 1;
        for(int i = m_ClientsInGroup.GetTopIndex(); i < VisibleEnd; ++i) 
        {
            if (m_ClientsInGroup.IsItemVisible(i))
            {
                lvitem.iItem    = i; 
                lvitem.iSubItem = 0;
                m_ClientsInGroup.GetItem(&lvitem); 

                CString keystr = m_ClientsInGroup.GetItemText(i, 1);
                INT ClientID = _ttoi(keystr);

                std::map< int,ClientRTStatistics>::const_iterator it = pConstInfo->find(ClientID);
                if (it != pConstInfo->end())
                {
                    CString ClientIP;
                    lvitem.iImage = GetClientStatus(ClientID, ClientIP, *pConstInfo);

                    m_ClientsInGroup.SetItem(&lvitem);
                    m_ClientsInGroup.SetItemText(i, 2, ClientIP);
                }

            }
        } 
    } 

    return 0;
}


LRESULT CGroupInfoCtrl::UpdateGroupInfo (WPARAM wp, LPARAM lp)
{
    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf< std::pair< std::pair<CString, ClientGroup>, std::pair<std::vector<int>,  
        std::map<int, ClientInfo> > > > MessageBuf;
    if (!WorkThreadDataManager< std::pair< std::pair<CString, ClientGroup >, std::pair<std::vector<int>,  
        std::map<int, ClientInfo> > > >::GetDataBuf(nMessageBufID, MessageBuf))
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////

    SetCurGroup(MessageBuf.Get()->first.second);

    ClearList();

    for (std::map<int, ClientInfo>::const_iterator it = MessageBuf.Get()->second.second.begin(); 
        it != MessageBuf.Get()->second.second.end(); ++it)
    {
        AddClient(it->second, it->first);
    }

    UpdateClientsStatus();

    return 0;
}

BOOL CGroupInfoCtrl::AddClient(const ClientInfo& client, INT clientID)
{
    // insert items
    ClientInfo info; 
    //CString key = client.GetHardwareInfo().c_str();
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
    lvi.iItem = m_ClientsInGroup.GetItemCount();
    lvi.iSubItem = 0;
    lvi.pszText = (LPWSTR)name.GetString();
    lvi.iImage = CLIENT_DISCONNECT;

    int index = m_ClientsInGroup.InsertItem(&lvi);
    TCHAR buf[16];
    _itot_s(key, buf, 10);
    m_ClientsInGroup.SetItemText(index,1,buf);
    m_ClientsInGroup.SetItemText(index,2,ClientIP);

    return TRUE;
}

BOOL CGroupInfoCtrl::RemoveClient(const ClientInfo& client, INT clientID)
{
    ClientInfo info; 
    INT key = clientID;

    if (m_ClientsPool.Lookup(key, info))
    {
        m_ClientsPool.RemoveKey(key);

        TCHAR buf[16];
        _itot_s(key, buf, 10);

        int n = m_ClientsInGroup.GetItemCount();
        for (int i=0;i<n;i++)
        {
            CString strText = m_ClientsInGroup.GetItemText(i,1);
            if(strText == buf)
            {
                m_ClientsInGroup.DeleteItem(i);
                break;
            }
        }

        return TRUE;
    }

    return FALSE;
}


BOOL CGroupInfoCtrl::UpdateClient(const ClientInfo& client, INT clientID)
{
    ClientInfo info; 
    INT key = clientID;

    if (m_ClientsPool.Lookup(key, info))
    {
        if (m_CurGroup.GetGroupID() != client.GetGroupID())
        {
            RemoveClient(client, clientID);
            RedrawWindow();
        }
    }
    else
    {
        if (m_CurGroup.GetGroupID() == client.GetGroupID())
        {
            AddClient(client, clientID);
            RedrawWindow();
        }
    }
    
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGroupInfoCtrl helpers

BOOL CGroupInfoCtrl::SetViewType(DWORD dwViewType)
{
    return(m_ClientsInGroup.ModifyStyle(LVS_TYPEMASK,dwViewType & LVS_TYPEMASK));
}

DWORD CGroupInfoCtrl::GetViewType()
{
    return(m_ClientsInGroup.GetStyle() & LVS_TYPEMASK);
}

/////////////////////////////////////////////////////////////////////////////
// CGroupInfoCtrl message handlers

void CGroupInfoCtrl::OnViewSmallIcons()
{
    if (GetViewType() != LVS_SMALLICON)
        SetViewType(LVS_SMALLICON);
}

void CGroupInfoCtrl::OnViewLargeIcons()
{
    if (GetViewType() != LVS_ICON)
        SetViewType(LVS_ICON);
}

void CGroupInfoCtrl::OnViewList()
{
    if (GetViewType() != LVS_LIST)
        SetViewType(LVS_LIST);
}

void CGroupInfoCtrl::OnViewDetails()
{
    if ((GetViewType() != LVS_REPORT))
    {
        if (GetViewType() != LVS_REPORT)
            SetViewType(LVS_REPORT);
    }
}


LRESULT CGroupInfoCtrl::OnDrop(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1L;

    INT CurGroup = GetCurGroup().GetGroupID();
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
                AddClient(client, ClientID);

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

                GetParent()->SendMessage(CLIENTMANAGER_GROUPINFO_CLIENT_GROUP_CHANGE, ClientID, (LPARAM)&client);
            }
            else
            {

            }
        }

        // Unlock memory
        GlobalUnlock(hGlobal);
    }

    return lr;
}

LRESULT CGroupInfoCtrl::OnDragEnter(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = DROPEFFECT_MOVE;

    return lr;
}

LRESULT CGroupInfoCtrl::OnDragOver(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = DROPEFFECT_MOVE;

    return lr;
}

LRESULT CGroupInfoCtrl::OnDragLeave(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0;

    return lr;
}

void CGroupInfoCtrl::OnNMClickListGroup(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Add your control notification handler code here

    //if (pNMItemActivate->iItem >= 0)
    //{
    //    CString ClientIP = m_ClientsInGroup.GetItemText(pNMItemActivate->iItem, 2);
    //    // this is not a real solution.
    //    GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, (LPARAM)ClientIP.GetBuffer());
    //}

    *pResult = 0;
}


void CGroupInfoCtrl::OnBnClickedButtonSave()
{
    // TODO: Add your control notification handler code here

    UpdateData(TRUE);

    m_CurGroup.SetGroupDesc(m_SelectedGroupDesc.GetString());
    m_CurGroup.SetGroupName(m_SelectedGroupName.GetString());
	m_CurGroup.SetVolume(m_volume);

	PowerOffSchedule powerOffSchedule;

	if (!m_bAlwaysWork)
	{
		S3Time startTime = S3Time::CreateTime(m_startTime);
		S3Time endTime   = S3Time::CreateTime(m_endTime);	
		powerOffSchedule.AddPowerOffDefine(std::make_pair(endTime, startTime));
	}	

	m_CurGroup.SetWorkTime(powerOffSchedule.ToXML());

    GetParent()->SendMessage(CLIENTMANAGER_GROUPINFO_SAVE);
}


BOOL CGroupInfoCtrl::SetCurGroup(const ClientGroup& group)
{
    m_CurGroup = group;

    m_SelectedGroupName = Translate(m_CurGroup.GetGroupName().c_str());
    m_SelectedGroupDesc = Translate(m_CurGroup.GetGroupDesc().c_str());
	m_volume            = m_CurGroup.GetVolume();

	m_bModified = FALSE;

	GetDlgItem( IDOK)->EnableWindow(m_bModified);

    if (m_CurGroup.GetGroupID() != S3NET_DEFAULT_CLIENTGROUP_ID)
    {
        ((CEdit *)GetDlgItem( IDC_EDIT_GNAME))->SetReadOnly(FALSE);
    }
	((CEdit *)GetDlgItem( IDC_EDIT_GDESC))->SetReadOnly(FALSE);

	 UpdateData(FALSE);

	if (m_CurGroup.GetWorkTime() != _T(""))
	{
		std::string message = StringUtility::ConvertUnicodeToUTF8(m_CurGroup.GetWorkTime());

		TiXmlDocument xml;
		xml.Parse(reinterpret_cast<LPCSTR>(message.c_str()), NULL, TIXML_ENCODING_UTF8);

		TiXmlElement* pPowerOffSchedule = xml.RootElement();
		if (NULL == pPowerOffSchedule)
		{
			ASSERT(0);
			return FALSE;
		}

		PowerOffSchedule powerOffSchedule;
		if (!powerOffSchedule.LoadFromXML(pPowerOffSchedule))
		{
			ASSERT(0);
			return FALSE;
		}

		std::vector<std::pair<S3Time, S3Time>> poweroffDefine;
		powerOffSchedule.LoadPowerOffDefine(poweroffDefine);

		if (poweroffDefine.size() > 0)
		{
			S3Time endTime   = poweroffDefine[0].first;
			S3Time startTime = poweroffDefine[0].second;

			m_startTime  = startTime.ToCOleDateTime();
			m_endTime    = endTime.ToCOleDateTime();			
		}
		else
		{
			m_bAlwaysWork = TRUE;
			GetDlgItem(IDC_DATETIMEPICKER_FROM)->EnableWindow(FALSE);
			GetDlgItem(IDC_DATETIMEPICKER_TO)->EnableWindow(FALSE);
		}

		UpdateData(FALSE);
	}   

    return TRUE;
}


BOOL CGroupInfoCtrl::InitGroup(INT GroupID)
{
     m_InitGroupID = GroupID;
     return TRUE;
}

INT CGroupInfoCtrl::GetCurClientID()
{
    POSITION pos = m_ClientsInGroup.GetFirstSelectedItemPosition();
    INT index = m_ClientsInGroup.GetNextSelectedItem(pos);

    if (index >= 0)
    {
        CString keystr = m_ClientsInGroup.GetItemText(index, 1);
        m_CurClientID = _ttoi(keystr);
    }
    else
    {
        m_CurClientID = -1;
    }

    return m_CurClientID;
}

void CGroupInfoCtrl::OnLvnBegindragListGroup(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here

    UNREFERENCED_PARAMETER(pResult);

    LPNMITEMACTIVATE pNMItemActive = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    INT Cnt = m_ClientsInGroup.GetSelectedCount();
    INT indexCnt = 0;
    INT* indexBuf = new INT[Cnt];

    POSITION pos = m_ClientsInGroup.GetFirstSelectedItemPosition();
    INT index = m_ClientsInGroup.GetNextSelectedItem(pos);

    while (index >= 0)
    {
        CString keystr = m_ClientsInGroup.GetItemText(index, 1);
        indexBuf[indexCnt] = _ttoi(keystr);
        indexCnt++;
        index = m_ClientsInGroup.GetNextSelectedItem(pos);
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


void CGroupInfoCtrl::OnEnChangeEditGname()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here

    m_bModified = TRUE;

    GetDlgItem( IDOK)->EnableWindow(m_bModified);

}


void CGroupInfoCtrl::OnEnChangeEditGdesc()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here

    m_bModified = TRUE;

    GetDlgItem( IDOK)->EnableWindow(m_bModified);
}

void CGroupInfoCtrl::OnBnClickedBtnSaveGroupInfo()
{
    // TODO: Add your control notification handler code here

    OnBnClickedButtonSave();
    m_bModified = FALSE;

    GetDlgItem( IDOK)->EnableWindow(m_bModified);

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
        (LPARAM)Translate(_T("Group Information Changed!")).GetString());
}



BOOL  CGroupInfoCtrl::OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR; 
    UINT nID =pNMHDR->idFrom; 
    // idFrom is actually the HWND of the control=IDC_xxx 
    if (pTTT->hdr.code==TTN_NEEDTEXT) 
    { 
        if (pTTT->uFlags & TTF_IDISHWND) 
        { 
            if(((CStatic*)GetDlgItem(IDC_LIST_GROUP))->m_hWnd==(HWND)nID)// 
            { 
                pTTT->lpszText= _T("xxxxxxxxx!");//TT 
            } 
            return TRUE; 
        }
        return(FALSE); 
    }

    return FALSE;
}

void CGroupInfoCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
    m_tooltip.UpdateTipText(Translate(_T("Client always on work status.")), pwnd);
}

BOOL CGroupInfoCtrl::PreTranslateMessage(MSG* pMsg)
{
    if(m_tooltip.m_hWnd!=NULL)  
        m_tooltip.RelayEvent(pMsg);

    return __super::PreTranslateMessage(pMsg);
}

void CGroupInfoCtrl::OnCancel()
{
     CDialogEx::OnCancel();
}

void CGroupInfoCtrl::OnOK()
{
    OnBnClickedButtonSave();

    CDialogEx::OnOK();
}


void CGroupInfoCtrl::OnDtnDatetimechangeDatetimepickerFrom(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here


	COleDateTime starttime,endtime;
	((CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER_FROM))->GetTime(starttime);
	((CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER_TO))->GetTime(endtime);
	if(0== starttime.GetHour() && 0== starttime.GetMinute() && 0==starttime.GetSecond() &&
		0== endtime.GetHour() && 0== endtime.GetMinute() && 0==endtime.GetSecond())
	{
		m_bAlwaysWork = true;
		GetDlgItem(IDC_DATETIMEPICKER_FROM)->EnableWindow(FALSE);
		GetDlgItem(IDC_DATETIMEPICKER_TO)->EnableWindow(FALSE);
		UpdateData(false);
	}

	m_bModified = TRUE;
	GetDlgItem( IDOK)->EnableWindow(m_bModified);

	*pResult = 0;
}


void CGroupInfoCtrl::OnDtnDatetimechangeDatetimepickerTo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	

	COleDateTime starttime,endtime;
	((CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER_FROM))->GetTime(starttime);
	((CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER_TO))->GetTime(endtime);
	if(0== starttime.GetHour() && 0== starttime.GetMinute() && 0==starttime.GetSecond() &&
		0== endtime.GetHour() && 0== endtime.GetMinute() && 0==endtime.GetSecond())
	{
		m_bAlwaysWork = true;
		GetDlgItem(IDC_DATETIMEPICKER_FROM)->EnableWindow(FALSE);
		GetDlgItem(IDC_DATETIMEPICKER_TO)->EnableWindow(FALSE);
		UpdateData(false);
	}

	m_bModified = TRUE;
	GetDlgItem( IDOK)->EnableWindow(m_bModified);

	*pResult = 0;
}


void CGroupInfoCtrl::OnNMReleasedcaptureSliderGroupVolume(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	m_bModified = TRUE;

	GetDlgItem( IDOK)->EnableWindow(m_bModified);

	*pResult = 0;
}


void CGroupInfoCtrl::OnBnClickedCheckAlwaysWork()
{
	// TODO: Add your control notification handler code here
	m_bModified = TRUE;

	GetDlgItem( IDOK)->EnableWindow(m_bModified);

	 UpdateData(TRUE);
	 if (m_bAlwaysWork)
	 {
		 GetDlgItem(IDC_DATETIMEPICKER_FROM)->EnableWindow(FALSE);
		 GetDlgItem(IDC_DATETIMEPICKER_TO)->EnableWindow(FALSE);
	 }
	 else
	 {
		 GetDlgItem(IDC_DATETIMEPICKER_FROM)->EnableWindow(TRUE);
		 GetDlgItem(IDC_DATETIMEPICKER_TO)->EnableWindow(TRUE);
	 }
}
