// F:\p4\sw\Demo\ContentManagement\Controller\Designer\Designer.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "DesignerPage.h"
#include "S3DrawRectObj.h"
#include "PropertyLayout.h"
#include "PropertyLayer.h"
#include "LayoutAddGroupCtrl.h"
#include "PlayerController.h"
#include "PreviewDownLoadDlg.h"
#include "FileTransferDlg.h"
#include "FolderDialog.h"
#include "PublishLayoutCtrl.h"
#include "LayoutWarningCtrl.h"

#include <tinyxml/tinyxml.h>
#include <strsafe.h>
#include "ZenLib/Ztring.h" //Note : I need it for universal atoi, but you have not to use it for be able to use MediaInfoLib
#include "MediaInfo/MediaInfo.h"
#include "Localization/Localization.h"
#include "Utilities/ProcessCall.h"
#include "CommonLib/base64.h"
#include <iterator>
#include "Utilities/SysCall.h"

#include "LayoutFileUtility.h"

#include "PublishLayoutDialog.h"
#include "LayoutPropertyMonitor.h"

using namespace MediaInfoLib;

const int IdProgressDownloadMediaFiles  = 3001;
// CDesignerPage

UINT WM_ON_LAYOUT_OPEN                = ::RegisterWindowMessage (_T("LAYOUT_OPEN"));
UINT WM_PROPERTY_SETTING_CHANGE		  = ::RegisterWindowMessage(_T("WM_PROPERTY_SETTING_CHANGE"));
IMPLEMENT_DYNAMIC(CDesignerPage, CBaseControllerPage)

#include <objbase.h>
char * MyGetGUID();
char * MyGetGUID()
{
	static char buf[64] = {0};
	GUID guid;
	CoInitialize(NULL);
	if (S_OK == ::CoCreateGuid(&guid))
	{
		_snprintf(buf, sizeof(buf),"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			guid.Data1,
			guid.Data2,
			guid.Data3,
			guid.Data4[0],
			guid.Data4[1],
			guid.Data4[2], 
			guid.Data4[3],
			guid.Data4[4],
			guid.Data4[5],
			guid.Data4[6],
			guid.Data4[7]);
	}
	CoUninitialize();
	return (char*)buf;
}
CDesignerPage::CDesignerPage()
{
    m_layoutSize.SetSize(0, 0);
    m_nDepth                = 0;
	m_LayoutElement = NULL;

    HRESULT hr = S_OK;
    // Init the layout group list
    std::vector<LayoutGroup>    LayoutGroupList;
    hr = GetControllerInterface->GetAllLayoutGroup(LayoutGroupList);
    if (hr != S_OK)
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
        }
        else
        {
            MessageBox(Translate(_T("Get group name failed!")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
        }
    }

    auto it = LayoutGroupList.begin(), ite = LayoutGroupList.end();
    for (; it != ite; ++it)
    {
        LAYOUTGROUP_SETTING* pGroup = new LAYOUTGROUP_SETTING();
        pGroup->GroupID     = it->GetGroupID();
        pGroup->GroupName   = it->GetGroupName().c_str();
		pGroup->GroupDesc   = it->GetGroupDesc().c_str();
        m_LayoutGroups.AddTail(pGroup);
    }
}

CDesignerPage::~CDesignerPage()
{
    // Remove layout groups
    RemoveLayoutGroups();

    PlayerController::GetInstance()->StopPlayer();

    // Close layout, clean the draw obj list and layout setting hash map
    CloseLayout();

    // Clean the plugin renderable obj array
}

BEGIN_MESSAGE_MAP(CDesignerPage, CBaseControllerPage)
    ON_WM_CREATE()
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_NEW, OnRibbonBtnNew)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_CLOSE, OnRibbonBtnClose)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_PUBLISH, OnRibbonBtnPublish)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_EXPORT, OnRibbonBtnExport)
    //ON_COMMAND(ID_RIBBON_BTN_DESIGNER_IMPORT, OnRibbonBtnImport)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_UNDO, OnRibbonBtnUndo)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_REDO, OnRibbonBtnRedo)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_DELETEITEM, OnRibbonBtnDeleteItem)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_COPY, OnRibbonBtnCopy)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_CUT, OnRibbonBtnCut)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_PASTE, OnRibbonBtnPaste)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_SELECTALL, OnRibbonBtnSelectAll)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_PREVIEWWINMODE, OnRibbonBtnPreviewWinMode)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_PREVIEW, OnRibbonBtnPreview)
    ON_COMMAND(ID_RIBBON_BTN_PREVIEW_10X7, OnRibbonBtnPreview10X7)
    ON_COMMAND(ID_RIBBON_BTN_PREVIEW_12X10, OnRibbonBtnPreview12X10)
    ON_COMMAND(ID_RIBBON_BTN_PREVIEW_19X10, OnRibbonBtnPreview19X12)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_REFRESH, OnRibbonBtnRefresh)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_MOVETOFRONT, OnRibbonBtnMoveToFront)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_MOVEFORWARD, OnRibbonBtnMoveForward)
   
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_MOVETOBACK, OnRibbonBtnMoveToBack)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_MOVEBACKWARD, OnRibbonBtnMoveBackward)
    ON_COMMAND(ID_RIBBON_COMBOX_LAYOUT_ZOOM, OnRibbonBtnZoom)
	ON_COMMAND(ID_RIBBON_BTN_DESIGNER_BINDLAY,OnRibbonBtnBindLay)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)

    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_SAME_INTERVAL1, OnRibbonBtnSameIntervalX)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_SAME_INTERVAL2, OnRibbonBtnSameIntervalY)
    ON_COMMAND(ID_RIBBON_BTN_DESIGNER_FILL_LAYOUT, OnRibbonBtnSameFillLayout)
    
    ON_REGISTERED_MESSAGE(WM_ON_LAYOUT_ZOOM, OnLayoutZoom)
    ON_REGISTERED_MESSAGE(WM_ON_LAYTOUT_SELECT_OBJ, OnLayoutSelectObj)
    ON_REGISTERED_MESSAGE(WM_ON_LAYOUT_REMOVE_OBJ, OnLayoutRemoveObj)
    ON_REGISTERED_MESSAGE(WM_ON_LAYOUT_INSERT_OBJ, OnLayoutInsertObj)
    ON_REGISTERED_MESSAGE(WM_ON_LAYOUT_ACTION, OnLayoutAction)

    ON_REGISTERED_MESSAGE(WM_ON_OBJ_ADDMEDIA, OnObjAddMedia)

    ON_REGISTERED_MESSAGE(WM_ON_LAYOUT_SIZE_CHANGE, OnLayoutSizeChange)

	ON_REGISTERED_MESSAGE(WM_ON_SIZE_POS_CHANGE2, OnLayerSizeChange2)
    ON_REGISTERED_MESSAGE(WM_ON_MEDIA_SEL_CHANGE, OnLayerMediaSelChange)
    ON_REGISTERED_MESSAGE(WM_ON_LAYOUT_OPEN, OnLayoutOpen)
    ON_REGISTERED_MESSAGE(WM_ON_LAYER_LOOP_CHANGE, OnLayerLoopChange)
    ON_REGISTERED_MESSAGE(WM_ON_LAYER_SEL_CHANGE, OnLayerSelChange)
    ON_REGISTERED_MESSAGE(WM_ON_LAYER_NAME_CHANGE, OnLayerNameChange)
	ON_REGISTERED_MESSAGE(DESIGNER_KEYDOWN, OnDesignerKeyDown)
	ON_REGISTERED_MESSAGE(WM_ON_SIMPLE_PLUGIN_SIZE_CHANGE,OnSimplePluginSizeChange)

#ifndef STARTER_EDITION
    ON_REGISTERED_MESSAGE(MEDIA_LIST_CATEGORY, MediaCategoryListSelChange)
#endif

	ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_BINDLAY ,OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_NEW, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_OPEN, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_CLOSE, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_SAVE, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_SAVEAS, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_PUBLISH, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_EXPORT, OnUpdateRibbonBtn)
    //ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_IMPORT, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_PASTE, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_COPY, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_DELETEITEM, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_SELECTALL, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_PREVIEW, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_MOVETOFRONT, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_MOVEFORWARD, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_MOVETOBACK, OnUpdateRibbonBtn)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_MOVEBACKWARD, OnUpdateRibbonBtn)
    
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_SAME_INTERVAL1, OnRibbonBtnUpdateSameInterval)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_SAME_INTERVAL2, OnRibbonBtnUpdateSameInterval)
    ON_UPDATE_COMMAND_UI(ID_RIBBON_BTN_DESIGNER_FILL_LAYOUT, OnRibbonBtnUpdateFillLayout)

    ON_UPDATE_COMMAND_UI(ID_RIBBON_COMBOX_LAYOUT_ZOOM, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateRibbonBtn)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateRibbonBtn)

    
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


/// CDesign message handle
int CDesignerPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (!CreateRibbonButtons())
    {
        TRACE0("Failed to create layout designer ribbon buttons\n");
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

    // Create and Install the media file browser pane
#ifndef STARTER_EDITION
    right   = rect.Width() / 6;
    bottom  = 5 * rect.Height() / 6;
#else
    right   = rect.Width() / 6;
    bottom  = rect.Height() / 2;
#endif

    if (!m_wndMediaFileBrowser.Create(_T("Media Category"),
        this, CRect(left, top, right, bottom), TRUE, ID_VIEW_MEDIAFILES,
        WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create media folders pane\n");
        return -1;      // fail to create
    }
    m_wndMediaFileBrowser.EnableDocking(CBRS_LEFT);
    DockPane(&m_wndMediaFileBrowser);


#ifndef STARTER_EDITION

    // Create and Install the media file explorer pane
    top     = bottom;
    bottom  = rect.bottom;

    if (!m_wndMediaInfo.Create(Translate(_T("Media Info")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_MEDIALIBRARY_INFO, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, NULL))
    {
        TRACE0("Failed to create MediaLibrary filter pane\n");
        return FALSE;      // fail to create
    }

    m_wndMediaInfo.EnableDocking(CBRS_BOTTOM);
    m_wndMediaInfo.DockToWindow(&m_wndMediaFileBrowser, CBRS_ALIGN_BOTTOM, CRect(left, top, right, bottom));

#else

    // Create and Install the media file explorer pane
    top     = bottom;
    bottom  = rect.bottom;

    if (!m_wndMediaFileExplorer.Create(Translate(_T("Media Files")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_SCHEDULEITEM, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create scheduler item edit pane\n");
        return -1;      // fail to create
    }
    m_wndMediaFileExplorer.SetMinSize(CSize(250,90));
    m_wndMediaFileExplorer.EnableDocking(CBRS_BOTTOM);
    m_wndMediaFileExplorer.DockToWindow(&m_wndMediaFileBrowser, CBRS_ALIGN_BOTTOM);

    CRect rectExplorer;
    m_wndMediaFileExplorer.GetClientRect(rectExplorer);
    m_wndFileList.SetPluginSupportFiles(&GetControllerApp->m_PluginType);
    m_wndFileList.Create (WS_CHILD | WS_VISIBLE | LVS_REPORT, rectExplorer, &m_wndMediaFileExplorer, IDC_MEDIAFILESLIST);
    m_wndFileList.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
    m_wndMediaFileBrowser.m_wndShellTree.SetRelatedList(&m_wndFileList);
    m_wndMediaFileExplorer.SetMainWnd(&m_wndFileList);

#endif

    // Create and Install the Properties pane
    left    = rect.Width() / 6 * 5;
    right   = rect.right;
    top     = rect.top;
    bottom  = rect.bottom;

 


    if (!m_wndLayoutProperties.Create(Translate(_T("Properties")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_LAYOUTPROPERTIES, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create layout properties pane\n");
        return -1;      // fail to create
    }
    m_wndLayoutProperties.EnableDocking(CBRS_RIGHT);
    DockPane(&m_wndLayoutProperties);
     
	CLayoutPropertyMonitor::Instance()->AddPropertyWnd((CWnd *)m_wndLayoutProperties.m_PropertyCtrl.m_pPropertyBasicContent);
	CLayoutPropertyMonitor::Instance()->AddPropertyWnd((CWnd *)m_wndLayoutProperties.m_PropertyCtrl.m_pPropertyLayer);
	CLayoutPropertyMonitor::Instance()->AddPropertyWnd((CWnd *)m_wndLayoutProperties.m_PropertyCtrl.m_pPropertyMessage);
	CLayoutPropertyMonitor::Instance()->AddPropertyWnd((CWnd *)m_wndLayoutProperties.m_PropertyCtrl.m_pPropertySimplePlugin);
	CLayoutPropertyMonitor::Instance()->AddPropertyWnd((CWnd *)m_wndLayoutProperties.m_PropertyCtrl.m_pPropertyTransitionOut);
	CLayoutPropertyMonitor::Instance()->AddPropertyWnd((CWnd *)m_wndLayoutProperties.m_PropertyCtrl.m_pPropertyPlugin);
	CLayoutPropertyMonitor::Instance()->AddPropertyWnd((CWnd *)m_wndLayoutProperties.m_PropertyCtrl.m_pPropertyLayout);
	CLayoutPropertyMonitor::Instance()->AddPropertyWnd((CWnd *)m_wndLayoutProperties.m_PropertyCtrl.m_pPropertyTransitionIn);
	CLayoutPropertyMonitor::Instance()->AddPropertyWnd((CWnd *)this);
    // Create and Install the component pane
    top     = 0;
    left    = rect.Width() / 6;
    right   = rect.Width() / 6 * 5;
    bottom  = rect.Height() / 8;

    if (!m_wndComponentPane.Create(Translate(_T("Components")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_COMPONENT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create component pane\n");
        return -1;      // fail to create
    }
    m_wndComponentPane.EnableDocking(CBRS_TOP);
    DockPane(&m_wndComponentPane);

    CRect rectList;
    m_wndComponentList.InitPluginObj(GetControllerApp->GetPluginInfo());
    m_wndComponentPane.GetClientRect(rectList);
    m_wndComponentList.Create (WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS,
        rectList, &m_wndComponentPane, IDC_COMPONENTLIST);
    m_wndComponentList.OnInitialUpdate();
    m_wndComponentPane.SetMainWnd(&m_wndComponentList);
    m_wndComponentPane.SetMinSize(CSize(0, 80));

    CStringArray arrComponentNames;
    m_wndComponentList.GetComponentNames(arrComponentNames);
    for (int i = 0; i < arrComponentNames.GetCount(); i++)
    {
        CString szComponent = arrComponentNames.GetAt(i);
        int nColor = i % 8;
        m_ObjFillColor.SetAt(szComponent, FillColor[nColor]);
    }

    // Create and Install the Templates pane
    top     = bottom + rect.Height() / 8 * 6;
    bottom  = rect.bottom;

    if (!m_wndLayoutTemplates.Create(Translate(_T("Layout Templates")), this, CRect(left, top, right, bottom),
        TRUE, ID_VIEW_LAYOUTTEMPLATES, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM | CBRS_HIDE_INPLACE | WS_CAPTION,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create layout templates pane\n");
        return -1;      // fail to create
    }
    m_wndLayoutTemplates.EnableDocking(CBRS_BOTTOM);
    DockPane(&m_wndLayoutTemplates);
    ShowPane(&m_wndLayoutTemplates, FALSE, FALSE, TRUE);

    // Create and Install the designer pane, should fix the size of the client area
    top     = rect.Height() / 8 + 1;
    bottom  = rect.Height() / 8 * 2 - 1;

	if (!m_wndLayoutDesign.Create(NULL, this, CRect(left, top, right, bottom),
		TRUE, ID_VIEW_LAYOUTDESIGNER, WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_HIDE_INPLACE,
        AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
    {
        TRACE0("Failed to create layout designer pane\n");
        return -1;      // fail to create
    }

    SetMainWnd(&m_wndLayoutDesign);

    m_pComboBoxZoom->SelectItem(2);

    return 0;
}


void CDesignerPage::OnDestroy()
{
    CBaseControllerPage::OnDestroy();

}

void CDesignerPage::Close()
{
    m_wndMediaFileBrowser.DestroyWindow();
    m_wndMediaFileExplorer.DestroyWindow();
    m_wndLayoutDesign.DestroyWindow();
    m_wndLayoutTemplates.DestroyWindow();
    m_wndLayoutProperties.DestroyWindow();
    m_wndComponentPane.DestroyWindow();
    m_wndMediaInfo.DestroyWindow();
}

BOOL CDesignerPage::CreateRibbonButtons()
{
    if (!m_pCategory)
    {
        return FALSE;
    }

    /// Remove the dummy panel, and create the real panels
    m_pCategory->RemovePanel(0);

    CString strPanelFile        = Translate(_T("File"));
    CString strPanelGroup       = Translate(_T("Group"));
    CString strPanelEdit        = Translate(_T("Edit"));
    CString strPanelView        = Translate(_T("Preview"));
    CString strPanelArrange     = Translate(_T("Arrange"));
    CString strZoom             = Translate(_T("Design View"));

    CMFCRibbonPanel* pPanelFile = m_pCategory->AddPanel(strPanelFile);


    CMFCRibbonButton* pBtnPublish = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_PUBLISH, Translate(_T("Publish")), 5, 5);
    pBtnPublish->SetAlwaysLargeImage();
	pBtnPublish->SetToolTipText(Translate(_T("Publish")));
    pBtnPublish->SetDescription(Translate(_T("Package the layout and upload to controller server.")));
    pPanelFile->Add(pBtnPublish);


    CMFCRibbonButton* pBtnClose = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_CLOSE, Translate(_T("Close")), 2, 2);
    pBtnClose->SetAlwaysLargeImage();
	pBtnClose->SetToolTipText(Translate(_T("Close")));
    pBtnClose->SetDescription(Translate(_T("Close current layout.")));
    pPanelFile->Add(pBtnClose);


	/* modified by RaymondLiu
    CMFCRibbonButton* pBtnImport = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_IMPORT, Translate(_T("Import")), 7, 7);
    pBtnImport->SetAlwaysLargeImage();
	pBtnImport->SetToolTipText(Translate(_T("Import")) + _T("\n") + Translate(_T("Import existing layout from Local")));
    pPanelFile->Add(pBtnImport);
	*/

    CMFCRibbonButton* pBtnExport = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_EXPORT, Translate(_T("Export")), 6, 6);
	pBtnExport->SetToolTipText(Translate(_T("Export")));
    pBtnExport->SetDescription(Translate(_T("Export layout to File(.xml).")));
    pBtnExport->SetAlwaysLargeImage();
    pPanelFile->Add(pBtnExport);



    CMFCRibbonPanel* pPanelEdit = m_pCategory->AddPanel(strPanelEdit);

    CMFCRibbonButton* pBtnPaste = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_PASTE, Translate(_T("Paste")), 9, 9);
    pBtnPaste->SetAlwaysLargeImage();
	pBtnPaste->SetToolTipText(Translate(_T("Paste")));
    pBtnPaste->SetDescription(Translate(_T("Paste the copied item.")));
    pPanelEdit->Add(pBtnPaste);

    CMFCRibbonButton* pBtnCopy = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_COPY, Translate(_T("Copy")), 10, 10);
    pBtnCopy->SetAlwaysLargeImage();
	pBtnCopy->SetToolTipText(Translate(_T("Copy")));
    pBtnCopy->SetDescription(Translate(_T("Copy the selected item.")));
    pPanelEdit->Add(pBtnCopy);

    //CMFCRibbonButton* pBtnCut = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_CUT, _T("Cut"), 1, -1);
    //pBtnCut->SetAlwaysLargeImage(FALSE);
    //pPanelEdit->Add(pBtnCut);

    CMFCRibbonButton* pBtnDelItem = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_DELETEITEM, Translate(_T("Delete")), 11, 11);
    pBtnDelItem->SetAlwaysLargeImage();
	pBtnDelItem->SetToolTipText(Translate(_T("Delete")));
    pBtnDelItem->SetDescription(Translate(_T("Delete the selected item.")));
    pPanelEdit->Add(pBtnDelItem);

    CMFCRibbonButton* pBtnSelAll = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_SELECTALL, Translate(_T("Select All")), 12, 12);
    pBtnSelAll->SetAlwaysLargeImage();
	pBtnSelAll->SetToolTipText(Translate(_T("Select All")));
    pBtnSelAll->SetDescription(Translate(_T("Select all items on the layout.")));
    pPanelEdit->Add(pBtnSelAll);

    CMFCRibbonButton* pBtnUndo = new CMFCRibbonButton(ID_EDIT_UNDO, Translate(_T("Undo")), 20, 20);
    pBtnUndo->SetAlwaysLargeImage(FALSE);
	pBtnUndo->SetToolTipText(Translate(_T("Undo")));
	pBtnUndo->SetDescription(Translate(_T("Undo the last action")));
    pPanelEdit->Add(pBtnUndo);

    CMFCRibbonButton* pBtnRedo = new CMFCRibbonButton(ID_EDIT_REDO, Translate(_T("Redo")), 19, 19);
    pBtnRedo->SetAlwaysLargeImage(FALSE);
	pBtnRedo->SetToolTipText(Translate(_T("Redo")));
	pBtnRedo->SetDescription(Translate(_T("Redo the previously undone action")));
    pPanelEdit->Add(pBtnRedo);


    CMFCRibbonPanel* pPanelView = m_pCategory->AddPanel(strPanelView);

    //CMFCRibbonButton* pBtnPreviewWin = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_PREVIEWWINMODE, _T("Preview Window Mode"), 0, 0);
    //pPanelView->Add(pBtnPreviewWin);

    CMFCRibbonButton* pBtnPreview = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_PREVIEW, Translate(_T("Preview")), 13, 13);
    //pBtnPreview->SetMenu(IDR_MENU_PREVIEW);
    pBtnPreview->AddSubItem(new CMFCRibbonButton(ID_RIBBON_BTN_PREVIEW_10X7, _T("1024 x 768")));
    pBtnPreview->AddSubItem(new CMFCRibbonButton(ID_RIBBON_BTN_PREVIEW_12X10, _T("1280 x 1024")));
    pBtnPreview->AddSubItem(new CMFCRibbonButton(ID_RIBBON_BTN_PREVIEW_19X10, _T("1920 x 1080")));

	pBtnPreview->SetToolTipText(Translate(_T("Preview")));
    pBtnPreview->SetDescription(Translate(_T("Preview layout in selected dimension.")));
    pPanelView->Add(pBtnPreview);

    //CMFCRibbonButton* pBtnRefresh = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_REFRESH, _T("Refresh"), 2, 2);
    //pPanelView->Add(pBtnRefresh);

    CMFCRibbonPanel* pPanelArrange = m_pCategory->AddPanel(strPanelArrange);

    CMFCRibbonButton* pBtnMoveToFront = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_MOVETOFRONT, Translate(_T("Move to Top")), 14, 14);
	pBtnMoveToFront->SetToolTipText(Translate(_T("Move to Top")));
    pBtnMoveToFront->SetDescription(Translate(_T("Move the layer to top.")));
    pPanelArrange->Add(pBtnMoveToFront);

    CMFCRibbonButton* pBtnMoveForward = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_MOVEFORWARD, Translate(_T("Move Forward")), 15, 15);
    pBtnMoveForward->SetToolTipText(Translate(_T("Move Forward")));
    pBtnMoveForward->SetDescription(Translate(_T("Move the layer forward.")));
	pPanelArrange->Add(pBtnMoveForward);

    CMFCRibbonButton* pBtnMoveToBack = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_MOVETOBACK, Translate(_T("Move to Bottom")), 16, 16);
    pBtnMoveToBack->SetToolTipText(Translate(_T("Move to Bottom")));
    pBtnMoveToBack->SetDescription(Translate(_T("Move the layer to bottom.")));
	pPanelArrange->Add(pBtnMoveToBack);

    CMFCRibbonButton* pBtnMoveBackward = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_MOVEBACKWARD, Translate(_T("Move Backward")), 17, 17);
    pBtnMoveBackward->SetToolTipText(Translate(_T("Move Backward")));
    pBtnMoveBackward->SetDescription(Translate(_T("Move the layer backward.")));
    pPanelArrange->Add(pBtnMoveBackward);

    //same interval in x order.
    CMFCRibbonButton* pBtnSameInterval1 = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_SAME_INTERVAL1, Translate(_T("Average X Interval")), 17, 17);
    pBtnSameInterval1->SetToolTipText(Translate(_T("Average X Interval")));
    pBtnSameInterval1->SetDescription(Translate(_T("Average Interval left to right")));
    pPanelArrange->Add(pBtnSameInterval1);

    //same interval in y order
    CMFCRibbonButton* pBtnSameInterval2 = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_SAME_INTERVAL2, Translate(_T("Average Y Interval")), 17, 17);
    pBtnSameInterval2->SetToolTipText(Translate(_T("Average Y Interval")));
    pBtnSameInterval2->SetDescription(Translate(_T("Average Interval up to down")));
    pPanelArrange->Add(pBtnSameInterval2);

    //fill layout client.
    CMFCRibbonButton* pBtnMFillLayout = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_FILL_LAYOUT, Translate(_T("Fit Layout")), 17, 17);
    pBtnMFillLayout->SetToolTipText(Translate(_T("Fit Layout")));
    pBtnMFillLayout->SetDescription(Translate(_T("Suitable size to fit Layout")));
    pPanelArrange->Add(pBtnMFillLayout);

	//   CMFCRibbonButton* pBtnBindLinesModel = new CMFCRibbonButton(ID_RIBBON_BTN_DESIGNER_BINDLAY, Translate(_T("Client Lay")), 17, 17);
	//pBtnMoveBackward->SetToolTipText(Translate(_T("Move Backward")));
	//pBtnMoveBackward->SetDescription(Translate(_T("Layout ")));
	//pPanelArrange->Add(pBtnBindLinesModel);
    CMFCRibbonPanel* pPanelZoom = m_pCategory->AddPanel(strZoom);

    m_pComboBoxZoom = new CMFCRibbonComboBox(ID_RIBBON_COMBOX_LAYOUT_ZOOM, TRUE, 50, Translate(_T("Zoom")));
	m_pComboBoxZoom->AddItem(Translate(_T("Auto Fit")));
    m_pComboBoxZoom->AddItem(_T("400%"));
    m_pComboBoxZoom->AddItem(_T("200%"));
    m_pComboBoxZoom->AddItem(_T("100%"));
    m_pComboBoxZoom->AddItem(_T("75%"));
    m_pComboBoxZoom->AddItem(_T("50%"));
   // m_pComboBoxZoom->AddItem(_T("25%"));
	m_pComboBoxZoom->SetToolTipText(Translate(_T("Zoom")));
    m_pComboBoxZoom->SetDescription(Translate(_T("Change the Zoom rate of the design page.")));
    pPanelZoom->Add(m_pComboBoxZoom);

    //CMFCRibbonPanel* pPanelProgress = m_pCategory->AddPanel(_T("Download Progress"));

    //pPanelProgress->Add(new CMFCRibbonLabel(_T("Progress:")));
    //pPanelProgress->Add(new CMFCRibbonProgressBar(ID_RIBBON_LAYOUT_PROGRESS, 100 /* Bar width */));
    //pPanelProgress->Add(new CMFCRibbonButton(ID_RIBBON_BTN_LAYOUT_PROGRESS, _T("Start")));
    return TRUE;
}


