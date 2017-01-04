// RenameCategory.cpp : 实现文件
//

#include "stdafx.h"
#include "Controller.h"
#include "RenameCategory.h"
#include "afxdialogex.h"
#include "MediaLibraryPage.h"


// CRenameCategory 对话框

IMPLEMENT_DYNAMIC(CRenameCategory, CDialogEx)

CRenameCategory::CRenameCategory(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRenameCategory::IDD, pParent)
    , m_OrigName(_T(""))
    , m_NewMediaName(_T(""))
    , m_NewUrlName(_T(""))
    , m_header(_T(""))
    , m_isUrl(FALSE)
{

}

CRenameCategory::CRenameCategory(BOOL isUrl, CWnd* pParent)
    :CDialogEx(CRenameCategory::IDD, pParent)
    , m_OrigName(_T(""))
    , m_NewMediaName(_T(""))
    , m_NewUrlName(_T(""))
    , m_header(_T(""))
    , m_isUrl(isUrl)
{

}

CRenameCategory::~CRenameCategory()
{
}

void CRenameCategory::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_ORIGINAL_MEDIANAME, m_OrigName);
    DDX_Text(pDX, IDC_NEW_MEDIANAME, m_NewMediaName);
    DDX_Text(pDX, IDC_NEW_URLNAME, m_NewUrlName);
    DDX_Control(pDX, IDC_COMBO1, m_urlType);
}


BEGIN_MESSAGE_MAP(CRenameCategory, CDialogEx)
END_MESSAGE_MAP()


// CRenameCategory 消息处理程序

BOOL CRenameCategory::InitMedia(CString mediaName, CString urldesc)
{
    m_OrigName = mediaName; 
    return TRUE;
}

void CRenameCategory::OnCancel()
{
    CDialogEx::OnCancel();
}

void CRenameCategory::OnOK()
{
    CMediaLibraryPage *pWnd = (CMediaLibraryPage*)
        SearchWndObj(AfxGetMainWnd(),RUNTIME_CLASS(CMediaLibraryPage));
 
    UpdateData();
    CString::PCXSTR interpunction;
    CString newName;
    if (m_isUrl)
    {
        interpunction = _T("*\"|<>\\");
    
        if(0x00 == m_NewUrlName.Find(_T("http://")) || 0x00 == m_NewUrlName.Find(_T("mms://")) ||
            0x00 == m_NewUrlName.Find(_T("rtsp://"))|| 0x00 ==  m_NewUrlName.Find(_T("https://")))
        {
            MessageBox(Translate(_T("Remove 'http://' or 'mms://' or 'rtsp://' prefix!")),Translate(_T("Warning:Rename url")), MB_OK|MB_ICONEXCLAMATION);
            return;
        }

        m_urlType.GetLBText(m_urlType.GetCurSel(), m_header);
        newName = m_header + m_NewUrlName;
    }
    else
    {
        interpunction = _T("*:\"/|<>?\\");
        newName = m_NewMediaName;
    }
    int i;
    bool ret = false;
    
    newName.Trim();
    if (newName.IsEmpty())
    {
        MessageBox(Translate(_T("New Name should not Null!")),Translate(_T("Warning:Rename media")),MB_OK|MB_ICONEXCLAMATION);
        return;
    }
    while ((i = newName.FindOneOf(interpunction)) != -1)
    {
       MessageBox(Translate(_T("Media name contains invalid character(s).")),Translate(_T("Warning:Rename media")),MB_OK|MB_ICONEXCLAMATION);
        return;
    }

    if (newName == m_OrigName)
    {
        MessageBox(Translate(_T("New name is same with original name!")),Translate(_T("Warning:Rename media")),MB_OK|MB_ICONEXCLAMATION);
        return;
    }


    CDialogEx::OnOK();
}

BOOL CRenameCategory::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    Translate(this);
	// TODO:  Add extra initialization here
    if (m_isUrl)
    {
        GetDlgItem(IDC_NEW_MEDIANAME)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_NEW_URLNAME)->ShowWindow(TRUE);
        GetDlgItem(IDC_COMBO1)->ShowWindow(TRUE);
        m_urlType.AddString(_T("http://"));
        m_urlType.AddString(_T("https://"));
        m_urlType.AddString(_T("mms://"));
        m_urlType.AddString(_T("rtsp://"));
        int index = 0;
        if(0x00 == m_OrigName.Find(_T("https://")))
            index = 1;
        else if(0x00 == m_NewUrlName.Find(_T("mms://"))) 
            index = 2;
        else if(0x00 == m_NewUrlName.Find(_T("rtsp://")))
            index = 3;

        m_urlType.SetCurSel(index);
    }  

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
