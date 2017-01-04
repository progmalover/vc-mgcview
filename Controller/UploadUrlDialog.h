#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// CUploadUrlDialog

class CUploadUrlDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CUploadUrlDialog)

public:
    CUploadUrlDialog(CWnd* pParent = NULL, CString WindowName = Translate(_T("Upload URL")));
    virtual ~CUploadUrlDialog();

    // Dialog Data
    enum { IDD = IDD_UPLOAD_URL};

    void GetLayoutNewName(CString& szName) { szName = m_szLayoutName; };
	void GetURLType(CString & szUrlType);
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

private:
    CString m_szLayoutName;
    CString m_szWindowName;
	CString m_szUrlType;

public:
    afx_msg void OnBnClickedOk();
	CComboBox m_URLHeader;
	afx_msg void OnCbnSelchangeUrlHeader();
	afx_msg void OnEnChangeEditName();
};
