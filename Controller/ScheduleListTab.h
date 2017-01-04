#pragma once


// CScheduleListTab
class ScheduleCtrl;
class CScheduleListTab : public CMFCTabCtrl
{
	DECLARE_DYNAMIC(CScheduleListTab)

public:
	CScheduleListTab();
	virtual ~CScheduleListTab();

public:
	BOOL OpenScheduleTag(int groupID);
	int GetGroupID();
	void RemoveScheduleTag();
	ScheduleCtrl* GetScheduleCtrl();
    void AddNewScheduleCtrl(ScheduleCtrl* pSC, int groupID); 

    int GetTimeDelta();
    void SetTimeDelta(int TimeDelta);

protected:
	virtual BOOL RemoveTab(int iTab, BOOL bRecalcLayout = TRUE);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void FireChangeActiveTab(int nNewTab);
	ScheduleCtrl* FindWnd(int groupID);


private:
	std::map<ScheduleCtrl*, int> m_scheduleCtrlMap;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);	
	DECLARE_MESSAGE_MAP()
};


