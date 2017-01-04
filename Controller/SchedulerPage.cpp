// F:\p4\sw\Demo\ContentManagement\Controller\Scheduler\Scheduler.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "SchedulerPage.h"
#include "ScheduleHorizontallyCopyDlg.h"
#include "ScheduleVerticallyCopyDlg.h"
#include "ScheduleContinuousDeleteDlg.h"
#include "SchedulesSelectDlg.h"
#include "SchedulePackageHandler.h"
#include "PlayerController.h"
#include "ExportLayoutsDlg.h"
#include "PreviewDownLoadDlg.h"
#include "Utilities/SysCall.h"
#include "LayoutSettings.h"
#include "Localization/Localization.h"
#include "Utilities/ConfigFileStd.h"
#include "LayoutFileUtility.h"
#include "RemoveStorage.h"

#ifdef STARTER_EDITION
#include "client/ClientConfigFile.h"
#endif

#ifdef STANDALONE_EDITION
#include "ClientManagerAddGroupDlg.h"
#include "GroupInfoCtrl.h"
#include "EditGroup.h"
#endif

#define  LAYOUT_ROW 6
#define  LAYOUT_FILTER_SPLIT 8
#define  LAYOUT_CALENDER_SPLIT 2
#define  LAYOUT_FILTER_HEIGHT 100
#define  LAYOUTS_EXPORT_FOLDER   _T("\\Layouts\\")
#define  MEDIALIBRARY_EXPORT_FOLDER _T("\\MediaLibrary\\")

const int IdProgressDownloadMediaFiles  = 3001;
// CSchedulerPage

IMPLEMENT_DYNAMIC(CSchedulerPage, CBaseControllerPage)

CSchedulerPage::CSchedulerPage()
: m_pComboBoxTimeDelta (NULL)
, m_bAppendingSchedule (FALSE)
{
    m_ScheduleItemIconList.Create(THUMBNAIL_W, THUMBNAIL_H, ILC_COLOR32|ILC_MASK, 1, 1);

    m_ScheduleItemIconList.Add(AfxGetApp()->LoadIconW(IDI_EMPTYCONTENT));
#ifdef STARTER_EDITION
	m_pComboBoxPlayerRotation = NULL;
#endif
}


CSchedulerPage::~CSchedulerPage()
{

    PlayerController::GetInstance()->StopPlayer();
}

BEGIN_MESSAGE_MAP(CSchedulerPage, CBaseControllerPage)
    ON_WM_CREATE()
	ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_COMMAND(ID_RIBBON_BTN_SCHEDULE_GO, OnRibbonBtn_Go)
    ON_COMMAND(ID_RIBBON_BTN_SCHEDULE_SAVE, OnRibbonBtn_Save)
    ON_COMMAND(ID_RIBBON_BTN_SCHEDULE_PRINT, OnRibbonBtn_Print)
    ON_COMMAND(ID_RIBBON_BTN_SCHEDULE_EXPORT, OnRibbonBtn_Export)
    ON_COMMAND(ID_RIBBON_BTN_SCHEDULE_IMPORT, OnRibbonBtn_Import)
    ON_COMMAND(ID_RIBBON_BTN_SCHEDULE_SET_DEFAULT, OnRibbonBtn_SetDefault)
    ON_COMMAND(ID_RIBBON_BTN_SCHEDULE_REMOVE_DEFAULT, OnRibbonBtn_RemoveDefault)
    ON_COMMAND(ID_RIBBON_BTN_SCHEDULE_COPY, OnRibbonBtn_Copy)
    ON_COMMAND(ID_RIBBON_BTN_SCHEDULE_PASTE, OnRibbonBtn_Paste)
    ON_COMMAND(ID_RIBBON_BTN_SCHEDULE_DEL, OnRibbonBtn_Delete)
    ON_COMMAND(ID_RIBBON_BTN_SCHEDULE_HMULTICOPY, OnRibbonBtn_HMultiCopy)
    ON_COMMAND(ID_RIBBON_BTN_SCHEDULE_VMULTICOPY, OnRibbonBtn_VMultiCopy)
    ON_COMMAND(ID_RIBBON_BTN_SCHEDULE_MULTIDEL, OnRibbonBtn_MultiDelete)
    ON_COMMAND(ID_RIBBON_BTN_SCHEDULE_PREVIEW, OnRibbonBtn_Preview)
    ON_COMMAND(ID_RIBBON_BTN_SCHEDULE_ALIGNUP, OnRibbonBtn_AlignUp)
    ON_COMMAND(ID_RIBBON_BTN_SCHEDULE_ALIGNDOWN, OnRibbonBtn_AlignDown)
    ON_COMMAND(ID_RIBBON_COMBOX_SCHEDULE_TIMEDELTA, OnRibbonBtn_TimeDelta)
#ifdef STARTER_EDITION
    ON_COMMAND(ID_RIBBON_COMBOX_PLAYER_SETTING, OnRibbonBtn_PlayerSetting)
#endif

#ifdef STANDALONE_EDITION
	ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_ADDGROUP, OnRibbonBtn_AddGroup)
	ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_DELGROUP, OnRibbonBtn_DeleteGroup)
	ON_COMMAND(ID_RIBBON_BTN_CLTMGMT_EDITGROUP, OnRibbonBtn_EditGroup)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_ADDGROUP, OnUpdateRibbonBtnPrivilege)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_DELGROUP, OnUpdateRibbonBtnPrivilege)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_CLTMGMT_EDITGROUP, OnUpdateRibbonBtnPrivilege)
#endif

    ON_COMMAND(ID_RIBBON_BTN_PREVIEW_10X7, OnRibbonBtn_Preview10X7)
    ON_COMMAND(ID_RIBBON_BTN_PREVIEW_12X10, OnRibbonBtn_Preview12X10)
    ON_COMMAND(ID_RIBBON_BTN_PREVIEW_19X10, OnRibbonBtn_Preview19X12)

    // my own message handler

    ON_REGISTERED_MESSAGE(SCHEDULE_ITEM_UPDATE, OnScheduleItemUpdate)
    ON_REGISTERED_MESSAGE(SCHEDULE_ITEM_UPDATE_DURATION, OnScheduleItemUpdateDuration)

    ON_REGISTERED_MESSAGE(SCHEDULE_SELECT_APPOINTMENT, OnSelectScheduleItemChange)

    ON_REGISTERED_MESSAGE(SCHEDULE_BEFORE_UPDATE_APPOINTMENT, OnScheduleCtrlAppointmentPreChange)
    ON_REGISTERED_MESSAGE(SCHEDULE_ADD_APPOINTMENT, OnScheduleCtrlAppointmentChanged)
    ON_REGISTERED_MESSAGE(SCHEDULE_UPDATE_APPOINTMENT, OnScheduleCtrlAppointmentChanged)
    ON_REGISTERED_MESSAGE(SCHEDULE_REMOVE_APPOINTMENT, OnScheduleCtrlAppointmentChanged)

    ON_REGISTERED_MESSAGE(BORROW_CALENDAR_ON_SELCHANGED, OnSelChanged)

    ON_REGISTERED_MESSAGE(SCHEDULE_KEYDOWN, OnScheduleKeyDown)

    ON_REGISTERED_MESSAGE(SCHEDULE_TIME_DELTA_CHANGED, OnScheduleCtrlTimeDeltaChanged)

    ON_REGISTERED_MESSAGE(AFX_WM_CHANGE_ACTIVE_TAB, OnSchedulerTabActiveChange)


    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_SCHEDULE_GO, OnUpdateRibbonBtnPrivilege)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_SCHEDULE_SAVE, OnUpdateRibbonBtnPrivilege)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_SCHEDULE_COPY, OnUpdateRibbonBtnPrivilege)

    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_SCHEDULE_DEL, OnUpdateRibbonBtnPrivilege)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_SCHEDULE_HMULTICOPY, OnUpdateRibbonBtnPrivilege)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_SCHEDULE_VMULTICOPY, OnUpdateRibbonBtnPrivilege)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_SCHEDULE_MULTIDEL, OnUpdateRibbonBtnPrivilege)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_SCHEDULE_PREVIEW, OnUpdateRibbonBtnPrivilege)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_SCHEDULE_SET_DEFAULT, OnUpdateRibbonBtnPrivilege)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_SCHEDULE_REMOVE_DEFAULT, OnUpdateRibbonBtnPrivilege)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_SCHEDULE_ALIGNUP, OnUpdateRibbonBtnPrivilege)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_SCHEDULE_ALIGNDOWN, OnUpdateRibbonBtnPrivilege)
	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_SCHEDULE_PASTE, OnUpdateRibbonBtnPrivilege)


    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_SCHEDULE_PRINT, OnUpdateRibbonBtnPrivilege)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_SCHEDULE_EXPORT, OnUpdateRibbonBtnPrivilege)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_SCHEDULE_IMPORT, OnUpdateRibbonBtnPrivilege)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_COMBOX_SCHEDULE_TIMEDELTA, OnUpdateRibbonBtnPrivilege)
#ifdef STARTER_EDITION
    ON_UPDATE_COMMAND_UI(ID_RIBBON_COMBOX_PLAYER_SETTING, OnUpdateRibbonBtnPrivilege)
#endif
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_PREVIEW_10X7, OnUpdateRibbonBtnPrivilege)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_PREVIEW_12X10, OnUpdateRibbonBtnPrivilege)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_PREVIEW_19X10, OnUpdateRibbonBtnPrivilege)

	ON_NOTIFY(NM_DBLCLK, IDW_LIST_GROUP ,OnGroupScheduleEdit)

	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/// CSchedulerPage Message Handle
int CSchedulerPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

