#pragma once


// CMFCButtonEx

class CMFCButtonEx : public CMFCButton
{
	DECLARE_DYNAMIC(CMFCButtonEx)

public:
	CMFCButtonEx();
	virtual ~CMFCButtonEx();
	afx_msg LRESULT OnInitControl(WPARAM,LPARAM);
protected:
	DECLARE_MESSAGE_MAP()
};


