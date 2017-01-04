#pragma once
#include "basecontrollerpane.h"


class CSchedulerItemSelectTab : public CMFCTabCtrl
{
    DECLARE_DYNAMIC(CSchedulerItemSelectTab)

public:
    CSchedulerItemSelectTab();
    virtual ~CSchedulerItemSelectTab();

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

};


class CSchedulerItemSelectPane :
    public CBaseControllerPane
{
public:
    CSchedulerItemSelectPane(void);
    virtual ~CSchedulerItemSelectPane(void);
public:
	CSchedulerItemSelectTab m_wndSchedulerSelectTab;
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();
};

