#include "stdafx.h"
#include "Controller.h"
#include "Resource.h"
#include "Localization/Localization.h"
#include "LayoutSettings.h"
#include "LayoutManagePage.h"
#include "LayoutRenameCtrl.h"
#include "PlayerController.h"
#include "PreviewDownLoadDlg.h"
#include "RejectReason.h"

#include "Utilities/SysCall.h"

#include "LayoutFileUtility.h"

UINT LAYOUT_OPEN			        = ::RegisterWindowMessage (_T("LAYOUT_OPEN"));
UINT LAYOUT_NEW			            = ::RegisterWindowMessage (_T("LAYOUT_NEW"));
UINT LAYOUT_IMPORT  	            = ::RegisterWindowMessage (_T("LAYOUT_IMPORT"));


CLayoutManagePage::CLayoutManagePage(void)
{
    m_WorkingCategoryID = -2;
    nTry = 0;
}


CLayoutManagePage::~CLayoutManagePage(void)
{
}


BEGIN_MESSAGE_MAP(CLayoutManagePage, CBaseControllerPage)
	ON_WM_CREATE()
	ON_WM_DESTROY()

    ON_REGISTERED_MESSAGE(LAYOUT_LIST_CATEGORY, OnUpdateLayoutCategoryList)
    ON_REGISTERED_MESSAGE(LAYOUT_LIST_FILE, OnLayoutSelected)
    ON_REGISTERED_MESSAGE(LAYOUT_LIST_REFRESH, OnLayoutListRefresh)
    ON_REGISTERED_MESSAGE(LAYOUT_LIST_SEARCH, OnLayoutSearch)
    ON_REGISTERED_MESSAGE(LAYOUT_SETACTIVE_TAB, OnLayoutSetActiveTab)
	ON_REGISTERED_MESSAGE(LAYOUTDLG_SEARCH, OnLayoutDlgSearch)
    ON_COMMAND(ID_RINBON_BTN_LAYOUT_APPROVE, &CLayoutManagePage::OnRinbonBtnLayoutApprove)
    ON_COMMAND(ID_RINBON_BTN_LAYOUT_DELETE, &CLayoutManagePage::OnRinbonBtnLayoutDelete)
    ON_COMMAND(ID_RINBON_BTN_LAYOUT_EDIT, &CLayoutManagePage::OnRinbonBtnLayoutEdit)
    ON_COMMAND(ID_RINBON_BTN_LAYOUT_NEW, &CLayoutManagePage::OnRinbonBtnLayoutNew)
    ON_COMMAND(ID_RINBON_BTN_LAYOUT_IMPORT, &CLayoutManagePage::OnRinbonBtnLayoutImport)
    ON_COMMAND(ID_RINBON_BTN_LAYOUT_PREVIEW, &CLayoutManagePage::OnRinbonBtnLayoutPreview)
    ON_COMMAND(ID_RINBON_BTN_LAYOUT_REFRESH, &CLayoutManagePage::OnRinbonBtnLayoutRefresh)
    ON_COMMAND(ID_RINBON_BTN_LAYOUT_REJECT, &CLayoutManagePage::OnRinbonBtnLayoutReject)
    ON_COMMAND(ID_RINBON_BTN_LAYOUT_RENAME, &CLayoutManagePage::OnRinbonBtnLayoutRename)
 //   ON_COMMAND(ID_RINBON_BTN_LAYOUT_REQUEST, &CLayoutManagePage::OnRinbonBtnLayoutRequest)
    ON_COMMAND(ID_RIBBON_BTN_PREVIEW_10X7, OnRibbonBtn_Preview10X7)
    ON_COMMAND(ID_RIBBON_BTN_PREVIEW_12X10, OnRibbonBtn_Preview12X10)
    ON_COMMAND(ID_RIBBON_BTN_PREVIEW_19X10, OnRibbonBtn_Preview19X12)


   // ON_UPDATE_COMMAND_UI(ID_RINBON_BTN_LAYOUT_REQUEST, &CLayoutManagePage::OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RINBON_BTN_LAYOUT_RENAME, &CLayoutManagePage::OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RINBON_BTN_LAYOUT_REJECT, &CLayoutManagePage::OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_PREVIEW_10X7, &CLayoutManagePage::OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_PREVIEW_12X10, &CLayoutManagePage::OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_PREVIEW_19X10, &CLayoutManagePage::OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RINBON_BTN_LAYOUT_PREVIEW, &CLayoutManagePage::OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RINBON_BTN_LAYOUT_NEW, &CLayoutManagePage::OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RINBON_BTN_LAYOUT_IMPORT, &CLayoutManagePage::OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RINBON_BTN_LAYOUT_EDIT, &CLayoutManagePage::OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RINBON_BTN_LAYOUT_DELETE, &CLayoutManagePage::OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RINBON_BTN_LAYOUT_APPROVE, &CLayoutManagePage::OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RINBON_BTN_LAYOUT_REFRESH, &CLayoutManagePage::OnUpdateRibbonBtn)


	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CLayoutManagePage, CBaseControllerPage)
