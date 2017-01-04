#include "StdAfx.h"
#include "TimeTableManagePane.h"

IMPLEMENT_DYNAMIC(CTimeTableManagePane, CBaseControllerPane)

CTimeTableManagePane::CTimeTableManagePane(void)
{
}


CTimeTableManagePane::~CTimeTableManagePane(void)
{
}

BEGIN_MESSAGE_MAP(CTimeTableManagePane, CBaseControllerPane)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


int CTimeTableManagePane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_TimeTableManageCtrl.Create(CTimeTableManageCtrl::IDD, this);
	m_TimeTableManageCtrl.ShowWindow(SW_SHOW);

	SetMainWnd(&m_TimeTableManageCtrl);
	return 0;
}

void CTimeTableManagePane::OnDestroy()
{
	CBaseControllerPane::OnDestroy();
}
