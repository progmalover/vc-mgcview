#pragma once
#include "BaseControllerPage.h"
#include "TimeTableCategoryCtrl.h"

class CTimeTableListPane : public CBaseControllerPane
{
	DECLARE_DYNAMIC(CTimeTableListPane)

public:
	CTimeTableListPane(void);
	virtual ~CTimeTableListPane(void);


    CTimeTableCategoryCtrl m_TimeTableListCtrl;

private:
 

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};


