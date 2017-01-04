// TimeTableLayoutFilterCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "TimeTableLayoutFilterCtrl.h"
#include "afxdialogex.h"
#include "Localization.h"


// CTimeTableLayoutFilterCtrl dialog

IMPLEMENT_DYNAMIC(CTimeTableLayoutFilterCtrl, CDialogEx)

CTimeTableLayoutFilterCtrl::CTimeTableLayoutFilterCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTimeTableLayoutFilterCtrl::IDD, pParent)
{

}

CTimeTableLayoutFilterCtrl::~CTimeTableLayoutFilterCtrl()
{
}

void CTimeTableLayoutFilterCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TIMETABLE_CATEGORY, m_GroupNames);
	DDX_Text(pDX, IDC_EDIT_TIMETABLE_NAME, m_sName);
	DDX_Control(pDX, IDC_EDIT_TIMETABLE_NAME, m_cEdit);
}


BEGIN_MESSAGE_MAP(CTimeTableLayoutFilterCtrl, CDialogEx)
	 ON_WM_SIZE()
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CTimeTableLayoutFilterCtrl)
	EASYSIZE(IDC_EDIT_TIMETABLE_NAME,IDC_STATIC_TIMETABLE_NAME,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_COMBO_TIMETABLE_CATEGORY,IDC_STATIC_TIMETABLE_CATEGORY,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_TIMETABLE_SEARCH,IDC_COMBO_TIMETABLE_CATEGORY,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
END_EASYSIZE_MAP

// CLayoutFilterCtrl message handlers
BOOL CTimeTableLayoutFilterCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	INIT_EASYSIZE;

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CTimeTableLayoutFilterCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}
