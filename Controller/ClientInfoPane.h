#pragma once
#include "ClientInfoCtrl.h"

// CClientInfoPane

class CClientInfoPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CClientInfoPane)

public:
    CClientInfoPane();
    virtual ~CClientInfoPane();

public:
    CClientInfoCtrl   m_InfoCtrl;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


