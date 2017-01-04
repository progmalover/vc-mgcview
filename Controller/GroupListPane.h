#pragma once

#include "GroupListCtrl.h"

// CGroupListPane

class CGroupListPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CGroupListPane)

public:
    CGroupListPane();
    virtual ~CGroupListPane();

    GroupListCtrl& GetGroupList(){return m_wndGroupList;}

public:
    GroupListCtrl        m_wndGroupList;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


