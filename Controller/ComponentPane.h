#pragma once


// CComponentPane

class CComponentPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CComponentPane)

public:
    CComponentPane();
    virtual ~CComponentPane();

public:
    CMFCListCtrl        m_wndComponentList;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


