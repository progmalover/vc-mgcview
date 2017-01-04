#pragma once


// CTemplateCtrl dialog

class CTemplateCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CTemplateCtrl)
    DECLARE_EASYSIZE
public:
	CTemplateCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTemplateCtrl();

// Dialog Data
	enum { IDD = IDD_TEMPLATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnOK(){}
    virtual void OnCancel(){}
    afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};
