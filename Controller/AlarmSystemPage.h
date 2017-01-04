#pragma once

#include "AlarmListPane.h"
#include "AlarmSearchPane.h"

// CAlarmSystemPage

class CAlarmSystemPage : public CBaseControllerPage
{

protected:
    DECLARE_DYNAMIC(CAlarmSystemPage)

public:
    CAlarmSystemPage();
    virtual ~CAlarmSystemPage();

public:

    // Close the main Alarm system window.
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
    CAlarmListPane    m_wndAlarmList;
    CAlarmSearchPane  m_wndAlarmSearch;
    friend class CAlarmFilterCtrl;
};


