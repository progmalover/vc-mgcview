#pragma once
#include "BaseControllerPage.h"
#include "TimeTablePropertyEditCtrl.h"

class CTimeTablePropertyEditPane : public CBaseControllerPane
{
	DECLARE_DYNAMIC(CTimeTablePropertyEditPane)

public:
	CTimeTablePropertyEditPane(void);
	virtual ~CTimeTablePropertyEditPane(void);

public:
	CTimeTablePropertyEditCtrl* GetCtrl();

private:
	CTimeTablePropertyEditCtrl m_TimeTablePropertyEditCtrl;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};

inline CTimeTablePropertyEditCtrl* CTimeTablePropertyEditPane::GetCtrl()
{
	return &m_TimeTablePropertyEditCtrl;
}

