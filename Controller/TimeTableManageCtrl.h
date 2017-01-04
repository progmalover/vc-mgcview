#pragma once

#include "Resource.h"
// CTimeTableManageCtrl dialog

class CTimeTableManageCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CTimeTableManageCtrl)
	DECLARE_EASYSIZE

public:
	CTimeTableManageCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTimeTableManageCtrl();

// Dialog Data
	enum { IDD = IDD_TIMETABLE_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};
