#pragma once
#include "basecontrollerpage.h"
#include "LayoutListPane.h"
#include "LayoutCategoryPane.h"
#include "LayoutInfoPane.h"
#include "LayoutSearchBarPane.h"
#include "SearchLayoutDlg.h"

extern UINT LAYOUT_OPEN;
extern UINT LAYOUT_NEW;
extern UINT LAYOUT_IMPORT;

class CLayoutManagePage :
    public CBaseControllerPage
{

  DECLARE_DYNAMIC(CLayoutManagePage)
public:
    CLayoutManagePage(void);
    virtual ~CLayoutManagePage(void);
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

    afx_msg void OnRinbonBtnLayoutApprove();
	afx_msg void OnRinbonBtnLayoutReject();

	afx_msg void OnRinbonBtnLayoutDelete();
    afx_msg void OnRinbonBtnLayoutEdit();
    afx_msg void OnRinbonBtnLayoutNew();
    afx_msg void OnRinbonBtnLayoutImport();
    afx_msg void OnRinbonBtnLayoutPreview();
    afx_msg void OnRinbonBtnLayoutRefresh();
    
    afx_msg void OnRinbonBtnLayoutRename();
  //  afx_msg void OnRinbonBtnLayoutRequest();
    afx_msg void OnRibbonBtn_Preview10X7();
    afx_msg void OnRibbonBtn_Preview12X10();
    afx_msg void OnRibbonBtn_Preview19X12();

    afx_msg void OnUpdateRibbonBtn(CCmdUI* pCmdUI);
  //  afx_msg void OnUpdateRinbonBtnLayoutRequest(CCmdUI *pCmdUI);
    afx_msg void OnUpdateRinbonBtnLayoutRename(CCmdUI *pCmdUI);
    
    afx_msg void OnUpdateRibbonBtnPreview(CCmdUI *pCmdUI);
    afx_msg void OnUpdateRinbonBtnLayoutNew(CCmdUI *pCmdUI);
    afx_msg void OnUpdateRinbonBtnLayoutEdit(CCmdUI *pCmdUI);
    afx_msg void OnUpdateRinbonBtnLayoutDelete(CCmdUI *pCmdUI);
#ifdef SERVER_EDITION
    afx_msg void OnUpdateRinbonBtnLayoutApprove(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRinbonBtnLayoutReject(CCmdUI *pCmdUI);
#endif

    afx_msg LRESULT OnUpdateLayoutCategoryList(WPARAM, LPARAM);
    afx_msg LRESULT OnLayoutSelected(WPARAM, LPARAM);
    afx_msg LRESULT OnLayoutListRefresh(WPARAM, LPARAM);
    afx_msg LRESULT OnLayoutSearch(WPARAM, LPARAM);
    afx_msg LRESULT OnLayoutSetActiveTab(WPARAM, LPARAM);
	afx_msg LRESULT OnLayoutDlgSearch(WPARAM, LPARAM);
    DECLARE_MESSAGE_MAP()
protected:
	BOOL CreateRibbonButtons();
	BOOL UpdateOptions();

	    // Preview layout
    HRESULT PreviewLayout(const int nWidth, const int nHeight);

	void Close();


	CLayoutCategoryPane m_wndLayoutCategory;
	CLayoutListPane     m_wndLayoutList;
    CLayoutInfoPane     m_wndLayoutInfo;
    CLayoutSearchBarPane m_wndLayoutSearch;
    INT                 m_WorkingCategoryID;
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
private:
    UINT            nTry;
};

