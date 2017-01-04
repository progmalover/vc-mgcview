#pragma once
#include <afx.h>
#include "resource.h"
#include "BaseDropTarget.h"
#include "Controller.h"
#include "ClientStatus.h"

extern UINT CLIENTMANAGER_GROUP_SELECTED;
extern UINT CLIENTMANAGER_GROUP_CLIENT_GROUP_CHANGE;

/////////////////////////////////////////////////////////////////////////////

class GroupListCtrl : public CListCtrl
{

    // Operations
public:

    GroupListCtrl();
    DECLARE_DYNCREATE(GroupListCtrl)
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(GroupListCtrl)
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    virtual void OnInitialUpdate(); // called first time after construct

    virtual INT  GetCurGroupID();
	virtual ClientGroup GetCurClientGroup();

    virtual void UpdateGroupsStatus();
    virtual void ShowOfflineGroup();
	void RefreshGroupsList();
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~GroupListCtrl();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    
protected:

    // Generated message map functions
protected:
    //{{AFX_MSG(GroupListCtrl)
    afx_msg void OnViewSmallIcons();
    afx_msg void OnViewLargeIcons();
    afx_msg void OnViewList();
    afx_msg void OnViewDetails();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point) ;
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    //afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnBeginDrag(NMHDR *pNMHDR, LRESULT *pResult);

    afx_msg LRESULT OnDragEnter(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragOver(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragLeave(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDrop(WPARAM wParam, LPARAM lParam);

    afx_msg LRESULT RefreshGroupsStatus(WPARAM wp, LPARAM lp);

    // list view image lists
protected:
    CImageList                  m_LargeImageList;
    CImageList                  m_SmallImageList;


    CBaseDropTarget             m_DropTarget;                   /// for drop target functionality
    COleDataSource              m_OleDataSource;

	BOOL m_showOfflineGroup;
    CMap<INT, INT, ClientGroup, ClientGroup&> m_GroupsPool;

    INT                         m_CurID;

    // view type helpers
public:
    BOOL SetViewType(DWORD dwViewType);
    DWORD GetViewType();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};


/////////////////////////////////////////////////////////////////////////////
