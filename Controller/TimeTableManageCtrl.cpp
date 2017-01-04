// TimeTableManageCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "TimeTableManageCtrl.h"
#include "afxdialogex.h"
#include "Localization.h"


// CTimeTableManageCtrl dialog

IMPLEMENT_DYNAMIC(CTimeTableManageCtrl, CDialogEx)

CTimeTableManageCtrl::CTimeTableManageCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTimeTableManageCtrl::IDD, pParent)
{

}

CTimeTableManageCtrl::~CTimeTableManageCtrl()
{
}

void CTimeTableManageCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTimeTableManageCtrl, CDialogEx)
	 ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CTimeTableManageCtrl)
	
END_EASYSIZE_MAP


// CTimeTableManageCtrl message handlers
BOOL CTimeTableManageCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	INIT_EASYSIZE;

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CTimeTableManageCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}
