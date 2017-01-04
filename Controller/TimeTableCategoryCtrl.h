#pragma once

#include "BaseDropTarget.h"
#include "Controller.h"
#include "ScheduleCtrl.h"
#include "DialogCtrlTreeBase.h"
#include "MFCButtonEx.h"
// CTimeTableCategoryCtrl dialog
extern UINT TIMETABLE_CATEGORY_SEL_CHANGE;
extern UINT TIMETABLE_CATEGORY_DBCLICK;

class CTimeTableCategoryCtrl : public CDialogCtrlTreeBase
{
	DECLARE_DYNAMIC(CTimeTableCategoryCtrl)

public:
	CTimeTableCategoryCtrl(CWnd* pParent = NULL, BOOL bReadOnly = FALSE);   // standard constructor
	virtual ~CTimeTableCategoryCtrl();

// Dialog Data
	enum { IDD = IDD_FILTER_TREE };

    enum ITEM_TYPE{
        ITEM_CATEGORY,
        ITEM_FILE
    };

    enum IMAGE_INDEX{
        CATEGORY_INDEX,
        FILE_INDEX,
        UNAPPROVED_FILE_INDEX,
    };



	bool isCategorySelected();

    INT   GetCurCategoryID();
    BOOL  GetSelectedTimeTable(Timetable& Table);
    BOOL AddTimetable(const Timetable& Table);

    BOOL InitRootCategory();

    void RefreshCurCategory();

    BOOL SerializeTimeTableCategoryTo (CFile& file, INT CategoryID);
	BOOL GetTimetableInfoString(Timetable tb,CString &strInfo);
	BOOL GetAuditInfoString(Audit adt,CString &strInfo);
    static BOOL SerializeTimeTableCategoryFrom (CFile& file, TimetableFilter &Category);

private:
    CImageList m_ImageList;
	HTREEITEM  m_newAddingItem;
    CBaseDropTarget         m_DropTarget;                   /// for drop target functionality
    COleDataSource          m_OleDataSource;

    CTreeCtrl m_ctrlCategoryTree;
    CMFCButtonEx m_ctrlCategoryNew;
    CMFCButtonEx m_ctrlCategoryDelete;
    CMFCButtonEx m_ctrlCategoryRename;
    CMFCButtonEx m_ctrlCategoryRefresh;
	CMFCButtonEx m_ctrlCategoryFilter;
    std::map<INT, Timetable>    m_TimetableMap;
    std::map<INT, Layout>       m_LayoutMap;

    BOOL					m_bEnableToolBar;
	CMFCToolTipCtrl 		*m_pToolTipCtrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    DECLARE_EASYSIZE
	DECLARE_MESSAGE_MAP()

    afx_msg LRESULT UpdateRootCategory (WPARAM wp, LPARAM lp);
    afx_msg LRESULT UpdateSubCategoryAndFiles (WPARAM wp, LPARAM lp);

    virtual void OnCancel();
    virtual void OnOK();
    HTREEITEM FindCategoryItem(HTREEITEM item, INT ID);
    void DeleteChildrens(HTREEITEM item);

    BOOL DeleteCategory(const TimetableFilter& Category, vector<TimetableFilter>& FailedSubCategorys, 
        vector<Timetable>& FailedSubTimetables);

    // serializing
    BOOL   SerializeTo (CFile& file, INT TimeTableID);
	BOOL   GetItemTimeTable(HTREEITEM ht,Timetable& Table);
    static BOOL SerializeTimeTableFrom (CFile& file, Timetable&Table);

public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual BOOL OnInitDialog();
    afx_msg void OnClickedFilterCategoryDel();
    afx_msg void OnClickedFilterCategoryNew();
    
	afx_msg void OnClickedFilterCategoryRefresh();
	void RefreshSubCategory(TimetableFilter & Category, HTREEITEM FatherItem);

	//added by RaymondLiu
	std::vector<INT> m_ExpandedCategoryList;

    afx_msg void OnClickedFilterCategoryRename();
    afx_msg void OnSelchangedFilterTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBegindragFilterTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeleteitemFilterTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBeginlabeleditFilterTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEndlabeleditFilterTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnItemexpandingFilterTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDblclkFilterTree(NMHDR *pNMHDR, LRESULT *pResult);

    afx_msg LRESULT OnDragEnter(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragOver(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragLeave(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDrop(WPARAM wParam, LPARAM lParam);

    UINT                    m_RootCategoryID;
    std::map<INT, TimetableFilter> m_TimetableCategoryMap;

    static CLIPFORMAT s_TimeTableCategoryFormat;
    static CLIPFORMAT s_TimeTableFormat;
    afx_msg void OnClickedFilterCategoryFilter();
    CString m_FilterValue;
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMClickFilterTree(NMHDR *pNMHDR, LRESULT *pResult);
};
