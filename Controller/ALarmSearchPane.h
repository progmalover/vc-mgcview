#pragma once
#include "AlarmFilterCtrl.h"

// CAlarmSearchPane

class CAlarmSearchPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CAlarmSearchPane)

public:
    CAlarmSearchPane();
    virtual ~CAlarmSearchPane();
    CAlarmFilterCtrl m_AlarmFilter;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


