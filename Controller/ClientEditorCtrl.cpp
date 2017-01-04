// ClientEditor.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ClientEditorCtrl.h"
#include "Localization/Localization.h"

#include "json/json.h"
#include "Utilities/StringUtility.h"

UINT CLIENTMANAGER_CLIENTINFO_SAVE = ::RegisterWindowMessage (_T("CLIENTMANAGER_CLIENTINFO_SAVE"));

// CClientEditorCtrl dialog

IMPLEMENT_DYNAMIC(CClientEditorCtrl, CDialogEx)

CClientEditorCtrl::CClientEditorCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientEditorCtrl::IDD, pParent)
    , m_bModified (FALSE)
{
}

CClientEditorCtrl::~CClientEditorCtrl()
{
}

void CClientEditorCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_DESC, m_Description);
    DDX_Text(pDX, IDC_EDIT_LOCATION, m_Location);
    DDX_Text(pDX, IDC_EDIT_ADMIN, m_Admin);
    DDX_Text(pDX, IDC_EDIT_MAIL, m_Email);
    DDX_Text(pDX, IDC_EDIT_COMPANY, m_Company);
    DDX_Text(pDX, IDC_EDIT_PHONE, m_Phone);
    DDX_Text(pDX, IDC_STATIC_INFO, m_UnchangableInfo);

	DDX_Control(pDX, IDC_COMBO1, m_comboElement);
	DDX_Control(pDX, IDC_COMBO2, m_comboOperator);
	DDX_Text(pDX, IDC_EDIT1, m_value);
	//DDV_MinMaxUInt(pDX, m_value, 0, 9999);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
	DDX_Control(pDX, IDC_EDIT1, m_edit1);
	DDX_Control(pDX, IDC_COMBO3, m_comboxAlarmLevel);

}


