// DesignerGridSetting.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "CommonSetting.h"
#include "Localization/Localization.h"

// CommonSetting dialog

IMPLEMENT_DYNAMIC(CommonSetting, CMFCPropertyPage)

CommonSetting::CommonSetting()
	: CMFCPropertyPage(CommonSetting::IDD)
    , m_RootWorkdirectory(_T(""))
    , m_strPort(_T(""))
    , m_nRotationSel(0)
{
	
}

CommonSetting::~CommonSetting()
{
}

void CommonSetting::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ROOT_DIR, m_RootWorkdirectory);
	DDX_Control(pDX, IDC_ROOT_DIR, m_ctrlBrowser);
	DDX_Control(pDX, IDC_IPADDRESS_SERVER, m_ctrlServerIP);
	DDX_Control(pDX, IDC_STATIC_SERVER_IP, m_staticServerIP);
	DDX_Control(pDX, IDC_SERVER_PORT, m_ctrlPort);
	DDX_Text(pDX, IDC_SERVER_PORT, m_strPort);
	DDX_Control(pDX, IDC_STATIC_PORT, m_staticPort);
	DDX_Control(pDX, IDC_COMBO_ROTATION, m_ctrlRotationComboBox);
	DDX_Control(pDX, IDC_STATIC_PLAYER_GROUP, m_ctrlPlayerGroup);
	DDX_Control(pDX, IDC_STATIC_ROTATION, m_ctrlStaticRotation);
	DDX_CBIndex(pDX, IDC_COMBO_ROTATION, m_nRotationSel);
	DDX_Control(pDX, IDC_HOTKEY1, m_HotKey);
	DDX_Control(pDX, IDC_STATIC_SHUTDOWN, m_staticShowDown);
}

BOOL CommonSetting::OnInitDialog()
{
    CMFCPropertyPage::OnInitDialog();

	m_ctrlServerIP.SetWindowText(m_strServerIP);
    m_ctrlRotationComboBox.AddString(Translate(_T("0 Degrees")));
    m_ctrlRotationComboBox.AddString(Translate(_T("90 Degrees")));
    m_ctrlRotationComboBox.AddString(Translate(_T("180 Degrees")));
    m_ctrlRotationComboBox.AddString(Translate(_T("270 Degrees")));

#ifdef STARTER_EDITION
	WORD wVirtualKeyCode;
	WORD wModifiers;
	wModifiers = m_iHotKey >> 16 & 0X0000FFFF;
	wVirtualKeyCode = m_iHotKey & 0X0000FFFF;
	m_HotKey.SetHotKey(wVirtualKeyCode, wModifiers);
#endif
#ifndef PROFESSIONAL_EDITION
	m_staticServerIP.ShowWindow(SW_HIDE);
	m_ctrlServerIP.ShowWindow(SW_HIDE);

	m_staticPort.ShowWindow(SW_HIDE);
	m_ctrlPort.ShowWindow(SW_HIDE);
#endif
#ifdef STANDALONE_EDITION
	m_staticServerIP.ShowWindow(SW_HIDE);
	m_ctrlServerIP.ShowWindow(SW_HIDE);

	m_staticPort.ShowWindow(SW_HIDE);
	m_ctrlPort.ShowWindow(SW_HIDE);
#endif

#ifndef STARTER_EDITION
    //m_ctrlUseMAMMPlayer.ShowWindow(SW_HIDE);
    m_ctrlRotationComboBox.ShowWindow(SW_HIDE);
	m_HotKey.ShowWindow(SW_HIDE);
    m_ctrlPlayerGroup.ShowWindow(SW_HIDE);
    m_ctrlStaticRotation.ShowWindow(SW_HIDE);
	m_staticShowDown.ShowWindow(SW_HIDE);
#endif

	UpdateData(FALSE);

	Translate(this);
    return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CommonSetting, CMFCPropertyPage)
    ON_EN_CHANGE(IDC_ROOT_DIR, &CommonSetting::OnEnChangeRootDir)
    ON_EN_CHANGE(IDC_IPADDRESS_SERVER, &CommonSetting::OnEnChangeIpaddressServer)
    ON_EN_CHANGE(IDC_SERVER_PORT, &CommonSetting::OnEnChangeServerPort)
    ON_CBN_SELCHANGE(IDC_COMBO_ROTATION, &CommonSetting::OnCbnSelchangeComboRotation)
END_MESSAGE_MAP()


// CommonSetting message handlers


void CommonSetting::OnEnChangeRootDir()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CMFCPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here

    SetModified();
}


void CommonSetting::OnEnChangeIpaddressServer()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CMFCPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    SetModified();
}


void CommonSetting::OnEnChangeServerPort()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CMFCPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    SetModified();
}


void CommonSetting::OnCbnSelchangeComboRotation()
{
    // TODO: Add your control notification handler code here
    SetModified();
}
