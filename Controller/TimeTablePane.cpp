#include "StdAfx.h"
#include "TimeTablePane.h"

extern UINT TIMETABLE_TAB_BEFORE_CLOSE = ::RegisterWindowMessage(_T("TIMETABLE_TAB_BEFORE_CLOSE;"));
#define TIMETABLE_CLOSE_ALL TIMETABLE_AFTERUNITDELETED
static COleDateTimeSpan Convert2Span(DWORD Duration)
{
    DWORD Days      = Duration/(24*60*60);
    DWORD Hours     = (Duration%(24*60*60))/(60*60);
    DWORD Minites   = (Duration%(60*60))/60;
    DWORD Seconds   = Duration%60;
    return COleDateTimeSpan(Days, Hours, Minites, Seconds);
}

static double Convert2Duration(COleDateTimeSpan Span)
{
    return Span.GetTotalSeconds();
}



// CClientStatePane

IMPLEMENT_DYNAMIC(CTimeTablePane, CBaseControllerPane)

CTimeTablePane::CTimeTablePane()
{
}

CTimeTablePane::~CTimeTablePane()
{

}


BEGIN_MESSAGE_MAP(CTimeTablePane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_TIMER()
END_MESSAGE_MAP()



// CClientStatePane message handlers
int CTimeTablePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    RECT rect;
    GetClientRect(&rect);

    m_wndTab.Create(CMFCTabCtrl::STYLE_3D_VS2005, rect, this, IDW_LIST_CLIENT_TAB);
    m_wndTab.SetLocation(CMFCBaseTabCtrl::LOCATION_TOP);
    m_wndTab.EnableActiveTabCloseButton(FALSE);
    m_wndTab.AutoDestroyWindow(TRUE);

    SetMainWnd(&m_wndTab);

    return 0;
}


void CTimeTablePane::OnDestroy()
{
    m_wndTab.RemoveAllTabs();
    m_wndTab.DestroyWindow();
    CBaseControllerPane::OnDestroy();
}


//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CTimeTableTab, CMFCTabCtrl)

CTimeTableTab::CTimeTableTab()
{
    m_ItemIconList.Create(THUMBNAIL_W, THUMBNAIL_H, ILC_COLOR32|ILC_MASK, 1, 1);

    m_ItemIconList.Add(AfxGetApp()->LoadIconW(IDI_EMPTYCONTENT));
}

CTimeTableTab::~CTimeTableTab()
{
    auto it = m_wndTables.begin(), ite = m_wndTables.end();
    for (; it != ite; ++it)
    {
        it->first->DestroyWindow();
        delete it->first;
    }

    m_wndTables.clear();

    m_ItemIconList.DeleteImageList();
}

BEGIN_MESSAGE_MAP(CTimeTableTab, CMFCTabCtrl)
    ON_WM_CREATE()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


int CTimeTableTab::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMFCTabCtrl::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}


void CTimeTableTab::FireChangeActiveTab(int nNewTab)
{
    CMFCTabCtrl::FireChangeActiveTab(nNewTab);

    EnableActiveTabCloseButton(TRUE);
}


void CTimeTableTab::SetCurTabModified()
{
     int iActive = GetActiveTab();

     CString Label;
     GetTabLabel(iActive, Label);

     if (Label.GetAt(Label.GetLength() - 1) != _T('*'))
     {
         Label += _T("*");
         SetTabLabel(iActive, Label);
     }
}

void CTimeTableTab::ClearTabModified(int iTab)
{
    CString Label;
    GetTabLabel(iTab, Label);

    if (Label.GetAt(Label.GetLength() - 1) == _T('*'))
    {
        Label = Label.Left(Label.GetLength() - 1);
        SetTabLabel(iTab, Label);
    }

    CTimeTableCtrl* pCtrl = (CTimeTableCtrl*)GetTabWnd(iTab);
    pCtrl->ClearModified();
}

