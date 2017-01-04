#include "stdafx.h"
#include "Controller.h"
#include "ControllerSettings.h"
#include <iostream>
#include <fstream>
#include <string>
#include <atlbase.h>
#include <atlconv.h>

#include "Client/ClientConfig.h"
#include "Client/ClientConfigFile.h"
#include "MediaServer/MediaServerConfig.h"
#include "ControllerServerConfiguer/ControllerServerConfig.h"
#include "Localization/Localization.h"
#include "Utilities/SysCall.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyPropertySheetoll

IMPLEMENT_DYNAMIC(CControllerSettings, CMFCPropertySheet)

CControllerSettings::CControllerSettings(ControllerConfig* pData, CWnd *pParent)
	 : CMFCPropertySheet(_T("Options"), pParent)
{
	SetLook (CMFCPropertySheet::PropSheetLook_List, 180);


    AddPage(&m_CommonSetting);
	AddPage(&m_DesignerGrid);

#if !defined(STARTER_EDITION) && !defined(STANDALONE_EDITION)

	AddPage(&m_ClientMonitorSettings);

#endif

    if (!GetControllerApp->m_bNoLang)
    {
        AddPage(&m_LanguageSetting);
    }


    m_pDate = pData;

    if (!m_pDate->ReLoad())
    {
        CString SettingFile = (GetControllerDefaultDir() + SETTING_FILE_NAME).c_str();
        if (PathFileExists(SettingFile))
        {
            m_pDate->Load(SettingFile);
        }
    }

    // Common Setting Init

    m_pDate->Get(ROOT_DIR, m_CommonSetting.m_RootWorkdirectory);
    m_pDate->Get(CONTROLLER_SERVER_IP, m_CommonSetting.m_strServerIP);
    m_pDate->Get(CONTROLLER_SERVER_PORT, m_CommonSetting.m_strPort);
#ifdef STARTER_EDITION
    ClientConfigFile ClientPlayConfig; // = ClientConfigFile::GetConfigFile();
    ClientPlayConfig.Load(GetClientDefaultDir() + CLIENT_CONFIG_FILE);
    // Server IP
    int rotatedegree = 0;
    ClientPlayConfig.Get(CLIENT_ROTATEDEGREE, rotatedegree);
    m_CommonSetting.m_nRotationSel = rotatedegree / 90;

	ClientPlayConfig.Get(CLIENT_SHORT_CUT, m_CommonSetting.m_iHotKey);
#endif
    // Designer Setting Init

    m_pDate->Get(DESIGNER_GRID_X, m_DesignerGrid.m_XSpace);
    m_pDate->Get(DESIGNER_GRID_Y, m_DesignerGrid.m_YSpace);

    CString strShowGrid;;
    m_pDate->Get(DESIGNER_GRID_SHOW, strShowGrid);
    strShowGrid.MakeUpper();
    m_DesignerGrid.m_bShowGrid = FALSE;
    if (strShowGrid == _T("TRUE"))
    {
        m_DesignerGrid.m_bShowGrid = TRUE;
    }
    CString strSnapGrid;
    m_pDate->Get(DESIGNER_SNAP_GRID, strSnapGrid);
    strSnapGrid.MakeUpper();
    m_DesignerGrid.m_bSnapToGrid = FALSE;
    if (strSnapGrid == _T("TRUE"))
    {
        m_DesignerGrid.m_bSnapToGrid = TRUE;
    }

    // Scheduler Setting Init

    // Client Manager Setting Init

    // Client Monitor Setting Init
    m_pDate->Get(CLIENT_MONITOR_REFRESH, m_ClientMonitorSettings.m_nDuration);

	//Language Setting Init
	m_pDate->Get(LANGUAGE_SETTING, m_LanguageSetting.m_SelLanguage);
	
}

CControllerSettings::~CControllerSettings()
{
}

