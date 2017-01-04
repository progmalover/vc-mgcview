// ScheduleListTab.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "ScheduleListTab.h"
#include "ScheduleCtrl.h"


// CScheduleListTab

IMPLEMENT_DYNAMIC(CScheduleListTab, CMFCTabCtrl)

CScheduleListTab::CScheduleListTab()
{
#if defined(STARTER_EDITION)
    HideSingleTab(TRUE);
#endif
}

CScheduleListTab::~CScheduleListTab()
{
	this->m_bAutoDestroyWindow = TRUE;
}


BEGIN_MESSAGE_MAP(CScheduleListTab, CMFCTabCtrl)
	ON_WM_CREATE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CScheduleListTab message handlers
ScheduleCtrl* CScheduleListTab::GetScheduleCtrl()
{
	if (GetTabsNum() <= 0)
	{
		return NULL;
	}      

	CWnd* pWndActive = GetActiveWnd();

	ScheduleCtrl* pCtrl = (ScheduleCtrl*)pWndActive;

	return pCtrl;
}

ScheduleCtrl* CScheduleListTab::FindWnd(int groupID)
{
	for (auto it = m_scheduleCtrlMap.begin(); it != m_scheduleCtrlMap.end(); it++)
	{
		if (it->second == groupID)
		{
			return it->first;
		}
	}

	return NULL;
}

