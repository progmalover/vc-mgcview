#pragma once
#include "Resource.h"
#include "BaseDropTarget.h"
#include "Controller.h"
#include "ScheduleCtrl.h"
#include "DialogCtrlTreeBase.h"
#include "afxbutton.h"
#include "MFCButtonEx.h"
#include "SearchFileDlg.h"
// CLayoutCategoryCtrl dialog
extern UINT LAYOUT_LIST_CATEGORY;
extern UINT LAYOUT_LIST_REFRESH;
extern UINT LAYOUT_SETACTIVE_TAB;


class CLayoutCategoryCtrl : public CDialogCtrlTreeBase
{
	DECLARE_DYNAMIC(CLayoutCategoryCtrl)

public:
	CLayoutCategoryCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLayoutCategoryCtrl();

// Dialog Data
	enum { IDD = IDD_FILTER_TREE };

    enum ITEM_TYPE_LAYOUT{
        CATEGORY_INDEX,
        FILE_INDEX,
        UNAPPROVED_FILE_INDEX,
    };

protected:
	CMFCButtonEx m_NewBtn;
	CMFCButtonEx m_DelBtn;
	CMFCButtonEx m_RenameBtn;
	CMFCButtonEx m_RefreshBtn;
	CMFCButtonEx m_SearchBtn;
	CMFCButtonEx m_FilterBtn;
	HTREEITEM    m_newAddingItem;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnCancel();
    virtual void OnOK();

    BOOL InitRootCategory();

    afx_msg LRESULT OnUpdateRootCategory (WPARAM wp, LPARAM lp);
    afx_msg LRESULT OnUpdateSubCategory (WPARAM wp, LPARAM lp);

    afx_msg LRESULT OnUpdateSubCategoryAndFiles (WPARAM wp, LPARAM lp);
        // drag and drop
    afx_msg LRESULT OnDragLeave(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDrop(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragEnter(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragOver(WPARAM wParam, LPARAM lParam);

    DECLARE_EASYSIZE
	DECLARE_MESSAGE_MAP()

    HTREEITEM FindCategoryItem(HTREEITEM item, INT ID);
    void DeleteChildrens(HTREEITEM item);

    BOOL DeleteCategory(const LayoutGroup& Category, vector<LayoutGroup>& FailedSubCategorys,
        vector<Layout>& FailedSubItems);

    // serializing
    BOOL SerializeLayoutTo (CFile& file, INT LayoutID);
    BOOL SerializeLayoutGroupTo (CFile& file, INT LayoutGroupID);

    //BOOL SerializeLayoutFrom (CFile& file, Layout &layout);
	BOOL SerializeLayoutFrom (CFile& file, std::vector<Layout> &layoutList);
    BOOL SerializeLayoutGroupFrom (CFile& file, LayoutGroup &layoutgroup);
	BOOL GetLayoutInfoString(int nLayoutID,CString &strInfo);
    CImageList              m_ImageList;
    BOOL                    m_bFileMode;
    BOOL                    m_bShowEmptyLayout;
    BOOL                    m_bShowPowerOffLayout;
    CBaseDropTarget         m_DropTarget;                   /// for drop target functionality
    COleDataSource          m_OleDataSource;
    UINT                    m_RootCategoryID;
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnClickedFilterCategoryDel();
    afx_msg void OnClickedFilterCategoryNew();
    afx_msg void OnClickedFilterCategoryRefresh();
	void RefreshSubCategory(LayoutGroup & Category, HTREEITEM FatherItem);
 	INT  GetSelectedCategory(ITEM_TYPE_LAYOUT& ItemType);
	//added by RaymondLiu
	std::vector<INT> m_ExpandedCategoryList;

    afx_msg void OnClickedFilterCategoryRename();
    afx_msg void OnClickedFilterCategorySearch();
    afx_msg void OnSelchangedFilterTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnItemexpandingFilterTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeleteitemFilterTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDblclkFilterTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBeginlabeleditFilterTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEndlabeleditFilterTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBegindragFilterTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnClickedLayoutCategory(NMHDR *pNMHDR, LRESULT *pResult);
    void EnableFileMode();
    void ShowPowerOffLayout();
    void ShowEmptyLayout();


    CTreeCtrl m_ctrlCategoryTree;
	CMFCToolTipCtrl  m_LayoutToolTipCtrl;
    std::map<INT, Layout>       m_LayoutMap;
    std::map<INT, LayoutGroup>  m_LayoutGroupMap;

    static CLIPFORMAT s_LayoutCategoryFormat;
    static CLIPFORMAT s_LayoutFormat;

    afx_msg void OnClickedFilterCategoryFilter();
    CString m_FilterValue;
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
