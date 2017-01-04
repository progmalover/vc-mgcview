#pragma once

#include "Resource.h"
#include "ScheduleListTab.h"

// CSchedulerPane

class ScheduleCtrl;
class CSchedulerPane : public CBaseControllerPane
{
	DECLARE_DYNAMIC(CSchedulerPane)

public:
	CSchedulerPane();
	virtual ~CSchedulerPane();
 
public:
	ScheduleCtrl* GetScheduleCtrl();

protected:
    void InitializeWnd();

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()   

public:
	CScheduleListTab    m_scheduleListTab;
    
};

inline ScheduleCtrl* CSchedulerPane::GetScheduleCtrl()
{
	return m_scheduleListTab.GetScheduleCtrl();
}