static void GetAllIpAddresses(std::vector<sockaddr_storage>& ips)
{
    ADDRINFO Hints, *pAddrList, *pAddr;
    char szMySelf[128];
    struct sockaddr_storage stg_addr;

    ips.clear();

    // list all interfaces
    gethostname(szMySelf, sizeof(szMySelf));
    // some hints
    memset(&Hints, 0, sizeof(Hints));
    {
        Hints.ai_family = AF_INET;

        if (getaddrinfo(szMySelf, NULL, & Hints, &pAddrList) == 0)
        {
            for (pAddr = pAddrList ; pAddr !=NULL; pAddr=pAddr->ai_next)
            {
                memcpy(&stg_addr, pAddr->ai_addr, pAddr->ai_addrlen);
                ips.push_back(stg_addr);
            }
            freeaddrinfo(pAddrList);
        }
    }
#if 0
    if (sSettings.bIPv6)
    {
        // put IPv6 after 
        Hints.ai_family = AF_INET6;
        Rc = getaddrinfo (szMySelf, NULL, & Hints, & pAddrList);
        if (Rc == 0)
        {
            for ( pAddr = pAddrList ; 
                pAddr !=NULL && new_if.nb_addr<SizeOfTab(new_if.ent);  
                pAddr=pAddr->ai_next )
            {
                if (! IN6_IS_ADDR_LOOPBACK (& (* (struct sockaddr_in6 *) pAddr->ai_addr).sin6_addr) )
                    memcpy (& new_if.ent[new_if.nb_addr++].stg_addr, pAddr->ai_addr, pAddr->ai_addrlen );
            }
            freeaddrinfo (pAddrList);
        }
    } // getaddrinfo OK
    else 
        Rc = WSAGetLastError ();
#endif
}

BOOL CControllerSettings::OnInitDialog()
{
    CMFCPropertySheet::OnInitDialog();
	
	m_CommonSetting.SetModified(FALSE);
	m_DesignerGrid.SetModified(FALSE);
	m_ClientMonitorSettings.SetModified(FALSE);  
	 
	m_LanguageSetting.SetModified(FALSE);
	UpdateData(FALSE);
	Translate(this);

    GetDlgItem(IDOK)->SetWindowText(Translate(_T("OK")));
    GetDlgItem(IDCANCEL)->SetWindowText(Translate(_T("Cancel")));
    GetDlgItem(ID_APPLY_NOW)->SetWindowText(Translate(_T("Apply")));

    std::vector<sockaddr_storage> IpAddresses;
    GetAllIpAddresses(IpAddresses);
    return TRUE;  // return TRUE  unless you set the focus to a control
}
BEGIN_MESSAGE_MAP(CControllerSettings, CMFCPropertySheet)
    ON_WM_SYSCOMMAND()
    ON_COMMAND(ID_APPLY_NOW, OnApply)
	ON_REGISTERED_MESSAGE(GRID_YSPACESETTINGINVALID, OnGridYSpaceSettingInvalid)
	ON_REGISTERED_MESSAGE(GRID_XSPACESETTINGINVALID, OnGridXSpaceSettingInvalid)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMyPropertySheet message handlers


void CControllerSettings::OnDrawPageHeader (CDC* pDC, int nPage, CRect rectHeader)
{
	rectHeader.top += 2;
	rectHeader.right -= 2;
	rectHeader.bottom -= 2;

	pDC->FillRect (rectHeader, &afxGlobalData.brBtnFace);
	pDC->Draw3dRect (rectHeader, afxGlobalData.clrBtnShadow, afxGlobalData.clrBtnShadow);

	CDrawingManager dm (*pDC);
	dm.DrawShadow (rectHeader, 2);	

	CString strText;
	strText.Format (Translate(_T("Page %d description...")), nPage + 1);

	CRect rectText = rectHeader;
	rectText.DeflateRect (10, 0);

	CFont* pOldFont = pDC->SelectObject (&afxGlobalData.fontBold);
	pDC->SetBkMode (TRANSPARENT);
	pDC->SetTextColor (afxGlobalData.clrBtnText);

	pDC->DrawText (strText, rectText, DT_SINGLELINE | DT_VCENTER);

	pDC->SelectObject (pOldFont);
}

