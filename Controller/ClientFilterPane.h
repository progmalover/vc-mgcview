#pragma once
#include "ClientFilterCtrl.h"

// CClientFilterPane

class CClientFilterPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CClientFilterPane)

public:
    CClientFilterPane();
    virtual ~CClientFilterPane();

public:
    CClientFilterCtrl   m_ClientFilterCtrl;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


