#pragma once
#include "BaseControllerPage.h"
#include "TimeTableManageCtrl.h"

class CTimeTableManagePane : public CBaseControllerPane
{
	DECLARE_DYNAMIC(CTimeTableManagePane)

public:
	CTimeTableManagePane(void);
	virtual ~CTimeTableManagePane(void);

public:
	CTimeTableManageCtrl* GetCtrl();

private:
	CTimeTableManageCtrl m_TimeTableManageCtrl;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};

inline CTimeTableManageCtrl* CTimeTableManagePane::GetCtrl()
{
	return &m_TimeTableManageCtrl;
}

