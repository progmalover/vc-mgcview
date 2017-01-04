#include "stdafx.h"
#include "Controller.h"
#include "ClientManagerAddGroupDlg.h"
#include "afxdialogex.h"
#include "Localization/Localization.h"

IMPLEMENT_DYNAMIC(ClientManagerAddGroupDlg, CDialogEx)

ClientManagerAddGroupDlg::ClientManagerAddGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ClientManagerAddGroupDlg::IDD, pParent)
{

}

ClientManagerAddGroupDlg::~ClientManagerAddGroupDlg()
{
}

void ClientManagerAddGroupDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

    DDX_Text(pDX, IDC_GROUP_NAME, m_GroupName);
    DDX_Text(pDX, IDC_GROUP_DESC, m_GroupDesc);
}


BEGIN_MESSAGE_MAP(ClientManagerAddGroupDlg, CDialogEx)
    ON_WM_SIZE()
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(ClientManagerAddGroupDlg)
	/*EASYSIZE(IDC_STATIC_INFO_GROUP,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_INFO,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_START_TIME,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_END_TIME,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_PLAY_DURATION,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_DATETIMEPICKER_START,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_DATETIMEPICKER_END,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_DATETIMEPICKER_DURATION,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)*/
END_EASYSIZE_MAP


// CScheduleItemCtrl message handlers
BOOL ClientManagerAddGroupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;
	
	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void ClientManagerAddGroupDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}
