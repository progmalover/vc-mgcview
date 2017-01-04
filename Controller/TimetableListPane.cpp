#include "StdAfx.h"
#include "resource.h"
#include "TimeTableListPane.h"


IMPLEMENT_DYNAMIC(CTimeTableListPane, CBaseControllerPane)

CTimeTableListPane::CTimeTableListPane(void)
{
}


CTimeTableListPane::~CTimeTableListPane(void)
{
}

BEGIN_MESSAGE_MAP(CTimeTableListPane, CBaseControllerPane)
	ON_WM_CREATE()
	ON_WM_DESTROY()
    ON_WM_SIZE()
END_MESSAGE_MAP()


int CTimeTableListPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
		return -1;


    RECT rect;
    GetClientRect(&rect);


    m_TimeTableListCtrl.Create(CTimeTableCategoryCtrl::IDD, this);
    m_TimeTableListCtrl.ShowWindow(SW_SHOW);

    SetMainWnd(&m_TimeTableListCtrl);
    SetMinSize(CSize(250,90));

	return 0;
}

void CTimeTableListPane::OnDestroy()
{
	CBaseControllerPane::OnDestroy();
}


void CTimeTableListPane::OnSize(UINT nType, int cx, int cy)
{
    CBaseControllerPane::OnSize(nType, cx, cy);

}


