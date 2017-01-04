#pragma once
#include "afxcmn.h"
#include "afxbutton.h"
#include "BaseDropTarget.h"
#include "DialogCtrlTreeBase.h"
#include "MFCButtonEx.h"
#include "SearchFileDlg.h"

enum MEDIA_LIST_TAB
{
    MEDIA_LIST_TAB1,
    UPLOAD_LIST_TAB2,
#ifndef PROFESSIONAL_EDITION
    DOWNLOAD_LIST_TAB3,
#endif
    CATEGORY_LIST_TAB4,
};

// CMediaLibraryCategoryCtrl dialog
extern UINT MEDIA_LIST_CATEGORY;
//extern UINT MEDIA_CATEGORY_REFRESH;
extern UINT MEDIA_SETACTIVE_TAB;

class CMediaLibraryCategoryCtrl : public CDialogCtrlTreeBase
{
	DECLARE_DYNAMIC(CMediaLibraryCategoryCtrl)
    DECLARE_EASYSIZE
public:
	CMediaLibraryCategoryCtrl(CWnd* pParent = NULL, BOOL bShowFiles = FALSE);   // standard constructor
	virtual ~CMediaLibraryCategoryCtrl();


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

    INT  GetSelectedCategory(ITEM_TYPE& ItemType);
	INT  GetItemCategory(HTREEITEM hItem,ITEM_TYPE& ItemType);
	BOOL GetMediaInfoString(int nMediaID,CString & strInfo );
    BOOL SetMediaFilterID(int mediaFilterID);

    BOOL SerializeMediaCategoryTo (CFile& file, INT CategoryID);
    static BOOL SerializeMediaCategoryFrom (CFile& file, MediaFilter &mediaCategory);

private:
    BOOL m_bShowFiles;
    CImageList m_ImageList;

    CBaseDropTarget         m_DropTarget;                   /// for drop target functionality
    COleDataSource          m_OleDataSource;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
    virtual void OnCancel();
    virtual void OnOK();


    afx_msg LRESULT OnDragEnter(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragOver(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragLeave(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDrop(WPARAM wParam, LPARAM lParam);

	BOOL SerializeMediaTo(CFile& file, INT MediaID);
	BOOL SerializeMediaFrom (CFile& file, std::vector<Media> &mediaList);

    BOOL InitRootCategory();

    afx_msg LRESULT UpdateRootCategory (WPARAM wp, LPARAM lp);
    afx_msg LRESULT UpdateSubCategory (WPARAM wp, LPARAM lp);

    afx_msg LRESULT UpdateSubCategoryAndFiles (WPARAM wp, LPARAM lp);

    HTREEITEM FindCategoryItem(HTREEITEM item, INT ID);
    void DeleteChildrens(HTREEITEM item);

    BOOL DeleteCategory(const MediaFilter& Category, vector<MediaFilter>& FailedSubCategorys,
        vector<Media>& FailedSubItems);

public:

    CTreeCtrl				m_ctrlCategoryTree;
	HTREEITEM				m_newAddingItem;
	CMFCToolTipCtrl			m_MediaInfoCtrl;
    CMFCButtonEx m_ctrlCategoryNew;
    CMFCButtonEx m_ctrlCategoryDelete;
    CMFCButtonEx m_ctrlCategoryRename;
    CMFCButtonEx m_ctrlCategoryRefresh;
    CMFCButtonEx m_ctrlCategoryFilter;
	CMFCButtonEx m_ctrlCategorySearch;
	
    afx_msg void OnTvnSelchangedMedialibTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnItemexpandingMedialibTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnDeleteitemMedialibTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkMedialibTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedMedialibCategoryNew();
    afx_msg void OnBnClickedMedialibCategoryDel();
    afx_msg void OnBnClickedMedialibCategoryRename();
	afx_msg void OnTvnBeginlabeleditMedialibTree(NMHDR *pNMHDR,LRESULT *pResult);
    afx_msg void OnTvnEndlabeleditMedialibTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnBegindragMedialibTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnClickedFilterCategoryRefresh();
    afx_msg void OnClickedMediaCategory(NMHDR *pNMHDR, LRESULT *pResult);
	void RefreshSubCategory(MediaFilter & Category, HTREEITEM FatherItem);
	
	int SearchSubCategory(MediaFilter & Category, HTREEITEM FatherItem, CString str );
    UINT                    m_RootCategoryID;
    std::map<INT, Media>       m_MediaMap;
    std::map<INT, MediaFilter> m_MediaCategoryMap;

	//added by RaymondLiu
	std::vector<INT> m_ExpandedCategoryList;
    static CLIPFORMAT s_MediaFormat;
    static CLIPFORMAT s_MediaCategoryFormat;

    CString m_FilterValue;
    afx_msg void OnClickedFilterCategoryFilter();
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedFilterCategorySearch();
};