BOOL CTimeTableTab::IsTablModified(int iTab)
{
    CString Label;
    GetTabLabel(iTab, Label);

    if (Label.GetAt(Label.GetLength() - 1) == _T('*'))
    {
        return TRUE;
    }

    return FALSE;
}

void CTimeTableTab::ClearTableModified(INT TableID)
{

    CWnd* pWnd = NULL;

    for(auto it = m_wndTables.begin(); it != m_wndTables.end(); it++)
    {
        if (it->second.GetTimetableID() == TableID)
        {
            pWnd = it->first;

            CTimeTableCtrl* pCtrl = (CTimeTableCtrl*)it->first;

            pCtrl->ClearModified();
            break;
        }
    }

    if (pWnd)
    {
        for (int i = 0; i < GetTabsNum(); i++)
        {

            if (GetTabWnd(i) == pWnd)
            {
                CString Label;
                GetTabLabel(i, Label);

                if (Label.GetAt(Label.GetLength() - 1) == _T('*'))
                {
                    Label = Label.Left(Label.GetLength() - 1);
                    SetTabLabel(i, Label);
                }

                break;;
            }
        }
    }
}

void CTimeTableTab::ClearCurTabModified()
{
    int iActive = GetActiveTab();

    ClearTabModified(iActive);
}

void CTimeTableTab::ClearAllTabModified()
{
    for (int i = 0; i < GetTabsNum(); i++)
    {
        ClearTabModified(i);
    }
}

BOOL CTimeTableTab::RemoveTab(int iTab, BOOL bRecalcLayout /*= TRUE*/)
{
    CWnd* wnd = GetTabWnd(iTab);

    auto it = m_wndTables.find(wnd), ite = m_wndTables.end();
    if (it != ite)
    {
        GetParent()->SendMessage(TIMETABLE_TAB_BEFORE_CLOSE, (WPARAM)&it->second, (LPARAM)iTab);
    }

    m_wndTables.erase(wnd);

    CMFCTabCtrl::RemoveTab(iTab, bRecalcLayout);
    delete wnd;

    return TRUE;
}

void CTimeTableTab::OnLButtonUp(UINT nFlags, CPoint point)
{
    CWnd* pWndActive = NULL;

    if (m_bTabCloseButtonPressed)
    {
        if (m_rectCloseButton.PtInRect(point))
        {
            pWndActive = GetActiveWnd();
        }
    }

    CMFCTabCtrl::OnLButtonUp(nFlags, point);
    if (pWndActive)
    {
        RemoveTab(GetActiveTab());
		if(m_wndTables.size() <= 0)
		{
			GetOwner()->SendMessage(TIMETABLE_CLOSE_ALL,0,0);
		}
    }
}

void CTimeTableTab::ChangeCurLabel(const CString& newLabel)
{
     SetTabLabel(GetActiveTab(), newLabel);
}

void CTimeTableTab::ChangeWndLabel(const CWnd* pWnd, const CString& newLabel)
{
    for (int i = 0; i < GetTabsNum(); i++)
    {

        if (GetTabWnd(i) == pWnd)
        {
            SetTabLabel(i, newLabel);
            break;;
        }
    }
}

BOOL CTimeTableTab::CheckTimeTableExist(CString TableName, int *pIndex)
{

	BOOL bAlreadyExist = TRUE;

	INT nTry = 1;

	bAlreadyExist = FALSE;
	for (int i = 0; i < GetTabsNum(); i++)
	{
		CString Label;

		if (GetTabLabel(i, Label) && (Label == TableName || Label == TableName + _T('*')) ||
			Label == TableName + Translate(_T("(ReadOnly)")) || Label == TableName + Translate(_T("(ReadOnly)*")))
		{
			*pIndex = i;
			bAlreadyExist = TRUE;
			break;
		}
	}

	return bAlreadyExist;
	
}

