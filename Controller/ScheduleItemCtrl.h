#pragma once
#include "Appointment.h"
#include "ScheduleCtrl.h"
#include "afxdtctl.h"
#include "DurationTimeCtrl.h"
// wParam - not used, lParam - not used
extern UINT SCHEDULE_ITEM_UPDATE;

// wParam - not used, lParam - not used
extern UINT SCHEDULE_ITEM_UPDATE_DURATION;

// CScheduleItemCtrl dialog

class CScheduleItemCtrl : public CDialogEx
{
    DECLARE_DYNAMIC(CScheduleItemCtrl)
    DECLARE_EASYSIZE
public:
    CScheduleItemCtrl(CWnd* pParent = NULL);   // standard constructor
    virtual ~CScheduleItemCtrl();

    void SetItemList(std::vector<Appointment*>& AppList);
    std::vector<Appointment*>& GetScheduleItems(){return m_AppList;}
    const COleDateTime&  GetStartDate(){return m_tStartDate;}
    const COleDateTime&  GetEndDate(){return m_tEndDate;}  
    const COleDateTimeSpan&  GetDuration(){return m_tDuration;}

// Dialog Data
    enum { IDD = IDD_SCHEDULE_ITEM };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);

    DECLARE_MESSAGE_MAP()
    virtual void OnOK() {}
    virtual void OnCancel() {}

public:
    afx_msg void OnDtnDatetimechangeDatetimepickerStart(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDtnDatetimechangeDatetimepickerEnd(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDtnDatetimechangeDatetimepickerDuration(NMHDR *pNMHDR, LRESULT *pResult);

private:
    COleDateTime   m_tStartDate;   
    COleDateTime   m_tEndDate;   
    COleDateTimeSpan   m_tDuration; 

    CEdit          m_strStaticInfo;

    std::vector<Appointment*>   m_AppList;

public:
    CDateTimeCtrl m_StartTime;
    CDateTimeCtrl m_EndTime;
    CDurationTimeCtrl m_Duration;
};