BOOL CControllerSettings::UpdateOptions()
{
	if (NULL != m_CommonSetting.GetSafeHwnd()/*this->GetActiveIndex() == this->FindPageIndexInList(&m_CommonSetting)*/)
	{
		//if (NULL != m_CommonSetting.GetSafeHwnd())
		{
			m_CommonSetting.UpdateData(TRUE);
        }

        CString path;
        m_CommonSetting.m_ctrlBrowser.GetWindowText(path);
        if (::PathFileExists(path))
        {
            m_pDate->Set(ROOT_DIR, path);
            m_pDate->Save();
        }
        else
        {
            MessageBox(Translate(_T("Failed to set Root Directory, it may not exsit!")),Translate(_T("Error:Update options")), MB_OK|MB_ICONERROR);
        }

#ifdef PROFESSIONAL_EDITION 

        BOOL bChanged = FALSE;

#ifndef STANDALONE_EDITION
        CString ServerIP;
        m_CommonSetting.m_ctrlServerIP.GetWindowText(ServerIP);
        if (m_pDate->Get(CONTROLLER_SERVER_IP) != ServerIP)
        {
            bChanged = TRUE;
            m_pDate->Set(CONTROLLER_SERVER_IP, ServerIP);
        }

        CString ServerPort;
        m_CommonSetting.m_ctrlPort.GetWindowText(ServerPort);
        if (m_pDate->Get(CONTROLLER_SERVER_PORT) != ServerPort)
        {
            bChanged = TRUE;
            m_pDate->Set(CONTROLLER_SERVER_PORT, ServerPort);
        }
#endif
        if (bChanged)
        {
            CString MediaServerFolder = GetConfig->Get(CONFIG_PATH);
            MediaServerFolder.Trim();
            if (MediaServerFolder.IsEmpty())
            {
                MediaServerFolder = GetMediaLibraryDefaultDir().c_str();
            }
            if ((MediaServerFolder.ReverseFind(_T('\\')) + 1) != MediaServerFolder.GetLength())
            {
                MediaServerFolder += _T("\\");
            }

            ControllerServerConfig CSConfig;
            CSConfig.LoadDefaults();
            CSConfig.Load((GetControllerServerDefaultDir() + CS_CONFIG_FILE).c_str());
#ifndef STANDALONE_EDITION
            CSConfig.Set(CS_MEDIA_SERVER_IP, ServerIP.GetString());
            CSConfig.Set(CS_PORT, ServerPort.GetString());
#endif
            CSConfig.Save();

            MediaServerConfig MSConfig;
            MSConfig.LoadDefaults();
            MSConfig.Load((GetMediaServerDefaultDir() + MS_CONFIG_FILE).c_str());
#ifndef STANDALONE_EDITION
            MSConfig.Set(MS_CONTROLLER_SERVER_IP, ServerIP.GetString());
#endif
            MSConfig.Save();

            m_pDate->Save();

            MessageBox(Translate(_T("Please Exit and Restart the Controller to make the change work because you have change the Server IP or Port!")), Translate(_T("Warning:Update options")),MB_OK|MB_ICONEXCLAMATION);
        }

#endif



#ifdef STARTER_EDITION 

        BOOL bChanged = FALSE;
        BOOL bAngleChanged = FALSE;

        ClientConfigFile ClientPlayConfig;// = ClientConfigFile::GetConfigFile();
        ClientPlayConfig.Load(GetClientDefaultDir() + CLIENT_CONFIG_FILE);

        INT Angle =  m_CommonSetting.m_ctrlRotationComboBox.GetCurSel();
        if ( ClientPlayConfig.GetN(CLIENT_ROTATEDEGREE) != Angle*90)
        {
            bAngleChanged = TRUE;
            ClientPlayConfig.Set(CLIENT_ROTATEDEGREE, Angle*90);     
        }
		WORD wVirtualKeyCode;
		WORD wModifiers;
		INT iHotKey = 0;

		m_CommonSetting.m_HotKey.GetHotKey(wVirtualKeyCode, wModifiers);
		iHotKey = (((wModifiers<<16) & 0XFFFF0000) | (wVirtualKeyCode & 0X0000FFFF));
	    if (ClientPlayConfig.GetN(CLIENT_SHORT_CUT) != iHotKey)
		{
			bChanged = TRUE;
			ClientPlayConfig.Set(CLIENT_SHORT_CUT, iHotKey);
		}

        if (bAngleChanged || bChanged)
        {
			ClientPlayConfig.Save();	
        }

		if (bAngleChanged)
        {
            GetControllerFrame->UpdateOptions(SETTING_PAGE_COMMON);
        }



#endif

        m_CommonSetting.SetModified(FALSE);

    }

    if (NULL != m_DesignerGrid.GetSafeHwnd()/*this->GetActiveIndex() == this->FindPageIndexInList(&m_DesignerGrid)*/)
    {
       // if (NULL != m_DesignerGrid.GetSafeHwnd())
        {
            m_DesignerGrid.UpdateData(TRUE);
        }


        INT ClipX = max(min(m_DesignerGrid.m_XSpace, 1920), 10);

        INT ClipY = max(min(m_DesignerGrid.m_YSpace, 1920), 10);

        if (m_DesignerGrid.m_XSpace != ClipX
            || m_DesignerGrid.m_YSpace != ClipY)
        {
            m_DesignerGrid.m_XSpace = ClipX;
            m_DesignerGrid.m_YSpace = ClipY;
            m_DesignerGrid.UpdateData(FALSE);
            m_DesignerGrid.UpdateData(FALSE);
        }
        else
        {
            m_pDate->Set(DESIGNER_GRID_X, m_DesignerGrid.m_XSpace);

            m_pDate->Set(DESIGNER_GRID_Y, m_DesignerGrid.m_YSpace);

            CString szShow = m_DesignerGrid.m_bShowGrid ? _T("TRUE") : _T("FALSE");
            m_pDate->Set(DESIGNER_GRID_SHOW, szShow);

            CString szSnap = m_DesignerGrid.m_bSnapToGrid ? _T("TRUE") : _T("FALSE");
            m_pDate->Set(DESIGNER_SNAP_GRID, szSnap);

            m_pDate->Save();


            GetControllerFrame->UpdateOptions(SETTING_PAGE_DESIGNER);          
        }
		m_DesignerGrid.SetModified(FALSE);
    }
    
	if (NULL != m_ClientMonitorSettings.GetSafeHwnd()/*this->GetActiveIndex() == this->FindPageIndexInList(&m_ClientMonitorSettings)*/)
    {
     //   if (NULL != m_ClientMonitorSettings.GetSafeHwnd())
        {
            m_ClientMonitorSettings.UpdateData(TRUE);
        }
		CString preSetting = m_pDate->Get(CLIENT_MONITOR_REFRESH);
        m_pDate->Set(CLIENT_MONITOR_REFRESH, m_ClientMonitorSettings.m_nDuration);

        m_pDate->Save();

        GetControllerFrame->UpdateOptions(SETTING_PAGE_CLIENT_MONITOER);
		if(preSetting !=  m_ClientMonitorSettings.m_nDuration)
			MessageBox(Translate(_T("The settings begin to take effect at next start!")),Translate(_T("Warning:Update options")), MB_OK|MB_ICONEXCLAMATION);
        m_ClientMonitorSettings.SetModified(FALSE);

    }

	if (NULL != m_LanguageSetting.GetSafeHwnd()/*this->GetActiveIndex() == this->FindPageIndexInList(&m_LanguageSetting)*/)
	{
		 
		if (NULL != m_LanguageSetting.GetSafeHwnd())
		{
		    m_LanguageSetting.UpdateData(TRUE);
		}
		CString preSetting = m_pDate->Get(LANGUAGE_SETTING);

		CString szLanguage = m_LanguageSetting.m_SelLanguage;
		//if (szLanguage == _T("中文"))
		//{
		//	szLanguage = _T("Chinese");
		//}
		//else if (szLanguage == _T("英文"))
		//{
		//	szLanguage = _T("English");
		//}
		//else
		//{
		//	//not convert
		//}

		m_pDate->Set(LANGUAGE_SETTING, szLanguage);
		m_pDate->Save();
		 
		if(preSetting !=  szLanguage)
		 MessageBox(Translate(_T("The selected language begin to take effect at next start!")),Translate(_T("Warning:Update options")), MB_OK|MB_ICONEXCLAMATION);
		m_LanguageSetting.SetModified(FALSE); 

	}


    return TRUE;
}