#ifndef STARTER_EDITION
    // Create and Install the calender pane
    left    = 0;
    top     = 0;
    right   = rect.Width() / LAYOUT_ROW;
    bottom  = rect.Height() / LAYOUT_CALENDER_SPLIT;

	if (!m_wndGroupList.Create(Translate(_T("Client Group List")), this, CRect(left, top, right, bottom),
		TRUE, ID_VIEW_GROUPLIST, WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_HIDE_INPLACE | WS_CAPTION,
		AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
	{
		TRACE0("Failed to create group list pane\n");
		return -1;      // fail to create
	}
#ifndef STANDALONE_EDITION
    m_wndGroupList.m_wndGroupList.ShowOfflineGroup();
#endif 
	m_wndGroupList.EnableDocking(CBRS_ALL);
	DockPane(&m_wndGroupList);

/*
    // Create and Install the scheduler item edit pane
    left    = 0;
    top     = rect.Height() / LAYOUT_CALENDER_SPLIT * (LAYOUT_CALENDER_SPLIT - 1);
	right   = rect.Width() / LAYOUT_ROW;
	bottom  = rect.bottom;

	if (!m_wndCalender.Create(Translate(_T("Calendar")),  this, CRect(left, top, right, bottom),
		TRUE, ID_VIEW_CALENDER, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM |CBRS_HIDE_INPLACE | WS_CAPTION,
		AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
	{
		TRACE0("Failed to create calender pane\n");
		return -1;      // fail to create
	}

	m_wndCalender.EnableDocking(CBRS_ALIGN_BOTTOM);
	m_wndCalender.DockToWindow(&m_wndGroupList, CBRS_ALIGN_BOTTOM);
*/
#else

    // Create and Install the scheduler item edit pane
/*
    left    = 0;
    top     = 0;
    right   = rect.Width() / LAYOUT_ROW;
    bottom  = rect.bottom;

    if (!m_wndCalender.Create(Translate(_T("Calendar")),  this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_CALENDER, WS_CHILD | WS_VISIBLE | CBRS_LEFT |CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create calender pane\n");
        return -1;      // fail to create
    }

    m_wndCalender.EnableDocking(CBRS_ALL);
    DockPane(&m_wndCalender);*/


#endif

    // Create and Install the layout list view pane

    left    = rect.Width() / LAYOUT_ROW * (LAYOUT_ROW - 1);
    top     = 0;
    right   = rect.right;
    bottom  = rect.Height() / LAYOUT_CALENDER_SPLIT;


    if (!m_wndSchedulerItemSelect.Create(Translate(_T("Schedule Item Select")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_LAYOUTFILTER, WS_CHILD | WS_VISIBLE | CBRS_RIGHT |CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create layout filter pane\n");
        return -1;      // fail to create
    }
    m_wndSchedulerItemSelect.EnableDocking(CBRS_ALIGN_RIGHT);
    DockPane(&m_wndSchedulerItemSelect);


	left    = rect.Width() / LAYOUT_ROW * (LAYOUT_ROW - 1);
	top     = rect.Height() / LAYOUT_CALENDER_SPLIT * (LAYOUT_CALENDER_SPLIT - 1);
	right   = rect.right;
	bottom  = rect.bottom;

	if (!m_wndScheduleItem.Create(Translate(_T("Scheduler Item Edit")), this, CRect(left, top, right, bottom),
		TRUE, ID_VIEW_SCHEDULEITEM, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM| CBRS_HIDE_INPLACE | WS_CAPTION,
		AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
	{
		TRACE0("Failed to create scheduler item edit pane\n");
		return -1;      // fail to create
	}
	m_wndScheduleItem.EnableDocking(CBRS_ALIGN_BOTTOM);
	m_wndScheduleItem.DockToWindow(&m_wndSchedulerItemSelect, CBRS_ALIGN_BOTTOM);


    // Create and Install the scheduler main wnd
    top     = rect.top;
    bottom  = rect.bottom;
    left   = rect.Width() / LAYOUT_ROW;
    right   = rect.Width() / LAYOUT_ROW * (LAYOUT_ROW - 1);

    CString SchedulePaneName = Translate(_T("Scheduler of Client Group:"));
#if defined(STARTER_EDITION)
	SchedulePaneName = Translate(_T("Scheduler"));
#endif

    if (!m_wndScheduer.Create(SchedulePaneName, this, CRect(left, top, right, bottom),
        TRUE, ID_SCHEDULER_WND, WS_CHILD | WS_VISIBLE | CBRS_ALL| CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create scheduler\n");
        return -1;      // fail to create
    }


    SetMainWnd(&m_wndScheduer);


#if defined(STARTER_EDITION)
    std::vector<ClientGroup> groupList;
    HRESULT hr = GetControllerInterface->GetAllClientGroup(groupList);
    if (!SUCCEEDED(hr)
		|| groupList.size() != 1)
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
        }
        else
        {
            TRACE0("Failed to Get Client Group!\n");
            MessageBox(Translate(_T("Error happen, Failed to initialize Schedule Page!")), Translate(_T("Error:Schedule page")), MB_OK|MB_ICONERROR);
        }
        return -1;
    }

    INT GroupID = groupList[0].GetGroupID();

    if (!m_wndScheduer.m_scheduleListTab.OpenScheduleTag(GroupID))
    {
        InitPage(GroupID);
    }

#endif

    return 0;
}


void CSchedulerPage::OnDestroy()
{
    CBaseControllerPage::OnDestroy();

}

void CSchedulerPage::Close()
{
	m_wndGroupList.DestroyWindow();
    m_wndScheduleItem.DestroyWindow();
    m_wndCalender.DestroyWindow();
    m_wndScheduer.DestroyWindow();
}

BOOL CSchedulerPage::InitPage(INT GroupID)
{
    ClientGroup clientGroup;
    CString sScheduleName;


	if (GroupID >= 0)
	{
        HRESULT hr;
        hr = GetControllerInterface->GetClientGroup(GroupID, clientGroup);

        if (!SUCCEEDED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                GetControllerApp->OnControllerServerOffline();
            }
            else
            {
                TRACE0("Failed to Get Client Group!\n");
            }
            return FALSE;
        }

        sScheduleName = clientGroup.GetGroupName().c_str();
	}
	else
	{
		sScheduleName = Translate(_T("Schedule for Offline"));
	}


    // Set the scheduler Timedelta
   // m_pComboBoxTimeDelta->SelectItem(10);
  //  OnRibbonBtn_TimeDelta();

	ScheduleCtrl* pScheduleCtrl = new ScheduleCtrl;

	CRect rect;
	GetClientRect(&rect);
    CRect tabrect;
    m_wndScheduer.m_scheduleListTab.GetTabsRect(tabrect);

    rect.top = tabrect.bottom;

	pScheduleCtrl->Create(NULL
		, sScheduleName
		, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL
		, rect
		, &(m_wndScheduer.m_scheduleListTab)
		, ID_TIMETABLE_WND);
	pScheduleCtrl->CreateButton();
	pScheduleCtrl->SetCalendar(m_wndCalender.GetMonthCalCtrl());

    if (GroupID >= 0)
    {

        // import the schedule of the selected group

        BeginApendSchedule();

        Schedule CurSchedule;

        COleDateTime now  = COleDateTime::GetCurrentTime();
        COleDateTime StartDate = COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay(), 0,0,0);
        COleDateTime EndDate = StartDate + COleDateTimeSpan(7, 23, 59, 59);

		std::vector<S3Time> evidence;
        HRESULT hr = GetControllerInterface->GetSchedule(S3Time::CreateTime(StartDate),
            S3Time::CreateTime(EndDate), CurSchedule, evidence, GroupID);

        if (!SUCCEEDED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                GetControllerApp->OnControllerServerOffline();
            }
            else
            {
                TRACE0("Failed to Get Schedule!\n");
            }
            EndApendSchedule();
            goto INIT_END;
        }

        std::vector<COleDateTime> Timestamps;
        if ((COleDateTimeSpan(EndDate - StartDate).GetDays()+1) != evidence.size())
        {
            TRACE0("Controller server issue, GetSchedule, Timestamps wrong!\n");
            ASSERT(0);
        }
        else
        {
            Timestamps.clear();
            for (auto it = evidence.begin(); it != evidence.end(); it++)
            {
                Timestamps.push_back(it->ToCOleDateTime());
            }
        }

        pScheduleCtrl->m_ScheduleRangeHolder.Clear();
        pScheduleCtrl->m_ScheduleRangeHolder.AppendNewQueriedRange(StartDate, EndDate, Timestamps);

        pScheduleCtrl->SetScheduleID(GroupID);

        int EntryCnt = CurSchedule.GetScheduleNum();

        pScheduleCtrl->RemoveAllAppointments();

        while (EntryCnt)
        {
            EntryCnt--;

            Appointment* pApp = new Appointment();
            ScheduleEntry Entry = CurSchedule.GetScheduleEntry(EntryCnt);
            Layout layout;
            GetControllerInterface->GetLayout(Entry.GetLayoutID(), layout);
            pApp->SetDescription(layout.GetLayoutName().c_str());
            pApp->SetInterval(Entry.GetStartTime().ToCOleDateTime(), Entry.GetEndTime().ToCOleDateTime());
            pApp->SetDetailID(Entry.GetLayoutID());
            pApp->SetDetailType(Appointment::APPOINTMENT_TYPE_LAYOUT);
            // UpdateIcon
            UINT Index = 0;
            if (!m_IconDetailIDMapper.Lookup(pApp->GetDetailID(), Index))
            {
                Layout layout;
                hr = GetControllerInterface->GetLayout(pApp->GetDetailID(), layout);
                if (SUCCEEDED(hr))
                {
                     Layout::thumbnail_t thumbnail;

                     GetControllerInterface->GetLayoutThumbnail(layout.GetLayoutID(), thumbnail);

					if (thumbnail.size() > 0)
					{
						HGLOBAL hGlobal  =  GlobalAlloc(GMEM_MOVEABLE, thumbnail.size() * sizeof(thumbnail[0]));
						void *  pData  =  GlobalLock(hGlobal);
						memcpy(pData, &thumbnail[0], thumbnail.size() * sizeof(thumbnail[0]));
						GlobalUnlock(hGlobal);

						IStream *  pStream  =  NULL;
						if  (CreateStreamOnHGlobal(hGlobal, TRUE,  & pStream)  ==  S_OK)
						{
							CImage image;
							if (SUCCEEDED(image.Load(pStream)))
							{
                                CImage ThumbImage;
                                ThumbImage.Create(THUMBNAIL_W, THUMBNAIL_H, 32);

                                HDC hDstDC = NULL;
                                BOOL bResult;

                                // Obtain a DC to the destination image
                                hDstDC = ThumbImage.GetDC();

                                CDC  cDC;
                                cDC.Attach(hDstDC);
                                cDC.FloodFill(0, 0, RGB(123,456,789));
                                cDC.Detach();

                                // Perform the blit
                                bResult = image.BitBlt( hDstDC, max(0, (THUMBNAIL_W - image.GetWidth()))/2,
                                    max(0, (THUMBNAIL_H - image.GetHeight()))/2,
                                    image.GetWidth(), image.GetHeight(), 0, 0 );

                                // Release the destination DC
                                ThumbImage.ReleaseDC();

                                HBITMAP hBitmap=ThumbImage.Detach();
                                CBitmap bmp;
                                bmp.Attach(hBitmap);

								Index = m_ScheduleItemIconList.Add(&bmp, RGB(123,456,789));
							}
							pStream -> Release();
						}
						GlobalFree(hGlobal);

						m_IconDetailIDMapper.SetAt(pApp->GetDetailID(), Index);
					}
                }
                else
                {
                    TRACE0("Failed to Get Layout thmbnail!\n");
                }

            }
            pApp->SetImage(&m_ScheduleItemIconList, Index);

            pScheduleCtrl->AddAppointment(pApp, FALSE, FALSE);
        }

        EntryCnt = CurSchedule.GetTimetableNum();

        while (EntryCnt)
        {
            EntryCnt--;

            Appointment* pApp = new Appointment();
            TimetableInSchedule Entry = CurSchedule.GetTimetable(EntryCnt);
            Timetable timetable;
            GetControllerInterface->GetTimetable(Entry.GetTimetableID(), timetable);
            pApp->SetDescription(timetable.GetTimetableName().c_str());
            pApp->SetInterval(Entry.GetStartTime().ToCOleDateTime(), Entry.GetEndTime().ToCOleDateTime());
            pApp->SetDetailID(Entry.GetTimetableID());
            pApp->SetDetailType(Appointment::APPOINTMENT_TYPE_TIMETABLE);

            // UpdateIco

            pApp->SetImage(&m_ScheduleItemIconList, 0);

            pScheduleCtrl->AddAppointment(pApp, FALSE, FALSE);
        }

        pScheduleCtrl->SetDateInterval(StartDate, EndDate);

        pScheduleCtrl->QueryAppointments(TRUE);

        EndApendSchedule();

        pScheduleCtrl->SetScheduleGroup(clientGroup);

        // default layout

        INT DefaultLayoutID = clientGroup.GetDefaultLayoutID();

        if (DefaultLayoutID > 0)
        {
            Layout layout;
            HRESULT hr = GetControllerInterface->GetLayout(DefaultLayoutID, layout);
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                GetControllerApp->OnControllerServerOffline();
                return FALSE;
            }

            Appointment* pApp = new Appointment();
            pApp->SetDescription(layout.GetLayoutName().c_str());
            pApp->SetDetailID(DefaultLayoutID);

            // UpdateIcon
            UINT Index = 0;
            if (!m_IconDetailIDMapper.Lookup(pApp->GetDetailID(), Index))
            {
                Layout::thumbnail_t thumbnail;

                GetControllerInterface->GetLayoutThumbnail(layout.GetLayoutID(), thumbnail);


			    if (thumbnail.size() > 0)
			    {
				    HGLOBAL hGlobal  =  GlobalAlloc(GMEM_MOVEABLE, thumbnail.size() * sizeof(thumbnail[0]));
				    void *  pData  =  GlobalLock(hGlobal);
				    memcpy(pData, &thumbnail[0], thumbnail.size() * sizeof(thumbnail[0]));
				    GlobalUnlock(hGlobal);

				    IStream *  pStream  =  NULL;
				    if  (CreateStreamOnHGlobal(hGlobal, TRUE,  & pStream)  ==  S_OK)
				    {
					    CImage image;
					    if (SUCCEEDED(image.Load(pStream)))
					    {
                            CImage ThumbImage;
                            ThumbImage.Create(THUMBNAIL_W, THUMBNAIL_H, 32);

                            HDC hDstDC = NULL;
                            BOOL bResult;

                            // Obtain a DC to the destination image
                            hDstDC = ThumbImage.GetDC();

                            CDC  cDC;
                            cDC.Attach(hDstDC);
                            cDC.FloodFill(0, 0, RGB(123,456,789));
                            cDC.Detach();

                            // Perform the blit
                            bResult = image.BitBlt( hDstDC, max(0, (THUMBNAIL_W - image.GetWidth()))/2,
                                max(0, (THUMBNAIL_H - image.GetHeight()))/2,
                                image.GetWidth(), image.GetHeight(), 0, 0 );

                            // Release the destination DC
                            ThumbImage.ReleaseDC();

                            HBITMAP hBitmap=ThumbImage.Detach();
                            CBitmap bmp;
                            bmp.Attach(hBitmap);

						    Index = m_ScheduleItemIconList.Add(&bmp, RGB(123,456,789));
					    }
					    pStream -> Release();
				    }
				    GlobalFree(hGlobal);

				    m_IconDetailIDMapper.SetAt(pApp->GetDetailID(), Index);
			    }

            }
            pApp->SetImage(&m_ScheduleItemIconList, Index);

            pScheduleCtrl->SetDefaultAppointment(pApp);

            delete pApp;
        }

    }

INIT_END:
	//---------------------by raymondLiu it's curious,crazy
	int sel = m_pComboBoxTimeDelta->GetCurSel();
	int delta = m_pComboBoxTimeDelta->GetItemData(m_pComboBoxTimeDelta->GetCurSel());
	//----------------------------------------------------

	m_wndScheduer.m_scheduleListTab.AddNewScheduleCtrl(pScheduleCtrl, GroupID);
	
	m_wndScheduer.m_scheduleListTab.InsertTab(pScheduleCtrl,
		sScheduleName,
		m_wndScheduer.m_scheduleListTab.GetTabsNum());

	m_wndScheduer.m_scheduleListTab.SetActiveTab(m_wndScheduer.m_scheduleListTab.GetTabsNum() - 1);

	//---------------------by raymondLiu it's curious,crazy
	m_pComboBoxTimeDelta->SelectItem(sel);
	m_wndScheduer.m_scheduleListTab.SetTimeDelta(delta);
	//----------------------------------------------------

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)(Translate(_T("Schedule Opened:")) + sScheduleName).GetString());
    return TRUE;
}

BOOL CSchedulerPage::CreateRibbonButtons()
{
    if (!m_pCategory)
    {
        return FALSE;
    }

    /// Remove the dummy panel, and create the real panels
	while (m_pCategory->GetPanelCount() > 0)
	{
		m_pCategory->RemovePanel(0);
	}

    CString strPanelFile     = Translate(_T("File"));
    CString strPanelEdit     = Translate(_T("Edit"));
    CString strPanelView     = Translate(_T("Preview"));
    CString strPanelScheduler     = Translate(_T("Scheduler View"));



	CMFCRibbonPanel* pPanelFile = m_pCategory->AddPanel(strPanelFile);

#if defined(STARTER_EDITION) || defined(STANDALONE_EDITION)
    CMFCRibbonButton* pBtnSave = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_SAVE, Translate(_T("Save")), 0, 0);
    pBtnSave->SetAlwaysLargeImage();
    pPanelFile->Add(pBtnSave);
#ifndef STANDALONE_EDITION
    CMFCRibbonButton* pBtnGo = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_GO, Translate(_T("Run")), 13, 13);
    pBtnGo->SetAlwaysLargeImage();
    pPanelFile->Add(pBtnGo);
#else
	CMFCRibbonButton* pBtnExport = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_EXPORT,  Translate(_T("Export")), 1, 1);
	pBtnExport->SetAlwaysLargeImage();
	pBtnExport->SetToolTipText(Translate(_T("Export")));
    pBtnExport->SetDescription(Translate(_T("Pack the schedule.")));
	pPanelFile->Add(pBtnExport);
	
    CMFCRibbonButton* pBtnImport = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_IMPORT,  Translate(_T("Import")), 2, 2);
    pBtnImport->SetAlwaysLargeImage();
    pBtnImport->SetToolTipText(Translate(_T("Import")));
    pBtnImport->SetDescription(Translate(_T("Open schedule package.")));
    pPanelFile->Add(pBtnImport);
#endif
#else


#ifndef STANDALONE_EDITION
	CMFCRibbonButton* pBtnSave = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_SAVE, Translate(_T("Publish")), 0, 0);
	pBtnSave->SetAlwaysLargeImage();
	pBtnSave->SetToolTipText(Translate(_T("Publish")));
    pBtnSave->SetDescription(Translate(_T("Publish the scheduler to client.")));
	pPanelFile->Add(pBtnSave);
#endif


	CMFCRibbonButton* pBtnExport = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_EXPORT,  Translate(_T("Export")), 1, 1);
	pBtnExport->SetAlwaysLargeImage();
	pBtnExport->SetToolTipText(Translate(_T("Export")));
    pBtnExport->SetDescription(Translate(_T("Pack the schedule.")));
	pPanelFile->Add(pBtnExport);

	CMFCRibbonButton* pBtnImport = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_IMPORT,  Translate(_T("Import")), 2, 2);
	pBtnImport->SetAlwaysLargeImage();
	pBtnImport->SetToolTipText(Translate(_T("Import")));
    pBtnImport->SetDescription(Translate(_T("Open schedule package.")));
	pPanelFile->Add(pBtnImport);

