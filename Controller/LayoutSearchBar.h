#pragma once
#include "atlcomtime.h"
#include "afxwin.h"
#include "setdpi.h"
#include "afxbutton.h"
#include "MFCButtonEx.h"
extern UINT LAYOUT_LIST_SEARCH;

// CMediaLibrarySearchBarCtrl dialog

#define CLIENT_FILTER_HEIGHT 100

class CLayoutSearchBarCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CLayoutSearchBarCtrl)
    DECLARE_EASYSIZE
public:
	CLayoutSearchBarCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLayoutSearchBarCtrl();

// Dialog Data
	enum { IDD = IDD_MEDIA_LIB_SEARCH_BAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnOK() { OnClickedButtonSearch(); }
    afx_msg void OnCancel() {}
    afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
    afx_msg LRESULT OnLayoutSearchButton(WPARAM, LPARAM);
public:
	afx_msg void OnDtnDatetimechangeDatetimepickStart(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepickEnd(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

private:
    void RecalcDropWidth(CComboBox* pComboBox);

public:
	CString m_strDesc;
	CString m_strOwner;
	CString m_strSize;
	CString m_strApproval;
	CString m_strStartTime;
	CString m_strEndTime;
	CMFCButtonEx m_SearchBtn;
    BOOL m_bUseDate;

    CComboBox m_ctlCategory;
	CComboBox m_ctlowner;
	CComboBox m_ctlSize;
	CComboBox m_CtlApproval;
	COleDateTime m_timeStart;
	COleDateTime m_timeEnd;
    CSetDPI m_dpi;

    afx_msg void OnClickedButtonSearch();

    void UpdateCategory();
	void UpdateOwner();
	void UpdateSize();
	void UpdateApproval();
    void GetAllCategory(LayoutGroup* pRoot, std::vector< pair<CString, LayoutGroup> > &category, const CString& PreFix = _T(""));

    afx_msg void OnEnChangeEditDesc();
	afx_msg void OnCbnSelchangeCombo2();


    afx_msg void OnBnClickedCheckDate();
    afx_msg void OnCbnDropdownComboGroup();
    afx_msg void OnCbnCloseupComboGroup();
};