BOOL CControllerSettings::OnCommand(WPARAM wParam, LPARAM lParam)    
{  
    // TODO: Add your specialized code here and/or call the base class    
    if(LOWORD(wParam)==IDOK)  
    {
        CWnd* pWnd = (CWnd*)GetDlgItem(ID_APPLY_NOW);
        if(pWnd->IsWindowEnabled())
            UpdateOptions();
    }

    return CMFCPropertySheet::OnCommand(wParam, lParam);  
}  

void CControllerSettings::OnApply( )
{
    UpdateOptions();
	m_CommonSetting.SetModified(FALSE);
	m_DesignerGrid.SetModified(FALSE);
	m_ClientMonitorSettings.SetModified(FALSE);
	m_LanguageSetting.SetModified(FALSE);
}

LRESULT CControllerSettings::OnGridXSpaceSettingInvalid(WPARAM wp, LPARAM lp)
{
	if (GetFocus() == GetDlgItem(IDCANCEL))
	{
		return 1;
	}

	int XSpace = (int)wp;

	if (XSpace < 10 || XSpace > 1920)
	{
		MessageBox(Translate(_T("An integer between 10 and 1092 is required for this field.")),Translate(_T("Warning:Update options")), MB_OK|MB_ICONEXCLAMATION);
		m_DesignerGrid.m_XEdit.SetFocus();
	    return 1;
	}
	return 0;

}

