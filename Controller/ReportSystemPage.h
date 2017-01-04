#pragma once

#include "ReportListPane.h"

//need add outlookpane

// CReportSystemPage

class CReportSystemPage : public CBaseControllerPage
{

protected:
    DECLARE_DYNAMIC(CReportSystemPage)

public:
    CReportSystemPage();
    virtual ~CReportSystemPage();

public:

    // Close the main report system window.
    void Close();

    // Create the ribbon buttons
    BOOL CreateRibbonButtons();


protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);


    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();



protected:
    // Pane
    CReportListPane    		m_wndReportList;
 	CMFCOutlookBarPane 		m_wndReportOutlook;
    CMFCOutlookBar          m_ReportOutlookBar;

};


