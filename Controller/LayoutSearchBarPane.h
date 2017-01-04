#pragma once
#include "LayoutSearchBar.h"

// CLayoutSearchBarPane

class CLayoutSearchBarPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CLayoutSearchBarPane)

public:
    CLayoutSearchBarPane();
    virtual ~CLayoutSearchBarPane();

public:
    CLayoutSearchBarCtrl   m_LayoutSearchBarCtrl;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


