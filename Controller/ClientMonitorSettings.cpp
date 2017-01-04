// ClientMonitorSettings.cpp : 实现文件
//

#include "stdafx.h"
#include "Controller.h"
#include "ClientMonitorSettings.h"
#include "Localization/Localization.h"

// CClientMonitorSettings

IMPLEMENT_DYNAMIC(CClientMonitorSettings, CMFCPropertyPage)

CClientMonitorSettings::CClientMonitorSettings()
    : CMFCPropertyPage(CClientMonitorSettings::IDD)
    , m_nDuration(_T(""))
{
    m_bInChangingDuration = false;
}

CClientMonitorSettings::~CClientMonitorSettings()
{
}

BOOL CClientMonitorSettings::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	UpdateData(FALSE);

	Translate(this);
	return TRUE;  
}

BEGIN_MESSAGE_MAP(CClientMonitorSettings, CMFCPropertyPage)
    ON_EN_CHANGE(IDC_REFRESH_DURATION, &CClientMonitorSettings::OnEnChangeRefreshDuration)
END_MESSAGE_MAP()

void CClientMonitorSettings::DoDataExchange( CDataExchange* pDX )
{
    CMFCPropertyPage::DoDataExchange(pDX);

    DDX_Text(pDX, IDC_REFRESH_DURATION, m_nDuration);
}



// CClientMonitorSettings 消息处理程序

void CClientMonitorSettings::OnEnChangeRefreshDuration()
{
    if (!m_bInChangingDuration)
    {
        m_bInChangingDuration = true;

        CString s;
        GetDlgItem(IDC_REFRESH_DURATION)->GetWindowText(s);
        int duration = _tstoi(s.GetString());
        if (duration > 3600)
        {
            m_nDuration = _T("3600");
            GetDlgItem(IDC_REFRESH_DURATION)->SetWindowText(_T("3600"));
        }
        else if (duration < 1)
        {
            m_nDuration = _T("1");
            GetDlgItem(IDC_REFRESH_DURATION)->SetWindowText(_T("1"));
        }
        m_bInChangingDuration = false;
    }

    SetModified();
}
