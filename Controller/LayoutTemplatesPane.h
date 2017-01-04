#pragma once

#include "TemplateCtrl.h"


// CLayoutTemplatesPane

class CLayoutTemplatesPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CLayoutTemplatesPane)

public:
    CLayoutTemplatesPane();
    virtual ~CLayoutTemplatesPane();

public:
    CTemplateCtrl           m_wndTemplateDlg;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


