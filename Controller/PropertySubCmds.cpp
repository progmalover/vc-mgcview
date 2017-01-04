#include "stdafx.h"
#include "Controller.h"
#include "LayoutDesignerCmds.h"
#include "PropertySubCmds.h"
#include "LayoutPropertyMonitor.h"
#include "designerpage.h"
#include "layoutdesignerctrl.h"
#include "propertylayer.h"
#include "propertylayout.h"
#include "propertymessage.h"
#include "propertyplugin.h"
#include "propertysimpleplugin.h"
#include "PropertyTransition.h"
#include "PropertyBasicContent.h"
#include "PropertyCtrl.h"

#include <algorithm>

extern UINT WM_ON_LAYTOUT_SELECT_OBJ;
extern UINT WM_ON_MEDIA_ITEM_CHANGE;
extern UINT WM_ON_LAYOUT_SIZE_CHANGE ;
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
using namespace DESIGNERCMDS;
//using namespace DESIGNERCMDS;
 
namespace LAYOUTPROPERTYCMDS
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//global functions
	/*
	reselect obj
	*/

	void ReselectObj(CS3DrawObjList *pSelList)
	{
		CLayoutPropertyMonitor::WND_HANDLES &propWnds = CLayoutPropertyMonitor::Instance()->GetWnds();
		CLayoutPropertyMonitor::WND_HANDLES::iterator it = propWnds.begin();
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pPage);
		ASSERT(pSelList);
		//pPage->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ,1,0);
		if(pSelList->GetSize() <= 0)
		{
			 pPage->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 1,0);
			 return;
		}
		CWnd *pProp = NULL;
		while( it != propWnds.end())
		{
			if(pPage->IsChild(it->first))
			{
				if( it->first->IsKindOf(RUNTIME_CLASS(CPropertyLayer)))
				{
					pProp =it->first;
					if(!pProp->IsWindowVisible())
						pProp->ShowWindow(SW_SHOW);
					break;
				}else
					it->first->ShowWindow(SW_HIDE);
			}
			it++;
		}
		
		pPage->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 0, (LPARAM)pSelList); 
	}


    /*
    Get/Set Open Client info Tag. 
    */
    BOOL  SetOpenClientTag(BOOL bOpenClient = 0)
    {
        CLayoutPropertyMonitor::WND_HANDLES &propWnds = CLayoutPropertyMonitor::Instance()->GetWnds();
		CLayoutPropertyMonitor::WND_HANDLES::iterator it = propWnds.begin();
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

		ASSERT(pPage);

		CWnd *pProp = NULL;
		while( it != propWnds.end())
		{
			if( it->first->IsKindOf(RUNTIME_CLASS(CPropertyLayout)) 
							&&(pPage->IsChild(it->first)))
			{
				pProp =it->first;
				break;
			}  
			it++;
		}

		ASSERT(pProp);
        BOOL bBK = ((CPropertyLayout *)pProp)->m_bOpenClient;
        ((CPropertyLayout *)pProp)->m_bOpenClient = bOpenClient;

        return bBK;
    }

    BOOL  GetOpenClientTag()
    {
        CLayoutPropertyMonitor::WND_HANDLES &propWnds = CLayoutPropertyMonitor::Instance()->GetWnds();
		CLayoutPropertyMonitor::WND_HANDLES::iterator it = propWnds.begin();
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

		ASSERT(pPage);

		CWnd *pProp = NULL;
		while( it != propWnds.end())
		{
			if( it->first->IsKindOf(RUNTIME_CLASS(CPropertyLayout)) 
							&&(pPage->IsChild(it->first)))
			{
				pProp =it->first;
				break;
			}  
			it++;
		}

		ASSERT(pProp);
        return  ((CPropertyLayout *)pProp)->m_bOpenClient;
    }

    /*
    Get Last Layout Open client Info Tag,used in undo method
    */
    BOOL GetLastOpenClientTag()
    {
        CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
        ASSERT(pPage);
        CLayoutDesignerCtrl *pCtrl =  (CLayoutDesignerCtrl *)SearchWndObj(pPage,RUNTIME_CLASS(CLayoutDesignerCtrl));
        ASSERT(pCtrl);
        DESIGNERCMDS::LayoutCmdEngine *pCmdEngine =  pCtrl->GetCmdEngine();
        _CMDS::CMD_STACK  &undoStack = pCmdEngine->GetUndoCmds();
        if(undoStack.empty())
            return FALSE;

         _CMDS::CMD_STACK::container_type  _Container = undoStack._Get_container();
         _CMDS::CMD_STACK::container_type::reverse_iterator it = _Container.rbegin();
        
        it++; //jump current,when undo exec ,the current cmd still in undo stack,must jump it.
        while(it != _Container.rend())
         {
             _baseCmd *pCmd =  *it;
             if(pCmd->isObjOf(CMD_PROPERLAYOUT))
             {
                 SUB_CMDS &cmds = pCmd->getSubCmds();
                 for(int i = 0;i < cmds.size();i++)
                 {
                     _baseCmd  * pCmd1 = cmds[i];
                     if(!pCmd1->isObjOf(CMD_PROPERACTIONS))
                         continue;
                     SUB_CMDS &cmds2 = pCmd1->getSubCmds();
                     for(int j = 0;j < cmds2.size();j++)
                     {
                          _baseCmd  * pCmd2 = cmds2[j];
                          if(pCmd2->isObjOf(CMD_PROPERLAYOUTSIZE))
                              return ((layoutSizeCmd *)pCmd2)->bOpenClient;
                          
                     }
                 }
             }
             it++;
         }

         return FALSE;
    }
	/*
	Set ComboBox and edit of "Layout size pane" to right status.
	*/
	void SetPropLayoutSizeComboBox(CSize sz,BOOL bOpenClient = FALSE)
	{
		CLayoutPropertyMonitor::WND_HANDLES &propWnds = CLayoutPropertyMonitor::Instance()->GetWnds();
		CLayoutPropertyMonitor::WND_HANDLES::iterator it = propWnds.begin();
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

		ASSERT(pPage);

		CWnd *pProp = NULL;
		while( it != propWnds.end())
		{
			if( it->first->IsKindOf(RUNTIME_CLASS(CPropertyLayout)) 
							&&(pPage->IsChild(it->first)))
			{
				pProp =it->first;
				if(!pProp->IsWindowVisible())
					pProp->ShowWindow(SW_SHOW);
				break;
			}  
			it++;
		}

		if(pProp)
		{
			CString strItem;
			strItem.Format(_T("%d x %d"),sz.cx,sz.cy);
			CPropertyLayout *pLayoutProp = (CPropertyLayout*)pProp;
			CComboBox  *pCmbBox = (CComboBox  *)pLayoutProp->GetDlgItem(IDC_COMBO_SIZE);
			if(-1 == pCmbBox->SelectString(-1,strItem) && !bOpenClient)
            {
                pCmbBox->SetCurSel(18);
            }
            if(bOpenClient)
                pCmbBox->SetCurSel(19);

			CWnd *pWnd = pLayoutProp->GetDlgItem(IDC_EDIT_WIDTH);
			strItem.Format(_T("%d"),sz.cx);
			pWnd->SetWindowText(strItem);
            pWnd->EnableWindow(FALSE);

			pWnd = pLayoutProp->GetDlgItem(IDC_EDIT_HEIGHT);
			strItem.Format(_T("%d"),sz.cy);
			pWnd->SetWindowText(strItem);
            pWnd->EnableWindow(FALSE);
		}
		
	}


    /*
    Select Layout: Select layout ,look like click on layout.
    */
    void SelectLayout()
    {
        CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
        ASSERT(pPage);
        CLayoutDesignerCtrl *pCtrl =  (CLayoutDesignerCtrl *)SearchWndObj(pPage,RUNTIME_CLASS(CLayoutDesignerCtrl));
        ASSERT(pCtrl);
        pCtrl->SelectAllDrawObj(TRUE); //TRUE :revert select all.
        pPage->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 1,0);
    }

    /*
    Select One S3DrawObj ,look like click on it.
    */
    void SelectDrawObj( CS3DrawObj *pObj ,BOOL bShowPane = TRUE)
    {
        if(NULL == pObj)
            return;
        CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
        ASSERT(pPage);
        CLayoutDesignerCtrl *pCtrl =  (CLayoutDesignerCtrl *)SearchWndObj(pPage,RUNTIME_CLASS(CLayoutDesignerCtrl));
        ASSERT(pCtrl);
        pCtrl->SelectAllDrawObj(TRUE); //TRUE : unselection all.
        pCtrl->SelectDrawObj(pObj,1);
        if(bShowPane)
        pPage->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ,3, (LPARAM)pCtrl->GetSelectDrawObj());
    }
    /*
  template<typename T>
  T *GetCurProperty()
  {
  CLayoutPropertyMonitor::WND_HANDLES &propWnds = CLayoutPropertyMonitor::Instance()->GetWnds();
  CLayoutPropertyMonitor::WND_HANDLES::iterator it = propWnds.begin();
  CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

  ASSERT(pPage);

  T *pProp = NULL;
  while( it != propWnds.end())
  {
  if( it->first->IsKindOf(RUNTIME_CLASS(T)) 
  &&(pPage->IsChild(it->first)))
  {
  pProp =(T *)it->first;
  break;
  }  
  it++;
  }

  return pProp;         
  }
  */
    CPropertyLayout *GetPropertyLayout()
    {
        CLayoutPropertyMonitor::WND_HANDLES &propWnds = CLayoutPropertyMonitor::Instance()->GetWnds();
        CLayoutPropertyMonitor::WND_HANDLES::iterator it = propWnds.begin();
        CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

        ASSERT(pPage);

        CWnd *pProp = NULL;
        while( it != propWnds.end())
        {
            if( it->first->IsKindOf(RUNTIME_CLASS(CPropertyLayout)) 
                &&(pPage->IsChild(it->first)))
            {
                pProp =it->first;
                if(!pProp->IsWindowVisible())
                {
                   SelectLayout();
                }
            }  
            it++;
        }

        return (CPropertyLayout *)pProp;           
    }

    CPropertyLayer *GetPropertyLayer()
    {
        CLayoutPropertyMonitor::WND_HANDLES &propWnds = CLayoutPropertyMonitor::Instance()->GetWnds();
        CLayoutPropertyMonitor::WND_HANDLES::iterator it = propWnds.begin();
        CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

        ASSERT(pPage);

        CWnd *pProp = NULL;
        while( it != propWnds.end())
        {
            if( it->first->IsKindOf(RUNTIME_CLASS(CPropertyLayer)) 
                &&(pPage->IsChild(it->first)))
            {
                pProp =it->first;
                break;
            }  
            it++;
        }

        return (CPropertyLayer *)pProp;           
    }

    CPropertyMessage *GetPropertyMessage()
    {
        CLayoutPropertyMonitor::WND_HANDLES &propWnds = CLayoutPropertyMonitor::Instance()->GetWnds();
        CLayoutPropertyMonitor::WND_HANDLES::iterator it = propWnds.begin();
        CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

        ASSERT(pPage);

        CWnd *pProp = NULL;
        while( it != propWnds.end())
        {
            if( it->first->IsKindOf(RUNTIME_CLASS(CPropertyMessage)) 
                &&(pPage->IsChild(it->first)))
            {
                pProp =it->first;
                break;
            }  
            it++;
        }

        return (CPropertyMessage *)pProp;           
    }

    CPropertyBasicContent *GetPropertyBaseContent()
    {
        CLayoutPropertyMonitor::WND_HANDLES &propWnds = CLayoutPropertyMonitor::Instance()->GetWnds();
        CLayoutPropertyMonitor::WND_HANDLES::iterator it = propWnds.begin();
        CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

        ASSERT(pPage);

        CWnd *pProp = NULL;
        while( it != propWnds.end())
        {
            if( it->first->IsKindOf(RUNTIME_CLASS(CPropertyBasicContent)) 
                &&(pPage->IsChild(it->first)))
            {
                pProp =it->first;
                break;
            }  
            it++;
        }

        return (CPropertyBasicContent *)pProp;         
    }

    CPropertyTransition *GetPropertyTranstion(BOOL bInOut)
    {
        CLayoutPropertyMonitor::WND_HANDLES &propWnds = CLayoutPropertyMonitor::Instance()->GetWnds();
        CLayoutPropertyMonitor::WND_HANDLES::iterator it = propWnds.begin();
        CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

        ASSERT(pPage);

        CWnd *pProp = NULL;
        while( it != propWnds.end())
        {
            if( it->first->IsKindOf(RUNTIME_CLASS(CPropertyTransition)) 
                &&(pPage->IsChild(it->first))
                &&((CPropertyTransition *)it->first)->m_bInOut == bInOut)
            {
                pProp =it->first;
                break;
            }  

            it++;
        }

        return (CPropertyTransition *)pProp;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //property layout sel color. 
    //
    COLORREF GetPropertySelColor()
    {
       CPropertyLayout *pProperLayout = GetPropertyLayout();
       if(pProperLayout)
       {
           CMFCColorButton *pColor = (CMFCColorButton *)pProperLayout->GetDlgItem(IDC_MFCCOLORBUTTON_BG);
           return  pColor->GetColor(); // pProperLayout->m_pLayout->BGColor;
       }
       return RGB(0,0,0);
    }
   
    void SetPropertySelColor(COLORREF clr)
    {

        CPropertyLayout *pProperLayout = GetPropertyLayout();
        if(pProperLayout)
        {
            CMFCColorButton *pColor = (CMFCColorButton *)pProperLayout->GetDlgItem(IDC_MFCCOLORBUTTON_BG);
            return  pColor->SetColor(clr); // pProperLayout->m_pLayout->BGColor;
        }
    }

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //scroll peropertys wnd pos

    void ScrollWnd(CWnd *pWnd ,int nPos)
    {
        SCROLLINFO  sInfo;
        DWORD dwStyle ;
        if(!pWnd)return;
        if(!::IsWindow(pWnd->m_hWnd))
            return;
        dwStyle= pWnd->GetStyle();
        if(!(dwStyle & WS_VSCROLL))
            return;
       
        pWnd->SetScrollPos(SB_VERT,nPos);
        pWnd->SendMessage(WM_SIZE,0,0);
       // pWnd->SendMessage(WM_VSCROLL,(WPARAM)SB_LINEDOWN,(LPARAM)nPos);
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	_basePropCmd::_basePropCmd(int id,CWnd *pTarget):_baseCmd(id,pTarget){} 
	_basePropCmd::~_basePropCmd(){}

	int _basePropCmd::undo()
	{
		__super::undo();
		for(int i = 0;i < this->m_subCmds.size();i++)
		{
			_baseCmd *pCmd = m_subCmds[i];
			pCmd->undo();
		}
		return SUCCESS;
	}
	int _basePropCmd::redo()
	{
		__super::redo();
  		for(int i = 0;i < this->m_subCmds.size();i++)
		{
			_baseCmd *pCmd = m_subCmds[i];
			pCmd->redo();
		}
		return SUCCESS;
	}
	int _basePropCmd::ondo()
	{
		__super::ondo();
		for(int i = 0;i < this->m_subCmds.size();i++)
		{
			_baseCmd *pCmd = m_subCmds[i];
			if(IS_FAILURE(pCmd->ondo()))
                return FAILURE;
		}
		return SUCCESS;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//template to hold actions of layout.
	actionsLayoutCmd::actionsLayoutCmd(int id,CWnd *pTarget):_basePropCmd(id,pTarget){};
	actionsLayoutCmd::~actionsLayoutCmd(){};

    BOOL   actionsLayoutCmd::BeginPrepareStatus()
    {
        CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
        CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

        ASSERT(pOwner);
        ASSERT(pPage);

        if(this->nMessage == WM_ON_LAYOUT_SIZE_CHANGE_BEGIN)
        {
            LayoutWidget *pLayout = (LayoutWidget *)pOwner->GetWidget(0);
            LayoutWidget::RECTS &rcts =  pLayout->GetMonitorRects();

            layoutSizeCmd *pCmd = new layoutSizeCmd(CMD_PROPERLAYOUTSIZE,pOwner);
            pCmd->storeSize = boost::make_tuple(pOwner->m_LayoutSize, pPage->m_LayoutElement->GetLayoutSize());
           
            if(rcts.size())
                pCmd->storeRects.get<0>().assign(rcts.begin(),rcts.end());//((LayoutWidget *)pOwner->GetWidget(0))->GetMonitorRects();
            pCmd->storeRotateDegree.get<0>() = pLayout->m_fRotateDegree;
            pCmd->storeTransX.get<0>() = pLayout->m_xTrans;
            pCmd->storeTransY.get<0>() = pLayout->m_yTrans;
            addSubCmd(pCmd);
        }

        if(this->nMessage == WM_SET_LAYOUT_COLOR)
        {
            layoutColorCmd *pCmd =  new layoutColorCmd( CMD_PROPERLAYOUTCOLOR,pOwner);
            addSubCmd(pCmd);
        }

        if(this->nMessage == WM_LAYOUT_TEXT_CHANGE)
        {
            LayoutTextChangeCmd *pCmd = new LayoutTextChangeCmd(CMD_PROPERLAYOUTTEXTCHANGE,pOwner);
            pCmd->nType = dwParam;
            addSubCmd(pCmd);
        }

        if(this->nMessage == WM_MEDIASETTING_UPDATE_BEGIN)
        {
            layerTextSettingCmd *pCmd = new layerTextSettingCmd(CMD_TEXTSETTING,pOwner);
            pCmd->SaveSwapSetting();
            addSubCmd(pCmd);
        }

        if(this->nMessage == WM_TRANS_UPDATE_BEGIN)
        {
            layerTransitionCmd *pCmd = new layerTransitionCmd(CMD_TRANSITON,pOwner);
            pCmd->bInOut = dwParam;
            pCmd->SaveSwapTrans();
            addSubCmd(pCmd);
        }  

        return TRUE;
    }

    BOOL  actionsLayoutCmd::EndPrepareStatus()
    {
        CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
        CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

        ASSERT(pOwner);
        ASSERT(pPage);

         if(this->nMessage == WM_ON_LAYOUT_SIZE_CHANGE_END)
        {
            LayoutWidget *pLayout = (LayoutWidget *)pOwner->GetWidget(0);
            LayoutWidget::RECTS &rcts =  pLayout->GetMonitorRects();
            SUB_CMDS &cmds = this->getSubCmds();
            layoutSizeCmd *pCmd = (layoutSizeCmd *)cmds[cmds.size()-1];//new layoutSizeCmd(0,pOwner);
            if(rcts.size())
                pCmd->storeRects.get<1>().assign(rcts.begin(),rcts.end());//((LayoutWidget *)pOwner->GetWidget(0))->GetMonitorRects();

            pCmd->storeRotateDegree.get<1>() = pLayout->m_fRotateDegree;
            pCmd->storeTransX.get<1>() = pLayout->m_xTrans;
            pCmd->storeTransY.get<1>() = pLayout->m_yTrans;
            pCmd->bOpenClient = GetOpenClientTag();
        }
        return TRUE;
    }

	int actionsLayoutCmd::undo()
	{
		__super::undo();

		return SUCCESS;
	}

	int actionsLayoutCmd::redo()
	{
		__super::redo();
		return SUCCESS;
	}

	int actionsLayoutCmd::ondo()
	{
		return __super::ondo();
		//according to the uMsg ,to produce cmds add to  "subcmds list" of parent cmd
		

		//return SUCCESS;
	}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
	actionsInstCmd::actionsInstCmd(int id  ,CWnd *pTarget):_basePropCmd(id,pTarget){};
	int actionsInstCmd::undo()
	{
		__super::undo();
		return SUCCESS;
	};
	int actionsInstCmd::redo()
	{
		__super::redo();
		return SUCCESS;
	};

    BOOL actionsInstCmd::BeginPrepareStatus()
    {
         CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
        if(this->nMessage == WM_MEDIA_BASICINFO_UPDATE_BEGIN)
        {
            layerBasicContentChangeCmd *pCmd = new layerBasicContentChangeCmd(CMD_BASECONTENT,pOwner);
            pCmd->SaveSwapMedia();
            addSubCmd(pCmd);
        }
        return TRUE;
    };

    BOOL actionsInstCmd::EndPrepareStatus()
    {
        return TRUE;
    };

	int actionsInstCmd::ondo()
	{
		//__super::ondo();
		//according to the uMsg ,to produce cmds add to  "subcmds list" of parent cmd
		CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

		ASSERT(pOwner);
		ASSERT(pPage);

		LayoutCmdEngine *pEngine = pOwner->GetCmdEngine();	
		LayoutCmdFactory *pFactory = (LayoutCmdFactory *)pEngine->getFactory();
		if(this->nMessage == WM_ON_SIZE_POS_CHANGE2)
		{
			objSizeCmd *pCmd = (objSizeCmd *)pFactory->newCmd(CMD_OBJSIZE);
			
			if(IS_FAILURE(pCmd->ondo())) //call ondo here
			{
				pFactory->delCmd(pCmd);
				return FAILURE;
			}
			addSubCmd(pCmd);
		}

		if(this->nMessage == WM_ON_LAYER_NAME_CHANGE)
        {
            layerNameChangeCmd *pCmd = new layerNameChangeCmd(CMD_LAYERNAME ,pOwner);
            if(IS_FAILURE(pCmd->ondo())) //call ondo here
			{
				delete pCmd;
                return FAILURE;
			}
			addSubCmd(pCmd);
        }

        if(this->nMessage == WM_MEDIA_BASICINFO_UPDATE_END)
        {
            SUB_CMDS &cmds = this->getSubCmds();
            if(cmds.size() <= 0)
                return FAILURE;
            layerBasicContentChangeCmd *pCmd = (layerBasicContentChangeCmd *)cmds[cmds.size() -1];
             if(IS_FAILURE(pCmd->ondo())) //call ondo here
			{
				delete pCmd;
                cmds.pop_back();
                return FAILURE;
			}
        }

        if(this->nMessage == WM_ON_LAYER_LOOP_CHANGE)
        {
            layerLoopCheckCmd *pCmd = new layerLoopCheckCmd(CMD_LOOPCHECK,pOwner);
            if(IS_FAILURE(pCmd->ondo()))
            {
                delete pCmd;
                return FAILURE;
            }
            addSubCmd(pCmd);
        }

        if(this->nMessage == WM_ON_LAYER_KEEP_ASPECT_CHANGE)
        {
            layerKeepAspectCheckCmd *pCmd = new layerKeepAspectCheckCmd(CMD_KEEPASPECTCHECK,pOwner);
            if(IS_FAILURE(pCmd->ondo()))
            {
                delete pCmd;
                return FAILURE;
            }
            addSubCmd(pCmd);
        }
		if(this->nMessage == WM_ON_MEDIA_NUM_CHANGE ||this->nMessage == WM_ON_MEDIA_ITEM_CHANGE)
		{
			WORD wType = LOWORD(dwWParam);
			switch(wType)
			{
			case ACTION_DEL://delete mediasetting from vsMedialist.
				{
					mediaListDeleteItemCmd *pCmd = new mediaListDeleteItemCmd(0,pOwner);
					pCmd->iIndex = HIWORD(dwWParam);
					pCmd->MediaSetting = *(std::shared_ptr<MediaElement> *)dwLParam;
					if(!pCmd->ondo())
					{
						delete pCmd;
						return FAILURE;
					}
					addSubCmd(pCmd);
				}
				break;
			case ACTION_ADD://add mediasetting to vsMedialist.
				{
					mediaListNewItemCmd *pCmd = new mediaListNewItemCmd(0,pOwner);
					pCmd->MediaSetting = *(std::shared_ptr<MediaElement> *)dwLParam;
					if(!pCmd->ondo())
					{
						delete pCmd;
						return FAILURE;
					}
					addSubCmd(pCmd);
				}
				break;
			case ACTION_ITEM_MOVE_UP://move up,and move down.
			case ACTION_ITEM_MOVE_DOWN:
				{
					mediaListItemChangeCmd *pCmd = new mediaListItemChangeCmd(0,pOwner);
					if(!pCmd->ondo())
					{
						delete pCmd;
						return FAILURE;
					}
					addSubCmd(pCmd);
				}
				break;
			default:
				ASSERT(0);
				return FAILURE;
				break;
			}
		}
		return SUCCESS;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//layoutSizeCmd
	//
	layoutSizeCmd::layoutSizeCmd(int nID,CWnd *pTarget): _baseCmd(nID,pTarget){
        bOpenClient = FALSE;
    }
	layoutSizeCmd::~layoutSizeCmd(){}

	int layoutSizeCmd::ondo()
	{
		__super::ondo();
		static BOOL bEnable = FALSE; // filter 
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		ASSERT(pOwner);

		if(pOwner->m_drawObjList.GetSize() <= 1 && FALSE == bEnable)//only has "BKGND" object.
			return FAILURE;
        
        LayoutWidget *pLayout =  (LayoutWidget *)pOwner->GetWidget(LAYOUT_Z_0);

        if(storeSize.get<0>() == storeSize.get<1>())
        {
            if(storeRects.get<0>().size() == storeRects.get<1>().size())
            if(!std::equal(storeRects.get<0>().begin(),storeRects.get<0>().end(),storeRects.get<1>().begin()))
                return FAILURE;
        }
       
		bEnable = TRUE;
		return SUCCESS;
	}

	int layoutSizeCmd::redo()
	{
		__super::redo();
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pPage);

        CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
        ASSERT(pOwner);
        LayoutWidget *pLayout =  (LayoutWidget *)pOwner->GetWidget(LAYOUT_Z_0);

        pLayout->m_xTrans = storeTransX.get<1>();
        pLayout->m_yTrans = storeTransY.get<1>();

        pLayout->SetClientLayout( storeSize.get<1>(),&(storeRects.get<1>()),FALSE,storeRotateDegree.get<1>());
     
        pOwner->m_LayoutSize =  storeSize.get<1>();
        pPage->m_layoutSize =  storeSize.get<1>();
        pPage->m_LayoutElement->GetLayoutSize() =  storeSize.get<1>();
        SetPropLayoutSizeComboBox(storeSize.get<1>(),bOpenClient);
		//pOwner->newl
       
        SetOpenClientTag(bOpenClient);
      
		return SUCCESS;
	}
	int layoutSizeCmd::undo()
	{
		__super::undo();
        CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pPage);
        CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
        ASSERT(pOwner);
        LayoutWidget *pLayout =  (LayoutWidget *)pOwner->GetWidget(LAYOUT_Z_0);
       

        BOOL bLastOpenClientFlag = GetLastOpenClientTag();
        pLayout->m_yTrans = storeTransY.get<0>();
        pLayout->m_xTrans = storeTransX.get<0>();
        pLayout->SetClientLayout( storeSize.get<0>(),&storeRects.get<0>(),FALSE,storeRotateDegree.get<0>());
        
        pPage->m_layoutSize = storeSize.get<0>();
        pOwner->m_LayoutSize = storeSize.get<0>();
        pPage->m_LayoutElement->SetLayoutSize(storeSize.get<0>());
        
		SetPropLayoutSizeComboBox(this->storeSize.get<0>(),bLastOpenClientFlag);
        SetOpenClientTag(bLastOpenClientFlag);
		return SUCCESS;
	}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////layoutColorCmd
    ///

    layoutColorCmd::layoutColorCmd(int nID,CWnd *pTarget):_baseCmd(nID,pTarget){
        curColor = RGB(0,0,0);// default white color.
        swapColor = RGB(0,0,0);
    }
     layoutColorCmd::~layoutColorCmd(){}

    int layoutColorCmd:: redo(){
        __super::redo();

        CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
        ASSERT(pOwner);
      
         SetPropertySelColor(swapColor);

         CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
         ASSERT(pPage);
         if(pPage && pPage->m_LayoutElement)
         pPage->m_LayoutElement->SetBGColor(swapColor);
        //CPropertyLayout *pProp = GetPropertyLayout();
        //if(pProp && pProp->m_pLayout )
        //{
        //    pProp->m_pLayout->BGColor = swapColor;
        //}
       
        return SUCCESS;
    }

    int layoutColorCmd:: undo(){
        __super::undo();

        CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
        ASSERT(pOwner);

        SetPropertySelColor(curColor);

        CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
        ASSERT(pPage);
        if(pPage && pPage->m_LayoutElement)
            pPage->m_LayoutElement->SetBGColor(curColor);
    
        return SUCCESS;
    }

    int layoutColorCmd::ondo(){
        __super::ondo();

        CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
        ASSERT(pOwner);

        CPropertyLayout *pProp =  GetPropertyLayout();
        if(NULL == pProp)
        return FAILURE;

       // COLORREF curColor = RGB(0,0,0);
        if(pProp && pProp->m_layoutInfo)
        {
            curColor = pProp->m_layoutInfo->GetBGColor();
        }
        swapColor = GetPropertySelColor();

        if(swapColor == curColor)
        return FAILURE;
        pProp->m_layoutInfo->SetBGColor(swapColor);       
       // swapColor = curColor;
        return SUCCESS;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Layout name and description.
    //LayoutTextChangeCmd
    //
    LayoutTextChangeCmd::LayoutTextChangeCmd(int nID ,CWnd *pTarget):_baseCmd(nID,pTarget){}
    LayoutTextChangeCmd::~LayoutTextChangeCmd(){}
    int LayoutTextChangeCmd::redo(){
        __super::redo();

        CPropertyLayout *pProp =  GetPropertyLayout();
        if(NULL == pProp)
        return FAILURE;

        if(NULL == pProp->m_layoutInfo)
        return FAILURE;

        if(nType == 1)
        {
            pProp->m_layoutInfo->SetLayoutName(tupleNames.get<1>());
            pProp->GetDlgItem(IDC_EDIT_NAME)->SetWindowText(tupleNames.get<1>());
        }

        if(nType == 2)
        {
            pProp->m_layoutInfo->SetDescription(tupleDescs.get<1>());
            pProp->GetDlgItem(IDC_EDIT_LDESC)->SetWindowText(tupleDescs.get<1>());
        }
        return SUCCESS;
    }

    int LayoutTextChangeCmd::undo(){
        __super::undo();

        CPropertyLayout *pProp =  GetPropertyLayout();
        if(NULL == pProp)
        return FAILURE;

        if(NULL == pProp->m_layoutInfo)
        return FAILURE;

        if(nType == 1)
        {
            pProp->m_layoutInfo->SetLayoutName(tupleNames.get<0>());
            pProp->GetDlgItem(IDC_EDIT_NAME)->SetWindowText(tupleNames.get<0>());
        }

        if(nType == 2)
        {
            pProp->m_layoutInfo->SetDescription(tupleDescs.get<0>());
            pProp->GetDlgItem(IDC_EDIT_LDESC)->SetWindowText(tupleDescs.get<0>());
        }
        return SUCCESS;
    }

    int LayoutTextChangeCmd::ondo(){
        __super::ondo();
         CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
        ASSERT(pOwner);

        CPropertyLayout *pProp =  GetPropertyLayout();
        if(NULL == pProp)
        return FAILURE;

        if(NULL == pProp->m_layoutInfo)
        return FAILURE;
        
        if(nType == 1) //1:name changed.
        {
            tupleNames.get<0>() = pProp->m_layoutInfo->GetLayoutName();
            pProp->GetDlgItem(IDC_EDIT_NAME)->GetWindowText(tupleNames.get<1>());
            if(tupleNames.get<0>() == tupleNames.get<1>())
                return FAILURE;
              pProp->m_layoutInfo->SetLayoutName(tupleNames.get<1>());
        }

        if(nType == 2)//2 :description changed.
        {
            tupleDescs.get<0>() = pProp->m_layoutInfo->GetDescription();
            pProp->GetDlgItem(IDC_EDIT_LDESC)->GetWindowText(tupleDescs.get<1>());
            if(tupleDescs.get<0>() == tupleDescs.get<1>())
                return FAILURE;
             pProp->m_layoutInfo->SetDescription(tupleDescs.get<1>());
        }
       
        
        return SUCCESS;
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //layer property
    //

    //
    //base  layer action to store/restore property pane status.
    //
    int GetVScrollPos(CWnd *pWnd)
    {
        UINT nStyle = pWnd->GetStyle();
        if(nStyle & WS_VSCROLL)
        {
            return pWnd->GetScrollPos(SB_VERT);
        }

        return -1;
    }

    int baseActionCmd::redo()
    {
        __super::redo();
         ScrollWnd(pScrollCtrl,nScrollPos);
        return SUCCESS;
    }
    int baseActionCmd::undo()
    {
        __super::undo();
        ScrollWnd(pScrollCtrl,nScrollPos);
        return SUCCESS;
    }
    int  baseActionCmd::ondo()
    {
        __super::ondo();
        CDesignerPage *pPage = (CDesignerPage*)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
        pScrollCtrl = SearchWndObj(pPage,RUNTIME_CLASS(CPropertyCtrl));
        ASSERT(pScrollCtrl!=NULL);
        if(!pScrollCtrl)
            return FAILURE;
        this->nScrollPos =  GetVScrollPos(pScrollCtrl);
        return SUCCESS;
    }
    //
    //layerNameChangeCmd
    //
    layerNameChangeCmd:: layerNameChangeCmd(int nID,CWnd *pTarget):baseActionCmd(nID,pTarget){
        this->pcurObj = NULL;
    };
    layerNameChangeCmd:: ~layerNameChangeCmd(){};
 
    int layerNameChangeCmd::redo()
    {
        __super::redo();
       
        CPropertyLayer * pProperty = GetPropertyLayer();
        ASSERT(pProperty);
        //select obj again.
        if(!pProperty->IsWindowVisible())
        SelectDrawObj(pcurObj);
        else
        SelectDrawObj(pcurObj,FALSE);

        CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
        ASSERT(pPage);

       
        pcurObj->SetSerialName(tupleNames.get<1>());
        pProperty->GetDlgItem(IDC_EDIT_NAME)->SetWindowText(tupleNames.get<1>());

        pPage->OnLayerNameChange((WPARAM)pcurObj,0); ;
        return SUCCESS;
    }

    int layerNameChangeCmd::undo()
    {
        __super::undo(); 
       
        CPropertyLayer * pProperty = GetPropertyLayer();
        ASSERT(pProperty);
        //select obj again.
        if(!pProperty->IsWindowVisible())
        SelectDrawObj(pcurObj);
        else
        SelectDrawObj(pcurObj,FALSE);

        CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
        ASSERT(pPage);

      
        pcurObj->SetSerialName(tupleNames.get<0>());
        pProperty->GetDlgItem(IDC_EDIT_NAME)->SetWindowText(tupleNames.get<0>());
        pPage->OnLayerNameChange((WPARAM)pcurObj,0); //SendMessage(WM_ON_LAYER_NAME_CHANGE,(WPARAM)pcurObj,0);
        return  SUCCESS;
    }

    int layerNameChangeCmd::ondo()
    {
        __super::ondo();
        CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
        ASSERT(pOwner);
        CPropertyLayer * pProperty = GetPropertyLayer();
        ASSERT(pProperty);

        if(pOwner->m_drawObjList.GetCount() <=0 || 
            pOwner->m_selectObjList.GetCount() <= 0 || 
            pOwner->m_selectObjList.GetCount() > 1)
            return FAILURE;

        pcurObj = pOwner->m_selectObjList.GetHead();
        pcurObj->GetSerialName( tupleNames.get<0>());
       
        pProperty->GetDlgItem(IDC_EDIT_NAME)->GetWindowText(tupleNames.get<1>());

        if(tupleNames.get<0>() == tupleNames.get<1>())
            return FAILURE;
        pcurObj->SetSerialName(tupleNames.get<1>());
        return  SUCCESS;;
    }

    //
    //layerLoopCheckCmd
    //
    layerLoopCheckCmd::layerLoopCheckCmd(int nID,CWnd *pTarget):baseActionCmd(nID,pTarget){}
    layerLoopCheckCmd::~layerLoopCheckCmd(){}
    int layerLoopCheckCmd::redo()
    {
        __super::redo();
        CDesignerPage  *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
        ASSERT(pPage);
        CPropertyLayer * pProperty = GetPropertyLayer();
        ASSERT(pProperty);

        if(!pProperty->IsWindowVisible())
            SelectDrawObj(pcurObj);
        else
            SelectDrawObj(pcurObj,FALSE);

        nCheck = (nCheck)? FALSE:TRUE;
        pcurObj->SetLoopSync(nCheck);
        ((CButton *)pProperty->GetDlgItem(IDC_CHECK_LOOPSYNC))->SetCheck(nCheck);
        pPage->OnLayerLoopChange( (WPARAM)pcurObj,0);
        pProperty->CalcLayerDuration();
        return SUCCESS;
    }
    int layerLoopCheckCmd::undo()
    {
        __super::undo();
        CDesignerPage  *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
        ASSERT(pPage);
        CPropertyLayer * pProperty = GetPropertyLayer();
        ASSERT(pProperty);

        if(!pProperty->IsWindowVisible())
            SelectDrawObj(pcurObj);
        else
            SelectDrawObj(pcurObj,FALSE);

        nCheck = (nCheck)? FALSE:TRUE;
        pcurObj->SetLoopSync(nCheck);
        ((CButton *)pProperty->GetDlgItem(IDC_CHECK_LOOPSYNC))->SetCheck(nCheck);
        pPage->OnLayerLoopChange( (WPARAM)pcurObj,0);
        pProperty->CalcLayerDuration();
        return SUCCESS;
    }
    int layerLoopCheckCmd::ondo()
    {
        __super::ondo();

        CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
        ASSERT(pOwner);
        CPropertyLayer * pProperty = GetPropertyLayer();
        ASSERT(pProperty);

        if(pOwner->m_drawObjList.GetCount() <=0 || 
            pOwner->m_selectObjList.GetCount() <= 0 || 
            pOwner->m_selectObjList.GetCount() > 1)
            return FAILURE;
         pcurObj = pOwner->m_selectObjList.GetHead();

         BOOL bLoop;
         nCheck = ((CButton *)pProperty->GetDlgItem(IDC_CHECK_LOOPSYNC))->GetCheck();
         pcurObj->GetLoopSync(bLoop);
         if(bLoop == nCheck)
             return FAILURE;
         pcurObj->SetLoopSync(nCheck);
         pProperty->CalcLayerDuration();
        return SUCCESS;
    }
    
    //
    //layerLoopCheckCmd
    //
    layerKeepAspectCheckCmd::layerKeepAspectCheckCmd(int nID,CWnd *pTarget):baseActionCmd(nID,pTarget){}
    layerKeepAspectCheckCmd::~layerKeepAspectCheckCmd(){}

    int layerKeepAspectCheckCmd::redo()
    {
        __super::redo();
        CDesignerPage  *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
        ASSERT(pPage);
        CPropertyLayer * pProperty = GetPropertyLayer();
        ASSERT(pProperty);

        if(!pProperty->IsWindowVisible())
            SelectDrawObj(pcurObj);
        else
            SelectDrawObj(pcurObj,FALSE);

        nCheck = (nCheck)? FALSE:TRUE;
        pcurObj->SetKeepAspect(nCheck);
        pcurObj->CalcAspectRatio();
        ((CButton *)pProperty->GetDlgItem(IDC_CHECK_KEEP_ASPACT))->SetCheck(nCheck);
        return SUCCESS;
    }
    int layerKeepAspectCheckCmd::undo()
    {
        __super::undo();
        CDesignerPage  *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
        ASSERT(pPage);
        CPropertyLayer * pProperty = GetPropertyLayer();
        ASSERT(pProperty);

        if(!pProperty->IsWindowVisible())
            SelectDrawObj(pcurObj);
        else
            SelectDrawObj(pcurObj,FALSE);

        nCheck = (nCheck)? FALSE:TRUE;
        pcurObj->SetKeepAspect(nCheck);
        pcurObj->CalcAspectRatio();
        ((CButton *)pProperty->GetDlgItem(IDC_CHECK_KEEP_ASPACT))->SetCheck(nCheck);
        return SUCCESS;
    }

    int layerKeepAspectCheckCmd::ondo()
    {
        __super::ondo();
        CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
        ASSERT(pOwner);
        CPropertyLayer * pProperty = GetPropertyLayer();
        ASSERT(pProperty);

        if(pOwner->m_drawObjList.GetCount() <=0 || 
            pOwner->m_selectObjList.GetCount() <= 0 || 
            pOwner->m_selectObjList.GetCount() > 1)
            return FAILURE;
        pcurObj = pOwner->m_selectObjList.GetHead();

         nCheck = ((CButton *)pProperty->GetDlgItem(IDC_CHECK_KEEP_ASPACT))->GetCheck();
         BOOL keepAspectRatio = pcurObj->GetKeepAspect();

         if(keepAspectRatio == nCheck)
             return FAILURE;

         pcurObj->SetKeepAspect(nCheck);
         pcurObj->CalcAspectRatio();
        return SUCCESS;
    }
    
	//
	//action for media list
    //
	mediaListItemChangeCmd::mediaListItemChangeCmd(int nID,CWnd *pTarget): baseActionCmd(nID,pTarget){
		pcurObj = NULL;
	}
	mediaListItemChangeCmd::~mediaListItemChangeCmd(){

		if(storeMedias.size())
		for(int i = 0;i < storeMedias.size();i++)
		{
			storeMedias[i].reset();
		}

		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		if(pPage && ::IsWindow(pOwner->GetSafeHwnd()))
		{

		}

	}

	int mediaListItemChangeCmd::redo()
	{
		__super::redo();

		CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

		ASSERT(pOwner);
		ASSERT(pPage);
		ASSERT(pcurObj);

		//pOwner->SelectDrawObj(this->pcurObj);
		//pPage->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 0, (LPARAM)&pOwner->m_selectObjList);

		MEDIAS tpMedias;

		//save current
		SaveInstMediaList(pcurObj,tpMedias);
		//restore old
	    RestoreInstMediaList(this->saveMedias,pcurObj);

		//exchange store data
		this->saveMedias.clear();
		this->saveMedias.assign(tpMedias.begin(),tpMedias.end());

		//pPage->m_wndLayoutProperties.m_PropertyCtrl.SetLayer(pLayerInfo, this->pcurObj);
		//pOwner->SelectDrawObj(this->pcurObj);
		//pPage->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 0, (LPARAM)&pOwner->m_selectObjList);
		pOwner->SelectDrawObj(this->pcurObj);
		ReselectObj(&pOwner->m_selectObjList);
		return SUCCESS;
	}

	int mediaListItemChangeCmd::undo()
	{      
		__super::undo();

		CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

		ASSERT(pOwner);
		ASSERT(pPage);
		ASSERT(pcurObj);

		MEDIAS tpMedias;
		//save current
		SaveInstMediaList(pcurObj,tpMedias);
		//restore old
		RestoreInstMediaList(this->saveMedias,pcurObj);

		//exchange store data
		this->saveMedias.clear();
		this->saveMedias.assign(tpMedias.begin(),tpMedias.end());	

		//pPage->m_wndLayoutProperties.m_PropertyCtrl.SetLayer(pLayerInfo, this->pcurObj);
		pOwner->SelectDrawObj(this->pcurObj);
		ReselectObj(&pOwner->m_selectObjList);
		return SUCCESS;
	}
	int mediaListItemChangeCmd::ondo()
	{
		__super::ondo();
		CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

		if(pOwner->m_selectObjList.GetSize() > 1 || 
				pOwner->m_selectObjList.GetSize() <= 0)
				return FAILURE;

		this->pcurObj = pOwner->m_selectObjList.GetHead();
		//Save Current data
		SaveInstMediaList(pcurObj,this->saveMedias);
	//	SaveInstMediaList(pcurObj,this->storeMedias,1);//save new 
		return SUCCESS;
	}

	void  mediaListItemChangeCmd::SaveInstMediaList(CS3DrawObj *pDrawObj,  MEDIAS &medias ,BOOL bCreate)
	{
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		//ASSERT(pPage->m_pLayoutSetting);
		//LAYOUT *pSetting = pPage->m_pLayoutSetting;
		BOOL bFound = FALSE;
		std::shared_ptr<LayerElement> layInfo = pPage->m_LayoutElement->GetLayerInfo(pDrawObj->GetLayerId(), bFound);
		if (bFound && layInfo.get() != NULL)
		{
			vector<std::shared_ptr<MediaElement> >& mediaList = layInfo->GetMediaList();
			medias = mediaList;
		}

#if 0
		LAYERINFO *pLayInfo=NULL; 
		if(pSetting->Layers.Lookup( pDrawObj,pLayInfo))
		if(pLayInfo)
		{
				//pLayInfo->AddTail(pAppendedSetting);
			POSITION pos = pLayInfo->GetHeadPosition();
			while(pos)
			{
				MEDIASETTING *p =  pLayInfo->GetAt(pos);
				if(bCreate)
				{
					MEDIASETTING *tp = new MEDIASETTING;
					if(p->pTextSetting)
						tp->pTextSetting = new S3SIGNAGE_TEXT_SETTING;

					*tp = *p;
					medias.push_back(tp);
				}
				else
					medias.push_back(p);
				pLayInfo->GetNext(pos);
			}
			
		}
#endif

	}

	std::shared_ptr<LayerElement> mediaListItemChangeCmd::RestoreInstMediaList( MEDIAS &medias,CS3DrawObj *pDrawObj)
	{

		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		//ASSERT(pPage->m_pLayoutSetting);
		//LAYOUT *pSetting = pPage->m_pLayoutSetting;
	
	//	MEDIAS tpMedias;
	//	SaveInstMediaList(pDrawObj,tpMedias);

		BOOL bFound = FALSE;
		std::shared_ptr<LayerElement> layInfo = pPage->m_LayoutElement->GetLayerInfo(pDrawObj->GetLayerId(), bFound);
		if (bFound && layInfo.get() != NULL)
		{
			layInfo->RemoveAll();
			for( int i = 0;i < medias.size();i++)
			{
				layInfo->AddTailMediaElement(medias[i]);
			}
			return layInfo;

		}
		else
		{
			ASSERT(FALSE);
			return layInfo;
		}

	
/*
		if(pSetting->Layers.Lookup( pDrawObj,pLayInfo))
		if(pLayInfo)
		{
				//pLayInfo->AddTail(pAppendedSetting);
			pLayInfo->RemoveAll();

			for( int i = 0;i < medias.size();i++)
			{
				pLayInfo->AddTail(medias[i]);
			}
			
		}
		return pLayInfo;*/
	}


	mediaListNewItemCmd::mediaListNewItemCmd(int nID,CWnd *pTarget):baseActionCmd(nID,pTarget){
		this->pDrawObj = NULL;
	}
	mediaListNewItemCmd::~mediaListNewItemCmd(){
		if(this->m_nOpt == OPT_REDO)
		{
			//delete this->pMediaSetting;
			MediaSetting.reset();
		}
   }

	int mediaListNewItemCmd::redo()
	{
		__super::redo();
		CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

		ASSERT(pOwner);
		ASSERT(pPage);
		
		BOOL bFound = FALSE;
		std::shared_ptr<LayerElement> layInfo = pPage->m_LayoutElement->GetLayerInfo(pDrawObj->GetLayerId(), bFound);
		if (!bFound || layInfo.get() == NULL)
		{
			ASSERT(FALSE);
			return FAILURE;
		}
		
		layInfo->AddTailMediaElement(MediaSetting);

#if 0
		if (!pPage->m_pLayoutSetting->Layers.Lookup(pDrawObj, pLayerInfo))
		{
			ASSERT(FALSE);
			return FAILURE;
		}
		pLayerInfo->AddTail(this->pMediaSetting);
#endif
		
		pOwner->SelectDrawObj(pDrawObj);
		ReselectObj(&pOwner->m_selectObjList);
		return SUCCESS;
	}

	int mediaListNewItemCmd::undo()
	{
		__super::undo();
		CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

		ASSERT(pOwner);
		ASSERT(pPage);

		BOOL bFound = FALSE;
		std::shared_ptr<LayerElement> layInfo = pPage->m_LayoutElement->GetLayerInfo(pDrawObj->GetLayerId(), bFound);
		if (!bFound || layInfo.get() == NULL)
		{
			ASSERT(FALSE);
			return FAILURE;
		}
		layInfo->RemoveTailElement();
#if 0
		LAYERINFO *pLayerInfo = NULL;
		if (!pPage->m_pLayoutSetting->Layers.Lookup(pDrawObj, pLayerInfo))
		{
			ASSERT(FALSE);
			return FAILURE;
		}

		POSITION pos = pLayerInfo->Find(this->pMediaSetting);
		if(pos)
		{
			pLayerInfo->RemoveAt(pos);
		}
#endif

		pOwner->SelectDrawObj(this->pDrawObj);
		ReselectObj(&pOwner->m_selectObjList);
		
		return SUCCESS;
	}
	int mediaListNewItemCmd::ondo()
	{
		__super::ondo();
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		pDrawObj = pPage->m_wndLayoutProperties.m_PropertyCtrl.m_pDrawObj;
		ASSERT(pDrawObj);
		return SUCCESS;
	}


	mediaListDeleteItemCmd::mediaListDeleteItemCmd(int nID,CWnd *pTarget):baseActionCmd(nID,pTarget){
		iIndex = -1;
		this->pDrawObj = NULL;
	}

	mediaListDeleteItemCmd::~mediaListDeleteItemCmd(){
		if(this->m_nOpt == OPT_UNDO)
			MediaSetting.reset();
			//delete this->pMediaSetting;
	}   

	int mediaListDeleteItemCmd::redo(){
		__super::redo();

		CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

		ASSERT(pOwner);
		ASSERT(pPage);

		BOOL bFound = FALSE;
		std::shared_ptr<LayerElement> layInfo = pPage->m_LayoutElement->GetLayerInfo(pDrawObj->GetLayerId(), bFound);
		if (!bFound || layInfo.get() == NULL)
		{
			ASSERT(FALSE);
			return FAILURE;
		}
		layInfo->RemoveAt(iIndex);
#if 0
		//
		//	CS3DrawObj *pObj = pSelectObj->GetHead();
		LAYERINFO *pLayerInfo = NULL;
		if (!pPage->m_pLayoutSetting->Layers.Lookup(pDrawObj, pLayerInfo))
		{
			//pLayerInfo = NULL;
			ASSERT(FALSE);
			return FAILURE;
		}

		POSITION pos = pLayerInfo->FindIndex(this->iIndex);
		pLayerInfo->RemoveAt(pos);
#endif 
		
		pOwner->SelectDrawObj(this->pDrawObj);
		ReselectObj(&pOwner->m_selectObjList);

		return SUCCESS;
	}

	int mediaListDeleteItemCmd::undo()
	{
		__super::undo();
		CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

		ASSERT(pOwner);
		ASSERT(pPage);

		//
		//	CS3DrawObj *pObj = pSelectObj->GetHead();
		BOOL bFound = FALSE;
		std::shared_ptr<LayerElement> layInfo = pPage->m_LayoutElement->GetLayerInfo(pDrawObj->GetLayerId(), bFound);
		if (!bFound || layInfo.get() == NULL)
		{
			ASSERT(FALSE);
			return FAILURE;
		}

		//Restore item to oranginal index.
		if(iIndex <= 0)
			layInfo->AddHead(MediaSetting);
		else if(iIndex >= layInfo->GetMediasCount())
			layInfo->AddTailMediaElement(MediaSetting);
		else
		{
		
			layInfo->Insert(MediaSetting, iIndex);
			/*	POSITION pos = pLayerInfo->GetHeadPosition();
			for( int i = 0;i < iIndex;i++)
			pLayerInfo->GetNext(pos);
			pLayerInfo->InsertBefore(pos,pMediaSetting);*/
		}
	
		pOwner->SelectDrawObj(this->pDrawObj);
		ReselectObj(&pOwner->m_selectObjList);
		return SUCCESS;
	}
	int mediaListDeleteItemCmd:: ondo()
	{
		__super::ondo();
		CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;		
		ASSERT(pOwner);

		if(pOwner->m_selectObjList.GetSize() > 1 || 
				pOwner->m_selectObjList.GetSize() <= 0)
				return FAILURE;
		if(iIndex < 0)
			return FAILURE;
		
		this->pDrawObj = pOwner->m_selectObjList.GetHead();
		return SUCCESS;
	}




#if 0
   //
   //Layer Playback Check Ctrl
   //
   layerPlaybackCheckCmd::layerPlaybackCheckCmd(int nID ,CWnd *pTarget):_baseCmd(nID,pTarget){
        pSetting = NULL;
        pcurObj = NULL;
   }

   layerPlaybackCheckCmd::~layerPlaybackCheckCmd(){}
   int  layerPlaybackCheckCmd::redo()
   {
       __super::redo();
       CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
       ASSERT(pOwner);
       pOwner->SelectAllDrawObj(1);
       pOwner->SelectDrawObj(pcurObj,1);

       return SUCCESS;
   }
   int  layerPlaybackCheckCmd::undo()
   {
       __super::undo();
       CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
       ASSERT(pOwner);
       pOwner->SelectAllDrawObj(1);
       pOwner->SelectDrawObj(pcurObj,1);

       //if(pSetting->)
       return SUCCESS;
   }
   int  layerPlaybackCheckCmd::ondo()
   {
       __super::ondo();

       CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
       ASSERT(pOwner);
       if(pOwner->m_selectObjList.GetSize() > 1 || pOwner->m_selectObjList.GetSize() <= 0)
           return FAILURE;
       pcurObj = pOwner->m_selectObjList.GetHead();

       CPropertyBasicContent *pBaseContent = GetPropertyBaseContent();
       ASSERT(pBaseContent);
       if(!pBaseContent)
           return FAILURE;

       pSetting = pBaseContent->m_pMedia;
       if(!pSetting)
           return FAILURE;

       
       return SUCCESS;
   }

#endif
#if 0
   //
   //Layer Transparentcy
   //
 
    layerTransparencyCmd:: layerTransparencyCmd(int nID ,CWnd *pTarget):_baseCmd(nID,pTarget){
        pSetting = NULL;
        pcurObj = NULL;
    }

    layerTransparencyCmd::~layerTransparencyCmd(){}
    int  layerTransparencyCmd::redo()
    {
        __super::redo();
         CPropertyBasicContent *pBaseContent = GetPropertyBaseContent();
        ASSERT(pBaseContent);
        if(!pBaseContent)
            return FAILURE;

        //select obj again.
        if(!pBaseContent->IsWindowVisible())
            SelectDrawObj(pcurObj);
        else
            SelectDrawObj(pcurObj,FALSE);

        if(pSetting->MediaType == szTypeName_Text)
        {
            pSetting->pTextSetting->Transparency = tupleVals.get<1>();
        }
        else if ( pSetting->MediaType == szTypeName_Video || pSetting->MediaType == _T("S3ImageViewer"))
        {
            pSetting->BGColor = tupleVals.get<1>();
        }

        pBaseContent->m_ctlTransparency.SetPos(tupleVals.get<1>());
        return SUCCESS;
    }

    int  layerTransparencyCmd::undo()
    {
        __super::undo();
        
        CPropertyBasicContent *pBaseContent = GetPropertyBaseContent();
        ASSERT(pBaseContent);
        if(!pBaseContent)
            return FAILURE;

        //select obj again.
        if(!pBaseContent->IsWindowVisible())
            SelectDrawObj(pcurObj);
        else
            SelectDrawObj(pcurObj,FALSE);

        if(pSetting->MediaType == szTypeName_Text)
        {
            pSetting->pTextSetting->Transparency = tupleVals.get<0>();
        }
        else if ( pSetting->MediaType == szTypeName_Video || pSetting->MediaType == _T("S3ImageViewer"))
        {
            pSetting->BGColor = tupleVals.get<0>();
        }

        
        pBaseContent->m_ctlTransparency.SetPos(tupleVals.get<0>());
        return SUCCESS;
    }

    int  layerTransparencyCmd::ondo()
    {
        __super::ondo();
        CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
        ASSERT(pOwner);
        if(pOwner->m_selectObjList.GetSize() > 1 || pOwner->m_selectObjList.GetSize() <= 0)
            return FAILURE;
        pcurObj = pOwner->m_selectObjList.GetHead();

        CPropertyBasicContent *pBaseContent = GetPropertyBaseContent();
        ASSERT(pBaseContent);
        if(!pBaseContent)
            return FAILURE;

        pSetting = pBaseContent->m_pMedia;
        if(!pSetting)
            return FAILURE;
        
        int nTransPos = pBaseContent->m_ctlTransparency.GetPos();
        if(pSetting->MediaType == szTypeName_Text)
        {
            tupleVals.get<0>() = pSetting->pTextSetting->Transparency;
            tupleVals.get<1>() = nTransPos;
            pSetting->pTextSetting->Transparency = tupleVals.get<1>();
        }
        else if ( pSetting->MediaType == szTypeName_Video || pSetting->MediaType == _T("S3ImageViewer"))
        {
            tupleVals.get<0>()= pSetting->BGColor;
            tupleVals.get<1>() = (pSetting->BGColor & 0x00ffffff) | ((DWORD)((float)nTransPos * 2.55) << 24);
            pSetting->BGColor = tupleVals.get<1>();
        }

        
        return SUCCESS;
    }


   //
   //Layer  Refresh Interval
   //
    layerRefreshIntervalCmd::layerRefreshIntervalCmd(int nID ,CWnd *pTarget):_baseCmd(nID,pTarget){
        pSetting = NULL;
        pcurObj = NULL;
    }

    layerRefreshIntervalCmd::~layerRefreshIntervalCmd(){}

    int layerRefreshIntervalCmd::redo()
    {
        __super::redo();
         CPropertyBasicContent *pBaseContent = GetPropertyBaseContent();
        ASSERT(pBaseContent);
        if(!pBaseContent)
            return FAILURE;
        //select obj again.
        if(!pBaseContent->IsWindowVisible())
            SelectDrawObj(pcurObj);
        else
            SelectDrawObj(pcurObj,FALSE);

        if(!pSetting)
            return FAILURE;

        int h = tupleTimes.get<1>() / 3600;
        int m = (tupleTimes.get<1>() % 3600)/60;
        int s = (tupleTimes.get<1>() % 3600) % 60;
        pBaseContent->m_refreshTime.SetTime(h,m,s);
        pBaseContent->UpdateData(1);
        return SUCCESS;
    }
    int layerRefreshIntervalCmd::undo()
    {
        __super::undo(); 
        
        CPropertyBasicContent *pBaseContent = GetPropertyBaseContent();
        ASSERT(pBaseContent);
        if(!pBaseContent)
            return FAILURE;
        //select obj again.
        if(!pBaseContent->IsWindowVisible())
            SelectDrawObj(pcurObj);
        else
            SelectDrawObj(pcurObj,FALSE);

       
        if(!pSetting)
            return FAILURE;

        int h = tupleTimes.get<0>() / 3600;
        int m = (tupleTimes.get<0>() % 3600)/60;
        int s = (tupleTimes.get<0>() % 3600) % 60;
        pBaseContent->m_refreshTime.SetTime(h,m,s);
        pBaseContent->UpdateData(1);
        return SUCCESS;
    }
    int layerRefreshIntervalCmd::ondo()
    {
        __super::ondo();

        CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
        ASSERT(pOwner);
        if(pOwner->m_selectObjList.GetSize() > 1 || pOwner->m_selectObjList.GetSize() <= 0)
            return FAILURE;
        pcurObj = pOwner->m_selectObjList.GetHead();

        CPropertyBasicContent *pBaseContent = GetPropertyBaseContent();
        ASSERT(pBaseContent);
        if(!pBaseContent)
            return FAILURE;

        pSetting = pBaseContent->m_pMedia;
        if(!pSetting)
            return FAILURE;
        tupleTimes.get<0>() =  pSetting->RefreshInterval ;
        tupleTimes.get<1>() =  (pBaseContent->m_refreshTime.GetHour() *60 + 
        pBaseContent->m_refreshTime.GetMinute()) * 60 + pBaseContent->m_refreshTime.GetSecond();

        if(tupleTimes.get<1>() == 0)
            tupleTimes.get<1>() = 1;

        if(tupleTimes.get<0>() == tupleTimes.get<1>())
            return FAILURE;

        pSetting->RefreshInterval = tupleTimes.get<1>();
        return SUCCESS;
    }
#endif

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //property layer basic content
    //
    layerBasicContentChangeCmd::layerBasicContentChangeCmd(int nID,CWnd *pTarget): baseActionCmd(nID,pTarget)
    {
        pcurObj = NULL;
        pcurMedia = NULL;
        nTransparency = 51;
    }
    layerBasicContentChangeCmd::~layerBasicContentChangeCmd(){}
    BOOL layerBasicContentChangeCmd::SaveSwapMedia()
    {
         CPropertyBasicContent *pContent = GetPropertyBaseContent();
         if(!pContent)
             return FALSE;
       
         pcurMedia = pContent->m_MediaInfo;

         if(pcurMedia)
             this->swapMedia = *pcurMedia;

         if(pcurMedia->GetMediaType() == szTypeName_Text)
         {
             shared_ptr<S3SIGNAGE_TEXT_SETTING> setting = pcurMedia->GetTextSetting();
             if(setting != NULL)
                 nTransparency =setting->Transparency;
         }
         return TRUE;
    }

    int layerBasicContentChangeCmd::redo()
    {
        __super::redo();
          CPropertyBasicContent *pContent = GetPropertyBaseContent();
         if(!pContent)
             return FAILURE;
         if(!pContent->IsWindowVisible())
             SelectDrawObj(pcurObj);  
         else
             SelectDrawObj(pcurObj,FALSE);

         if(!pcurMedia)
             return FAILURE;

         MediaElement setting = *pcurMedia.get();
         *pcurMedia = swapMedia; 
         
         if(pcurMedia->GetMediaType() == szTypeName_Text)
         {
             shared_ptr<S3SIGNAGE_TEXT_SETTING> setting = pcurMedia->GetTextSetting();;
             if(setting != NULL)
             {
                 int n = setting->Transparency;
                 setting->Transparency = nTransparency;
                 nTransparency = n;
             }
         }
         pContent->SetContent(pcurMedia);
         swapMedia = setting;

        return SUCCESS;
    }
    int layerBasicContentChangeCmd::undo()
    {
        __super::undo();
          CPropertyBasicContent *pContent = GetPropertyBaseContent();
         if(!pContent)
             return FAILURE;
          if(!pContent->IsWindowVisible())
             SelectDrawObj(pcurObj);
         else
             SelectDrawObj(pcurObj,FALSE);

         if(!pcurMedia)
             return FAILURE;
         MediaElement setting = *pcurMedia.get();
         *pcurMedia = swapMedia; 
         
         if(pcurMedia->GetMediaType() == szTypeName_Text)
         {
             shared_ptr<S3SIGNAGE_TEXT_SETTING> setting = pcurMedia->GetTextSetting();
             if(setting != NULL)
             {
                 int n = setting->Transparency;
                 setting->Transparency = nTransparency;
                 nTransparency = n;
             }
         }

         pContent->SetContent(pcurMedia);
         swapMedia = setting;

        
        return SUCCESS;
    }

    int layerBasicContentChangeCmd::ondo()
    {
        __super::ondo();
        CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
        ASSERT(pOwner);
        if(pOwner->m_drawObjList.GetCount() <=0 || 
            pOwner->m_selectObjList.GetCount() <= 0 || 
            pOwner->m_selectObjList.GetCount() > 1)
            return FAILURE;

        pcurObj =  pOwner->m_selectObjList.GetHead();

         CPropertyBasicContent *pContent = GetPropertyBaseContent();
         if(!pContent)
             return FAILURE;
         if(!pcurMedia)
             return FAILURE;

         BOOL bFailure = FALSE;
         if(swapMedia == *pcurMedia.get())
         {
             bFailure = TRUE;
             if(pcurMedia->GetMediaType() == szTypeName_Text)
             {
                 shared_ptr<S3SIGNAGE_TEXT_SETTING> setting = pcurMedia->GetTextSetting();
                 if(setting != NULL)
                 if(nTransparency != setting->Transparency)
                    bFailure = FALSE;
             }
              
         }
       
         if(bFailure)
             return FAILURE;
        return SUCCESS;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //layer text setting :font ,font size ,font color ,font style(bold,italy , under line)
    //
    layerTextSettingCmd::layerTextSettingCmd(int nID ,CWnd *pTarget):baseActionCmd(nID,pTarget)
    {
        pcurObj = NULL;
        pcurSetting = NULL;
    }
    layerTextSettingCmd::~layerTextSettingCmd(){}

    BOOL layerTextSettingCmd::SaveSwapSetting()
    {
         CPropertyMessage *pProper = GetPropertyMessage();
        ASSERT(pProper);
        if(!pProper)
            return FALSE;
      
        pcurSetting = pProper->m_pMessage;
        if(pcurSetting)
        swapSetting = *pcurSetting ;

        return TRUE;
    }

    BOOL layerTextSettingCmd::IsNullSetting(S3SIGNAGE_TEXT_SETTING *pSetting)
    {
        if(NULL == pSetting)
            return FALSE;
       S3SIGNAGE_TEXT_SETTING  setting;
       if(setting == *pSetting)
            return TRUE;
        return FALSE;
    }

    int  layerTextSettingCmd::ondo()
    {
        __super::ondo();

        CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
        ASSERT(pOwner);
        if(pOwner->m_drawObjList.GetCount() <=0 || 
            pOwner->m_selectObjList.GetCount() <= 0 || 
            pOwner->m_selectObjList.GetCount() > 1)
            return FAILURE;

        pcurObj =  pOwner->m_selectObjList.GetHead();

        CPropertyMessage *pProper = GetPropertyMessage();
        ASSERT(pProper);
        if(!pProper)
            return FAILURE;
        if(!pcurSetting)
            return FAILURE;

        if(swapSetting == *pcurSetting)
            return FAILURE;

        return SUCCESS;
    }

    int  layerTextSettingCmd::redo()
    {   
        __super::redo();  
        CPropertyMessage *pProper = GetPropertyMessage();
        ASSERT(pProper);
        if(!pProper)
            return FAILURE;
        //select obj again.
        if(!pProper->IsWindowVisible())
            SelectDrawObj(pcurObj);
        else
            SelectDrawObj(pcurObj,FALSE);

        //ScrollWndToBottom(pProper->GetParent());
      
        if(!pcurSetting)
            return FAILURE;

        S3SIGNAGE_TEXT_SETTING setting = *pcurSetting;
        *pcurSetting = swapSetting;
        pProper->SetContent(pcurSetting);
        swapSetting = setting;

        return SUCCESS;
    }

    int  layerTextSettingCmd::undo()
    {
        __super::undo();
        //select obj again. 
        CPropertyMessage *pProper = GetPropertyMessage();
        ASSERT(pProper);
        if(!pProper)
            return FAILURE;
        if(!pProper->IsWindowVisible())
            SelectDrawObj(pcurObj);
        else
            SelectDrawObj(pcurObj,FALSE);
        //ScrollWndToBottom(pProper->GetParent());

        if(!pcurSetting)
            return FAILURE;

        S3SIGNAGE_TEXT_SETTING setting = *pcurSetting;
        *pcurSetting = swapSetting;
        pProper->SetContent(pcurSetting);
        swapSetting = setting;

        return SUCCESS;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Transition
    //
    layerTransitionCmd::layerTransitionCmd(int nID ,CWnd *pTarget):baseActionCmd(nID,pTarget)
    {
        pcurObj = NULL;
        pcurTrans = NULL;
    }  

    layerTransitionCmd::~layerTransitionCmd(){}

    BOOL layerTransitionCmd::SaveSwapTrans()
    {
        CPropertyTransition *pProper = GetPropertyTranstion(bInOut);
        ASSERT(pProper);
        if(!pProper)
            return FALSE;

        pcurTrans = pProper->m_pTransition;
        if(pcurTrans)
        swapTransition = *pProper->m_pTransition ;
        return TRUE;
    }

    BOOL layerTransitionCmd::IsNullTrans(S3SIGNAGE_TRANSIION *pTrans)
    {

        S3SIGNAGE_TRANSIION nullTrans;
        if(*pTrans == nullTrans)
            return TRUE;
        return FALSE;
    }

    int layerTransitionCmd::ondo()
    {
         __super::ondo();
         CLayoutDesignerCtrl *pOwner = (CLayoutDesignerCtrl *) this->m_pTargetWnd;
         ASSERT(pOwner);
         if(pOwner->m_drawObjList.GetCount() <=0 || 
             pOwner->m_selectObjList.GetCount() <= 0 || 
             pOwner->m_selectObjList.GetCount() > 1)
             return FAILURE;
         //keep current obj pointer.
         pcurObj = pOwner->m_selectObjList.GetHead();
         CPropertyTransition *pProper = GetPropertyTranstion(bInOut);
         ASSERT(pProper);
         if(!pProper)
             return FAILURE;

         if(!pcurTrans)
             pcurTrans =  pProper->m_pTransition;
         if(!pcurTrans)
             return FAILURE;

         if(this->swapTransition == *pcurTrans)
             return FAILURE;
        return SUCCESS;
    }

    int layerTransitionCmd::redo()
    {
         __super::redo();    
         CPropertyTransition *pProper = GetPropertyTranstion(bInOut);
         ASSERT(pProper);
         if(!pProper)
             return FAILURE;

         if(!pProper->IsWindowVisible())
            SelectDrawObj(pcurObj);
          else
            SelectDrawObj(pcurObj,FALSE);

        // ScrollWndToBottom(pProper->GetParent());
     
         S3SIGNAGE_TRANSIION trans = *pcurTrans;
         *pcurTrans = this->swapTransition;
         pProper->SetContent(pcurTrans);
         swapTransition = trans;

         return SUCCESS;
    }

    int layerTransitionCmd::undo()
    {
         __super::undo();
         CPropertyTransition *pProper = GetPropertyTranstion(bInOut);
         ASSERT(pProper);
         if(!pProper)
             return FAILURE;

         if(!pProper->IsWindowVisible())
            SelectDrawObj(pcurObj);
         else
            SelectDrawObj(pcurObj,FALSE);
        // ScrollWndToBottom(pProper->GetParent());

         S3SIGNAGE_TRANSIION trans = *pcurTrans;
         *pcurTrans= this->swapTransition;
         pProper->SetContent(pcurTrans);
         swapTransition = trans;
         return SUCCESS;
    }

};   