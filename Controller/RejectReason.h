#pragma once
#include "afxwin.h"


// CRejectReason �Ի���

class CRejectReason : public CDialogEx
{
	DECLARE_DYNAMIC(CRejectReason)

public:
	CRejectReason(CWnd* pParent = NULL, BOOL bReadOnly = FALSE, CString szRejectReason = _T(""));   // ��׼���캯��
	virtual ~CRejectReason();

// �Ի�������
	enum { IDD = IDD_REJECT_REASON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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



