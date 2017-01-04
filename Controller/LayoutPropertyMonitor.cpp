#include "StdAfx.h"
#include "controller.h"
#include "DesignerPage.h"
#include "LayoutDesignerCtrl.h"
#include "LayoutPropertyMonitor.h"
#include "LayoutDesignerCmds.h"
#include "PropertySubCmds.h"

using namespace LAYOUTPROPERTYCMDS;
extern UINT WM_ON_MEDIA_ITEM_CHANGE;

extern UINT WM_SET_LAYOUT_COLOR;
extern UINT WM_LAYOUT_TEXT_CHANGE;
extern UINT WM_ON_LAYER_NAME_CHANGE;

extern UINT WM_ON_LAYOUT_SIZE_CHANGE_BEGIN;
#define WM_ON_LAYOUT_SIZE_CHANGE_END WM_ON_LAYOUT_SIZE_CHANGE

extern UINT WM_MEDIASETTING_UPDATE_BEGIN ;
extern UINT WM_MEDIASETTING_UPDATE_END;
extern UINT WM_TRANS_UPDATE_BEGIN;
extern UINT WM_TRANS_UPDATE_END;
extern UINT WM_MEDIA_BASICINFO_UPDATE_BEGIN ;
extern UINT WM_MEDIA_BASICINFO_UPDATE_END;
extern UINT WM_ON_LAYER_KEEP_ASPECT_CHANGE;
#define _USE_LAYOUT_COLOR_CHANGE_CMD
namespace DESIGNERCMDS
{
	IMPLEMENT_SINGLETON(CLayoutPropertyMonitor)
		
	//LRESULT (CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
	static LRESULT MonitorProc(HWND hWnd ,UINT dwMsg,WPARAM wParam,LPARAM lParam)
	{
		CWnd * pWnd = CWnd::FromHandle(hWnd);
		return CLayoutPropertyMonitor::Instance()->HandleMsg(pWnd,dwMsg,wParam,lParam);
	}

	CLayoutPropertyMonitor::CLayoutPropertyMonitor(void)
	{
		m_bUpdated = FALSE;// tag to update status.
	}


	CLayoutPropertyMonitor::~CLayoutPropertyMonitor(void)
	{

	}

	void CLayoutPropertyMonitor::AddPropertyWnd( CWnd *pWnd)
	{
		ASSERT(pWnd);
		ASSERT(::IsWindow(pWnd->GetSafeHwnd()));
	//	return;
		WND_HANDLES::iterator it = m_Wnds.find(pWnd);
		if( it != m_Wnds.end())
			return;

		WND_HANDLE wndHandle;
		wndHandle.first = pWnd; 
		wndHandle.second.orgProc=(WNDPROC) SetWindowLongPtr(pWnd->GetSafeHwnd(),GWLP_WNDPROC,(LONG_PTR)MonitorProc);
		m_Wnds.insert(wndHandle);

#if 0   // i can't catch message that i wanded ,and some error occured when i hook the control,

        //
		//set hook to controls ,try to catch some messages like : WM_SETFOCUS,WM_KILLFOCUS
		//
		CWnd *pSubWnd = pWnd->GetWindow(GW_CHILD);
		while(pSubWnd)
		{
			
			TCHAR strClassName[256]={0};
			GetClassName(pSubWnd->GetSafeHwnd(),strClassName,255);
			if(StrCmpI(strClassName,_T("Edit")) == 0
				||StrCmpI(strClassName,_T("ListBox")) == 0
				||StrCmpI(strClassName,_T("SysListView32"))== 0)
			{
				
				//wndHandle.first = pSubWnd; 
				WNDPROC second =(WNDPROC) SetWindowLong(pSubWnd->GetSafeHwnd(),GWL_WNDPROC,(long)MonitorProc);
				m_Wnds.insert(make_pair(pSubWnd,second));
			}

			pSubWnd = pSubWnd->GetNextWindow(GW_HWNDNEXT);
		}
#endif
	}

	//
	//消息处理 
	//
	LRESULT CLayoutPropertyMonitor::HandleMsg(CWnd *pProperty,DWORD uMsg,WPARAM wParam,LPARAM lParam)
	{
		//TRACE(_T("UMSG :0x%x\n"),uMsg);
		LRESULT lRet = -1;
		WND_HANDLES::iterator it = m_Wnds.find(pProperty);
		if(it == m_Wnds.end())
			return ::DefWindowProc(pProperty->GetSafeHwnd(),uMsg,wParam,lParam);
     
        //
        //when WM_DESTROY message coming,Process it ,and remove wnd obj from m_wnds.
        //
        if(uMsg == WM_DESTROY || uMsg == WM_NCDESTROY)
        {
            lRet = CallWindowProc(it->second.orgProc,pProperty->GetSafeHwnd(),uMsg,wParam,lParam);
            ::SetWindowLongPtr(pProperty->GetSafeHwnd(),GWLP_WNDPROC,(LONG_PTR)it->second.orgProc);
             m_Wnds.erase(it);

             return lRet;
        }

		DetectUpdateBegin(it->first,uMsg,wParam,lParam);
		//send message to target wndproc,before "DetectUpdateEnd"
        lRet = CallWindowProc(it->second.orgProc,pProperty->GetSafeHwnd(),uMsg,wParam,lParam);
		
        DetectUpdateEnd();
		MakeUpdateCmd();

		return lRet;
		// 
	}
};

