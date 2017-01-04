#pragma once

#include "MediaLibraryListPane.h"
#include "MediaLibraryCategoryPane.h"
#include "MediaLibrarySearchBarPane.h"
#include "MediaLibraryInfoPane.h"
#include "SearchFileDlg.h"

extern UINT MEDIASEARCH_BUTTON;
// CMediaLibraryPage

class CMediaLibraryPage : public CBaseControllerPage
{
    friend class CMediaLibraryListPane;

protected:
    DECLARE_DYNAMIC(CMediaLibraryPage)

public:
    CMediaLibraryPage();
    virtual ~CMediaLibraryPage();

public:

    // Close the main MediaLibrary monitor window.
    void Close();

//	void  UpdateMediaList();
    // Create the ribbon buttons
    BOOL CreateRibbonButtons();
	void DoMessage();
    BOOL UpdateOptions();
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    // ribbon buttons event

	afx_msg void OnRibbonBtnPreview();
    afx_msg void OnRefreshCurList();
	afx_msg void OnRibbonBtnApprove();
	afx_msg void OnRibbonBtnReject();
	afx_msg void OnRibbonBtnRequest();
	afx_msg void OnRibbonBtnUploadFile();
    afx_msg void OnRibbonBtnUploadURL();
	afx_msg void OnRibbonBtnChangeFile();
	afx_msg void OnRibbonBtnSyncFile();
	afx_msg void OnRibbonBtnForceSyncFile();
	afx_msg void OnRibbonBtnDeleteFile();
	afx_msg void OnRibbonBtnRetireFile();
	afx_msg void OnRibbonBtnAddWorkspace();
	afx_msg void OnRibbonBtnSyncWorkspace();
	afx_msg void OnRibbonBtnStartUpload();
	afx_msg void OnRibbonBtnPauseUpload();
	afx_msg void OnRibbonBtnDeleteUpload();
	afx_msg void OnRibbonBtnStartDownload();
	afx_msg void OnRibbonBtnPauseDownload();
	afx_msg void OnRibbonBtnDeleteDownload();
	afx_msg void OnUploadingList();
	afx_msg void OnUploadedList();
	afx_msg void OnUploadFailList();
	afx_msg void OnDownloadingList();
	afx_msg void OnDownloadedList();
	afx_msg void OnDownloadFailList();
    afx_msg void OnRenameCategoryList();

#ifdef SERVER_EDITION
	afx_msg void OnUpdateRibbonBtnApprove(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonBtnReject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonBtnRetire(CCmdUI* pCmdUI);
#endif

    afx_msg void OnUpdateRibbonBtn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonBtnPreview(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonBtnNeedSelect(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnNeedSelectServerFile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonBtnDelSelectServerFile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonBtnUpload(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonBtnDownload(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonCheckBtnUploading(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonCheckBtnUploaded(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonCheckBtnUploadFail(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonCheckBtnDownloading(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonCheckBtnDownloaded(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonCheckBtnDownloadFail(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnNeedSelectCategory(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnNeedSelSigCate(CCmdUI* pCmdUI);
	
    afx_msg LRESULT OnMediaLibrarySearch (WPARAM, LPARAM);
    afx_msg LRESULT OnMediaDlgSearch(WPARAM wp, LPARAM lp);
    afx_msg LRESULT UpdateMediaCategoryList(WPARAM, LPARAM);
    afx_msg LRESULT OnGetUploadUserInfo(WPARAM, LPARAM);
    afx_msg LRESULT OnGetMediaLibraryMediaList(WPARAM wp, LPARAM lp);
    afx_msg LRESULT OnGetServerMediaList(WPARAM wp,LPARAM lp);
    afx_msg LRESULT OnMediaLibraryCategoryRefresh(WPARAM, LPARAM);
	afx_msg LRESULT OnUpdateMediaList(WPARAM wp, LPARAM lp);
    afx_msg LRESULT OnMediaLibraryUploadList(WPARAM, LPARAM);
    afx_msg LRESULT OnMediaLibraryDownloadList(WPARAM, LPARAM);

    afx_msg LRESULT OnMediaInfoShow(WPARAM, LPARAM);
    afx_msg LRESULT OnMediaInfoClear(WPARAM, LPARAM);
    afx_msg LRESULT OnMediaSetActiveTab(WPARAM wp, LPARAM lp);

    afx_msg LRESULT OnMediaUpload(WPARAM, LPARAM);

    afx_msg void OnTimer(UINT_PTR nIDEvent);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();
	int IsCategoryCanBeDel(int categoryID);

    BOOL        GetIsUrl();
	static  void        GetCategoryFullPath(int CategoryID, CString& fullPath);
public:
	// Pane
	CMediaLibraryCategoryPane       m_wndMediaLibraryCategory;
	CMediaLibraryListPane           m_wndMediaLibraryList;

    CMediaLibraryInfoPane           m_wndMediaInfo;

protected:
	BOOL		m_bUploadingList;
	BOOL		m_bUploadedList;
	BOOL		m_bUploadFailList;
	BOOL		m_bDownloadingList;
	BOOL		m_bDownloadedList;
	BOOL		m_bDownloadFailList;

    BOOL        m_bIsUrl;

    void        Refresh();
    void        GetSelectItem(std::vector<int> &mediaIDList);
};


