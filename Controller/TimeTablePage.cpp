#include "StdAfx.h"
#include "TimeTablePage.h"
#include "Localization/Localization.h"
#include "PlayerController.h"
#include "PreviewDownLoadDlg.h"
#include "LayoutRenameCtrl.h"
#include "LayoutFileUtility.h"
#include "RejectReason.h"
#include "RenameCategory.h"

#include "Utilities/SysCall.h"

#define LAYOUT_ROW    4
#define LAYOUT_COLUMN 5

IMPLEMENT_DYNAMIC(CTimeTablePage, CBaseControllerPage)

CTimeTablePage::CTimeTablePage(void)
{
}


CTimeTablePage::~CTimeTablePage(void)
{

}

BEGIN_MESSAGE_MAP(CTimeTablePage, CBaseControllerPage)
	ON_WM_CREATE()
	ON_WM_DESTROY()
    ON_COMMAND(ID_RIBBON_BTN_TIMETABLE_PREVIEWWINMODE, OnRibbonBtnPreviewWinMode)
    ON_COMMAND(ID_RIBBON_BTN_PREVIEW_10X7, OnRibbonBtnPreview10X7)
    ON_COMMAND(ID_RIBBON_BTN_PREVIEW_12X10, OnRibbonBtnPreview12X10)
    ON_COMMAND(ID_RIBBON_BTN_PREVIEW_19X10, OnRibbonBtnPreview19X12)
    ON_COMMAND(ID_RIBBON_BTN_TIMETABLE_CREATE, OnRibbonBtnCreate)
    ON_COMMAND(ID_RIBBON_BTN_TIMETABLE_CLOSE, OnRibbonBtnClose)
    ON_COMMAND(ID_RIBBON_BTN_TIMETABLE_PUBLISH, OnRibbonBtnPublish)
    ON_COMMAND(ID_RIBBON_BTN_TIMETABLE_COPY, OnRibbonBtnCopy)
    ON_COMMAND(ID_RIBBON_BTN_TIMETABLE_PASTE, OnRibbonBtnPaste)
    ON_COMMAND(ID_RIBBON_BTN_TIMETABLE_DEL, OnRibbonBtnDel)
    ON_COMMAND(ID_RIBBON_COMBOX_TIMETABLE_TIMEDELTA, OnRibbonBtnZoom)
    ON_COMMAND(ID_RIBBON_BTN_TIMETABLE_REQUEST, OnRibbonBtnRequest)
    ON_COMMAND(ID_RIBBON_BTN_TIMETABLE_REJECT, OnRibbonBtnReject)
    ON_COMMAND(ID_RIBBON_BTN_TIMETABLE_APPROVE, OnRibbonBtnApprove)
    ON_COMMAND(ID_RIBBON_BTN_TIMETABLE_EDIT, OnRibbonBtnEdit)
    ON_COMMAND(ID_RIBBON_BTN_TIMETABLE_PUBLISH_ALL, OnRibbonBtnPublishAll)
    ON_COMMAND(ID_RIBBON_BTN_TIMETABLE_DELTABLE, OnRibbonBtnDeleteTimeTable)
    ON_COMMAND(ID_RIBBON_BTN_TIMETABLE_RENAME, OnRibbonBtnRename)

    ON_REGISTERED_MESSAGE(TIMETABLE_CATEGORY_SEL_CHANGE, OnTimeTableCategorySelChange)
    ON_REGISTERED_MESSAGE(TIMETABLE_CATEGORY_DBCLICK, OnTimeTableCategoryDbClick)
    ON_REGISTERED_MESSAGE(TIMETABLE_SELECT_APPOINTMENT, OnTimeTableItemSelChange)
    ON_REGISTERED_MESSAGE(TIMETABLE_ITEM_PROPERTY_CHANGE, OnTimeTableItemPropertyChange)
    ON_REGISTERED_MESSAGE(AFX_WM_CHANGE_ACTIVE_TAB, OnTimeTableTabActiveChange)
    ON_REGISTERED_MESSAGE(TIMETABLE_TAB_BEFORE_CLOSE, OnTimeTableBeforeTabClose)

    ON_REGISTERED_MESSAGE(TIMETABLE_KEYDOWN, OnTimeTableKeyDown)
    ON_REGISTERED_MESSAGE(TIMETABLE_TIME_DELTA_CHANGED, OnTimeTableTimedeltaChanged)
	ON_REGISTERED_MESSAGE(TIMETABLE_AFTERUNITDELETED,OnTimeTableAfterUnitDelete)

    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_TIMETABLE_PREVIEWWINMODE, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_PREVIEW_10X7, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_PREVIEW_12X10, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_PREVIEW_19X10, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_TIMETABLE_CREATE, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_TIMETABLE_CLOSE, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_TIMETABLE_PUBLISH, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_TIMETABLE_COPY, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_TIMETABLE_PASTE, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_TIMETABLE_DEL, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_COMBOX_TIMETABLE_TIMEDELTA, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_TIMETABLE_REQUEST, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_TIMETABLE_REJECT, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_TIMETABLE_APPROVE, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_TIMETABLE_EDIT, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_TIMETABLE_DELTABLE, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_TIMETABLE_PUBLISH_ALL, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_TIMETABLE_RENAME, OnUpdateRibbonBtn)

	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


int CTimeTablePage::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

    // Create and Install the searched timetable bar
    left    = 0;
    right   = rect.Width() / LAYOUT_ROW;
    top     = 0;
    bottom  = rect.Height() - 200;

    if (!m_TimeTableListPane.Create(Translate(_T("Timetable Category"))
        , this
        , CRect(left, top, right, bottom)
        , TRUE
        , ID_TIMETABLE_SERACHED_TIMETABLE_BAR
        , WS_CHILD | WS_VISIBLE | CBRS_LEFT |CBRS_HIDE_INPLACE | WS_CAPTION
        , AFX_CBRS_REGULAR_TABS
        , AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create timetable manage pane\n");
        return -1;      // fail to create
    }

    m_TimeTableListPane.EnableDocking(CBRS_LEFT);
    DockPane(&m_TimeTableListPane);

    // Create and Install the timetable edit pane
    top     = rect.Height() - 200;
    bottom  = rect.Height();

    if (!m_TimeTablePropertyEditPane.Create(Translate(_T("Timetable Info"))
        , this
        , CRect(left, top, right, bottom)
        , TRUE
        , ID_TIMETABLE_EDIT
        , WS_CHILD | WS_VISIBLE | CBRS_BOTTOM |CBRS_HIDE_INPLACE | WS_CAPTION
        , AFX_CBRS_REGULAR_TABS
        , NULL))
    {
        TRACE0("Failed to create timetable edit pane\n");
        return -1;      // fail to create
    }

    m_TimeTablePropertyEditPane.EnableDocking(CBRS_BOTTOM);
    m_TimeTablePropertyEditPane.DockToWindow(&m_TimeTableListPane, CBRS_ALIGN_BOTTOM, CRect(left, top, right, bottom));


    // Create and Install the searched timetable Layouts bar
    left    = rect.Width() / LAYOUT_ROW *3;
    right   = rect.Width();
    top     = 0;
    bottom  = rect.Height();

    m_TimeTableLayoutsPane.m_LayoutList.EnableFileMode();
    m_TimeTableLayoutsPane.m_LayoutList.ShowEmptyLayout();
    if(!m_TimeTableLayoutsPane.Create(Translate(_T("Layout List"))
        , this
        , CRect(left, top, right, bottom)
        , TRUE
        , ID_TIMETABLE_SERACHED_LAYOUT_BAR
        , WS_CHILD | WS_VISIBLE | CBRS_RIGHT |CBRS_HIDE_INPLACE | WS_CAPTION
        , AFX_CBRS_REGULAR_TABS
        , AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create layout filter pane\n");
        return -1;      // fail to create
    }

    m_TimeTableLayoutsPane.EnableDocking(CBRS_RIGHT);
    DockPane(&m_TimeTableLayoutsPane);

    // Create and Install the timetable edit pane
    top     = rect.Height() - 200;
    bottom  = rect.Height();

    if (!m_TimeTableLayoutPropertyPane.Create(Translate(_T("Timetable Item Edit"))
        , this
        , CRect(left, top, right, bottom)
        , TRUE
        , ID_TIMETABLE_EDIT
        , WS_CHILD | WS_VISIBLE | CBRS_BOTTOM |CBRS_HIDE_INPLACE | WS_CAPTION
        , AFX_CBRS_REGULAR_TABS
        , NULL))
    {
        TRACE0("Failed to create timetable edit pane\n");
        return -1;      // fail to create
    }

    m_TimeTableLayoutPropertyPane.EnableDocking(CBRS_BOTTOM);
    m_TimeTableLayoutPropertyPane.DockToWindow(&m_TimeTableLayoutsPane, CBRS_ALIGN_BOTTOM, CRect(left, top, right, bottom));



	// Create and Install the layout filter pane

	top     = rect.Height() / LAYOUT_COLUMN;
	bottom  = rect.bottom;
	left    = rect.Width() / LAYOUT_ROW;
	right   = rect.Width() / LAYOUT_ROW * (LAYOUT_ROW - 1);

	if (!m_TimeTablePane.Create(_T("")
		, this
		, CRect(left, top, right, bottom)
		, TRUE
		, ID_SCHEDULER_WND
		, WS_CHILD | WS_VISIBLE | CBRS_ALL| CBRS_HIDE_INPLACE | WS_CAPTION
		, AFX_CBRS_REGULAR_TABS
		, AFX_CBRS_RESIZE))
	{
		TRACE0("Failed to create timetable\n");
		return -1;      // fail to create
	}

	GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO
		, NULL
		, (LPARAM)(Translate(_T("Ready"))).GetString());

	SetMainWnd(&m_TimeTablePane);

	return 0;
}

