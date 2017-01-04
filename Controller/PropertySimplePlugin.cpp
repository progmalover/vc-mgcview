// PropertySimplePlugin.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "PropertySimplePlugin.h"
#include "afxdialogex.h"
#include "PropertyCtrl.h"
#include "Localization/Localization.h"
#include "RenderableObject/S3Clock/ConfigClockDlg.h"

extern UINT WM_PROPERTY_SETTING_CHANGE	;
UINT WM_ON_SIMPLE_PLUGIN_THUMB_CHANGE = ::RegisterWindowMessage(_T("PROPERTY_SIMPLE_PLUGIN_THUMB_CHANGE"));
UINT WM_ON_SIMPLE_PLUGIN_SIZE_CHANGE = ::RegisterWindowMessage(_T("WM_ON_SIMPLE_PLUGIN_SIZE_CHANGE"));

// CPropertySimplePlugin dialog

IMPLEMENT_DYNAMIC(CPropertySimplePlugin, CDialogEx)

CPropertySimplePlugin::CPropertySimplePlugin(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPropertySimplePlugin::IDD, pParent)
    , m_bSelJudge(TRUE)
{

}

CPropertySimplePlugin::~CPropertySimplePlugin()
{
}

void CPropertySimplePlugin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropertySimplePlugin, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CPropertySimplePlugin::OnBnClickedButtonConfigure)
	ON_MESSAGE(WM_TRANSLATE_BEFORE, &CPropertySimplePlugin::OnTranslateBefore)
    ON_MESSAGE(WM_CLOCK_CANCEL, &CPropertySimplePlugin::OnSelJudge)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CPropertySimplePlugin)
	EASYSIZE(IDC_BUTTON_CONFIGURE,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_HCENTER)
END_EASYSIZE_MAP


// CPropertySimplePlugin message handlers
BOOL CPropertySimplePlugin::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;
	

	Translate(this);
	return FALSE;  // return TRUE  unless you set the focus to a control
}


void CPropertySimplePlugin::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

VOID CPropertySimplePlugin::SetContent(std::shared_ptr<IS3RODesigner>& pDesigner,CS3DrawObj *pDrawObj)
{
    m_pDesigner = pDesigner;
	m_pDrawObj = pDrawObj;
}

void CPropertySimplePlugin::OnBnClickedButtonConfigure()
{
    BOOL bNewThumbnail;
	PRO_CONTENTINFO pContentInfo = new RO_CONTENTINFO;
    LANGID langId = 0;
    if (GetLocLanName() == _T("english"))
        langId = 0;
    else if (GetLocLanName() == _T("chinese"))
        langId = 1;
    else if (GetLocLanName() == _T("japan"))
        langId = 2;
    m_pDesigner->ConfigProperty(this->m_hWnd, langId, &bNewThumbnail);
    if(bNewThumbnail)
    {
         GetParent()->PostMessage(WM_ON_SIMPLE_PLUGIN_THUMB_CHANGE, 0, 0);
    }
    
	m_pDesigner->GetContentInfo(pContentInfo);
	CString name;
	m_pDrawObj->GetName(name);
    if(m_bSelJudge)
    {
        if (0 == name.CompareNoCase(_T("S3Clock")))
        {
            GetParent()->PostMessage(WM_ON_SIMPLE_PLUGIN_SIZE_CHANGE,(WPARAM)pContentInfo,(LPARAM)m_pDrawObj);
        }
    } 
}

LRESULT CPropertySimplePlugin::OnTranslateBefore(WPARAM wp, LPARAM lp)
{
    CWnd *pWnd = CWnd::FromHandle((HWND)lp);
    Translate(pWnd);
    return S_OK;
}

LRESULT CPropertySimplePlugin::OnSelJudge(WPARAM wp, LPARAM lp)
{
    m_bSelJudge = (BOOL)wp;
    return S_OK;
}

BOOL CPropertySimplePlugin::PreTranslateMessage(MSG* pMsg)
{
    m_focus.InitFocusEx(GetFocusableWindow, this);

    if (m_focus.ProcessKeyPressMessage(this, pMsg))
    {
        return TRUE;
    }
    if (pMsg->message == WM_MOUSEWHEEL)
    {
        GetParent()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
        return TRUE;
    }
    if (pMsg->message == WM_LBUTTONDOWN)
    {
        GetParent()->SetFocus();
    }

    return __super::PreTranslateMessage(pMsg);
}

HWND CPropertySimplePlugin::GetFocusableWindow(int nPoisition, LPVOID lParam, BOOL shift)
{
    CPropertySimplePlugin* pThis = (CPropertySimplePlugin*)lParam;
    switch (nPoisition)
    {
    case FOCUSABLEWINDOW_POSITION_FIRST:
        return CFocusEx::GetFirstFocusableWindow(pThis->GetSafeHwnd());
    case FOCUSABLEWINDOW_POSITION_LAST:
        return CFocusEx::GetLastFocusableWindow(pThis->GetSafeHwnd());
    case FOCUSABLEWINDOW_POSITION_FOCUSABLE:
        {
            CPropertyCtrl* pParent = (CPropertyCtrl*)(pThis->GetParent());
            return pParent->GetNextFocusWindow(pThis->GetSafeHwnd(), shift);
        }
    }
    return NULL;
}
