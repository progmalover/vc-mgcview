#pragma once
#include "afxwin.h"
#include "afxeditbrowsectrl.h"
#include "afxcmn.h"


// CommonSetting dialog

class CommonSetting : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CommonSetting)

public:
	CommonSetting();   // standard constructor
	virtual ~CommonSetting();

// Dialog Data
	enum { IDD = IDD_COMMON_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    CString m_RootWorkdirectory;
    CMFCEditBrowseCtrl m_ctrlBrowser;
    CIPAddressCtrl m_ctrlServerIP;
    CStatic m_staticServerIP;
    CString m_strServerIP;
    CEdit m_ctrlPort;
    CString m_strPort;
    CStatic m_staticPort;
    afx_msg void OnEnChangeRootDir();
    afx_msg void OnEnChangeIpaddressServer();
    afx_msg void OnEnChangeServerPort();
    CComboBox m_ctrlRotationComboBox;
    CStatic m_ctrlPlayerGroup;
    CStatic m_ctrlStaticRotation;
    int m_nRotationSel;
    afx_msg void OnCbnSelchangeComboRotation();
	CHotKeyCtrl m_HotKey;
	INT m_iHotKey;
	CStatic m_staticShowDown;
};
