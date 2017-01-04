#include "StdAfx.h"
#include "TimeTableLayoutPropertyPane.h"

IMPLEMENT_DYNAMIC(CTimeTableLayoutPropertyPane, CBaseControllerPane)

CTimeTableLayoutPropertyPane::CTimeTableLayoutPropertyPane(void)
{
}


CTimeTableLayoutPropertyPane::~CTimeTableLayoutPropertyPane(void)
{
}

BEGIN_MESSAGE_MAP(CTimeTableLayoutPropertyPane, CBaseControllerPane)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


int CTimeTableLayoutPropertyPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_TimeTableLayoutPropertyCtrl.Create(CTimeTableLayoutPropertyCtrl::IDD, this);
	m_TimeTableLayoutPropertyCtrl.ShowWindow(SW_SHOW);

	SetMainWnd(&m_TimeTableLayoutPropertyCtrl);
	return 0;
}

void CTimeTableLayoutPropertyPane::OnDestroy()
{
	CBaseControllerPane::OnDestroy();
}