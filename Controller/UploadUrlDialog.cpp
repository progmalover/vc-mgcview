// UploadUrlDialog.cpp
//

#include "stdafx.h"
#include "Controller.h"
#include "UploadUrlDialog.h"
#include "Localization/Localization.h"
#include "AFXINET.H"
 
// CUploadUrlDialog

IMPLEMENT_DYNAMIC(CUploadUrlDialog, CDialogEx)

CUploadUrlDialog::CUploadUrlDialog(CWnd* pParent /*=NULL*/, CString WindowName)
    : CDialogEx(CUploadUrlDialog::IDD, pParent)
{
    m_szLayoutName = _T("");
    m_szWindowName = WindowName;
}

CUploadUrlDialog::~CUploadUrlDialog()
{
}

void CUploadUrlDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_EDIT_NAME, m_szLayoutName);
	DDX_Control(pDX, IDC_URL_HEADER, m_URLHeader);
}


BEGIN_MESSAGE_MAP(CUploadUrlDialog, CDialogEx)

    ON_BN_CLICKED(IDOK, &CUploadUrlDialog::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_URL_HEADER, &CUploadUrlDialog::OnCbnSelchangeUrlHeader)
	ON_EN_CHANGE(IDC_EDIT_NAME, &CUploadUrlDialog::OnEnChangeEditName)
END_MESSAGE_MAP()


BOOL CUploadUrlDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

	Translate(this);

    SetWindowTextW(m_szWindowName);

	GetDlgItem(IDC_STATIC)->SetWindowTextW(Translate(_T("Input URL")));

	m_URLHeader.AddString(_T("http://"));
    m_URLHeader.AddString(_T("https://"));
	m_URLHeader.AddString(_T("mms://"));
	m_URLHeader.AddString(_T("rtsp://"));

	m_URLHeader.SetCurSel(0);
//	GetDlgItem(IDC_EDIT_NAME)->SetWindowTextW(_T("http://"));

	//get url type, modified by RaymondLiu
	m_URLHeader.GetLBText( m_URLHeader.GetCurSel(),m_szUrlType);
    return TRUE;
}


// CUploadUrlDialog Message Handle


void CUploadUrlDialog::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    GetDlgItem(IDC_EDIT_NAME)->GetWindowText(m_szLayoutName);
    if(0x00 == m_szLayoutName.Find(_T("http://")) || 0x00 == m_szLayoutName.Find(_T("mms://")) ||
        0x00 == m_szLayoutName.Find(_T("rtsp://"))|| 0x00 ==  m_szLayoutName.Find(_T("https://")))
    {
        MessageBox(Translate(_T("Remove 'http://' or 'mms://' or 'rtsp://' prefix!")),Translate(_T("Warning:Upload url")), MB_OK|MB_ICONEXCLAMATION);
        return;
    }
    CString strHeader;
	m_szLayoutName.TrimLeft();

    m_URLHeader.GetLBText(m_URLHeader.GetCurSel(), strHeader);
	CString strURL(strHeader + m_szLayoutName);
    m_szLayoutName = strURL;
    
    

	strURL.MakeLower();

	DWORD			dwServiceType;
	CString			strServer;
	CString			strObject;
	INTERNET_PORT	nPort;
	CString			strUsername;
	CString			strPassword;

	BOOL bIsURL = AfxParseURLEx(strURL,dwServiceType,
		strServer,strObject,nPort,strUsername,strPassword,ICU_DECODE|ICU_ENCODE_PERCENT);

	if( !bIsURL || strServer.GetLength()<= 0)
	{
		MessageBox(Translate(_T("Please correct URL!")), Translate(_T("Warning:Upload url")),MB_OK|MB_ICONEXCLAMATION);
		return;
	}
		
	if(dwServiceType == 0x03) //http
	{
		if(nPort == 0)
		{
			MessageBox(Translate(_T("Please correct URL!")),Translate(_T("Warning:Upload url")), MB_OK| MB_ICONEXCLAMATION);
			return;
		}
		
	} 
	/*server部分最少由一个节组成*/
	CString strSec;
	if( !::AfxExtractSubString(strSec,strServer,1,'.')) //maybe is localhost
	{
		if(strServer[0] == '-')
		{
			MessageBox(Translate(_T("Please correct URL!")),Translate(_T("Warning:Upload url")), MB_OK|MB_ICONEXCLAMATION);
			return;
		}
	}

    CDialogEx::OnOK();
}


void CUploadUrlDialog::OnCbnSelchangeUrlHeader()
{
	// TODO: Add your control notification handler code here
	CString strItem ;
	m_URLHeader.GetLBText( m_URLHeader.GetCurSel(),strItem);
	m_szUrlType = strItem;

	CString strText ,strSec;
	GetDlgItem(IDC_EDIT_NAME)->GetWindowText(strText);
	if(AfxExtractSubString(strSec,strText,1,':' ) && AfxExtractSubString(strSec,strText,0,':' ))
	{
		strText.Replace(strSec,strItem);
	}else
	{
		if(strText.GetLength())
			strText.Insert(0,strItem + _T("://"));
	}
	GetDlgItem(IDC_EDIT_NAME)->SetWindowText(strText);

}


void CUploadUrlDialog::OnEnChangeEditName()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

//  modified by RaymondLiu
// 	CString strText,strHeader;
// 	GetDlgItem(IDC_EDIT_NAME)->GetWindowText(strText);
// 
// 	if(::AfxExtractSubString(strHeader,strText,0,':'))
// 	{
// 		if(strHeader.GetLength() > 4 || strHeader.GetLength() <= 0)
// 			return;
// 
// 		for( int i = 0;i < this->m_URLHeader.GetCount();i++)
// 		{
// 			CString strLB;
// 			m_URLHeader.GetLBText(i,strLB);
// 			if( 0 == strHeader.CollateNoCase(strLB))
// 			{
// 				m_URLHeader.SetCurSel(i);
// 				break;
// 			}
// 		}
// 	}
	
}

void CUploadUrlDialog::GetURLType(CString & szUrlType)
{
	//m_URLHeader.GetLBText( m_URLHeader.GetCurSel(),szUrlType);
	szUrlType = m_szUrlType ;
}
