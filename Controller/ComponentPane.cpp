// F:\p4\sw\Demo\ContentManagement\Controller\Designer\Component.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ComponentPane.h"


// CComponentPane

IMPLEMENT_DYNAMIC(CComponentPane, CBaseControllerPane)

CComponentPane::CComponentPane()
{

}

CComponentPane::~CComponentPane()
{
}


BEGIN_MESSAGE_MAP(CComponentPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CComponentPane message handlers
int CComponentPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    //CRect rectDummy (0, 0, 0, 0);
    //const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | TVS_HASLINES | 
    //    TVS_LINESATROOT | TVS_HASBUTTONS;

    //m_wndComponentList.Create(dwViewStyle, rectDummy, this, 1);


    return 0;
}



void CComponentPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