BOOL CDesignerPage::UpdateOptions()
{
    return m_wndLayoutDesign.m_wndDesign.UpdateOptions();
}


void CDesignerPage::NewLayout(int width, int height, FLOAT fRotateDegree)
{
    BOOL bResize = FALSE;
    CRect rect;
    CRect position;
    FRect rectLogic;
    FRect rectLogicPre;
    FLOAT fWidth    = 0.0f;
    FLOAT fHeight   = 0.0f;

    if (m_layoutSize.cx != 0)
    {
        fWidth = (FLOAT)width / (FLOAT)m_layoutSize.cx;
    }
    if (m_layoutSize.cy != 0)
    {
        fHeight = (FLOAT)height / (FLOAT)m_layoutSize.cy;
    }

    m_layoutSize.SetSize(width, height);
    m_wndLayoutDesign.m_wndDesign.SetLayoutSize(m_layoutSize);
    m_wndLayoutDesign.m_wndDesign.SetupScrollBars(TRUE);
	m_wndLayoutDesign.m_wndDesign.CalcTransform();

    /// Create layout, background
    rect.left   = 0;
    rect.top    = 0;
    rect.right  = m_layoutSize.cx;
    rect.bottom = m_layoutSize.cy;

    BOOL bExist = FALSE;
    CS3DrawObj* pObj = NULL;
    PROPERTY_TYPE propertyType;
    POSITION pos = m_DrawObjList.GetHeadPosition();
    while (pos != NULL)
    {
        pObj = m_DrawObjList.GetNext(pos);
        pObj->GetPropertyType(propertyType);
        if (propertyType == BACKGROUND)
        {
            bExist = TRUE;
            position = rect;
            rectLogic = rect;
            m_wndLayoutDesign.m_wndDesign.LayoutToClient(position);
            pObj->SetLogicPos(rectLogic);
            pObj->SetPosition(position);
        }
        else
        {
            pObj->GetLogicPos(rectLogicPre);
            rectLogic.left      = rectLogicPre.left * fWidth;
            rectLogic.right     = rectLogicPre.right * fWidth;
            rectLogic.top       = rectLogicPre.top * fHeight;
            rectLogic.bottom    = rectLogicPre.bottom * fHeight;
            rectLogic.ConvertToCRect(position);
            m_wndLayoutDesign.m_wndDesign.LayoutToClient(position);
            pObj->SetLogicPos(rectLogic);
            pObj->SetPosition(position);
        }
    }

	if(!m_wndLayoutDesign.m_wndDesign.IsBindLayEnable()) 
	{
	    AXIS_ARRAY &xArray =  m_wndLayoutDesign.m_Ruler.GetXBindLines();
	    AXIS_ARRAY &yArray =  m_wndLayoutDesign.m_Ruler.GetYBindLines();
	    for( int i = 0;i < xArray.size();i++)
	    {
		    xArray[i] = xArray[i] * fWidth;
	    }

	    for(int j = 0;j < yArray.size();j++)
	    {
		    yArray[j] = yArray[j] * fHeight;
		}
	}
    if (!bExist)
    {
        if (m_LayoutElement == NULL)
        {
            m_LayoutElement = new LayoutElement;
        }
		m_LayoutElement->SetLayoutSize(CSize(width, height));
       // m_pLayoutSetting->LayoutSize.SetSize(width, height);
        m_wndLayoutProperties.m_PropertyCtrl.SetLayout(m_LayoutElement, &m_LayoutGroups, &m_DrawObjList);

        CreateRectDrawObj(rect, BACKGROUND, _T(""));
    }

    m_wndLayoutDesign.m_wndDesign.RedrawWindow();
    m_wndLayoutDesign.m_Ruler.RedrawWindow();

}

void CDesignerPage::OnRibbonBtnNew()
{
    if (m_layoutSize.cx != 0 || m_layoutSize.cy != 0)
    {
        MessageBox(Translate(_T("Already opened a layout, please save or close the currently opened layout first!")), Translate(_T("Tips:New layout")), MB_OK|MB_ICONINFORMATION);
        return;
    }

    //ShowPane(&m_wndLayoutTemplates, TRUE, FALSE, TRUE);

    //m_ComponentObjCount.RemoveAll();

    m_nDepth = 0;
    NewLayout();

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)(Translate(_T("New Layout Created"))).GetString());
}

void CDesignerPage::CreateLayout(INT GroupID)
{
    OnRibbonBtnNew();
	m_LayoutElement->SetGroupID(GroupID);
    InitLayout();
}



void CDesignerPage::ImportLayout(int groupId, const CString& path)
{
    OnRibbonBtnNew();
    if (m_layoutSize.cx > 0 || m_layoutSize.cy > 0)
    {
        m_wndLayoutDesign.m_wndDesign.CloseLayout();

        CloseLayout();

        m_wndLayoutDesign.m_wndDesign.RedrawWindow();
        m_wndLayoutProperties.m_PropertyCtrl.Clear();
    }

    LoadLayoutFile(path);
    m_LayoutElement->SetGroupID(groupId);
    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)(Translate(_T("Layout Imported Successfully:")) + m_LayoutElement->GetLayoutName()).GetString());
}


void CDesignerPage::OnRibbonBtnClose()
{
    //ShowPane(&m_wndLayoutTemplates, FALSE, FALSE, TRUE);
    CString szLayoutName;
    if (m_LayoutElement)
    {
        szLayoutName = m_LayoutElement->GetLayoutName();
    }

	CString mess = Translate(_T("Don't forget to publish or save the layout!")) + _T("\n") + Translate(_T("Confirm to close the layout?"));
	if (MessageBox(mess, Translate(_T("Tips")), MB_OKCANCEL) == IDCANCEL)
	{
        return;
    }

    m_wndLayoutDesign.m_wndDesign.CloseLayout();

    CloseLayout();

    m_wndLayoutDesign.m_wndDesign.RedrawWindow();
    m_wndLayoutProperties.m_PropertyCtrl.Clear();

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)(Translate(_T("Layout Closed:")) + szLayoutName).GetString());

    GetParent()->PostMessage(CLOSE_CURRENT_PAGE, NULL, NULL);
}

void CDesignerPage::FocusLayoutNameEditCtrl()
{
	CLayoutPropertyMonitor::WND_HANDLES &propWnds = CLayoutPropertyMonitor::Instance()->GetWnds();
	CLayoutPropertyMonitor::WND_HANDLES::iterator it = propWnds.begin();

	CWnd *pProp = NULL;
	while( it != propWnds.end())
	{
		if( it->first->IsKindOf(RUNTIME_CLASS(CPropertyLayout)) 
			&&(this->IsChild(it->first)))
		{
			pProp =it->first;
			if(!pProp->IsWindowVisible())
			{
				OnLayoutSelectObj(1,0);
			}

			break;
		}

		it++;
	}

	if(pProp)
	{
		CString strText;
		CEdit *pLayoutNameCtrl = (CEdit *)pProp->GetDlgItem(IDC_EDIT_NAME);
		pProp->SetFocus();
		
		pLayoutNameCtrl->GetWindowText(strText);
		pLayoutNameCtrl->SetSel(0,strText.GetLength());
		pLayoutNameCtrl->SetFocus();
	}
}

void CDesignerPage::OnRibbonBtnPublish()
{
    HRESULT hr = S_OK;
	BOOL bExsitLayout = FALSE;

	int layoutID = S3NET_NEW_LAYOUT_ID;
    hr = CheckLayout(layoutID);
    if (!SUCCEEDED(hr))
    {
        if (hr ==E_INVALIDARG/*!= HRESULT_FROM_WIN32(ERROR_NOT_FOUND)*/)//E_INVALIDARG ,use as layout name error.
        {
			//2012.4.12 zxy 
			//this code will show dialog-box twice when name error occur,remove it. 
			// MessageBox(Translate(_T("Failed to publish layout.")), Translate(_T("Error:Publish layout")),MB_OK|MB_ICONERROR);

			//set input box focus.
			FocusLayoutNameEditCtrl();
            return;
		}
    }
	
    Layout layout;
    if (layoutID != S3NET_NEW_LAYOUT_ID)
    {
		bExsitLayout = TRUE;

        S3Time nearestRefTime = S3Time::Now();
        std::vector<int> scheduleIDList;
		std::vector<int> ClientGroupIDList;
		std::vector<int> refTimetableIDList;
        hr = GetControllerInterface->CheckLayoutRef(layoutID, nearestRefTime, scheduleIDList, ClientGroupIDList , refTimetableIDList);
        if (!SUCCEEDED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                GetControllerApp->OnControllerServerOffline();
            }
            else
            {
                MessageBox(Translate(_T("The layout which will be published is already exist, but can not get its information.")), Translate(_T("Error:Publish layout")), MB_OK|MB_ICONERROR);
            }
            return;
        }

        if (scheduleIDList.size() == 0 && ClientGroupIDList.size() == 0 && refTimetableIDList.size() ==0 )
        {
            // TRANS:
			CString messStr = Translate(_T("The layout already exists, continue publish will update the existing data.")) + _T("\r\n") +
				Translate(_T("Click")) + _T("\"")+ Translate(_T("OK")) + _T("\"") + Translate(_T("to continue publish, Click")) + _T("\"") +
				Translate(_T("Cancel")) + _T("\"") + Translate(_T("to cancel the publish."));
            if (MessageBox(messStr, Translate(_T("Warning")), MB_OKCANCEL) == IDCANCEL)
            {
                return;
            }
        }
        else
        {
            MessageBox(Translate(_T("The layout already exists and is in schedule list, Can not publish the layout! Please edit the schedule list first!")),
                Translate(_T("Warning:Publish layout")), MB_OK|MB_ICONEXCLAMATION);
            return;
        }
    }

    CString szLayoutFile;
    CString szLayoutThumbnail = _T("");
    CString szLocalLayoutPath;
    CString szSeverLayoutPath;
    CString szLayoutName = m_LayoutElement->GetLayoutName();

    // filter all the space
    szLayoutName.Replace(_T(" "), _T(""));
    if (szLayoutName.IsEmpty())
    {
        MessageBox(Translate(_T("The layout name is invalid. Can not publish the layout!")), Translate(_T("Warning:Publish layout")), MB_OK|MB_ICONERROR);
        return;
    }

    SYSTEMTIME times;

    GetSystemTime(&times);
    //szLayoutName.AppendFormat(_T("_%d.%0.2d.%0.2d_%0.2d.%0.2d.%0.2d"), times.wYear, times.wMonth, times.wDay, times.wHour, times.wMinute, times.wSecond);

    GetLocalTempPath(szLocalLayoutPath);
    GetMediaServerLayoutPath(szSeverLayoutPath);

//#ifdef STARTER_EDITION

    szLocalLayoutPath += szLayoutName + _T("\\");

