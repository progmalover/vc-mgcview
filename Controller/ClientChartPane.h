#pragma once
#include "ClientChartDlg.h"

// CClientChartPane

class CClientChartPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CClientChartPane)

public:
    CClientChartPane(int clientid);
    virtual ~CClientChartPane();

    CClientChartDlg m_wndClientChart;
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()
private:
	int m_clientid;
public:
    afx_msg void OnDestroy();

};


