// TemplateCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "TemplateCtrl.h"
#include "afxdialogex.h"
#include "Localization/Localization.h"

// CTemplateCtrl dialog

IMPLEMENT_DYNAMIC(CTemplateCtrl, CDialogEx)

CTemplateCtrl::CTemplateCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTemplateCtrl::IDD, pParent)
{

}

CTemplateCtrl::~CTemplateCtrl()
{
}

void CTemplateCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTemplateCtrl, CDialogEx)
    ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CTemplateCtrl)
    //EASYSIZE(IDC_BUTTON_SEARCH,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_HCENTER)
    //EASYSIZE(IDC_EDIT_NAME,ES_BORDER,ES_BORDER,IDC_BUTTON_SEARCH,ES_KEEPSIZE,0)
    //EASYSIZE(IDC_COMBO_GROUP,IDC_BUTTON_SEARCH,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_LIST_TEMPLATE,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

// CTemplateCtrl message handlers
BOOL CTemplateCtrl::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    INIT_EASYSIZE;

	Translate(this);
    return TRUE;  // return TRUE  unless you set the focus to a control
}


void CTemplateCtrl::OnSize(UINT nType, int cx, int cy) 
{
    CDialogEx::OnSize(nType, cx, cy);
    UPDATE_EASYSIZE;
}
