#pragma once


// CUserEditorCtrl dialog

class CUserEditorCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CUserEditorCtrl)
    DECLARE_EASYSIZE
public:
	CUserEditorCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUserEditorCtrl();

    VOID SetCurrentUser(int UserID);
// Dialog Data
	enum { IDD = IDD_USER_EDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
public:
    CString m_Username;
    CString m_UserDesc;
    int     m_UserID;
    afx_msg void OnBnClickedButtonSave();
    afx_msg void OnBnClickedButtonSetpass();
    int m_nPrivilage;
    virtual void OnOK();
    virtual void OnCancel();
};
