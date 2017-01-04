#pragma once

extern UINT CLIENTMANAGER_CLIENTINFO_SAVE;

// CClientEditorCtrl dialog

class CClientEditorCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CClientEditorCtrl)
    DECLARE_EASYSIZE
public:
	CClientEditorCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CClientEditorCtrl();

    virtual BOOL SetCurClient(ClientInfo client, INT clientID, CString IP);
    virtual INT  GetCurClientID(){return m_CurClientID;}
    virtual const ClientInfo& GetCurClient() const {return m_CurClient;}

    void Clear();
// Dialog Data
	enum { IDD = IDD_CLIENT_EDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
    virtual void OnOK();
    virtual void OnCancel();

public:
    CString m_Description;
    CString m_Location;
    CString m_Admin;
    CString m_Email;
    CString m_Company;
    CString m_Phone;
    afx_msg void OnBnClickedButtonSave();

    ClientInfo m_CurClient;
    INT        m_CurClientID;
    CString    m_UnchangableInfo;
    BOOL       m_bModified;
    afx_msg void OnEnChangeEditDesc();
    afx_msg void OnEnChangeEditLocation();
    afx_msg void OnEnChangeEditAdmin();
    afx_msg void OnEnChangeEditMail();
    afx_msg void OnEnChangeEditCompany();
    afx_msg void OnEnChangeEditPhone();
    afx_msg void OnBnClickedBtnSave();

	CComboBox m_comboElement;
	CComboBox m_comboOperator;
	DWORD m_value;
	CListCtrl m_listCtrl;
	afx_msg void OnBnClickedButton1();
	CEdit m_edit1;
	CComboBox m_comboxAlarmLevel;

	CString m_szAlarmSetting;

	virtual BOOL PreTranslateMessage(MSG* pMsg) ;
	afx_msg void OnEnKillfocusEditMail();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusList1(NMHDR *pNMHDR, LRESULT *pResult);
};