#endif

    /*CMFCRibbonButton* pBtnPrint = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_PRINT, _T("Print"), 3, -1);
    pBtnPrint->SetAlwaysLargeImage(FALSE);
    pPanelFile->Add(pBtnPrint);*/

    CMFCRibbonPanel* pPanelEdit = m_pCategory->AddPanel(strPanelEdit);

    CMFCRibbonButton* pBtnSet = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_SET_DEFAULT,  Translate(_T("Set Default")), 11, 11);
	pBtnSet->SetToolTipText(Translate(_T("Set Default")));
    pBtnSet->SetDescription(Translate(_T("Set the default schedule if no schedule set.")));
    pPanelEdit->Add(pBtnSet);

    CMFCRibbonButton* pBtnRemove = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_REMOVE_DEFAULT,  Translate(_T("Remove Default")), 12, 12);
    pBtnRemove->SetToolTipText(Translate(_T("Remove Default")));
    pBtnRemove->SetDescription(Translate(_T("Remove the default schedule.")));
	pPanelEdit->Add(pBtnRemove);

    CMFCRibbonButton* pBtnCopy = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_COPY,  Translate(_T("Copy")), 3, 3);
    pBtnCopy->SetAlwaysLargeImage();
	pBtnCopy->SetToolTipText(Translate(_T("Copy")));
    pBtnCopy->SetDescription(Translate(_T("Copy the selected layout cell.")));
    pPanelEdit->Add(pBtnCopy);

    CMFCRibbonButton* pBtnPaste = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_PASTE,  Translate(_T("Paste")), 4, 4);
    pBtnPaste->SetAlwaysLargeImage();
	pBtnPaste->SetToolTipText(Translate(_T("Paste")));
    pBtnPaste->SetDescription(Translate(_T("Paste the layout cell to the selected date.")));
    pPanelEdit->Add(pBtnPaste);

    CMFCRibbonButton* pBtnDel = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_DEL,  Translate(_T("Delete")), 5, 5);
    pBtnDel->SetAlwaysLargeImage();
	pBtnDel->SetToolTipText(Translate(_T("Delete")));
    pBtnDel->SetDescription(Translate(_T("Delete the selected layout cell.")));
    pPanelEdit->Add(pBtnDel);

    CMFCRibbonButton* pBtnHMultiCopy = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_HMULTICOPY,  Translate(_T("Horizontal Copy")), 6, 6);
    pBtnHMultiCopy->SetAlwaysLargeImage();
	pBtnHMultiCopy->SetToolTipText(Translate(_T("Horizontal Copy")));
    pBtnHMultiCopy->SetDescription(Translate(_T("Copy a layout cell horizontally.")));
    pPanelEdit->Add(pBtnHMultiCopy);

    CMFCRibbonButton* pBtnVMultiCopy = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_VMULTICOPY,  Translate(_T("Vertical Copy")), 7, 7);
    pBtnVMultiCopy->SetAlwaysLargeImage();
	pBtnVMultiCopy->SetToolTipText(Translate(_T("Vertical Copy")));
    pBtnVMultiCopy->SetDescription(Translate(_T("Copy a layout cell vertically.")));
    pPanelEdit->Add(pBtnVMultiCopy);

    //CMFCRibbonButton* pBtnMultiCopy = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_MULTIDEL, _T("Continuously Delete"), 5, -1);
    //pPanelEdit->Add(pBtnMultiCopy);

    CMFCRibbonButton* pBtnAlignUp = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_ALIGNUP,  Translate(_T("Align Up")), 8, 8);
    pBtnAlignUp->SetAlwaysLargeImage();
	pBtnAlignUp->SetToolTipText(Translate(_T("Align Up")));
    pBtnAlignUp->SetDescription(Translate(_T("The layout will set the start time and end time automatically aligned to the previous layout.")));
    pPanelEdit->Add(pBtnAlignUp);

    CMFCRibbonButton* pBtnAlignDown = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_ALIGNDOWN,  Translate(_T("Align Down")), 9, 9);
    pBtnAlignDown->SetAlwaysLargeImage();
	pBtnAlignDown->SetToolTipText(Translate(_T("Align Down")));
    pBtnAlignDown->SetDescription(Translate(_T("The layout will set the start time and end time automatically aligned to the next layout.")));
    pPanelEdit->Add(pBtnAlignDown);


    CMFCRibbonPanel* pPanelView = m_pCategory->AddPanel(strPanelView);

    //CMFCRibbonButton* pBtnPreviewWin = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_PREVIEWWINMODE, _T("Preview Window Mode"), 0, 0);
    //pPanelView->Add(pBtnPreviewWin);

    CMFCRibbonButton* pBtnPreview = new CMFCRibbonButton(ID_RIBBON_BTN_SCHEDULE_PREVIEW,  Translate(_T("Preview")), 10, 10);
    pBtnPreview->AddSubItem(new CMFCRibbonButton(ID_RIBBON_BTN_PREVIEW_10X7, _T("1024 x 768")));
    pBtnPreview->AddSubItem(new CMFCRibbonButton(ID_RIBBON_BTN_PREVIEW_12X10, _T("1280 x 1024")));
    pBtnPreview->AddSubItem(new CMFCRibbonButton(ID_RIBBON_BTN_PREVIEW_19X10, _T("1920 x 1080")));
    pBtnPreview->SetToolTipText(Translate(_T("Preview")));
    pBtnPreview->SetDescription(Translate(_T("Preview the selected layout cell.")));
	pPanelView->Add(pBtnPreview);

    m_pComboBoxTimeDelta = new CMFCRibbonComboBox(ID_RIBBON_COMBOX_SCHEDULE_TIMEDELTA, FALSE, 75,  Translate(_T("Zoom")));

    m_pComboBoxTimeDelta->AddItem(Translate(_T("60 min")), 60*60);
    m_pComboBoxTimeDelta->AddItem(Translate(_T("30 min")), 30*60);
	m_pComboBoxTimeDelta->AddItem(Translate(_T("15 min")), 15*60);
    m_pComboBoxTimeDelta->AddItem(Translate(_T("10 min")), 10*60);
    m_pComboBoxTimeDelta->AddItem(Translate(_T("6 min")), 6*60);
	m_pComboBoxTimeDelta->AddItem(Translate(_T("4 min")), 4*60);
    m_pComboBoxTimeDelta->AddItem(Translate(_T("1 min")), 1*60);

	m_pComboBoxTimeDelta->AddItem(Translate(_T("30 sec")), 30);
	m_pComboBoxTimeDelta->AddItem(Translate(_T("5 sec")), 5);
//	m_pComboBoxTimeDelta->AddItem(Translate(_T("1 sec")), 1);
#if 0
	m_pComboBoxTimeDelta->SelectItem(5/*m_pComboBoxTimeDelta->GetCount() -1*/);
#else
	m_pComboBoxTimeDelta->SelectItem(0);
#endif
    CMFCRibbonPanel* pPanelScheduler = m_pCategory->AddPanel(strPanelScheduler);
	m_pComboBoxTimeDelta->SetToolTipText(Translate(_T("Zoom")));
    m_pComboBoxTimeDelta->SetDescription(Translate(_T("Change the Zoom rate of the schedule page.")));
    pPanelScheduler->Add(m_pComboBoxTimeDelta);

#ifdef STARTER_EDITION
	m_pComboBoxPlayerRotation = new CMFCRibbonComboBox(ID_RIBBON_COMBOX_PLAYER_SETTING, FALSE, 75, Translate(_T("Rotation")));
	m_pComboBoxPlayerRotation->AddItem(Translate(_T("0 Degrees")), 0);
	m_pComboBoxPlayerRotation->AddItem(Translate(_T("90 Degrees")), 1);
	m_pComboBoxPlayerRotation->AddItem(Translate(_T("180 Degrees")), 2);
	m_pComboBoxPlayerRotation->AddItem(Translate(_T("270 Degrees")), 3);

	ClientConfigFile ClientPlayConfig; // = ClientConfigFile::GetConfigFile();
	ClientPlayConfig.Load(GetClientDefaultDir() + CLIENT_CONFIG_FILE);
	INT iSelRotation = ClientPlayConfig.GetN(CLIENT_ROTATEDEGREE) / 90;
	if (iSelRotation < 0 || iSelRotation > 3)
	{
		iSelRotation = 0;
	}

	m_pComboBoxPlayerRotation->SelectItem(iSelRotation);
	CString strPlaySetting = Translate(_T("Player Setting"));
	CMFCRibbonPanel* pPlaySetting = m_pCategory->AddPanel(strPlaySetting);
	m_pComboBoxPlayerRotation->SetToolTipText(Translate(_T("Rotation")));
	m_pComboBoxPlayerRotation->SetDescription(Translate(_T("Change Play Degree.")));
	pPlaySetting->Add(m_pComboBoxPlayerRotation);
#endif

#ifdef STANDALONE_EDITION
	CMFCRibbonPanel* pPanelGroupEdit = m_pCategory->AddPanel(Translate(_T("Group Edit")));
	
	CMFCRibbonButton* pBtnAddGroup = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_ADDGROUP, Translate(_T("Add Group")), 14, 14);
	pBtnAddGroup->SetAlwaysLargeImage();
	pBtnAddGroup->SetToolTipText(Translate(_T("Add Group")));
	pBtnAddGroup->SetDescription(Translate(_T("Add a new group.")));
	pPanelGroupEdit->Add(pBtnAddGroup);

	CMFCRibbonButton* pBtnDelGroup = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_DELGROUP, Translate(_T("Delete Group")), 15, 15);
	pBtnDelGroup->SetAlwaysLargeImage();
	pBtnDelGroup->SetToolTipText(Translate(_T("Delete Group")));
	pBtnDelGroup->SetDescription(Translate(_T("Delete the selected group.")));
	pPanelGroupEdit->Add(pBtnDelGroup);

	CMFCRibbonButton* pBtnEditGroup = new CMFCRibbonButton(ID_RIBBON_BTN_CLTMGMT_EDITGROUP, Translate(_T("Edit Group")), 14, 14);
	pBtnEditGroup->SetAlwaysLargeImage();
	pBtnEditGroup->SetToolTipText(Translate(_T("Edit Group")));
	pBtnEditGroup->SetDescription(Translate(_T("Edit the selected group.")));
	pPanelGroupEdit->Add(pBtnEditGroup);
#endif

    return TRUE;
}


void CSchedulerPage::OnRibbonBtn_Save()
{
/*
#ifdef STANDALONE_EDITION
    OnRibbonBtn_Export();
    return;
#endif*/

	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return;

    INT ID = pScheduleCtrl->GetScheduleID();

    if (ID < 0)
    {
        MessageBox(Translate(_T("Please open the Schedule first!")), Translate(_T("Warning:Save schedule")), MB_OK|MB_ICONEXCLAMATION);
        return;
    }
//--------------------by raymondliu-----------------------------------
	SchedulePackageHandler PackageHander;

	pScheduleCtrl->QueryAll();

	AppointmentArray DefaultAppArray;
	DefaultAppArray.RemoveAll();
	Appointment* pDefaultApp = const_cast<Appointment*>(pScheduleCtrl->GetDefaultAppointment());
	if (pDefaultApp)
	{
		DefaultAppArray.Add(pDefaultApp);
	}
	AppointmentArray  &AppArray = pScheduleCtrl->GetQueryedAppointments();


	DWORD64 LayoutSize = 0;

	std::vector<Layout> LayoutBuf;
	std::vector<Timetable> TimetableBuf;

	CMap<INT, INT&, INT, INT&> ExportLayouts;
	CMap<INT, INT&, INT, INT&> ExportTimeTables;

	if (pDefaultApp)
	{
		INT Value, Key = pDefaultApp->GetDetailID();

		if(pDefaultApp->GetDetailType() == Appointment::APPOINTMENT_TYPE_LAYOUT)
		{
			if (!ExportLayouts.Lookup(Key, Value))
			{
				ExportLayouts.SetAt(Key, Value);
				Layout layout;
				HRESULT hr = GetControllerInterface->GetLayout(Key, layout);
				if (!SUCCEEDED(hr))
				{
					tstring ErrorDesc;
					GetControllerInterface->GetLastError(ErrorDesc);
					CString szname;
					szname.Format(_T("[%s]"), pDefaultApp->GetDescription().GetString());
					MessageBox(szname + Translate(ErrorDesc.c_str()) , Translate(_T("Warning:Schedule page")), MB_OK|MB_ICONWARNING);
					return;
				}

				if (!layout.GetLocation().empty())
				{
					LayoutBuf.push_back(layout);
					LayoutSize += layout.GetLayoutSize();
				}
			}
		}
		else
		{
			if (!ExportTimeTables.Lookup(Key, Value))
			{
				ExportTimeTables.SetAt(Key, Value);
				Timetable timetable;
				HRESULT hr = GetControllerInterface->GetTimetable(Key, timetable);
				if (!SUCCEEDED(hr))
				{
					tstring ErrorDesc;
					GetControllerInterface->GetLastError(ErrorDesc);
					CString szname;
					szname.Format(_T("[%s]"), pDefaultApp->GetDescription().GetString());
					MessageBox(szname + Translate(ErrorDesc.c_str()), Translate(_T("Warning:Schedule page")), MB_OK|MB_ICONWARNING);
					return;
				}
				TimetableBuf.push_back(timetable);
			}
		}
	}

	for (int i = 0; i < AppArray.GetSize(); i++)
	{
		INT Value, Key = AppArray[i]->GetDetailID();
		if(AppArray[i]->GetDetailType() == Appointment::APPOINTMENT_TYPE_LAYOUT)
		{
			if (!ExportLayouts.Lookup(Key, Value))
			{
				ExportLayouts.SetAt(Key, Value);
				Layout layout;
				HRESULT hr = GetControllerInterface->GetLayout(Key, layout);
				if (!SUCCEEDED(hr))
				{
					tstring ErrorDesc;
					GetControllerInterface->GetLastError(ErrorDesc);
					CString szname;
					szname.Format(_T("[%s]"), AppArray[i]->GetDescription().GetString());
					MessageBox(szname + Translate(ErrorDesc.c_str()), Translate(_T("Warning:Schedule page")), MB_OK|MB_ICONWARNING);
					return;
				}

				if (!layout.GetLocation().empty())
				{
					LayoutBuf.push_back(layout);
					LayoutSize += layout.GetLayoutSize();
				}
			}
		}
		else
		{
			if (!ExportTimeTables.Lookup(Key, Value))
			{
				ExportTimeTables.SetAt(Key, Value);
				Timetable timetable;
				HRESULT hr = GetControllerInterface->GetTimetable(Key, timetable);
				if (!SUCCEEDED(hr))
				{
					tstring ErrorDesc;
					GetControllerInterface->GetLastError(ErrorDesc);
					CString szname;
					szname.Format(_T("[%s]"), AppArray[i]->GetDescription().GetString());

					MessageBox(szname + Translate(ErrorDesc.c_str()), Translate(_T("Warning:Schedule page")), MB_OK|MB_ICONWARNING);
					return;
				}
				TimetableBuf.push_back(timetable);
			}
		}
	}

	for(size_t i=0; i< TimetableBuf.size(); i++)
	{
		auto it = TimetableBuf[i].LayoutEntryList.begin();
		while(it != TimetableBuf[i].LayoutEntryList.end())
		{
			INT Value, Key = it->GetLayoutID();

			if (!ExportLayouts.Lookup(Key, Value))
			{
				ExportLayouts.SetAt(Key, Value);
				Layout layout;
				GetControllerInterface->GetLayout(Key, layout);

				if (!layout.GetLocation().empty())
				{
					LayoutBuf.push_back(layout);
					LayoutSize += layout.GetLayoutSize();
				}
			}
			it++;
		}
	}

	INT LayoutSizeMB = static_cast<INT>(LayoutSize>>20);
	
	// Put all the clients to Default Group
	CString Conditons;
	Conditons.Format( _T("GroupId =  %d"), ID);
	Condition cond(Conditons.GetString());

	std::vector<int> clientIDList;

	HRESULT hr = GetControllerInterface->SearchClient(cond, clientIDList);
	if (!SUCCEEDED(hr))
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
		{
			GetControllerApp->OnControllerServerOffline();
		}
		else
		{
			TRACE0("Failed to Search Client!\n");
		}
		return;
	}
	for (std::vector<int>::iterator iter = clientIDList.begin(); iter != clientIDList.end(); ++iter)
	{
		ClientRTStatistics clientrtstatus;
		hr = GetControllerInterface->GetClientRTStatistics(*iter,  clientrtstatus);
		ClientInfo clientInfo;
		HRESULT hr2 = GetControllerInterface->GetClient(*iter, clientInfo);

		if (SUCCEEDED(hr) && SUCCEEDED(hr2))
		{
			if(clientrtstatus.IsOnline())
			{
				SYSTEM_STATUS_DESC sysStatusDesc;
				clientrtstatus.GetSysStatusDesc(sysStatusDesc);
				
				if( sysStatusDesc.DiskFreeSpace < LayoutSizeMB + 1)
				{
					CString cWarning, str1, str2 ,str3;
					str1.Format(_T("%d MB"),LayoutSizeMB);
					str2.Format(_T("%d MB"),sysStatusDesc.DiskFreeSpace);
					str3 = clientInfo.GetClientDesc().c_str();
                    cWarning.Format(Translate(_T("The schedule package (%s) may be too big for the client's disk(Free size: %s). Can't export to %s , do you want to continue?")), str1.GetString(), str2.GetString(), str3.GetString());
/*
					cWarning = Translate(_T("The schedule package(")) + str1 +
						Translate(_T(")may be too big for this disk(Free size: ")) +
						str2 + Translate(_T("), Can't export to ")) + str3 + Translate(_T(", do you want to continue?"));
*/
					if(MessageBox(cWarning, Translate(_T("Warning:Schedule page")),MB_YESNO|MB_ICONINFORMATION) != IDYES)
					{
						return;
					}
				}
			}
		}
		else
		{
			if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
			{
				GetControllerApp->OnControllerServerOffline();
			}
			else
			{
				TRACE0("Failed to Get Client!\n");
			}
		}
	}