BOOL CScheduleListTab::OpenScheduleTag(int groupID)
{
	ScheduleCtrl* pScheduleCtrl = FindWnd(groupID);

	if (pScheduleCtrl)
	{
		for (int i = 0; i < GetTabsNum(); i++)
		{
			if (GetTabWnd(i) == pScheduleCtrl)
			{
				SetActiveTab(i);
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

int CScheduleListTab::GetGroupID()
{
	if (GetTabsNum() <= 0)
	{
		return -1;
	}         

	ScheduleCtrl* pWndActive = dynamic_cast<ScheduleCtrl*>(GetActiveWnd());

	auto it = m_scheduleCtrlMap.find(pWndActive);
	if (it != m_scheduleCtrlMap.end())
	{
		return it->second;
	}

	return -1;
}

void CScheduleListTab::RemoveScheduleTag()
{
	RemoveTab(GetActiveTab());
}

BOOL CScheduleListTab::RemoveTab(int iTab, BOOL bRecalcLayout)
{
	this->m_bAutoDestroyWindow = FALSE;
	CMFCTabCtrl::RemoveTab(iTab, bRecalcLayout);

	return TRUE;
}

LRESULT CScheduleListTab::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//return __super::WindowProc(message,wParam,lParam);
	ASSERT_VALID(this);

	LRESULT lResult;
	if(message >= 0xC000)
	{
	if (OnWndMsg(message, wParam, lParam, &lResult))
	return lResult;
	else //Translate Message to Parent window,that all "modified notice" of scheduleCtrl.
	{
	this->GetOwner()->SendMessage(message, wParam, lParam);
	}
	}

	return CMFCTabCtrl::WindowProc(message, wParam, lParam);
}

int CScheduleListTab::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CScheduleListTab::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bTabCloseButtonPressed)
	{
		if (m_rectCloseButton.PtInRect(point))
		{
			auto itor = m_scheduleCtrlMap.find(dynamic_cast<ScheduleCtrl*>(GetActiveWnd()));
			if (itor != m_scheduleCtrlMap.end())
			{
				m_scheduleCtrlMap.erase(itor);
			}

			CMFCTabCtrl::OnLButtonUp(nFlags, point);
			RemoveTab(GetActiveTab());
		}
	}
	CMFCTabCtrl::OnLButtonUp(nFlags, point);
}

void CScheduleListTab::AddNewScheduleCtrl(ScheduleCtrl* pSC, int groupID)
{
	m_scheduleCtrlMap[pSC] = groupID;
}

void CScheduleListTab::FireChangeActiveTab(int nNewTab)
{
	CMFCTabCtrl::FireChangeActiveTab(nNewTab);

	EnableActiveTabCloseButton(TRUE);
}

void CScheduleListTab::SetTimeDelta(int delta)
{
    ScheduleCtrl* pScheduleCtrl = GetScheduleCtrl();
    if (NULL == pScheduleCtrl)
        return;

    _SVIEW_TIME_DELTA TimeDelta;
    switch (delta)
    {
    case 60*60:
        TimeDelta = _SVIEW_TIME_DELTA_60;
        break;
    case 30*60:
        TimeDelta = _SVIEW_TIME_DELTA_30;
        break;
    case 20*60:
        TimeDelta = _SVIEW_TIME_DELTA_20;
        break;
    case 15*60:
        TimeDelta = _SVIEW_TIME_DELTA_15;
        break;
    case 10*60:
        TimeDelta = _SVIEW_TIME_DELTA_10;
        break;
    case 6*60:
        TimeDelta = _SVIEW_TIME_DELTA_6;
        break;
    case 5*60:
        TimeDelta = _SVIEW_TIME_DELTA_5;
        break;
    case 4*60:
        TimeDelta = _SVIEW_TIME_DELTA_4;
        break;
    case 3*60:
        TimeDelta = _SVIEW_TIME_DELTA_3;
        break;
    case 2*60:
        TimeDelta = _SVIEW_TIME_DELTA_2;
        break;
    case 1*60:
        TimeDelta = _SVIEW_TIME_DELTA_1;
        break;
	case 30:
		TimeDelta = _SVIEW_TIME_DELTA_30S;
		break;
	case 15:
		TimeDelta = _SVIEW_TIME_DELTA_15S;
		break;
	case 5:
		TimeDelta = _SVIEW_TIME_DELTA_5S;
		break;
	case 1:
		TimeDelta = _SVIEW_TIME_DELTA_1S;
		break;
    default:
        TimeDelta = _SVIEW_TIME_DELTA_30;
        break;
    }

    pScheduleCtrl->SetTimeDelta(TimeDelta);
}

int CScheduleListTab::GetTimeDelta()
{
    if (GetTabsNum() <= 0)
    {
        return 1;
    }   

    ScheduleCtrl* pCtrl = GetScheduleCtrl();
    if (!pCtrl)
    {
        return 1;
    }

    _SVIEW_TIME_DELTA Delta = pCtrl->GetTimeDelta();

    int TimeDelta = 60;
    switch (Delta)
    {
    case _SVIEW_TIME_DELTA_60:
        TimeDelta = 60 *60;
        break;
    case _SVIEW_TIME_DELTA_30:
        TimeDelta = 30*60;
        break;
    case _SVIEW_TIME_DELTA_20:
        TimeDelta = 20*60;
        break;
    case _SVIEW_TIME_DELTA_15:
        TimeDelta = 15*60;
        break;
    case _SVIEW_TIME_DELTA_10:
        TimeDelta = 10*60;
        break;
    case _SVIEW_TIME_DELTA_6:
        TimeDelta = 6*60;
        break;
    case _SVIEW_TIME_DELTA_5:
        TimeDelta = 5*60;
        break;
    case _SVIEW_TIME_DELTA_4:
        TimeDelta = 4*60;
        break;
    case _SVIEW_TIME_DELTA_3:
        TimeDelta = 3*60;
        break;
    case _SVIEW_TIME_DELTA_2:
        TimeDelta = 2*60;
        break;
    case _SVIEW_TIME_DELTA_1:
        TimeDelta = 1*60;
        break;
	case _SVIEW_TIME_DELTA_30S:
		TimeDelta = 30;
		break;
	case _SVIEW_TIME_DELTA_15S:
		TimeDelta = 15;
		break;
	case _SVIEW_TIME_DELTA_5S:
		TimeDelta = 5;
		break;
	case _SVIEW_TIME_DELTA_1S:
		TimeDelta = 1;
		break;
    default:
        TimeDelta = 30;
        break;
    }

    return TimeDelta;
}