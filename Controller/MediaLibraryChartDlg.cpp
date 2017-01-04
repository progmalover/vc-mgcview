// UserMonitorChartDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Controller.h"
#include "MediaLibraryChartDlg.h"
#include "afxdialogex.h"


// CUserMonitorChartDlg 对话框

IMPLEMENT_DYNAMIC(CMediaLibraryChartDlg, CDialogEx)

CMediaLibraryChartDlg::CMediaLibraryChartDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMediaLibraryChartDlg::IDD, pParent)
{

}

CMediaLibraryChartDlg::~CMediaLibraryChartDlg()
{
}

void CMediaLibraryChartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMediaLibraryChartDlg, CDialogEx)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CUserMonitorChartDlg 

void CMediaLibraryChartDlg::OnCancel()
{
}


void CMediaLibraryChartDlg::OnOK()
{
}


void CMediaLibraryChartDlg::OnClose()
{
/*
    CMediaLibraryListTab* parent = (CMediaLibraryListTab*)GetParent();
    parent->RemoveTab(parent->GetActiveTab());
*/
    CDialogEx::OnClose();
}
