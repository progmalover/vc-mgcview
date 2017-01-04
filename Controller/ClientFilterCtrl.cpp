// ClientFilterCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ClientFilterCtrl.h"
#include "Localization/Localization.h"
UINT CLIENT_FILTER_SEARCH = ::RegisterWindowMessage (_T("CLIENT_FILTER_SEARCH"));

// CClientFilterCtrl dialog

IMPLEMENT_DYNAMIC(CClientFilterCtrl, CDialogEx)

CClientFilterCtrl::CClientFilterCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientFilterCtrl::IDD, pParent)
{

}

CClientFilterCtrl::~CClientFilterCtrl()
{
}

void CClientFilterCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_DESC, m_strDesc);
    DDX_Control(pDX, IDC_COMBO_GROUP, m_ctlGroup);
}


BEGIN_MESSAGE_MAP(CClientFilterCtrl, CDialogEx)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CClientFilterCtrl::OnClickedButtonSearch)
    ON_EN_CHANGE(IDC_EDIT_DESC, &CClientFilterCtrl::OnEnChangeEditDesc)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CClientFilterCtrl)
EASYSIZE(IDC_EDIT_DESC,IDC_STATIC_DESC,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_COMBO_GROUP,IDC_STATIC_GROUP,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_SEARCH,IDC_COMBO_GROUP,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
END_EASYSIZE_MAP


// CClientFilterCtrl message handlers
BOOL CClientFilterCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;

    UpdateGroup();
	
	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CClientFilterCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}


void CClientFilterCtrl::OnClickedButtonSearch()
{
    UpdateData(TRUE);

    // search
    // It should be done in the page, not here

    GetParent()->SendMessage(CLIENT_FILTER_SEARCH);
}

void CClientFilterCtrl::UpdateGroup()
{
    m_ctlGroup.ResetContent();
    std::vector<ClientGroup> clientGroup;
    HRESULT hr = GetControllerInterface->GetAllClientGroup(clientGroup);
    if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
    {
        GetControllerApp->OnControllerServerOffline();
        return;
    }

    auto it = clientGroup.begin(), ite = clientGroup.end();

    m_ctlGroup.InsertString(0, _T("All Groups"));
    m_ctlGroup.SetItemData(0, -1);

    for (int i = 1; it != ite; ++it, ++i)
    {
        m_ctlGroup.InsertString(i, it->GetGroupName().c_str());
        m_ctlGroup.SetItemData(i, it->GetGroupID());
    }

    m_ctlGroup.SetCurSel(0);
}

static bool RemoveInterpunction(CString &s)
{
    const CString::PCXSTR interpunction = _T("`!@#$%^&*()+-[]{}\\;:'\",./|=<>?~¡«£à£¡£À££¡ç£¥¦ä£¦£ª£¨£©£­£«£½£Û£Ý£û£ý£Ü£ü£º£»¡®¡¯¡°¡±¡¶¡·£¯£¿£¬¡£");
    int i;
    bool ret = false;
    while ((i = s.FindOneOf(interpunction)) != -1)
    {
        ret = true;
        s.Delete(i);
    }

    return ret;
}


void CClientFilterCtrl::OnEnChangeEditDesc()
{
    CString str;
    GetDlgItem(IDC_EDIT_DESC)->GetWindowText(str);

    if (RemoveInterpunction(str))
    {
        MessageBox(Translate(_T("Interpunctions are not allowed")), Translate(_T("Error")), MB_OK|MB_ICONERROR);
        GetDlgItem(IDC_EDIT_DESC)->SetWindowText(str);
    }
}
