#pragma once
#include "BaseControllerPage.h"
#include "TimeTableCtrl.h"
#include "Controller.h"

#include <set>

extern UINT TIMETABLE_TAB_BEFORE_CLOSE;

class CTimeTableTab : public CMFCTabCtrl
{
    DECLARE_DYNAMIC(CTimeTableTab)

public:
    CTimeTableTab();
    virtual ~CTimeTableTab();

    BOOL AddTimeTable(Timetable& Table, BOOL bForceAdd = FALSE, BOOL bReadOnly = FALSE);      
    
    BOOL GetTheTimeTable(Timetable& Table);
    BOOL SetTheTimeTable(const Timetable& Table);
    BOOL GetTabTimeTable(INT iTab, Timetable& Table);
    BOOL GetAllModifiedTimeTable(vector<pair<CWnd*,Timetable> >& Tables);
    BOOL SetWndTimeTable(const CWnd*, Timetable);
    BOOL IsTimetableModified();
    Audit::Status GetTheTimeTableStatus();
    void SetTimetableAudit(const Audit& audit);
    BOOL CloseTheTimeTable();      
    void SetTimeDelta(const int& Delta);
    int  GetTimeDelta();
    BOOL IsTimeTableOpend(INT TableID);
    BOOL CloseTimeTable(INT TableID);
    BOOL IsTimetableModified(INT TableID);
    BOOL GetTimeTable(INT TableID, Timetable& Table);
    void ChangeCurLabel(const CString& newLabel);
    void ChangeWndLabel(const CWnd*, const CString& newLabel);

    CTimeTableCtrl* GetTheCtrl();

    std::map<CWnd*, Timetable> m_wndTables;

    virtual BOOL RemoveTab(int iTab, BOOL bRecalcLayout = TRUE);

    void SetCurTabModified();
    void ClearCurTabModified();
    void ClearTableModified(INT TableID);
    void ClearTabModified(int iTab);
    BOOL IsTablModified(int iTab);
    void ClearAllTabModified();

	BOOL CheckTimeTableExist(CString TableName, int *pIndex);
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    virtual void FireChangeActiveTab(int nNewTab);

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);


    // Used for updating schedule, addappointment
    CImageList              m_ItemIconList;
    typedef CMap<UINT, UINT, UINT, UINT>     CMapImageIndex;
    CMapImageIndex          m_IconDetailIDMapper;
};

// CClientStatePane

class CTimeTablePane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CTimeTablePane)

public:
    CTimeTablePane();
    virtual ~CTimeTablePane();

    CTimeTableTab         m_wndTab;
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();
};