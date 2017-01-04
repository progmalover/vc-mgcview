#pragma once
#include <afx.h>
#include "resource.h"
#include "BaseDropTarget.h"
#include "Controller.h"
#include "ClientStatus.h"

/////////////////////////////////////////////////////////////////////////////


class ClientListCtrl : public CListCtrl
{

    // Operations
public:

    ClientListCtrl();
    DECLARE_DYNCREATE(ClientListCtrl)
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(ClientListCtrl)
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    virtual void OnInitialUpdate(); // called first time after construct

    virtual BOOL InitGroupsInfo(const std::vector<ClientGroup>& groups);

    virtual BOOL AddClient(const ClientInfo& client, INT clientID); 
    virtual BOOL RemoveClient(const ClientInfo& client, INT clientID); 
    virtual BOOL UpdateClient(const ClientInfo& client, INT clientID); 

    virtual void ClearList(); 

    virtual INT GetCurClientID(CString& IPAddress);//return ClientID, parameter is IP address

    virtual void UpdateClientsStatus();
    

    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~ClientListCtrl();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    
protected:

    // Generated message map functions
protected:
    //{{AFX_MSG(ClientListCtrl)
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

    afx_msg void OnGetToolTip(NMHDR* pNMHDR, LRESULT *pResult);

    afx_msg LRESULT RefreshClientsStatus(WPARAM wp, LPARAM lp);

    // list view image lists
protected:
    CImageList                  m_LargeImageList;
    CImageList                  m_SmallImageList;


    CBaseDropTarget             m_DropTarget;                   /// for drop target functionality
    COleDataSource              m_OleDataSource;

    INT                         m_CurClientID;

    //CMap<CString, LPCTSTR, ClientInfo, ClientInfo&> m_ClientsPool;
    CMap<INT, INT&, ClientInfo, ClientInfo&> m_ClientsPool;
    CMap<INT, INT&, ClientGroup, ClientGroup&> m_GroupsPool;

    // view type helpers
public:
    BOOL SetViewType(DWORD dwViewType);
    DWORD GetViewType();
};


/////////////////////////////////////////////////////////////////////////////
