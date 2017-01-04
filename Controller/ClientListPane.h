#pragma once

#include "ClientListCtrl.h"
#include "ClientFilterCtrl.h"
// CClientListPane

class CClientListPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CClientListPane)

public:
    CClientListPane();
    virtual ~CClientListPane();

    ClientListCtrl& GetClientList(){return m_ClientList;}

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

private:
    ClientListCtrl        m_ClientList;
    CClientFilterCtrl     m_wndClientFilter;;
};


