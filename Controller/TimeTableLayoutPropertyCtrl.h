#pragma once
#include "Resource.h"
#include "atlcomtime.h"
#include "Appointment.h"
#include "setdpi.h"
#include "amsedit.h"
// wParam - not used, lParam - not used
extern UINT TIMETABLE_ITEM_PROPERTY_CHANGE;

// CTimeTableLayoutPropertyCtrl dialog

class CTimeTableLayoutPropertyCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CTimeTableLayoutPropertyCtrl)
	DECLARE_EASYSIZE

public:
	CTimeTableLayoutPropertyCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTimeTableLayoutPropertyCtrl();

    void ClearItem();
    void SetItem(Appointment* pApp);

// Dialog Data
	enum { IDD = IDD_TIMETABLE_SELECTED_LAYOUT_INFO };

protected:
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual void OnCancel(){}
    virtual void OnOK(){}
	DECLARE_MESSAGE_MAP()

private:
	CString  m_LayoutName;
	CString  m_Category;

    Appointment m_App;
    COleDateTimeSpan m_OrigDuration;
    CSetDPI m_dpi;
	CAMSNumericEdit m_LoopEdit;
public:
    COleDateTime m_DurationTime;
    float m_fLoopTimes;
    afx_msg void OnDtnDatetimechangeEditTimetableLayoutDurationTime(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEnChangeEditTimetableLayoutLooptimes();
};
