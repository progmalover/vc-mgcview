// F:\p4\sw\Demo\ContentManagement\Controller\User Management\UserEditorPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "UserEditorPane.h"


// CUserEditorPane

IMPLEMENT_DYNAMIC(CUserEditorPane, CBaseControllerPane)

CUserEditorPane::CUserEditorPane()
{

}

CUserEditorPane::~CUserEditorPane()
{
}


BEGIN_MESSAGE_MAP(CUserEditorPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CUserEditorPane message handlers
int CUserEditorPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_UserEditor.Create(CUserPrivilegeEditDlg::IDD,this);
    m_UserEditor.ShowWindow(SW_SHOW);

    SetMainWnd(&m_UserEditor);
    SetMinSize(CSize(350, 365));

    return 0;
}


void CUserEditorPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}


VOID CUserEditorPane::SetCurrentUser(int UserID)
{
    m_UserEditor.SetCurrentUser(UserID);
}