//#endif

    hr = PackLayout(szLocalLayoutPath, szLayoutFile, szLayoutThumbnail);
    if (hr != S_OK)
    {
        MessageBox(Translate(_T("Failed to pack the layout files!")), Translate(_T("Error:Pack layout")), MB_OK|MB_ICONERROR);
        return;
    }

    // Popup window to display the layout info: canvas size, total duration, total size, each media files' size
    // Need user confirm to continue publish.
    CString szLayoutInfo;
    CString ContentXML;
    hr = WriteLayoutFile(szLayoutFile, ContentXML);

    if (hr != S_OK)
    {
        MessageBox(Translate(_T("Failed to generate the layout file!")), Translate(_T("Error:Publish layout")), MB_OK|MB_ICONERROR);
        return;
    }


    if ((szSeverLayoutPath.ReverseFind(_T('\\')) + 1) == szSeverLayoutPath.GetLength())
    {
        szSeverLayoutPath.Append(szLayoutName);
    }
    else
    {
        szSeverLayoutPath.AppendFormat(_T("\\%s"), szLayoutName);
    }

    Layout publishLayout;

    vector<INT> MediaIDs;

    if (!GetMediaIDListFromLayout(m_LayoutElement, MediaIDs))
    {
        MessageBox(Translate(_T("Failed to generate the layout media list!")), Translate(_T("Error:Publish layout")), MB_OK|MB_ICONERROR);
        return;
    }

    publishLayout.SetMediaIDList(MediaIDs);

	DWORD64 MediaContentSize = 0;
	if(MediaIDs.size())
	{
		//add by raymondliu
		std::vector<Media> Medias;
		hr = GetControllerInterface->GetMedias(MediaIDs, Medias);
		if(FAILED(hr))
		{
			MessageBox(Translate(_T("Failed to get the layout media list!")), Translate(_T("Error:Publish layout")), MB_ICONERROR);
			return;
		}
		MediaContentSize = 0;
		for(auto iter = Medias.begin(); iter != Medias.end() ;++iter)
		{
			MediaContentSize +=  iter->GetFileSize();
		}
	}
	else
	{
		MediaContentSize = 0;
	}
	m_LayoutElement->SetMediaContentSize(MediaContentSize);

    if (layoutID != S3NET_NEW_LAYOUT_ID)
    {
        hr = GetControllerInterface->GetLayout(layoutID, publishLayout);
        if (!SUCCEEDED(hr))
        {
            tstring ErrorDesc;
            GetControllerInterface->GetLastError(ErrorDesc);
            MessageBox(Translate(ErrorDesc.c_str()), Translate(_T("Error:Publish layout")), MB_OK|MB_ICONERROR);
            return;
        }

        szSeverLayoutPath = publishLayout.GetLocation().c_str();
    }




    DWORD nFileSize = 0;
    DWORD nReadSize = 0;
    //BYTE* pThumbnail = NULL;
    Layout::thumbnail_t thum;
    if (::PathFileExists(szLayoutThumbnail))
    {
        HANDLE hFile = NULL;
        hFile = ::CreateFile(szLayoutThumbnail, FILE_GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            MessageBox(Translate(_T("Failed to generate layout thumbnail!")), Translate(_T("Error:Publish layout")), MB_OK|MB_ICONERROR);
            return;
        }

        nFileSize = GetFileSize(hFile, NULL);
        if (nFileSize == 0xFFFFFFFF || nFileSize == 0)
        {
            MessageBox(Translate(_T("Failed to read layout thumbnail!")), Translate(_T("Error:Publish layout")), MB_OK|MB_ICONERROR);
            CloseHandle(hFile);
            return;
        }

        thum.resize(nFileSize);
        //pThumbnail = new BYTE[nFileSize];
        if (!::ReadFile(hFile, &thum[0], nFileSize, &nReadSize, NULL))
        {
            MessageBox(Translate(_T("Failed to read layout thumbnail!")), Translate(_T("Error:Publish layout")), MB_OK|MB_ICONERROR);
            CloseHandle(hFile);
            return;
        }
        CloseHandle(hFile);
    }
    else
    {
        nFileSize = 4;
        nReadSize = nFileSize;
        //pThumbnail = new BYTE[nFileSize];
        //memset(pThumbnail, 0XFFFFFFFF, sizeof(BYTE) * nFileSize);
        thum.resize(nFileSize);
        memset(&thum[0], 0xff, nFileSize);
    }

/*
    Layout::thumbnail_t thum;
    for (DWORD i = 0; i < nReadSize; i++)
    {
        thum.push_back(pThumbnail[i]);
    }
*/

StartPoint:
	
    S3Time createTime = S3Time::NULLTime;
    
    if (layoutID != S3NET_NEW_LAYOUT_ID)
    {
        publishLayout.SetLayoutName((LPCTSTR)m_LayoutElement->GetLayoutName());
        publishLayout.SetDuration(m_LayoutElement->GetLayoutDuration());
        publishLayout.SetGroupID(m_LayoutElement->GetGroupID());
        publishLayout.SetOwner((LPCTSTR)theApp.m_Username);
        //publishLayout.SetCreateTime(createTime);   /* Do not set time when edit layout */
        publishLayout.SetClientID(m_LayoutElement->GetClientID());
		
		publishLayout.SetDescription((LPCTSTR)m_LayoutElement->GetDescription());
		publishLayout.SetWidth(m_LayoutElement->GetLayoutSize().cx);
		publishLayout.SetHeight(m_LayoutElement->GetLayoutSize().cy);
		publishLayout.SetLayoutSize(m_LayoutElement->GetMediaContentSize());
		publishLayout.SetMediaCount(m_LayoutElement->GetMediaCount());
		

        Audit tmpAudit = publishLayout.GetAudit();
#ifdef  SERVER_EDITION
        Audit tmp(GetControllerInterface->GetUserId(), S3Time::Now(), tmpAudit.GetApproveUserID(),
            tmpAudit.GetApproveTime(), tmpAudit.CovertStatus(Audit::CREATEBUTNOTAPPROVED));
#else
        Audit tmp(GetControllerInterface->GetUserId(), S3Time::Now(), tmpAudit.GetApproveUserID(),
            tmpAudit.GetApproveTime(), tmpAudit.CovertStatus(Audit::APPROVED));
#endif
        publishLayout.SetAudit(tmp);
        publishLayout.SetMediaIDList(MediaIDs);

        if (!GetControllerInterface->CheckPrivilege(UserPrivilege_PublishLayout))
        {
            MessageBox(Translate(_T("You don't have privilege to edit layout")), Translate(_T("Warning:Edit layout")), MB_OK|MB_ICONEXCLAMATION);
            return;
        }

        hr = GetControllerInterface->EditLayout(publishLayout);
        if (!SUCCEEDED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                GetControllerApp->OnControllerServerOffline();
				return;
            }
			
			std::tstring desc;
			int errorID = GetControllerInterface->GetLastError(desc);
			//CString strdesc(desc.c_str());
			if(S3NET_LAYOUT_GROUP_ID_NOT_FOUND == errorID)
			{
				MessageBox(Translate(_T("The selected layout category is deleted, please select another layout category!")),Translate(_T("Warning:Edit layout")), MB_OK|MB_ICONEXCLAMATION);

				CPublishLayoutDialog dlg;
				if( dlg.DoModal() == IDOK)
				{
					m_LayoutElement->SetGroupID(dlg.GetCurCategoryID());
					goto StartPoint;
				}
				else 
				{
					return;
				}
			}
            else
            {
                std::tstring desc;
                int err = GetControllerInterface->GetLastError(desc);
                MessageBox(Translate(_T("Failed to publish layout!")) + Translate(desc.c_str()), Translate(_T("Error:Publish layout")),MB_OK|MB_ICONERROR);
				return;
            }
            
        }
    }
    else
    {
        publishLayout.SetLayoutID(S3NET_NEW_LAYOUT_ID);
        publishLayout.SetLayoutName((LPCTSTR)m_LayoutElement->GetLayoutName());
        publishLayout.SetDuration(m_LayoutElement->GetLayoutDuration());
        publishLayout.SetLocation((LPCTSTR)szSeverLayoutPath);
        publishLayout.SetGroupID(m_LayoutElement->GetGroupID());
        publishLayout.SetOwner((LPCTSTR)theApp.m_Username);
        publishLayout.SetCreateTime(createTime);
        publishLayout.SetClientID(m_LayoutElement->GetClientID());
        publishLayout.SetCreateTime(createTime);

		publishLayout.SetDescription((LPCTSTR)m_LayoutElement->GetDescription());
		publishLayout.SetWidth(m_LayoutElement->GetLayoutSize().cx);
		publishLayout.SetHeight(m_LayoutElement->GetLayoutSize().cy);
		publishLayout.SetLayoutSize(m_LayoutElement->GetMediaContentSize());
		publishLayout.SetMediaCount(m_LayoutElement->GetMediaCount());
	
        Audit tmpAudit = publishLayout.GetAudit();
#ifdef  SERVER_EDITION
        Audit tmp(GetControllerInterface->GetUserId(), S3Time::Now(), tmpAudit.GetApproveUserID(),
            tmpAudit.GetApproveTime(), tmpAudit.CovertStatus(Audit::CREATEBUTNOTAPPROVED));
#else
        Audit tmp(GetControllerInterface->GetUserId(), S3Time::Now(), tmpAudit.GetApproveUserID(),
            tmpAudit.GetApproveTime(), tmpAudit.CovertStatus(Audit::APPROVED));
#endif
        publishLayout.SetAudit(tmp);
        publishLayout.SetMediaIDList(MediaIDs);

        if (!GetControllerInterface->CheckPrivilege(UserPrivilege_PublishLayout))
        {
            MessageBox(Translate(_T("You don't have privilege to publish layout.")),Translate(_T("Warning:Publish layout")),MB_OK|MB_ICONEXCLAMATION);
            return;
        }
        hr = GetControllerInterface->PublishLayout(publishLayout);
        if (!SUCCEEDED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                GetControllerApp->OnControllerServerOffline();
				return;
            }

			std::tstring desc;
			int errorID = GetControllerInterface->GetLastError(desc);
			//CString strdesc(desc.c_str());
			if(S3NET_LAYOUT_GROUP_ID_NOT_FOUND == errorID)
			{
				MessageBox(Translate(_T("The selected layout category is deleted, please select another layout category!")),Translate(_T("Warning:Publish layout")), MB_OK|MB_ICONEXCLAMATION);

				CPublishLayoutDialog dlg;
				if( dlg.DoModal() == IDOK)
				{
					m_LayoutElement->SetGroupID(dlg.GetCurCategoryID());
					goto StartPoint;
				}
				else 
				{
					return;
				}
			}
			else
            {
                std::tstring desc;
                int err = GetControllerInterface->GetLastError(desc);
                MessageBox(Translate(_T("Failed to publish layout!")) + Translate(desc.c_str()), Translate(_T("Error:Publish layout")), MB_OK|MB_ICONERROR);
				return;
            }
            
        }
    }

    std::vector<BYTE> LayoutContentByte;
    std::string LayoutContentString = CW2A(ContentXML, CP_UTF8);

    auto it = LayoutContentString.begin();
    while(it != LayoutContentString.end())
    {
        LayoutContentByte.push_back(*it);
        it++;
    }

    hr = GetControllerInterface->SetLayoutContent(publishLayout.GetLayoutID(), LayoutContentByte);
    if (!SUCCEEDED(hr))
    {

        MessageBox(Translate(_T("Failed to publish layout content!")), Translate(_T("Error:Publish layout")),MB_OK| MB_ICONERROR);
        return;
    }
    publishLayout.IncreaseVersion();

    hr = GetControllerInterface->SetLayoutThumbnail(publishLayout.GetLayoutID(), thum);
    if (!SUCCEEDED(hr))
    {
        MessageBox(Translate(_T("Failed to publish layout content!")), Translate(_T("Error:Publish layout")),MB_OK|MB_ICONERROR);
        return;
    }
    publishLayout.IncreaseVersion();

    if (hr != S_OK)
    {
        tstring ErrorDesc;
        GetControllerInterface->GetLastError(ErrorDesc);
        MessageBox(Translate(ErrorDesc.c_str()), Translate(_T("Error:Publish layout")),MB_OK|MB_ICONERROR);
        return;
    }


    MessageBox(Translate(_T("Layout Published Finished!")), Translate(_T("Publish")), MB_OK);

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)(Translate(_T("Layout Published Successfully:")) + m_LayoutElement->GetLayoutName()).GetString());

}

void CDesignerPage::OnRibbonBtnExport()
{
#if defined(STANDALONE_EDITION) || defined(PROFESSIONAL_EDITION) || defined(SERVER_EDITION)
	TCHAR szDefaultDir[MAX_PATH];

	CString strDef(_T("C://WINDOWS//"));
	memcpy(szDefaultDir,strDef.GetBuffer(strDef.GetLength()),strDef.GetLength());
	strDef.ReleaseBuffer();

	TCHAR szPath[MAX_PATH];
	BROWSEINFO br;
	ITEMIDLIST* pItem;

	br.hwndOwner = this->GetSafeHwnd();
	br.pidlRoot = 0;
	br.pszDisplayName = 0;
	br.lpszTitle = _T("Please Select Directory:");
	br.ulFlags = BIF_STATUSTEXT | BIF_USENEWUI;
	br.lpfn = NULL ;        
	br.iImage = 0;
	br.lParam = long(&szDefaultDir);    
	//
	pItem = SHBrowseForFolder(&br);
	if(pItem != NULL)
	{
		if(SHGetPathFromIDList(pItem,szPath) == TRUE)
		{
			CString filePath = szPath;
			if(filePath.Right(1) != _T("\\"))
			{
				filePath += "\\";
			}

			CString dirPath = filePath;

			filePath +=  _T("index.xml");

			if(_taccess(filePath, 0) == 0)
			{
				CString messStr;
				messStr.Format(Translate(_T("The file %s have exist! Are you want to overwrite?")),filePath);
				if(MessageBox(messStr,Translate(_T("Tips:Export file")),MB_YESNO|MB_ICONQUESTION) != IDYES)
				{
					return;
				}
			}

			
			std::vector<int> mediaIDList;
			mediaIDList.empty();
			list<std::shared_ptr<LayerElement>> layers = m_LayoutElement->GetAllLayers();
			list<std::shared_ptr<LayerElement>> :: iterator it;
			for(it = layers.begin(); it != layers.end(); it++)
			{
				vector<std::shared_ptr<MediaElement>> medias = (*it)->GetMediaList();
				vector<std::shared_ptr<MediaElement>> :: iterator itm;
				for(itm = medias.begin(); itm != medias.end(); itm++)
				{
					int nMediaID = (*itm)->GetMediaID();
					if (nMediaID >= 0)
					{
						vector<int>::iterator result = find( mediaIDList.begin(), mediaIDList.end(), nMediaID ); //find nMediaID
						if  ( result == mediaIDList.end() ) //not find
						{
							mediaIDList.push_back(nMediaID);
						}
					}
				}
			}
			std::vector<Media> mediaList;
			HRESULT hr2 = GetControllerInterface->GetMedias(mediaIDList, mediaList);
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
			std::vector<Media>::iterator it2;
			for(it2 = mediaList.begin(); it2!= mediaList.end(); ++it2)
			{
				LayoutSize += it2->GetFileSize();
			}

			// collect exported layout list
			INT FreeRemainMB = SysCall::GetFreeSpaceMB(dirPath.GetString());

			// copy layout to disc
			INT LayoutSizeMB = static_cast<INT>(LayoutSize>>20);
			if (LayoutSizeMB > (FreeRemainMB - 1))
			{
				CString cWarning, str1, str2;
				str1.Format(_T("%d MB"),LayoutSizeMB);
				str2.Format(_T("%d MB"),FreeRemainMB);
				cWarning = m_LayoutElement->GetLayoutName() + _T("(") + str1 +
					Translate(_T(")may be too big for this disk(Free size: ")) +
					str2 + Translate(_T("), Can't export to this disk!"));
				MessageBox(cWarning, Translate(_T("Tips:Export file")),MB_OK|MB_ICONEXCLAMATION);
				return;
			}
			
			#ifdef SERVER_EDITION
			{
				HRESULT hr3 = S_OK;

				MEDIA_LIST classMediaList;
				classMediaList.m_MediaIDs = mediaIDList;

				if (mediaIDList.size() > 0)
				{
					//Need download the media files

					// copy layout to disc
					MEDIA_DETAIL_INFO MediaInfo;
					hr3 = GetMediaLibraryInterface->GetMedia(classMediaList,MediaInfo);
					if (!SUCCEEDED(hr3))
					{
						TRACE0("Error happen in GetMedia, PreviewDownLoadDlg");
						//AfxMessageBox(Translate(_T("Failed to get media info!")), MB_OK|MB_ICONINFORMATION);
						std::vector<Media> vecmediaList;
						CString msg = Translate(_T("Preview failed:")) + _T("\n");
						GetControllerInterface->GetMedias(classMediaList.m_MediaIDs, vecmediaList);
						auto it = vecmediaList.begin(), ite = vecmediaList.end();
						for (; it != ite; ++it)
						{
							if (it->GetAudit().GetStatus() == Audit::DELETED)
							{
								CString s;
								s.Format(Translate(_T("Media '%s' is %s.")), it->GetName().c_str(), Translate(_T("deleted")));
								msg += s + _T("\n");
							}
							else if (it->GetAudit().GetStatus() == Audit::CREATEBUTNOTAPPROVED)
							{
								CString s;
								s.Format(Translate(_T("Media '%s' is %s.")), it->GetName().c_str(), Translate(_T("not approved")));
								msg += s + _T("\n");
							}
							else if (it->GetAudit().GetStatus() == Audit::REJECTED)
							{
								CString s;
								s.Format(Translate(_T("Media '%s' is %s.")), it->GetName().c_str(), Translate(_T("rejected")));
								msg += s + _T("\n");
							}
						}
						ShowXMessageBox(NULL,msg, Translate(_T("Tips:Preview layout")), MB_OK|MB_ICONINFORMATION);
						return;//return E_UNEXPECTED;
					}

					if (MediaInfo.m_FinishedCnt != MediaInfo.m_TotalCnt)
					{
						PreviewDownLoadDlg dlg;

						// collect exported layout list
						DWORD64 FreeRemain = GetMediaLibraryInterface->GetMediaLibraryRootFreeSpace();

						DWORD64 LayoutSize = MediaInfo.m_TotalSize;
						DWORD64 SizeStillNeedDownload = MediaInfo.m_TotalSize - MediaInfo.m_FinishedSize;
						if (LayoutSize >= FreeRemain)
						{
							CString cWarning, str1, str2, strSizeStillNeed;
							str1.Format(_T("%d MB"),LayoutSize>>20);
							str2.Format(_T("%d MB"),FreeRemain>>20);
							strSizeStillNeed.Format(_T("%d MB"),SizeStillNeedDownload>>20);
							cWarning =  Translate(_T("Insufficient disk space, Unable to download the layout!\n\nLayout total size: "))+ str1 +
								/*Translate(_T(";\nMedia that need download: ")) + strSizeStillNeed + */
								Translate(_T(";\nController disk free size: ")) + str2 +
								Translate(_T("; \n\nPlease make sure there are enough disk free space for downloading the media!"));
							ShowXMessageBox(NULL,cWarning, Translate(_T("Warning:Preview layout")), MB_OK|MB_ICONEXCLAMATION);
							return;//return E_UNEXPECTED;
						}

						if (!dlg.AddMediaListJob(classMediaList))
						{
							ShowXMessageBox(NULL,Translate(_T("Media Library failed to add download task, some media used in the layout may be not downloaded!")),Translate(_T("Error:Preview layout")), MB_OK|MB_ICONERROR);
						}

						if (dlg.DoModal() != IDOK || dlg.m_Progress< PROGRESS_SLICE)
						{
							return;//return E_UNEXPECTED;
						}
					}
				}
			}
			#endif

					
			{
				CWaitDlg waitdlg;
				CString ContentXML;
				//WriteLayoutFile(filePath, ContentXML);
				WriteLayoutFile(dirPath, ContentXML,true);
			}
			
			MessageBox(Translate(_T("Layout Exported Successfully:")),Translate(_T("Tips:Export file")),MB_OK);

			GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
				(LPARAM)(Translate(_T("Layout Exported Successfully:")) + m_LayoutElement->GetLayoutName()).GetString());

		}
	}
#else
	/* By RaymondLiu
	CString filePath;
	CFileDialog fileDialog(FALSE, NULL, NULL, OFN_HIDEREADONLY, L"XML (*.xml)|*.xml|", this);
	if (fileDialog.DoModal() == IDOK)
	{
		filePath = fileDialog.GetPathName();

		CString szExt = ::PathFindExtension(filePath);
		if (szExt.CompareNoCase(_T(".xml")) != 0)
		{
			filePath.Append(_T(".xml"));
		}

		if(_taccess(filePath, 0) == 0)
		{
			CString messStr;
			messStr.Format(Translate(_T("The file %s have exist! Are you want to overwrite?")),filePath);
			if(MessageBox(messStr,Translate(_T("Tips:Export file")),MB_OKCANCEL|MB_ICONQUESTION) != IDOK)
			{
				return;
			}
		}

		{
			CWaitDlg waitdlg;
			CString ContentXML;
			WriteLayoutFile(filePath, ContentXML);
		}
		
		MessageBox(Translate(_T("Layout Exported Successfully:")),Translate(_T("Tips:Export file")),MB_OK);

		GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
			(LPARAM)(Translate(_T("Layout Exported Successfully:")) + m_LayoutElement->GetLayoutName()).GetString());
	}
	*/