void CTimeTablePage::OnDestroy()
{
	CBaseControllerPage::OnDestroy();
}

BOOL CTimeTablePage::CreateRibbonButtons()
{
	if (!m_pCategory)
    {
        return FALSE;
    }

    /// Remove the dummy panel, and create the real panels
    m_pCategory->RemovePanel(0);

    CString strPanelFile     = Translate(_T("File"));
    CString strPanelApproval  = Translate(_T("Approval"));
    CString strPanelEdit     = Translate(_T("Edit"));
    CString strPanelView     = Translate(_T("Preview"));
    CString strPanelScheduler     = Translate(_T("TimeTable View"));

    CMFCRibbonPanel* pPanelFile = m_pCategory->AddPanel(strPanelFile);

    CMFCRibbonButton* pBtnCreate = new CMFCRibbonButton(ID_RIBBON_BTN_TIMETABLE_CREATE,  Translate(_T("New")), 0, 0);
    pBtnCreate->SetAlwaysLargeImage();
    pBtnCreate->SetToolTipText(Translate(_T("New")));
    pBtnCreate->SetDescription(Translate(_T("Create a new timetable in selected timetable category.")));
    pPanelFile->Add(pBtnCreate);

    CMFCRibbonButton* pBtnEdit = new CMFCRibbonButton(ID_RIBBON_BTN_TIMETABLE_EDIT,  Translate(_T("Edit")), 1, 1);
    pBtnEdit->SetAlwaysLargeImage();
    pBtnEdit->SetToolTipText(Translate(_T("Edit")));
    pBtnEdit->SetDescription(Translate(_T("Edit the selected timetable.")));
    pPanelFile->Add(pBtnEdit);

    CMFCRibbonButton* pBtnDelTable = new CMFCRibbonButton(ID_RIBBON_BTN_TIMETABLE_DELTABLE,  Translate(_T("Delete")), 2, 2);
    pBtnDelTable->SetAlwaysLargeImage();
    pBtnDelTable->SetToolTipText(Translate(_T("Delete")));
    pBtnDelTable->SetDescription(Translate(_T("Delete the selected timetable.")));
    pPanelFile->Add(pBtnDelTable);

    CMFCRibbonButton* pBtnRename = new CMFCRibbonButton(ID_RIBBON_BTN_TIMETABLE_RENAME,  Translate(_T("Rename")), 10, 10);
    pBtnRename->SetAlwaysLargeImage();
    pBtnRename->SetToolTipText(Translate(_T("Rename")));
    pBtnRename->SetDescription(Translate(_T("Rename the selected timetable.")));
    pPanelFile->Add(pBtnRename);

    CMFCRibbonButton* pBtnSave = new CMFCRibbonButton(ID_RIBBON_BTN_TIMETABLE_PUBLISH, Translate(_T("Publish")), 3, 3);
    pBtnSave->SetAlwaysLargeImage();
#ifdef STARTER_EDITION
    pBtnSave->SetToolTipText(Translate(_T("Publish")));
    pBtnSave->SetDescription(Translate(_T("Publish the selected timetable.")));
#else
	pBtnSave->SetToolTipText(Translate(_T("Publish")));
    pBtnSave->SetDescription(Translate(_T("Publish the selected timetable to controller server.")));
#endif
    pPanelFile->Add(pBtnSave);

    CMFCRibbonButton* pBtnSaveAll= new CMFCRibbonButton(ID_RIBBON_BTN_TIMETABLE_PUBLISH_ALL,  Translate(_T("Publish All")), 3, 3);
    pBtnSaveAll->SetAlwaysLargeImage();
#ifdef STARTER_EDITION
    pBtnSaveAll->SetToolTipText(Translate(_T("Publish all")));
    pBtnSaveAll->SetDescription(Translate(_T("Publish all the modified timetables.")));
#else
    pBtnSaveAll->SetToolTipText(Translate(_T("Publish all")));
    pBtnSaveAll->SetDescription(Translate(_T("Publish all the modified timetables to controller server.")));
#endif // _DEBUG
    pPanelFile->Add(pBtnSaveAll);


#ifdef SERVER_EDITION

    CMFCRibbonPanel* pPanelApproval = m_pCategory->AddPanel(strPanelApproval);

    CMFCRibbonButton* pBtnApprove = new CMFCRibbonButton(ID_RIBBON_BTN_TIMETABLE_APPROVE, Translate(_T("Approve")), 4, 4);
    pBtnApprove->SetAlwaysLargeImage();
    pBtnApprove->SetToolTipText(Translate(_T("Approve")));
    pBtnApprove->SetDescription(Translate(_T("Approve the selected timetable.")));
    pPanelApproval->Add(pBtnApprove);

    CMFCRibbonButton* pBtnReject = new CMFCRibbonButton(ID_RIBBON_BTN_TIMETABLE_REJECT,  Translate(_T("Reject")), 5, 5);
    pBtnReject->SetAlwaysLargeImage();
    pBtnReject->SetToolTipText(Translate(_T("Reject")));
    pBtnReject->SetDescription(Translate(_T("Reject the selected timetable.")));
    pPanelApproval->Add(pBtnReject);

/*
    CMFCRibbonButton* pBtnReq = new CMFCRibbonButton(ID_RIBBON_BTN_TIMETABLE_REQUEST,  Translate(_T("Request")), 6, 6);
    pBtnReq->SetAlwaysLargeImage();
    pBtnReq->SetToolTipText(Translate(_T("Request")) + _T("\n") + Translate(_T("Request approbation for the timetable")));
    pPanelApproval->Add(pBtnReq);
*/

#endif

    CMFCRibbonPanel* pPanelEdit = m_pCategory->AddPanel(strPanelEdit);

    CMFCRibbonButton* pBtnCopy = new CMFCRibbonButton(ID_RIBBON_BTN_TIMETABLE_COPY,  Translate(_T("Copy")), 7, 7);
    pBtnCopy->SetAlwaysLargeImage();
	pBtnCopy->SetToolTipText(Translate(_T("Copy")));
    pBtnCopy->SetDescription(Translate(_T("Copy the selected layout cell.")));
    pPanelEdit->Add(pBtnCopy);

    CMFCRibbonButton* pBtnPaste = new CMFCRibbonButton(ID_RIBBON_BTN_TIMETABLE_PASTE,  Translate(_T("Paste")), 8, 8);
    pBtnPaste->SetAlwaysLargeImage();
	pBtnPaste->SetToolTipText(Translate(_T("Paste")));
    pBtnPaste->SetDescription(Translate(_T("Paste the layout cell to the selected date.")));
    pPanelEdit->Add(pBtnPaste);

    CMFCRibbonButton* pBtnDel = new CMFCRibbonButton(ID_RIBBON_BTN_TIMETABLE_DEL,  Translate(_T("Delete")), 2, 2);
    pBtnDel->SetAlwaysLargeImage();
	pBtnDel->SetToolTipText(Translate(_T("Delete")));
    pBtnDel->SetDescription(Translate(_T("Delete the selected layout cell.")));
    pPanelEdit->Add(pBtnDel);

    CMFCRibbonPanel* pPanelView = m_pCategory->AddPanel(strPanelView);

    CMFCRibbonButton* pBtnPreview = new CMFCRibbonButton(ID_RIBBON_BTN_TIMETABLE_PREVIEWWINMODE,  Translate(_T("Preview")), 9, 9);
    pBtnPreview->AddSubItem(new CMFCRibbonButton(ID_RIBBON_BTN_PREVIEW_10X7, _T("1024 x 768")));
    pBtnPreview->AddSubItem(new CMFCRibbonButton(ID_RIBBON_BTN_PREVIEW_12X10, _T("1280 x 1024")));
    pBtnPreview->AddSubItem(new CMFCRibbonButton(ID_RIBBON_BTN_PREVIEW_19X10, _T("1920 x 1080")));
    pBtnPreview->SetToolTipText(Translate(_T("Preview")));
    pBtnPreview->SetDescription(Translate(_T("Preview the selected layout cell.")));
	pPanelView->Add(pBtnPreview);

    m_pComboBoxTimeDelta = new CMFCRibbonComboBox(ID_RIBBON_COMBOX_TIMETABLE_TIMEDELTA, FALSE, 75,  Translate(_T("Zoom")));

    m_pComboBoxTimeDelta->AddItem(Translate(_T("60 min")), 60*60);
    m_pComboBoxTimeDelta->AddItem(Translate(_T("30 min")), 30*60);
	m_pComboBoxTimeDelta->AddItem(Translate(_T("15 min")), 15*60);
	m_pComboBoxTimeDelta->AddItem(Translate(_T("10 min")), 10*60);
	m_pComboBoxTimeDelta->AddItem(Translate(_T("6 min")), 6*60);
    m_pComboBoxTimeDelta->AddItem(Translate(_T("1 min")), 60);
	m_pComboBoxTimeDelta->AddItem(Translate(_T("30 sec")), 30);
	m_pComboBoxTimeDelta->AddItem(Translate(_T("5 sec")), 5);
	//m_pComboBoxTimeDelta->AddItem(Translate(_T("1 sec")), 1);//1 sec will occur an error.
	m_pComboBoxTimeDelta->SelectItem(4);
    CMFCRibbonPanel* pPanelScheduler = m_pCategory->AddPanel(strPanelScheduler);
	m_pComboBoxTimeDelta->SetToolTipText(Translate(_T("Zoom")));
    m_pComboBoxTimeDelta->SetDescription(Translate(_T("Change the Zoom rate of the timetable page.")));
    pPanelScheduler->Add(m_pComboBoxTimeDelta);

    m_pComboBoxTimeDelta->SelectItem(4);

    return TRUE;
}

