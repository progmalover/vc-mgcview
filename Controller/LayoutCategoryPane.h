#pragma once
#include "basecontrollerpane.h"

#include "LayoutCategoryCtrl.h"


class CLayoutCategoryPane :
    public CBaseControllerPane
{
public:
    CLayoutCategoryPane(void);
    virtual ~CLayoutCategoryPane(void);

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDestroy();
    
    CLayoutCategoryCtrl    m_LayoutList;
};

