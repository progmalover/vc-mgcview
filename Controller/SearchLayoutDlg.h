#pragma once
#include "atlcomtime.h"
#include "afxwin.h"
#include "afxbutton.h"
#include "MFCButtonEx.h"
#include "Singleton.h"
extern UINT LAYOUTDLG_SEARCH;


// CSearchLayoutDlg 对话框
class LayoutSearchOptions
{
    DECLARE_DYNC_SINGLETON(LayoutSearchOptions)
public:
    LayoutSearchOptions()
    {
        _StrDesc = _T("");
        _GroupID = -1;
        _GroupIDSel = 0;
        _OwnerID = -1;
        _OwnerIDSel = 0;
        _SizeID = -1;
        _SizeIDSel = 0;
        _AuditID = -1;
        _AuditIDSel = 0;
        _bUseDate = FALSE;
        _timeStart = COleDateTime::GetCurrentTime();
        _timeEnd = COleDateTime::GetCurrentTime();
    };
    ~LayoutSearchOptions()
    {

    };
    CString _StrDesc;
    int		_GroupID;
    int     _GroupIDSel;
    int		_OwnerID;
    int     _OwnerIDSel;
    int		_SizeID;
    int     _SizeIDSel;
    int    _AuditID;
    int     _AuditIDSel;
    BOOL  _bUseDate;
    COleDateTime  _timeStart;
    COleDateTime  _timeEnd;
};
class CSearchLayoutDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CSearchLayoutDlg)

public:
    CSearchLayoutDlg(CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CSearchLayoutDlg();

    // 对话框数据
    enum { IDD = IDD_SEARCH_LAYOUT };


public:
    CString m_strDesc;
    CString m_strGroup;
    CString m_strOwner;
    CString m_strSize;
    CString m_strAudit;
    CString m_strStartTime;
    CString m_strEndTime;
    CMFCButtonEx m_SearchBtn;
    BOOL m_bUseDate;

    CComboBox m_ctlGroup;
    CComboBox m_ctlowner;
    CComboBox m_ctlSize;
    CComboBox m_ctlAudit;
    COleDateTime m_timeStart;
    COleDateTime m_timeEnd;
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();
public:
    afx_msg void OnDtnDatetimechangeDatetimepickStart(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDtnDatetimechangeDatetimepickEnd(NMHDR *pNMHDR, LRESULT *pResult);
 	afx_msg void OnClickedButtonSearch();
    DECLARE_MESSAGE_MAP()
private:
    void RecalcDropWidth(CComboBox* pComboBox);
   

    void UpdateGroup();
    void UpdateOwner();
    void UpdateSize();
    void UpdateAudit();

    void GetAllCategory(LayoutGroup* pRoot, std::vector< pair<CString, LayoutGroup> > &category, const CString& PreFix = _T(""));

    afx_msg void OnEnChangeEditDesc();
    afx_msg void OnBnClickedCheckDate();
};

