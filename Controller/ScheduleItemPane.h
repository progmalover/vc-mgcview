#pragma once
#include "ScheduleItemCtrl.h"

// CScheduleItemPane

class CScheduleItemPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CScheduleItemPane)

public:
    CScheduleItemPane();
    virtual ~CScheduleItemPane();

    CScheduleItemCtrl* GetScheduleItemCtrl(){return &m_ScheduleItem;}

private:
    CScheduleItemCtrl m_ScheduleItem;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


