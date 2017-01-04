#pragma once
#include "Resource.h"
#include "afxwin.h"
#include "Controller.h"
#include "InfoDisplayCtrl.h"

// CTimeTablePropertyEditCtrl dialog

class CTimeTablePropertyEditCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CTimeTablePropertyEditCtrl)
	DECLARE_EASYSIZE

public:
	CTimeTablePropertyEditCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTimeTablePropertyEditCtrl();
    void SetTimeTable(const Timetable& Table);
    void ClearTimeTable();


// Dialog Data
	enum { IDD = IDD_TIMETABLE_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

    virtual void OnCancel();
    virtual void OnOK();

private:
    CInfoDisplayCtrl     m_Info;

public:                        
    CString     m_szRejectReason;
    CButton     m_ctrlRejectReason;
    afx_msg void OnBnClickedBtnRejectReason();
};