BOOL CTimeTablePage::UpdateOptions()
{
	return TRUE;
}

void CTimeTablePage::Close()
{
    if (m_TimeTablePane.m_wndTab.IsTimetableModified())
    {
        if (MessageBox(Translate(_T("There are timetable modified, are your going to save them?")),Translate(_T("Tips:Timetable page")), MB_YESNO|MB_ICONQUESTION) == IDYES)
        {
            OnRibbonBtnPublishAll();
        }
    }

    m_TimeTablePropertyEditPane.DestroyWindow();
    m_TimeTableLayoutPropertyPane.DestroyWindow();
    m_TimeTablePane.DestroyWindow();
    m_TimeTableListPane.DestroyWindow();
    m_TimeTableLayoutsPane.DestroyWindow();
}


void CTimeTablePage::OnRibbonBtnPreviewWinMode()
{
    PreviewLayout(800, 600);
}

void CTimeTablePage::OnRibbonBtnPreview10X7()
{
    PreviewLayout(1024, 768);
}

void CTimeTablePage::OnRibbonBtnPreview12X10()
{
    PreviewLayout(1280, 1024);
}

void CTimeTablePage::OnRibbonBtnPreview19X12()
{
    PreviewLayout(1920, 1080);
}

LRESULT CTimeTablePage::OnTimeTableAfterUnitDelete(WPARAM,LPARAM)
{
	m_TimeTableLayoutPropertyPane.GetCtrl()->SetItem(NULL);
	return S_OK;
}
HRESULT CTimeTablePage::PreviewLayout(const int nWidth, const int nHeight)
{
    CTimeTableCtrl* pCtrl = m_TimeTablePane.m_wndTab.GetTheCtrl();
    if (NULL == pCtrl)
        return E_FAIL;

    HRESULT hr = S_OK;

    INT nSelectedCnt = pCtrl->GetSelectedAppointmentsCount();
    if (nSelectedCnt == 1)
    {
        POSITION pos = pCtrl->GetFirstSelectedAppointment();
        Appointment* pApp = pCtrl->GetNextSelectedAppointment(pos);

        Layout layout;
        hr = GetControllerInterface->GetLayout(pApp->GetDetailID(), layout);
        if (!SUCCEEDED(hr))
        {
            CString strMess= Translate(_T("Failed to get layout, maybe this layout has been deleted.")) + _T("\n") + Translate(_T("Please reopen this page to update the schedule."));
            MessageBox(strMess,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);

            return hr;
        }

        CString CurDownLoadedLayoutPath = GetConfig->Get(ROOT_DIR) + DESIGNER_PATH + CString(layout.GetLayoutName().c_str()) + _T("\\");

        vector<INT>  MediaIDs = layout.GetMediaIDList();

        if (!CurDownLoadedLayoutPath.IsEmpty())
        {
            GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
                (LPARAM)Translate(_T("Layouts that used for previewing Downloaded!")).GetString());
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
                    MessageBox(Translate(_T("Failed to get layout content!")),Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);
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


void CTimeTablePage::OnRibbonBtnEdit()
{
    Timetable Table;
    if (m_TimeTableListPane.m_TimeTableListCtrl.GetSelectedTimeTable(Table))
    {

        OpenTimetable(Table.GetTimetableID());
    }
}

void CTimeTablePage::OnRibbonBtnDeleteTimeTable()
{
    Timetable Table;
    if (m_TimeTableListPane.m_TimeTableListCtrl.GetSelectedTimeTable(Table))
    {
        if (MessageBox(Translate(_T("Are you sure to delete this selected TimeTable?")), Translate(_T("Tips:Timetable page")),MB_YESNO|MB_ICONQUESTION) != IDYES)
        {
            return;
        }

        HRESULT hr = GetControllerInterface->DeleteTimetable(Table.GetTimetableID());
        if (!SUCCEEDED(hr))
        {
            vector<INT> GroupIDList;
            hr = GetControllerInterface->GetTimetableRefInSchedule(Table.GetTimetableID(), GroupIDList);
            CString Info(Translate(_T("Failed to delete the selected timetable!")));
            if (SUCCEEDED(hr))
            {
                CString szGroups;
                for (auto it = GroupIDList.begin(); it != GroupIDList.end(); it++)
                {
                    ClientGroup group;
                    hr = GetControllerInterface->GetClientGroup(*it, group);
                    if (SUCCEEDED(hr))
                    {
                        szGroups += (group.GetGroupName() + _T(";")).c_str();
                    }
                }
#ifndef STARTER_EDITION
                Info.Format(Translate(_T("Failed to delete the selected timetable, It is used in the schedule of %d group(s)(%s)!")), GroupIDList.size(), szGroups);
#else
				Info.Format(Translate(_T("Failed to delete the selected timetable, It is used in the schedule.")));
#endif
            }
            MessageBox(Info,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);
        }
        else
        {
            CloseTimeTable(Table.GetTimetableID(), false);

            GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
                (LPARAM)Translate(_T("Succeded to delete the selected timetable!")).GetString());

			m_TimeTableListPane.m_TimeTableListCtrl.RefreshCurCategory();
        }
    }
}

void CTimeTablePage::OnRibbonBtnPublishAll()
{
    if (!m_TimeTablePane.m_wndTab.GetTheCtrl())
    {
        return;
    }

    vector<pair<CWnd*,Timetable> > theTables;
    if (m_TimeTablePane.m_wndTab.GetAllModifiedTimeTable(theTables))
    {
        CString PublishedTimeTable;
        for (auto it = theTables.begin(); it != theTables.end(); it++)
        {
            Timetable& theTable = it->second;
            if (theTable.GetTimetableID() < 0)
            {
                Timetable newTable;
                INT myID = GetControllerInterface->GetUserId();

                newTable.SetCreateUserID(myID);
                //newTable.SetParentFilterID(m_TimeTableListPane.m_TimeTableListCtrl.GetCurCategoryID());
				newTable.SetParentFilterID(theTable.GetParentFilterID());

                INT TimeTableID, version;

                CString TableName;
                int TryCount = 1;
                while(TryCount < 100)
                {
                    CString Title;
                    Title.Format(Translate(_T("Add Timetable for \'%s\'")), theTable.GetTimetableName().c_str());
                    CLayoutRenameCtrl Dlg(this, Title, theTable.GetTimetableName().c_str());
                    if (Dlg.DoModal() == IDOK)
                    {
                        Dlg.GetLayoutNewName(TableName);
                    }
                    else
                    {
                        return;
                    }

                    newTable.SetTimetableName(TableName.GetString());

                    HRESULT hr = GetControllerInterface->AddTimetable(newTable, TimeTableID, version);
                    if (!SUCCEEDED(hr))
                    {
                        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                        {
                            TRACE0("Failed to add timetable\n");
                            GetControllerApp->OnControllerServerOffline();
                            return;
                        }
	
						//the tabletime is opened. show counld not been modified tips
						int index;
						if (m_TimeTablePane.m_wndTab.CheckTimeTableExist(TableName, &index))
						{
							CString messStr;
							messStr.Format(Translate(_T("The Timetable (%s) already exists, Given name can not be the same with the opened timetable.")), TableName);
							MessageBox(messStr,Translate(_T("Warning:Timetable page")), MB_OK|MB_ICONEXCLAMATION);
							TryCount ++;
						}
						else
						{
                            // TRANS:
							CString messStr;
							messStr.Format(Translate(_T("The Timetable (%s) already exists, continue publish will update the existing data.")), TableName);
							messStr += _T("\r\n") +
								Translate(_T("Click")) + _T("\"")+ Translate(_T("OK")) + _T("\"") + Translate(_T("to continue publish, Click")) + _T("\"") +
								Translate(_T("Cancel")) + _T("\"") + Translate(_T("to cancel the publish."));
							if (MessageBox(messStr,Translate(_T("Warning:Timetable page")), MB_OKCANCEL) == IDOK)
							{
								theTable.SetTimetableID(TimeTableID);
								theTable.SetVersion(version);
								theTable.SetCreateUserID(myID);
								theTable.SetTimetableName(newTable.GetTimetableName());
								//theTable.SetParentFilterID(newTable.GetParentFilterID());

								m_TimeTablePane.m_wndTab.ChangeWndLabel(it->first, TableName);
							    m_TimeTableListPane.m_TimeTableListCtrl.RefreshCurCategory();
								m_TimeTablePane.m_wndTab.SetWndTimeTable(it->first,theTable);

								break;
							}
							else
							{
								return;
							}
						}			

					}else
                    {
                        theTable.SetTimetableID(TimeTableID);
                        theTable.SetCreateUserID(myID);
                        theTable.SetTimetableName(TableName.GetString());
                        //theTable.SetParentFilterID(m_TimeTableListPane.m_TimeTableListCtrl.GetCurCategoryID());

                        m_TimeTablePane.m_wndTab.ChangeWndLabel(it->first, TableName);
                        m_TimeTableListPane.m_TimeTableListCtrl.RefreshCurCategory();

                        m_TimeTablePane.m_wndTab.SetWndTimeTable(it->first, theTable);

                        break;
                    }
                }
            }

#ifndef SERVER_EDITION
            Audit tmp(GetControllerInterface->GetUserId(), S3Time::Now(), -1,
                S3Time::NULLTime, Audit::APPROVED);

            theTable.SetAudit(tmp);
#else
            Audit tmp(GetControllerInterface->GetUserId(), S3Time::Now(), -1,
                S3Time::NULLTime, Audit::CREATEBUTNOTAPPROVED);

            theTable.SetAudit(tmp);
#endif

            vector<INT> GroupIDList;
            CString TableName = theTable.GetTimetableName().c_str();
            CString Info;
            HRESULT hr = GetControllerInterface->GetTimetableRefInSchedule(theTable.GetTimetableID(), GroupIDList);
            if (SUCCEEDED(hr))
            {
                if (GroupIDList.size() > 0)
                {

                    CString szGroups;
                    for (auto it = GroupIDList.begin(); it != GroupIDList.end(); it++)
                    {
                        ClientGroup group;
                        hr = GetControllerInterface->GetClientGroup(*it, group);
                        if (SUCCEEDED(hr))
                        {
                            szGroups += (group.GetGroupName() + _T(";")).c_str();
                        }
                    }

                    Info.Format(Translate(_T("Failed to publish timetable (%s), It is used in the schedule of %d group(s)(%s)!")),
                        TableName, GroupIDList.size(), szGroups);

                    MessageBox(Info, Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);

                    return;
                }

            }
            else
            {
                Info.Format(Translate(_T("Failed to publish timetable(%s), unable to get the timetable referrence count in schedule!")),
                    TableName);

                MessageBox(Info,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);
                return;
            }

            hr = GetControllerInterface->EditTimetable(theTable);
            if (!SUCCEEDED(hr))
            {
                CString Info;
                Info.Format(Translate(_T("Failed to publish timetable %s.")), theTable.GetTimetableName().c_str());
                MessageBox(Info, Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);
            }
            else
            {
                PublishedTimeTable += (_T("  ") + theTable.GetTimetableName() + _T("\n")).c_str();

                m_TimetableCacheMap[theTable.GetTimetableID()] = theTable;

                m_TimeTablePane.m_wndTab.ClearTableModified(theTable.GetTimetableID());
            }
        }

        if (!PublishedTimeTable.IsEmpty())
        {
            CString Info;
            Info.Format(Translate(_T("Timetable (%s) published!")), PublishedTimeTable);
            MessageBox(Info, Translate(_T("Tips:Timetable page")), MB_OK);

            m_TimeTableListPane.m_TimeTableListCtrl.InitRootCategory();
        }
    }
    else
    {
        MessageBox(Translate(_T("Please open the timetable firstly")),Translate(_T("Warning:Timetable page")), MB_OK|MB_ICONEXCLAMATION);
    }
}

void CTimeTablePage::OnRibbonBtnCreate()
{
    Timetable newTable;
    INT myID = GetControllerInterface->GetUserId();

    newTable.SetCreateUserID(myID);
    newTable.SetParentFilterID(m_TimeTableListPane.m_TimeTableListCtrl.GetCurCategoryID());

    newTable.SetTimetableID(-1);
	newTable.SetTimetableName(Translate(_T("New TimeTable")).GetBuffer());

    m_TimeTablePane.m_wndTab.AddTimeTable(newTable, TRUE);
}

void CTimeTablePage::OnRibbonBtnClose()
{
    CloseCurTimetable();
}

void CTimeTablePage::OnRibbonBtnPublish()
{
    if (!m_TimeTablePane.m_wndTab.GetTheCtrl())
    {
        return;
    }

    Timetable theTable;
    if (m_TimeTablePane.m_wndTab.GetTheTimeTable(theTable))
    {
        if (theTable.GetTimetableID() < 0)
        {
            Timetable newTable;
            INT myID = GetControllerInterface->GetUserId();

            newTable.SetCreateUserID(myID);
            newTable.SetParentFilterID(m_TimeTableListPane.m_TimeTableListCtrl.GetCurCategoryID());


            INT TimeTableID, version;

            CString TableName;
            int TryCount = 1;
            while(TryCount < 100)
            {
                CString Title;
                Title.Format(Translate(_T("Add Timetable for '%s'")), theTable.GetTimetableName().c_str());
                CLayoutRenameCtrl Dlg(this, Title, theTable.GetTimetableName().c_str());
                if (Dlg.DoModal() == IDOK)
                {
                    Dlg.GetLayoutNewName(TableName);
                }
                else
                {
                    return;
                }

                newTable.SetTimetableName(TableName.GetString());

                HRESULT hr = GetControllerInterface->AddTimetable(newTable, TimeTableID, version);
                if (!SUCCEEDED(hr))
                {
                    if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                    {
                        TRACE0("Failed to add timetable\n");
                        GetControllerApp->OnControllerServerOffline();
                        return;
                    }

					std::tstring desc;
					int errorID = GetControllerInterface->GetLastError(desc);
					CString strdesc(desc.c_str());
					if(S3NET_TIMETABLEFILTER_NOT_FOUND == errorID)
					{
						MessageBox(Translate(_T("The selected timetable category is deleted, please refresh the timetable category!")),Translate(_T("Warning:Timetable page")), MB_OK|MB_ICONEXCLAMATION);
						return;
					}
					else if(S3NET_TIMETABLE_EXISTS == errorID ) 
					{
						//the tabletime is opened. show counld not been modified tips
						int index;
						if (m_TimeTablePane.m_wndTab.CheckTimeTableExist(TableName, &index))
						{
							CString messStr;
							messStr.Format(Translate(_T("The Timetable (%s) already exists, Given name can not be the same with the opened timetable.")), TableName);
							MessageBox(messStr,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);
							TryCount ++;
						}
						else
						{
							CString messStr;
                            // TRANS:
							messStr.Format(Translate(_T("The Timetable (%s) already exists, continue publish will update the existing data.")), TableName);
							messStr += _T("\r\n") +
								Translate(_T("Click")) + _T("\"")+ Translate(_T("OK")) + _T("\"") + Translate(_T("to continue publish, Click")) + _T("\"") +
								Translate(_T("Cancel")) + _T("\"") + Translate(_T("to cancel the publish."));
							if (MessageBox(messStr, Translate(_T("Warning:Timetable page")), MB_OKCANCEL) == IDOK)
							{
								theTable.SetTimetableID(TimeTableID);
								theTable.SetVersion(version);
								theTable.SetCreateUserID(myID);
								theTable.SetTimetableName(newTable.GetTimetableName());
								theTable.SetParentFilterID(newTable.GetParentFilterID());

								m_TimeTablePane.m_wndTab.ChangeCurLabel(TableName);
								m_TimeTablePane.m_wndTab.SetTheTimeTable(theTable);

								break;
							}
							else
							{
								return;
							}
						}						
					}
					else
					{
						MessageBox(Translate(strdesc), Translate(_T("Publish")), MB_OK);
						return;
					}
					
                }else
                {
                    theTable.SetTimetableID(TimeTableID);
                    theTable.SetCreateUserID(myID);
                    theTable.SetTimetableName(TableName.GetString());
                    theTable.SetParentFilterID(m_TimeTableListPane.m_TimeTableListCtrl.GetCurCategoryID());

                    m_TimeTablePane.m_wndTab.ChangeCurLabel(TableName);
                    m_TimeTableListPane.m_TimeTableListCtrl.RefreshCurCategory();

                    m_TimeTablePane.m_wndTab.SetTheTimeTable(theTable);

                    break;
                }
            }
        }

#ifndef SERVER_EDITION
        Audit tmp(GetControllerInterface->GetUserId(), S3Time::Now(), -1,
            S3Time::NULLTime, Audit::APPROVED);

        theTable.SetAudit(tmp);
#else
        Audit tmp(GetControllerInterface->GetUserId(), S3Time::Now(), -1,
            S3Time::NULLTime, Audit::CREATEBUTNOTAPPROVED);

        theTable.SetAudit(tmp);
#endif

        vector<INT> GroupIDList;
        CString TableName = theTable.GetTimetableName().c_str();
        CString Info;
        HRESULT hr = GetControllerInterface->GetTimetableRefInSchedule(theTable.GetTimetableID(), GroupIDList);
        if (SUCCEEDED(hr))
        {
            if (GroupIDList.size() > 0)
            {

                CString szGroups;
                for (auto it = GroupIDList.begin(); it != GroupIDList.end(); it++)
                {
                    ClientGroup group;
                    hr = GetControllerInterface->GetClientGroup(*it, group);
                    if (SUCCEEDED(hr))
                    {
                        szGroups += (group.GetGroupName() + _T(";")).c_str();
                    }
                }

                Info.Format(Translate(_T("Failed to publish timetable (%s), It is used in the schedule of %d group(s)(%s)!")),
                    TableName, GroupIDList.size(), szGroups);

                MessageBox(Info,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);

                return;
            }

        }
        else
        {
            Info.Format(Translate(_T("Failed to publish timetable(%s), unable to get the timetable referrence count in schedule!")),
                TableName);

            MessageBox(Info,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);
            return;
        }

		hr = GetControllerInterface->EditTimetable(theTable);
		if (!SUCCEEDED(hr))
		{
			MessageBox(Translate(_T("Failed to publish the timetable")),Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);
			return;
		}

        m_TimetableCacheMap[theTable.GetTimetableID()] = theTable;

        Info.Format(Translate(_T("Timetable %s published!")), theTable.GetTimetableName().c_str());
        MessageBox(Info, Translate(_T("Tips:Timetable page")), MB_OK);

        m_TimeTablePane.m_wndTab.ClearCurTabModified();

        m_TimeTableListPane.m_TimeTableListCtrl.InitRootCategory();
    }
    else
    {
         MessageBox(Translate(_T("Please open the timetable firstly")),Translate(_T("Warning:Timetable page")), MB_OK|MB_ICONEXCLAMATION);
    }

}

void CTimeTablePage::OnRibbonBtnCopy()
{
    CTimeTableCtrl* pCtrl = m_TimeTablePane.m_wndTab.GetTheCtrl();
    if (NULL == pCtrl)
        return;

    if (::OpenClipboard(GetSafeHwnd()))
    {
        ::EmptyClipboard();

        try
        {
            CSharedFile globalFile;

            if (pCtrl->SerializeTo(globalFile))
            {
                HGLOBAL hClipboardData =  globalFile.Detach();
                ::SetClipboardData(ScheduleCtrl::GetClipboardFormat(), hClipboardData);
            }

        }
        catch (COleException* pEx)
        {
            TRACE(_T("TimeTableCtrl DragDrop. OLE exception: %x\r\n"),
                pEx->m_sc);
            pEx->Delete ();
        }
        catch (CNotSupportedException *pEx)
        {
            TRACE(_T("TimeTableCtrl DragDrop. \"Not Supported\" exception\r\n"));
            pEx->Delete ();
        }

        ::CloseClipboard();
    }
}


void CTimeTablePage::OnRibbonBtnPaste()
{
    CTimeTableCtrl* pCtrl = m_TimeTablePane.m_wndTab.GetTheCtrl();
    if (NULL == pCtrl)
        return;

    if(::OpenClipboard(GetSafeHwnd()))
    {
        if(::IsClipboardFormatAvailable(ScheduleCtrl::GetClipboardFormat()) )
        {
            HGLOBAL hClipboardData = GetClipboardData(ScheduleCtrl::GetClipboardFormat());
            DWORD dwLength = GlobalSize(hClipboardData);

            CSharedFile globalFile;

            LPBYTE lpClipMemory = (LPBYTE)GlobalLock(hClipboardData);

            HANDLE hGlobalMemory = GlobalAlloc(GHND, dwLength + 1);
            LPBYTE lpGlobalMemory = (LPBYTE)GlobalLock(hGlobalMemory);
            memcpy(lpGlobalMemory, lpClipMemory, dwLength);
            GlobalUnlock(hGlobalMemory);
            globalFile.SetHandle(hGlobalMemory);

            GlobalUnlock(hClipboardData);

            if (pCtrl->SerializeFrom(globalFile,
                pCtrl->GetCurSelectedDate(), ScheduleCtrl::GetClipboardFormat()))
            {

            }

            GlobalFree(hGlobalMemory);
        }

        ::CloseClipboard();
    }
}

void CTimeTablePage::OnRibbonBtnDel()
{
    if (!m_TimeTablePane.m_wndTab.GetTheCtrl())
    {
        return;
    }

    m_TimeTablePane.m_wndTab.GetTheCtrl()->DelSelections();
}

void CTimeTablePage::OnRibbonBtnZoom()
{
    int delta = m_pComboBoxTimeDelta->GetItemData(m_pComboBoxTimeDelta->GetCurSel());

    m_TimeTablePane.m_wndTab.SetTimeDelta(delta);
}

void CTimeTablePage::OnRibbonBtnRequest()
{
    Timetable Table;
    if (!m_TimeTableListPane.m_TimeTableListCtrl.GetSelectedTimeTable(Table))
    {
        MessageBox(Translate(_T("You need select TimeTable in TimeTable category list first!")),Translate(_T("Warning:Timetable page")), MB_OK|MB_ICONEXCLAMATION);
        return;
    }


    Timetable serverTable;
    HRESULT hr = GetControllerInterface->GetTimetable(Table.GetTimetableID(), serverTable);
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to get timetable %d", Table.GetTimetableID());

        CString Info;
        Info.Format(Translate(_T("Failed to request approbation for timetable %d!")), Table.GetTimetableID());
        MessageBox(Info,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);

        return;
    }

    Audit tmp(GetControllerInterface->GetUserId(), S3Time::Now(), serverTable.GetAudit().GetApproveUserID(),
        serverTable.GetAudit().GetApproveTime(), Audit::CREATEBUTNOTAPPROVED);
    serverTable.SetAudit(tmp);
    hr = GetControllerInterface->EditTimetable(serverTable);
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to edit timetable %d", serverTable.GetTimetableID());

        CString Info;
        Info.Format(Translate(_T("Failed to request approbation for timetable %s!")), serverTable.GetTimetableName().c_str());
        MessageBox(Info,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);

        return;
    }

    m_TimetableCacheMap[serverTable.GetTimetableID()] = serverTable;

    m_TimeTablePropertyEditPane.GetCtrl()->SetTimeTable(serverTable);
    m_TimeTableListPane.m_TimeTableListCtrl.RefreshCurCategory();

    CString Info;
    Info.Format(Translate(_T("Request sent for timetable %s!")), serverTable.GetTimetableName().c_str());
    MessageBox(Info,Translate(_T("Tips:Timetable page")),MB_OK);

}

