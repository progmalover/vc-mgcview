#pragma once


// CUserListPane

class CUserListCtrl : public CMFCListCtrl
{
public:
    CUserListCtrl();
    ~CUserListCtrl();
    DECLARE_MESSAGE_MAP()
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
    int m_item;
};

class CUserListPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CUserListPane)

public:
    CUserListPane();
    virtual ~CUserListPane();

    CUserListCtrl        m_wndComponentList;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()
    CImageList              m_LargeImageList;


public:
    afx_msg void OnDestroy();

    void RefreshUsers();
};