BOOL CTimeTableTab::AddTimeTable(Timetable& Table, BOOL bForceAdd, BOOL bReadOnly)
{
    if (bForceAdd)
    {
        CString TableName = Translate(_T("New TimeTable"));

        BOOL bAlreadyExist = TRUE;
        INT nTry = 1;
        while (bAlreadyExist)
        {
            bAlreadyExist = FALSE;
            for (int i = 0; i < GetTabsNum(); i++)
            {
                CString Label;

                if (GetTabLabel(i, Label) && (Label == TableName || Label == TableName + _T('*')) ||
                    Label == TableName + Translate(_T("(ReadOnly)")) || Label == TableName + Translate(_T("(ReadOnly)*")))
                {
                    nTry++;
                    TableName.Format(Translate(_T("New TimeTable (%d)")), nTry);
                    bAlreadyExist = TRUE;
                    break;
                }
            }
        }

        Table.SetTimetableName(TableName.GetString());
    }
    else
    {
        for (auto it = m_wndTables.begin(); it != m_wndTables.end(); it++)
        {
            if (it->second.GetTimetableID() == Table.GetTimetableID())
            {
                for (int i = 0; i < GetTabsNum(); i++)
                {
                    if (GetTabWnd(i) == it->first)
                    {
                        SetActiveTab(i);
                        break;
                    }
                }
                return FALSE;
            }
        }
    }


    Timetable newTable = Table;

    CTimeTableCtrl* newTimeTableCtrl = new CTimeTableCtrl;

    newTimeTableCtrl->SetReadOnly(bReadOnly);

	CRect rect;
	GetClientRect(&rect);
    CRect tabrect;
    GetTabsRect(tabrect);

    rect.top = tabrect.bottom;

    newTimeTableCtrl->Create(NULL
        , _T("TimeTable View")
        , WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL
        , rect
        , this
        , ID_TIMETABLE_WND);

    newTimeTableCtrl->BeginPreparingTimeTable(newTable.GetTimetableID());

    COleDateTime BaseTime(1983,3,17,0,0,0);
    for (auto it = newTable.LayoutEntryList.begin(); it != newTable.LayoutEntryList.end(); it++)
    {

        Appointment* pItem = new Appointment;
        pItem->SetInterval(BaseTime, BaseTime + Convert2Span(it->GetDuration()));
        pItem->SetDetailID(it->GetLayoutID());

        Layout layout;
        HRESULT hr = GetControllerInterface->GetLayout(it->GetLayoutID(), layout);
        if (!SUCCEEDED(hr))
        {
            TRACE1("Failed to get layout %d", it->GetLayoutID());
            delete pItem;
            continue;
        }
        pItem->SetDescription(layout.GetLayoutName().c_str());

        // UpdateIcon
        UINT Index = 0;
        if (!m_IconDetailIDMapper.Lookup(layout.GetLayoutID(), Index))
        {
            Layout::thumbnail_t thumbnail;

            GetControllerInterface->GetLayoutThumbnail(layout.GetLayoutID(), thumbnail);

            if (thumbnail.size() > 0)
            {
                HGLOBAL hGlobal  =  GlobalAlloc(GMEM_MOVEABLE, thumbnail.size() * sizeof(thumbnail[0]));
                void *  pData  =  GlobalLock(hGlobal);
                memcpy(pData, &thumbnail[0], thumbnail.size() * sizeof(thumbnail[0]));
                GlobalUnlock(hGlobal);

                IStream *  pStream  =  NULL;
                if  (CreateStreamOnHGlobal(hGlobal, TRUE,  & pStream)  ==  S_OK)
                {
                    CImage image;
                    if (SUCCEEDED(image.Load(pStream)))
                    {
                        CImage ThumbImage;
                        ThumbImage.Create(THUMBNAIL_W, THUMBNAIL_H, 32);

                        HDC hDstDC = NULL;
                        BOOL bResult;

                        // Obtain a DC to the destination image
                        hDstDC = ThumbImage.GetDC();

                        CDC  cDC;
                        cDC.Attach(hDstDC);
                        cDC.FloodFill(0, 0, RGB(123,456,789));
                        cDC.Detach();

                        // Perform the blit
                        bResult = image.BitBlt( hDstDC, max(0, (THUMBNAIL_W - image.GetWidth()))/2,
                            max(0, (THUMBNAIL_H - image.GetHeight()))/2,
                            image.GetWidth(), image.GetHeight(), 0, 0 );

                        // Release the destination DC
                        ThumbImage.ReleaseDC();

                        HBITMAP hBitmap=ThumbImage.Detach();
                        CBitmap bmp;
                        bmp.Attach(hBitmap);

                        Index = m_ItemIconList.Add(&bmp, RGB(123,456,789));
                    }
                    pStream -> Release();
                }
                GlobalFree(hGlobal);

                m_IconDetailIDMapper.SetAt(layout.GetLayoutID(), Index);
            }
        }
        pItem->SetImage(&m_ItemIconList, Index);

        newTimeTableCtrl->AddAppointment(pItem, FALSE, FALSE);

        BaseTime += Convert2Span(it->GetDuration());
    }

    newTimeTableCtrl->EndPreparingTimeTable();

    m_wndTables.insert(std::map<CWnd*, Timetable>::value_type(newTimeTableCtrl, newTable));

    CString TabName = newTable.GetTimetableName().c_str();

    if (bReadOnly)
    {
        TabName += Translate(_T("(ReadOnly)"));
    }
    InsertTab(newTimeTableCtrl, TabName, GetTabsNum());

    SetActiveTab(GetTabsNum() - 1);

    return TRUE;

}