int CLayoutManagePage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!CreateRibbonButtons())
	{
		TRACE0("Failed to create schedule ribbon buttons\n");
		return -1;      // fail to create
	}

	if (CBaseControllerPage::OnCreate(lpCreateStruct) == -1)
		return -1;

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);




    CRect rect;
    GetClientRect(&rect);

    int left    = 0;
    int right   = 0;
    int top     = 0;
    int bottom  = 0;

    // Create and Install the Layout List pane
    left    = rect.Width() / 4;
    right   = rect.right;
    bottom  = rect.bottom;

    if (!m_wndLayoutList.Create(Translate(_T("Layout List")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_LAYOUTLISTVIEW, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create Layout List pane\n");
        return -1;      // fail to create
    }

    SetMainWnd(&m_wndLayoutList);

/*
    // Create and Install the MediaLibrary filter pane
    left    = 0;
    top     = 0;
    bottom  = 60;

    if (!m_wndLayoutSearch.Create(Translate(_T("Layout Search Bar")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_MEDIALIBRARY_SEARCHBAR, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, NULL))
    {
        TRACE0("Failed to create Layout filter pane\n");
        return FALSE;      // fail to create
    }
    m_wndLayoutSearch.EnableDocking(CBRS_TOP);
    DockPane(&m_wndLayoutSearch);
*/

    // Create and Install the Layout category pane
    left    = 0;
    right   = rect.Width() / 4;
    bottom  = rect.bottom;

    if (!m_wndLayoutCategory.Create(Translate(_T("Layout Category Tree")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_LAYOUT_CATEGORY, WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create Layout Category Tree\n");
        return FALSE;      // fail to create
    }
    m_wndLayoutCategory.EnableDocking(CBRS_LEFT);
    DockPane(&m_wndLayoutCategory);



    // Create and Install the Layout Info pane
    left    = rect.Width() / 4;
    top     = rect.bottom - 100;
    bottom  = rect.bottom;
    right   = rect.right;

    if (!m_wndLayoutInfo.Create(Translate(_T("Layout Info")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_LAYOUT_INFO, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, NULL))
    {
        TRACE0("Failed to create Layout info pane\n");
        return FALSE;      // fail to create
    }
    m_wndLayoutInfo.EnableDocking(CBRS_BOTTOM);
    DockPane(&m_wndLayoutInfo);
	return 0;
}

void CLayoutManagePage::OnDestroy()
{
	CBaseControllerPage::OnDestroy();
}

BOOL CLayoutManagePage::CreateRibbonButtons()
{
    if (!m_pCategory)
    {
        return FALSE;
    }

    /// Remove the dummy panel, and create the real panels
    m_pCategory->RemovePanel(0);

	CString strPanelFile			    = Translate(_T("File"));
	CString strPanelApproval			= Translate(_T("Approval"));
	CString strPanelPreview             = Translate(_T("Preview"));
	CString strPanelView			    = Translate(_T("View"));


    CMFCRibbonPanel* pPanelFile = m_pCategory->AddPanel(strPanelFile);
	CMFCRibbonButton* pBtnNewFile = new CMFCRibbonButton(ID_RINBON_BTN_LAYOUT_NEW,  Translate(_T("New")),0, 0);
	pBtnNewFile->SetAlwaysLargeImage();
	pBtnNewFile->SetToolTipText(Translate(_T("New")));
    pBtnNewFile->SetDescription(Translate(_T("Create a new layout in selected layout category.")));
	pPanelFile->Add(pBtnNewFile);

	CMFCRibbonButton* pBtnDelete= new CMFCRibbonButton(ID_RINBON_BTN_LAYOUT_DELETE,  Translate(_T("Delete")),1, 1);
	pBtnDelete->SetAlwaysLargeImage();
	pBtnDelete->SetToolTipText(Translate(_T("Delete")));
    pBtnDelete->SetDescription(Translate(_T("Delete the selected layout.")));
	pPanelFile->Add(pBtnDelete);

	CMFCRibbonButton* pBtnRename = new CMFCRibbonButton(ID_RINBON_BTN_LAYOUT_RENAME,  Translate(_T("Rename")),2, 2);
	pBtnRename->SetAlwaysLargeImage();
	pBtnRename->SetToolTipText(Translate(_T("Rename")));
    pBtnRename->SetDescription(Translate(_T("Rename the selected layout.")));
	pPanelFile->Add(pBtnRename);

	CMFCRibbonButton* pBtnEdit = new CMFCRibbonButton(ID_RINBON_BTN_LAYOUT_EDIT,  Translate(_T("Edit")),3, 3);
	pBtnEdit->SetAlwaysLargeImage();
	pBtnEdit->SetToolTipText(Translate(_T("Edit")));
    pBtnEdit->SetDescription(Translate(_T("Edit the selected layout.")));
	pPanelFile->Add(pBtnEdit);

    CMFCRibbonButton* pBtnImport = new CMFCRibbonButton(ID_RINBON_BTN_LAYOUT_IMPORT,  Translate(_T("Import")), 9, 9);
    pBtnImport->SetAlwaysLargeImage();
    pBtnImport->SetToolTipText(Translate(_T("Import")));
    pBtnImport->SetDescription(Translate(_T("Import a layout from local.")));
    pPanelFile->Add(pBtnImport);


#ifdef  SERVER_EDITION

	CMFCRibbonPanel* pPanelApproval = m_pCategory->AddPanel(strPanelApproval);
	CMFCRibbonButton* pBtnApprove = new CMFCRibbonButton(ID_RINBON_BTN_LAYOUT_APPROVE,  Translate(_T("Approve")),4, 4);
	pBtnApprove->SetAlwaysLargeImage();
	pBtnApprove->SetToolTipText(Translate(_T("Approve")));
    pBtnApprove->SetDescription(Translate(_T("Approve layout to publish.")));
	pPanelApproval->Add(pBtnApprove);

	CMFCRibbonButton* pBtnReject = new CMFCRibbonButton(ID_RINBON_BTN_LAYOUT_REJECT,  Translate(_T("Reject")),5, 5);
	pBtnReject->SetAlwaysLargeImage();
	pBtnReject->SetToolTipText(Translate(_T("Reject")));
    pBtnReject->SetDescription(Translate(_T("Reject layout to publish.")));
	pPanelApproval->Add(pBtnReject);

    /*
	CMFCRibbonButton* pBtnRequest = new CMFCRibbonButton(ID_RINBON_BTN_LAYOUT_REQUEST,  Translate(_T("Request")),6, 6);
	pBtnRequest->SetAlwaysLargeImage();
	pBtnRequest->SetToolTipText(Translate(_T("Request")) + _T("\n") + Translate(_T("Request Layout")));
	pPanelApproval->Add(pBtnRequest);
    */
#endif


    CMFCRibbonPanel* pPanelPreview = m_pCategory->AddPanel(strPanelPreview);
	CMFCRibbonButton* pBtnPreview = new CMFCRibbonButton(ID_RINBON_BTN_LAYOUT_PREVIEW,  Translate(_T("Preview")), 7, 7);
	pBtnPreview->SetAlwaysLargeImage();
	pBtnPreview->SetToolTipText(Translate(_T("Preview")));
    pBtnPreview->SetDescription(Translate(_T("Preview layout in selected dimension.")));
    pBtnPreview->AddSubItem(new CMFCRibbonButton(ID_RIBBON_BTN_PREVIEW_10X7, _T("1024 x 768")));
    pBtnPreview->AddSubItem(new CMFCRibbonButton(ID_RIBBON_BTN_PREVIEW_12X10, _T("1280 x 1024")));
    pBtnPreview->AddSubItem(new CMFCRibbonButton(ID_RIBBON_BTN_PREVIEW_19X10, _T("1920 x 1080")));
    pPanelPreview->Add(pBtnPreview);

    CMFCRibbonPanel* pPanelRefresh = m_pCategory->AddPanel(strPanelView);

    CMFCRibbonButton* pBtnRefresh = new CMFCRibbonButton(ID_RINBON_BTN_LAYOUT_REFRESH, Translate(_T("Refresh")), 8, 8);
    pBtnRefresh->SetAlwaysLargeImage();
	pBtnRefresh->SetToolTipText(Translate(_T("Refresh")));
    pBtnRefresh->SetDescription(Translate(_T("Refresh layout Status.")));
    pPanelRefresh->Add(pBtnRefresh);

    return TRUE;
}

BOOL CLayoutManagePage::UpdateOptions()
{
	return TRUE;
}

void CLayoutManagePage::Close()
{

}

void CLayoutManagePage::OnRinbonBtnLayoutApprove()
{
    HRESULT hr = S_OK;
    std::vector<Layout> LayoutList;

    hr = m_wndLayoutList.m_wndLayoutListTab.GetSelectedLayoutList(LayoutList);

    auto it = LayoutList.begin(), ite = LayoutList.end();
    Audit tmpAudit;
    for (; it != ite; ++it)
    {
        Layout layout;
        HRESULT hr = GetControllerInterface->GetLayout(it->GetLayoutID(),layout);
        if (!SUCCEEDED(hr))
        {
            MessageBox(Translate(_T("Failed to get the selected layout.")), Translate(_T("Error:Layout manager page")), MB_ICONERROR);
            return;
        }

        tmpAudit = layout.GetAudit();
        Audit tmp(tmpAudit.GetRequestUserID(), tmpAudit.GetRequestTime(), GetControllerInterface->GetUserId(),
            S3Time::Now(), tmpAudit.CovertStatus(Audit::APPROVED));
        layout.SetAudit(tmp);
        // modify controllerserver data
        //hr = GetControllerInterface->EditLayout(layout);
        hr = GetControllerInterface->AuditLayout(layout.GetLayoutID(), layout.GetAudit());
        if (!SUCCEEDED(hr))
        {
            MessageBox(Translate(_T("Failed to Approve the layout!")), Translate(_T("Error:Layout manager page")), MB_ICONERROR);

            return;
        }
    }
	if (m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() == 0x00) //layout list
	{
		m_wndLayoutList.m_wndLayoutListTab.m_grid.RefreshPartGrid();
	}
	else
	{
		m_wndLayoutList.m_wndLayoutListTab.Refresh();
	}
}


void CLayoutManagePage::OnRinbonBtnLayoutDelete()
{
    // TODO: Add your control notification handler code here
    HRESULT hr = S_OK;

    std::vector<Layout> LayoutList;

    m_wndLayoutList.m_wndLayoutListTab.GetSelectedLayoutList(LayoutList);

    if (LayoutList.empty())
    {
        return;
    }

	CString szContent, layoutStr;
	for(int i = 0;i < LayoutList.size();i++)
	{
		CString strPrompt ;
		strPrompt.Format(_T("\n\"%s\""), LayoutList[i].GetLayoutName().c_str());
		layoutStr += strPrompt;
	}
	
	layoutStr += _T("?");
	szContent = Translate(_T("Confirm to delete the layout:")) + layoutStr;
	if (MessageBox(szContent, Translate(_T("Delete")), MB_OKCANCEL) == IDCANCEL)
	{
			return;
	}

	CWaitCursor waitCursor;
	for(int i = 0;i < LayoutList.size();i++)
	{
		Layout WorkingLayout = LayoutList[i];

		S3Time nearestRefTime = S3Time::NULLTime;
		std::vector<int> refScheduleItemIDList;
		std::vector<int> ClientGroupIDList;
		std::vector<int> refTimetableIDList;

		// TODO just add sample code by Tod
		hr = GetControllerInterface->CheckLayoutRef(WorkingLayout.GetLayoutID(), nearestRefTime, refScheduleItemIDList, ClientGroupIDList , refTimetableIDList);
		if (!SUCCEEDED(hr))
		{
			if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
			{
				GetControllerApp->OnControllerServerOffline();
			}
			else
			{
				CString strPrompt = Translate(_T("Error:"));
				strPrompt.AppendFormat(_T("\"%s\""), LayoutList[i].GetLayoutName().c_str());
				MessageBox(Translate(_T("Failed to delete the layout, because can not get the layout's information.")), strPrompt, MB_ICONERROR);
			}
			continue;
		}

		if (refScheduleItemIDList.size() != 0 || ClientGroupIDList.size() != 0 || refTimetableIDList.size() != 0 )
		{
			CString strPrompt = Translate(_T("Warning:"));
			strPrompt.AppendFormat(_T("\"%s\""), LayoutList[i].GetLayoutName().c_str());
			MessageBox(Translate(_T("The layout is in schedule list, can not delete the layout!")),
				strPrompt, MB_ICONEXCLAMATION);
			continue;
		}

		hr = GetControllerInterface->DeleteScheduleItem(refScheduleItemIDList);
		if (!SUCCEEDED(hr))
		{
			if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
			{
				GetControllerApp->OnControllerServerOffline();
			}
			else
			{
				CString strPrompt = Translate(_T("Error:"));
				strPrompt.AppendFormat(_T("\"%s\""), LayoutList[i].GetLayoutName().c_str());
				MessageBox(Translate(_T("Failed to delete the schedule!")),strPrompt, MB_OK|MB_ICONERROR);
			}
			continue;
		}

		hr = GetControllerInterface->DeleteLayout(WorkingLayout.GetLayoutID());
		if (!SUCCEEDED(hr))
		{
			if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
			{
				GetControllerApp->OnControllerServerOffline();
			}
			else
			{
				//MessageBox(Translate(_T("Failed to delete layout!")), Translate(_T("Error")), MB_ICONERROR);
				CString strPrompt = Translate(_T("Error:"));
				strPrompt.AppendFormat(_T("\"%s\""), LayoutList[i].GetLayoutName().c_str());
				MessageBox(Translate(_T("Failed to delete layout, maybe the layout is in timetable list!")), strPrompt, MB_ICONERROR);
			}
			continue;
		}

		CString MediaServerFolder = WorkingLayout.GetLocation().c_str();

		// Delete the files in media server
		if (!GetMediaLibraryInterface->DeleteServerFolderAndFilesImm(MediaServerFolder.GetString()))
		{
			CString strPrompt = Translate(_T("Error: Edit layout"));
			strPrompt.AppendFormat(_T(" \"%s\""), LayoutList[i].GetLayoutName().c_str());
			MessageBox(Translate(_T("Failed to Clear the uploaded files in the media server")),strPrompt, MB_OK|MB_ICONERROR);
		}
	}
	if (m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() == 0x00) //layout list
	{
		m_wndLayoutList.m_wndLayoutListTab.m_grid.RefreshLayoutGrid();
	}
	else
	{
		m_wndLayoutList.m_wndLayoutListTab.Refresh();
	}
}


void CLayoutManagePage::OnRinbonBtnLayoutEdit()
{
    std::vector<Layout> LayoutList;

    m_wndLayoutList.m_wndLayoutListTab.GetSelectedLayoutList(LayoutList);

    if(LayoutList.size() > 0)
    {
        Layout WorkingLayout = LayoutList[0];

        GetParent()->SendMessage(LAYOUT_OPEN, (WPARAM)WorkingLayout.GetLayoutName().c_str(),(LPARAM) &WorkingLayout);
    }

}


void CLayoutManagePage::OnRinbonBtnLayoutNew()
{
	LayoutGroup Category;
    nTry++;
    HRESULT hr = GetControllerInterface->GetLayoutFilter(m_WorkingCategoryID, Category);
    if (!SUCCEEDED(hr))
    {
        MessageBox(Translate(_T("The selected category is deleted, please refresh the category!")), Translate(_T("Warning:Layout manager page")), MB_OK|MB_ICONEXCLAMATION);
        return;
    }
    GetParent()->SendMessage(LAYOUT_NEW, nTry, m_WorkingCategoryID);

}

void CLayoutManagePage::OnRinbonBtnLayoutImport()
{
    GetParent()->SendMessage(LAYOUT_IMPORT, NULL, m_WorkingCategoryID);
}


void CLayoutManagePage::OnRinbonBtnLayoutPreview()
{
    PreviewLayout(800, 600);
}

void CLayoutManagePage::OnRibbonBtn_Preview10X7()
{
    PreviewLayout(1024, 768);
}

void CLayoutManagePage::OnRibbonBtn_Preview12X10()
{
    PreviewLayout(1280, 1024);
}

void CLayoutManagePage::OnRibbonBtn_Preview19X12()
{
    PreviewLayout(1920, 1080);
}

void CLayoutManagePage::OnRinbonBtnLayoutRefresh()
{
	if (m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() == 0x00) //layout list
	{
		m_wndLayoutList.m_wndLayoutListTab.m_grid.RefreshLayoutGrid();
	}
	else
	{
		m_wndLayoutList.m_wndLayoutListTab.Refresh();
	}
}


void CLayoutManagePage::OnRinbonBtnLayoutReject()
{
    HRESULT hr = S_OK;
    std::vector<Layout> LayoutList;

    hr = m_wndLayoutList.m_wndLayoutListTab.GetSelectedLayoutList(LayoutList);

    CRejectReason dlg(this);

    if (IDOK == dlg.DoModal())
    {
        UpdateData(TRUE);

        auto it = LayoutList.begin(), ite = LayoutList.end();
        Audit tmpAudit;
        for (; it != ite; ++it)
        {
            Layout layout;
            HRESULT hr = GetControllerInterface->GetLayout(it->GetLayoutID(),layout);
            if (!SUCCEEDED(hr))
            {
                MessageBox(Translate(_T("Failed to get the selected layout.")), Translate(_T("Error:Layout manager page")), MB_ICONERROR);
                return;
            }

            tmpAudit = layout.GetAudit();
            Audit tmp(tmpAudit.GetRequestUserID(), tmpAudit.GetRequestTime(), GetControllerInterface->GetUserId(),
                S3Time::Now(), tmpAudit.CovertStatus(Audit::REJECTED));

            tmp.SetComment(dlg.m_RejectReason.GetString());
            layout.SetAudit(tmp);
            // modify controllerserver data
            //hr = GetControllerInterface->EditLayout(layout);
            hr = GetControllerInterface->AuditLayout(layout.GetLayoutID(), layout.GetAudit());
            if (!SUCCEEDED(hr))
            {
                MessageBox(Translate(_T("Failed to reject the layout!")), Translate(_T("Error:Layout manager page")), MB_ICONERROR);
                return;
            }
        }
		if (m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() == 0x00) //layout list
		{
			m_wndLayoutList.m_wndLayoutListTab.m_grid.RefreshPartGrid();
		}
		else
		{
			m_wndLayoutList.m_wndLayoutListTab.Refresh();
		}
    }


}


void CLayoutManagePage::OnRinbonBtnLayoutRename()
{
    HRESULT hr = S_OK;
    CString NewName;

    CLayoutRenameCtrl renameDlg(this);
    if (renameDlg.DoModal() == IDOK)
    {
        renameDlg.GetLayoutNewName(NewName);
    }
    else
    {
        return;
    }

    std::vector<Layout> LayoutList;

    m_wndLayoutList.m_wndLayoutListTab.GetSelectedLayoutList(LayoutList);

    Layout WorkingLayout = LayoutList[0];

    S3Time nearestRefTime = S3Time::NULLTime;
    std::vector<int> refScheduleItemIDList;
	std::vector<int> ClientGroupIDList;
	std::vector<int> refTimetableIDList;
    hr = GetControllerInterface->CheckLayoutRef(WorkingLayout.GetLayoutID(), nearestRefTime, refScheduleItemIDList, ClientGroupIDList , refTimetableIDList);
    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
        }
        else
        {
            MessageBox(Translate(_T("Failed to rename the layout, because can not get the layout's information..")), Translate(_T("Error:Rename layout")), MB_ICONERROR);
        }
        return;
    }

    if (refScheduleItemIDList.size() != 0 || ClientGroupIDList.size() != 0  || refScheduleItemIDList.size() != 0 )
    {
        MessageBox(Translate(_T("The layout is in schedule list, can not rename it!")),
            Translate(_T("Error:Rename layout")), MB_ICONERROR);
        return;
    }

    hr = GetControllerInterface->GetLayout(WorkingLayout.GetLayoutID(),WorkingLayout);
    if (!SUCCEEDED(hr))
    {
        MessageBox(Translate(_T("Failed to get the selected layout.")), Translate(_T("Error:Layout manager page")), MB_ICONERROR);
        return;
    }

    WorkingLayout.SetLayoutName(NewName.GetString());
    hr = GetControllerInterface->EditLayout(WorkingLayout);
    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
        }
        else
        {
            MessageBox(Translate(_T("Failed to rename the layout.")), Translate(_T("Error:Rename layout")), MB_ICONERROR);
        }
        return;
    }
	if (m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() == 0x00) //layout list
	{
		m_wndLayoutList.m_wndLayoutListTab.m_grid.RefreshPartGrid();
	}
	else
	{
		m_wndLayoutList.m_wndLayoutListTab.Refresh();
	}
}

