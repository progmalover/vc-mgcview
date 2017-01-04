#pragma once


// CDialogNewUser 对话框

class CDialogNewUser : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogNewUser)

public:
	CDialogNewUser(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogNewUser();

// 对话框数据
	enum { IDD = IDD_NEW_USER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    afx_msg void OnBnClickedOk();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
public:
//    CComboBox m_ctlCharater;
//    CEdit m_strUserName;
    CString m_strUserName;
    CString m_strPassword;
    CString m_strPassword2;

    virtual BOOL OnInitDialog();

    CString m_strDescription;
    CComboBox m_ctlCharater;
};
