#pragma once
#include "Resource.h"

class CScrollHelper;

// CUserPrivilegeEditDlg dialog

class CUserPrivilegeEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUserPrivilegeEditDlg)
public:
	CUserPrivilegeEditDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUserPrivilegeEditDlg();

    VOID SetCurrentUser(int UserID);
    bool NeedSave();
	bool IsUserDescChanged();

// Dialog Data
	enum { IDD = IDD_PRIVILEGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

    void DisableAllControls();

    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point)
	{
			this->SetFocus();
	}
	DECLARE_MESSAGE_MAP()

    std::set<UserPrivilege> m_privileges;
    std::set<UserPrivilege> m_unSavedPrivileges;
    int m_nUserId;

	std::tstring m_description;
    CScrollHelper* m_scrollHelper;
public:
    std::vector<UserPrivilege> m_vPrivileges;
    afx_msg void OnBnClickedButtonSave();
    afx_msg void OnBnClickedButtonSetPassword();
    afx_msg void OnCheckPrivilege(UINT id);
    int m_nPrivilage;
    virtual void OnOK();
    virtual void OnCancel();
    void SetPrivilege(const std::set<UserPrivilege>& prvs);
    afx_msg void OnBnClickedBtnCheckAll();
    afx_msg void OnBnClickedBtnUncheckAll();
	afx_msg void OnEnChangeUserDesc();
};