/*
void CLayoutManagePage::OnRinbonBtnLayoutRequest()
{
    HRESULT hr = S_OK;
    std::vector<Layout> LayoutList;

    hr = m_wndLayoutList.m_wndLayoutListTab.GetSelectedLayoutList(LayoutList);

    auto it = LayoutList.begin(), ite = LayoutList.end();
    Audit tmpAudit;
    for (; it != ite; ++it)
    {
        Layout layout;
        HRESULT hr = GetControllerInterface->GetLayout(it->GetLayoutID(),layout);
        if (!SUCCEEDED(hr))
        {
            MessageBox(Translate(_T("Failed to get the selected layout.")), Translate(_T("Error:Layout manager page")), MB_ICONERROR);
            return;
        }

        tmpAudit = layout.GetAudit();
        Audit tmp(GetControllerInterface->GetUserId(), S3Time::Now(), tmpAudit.GetApproveUserID(),
            tmpAudit.GetApproveTime(), tmpAudit.CovertStatus(Audit::CREATEBUTNOTAPPROVED));
        layout.SetAudit(tmp);
        // modify controllerserver data
        hr = GetControllerInterface->EditLayout(layout);
        if (!SUCCEEDED(hr))
        {
            MessageBox(Translate(_T("Failed to Request the layout!")), Translate(_T("Error:Layout manager page")), MB_ICONERROR);
            return;
        }
    }
	if (m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() == 0x00) //layout list
	{
		m_wndLayoutList.m_wndLayoutListTab.m_grid.RefreshPartGrid();
	}
	else
	{
		m_wndLayoutList.m_wndLayoutListTab.Refresh();
	}
}
}*/

