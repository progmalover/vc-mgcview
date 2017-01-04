#pragma once

#include "LogListPane.h"
#include "LogSearchPane.h"

// CLogSystemPage

class CLogSystemPage : public CBaseControllerPage
{

protected:
    DECLARE_DYNAMIC(CLogSystemPage)

public:
    CLogSystemPage();
    virtual ~CLogSystemPage();

public:

    // Close the main log system window.
    void Close();



    // Create the ribbon buttons
    BOOL CreateRibbonButtons();


protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnRibbonBtnRefresh();
    afx_msg void OnRibbonBtnPrint();

    afx_msg void OnUpdateRefresh(CCmdUI* pCmdUI);
    afx_msg void OnUpdatePrint(CCmdUI* pCmdUI);



    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();



protected:
    // Pane
    CLogListPane    m_wndLogList;
    CLogSearchPane  m_wndLogSearch;
    friend class CLogFilterCtrl;
};


