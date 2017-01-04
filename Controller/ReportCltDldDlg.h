#pragma once
#include "atlcomtime.h"
#include "afxwin.h"
#include "MagicSortListCtrl.h"
//extern UINT MEDIALIB_SEARCH;

// CReportCltDldDlg dialog

class CReportCltDldDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CReportCltDldDlg)
    DECLARE_EASYSIZE
public:
	CReportCltDldDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CReportCltDldDlg();

// Dialog Data
	enum { IDD = IDD_REPORT_CLIENT_DOWNLOAD }; 

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnOK() { OnClickedButtonSearch(); }
    afx_msg void OnCancel() {}
public:	
	afx_msg void OnDtnDatetimechangeDatetimepickStart(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepickEnd(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

public:
	CString m_strDesc;
	CString m_strType;
	CString m_strStartTime;
	CString m_strEndTime;
	
    CComboBox m_ctlType;

	COleDateTime m_timeStart;
	COleDateTime m_timeEnd;

    afx_msg void OnClickedButtonSearch();
	afx_msg void OnClickedButtonExport();

    void SaveCsv(CFile& file, CString szFileName);

    void UpdateType();

    CMagicSortListCtrl m_ReportCltDldList;
};