BEGIN_MESSAGE_MAP(CClientEditorCtrl, CDialogEx)
    ON_WM_SIZE()
    ON_EN_CHANGE(IDC_EDIT_DESC, &CClientEditorCtrl::OnEnChangeEditDesc)
    ON_EN_CHANGE(IDC_EDIT_LOCATION, &CClientEditorCtrl::OnEnChangeEditLocation)
    ON_EN_CHANGE(IDC_EDIT_ADMIN, &CClientEditorCtrl::OnEnChangeEditAdmin)
    ON_EN_CHANGE(IDC_EDIT_MAIL, &CClientEditorCtrl::OnEnChangeEditMail)
    ON_EN_CHANGE(IDC_EDIT_COMPANY, &CClientEditorCtrl::OnEnChangeEditCompany)
    ON_EN_CHANGE(IDC_EDIT_PHONE, &CClientEditorCtrl::OnEnChangeEditPhone)
    ON_BN_CLICKED(IDC_BTN_SAVE, &CClientEditorCtrl::OnBnClickedBtnSave)

	ON_BN_CLICKED(IDC_BTN_ADD_ALARMSET, &CClientEditorCtrl::OnBnClickedButton1)

	ON_EN_KILLFOCUS(IDC_EDIT_MAIL, &CClientEditorCtrl::OnEnKillfocusEditMail)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CClientEditorCtrl::OnLvnItemchangedList1)
	ON_NOTIFY(NM_KILLFOCUS, IDC_LIST1, &CClientEditorCtrl::OnNMKillfocusList1)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CClientEditorCtrl)
	EASYSIZE(IDC_STATIC_INFO_GROUP,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_INFO,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_DESC,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)

	EASYSIZE(IDC_EDIT_DESC,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	
    EASYSIZE(IDC_STATIC_LOCATION,IDC_EDIT_DESC,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_COMPANY,IDC_EDIT_DESC,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)

    EASYSIZE(IDC_EDIT_LOCATION,IDC_STATIC_LOCATION,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_EDIT_COMPANY,IDC_STATIC_COMPANY,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)

    EASYSIZE(IDC_STATIC_ADMIN,IDC_EDIT_LOCATION,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_PHONE,IDC_EDIT_COMPANY,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)

    EASYSIZE(IDC_EDIT_ADMIN,IDC_STATIC_ADMIN,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_EDIT_PHONE,IDC_STATIC_PHONE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)

    EASYSIZE(IDC_STATIC_MAIL,IDC_EDIT_ADMIN,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_EDIT_MAIL,IDC_STATIC_MAIL,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)

    EASYSIZE(IDOK,IDC_EDIT_MAIL,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDCANCEL,IDOK,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)

END_EASYSIZE_MAP



// CClientEditorCtrl message handlers
BOOL CClientEditorCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;

	Translate(this);
	
	m_listCtrl.InsertColumn(0,Translate(_T("Element")),LVCFMT_LEFT, 150);
	m_listCtrl.InsertColumn(1,Translate(_T(" Operator")),LVCFMT_LEFT, 80);
	m_listCtrl.InsertColumn(2,Translate(_T("Value")), LVCFMT_LEFT, 80);
	m_listCtrl.InsertColumn(3,Translate(_T("Alarm Level")),LVCFMT_LEFT, 120);
	m_listCtrl.InsertColumn(4,_T(""),LVCFMT_LEFT,0);
	m_listCtrl.SetExtendedStyle(m_listCtrl.GetExtendedStyle() |LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_comboElement.ResetContent();
	int i;
	for( i = 0;i < ElementTypeCount; i++)
	{
		m_comboElement.AddString(Translate(ElementTypeArray[i]));
	}
	m_comboElement.SetCurSel(0);

	m_comboOperator.ResetContent();
	for(i = 0; i < OperatorTypeCount ;i++)
	{
		m_comboOperator.AddString(Translate(OperatorTypeArray[i]));
	}
	m_comboOperator.SetCurSel(0);


	m_value = 0;

	m_comboxAlarmLevel.ResetContent();
	for(i = 0; i < AlarmLevelTypeCount ;i++)
	{
		m_comboxAlarmLevel.AddString(Translate(AlarmLevelArray[i]));
	}
	m_comboxAlarmLevel.SetCurSel(AlarmLevelTypeCount-1);
    //Clear();
	
	Json::Reader jsreader;
	Json::Value conf;
	std::string strAlarmSettings = StringUtility::ConvertUnicodeToAnsi(m_szAlarmSetting.GetString());
	if (jsreader.parse(strAlarmSettings, conf))
	{
		int len = conf.size();

		m_listCtrl.DeleteAllItems();
		for (int i = 0; i< len; i++)
		{
			size_t nElement = conf[i]["element"].asUInt();
			size_t nOperator = conf[i]["operator"].asUInt();
			int nValue = conf[i]["value"].asInt();
			size_t nAlarmLevel = conf[i]["alarmlevel"].asUInt();

			m_listCtrl.InsertItem(i,Translate(ElementTypeArray[nElement]));     
			m_listCtrl.SetItemText(i,1,Translate((OperatorTypeArray[nOperator])));
			CString str2;
			str2.Format(_T("%d"),nValue);
			m_listCtrl.SetItemText(i,2,str2);
			m_listCtrl.SetItemText(i,3,Translate(AlarmLevelArray[nAlarmLevel]));
			CString str4;
			str4.Format(_T("{\"element\":%d,\"operator\":%d,\"value\":%d,\"alarmlevel\":%d}"),nElement,nOperator,nValue,nAlarmLevel );
			m_listCtrl.SetItemText(i,4,str4);
		}
	}
	

    GetDlgItem( IDOK)->EnableWindow(m_bModified);

    GetDlgItem( IDC_EDIT_DESC)->EnableWindow(TRUE);
    GetDlgItem( IDC_EDIT_LOCATION)->EnableWindow(TRUE);
    GetDlgItem( IDC_EDIT_ADMIN)->EnableWindow(TRUE);
    GetDlgItem( IDC_EDIT_MAIL)->EnableWindow(TRUE);
    GetDlgItem( IDC_EDIT_COMPANY)->EnableWindow(TRUE);
    GetDlgItem( IDC_EDIT_PHONE)->EnableWindow(TRUE);

    UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CClientEditorCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}



void CClientEditorCtrl::OnBnClickedButtonSave()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);

	m_CurClient.AddAdminName(m_Admin.GetString());
    m_CurClient.AddCompany(m_Company.GetString());
    m_CurClient.AddEmail(m_Email.GetString());
    m_CurClient.AddClientDesc(m_Description.GetString());
    m_CurClient.AddTelPhone(m_Phone.GetString());
    m_CurClient.AddLocation(m_Location.GetString());

	m_szAlarmSetting = _T("[");
	int count = m_listCtrl.GetItemCount();
	for(int i = 0 ; i< count ; i++)
	{
		m_szAlarmSetting += m_listCtrl.GetItemText(i,4);
		if(i != count -1) m_szAlarmSetting += _T(",");
	}
	m_szAlarmSetting += _T("]");
	m_CurClient.AddALarmSetting(m_szAlarmSetting.GetString());
    GetParent()->SendMessage(CLIENTMANAGER_CLIENTINFO_SAVE, GetCurClientID(), (LPARAM)&m_CurClient);
}