LRESULT CControllerSettings::OnGridYSpaceSettingInvalid(WPARAM wp, LPARAM lp)
{
	if (GetFocus() == GetDlgItem(IDCANCEL))
	{
		return 1;
	}

	int YSpace = (int)wp;

	if (YSpace < 10 || YSpace > 1920)
	{
		MessageBox(Translate(_T("An integer between 10 and 1092 is required for this field.")), Translate(_T("Warning:Update options")),MB_OK|MB_ICONEXCLAMATION);
		m_DesignerGrid.m_YEdit.SetFocus();
		return 1;
	}
	return 0;

}

ControllerConfig::ControllerConfig()
{
    LoadDefaults();
}

ControllerConfig::~ControllerConfig()
{

}

BOOL ControllerConfig::LoadDefaults()
{
    Set(ROOT_DIR, _T("c:\\"));
    Set(CONTROLLER_SERVER_IP, _T("127.0.0.1"));
    Set(CONTROLLER_SERVER_PORT, _T("2000"));
    Set(DESIGNER_GRID_X, _T("100"));
    Set(DESIGNER_GRID_Y, _T("100"));
    Set(DESIGNER_SNAP_GRID, _T("TRUE"));
    Set(DESIGNER_GRID_SHOW, _T("TRUE"));
    Set(CLIENT_MONITOR_REFRESH, _T("10"));
	Set(LANGUAGE_SETTING, _T("English"));

    return TRUE;
}