void  CLayoutPropertyMonitor::DetectUpdateBegin(CWnd *pWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_COMMAND:
			break;
		case WM_NOTIFY:
			{
				LPNMHDR pNmHdr = (LPNMHDR) lParam;
				TCHAR strClassName[256]={0};
				GetClassName(pNmHdr->hwndFrom,strClassName,255);
				//LVN_KEYDOWN
				if(pNmHdr->code == LVN_KEYDOWN )
				{
					//int ddd = 30;
				}
			//	TRACE(_T("UMSG :WM_NOTIFY: %s,code:0x%x\n"),strClassName,pNmHdr->code);
			}
		case WM_LBUTTONDOWN:
			{
				//TRACE(_T("UMSG :WM_LBUTTONDOWN\n"));
			}
			break;
		case WM_SHOWWINDOW:
			{
			//	TRACE(_T("UMSG :WM_SHOWWINDOW\n"));
#if 0
				if(wParam == SW_HIDE)
				{
					WND_HANDLES::iterator it = this->m_Wnds.find(pWnd);
					if( it != m_Wnds.end())
					{

					}
				}
#endif
			}
			break;
	}  

	//must get these handles at runing time,because,this function maybe call in several designer wnd instance.
	CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
	if(NULL == pPage)
		return;
	CLayoutDesignerCtrl *pDesigner = (CLayoutDesignerCtrl *)SearchWndObj(pPage,RUNTIME_CLASS(CLayoutDesignerCtrl));
	if(NULL == pDesigner)
		return;
	LayoutCmdEngine *pEngine = pDesigner->GetCmdEngine();
	LayoutCmdFactory *pFactory = pEngine->getFactory();

    //
    //message that needed delayed.
    //
	if(uMsg == WM_ON_LAYOUT_SIZE_CHANGE_BEGIN || uMsg == WM_ON_LAYOUT_SIZE_CHANGE_END ||
        uMsg == WM_MEDIASETTING_UPDATE_BEGIN || uMsg == WM_MEDIASETTING_UPDATE_END ||
        uMsg == WM_TRANS_UPDATE_BEGIN || uMsg == WM_TRANS_UPDATE_END ||WM_MEDIA_BASICINFO_UPDATE_BEGIN
        ||WM_MEDIA_BASICINFO_UPDATE_END)
	{
        properLayoutCmd *pCmd = NULL;
        if(uMsg == WM_ON_LAYOUT_SIZE_CHANGE_BEGIN ||
           uMsg == WM_MEDIASETTING_UPDATE_BEGIN||
           uMsg == WM_TRANS_UPDATE_BEGIN 
           ) //start ...
        {
		    pCmd = (properLayoutCmd *)pFactory->newCmdBegin(CMD_PROPERLAYOUT);
            ASSERT(pCmd != NULL);
            actionsLayoutCmd *pSubCmd = new actionsLayoutCmd(CMD_PROPERACTIONS,pDesigner);
            pSubCmd->nMessage = uMsg;
            pSubCmd->dwParam =  wParam;
            pSubCmd->dwLParam = lParam;
            pSubCmd->BeginPrepareStatus();
            pCmd->addSubCmd(pSubCmd);
        }
        else if( uMsg == WM_ON_LAYOUT_SIZE_CHANGE_END ||
            uMsg == WM_MEDIASETTING_UPDATE_END ||
            uMsg == WM_TRANS_UPDATE_END
            ) //2:end size change.
        {
            _baseCmd *pBaseCmd = pFactory->newCmdEnd();
           // ASSERT(pBaseCmd);
            if(NULL == pBaseCmd)
                return;
            if(pBaseCmd->isObjOf(CMD_PROPERLAYOUT))
                pCmd = (properLayoutCmd *)pBaseCmd;
            ASSERT(pCmd);
            SUB_CMDS &cmds = pCmd->getSubCmds();
            ((actionsLayoutCmd*)cmds[cmds.size()-1])->nMessage = uMsg;
            ((actionsLayoutCmd*)cmds[cmds.size()-1])->dwParam = wParam;
            ((actionsLayoutCmd*)cmds[cmds.size()-1])->dwLParam = lParam;
            ((actionsLayoutCmd*)cmds[cmds.size()-1])->EndPrepareStatus();

            if(!pEngine->execCmd(pCmd))
                pFactory->delCmd(pCmd);
        }
       
	}

    if(uMsg == WM_MEDIA_BASICINFO_UPDATE_BEGIN || uMsg == WM_MEDIA_BASICINFO_UPDATE_END)
    {
        properInstCmd *pCmd = NULL;
        if(uMsg == WM_MEDIA_BASICINFO_UPDATE_BEGIN)
        {
            pCmd = (properInstCmd *)pFactory->newCmdBegin(CMD_PROPERINST);
            ASSERT(pCmd != NULL);
		    actionsInstCmd *pSubCmd = new actionsInstCmd(0,pDesigner);
		    pSubCmd->nMessage = uMsg;
		    pSubCmd->dwWParam = wParam;
		    pSubCmd->dwLParam = lParam;
            pSubCmd->BeginPrepareStatus();
		    //pSubCmd->pDrawObj = (CS3DrawObj *)lParam;
		    pCmd->addSubCmd(pSubCmd);
		   
        }else if(uMsg == WM_MEDIA_BASICINFO_UPDATE_END)
        {
               _baseCmd *pBaseCmd = pFactory->newCmdEnd();
           // ASSERT(pBaseCmd);
            if(NULL == pBaseCmd)
                return;
            if(pBaseCmd->isObjOf(CMD_PROPERINST))
                pCmd = (properInstCmd *)pBaseCmd;
            ASSERT(pCmd);
            SUB_CMDS &cmds = pCmd->getSubCmds();
            ((actionsInstCmd*)cmds[cmds.size()-1])->nMessage = uMsg;
            ((actionsInstCmd*)cmds[cmds.size()-1])->dwWParam = wParam;
            ((actionsInstCmd*)cmds[cmds.size()-1])->dwLParam = lParam;
            ((actionsInstCmd*)cmds[cmds.size()-1])->EndPrepareStatus();

            if(!pEngine->execCmd(pCmd))
                pFactory->delCmd(pCmd);
        }
    }
    //
    //message from "propertyLayoutCtrl"
    //
    if(uMsg ==WM_LAYOUT_TEXT_CHANGE
#ifdef _USE_LAYOUT_COLOR_CHANGE_CMD
        || uMsg == WM_SET_LAYOUT_COLOR
#endif
        )
    {
         properLayoutCmd *pCmd = (properLayoutCmd *)pFactory->newCmd(CMD_PROPERLAYOUT);
        ASSERT(pCmd != NULL);
        actionsLayoutCmd *pSubCmd = new actionsLayoutCmd(CMD_PROPERACTIONS,pDesigner);
        pSubCmd->nMessage = uMsg;
        pSubCmd->dwParam = wParam;
        pSubCmd->BeginPrepareStatus();
        pCmd->addSubCmd(pSubCmd);
        
        if(!pEngine->execCmd(pCmd))
            pFactory->delCmd(pCmd);
    }


    //
    //message from "propertyLayerCtrl"
    //
	if(uMsg == WM_ON_SIZE_POS_CHANGE2||uMsg == WM_ON_MEDIA_NUM_CHANGE 
		||uMsg == WM_ON_MEDIA_ITEM_CHANGE || uMsg == WM_ON_LAYER_NAME_CHANGE||
				uMsg == WM_ON_LAYER_LOOP_CHANGE ||uMsg == WM_ON_LAYER_KEEP_ASPECT_CHANGE)
	{
		properInstCmd *pCmd = (properInstCmd *)pFactory->newCmd(CMD_PROPERINST);
		actionsInstCmd *pSubCmd = new actionsInstCmd(0,pDesigner);
		pSubCmd->nMessage = uMsg;
		pSubCmd->dwWParam = wParam;
		pSubCmd->dwLParam = lParam;
		//pSubCmd->pDrawObj = (CS3DrawObj *)lParam;
		pCmd->addSubCmd(pSubCmd);
		if(!pEngine->execCmd(pCmd))
			pFactory->delCmd(pCmd);
	}

}

