#pragma once
#include "atlcomtime.h"
#include "afxwin.h"
#include "MFCButtonEx.h"


// CAlarmFilterCtrl dialog

class CAlarmFilterCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CAlarmFilterCtrl)
    DECLARE_EASYSIZE
public:
	CAlarmFilterCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAlarmFilterCtrl();

// Dialog Data
	enum { IDD = IDD_LOG_FILTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
public:
    BOOL m_bUseDate;
    BOOL m_bUseOperator;
    afx_msg void OnBnClickedButtonSearch();
    afx_msg void OnBnClickedButtonExportCsv();
    afx_msg void OnBnClickedRefreshUser();
    void SaveCsv(CFile& file, CString szFileName, unsigned long len);

    COleDateTime m_timeStart;
    COleDateTime m_timeEnd;
    CComboBox m_ctlOperators;
    CMFCButtonEx m_ctrlRefresh;
protected:
    std::vector<User> m_vUserList;
    virtual void OnOK();
    virtual void OnCancel();
public:
    afx_msg void OnDtnDatetimechangeDatetimepickerEndLog(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDtnDatetimechangeDatetimepickerStartLog(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckDate();
	afx_msg void OnBnClickedCheckOperator();
};