/*
void CLayoutManagePage::OnUpdateRinbonBtnLayoutRequest(CCmdUI *pCmdUI)
{
    // TODO: Add your command update UI handler code here
    if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if (1)// (0 == m_wndLayoutList.m_wndLayoutListTab.GetActiveTab())
	{
        if ((m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() == 0 &&
            m_wndLayoutList.m_wndLayoutListTab.m_grid.GetSelectionCount() > 0) ||
            (m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() == 1 &&
            m_wndLayoutList.m_wndLayoutListTab.m_wndSearchList->GetSelectedCount() > 0))
        {
            pCmdUI->Enable(TRUE);
        }
        else
        {
            pCmdUI->Enable(FALSE);
        }
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
*/

void CLayoutManagePage::OnUpdateRinbonBtnLayoutRename(CCmdUI *pCmdUI)
{
    if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if ((m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() == 0 &&
		m_wndLayoutList.m_wndLayoutListTab.m_grid.GetSelectionCount() == 1) ||
		(m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() == 1 &&
		m_wndLayoutList.m_wndLayoutListTab.m_wndSearchList->GetSelectedCount() == 1))
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

#ifdef SERVER_EDITION
void CLayoutManagePage::OnUpdateRinbonBtnLayoutApprove(CCmdUI *pCmdUI)
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if (0 == m_wndLayoutList.m_wndLayoutListTab.GetActiveTab())
	{
		vector<int> layoutIDList;
		m_wndLayoutList.m_wndLayoutListTab.m_grid.GetSelectedMultiID(layoutIDList);
		if(layoutIDList.size() > 0) 
		{
			if (m_wndLayoutList.m_wndLayoutListTab.m_grid.JudgeFirstSelAudit(Translate(_T("APPROVED")), CLayoutListTab::LAYOUT_AUDIT))
			{
				pCmdUI->Enable(FALSE);
				return;
			}
			else
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else if(1 == m_wndLayoutList.m_wndLayoutListTab.GetActiveTab())
	{
		if(m_wndLayoutList.m_wndLayoutListTab.m_wndSearchList->GetSelectedCount() > 0) 
		{
			/* by RaymondLiu
			POSITION pos = m_wndLayoutList.m_wndLayoutListTab.m_wndSearchList->GetFirstSelectedItemPosition();
			INT index = m_wndLayoutList.m_wndLayoutListTab.m_wndSearchList->GetNextSelectedItem(pos);

			CString szText = m_wndLayoutList.m_wndLayoutListTab.m_wndSearchList->GetItemText(index, CLayoutListTab::LAYOUT_AUDIT);

			if(szText == Translate(_T("APPROVED")) )
			{
				pCmdUI->Enable(FALSE);
			}
			else
			{
				pCmdUI->Enable(TRUE);
			}*/pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}


void CLayoutManagePage::OnUpdateRinbonBtnLayoutReject(CCmdUI *pCmdUI)
{
    if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	if (0 == m_wndLayoutList.m_wndLayoutListTab.GetActiveTab())
	{
		vector<int> layoutIDList;
		m_wndLayoutList.m_wndLayoutListTab.m_grid.GetSelectedMultiID(layoutIDList);
		if(layoutIDList.size() > 0) 
        {
			if (m_wndLayoutList.m_wndLayoutListTab.m_grid.JudgeFirstSelAudit(Translate(_T("REJECTED")), CLayoutListTab::LAYOUT_AUDIT))
			{
				pCmdUI->Enable(FALSE);
				return;
			}
			else
			{
				pCmdUI->Enable(TRUE);
				return;
			}
        }
        else
        {
            pCmdUI->Enable(FALSE);
        }
	}
	else if(1 == m_wndLayoutList.m_wndLayoutListTab.GetActiveTab())
	{
		if(m_wndLayoutList.m_wndLayoutListTab.m_wndSearchList->GetSelectedCount() > 0) 
		{
			POSITION pos = m_wndLayoutList.m_wndLayoutListTab.m_wndSearchList->GetFirstSelectedItemPosition();
			INT index = m_wndLayoutList.m_wndLayoutListTab.m_wndSearchList->GetNextSelectedItem(pos);
			CString szText = m_wndLayoutList.m_wndLayoutListTab.m_wndSearchList->GetItemText(index, CLayoutListTab::LAYOUT_AUDIT);

			if(szText == Translate(_T("REJECTED")) )
			{
				pCmdUI->Enable(FALSE);
			}
			else
			{
				pCmdUI->Enable(TRUE);
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

#endif


void CLayoutManagePage::OnUpdateRinbonBtnLayoutNew(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(m_WorkingCategoryID >= -1);
}


void CLayoutManagePage::OnUpdateRinbonBtnLayoutEdit(CCmdUI *pCmdUI)
{
    if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if (0 == m_wndLayoutList.m_wndLayoutListTab.GetActiveTab())
	{
        if ( m_wndLayoutList.m_wndLayoutListTab.m_grid.GetSelectionCount() == 1) 
        {
            pCmdUI->Enable(TRUE);
        }
        else
        {
            pCmdUI->Enable(FALSE);
        }
	}
	else if (1 == m_wndLayoutList.m_wndLayoutListTab.GetActiveTab())
	{
		if (m_wndLayoutList.m_wndLayoutListTab.m_wndSearchList->GetSelectedCount() == 1)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}


void CLayoutManagePage::OnUpdateRinbonBtnLayoutDelete(CCmdUI *pCmdUI)
{
    if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if (0 == m_wndLayoutList.m_wndLayoutListTab.GetActiveTab())
	{
		vector<int> layoutIDList;
		m_wndLayoutList.m_wndLayoutListTab.m_grid.GetSelectedMultiID(layoutIDList);
		if (layoutIDList.size() > 0)
		{
			if (!m_wndLayoutList.m_wndLayoutListTab.m_grid.JudgeReferCount())
			{
				pCmdUI->Enable(TRUE);
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else if (1 == m_wndLayoutList.m_wndLayoutListTab.GetActiveTab())
	{
		if ((m_wndLayoutList.m_wndLayoutListTab.m_wndSearchList->GetSelectedCount() >= 1))
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}


void CLayoutManagePage::OnUpdateRibbonBtnPreview(CCmdUI *pCmdUI)
{
    if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if (1)//(0 == m_wndLayoutList.m_wndLayoutListTab.GetActiveTab())
	{
        if ((m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() == 0 &&
            m_wndLayoutList.m_wndLayoutListTab.m_grid.GetSelectionCount() == 1) ||
            (m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() == 1 &&
            m_wndLayoutList.m_wndLayoutListTab.m_wndSearchList->GetSelectedCount() == 1))
        {
            pCmdUI->Enable(TRUE);
        }
        else
        {
            pCmdUI->Enable(FALSE);
        }
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}


LRESULT CLayoutManagePage::OnUpdateLayoutCategoryList(WPARAM wp, LPARAM lp)
{
    m_WorkingCategoryID = wp;
    m_wndLayoutList.m_wndLayoutListTab.SetLayoutCategory(wp);
    return 0;
}

HRESULT CreateStreamOnResource(HMODULE Instance, PCTSTR Type, PCTSTR Name, IStream** Stream);


LRESULT CLayoutManagePage::OnLayoutSelected(WPARAM wp, LPARAM lp)
{
    HRESULT hr;
    std::vector<Layout> LayoutList;

    hr = m_wndLayoutList.m_wndLayoutListTab.GetSelectedLayoutList(LayoutList);

    m_wndLayoutInfo.m_LayoutInfo.ClearInfo();

    if(LayoutList.size() == 1)
    {
        m_wndLayoutInfo.m_LayoutInfo.SetMediaInfo(LayoutList[0]);

        vector<BYTE> data;
        GetControllerInterface->GetLayoutThumbnail(LayoutList[0].GetLayoutID(), data);

        MLThumbnail thumbnailImage(data);
        thumbnailImage.InitFromData(data);

        if(thumbnailImage.GetImage().IsNull())
        {
            CImage EmptyImage;
            //IStream* stream = NULL;
            //CreateStreamOnResource(AfxGetInstanceHandle(), RT_GROUP_ICON, MAKEINTRESOURCE(IDI_EMPTYCONTENT), &stream);
            //if(stream != NULL)
            //{
            //    EmptyImage.Load(stream);
            //    stream->Release();
            //    m_wndLayoutInfo.m_LayoutInfo.SetMediaThumbnail(EmptyImage);
            //}

            HICON hIcon = AfxGetApp()-> LoadIcon(MAKEINTRESOURCE(IDI_EMPTYCONTENT));
            ICONINFO info;
            if (::GetIconInfo   (hIcon,   &info))
            {
                BITMAP   bmp;
                if (::GetObject   (info.hbmColor,   sizeof   (BITMAP),   (LPVOID)   &bmp))
                {
                    HBITMAP hBmp = (HBITMAP)::CopyImage(info.hbmColor, IMAGE_BITMAP, 0, 0, 0);

                    EmptyImage.Attach(hBmp);

                    m_wndLayoutInfo.m_LayoutInfo.SetMediaThumbnail(EmptyImage);

                    ::DeleteObject   (hBmp);
                }
            }
            ::DeleteObject   (info.hbmColor);
            ::DeleteObject   (info.hbmMask);

        }else
        {
            m_wndLayoutInfo.m_LayoutInfo.SetMediaThumbnail(thumbnailImage.GetImage());
        }


    }
    return 0;
}

LRESULT CLayoutManagePage::OnLayoutListRefresh(WPARAM, LPARAM)
{
	if (m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() == 0x00) //layout list
	{
		m_wndLayoutList.m_wndLayoutListTab.m_grid.RefreshLayoutGrid();
	}
	else
	{
		m_wndLayoutList.m_wndLayoutListTab.Refresh();
	}
    return 0;
}

LRESULT CLayoutManagePage::OnLayoutDlgSearch(WPARAM wp, LPARAM lp)
{
    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("MediaLibrary List Updating!")).GetString());
    return OnLayoutSearch(wp, lp);
#if 0
    TCHAR buf[16];
    CString strID;

    CSearchLayoutDlg* pDlg = (CSearchLayoutDlg*)lp;

    CString Desc = pDlg->m_strDesc;

    CComboBox *pCB = &pDlg->m_ctlCategory;
    int CategoryID = pCB->GetItemData(pCB->GetCurSel());

    CComboBox *pCBowner = &pDlg->m_ctlowner;
    int OwnerID = pCBowner->GetItemData(pCBowner->GetCurSel());

    CComboBox *pCBsize = & pDlg->m_ctlSize;
    int SizeId = pCBsize->GetItemData(pCBsize->GetCurSel());

    CComboBox *pCBapproval = & pDlg->m_CtlApproval;
    int ApprovalID = pCBapproval->GetItemData(pCBapproval->GetCurSel());

    CString Conditons = _T(" LayoutName like '%") + Desc + _T("%' ");

    if (CategoryID >=0)
    {
        _itot_s(CategoryID, buf, 10);
        strID = buf;
        Conditons += _T("and LayoutFilterId = ") + strID;
    }
    if (OwnerID >= 0)
    {
        _itot_s(OwnerID, buf, 10);
        strID = buf;
        Conditons += _T(" and CreateUserId = ") + strID;
    }
    if (ApprovalID >= 0)
    {
        _itot_s(ApprovalID, buf, 10);
        strID = buf;
        Conditons += _T(" and Status = ") + strID;
    }

    if (SizeId >= 0)
    {
        switch(SizeId)
        {
        case 0:
            {
                Conditons += _T(" and ExtractValue(description, '//Entry[3]//Value') > 0 and ExtractValue(description, '//Entry[3]//Value') <= 9*1024*1024");
            }
            break;
        case 1:
            {
                Conditons += _T(" and ExtractValue(description, '//Entry[3]//Value') > 9*1024*1024 and ExtractValue(description, '//Entry[3]//Value') <= 15*1024*1024");
            }
            break;
        case 2:
            {
                Conditons += _T(" and ExtractValue(description, '//Entry[3]//Value') > 15*1024*1024 and ExtractValue(description, '//Entry[3]//Value') <= 127*1024*1024 ");
            }
            break;
        case 3:
            {
                Conditons += _T(" and ExtractValue(description, '//Entry[3]//Value') > 127*1024*1024");
            }
            break;
        default:
            break;
        }
    }

    if (pDlg->m_bUseDate)
    {
        S3Time startTime(S3Time::NULLTime), endTime(S3Time::OneDayAfter());
        ATL::COleDateTime t1, t2;
        COleDateTime TmpTimeStart =  pDlg->m_timeStart;
        COleDateTime TmpTimeEnd =  pDlg->m_timeEnd;

        t1.SetDateTime(TmpTimeStart.GetYear(),TmpTimeStart.GetMonth(), TmpTimeStart.GetDay(),0, 0,0);
        t2.SetDateTime(TmpTimeEnd.GetYear(), TmpTimeEnd.GetMonth(), TmpTimeEnd.GetDay(), 23,59,59);

        startTime = S3Time::CreateTime(t1);
        endTime = S3Time::CreateTime(t2);

        std::wstring tmpStart = startTime.ToString();
        std::wstring tmpEnd = endTime.ToString();
        CString tmp1(tmpStart.c_str());
        CString tmp2(tmpEnd.c_str());
        // Conditons += _T(" and UploadTime >= ") + tmp1 + _T(" and UploadTime < ") + tmp2;
        Conditons += _T(" and CreateTime between s3datetime('") + tmp1 + _T("') and s3datetime('") + tmp2 + _T("')");
    }

    // do search now
    Condition cond(Conditons.GetString());

    //How to new a buf and post it to work thread
    UINT nBufID;
    Condition* pInfo = WorkThreadDataManager< Condition >::NewDataBuf(nBufID); //create dynamic msg
    if (pInfo == NULL)
    {
        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)Translate(_T("Failed to search!")).GetString());

        return -1;
    }

    *pInfo = cond;

    DWORD MessageID = USER_MSG_SEARCH_LAYOUT_LIST;

    if (m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() != 1)
    {
        m_wndLayoutList.m_wndLayoutListTab.SetActiveTab(1);
    }

    if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,
        (WPARAM)m_wndLayoutList.m_wndLayoutListTab.GetSafeHwnd(), (LPARAM)nBufID))//post thread msg
    {
        TRACE1("post message failed,errno:%d\n",::GetLastError());

        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)Translate(_T("Failed to search!")).GetString());

        return -1;
    }

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("Searching......")).GetString());
#endif
    return 0;
}

LRESULT CLayoutManagePage::OnLayoutSearch(WPARAM, LPARAM)
{
    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("MediaLibrary List Updating!")).GetString());

    TCHAR buf[16];
    CString strID;
    //retrieve data from dialog
    LayoutSearchOptions *pOptions = LayoutSearchOptions::Instance();
    CString Desc = pOptions->_StrDesc;
    Desc.Replace(_T("%"), _T("\\%"));
    Desc.Replace(_T("_"), _T("\\_"));

    int CategoryID = pOptions->_GroupID;

    int OwnerID = pOptions->_OwnerID;


    int SizeId = pOptions->_SizeID;


    int ApprovalID = pOptions->_AuditID;

    CString Conditons = _T(" lower(LayoutName) like lower('%") + Desc + _T("%') ");

    if (OwnerID >= 0)
    {
        _itot_s(OwnerID, buf, 10);
        strID = buf;
        Conditons += _T(" and CreateUserId = ") + strID;
    }
    if (ApprovalID >= 0)
    {
        _itot_s(ApprovalID, buf, 10);
        strID = buf;
        Conditons += _T(" and Status = ") + strID;
    }
#ifdef SERVER_EDITION
    if (SizeId >= 0)
    {
        switch(SizeId)
        {
        case 0:
            {
                Conditons += _T(" and ExtractValue(description, '//Entry[3]//Value') >= 0 and ExtractValue(description, '//Entry[3]//Value') <= 9*1024*1024");
            }
            break;
        case 1:
            {
                Conditons += _T(" and ExtractValue(description, '//Entry[3]//Value') > 9*1024*1024 and ExtractValue(description, '//Entry[3]//Value') <= 15*1024*1024");
            }
            break;
        case 2:
            {
                Conditons += _T(" and ExtractValue(description, '//Entry[3]//Value') > 15*1024*1024 and ExtractValue(description, '//Entry[3]//Value') <= 127*1024*1024 ");
            }
            break;
        case 3:
            {
                Conditons += _T(" and ExtractValue(description, '//Entry[3]//Value') > 127*1024*1024");
            }
            break;
        default:
            break;
        }
    }
#endif

    if (LayoutSearchOptions::Instance()->_bUseDate)
    {
        S3Time startTime(S3Time::NULLTime), endTime(S3Time::OneDayAfter());
        ATL::COleDateTime t1, t2;
        COleDateTime TmpTimeStart =  LayoutSearchOptions::Instance()->_timeStart;
        COleDateTime TmpTimeEnd =  LayoutSearchOptions::Instance()->_timeEnd;

        t1.SetDateTime(TmpTimeStart.GetYear(),TmpTimeStart.GetMonth(), TmpTimeStart.GetDay(),0, 0,0);
        t2.SetDateTime(TmpTimeEnd.GetYear(), TmpTimeEnd.GetMonth(), TmpTimeEnd.GetDay(), 23,59,59);

        startTime = S3Time::CreateTime(t1);
        endTime = S3Time::CreateTime(t2);

        std::wstring tmpStart = startTime.ToString();
        std::wstring tmpEnd = endTime.ToString();
        CString tmp1(tmpStart.c_str());
        CString tmp2(tmpEnd.c_str());
        // Conditons += _T(" and UploadTime >= ") + tmp1 + _T(" and UploadTime < ") + tmp2;
        Conditons += _T(" and CreateTime between s3datetime('") + tmp1 + _T("') and s3datetime('") + tmp2 + _T("')");
    }


    if (CategoryID >=0)
    {
        _itot_s(CategoryID, buf, 10);
        strID = buf;
        Conditons += _T(" and (LayoutFilterId = ") + strID;
    }
    // do search now
//    Condition cond(Conditons.GetString());

    //How to new a buf and post it to work thread
    UINT nBufID;
    std::map<int, CString>*pInfo = WorkThreadDataManager< std::map<int, CString> >::NewDataBuf(nBufID);
  //  Condition* pInfo = WorkThreadDataManager< Condition >::NewDataBuf(nBufID); //create dynamic msg
    if (pInfo == NULL)
    {
        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)Translate(_T("Failed to search!")).GetString());

        return -1;
    }

    pInfo[0].insert(std::map<int, CString>::value_type(CategoryID, Conditons));

    DWORD MessageID = USER_MSG_SEARCH_LAYOUT_LIST;

    if (m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() != 1)
    {
        m_wndLayoutList.m_wndLayoutListTab.SetActiveTab(1);
    }

    if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,
        (WPARAM)m_wndLayoutList.m_wndLayoutListTab.GetSafeHwnd(), (LPARAM)nBufID))//post thread msg
    {
        TRACE1("post message failed,error:%d\n",::GetLastError());

        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)Translate(_T("Failed to search!")).GetString());

        return -1;
    }

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("Searching......")).GetString());

    return 0;
}