#endif
}

void CDesignerPage::OnRibbonBtnImport()
{

    CString filePath;
    CFileDialog fileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, L"XML (*.xml)|*.xml|", this);
    if (fileDialog.DoModal() == IDOK)
    {
        if (m_layoutSize.cx > 0 || m_layoutSize.cy > 0)
        {
            m_wndLayoutDesign.m_wndDesign.CloseLayout();

            CloseLayout();

            m_wndLayoutDesign.m_wndDesign.RedrawWindow();
            m_wndLayoutProperties.m_PropertyCtrl.Clear();
        }


        filePath = fileDialog.GetPathName();

        m_arrLosedFiles.RemoveAll();
        m_arrSuspicionFiles.RemoveAll();

        LoadLayoutFile(filePath);

        if (m_arrLosedFiles.GetCount() > 0)
        {
            CString szWarning;
            szWarning = _T("<html>\r\n");
            szWarning.AppendFormat(_T("<body>\r\n"));
            szWarning.AppendFormat(_T("<p align=\"center\"><font fac=\"Calibri\" color=\"#FF0000\" size=4>"));
			szWarning += Translate(_T("Failed to find following files"));
			szWarning.AppendFormat(_T("</font></p>\r\n"));

            for (int i = 0; i < m_arrLosedFiles.GetCount(); i++)
            {
                szWarning.AppendFormat(_T("<p><font fac=\"Calibri\" color=\"#330033\" size=3>    %s</font></p>\r\n"), m_arrLosedFiles.GetAt(i));
            }

            szWarning.AppendFormat(_T("</body>\r\n"));
            szWarning.AppendFormat(_T("</html>"));

            CLayoutWarningCtrl warningCtrl(this);
            warningCtrl.SetWarning(szWarning);
            warningCtrl.DoModal();
            //return;
        }

        GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
            (LPARAM)(Translate(_T("Layout Imported Successfully:")) + m_LayoutElement->GetLayoutName()).GetString());
    }
}


void CDesignerPage::OnRibbonBtnUndo()
{

}

void CDesignerPage::OnRibbonBtnRedo()
{

}

void CDesignerPage::OnRibbonBtnDeleteItem()
{
    m_wndLayoutDesign.m_wndDesign.DeleteObj();
}

void CDesignerPage::OnRibbonBtnTabChange()
{
   m_wndLayoutDesign.m_wndDesign.TabChange();
}

void CDesignerPage::OnRibbonBtnMoveLeft()
{
    m_wndLayoutDesign.m_wndDesign.MoveLeft();
}

void CDesignerPage::OnRibbonBtnMoveRight()
{
    m_wndLayoutDesign.m_wndDesign.MoveRight();
}

void CDesignerPage::OnRibbonBtnMoveUp()
{
    m_wndLayoutDesign.m_wndDesign.MoveUp();
}

void CDesignerPage::OnRibbonBtnMoveDown()
{
    m_wndLayoutDesign.m_wndDesign.MoveDown();
}
void CDesignerPage::OnRibbonBtnCopy()
{
    CS3DrawObj* pObj = NULL;
    PROPERTY_TYPE proType;

    CleanClipDrawObj();
    m_wndLayoutDesign.m_wndDesign.PrepareClone();

    TRY
    {
        CS3DrawObjList* pObjList = m_wndLayoutDesign.m_wndDesign.GetSelectDrawObj();
        POSITION pos = pObjList->GetHeadPosition();
        while (pos != NULL)
        {
            pObj = pObjList->GetNext(pos);
            pObj->GetPropertyType(proType);
            if (proType != AUDIO)
            {
                m_ClipObjList.AddTail(pObj);
            }
        }
    }
    CATCH (CMemoryException, e)
    {
        CleanClipDrawObj();
    }
    END_CATCH
}

void CDesignerPage::OnRibbonBtnCut()
{

}

void CDesignerPage::OnRibbonBtnPaste()
{
    PROPERTY_TYPE propertyType;
    FRect logicPos;
    CRect rect;
    CS3DrawObj* pObj = NULL;
    CS3DrawObjList m_cloneList;
    //LAYERINFO *pLayerInfo;
    COLORREF fillColor = RGB(255, 255, 255);
    CString szName;
    BOOL bDesinger2 = FALSE;
    HRESULT hr = S_OK;
    DWORD nCount = 0;

    POSITION pos = m_ClipObjList.GetHeadPosition();
    while (pos != NULL)
    {
        pObj = m_ClipObjList.GetNext(pos);

        pObj->GetPropertyType(propertyType);
        pObj->GetLogicPos(logicPos);
        pObj->GetPosition(rect);
        pObj->GetName(szName);
        m_nDepth += 1;
        nCount = 0;

        if (pObj->IsKindOf(RUNTIME_CLASS(CS3DrawRectObj)))
        {
            CS3DrawRectObj* pCopyObj = new CS3DrawRectObj(logicPos, rect, m_nDepth, propertyType);
            pCopyObj->SetKeepAspect(pObj->GetKeepAspect());
            BOOL isSync = FALSE;
            pObj->GetLoopSync(isSync);
            pCopyObj->SetLoopSync(isSync);

            if (propertyType == LAYER)
            {
                fillColor = RGB(167, 224, 180);
            }
            else if (propertyType == MESSAGE)
            {
                fillColor = RGB(255, 128, 0);
            }
			/* LAYOUT-DESIGNER-NEW-VER-RAYMONDLIU */
            else if (propertyType == USER_DEFINED_PLUGIN)
            {
				if(m_ObjFillColor.Lookup(szName, fillColor) != TRUE)
				{
					fillColor = RGB(255,0,0);
				}
            }

            pCopyObj->SetFillColor(fillColor);
            pCopyObj->SetName(szName);

            m_ObjTypeCount.Lookup(szName, nCount);
            nCount += 1;
            m_ObjTypeCount.SetAt(szName, nCount);
            pCopyObj->SetTypeCount(nCount);

            pCopyObj->SetHitTestSize(m_nHandleSize, m_nLineSize);

            m_DrawObjList.AddTail(pCopyObj);
            m_cloneList.AddTail(pCopyObj);

			BOOL bFound = FALSE; 
			std::shared_ptr<LayerElement> layerInfo = m_LayoutElement->GetLayerInfo(pObj->GetLayerId(), bFound);
            if (bFound && layerInfo.get() != NULL)
            {
                //LAYERINFO *pLayerCopy = new LAYERINFO;
				std::shared_ptr<LayerElement> NewLayerElement;
				NewLayerElement = m_LayoutElement->CreateLayer();
                m_LayoutElement->AddTailLayer(NewLayerElement);

				pCopyObj->SetLayerId(NewLayerElement->GetLayerId());

				vector<std::shared_ptr<MediaElement>> medias = layerInfo->GetMediaList();
				vector<std::shared_ptr<MediaElement>> :: iterator itm;
                //posLayer = pLayerInfo->GetHeadPosition();
				for(itm = medias.begin(); itm != medias.end(); itm++)
                {
                    //pMedia = pLayerInfo->GetNext(posLayer);

					std::shared_ptr<MediaElement> CopyMedia;
					NewLayerElement->CreateMedia(CopyMedia);

                    CopyMedia->SetMediaID((*itm)->GetMediaID());
                    CopyMedia->SetMediaName((*itm)->GetMediaName());
                    CopyMedia->SetMediaType((*itm)->GetMediaType());
                    CopyMedia->SetMediaFile((*itm)->GetMediaFile());
                    CopyMedia->SetMessageFile((*itm)->GetMessageFile());
                    CopyMedia->SetPosition((*itm)->GetPosition());
                    CopyMedia->SetStartTime((*itm)->GetStartTime());
                    CopyMedia->SetDuration((*itm)->GetDuration());
                    CopyMedia->SetLoopCount((*itm)->GetLoopCount());
                    CopyMedia->SetVolumeCount((*itm)->GetVolumeCount());
                    CopyMedia->SetKeepAspect((*itm)->GetKeepAspect());
                    CopyMedia->SetBGColor((*itm)->GetBGColor());
                    CopyMedia->SetTextSetting(NULL);
                    CopyMedia->SetDesigner2(NULL);
                    CopyMedia->SetDesigner(NULL);

                    if ((*itm)->GetTextSetting())
                    {
						CopyMedia->CreateTextSetting();
						std::shared_ptr<S3SIGNAGE_TEXT_SETTING>  pOrigTextSetting = (*itm)->GetTextSetting();
                        std::shared_ptr<S3SIGNAGE_TEXT_SETTING>  pTextSetting = CopyMedia->GetTextSetting();
                        pTextSetting->szMessage     = pOrigTextSetting->szMessage;
                        pTextSetting->Alignment     = pOrigTextSetting->Alignment;
                        pTextSetting->Transparency  = pOrigTextSetting->Transparency;
                        pTextSetting->TextDirection = pOrigTextSetting->TextDirection;
                        pTextSetting->FontDirection = pOrigTextSetting->FontDirection;
                        pTextSetting->TextLayout    = pOrigTextSetting->TextLayout;
                        pTextSetting->fScale        = pOrigTextSetting->fScale;
                        pTextSetting->FontSpeed     = pOrigTextSetting->FontSpeed;
                        pTextSetting->FontColor     = pOrigTextSetting->FontColor;
                        pTextSetting->szMultiLineMessage = pOrigTextSetting->szMultiLineMessage;

						S3SIGNAGE_TRANSIION TransIn = (*itm)->GetTransIn();
						S3SIGNAGE_TRANSIION DestTransIn = CopyMedia->GetTransIn();
                        DestTransIn.Name = TransIn.Name;
                        DestTransIn.Duration = TransIn.Duration;
                        DestTransIn.Direction = TransIn.Direction;
                        

						S3SIGNAGE_TRANSIION TransOut = (*itm)->GetTransOut();
						S3SIGNAGE_TRANSIION DestTransOut = CopyMedia->GetTransOut();
						DestTransOut.Name = TransOut.Name;
						DestTransOut.Duration = TransOut.Duration;
						DestTransOut.Direction = TransOut.Direction;
                        
                        LOGFONT logFont;
                        pOrigTextSetting->pFontInfo->GetLogFont(&logFont);
                        pTextSetting->pFontInfo->DeleteObject();
                        pTextSetting->pFontInfo->CreateFontIndirect(&logFont);

                    }
                    else
                    {
                        bDesinger2 = FALSE;
                        std::shared_ptr<IS3RODesigner2> pPluginDesigner2    = NULL;
                        std::shared_ptr<IS3RODesigner>  pPluginDesigner     = NULL;
                        hr = QueryDesignerWithName((*itm)->GetMediaName(), pPluginDesigner2, pPluginDesigner);
                        if (hr != S_OK)
                        {
                            pPluginDesigner2    = NULL;
                            pPluginDesigner     = NULL;
                        }
                        else
                        {
                            if (pPluginDesigner2)
                            {
                                RO_PROPERTY_SETTING PropertySetting;
                                PRO_PROPERTY        properties = new RO_PROPERTY[128];

                                DWORD propertyCount;

                                (*itm)->GetDesigner2()->GetPropertySettings(&propertyCount, properties);
                                PropertySetting.propertyCount = propertyCount;
                                PropertySetting.pProperties = properties;

                                pPluginDesigner2->CreateDesigner(CopyMedia->GetMediaFile(), &PropertySetting);
                                delete [] properties;
                            }
                            if (pPluginDesigner)
                            {
							    RO_PROPERTY_SETTING PropertySetting;
                                PRO_PROPERTY        properties = new RO_PROPERTY[128];

                                DWORD propertyCount;

                                (*itm)->GetDesigner()->GetPropertySettings(&propertyCount, properties);
                                PropertySetting.propertyCount = propertyCount;
                                PropertySetting.pProperties = properties;

                                pPluginDesigner->CreateDesigner(CopyMedia->GetMediaFile(), &PropertySetting);
                                delete [] properties;
                            }

                        }

						S3SIGNAGE_TRANSIION TransIn = (*itm)->GetTransIn();
						S3SIGNAGE_TRANSIION destTransIn = CopyMedia->GetTransIn();
						destTransIn.Name = TransIn.Name;
						destTransIn.Duration = TransIn.Duration;
						destTransIn.Direction = TransIn.Direction;
                        

						S3SIGNAGE_TRANSIION TransOut = (*itm)->GetTransOut();
						S3SIGNAGE_TRANSIION destTranOut = CopyMedia->GetTransOut();
						destTranOut.Name = TransOut.Name;
                        destTranOut.Duration = TransOut.Duration;
                        destTranOut.Direction = TransOut.Direction;
                        

                        CopyMedia->SetDesigner2(pPluginDesigner2);
                        CopyMedia->SetDesigner(pPluginDesigner);
                    }

					NewLayerElement->AddTailMediaElement(CopyMedia);
                }

                //m_LayoutSetting->Layers.SetAt(pCopyObj, pLayerCopy);
            }
        }
    }

    m_wndLayoutDesign.m_wndDesign.CloneSelection(&m_cloneList);
    m_cloneList.RemoveAll();

    m_wndLayoutDesign.m_wndDesign.RedrawWindow();
}

void CDesignerPage::OnRibbonBtnSelectAll()
{
    m_wndLayoutDesign.m_wndDesign.SelectAllDrawObj();
}

void CDesignerPage::OnRibbonBtnPreviewWinMode()
{

}

void CDesignerPage::OnRibbonBtnPreview()
{
    PreviewLayout(800, 600);
}

void CDesignerPage::OnRibbonBtnPreview10X7()
{
    PreviewLayout(1024, 768);
}

void CDesignerPage::OnRibbonBtnPreview12X10()
{
    PreviewLayout(1280, 1024);
}

void CDesignerPage::OnRibbonBtnPreview19X12()
{
    PreviewLayout(1920, 1080);
}

void CDesignerPage::OnRibbonBtnRefresh()
{

}

void CDesignerPage::OnRibbonBtnMoveToFront()
{
    m_wndLayoutDesign.m_wndDesign.MoveToFront();
}

void CDesignerPage::OnRibbonBtnMoveForward()
{
    m_wndLayoutDesign.m_wndDesign.MoveForward();
}

void CDesignerPage::OnRibbonBtnMoveToBack()
{
    m_wndLayoutDesign.m_wndDesign.MoveToBack();
}

void CDesignerPage::OnRibbonBtnBindLay()
{
	m_wndLayoutDesign.m_wndDesign.EnableBindLay( !m_wndLayoutDesign.m_wndDesign.IsBindLayEnable());
}
void CDesignerPage::OnRibbonBtnMoveBackward()
{
    m_wndLayoutDesign.m_wndDesign.MoveBack();
}

void CDesignerPage:: OnRibbonBtnUpdateSameInterval(CCmdUI *pui)
{
    CS3DrawObjList *SelList = m_wndLayoutDesign.m_wndDesign.GetSelectDrawObj();
    if(SelList->GetCount() < 3)//2 interval is enabled.
    {
        pui->Enable(FALSE);
    }else
        pui->Enable(TRUE);
}
 
void CDesignerPage::OnRibbonBtnUpdateFillLayout(CCmdUI *pui)
{
    CS3DrawObjList *SelList = m_wndLayoutDesign.m_wndDesign.GetSelectDrawObj();
    if(SelList->GetCount()==1)//An object is allow.
    {
        pui->Enable(TRUE);
    }else
        pui->Enable(FALSE);
}


void CDesignerPage::OnRibbonBtnSameIntervalX()
{
    m_wndLayoutDesign.m_wndDesign.SameInterval();
}

void CDesignerPage::OnRibbonBtnSameIntervalY()
{
    m_wndLayoutDesign.m_wndDesign.SameInterval(TRUE);
}

void CDesignerPage::OnRibbonBtnSameFillLayout()
{
    m_wndLayoutDesign.m_wndDesign.FillLayout();
}

void CDesignerPage::OnRibbonBtnZoom()
{
    BOOL bUpdateText = FALSE;
    FLOAT zoomFactor = 0.0f;
    CString szText = m_pComboBoxZoom->GetEditText();

	if(szText == Translate(_T("Auto Fit")))
	{
		m_wndLayoutDesign.m_wndDesign.SetLayoutSize(m_layoutSize);
		m_wndLayoutDesign.m_wndDesign.SetupScrollBars(TRUE);
		m_wndLayoutDesign.m_wndDesign.CalcTransform();
		m_wndLayoutDesign.m_wndDesign.Invalidate();
		return;
	}

    int nFind = szText.Find(_T("%"));
    if (nFind != -1)
    {
        szText = szText.Mid(0, nFind);
    }
    else
    {
        bUpdateText = TRUE;
    }

    zoomFactor = (FLOAT)_wtof((LPCWSTR)szText);
    zoomFactor = zoomFactor / 100;

    if (bUpdateText)
    {
        szText.Append(_T("%"));
        m_pComboBoxZoom->SetEditText(szText);
    }

    if (zoomFactor < 0.02f || zoomFactor > 16.0f)
    {
		CString messStr = Translate(_T("Invalid zoom factor value. Please input the value between ")) + _T("2%") + Translate(_T("to")) +_T("1600%!");
        MessageBox(messStr,Translate(_T("Warning:Zoom")) ,MB_OK|MB_ICONEXCLAMATION);
        FLOAT zoomFactor = 100.0f * m_wndLayoutDesign.m_wndDesign.GetZoomFactor();
        szText.Format(_T("%.1f%%"), zoomFactor);
        m_pComboBoxZoom->SetEditText(szText);
        return;
    }

    if (m_layoutSize.cx != 0 || m_layoutSize.cy != 0)
    {
		m_wndLayoutDesign.m_wndDesign.ZoomImage(zoomFactor);
		m_wndLayoutDesign.m_wndDesign.CalcTransform(zoomFactor);
    }
}

LRESULT  CDesignerPage::OnDesignerKeyDown(WPARAM wp, LPARAM lp)

{
   UINT nChar = (UINT)wp;

   UINT nRepCnt = LOWORD(lp);
   UINT nFlags = HIWORD(lp);

   if (nChar == VK_DELETE)
   {
       OnRibbonBtnDeleteItem();
       return 0;
   }

   if(nChar == VK_TAB)
   {
       OnRibbonBtnTabChange();
       return 0;
   }

  if (nChar == VK_LEFT)
  {
      OnRibbonBtnMoveLeft();
      return 0;
  }

  if (nChar == VK_RIGHT)
  {
      OnRibbonBtnMoveRight();
      return 0;
  }

  if (nChar == VK_UP)
  {
      OnRibbonBtnMoveUp();
      return 0;
  }

  if (nChar == VK_DOWN)
  {
      OnRibbonBtnMoveDown();
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
	   else if (nChar == _T('a') || nChar == _T('A'))
	   {
		   OnRibbonBtnSelectAll();
		   return 0;
	   }
   }

    return 0;
}

