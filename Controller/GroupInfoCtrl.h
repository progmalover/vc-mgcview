#pragma once

#include "ClientListCtrl.h"
#include "afxcmn.h"
#include "atlcomtime.h"

extern UINT CLIENTMANAGER_GROUPINFO_SAVE;
extern UINT CLIENTMANAGER_GROUPINFO_CLIENT_GROUP_CHANGE;

// CGroupInfoCtrl dialog

class CGroupInfoCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CGroupInfoCtrl)
    DECLARE_EASYSIZE

public:
	CGroupInfoCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGroupInfoCtrl();

// Dialog Data
	enum { IDD = IDD_GROUP_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(ClientListCtrl)
public:
    virtual void OnInitialUpdate(); // called first time after construct

    virtual BOOL AddClient(const ClientInfo& client, INT clientID); 
    virtual BOOL RemoveClient(const ClientInfo& client, INT clientID); 
    virtual BOOL UpdateClient(const ClientInfo& client, INT clientID); 

    virtual void ClearList(); 

    virtual BOOL SetCurGroup(const ClientGroup& group);
    virtual const ClientGroup& GetCurGroup() const {return m_CurGroup;}
    virtual void Clear();

    virtual BOOL InitGroup(INT GroupID);

    virtual INT GetCurClientID();

    virtual void UpdateClientsStatus();

    //}}AFX_VIRTUAL

protected:

    // Generated message map functions
protected:
    //{{AFX_MSG(ClientListCtrl)
    afx_msg void OnViewSmallIcons();
    afx_msg void OnViewLargeIcons();
    afx_msg void OnViewList();
    afx_msg void OnViewDetails();
    //}}AFX_MSG

    //afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);

    afx_msg LRESULT OnDragEnter(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragOver(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragLeave(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDrop(WPARAM wParam, LPARAM lParam);

    afx_msg LRESULT RefreshClientsStatus(WPARAM wp, LPARAM lp);
    afx_msg LRESULT UpdateGroupInfo (WPARAM wp, LPARAM lp);

    afx_msg BOOL  OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult ) ;

    afx_msg void OnMouseMove(UINT nFlags, CPoint point);

    // list view image lists
protected:
    CImageList                  m_LargeImageList;
    CImageList                  m_SmallImageList;

    ClientGroup                 m_CurGroup;
    INT                         m_InitGroupID;

    CBaseDropTarget             m_DropTarget;                   /// for drop target functionality
    COleDataSource              m_OleDataSource;


    // for tooltip
    CToolTipCtrl  m_tooltip;
    CWnd*         pwnd;
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    //CMap<CString, LPCTSTR, ClientInfo, ClientInfo&> m_ClientsPool;
    CMap<INT, INT&, ClientInfo, ClientInfo&> m_ClientsPool;

    // view type helpers
public:
    BOOL SetViewType(DWORD dwViewType);
    DWORD GetViewType();
private:

public:
    CListCtrl m_ClientsInGroup;
    INT       m_CurClientID;
    afx_msg void OnNMClickListGroup(NMHDR *pNMHDR, LRESULT *pResult);
    CString m_SelectedGroupName;
    CString m_SelectedGroupDesc;
    afx_msg void OnBnClickedButtonSave();
    afx_msg void OnLvnBegindragListGroup(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEnChangeEditGname();
    afx_msg void OnEnChangeEditGdesc();
    BOOL      m_bModified;
    afx_msg void OnBnClickedBtnSaveGroupInfo();
	int m_volume;
	COleDateTime m_startTime;
	COleDateTime m_endTime;
	afx_msg void OnDtnDatetimechangeDatetimepickerFrom(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepickerTo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderGroupVolume(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckAlwaysWork();
	BOOL m_bAlwaysWork;
};
