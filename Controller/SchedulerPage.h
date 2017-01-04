#pragma once

#include "CalenderPane.h"
#include "SchedulerPane.h"
#include "ScheduleItemPane.h"
#include "LayoutCategoryPane.h"
#include "TimeTableListPane.h"
#include "SchedulerItemSelectPane.h"
#include "GroupListPane.h"
#include "Controller.h"

// CSchedulerPage

class CSchedulerPage : public CBaseControllerPage
{
    friend class CControllerFrame;

protected:
    DECLARE_DYNAMIC(CSchedulerPage)

public:
    CSchedulerPage();
    virtual ~CSchedulerPage();

public:
    BOOL UpdateOptions();

    // Close the main scheduler window.
    void Close();

    // Create the ribbon buttons
    BOOL CreateRibbonButtons();

	// Refresh client group
	BOOL RefreshClientGroup();

    /// Init all sub controls
    BOOL InitSubControls();

    void SetDateInterval (const COleDateTime& date1, const COleDateTime& date2);

    HRESULT DownloadMediaFiles(CString MediaServerFolder, CString LocalFolder);

    void OnUpdateRibbonBtnPreview(CCmdUI* pCmdUI);

    static void DeleteDir(CString aimPath);

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);


    afx_msg void OnRibbonBtn_Go();
    afx_msg void OnRibbonBtn_Save();
    afx_msg void OnRibbonBtn_Print();
    afx_msg void OnRibbonBtn_Export();
    afx_msg void OnRibbonBtn_Import();
    afx_msg void OnRibbonBtn_SetDefault();
    afx_msg void OnRibbonBtn_RemoveDefault();
    afx_msg void OnRibbonBtn_Copy();
    afx_msg void OnRibbonBtn_Paste();
    afx_msg void OnRibbonBtn_Delete();
    afx_msg void OnRibbonBtn_HMultiCopy();
    afx_msg void OnRibbonBtn_VMultiCopy();
    afx_msg void OnRibbonBtn_MultiDelete();
    afx_msg void OnRibbonBtn_Preview();
    afx_msg void OnRibbonBtn_AlignUp();
    afx_msg void OnRibbonBtn_AlignDown();
    afx_msg void OnRibbonBtn_TimeDelta();
#ifdef STARTER_EDITION
	afx_msg void OnRibbonBtn_PlayerSetting();
#endif

#ifdef STANDALONE_EDITION
	afx_msg void OnRibbonBtn_AddGroup();
	afx_msg void OnRibbonBtn_DeleteGroup();
	afx_msg void OnRibbonBtn_EditGroup();
#endif

    afx_msg void OnRibbonBtn_Preview10X7();
    afx_msg void OnRibbonBtn_Preview12X10();
    afx_msg void OnRibbonBtn_Preview19X12();

    afx_msg void OnUpdateRibbonBtnPrivilege(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtn(CCmdUI* pCmdUI);
    DECLARE_MESSAGE_MAP()


    // My page logic message handler
    afx_msg LRESULT OnScheduleItemUpdate (WPARAM, LPARAM);
    afx_msg LRESULT OnScheduleItemUpdateDuration (WPARAM, LPARAM);
    afx_msg LRESULT OnSelectScheduleItemChange (WPARAM, LPARAM);
    afx_msg LRESULT OnSelChanged (WPARAM, LPARAM);
    afx_msg LRESULT OnScheduleKeyDown (WPARAM, LPARAM);

    afx_msg LRESULT OnScheduleCtrlAppointmentChanged (WPARAM, LPARAM);
    afx_msg LRESULT OnScheduleCtrlAppointmentPreChange (WPARAM, LPARAM);

    afx_msg LRESULT OnScheduleCtrlTimeDeltaChanged (WPARAM, LPARAM);
    afx_msg LRESULT OnSchedulerTabActiveChange (WPARAM, LPARAM);


    afx_msg void OnUpdateRibbonBtnNeedSelect(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnNeedSingleSelect(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnPreviewLayout(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnStarterEditionRun(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnSave(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnRemoveDefault(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonBtnImport(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonBtnExport(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonBtnPaste(CCmdUI* pCmdUI);

	afx_msg void OnGroupScheduleEdit(NMHDR *pNMHDR, LRESULT *pResult);

public:
    afx_msg void OnDestroy();


    CString GetLayoutExportLocation(INT LayoutID);
private:

    BOOL InitPage(INT GroupID);

    void BeginApendSchedule();
    void EndApendSchedule();

    BOOL UpdateSchedule(COleDateTime Start, COleDateTime End);

    // Preview layout
    HRESULT PreviewLayout(const int nWidth, const int nHeight);

#ifdef STANDALONE_EDITION
	BOOL Refresh();
	void OnUpdateRibbonBtnNormal(CCmdUI* pCmdUI);
	void OnUpdateGroupSingleSelectRibbonBtn( CCmdUI* pCmdUI );
	void OnUpdateGroupRibbonBtn( CCmdUI* pCmdUI );
	BOOL isGroupExist();
#endif

protected:
    // Pane
    CCalenderPane            m_wndCalender;
    CScheduleItemPane        m_wndScheduleItem;
    CSchedulerItemSelectPane m_wndSchedulerItemSelect;
	CGroupListPane           m_wndGroupList;

    CSchedulerPane           m_wndScheduer;

    CMFCRibbonComboBox*      m_pComboBoxTimeDelta;

#ifdef STARTER_EDITION
	CMFCRibbonComboBox*      m_pComboBoxPlayerRotation;
#endif
    // the following is for layout lists


    BOOL                     m_bAppendingSchedule;

    // Used for updating schedule, addappointment
    CImageList               m_ScheduleItemIconList;
    typedef CMap<UINT, UINT, UINT, UINT>     CMapImageIndex;
    CMapImageIndex           m_IconDetailIDMapper;

public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};


