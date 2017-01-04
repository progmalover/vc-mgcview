#pragma once
#include "LogFilterCtrl.h"

// CLogSearchPane

class CLogSearchPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CLogSearchPane)

public:
    CLogSearchPane();
    virtual ~CLogSearchPane();
    CLogFilterCtrl m_LogFilter;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