void CTimeTablePage::OnRibbonBtnReject()
{
    Timetable Table;
    if (!m_TimeTableListPane.m_TimeTableListCtrl.GetSelectedTimeTable(Table))
    {
        MessageBox(Translate(_T("You need select TimeTable in TimeTable category list first!")),Translate(_T("Warning:Timetable page")), MB_OK|MB_ICONEXCLAMATION);
        return;
    }

    CRejectReason dlg(this);

    if (IDOK == dlg.DoModal())
    {
        UpdateData(TRUE);

        Timetable serverTable;
        HRESULT hr = GetControllerInterface->GetTimetable(Table.GetTimetableID(), serverTable);
        if (!SUCCEEDED(hr))
        {
            TRACE1("Failed to get timetable %d", Table.GetTimetableID());

            CString Info;
            Info.Format(Translate(_T("Failed to reject timetable %d!")), Table.GetTimetableID());
            MessageBox(Info,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);

            return;
        }

        Audit tmp(serverTable.GetAudit().GetRequestUserID(), serverTable.GetAudit().GetRequestTime(), GetControllerInterface->GetUserId(),
            S3Time::Now(), Audit::REJECTED);

        tmp.SetComment(dlg.m_RejectReason.GetString());
        serverTable.SetAudit(tmp);
        hr = GetControllerInterface->AuditTimetable(serverTable.GetTimetableID(), serverTable.GetAudit());
        if (!SUCCEEDED(hr))
        {
            TRACE1("Failed to edit timetable %d", serverTable.GetTimetableID());

            CString Info;
            Info.Format(Translate(_T("Failed to reject timetable %s!")), serverTable.GetTimetableName().c_str());
            MessageBox(Info,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);

            return;
        }


        m_TimetableCacheMap[serverTable.GetTimetableID()] = serverTable;

        m_TimeTablePropertyEditPane.GetCtrl()->SetTimeTable(serverTable);
        m_TimeTableListPane.m_TimeTableListCtrl.RefreshCurCategory();

        CString Info;
        Info.Format(Translate(_T("Timetable %s has been rejected!")), serverTable.GetTimetableName().c_str());
        MessageBox(Info,Translate(_T("Tips:Timetable page")), MB_OK);
    }



}