int   CLayoutPropertyMonitor::DetectUpdateEnd()
{
	return SUCCESS;
}
//struction for snapshot functon.
void  CLayoutPropertyMonitor::MakeUpdateCmd()
{
	if(FALSE == this->m_bUpdated)
		return;

	//Make cmd instance here.

	m_bUpdated = FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//context format: name=val;name2=val;name3=val.....
//this code is designed for "snapshot mode" undo/redo system,isn't used.

StatusContext::StatusContext(){}
StatusContext::~StatusContext(){}
void StatusContext::AttachContext(Context_Map &context)
{
	m_mapContext.erase(m_mapContext.begin(),m_mapContext.end());
	Context_Map::iterator it = context.begin();
	for(;it!=context.end();it++)
	{
		m_mapContext.insert(make_pair(it->first,it->second));
	}
}

void StatusContext::SetContext(CString strName,CString strVal)
{
	Context_Map::iterator it  = m_mapContext.find(strName);
	if(it != m_mapContext.end())
	{
		it->second = strVal;
		return;
	}

	m_mapContext.insert(make_pair(strName,strVal));
}

CString StatusContext:: GetContextVal(CString strName)
{
	Context_Map::iterator it  = m_mapContext.find(strName);
	if(it != m_mapContext.end())
		return it->first;

	return _T("");
}
