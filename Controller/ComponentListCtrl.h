#pragma once

#include "BaseDropTarget.h"
#include "CommonLib/RenderableObject.h"

// CComponentListCtrl
struct COMPONENT_INFO
{
    CString     szName;
    HICON       hIcon;
};

class CComponentListCtrl : public CListCtrl
{
    DECLARE_DYNCREATE(CComponentListCtrl)

    // Operations
public:

    CComponentListCtrl();
    virtual ~CComponentListCtrl();


public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    virtual void OnInitialUpdate(); // called first time after

    virtual void InitPluginObj(std::vector<PLUGIN_INFO*>& pArrRenderableObj);

    void GetSelectedItem(int& selectedItem) { selectedItem = m_nSelectedItem; };
    BOOL GetSelectedItemName(CString& itemName);

    void GetComponentNames(CStringArray& itemNames);

    // list view image lists
protected:
    CImageList              m_LargeImageList;
    CImageList              m_SmallImageList;

    CBaseDropTarget         m_DropTarget;                   /// for drop target functionality
    COleDataSource          m_OleDataSource;

    CArray<COMPONENT_INFO*, COMPONENT_INFO*> m_Components;
    int                     m_nSelectedItem;

    int                     m_nImageWidth;
    int                     m_nImageHeight;

    // view type helpers
public:
    BOOL SetViewType(DWORD dwViewType);
    DWORD GetViewType();


    // Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLvnBeginDrag(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnGetToolTip(NMHDR* pNMHDR, LRESULT *pResult);

    afx_msg LRESULT OnDragEnter(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragOver(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragLeave(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDrop(WPARAM wParam, LPARAM lParam);


public:
    afx_msg void OnDestroy();

};