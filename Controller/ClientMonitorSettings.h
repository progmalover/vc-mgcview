#pragma once


// CClientMonitorSettings

class CClientMonitorSettings : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CClientMonitorSettings)

public:
	CClientMonitorSettings();
	virtual ~CClientMonitorSettings();

    // Dialog Data
    enum { IDD = IDD_CLIENT_MONITOR_SETTINGS };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected:
	DECLARE_MESSAGE_MAP()
    bool m_bInChangingDuration;

public:
    afx_msg void OnEnChangeRefreshDuration();
    CString m_nDuration;
};


