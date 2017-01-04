#pragma once
#include "afxwin.h"
#include "afxbutton.h"
#include "MFCButtonEx.h"
extern UINT CLIENTDLG_SEARCH;
// CSearchClientDlg �Ի���

class CSearchClientDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchClientDlg)

public:
	CSearchClientDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSearchClientDlg();

// �Ի�������
	enum { IDD = IDD_CLIENT_SEARCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
    CString     m_strName;
    CString     m_strAdmin;
    CString     m_strLocation;
    CString     m_strTel;
    CString     m_strCompany;
    CString     m_strEmail;
    CString     m_strHardware;
    CString     m_strIp;
    CMFCButtonEx m_SearchBtn;

    CComboBox   m_ctlGroup;

    void UpdateGroup();
    afx_msg void OnBnClickedButtonSearchdlgSearch();
private:
    void RecalcDropWidth(CComboBox* pComboBox);
};
