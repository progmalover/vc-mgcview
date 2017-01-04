#pragma once
#include "UserPrivilegeEditDlg.h"

// CUserEditorPane

class CUserEditorPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CUserEditorPane)

public:
    CUserEditorPane();
    virtual ~CUserEditorPane();
    VOID SetCurrentUser(int UserID);

    CUserPrivilegeEditDlg m_UserEditor;
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


