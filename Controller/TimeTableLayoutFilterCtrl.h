#pragma once
#include "Resource.h"

// CTimeTableLayoutFilterCtrl dialog

class CTimeTableLayoutFilterCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CTimeTableLayoutFilterCtrl)
	DECLARE_EASYSIZE

public:
	CTimeTableLayoutFilterCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTimeTableLayoutFilterCtrl();

// Dialog Data
	enum { IDD = IDD_LAYOUT_FILTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

private:
	CComboBox m_GroupNames;
	CString m_sName;
	CEdit m_cEdit;
};