void CDesignerPage::CreateRectDrawObj(const CRect rect, const PROPERTY_TYPE propertyType, const CString szMediaType, std::shared_ptr<IS3RODesigner2> pDesigner2,
    std::shared_ptr<IS3RODesigner> pDesigner, std::vector<std::pair<CString, CLayoutDesignerCtrl::DragMediaInfo> >* pMedia)
{
    COLORREF fillColor = RGB(255, 255, 255);
    CS3DrawRectObj* pObj = NULL;
    CRect position = rect;
    FRect rectLogic = rect;
    CString szTypeName = _T("");

	m_wndLayoutDesign.m_wndDesign.LayoutToClient(position) ;//layout position to client.
	//建立一个 "Draw"对象 
	pObj = new CS3DrawRectObj(rectLogic, position, m_nDepth, propertyType);
	m_nDepth += 1; //当前创建对象该对象的z-order深度标示加一
	

	//对象的bk color.
	switch(propertyType)
	{
	case BACKGROUND:
		fillColor = RGB(128,128,128);
		break;
	case LAYER:
		fillColor = RGB(167, 224, 180);
		szTypeName = szTypeName_Layer;
		break;
	case MESSAGE:
		fillColor = RGB(255, 128, 0);
		szTypeName = szTypeName_Message;
		break;
	case USER_DEFINED_PLUGIN://To find out plugin color that user defined.
		szTypeName = szMediaType;
		if( m_ObjFillColor.Lookup(szMediaType, fillColor) != TRUE)
		{
			fillColor = RGB(255,0,0);
		}
		break;
	case AUDIO:
		szTypeName = szTypeName_Audio;
        fillColor = RGB(255, 255, 0);
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	//draw 对象设置类型名
	pObj->SetName(szTypeName);
	 //设置颜色 
    pObj->SetFillColor(fillColor);
	//设置把手
	pObj->SetHitTestSize(m_nHandleSize, m_nLineSize);  
	

	//加入 drawobj管理
    m_DrawObjList.AddTail(pObj);
	//绘画实体交付 designerctrl wnd.
    m_wndLayoutDesign.m_wndDesign.InsertDrawObj(pObj);

	SetLayerInfo(pObj, szTypeName, rectLogic, propertyType, szMediaType, pDesigner2, pDesigner, pMedia);
    //m_pLayoutSetting->Layers.SetAt(pObj, pLayerInfo);
    CS3DrawObjList selectList;
    selectList.AddTail(pObj);
    if (propertyType == BACKGROUND)
    {
        OnLayoutSelectObj(1, (LPARAM)&selectList);
    }
    else
    {
        OnLayoutSelectObj(0, (LPARAM)&selectList);
    }

    m_wndLayoutDesign.m_wndDesign.RedrawWindow();

}

BOOL CDesignerPage::RemoveDrawObj(CS3DrawObj* pObj)
{
    m_ObjItemCount.RemoveKey(pObj);
    m_layerDuration.RemoveKey(pObj);

    BOOL bRemove = FALSE;
    POSITION pos = m_DrawObjList.Find(pObj);
    if (pos != NULL)
    {
        m_DrawObjList.RemoveAt(pos);
        RemoveLayoutSettingWithObj(pObj);
        delete pObj;
        pObj = NULL;

        bRemove = TRUE;
    }

    return bRemove;
}

void CDesignerPage::RemoveAllDrawObj()
{
    POSITION pos = m_DrawObjList.GetHeadPosition();
    while (pos != NULL)
    {
        CS3DrawObj* pObj = m_DrawObjList.GetNext(pos);
        RemoveDrawObj(pObj);
    }

    m_DrawObjList.RemoveAll();
}

BOOL CDesignerPage::RemoveLayoutSettingWithObj(CS3DrawObj* pObj)
{
	BOOL bFound = FALSE;
	std::shared_ptr<LayerElement> layerInfo = m_LayoutElement->GetLayerInfo(pObj->GetLayerId(), bFound);
    if (bFound && layerInfo != NULL)
    {
		vector<std::shared_ptr<MediaElement>>  medias = layerInfo->GetMediaList();
		vector<std::shared_ptr<MediaElement>> ::iterator it;
		for (it = medias.begin(); it != medias.end(); it++)
		{
			std::shared_ptr<S3SIGNAGE_TEXT_SETTING>  pTextSetting = (*it)->GetTextSetting();
			if (pTextSetting)
			{
				if (pTextSetting->pFontInfo)
				{
					pTextSetting->pFontInfo->DeleteObject();
					delete pTextSetting->pFontInfo;
					pTextSetting->pFontInfo = NULL;
				}
			}
			if ((*it)->GetDesigner2())
			{
				(*it)->SetDesigner2(NULL);
			}
			if ((*it)->GetDesigner())
			{
				(*it)->SetDesigner(NULL);
			}

			//(*it).reset();
		}
		//layerInfo.reset();
		m_LayoutElement->RemoveLayer(pObj->GetLayerId());
	}

    return TRUE;
}

CS3DrawObj* CDesignerPage::GetSelectDrawObj()
{
    CS3DrawObj* pObj = NULL;

    //pObjList = m_wndLayoutDesign.m_wndDesign.GetSelectDrawObj();

    return pObj;
}



void CDesignerPage::CleanClipDrawObj()
{
    //CS3DrawObj* pObj = NULL;
    //POSITION pos = m_ClipObjList.GetHeadPosition();
    //while (pos != NULL)
    //{
    //    pObj = m_ClipObjList.GetNext(pos);
    //    delete pObj;
    //    pObj = NULL;
    //}

    m_ClipObjList.RemoveAll();
}

HRESULT CDesignerPage::QueryDesignerWithName(CString szName, std::shared_ptr<IS3RODesigner2>& pDesigner2, std::shared_ptr<IS3RODesigner>& pDesigner)
{
    HRESULT hr = S_OK;
    std::shared_ptr<IS3RODesigner2> pPluginDesigner2    = NULL;
    std::shared_ptr<IS3RODesigner>  pPluginDesigner     = NULL;
    //std::shared_ptr<IS3RenderableObject> pPluginObj = GetControllerApp->FindObj(szName);
    std::shared_ptr<IS3RenderableObject> pPluginObj = GetControllerApp->m_plugMgr.CreateObject(szName.GetString());
    if (!pPluginObj)
    {
        hr = E_FAIL;
    }
    else
    {
        //
        pPluginDesigner2 = std::dynamic_pointer_cast<IS3RODesigner2>(pPluginObj);
        if (!pPluginDesigner2)
        {
            pPluginDesigner = std::dynamic_pointer_cast<IS3RODesigner>(pPluginObj);
            if (!pPluginDesigner)
            {
                return E_FAIL;
            }
        }
        /*
        hr = pPluginObj->QueryInterface(IID_IS3RODesigner2, (void**)&pPluginDesigner2);
        if(FAILED(hr))
        {
            hr = pPluginObj->QueryInterface(IID_IS3RODesigner, (void**)&pPluginDesigner);
        }
        }*/
    }

    pDesigner2   = pPluginDesigner2;
    pDesigner    = pPluginDesigner;

    return hr;
}

HRESULT CDesignerPage::QueryMediaType(const CString szFilePath, CString& szType)
{
    HRESULT hr = S_OK;
    CString szExt;
    BOOL bFind = FALSE;
    //if (::PathFileExists(szFilePath))
    {
        szExt = ::PathFindExtension(szFilePath);
        int n = szExt.ReverseFind('.');

        if (n != -1)
        {
            szExt = szExt.Mid(n + 1);
            for (UINT i = 0; i < nVideoSupport; i++)
            {
                if (szExt.CompareNoCase(szVideoSupportType[i]) == 0)
                {
                    bFind = TRUE;
                    szType = szTypeName_Video;
                    break;
                }
            }

            if (!bFind)
            {
                for (UINT i = 0; i < nAudioSupport; i++)
                {
                    if (szExt.CompareNoCase(szAudioSupportType[i]) == 0)
                    {
                        bFind = TRUE;
                        szType = szTypeName_Audio;
                        break;
                    }
                }
            }

            if (!bFind)
            {
                if (GetControllerApp->FindPluginByExt(szExt, szType))
                    bFind = true;
            }

            if (!bFind)
            {
                hr = E_FAIL;
            }
        }
        else
        {
            hr = E_INVALIDARG;
        }
    }
    //else
    //{
    //    hr = E_INVALIDARG;
    //}

    return hr;
}

void CDesignerPage::InitLayout()
{
	// modified by raymondliu
	PROPERTY_TYPE propertyType;
    POSITION pos = m_DrawObjList.GetHeadPosition();
	
    while (pos != NULL)
    {
        CS3DrawObj* pObj = m_DrawObjList.GetNext(pos);
		pObj->GetPropertyType(propertyType);
		if(propertyType != BACKGROUND)
		{
			m_wndLayoutDesign.m_wndDesign.InsertDrawObj(pObj);
		}
    }
	

    m_wndLayoutProperties.m_PropertyCtrl.SetLayout(m_LayoutElement, &m_LayoutGroups, &m_DrawObjList);

    m_wndLayoutDesign.m_wndDesign.RedrawWindow();

	Translate(this);
}

HRESULT CDesignerPage::LoadLayoutFile(CString Filename, const CString& LayoutName, int GroupID, CString ContentXML)
{
    HRESULT hr = S_OK;

    if (m_LayoutElement)
    {
        delete m_LayoutElement;
        m_LayoutElement = NULL;
    }
    m_LayoutElement = new LayoutElement;
    m_LayoutElement->SetLayoutFile(Filename);

	hr = m_LayoutElement->LoadLayout(Filename, GetControllerApp->m_plugMgr, LayoutName, ContentXML);
	if (FAILED(hr))
	{
		GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
			(LPARAM)(Translate(_T("Can not load the layout, try to open it again!"))).GetString());
		return hr;
	}

	NewLayout(m_LayoutElement->GetLayoutSize().cx, m_LayoutElement->GetLayoutSize().cy);
	m_LayoutElement->SetGroupID(GroupID);
	SetObjInfoByLayerInfo();
	InitLayout();

    return hr;
}


HRESULT CDesignerPage::WriteLayoutFile(CString Filename, CString &ContentXML, BOOL bExportToDir)
{
    USES_CONVERSION;

    HRESULT hr = S_OK;
    CString szFileName;
    CString szExt;
    PROPERTY_TYPE propertyType;
	CString dirPath;
	int layerDuration = 0;
    POSITION pos;

	if(TRUE == bExportToDir)
	{
		 dirPath = Filename;
		::CreateDirectory(Filename + _T("\\media\\"),NULL);

		Filename += _T("index.xml");
	}
    else
    {
        dirPath = Filename;
    }

    szExt = ::PathFindExtension(Filename);
    if (szExt.CompareNoCase(_T(".xml")) != 0)
    {
        Filename.Append(_T(".xml"));
    }

	m_LayoutElement->SetLayoutDuration(0);
	m_LayoutElement->SetLayoutFile(Filename);

    pos = m_DrawObjList.GetTailPosition();
    while (pos != NULL)
    {
        CS3DrawObj* pObj = m_DrawObjList.GetPrev(pos);
        pObj->GetPropertyType(propertyType);
        if (propertyType == BACKGROUND)
        {
            continue;
        }

        layerDuration = 0;
        //m_pLayoutSetting->Layers.Lookup(pObj, pLayerInfo);
		BOOL bFound = FALSE;
		std::shared_ptr<LayerElement> layerInfo = m_LayoutElement->GetLayerInfo(pObj->GetLayerId(), bFound);
        if (layerInfo.get() && bFound)
        {
			layerDuration = layerInfo->GetMediasTotalDuraiton();
			SetLayerInfoByObj(pObj, layerInfo);

        }

        if (m_LayoutElement->GetLayoutDuration() < layerDuration)
        {
            m_LayoutElement->SetLayoutDuration(layerDuration);
        }
    }

	//m_LayoutElement->Save(Filename, bExportToDir, ContentXML);
	m_LayoutElement->Save(dirPath, bExportToDir, ContentXML);

    return hr;
}

void CDesignerPage::ConvertBOOL2Str(BOOL bValue, char* pValue)
{
    if (bValue)
    {
        strcpy_s(pValue, MAX_PATH, "TRUE");
    }
    else
    {
        strcpy_s(pValue, MAX_PATH, "FALSE");
    }
}

void CDesignerPage::ConvertDuration2Str(DWORD duration, char* pValue)
{
    int Hour, Minute, Second;
    Hour    = duration / (60 * 60);
    Minute  = (duration % (60 * 60)) / 60;
    Second  = (duration % (60 * 60)) % 60;

    sprintf_s(pValue, MAX_PATH, "%02d:%02d:%02d", Hour, Minute, Second);
}

void CDesignerPage::RemoveLayoutGroups()
{
    POSITION pos = m_LayoutGroups.GetHeadPosition();
    while (pos != NULL)
    {
        LAYOUTGROUP_SETTING* pGroup = m_LayoutGroups.GetNext(pos);
        if (pGroup)
        {
            delete pGroup;
            pGroup = NULL;
        }
    }
    m_LayoutGroups.RemoveAll();
}

void CDesignerPage::GetMediaServerLayoutPath(CString& szLayoutPath)
{
    szLayoutPath = _T("\\Layouts\\");

}

void CDesignerPage::GetLocalTempPath(CString& szTempPath)
{
    szTempPath = GetConfig->Get(ROOT_DIR);
	if(szTempPath.Right(1) != _T("\\"))
	{
		szTempPath += "\\";
	}
	szTempPath += DESIGNER_PATH;

    ProcessCall::CheckAndCreatePath((LPCTSTR)szTempPath);

}

HRESULT CDesignerPage::PackLayout(const CString szLayoutPath, CString& szLayoutFile, CString& szLayoutThumbnail)
{
    HRESULT hr = S_OK;

    if (!ProcessCall::CheckAndCreatePath((LPCTSTR)szLayoutPath))
    {
        return E_FAIL;
    }

    szLayoutFile = szLayoutPath;
    szLayoutThumbnail = szLayoutPath;
    if ((szLayoutFile.ReverseFind(_T('\\')) + 1) == szLayoutFile.GetLength())
    {
        szLayoutFile.Append(_T("index.xml"));
        szLayoutThumbnail.Append(_T("layout.jpg"));
    }
    else
    {
        szLayoutFile.AppendFormat(_T("\\index.xml"));
        szLayoutThumbnail.AppendFormat(_T("\\layout.jpg"));
    }
    // TRANS:
	CString messStr = Translate(_T("Need preview the layout to generate the layout thumbnail, otherwise the layout will not has the thumbnail.")) + _T("\r\n") +
				Translate(_T("Click")) + _T("\"")+ Translate(_T("Yes")) + _T("\"") + Translate(_T("to preview the layout, Click")) + _T("\"") +
				Translate(_T("No")) + _T("\"") + Translate(_T("to cancel the preview."));
    if (MessageBox(messStr, Translate(_T("Confirm Preview")), MB_YESNO) == IDNO)
    {
        szLayoutThumbnail = _T("");
        return hr;
    }

    hr = PreviewLayout(400,300, szLayoutThumbnail);

    return hr;
}