Audit::Status CTimeTableTab::GetTheTimeTableStatus()
{
    if (GetTabsNum() <= 0)
    {
        return Audit::UNEDITABLE;
    }

    CWnd* pWndActive = GetActiveWnd();

    auto it = m_wndTables.find(pWndActive), ite = m_wndTables.end();
    if (it != ite)
    {
        return it->second.GetAudit().GetStatus();
    }

    return Audit::UNEDITABLE;

}

void CTimeTableTab::SetTimetableAudit(const Audit& audit)
{
    if (GetTabsNum() <= 0)
    {
        return;
    }

    CWnd* pWndActive = GetActiveWnd();

    auto it = m_wndTables.find(pWndActive), ite = m_wndTables.end();
    if (it != ite)
    {
        it->second.SetAudit(audit);
    }
}

BOOL CTimeTableTab::GetTheTimeTable(Timetable& Table)
{
    if (GetTabsNum() <= 0)
    {
        return FALSE;
    }

    CWnd* pWndActive = GetActiveWnd();

    auto it = m_wndTables.find(pWndActive), ite = m_wndTables.end();
    if (it != ite)
    {
        if (it->second.GetTimetableID() >= 0)
        {
            HRESULT hr = GetControllerInterface->GetTimetable(it->second.GetTimetableID(), it->second);
            if (!SUCCEEDED(hr))
            {
                TRACE1("Failed to get timetable %d\n", it->second.GetTimetableID());
                return FALSE;
            }
        }

        Table = it->second;

        CTimeTableCtrl* pCtrl = (CTimeTableCtrl*)pWndActive;

        Table.LayoutEntryList.clear();

        const AppointmentList& ItemList = pCtrl->GetItemList();


        POSITION Pos = ItemList.GetHeadPosition();

        while (Pos)
        {
            Appointment* pItem = ItemList.GetNext(Pos);

            TimetableEntry Entry((INT)Convert2Duration(pItem->GetDuration()), pItem->GetDetailID());

            Table.LayoutEntryList.push_back(Entry);
        }

        it->second = Table;

        return TRUE;

    }


    return FALSE;
}

