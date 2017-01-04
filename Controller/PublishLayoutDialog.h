#pragma once


// CPublishLayoutDialog dialog

class CPublishLayoutDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CPublishLayoutDialog)

public:
	CPublishLayoutDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPublishLayoutDialog();

	CTreeCtrl m_ctrlCategoryTree;

// Dialog Data
	enum { IDD = IDD_LAYOUT_PUBLISH_NEW };
	
	enum {
		CATEGORY_INDEX,
		FILE_INDEX,
		UNAPPROVED_FILE_INDEX,
	};
protected:
	BOOL InitRootCategory();

	afx_msg LRESULT OnUpdateRootCategory (WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnUpdateSubCategory (WPARAM wp, LPARAM lp);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	int m_CurCategoryID;
	CImageList              m_ImageList;
	UINT                    m_RootCategoryID;
	HTREEITEM FindCategoryItem(HTREEITEM item, INT ID);
	void DeleteChildrens(HTREEITEM item);
public:

	std::map<INT, Layout>       m_LayoutMap;
	std::map<INT, LayoutGroup>  m_LayoutGroupMap;

	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedFilterTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemexpandingFilterTree(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnDblclkFilterTree(NMHDR *pNMHDR, LRESULT *pResult);

	int GetCurCategoryID(void);
	afx_msg void OnBnClickedOk();
};
