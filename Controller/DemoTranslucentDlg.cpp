// DemoTranslucentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DemoTranslucentDlg.h"
#include "DemoStandardDlg.h"
#include "Localization/Localization.h"

// CDemoTranslucentDlg dialog
//IMPLEMENT_DYNAMIC(CDemoTranslucentDlg, CTranslucentDialog)

CDemoTranslucentDlg::CDemoTranslucentDlg(LPCTSTR lpszFile, CWnd* pParent /*=NULL*/)
	: CTranslucentDialog(CDemoTranslucentDlg::IDD, lpszFile, pParent)
{
}

CDemoTranslucentDlg::CDemoTranslucentDlg(UINT nImgID, LPCTSTR lpszType/* = _T("PNG")*/, HINSTANCE hResourceModule/* = NULL*/, CWnd* pParent /*=NULL*/)
	: CTranslucentDialog(CDemoTranslucentDlg::IDD, nImgID, lpszType, hResourceModule, pParent)
{
}

CDemoTranslucentDlg::~CDemoTranslucentDlg()
{
}

void CDemoTranslucentDlg::DoDataExchange(CDataExchange* pDX)
{
	CTranslucentDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDemoTranslucentDlg, CTranslucentDialog)
END_MESSAGE_MAP()


CStandardDialog* CDemoTranslucentDlg::CreateStandardDialog()
{
	return ::new CDemoStandardDlg(this);
}

CDemoStandardDlg* CDemoTranslucentDlg::GetStandardDialog()
{
    return static_cast<CDemoStandardDlg*>(m_pStandardDlg);
}
void CDemoTranslucentDlg::OnInitChildrenWnds()
{
}

void CDemoTranslucentDlg::GetTitle(CString& s)
{
    s = Translate(_T("Login"));
}