LRESULT CLayoutManagePage::OnLayoutSetActiveTab(WPARAM wp, LPARAM lp)
{
    int indexTab = (int)wp;
    if(m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() != indexTab)
        m_wndLayoutList.m_wndLayoutListTab.SetActiveTab(indexTab);
    return 0;
}

HRESULT CLayoutManagePage::PreviewLayout(const int nWidth, const int nHeight)
{
    HRESULT hr = S_OK;
    BOOL bExistFiles = FALSE;

    std::vector<Layout> LayoutList;

    hr = m_wndLayoutList.m_wndLayoutListTab.GetSelectedLayoutList(LayoutList);

    if (LayoutList.size() == 1)
    {

        Layout layout;

        hr = GetControllerInterface->GetLayout(LayoutList[0].GetLayoutID(),   layout);
        if(FAILED(hr))
        {
            MessageBox(Translate(_T("Failed to get layout!")), Translate(_T("Error:Layout manager page")), MB_ICONERROR);
            return hr;
        }

        CString CurDownLoadedLayoutPath = GetConfig->Get(ROOT_DIR) + DESIGNER_PATH + CString(layout.GetLayoutName().c_str());
        CurDownLoadedLayoutPath.TrimRight(_T(" "));
        CurDownLoadedLayoutPath +=  _T("\\");

        vector<INT> MediaIDs = layout.GetMediaIDList();
        if (!CurDownLoadedLayoutPath.IsEmpty())
        {
            CString szLayoutName = CurDownLoadedLayoutPath;

            

            ProcessCall::CheckAndCreatePath(CurDownLoadedLayoutPath.GetString());

            if (::PathFileExists(CurDownLoadedLayoutPath))
            {
                if ((szLayoutName.ReverseFind(_T('\\')) + 1) == szLayoutName.GetLength())
                {
                    szLayoutName.Append(_T("index.xml"));
                }
                else
                {
                    szLayoutName.Append(_T("\\index.xml"));
                }

                std::vector<BYTE> LayoutContent;
                hr = GetControllerInterface->GetLayoutContent(layout.GetLayoutID(),   LayoutContent);
                if(FAILED(hr))
                {
                    MessageBox(Translate(_T("Failed to get layout content!")), Translate(_T("Error:Layout manager page")), MB_ICONERROR);
                    return hr;
                }


                std::ofstream file;

                file.open((LPCWSTR)szLayoutName, std::ios_base::trunc);

                if(!file.is_open())

                    return E_FAIL;

                file.write(reinterpret_cast<char*>(&LayoutContent[0]), LayoutContent.size());
                file.close();

                LayoutFileUtility::PreviewLayout(szLayoutName, MediaIDs, nWidth, nHeight);
            }
        }
    }

    return hr;
}