//--------------------------------------------------------------

    std::vector<int> GroupIdlist;
    GroupIdlist.push_back(ID);

    UINT RangeCnt = pScheduleCtrl->m_ScheduleRangeHolder.GetModifiedRangeCount();

    std::vector<S3Time> TotalconflictDays;

    COleDateTime Start;
    COleDateTime End;
    BOOL bConflict = FALSE;
    while (RangeCnt)
    {
        Schedule CurSchedule;
        RangeCnt--;

        pScheduleCtrl->m_ScheduleRangeHolder.GetModifiedRange(RangeCnt, Start, End);

		COleDateTime PublishStart(Start.GetYear(), Start.GetMonth(), Start.GetDay(), 0, 0, 0);
		COleDateTime PublishEnd(End.GetYear(), End.GetMonth(), End.GetDay(), 23, 59, 59);
		COleDateTimeSpan OneDay(1, 0, 0, 0);

        pScheduleCtrl->QueryAppointments(Start, End);

        AppointmentArray& arApps = pScheduleCtrl->GetQueryedAppointments();

        for (int i = 0; i < arApps.GetSize (); i++)
        {
            ScheduleEntry sEntry;
            TimetableInSchedule tEntry;

            if (arApps[i]->GetStart() > arApps[i]->GetFinish())
            {
                continue;
            }

			if (arApps[i]->GetFinish() > PublishEnd)
			{
				PublishEnd = min(PublishEnd, arApps[i]->GetStart());
				continue;
			}

			if (arApps[i]->GetStart() < PublishStart && arApps[i]->GetFinish() >=  PublishStart)
			{
				PublishStart = max(PublishStart, arApps[i]->GetFinish());
				continue;
			}

            if(arApps[i]->GetDetailType() == Appointment::APPOINTMENT_TYPE_LAYOUT)
            {
                sEntry.SetStartTime(S3Time::CreateTime(arApps[i]->GetStart()));
                sEntry.SetEndTime(S3Time::CreateTime(arApps[i]->GetFinish()));
                sEntry.SetLayoutID(arApps[i]->GetDetailID());

                CurSchedule.AddScheduleEntry(sEntry);
            }

            if(arApps[i]->GetDetailType() == Appointment::APPOINTMENT_TYPE_TIMETABLE)
            {
                tEntry.SetStartTime(S3Time::CreateTime(arApps[i]->GetStart()));
                tEntry.SetEndTime(S3Time::CreateTime(arApps[i]->GetFinish()));
                tEntry.SetTimetableID(arApps[i]->GetDetailID());

                CurSchedule.AddTimetable(tEntry);
            }
        }

		std::vector<S3Time> evidence;

        COleDateTime StartTemp(PublishStart.GetYear(), PublishStart.GetMonth(), PublishStart.GetDay(), 0, 0, 0);
        COleDateTime EndTemp(PublishEnd.GetYear(), PublishEnd.GetMonth(), PublishEnd.GetDay(), 23, 59, 59);

        for (COleDateTime it = StartTemp; it < EndTemp; it += OneDay)
        {
            evidence.push_back(S3Time::CreateTime(pScheduleCtrl->m_ScheduleRangeHolder.GetTimeStamp(it)));
        }

		std::vector<S3Time> conflictDays;
        std::tstring errName;
        HRESULT hr = GetControllerInterface->PublishSchedule(evidence, CurSchedule,
            S3Time::CreateTime(PublishStart), S3Time::CreateTime(PublishEnd), ID, conflictDays, errName);
        if (!SUCCEEDED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                GetControllerApp->OnControllerServerOffline();

                return;
            }
            else if (conflictDays.size() > 0)
            {
                TotalconflictDays;
                for (auto it = conflictDays.begin(); it != conflictDays.end(); it++)
                {
                    TotalconflictDays.push_back(*it);
                }

                bConflict = TRUE;
            }
            else
            {
                TRACE0("Failed to Publish Schedule!\n");
                std::tstring desc;
                int err = GetControllerInterface->GetLastError(desc);

                CString msg = Translate(_T("Failed to publish schedule: ")) + _T(" ") + Translate(desc.c_str()) +_T("(") + errName.c_str() + _T(")");

                MessageBox(msg, Translate(_T("Error:Schedule page")), MB_OK|MB_ICONERROR);
                return;
            }
        }
    }

    if (bConflict)
    {
        CString strConflictDays;
		CString tmpstr;
        for (auto it = TotalconflictDays.begin(); it != TotalconflictDays.end(); it++)
        {
            COleDateTime tm = it->ToCOleDateTime();
			tmpstr.Format(_T("%04d/%02d/%02d; "),tm.GetYear(),tm.GetMonth(),tm.GetDay());
			strConflictDays += tmpstr;
        }


		CString mess = Translate(_T("There are some days' schedule conflict with the server data,\nmaybe somebody else has edit it before you, so you need to re-edit them.\nThe conflict days are: ")) + strConflictDays;
        MessageBox(mess, Translate(_T("Warning:Schedule page")), MB_OK|MB_ICONINFORMATION);

    }

    pScheduleCtrl->m_ScheduleRangeHolder.Clear();

    pScheduleCtrl->RemoveAllAppointments(FALSE);

    COleDateTime date1;
    COleDateTime date2;
	/*
    CList<DATE, DATE&> lstDates;

    m_wndCalender.GetMonthCalCtrl()->GetSelectedDates(lstDates);
    CBORROW_Calendar::GetMinMaxSelection (lstDates, date1, date2);
	*/
	date1 = m_wndScheduer.GetScheduleCtrl()->GetStartTime();
	date2 = m_wndScheduer.GetScheduleCtrl()->GetEndTime();

    UpdateSchedule(date1, date2);

    SetDateInterval (date1, date2);

    pScheduleCtrl->QueryAppointments(TRUE);


    // default layout
    INT DefaultLayoutID = S3NET_DEFAULT_LAYOUTGROUP_ID;
    if (pScheduleCtrl->GetDefaultAppointment())
    {
        DefaultLayoutID = pScheduleCtrl->GetDefaultAppointment()->GetDetailID();

    }

    if (pScheduleCtrl->GetScheduleGroup().GetDefaultLayoutID() != DefaultLayoutID)
    {
        pScheduleCtrl->GetScheduleGroup().SetDefaultLayoutID(DefaultLayoutID);
        HRESULT hr = GetControllerInterface->EditClientGroup(pScheduleCtrl->GetScheduleGroup());

        if (!SUCCEEDED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                GetControllerApp->OnControllerServerOffline();
            }
            else
            {
                TRACE0("Failed to Set Default Layout of Client Group!\n");
            }
            return;
        }
    }

    CString sScheduleName = pScheduleCtrl->GetScheduleGroup().GetGroupName().c_str();

    //AfxMessageBox(Translate(_T("Schedule Published Successfully!")));

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, (LPARAM)Translate(_T("Schedule Published Successfully!")).GetString());
}

void CSchedulerPage::OnRibbonBtn_Print()
{

}

void CSchedulerPage::DeleteDir(CString lpszPath)
{
    if ((lpszPath.ReverseFind(_T('\\')) + 1) != lpszPath.GetLength())
    {
        lpszPath.Append(_T("\\"));
    }

    TCHAR szFind[MAX_PATH];
    lstrcpy(szFind, lpszPath);
    TCHAR szFile[MAX_PATH];
    TCHAR szRoot[4];
    wsprintf(szRoot, _T("%c:\\"), lpszPath[0]);

    if ((lstrcmp(szRoot, lpszPath) == 0))
    {
        lstrcat(szFind, _T("\\"));
    }

    lstrcat(szFind, _T("*.*"));
    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile(szFind, &wfd);
    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do
    {
        if (wfd.cFileName[0] == _T('.'))
            continue; // filter the "." ".."

        wsprintf(szFile, _T("%s%s"), lpszPath, wfd.cFileName);

        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            DeleteDir(szFile);
        }
        else
        {
            DeleteFile(szFile);
        }
    } while (FindNextFile(hFind, &wfd));

    FindClose(hFind);

    RemoveDirectory(lpszPath);
}

#ifdef STANDALONE_EDITION
BOOL CSchedulerPage::isGroupExist()
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
	{
		return FALSE;
	}
	CString CurrentGroupName = pScheduleCtrl->GetScheduleGroup().GetGroupName().c_str();
	GroupListCtrl *pGroupList = &m_wndGroupList.GetGroupList();
	int i = 0;
	for (; i < pGroupList->GetItemCount(); i++)
	{
		CString groupName = pGroupList->GetItemText(i, 0);
		if (CurrentGroupName.CompareNoCase(groupName) == 0)
		{
			break;
		}
	}

	if (i == pGroupList->GetItemCount())
	{
		return FALSE;
	}
	return TRUE;
}
#endif

