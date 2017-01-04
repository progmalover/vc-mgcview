#include "StdAfx.h"
#include "TimeTablePropertyEditPane.h"

IMPLEMENT_DYNAMIC(CTimeTablePropertyEditPane, CBaseControllerPane)

CTimeTablePropertyEditPane::CTimeTablePropertyEditPane(void)
{
}


CTimeTablePropertyEditPane::~CTimeTablePropertyEditPane(void)
{
}

BEGIN_MESSAGE_MAP(CTimeTablePropertyEditPane, CBaseControllerPane)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

int CTimeTablePropertyEditPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_TimeTablePropertyEditCtrl.Create(CTimeTablePropertyEditCtrl::IDD, this);
	m_TimeTablePropertyEditCtrl.ShowWindow(SW_SHOW);

	SetMainWnd(&m_TimeTablePropertyEditCtrl);
	return 0;
}

void CTimeTablePropertyEditPane::OnDestroy()
{
	CBaseControllerPane::OnDestroy();
}
