// RejectReason.cpp : 实现文件
//

#include "stdafx.h"
#include "Controller.h"
#include "RejectReason.h"
#include "afxdialogex.h"


// CRejectReason 对话框

IMPLEMENT_DYNAMIC(CRejectReason, CDialogEx)

 CRejectReason::CRejectReason(CWnd* pParent /*=NULL*/, BOOL bReadOnly, CString szRejectReason)
 : CDialogEx(CRejectReason::IDD, pParent)   
, m_RejectReason (szRejectReason)
, m_bReadOnly (bReadOnly)
{

}

CRejectReason::~CRejectReason()
{
}

void CRejectReason::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_REJECT_REASON, m_RejectReason);
    DDX_Control(pDX, IDC_EDIT_REJECT_REASON, m_ctrlEditReason);
}

BOOL CRejectReason::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    GetDlgItem(IDC_EDIT_REJECT_REASON)->SetFocus();

    m_ctrlEditReason.SetReadOnly(m_bReadOnly);
    if (m_bReadOnly)
    {
       GetDlgItem(IDC_STATIC)->SetWindowText(Translate(_T("Reject reason:")));
       CRect BtnRect;
       GetDlgItem(IDCANCEL)->GetWindowRect(&BtnRect);
       ScreenToClient(&BtnRect);
       GetDlgItem(IDOK)->MoveWindow(BtnRect);
       GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
    }

    Translate(this);
    return TRUE;
}

BEGIN_MESSAGE_MAP(CRejectReason, CDialogEx)
END_MESSAGE_MAP()

void CRejectReason::OnCancel()
{
    CDialogEx::OnCancel();
}

void CRejectReason::OnOK()
{

    CDialogEx::OnOK();
}
// CRejectReason 消息处理程序