void CSchedulerPage::OnRibbonBtn_Export()
{

	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return;

    //CFileDialog SPED(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("SCL (*.scl)|*.scl|"), this);
    //CFileDialog SPED(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("XML (*.xml)|*.xml|"), this);
#ifdef STANDALONE_EDITION
	if (!isGroupExist())
	{
		MessageBox(Translate(_T("Failed to export schedule, the group is not exist.")), Translate(_T("Warning")), 0);
		return;
	}
#endif
	TCHAR Dir[255] = {0};

	TCHAR szVolumeName[255] = {0};
	TCHAR szPathName[255] = {0};
	vector<CString> szPathList;
	BOOL  bHaveRemovableDri = FALSE;
	HANDLE hd;
	UINT iDriveType;
	DWORD dwReturnLen = 0;

	//get removeable storage
	szPathList.clear();
	hd = FindFirstVolume(szVolumeName, 255);
	iDriveType = GetDriveType(szVolumeName);
	if(DRIVE_REMOVABLE == iDriveType)
	{
		bHaveRemovableDri = TRUE;

		GetVolumePathNamesForVolumeName(szVolumeName, szPathName, 255, &dwReturnLen);
		//_tsplitpath(szPathName, szDrive, szDir, szFile, szExt);
		TCHAR szRoot[255];
		lstrcpyn(szRoot, szPathName, 255);
		:: PathStripToRoot(szRoot);

		szPathList.push_back(szRoot);
	}

	while(FindNextVolume(hd, szVolumeName, 255))
	{
		iDriveType = GetDriveType(szVolumeName);
		if(DRIVE_REMOVABLE == iDriveType)
		{
			bHaveRemovableDri = TRUE;
			GetVolumePathNamesForVolumeName(szVolumeName, szPathName, 255, &dwReturnLen);
			//_tsplitpath(szPathName, szDrive, szDir, szFile, szExt);
			TCHAR szRoot[255];
			lstrcpyn(szRoot, szPathName, 255);
			:: PathStripToRoot(szRoot);

			szPathList.push_back(szRoot);
		}
	}

	FindVolumeClose(hd);
	if (!bHaveRemovableDri)
	{
		MessageBox(Translate(_T("Please insert removable disk!")), Translate(_T("Warning")), MB_OK);
		return;
	}


	CString Path;
	CRemoveStorage removeStorage(szPathList, this);
	if (removeStorage.DoModal() == IDOK)
	{
		removeStorage.GetSelectPath(Path);
	}
	else
	{
		return;
	}


    if(PathFileExists(Path))
    {
        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, (LPARAM)Translate(_T("Exporting Schedule!")).GetString());

        SchedulePackageHandler PackageHander;

        pScheduleCtrl->QueryAll();

        AppointmentArray DefaultAppArray;
        DefaultAppArray.RemoveAll();
        Appointment* pDefaultApp = const_cast<Appointment*>(pScheduleCtrl->GetDefaultAppointment());
        if (pDefaultApp)
        {
            DefaultAppArray.Add(pDefaultApp);
        }
        AppointmentArray  &AppArray = pScheduleCtrl->GetQueryedAppointments();


        std::vector<Layout> LayoutBuf;
        std::vector<Timetable> TimetableBuf;

        CMap<INT, INT&, INT, INT&> ExportLayouts;
        CMap<INT, INT&, INT, INT&> ExportTimeTables;

        if (pDefaultApp)
        {
            INT Value, Key = pDefaultApp->GetDetailID();

            if(pDefaultApp->GetDetailType() == Appointment::APPOINTMENT_TYPE_LAYOUT)
            {
                if (!ExportLayouts.Lookup(Key, Value))
                {
                    ExportLayouts.SetAt(Key, Value);
                    Layout layout;
                    GetControllerInterface->GetLayout(Key, layout);

                    if (!layout.GetLocation().empty())
                    {
                        LayoutBuf.push_back(layout);
						//LayoutSize += layout.GetLayoutSize();
                    }
                }
            }else
            {
                if (!ExportTimeTables.Lookup(Key, Value))
                {
                    ExportTimeTables.SetAt(Key, Value);
                    Timetable timetable;
                    GetControllerInterface->GetTimetable(Key, timetable);
                    TimetableBuf.push_back(timetable);
                }
            }
        }

        for (int i = 0; i < AppArray.GetSize(); i++)
        {
            INT Value, Key = AppArray[i]->GetDetailID();
            if(AppArray[i]->GetDetailType() == Appointment::APPOINTMENT_TYPE_LAYOUT)
            {
                if (!ExportLayouts.Lookup(Key, Value))
                {
                    ExportLayouts.SetAt(Key, Value);
                    Layout layout;
                    GetControllerInterface->GetLayout(Key, layout);

                    if (!layout.GetLocation().empty())
                    {
                        LayoutBuf.push_back(layout);
						//LayoutSize += layout.GetLayoutSize();
                    }
                }
            }else
            {
                if (!ExportTimeTables.Lookup(Key, Value))
                {
                    ExportTimeTables.SetAt(Key, Value);
                    Timetable timetable;
                    GetControllerInterface->GetTimetable(Key, timetable);
                    TimetableBuf.push_back(timetable);
                }
            }
        }

        for(size_t i=0; i< TimetableBuf.size(); i++)
        {
            auto it = TimetableBuf[i].LayoutEntryList.begin();
            while(it != TimetableBuf[i].LayoutEntryList.end())
            {
                INT Value, Key = it->GetLayoutID();

                if (!ExportLayouts.Lookup(Key, Value))
                {
                    ExportLayouts.SetAt(Key, Value);
                    Layout layout;
                    GetControllerInterface->GetLayout(Key, layout);

                    if (!layout.GetLocation().empty())
                    {
                        LayoutBuf.push_back(layout);
						//LayoutSize += layout.GetLayoutSize();
                    }
                }
                it++;
            }
        }

		std::vector<int> vecMediaIDList;
		vecMediaIDList.clear();
		std::vector<Layout>::iterator itLayout;
		for(itLayout = LayoutBuf.begin(); itLayout != LayoutBuf.end(); ++itLayout)
		{
			std::vector<int> vecMediaIDList2 = itLayout->GetMediaIDList();
			std::vector<int>::iterator itLayout2;
			for(itLayout2 = vecMediaIDList2.begin(); itLayout2 != vecMediaIDList2.end(); itLayout2++)
			{
				int nMediaID = (*itLayout2);
				if (nMediaID >= 0)
				{
					vector<int>::iterator result = find( vecMediaIDList.begin(), vecMediaIDList.end(), nMediaID ); //find nMediaID
					if  ( result == vecMediaIDList.end() ) //not find
					{
						vecMediaIDList.push_back(nMediaID);
					}
				}
			}
		}
		std::vector<Media> vecMediaList;
		HRESULT hr2 = GetControllerInterface->GetMedias(vecMediaIDList, vecMediaList);
		if (!SUCCEEDED(hr2))
		{
			if (hr2 == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
			{
				GetControllerApp->OnControllerServerOffline();
				return;
			}

			MessageBox(Translate(_T("Failed to pack the layout files!")), Translate(_T("Error:Pack layout")), MB_OK|MB_ICONERROR);
			//MessageBox(Translate(_T("Layout Exported Failed:")),Translate(_T("Tips:Export file")),MB_OK);
			return ;
		}

		DWORD64 LayoutSize = 0;
		std::vector<Media>::iterator itMedia;
		for(itMedia = vecMediaList.begin(); itMedia!= vecMediaList.end(); ++itMedia)
		{
			LayoutSize += itMedia->GetFileSize();
		}


		//if (AfxMessageBox(Translate(_T("The selected disk contains old schedules, do you want to delete it?")), MB_YESNO|MB_ICONQUESTION) != IDYES)
		{
			CString szNewPath;
			szNewPath = Path + MEDIALIBRARY_EXPORT_FOLDER;
			DeleteDir(szNewPath);
			SysCall::NewFolder(szNewPath.GetString());

			szNewPath = Path+LAYOUTS_EXPORT_FOLDER;
			DeleteDir(szNewPath);
			SysCall::NewFolder(szNewPath.GetString());
		}
		

        // export schedule;
        if (!PackageHander.ExportPackage(AppArray,
            DefaultAppArray, TimetableBuf, pScheduleCtrl, Path + _T("\\index"), this))
        {

        }

        //export Layout
        auto it = LayoutBuf.begin();
        while(it != LayoutBuf.end())
        {
            CString CurDownLoadedLayoutPath = Path + it->GetLocation().c_str() + _T("\\");
            CString szLayoutName = CurDownLoadedLayoutPath;
            ProcessCall::CheckAndCreatePath((LPCTSTR)szLayoutName);

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
                HRESULT hr = GetControllerInterface->GetLayoutContent(it->GetLayoutID(),   LayoutContent);
                if(FAILED(hr))
                {
                    MessageBox(Translate(_T("Failed to get layout content!")), Translate(_T("Error:Schedule page")), MB_ICONERROR);
                    continue;
                }


                std::ofstream file;
                file.open((LPCWSTR)szLayoutName, std::ios_base::trunc);

                if(!file.is_open())
                    continue;

                file.write(reinterpret_cast<char*>(&LayoutContent[0]), LayoutContent.size());
                file.close();
            }
            it++;
        }

		// collect exported layout list
        INT FreeRemainMB = SysCall::GetFreeSpaceMB(Path.GetString());

        // copy layout to disc
        INT LayoutSizeMB = static_cast<INT>(LayoutSize>>20);
        if (LayoutSizeMB > (FreeRemainMB - 1))
        {
			// clear the exported files
			CString scheduleFile = Path + _T("\\index") + SCHEDULEPACKAGE_POSTFIX;

			if (PathFileExists(scheduleFile))
			{
				DeleteFile(scheduleFile);
			}

			DeleteDir(Path + LAYOUTS_EXPORT_FOLDER);

            CString cWarning, str1, str2;
			str1.Format(_T("%d MB"),LayoutSizeMB);
			str2.Format(_T("%d MB"),FreeRemainMB);
			cWarning = Translate(_T("The schedule package(")) + str1 +
				Translate(_T(")may be too big for this disk(Free size: ")) +
				str2 + Translate(_T("), Can't export to this disk!"));
            MessageBox(cWarning, Translate(_T("Warning:Schedule page")),MB_OK|MB_ICONEXCLAMATION);
            return;
        }

        ExportLayoutsDlg dlg(this);

        for (UINT i = 0; i < LayoutBuf.size(); i++)
        {
            MEDIA_LIST MediaList;
            MediaList.m_MediaIDs = LayoutBuf[i].GetMediaIDList();
            if (!dlg.AddMediaListJob(MediaList, Path))
            {
                MessageBox(Translate(_T("Media Library Issue, Failed to download!")), Translate(_T("Error:Schedule page")), MB_ICONERROR);
                return;
            }
        }

        if(IDOK != dlg.DoModal())
        {
            // clear the exported files
            CString scheduleFile = Path + _T("\\index") + SCHEDULEPACKAGE_POSTFIX;

            if (PathFileExists(scheduleFile))
            {
                DeleteFile(scheduleFile);
            }

            DeleteDir(Path + LAYOUTS_EXPORT_FOLDER);

            pScheduleCtrl->QueryAppointments();

            GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
                (LPARAM)(Translate(_T("Schedule Export Canceled, Exported Files Cleared!")) + Path/*SPED.GetFolderPath()*/).GetString());

            return;

        }

        pScheduleCtrl->QueryAppointments();

        MessageBox(_T("    ") + Translate(_T("Schedule Export Finished!")) + _T("    "),Translate(_T("Tips:Schedule page")), MB_OK);

        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)(Translate(_T("Schedule Export Finished!")) + Path/*SPED.GetFolderPath()*/).GetString());
    }
}

void CSchedulerPage::OnRibbonBtn_Import()
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return;

    CFileDialog SPID(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("XML (*.xml)|*.xml|"), this);
    if (SPID.DoModal() == IDOK)
    {
        CString PathName = SPID.GetPathName();

		ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
		if (NULL == pScheduleCtrl)
		{
			return;
		}

        SchedulePackageHandler PackageHander;

        AppointmentArray AA;
        AppointmentArray DefaultAA;
		CString GroupName;
        if (!PackageHander.ImportPackage(GroupName, AA, DefaultAA, PathName, pScheduleCtrl))
        {
            MessageBox(Translate(_T("Schedule Package Import Failed!")),Translate(_T("Error:Schedule page")), MB_OK|MB_ICONERROR);
            return;
        }
		
		int iGroupID = pScheduleCtrl->GetScheduleGroup().GetGroupID();
		std::tstring  szScheduleGroupName;
		if (iGroupID == S3NET_DEFAULT_CLIENTGROUP_ID)
		{
			szScheduleGroupName = (_T("Default"));
		}
		else
		{
			szScheduleGroupName = pScheduleCtrl->GetScheduleGroup().GetGroupName();
		}

		if (GroupName.CompareNoCase(szScheduleGroupName.c_str()) != 0 )
		{
			int retCode = MessageBox(Translate(_T("The group name of import schedule is different from the opened group name, do you want to continue?")),Translate(_T("Tips:Schedule page")), MB_YESNO|MB_ICONQUESTION);
			if (retCode == IDNO)
			{
				return;
			}
		}

        pScheduleCtrl->RemoveAllAppointments();

        COleDateTime StartDate;
        if (AA.GetSize() > 0)
        {
            StartDate = AA[0]->GetStart();
        }

        BeginApendSchedule();

        for (int i = 0; i < AA.GetSize(); i++)
        {
            if (AA[i]->GetDetailType() == Appointment::APPOINTMENT_TYPE_LAYOUT)
            {
                Layout layout;
                HRESULT hr = GetControllerInterface->GetLayout(AA[i]->GetDetailID(), layout);
                if (!SUCCEEDED(hr))
                {
                    if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                    {
                        GetControllerApp->OnControllerServerOffline();
                    }
                    else
                    {
                        MessageBox(Translate(_T("Schedule Package Import Failed!")),Translate(_T("Error:Schedule page")), MB_OK|MB_ICONERROR);
                    }
                    EndApendSchedule();
                    return;
                }
                AA[i]->SetDescription(layout.GetLayoutName().c_str());

                pScheduleCtrl->AddAppointment(AA[i], FALSE, FALSE);
                StartDate = min(AA[i]->GetStart(), StartDate);
            }
            else if (AA[i]->GetDetailType() == Appointment::APPOINTMENT_TYPE_TIMETABLE)
            {
                Timetable tt;
                HRESULT hr = GetControllerInterface->GetTimetable(AA[i]->GetDetailID(), tt);
                if (!SUCCEEDED(hr))
                {
                    if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                    {
                        GetControllerApp->OnControllerServerOffline();
                    }
                    else
                    {
                        MessageBox(Translate(_T("Schedule Package Import Failed!")),Translate(_T("Error:Schedule page")), MB_OK|MB_ICONERROR);
                    }
                    EndApendSchedule();
                    return;
                }
                AA[i]->SetDescription(tt.GetTimetableName().c_str());

                pScheduleCtrl->AddAppointment(AA[i], FALSE, FALSE);
                StartDate = min(AA[i]->GetStart(), StartDate);
            }

        }

        if (DefaultAA.GetSize() > 0)
        {
            pScheduleCtrl->SetDefaultAppointment(DefaultAA[0], FALSE);
        }

        pScheduleCtrl->SetDate(StartDate);
        pScheduleCtrl->QueryAppointments();

        EndApendSchedule();

         //by RaymondLiu fix issue 81538
		for(int kk = 0; kk < AA.GetSize(); kk++)
		{
			OnScheduleCtrlAppointmentChanged(0, (LPARAM)AA[kk]);
		}
		for(int jj = 0; jj < DefaultAA.GetSize(); jj++)
		{
			OnScheduleCtrlAppointmentChanged(0, (LPARAM)DefaultAA[jj]);
		}


        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)(Translate(_T("Schedule Package Imported:")) + PathName).GetString());

    }
}

void CSchedulerPage::OnRibbonBtn_Copy()
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return;

    if (::OpenClipboard(GetSafeHwnd()))
    {
        ::EmptyClipboard();

        try
        {
            CSharedFile globalFile;

            if (pScheduleCtrl->SerializeTo(globalFile))
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

    /*if (!pScheduleCtrl->Copy())
    {
		CString mess = Translate(_T("Copy failed!")) + _T("\n") + Translate(_T("Please select some appointments first!"));
        AfxMessageBox(mess);
        return;
    }*/
}

void CSchedulerPage::OnRibbonBtn_Paste()
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return;

    if(::OpenClipboard(GetSafeHwnd()))
    {
        if(::IsClipboardFormatAvailable(ScheduleCtrl::GetClipboardFormat()))
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

            BOOL bResult = FALSE;

            AppointmentArray ar;
            bResult = ScheduleCtrl::SerializeFromAppointment (globalFile, ar, pScheduleCtrl->GetStyle(), pScheduleCtrl->GetCurSelectedDate());

            if (bResult)
            {
                if (!pScheduleCtrl->Paste(ar))
                {
                    CString mess = Translate(_T("Paste failed!")) + _T("\n") + Translate(_T("Please make sure the target place is clean!"));
                    MessageBox(mess,Translate(_T("Error:Schedule page")), MB_OK|MB_ICONERROR);
                }

            }
            else
            {
                CString mess = Translate(_T("Failed to paste"));
                MessageBox(mess,Translate(_T("Error:Schedule page")), MB_OK|MB_ICONERROR);
            }

            for (int i = 0; i < ar.GetSize (); i++)
            {
                delete ar[i];
            }
            ar.RemoveAll();

            GlobalFree(hGlobalMemory);
        }
        else
        {
            CString mess = Translate(_T("Failed to paste"));
            MessageBox(mess,Translate(_T("Error:Schedule page")), MB_OK|MB_ICONERROR);
        }

        ::CloseClipboard();
    }
  //  if (!pScheduleCtrl->Paste())
  //  {
		//CString mess = Translate(_T("Paste failed!")) + _T("\n") + Translate(_T("Please copy first and make sure the target place is clean!"));
  //      AfxMessageBox(mess);
  //      return;
  //  }
}