BOOL CClientEditorCtrl::SetCurClient(ClientInfo client, INT clientID, CString IP)
{
    m_CurClientID = clientID;
    m_CurClient = client;

    m_Admin = m_CurClient.GetAdminName().c_str();
    m_Company = m_CurClient.GetCompany().c_str();
    m_Email = m_CurClient.GetEmail().c_str();
    m_Description = m_CurClient.GetClientDesc().c_str();
    m_Phone = m_CurClient.GetTelPhone().c_str();
    m_Location = m_CurClient.GetLocation().c_str();
	m_szAlarmSetting = m_CurClient.GetAlarmSetting().c_str();

    m_UnchangableInfo = _T("");

    if (!IP.IsEmpty())
    {
        CString IPaddress, strIP;
		strIP.Format(_T("%s\n"), IP);
        IPaddress = Translate(_T("IP Address: ")) + strIP;
        m_UnchangableInfo += IPaddress;
    }

    m_UnchangableInfo += Translate(_T("Display Card Type: "));
    m_UnchangableInfo +=  m_CurClient.GetDisplayCardType().c_str();
    m_UnchangableInfo += _T("\t\t\n");
    m_UnchangableInfo += Translate(_T("Software Info: "));
    m_UnchangableInfo +=  m_CurClient.GetSoftwareInfo().c_str();
    m_UnchangableInfo += _T("\t\t\n");

	CString szHardWardInfo = m_CurClient.GetHardwareInfo().c_str();
	CString separate_str = _T("Memory");
	CString szCPUinfo;
	CString szMemoryinfo;
	int index = 0;
	if(-1 != (index = szHardWardInfo.Find(separate_str)))
	{
		szCPUinfo = szHardWardInfo.Mid(0, index);
		szCPUinfo.TrimLeft();
		szMemoryinfo = szHardWardInfo.Mid(index+separate_str.GetLength()+1, szHardWardInfo.GetLength()-index-separate_str.GetLength()-1) ;
		szMemoryinfo.TrimLeft();

		m_UnchangableInfo += szCPUinfo;
		m_UnchangableInfo += Translate(_T("Memory: "));
		m_UnchangableInfo += szMemoryinfo;

		//m_UnchangableInfo += Translate(_T("Hardware Info: "));
		//CString tmp = _T("CPU: ") + szCPUinfo + _T(" ") +Translate(_T("Memory: ")) + szMemoryinfo;
		//m_UnchangableInfo += tmp;
	}
	else
	{
		m_UnchangableInfo += Translate(_T("Hardware Info: "));
		m_UnchangableInfo +=  m_CurClient.GetHardwareInfo().c_str();
	}
    m_UnchangableInfo += _T("\t\t");
    m_bModified = FALSE;


    return TRUE;
}

void CClientEditorCtrl::Clear()
{

    m_CurClientID = -1;
    m_CurClient = ClientInfo();

    m_Admin = _T("");
    m_Company = _T("");
    m_Email = _T("");
    m_Description = _T("");
    m_Phone = _T("");
    m_Location = _T("");

	m_szAlarmSetting = _T("");

    m_UnchangableInfo = _T("");

    m_bModified = FALSE;
    GetDlgItem( IDOK)->EnableWindow(m_bModified);

    GetDlgItem( IDC_EDIT_DESC)->EnableWindow(FALSE);
    GetDlgItem( IDC_EDIT_LOCATION)->EnableWindow(FALSE);
    GetDlgItem( IDC_EDIT_ADMIN)->EnableWindow(FALSE);
    GetDlgItem( IDC_EDIT_MAIL)->EnableWindow(FALSE);
    GetDlgItem( IDC_EDIT_COMPANY)->EnableWindow(FALSE);
    GetDlgItem( IDC_EDIT_PHONE)->EnableWindow(FALSE);

    UpdateData(FALSE);

    return;
}

void CClientEditorCtrl::OnEnChangeEditDesc()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    m_bModified= TRUE;
    GetDlgItem( IDOK)->EnableWindow(m_bModified);
}


void CClientEditorCtrl::OnEnChangeEditLocation()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    m_bModified= TRUE;
    GetDlgItem( IDOK)->EnableWindow(m_bModified);
}


void CClientEditorCtrl::OnEnChangeEditAdmin()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    m_bModified= TRUE;
    GetDlgItem( IDOK)->EnableWindow(m_bModified);
}


void CClientEditorCtrl::OnEnChangeEditMail()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    m_bModified= TRUE;
    GetDlgItem( IDOK)->EnableWindow(m_bModified);
}


void CClientEditorCtrl::OnEnChangeEditCompany()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    m_bModified= TRUE;
    GetDlgItem( IDOK)->EnableWindow(m_bModified);
}


void CClientEditorCtrl::OnEnChangeEditPhone()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    m_bModified= TRUE;
    GetDlgItem( IDOK)->EnableWindow(m_bModified);
}

void CClientEditorCtrl::OnBnClickedBtnSave()
{
    // TODO: Add your control notification handler code here

    OnBnClickedButtonSave();
    m_bModified = FALSE;
    GetDlgItem( IDOK)->EnableWindow(m_bModified);

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
        (LPARAM)Translate(_T("Group Information Changed!")).GetString());
}


