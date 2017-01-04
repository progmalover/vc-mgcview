#pragma once

extern UINT CLIENT_FILTER_SEARCH;

// CClientFilterCtrl dialog

#define CLIENT_FILTER_HEIGHT 100

class CClientFilterCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CClientFilterCtrl)
    DECLARE_EASYSIZE
public:
	CClientFilterCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CClientFilterCtrl();

// Dialog Data
	enum { IDD = IDD_CLIENT_FILTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnOK() { OnClickedButtonSearch(); }
    afx_msg void OnCancel() {}

	DECLARE_MESSAGE_MAP()

public:
    CString m_strDesc;
    CComboBox m_ctlGroup;
    afx_msg void OnClickedButtonSearch();

    void UpdateGroup();
    afx_msg void OnEnChangeEditDesc();
};