void CSchedulerPage::OnRibbonBtn_Delete()
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return;

    if (pScheduleCtrl->GetSelectedAppointmentsCount() > 0)
    {

        if (MessageBox(Translate(_T("Are you sure to delete the selected Layouts in the schedule?")),Translate(_T("Tips:Schedule page")),MB_YESNO|MB_ICONQUESTION) != IDYES)
        {
            return;
        }

		pScheduleCtrl->RemoveSelections();

        std::vector<Appointment*> AppList;
        m_wndScheduleItem.GetScheduleItemCtrl()->SetItemList(AppList);
    }
}

void CSchedulerPage::OnRibbonBtn_HMultiCopy()
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return;

    if (pScheduleCtrl->GetSelectedAppointmentsCount() <= 0)
    {
		CString mess = Translate(_T("No appointments selected!")) + _T("\n") + Translate(_T("Please select some appointments first!"));
        MessageBox(mess,Translate(_T("Warning:Schedule page")), MB_OK|MB_ICONINFORMATION);
        return;
    }

    CScheduleHorizontallyCopyDlg HCDlg(this);

    if(HCDlg.DoModal() == IDOK)
    {
        COleDateTimeSpan Span = HCDlg.m_EndTime - HCDlg.m_StartTime;

        UpdateSchedule(HCDlg.m_StartTime, HCDlg.m_EndTime);

        INT nHorizontallyCnt = static_cast<INT>(Span / pScheduleCtrl->GetSelectedAppointmentsRange());
        DWORD DaysInWeek = 0;
        if (HCDlg.m_bMonday)
        {
            DaysInWeek |= MULTICOPY_MONDAY;
        }
        if (HCDlg.m_bTuesday)
        {
            DaysInWeek |= MULTICOPY_TUESDAY;
        }
        if (HCDlg.m_bWednesday)
        {
            DaysInWeek |= MULTICOPY_WEDNESDAY;
        }
        if (HCDlg.m_bThursday)
        {
            DaysInWeek |= MULTICOPY_THURSDAY;
        }
        if (HCDlg.m_bFriday)
        {
            DaysInWeek |= MULTICOPY_FRIDAY;
        }
        if (HCDlg.m_bSaturday)
        {
            DaysInWeek |= MULTICOPY_SATURDAY;
        }
        if (HCDlg.m_bSunday)
        {
            DaysInWeek |= MULTICOPY_SUNDAY;
        }

        if (!pScheduleCtrl->HorizontallyCopyAndPaste
                (HCDlg.m_StartTime, HCDlg.m_EndTime, DaysInWeek))
        {
			CString mess = Translate(_T("Horizontally copy failed!")) + _T("\n") + Translate(_T("Please make sure the target range is clean!"));
            MessageBox(mess,Translate(_T("Error:Schedule page")), MB_OK|MB_ICONERROR);
        }

    }

}

void CSchedulerPage::OnRibbonBtn_VMultiCopy()
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return;

    if (pScheduleCtrl->GetSelectedAppointmentsCount() <= 0)
    {
        CString mess = Translate(_T("No appointments selected!")) + _T("\n") + Translate(_T("Please select some appointments first!"));
        MessageBox(mess,Translate(_T("Error:Schedule page")), MB_OK|MB_ICONERROR);
        return;
    }

    CScheduleVerticallyCopyDlg VCDlg(this);

    if(VCDlg.DoModal() == IDOK)
    {
        INT nVerticallyCnt = VCDlg.m_VerticallyCnt;
        if (nVerticallyCnt > 0)
        {
            if (!pScheduleCtrl->VerticallyCopyAndPaste(abs(nVerticallyCnt), COPY_DOWN))
            {
				CString mess = Translate(_T("Vertically copy failed!")) + _T("\n") + Translate(_T("Please make sure the target range is clean and it should not cross days!"));
                MessageBox(mess,Translate(_T("Error:Schedule page")), MB_OK|MB_ICONERROR);
            }
        }
        else if (nVerticallyCnt < 0)
        {
            if (!pScheduleCtrl->VerticallyCopyAndPaste(abs(nVerticallyCnt), COPY_UP))
            {
                CString mess = Translate(_T("Vertically copy failed!")) + _T("\n") + Translate(_T("Please make sure the target range is clean and it should not cross days!"));
                MessageBox(mess,Translate(_T("Error:Schedule page")), MB_OK|MB_ICONERROR);
            }
        }
    }
}

void CSchedulerPage::OnRibbonBtn_MultiDelete()
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return;

    if (pScheduleCtrl->GetSelectedAppointmentsCount() <= 0)
    {
		CString mess = Translate(_T("No appointments selected!")) + _T("\n") + Translate(_T("Please select some appointments first!"));
        MessageBox(mess,Translate(_T("Warning:Schedule page")), MB_OK|MB_ICONINFORMATION);
		return;
    }

    CScheduleContinuousDeleteDlg CDDlg(this);

    if(CDDlg.DoModal() == IDOK)
    {
        INT Cnt = CDDlg.m_ReoccurencCnt;
        BOOL bHorizontally = CDDlg.m_bHorizontally;

        COPY_DIRECTION direction = COPY_RIGHT;
        if (bHorizontally)
        {
            if (Cnt > 0)
            {
                direction = COPY_RIGHT;
            }
            else
            {
                direction = COPY_LEFT;
            }
        }
        else
        {
            if (Cnt > 0)
            {
                direction = COPY_DOWN;
            }
            else
            {
                direction = COPY_UP;
            }
        }

        COleDateTime Stime, Etime;
        pScheduleCtrl->GetContinuerousDeleteRanageDate(abs(Cnt), direction, Stime, Etime);

        UpdateSchedule(Stime, Etime);

        pScheduleCtrl->ContinuerousDelete(abs(Cnt), direction);
    }
}

HRESULT CSchedulerPage::PreviewLayout(const int nWidth, const int nHeight)
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return E_FAIL;

    HRESULT hr = S_OK;

    INT nSelectedCnt = pScheduleCtrl->GetSelectedAppointmentsCount();
    if (nSelectedCnt == 1)
    {
        POSITION pos = pScheduleCtrl->GetFirstSelectedAppointment();
        Appointment* pApp = pScheduleCtrl->GetNextSelectedAppointment(pos);

        Layout layout;
        hr = GetControllerInterface->GetLayout(pApp->GetDetailID(), layout);
        if (!SUCCEEDED(hr))
        {
			CString strMess= Translate(_T("Failed to get layout, maybe this layout has been deleted.")) + _T("\n") + Translate(_T("Please reopen this page to update the schedule."));
            MessageBox(strMess,Translate(_T("Error:Schedule page")), MB_OK|MB_ICONERROR);
            return hr;
        }

         CString CurDownLoadedLayoutPath = GetConfig->Get(ROOT_DIR) + DESIGNER_PATH + CString(layout.GetLayoutName().c_str()) + _T("\\");


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
                    MessageBox(Translate(_T("Failed to get layout content!")), Translate(_T("Error:Schedule page")), MB_ICONERROR);
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

void CSchedulerPage::OnRibbonBtn_Preview()
{
    PreviewLayout(800, 600);
}

void CSchedulerPage::OnRibbonBtn_Preview10X7()
{
    PreviewLayout(1024, 768);
}

void CSchedulerPage::OnRibbonBtn_Preview12X10()
{
    PreviewLayout(1280, 1024);
}

void CSchedulerPage::OnRibbonBtn_Preview19X12()
{
    PreviewLayout(1920, 1080);
}

void CSchedulerPage::OnRibbonBtn_SetDefault()
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return;

    HRESULT hr = S_OK;
    INT nSelectedCnt = pScheduleCtrl->GetSelectedAppointmentsCount();
    if (nSelectedCnt == 1)
    {
        POSITION pos = pScheduleCtrl->GetFirstSelectedAppointment();
        Appointment* pApp = pScheduleCtrl->GetNextSelectedAppointment(pos);

        pScheduleCtrl->SetDefaultAppointment(pApp, TRUE);
    }
}

void CSchedulerPage::OnRibbonBtn_RemoveDefault()
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return;

    pScheduleCtrl->SetDefaultAppointment(NULL, TRUE);
}

void CSchedulerPage::OnRibbonBtn_AlignUp()
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return;

     pScheduleCtrl->AlignSelections(TRUE);//pScheduleCtrl->AlignUpSelectedAppointments(TRUE);
}

void CSchedulerPage::OnRibbonBtn_AlignDown()
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return;

    pScheduleCtrl->AlignSelections(FALSE);//AlignDownSelectedAppointments(TRUE);
}

void CSchedulerPage::SetDateInterval (const COleDateTime& date1, const COleDateTime& date2)
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return;

    pScheduleCtrl->SetDateInterval (date1, date2);
}

void CSchedulerPage::OnRibbonBtn_TimeDelta()
{
    int delta = m_pComboBoxTimeDelta->GetItemData(m_pComboBoxTimeDelta->GetCurSel());

    m_wndScheduer.m_scheduleListTab.SetTimeDelta(delta);
}

#ifdef STARTER_EDITION
void CSchedulerPage::OnRibbonBtn_PlayerSetting()
{
	BOOL bChanged = FALSE;
	ClientConfigFile ClientPlayConfig; // = ClientConfigFile::GetConfigFile();
    ClientPlayConfig.Load(GetClientDefaultDir() + CLIENT_CONFIG_FILE);

	INT Angle = m_pComboBoxPlayerRotation->GetItemData(m_pComboBoxPlayerRotation->GetCurSel());

	if ( ClientPlayConfig.GetN(CLIENT_ROTATEDEGREE) != Angle*90)
	{
		bChanged = TRUE;
		ClientPlayConfig.Set(CLIENT_ROTATEDEGREE, Angle*90);
	}

	if (bChanged)
	{
		ClientPlayConfig.Save();
	}
}
#endif

LRESULT CSchedulerPage::OnScheduleItemUpdate (WPARAM w, LPARAM l)
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return 0;

    std::vector<Appointment*>& AppList = m_wndScheduleItem.GetScheduleItemCtrl()->GetScheduleItems();

    if (AppList.size() == 1)
    {
        Appointment* pApp = AppList[0];

        pScheduleCtrl->UpdateAppointment(pApp,
			m_wndScheduleItem.GetScheduleItemCtrl()->GetStartDate(),
            m_wndScheduleItem.GetScheduleItemCtrl()->GetEndDate());
    }

    OnSelectScheduleItemChange(NULL, NULL);

    return 0;
}

LRESULT CSchedulerPage::OnScheduleItemUpdateDuration (WPARAM w, LPARAM l)
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return 0;

    std::vector<Appointment*>& AppList = m_wndScheduleItem.GetScheduleItemCtrl()->GetScheduleItems();

	COleDateTime oleDuration = m_wndScheduleItem.GetScheduleItemCtrl()->GetDuration();

	/*int nDays = oleDuration.GetDay();
	int nHours = oleDuration.GetHour();
	int nMin = oleDuration.GetMinute();
	int nSec = oleDuration.GetSecond();
	nHours += nDays * 24;
	if(nHours >= 1000)
	{
		AfxMessageBox(Translate(_T("oleDuration")));
		oleDuration.SetDateTime(0,0,999/24,999%24,nMin,nSec);
	}*/

    if (AppList.size() >= 1)
    {
        std::vector<Appointment*> TempAppList = AppList;
        for (std::vector<Appointment*>::iterator it = TempAppList.begin(); it != TempAppList.end(); it++)
        {
            pScheduleCtrl->UpdateAppointment((*it), (*it)->GetStart(),
                (*it)->GetStart() + oleDuration /*m_wndScheduleItem.GetScheduleItemCtrl()->GetDuration()*/);
        }
    }

    OnSelectScheduleItemChange(NULL, NULL);

    return 0;
}


LRESULT CSchedulerPage::OnSelectScheduleItemChange (WPARAM, LPARAM)
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return 0;

    std::vector<Appointment*> AppList;

    POSITION pos = pScheduleCtrl->GetFirstSelectedAppointment();
    while(pos)
    {
        AppList.push_back(pScheduleCtrl->GetNextSelectedAppointment(pos));

    }

    m_wndScheduleItem.GetScheduleItemCtrl()
        ->SetItemList(AppList);

    return 0;
}


