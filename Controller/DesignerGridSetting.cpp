// DesignerGridSetting.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "DesignerGridSetting.h"
#include "Localization/Localization.h"

// CDesignerGridSetting dialog

UINT GRID_XSPACESETTINGINVALID = ::RegisterWindowMessage (_T("GRID_XSPACESETTINGINVALID"));
UINT GRID_YSPACESETTINGINVALID = ::RegisterWindowMessage (_T("GRID_YSPACESETTINGINVALID"));

IMPLEMENT_DYNAMIC(CDesignerGridSetting, CMFCPropertyPage)

CDesignerGridSetting::CDesignerGridSetting()
	: CMFCPropertyPage(CDesignerGridSetting::IDD)
    , m_XSpace(100)
    , m_YSpace(100)
    , m_bShowGrid(TRUE)
    , m_bSnapToGrid(FALSE)
{

}

CDesignerGridSetting::~CDesignerGridSetting()
{
}

void CDesignerGridSetting::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_X, m_XSpace);
	DDX_Text(pDX, IDC_EDIT_Y, m_YSpace);
	//DDV_MinMaxInt(pDX, m_XSpace, 10, 1080);
	//DDV_MinMaxInt(pDX, m_YSpace, 10, 1920);

	DDX_Check(pDX, IDC_CHECK_SHOW, m_bShowGrid);
	DDX_Check(pDX, IDC_CHECK_SNAP, m_bSnapToGrid);
	DDX_Control(pDX, IDC_EDIT_X, m_XEdit);
	DDX_Control(pDX, IDC_EDIT_Y, m_YEdit);
}

BOOL CDesignerGridSetting::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	UpdateData(FALSE);

     if(m_bShowGrid)
    {
       GetDlgItem(IDC_EDIT_X)->EnableWindow(TRUE);
       GetDlgItem(IDC_EDIT_Y)->EnableWindow(TRUE);
    }
    else
    {
        GetDlgItem(IDC_EDIT_X)->EnableWindow(FALSE);
        GetDlgItem(IDC_EDIT_Y)->EnableWindow(FALSE);
    }
	Translate(this);
	return TRUE;  
}

BEGIN_MESSAGE_MAP(CDesignerGridSetting, CMFCPropertyPage)
    ON_EN_CHANGE(IDC_EDIT_X, &CDesignerGridSetting::OnEnChangeEditX)
    ON_EN_CHANGE(IDC_EDIT_Y, &CDesignerGridSetting::OnEnChangeEditY)
    ON_BN_CLICKED(IDC_CHECK_SHOW, &CDesignerGridSetting::OnBnClickedCheckShow)
    ON_BN_CLICKED(IDC_CHECK_SNAP, &CDesignerGridSetting::OnBnClickedCheckSnap)
	ON_EN_KILLFOCUS(IDC_EDIT_X, &CDesignerGridSetting::OnEnKillfocusEditX)
	ON_EN_KILLFOCUS(IDC_EDIT_Y, &CDesignerGridSetting::OnEnKillfocusEditY)
END_MESSAGE_MAP()


// CDesignerGridSetting message handlers


void CDesignerGridSetting::OnEnChangeEditX()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CMFCPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    SetModified();
}


void CDesignerGridSetting::OnEnChangeEditY()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CMFCPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here

    SetModified();
}


void CDesignerGridSetting::OnBnClickedCheckShow()
{
    // TODO: Add your control notification handler code here
    SetModified();
    UpdateData(TRUE);
    if(m_bShowGrid)
    {
       GetDlgItem(IDC_EDIT_X)->EnableWindow(TRUE);
       GetDlgItem(IDC_EDIT_Y)->EnableWindow(TRUE);
    }
    else
    {
        GetDlgItem(IDC_EDIT_X)->EnableWindow(FALSE);
        GetDlgItem(IDC_EDIT_Y)->EnableWindow(FALSE);
    }
}


void CDesignerGridSetting::OnBnClickedCheckSnap()
{
    // TODO: Add your control notification handler code here
    SetModified();
}


void CDesignerGridSetting::OnEnKillfocusEditX()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	GetOwner()->SendMessage(GRID_XSPACESETTINGINVALID, m_XSpace);
}


void CDesignerGridSetting::OnEnKillfocusEditY()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	GetOwner()->SendMessage(GRID_YSPACESETTINGINVALID, m_YSpace);
}


BOOL CDesignerGridSetting::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	
	if(GetFocus() == GetDlgItem(IDC_EDIT_X) ||GetFocus() == GetDlgItem(IDC_EDIT_Y) )
	{
		if(pMsg->message == WM_KEYDOWN)  
		{
			if( isdigit(pMsg->wParam) /*|| pMsg->wParam == VK_OEM_MINUS */)
			{
				;
			}
			else if (pMsg->wParam >= VK_NUMPAD0 && pMsg->wParam <= VK_NUMPAD9 )
			{
				;
			}
			else if(pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_DELETE 
				|| pMsg->wParam == VK_BACK || pMsg->wParam == VK_TAB)
			{
				;
			}
			else
			{
				CWnd * curFocus = GetFocus();
				//AfxMessageBox(Translate(_T("Please input integer!")));
				curFocus->SetFocus();
				return TRUE;
			}
		}

	}
	return CMFCPropertyPage::PreTranslateMessage(pMsg);
}
