#pragma once
#include "MFCButtonEx.h"
#include "Singleton.h"
extern UINT MEDIADLG_SEARCH;
// CSearchFileDlg 对话框

class MediaSearchOptions 
{
	DECLARE_DYNC_SINGLETON(MediaSearchOptions)
public:
	MediaSearchOptions()
	{
			_CategoryID = -1;
            _CategoryIDSel = 0;
			_OwnerID = -1;
            _OwnerIDSel = 0;
			_SizeId = -1;
            _SizeIdSel = 0;
			_ApprovalID = -1;
            _ApprovalIDSel = 0;
			_TypeID = -1;
            _TypeIDSel = 0;
			_HeightID = -1;
            _HeightIDSel = 0;
			_WidthID = -1;
            _WidthIDSel = 0;
			_DurID = -1;
            _DurIDSel = 0;
			_MediaTypeID = -1; 
			_bUseDate = FALSE;
			_strMediaType = _T("All Type");
			_timeStart = COleDateTime::GetCurrentTime();
			_timeEnd = COleDateTime::GetCurrentTime();
	};
	~MediaSearchOptions()
	{

	};

	CString _StrDesc;
	int		_CategoryID;
    int     _CategoryIDSel;
	int		_OwnerID;
    int     _OwnerIDSel;
	int		_SizeId;
    int     _SizeIdSel;
	int		_ApprovalID;
    int     _ApprovalIDSel;
	int		_TypeID;
    int     _TypeIDSel;
	int		_HeightID;
    int     _HeightIDSel;
	int		_WidthID;
    int     _WidthIDSel;
	int		_DurID;
    int     _DurIDSel;
	int		_MediaTypeID;
	BOOL	_bUseDate ;
	
	CString _strMediaType;
	COleDateTime  _timeStart;
    COleDateTime  _timeEnd;
};
class CSearchFileDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchFileDlg)

public:
	CSearchFileDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSearchFileDlg();


public:
    CString m_strDesc;
    CString m_strOwner;
    CString m_strSize;
    CString m_strAudit;
    CString m_strStartTime;
    CString m_strEndTime;

    BOOL m_bUseDate;

    CComboBox m_ctlCategory;
    CComboBox m_ctlOwner;
    CComboBox m_ctlSize;
    CComboBox m_ctlAudit;
    CComboBox m_ctlHeight;
    CComboBox m_ctlWidth;
    CComboBox m_ctlType;
    CComboBox m_ctlDur;
   
	COleDateTime m_timeStart;
    COleDateTime m_timeEnd;

    CMFCButtonEx  m_SearchBtn;

// 对话框数据
	enum { IDD = IDD_SEARCH_FILE_MEDIA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();
    afx_msg void OnDtnDatetimechangeDatetimepickStart(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDtnDatetimechangeDatetimepickEnd(NMHDR *pNMHDR, LRESULT *pResult);


	DECLARE_MESSAGE_MAP()
 //   virtual void OnCancel();
private:
    void RecalcDropWidth(CComboBox* pComboBox);

public:
    void UpdateCategory();
    void UpdateOwner();
    void UpdateSize();
    void UpdateAudit();
    void UpdateHeight();
    void UpdateWidth();
    void UpdateFileType();
    void UpdateDur();
    void GetAllCategory(MediaFilter* pRoot, std::vector< pair<CString, MediaFilter> > &category, const CString& PreFix = _T(""));

    afx_msg void OnEnChangeEditDesc();

    afx_msg void OnBnClickedButtonSearchdlgSearch();
    afx_msg void OnBnClickedCheckSearchdlgDate();
};
