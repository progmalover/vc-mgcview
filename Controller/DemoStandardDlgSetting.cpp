// MyContentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DemoStandardDlgSetting.h"
#include "Controller.h"
#include "Localization/Localization.h"

// CDemoStandardDlgSetting dialog

IMPLEMENT_DYNAMIC(CDemoStandardDlgSetting, CStandardDialog)
CDemoStandardDlgSetting::CDemoStandardDlgSetting(CWnd* pParent/* = NULL*/)
	: CStandardDialog(CDemoStandardDlgSetting::IDD, RGB(0, 255, 0), pParent)
    , m_strIP(_T(""))
    , m_Port(_T(""))
    , m_bCancel (TRUE)
{
}

CDemoStandardDlgSetting::~CDemoStandardDlgSetting()
{
}

void CDemoStandardDlgSetting::DoDataExchange(CDataExchange* pDX)
{
    CStandardDialog::DoDataExchange(pDX);

    DDX_Text(pDX, IDC_EDIT_PASSWORD, m_Port);
    DDX_Control(pDX, IDC_IPADDRESS1, m_ctrlIP);
}


BEGIN_MESSAGE_MAP(CDemoStandardDlgSetting, CStandardDialog)
    //ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


// CDemoStandardDlgSetting message handlers

void CDemoStandardDlgSetting::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	ShowXMessageBox(NULL, Translate(_T("Hello")), Translate(_T("Hello World")), MB_OK);
}

void CDemoStandardDlgSetting::OnOK()
{
    m_bCancel = FALSE;
    GetDlgItemText(IDC_IPADDRESS1, m_strIP);

    UpdateData(TRUE);

    GetConfig->Set(CONTROLLER_SERVER_IP, m_strIP);
    GetConfig->Set(CONTROLLER_SERVER_PORT, m_Port);

    GetConfig->Save();

	CStandardDialog::OnOK();
}

BOOL CDemoStandardDlgSetting::OnInitDialog()
{
	CStandardDialog::OnInitDialog();
    m_dpi.Attach(AfxFindResourceHandle(MAKEINTRESOURCE(IDD), RT_DIALOG), m_hWnd, IDD);

    GetConfig->Get(CONTROLLER_SERVER_IP, m_strIP);
    SetDlgItemText(IDC_IPADDRESS1, m_strIP);

    GetConfig->Get(CONTROLLER_SERVER_PORT, m_Port);

    UpdateData(FALSE);

	CString strLanguage = GetLocLanName();

	//auto set font for japan.
	if(strLanguage == _T("japan"))
	{
		CFont *pFont = this->GetFont();
		LOGFONT lf;
		pFont->GetLogFont(&lf);
		 
		CWnd *pWndCap = GetDlgItem(IDC_STATIC_HOST);
		CRect wndCapRect;
		pWndCap->GetWindowRect(&wndCapRect);
		CString orgString,transString ;
		pWndCap->GetWindowText(orgString);
		transString = Translate(orgString);
		 
		CDC *pDC = pWndCap->GetDC();
		pDC->SaveDC();

		CFont newFont;
		_tcscpy(lf.lfFaceName,_T("Courier"));
		while(1)
		{
			newFont.CreateFontIndirect(&lf); 
			pDC->SelectObject(&newFont);
			int nStrWidth = pDC->GetTextExtent( transString).cx;
			if( nStrWidth <= (wndCapRect.Width() - 2))
				 break;
			if(lf.lfHeight > 0 || lf.lfWidth > 0)
			{
				if(lf.lfHeight)
				lf.lfHeight --;
				if(lf.lfWidth)
				lf.lfWidth--;
			}
			else
			if(lf.lfWeight > 0)
					lf.lfWeight--;
			else
				break;
			//lf.lfWidth  --;
			//lf.lfWeight -= 30;
			newFont.DeleteObject();	 
		}
		pDC->RestoreDC(-1);
		pWndCap->ReleaseDC(pDC);
		GetDlgItem(IDC_STATIC_PORT)->SetFont(&newFont);
		GetDlgItem(IDC_STATIC_HOST)->SetFont(&newFont);
		newFont.Detach();
	}
	Translate(this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