BOOL CTimeTableTab::SetTheTimeTable(const Timetable& Table)
{
    if (GetTabsNum() <= 0)
    {
        return FALSE;
    }

    CWnd* pWndActive = GetActiveWnd();

    auto it = m_wndTables.find(pWndActive), ite = m_wndTables.end();
    if (it != ite)
    {
        it->second = Table;

        return TRUE;
    }

    return FALSE;
}

BOOL CTimeTableTab::GetTabTimeTable(INT iTab, Timetable& Table)
{
    if (GetTabsNum() <= 0)
    {
        return FALSE;
    }

    CWnd* pWndActive = GetTabWnd(iTab);

    auto it = m_wndTables.find(pWndActive), ite = m_wndTables.end();
    if (it != ite)
    {
        if (it->second.GetTimetableID() >= 0)
        {
            HRESULT hr = GetControllerInterface->GetTimetable(it->second.GetTimetableID(), it->second);
            if (!SUCCEEDED(hr))
            {
                TRACE1("Failed to get timetable %d\n", it->second.GetTimetableID());
                return FALSE;
            }
        }

        Table = it->second;
        CTimeTableCtrl* pCtrl = (CTimeTableCtrl*)pWndActive;

        Table.LayoutEntryList.clear();

        const AppointmentList& ItemList = pCtrl->GetItemList();


        POSITION Pos = ItemList.GetHeadPosition();

        while (Pos)
        {
            Appointment* pItem = ItemList.GetNext(Pos);

            TimetableEntry Entry((INT)Convert2Duration(pItem->GetDuration()), pItem->GetDetailID());

            Table.LayoutEntryList.push_back(Entry);
        }

        it->second = Table;

        return TRUE;

    }


    return FALSE;
}


BOOL CTimeTableTab::IsTimetableModified()
{
    if (GetTabsNum() <= 0)
    {
        return FALSE;
    }

    for(auto it = m_wndTables.begin(); it != m_wndTables.end(); it++)
    {
        Timetable Table = it->second;
        CTimeTableCtrl* pCtrl = (CTimeTableCtrl*)it->first;

        if (pCtrl->IsModified())
        {
            return TRUE;
        }
    }


    return FALSE;

}

