// ScheduleItemCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "SchedulePackageExportDlg.h"
#include "SchedulerPage.h"
#include "afxdialogex.h"
#include "ScheduleCtrl.h"


// CScheduleItemCtrl dialog

IMPLEMENT_DYNAMIC(CSchedulePackageExportDlg, CDialogEx)

CSchedulePackageExportDlg::CSchedulePackageExportDlg(CWnd* pParent)
	: CDialogEx(CSchedulePackageExportDlg::IDD, pParent)
{ 
    
}

CSchedulePackageExportDlg::~CSchedulePackageExportDlg()
{
}

void CSchedulePackageExportDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_SCHEDULEPACKAGE_EXPORT, m_SchedulePackagePath);
    DDX_Text(pDX, IDC_EDIT1, m_SchedulePackageName);
}


BEGIN_MESSAGE_MAP(CSchedulePackageExportDlg, CDialogEx)
    ON_WM_SIZE()
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CSchedulePackageExportDlg)
    //EASYSIZE(IDC_LIST_SCHEDULES,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP


BOOL CSchedulePackageExportDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CSchedulePackageExportDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}