void CTimeTablePage::OnRibbonBtnApprove()
{
    Timetable Table;
    if (!m_TimeTableListPane.m_TimeTableListCtrl.GetSelectedTimeTable(Table))
    {
        MessageBox(Translate(_T("You need select TimeTable in TimeTable category list first!")),Translate(_T("Warning:Timetable page")), MB_OK|MB_ICONEXCLAMATION);
        return;
    }

    Timetable serverTable;
    HRESULT hr = GetControllerInterface->GetTimetable(Table.GetTimetableID(), serverTable);
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to get timetable %d", Table.GetTimetableID());

        CString Info;
        Info.Format(Translate(_T("Failed to approve timetable %d!")), Table.GetTimetableID());
        MessageBox(Info,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);

        return;
    }

    Audit tmp(serverTable.GetAudit().GetRequestUserID(), serverTable.GetAudit().GetRequestTime(), GetControllerInterface->GetUserId(),
        S3Time::Now(), Audit::APPROVED);
    serverTable.SetAudit(tmp);
    hr = GetControllerInterface->AuditTimetable(serverTable.GetTimetableID(), serverTable.GetAudit());
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to edit timetable %d", serverTable.GetTimetableID());

        CString Info;
        Info.Format(Translate(_T("Failed to approve timetable %s!")), serverTable.GetTimetableName().c_str());
        MessageBox(Info,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);

        return;
    }


    m_TimetableCacheMap[serverTable.GetTimetableID()] = serverTable;

    m_TimeTablePropertyEditPane.GetCtrl()->SetTimeTable(serverTable);
    m_TimeTableListPane.m_TimeTableListCtrl.RefreshCurCategory();

    CString Info;
    Info.Format(Translate(_T("Timetable %s has been approved!")), serverTable.GetTimetableName().c_str());
    MessageBox(Info,Translate(_T("Tips:Timetable page")), MB_OK);

}