HRESULT CDesignerPage::CheckLayout(int& layoutID)
{
	static TCHAR *validLayoutNames[] = //zxy
	{
		_T("Empty Layout"),
		_T("Power Off Layout")
	};

    HRESULT hr = S_OK;

    if (m_LayoutElement->GetLayoutName().GetLength() <= 0)
    {
        MessageBox(Translate(_T("The layout name can not be null!")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
        return E_INVALIDARG;
    }
	//zxy
	//invalidate names.
	CString strCurName = m_LayoutElement->GetLayoutName();
	strCurName.Trim();
	for(int i = 0;i < sizeof(validLayoutNames)/sizeof(TCHAR *);i++)
	{
		CString strValidName = validLayoutNames[i];
		strValidName.Trim();
		if(strCurName.CompareNoCase(strValidName) == 0)
		{
			MessageBox(Translate(_T("Layout name is occuped by system,please rename it!")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
			return E_INVALIDARG;
		}
	}

    LayoutGroup layoutgroup;
    hr = GetControllerInterface->GetLayoutFilter(m_LayoutElement->GetGroupID(), layoutgroup);
    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
            return hr;
        }
         MessageBox(Translate(_T("Layout Category Does no exist!")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);


        return E_INVALIDARG;
    }


    CString sName;
    sName.Format(_T("LayoutName = '%s' AND LayoutFilterId = %4d"), m_LayoutElement->GetLayoutName(), m_LayoutElement->GetGroupID());

    std::vector<int> layoutIDList;
    Condition condition(sName.GetString());
    hr = GetControllerInterface->SearchLayout(condition, layoutIDList);
    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
            return hr;
        }
        else if (hr == E_UNEXPECTED)
        {
            return hr;
        }

        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    // New layout
    if (layoutIDList.size() == 0)
    {
        return hr;
    }

    // Existing layout, should do update in publish
    auto it = layoutIDList.begin();
    layoutID = *it;

    return hr;
}

HRESULT CDesignerPage::CloseLayout()
{
    HRESULT hr = S_OK;

    m_arrLosedFiles.RemoveAll();
    m_arrSuspicionFiles.RemoveAll();
    m_ObjItemCount.RemoveAll();
    m_ObjTypeCount.RemoveAll();
    m_ClipObjList.RemoveAll();
    //m_ComponentObjCount.RemoveAll();
    m_layerDuration.RemoveAll();

    RemoveAllDrawObj();

    if (m_LayoutElement)
    {
        delete m_LayoutElement;
        m_LayoutElement = NULL;
    }

    m_nDepth = 0;
    m_layoutSize.SetSize(0, 0);

    return hr;
}

LRESULT CDesignerPage::OnLayoutZoom(WPARAM wParam, LPARAM lParam)
{
    LRESULT hr = S_OK;

    FLOAT* zoomFactor = (FLOAT*)lParam;
    FLOAT fFactor = *zoomFactor * 100;
    CString strFactor;
    strFactor.Format(_T("%.1f"), fFactor);
    strFactor.Append(_T("%"));

	m_pComboBoxZoom->SetEditText(strFactor);
    return hr;
}

LRESULT CDesignerPage::OnLayoutSelectObj(WPARAM wParam, LPARAM lParam)
{
    LRESULT hr = S_OK;

    if (wParam == -1)
    {
        m_wndLayoutProperties.m_PropertyCtrl.SetLayout(m_LayoutElement, &m_LayoutGroups, &m_DrawObjList);
    }
    else if (wParam == 1)
    {
        m_wndLayoutProperties.m_PropertyCtrl.SetLayout(m_LayoutElement, &m_LayoutGroups, &m_DrawObjList);
    }
    else
    {
        CS3DrawObjList *pSelectObj = (CS3DrawObjList*)lParam;
        if (pSelectObj->GetCount() == 1)
        {
            CS3DrawObj *pObj = pSelectObj->GetHead();
            //LAYERINFO *pLayerInfo;
			std::shared_ptr<LayerElement> layerInfo;
			BOOL bFound = FALSE;
			layerInfo = m_LayoutElement->GetLayerInfo(pObj->GetLayerId(), bFound);

			m_wndLayoutProperties.m_PropertyCtrl.SetLayer(layerInfo, pObj);

	    }
        else
        {
            m_wndLayoutProperties.m_PropertyCtrl.Clear();
        }
    }

    return hr;
}

#define DEFAULT_HEIGHT 256

LRESULT CDesignerPage::OnLayoutInsertObj(WPARAM wParam, LPARAM lParam)
{
    LRESULT hr = S_OK;
    CString szName = _T("");
    POBJINFO pObjInfo = (POBJINFO)wParam;
    std::vector<std::pair<CString, CLayoutDesignerCtrl::DragMediaInfo> >* pArrMedia =
        (std::vector<std::pair<CString, CLayoutDesignerCtrl::DragMediaInfo> >*)lParam;

    std::shared_ptr<IS3RODesigner2> pDesigner2 = NULL;
    std::shared_ptr<IS3RODesigner>  pDesigner = NULL;
    if (pArrMedia->empty())
    {
		/* LAYOUT-DESIGNER-NEW-VER-RAYMONDLIU */
		//if (pObjInfo->proType == MESSAGE || pObjInfo->proType == AUDIO )
        if (pObjInfo->proType == MESSAGE || pObjInfo->proType == AUDIO )
        {
            pDesigner2   = NULL;
            pDesigner    = NULL;
        }
        else
        {
            HRESULT hr = S_OK;
            if (m_wndComponentList.GetSelectedItemName(szName))
            {

				PRO_CONTENTINFO pContentInfo = new RO_CONTENTINFO;

                hr = QueryDesignerWithName(szName, pDesigner2, pDesigner);
                if (hr != S_OK)
                {
                    pDesigner2  = NULL;
                    pDesigner   = NULL;
                }
                else
                {
                    if (pDesigner2)
                    {
                        hr = pDesigner2->CreateDesigner(NULL, NULL);
                        if (!SUCCEEDED(hr))
                        {
                            MessageBox(Translate(_T("Error to create plug in object.")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
                            return E_FAIL;
                        }
                    }
                    if (pDesigner)
                    {
                        hr = pDesigner->CreateDesigner(NULL, NULL);
                        if (!SUCCEEDED(hr))
                        {
                            MessageBox(Translate(_T("Error to create plug in object.")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
                            return E_FAIL;
                        }
						if (szName.CompareNoCase(_T("S3Clock")) == 0)
						{

							hr = pDesigner->GetContentInfo(pContentInfo);

							LONG height,width;
							FLOAT AspectRatio;
							height = DEFAULT_HEIGHT;
							AspectRatio = (float)height / (float)pContentInfo->height;
							width = (long)floor((AspectRatio * pContentInfo->width) + 0.5f);
							

							pObjInfo->position.right = pObjInfo->position.left + width;
							pObjInfo->position.bottom = pObjInfo->position.top + height;
							
							delete pContentInfo;
						}
                    }
                }
            }
        }

        CreateRectDrawObj(pObjInfo->position, pObjInfo->proType, szName, pDesigner2, pDesigner);
    }
	else
    {
        if (pObjInfo->proType == LAYER)
		{
			LRESULT hr = S_OK;
			if (m_wndComponentList.GetSelectedItemName(szName))
			{
				szName.Empty();
				
				hr = QueryDesignerWithName(szName, pDesigner2, pDesigner);
				if (hr != S_OK)
				{
					pDesigner2  = NULL;
					pDesigner   = NULL;
				}
				else
				{
					if (pDesigner2)
					{
						hr = pDesigner2->CreateDesigner(NULL, NULL);
						if (!SUCCEEDED(hr))
						{
							MessageBox(Translate(_T("Error to create plug in object.")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
							return E_FAIL;
						}
					}
					if (pDesigner)
					{
						hr = pDesigner->CreateDesigner(NULL, NULL);
						if (!SUCCEEDED(hr))
						{
							MessageBox(Translate(_T("Error to create plug in object.")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
							return E_FAIL;
						}
					}
				}
			}
		}
		CreateRectDrawObj(pObjInfo->position, pObjInfo->proType, szName, pDesigner2, pDesigner, pArrMedia);
    }

    return hr;
}

LRESULT CDesignerPage::OnLayoutRemoveObj(WPARAM wParam, LPARAM lParam)
{
    LRESULT hr = S_OK;

    CS3DrawObjList* pObjList = (CS3DrawObjList*)lParam;
    POSITION posSelect;
    POSITION posFind;
    CS3DrawObj* pObj = NULL;
    posSelect = pObjList->GetHeadPosition();
    while (posSelect != NULL)
    {
        pObj = pObjList->GetNext(posSelect);

        posFind = m_ClipObjList.Find(pObj);
        if (posFind != NULL)
        {
            m_ClipObjList.RemoveAt(posFind);
        }

        RemoveDrawObj(pObj);
    }

    m_wndLayoutProperties.m_PropertyCtrl.Clear();

    return hr;
}

LRESULT CDesignerPage::OnLayoutAction(WPARAM wParam, LPARAM lParam)
{
    LRESULT hr = S_OK;

    return hr;
}

LRESULT CDesignerPage::OnObjAddMedia(WPARAM wParam, LPARAM lParam)
{
    LRESULT hr = S_OK;

    CS3DrawObj* pObj = (CS3DrawObj*)wParam;
    std::vector<std::pair<CString, CLayoutDesignerCtrl::DragMediaInfo> >* pArrMedia =
        (std::vector<std::pair<CString, CLayoutDesignerCtrl::DragMediaInfo> >*)lParam;
    PROPERTY_TYPE propertyType;
	CString szLayerName;
    pObj->GetPropertyType(propertyType);
	pObj->GetName(szLayerName);

    //LAYERINFO *pLayerInfo;
	BOOL bFound = FALSE;
	std::shared_ptr<LayerElement>  layerInfo = m_LayoutElement->GetLayerInfo(pObj->GetLayerId(), bFound);
    if (!bFound || layerInfo.get() == NULL)
    {
        ASSERT(FALSE);
    }

    // Query if the renderable object
    CString szName = _T("");
    BOOL bDesinger2 = FALSE;
    UINT nCount = 0;
    std::shared_ptr<IS3RODesigner2> pPluginDesigner2    = NULL;
    std::shared_ptr<IS3RODesigner>  pPluginDesigner     = NULL;
    if (m_wndComponentList.GetSelectedItemName(szName))
    {
        hr = QueryDesignerWithName(szName, pPluginDesigner2, pPluginDesigner);
        if (hr != S_OK)
        {
            pPluginDesigner2    = NULL;
            pPluginDesigner     = NULL;
        }
        else
        {
            if (pPluginDesigner2)
            {
                pPluginDesigner2->CreateDesigner(NULL, NULL);
            }
            if (pPluginDesigner)
            {
                pPluginDesigner->CreateDesigner(NULL, NULL);
            }
		}
    }

	/* LAYOUT-DESIGNER-NEW-VER-RAYMONDLIU */
	if (propertyType == MESSAGE )
	{//message layer can only accept "message" object
		if(0x00 == szName.CompareNoCase(szTypeName_Message) && pArrMedia->empty())
		{

		}
		else if(0x00 == szName.CompareNoCase(szTypeName_EmptyContent) && pArrMedia->empty())
		{

		}
		else
		{
			MessageBox(Translate(_T("Only Message or EmptyContent can be added to Message layer!")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
			return E_FAIL;
		}
	}
	else if(propertyType == AUDIO)
	{// audio layer can only accept "audio" object
		if(pArrMedia->empty())
		{
			MessageBox(Translate(_T("Only audio files can be added to audio layers!")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
			return E_FAIL;
		}
		;//it will be checked in the following code of this function.
	}
	else if(propertyType == LAYER)
	{// layer cannot accept "message","audio","clock" object
		if(0x00 == szName.CompareNoCase(szTypeName_EmptyContent) && pArrMedia->empty() )
		{
			;
		}
		else if(pArrMedia->size())
		{
			;
		}
		else
		{
			MessageBox(Translate(_T("Only media file or EmptyContent can be added to the layer!")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
			return E_FAIL;
		}
	}
	else if (propertyType == USER_DEFINED_PLUGIN)
	{
		if(0x00 == szName.CompareNoCase(szTypeName_EmptyContent) && pArrMedia->empty() )
		{
			;
		}
		else if(0x00 == szName.CompareNoCase(szLayerName) && pArrMedia->empty() )
		{
			;
		}
		else
		{
			MessageBox(Translate(_T("Only same type component or EmptyContent can be added to the layer!")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
			return E_FAIL;
		}
	}
	//there is no "else"

	if (pArrMedia->empty())
    {
		// Empty content to set time
		if(0x00 == szName.CompareNoCase(szTypeName_EmptyContent))
		{
			std::shared_ptr<MediaElement> NewMediaEle;
			layerInfo->CreateMedia(NewMediaEle);
			NewMediaEle->SetMediaFile(szTypeName_EmptyContent);
			NewMediaEle->SetMediaType(szTypeName_EmptyContent);
			NewMediaEle->SetMediaName(szTypeName_EmptyContent);
			NewMediaEle->SetTextSetting(NULL);
			NewMediaEle->SetDesigner2(NULL);
			NewMediaEle->SetDesigner(NULL);
			NewMediaEle->SetBGColor(RGB(0, 0, 0));

			layerInfo->AddTailMediaElement(NewMediaEle);
			AddLayerDuration(pObj, NewMediaEle->GetDuration());
		}
		else if (propertyType == MESSAGE)
        {
			std::shared_ptr<MediaElement> NewMediaEle;
			layerInfo->CreateMedia(NewMediaEle);
			NewMediaEle->SetMediaType(szTypeName_Text);
			NewMediaEle->SetMediaName(szTypeName_Message);
			NewMediaEle->CreateTextSetting();
			NewMediaEle->SetDesigner2(NULL);
			NewMediaEle->SetDesigner(NULL);

            nCount = 1;
            if (m_ObjItemCount.Lookup(pObj, nCount) != 0)
            {
                nCount += 1;
                m_ObjItemCount.SetAt(pObj, nCount);
            }
            else
            {
                m_ObjItemCount.SetAt(pObj, nCount);
            }

			CString szMediaFile;
			szMediaFile.Format(_T("%s%d"), szTypeName_Message, nCount);
            NewMediaEle->SetMediaFile(szMediaFile);

            AddLayerDuration(pObj, NewMediaEle->GetDuration());
           	layerInfo->AddTailMediaElement(NewMediaEle);
        }
		/* LAYOUT-DESIGNER-NEW-VER-RAYMONDLIU */
        //else if (propertyType == PLUGIN)
		else if (propertyType == LAYER || propertyType == USER_DEFINED_PLUGIN)
        {
            PLUGIN_SUPPORTFILES* pSupportFiles = NULL;
            if ((GetControllerApp->FindPluginSupportFile(szName, pSupportFiles) == 0) || szName.CompareNoCase(_T("S3WebBrowser")) == 0)
            {

				std::shared_ptr<MediaElement> NewMediaEle;
				layerInfo->CreateMedia(NewMediaEle);
				NewMediaEle->SetMediaType(szName);
				NewMediaEle->SetMediaName(szName);
				NewMediaEle->SetTextSetting(NULL);
				NewMediaEle->SetDesigner2(pPluginDesigner2);
				NewMediaEle->SetDesigner(pPluginDesigner);

				nCount = 1;
                if (m_ObjItemCount.Lookup(pObj, nCount) != 0)
                {
                    nCount += 1;
                    m_ObjItemCount.SetAt(pObj, nCount);
                }
                else
                {
                    m_ObjItemCount.SetAt(pObj, nCount);
                }
				
				CString szMediaFile;
#ifdef STARTER_EDITION
				szMediaFile.AppendFormat(_T("%s"), szName);
				NewMediaEle->SetMediaFile(szMediaFile);
#else
				szMediaFile.AppendFormat(_T("%s%d"), szName, nCount);
				NewMediaEle->SetMediaFile(szMediaFile);
#endif

                AddLayerDuration(pObj, NewMediaEle->GetDuration());
				layerInfo->AddTailMediaElement(NewMediaEle);
            }

        }
    }
    else
    {
        BOOL bAudioError = FALSE;
        BOOL bLayerError = FALSE;
        for (int i = 0; i < pArrMedia->size(); i++)
        {
			std::shared_ptr<MediaElement> NewMediaEle;
			layerInfo->CreateMedia(NewMediaEle);

            // Empty content to set time
            if (pArrMedia->at(i).first.CompareNoCase(szTypeName_EmptyContent) == 0)
            {
                NewMediaEle->SetMediaFile(szTypeName_EmptyContent);
                NewMediaEle->SetMediaType(szTypeName_EmptyContent);
                NewMediaEle->SetMediaName(szTypeName_EmptyContent);
                NewMediaEle->SetTextSetting(NULL);
                NewMediaEle->SetDesigner2(NULL);
                NewMediaEle->SetDesigner(NULL);
                NewMediaEle->SetBGColor(RGB(0, 0, 0));
				layerInfo->AddTailMediaElement(NewMediaEle);
            }
            else
            {
                NewMediaEle->SetMediaFile(pArrMedia->at(i).first);
                NewMediaEle->SetMediaID(pArrMedia->at(i).second.MediaID);
                NewMediaEle->SetMediaName(pArrMedia->at(i).second.MediaName);
                NewMediaEle->SetDesigner2(pPluginDesigner2);
                NewMediaEle->SetDesigner(pPluginDesigner);
				if (pArrMedia->at(i).second.MediaType == MEDIA_URL)
				{
					NewMediaEle->SetMediaType(_T("S3WebBrowser"));
				}
                else
				{
					CString szType;
					QueryMediaType(pArrMedia->at(i).first, szType);
					NewMediaEle->SetMediaType(szType);
				}

                int ContentDuration = pArrMedia->at(i).second.Duration;

                if(ContentDuration != 0)
                {
                    NewMediaEle->SetDuration(ContentDuration/1000);
                }
                NewMediaEle->SetDuration(max(1, NewMediaEle->GetDuration()));

                // The audio layer only support audio files
                if (NewMediaEle->GetMediaType().CompareNoCase(szTypeName_Audio) == 0)
                {
                    if (propertyType == AUDIO)
                    {
						layerInfo->AddTailMediaElement(NewMediaEle);
                    }
                    else
                    {
                        bAudioError = TRUE;
                        
						/*delete pMediaSetting;
                        pMediaSetting = NULL;*/
                    }
                }
                else
                {
                    if (propertyType == AUDIO)
                    {
                        bLayerError = TRUE;
 /*                       delete pMediaSetting;
                        pMediaSetting = NULL;*/
                    }
                    else
                    {
                        if (NewMediaEle->GetMediaType().CompareNoCase(szTypeName_Video) == 0)
                        {
                            NewMediaEle->SetKeepAspect(TRUE);
                        }
                        else if (NewMediaEle->GetMediaType().CompareNoCase(_T("S3ImageViewer")) == 0)
                        {
                            NewMediaEle->SetKeepAspect(TRUE);
                        }
						layerInfo->AddTailMediaElement(NewMediaEle);
                    }
                }
            }

            if(NewMediaEle.get())
            {
                AddLayerDuration(pObj, NewMediaEle->GetDuration());
            }

        }

        if (bAudioError)
        {
            MessageBox(Translate(_T("Only audio layer can input audio files!")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
            return hr;
        }
        if (bLayerError)
        {
            MessageBox(Translate(_T("Only audio files can be added to audio layers!")), Translate(_T("Error:Designer page")),MB_OK| MB_ICONERROR);
            return hr;
        }
    }

    CS3DrawObjList selectList;
    selectList.AddTail(pObj);
    if (propertyType == BACKGROUND)
    {
        OnLayoutSelectObj(1, (LPARAM)&selectList);
    }
    else
    {
        OnLayoutSelectObj(0, (LPARAM)&selectList);
    }

    return hr;
}

LRESULT CDesignerPage::OnLayoutSizeChange(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0L;

	//AXIS_ARRAY &xArray =  m_wndLayoutDesign.m_Ruler.GetXBindLines();
	//AXIS_ARRAY &yArray =  m_wndLayoutDesign.m_Ruler.GetYBindLines();
	//
	//if(xArray.size() || yArray.size())
	//{
	//	int ret = AfxMessageBox(Translate(_T("Reset Layout size,will clear all bindlines,are you sure?")),MB_OKCANCEL|MB_ICONINFORMATION);
	//	if(ret != IDOK)
	//		return lr;
	//	xArray.clear();
	//	yArray.clear();
	//}
    NewLayout(m_LayoutElement->GetLayoutSize().cx, m_LayoutElement->GetLayoutSize().cy, m_LayoutElement->GetLayoutRotateDegree());

    return lr;
}

LRESULT CDesignerPage::OnLayerSizeChange2(WPARAM wParam, LPARAM lParam)
{
	LRESULT lr = 0L;

	BOOL bInvalid = FALSE;
	BOOL bMove = FALSE;
	POS_CHANGE_INFO *pChangePos = (POS_CHANGE_INFO *)lParam;
	CS3DrawObj *pObj = (CS3DrawObj *)wParam;
	if (pObj == NULL)
	{
		return lr;
	}

	if (pChangePos->ChangeType == VALUE_TYPE_X)
	{
		if (pChangePos->rectChanged.left < -30000 || pChangePos->rectChanged.left > 30000)
		{
			MessageBox(Translate(_T("Invalid value of the layer's left position. Please input the value in the range: -30000 - 30000!")),
				Translate(_T("Error:Check value")), MB_OK|MB_ICONERROR);
			bInvalid = TRUE;
		}
		bMove = TRUE;
	}
	else if (pChangePos->ChangeType == VALUE_TYPE_Y)
	{
		if (pChangePos->rectChanged.top < -30000 || pChangePos->rectChanged.top > 30000)
		{
			MessageBox(Translate(_T("Invalid value of the layer's top position. Please input the value in the range: -30000 - 30000!")),
				Translate(_T("Error:Check value")), MB_OK|MB_ICONERROR);
			bInvalid = TRUE;
		}
		bMove = TRUE;
	}
	else if (pChangePos->ChangeType == VALUE_TYPE_W)
	{
		if (pChangePos->rectChanged.Width() < 1 || pChangePos->rectChanged.Width() > 30000)
		{
			MessageBox(Translate(_T("Invalid value of the layer's width. Please input the value in the range: 1 - 30000!")),
				Translate(_T("Error:Check value")), MB_OK|MB_ICONERROR);
			bInvalid = TRUE;
		}
	}
	else if (pChangePos->ChangeType == VALUE_TYPE_H)
	{
		if (pChangePos->rectChanged.Height() < 1 || pChangePos->rectChanged.Height() > 30000)
		{
			MessageBox(Translate(_T("Invalid value of the layer's height. Please input the value in the range: 1 - 30000!")),
				Translate(_T("Error:Check value")), MB_OK|MB_ICONERROR);
			bInvalid = TRUE;
		}
	}


	FPoint fPointClick;
	FRect rectLogic;
	CRect rectPos;

	fPointClick.SetPoint(0, 0);
	pObj->GetLogicPos(rectLogic);

	if (!bInvalid)
	{
		if (bMove)
		{
			m_wndLayoutDesign.m_wndDesign.MoveObjPos(pObj, pChangePos->rectChanged, fPointClick);
		}
		else
		{
			if (pChangePos->ChangeType == VALUE_TYPE_W)
			{
				fPointClick.SetPoint(pChangePos->rectChanged.right, rectLogic.bottom);
				m_wndLayoutDesign.m_wndDesign.ResizeObjLine(pObj, fPointClick, 2);
			}
			else if (pChangePos->ChangeType == VALUE_TYPE_H)
			{
				fPointClick.SetPoint(rectLogic.right, pChangePos->rectChanged.bottom);
				m_wndLayoutDesign.m_wndDesign.ResizeObjLine(pObj, fPointClick, 3);
			}
		}

		m_wndLayoutDesign.m_wndDesign.CalcScrollRange();

		m_wndLayoutDesign.m_wndDesign.RedrawWindow();
	}

	//added by RaymondLiu
	pObj->SetLogicPos(pChangePos->rectChanged);


	return lr;
}

LRESULT CDesignerPage::OnSimplePluginSizeChange(WPARAM wParam, LPARAM lParam)
{
	LRESULT lr = 0L;

	BOOL bInvalid = FALSE;
	BOOL bMove = FALSE;
	PRO_CONTENTINFO pRoInfo = (PRO_CONTENTINFO)wParam;
	CS3DrawObj *pObj = (CS3DrawObj *)lParam;
	if (pObj == NULL)
	{
		return lr;
	}
	

	CString ObjName;
	FPoint fPointClick;
	FRect rectLogic;
	CRect rectPos;

	fPointClick.SetPoint(0, 0);
	pObj->GetLogicPos(rectLogic);
	pObj->GetName(ObjName);
	if (0 == ObjName.CompareNoCase(_T("S3Clock")))
	{
		pObj->SetKeepAspect(FALSE);
	}



	if (!bInvalid)
	{



				fPointClick.SetPoint(rectLogic.left + pRoInfo->width, rectLogic.top + pRoInfo->height);
				m_wndLayoutDesign.m_wndDesign.ResizeObjLine(pObj, fPointClick, 2);


//				fPointClick.SetPoint(rectLogic.right, rectLogic.top + pRoInfo->height);
				m_wndLayoutDesign.m_wndDesign.ResizeObjLine(pObj, fPointClick, 3);
			

		m_wndLayoutDesign.m_wndDesign.CalcScrollRange();
		m_wndLayoutDesign.m_wndDesign.RedrawWindow();
	}

	FRect rect;
	FLOAT AspectRatio;
	LONG height,width;
	height = rectLogic.Height();
	AspectRatio = (FLOAT)height / (FLOAT)pRoInfo->height;
	width = (LONG)floor((pRoInfo->width) * AspectRatio + 0.5f);
	rect.left = rectLogic.left;
	rect.right = rectLogic.left + width;
	rect.top = rectLogic.top;
	rect.bottom = rectLogic.top + height;


	if (0 == ObjName.CompareNoCase(_T("S3Clock")))
	{
		pObj->SetKeepAspect(TRUE);
	}
	pObj->SetLogicPos(rect);

	BOOL bFound = FALSE;
	std::shared_ptr<LayerElement>  layerInfo = m_LayoutElement->GetLayerInfo(pObj->GetLayerId(), bFound);

	if (!bFound)
	{
		layerInfo.reset();
	}
	m_wndLayoutProperties.m_PropertyCtrl.SetLayer(layerInfo, pObj);

	m_wndLayoutDesign.m_wndDesign.SelectDrawObj(pObj, 0);
	m_wndLayoutDesign.m_wndDesign.RedrawWindow();


	return lr;
}
/*
LRESULT CDesignerPage::OnLayerSizeChange(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0L;

    BOOL bInvalid = FALSE;
    BOOL bMove = FALSE;
    POS_CHANGE_INFO *pChangePos = (POS_CHANGE_INFO *)lParam;
    CS3DrawObj *pObj = (CS3DrawObj *)wParam;
    if (pObj == NULL)
    {
        return lr;
    }

    if (pChangePos->ChangeType == VALUE_TYPE_X)
    {
        if (pChangePos->rectChanged.left < -30000 || pChangePos->rectChanged.left > 30000)
        {
            MessageBox(Translate(_T("Invalid value of the layer's left position. Please input the value in the range: -30000 - 30000!")),
                Translate(_T("Error:Check value")), MB_OK|MB_ICONERROR);
            bInvalid = TRUE;
        }
        bMove = TRUE;
    }
    else if (pChangePos->ChangeType == VALUE_TYPE_Y)
    {
        if (pChangePos->rectChanged.top < -30000 || pChangePos->rectChanged.top > 30000)
        {
            MessageBox(Translate(_T("Invalid value of the layer's top position. Please input the value in the range: -30000 - 30000!")),
                Translate(_T("Error:Check value")), MB_OK|MB_ICONERROR);
            bInvalid = TRUE;
        }
        bMove = TRUE;
    }
    else if (pChangePos->ChangeType == VALUE_TYPE_W)
    {
        if (pChangePos->rectChanged.Width() < 1 || pChangePos->rectChanged.Width() > 30000)
        {
            MessageBox(Translate(_T("Invalid value of the layer's width. Please input the value in the range: 1 - 30000!")),
                Translate(_T("Error:Check value")), MB_OK|MB_ICONERROR);
            bInvalid = TRUE;
        }
    }
    else if (pChangePos->ChangeType == VALUE_TYPE_H)
    {
        if (pChangePos->rectChanged.Height() < 1 || pChangePos->rectChanged.Height() > 30000)
        {
            MessageBox(Translate(_T("Invalid value of the layer's height. Please input the value in the range: 1 - 30000!")),
                Translate(_T("Error:Check value")), MB_OK|MB_ICONERROR);
            bInvalid = TRUE;
        }
    }

    FPoint fPointClick;
    FRect rectLogic;
    CRect rectPos;

    fPointClick.SetPoint(0, 0);
    pObj->GetLogicPos(rectLogic);

    if (!bInvalid)
    {
        if (bMove)
        {
            m_wndLayoutDesign.m_wndDesign.MoveObjPos(pObj, pChangePos->rectChanged, fPointClick);
        }
        else
        {
            if (pChangePos->ChangeType == VALUE_TYPE_W)
            {
                fPointClick.SetPoint(pChangePos->rectChanged.right, rectLogic.bottom);
                m_wndLayoutDesign.m_wndDesign.ResizeObjLine(pObj, fPointClick, 2);
            }
            else if (pChangePos->ChangeType == VALUE_TYPE_H)
            {
                fPointClick.SetPoint(rectLogic.right, pChangePos->rectChanged.bottom);
                m_wndLayoutDesign.m_wndDesign.ResizeObjLine(pObj, fPointClick, 3);
            }
        }

        m_wndLayoutDesign.m_wndDesign.CalcScrollRange();
        m_wndLayoutDesign.m_wndDesign.RedrawWindow();
    }

	BOOL bFound = FALSE;
	std::shared_ptr<LayerElement>  layerInfo = m_LayoutElement->GetLayerInfo(pObj->GetLayerId(), bFound);
    //LAYERINFO *pLayerInfo;
    if (!bFound)
    {
        layerInfo.reset();
    }
    m_wndLayoutProperties.m_PropertyCtrl.SetLayer(layerInfo, pObj);

    return lr;
}
*/

LRESULT CDesignerPage::OnLayerMediaSelChange(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0L;

    return lr;
}

LRESULT CDesignerPage::OnLayoutOpen(WPARAM wParam, LPARAM lParam)
{
    HRESULT hr = S_OK;
    Layout* pLayout = (Layout*)lParam;

    CString szLocalLayoutPath;

    GetLocalTempPath(szLocalLayoutPath);


    if (m_layoutSize.cx > 0 || m_layoutSize.cy > 0)
    {
        OnRibbonBtnClose();
    }

    std::vector<BYTE> LayoutContentByte;

    if (pLayout->GetContainUnapprovedMedia())
    {
        MessageBox(Translate(_T("This layout contain some media(s) which is not approved!")), Translate(_T("Warning:Designer page")), MB_OK|MB_ICONEXCLAMATION);
    }

    hr = GetControllerInterface->GetLayoutContent(pLayout->GetLayoutID(), LayoutContentByte);
    if (!SUCCEEDED(hr))
    {

        MessageBox(Translate(_T("Failed to get layout content!")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
        GetParent()->PostMessage(CLOSE_CURRENT_PAGE, NULL, NULL);
        return 0;
    }

    std::string LayoutContentString;
    auto it = LayoutContentByte.begin();
    while(it != LayoutContentByte.end())
    {
        LayoutContentString.push_back(*it);
        it++;
    }
    CString LayoutContent = CA2W(LayoutContentString.c_str(),CP_UTF8);

    LoadLayoutFile(_T(""), pLayout->GetLayoutName().c_str(), pLayout->GetGroupID(), LayoutContent);

    return hr;
}


void CDesignerPage::OnUpdateRibbonBtnNeedSelect(CCmdUI* pCmdUI)
{
    CS3DrawObjList* pObjList = m_wndLayoutDesign.m_wndDesign.GetSelectDrawObj();
    INT nSelectedCnt = pObjList->GetCount();
    if (nSelectedCnt > 0)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CDesignerPage::OnUpdateRibbonBtnNeedSelectCopy(CCmdUI* pCmdUI)
{
    BOOL bAudio = FALSE;
    CS3DrawObj *pObj = NULL;
    CS3DrawObjList* pObjList = m_wndLayoutDesign.m_wndDesign.GetSelectDrawObj();
    INT nSelectedCnt = pObjList->GetCount();
    if (nSelectedCnt == 1)
    {
        POSITION pos = pObjList->GetHeadPosition();
        while (pos != NULL)
        {
            pObj = pObjList->GetNext(pos);
            PROPERTY_TYPE protype;
            pObj->GetPropertyType(protype);
            if (protype == AUDIO)
            {
                bAudio = TRUE;
            }
        }
    }

    if (nSelectedCnt > 0 && !bAudio)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}


void CDesignerPage::OnUpdateRibbonBtnCanMoveForward(CCmdUI* pCmdUI)
{
	BOOL bAudio = FALSE;
	CS3DrawObj *pObj = NULL;
	CS3DrawObjList* pObjList = m_wndLayoutDesign.m_wndDesign.GetSelectDrawObj();
	INT nSelectedCnt = pObjList->GetCount();
	INT nCnt = m_DrawObjList.GetCount();

	CS3DrawObj* pObjxx;
	PROPERTY_TYPE propertyType;

	POSITION pos2;
	pos2 = m_DrawObjList.GetHeadPosition();
	while(pos2 != NULL)
	{
		pObjxx = m_DrawObjList.GetNext(pos2);
		pObjxx->GetPropertyType(propertyType);
	}

	if (nSelectedCnt == 1 && nCnt > 2 )
	{
		POSITION SelectPos = pObjList->GetHeadPosition();
		while (SelectPos != NULL)
		{
			pObj = pObjList->GetNext(SelectPos);
		}
		
		POSITION pos = m_DrawObjList.Find(pObj);
		POSITION TailPos = m_DrawObjList.GetTailPosition();
		
		if (pos != TailPos)
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

void CDesignerPage::OnUpdateRibbonBtnCanMoveBackward(CCmdUI* pCmdUI)
{
	BOOL bAudio = FALSE;
	CS3DrawObj *pObj = NULL;
	CS3DrawObjList* pObjList = m_wndLayoutDesign.m_wndDesign.GetSelectDrawObj();
	INT nSelectedCnt = pObjList->GetCount();
	INT nCnt = m_DrawObjList.GetCount();

	if (nSelectedCnt == 1 && nCnt > 2 )
	{
		POSITION SelectPos = pObjList->GetHeadPosition();
		while (SelectPos != NULL)
		{
			pObj = pObjList->GetNext(SelectPos);
		}

		POSITION pos = m_DrawObjList.Find(pObj);
		POSITION HeadPos = m_DrawObjList.GetHeadPosition();
		CS3DrawObj *pHeadObj = NULL;
		pHeadObj = m_DrawObjList.GetNext(HeadPos);

		if (pos != HeadPos)
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


void CDesignerPage::OnUpdateRibbonBtnNeedCopied(CCmdUI* pCmdUI)
{
    BOOL bAudio = FALSE;
    CS3DrawObj *pObj = NULL;
    CS3DrawObjList* pObjList = m_wndLayoutDesign.m_wndDesign.GetSelectDrawObj();
    if (pObjList->GetCount() == 1)
    {
        POSITION pos = pObjList->GetHeadPosition();
        while (pos != NULL)
        {
            pObj = pObjList->GetNext(pos);
            PROPERTY_TYPE protype;
            pObj->GetPropertyType(protype);
            if (protype == AUDIO)
            {
                bAudio = TRUE;
            }
        }
    }

    INT nSelectedCnt = m_ClipObjList.GetCount();
    if (nSelectedCnt > 0 && !bAudio)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CDesignerPage::OnUpdateRibbonBtnNeedLayers(CCmdUI* pCmdUI)
{
    INT nCnt = m_DrawObjList.GetCount();
    if (nCnt > 1)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CDesignerPage::OnUpdateBindLayBtn(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_wndLayoutDesign.m_wndDesign.IsBindLayEnable());
}
void CDesignerPage::OnUpdateRibbonBtnCreateLayout(CCmdUI* pCmdUI)
{
    if (m_layoutSize.cx != 0 || m_layoutSize.cy != 0)
    {
        pCmdUI->Enable(FALSE);
    }
    else
    {
        pCmdUI->Enable(TRUE);
    }
}

void CDesignerPage::OnUpdateRibbonBtnCloseLayout(CCmdUI* pCmdUI)
{
    if (m_layoutSize.cx != 0 || m_layoutSize.cy != 0)
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CDesignerPage::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	 LayoutCmdEngine *pEngine = (LayoutCmdEngine *)m_wndLayoutDesign.m_wndDesign.GetCmdEngine();
	 pCmdUI->Enable(pEngine->canUndo());
}

void CDesignerPage::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	LayoutCmdEngine *pEngine = (LayoutCmdEngine *)m_wndLayoutDesign.m_wndDesign.GetCmdEngine();
	pCmdUI->Enable(pEngine->canRedo());
}

void CDesignerPage::OnUpdateRibbonBtnPreview(CCmdUI* pCmdUI)
{
    if (pCmdUI->m_nID >= ID_RIBBON_BTN_DESIGNER_PREVIEW && pCmdUI->m_nID <= ID_RIBBON_BTN_PREVIEW_19X10)
    {
        BOOL bPlayer = PlayerController::GetInstance()->IsPlayerRunning();
        if ((m_layoutSize.cx != 0 || m_layoutSize.cy != 0) && !bPlayer)
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
        if ((m_layoutSize.cx != 0 || m_layoutSize.cy != 0))
        {
            pCmdUI->Enable(TRUE);
        }
        else
        {
            pCmdUI->Enable(FALSE);
        }
    }
}

void CDesignerPage::OnEditRedo()
{
	LayoutCmdEngine *pEngine = (LayoutCmdEngine *)m_wndLayoutDesign.m_wndDesign.GetCmdEngine();
	pEngine->redo();
}

void CDesignerPage::OnEditUndo()
{
	LayoutCmdEngine *pEngine = (LayoutCmdEngine *)m_wndLayoutDesign.m_wndDesign.GetCmdEngine();
	pEngine->undo();
}

HRESULT CDesignerPage::PreviewLayout(const int nWidth, const int nHeight, CString szLayoutThumbnail)
{
    HRESULT hr = S_OK;
    CString szLayoutName;
    CString szLocalPath;
    GetLocalTempPath(szLocalPath);
    CString szSnapshot;
    BOOL bExistFiles = FALSE;

    m_wndLayoutDesign.SetFocus();

    GetLocalTempPath(szLayoutName);

    szLayoutName += _T("index.xml");

    vector<INT> MediaIDs;
    GetMediaIDListFromLayout(m_LayoutElement, MediaIDs);

    m_arrLosedFiles.RemoveAll();
    m_arrSuspicionFiles.RemoveAll();

    CString ContentXML;
    WriteLayoutFile(szLayoutName, ContentXML);

    LayoutFileUtility::PreviewLayout(szLayoutName, MediaIDs, nWidth, nHeight, szLayoutThumbnail);

    return hr;
}

LRESULT CDesignerPage::OnLayerLoopChange(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0L;

    CS3DrawObj *pObjChange = (CS3DrawObj *)wParam;
    if (pObjChange == NULL)
    {
        return lr;
    }

    m_LayoutElement->SetLayoutDuration(0);

    //LAYERINFO *pLayerInfo;
    PROPERTY_TYPE propertyType;
    POSITION pos;
    DWORD layerDuration = 0;
    pos = m_DrawObjList.GetTailPosition();
    while (pos != NULL)
    {
        CS3DrawObj* pObj = m_DrawObjList.GetPrev(pos);
        pObj->GetPropertyType(propertyType);
        if (propertyType == BACKGROUND)
        {
            continue;
        }

        layerDuration = 0;
		BOOL bFound = FALSE;
		std::shared_ptr<LayerElement> layer = m_LayoutElement->GetLayerInfo(pObj->GetLayerId(), bFound);
        if (bFound && layer.get() != NULL)
        {
			layerDuration = layer->GetMediasTotalDuraiton();

        }

        if (m_LayoutElement->GetLayoutDuration() < layerDuration)
        {
            m_LayoutElement->SetLayoutDuration(layerDuration);
        }
    }


	m_wndLayoutProperties.m_PropertyCtrl.SetLayoutDuration(m_LayoutElement->GetLayoutDuration());

    return lr;
}

void CDesignerPage::AddLayerDuration(CS3DrawObj* pObj, const DWORD duration)
{
    DWORD nDuration = 0;

    if (m_layerDuration.Lookup(pObj, nDuration) != 0)
    {
        nDuration += duration;
        m_layerDuration.SetAt(pObj, nDuration);
    }
    else
    {
        nDuration = duration;
        m_layerDuration.SetAt(pObj, nDuration);
    }

	if(m_LayoutElement->GetLayoutDuration() < nDuration)
	{
		m_LayoutElement->SetLayoutDuration(nDuration);
	}
/*
    if (m_pLayoutSetting->LayoutDuration < nDuration)
    {
        m_pLayoutSetting->LayoutDuration = nDuration;
    }*/

}

LRESULT CDesignerPage::OnLayerNameChange(WPARAM wParam,LPARAM lParam)
 {
     LRESULT lr = 0L;
     CS3DrawObj* pObj = (CS3DrawObj*)wParam;
     FRect fObjRect;
     pObj->GetLogicPos(fObjRect);
     m_wndLayoutDesign.m_wndDesign.LayoutToClient(fObjRect);

     CRect ObjRect;
     fObjRect.ConvertToCRect(ObjRect);
     m_wndLayoutDesign.m_wndDesign.InvalidateRect(ObjRect);
     return lr;
 }
 
LRESULT CDesignerPage::OnLayerSelChange(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0L;

    CS3DrawObj* pObj = (CS3DrawObj*)wParam;
    m_wndLayoutDesign.m_wndDesign.SelectDrawObj(pObj, 0);

    m_wndLayoutDesign.m_wndDesign.RedrawWindow();

    return lr;
}

void CDesignerPage::OnUpdateRibbonBtnPublish(CCmdUI* pCmdUI)
{
    if (!GetControllerApp->IsLogin())
    {
        pCmdUI->Enable(FALSE);
        return;
    }
    OnUpdateRibbonBtnCloseLayout(pCmdUI);
}

void CDesignerPage::OnUpdateRibbonBtnMgrLayout(CCmdUI* pCmdUI)
{
    if (!GetControllerApp->IsLogin())
    {
        pCmdUI->Enable(FALSE);
        return;
    }
    OnUpdateRibbonBtnCreateLayout(pCmdUI);
}

BOOL CDesignerPage::CheckSupportFile(const CString &ext)
{
    BOOL bFilter = FALSE;
    int n = ext.ReverseFind('.');
    for (UINT i = 0; i < nVideoSupport; i++)
    {
        if (ext.CompareNoCase(szVideoSupportType[i]) == 0)
        {
            return TRUE;
        }
    }

    for (UINT i = 0; i < nAudioSupport; i++)
    {
        if (ext.CompareNoCase(szAudioSupportType[i]) == 0)
        {
            return TRUE;
        }
    }

    CString szPluginName;
    PPLUGIN_SUPPORTFILES pSupportList = NULL;
    
    if (GetControllerApp->FindPluginByExt(ext, szPluginName))
        return TRUE;

    return FALSE;
}

#ifndef STARTER_EDITION

LRESULT CDesignerPage::MediaCategoryListSelChange(WPARAM wp, LPARAM lp)
{
    CMediaLibraryCategoryCtrl::ITEM_TYPE ItemType;
    INT ID = m_wndMediaFileBrowser.m_wndStateTab.m_wndMediaLibrary->GetSelectedCategory(ItemType);
    if (ItemType == CMediaLibraryCategoryCtrl::ITEM_CATEGORY)
    {
        MediaFilter Category;
        HRESULT hr = GetControllerInterface->GetMediaFilter(ID, Category);
        if (!SUCCEEDED(hr))
        {
            TRACE1("Failed to Get Media Filter %d!\n", ID);
        }

        m_wndMediaInfo.m_MediaLibraryInfoCtrl.SetCategoryInfo(Category);
    }
    else if (ItemType == CMediaLibraryCategoryCtrl::ITEM_FILE)
    {
        int mediaID = ID;
        std::vector<int> mediaSingleList;
        std::vector<Media> mediaSingleInfo;
        mediaSingleList.push_back(mediaID);
        //get media thumbnail
        MediaThumbnail thumbnail;
        HRESULT hr = GetControllerInterface->GetMediaThumbnail(mediaID, thumbnail);
        if (!SUCCEEDED(hr))
        {
            TRACE0("Failed to Get Media Thumbnail!\n");
        }
        //show media thumbnail in picture control


        vector<BYTE> data;
        thumbnail.GetThumbnail(data);
        MLThumbnail thumbnailImage(data);
        thumbnailImage.InitFromData(data);

        m_wndMediaInfo.m_MediaLibraryInfoCtrl.SetMediaThumbnail(thumbnailImage.GetImage());

        //get media detail info
        hr = GetControllerInterface->GetMedias(mediaSingleList, mediaSingleInfo);
        if (!SUCCEEDED(hr) || mediaSingleInfo.size() < 1)
        {
            TRACE0("Get media detail info fail!\n");
            return E_FAIL;
        }

        m_wndMediaInfo.m_MediaLibraryInfoCtrl.SetMediaInfo(*mediaSingleInfo.begin());
    }

    return 0;

}

#endif

BOOL CDesignerPage::GetMediaIDListFromLayout(LayoutElement *pLayoutInfo, vector<int>& MediaIDs)
{
    //LAYERINFO* pLayerInfo;


    CMap<CString, LPCTSTR, INT, INT> TestFilePool;
    // If a file path name exist in the pool, it is already added to the filelist. no need to add it again.

	list<std::shared_ptr<LayerElement>> layers = pLayoutInfo->GetAllLayers();
	list<std::shared_ptr<LayerElement>> :: iterator it;
	for(it = layers.begin(); it != layers.end(); it++)
	{
		vector<std::shared_ptr<MediaElement>> medias = (*it)->GetMediaList();
		vector<std::shared_ptr<MediaElement>> :: iterator itm;
		for(itm = medias.begin(); itm != medias.end(); itm++)
		{
			CString   strSize;
			CString   strDateModify;

			INT TestValue;

			CString File;
			if (!(*itm)->GetMediaFile().IsEmpty())
			{
				File = (*itm)->GetMediaFile();
			}

			if (!(*itm)->GetMessageFile().IsEmpty())
			{
				File = (*itm)->GetMessageFile();
			}

			if (File.IsEmpty() || TestFilePool.Lookup(File, TestValue))
			{
				continue;
			}

			TestFilePool.SetAt(File, 1);

			if ((*itm)->GetMediaID() >= 0)
			{
				MediaIDs.push_back((*itm)->GetMediaID());
			}

		}
	}
    return TRUE;

}

void CDesignerPage::OnUpdateRibbonBtn(CCmdUI* pCmdUI)
{
    if (!IsEnable())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    std::map<int, std::pair<std::function<void (CCmdUI*)>, std::function<bool ()> > > dispatchMap;
    // check privilege
    {
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_NEW, &CDesignerPage::OnUpdateRibbonBtnCreateLayout, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_OPEN, &CDesignerPage::OnUpdateRibbonBtnMgrLayout, LAMBDA(SC(ListLayoutFilter) && SC(GetLayout)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_CLOSE, &CDesignerPage::OnUpdateRibbonBtnCloseLayout, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_SAVE, &CDesignerPage::OnUpdateRibbonBtnCloseLayout, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_SAVEAS, &CDesignerPage::OnUpdateRibbonBtnCloseLayout, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_PUBLISH, &CDesignerPage::OnUpdateRibbonBtnPublish, LAMBDA(SC(PublishLayout)));
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_EXPORT, &CDesignerPage::OnUpdateRibbonBtnCloseLayout, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_IMPORT, &CDesignerPage::OnUpdateRibbonBtnCloseLayout, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_PASTE, &CDesignerPage::OnUpdateRibbonBtnNeedCopied, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_COPY, &CDesignerPage::OnUpdateRibbonBtnNeedSelectCopy, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_DELETEITEM, &CDesignerPage::OnUpdateRibbonBtnNeedSelect, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_SELECTALL, &CDesignerPage::OnUpdateRibbonBtnNeedLayers, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_PREVIEW, &CDesignerPage::OnUpdateRibbonBtnPreview, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_MOVETOFRONT, &CDesignerPage::OnUpdateRibbonBtnCanMoveForward, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_MOVEFORWARD, &CDesignerPage::OnUpdateRibbonBtnCanMoveForward, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_MOVETOBACK, &CDesignerPage::OnUpdateRibbonBtnCanMoveBackward, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_MOVEBACKWARD, &CDesignerPage::OnUpdateRibbonBtnCanMoveBackward, RET_TRUE());
        UPDATE_COMMAND_ENTRY(ID_RIBBON_COMBOX_LAYOUT_ZOOM, &CDesignerPage::OnUpdateRibbonBtnCloseLayout, RET_TRUE());
		UPDATE_COMMAND_ENTRY(ID_EDIT_REDO, &CDesignerPage::OnUpdateEditRedo, RET_TRUE());
		UPDATE_COMMAND_ENTRY(ID_EDIT_UNDO, &CDesignerPage::OnUpdateEditUndo, RET_TRUE());
		UPDATE_COMMAND_ENTRY(ID_RIBBON_BTN_DESIGNER_BINDLAY,&CDesignerPage::OnUpdateBindLayBtn,RET_TRUE());
    }

    auto it = dispatchMap.find(pCmdUI->m_nID), ite = dispatchMap.end();
    // id exist and have privilege
    if (it != ite && it->second.second())
    {
        it->second.first(pCmdUI);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}
CSize CDesignerPage::GetCurLayoutSize()
{
	return m_LayoutElement->GetLayoutSize();
}




//zxy 2011/12/21
void CDesignerPage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CBaseControllerPage::OnShowWindow(bShow, nStatus);
	if(bShow)
	{
		#ifndef STARTER_EDITION       
			//CMediaFileBrowser         m_wndStateTab;
			m_wndMediaFileBrowser.m_wndStateTab.m_wndMediaLibrary->OnClickedFilterCategoryRefresh();
		#endif
	}
	// TODO: Add your message handler code here
}

void CDesignerPage:: SetLayerInfo(CS3DrawRectObj* pObj, const CString &szTypeName, const FRect &rectLogic, const PROPERTY_TYPE propertyType, const CString szMediaType, std::shared_ptr<IS3RODesigner2> pDesigner2,
	std::shared_ptr<IS3RODesigner> pDesigner, std::vector<std::pair<CString, CLayoutDesignerCtrl::DragMediaInfo> >* pMedia)
{
	std::shared_ptr<LayerElement> NewLayerElement;
	NewLayerElement = m_LayoutElement->CreateLayer();
    m_LayoutElement->AddTailLayer(NewLayerElement);
	//NewLayerElement->SetLogicPos(rectLogic);
	//NewLayerElement->SetPropertyType(propertyType);
	pObj->SetLayerId(NewLayerElement->GetLayerId());

	//不是backgruund对象，进行类型的引用计数 
	if(propertyType != BACKGROUND)
	{
		DWORD nCount = 0;
		m_ObjTypeCount.Lookup(szTypeName, nCount);
		nCount++;
		m_ObjTypeCount.SetAt(szTypeName, nCount);
        pObj->SetTypeCount(nCount);
		//NewLayerElement->SetTypeCount(nCount);

		CString strSerialName = szTypeName;
		strSerialName.AppendFormat(_T("%d"),nCount);
		pObj->SetSerialName(strSerialName);
	}

	//组件对象或者bkgnd？
    if (pMedia == NULL || pMedia->empty())
    {
        UINT nCount = 0;
        if (propertyType == MESSAGE)
        {
			//建立媒体设置对象（媒体设置面板？）
			std::shared_ptr<MediaElement> NewMediaEle;
			NewLayerElement->CreateMedia(NewMediaEle);
			NewMediaEle->SetMediaName(szTypeName);
			NewMediaEle->SetMediaType(szTypeName_Text);
			NewMediaEle->CreateTextSetting();

			CString mediaFile;
			mediaFile.Format(_T("%s%d"), szTypeName_Message, nCount);
			NewMediaEle->SetMediaFile(mediaFile);
            nCount = 1;

			//设置layer默认的播放时长
            AddLayerDuration(pObj, NewMediaEle->GetDuration());
			NewLayerElement->AddTailMediaElement(NewMediaEle);

        }
	      
		else if (propertyType == LAYER || 
					propertyType == USER_DEFINED_PLUGIN)//用户自定义插件，或者空白组件
        {
            PLUGIN_SUPPORTFILES* pSupportFiles = NULL;
           
            if ((GetControllerApp->FindPluginSupportFile(szMediaType, pSupportFiles) == 0) || szMediaType.CompareNoCase(_T("S3WebBrowser")) == 0)
            {
				//建立媒体设置对象
				std::shared_ptr<MediaElement> NewMediaEle;
				NewLayerElement->CreateMedia(NewMediaEle);
				NewMediaEle->SetMediaName(szMediaType);
				NewMediaEle->SetMediaType(szMediaType);
				NewMediaEle->SetDesigner2(pDesigner2);
				NewMediaEle->SetDesigner(pDesigner);
				NewMediaEle->SetTextSetting(NULL);
                //pMediaSetting->pTextSetting = NULL;

               	CString mediaFile;
#ifdef STARTER_EDITION
				mediaFile.AppendFormat(_T("%s"), szMediaType);
#else				
				mediaFile.AppendFormat(_T("%s%d"), szMediaType, nCount);
#endif
				NewMediaEle->SetMediaFile(mediaFile);
                AddLayerDuration(pObj, NewMediaEle->GetDuration());
				NewLayerElement->AddTailMediaElement(NewMediaEle);

                //customer's curious demand
				if(0x00 == szMediaType.CompareNoCase(_T("Clock")) ||  0x00 == szMediaType.CompareNoCase(_T("S3Clock")))
				{
					pObj->SetKeepAspect(TRUE);
					//NewLayerElement->SetKeepAspect(TRUE);
				}
            }
        }

    }
    else //媒体对象（从媒体库中拖出得对象）
    {
		
        for (int i = 0; i < pMedia->size(); i++)
        {
			std::shared_ptr<MediaElement> NewMediaEle;
			NewLayerElement->CreateMedia(NewMediaEle);

			NewMediaEle->SetMediaFile(pMedia->at(i).first);
			NewMediaEle->SetMediaName(pMedia->at(i).second.MediaName);
			NewMediaEle->SetMediaID(pMedia->at(i).second.MediaID);

            int ContentDuration = pMedia->at(i).second.Duration;
            if(ContentDuration != 0)
            {
				ContentDuration = ContentDuration/1000;
            }
			NewMediaEle->SetDuration(max(1, NewMediaEle->GetDuration()));
            
            if (pMedia->at(i).second.MediaType == MEDIA_URL)
            {
                CString szMyMediaType = _T("S3WebBrowser");

                UINT nCount = 0;
				/*if(m_ComponentObjCount.Lookup(szMyMediaType, nCount)!=0)
					nCount++;
				m_ComponentObjCount.SetAt(szMyMediaType, nCount);*/
               

                HRESULT hr = QueryDesignerWithName(szMyMediaType, pDesigner2, pDesigner);
                if (hr != S_OK)
                {
                    pDesigner2  = NULL;
                    pDesigner   = NULL;
                }
                else
                {
                    if (pDesigner2)
                    {
                        hr = pDesigner2->CreateDesigner(NULL, NULL);
                        if (!SUCCEEDED(hr))
                        {
                            MessageBox(Translate(_T("Error to create plug in object.")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
                            return;
                        }
                    }
                    if (pDesigner)
                    {
                        hr = pDesigner->CreateDesigner(NULL, NULL);
                        if (!SUCCEEDED(hr))
                        {
                            MessageBox(Translate(_T("Error to create plug in object.")), Translate(_T("Error:Designer page")), MB_OK|MB_ICONERROR);
                            return;
                        }
                    }
                }

				NewMediaEle->SetMediaType(szMyMediaType);
				NewMediaEle->SetDesigner2(pDesigner2);
				NewMediaEle->SetDesigner(pDesigner);
                NewMediaEle->SetTextSetting(NULL);
            }
            else
            {
				CString szType;
                QueryMediaType(pMedia->at(i).first, szType);
				NewMediaEle->SetMediaType(szType);
				NewMediaEle->SetDesigner2(pDesigner2);
				NewMediaEle->SetDesigner(pDesigner);
            }

            if (NewMediaEle->GetMediaType().CompareNoCase(szTypeName_Video) == 0)
            {
				NewMediaEle->SetKeepAspect();
                //pMediaSetting->bKeepAspect = TRUE;
            }
            else if (NewMediaEle->GetMediaType().CompareNoCase(_T("S3ImageViewer")) == 0)
            {
				NewMediaEle->SetKeepAspect();
                //pMediaSetting->bKeepAspect = TRUE;
            }

            AddLayerDuration(pObj, NewMediaEle->GetDuration());
			NewLayerElement->AddTailMediaElement(NewMediaEle);
	
        }
    }

	SetLayerInfoByObj(pObj, NewLayerElement);
}

void CDesignerPage:: SetLayerInfoByObj(CS3DrawObj* pObj, std::shared_ptr<LayerElement> layerInfo)
{
	PROPERTY_TYPE propertyType;
	FRect		  rect;
	int			  nCount;
	BOOL		  bLoop;
	CString       szTypeName;
	UINT		  depth;
	CRect		  position;
    CString       szSerialName;

	pObj->GetPropertyType(propertyType);
	pObj->GetLogicPos(rect);
	pObj->GetTypeCount(nCount);
	pObj->GetLoopSync(bLoop);
	pObj->GetName(szTypeName);
	pObj->GetDepth(depth);
	pObj->GetPosition(position);	
    pObj->GetSerialName(szSerialName);

	layerInfo->SetPropertyType(propertyType);
	layerInfo->SetLogicPos(rect);
	layerInfo->SetTypeCount(nCount);
	layerInfo->SetKeepAspect(pObj->GetKeepAspect());
	layerInfo->SetbLoopSync(bLoop);
	layerInfo->SetTypeName(szTypeName);
	layerInfo->SetDepth(depth);
	layerInfo->SetPosition(position);
    layerInfo->SetSerialName(szSerialName);

}


void CDesignerPage:: SetObjInfoByLayerInfo()
{
	list<std::shared_ptr<LayerElement>> layers = m_LayoutElement->GetAllLayers();
	list<std::shared_ptr<LayerElement>> :: iterator it;
	CS3DrawRectObj* pObj = NULL;
	for(it = layers.begin(); it != layers.end(); it++)
	{
		CRect position = (*it)->GetPosition();
		FRect rectLogic = (*it)->GetLogicPos();
		m_wndLayoutDesign.m_wndDesign.LayoutToClient(position);

		PROPERTY_TYPE propertyType = (*it)->GetPropertyType();
		pObj = new CS3DrawRectObj(rectLogic, position, (*it)->GetDepth(), propertyType);
		pObj->SetLayerId((*it)->GetLayerId());

		COLORREF fillColor = RGB(167, 224, 180);
		CString szLayerName = (*it)->GetTypeName();

		if (propertyType == LAYER)
		{
			fillColor = RGB(167, 224, 180);
		}
		else if (propertyType == AUDIO)
		{
			fillColor = RGB(255, 255, 0);
		}
		else if (propertyType == MESSAGE)
		{
			fillColor = RGB(255, 128, 0);
		}
		else if (propertyType == USER_DEFINED_PLUGIN)
		{
			if(m_ObjFillColor.Lookup(szLayerName, fillColor) != TRUE)
			{
				fillColor = RGB(255,0,0);
			}
		}
		pObj->SetFillColor(fillColor);
		pObj->SetName((*it)->GetTypeName());
		pObj->SetKeepAspect((*it)->GetKeepAspect());
        pObj->SetSerialName((*it)->GetSerialName());
		pObj->SetLoopSync((*it)->GetLoopSync());
		DWORD  nCount = 0;
		int	   TypeCount = (*it)->GetTypeCount();
		if( m_ObjTypeCount.Lookup(szLayerName, nCount) != 0)//zxy,Restore the creating index.
		{
			nCount = max(TypeCount, nCount);
			m_ObjTypeCount.SetAt(szLayerName, nCount);
			pObj->SetTypeCount(TypeCount);
		}else
		{
			m_ObjTypeCount.SetAt(szLayerName, TypeCount);
			pObj->SetTypeCount(TypeCount);
		}

		pObj->SetHitTestSize(m_nHandleSize, m_nLineSize);
		m_DrawObjList.AddTail(pObj);

	}
	return;
}

BOOL CDesignerPage:: GetMediaSettingsByObj(CS3DrawObj* pObj, vector<std::shared_ptr<MediaElement>>* &mediaList)
{
	BOOL bFound = FALSE;
	std::shared_ptr<LayerElement> layer = m_LayoutElement->GetLayerInfo(pObj->GetLayerId(), bFound);
	if (bFound && layer.get() != NULL)
	{
		mediaList = &(layer->GetMediaList());
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CDesignerPage:: GetLayerInfoByObj(CS3DrawObj* pObj, std::shared_ptr<LayerElement> &layerInfo)
{
	BOOL bFound = FALSE;
	std::shared_ptr<LayerElement> layer = m_LayoutElement->GetLayerInfo(pObj->GetLayerId(), bFound);
	if (bFound && layer.get() != NULL)
	{
		layerInfo = layer;
		//m_LayoutElement->RemoveLayer(pObj->GetLayerId(),);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CDesignerPage:: AddLayerIntoLayout(CS3DrawObj * pInstance, std::shared_ptr<LayerElement> &layerInfo)
{
	BOOL bFound = FALSE;
	std::shared_ptr<LayerElement> layer = m_LayoutElement->GetLayerInfo(pInstance->GetLayerId(), bFound);
	if (bFound && layer.get() != NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	ASSERT(pInstance->GetLayerId() == layerInfo->GetLayerId());
	m_LayoutElement->AddTailLayer(layerInfo);
	return TRUE;

}

void CDesignerPage::AdjustLayerInfoBySortedObjs(BOOL bRevert)
{

    PROPERTY_TYPE propertyType;
    list<std::shared_ptr<LayerElement>> shpLayerArray = m_LayoutElement->GetAllLayers();

    m_LayoutElement->RemoverAllLayers();
    POSITION pos = m_DrawObjList.GetHeadPosition();
    while (pos != NULL)
    {
        CS3DrawObj* pObj = m_DrawObjList.GetNext(pos);
        pObj->GetPropertyType(propertyType);
        if (propertyType == BACKGROUND)
        {
            continue;
        }
        std::shared_ptr<LayerElement> NewLayerElement;
        NewLayerElement = m_LayoutElement->CreateLayer();
        if (bRevert)
        {
            m_LayoutElement->AddHeadLayer(NewLayerElement);
        }
        else
        {
            m_LayoutElement->AddTailLayer(NewLayerElement);
        }

        NewLayerElement->SetLayerId(pObj->GetLayerId());
        SetLayerInfoByObj(pObj, NewLayerElement);

        list<std::shared_ptr<LayerElement>> :: iterator it;
        for (it = shpLayerArray.begin(); it != shpLayerArray.end(); it++)
        {
            if((*it)->GetLayerId() == NewLayerElement->GetLayerId())
            {
                NewLayerElement->SetMediaList((*it)->GetMediaList());
                break;
            }
        }

    }
    return;
}