#pragma once
#include "BaseControllerPage.h"
#include "TimeTableLayoutPropertyCtrl.h"

class CTimeTableLayoutPropertyPane : public CBaseControllerPane
{
	DECLARE_DYNAMIC(CTimeTableLayoutPropertyPane)

public:
	CTimeTableLayoutPropertyPane(void);
	virtual ~CTimeTableLayoutPropertyPane(void);

public:
	CTimeTableLayoutPropertyCtrl* GetCtrl();

private:
	CTimeTableLayoutPropertyCtrl m_TimeTableLayoutPropertyCtrl;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};

inline CTimeTableLayoutPropertyCtrl* CTimeTableLayoutPropertyPane::GetCtrl()
{
	return &m_TimeTableLayoutPropertyCtrl;
}