void CTimeTablePage::OnUpdateRibbonBtnDefault(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    //pCmdUI->Enable(TRUE);
}

void CTimeTablePage::OnUpdateRibbonBtnPreview(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    CTimeTableCtrl* pCtrl = m_TimeTablePane.m_wndTab.GetTheCtrl();
    if (NULL == pCtrl)
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    INT nSelectedCnt = pCtrl->GetSelectedAppointmentsCount();
    BOOL bPlayer = PlayerController::GetInstance()->IsPlayerRunning();
    if ((nSelectedCnt == 1) && !bPlayer)
    {
		POSITION pos = pCtrl->GetFirstSelectedAppointment();
		Appointment* pApp = pCtrl->GetNextSelectedAppointment(pos);
		ASSERT(pApp);

        if(pApp->GetDetailID() == S3NET_EMPTY_LAYOUT_ID || pApp->GetDetailID() == S3NET_TURN_OFF_LAYOUT_ID)
        {
			pCmdUI->Enable(FALSE);
        }else
        {
			pCmdUI->Enable(TRUE);
        }
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }


}

void CTimeTablePage::OnUpdateRibbonBtnCreate(CCmdUI* pCmdUI)
{
    //pCmdUI->Enable(TRUE);
	if (m_TimeTableListPane.m_TimeTableListCtrl.isCategorySelected())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTimeTablePage::OnUpdateRibbonBtnEdit(CCmdUI* pCmdUI)
{
    Timetable Table;
    if (m_TimeTableListPane.m_TimeTableListCtrl.GetSelectedTimeTable(Table))
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CTimeTablePage::OnUpdateRibbonBtnPublishAll(CCmdUI* pCmdUI)
{
    if (m_TimeTablePane.m_wndTab.IsTimetableModified())
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CTimeTablePage::OnUpdateRibbonBtnClose(CCmdUI* pCmdUI)
{
    if (!m_TimeTablePane.m_wndTab.GetTheCtrl())
    {
        pCmdUI->Enable(FALSE);
    }
    else
    {
        pCmdUI->Enable(TRUE);
    }
}

void CTimeTablePage::OnUpdateRibbonBtnPublish(CCmdUI* pCmdUI)
{
    if (m_TimeTablePane.m_wndTab.GetTheCtrl() && m_TimeTablePane.m_wndTab.GetTheCtrl()->IsModified())
    {
        m_TimeTablePane.m_wndTab.SetCurTabModified();

        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }

}
void CTimeTablePage::OnUpdateRibbonBtnCopy(CCmdUI* pCmdUI)
{
    if (m_TimeTablePane.m_wndTab.GetTheCtrl() && m_TimeTablePane.m_wndTab.GetTheCtrl()->GetSelectedAppointmentsCount() > 0)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CTimeTablePage::OnUpdateRibbonBtnPaste(CCmdUI* pCmdUI)
{
    if (m_TimeTablePane.m_wndTab.GetTheCtrl())
    {
        if(::OpenClipboard(GetSafeHwnd()))
        {
            if(::IsClipboardFormatAvailable(ScheduleCtrl::GetClipboardFormat()) )
            {
                pCmdUI->Enable(TRUE);
            }
            else
            {
                pCmdUI->Enable(FALSE);
            }
            ::CloseClipboard();
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

void CTimeTablePage::OnUpdateRibbonBtnDel(CCmdUI* pCmdUI)
{
    if (!m_TimeTablePane.m_wndTab.GetTheCtrl())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    INT n = m_TimeTablePane.m_wndTab.GetTheCtrl()->GetSelectedAppointmentsCount();

    if (n > 0)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CTimeTablePage::OnUpdateRibbonBtnZoom(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

void CTimeTablePage::OnUpdateRibbonBtnRequest(CCmdUI* pCmdUI)
{
    Timetable Table;
    if (m_TimeTableListPane.m_TimeTableListCtrl.GetSelectedTimeTable(Table)
     && Table.GetAudit().GetStatus() != Audit::CREATEBUTNOTAPPROVED)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}


void CTimeTablePage::OnUpdateRibbonBtnReject(CCmdUI* pCmdUI)
{
    Timetable Table;
    if (m_TimeTableListPane.m_TimeTableListCtrl.GetSelectedTimeTable(Table)
        && Table.GetAudit().GetStatus() != Audit::REJECTED)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}


void CTimeTablePage::OnUpdateRibbonBtnApprove(CCmdUI* pCmdUI)
{
    Timetable Table;
    if (m_TimeTableListPane.m_TimeTableListCtrl.GetSelectedTimeTable(Table)
        && Table.GetAudit().GetStatus() != Audit::APPROVED)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

BOOL CTimeTablePage::OpenTimetable(INT TimetableID)
{
    Timetable Table;
    BOOL bReadOnly = FALSE;

    vector<INT> GroupIDList;
    HRESULT hr = GetControllerInterface->GetTimetableRefInSchedule(TimetableID, GroupIDList);
    if (SUCCEEDED(hr))
    {
        if (GroupIDList.size() > 0)
        {
            bReadOnly = TRUE;
        }
    }
    else
    {

        TRACE1("Failed to GetTimetableRefInSchedule %d\n", TimetableID);
        MessageBox(Translate(_T("Failed to open timetable, unable to get the timetable referrence count in schedule!")),Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);
        return FALSE;
    }

    hr = GetControllerInterface->GetTimetable(TimetableID, Table);
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to get timetable %d\n", TimetableID);
        MessageBox(Translate(_T("Failed to open timetable !")),Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);
        return FALSE;
    }

    m_TimetableCacheMap[Table.GetTimetableID()] = Table;

    return m_TimeTablePane.m_wndTab.AddTimeTable(Table, FALSE, bReadOnly);
}

BOOL CTimeTablePage::CloseCurTimetable()
{
    Timetable Table;
    if (!m_TimeTablePane.m_wndTab.GetTheTimeTable(Table))
    {
        return FALSE;
    }

    auto it = m_TimetableCacheMap.find(Table.GetTimetableID());
    if (m_TimetableCacheMap.end() != it)
    {
        if (!(it->second == Table) && m_TimeTablePane.m_wndTab.GetTheCtrl()->IsModified())
        {
            if (MessageBox(Translate(_T("Do you want to save the changes to current timetable?")),Translate(_T("Tips:Timetable page")),
                MB_YESNO|MB_ICONINFORMATION) == IDYES)
            {
                OnRibbonBtnPublish();
            }
        }

        return m_TimeTablePane.m_wndTab.CloseTheTimeTable();
    }

    return TRUE;
}

void CTimeTablePage::CloseTimeTable(INT TableID, bool popMsgBox)
{
    if (!m_TimeTablePane.m_wndTab.IsTimeTableOpend(TableID))
    {
        return;
    }

    if (!popMsgBox)
    {
        m_TimeTablePane.m_wndTab.ClearTableModified(TableID);
        m_TimeTablePane.m_wndTab.CloseTimeTable(TableID);
        return;
    }

    auto it = m_TimetableCacheMap.find(TableID);
    if (m_TimetableCacheMap.end() != it)
    {
        if (m_TimeTablePane.m_wndTab.IsTimetableModified(TableID))
        {
            if (MessageBox(Translate(_T("Do you want to save the changes to the timetable?")),Translate(_T("Tips:Timetable page")),
                MB_YESNO|MB_ICONINFORMATION) == IDYES)
            {
                Timetable theTable;
                if (m_TimeTablePane.m_wndTab.GetTimeTable(TableID, theTable))
                {
                    Audit tmp(GetControllerInterface->GetUserId(), S3Time::Now(), -1,
                        S3Time::NULLTime, Audit::CREATEBUTNOTAPPROVED);

                    theTable.SetAudit(tmp);

                    vector<INT> GroupIDList;
                    CString TableName = theTable.GetTimetableName().c_str();
                    CString Info;
                    HRESULT hr = GetControllerInterface->GetTimetableRefInSchedule(theTable.GetTimetableID(), GroupIDList);
                    if (SUCCEEDED(hr))
                    {
                        if (GroupIDList.size() > 0)
                        {

                            CString szGroups;
                            for (auto it = GroupIDList.begin(); it != GroupIDList.end(); it++)
                            {
                                ClientGroup group;
                                hr = GetControllerInterface->GetClientGroup(*it, group);
                                if (SUCCEEDED(hr))
                                {
                                    szGroups += (group.GetGroupName() + _T(";")).c_str();
                                }
                            }

                            Info.Format(Translate(_T("Failed to publish timetable (%s), It is used in the schedule of %d group(s)(%s)!")),
                                TableName, GroupIDList.size(), szGroups);

                            MessageBox(Info,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);

                            return;
                        }

                    }
                    else
                    {
                        Info.Format(Translate(_T("Failed to publish timetable(%s), unable to get the timetable referrence count in schedule!")),
                            TableName);

                        MessageBox(Info,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);
                        return;
                    }

#ifndef SERVER_EDITION
                    Audit approved(GetControllerInterface->GetUserId(), S3Time::Now(), -1,
                        S3Time::NULLTime, Audit::APPROVED);

                    theTable.SetAudit(approved);
#endif

                    hr = GetControllerInterface->EditTimetable(theTable);
                    if (!SUCCEEDED(hr))
                    {
                        MessageBox(Translate(_T("Failed to publish the timetable")),Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);
                        return;
                    }

                    m_TimetableCacheMap[theTable.GetTimetableID()] = theTable;

                    Info.Format(Translate(_T("Timetable %s published!")), theTable.GetTimetableName().c_str());
                    MessageBox(Info,Translate(_T("Tips:Timetable page")),MB_OK);

                    m_TimeTablePane.m_wndTab.ClearTableModified(TableID);
                }
            }
        }

        m_TimeTablePane.m_wndTab.CloseTimeTable(TableID);

        m_TimeTableListPane.m_TimeTableListCtrl.InitRootCategory();
    }
}

LRESULT CTimeTablePage::OnTimeTableCategorySelChange (WPARAM wp, LPARAM lp)
{
    Timetable Table;
    if (m_TimeTableListPane.m_TimeTableListCtrl.GetSelectedTimeTable(Table))
    {

        Timetable serverTable;
        HRESULT hr = GetControllerInterface->GetTimetable(Table.GetTimetableID(), serverTable);
        if (!SUCCEEDED(hr))
        {
            TRACE1("Failed to get timetable %d", Table.GetTimetableID());

            CString Info;
            Info.Format(Translate(_T("Failed to request approbation for timetable %d!")), Table.GetTimetableID());
            MessageBox(Info,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);

            return 1;
        }

        m_TimeTablePropertyEditPane.GetCtrl()->SetTimeTable(serverTable);

        return 0;
    }
    else
    {
        m_TimeTablePropertyEditPane.GetCtrl()->ClearTimeTable();
    }

    return -1;

}

LRESULT CTimeTablePage::OnTimeTableCategoryDbClick (WPARAM wp, LPARAM lp)
{
    Timetable Table;
    if (m_TimeTableListPane.m_TimeTableListCtrl.GetSelectedTimeTable(Table))
    {
        if (!GetControllerInterface->CheckPrivilege(UserPrivilege_PublishTimetable))
        {
            MessageBox(Translate(_T("You don't have privilege to edit time table")),Translate(_T("Warning:Timetable page")), MB_OK|MB_ICONEXCLAMATION);
            return 0;
        }
        OpenTimetable(Table.GetTimetableID());

        return 0;
    }

    return -1;

}

LRESULT CTimeTablePage::OnTimeTableTabActiveChange (WPARAM wp, LPARAM lp)
{
    if (!m_TimeTablePane.m_wndTab.GetTheCtrl())
    {
        return -1;
    }

    Timetable Table;

    m_TimeTablePane.m_wndTab.GetTheTimeTable(Table);

    POSITION pos = m_TimeTablePane.m_wndTab.GetTheCtrl()->GetFirstSelectedAppointment();
    if (pos)
    {
        Appointment* pApp = m_TimeTablePane.m_wndTab.GetTheCtrl()->GetNextSelectedAppointment(pos);
        m_TimeTableLayoutPropertyPane.GetCtrl()->SetItem(pApp);
    }
    else
    {
        m_TimeTableLayoutPropertyPane.GetCtrl()->ClearItem();
    }

    int Delta = m_TimeTablePane.m_wndTab.GetTimeDelta();
    CString szDelta;
    
	if(Delta < 60)
    szDelta.Format(Translate(_T("%d sec")), Delta);
	else
	szDelta.Format(Translate(_T("%d min")), Delta/60);
	
	m_pComboBoxTimeDelta->SelectItem(szDelta);

    return 0;

}

LRESULT CTimeTablePage::OnTimeTableItemSelChange (WPARAM wp, LPARAM lp)
{
    Appointment* pApp = (Appointment*) lp;
    BOOL bSelect = (BOOL)wp;

    if (bSelect)
    {
         m_TimeTableLayoutPropertyPane.GetCtrl()->SetItem(pApp);
    }
    else
    {
         m_TimeTableLayoutPropertyPane.GetCtrl()->ClearItem();
    }

    return 0;

}


LRESULT CTimeTablePage::OnTimeTableItemPropertyChange (WPARAM wp, LPARAM lp)
{
    if (!m_TimeTablePane.m_wndTab.GetTheCtrl())
    {
        return -1;
    }

    POSITION pos = m_TimeTablePane.m_wndTab.GetTheCtrl()->GetFirstSelectedAppointment();
    if (pos)
    {
        Appointment* pApp = m_TimeTablePane.m_wndTab.GetTheCtrl()->GetNextSelectedAppointment (pos);

        COleDateTimeSpan Span = m_TimeTableLayoutPropertyPane.GetCtrl()->m_DurationTime;

        m_TimeTablePane.m_wndTab.GetTheCtrl()->UpdateAppointment(pApp, pApp->GetStart(),
            pApp->GetStart() + Span, TRUE, FALSE);

        OnTimeTableItemSelChange (TRUE, (LPARAM)pApp);
    }

    return -1;

}

void CTimeTablePage::PublishTab(INT iTab)
{
    Timetable theTable;
    if (m_TimeTablePane.m_wndTab.GetTabTimeTable(iTab, theTable))
    {
        if (theTable.GetTimetableID() < 0)
        {
            Timetable newTable;
            INT myID = GetControllerInterface->GetUserId();

            newTable.SetCreateUserID(myID);
            newTable.SetParentFilterID(m_TimeTableListPane.m_TimeTableListCtrl.GetCurCategoryID());


            INT TimeTableID, version;

            CString TableName;
            int TryCount = 1;
            while(TryCount < 100)
            {
                CLayoutRenameCtrl Dlg(this, Translate(_T("Add Timetable")), theTable.GetTimetableName().c_str());
                if (Dlg.DoModal() == IDOK)
                {
                    Dlg.GetLayoutNewName(TableName);
                }
                else
                {
                    return;
                }

                newTable.SetTimetableName(TableName.GetString());

                HRESULT hr = GetControllerInterface->AddTimetable(newTable, TimeTableID, version);
                if (!SUCCEEDED(hr))
                {
                    if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                    {
                        TRACE0("Failed to add timetable\n");
                        GetControllerApp->OnControllerServerOffline();
                        return;
                    }

					//the tabletime is opened. show counld not been modified tips
					int index;
					if (m_TimeTablePane.m_wndTab.CheckTimeTableExist(TableName, &index))
					{
						CString messStr;
						messStr.Format(Translate(_T("The Timetable (%s) already exists, Given name can not be the same with the opened timetable.")), TableName);
						MessageBox(messStr,Translate(_T("Warning:Timetable page")), MB_OK|MB_ICONEXCLAMATION);
						TryCount ++;
					}
					else
					{
						CString messStr;
                        // TRANS:
						messStr.Format(Translate(_T("The Timetable (%s) already exists, continue publish will update the existing data.")), TableName);
						messStr += _T("\r\n") +
							Translate(_T("Click")) + _T("\"")+ Translate(_T("OK")) + _T("\"") + Translate(_T("to continue publish, Click")) + _T("\"") +
							Translate(_T("Cancel")) + _T("\"") + Translate(_T("to cancel the publish."));
						if (MessageBox(messStr, Translate(_T("Warning:Timetable page")), MB_OKCANCEL) == IDOK)
						{
							theTable.SetTimetableID(TimeTableID);
							theTable.SetVersion(version);
							theTable.SetCreateUserID(myID);
							theTable.SetTimetableName(newTable.GetTimetableName());
							theTable.SetParentFilterID(newTable.GetParentFilterID());

							m_TimeTablePane.m_wndTab.ChangeCurLabel(TableName);
							m_TimeTablePane.m_wndTab.SetTheTimeTable(theTable);

							break;
						}
						else
						{
							return;
						}
					}

                }else
                {
                    theTable.SetTimetableID(TimeTableID);
                    theTable.SetCreateUserID(myID);
                    theTable.SetTimetableName(TableName.GetString());
                    theTable.SetParentFilterID(m_TimeTableListPane.m_TimeTableListCtrl.GetCurCategoryID());

                    m_TimeTablePane.m_wndTab.ChangeCurLabel(TableName);
                    m_TimeTableListPane.m_TimeTableListCtrl.RefreshCurCategory();

                    break;
                }
            }
        }

#ifndef SERVER_EDITION
        Audit tmp(GetControllerInterface->GetUserId(), S3Time::Now(), -1,
            S3Time::NULLTime, Audit::APPROVED);

        theTable.SetAudit(tmp);
#else
        Audit tmp(GetControllerInterface->GetUserId(), S3Time::Now(), -1,
            S3Time::NULLTime, Audit::CREATEBUTNOTAPPROVED);

        theTable.SetAudit(tmp);
#endif

        vector<INT> GroupIDList;
        CString TableName = theTable.GetTimetableName().c_str();
        CString Info;
        HRESULT hr = GetControllerInterface->GetTimetableRefInSchedule(theTable.GetTimetableID(), GroupIDList);
        if (SUCCEEDED(hr))
        {
            if (GroupIDList.size() > 0)
            {

                CString szGroups;
                for (auto it = GroupIDList.begin(); it != GroupIDList.end(); it++)
                {
                    ClientGroup group;
                    hr = GetControllerInterface->GetClientGroup(*it, group);
                    if (SUCCEEDED(hr))
                    {
                        szGroups += (group.GetGroupName() + _T(";")).c_str();
                    }
                }

                Info.Format(Translate(_T("Failed to publish timetable (%s), It is used in the schedule of %d group(s)(%s)!")),
                    TableName, GroupIDList.size(), szGroups);

                MessageBox(Info,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);

                return;
            }

        }
        else
        {
            Info.Format(Translate(_T("Failed to publish timetable(%s), unable to get the timetable referrence count in schedule!")),
                TableName);

            MessageBox(Info,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);
            return;
        }

        hr = GetControllerInterface->EditTimetable(theTable);
        if (!SUCCEEDED(hr))
        {
            MessageBox(Translate(_T("Failed to publish the timetable")), Translate(_T("Tips:Timetable page")), MB_OK);

            return;
        }


        m_TimetableCacheMap[theTable.GetTimetableID()] = theTable;

        Info.Format(Translate(_T("Timetable %s published!")), theTable.GetTimetableName().c_str());
        MessageBox(Info,Translate(_T("Tips:Timetable page")),MB_OK);

        m_TimeTablePane.m_wndTab.ClearCurTabModified();

        m_TimeTableListPane.m_TimeTableListCtrl.InitRootCategory();
    }
    else
    {
        MessageBox(Translate(_T("Please open the timetable firstly")),Translate(_T("Warning:Timetable page")), MB_OK|MB_ICONEXCLAMATION);
    }

}

LRESULT CTimeTablePage::OnTimeTableBeforeTabClose (WPARAM wp, LPARAM lp)
{
    Timetable* pTable = (Timetable*)wp;
    INT iTab = lp;

    if (m_TimeTablePane.m_wndTab.IsTablModified(iTab))
    {
        if (MessageBox(Translate(_T("Do you want to save the changes to the closing timetable?")),Translate(_T("Tips:Timetable page")),
            MB_YESNO|MB_ICONINFORMATION) == IDYES)
        {
            PublishTab(iTab);
        }

		m_TimeTableLayoutPropertyPane.GetCtrl()->ClearItem();
        return 0;
    }
	m_TimeTableLayoutPropertyPane.GetCtrl()->ClearItem();
    return -1;
}

LRESULT  CTimeTablePage::OnTimeTableTimedeltaChanged(WPARAM wp, LPARAM lp)
{
    int Delta = m_TimeTablePane.m_wndTab.GetTimeDelta();
    CString szDelta;
	if(Delta < 60) // in sec range.
		szDelta.Format(Translate(_T("%d sec")), Delta);
	else
		szDelta.Format(Translate(_T("%d min")), Delta/60);

    m_pComboBoxTimeDelta->SelectItem(szDelta);

    return 0;
}

LRESULT  CTimeTablePage::OnTimeTableKeyDown(WPARAM wp, LPARAM lp)
{
    UINT nChar = (UINT)wp;

    UINT nRepCnt = LOWORD(lp);
    UINT nFlags = HIWORD(lp);

    if (nChar == VK_DELETE)
    {
        OnRibbonBtnDel();
        return 0;
    }

    BOOL bControl = (nFlags & MK_CONTROL) != 0;
    if (bControl)
    {
        if (nChar == _T('c') || nChar == _T('C'))
        {
            OnRibbonBtnCopy();
            return 0;
        }
        else if (nChar == _T('v') || nChar == _T('V'))
        {
            OnRibbonBtnPaste();
            return 0;
        }
    }

    return 0;
}

void CTimeTablePage::OnUpdateRibbonBtn(CCmdUI* pCmdUI)
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
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_TIMETABLE_PREVIEWWINMODE, &CTimeTablePage::OnUpdateRibbonBtnPreview, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_PREVIEW_10X7, &CTimeTablePage::OnUpdateRibbonBtnDefault, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_PREVIEW_12X10, &CTimeTablePage::OnUpdateRibbonBtnDefault, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_PREVIEW_19X10, &CTimeTablePage::OnUpdateRibbonBtnDefault, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_TIMETABLE_CREATE, &CTimeTablePage::OnUpdateRibbonBtnCreate, LAMBDA(SC(PublishTimetable)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_TIMETABLE_CLOSE, &CTimeTablePage::OnUpdateRibbonBtnClose, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_TIMETABLE_PUBLISH, &CTimeTablePage::OnUpdateRibbonBtnPublish, LAMBDA(SC(PublishTimetable)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_TIMETABLE_COPY, &CTimeTablePage::OnUpdateRibbonBtnCopy, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_TIMETABLE_PASTE, &CTimeTablePage::OnUpdateRibbonBtnPaste, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_TIMETABLE_DEL, &CTimeTablePage::OnUpdateRibbonBtnDel, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_COMBOX_TIMETABLE_TIMEDELTA, &CTimeTablePage::OnUpdateRibbonBtnZoom, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_TIMETABLE_REQUEST, &CTimeTablePage::OnUpdateRibbonBtnRequest, LAMBDA(SC(PublishTimetable)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_TIMETABLE_REJECT, &CTimeTablePage::OnUpdateRibbonBtnReject, LAMBDA(SC(ApproveTimetableOperation)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_TIMETABLE_APPROVE, &CTimeTablePage::OnUpdateRibbonBtnApprove, LAMBDA(SC(ApproveTimetableOperation)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_TIMETABLE_EDIT, &CTimeTablePage::OnUpdateRibbonBtnEdit, LAMBDA(SC(PublishTimetable)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_TIMETABLE_DELTABLE, &CTimeTablePage::OnUpdateRibbonBtnEdit, LAMBDA(SC(DeleteTimetable)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_TIMETABLE_PUBLISH_ALL, &CTimeTablePage::OnUpdateRibbonBtnPublishAll, LAMBDA(SC(PublishTimetable)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_TIMETABLE_RENAME, &CTimeTablePage::OnUpdateRibbonBtnRename, LAMBDA(SC(PublishTimetable)));
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

void CTimeTablePage::OnRibbonBtnRename()
{
    Timetable Table;
    if (m_TimeTableListPane.m_TimeTableListCtrl.GetSelectedTimeTable(Table))
    {
		vector<INT> GroupIDList;
		HRESULT hResult = GetControllerInterface->GetTimetableRefInSchedule(Table.GetTimetableID(), GroupIDList);
		if (SUCCEEDED(hResult) && GroupIDList.size() != 0)
		{
			CString szGroups;
			CString Info;
			for (auto it = GroupIDList.begin(); it != GroupIDList.end(); it++)
			{
				ClientGroup group;
				hResult = GetControllerInterface->GetClientGroup(*it, group);
				if (SUCCEEDED(hResult))
				{
					szGroups += (group.GetGroupName() + _T(";")).c_str();
				}
			}
#ifndef STARTER_EDITION
			Info.Format(Translate(_T("Could not rename the selected timetable, It is used in the schedule of %d group(s)(%s).")),
				GroupIDList.size(), szGroups);
#else
			Info.Format(Translate(_T("Could not rename the selected timetable, It is used in the schedule.")));
#endif
			MessageBox(Info,Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);
			
			return;
		}

        if (m_TimeTablePane.m_wndTab.IsTimeTableOpend(Table.GetTimetableID()))
        {
            if (MessageBox(Translate(_T("The timetable is open, and renaming will close it, do you want to proceed?")),Translate(_T("Tips:Timetable page")), MB_YESNO) == IDNO)
            {
                return;
            }
        }

        CRenameCategory dlg(this);
        dlg.InitMedia(Table.GetTimetableName().c_str());
        if (dlg.DoModal() == IDOK)
        {
            Table.SetTimetableName(dlg.m_NewMediaName.GetString());

#ifndef SERVER_EDITION
            Audit tmp(GetControllerInterface->GetUserId(), S3Time::Now(), -1,
                S3Time::NULLTime, Audit::APPROVED);

            Table.SetAudit(tmp);
#endif

            HRESULT hr = GetControllerInterface->EditTimetable(Table);
            if (!SUCCEEDED(hr))
            {
                GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
                    (LPARAM)Translate(_T("Failed to rename the selected timetable!")).GetString());

				MessageBox(Translate(_T("Failed to rename the selected timetable!")),Translate(_T("Error:Timetable page")), MB_OK|MB_ICONERROR);

                m_TimeTableListPane.m_TimeTableListCtrl.RefreshCurCategory();
            }
            else
            {
                CloseTimeTable(Table.GetTimetableID());

                GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
                    (LPARAM)Translate(_T("Succeded to rename the selected timetable!")).GetString());

                m_TimeTableListPane.m_TimeTableListCtrl.RefreshCurCategory();
            }
        }
    }
}

void CTimeTablePage::OnUpdateRibbonBtnRename( CCmdUI* pCmdUI )
{
    Timetable Table;
    if (m_TimeTableListPane.m_TimeTableListCtrl.GetSelectedTimeTable(Table))
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}


void CTimeTablePage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CBaseControllerPage::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	if(bShow)
	{
		m_TimeTableListPane.m_TimeTableListCtrl.OnClickedFilterCategoryRefresh();
		m_TimeTableLayoutsPane.m_LayoutList.OnClickedFilterCategoryRefresh();
			 
	}
}
