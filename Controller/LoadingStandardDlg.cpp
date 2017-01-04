// MyContentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "LoadingStandardDlg.h"
#include "Localization/Localization.h"

// CLoadingStandardDlg dialog

IMPLEMENT_DYNAMIC(CLoadingStandardDlg, CStandardDialog)
CLoadingStandardDlg::CLoadingStandardDlg(CWnd* pParent/* = NULL*/)
	: CStandardDialog(CLoadingStandardDlg::IDD, RGB(0, 255, 0), pParent)
    , nCnt(0)
    , m_LoadingStr(_T(""))
{
}

CLoadingStandardDlg::~CLoadingStandardDlg()
{
}

void CLoadingStandardDlg::DoDataExchange(CDataExchange* pDX)
{
    CStandardDialog::DoDataExchange(pDX);

    DDX_Text(pDX, IDC_LOADING_STR, m_LoadingStr);
}


BEGIN_MESSAGE_MAP(CLoadingStandardDlg, CStandardDialog)
    ON_WM_TIMER()
END_MESSAGE_MAP()

void CLoadingStandardDlg::OnTimer(UINT_PTR)
{
    CString dots;

    for (int i = 0; i < nCnt; i++)
    {
        dots += _T(".");
    }

    m_LoadingStr = dots;

    UpdateData(FALSE);

    nCnt = (++nCnt) % 32;
}

// CLoadingStandardDlg message handlers


BOOL CLoadingStandardDlg::OnInitDialog()
{
	CStandardDialog::OnInitDialog();

    SetTimer(1, 200, NULL);

	Translate(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLoadingStandardDlg::OnOK()
{
}

void CLoadingStandardDlg::OnCancel()
{
}