BOOL CTimeTableTab::IsTimeTableOpend(INT TableID)
{
    if (GetTabsNum() <= 0)
    {
        return FALSE;
    }

    for(auto it = m_wndTables.begin(); it != m_wndTables.end(); it++)
    {
        Timetable& Table = it->second;
        CTimeTableCtrl* pCtrl = (CTimeTableCtrl*)it->first;

        if (Table.GetTimetableID() == TableID)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL CTimeTableTab::GetTimeTable(INT TableID, Timetable& Table)
{
    if (GetTabsNum() <= 0)
    {
        return FALSE;
    }

    for(auto it = m_wndTables.begin(); it != m_wndTables.end(); it++)
    {
        CTimeTableCtrl* pCtrl = (CTimeTableCtrl*)it->first;

        if (it->second.GetTimetableID() == TableID)
        {
            if (it->second.GetTimetableID() >= 0)
            {
                HRESULT hr = GetControllerInterface->GetTimetable(it->second.GetTimetableID(), it->second);
                if (!SUCCEEDED(hr))
                {
                    TRACE1("Failed to get timetable %d\n", it->second.GetTimetableID());
                    return FALSE;
                }
            }

            Table = it->second;

            CTimeTableCtrl* pCtrl = (CTimeTableCtrl*)it->first;

            Table.LayoutEntryList.clear();

            const AppointmentList& ItemList = pCtrl->GetItemList();


            POSITION Pos = ItemList.GetHeadPosition();

            while (Pos)
            {
                Appointment* pItem = ItemList.GetNext(Pos);

                TimetableEntry Entry((INT)Convert2Duration(pItem->GetDuration()), pItem->GetDetailID());

                Table.LayoutEntryList.push_back(Entry);
            }

            it->second = Table;

            return TRUE;
        }
    }


    return FALSE;
}

BOOL CTimeTableTab::IsTimetableModified(INT TableID)
{
    if (GetTabsNum() <= 0)
    {
        return FALSE;
    }

    for(auto it = m_wndTables.begin(); it != m_wndTables.end(); it++)
    {
        Timetable& Table = it->second;
        CTimeTableCtrl* pCtrl = (CTimeTableCtrl*)it->first;

        if (pCtrl->IsModified() && Table.GetTimetableID() == TableID)
        {
            return TRUE;
        }
    }

    return FALSE;

}


BOOL CTimeTableTab::CloseTimeTable(INT TableID)
{
    if (GetTabsNum() <= 0)
    {
        return FALSE;
    }

    CWnd* pWnd = NULL;

    for(auto it = m_wndTables.begin(); it != m_wndTables.end(); it++)
    {
        if (it->second.GetTimetableID() == TableID)
        {
            pWnd = it->first;
        }
    }

    if (pWnd)
    {
        for (int i = 0; i < GetTabsNum(); i++)
        {

            if (GetTabWnd(i) == pWnd)
            {
                RemoveTab(i);
                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL CTimeTableTab::GetAllModifiedTimeTable(vector<pair<CWnd*,Timetable> >& Tables)
{
    if (GetTabsNum() <= 0)
    {
        return FALSE;
    }

    for(auto it = m_wndTables.begin(); it != m_wndTables.end(); it++)
    {
        if (it->second.GetTimetableID() >= 0)
        {
            HRESULT hr = GetControllerInterface->GetTimetable(it->second.GetTimetableID(), it->second);
            if (!SUCCEEDED(hr))
            {
                TRACE1("Failed to get timetable %d\n", it->second.GetTimetableID());
                return FALSE;
            }
        }

        Timetable Table = it->second;
        CTimeTableCtrl* pCtrl = (CTimeTableCtrl*)it->first;

        if (!pCtrl->IsModified())
        {
            continue;
        }

        Table.LayoutEntryList.clear();

        const AppointmentList& ItemList = pCtrl->GetItemList();


        POSITION Pos = ItemList.GetHeadPosition();

        while (Pos)
        {
            Appointment* pItem = ItemList.GetNext(Pos);

            TimetableEntry Entry((INT)Convert2Duration(pItem->GetDuration()), pItem->GetDetailID());

            Table.LayoutEntryList.push_back(Entry);
        }

        it->second = Table;

        Tables.push_back(make_pair(pCtrl, Table));

    }


    return TRUE;

}

BOOL CTimeTableTab::SetWndTimeTable(const CWnd* pCtrl, Timetable Table)
{
    if (GetTabsNum() <= 0)
    {
        return FALSE;
    }

    for(auto it = m_wndTables.begin(); it != m_wndTables.end(); it++)
    {
        if (it->first == pCtrl)
        {
            it->second = Table;
        }
    }

    return TRUE;

}

BOOL CTimeTableTab::CloseTheTimeTable()
{
    RemoveTab(GetActiveTab());

    return TRUE;
}


void CTimeTableTab::SetTimeDelta(const int& Delta)
{
    if (GetTabsNum() <= 0)
    {
        return;
    }

    _TVIEW_TIME_DELTA TimeDelta;
    switch (Delta)
    {
    case 60*60:
        TimeDelta = _TVIEW_TIME_DELTA_60;
        break;
    case 30*60:
        TimeDelta = _TVIEW_TIME_DELTA_30;
        break;
    case 20*60:
        TimeDelta = _TVIEW_TIME_DELTA_20;
        break;
    case 15*60:
        TimeDelta = _TVIEW_TIME_DELTA_15;
        break;
    case 10*60:
        TimeDelta = _TVIEW_TIME_DELTA_10;
        break;
    case 6*60:
        TimeDelta = _TVIEW_TIME_DELTA_6;
        break;
    case 5*60:
        TimeDelta = _TVIEW_TIME_DELTA_5;
        break;
    case 4*60:
        TimeDelta = _TVIEW_TIME_DELTA_4;
        break;
    case 3*60:
        TimeDelta = _TVIEW_TIME_DELTA_3;
        break;
    case 2*60:
        TimeDelta = _TVIEW_TIME_DELTA_2;
        break;
    case 1*60:
        TimeDelta = _TVIEW_TIME_DELTA_1;
        break;
	case 30:
		 TimeDelta = _TVIEW_TIME_DELTA_30S;
		break;
	case 15:
		 TimeDelta = _TVIEW_TIME_DELTA_15S;
		break;
	case 5:
		 TimeDelta = _TVIEW_TIME_DELTA_5S;
		break;
	case 1:
		 TimeDelta = _TVIEW_TIME_DELTA_1S;
		break;
    default:
        TimeDelta = _TVIEW_TIME_DELTA_30;
        break;
    }

    CWnd* pWndActive = GetActiveWnd();

    CTimeTableCtrl* pCtrl = (CTimeTableCtrl*)pWndActive;

    pCtrl->SetTimeDelta(TimeDelta);
}


int CTimeTableTab::GetTimeDelta()
{
    if (GetTabsNum() <= 0)
    {
        return 60;
    }

    CWnd* pWndActive = GetActiveWnd();

    CTimeTableCtrl* pCtrl = (CTimeTableCtrl*)pWndActive;

    _TVIEW_TIME_DELTA Delta = pCtrl->GetTimeDelta();

    int TimeDelta = 60;
    switch (Delta)
    {
    case _TVIEW_TIME_DELTA_60:
        TimeDelta = 60*60;
        break;
    case _TVIEW_TIME_DELTA_30:
        TimeDelta = 30*60;
        break;
    case _TVIEW_TIME_DELTA_20:
        TimeDelta = 20*60;
        break;
    case _TVIEW_TIME_DELTA_15:
        TimeDelta = 15*60;
        break;
    case _TVIEW_TIME_DELTA_10:
        TimeDelta = 10*60;
        break;
    case _TVIEW_TIME_DELTA_6:
        TimeDelta = 6*60;
        break;
    case _TVIEW_TIME_DELTA_5:
        TimeDelta = 5*60;
        break;
    case _TVIEW_TIME_DELTA_4:
        TimeDelta = 4*60;
        break;
    case _TVIEW_TIME_DELTA_3:
        TimeDelta = 3*60;
        break;
    case _TVIEW_TIME_DELTA_2:
        TimeDelta = 2*60;
        break;
    case _TVIEW_TIME_DELTA_1:
        TimeDelta = 1*60;
        break;
	case  _TVIEW_TIME_DELTA_30S :
		TimeDelta = 30;
		break;
	case _TVIEW_TIME_DELTA_15S :
			TimeDelta = 15;
		break;
	case _TVIEW_TIME_DELTA_5S :
			TimeDelta = 5;
		break;
	case _TVIEW_TIME_DELTA_1S :
			TimeDelta = 1;
		break;
    default:
        TimeDelta = 30;
        break;
    }

    return TimeDelta;
}

CTimeTableCtrl* CTimeTableTab::GetTheCtrl()
{
    if (GetTabsNum() <= 0)
    {
        return NULL;
    }

    CWnd* pWndActive = GetActiveWnd();

    CTimeTableCtrl* pCtrl = (CTimeTableCtrl*)pWndActive;

    return pCtrl;
}

LRESULT CTimeTableTab::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    ASSERT_VALID(this);

    LRESULT lResult;
    if(message >= 0xC000)
    {
        if (OnWndMsg(message, wParam, lParam, &lResult))
            return lResult;
        else
        {
            this->GetOwner()->SendMessage(message, wParam, lParam);
        }
    }

    return CMFCTabCtrl::WindowProc(message, wParam, lParam);
}
