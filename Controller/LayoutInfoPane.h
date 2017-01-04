#pragma once
#include "basecontrollerpane.h"
#include "LayoutInfoCtrl.h"
class CLayoutInfoPane :
    public CBaseControllerPane
{
public:
    CLayoutInfoPane(void);
    virtual ~CLayoutInfoPane(void);
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDestroy();
    
    CLayoutInfoCtrl    m_LayoutInfo;
};

