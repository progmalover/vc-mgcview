// ScheduleItemCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "SchedulePackageImportDlg.h"
#include "SchedulerPage.h"
#include "afxdialogex.h"
#include "ScheduleCtrl.h"


// CScheduleItemCtrl dialog

IMPLEMENT_DYNAMIC(CSchedulePackageImportDlg, CDialogEx)

CSchedulePackageImportDlg::CSchedulePackageImportDlg(CWnd* pParent)
	: CDialogEx(CSchedulePackageImportDlg::IDD, pParent)
{ 
    
}

CSchedulePackageImportDlg::~CSchedulePackageImportDlg()
{
}

void CSchedulePackageImportDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_SCHEDULEPACKAGE_BROWSE, m_SchedulePackagePathName);
}


BEGIN_MESSAGE_MAP(CSchedulePackageImportDlg, CDialogEx)
    ON_WM_SIZE()
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CSchedulePackageImportDlg)
    //EASYSIZE(IDC_LIST_SCHEDULES,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP


BOOL CSchedulePackageImportDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CSchedulePackageImportDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}