void CClientEditorCtrl::OnCancel()
{
    CDialogEx::OnCancel();
}

void CClientEditorCtrl::OnOK()
{
    OnBnClickedButtonSave();

    CDialogEx::OnOK();
}

void CClientEditorCtrl::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	CString str2;
	m_edit1.GetWindowText(str2);
	if(str2 == _T(""))
	{
		GetDlgItem(IDC_EDIT1)->SetFocus();
		return;
	}

	UpdateData(TRUE);

	if(m_value < 0 || m_value > 9999)
	{
		GetDlgItem(IDC_EDIT1)->SetFocus();
		return;
	}

	CString str0;
	int cursel0 = m_comboElement.GetCurSel();
	m_comboElement.GetWindowText(str0);

	CString str1;
	int cursel1 = m_comboOperator.GetCurSel();
	m_comboOperator.GetWindowText(str1);

	//CString str2;
	//m_edit1.GetWindowText(str2);

	CString str3;
	int cursel3 = m_comboxAlarmLevel.GetCurSel();
	m_comboxAlarmLevel.GetWindowText(str3);

	CString str4;
	str4.Format(_T("{\"element\":%d,\"operator\":%d,\"value\":%d,\"alarmlevel\":%d}"),cursel0,cursel1,m_value,cursel3 );

	int count = m_listCtrl.GetItemCount();
	m_listCtrl.InsertItem(count,str0);     
	m_listCtrl.SetItemText(count,1,str1);
	m_listCtrl.SetItemText(count,2,str2);
	m_listCtrl.SetItemText(count,3,str3);
	m_listCtrl.SetItemText(count,4,str4);
	//m_listCtrl.SetItemText(0,3,_T("P1"));

	m_bModified= TRUE;
	GetDlgItem( IDOK)->EnableWindow(m_bModified);

	//CClientConfiguerDlg * dlg = (CClientConfiguerDlg * )AfxGetMainWnd();
	//dlg->m_saveButton.EnableWindow(TRUE);
}

BOOL CClientEditorCtrl::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		{
			if(GetFocus() == GetDlgItem(IDC_EDIT1) )
			{
				if(pMsg->message == WM_KEYDOWN)  
				{
					if( isdigit(pMsg->wParam) /*|| pMsg->wParam == VK_OEM_MINUS */)
					{
						;
					}
					// 			else if( pMsg->wParam >= 0x00 && pMsg->wParam< 0x30) //Virtual Keys, Standard Set
					// 			{
					// 				;
					// 			}
					else if (pMsg->wParam >= VK_NUMPAD0 && pMsg->wParam <= VK_NUMPAD9 )
					{
						;
					}
					else if(pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_DELETE 
						|| pMsg->wParam == VK_BACK || pMsg->wParam == VK_TAB)
						// 			else if (pMsg->wParam == VK_SUBTRACT) // -
					{
						;
					}
					else
					{
						CWnd * curFocus = GetFocus();
						MessageBox(Translate(_T("Please input integer!")),Translate(_T("Warning:Client configure")), MB_OK|MB_ICONEXCLAMATION);
						curFocus->SetFocus();
						return TRUE;
					}
				}
			}
			else if (pMsg->wParam == VK_DELETE )
			{
				if(m_listCtrl.GetFocus() && m_listCtrl.GetSelectedCount())
				{
					std::vector<int> array;
					array.clear();
					POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
					while(pos)
					{
						INT index = m_listCtrl.GetNextSelectedItem(pos);

						if (index >= 0)
						{
							array.push_back(index);
							//m_listCtrl.DeleteItem(index);
						}
					}
					for(int i = array.size();i>0;i--)
					{
						m_listCtrl.DeleteItem(array[i-1]);
					}
				}
				return TRUE;
			}
			break;
		}
	default:
		break;
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CClientEditorCtrl::OnEnKillfocusEditMail()
{
	// TODO: Add your control notification handler code here
	CString email = m_Email;
	UpdateData(TRUE);

	int i = m_Email.Find('@');
	if(-1 == i || 0 == i || m_Email.GetLength() == i+1)
	{
		MessageBox(Translate(_T("Email is invalid!")),Translate(_T("Warning:Client configure")), MB_OK|MB_ICONEXCLAMATION);
		GetDlgItem(IDC_EDIT_MAIL)->SetFocus();
		return;
	}

}


void CClientEditorCtrl::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if(m_listCtrl.GetSelectedCount())
	{
		GetDlgItem(IDC_BTN_DELETE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BTN_DELETE)->EnableWindow(FALSE);
	}
}


void CClientEditorCtrl::OnNMKillfocusList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	GetDlgItem(IDC_BTN_DELETE)->EnableWindow(FALSE);
}