BOOL CSchedulerPage::UpdateSchedule(COleDateTime Start, COleDateTime End)
{


	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)  
		return FALSE;

    // offline schedule need no update
    if (pScheduleCtrl->GetScheduleID() < 0)
    {
        return TRUE;
    }

    if (!pScheduleCtrl->m_ScheduleRangeHolder.IsQueriedRange(Start, End))
    {
        COleDateTimeSpan OneDay(1, 0, 0, 0);
        COleDateTime EndLine = COleDateTime(End.GetYear(), End.GetMonth(), End.GetDay(), 23,59,59);
        for (COleDateTime date = Start; date <= EndLine; date += OneDay)
        {
            COleDateTime dataS = COleDateTime(date.GetYear(), date.GetMonth(), date.GetDay(), 0,0,0);
            COleDateTime dataE = COleDateTime(date.GetYear(), date.GetMonth(), date.GetDay(), 23,59,59);
            if (!pScheduleCtrl->m_ScheduleRangeHolder.IsQueriedRange(dataS, dataE))
            {
                // Query the exsit apps, then need to compare, if it is already in schedule, no need to add
                pScheduleCtrl->QueryAppointments(dataS, dataE);
                AppointmentArray& ExsitApps = pScheduleCtrl->GetQueryedAppointments();


                Schedule CurSchedule;
				// TODO need pass the evidence to publish schedule
				// By Tod
				std::vector<S3Time> evidence;
                HRESULT hr = GetControllerInterface->GetSchedule(S3Time::CreateTime(dataS), S3Time::CreateTime(dataE),
                    CurSchedule, evidence, pScheduleCtrl->GetScheduleID()/*pScheduleCtrl->GetScheduleID(ID)*/);
                if (!SUCCEEDED(hr))
                {
                    if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                    {
                        GetControllerApp->OnControllerServerOffline();
                    }
                    else
                    {
                        TRACE0("Failed to Get Schedule!\n");
                    }
                    return FALSE;
                }


                std::vector<COleDateTime> Timestamps;
                if ((COleDateTimeSpan(dataE - dataS).GetDays()+1) != evidence.size())
                {
                    TRACE0("Controller server issue, GetSchedule, Timestamps wrong!\n");
                    ASSERT(0);
                }
                else
                {
                    Timestamps.clear();
                    for (auto it = evidence.begin(); it != evidence.end(); it++)
                    {
                        Timestamps.push_back(it->ToCOleDateTime());
                    }
                }

                pScheduleCtrl->m_ScheduleRangeHolder.AppendNewQueriedRange(dataS, dataE, Timestamps);


                int EntryCnt = CurSchedule.GetScheduleNum();

                BeginApendSchedule();

                while (EntryCnt)
                {
                    EntryCnt--;

                    Appointment* pApp = new Appointment();
                    ScheduleEntry Entry = CurSchedule.GetScheduleEntry(EntryCnt);

                    //// If start time is less than dateS, it is already added in the last round
                    //if (Entry.GetStartTime().ToCOleDateTime() < dataS)
                    //{
                    //    continue;
                    //}


                    // Check if the app already exist in schedule
                    // To be refined later, maybe we still need use the previous way which is shown in the up codes
                    BOOL bAlreadyExsit = FALSE;
                    for (INT i = 0; i < ExsitApps.GetSize(); i++)
                    {
                        if (ExsitApps[i]->GetDetailID() == Entry.GetLayoutID() &&
                            ExsitApps[i]->GetStart() == Entry.GetStartTime().ToCOleDateTime() &&
                            ExsitApps[i]->GetFinish() == Entry.GetEndTime().ToCOleDateTime())
                        {
                            bAlreadyExsit = TRUE;
                            break;
                        }
                    }
                    if (bAlreadyExsit)
                    {
                        continue;
                    }


                    Layout layout;
                    GetControllerInterface->GetLayout(Entry.GetLayoutID(), layout);
                    pApp->SetDescription(layout.GetLayoutName().c_str());
                    pApp->SetInterval(Entry.GetStartTime().ToCOleDateTime(), Entry.GetEndTime().ToCOleDateTime());
                    pApp->SetDetailID(Entry.GetLayoutID());
                    pApp->SetDetailType(Appointment::APPOINTMENT_TYPE_LAYOUT);


                    // UpdateIcon
                    UINT Index = 0;
                    if (!m_IconDetailIDMapper.Lookup(pApp->GetDetailID(), Index))
                    {
                        Layout layout;
                        hr = GetControllerInterface->GetLayout(pApp->GetDetailID(), layout);
                        if (!SUCCEEDED(hr))
                        {
                            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                            {
                                GetControllerApp->OnControllerServerOffline();
                            }
                            else
                            {
                                TRACE0("Failed to Get Layout thmbnail!\n");
                            }
                        }

                 Layout::thumbnail_t thumbnail;

                 GetControllerInterface->GetLayoutThumbnail(layout.GetLayoutID(), thumbnail);


						if (thumbnail.size() > 0)
						{
							HGLOBAL hGlobal  =  GlobalAlloc(GMEM_MOVEABLE, thumbnail.size() * sizeof(thumbnail[0]));
							void *  pData  =  GlobalLock(hGlobal);
							memcpy(pData, &thumbnail[0], thumbnail.size() * sizeof(thumbnail[0]));
							GlobalUnlock(hGlobal);

							IStream *  pStream  =  NULL;
							if  (CreateStreamOnHGlobal(hGlobal, TRUE,  & pStream)  ==  S_OK)
							{
								CImage image;
								if (SUCCEEDED(image.Load(pStream)))
								{
                                    CImage ThumbImage;
                                    ThumbImage.Create(THUMBNAIL_W, THUMBNAIL_H, 32);

                                    HDC hDstDC = NULL;
                                    BOOL bResult;

                                    // Obtain a DC to the destination image
                                    hDstDC = ThumbImage.GetDC();

                                    CDC  cDC;
                                    cDC.Attach(hDstDC);
                                    cDC.FloodFill(0, 0, RGB(123,456,789));
                                    cDC.Detach();

                                    // Perform the blit
                                    bResult = image.BitBlt( hDstDC, max(0, (THUMBNAIL_W - image.GetWidth()))/2,
                                        max(0, (THUMBNAIL_H - image.GetHeight()))/2,
                                        image.GetWidth(), image.GetHeight(), 0, 0 );

                                    // Release the destination DC
                                    ThumbImage.ReleaseDC();

                                    HBITMAP hBitmap=ThumbImage.Detach();
                                    CBitmap bmp;
                                    bmp.Attach(hBitmap);

									Index = m_ScheduleItemIconList.Add(&bmp, RGB(123,456,789));
								}
								pStream -> Release();
							}
							GlobalFree(hGlobal);

							m_IconDetailIDMapper.SetAt(pApp->GetDetailID(), Index);
						}
                    }
                    pApp->SetImage(&m_ScheduleItemIconList, Index);

                    pScheduleCtrl->AddAppointment(pApp, TRUE, FALSE, FALSE);
                }

                EntryCnt = CurSchedule.GetTimetableNum();

                BeginApendSchedule();

                while (EntryCnt)
                {
                    EntryCnt--;

                    Appointment* pApp = new Appointment();
                    TimetableInSchedule Entry = CurSchedule.GetTimetable(EntryCnt);

                    //// If start time is less than dateS, it is already added in the last round
                    //if (Entry.GetStartTime().ToCOleDateTime() < dataS)
                    //{
                    //    continue;
                    //}


                    // Check if the app already exist in schedule
                    // To be refined later, maybe we still need use the previous way which is shown in the up codes
                    BOOL bAlreadyExsit = FALSE;
                    for (INT i = 0; i < ExsitApps.GetSize(); i++)
                    {
                        if (ExsitApps[i]->GetDetailID() == Entry.GetTimetableID() &&
                            ExsitApps[i]->GetStart() == Entry.GetStartTime().ToCOleDateTime() &&
                            ExsitApps[i]->GetFinish() == Entry.GetEndTime().ToCOleDateTime())
                        {
                            bAlreadyExsit = TRUE;
                            break;
                        }
                    }
                    if (bAlreadyExsit)
                    {
                        continue;
                    }


                    Timetable timetable;
                    GetControllerInterface->GetTimetable(Entry.GetTimetableID(), timetable);
                    pApp->SetDescription(timetable.GetTimetableName().c_str());
                    pApp->SetInterval(Entry.GetStartTime().ToCOleDateTime(), Entry.GetEndTime().ToCOleDateTime());
                    pApp->SetDetailID(Entry.GetTimetableID());
                    pApp->SetDetailType(Appointment::APPOINTMENT_TYPE_TIMETABLE);


                    // UpdateIcon
                    UINT Index = 0;
                    pApp->SetImage(&m_ScheduleItemIconList, Index);

                    m_wndScheduer.GetScheduleCtrl()->AddAppointment(pApp, TRUE, FALSE, FALSE);
                }



                EndApendSchedule();
            }

            pScheduleCtrl->QueryAppointments();
        }
    }

    return TRUE;
}

LRESULT  CSchedulerPage::OnSelChanged(WPARAM, LPARAM lp)
{
    COleDateTime date1;
    COleDateTime date2;

    CList<DATE, DATE&> lstDates;

    m_wndCalender.GetMonthCalCtrl()->GetSelectedDates(lstDates);
    CBORROW_Calendar::GetMinMaxSelection (lstDates, date1, date2);

    UpdateSchedule(date1, date2);

    SetDateInterval (date1, date2);

    return 0;
}

LRESULT CSchedulerPage::OnScheduleCtrlAppointmentPreChange (WPARAM wp, LPARAM lp)
{
    Appointment* pApp = reinterpret_cast<Appointment*>(lp);

    UpdateSchedule(pApp->GetStart(), pApp->GetFinish());

    if (m_bAppendingSchedule)
    {
        return 1;
    }

	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return 0;

    pScheduleCtrl->m_ScheduleRangeHolder.AppendNewModifiedRange(pApp->GetStart(), pApp->GetFinish());

    return 0;
}

LRESULT CSchedulerPage::OnScheduleCtrlAppointmentChanged (WPARAM wp, LPARAM lp)
{
    Appointment* pApp = reinterpret_cast<Appointment*>(lp);

    UpdateSchedule(pApp->GetStart(), pApp->GetFinish());

    if (m_bAppendingSchedule)
    {
        return 1;
    }

	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return 0;

    pScheduleCtrl->m_ScheduleRangeHolder.AppendNewModifiedRange(pApp->GetStart(), pApp->GetFinish());

    //OnSelectScheduleItemChange(wp, lp);
    std::vector<Appointment*> AppList;

    AppList.push_back(pApp);

    m_wndScheduleItem.GetScheduleItemCtrl()
        ->SetItemList(AppList);

    return 0;
}

LRESULT CSchedulerPage::OnScheduleCtrlTimeDeltaChanged (WPARAM wp, LPARAM lp)
{
    _SVIEW_TIME_DELTA TimeDelta = _SVIEW_TIME_DELTA(wp);

    m_pComboBoxTimeDelta->SelectItem(TimeDelta);

    return 0;
}

LRESULT CSchedulerPage::OnSchedulerTabActiveChange (WPARAM wp, LPARAM lp)
{
    if (!m_wndScheduer.m_scheduleListTab.GetScheduleCtrl())
    {
        return -1;
    }


    OnSelectScheduleItemChange (wp, lp);

    int Delta = m_wndScheduer.m_scheduleListTab.GetTimeDelta();
    CString szDelta;
	if(Delta >= 60)
		szDelta.Format(Translate(_T("%d min")), Delta / 60);
	else
		szDelta.Format(Translate(_T("%d sec")),Delta);

    m_pComboBoxTimeDelta->SelectItem(szDelta);

    return 0;

}

void CSchedulerPage::BeginApendSchedule()
{
    m_bAppendingSchedule = TRUE;
}

void CSchedulerPage::EndApendSchedule()
{
    m_bAppendingSchedule = FALSE;
}

void CSchedulerPage::OnUpdateRibbonBtn(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }
}

