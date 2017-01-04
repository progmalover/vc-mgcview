#pragma once
#include "Controller.h"
#include "afxcmn.h"

class CSchedulesSelectDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CSchedulesSelectDlg)
    DECLARE_EASYSIZE
public:
    CSchedulesSelectDlg(CWnd* pParent, const std::vector<ClientGroup>& Groups);   // standard constructor
    virtual ~CSchedulesSelectDlg();

    BOOL InitClientGroups(const std::vector<ClientGroup>& Groups);

    INT GetSelectGroupID();

// Dialog Data
    enum { IDD = IDD_SCHEDULES };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnHdnItemclickListSchedules(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickListSchedules(NMHDR *pNMHDR, LRESULT *pResult);

    DECLARE_MESSAGE_MAP()

private:
    CListCtrl m_GroupsList;
    std::vector<ClientGroup> m_Groups;
    INT m_SelectedGroupID;

};
