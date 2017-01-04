#pragma once
#include "TimeTablePropertyEditPane.h"
#include "TimeTableLayoutPropertyPane.h"
#include "TimeTablePane.h"
#include "LayoutListPane.h"
#include "TimeTableListPane.h"
#include "LayoutCategoryPane.h"
#include <vector>

class CTimeTablePage : public CBaseControllerPage
{
	friend class CControllerFrame;

protected:
	DECLARE_DYNAMIC(CTimeTablePage)

public:
	CTimeTablePage(void);
	virtual ~CTimeTablePage(void);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

    // Preview layout
    HRESULT PreviewLayout(const int nWidth, const int nHeight);

    void OnRibbonBtnPreviewWinMode();
    void OnRibbonBtnPreview10X7();
    void OnRibbonBtnPreview12X10();
    void OnRibbonBtnPreview19X12();
    void OnRibbonBtnCreate();
    void OnRibbonBtnClose();
    void OnRibbonBtnPublish();
    void OnRibbonBtnCopy();
    void OnRibbonBtnPaste();
    void OnRibbonBtnDel();
    void OnRibbonBtnZoom();
    void OnRibbonBtnRequest();
    void OnRibbonBtnReject();
    void OnRibbonBtnApprove();
    void OnRibbonBtnEdit();
    void OnRibbonBtnPublishAll();
    void OnRibbonBtnDeleteTimeTable();
    void OnRibbonBtnRename();

    afx_msg void OnUpdateRibbonBtn(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnDefault(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnPreview(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnCreate(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnClose(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnPublish(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnPaste(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnDel(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnZoom(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnRequest(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnReject(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnApprove(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnEdit(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnPublishAll(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnRename(CCmdUI* pCmdUI);

    afx_msg LRESULT OnTimeTableCategorySelChange (WPARAM, LPARAM);
    afx_msg LRESULT OnTimeTableCategoryDbClick (WPARAM, LPARAM);
    afx_msg LRESULT OnTimeTableItemSelChange (WPARAM, LPARAM);
    afx_msg LRESULT OnTimeTableItemPropertyChange (WPARAM, LPARAM);
    afx_msg LRESULT OnTimeTableTabActiveChange (WPARAM, LPARAM);
    afx_msg LRESULT OnTimeTableBeforeTabClose (WPARAM, LPARAM);
	afx_msg LRESULT OnTimeTableAfterUnitDelete(WPARAM,LPARAM);
    afx_msg LRESULT OnTimeTableKeyDown (WPARAM, LPARAM);
    afx_msg LRESULT OnTimeTableTimedeltaChanged (WPARAM, LPARAM);

    void PublishTab(INT iTab);

    BOOL OpenTimetable(INT TimetableID);
    BOOL CloseCurTimetable();
    void CloseTimeTable(INT TableID, bool popMsgBox = true);

protected:
	BOOL CreateRibbonButtons();
	BOOL UpdateOptions();

	void Close();

private:
	CTimeTablePropertyEditPane         m_TimeTablePropertyEditPane;
	CTimeTableLayoutPropertyPane       m_TimeTableLayoutPropertyPane;
	CTimeTablePane                     m_TimeTablePane;
    CTimeTableListPane                 m_TimeTableListPane;
    CLayoutCategoryPane                m_TimeTableLayoutsPane;

    CMFCRibbonComboBox*                m_pComboBoxTimeDelta;

    std::map<INT, Timetable>           m_TimetableCacheMap;
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

