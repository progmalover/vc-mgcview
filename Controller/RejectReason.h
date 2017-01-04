#pragma once
#include "afxwin.h"


// CRejectReason 对话框

class CRejectReason : public CDialogEx
{
	DECLARE_DYNAMIC(CRejectReason)

public:
	CRejectReason(CWnd* pParent = NULL, BOOL bReadOnly = FALSE, CString szRejectReason = _T(""));   // 标准构造函数
	virtual ~CRejectReason();

// 对话框数据
	enum { IDD = IDD_REJECT_REASON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
    virtual void OnOK();
    virtual void OnCancel();
private:
    BOOL        m_bReadOnly;
public:
    CString     m_RejectReason;
    CEdit m_ctrlEditReason;
};



