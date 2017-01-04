#pragma once
#include "GroupInfoCtrlNew.h"

// CGroupInfoPane

class CGroupInfoPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CGroupInfoPane)

public:
    CGroupInfoPane();
    virtual ~CGroupInfoPane();

    CGroupInfoCtrlNew& GetGroupInfo(){return m_GroupInfo;}

public:
    CGroupInfoCtrlNew  m_GroupInfo;


protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


