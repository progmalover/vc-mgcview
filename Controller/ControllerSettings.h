#pragma once
#include "DesignerGridSetting.h"
#include "CommonSetting.h"
#include "ClientMonitorSettings.h"
#include "Utilities/ConfigFile.h"
#include "LanguageSetting.h"

#define  ROOT_DIR  _T("Root Directory: ")
#define  CONTROLLER_SERVER_IP  _T("Controller Server IP Address: ")
#define  CONTROLLER_SERVER_PORT  _T("Controller Server Port: ")
#define  DESIGNER_GRID_X  _T("Designer Grid Width: ")
#define  DESIGNER_GRID_Y  _T("Designer Grid Height: ")
#define  DESIGNER_SNAP_GRID  _T("Snap to Grid: ")
#define  DESIGNER_GRID_SHOW  _T("Show Grid: ")
#define  CLIENT_MONITOR_REFRESH  _T("Client Monitor Refresh Rate (s): ")
#define  LANGUAGE_SETTING  _T("Language: ")

class ControllerConfig : public ConfigFile
{
public:
    ControllerConfig();
    ~ControllerConfig();

    BOOL LoadDefaults();
};


/////////////////////////////////////////////////////////////////////////////
// CControllerSettings

class CControllerSettings : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(CControllerSettings)

// Construction
public:
	CControllerSettings(ControllerConfig* pData, CWnd *pParent = NULL);

// Attributes
public:
    CDesignerGridSetting  m_DesignerGrid;
    CommonSetting         m_CommonSetting;
    CClientMonitorSettings m_ClientMonitorSettings;
	LanguageSetting        m_LanguageSetting;

private:
    ControllerConfig*      m_pDate;
// Operations
public:

    enum SETTING_PAGE{
        SETTING_PAGE_COMMON = 0,
        SETTING_PAGE_DESIGNER = 1,
        SETTING_PAGE_SCHEDULER = 2,
        SETTING_PAGE_CLIENT_MANAGER = 3,
        SETTING_PAGE_CLIENT_MONITOER = 4,
		SETTING_PAGE_LANGUAGE = 5,
		SETTING_PAGE_TIMETABLE = 6,
        SETTING_PAGE_CNT = 8
    };

// Overrides
	virtual void OnDrawPageHeader (CDC* pDC, int nPage, CRect rectHeader);

// Implementation
public:
	virtual ~CControllerSettings();

protected:

    afx_msg BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg void OnApply();
	virtual BOOL OnInitDialog();
    BOOL UpdateOptions();
	LRESULT OnGridYSpaceSettingInvalid(WPARAM wp, LPARAM lp);
	LRESULT OnGridXSpaceSettingInvalid(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()

};