void CLayoutManagePage::OnUpdateRibbonBtn(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    static std::map<int, std::pair<std::function<void (CCmdUI*)>, std::function<bool ()> > > dispatchMap;
    // check privilege
    if (dispatchMap.empty())
    {
     //   UPDATE_COMMAND_ENTRY(ID_RINBON_BTN_LAYOUT_REQUEST, &CLayoutManagePage::OnUpdateRinbonBtnLayoutRequest, LAMBDA(SC(ApproveLayoutOperation)));
        UPDATE_COMMAND_ENTRY(ID_RINBON_BTN_LAYOUT_RENAME, &CLayoutManagePage::OnUpdateRinbonBtnLayoutRename, LAMBDA(SC(PublishLayout)));
#ifdef SERVER_EDITION
        UPDATE_COMMAND_ENTRY(ID_RINBON_BTN_LAYOUT_REJECT, &CLayoutManagePage::OnUpdateRinbonBtnLayoutReject, LAMBDA(SC(ApproveLayoutOperation)));
		UPDATE_COMMAND_ENTRY(ID_RINBON_BTN_LAYOUT_APPROVE, &CLayoutManagePage::OnUpdateRinbonBtnLayoutApprove, LAMBDA(SC(ApproveLayoutOperation)));
#endif
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_PREVIEW_10X7, &CLayoutManagePage::OnUpdateRibbonBtnPreview, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_PREVIEW_12X10, &CLayoutManagePage::OnUpdateRibbonBtnPreview, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_PREVIEW_19X10, &CLayoutManagePage::OnUpdateRibbonBtnPreview, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RINBON_BTN_LAYOUT_PREVIEW, &CLayoutManagePage::OnUpdateRibbonBtnPreview, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RINBON_BTN_LAYOUT_NEW, &CLayoutManagePage::OnUpdateRinbonBtnLayoutNew, LAMBDA(SC(PublishLayout)));
        UPDATE_COMMAND_ENTRY(ID_RINBON_BTN_LAYOUT_IMPORT, &CLayoutManagePage::OnUpdateRinbonBtnLayoutNew, LAMBDA(SC(PublishLayout)));
        UPDATE_COMMAND_ENTRY(ID_RINBON_BTN_LAYOUT_EDIT, &CLayoutManagePage::OnUpdateRinbonBtnLayoutEdit, LAMBDA(SC(PublishLayout)));
        UPDATE_COMMAND_ENTRY(ID_RINBON_BTN_LAYOUT_DELETE, &CLayoutManagePage::OnUpdateRinbonBtnLayoutDelete, LAMBDA(SC(DeleteLayout)));
        
    }

    auto it = dispatchMap.find(pCmdUI->m_nID), ite = dispatchMap.end();
    // id exist and have privilege
    if (it != ite && it->second.second())
    {
        it->second.first(pCmdUI);
    }
    else
    {
        pCmdUI->Enable(TRUE);
    }
}

//zxy 2011/12/21
void CLayoutManagePage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CBaseControllerPage::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	if(bShow)
	{
		m_wndLayoutCategory.m_LayoutList.OnClickedFilterCategoryRefresh();
		if (m_wndLayoutList.m_wndLayoutListTab.GetActiveTab() == 0x00) //layout list
		{
			m_wndLayoutList.m_wndLayoutListTab.m_grid.RefreshLayoutGrid();
		}
		else
		{
			m_wndLayoutList.m_wndLayoutListTab.Refresh();
		}
	}
}