void CSchedulerPage::OnUpdateRibbonBtnNeedSelect(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
    {
        pCmdUI->Enable(FALSE);
		return;
    }

    INT nSelectedCnt = pScheduleCtrl->GetSelectedAppointmentsCount();
    if (nSelectedCnt > 0)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CSchedulerPage::OnUpdateRibbonBtnNeedSingleSelect(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
    {
        pCmdUI->Enable(FALSE);
		return;
    }

    INT nSelectedCnt = pScheduleCtrl->GetSelectedAppointmentsCount();
    if (nSelectedCnt == 1)
    {
		AppointmentList &selectList = pScheduleCtrl->GetSelectedAppointments();
		Appointment *pApp = selectList.GetHead();
		if(pApp->GetDetailType() == Appointment::APPOINTMENT_TYPE_TIMETABLE)
			pCmdUI->Enable(FALSE);
		else
			pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CSchedulerPage::OnUpdateRibbonBtnRemoveDefault(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
    {
        pCmdUI->Enable(FALSE);
		return;
    }

    if (pScheduleCtrl->GetDefaultAppointment())
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

CString CSchedulerPage::GetLayoutExportLocation(INT LayoutID)
{
    Layout layout;
    HRESULT hr = GetControllerInterface->GetLayout(LayoutID, layout);
    if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
    {
        GetControllerApp->OnControllerServerOffline();
    }
    return CString(layout.GetLocation().c_str()) + _T("\\");
}
#ifdef STARTER_EDITION
#define  CLIENT_NAME (_T("StarterClient.exe"))
#else
#define  CLIENT_NAME (_T("Client.exe"))
#endif
void CSchedulerPage::OnRibbonBtn_Go()
{
	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
		return;

    BOOL bChanged = (pScheduleCtrl->m_ScheduleRangeHolder.GetModifiedRangeCount() > 0);

    INT DefaultLayoutID = -1;
    if (pScheduleCtrl->GetDefaultAppointment())
    {
        DefaultLayoutID = pScheduleCtrl->GetDefaultAppointment()->GetDetailID();

    }
    if (pScheduleCtrl->GetScheduleGroup().GetDefaultLayoutID() != DefaultLayoutID)
    {
        bChanged = TRUE;
    }

    if (bChanged)
    {
        if (IDYES == MessageBox(Translate(_T("Schedule Changed, Are you going to Save the change?")),Translate(_T("Warning:Schedule page")), MB_YESNO|MB_ICONQUESTION))
        {
            OnRibbonBtn_Save();
            //MessageBox(Translate(_T("Schedule Saved!")));
        }
    }

    PROCESS_INFORMATION ServerProcess = {0};

    // start controller server
    if (ProcessCall::FindProcess(CLIENT_NAME) > 0)
    {
        ProcessCall::KillProcess(CLIENT_NAME);
    }

    TString processPath = GetAppPath();
    processPath  += TString(_T("\\")) + CLIENT_NAME;

    if (!PathFileExists(processPath.c_str()))
    {
        /*AfxMessageBox(NULL, _T("Can not find the Signage Client, please Copy the \
                            server app to the folder of configuer!"),
                            _T("S3 Signage Client"), MB_ICONERROR);*/
    }
	
	processPath += TString(_T(" /szCurLanguage "));
	CString curlanguage;
	GetConfig->Get(LANGUAGE_SETTING,curlanguage);
	map<CString,CString> & LanMap = GetLanguageNameMap();
	//CString lanLocName = ChangeEngToLocLanName(curlanguage);
	//processPath += lanLocName.GetString();
	processPath += curlanguage.GetString();

    STARTUPINFOW si = {0};
    if( !CreateProcessW( NULL,   // No module name (use command line)
        (LPWSTR)processPath.c_str(),           // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &ServerProcess )  // Pointer to PROCESS_INFORMATION structure
        )
    {
        // failed
        //MessageBox(NULL, Translate(_T("Filed to start Signage Client!")), Translate(_T("S3 Signage Client")), MB_ICONERROR);
    }
}

void CSchedulerPage::OnUpdateRibbonBtnPreview(CCmdUI* pCmdUI)
{

    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
    {
        pCmdUI->Enable(FALSE);
		return;
    }

    INT nSelectedCnt = pScheduleCtrl->GetSelectedAppointmentsCount();
    if (pCmdUI->m_nID >= ID_RIBBON_BTN_SCHEDULE_PREVIEW && pCmdUI->m_nID <= ID_RIBBON_BTN_PREVIEW_19X10)
    {
        BOOL bPlayer = PlayerController::GetInstance()->IsPlayerRunning();
        if ((nSelectedCnt == 1) && !bPlayer)
        {
			POSITION pos = pScheduleCtrl->GetFirstSelectedAppointment();
			Appointment* pApp = pScheduleCtrl->GetNextSelectedAppointment(pos);
			ASSERT(pApp);

			if (pApp->GetDetailType() == Appointment::APPOINTMENT_TYPE_TIMETABLE)
			{
				pCmdUI->Enable(FALSE);
			}
			else
			{
                if(pApp->GetDetailID() == S3NET_EMPTY_LAYOUT_ID || pApp->GetDetailID() == S3NET_TURN_OFF_LAYOUT_ID)
                {
				    pCmdUI->Enable(FALSE);
                }else
                {
				    pCmdUI->Enable(TRUE);
                }
			}
        }
        else
        {
            pCmdUI->Enable(FALSE);
        }
    }
    else
    {
        if (nSelectedCnt == 1)
        {
			 pCmdUI->Enable(TRUE);
        }
        else
        {
            pCmdUI->Enable(FALSE);
        }
    }

}

void CSchedulerPage::OnUpdateRibbonBtnStarterEditionRun(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    BOOL bRunning = ProcessCall::FindProcess(CLIENT_NAME);
    if (!bRunning)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }

}

void CSchedulerPage::OnUpdateRibbonBtnSave(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
    {
         pCmdUI->Enable(FALSE);
		return;
    }

    if(pScheduleCtrl->GetScheduleID() < 0 )
    {
#ifdef STANDALONE_EDITION
        pCmdUI->Enable(TRUE);
#else
        pCmdUI->Enable(FALSE);
#endif
		return;
    }


    BOOL bChanged = (pScheduleCtrl->m_ScheduleRangeHolder.GetModifiedRangeCount() > 0);

    INT DefaultLayoutID = 0;
    if (pScheduleCtrl->GetDefaultAppointment())
    {
        DefaultLayoutID = pScheduleCtrl->GetDefaultAppointment()->GetDetailID();

    }
    if (pScheduleCtrl->GetScheduleGroup().GetDefaultLayoutID() != DefaultLayoutID)
    {
        bChanged = TRUE;
    }

    if (bChanged)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CSchedulerPage::OnUpdateRibbonBtnExport(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
    {
        pCmdUI->Enable(FALSE);
		return;
    }

#ifndef STANDALONE_EDITION
    if(pScheduleCtrl->GetScheduleID() >= 0)
    {
         pCmdUI->Enable(FALSE);
		return;
    }
#endif

    /*BOOL bChanged = (pScheduleCtrl->m_ScheduleRangeHolder.GetModifiedRangeCount() > 0);

    INT DefaultLayoutID = 0;
    if (pScheduleCtrl->GetDefaultAppointment())
    {
        DefaultLayoutID = pScheduleCtrl->GetDefaultAppointment()->GetDetailID();

    }
    if (pScheduleCtrl->GetScheduleGroup().GetDefaultLayoutID() != DefaultLayoutID)
    {
        bChanged = TRUE;
    }

    if (bChanged)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }*/
}


void CSchedulerPage::OnUpdateRibbonBtnImport(CCmdUI* pCmdUI)
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	ScheduleCtrl* pScheduleCtrl = m_wndScheduer.GetScheduleCtrl();
	if (NULL == pScheduleCtrl)
    {
        pCmdUI->Enable(FALSE);
		return;
    }

#ifndef STANDALONE_EDITION
    if(pScheduleCtrl->GetScheduleID() >= 0)
    {
         pCmdUI->Enable(FALSE);
		return;
    }
#endif
}

LRESULT  CSchedulerPage::OnScheduleKeyDown(WPARAM wp, LPARAM lp)
{
   UINT nChar = (UINT)wp;

   UINT nRepCnt = LOWORD(lp);
   UINT nFlags = HIWORD(lp);

   if (nChar == VK_DELETE)
   {
       OnRibbonBtn_Delete();
       return 0;
   }

   BOOL bControl = (nFlags & MK_CONTROL) != 0;
   if (bControl)
   {
       if (nChar == _T('c') || nChar == _T('C'))
       {
           OnRibbonBtn_Copy();
           return 0;
       }
       else if (nChar == _T('v') || nChar == _T('V'))
       {
		   OnRibbonBtn_Paste();
           return 0;
       }
   }

    return 0;
}

BOOL CSchedulerPage::UpdateOptions()
{
#ifdef STARTER_EDITION
	//update rotation ui
	INT iSelRotation = 0;
	ClientConfigFile ClientPlayConfig;
	ClientPlayConfig.Load(GetClientDefaultDir() + CLIENT_CONFIG_FILE);
	iSelRotation = ClientPlayConfig.GetN(CLIENT_ROTATEDEGREE) / 90;
	if (iSelRotation < 0 || iSelRotation > 3)
	{
		iSelRotation = 0;
	}

	m_pComboBoxPlayerRotation->SelectItem(iSelRotation);
	
#endif
    return TRUE;
}


void CSchedulerPage::OnGroupScheduleEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	int GroupID = m_wndGroupList.GetGroupList().GetCurGroupID();

	if(GroupID != -100)
	{
		if (!m_wndScheduer.m_scheduleListTab.OpenScheduleTag(GroupID))
		{
			InitPage(GroupID);
		}
	}
}


void CSchedulerPage::OnUpdateRibbonBtnPaste(CCmdUI* pCmdUI)
{
	if (m_wndScheduer.m_scheduleListTab.GetScheduleCtrl())
	{
        if(::OpenClipboard(GetSafeHwnd()))
        {
            if(::IsClipboardFormatAvailable(ScheduleCtrl::GetClipboardFormat()))
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

void CSchedulerPage::OnUpdateRibbonBtnPrivilege(CCmdUI* pCmdUI)
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
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_SCHEDULE_GO, &CSchedulerPage::OnUpdateRibbonBtnStarterEditionRun, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_SCHEDULE_SAVE, &CSchedulerPage::OnUpdateRibbonBtnSave, LAMBDA(SC(PublishSchedule)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_SCHEDULE_COPY, &CSchedulerPage::OnUpdateRibbonBtnNeedSelect, LAMBDA(SC(PublishSchedule)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_SCHEDULE_DEL, &CSchedulerPage::OnUpdateRibbonBtnNeedSelect, LAMBDA(SC(PublishSchedule)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_SCHEDULE_HMULTICOPY, &CSchedulerPage::OnUpdateRibbonBtnNeedSelect, LAMBDA(SC(PublishSchedule)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_SCHEDULE_VMULTICOPY, &CSchedulerPage::OnUpdateRibbonBtnNeedSelect, LAMBDA(SC(PublishSchedule)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_SCHEDULE_MULTIDEL, &CSchedulerPage::OnUpdateRibbonBtnNeedSelect, LAMBDA(SC(PublishSchedule)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_SCHEDULE_PREVIEW, &CSchedulerPage::OnUpdateRibbonBtnPreview, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_SCHEDULE_SET_DEFAULT, &CSchedulerPage::OnUpdateRibbonBtnNeedSingleSelect, LAMBDA(SC(PublishSchedule) && SC(EditClientGroup)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_SCHEDULE_REMOVE_DEFAULT, &CSchedulerPage::OnUpdateRibbonBtnRemoveDefault, LAMBDA(SC(PublishSchedule) && SC(EditClientGroup)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_SCHEDULE_ALIGNUP, &CSchedulerPage::OnUpdateRibbonBtnNeedSelect, LAMBDA(SC(PublishSchedule)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_SCHEDULE_ALIGNDOWN, &CSchedulerPage::OnUpdateRibbonBtnNeedSelect, LAMBDA(SC(PublishSchedule)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_SCHEDULE_PASTE, &CSchedulerPage::OnUpdateRibbonBtnPaste, LAMBDA(SC(PublishSchedule)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_SCHEDULE_PRINT, &CSchedulerPage::OnUpdateRibbonBtn, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_SCHEDULE_EXPORT, &CSchedulerPage::OnUpdateRibbonBtnExport, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_SCHEDULE_IMPORT, &CSchedulerPage::OnUpdateRibbonBtnImport, LAMBDA(SC(PublishSchedule)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_COMBOX_SCHEDULE_TIMEDELTA, &CSchedulerPage::OnUpdateRibbonBtn, RET_TRUE());
#if defined(STARTER_EDITION) || defined(STANDALONE_EDITION)
        UPDATE_COMMAND_ENTRY(ID_RIBBON_COMBOX_PLAYER_SETTING, &CSchedulerPage::OnUpdateRibbonBtn, RET_TRUE());
#endif
#ifdef STANDALONE_EDITION
		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_ADDGROUP, &CSchedulerPage::OnUpdateRibbonBtnNormal, LAMBDA(SC(CreateClientGroup)));
		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_EDITGROUP, &CSchedulerPage::OnUpdateGroupSingleSelectRibbonBtn, LAMBDA(SC(EditClientGroup)));
		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_CLTMGMT_DELGROUP, &CSchedulerPage::OnUpdateGroupRibbonBtn, LAMBDA(SC(DeleteClientGroup)));
#endif
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_PREVIEW_10X7, &CSchedulerPage::OnUpdateRibbonBtn, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_PREVIEW_12X10, &CSchedulerPage::OnUpdateRibbonBtn, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_PREVIEW_19X10, &CSchedulerPage::OnUpdateRibbonBtn, RET_TRUE());
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
#ifdef STANDALONE_EDITION
void CSchedulerPage::OnUpdateRibbonBtnNormal(CCmdUI* pCmdUI)
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
}
void CSchedulerPage::OnUpdateGroupSingleSelectRibbonBtn( CCmdUI* pCmdUI )
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    POSITION pos = m_wndGroupList.GetGroupList().GetFirstSelectedItemPosition();
    INT index = m_wndGroupList.GetGroupList().GetNextSelectedItem(pos);

    if (index >= 0)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CSchedulerPage::OnUpdateGroupRibbonBtn( CCmdUI* pCmdUI )
{
	if (!IsEnable())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	POSITION pos = m_wndGroupList.GetGroupList().GetFirstSelectedItemPosition();
	INT index = m_wndGroupList.GetGroupList().GetNextSelectedItem(pos);


	UINT GroupID;
	CString GroupName;
	if (index >= 0 && m_wndGroupList.GetGroupList().GetSafeHwnd() != NULL)
	{
		GroupID = m_wndGroupList.GetGroupList().GetItemData(index);
		GroupName = m_wndGroupList.GetGroupList().GetItemText(index, 0);
	}

	if (index >= 0 && GroupID != S3NET_DEFAULT_CLIENTGROUP_ID)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
#endif

void CSchedulerPage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CBaseControllerPage::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	if(bShow)
	{
		CWnd *pWnd1 = m_wndSchedulerItemSelect.m_wndSchedulerSelectTab.GetTabWnd(0);
		CWnd *pWnd2 = m_wndSchedulerItemSelect.m_wndSchedulerSelectTab.GetTabWnd(1);

		if(pWnd1->IsKindOf(RUNTIME_CLASS(CLayoutCategoryCtrl)))
		{
			((CLayoutCategoryCtrl *)pWnd1)->OnClickedFilterCategoryRefresh();
			((CTimeTableCategoryCtrl*)pWnd2)->OnClickedFilterCategoryRefresh();
		}else
		{
			((CLayoutCategoryCtrl *)pWnd2)->OnClickedFilterCategoryRefresh();
			((CTimeTableCategoryCtrl*)pWnd1)->OnClickedFilterCategoryRefresh();
		}
	}
}

#ifdef STANDALONE_EDITION
BOOL CSchedulerPage::Refresh()
{
	std::vector<ClientGroup> groupList;
	HRESULT hr = GetControllerInterface->GetAllClientGroup(groupList);
	if (!SUCCEEDED(hr))
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
		{
			GetControllerApp->OnControllerServerOffline();
		}
		else
		{
			TRACE0("Failed to Get Client Group!\n");
		}
		return FALSE;
	}
	m_wndGroupList.m_wndGroupList.UpdateGroupsStatus();
	return TRUE;
}

void CSchedulerPage::OnRibbonBtn_AddGroup()
{
	HRESULT hr = E_FAIL;
	ClientManagerAddGroupDlg dlg(this);

	if (dlg.DoModal() == IDOK)
	{
		CString NewName, NewDesc;
		NewName = dlg.m_GroupName;
		NewDesc = dlg.m_GroupDesc;

		if (!NewName.IsEmpty())
		{
			ClientGroup group;
			hr = GetControllerInterface->NewClientGroup(NewName.GetString(), NewDesc.GetString(), group);
			if (!SUCCEEDED(hr))
			{
				if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
				{
					GetControllerApp->OnControllerServerOffline();
				}
				else
				{
					std::tstring desc;
					int err = GetControllerInterface->GetLastError(desc);
					MessageBox(Translate(_T("Failed to Add Client Group: ")) + Translate(desc.c_str()),Translate(_T("Error:Schedule page")), MB_OK|MB_ICONERROR);
					TRACE0("Failed to New Client Group!\n");
				}
				return;
			}

			Refresh();
			GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
				(LPARAM)(Translate(_T("New Client Group Added:")) + NewName).GetString());
		}
	}
}

void CSchedulerPage::OnRibbonBtn_DeleteGroup()
{
	if (MessageBox(Translate(_T("Are you sure to delete this Group?")),Translate(_T("Tips:Schedule page")), MB_OKCANCEL|MB_ICONQUESTION) != IDOK)
	{
		return;
	}

	INT CurGroupID = m_wndGroupList.GetGroupList().GetCurGroupID();
	if (CurGroupID >= 0)
	{
		HRESULT hr = GetControllerInterface->DeleteClientGroup(CurGroupID);
		if (!SUCCEEDED(hr))
		{
			if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
			{
				GetControllerApp->OnControllerServerOffline();
			}
			else
			{
				TRACE0("Failed to Delete Client Group!\n");
			}
			return;
		}

		POSITION pos = m_wndGroupList.GetGroupList().GetFirstSelectedItemPosition();
		INT index = m_wndGroupList.GetGroupList().GetNextSelectedItem(pos);

		CString strName = m_wndGroupList.GetGroupList().GetItemText(index, 0);
		Refresh();
		GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
			(LPARAM)(Translate(_T("Client Group Deleted:")) + strName).GetString());
	}
}


void CSchedulerPage::OnRibbonBtn_EditGroup()
{
	CEditGroup dlg(this);
	dlg.InitGroup(m_wndGroupList.GetGroupList().GetCurGroupID());

	if (IDOK == dlg.DoModal())
	{
		HRESULT hr = GetControllerInterface->EditClientGroup(dlg.GetCurGroup());
		if (!SUCCEEDED(hr))
		{
			if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
			{
				GetControllerApp->OnControllerServerOffline();
			}
			else
			{
				std::tstring desc;
				int err = GetControllerInterface->GetLastError(desc);
				if (err == S3NET_CLIENT_GROUPNAME_CONFLICT)
				{
					MessageBox(Translate(_T("Edit client group failed: group name already exists.")), NULL, MB_OK);
				}
				else if (err == S3NET_NAME_EMPTY)
				{
					MessageBox(Translate(_T("Edit client group failed: group name can't be empty.")), NULL, MB_OK);
				}
				else if (err == S3NET_CLIENT_GROUP_ID_NOT_FOUND)
				{
					MessageBox((Translate(_T("Edit client group failed: group not exists, maybe deleted by others."))), NULL, MB_OK);
				}
				TRACE0("Failed to Edit Client Group!\n");
			}
			return;
		}

		Refresh();

		GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
			(LPARAM)Translate(_T("Group Information Changed!")).GetString());
	}
}
#endif
