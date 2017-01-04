#pragma once
#include "PropertyCtrl.h"
#include "LayoutSettings.h"

// CLayoutPropertiesPane

class CLayoutPropertiesPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CLayoutPropertiesPane)

public:
    CLayoutPropertiesPane();
    virtual ~CLayoutPropertiesPane();

public:
    CPropertyCtrl m_PropertyCtrl;


protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()


};


