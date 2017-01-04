#pragma once

#include "ReportCltStatusDlg.h"
#include "ReportCltDldDlg.h"
#include "ReportCltPlayDlg.h"
#include "ReportCltPlaySumDlg.h"
#include "ReportCltDetailDlg.h"
#include "ReportCltAvailDlg.h"
#include "ReportCltOnlineDlg.h"


class CReportListTab : public CMFCTabCtrl
{
 	DECLARE_DYNAMIC(CReportListTab)
	
public:
    CReportListTab();
    virtual ~CReportListTab();
	
    void ClearList();
//   void SetLogList(const std::vector<Log> & logList);

    CReportCltStatusDlg              m_ReportCltStatusCtrl; 
    CReportCltDldDlg                 m_ReportCltDldCtrl;
    CReportCltPlayDlg                m_ReportCltPlayCtrl;
    CReportCltPlaySumDlg             m_ReportCltPlaySumCtrl;
    CReportCltDetailDlg              m_ReportCltDetailCtrl;
    CReportCltAvailDlg               m_ReportCltAvailCtrl;
    CReportCltOnlineDlg              m_ReportCltOnlineCtrl;
	
	Condition    m_CurCondition;
	
    CString FormatSizeString(DWORD64 Size);

	enum{
        CLIENT_INDEX = 0,
    	CLIENT_NAME,
        GROUP_NAME,
		CLIENT_STATUS,
		CLIENT_REPORT_TIME,
		DOWN_FILE_NAME,
		DOWN_RESULT_STATUS,
		DOWN_REPORT_TIME,
		PLAY_TASK_NAME,
        PLAY_TASK_TYPE,
		PLAY_REGION_NAME,
		PLAY_TIMES,
		PLAY_START_TIME,
		PLAY_END_TIME,
		PLAY_REPORT_TIME,
		PLAY_TASK_ID, //for Client play sum tab
		SUM_TASK_PLAY_TIMES,
		DISK_REMAIN_CAPACITY,
		RELATED_LED_NUM,
        LINE_NUM,
        BUSINESS_PROPERTY,
        CLIENT_GROUP_ID,
        CLIENT_GROUP_NAME,
		REMAIN_RATE,
		MONITOR_TIME,
		ONLINE_STATUS,
		ONLINE_RATE,		
    };

    enum REPORT_SYS_TAB
    {
        CLIENT_STATUS_QUERY = 0,
        CLIENT_DOWNLOAD_QUERY,
        CLIENT_PLAY_QUERY,
        CLIENT_PLAY_SUM_QUERY,
        CLIENT_DETAILS_QUERY,
        CLIENT_AVAILABILITY_QUERY,
        CLINET_ONLINE_RATE,
        CLIENT_MAX_TAB = 15,
    };

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	 DECLARE_MESSAGE_MAP()	
};

////////////////////////////////////////////////////////////////////////////////////
// CReportListPane
class CReportListPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CReportListPane)

public:
    CReportListPane();
    virtual ~CReportListPane();

	CReportListTab 		m_wndStateTab;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};
