#pragma once

#include "BorrowSkin/BORROW_CalendarBar.h"
// CCalenderPane

class CCalenderPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CCalenderPane)

public:
    CCalenderPane();
    virtual ~CCalenderPane();

    CBORROW_Calendar* GetMonthCalCtrl(){return &m_wndMonthCal;}

public:
    CBORROW_Calendar       m_wndMonthCal;


protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


