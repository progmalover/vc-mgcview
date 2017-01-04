// DemoTranslucentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DemoTranslucentDlgSetting.h"
#include "DemoStandardDlgSetting.h"
#include "Localization/Localization.h"

// CDemoTranslucentDlgSetting dialog
//IMPLEMENT_DYNAMIC(CDemoTranslucentDlgSetting, CTranslucentDialog)

CDemoTranslucentDlgSetting::CDemoTranslucentDlgSetting(LPCTSTR lpszFile, CWnd* pParent /*=NULL*/)
	: CTranslucentDialog(CDemoTranslucentDlgSetting::IDD, lpszFile, pParent)
{
}

CDemoTranslucentDlgSetting::CDemoTranslucentDlgSetting(UINT nImgID, LPCTSTR lpszType/* = _T("PNG")*/, HINSTANCE hResourceModule/* = NULL*/, CWnd* pParent /*=NULL*/)
	: CTranslucentDialog(CDemoTranslucentDlgSetting::IDD, nImgID, lpszType, hResourceModule, pParent)
{
}

CDemoTranslucentDlgSetting::~CDemoTranslucentDlgSetting()
{
}

void CDemoTranslucentDlgSetting::DoDataExchange(CDataExchange* pDX)
{
	CTranslucentDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDemoTranslucentDlgSetting, CTranslucentDialog)
END_MESSAGE_MAP()


CStandardDialog* CDemoTranslucentDlgSetting::CreateStandardDialog()
{
	return ::new CDemoStandardDlgSetting(this);
}

CDemoStandardDlgSetting* CDemoTranslucentDlgSetting::GetStandardDialog()
{
    return static_cast<CDemoStandardDlgSetting*>(m_pStandardDlg);
}
void CDemoTranslucentDlgSetting::OnInitChildrenWnds()
{
}

void CDemoTranslucentDlgSetting::GetTitle(CString& s)
{
    s = Translate(_T("Setting"));
}
