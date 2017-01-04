
#include "stdafx.h"
#include "Controller.h"
#include "LayoutDesignerCmds.h"
#include "LayoutDesignerCtrl.h"
#include "DesignerPage.h"
#include "RulerCtrl.h"
#include "Localization/Localization.h"
#include "propertysubCmds.h"
using namespace LAYOUTPROPERTYCMDS;
namespace DESIGNERCMDS
{
	#define _ADDIN_ACTION_SUPPORT

	//Util functions
	LPCTSTR GetTypeName(  const PROPERTY_TYPE propertyType)
	{
		switch(propertyType)
		{
		case BACKGROUND:
			return _T("Layout");
		case LAYER:
			return szTypeName_Layer;
			break;
		case MESSAGE:
			return  szTypeName_Message;
			
	//	case USER_DEFINED_PLUGIN://To find out plugin color that user defined.
		//	szTypeName = szMediaType;
			
		case AUDIO:
			return szTypeName_Audio;
		
		default:
			return _T("");
		}
	}

	BOOL RemoveDrawObjFromList(CS3DrawObjList *pInsts,CS3DrawObj *pObj)
	{
		ASSERT(pInsts!=NULL);
		ASSERT(pObj!=NULL);

		if(pInsts->GetCount() <= 0)
		return FALSE;

		POSITION itPos = pInsts->GetHeadPosition();
		while(itPos)
		{
			CS3DrawObj *p = pInsts->GetAt(itPos);
			if( p == pObj)
			{
				POSITION tpPos =  itPos;
				pInsts->GetNext(itPos); 

				pInsts->RemoveAt(tpPos);
				
				continue;
				//return TRUE;
			}
			pInsts->GetNext(itPos);
		}
		return TRUE;
	}

	BOOL InsertToObjListByIndex(CS3DrawObjList *pInsts,CS3DrawObj *pObj,int instPos)
	{
		if(instPos <= 1) //head is bkgnd obj.
		{
			pInsts->InsertAfter(pInsts->GetHeadPosition(),pObj);
			return TRUE;
		}

		POSITION itPos = pInsts->GetHeadPosition();
		//pInsts->GetNext(itPos);
		for( int i = 1;i < instPos;i++)
		{
			ASSERT(itPos != NULL);
			pInsts->GetNext(itPos);
		}

		if(NULL == itPos)
		{
			pInsts->AddTail(pObj);
			return TRUE;
		}

		pInsts->InsertAfter(itPos,pObj);
		return TRUE;
	}

	BOOL FindObjInObjList(CS3DrawObj *pObj,CS3DrawObjList *pList ,POSITION  & pos ,int &index)
	{
		index = 0;
		pos = pList->GetHeadPosition();
		while(pos)
		{
			CS3DrawObj *p = pList->GetAt(pos);
			if(p == pObj)
			{
				return TRUE;
			}
			pList->GetNext(pos);
			index++;// >= 1
		}

		return FALSE;
	}
	//

	BOOL RemoveInst(CS3DrawObj* pInst,CLayoutDesignerCtrl *pOwner,CDesignerPage *pPage)
	{
		  ASSERT(pInst!=NULL);
		  ASSERT(pOwner!=NULL);
		  ASSERT(pPage!=NULL);

		  CS3DrawObjList drawList;
		  pOwner->RemoveDrawObj(pInst);
		  drawList.AddTail(pInst);
		  pPage->SendMessage(WM_ON_LAYOUT_REMOVE_OBJ, 1, (LPARAM)&drawList);

		  return TRUE;
	}
	//remove obj in system ,but dont clear obj ref in designerctrl.

	BOOL RemoveInsts( CS3DrawObjList *pInsts,CLayoutDesignerCtrl *pOwner,CDesignerPage *pPage)
	{
		ASSERT(pInsts!=NULL);
		ASSERT(pOwner!=NULL);
		ASSERT(pPage!=NULL);
		
		PROPERTY_TYPE proType;
		POSITION pos = pInsts->GetHeadPosition();
		while (pos)
		{
			CS3DrawObj *pObj = pInsts->GetNext(pos);
			pObj->GetPropertyType(proType);
			pOwner->RemoveDrawObj(pObj);
		}
		pPage->SendMessage(WM_ON_LAYOUT_REMOVE_OBJ, 1, (LPARAM)pInsts);
		return TRUE;
	}

	bool RectInRect(FRect fRect ,FRect fDeskRect)
	{
		if(fDeskRect.left <= fRect.left && fDeskRect.right >= fRect.right
			&& fDeskRect.top <= fRect.top && fDeskRect.bottom >= fRect.bottom)
			return true;
		return false;
	}

	void getObjsInRect( CS3DrawObjList *pSource,OBS_INFO *pDesk,FRect selRect)
	{
		ASSERT(pSource);
		ASSERT(pDesk);
	
		CS3DrawObjList &objs = *pSource;
		OBS_INFO &objsList = *pDesk;
		POSITION posObj = objs.GetHeadPosition();
		while (posObj)
		{
			CS3DrawObj* pObj = objs.GetNext(posObj);
			PROPERTY_TYPE proType;
			pObj->GetPropertyType(proType);
			if (proType != BACKGROUND)
			{
				FRect objRect;
				pObj->GetLogicPos(objRect);
				if (/*pObj->Intersects(selRect)*/RectInRect(objRect,selRect))
				{
					BOOL bIn = FALSE;
					for(size_t i = 0;i < objsList.size();i++)
					{
						OBJ_ORG_INFO & oinf =  objsList[i];
						if(oinf.pInst == pObj)
						{
							bIn = TRUE;
							break;
						}
					}
					if(FALSE == bIn)
					{
						TRACE(_T("objMoveCmd::ondo() ::Add An Obj!\n"));
						OBJ_ORG_INFO inf;
						inf.pInst = pObj;
						pObj->GetLogicPos(inf.orgRect);
						objsList.push_back(inf);
					}
				}
			}
		}
	}

	void CopyObjsList( CS3DrawObjList *pSource,CS3DrawObjList *pDesk,BOOL bRevert = FALSE)
	{
		ASSERT(pSource);
		ASSERT(pDesk);

		POSITION pos = pSource->GetHeadPosition();
		while(pos)
		{
			if(bRevert)
				pDesk->AddHead(pSource->GetNext(pos));
			else
				pDesk->AddTail(pSource->GetNext(pos));
		}
	}


	void FreeObjLayerInfo(std::shared_ptr<LayerElement> &pInstLayerElement)
	{
		ASSERT(pInstLayerElement.get() != NULL);
		if(pInstLayerElement.get() == NULL)return;

#if 0        
		if(pLayer->GetCount())
		{
			POSITION pos = pLayer->GetHeadPosition();
			while(pos)
			{
				MEDIASETTING *pSetting = pLayer->GetAt(pos);
				if(pSetting->pTextSetting)
				{
					if(pSetting->pTextSetting->pFontInfo)
						delete pSetting->pTextSetting->pFontInfo;
					delete pSetting->pTextSetting;
				}

				delete pSetting;

				pLayer->GetNext(pos);
			}
		}
		delete pLayer;
#endif

		vector<std::shared_ptr<MediaElement>> medias = pInstLayerElement->GetMediaList();
		vector<std::shared_ptr<MediaElement>> :: iterator it;
		for(it = medias.begin(); it != medias.end(); it++)
		{
			std::shared_ptr<S3SIGNAGE_TEXT_SETTING> pTextSetting = (*it)->GetTextSetting();
			if (pTextSetting.get() != NULL)
			{
				if(pTextSetting->pFontInfo)
					delete pTextSetting->pFontInfo;
				pTextSetting.reset();
			}

			(*it).reset();

		}
		pInstLayerElement.reset();
	}
	/*
		get last slibing obj by create order.
	*/
	 CS3DrawObj *  GetLastCreateObjInUndoCmds( LayoutCmdEngine *pEngine,CS3DrawObj *pObj)
	 {
		// LayoutCmdEngine *pEngine = pDesigner->GetCmdEngine();
		 ASSERT(pEngine);

		 CMD_STACK &cmds = pEngine->GetUndoCmds();
		 CMD_STACK::container_type  _Contain = cmds._Get_container();
		 CMD_STACK::container_type::reverse_iterator it = _Contain.rbegin();

		 if(cmds.size() <= 0)
			 return NULL;

		 if(NULL == pObj)//find first "build cmd" and return drawobj.
		 while(it != _Contain.rend())
		 {
			 _baseCmd *pCmd = *it;
			 if(pCmd->getID() == CMD_OBJBUILD)
			 {
				 objBuildCmd *pBuildCmd = (objBuildCmd *)pCmd;
				 if(pBuildCmd->pContainObj)
					 return pBuildCmd->pContainObj;
				 if(pBuildCmd->pInstance)
					 return pBuildCmd->pInstance;
			 }
			 it++;
		 }

		 //
		 //The first to find out "cmd" that created the pObj
		 //
		 while(it != _Contain.rend())
		 {
			 _baseCmd *pCmd = *it;
			 if(pCmd->getID() == CMD_OBJBUILD)
			 {
				objBuildCmd *pBuildCmd = (objBuildCmd *)pCmd;
				if(pBuildCmd->pContainObj == pObj)
					return pObj;
				if( pBuildCmd->pInstance == pObj)//Has found "cmd" that created pObj.
				{
					it++;
					while(it != _Contain.rend())//continue to found the last slibing cmd.
					{
						pCmd = *it;
						if(pCmd->getID() == CMD_OBJBUILD)//find it.
						{
							 pBuildCmd = (objBuildCmd *)pCmd;
							 if(pBuildCmd->pContainObj)
								 return pBuildCmd->pContainObj;
							 if(pBuildCmd->pInstance)
								 return pBuildCmd->pInstance;

						}
						it++;
					}

					return NULL;//not found 
				}
			 }
			 it++;
		 }

		 return NULL;
	 }
	/*
	*/

	template<class T>
	T *LayoutCreateCmd(CLayoutDesignerCtrl *pDesigner,int nCmd ,int nType = 0)
	{
		LayoutCmdEngine *pEngine = pDesigner->GetCmdEngine();
		LayoutCmdFactory * pFactory = (LayoutCmdFactory *) pEngine->getFactory();
		if(!nType)
			return (T *)pFactory->newCmd(nCmd);
		return (T *)pFactory->newCmdBegin(nCmd);
	}   
	//pos is invalidate ,when delete obj from list,so i must keep index of list.

	//
	//objBuildCmd
	//
	_baseLayoutWndCmd::_baseLayoutWndCmd(int nID,CWnd *pTarget):_baseCmd(nID,pTarget){
	}

	_baseLayoutWndCmd::~_baseLayoutWndCmd(){
	}

	int _baseLayoutWndCmd::redo()
	{
		__super::redo();
		if(this->m_subCmds.empty())
			return FAILURE;
		for(SUB_CMDS::iterator it = m_subCmds.begin();
			it != m_subCmds.end();it++)
		{
			_baseCmd *pCmd = *it;
			pCmd->redo();
		}
		return SUCCESS;
	}

	int _baseLayoutWndCmd::undo()
	{
		__super::undo();
		if(this->m_subCmds.empty())
			return FAILURE;
		for(SUB_CMDS::iterator it = m_subCmds.begin();
								it != m_subCmds.end();it++)
		{
			_baseCmd *pCmd = *it;
			pCmd->undo();
		}
		return SUCCESS;
	}

	int _baseLayoutWndCmd::ondo()
	{
		__super::ondo();
		wndScrollCmd * pCmd = LayoutCreateCmd<wndScrollCmd>((CLayoutDesignerCtrl *)this->m_pTargetWnd,CMD_WNDSCROLL);
		int nRet =  pCmd->ondo();
		this->m_subCmds.push_back(pCmd);
		return nRet;
	}

	/////////////////////////////////////////////////////////////////////////////////
	//
	//wndScrollCmd
	//
	wndScrollCmd::wndScrollCmd(int nID,CWnd *pTarget): _baseCmd(nID,pTarget){
		this->xFTrans = 0.0;
		this->yFTrans = 0.0;
		this->fZoomScale = 1.0;
		this->nVScrollPos = 0;
		this->nHScrollPos = 0;
		this->cPageRect.SetRectEmpty();
		this->viewPort.SetRectEmpty();
	 
	}
	wndScrollCmd::~wndScrollCmd(){}

	//
	//根据要恢复的当前点 和 layout size ,计算合理范围的回滚距离 ?
	//
	int wndScrollCmd::RetoreStatus()
	{
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

        DWORD dwStyle = pOwner->GetStyle();
        if(!(dwStyle & WS_VSCROLL) && !(dwStyle & WS_HSCROLL))
        {
            pPage->SendMessage(WM_ON_LAYOUT_ZOOM,0, (LPARAM)&fZoomScale);
            return SUCCESS;
        }

		ASSERT(pPage);
		ASSERT(pOwner);
        //layout window size is changed.
        SCROLLINFO   OrigsiH;
        pOwner->GetScrollInfo(SB_HORZ, &OrigsiH);
        SCROLLINFO   OrigsiV;
        pOwner->GetScrollInfo(SB_VERT, &OrigsiV);  

        CRect curPageRect  = cPageRect;
       // pOwner->GetClientRect(curPageRect);

        int   nXCurPos = nHScrollPos;
        int   nYCurPos = nVScrollPos;
        FLOAT xFitTrans = xFTrans;
        FLOAT yFitTrans = yFTrans;

 /*       FLOAT curXTransMax =  - (OrigsiH.nMax - OrigsiH.nMin - OrigsiH.nPage ) * pOwner->m_Scale;
        FLOAT curXTransMin =  -  OrigsiH.nMin * pOwner->m_Scale;
        FLOAT curYTransMax =  - (OrigsiV.nMax - OrigsiH.nMin - OrigsiV.nPage ) * pOwner->m_Scale;
        FLOAT curYTransMin =  -  OrigsiV.nMin * pOwner->m_Scale;

        if(xFitTrans < curXTransMin)
        {
            nXCurPos =  OrigsiH.nMin;
            xFitTrans = curXTransMin;
        }
        if(xFitTrans > curXTransMax)
        {
             nXCurPos =  OrigsiH.nMax - OrigsiH.nMin - OrigsiH.nPage;
             xFitTrans = curXTransMax;
        }
        
        if(yFitTrans < curYTransMin)
        {
            nYCurPos =  OrigsiV.nMin;
            yFitTrans = curYTransMin;
        }
        if(yFitTrans > curYTransMax)
        {
            nYCurPos =  OrigsiV.nMax - OrigsiV.nMin - OrigsiV.nPage;
            yFitTrans = curYTransMax;
        }*/

        pOwner->m_XTrans = yFitTrans;
        pOwner->m_YTrans = yFitTrans;
        pOwner->m_HscrollPos = nXCurPos;
        pOwner->m_VscrollPos = nYCurPos;
        pOwner->m_Scale = fZoomScale;
		pOwner->m_ViewportRect = this->viewPort;

		//restore layout ,and scroll bar
		 
		OrigsiH.nMax = viewPort.right;
		OrigsiH.nMin = viewPort.left;
		OrigsiH.nPos = nXCurPos;
		OrigsiH.nPage = curPageRect.Width();

		pOwner->SetScrollInfo(SB_HORZ,&OrigsiH,TRUE);

		 
		OrigsiV.nPos = nYCurPos;
		OrigsiV.nPage = curPageRect.Height();
		OrigsiV.nMax = viewPort.bottom;
		OrigsiV.nMin = viewPort.top;
		pOwner->SetScrollInfo(SB_VERT,&OrigsiV,TRUE);
 		pOwner->SetupScrollBars();
        pOwner->CalcTransform();       
	//Owner->Invalidate();
		//restore combobox on ribbonbar.
		pPage->SendMessage(WM_ON_LAYOUT_ZOOM,0, (LPARAM)&fZoomScale);
		return SUCCESS;
	}

	int wndScrollCmd::ondo()
	{
		__super::ondo();
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		xFTrans	= pOwner->m_XTrans;
		yFTrans = pOwner->m_YTrans;
		nVScrollPos = pOwner->m_VscrollPos;
		nHScrollPos = pOwner->m_HscrollPos;
		fZoomScale = pOwner->m_Scale;
		viewPort =  pOwner->m_ViewportRect;
		pOwner->GetClientRect(&cPageRect);
		pOwner->ClientToLayout(cPageRect);
		return SUCCESS;
	}
	int wndScrollCmd::redo()
	{
		__super::redo();

		return RetoreStatus();
	}
	int wndScrollCmd::undo()
	{
		__super::undo();
		return 	RetoreStatus();
	}

	/////////////////////////////////////////////////////////////////////////////////
	#define DEFAULT_LENGHT 256
	objBuildCmd::objBuildCmd(int nID,CWnd *pTarget): _baseLayoutWndCmd(nID,pTarget)
	{
		pObjInfo = NULL;
		pContainObj = NULL;
		pInstance = NULL;
		pAppendedSetting.reset();
		pInstLayerInfo = NULL;
	}

	objBuildCmd::~objBuildCmd()
	{
		if(pObjInfo)
			delete pObjInfo;

		if(this->m_nOpt != OPT_REDO) //ojb in undo stack.	
			return;

		if(pInstance)
		{
			
			/*CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
			CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
			if(pPage && ::IsWindow(pOwner->GetSafeHwnd()))
				RemoveInst(pInstance ,pOwner,pPage);
			else*/
				ASSERT(pInstLayerInfo);
				delete pInstance;
				
				FreeObjLayerInfo(pInstLayerElement);
				//delete pInstLayerInfo;
			
		}else
		if(pAppendedSetting.get() != NULL)
			pAppendedSetting.reset();
		
	}

	int objBuildCmd::ondo()
	{
		__super::ondo();

		TRACE(_T("objBuildCmd::ondo()! \n"));
		//
		//New Create Instance.
		//
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pPage!=NULL);
		ASSERT(pOwner!=NULL);

		FPoint fpt(this->dragPoint);
		
		pObjInfo = new OBJINFO();
		pObjInfo->proType  = LAYER;

		pOwner->ClientToLayout(fpt);
		pContainObj = pOwner->GetDrawObjAtPoint(fpt);//Useing to judge if it is inserted to an obj 
		BOOL bIsAudio = FALSE;
#ifdef STARTER_EDITION
		if (::PathFileExists(strMediaFile))
		{
			CWaitCursor wait;

			CString szMediaPath = strMediaFile;
			CLayoutDesignerCtrl::DragMediaInfo Information;

			Information.MediaID  = -1;
			Information.Duration = 0;
			Information.MediaName = strMediaFile;
			Information.MediaType =  CLayoutDesignerCtrl::DragMediaType::MEDIA_FILE;

			//MediaFilter Category;
			Media NewMedia;
			if (MediaLibrary::GenerateMediaInfo(szMediaPath.GetString(), NewMedia))
			{
				Information.Duration = NewMedia.GetDuration();
				Information.MediaName = NewMedia.GetName().c_str();

				Information.nWidth    = NewMedia.GetWidth();
				Information.nHeight  = NewMedia.GetHeight();
				nWidth = Information.nWidth;
				nHeight = Information.nHeight;
			}

			pOwner->m_DragMediaFiles.push_back(make_pair(szMediaPath, Information));

			int n = szMediaPath.ReverseFind(_T('.'));
			if (n != -1)
			{
				CString szExt = szMediaPath.Mid(n + 1);
				for (UINT i = 0; i < nAudioSupport; i++)
				{
					if (szExt.CompareNoCase(szAudioSupportType[i]) == 0)
					{
						pObjInfo->proType = AUDIO;
						break;
					}
				}
			}

		}
#else
		
		if(StringUtility::IsDigitalStr(strMediaFile.GetString()))//is media file object.
		{
			INT MediaID = StringUtility::stoi(strMediaFile.GetString());
			vector<int> Medias;
			Medias.push_back(MediaID);
			vector<Media> DragMedias;
			HRESULT hr = GetControllerInterface->GetMedias(Medias, DragMedias);
			if (SUCCEEDED(hr) && !DragMedias.empty())
			{
				CString szMediaPath = DragMedias[0].GetMediaServerPath().c_str();

				if (szMediaPath.IsEmpty())
				{
					// Unlock memory
					ShowXMessageBox(AfxGetMainWnd()->m_hWnd,Translate(_T("The media has not been uploaded to \
						media server, please upload it firstly")),\
						Translate(_T("Tips:Drop file")), MB_OK|MB_ICONEXCLAMATION);
					delete pObjInfo;
					return FAILURE;
				}

				CLayoutDesignerCtrl::DragMediaInfo Information;
				Information.MediaID  = DragMedias[0].GetMeidaID();
				Information.Duration = DragMedias[0].GetDuration();
				Information.MediaName = DragMedias[0].GetName().c_str();
				Information.nWidth    = DragMedias[0].GetWidth();
				Information.nHeight  = DragMedias[0].GetHeight();
				nWidth = Information.nWidth;
				nHeight = Information.nHeight;

				if (DragMedias[0].GetFileSize() == 0)
				{
					Information.MediaType = CLayoutDesignerCtrl::DragMediaType::MEDIA_URL;
				}
				else
				{
					Information.MediaType = CLayoutDesignerCtrl::DragMediaType::MEDIA_FILE;
				}

				pOwner->m_DragMediaFiles.push_back(make_pair(szMediaPath, Information));

				int n = szMediaPath.ReverseFind(_T('.'));
				if (n != -1)
				{
					CString szExt = szMediaPath.Mid(n + 1);
					for (UINT i = 0; i < nAudioSupport; i++)
					{
						if (szExt.CompareNoCase(szAudioSupportType[i]) == 0)
						{  
							pObjInfo->proType = AUDIO;
							bIsAudio = TRUE;
							break;
						}
					}
				}
			}
			else
			{
				TRACE1("Failed to get media info from controller server %d", MediaID);
				ShowXMessageBox(AfxGetMainWnd()->m_hWnd,Translate(_T("The media may be not exist.")), 
					Translate(_T("Tips:Drop file")), 
					MB_OK|MB_ICONINFORMATION);
				delete pObjInfo;
				return FAILURE;
			}
		}
#endif
		else
		{
			pObjInfo->proType = USER_DEFINED_PLUGIN;
			CString szName = _T("");
			if (strMediaFile.CompareNoCase(szTypeName_Message) == 0)
			{
				pObjInfo->proType = MESSAGE;
			}
			else if (strMediaFile.CompareNoCase(szTypeName_EmptyContent) == 0)
			{
				pObjInfo->proType = LAYER;
			}
		}


		//restore some context.
		if(pOwner->OnAddObject(pObjInfo,dragPoint,nWidth,nHeight) == S_OK) //add sucess
		{

			if( !pContainObj) //have no contain obj(create an instance).
			{
				pInstance = pOwner->m_drawObjList.GetTail();
				strMeidaType = GetTypeName( pObjInfo->proType);
				if(strMeidaType.IsEmpty()) // maybe user defined plugin.
					 pPage->m_wndComponentList.GetSelectedItemName(strMeidaType);
				pOwner->SelectDrawObj(NULL);
				pOwner->SelectDrawObj(pInstance);

				
				pPage->GetMediaSettingsByObj(pInstance, pInstLayerInfo);
				pPage->GetLayerInfoByObj(pInstance, pInstLayerElement);

				//LAYOUT *pSetting = pPage->m_pLayoutSetting;
				//pSetting->Layers.Lookup( pInstance,pInstLayerInfo);

				//
				//Audio only has one instance in layout,so need specific code here~
				//
				
				PROPERTY_TYPE mediaType;
				pInstance->GetPropertyType(mediaType);
				if(mediaType == AUDIO )
				{
					if(pInstLayerInfo->size()  > 1)
					{
						pContainObj = pInstance;
						pInstance = NULL;
						pAppendedSetting = *(pInstLayerInfo->rbegin());//pInstLayerInfo->GetTail();
					}
				}
				
			}else // Add stuff to obj.
			{
				ASSERT(pPage->m_LayoutElement!=NULL);
				LayoutElement  *pSetting = pPage->m_LayoutElement;
				vector<std::shared_ptr<MediaElement>>   *pLayInfo=NULL; 

				//
				//Audio only has one instance in layout,so need specific code here~
				//  
				PROPERTY_TYPE mediaType;
				pContainObj->GetPropertyType(mediaType);
				if(mediaType != AUDIO )
				if(bIsAudio) //
				{
					vector<std::shared_ptr<MediaElement>>   *pAudioLay=NULL; 
					pPage->GetMediaSettingsByObj(pOwner->m_pAudioObj, pAudioLay);
					if (pAudioLay->size() > 1)
					{
						pContainObj =  pOwner->m_pAudioObj;
					}

				}

				pPage->GetMediaSettingsByObj(pContainObj, pLayInfo);
				pAppendedSetting = *(pLayInfo->rbegin());
				pOwner->SelectDrawObj(pContainObj);

#if 0
				pSetting->Layers.Lookup( pContainObj,pLayInfo);
				pAppendedSetting = pLayInfo->GetTail();
				pOwner->SelectDrawObj(pContainObj);
#endif

			}
			//pInstance =  pContainObj;
			ReselectObj(&pOwner->m_selectObjList);
			//pPage->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 0, (LPARAM)&pOwner->m_selectObjList); 
			return SUCCESS;
		}
		return FAILURE;
	}
	   
	int objBuildCmd::redo()
	{
		__super::redo();

		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pPage!=NULL);
		ASSERT(pOwner!=NULL);

		if(pContainObj)
		{
#ifdef _ADDIN_ACTION_SUPPORT
			/*
			把已经撤销添加的内容恢复
			1.超找到对象保存信息的数据结构
			2.把加入内容信息重新加入对象数据结构。
			*/
			ASSERT(pPage->m_LayoutElement);
			LayoutElement *pSetting = pPage->m_LayoutElement;

			vector<std::shared_ptr<MediaElement>> *pMediaList = NULL;
			pPage->GetMediaSettingsByObj(pContainObj, pMediaList);
			if (pMediaList)
			{
				if (pAppendedSetting.get() != NULL)
				{
					pMediaList->push_back(pAppendedSetting);
				}
			}

			pOwner->SelectDrawObj(pContainObj);
			//pOwner->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 0, (LPARAM)&pOwner->m_selectObjList);
#endif
		}else 
		{
			pOwner->m_drawObjList.AddTail(pInstance);
			//pPage->m_DrawObjList.AddTail(pInstance);//for objlist of property 
			pOwner->SelectDrawObj(pInstance);
			 
			pPage->m_DrawObjList.RemoveAll(); //sync two same struct...
			CopyObjsList(&pOwner->m_drawObjList,&pPage->m_DrawObjList);

			ASSERT(pInstLayerInfo);
			LayoutElement *pSetting = pPage->m_LayoutElement;
			pPage->AddLayerIntoLayout(pInstance, pInstLayerElement);
			//pSetting->Layers.SetAt( pInstance,pInstLayerInfo);

			//
			//Audio only has one instance in layout,so need specific code here~
			//
			PROPERTY_TYPE mediaType;
			pInstance->GetPropertyType(mediaType);
			if(mediaType == AUDIO)
				pOwner->m_pAudioObj = pInstance;
		}
		//pPage->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 0, (LPARAM)&pOwner->m_selectObjList);
		ReselectObj(&pOwner->m_selectObjList);
		pOwner->Invalidate();
		return SUCCESS;
	}

	int objBuildCmd::undo()
	{
		__super::undo();
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pPage!=NULL);
		ASSERT(pOwner!=NULL);
		TRACE(_T("objBuildCmd::undo()! \n"));
		if(pContainObj)
		{
#ifdef  _ADDIN_ACTION_SUPPORT
			/*
			撤销已经添加的内容
			1.超找到对象保存信息的数据结构
			2.把加入内容从对象数据结构中移除。
			*/
			ASSERT(pPage->m_LayoutElement);
			//LayoutElement  *pSetting = pPage->m_LayoutElement;
			//LAYERINFO *pLayInfo=NULL; 

			vector<std::shared_ptr<MediaElement>> *pMediaList;
			pPage->GetMediaSettingsByObj(pContainObj, pMediaList);

			if (pMediaList && pMediaList->size())
			{
				vector<std::shared_ptr<MediaElement>> ::iterator it;
				for (it = pMediaList->begin(); it != pMediaList->end(); it++)
				{
					if (pAppendedSetting == (*it))
					{
						(*it).reset();
						pMediaList->erase(it);
						break;		
					}
				}
	
			}
			pOwner->SelectDrawObj(pContainObj);
#if 0
			if(pLayInfo)
			{
			
				//若undo过程中没有发现对象存在，可能用户已经手动删除该内容，则不在进行redo操作。
				POSITION pos = pLayInfo->GetHeadPosition();
				while(pos)//remove mediasetting from layinfo list.
				{
					MEDIASETTING *p = pLayInfo->GetAt(pos);
					if( p == pAppendedSetting)
					{
						pLayInfo->RemoveAt(pos);
						break;
					}
					pLayInfo->GetNext(pos);
				}

			}
			pOwner->SelectDrawObj(pContainObj);
#endif
			//pOwner->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 0, (LPARAM)&pOwner->m_selectObjList);
#endif
		}else
		{

			//对新加入的对象undo
			CS3DrawObjList objList;// = m_selectObjList.GetHeadPosition();
			CRect objRect;

			pInstance->GetPosition(objRect);
			//pOwner->m_drawObjList.RemoveTail();
			RemoveDrawObjFromList(&pOwner->m_drawObjList,pInstance);

			ASSERT(pPage->m_LayoutElement!=NULL);//map member of designerpage,to hold relation between drawobj and mediasetting.
			//pPage->GetMediaSettingsByObj(pInstance, pMediaList);

			BOOL bDel = pPage->m_LayoutElement->RemoveLayer(pInstance->GetLayerId());
			ASSERT(bDel);
			//S3SIGNAGE_LAYER  &Layer = pPage->m_pLayoutSetting->Layers;
			if (bDel)
			{
				PROPERTY_TYPE mediaType;
				pInstance->GetPropertyType(mediaType);
				if(mediaType == AUDIO)
					pOwner->m_pAudioObj = NULL;
			}


#if 0
			LAYERINFO *pLayerInfo = NULL;
			if(Layer.Lookup(pInstance,pLayerInfo))
			{
				Layer.RemoveKey(pInstance);
				ASSERT(pLayerInfo == this->pInstLayerInfo);

				//
				//Audio only has one instance in layout,so need specific code here~
				//
				PROPERTY_TYPE mediaType;
				pInstance->GetPropertyType(mediaType);
				if(mediaType == AUDIO)
					pOwner->m_pAudioObj = NULL;
			}
#endif

			pPage->m_DrawObjList.RemoveAll(); //sync two same struct...
			CopyObjsList(&pOwner->m_drawObjList,&pPage->m_DrawObjList);
			pOwner->InvalidateRect(objRect);
			
			CS3DrawObj *pLastObj =  GetLastCreateObjInUndoCmds(pOwner->GetCmdEngine(),pInstance);
			pOwner->SelectDrawObj(pLastObj);
		}
	
		ReselectObj(&pOwner->m_selectObjList);
		TRACE(_T("objBuildCmd undo : size of m_selectObjList:%d \n"),pOwner->m_selectObjList.GetSize());
		pOwner->Invalidate();
		return SUCCESS;
	}

	//
	//objMoveCmd;
	//
	void objMoveCmd::addObj(CS3DrawObj *pObj)
	{
		ASSERT(pObj);
		for( int i = 0;i < objsList.size();i++)
		{
			if(objsList[i].pInst == pObj)
				return ;
		}
		
		OBJ_ORG_INFO  obj;
		obj.pInst = pObj;
		pObj->GetLogicPos(obj.orgRect);
		objsList.push_back(obj);
	}

	objMoveCmd::objMoveCmd(int nID,CWnd *pTarget): _baseLayoutWndCmd(nID,pTarget){
		boundObj.pInst = NULL;
	}
	objMoveCmd::~objMoveCmd(){
	}

	void objMoveCmd::addBoundObj(CS3DrawObj *pObj)
	{
		if(NULL == pObj) return;
		if(boundObj.pInst) return;

		boundObj.pInst = pObj;
		pObj->GetLogicPos(boundObj.orgRect);
	}

	void objMoveCmd::restoreMove()
	{
		if(objsList.size() <= 0)
			return;
		for( int i = 0;i < objsList.size();i++)
		{
			OBJ_ORG_INFO &obj = objsList[i];
			FRect curRect;
			obj.pInst->GetLogicPos(curRect);
			obj.pInst->SetLogicPos(obj.orgRect);
			obj.orgRect = curRect;
		}

	}

	void  objMoveCmd::restoreBoundMove()
	{
		if(!boundObj.pInst)
			return;
		FRect rc;
		boundObj.pInst->GetLogicPos(rc);
		boundObj.pInst->SetLogicPos(boundObj.orgRect);
		boundObj.orgRect = rc;
	}

	int objMoveCmd::ondo()
	{
		__super::ondo();
		TRACE(_T("objMoveCmd::ondo()!\n"));
	
		return SUCCESS;
	}
	int objMoveCmd::redo() //redo is equal to redo.
	{
		__super::redo();
		TRACE(_T("objMoveCmd::redo() !\n"));
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		restoreMove();
		restoreBoundMove();
		//getObjsInRect( &pOwner->m_drawObjList,&objsList,boundObj.orgRect);

		pOwner->m_selectObjList.RemoveAll();
		for(int i = 0;i < objsList.size();i++)
			pOwner->m_selectObjList.AddTail(objsList[i].pInst);
		pOwner->m_pSelectObj = boundObj.pInst;

		if(pOwner->m_selectObjList.GetCount() <= 1)
			ReselectObj(&pOwner->m_selectObjList);
			//pPage->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 0, (LPARAM)&pOwner->m_selectObjList);
		pOwner->Invalidate();
		return SUCCESS;
	}
	int objMoveCmd::undo()
	{
		__super::undo();
		TRACE(_T("objMoveCmd::undo() !\n"));
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		restoreMove();
		//objsList.clear();
		//getObjsInRect( &pOwner->m_drawObjList,&objsList,boundObj.orgRect);
		restoreBoundMove();
		
		pOwner->m_selectObjList.RemoveAll();
		for(int i = 0;i < objsList.size();i++)
			pOwner->m_selectObjList.AddTail(objsList[i].pInst);
	
		pOwner->m_pSelectObj = boundObj.pInst;
		if(pOwner->m_selectObjList.GetCount() <= 1)
			ReselectObj(&pOwner->m_selectObjList);
			//pPage->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 0, (LPARAM)&pOwner->m_selectObjList);
		pOwner->Invalidate();
		return SUCCESS;
	}

	//
	//objSizeCmd
	//
	objSizeCmd::objSizeCmd(int nID,CWnd *pTarget): _baseLayoutWndCmd(nID,pTarget){
		pDragObj = NULL;
	}
	objSizeCmd::~objSizeCmd(){}
	int objSizeCmd::ondo()
	{
		__super::ondo();
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		pDragObj = pOwner->m_selectObjList.GetHead();
	
		pDragObj->GetLogicPos(this->storeRC);
	
		return SUCCESS;
	}
	int objSizeCmd::redo()
	{
		__super::redo();
		TRACE(_T(" objSizeCmd::redo()!\n"));
		ASSERT(pDragObj);

		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		FRect fRC ;
		pDragObj->GetLogicPos(fRC);
		pDragObj->SetLogicPos(this->storeRC);

		storeRC = fRC;
		pOwner->Invalidate();
		pOwner->SelectDrawObj(pDragObj);
		if(pOwner->m_selectObjList.GetCount() <= 1)
			ReselectObj(&pOwner->m_selectObjList);
			//pPage->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 0, (LPARAM)&pOwner->m_selectObjList);
		pOwner->Invalidate();
		return SUCCESS;
	}
	int objSizeCmd::undo()
	{
		__super::undo();
		TRACE(_T(" objSizeCmd::undo()!\n"));
		ASSERT(pDragObj);

		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		FRect fRC ;
		pDragObj->GetLogicPos(fRC);
		pDragObj->SetLogicPos(this->storeRC);

		storeRC = fRC;
		pOwner->SelectDrawObj(pDragObj);
		if(pOwner->m_selectObjList.GetCount() <= 1)
			ReselectObj(&pOwner->m_selectObjList);
			//pPage->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 0, (LPARAM)&pOwner->m_selectObjList);
		pOwner->Invalidate();
		return SUCCESS;
	}
	//
	//objDeleteCmd
	//

	objDeleteCmd::objDeleteCmd(int nID,CWnd *pTarget): _baseLayoutWndCmd(nID,pTarget){
		pCtrlSelList = NULL;
	}
	objDeleteCmd::~objDeleteCmd(){
		if(this->m_nOpt != OPT_UNDO)
			return;
		
		for(int i = 0;i < storeObjs.size();i++)
		{
			delete storeObjs[i].pInstance;
			FreeObjLayerInfo(storeObjs[i].layerInfo);
			 
		}
		
	}

	//
	//delete inst from designerctrl
	//
	int objDeleteCmd::ondo()
	{
		__super::ondo();
		TRACE(_T("objDeleteCmd::ondo()!\n"));
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

		ASSERT(pPage);
		ASSERT(pOwner);
		ASSERT(pCtrlSelList);


		if(pCtrlSelList->GetSize() <= 0)
			return FAILURE;
		
		CS3DrawObjList &drawObjList = pOwner->m_drawObjList;
		int nObjCount = pOwner->m_drawObjList.GetCount();
		if(nObjCount <= 0)
			return FAILURE;

		POSITION selPos = pCtrlSelList->GetHeadPosition();
		
		while(selPos)
		{
			CS3DrawObj *pSelObj = pCtrlSelList->GetNext(selPos);
            
			int posIndex = 0;
         POSITION objPos = drawObjList.GetHeadPosition();

			while(objPos)
			{
				CS3DrawObj *pObj = drawObjList.GetAt(objPos);
				if(pObj == pSelObj)
				{
					OBJ_INFO insInfo;
					
					insInfo.pInstance = pObj;
					insInfo.instPos = posIndex;

					//
					//remove media info from mediasetting.
					//
					BOOL bFound = FALSE;
					std::shared_ptr<LayerElement> layInfo = pPage->m_LayoutElement->GetLayerInfo(pObj->GetLayerId(), bFound);
					if (bFound && layInfo.get())
					{
						pPage->m_LayoutElement->RemoveLayer(pObj->GetLayerId());
						PROPERTY_TYPE mediaType;
						insInfo.pInstance->GetPropertyType(mediaType);
                        insInfo.layerInfo = layInfo;
						if(mediaType == AUDIO)
							pOwner->m_pAudioObj = NULL;
					}

#if 0
					if(Layers.Lookup( insInfo.pInstance,insInfo.pLayerInfo))
					{
						Layers.RemoveKey(insInfo.pInstance);

						//
						//Audio only has one instance in layout,so need specific code here~
						//
						PROPERTY_TYPE mediaType;
						insInfo.pInstance->GetPropertyType(mediaType);
						if(mediaType == AUDIO)
							pOwner->m_pAudioObj = NULL;
					}
#endif 

					storeObjs.push_back(insInfo);
					//drawObjList.RemoveAt(objPos);
					//remove from page.
					RemoveDrawObjFromList(&drawObjList,pObj);
					break;
				}
				drawObjList.GetNext(objPos);
				posIndex++;
			}
		}
		pCtrlSelList->RemoveAll();
		pPage->m_DrawObjList.RemoveAll(); //sync two same struct...
		CopyObjsList(&pOwner->m_drawObjList,&pPage->m_DrawObjList);
		pOwner->Invalidate();
		return SUCCESS;
	}

	//
	//remove inst to designerctrl.
	//
	int objDeleteCmd::redo()
	{
		__super::redo();

		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pPage);
		ASSERT(pOwner);

		CS3DrawObjList &drawObjList = pOwner->m_drawObjList;
		if(this->storeObjs.size() <= 0)
			return FAILURE;
		//S3SIGNAGE_LAYER &Layers =  pPage->m_pLayoutSetting->Layers ;//.Lookup(pObj, pLayerInfo);
		for(int i = 0;i < storeObjs.size();i++)
		{
			CS3DrawObj *pStoreObj = storeObjs[i].pInstance;
			RemoveDrawObjFromList(&drawObjList,pStoreObj);

			BOOL bFound = FALSE;
			std::shared_ptr<LayerElement> layInfo = pPage->m_LayoutElement->GetLayerInfo(pStoreObj->GetLayerId(), bFound);
			if (bFound && layInfo.get())
			{
				pPage->m_LayoutElement->RemoveLayer(pStoreObj->GetLayerId());
				ASSERT(layInfo ==  storeObjs[i].layerInfo);
				PROPERTY_TYPE mediaType;
				pStoreObj->GetPropertyType(mediaType);
				if(mediaType == AUDIO)
					pOwner->m_pAudioObj = NULL;
			}

#if 0
			LAYERINFO *pLayerInfo = NULL;
			if(Layers.Lookup(  pStoreObj,pLayerInfo))
			{
				Layers.RemoveKey(pStoreObj);
				ASSERT(pLayerInfo ==  storeObjs[i].pLayerInfo);

				//
				//Audio only has one instance in layout,so need specific code here~
				//
				PROPERTY_TYPE mediaType;
				pStoreObj->GetPropertyType(mediaType);
				if(mediaType == AUDIO)
					pOwner->m_pAudioObj = NULL;
			}
#endif 
			//RemoveDrawObjFromList(&pPage->m_DrawObjList,pStoreObj);
			//pStoreObj->GetPosition(objRect);
			//pOwner->InvalidateRect(objRect);
		}
		pPage->m_DrawObjList.RemoveAll(); //sync two same struct...
		CopyObjsList(&drawObjList,&pPage->m_DrawObjList);
		pOwner->Invalidate();
		return SUCCESS;
	}

	//
	//restore inst to designerctrl
	//
	int objDeleteCmd::undo()
	{	
		__super::undo();

		if(this->storeObjs.size() <= 0)
			return FAILURE;

		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pPage);
		ASSERT(pOwner);

		CS3DrawObjList &drawObjList = pOwner->m_drawObjList;
		//S3SIGNAGE_LAYER &Layers =  pPage->m_pLayoutSetting->Layers ;//.Lookup(pObj, pLayerInfo);
		for(size_t i = 0;i < storeObjs.size();i++)
		{
			OBJ_INFO &insInfo = storeObjs[i];
			if(drawObjList.GetCount()<=0)
				drawObjList.AddTail(insInfo.pInstance);
			else
			{
				POSITION pos = drawObjList.GetHeadPosition();
				for( int i = 1;i < insInfo.instPos;i++)//base index is 1,for "BKGND" obj.
				{
					drawObjList.GetNext(pos);
				}
				drawObjList.InsertAfter(pos,insInfo.pInstance);
				
				//pPage->m_DrawObjList.InsertBefore(pos,insInfo.pInstance);
				
			}  
			if(insInfo.layerInfo)
			{
				pPage->AddLayerIntoLayout(insInfo.pInstance, insInfo.layerInfo);
				//Layers.SetAt(insInfo.pInstance,insInfo.pLayerInfo);

				//
				//Audio only has one instance in layout,so need specific code here~
				//
				PROPERTY_TYPE mediaType;
				insInfo.pInstance->GetPropertyType(mediaType);
				if(mediaType == AUDIO)
					pOwner->m_pAudioObj = insInfo.pInstance;
			}
			pOwner->SelectDrawObj(insInfo.pInstance);

		}
		ReselectObj(&pOwner->m_selectObjList);
		//pPage->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 0, (LPARAM)&pOwner->m_selectObjList);
		//
		pPage->m_DrawObjList.RemoveAll(); //sync two same struct...
		CopyObjsList(&drawObjList,&pPage->m_DrawObjList);
		pOwner->Invalidate();
		return SUCCESS;
	}

	//
	//objToFrontCmd
	//
	//reset bkgnd postion in list.
	void EnsureDrawListBkGndPos(CS3DrawObjList &drawList)
	{
		POSITION  pos = drawList.GetHeadPosition();
		while(pos)
		{
			CS3DrawObj *pObj = drawList.GetAt(pos);
			PROPERTY_TYPE propertyType;
			pObj->GetPropertyType(propertyType);
			if(propertyType == BACKGROUND )
			{
				if(pos == drawList.GetHeadPosition()) // in right place.
					return;
				else
				{
					drawList.RemoveAt(pos);
					drawList.InsertBefore(drawList.GetHeadPosition(),pObj);
					return;
				}
			}
			drawList.GetNext(pos);
		}
	}

	objToFrontCmd::objToFrontCmd(int nID,CWnd *pTarget): _baseLayoutWndCmd(nID,pTarget){
		nType = ONE_STEP;
		pDrawObj = NULL;
		nPosIndex = 0;
	}
	objToFrontCmd::~objToFrontCmd(){}

	int objToFrontCmd::DoMove(CS3DrawObj *pObj )
	{
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pOwner);
		ASSERT(pPage);
		ASSERT(pObj);

		if(pOwner->m_drawObjList.GetCount() <= 2)
			return FAILURE;

		if(nType == ONE_STEP)//zxy...典型的逻辑内聚代码，反面教材。。
		{
			EnsureDrawListBkGndPos(pOwner->m_drawObjList);//ensure bkgnd is at top of list.
			POSITION posNext,pos = pOwner->m_drawObjList.Find(pObj);
			ASSERT(pos != NULL);
			posNext = pos;

			pOwner->m_drawObjList.GetNext(posNext);
			if(NULL == posNext)return FAILURE;
			CS3DrawObj *pObjNext = pOwner->m_drawObjList.GetAt(posNext); 

			pOwner->m_drawObjList.SetAt(pos,pObjNext);//RemoveAt(pos);
			pOwner->m_drawObjList.SetAt(posNext, pObj);
			pOwner->InvalidateDrawObj(pObj);

			pPage->m_DrawObjList.RemoveAll();
			CopyObjsList(&pOwner->m_drawObjList,&pPage->m_DrawObjList);

            pPage->AdjustLayerInfoBySortedObjs();

		}else{ //与tail交换

			EnsureDrawListBkGndPos(pOwner->m_drawObjList);// ensure bkgnd is at top of list.

			POSITION pos;// = pOwner->m_drawObjList.Find(pObj);	
			FindObjInObjList(pObj,&pOwner->m_drawObjList,pos,nPosIndex);

			pOwner->m_drawObjList.RemoveAt(pos);
			pOwner->m_drawObjList.AddTail(pObj);

			pPage->m_DrawObjList.RemoveAll();
			CopyObjsList(&pOwner->m_drawObjList,&pPage->m_DrawObjList);
            pPage->AdjustLayerInfoBySortedObjs();
		}
		pOwner->Invalidate();
		return SUCCESS;
	}

	int objToFrontCmd::ondo()
	{
		__super::ondo();
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		pDrawObj = pOwner->m_selectObjList.GetHead();
		return DoMove(pDrawObj);
	}

	int objToFrontCmd::redo()
	{
		__super::redo();
		return DoMove(pDrawObj);
	}
	int objToFrontCmd::undo()
	{
		__super::undo();

		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pOwner);
		ASSERT(pPage);

		POSITION pos = pOwner->m_drawObjList.Find(pDrawObj);
		ASSERT(pos != NULL);

		if(pOwner->m_drawObjList.GetCount() <= 2)
			return FAILURE;
		//POSITION HeadPos = pOwner->m_drawObjList.GetHeadPosition();
		//pOwner->m_drawObjList.GetNext(HeadPos);
	
		if(nType == ONE_STEP)
		{
			EnsureDrawListBkGndPos(pOwner->m_drawObjList);

			POSITION posPrev = pos;
			pOwner->m_drawObjList.GetPrev(posPrev);
			if(NULL == posPrev )
				return SUCCESS;
			CS3DrawObj *pObjPrev  = pOwner->m_drawObjList.GetAt(posPrev); 

			pOwner->m_drawObjList.SetAt(pos,pObjPrev);//RemoveAt(pos);
			pOwner->m_drawObjList.SetAt(posPrev, pDrawObj);
			pOwner->InvalidateDrawObj(pDrawObj);
			
			//reset bkgnd postion.
			pPage->m_DrawObjList.RemoveAll();
			CopyObjsList(&pOwner->m_drawObjList,&pPage->m_DrawObjList);
            pPage->AdjustLayerInfoBySortedObjs();
			
		}else
		{
			EnsureDrawListBkGndPos(pOwner->m_drawObjList);//ensure bkgnd is at top of list.
			
			InsertToObjListByIndex(&pOwner->m_drawObjList,pDrawObj,nPosIndex);
			//RemoveDrawObjFromList(&pOwner->m_drawObjList,pDrawObj);
			pOwner->m_drawObjList.RemoveTail();

			pPage->m_DrawObjList.RemoveAll(); //sync two same struct...
			CopyObjsList(&pOwner->m_drawObjList,&pPage->m_DrawObjList);
			//pPage->m_DrawObjList.RemoveTail();
            pPage->AdjustLayerInfoBySortedObjs();
		}
		pOwner->Invalidate();
		return SUCCESS;
	}

 	//
	//objToBackCmd
	//
	objToBackCmd::objToBackCmd(int nID,CWnd *pTarget): _baseLayoutWndCmd(nID,pTarget){
		nType = ONE_STEP;
		pDrawObj = NULL;
		nPosIndex = 0;
	}
	objToBackCmd::~objToBackCmd(){}

	int objToBackCmd::DoMove(CS3DrawObj *pObj)
	{
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pOwner);
		ASSERT(pPage);
		ASSERT(pObj);

		if(pOwner->m_drawObjList.GetCount() <= 2)
			return FAILURE;

		if(nType == ONE_STEP)//zxy...典型的逻辑内聚代码，反面教材。。
		{
			EnsureDrawListBkGndPos(pOwner->m_drawObjList);

			POSITION posPrev,pos = pOwner->m_drawObjList.Find(pObj);
			ASSERT(pos != NULL);
			posPrev = pos;

			pOwner->m_drawObjList.GetPrev(posPrev);
			if(pOwner->m_drawObjList.GetHeadPosition() == posPrev)
				return FAILURE;
			CS3DrawObj *pObjPrev = pOwner->m_drawObjList.GetAt(posPrev); 

			pOwner->m_drawObjList.SetAt(pos,pObjPrev);//RemoveAt(pos);
			pOwner->m_drawObjList.SetAt(posPrev, pObj);
			//pOwner->InvalidateDrawObj(pObj);

			EnsureDrawListBkGndPos(pOwner->m_drawObjList);
			pPage->m_DrawObjList.RemoveAll();
			CopyObjsList(&pOwner->m_drawObjList,&pPage->m_DrawObjList);
            pPage->AdjustLayerInfoBySortedObjs();

		}else{ //与headpos交换
			
			EnsureDrawListBkGndPos(pOwner->m_drawObjList);
			
			POSITION pos;// = pOwner->m_drawObjList.Find(pObj);	
			FindObjInObjList(pObj,&pOwner->m_drawObjList,pos,nPosIndex);

			pOwner->m_drawObjList.RemoveAt(pos);
			pOwner->m_drawObjList.InsertAfter(pOwner->m_drawObjList.GetHeadPosition(),pObj);
			//EnsureDrawListBkGndPos(pOwner->m_drawObjList);
			pPage->m_DrawObjList.RemoveAll();
			CopyObjsList(&pOwner->m_drawObjList,&pPage->m_DrawObjList);
            pPage->AdjustLayerInfoBySortedObjs();
		}
		pOwner->Invalidate();
		return SUCCESS;
	}

	int objToBackCmd::ondo()
	{
		__super::ondo();
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		pDrawObj = pOwner->m_selectObjList.GetHead();
		return DoMove(pDrawObj);
	}

	int objToBackCmd::redo()
	{
		__super::redo();
		return DoMove(pDrawObj);
	}
	int objToBackCmd::undo()//to top 
	{
		__super::undo();
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		
		ASSERT(pPage);
		ASSERT(pOwner);
		ASSERT(pDrawObj);

		if(pOwner->m_drawObjList.GetCount() <= 2)
			return FAILURE;
		if(nType == ONE_STEP)
		{

			EnsureDrawListBkGndPos(pOwner->m_drawObjList);
			POSITION pos = pOwner->m_drawObjList.Find(pDrawObj);
			ASSERT(pos != NULL);
		
			POSITION posNext = pos;
			pOwner->m_drawObjList.GetNext(posNext);
			if(NULL == posNext)
				return SUCCESS;

			CS3DrawObj *pObjNext = 	pOwner->m_drawObjList.GetAt(posNext);

			pOwner->m_drawObjList.SetAt(pos,pObjNext);//RemoveAt(pos);
			pOwner->m_drawObjList.SetAt(posNext, pDrawObj);
			//pOwner->InvalidateDrawObj(pDrawObj);
			EnsureDrawListBkGndPos(pOwner->m_drawObjList);
			pPage->m_DrawObjList.RemoveAll();
			CopyObjsList(&pOwner->m_drawObjList,&pPage->m_DrawObjList);
            pPage->AdjustLayerInfoBySortedObjs();

		}else
		{
			EnsureDrawListBkGndPos(pOwner->m_drawObjList);
			POSITION pos = pOwner->m_drawObjList.GetHeadPosition();
			pOwner->m_drawObjList.GetNext(pos);
			pOwner->m_drawObjList.RemoveAt(pos);
			InsertToObjListByIndex(&pOwner->m_drawObjList,pDrawObj,nPosIndex);
			//RemoveDrawObjFromList(&pOwner->m_drawObjList,pDrawObj);
			
			//InsertToObjListByIndex(&pPage->m_DrawObjList,pDrawObj,nPosIndex);
			//RemoveDrawObjFromList(&pPage->m_DrawObjList,pDrawObj);
			pPage->m_DrawObjList.RemoveAll(); //sync two same struct...
			CopyObjsList(&pOwner->m_drawObjList,&pPage->m_DrawObjList,TRUE);
            pPage->AdjustLayerInfoBySortedObjs(TRUE);
		}
		pOwner->Invalidate();
		return SUCCESS;
	}
	
	//
	//objsColoneCmd
	//
	objsColoneCmd::objsColoneCmd(int nID,CWnd *pTarget):_baseLayoutWndCmd(nID,pTarget){
		pCopyObjsList = NULL;
	}
	objsColoneCmd::~objsColoneCmd(){
		
		if(this->m_nOpt != OPT_REDO)//undo 栈中，由designer负责释放,redo 中自己释放
			return;

		if(objsColoneList.GetCount() <= 0)
			return;
		
		POSITION pos = objsColoneList.GetHeadPosition();
		while(pos)
		{
			OBJ_INFO &info = objsColoneList.GetAt(pos);
			delete info.pInstance;
			FreeObjLayerInfo(info.layerInfo);
		 
			objsColoneList.GetNext(pos);
		}
	}

	int objsColoneCmd::ondo()
	{
		__super::ondo();

		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pPage);
		ASSERT(pOwner);
		ASSERT(this->pCopyObjsList);
		if(pCopyObjsList->GetCount()<=0)
			return FAILURE;

		FPoint clipPoint(pOwner->m_lastClonePoint);
		pOwner->ClientToLayout(clipPoint);
		FPoint pointOffset(0, 0);
		FPoint pointOrigin(0, 0);
		FRect rectLogic;
		CRect rectPos;
		BOOL bStartPoint = FALSE;
		POSITION pos = pCopyObjsList->GetHeadPosition();
		while (pos != NULL)
		{
			CS3DrawObj* pObj = pCopyObjsList->GetNext(pos);

			if (!bStartPoint)
			{
				pObj->GetLogicPos(rectLogic);
				pointOrigin.x = rectLogic.left;
				pointOrigin.y = rectLogic.top;
				pointOffset = clipPoint;
			}
			else
			{
				pObj->GetLogicPos(rectLogic);
				pointOffset.x = clipPoint.x + (rectLogic.left - pointOrigin.x);
				pointOffset.y = clipPoint.y + (rectLogic.top - pointOrigin.y);
			}

			pObj->MoveTo(pointOffset);
			pObj->GetLogicPos(rectLogic);
			pOwner->LayoutToClient(rectLogic);
			rectLogic.ConvertToCRect(rectPos);
			pObj->SetPosition(rectPos);
			bStartPoint = TRUE;

			pOwner->InsertDrawObj(pObj);

			OBJ_INFO info;
			info.pInstance = pObj;
			//pPage->m_pLayoutSetting->Layers.Lookup(pObj,info.pLayerInfo);
			BOOL bFound = FALSE;
			info.layerInfo = pPage->m_LayoutElement->GetLayerInfo(pObj->GetLayerId(), bFound);
			this->objsColoneList.AddTail(info);
		}
		return SUCCESS;
	}

	int objsColoneCmd::redo()
	{
		__super::redo();
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pOwner);
		if(objsColoneList.GetCount() <= 0)
			return FAILURE;
		POSITION pos = objsColoneList.GetHeadPosition();
		while(pos)
		{
			OBJ_INFO &info = objsColoneList.GetAt(pos);
			pOwner->InsertDrawObj(info.pInstance);
			//pPage->m_DrawObjList.AddTail(pObj);
			pPage->AddLayerIntoLayout(info.pInstance, info.layerInfo);
			//pPage->m_pLayoutSetting->Layers.SetAt(info.pInstance,info.pLayerInfo);
			objsColoneList.GetNext(pos);
		}
		pPage->m_DrawObjList.RemoveAll(); //sync two same struct...
		CopyObjsList(&pOwner->m_drawObjList,&pPage->m_DrawObjList);
		pOwner->Invalidate();
		return SUCCESS;
	}

	//move objs to cmd.
	int objsColoneCmd::undo()
	{
		__super::undo();
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pOwner);
		POSITION pos = objsColoneList.GetHeadPosition();
		while(pos)
		{
			OBJ_INFO &info = objsColoneList.GetAt(pos);
			RemoveDrawObjFromList(&pOwner->m_drawObjList,info.pInstance);

			pPage->m_LayoutElement->RemoveLayer(info.pInstance->GetLayerId());

			//pPage->m_pLayoutSetting->Layers.RemoveKey(info.pInstance);
			//RemoveDrawObjFromList(&pPage->m_DrawObjList,pObj);
			objsColoneList.GetNext(pos);
		}

		pPage->m_DrawObjList.RemoveAll(); //sync two same struct...
		CopyObjsList(&pOwner->m_drawObjList,&pPage->m_DrawObjList);
		pOwner->Invalidate();
		return SUCCESS;
	}

	//
	//objsGroupedCmd
	//not enough

	void objsGroupedCmd::addObj(CS3DrawObj *pObj)
	{
		ASSERT(pObj);
		POSITION pos = objsSelList.GetHeadPosition();
		while(pos)
		{
			CS3DrawObj *p = objsSelList.GetNext(pos);
			if(p == pObj)
				return;
		}
		objsSelList.AddTail(pObj);
	}

	objsGroupedCmd::objsGroupedCmd(int nID,CWnd *pTarget): _baseLayoutWndCmd(nID,pTarget){
		pGroupBound = NULL;
	}
	objsGroupedCmd::~objsGroupedCmd(){
		if(pGroupBound)
			delete pGroupBound;
	}

	void  objsGroupedCmd::setSelObjs(CS3DrawObjList *pList)
	{
		objsSelList.RemoveAll();
		CopyObjsList(pList,&objsSelList);
	}
	int objsGroupedCmd::ondo()
	{
		__super::ondo();
        //return FAILURE;
			TRACE(_T("objsGroupedCmd::ondo()!\n"));
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CopyObjsList(&pOwner->m_selectObjList,&objsSelList);
		pGroupBound = 	pOwner->m_pSelectObj ;
		return SUCCESS;
	}
	int objsGroupedCmd::redo()
	{
		__super::redo();
		TRACE(_T("objsGroupedCmd::redo()! \n"));
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
 		pOwner->m_pSelectObj = pGroupBound;
		pOwner->m_selectObjList.RemoveAll();
		CopyObjsList(&objsSelList,&pOwner->m_selectObjList);
		pOwner->RecalcMultiSelectMergeRect();
		pOwner->Invalidate();
		return SUCCESS;
	}
	int objsGroupedCmd::undo()
	{
		__super::undo();
		TRACE(_T("objsGroupedCmd::undo()! \n"));
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		pOwner->m_pSelectObj = NULL;
		pOwner->m_selectRect.SetRectEmpty();
		pOwner->m_selectObjList.RemoveAll();
		
		pOwner->Invalidate();
		return SUCCESS;
	}
	
	//
	//objsGroupMoveCmd
	//

	objsGroupMoveCmd::objsGroupMoveCmd(int nID,CWnd *pTarget): _baseLayoutWndCmd(nID,pTarget){
	}
	objsGroupMoveCmd::~objsGroupMoveCmd(){}
	int objsGroupMoveCmd::ondo()
	{
		__super::ondo();
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pOwner);
		ASSERT(pPage);
		ASSERT(pOwner->m_pSelectObj);

		this->boundData.pDrawObj = pOwner->m_pSelectObj;
		this->boundData.pDrawObj->GetLogicPos(boundData.orgRect);
		ASSERT(pOwner->m_selectObjList.GetSize() > 1);

		for(int i = 0;i < pOwner->m_selectObjList.GetSize();i++)
		{
			objsGroupMoveCmd::FRECT_DATA rectData;
			rectData.pDrawObj = pOwner->m_selectObjList.GetAt(pOwner->m_selectObjList.FindIndex(i));
			rectData.pDrawObj->GetLogicPos(rectData.orgRect);
			this->fRects.push_back(rectData);
		}

		return SUCCESS;
	}

	void  objsGroupMoveCmd:: DoDataExchange()
	{
		FRECT_DATA tpBoundData;
		FRECT_DATAS tpRects;

		tpBoundData.pDrawObj = boundData.pDrawObj;
		tpBoundData.pDrawObj->GetLogicPos(tpBoundData.orgRect);

		for( int i = 0;i < fRects.size();i++)
		{
			FRECT_DATA tpData;
			tpData.pDrawObj = fRects[i].pDrawObj;
			tpData.pDrawObj->GetLogicPos(tpData.orgRect);
			tpRects.push_back(tpData);
		}

		boundData.pDrawObj->SetLogicPos(boundData.orgRect);
		boundData.orgRect = tpBoundData.orgRect;

		for(int i = 0;i < fRects.size();i++)
		{
			fRects[i].pDrawObj->SetLogicPos(fRects[i].orgRect);
			fRects[i].orgRect = tpRects[i].orgRect;
		}
	}

	int objsGroupMoveCmd::redo()
	{
		__super::redo();
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		DoDataExchange();
		pOwner->m_pSelectObj = boundData.pDrawObj;
		pOwner->Invalidate();
		return SUCCESS;
	}

	int objsGroupMoveCmd::undo()
	{
		__super::undo();
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		DoDataExchange();
		pOwner->m_pSelectObj = boundData.pDrawObj;
		pOwner->Invalidate();
		return SUCCESS;
	}
	
	//
	//objsGroupDeleteCmd ,delete all obj in selobjlist.
	//
	
	objsGroupDeleteCmd::objsGroupDeleteCmd(int nID,CWnd *pTarget): _baseLayoutWndCmd(nID,pTarget){
		pGroupBound = NULL;
	}

	objsGroupDeleteCmd::~objsGroupDeleteCmd()
	{
		if(this->m_nOpt != OPT_UNDO) //该命令在 undo栈中需要删除实例 
			return;
	
		for(int i = 0;i < objStoreList.size();i++)
		{
			OBJ_INFO &obj = objStoreList[i];
			delete obj.pInstance;
			FreeObjLayerInfo(obj.layerInfo);
			 
		}

	}

	void objsGroupDeleteCmd::addObjsToStoreList(CS3DrawObjList *pObjs)
	{
		POSITION pos = pObjs->GetHeadPosition();
		while(pos)
		{
			OBJ_INFO storeObj;
			storeObj.pInstance = pObjs->GetAt(pos);
			storeObj.instPos = -1;
			//this->objSelList.AddTail(pObj);
			objStoreList.push_back(storeObj);
			pObjs->GetNext(pos);
		}
	}

	void objsGroupDeleteCmd::removeStoreObjFromDesigner()
	{
		if(this->objStoreList.size() <= 0)
			return;
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pPage);
		ASSERT(pOwner);
		ASSERT(pOwner->m_drawObjList.GetCount());
		CS3DrawObjList &drawList =  pOwner->m_drawObjList;

		for( size_t i = 0;i < objStoreList.size();i++)
		{
			OBJ_INFO &obj = objStoreList[i];

			POSITION pos;
		    if(!FindObjInObjList(obj.pInstance,&drawList,pos,obj.instPos))	// Scanf z-order val in list.
			{
				ASSERT(FALSE);//error context
				break; 
			}
		}


		//S3SIGNAGE_LAYER &Layers =  pPage->m_pLayoutSetting->Layers ;//.Lookup(pObj, pLayerInfo); 
		for( int i = 0;i < objStoreList.size();i++)
		{
			OBJ_INFO &obj = objStoreList[i];
			RemoveDrawObjFromList(&drawList,obj.pInstance);

			//
			//remove mediasetting from layers
			//
			BOOL bFound = FALSE;
			std::shared_ptr<LayerElement> layInfo = pPage->m_LayoutElement->GetLayerInfo(obj.pInstance->GetLayerId(), bFound);
			if (bFound && layInfo.get())
			{
				obj.layerInfo = layInfo;
				BOOL bDel = pPage->m_LayoutElement->RemoveLayer(obj.pInstance->GetLayerId());
				ASSERT(bDel);
			}
			//RemoveDrawObjFromList(&pPage->m_DrawObjList,obj.pInstance);
		}
		pPage->m_DrawObjList.RemoveAll(); //sync two same struct...
		CopyObjsList(&drawList,&pPage->m_DrawObjList);
	}

	void objsGroupDeleteCmd::RestoreStoreObjToDesigner()
	{
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pOwner);
		ASSERT(objStoreList.size());//error context...

		CS3DrawObjList &drawList =  pOwner->m_drawObjList;
		//S3SIGNAGE_LAYER &Layers =  pPage->m_pLayoutSetting->Layers ;//.Lookup(pObj, pLayerInfo); 
		for( size_t i = 0;i < objStoreList.size();i++)
		{
			OBJ_INFO &obj = objStoreList[i];
		 
			InsertToObjListByIndex(&drawList,obj.pInstance,obj.instPos);

			//
			//restore "mediasetting" to layers
			//
			vector<std::shared_ptr<MediaElement>> *pMediaList = NULL;
			if (pPage->GetMediaSettingsByObj(obj.pInstance, pMediaList))
			{
				BOOL bFound = FALSE;
				std::shared_ptr<LayerElement> layerInfo = pPage->m_LayoutElement->GetLayerInfo(obj.pInstance->GetLayerId(), bFound);
				if (bFound)
				{
					layerInfo->SetMediaList(*pMediaList);
				}
				else
				{
					ASSERT(FALSE);
				}
			}

		}
		pPage->m_DrawObjList.RemoveAll(); //sync two same struct...
		CopyObjsList(&drawList,&pPage->m_DrawObjList);
	}

	int objsGroupDeleteCmd::ondo()
	{
		__super::ondo();
		TRACE(_T("objsGroupDeleteCmd::ondo()! \n"));
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;

		if(!pOwner->m_pSelectObj)
			return FAILURE;
		if(pOwner->m_selectObjList.GetCount() <= 1)
			return FAILURE;

		pGroupBound = pOwner->m_pSelectObj;
		pOwner->m_pSelectObj = NULL;

		addObjsToStoreList(&pOwner->m_selectObjList);
		pOwner->m_selectObjList.RemoveAll();

		removeStoreObjFromDesigner();
		pOwner->SelectDrawObj(NULL);
		pOwner->m_selectRect.SetRectEmpty();
		pOwner->Invalidate();
		return SUCCESS;
	}
	int objsGroupDeleteCmd::redo()
	{
		__super::redo();
		TRACE(_T("objsGroupDeleteCmd::redo()")); 
		 
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
		ASSERT(pPage);
		pOwner->m_pSelectObj = NULL;
		pOwner->m_selectObjList.RemoveAll();

		//S3SIGNAGE_LAYER &Layers =  pPage->m_pLayoutSetting->Layers ;//.Lookup(pObj, pLayerInfo);
		for( size_t i = 0;i <this->objStoreList.size();i++)
		{
			OBJ_INFO &inf =  objStoreList[i];
			RemoveDrawObjFromList(&pOwner->m_drawObjList,inf.pInstance);

			//
			//remove mediasetting from layers
			//
			BOOL bFound = FALSE;
			std::shared_ptr<LayerElement> layInfo = pPage->m_LayoutElement->GetLayerInfo(inf.pInstance->GetLayerId(), bFound);
			if (bFound && layInfo.get())
			{
				BOOL bDel = pPage->m_LayoutElement->RemoveLayer(inf.pInstance->GetLayerId());
				ASSERT(bDel);
				ASSERT(inf.layerInfo == layInfo);
			}
		
/*
			LAYERINFO *pLayerInfo = NULL;
			if(Layers.Lookup(inf.pInstance,pLayerInfo))
			{
				//Layers.SetAt(inf.pInstance,inf.pLayerInfo);
				Layers.RemoveKey(inf.pInstance);
				ASSERT(inf.pLayerInfo == pLayerInfo);
			}*/
			//RemoveDrawObjFromList(&pPage->m_DrawObjList,inf.pInstance);
		}
		pPage->m_DrawObjList.RemoveAll(); //sync two same struct...
		CopyObjsList(&pOwner->m_drawObjList,&pPage->m_DrawObjList);
		pOwner->Invalidate();
		return SUCCESS;
	}

	int objsGroupDeleteCmd::undo()
	{
		__super::undo();
		TRACE(_T("objsGroupDeleteCmd::undo()")); 
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		//restore "group context"
		pOwner->m_pSelectObj = pGroupBound;
		pOwner->m_selectObjList.RemoveAll();
		
		for( int i = 0;i < objStoreList.size();i++)
		{
			OBJ_INFO  &obj = objStoreList[i];
			pOwner->m_selectObjList.AddTail(obj.pInstance);
		}

		RestoreStoreObjToDesigner();
		
		//restore to designer.
		pOwner->Invalidate();
		return SUCCESS;
	}

	//
	//layoutClrCmd
	//
	layoutClrCmd::layoutClrCmd(int nID,CWnd *pTarget): _baseLayoutWndCmd(nID,pTarget){}
	layoutClrCmd::~layoutClrCmd(){}
	int layoutClrCmd::ondo()
	{
		__super::ondo();
		return SUCCESS;
	}
	int layoutClrCmd::redo()
	{
		__super::redo();
		return SUCCESS;
	}
	int layoutClrCmd::undo()
	{
		__super::undo();
		return SUCCESS;
	}

	//
	//layoutZoomCmd
	//
	layoutZoomCmd::layoutZoomCmd(int nID,CWnd *pTarget): _baseLayoutWndCmd(nID,pTarget){}
	layoutZoomCmd::	~layoutZoomCmd(){}
	int layoutZoomCmd::ondo()
	{
		__super::ondo();
		return SUCCESS;
	}
	int layoutZoomCmd::redo()
	{
		__super::redo();
		return SUCCESS;
	}

	int layoutZoomCmd::undo()
	{
		__super::undo();
		return SUCCESS;
	}

	//
	//bindLineCmd
	//
	void UpdateBindLine(CPoint pt, CLayoutDesignerCtrl *pOwner,BOOL bXLine = TRUE)
	{
		   pOwner->Invalidate();
#if 0 
			CRect updateRect;
			pOwner->GetClientRect(updateRect);
			pOwner->LayoutToClient(pt);
			if(bXLine)
			{
				updateRect.left = updateRect.right = pt.x;
				updateRect.InflateRect(2,0);
			}else
			{
				updateRect.top = updateRect.bottom = pt.y;
				updateRect.InflateRect(0,2);
			}
			pOwner->InvalidateRect(updateRect);
#endif
	}


	bindLineCmd::bindLineCmd(int nID,CWnd *pTarget): _baseLayoutWndCmd(nID,pTarget){

		nXIndex = -1;
		nYIndex = -1;
		nXLineData = -1;
		nYLineData = -1;
	}
	bindLineCmd::~bindLineCmd(){}
	int bindLineCmd::ondo()
	{
		__super::ondo();
		//not need implement.
		return SUCCESS;
	}
	int bindLineCmd::redo()
	{
		__super::redo();
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		ASSERT(pOwner);
		CRulerCtrl *pCtrl =(CRulerCtrl*)::GetWindowLongPtr(pOwner->m_hWnd,GWLP_USERDATA);
		ASSERT(pCtrl);

		AXIS_ARRAY & xLines = pCtrl->GetXBindLines();
		AXIS_ARRAY & yLines = pCtrl->GetYBindLines();

		CPoint pt;
		if(this->nXIndex >= 0)
		{
			AXIS_ARRAY::iterator it = xLines.begin();
			int i = 0;

			if( this->nXIndex == 0)
				xLines.insert(xLines.begin(),this->nXLineData);
			else if((size_t)this->nXIndex >= xLines.size())
				xLines.push_back(this->nXLineData);
			else
			{
				for(;it != xLines.end();it++,i++)
				{
					if(i == this->nXIndex)
					{
						xLines.insert(it,this->nXLineData);
					}
				}
			}
			pt.x = this->nXLineData;
			UpdateBindLine(pt,pOwner);
		}

		if(this->nYIndex >= 0)
		{
			AXIS_ARRAY::iterator it = yLines.begin();
			int i = 0;

			if( this->nYIndex == 0)
				yLines.insert(yLines.begin(),this->nYLineData);
			else if((size_t)this->nYIndex >= yLines.size())
				yLines.push_back(this->nYLineData);
			else
			{
				for(;it != yLines.end();it++,i++)
				{
					if(i == this->nYIndex)
					{
						xLines.insert(it,this->nYLineData);
					}
				}
			}

			pt.y = this->nYLineData;
			UpdateBindLine(pt,pOwner,FALSE);
		}
		return SUCCESS;
	}
	int bindLineCmd::undo()
	{
		__super::undo();

		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		ASSERT(pOwner);
		CRulerCtrl *pCtrl =(CRulerCtrl*)::GetWindowLongPtr(pOwner->m_hWnd,GWLP_USERDATA);
		ASSERT(pCtrl);

		AXIS_ARRAY & xLines = pCtrl->GetXBindLines();
		AXIS_ARRAY & yLines = pCtrl->GetYBindLines();

		CPoint pt;
		if(this->nXIndex >= 0)
		{
			CRect updateRect;
			pOwner->GetClientRect(updateRect);

			AXIS_ARRAY::iterator it = xLines.begin();
			int i = 0;
			for(;it != xLines.end();it++,i++)
			{
				if( i == nXIndex)
				{
					if(this->nXLineData < 0)
						this->nXLineData = *it;
					xLines.erase(it);
					break;
				}
			}
			//update line rect.
			pt.x = this->nXLineData;
			UpdateBindLine(pt,pOwner);
		}

		if(this->nYIndex >= 0)
		{
			CRect updateRect;
			pOwner->GetClientRect(updateRect);

			AXIS_ARRAY::iterator it = yLines.begin();
			int i = 0;
	
			for(;it != yLines.end();it++,i++)
			{
				if( i == nYIndex)
				{
					if(this->nYLineData < 0)
						this->nYLineData = *it;
					yLines.erase(it);
					break;
				}

			}
			//update line rect.
			pt.y = this->nYLineData;
			UpdateBindLine(pt,pOwner,FALSE);
		}
		return SUCCESS;
	}

	//
	//bindLineMoveCmd
	//
	
	bindLineMoveCmd::bindLineMoveCmd(int nID,CWnd *pTarget): _baseLayoutWndCmd(nID,pTarget){

		nXLine = -1;
		nXLineDate = -1;
		nYLine = -1;
		nYLineDate = -1;  
	}
	bindLineMoveCmd::~bindLineMoveCmd(){}
	void bindLineMoveCmd::restoreLine()
	{
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		ASSERT(pOwner);
		CRulerCtrl *pCtrl =(CRulerCtrl*)::GetWindowLongPtr(pOwner->m_hWnd,GWLP_USERDATA);
		ASSERT(pCtrl);
		
		AXIS_ARRAY &xArray = pCtrl->GetXBindLines();
		AXIS_ARRAY &yArray = pCtrl->GetYBindLines();

		if(this->nXLine >= 0)
		{
			int xdata = xArray[this->nXLine];
			xArray[this->nXLine] = this->nXLineDate;
			this->nXLineDate = xdata;
		}

		if(this->nYLine >= 0)
		{
			int ydata = yArray[this->nYLine];
			yArray[this->nYLine] = this->nYLineDate;
			this->nYLineDate = ydata;
		}


	}
	void bindLineMoveCmd::restoreObjs()
	{
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		ASSERT(pOwner);
		if(attachObjs.size() <= 0)
			return;
		for( size_t  i = 0;i < attachObjs.size();i++)
		{
			OBJ_ORG_INFO &info = attachObjs[i];
			FRect storeRect;// = info.orgRect;
			info.pInst->GetLogicPos(storeRect);
			info.pInst->SetLogicPos(info.orgRect);
			info.orgRect = storeRect;
		}

		//pOwner->Invalidate();
	}

	bool bindLineMoveCmd::addAttach(CS3DrawObj *pObj)
	{
		ASSERT(pObj);
		OBJ_ORG_INFO orgInfo;
		orgInfo.pInst = pObj;
		pObj->GetLogicPos(orgInfo.orgRect);
		
		for(size_t i = 0;i < attachObjs.size();i++)
		{
			OBJ_ORG_INFO &info = attachObjs[i];
			if(info.pInst == pObj)
				return false;
		}
		
		attachObjs.push_back(orgInfo);
		return true;
	}

	int bindLineMoveCmd::ondo()
	{
		__super::ondo();
	
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		ASSERT(pOwner);
		CRulerCtrl *pCtrl =(CRulerCtrl*)::GetWindowLongPtr(pOwner->m_hWnd,GWLP_USERDATA);
		ASSERT(pCtrl);
		
		AXIS_ARRAY &xArray = pCtrl->GetXBindLines();
		AXIS_ARRAY &yArray = pCtrl->GetYBindLines();

		if(pOwner->m_bXLineCatched)
		{
			this->nXLine = pOwner->m_nXCatchedLine ;
			this->nXLineDate = xArray[this->nXLine];
		}

		if(pOwner->m_bYLineCatched)
		{
			this->nYLine = pOwner->m_nYCatchedLine ;
			this->nYLineDate = yArray[this->nYLine];
		}

		if(pCtrl->m_bSnapYLine)
		{
			this->nYLine = pCtrl->m_nSnapYLine ;
			this->nYLineDate = yArray[this->nYLine];
		}

		if(pCtrl->m_bSnapXLine)
		{
			this->nXLine = pCtrl->m_nSnapXLine;
			this->nXLineDate = xArray[this->nXLine];
		}

		
		return SUCCESS;
	}

	int bindLineMoveCmd::redo()
	{
		__super::redo();
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		//restore attached objs.
		restoreObjs();
		//restore bindline position.
		restoreLine();

		pOwner->RecalcMultiSelectMergeRect(); 
		pOwner->Invalidate();
		return SUCCESS;
	}
	int bindLineMoveCmd::undo()
	{
		__super::undo();
		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		//restore attached objs.
		restoreObjs();
		//restore bindline position.
		restoreLine();
		//may changed multi-selection,so restore it. 
		pOwner->RecalcMultiSelectMergeRect(); 
		pOwner->Invalidate();
		return SUCCESS;
	}

	//
	//bindLineDeleteCmd
	//
	bindLineDeleteCmd::bindLineDeleteCmd(int nID,CWnd *pTarget): _baseLayoutWndCmd(nID,pTarget){
		 nXLine = -1; //line pos in bindline vector.
		 nYLine = -1;
		 nXLineData = 0;
		 nYLineData = 0;

	}
	bindLineDeleteCmd::~bindLineDeleteCmd(){}
	int bindLineDeleteCmd::ondo()
	{
		__super::ondo();

		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		ASSERT(pOwner);
		CRulerCtrl *pCtrl =(CRulerCtrl*)::GetWindowLongPtr(pOwner->m_hWnd,GWLP_USERDATA);
		ASSERT(pCtrl);

		AXIS_ARRAY & xLines = pCtrl->GetXBindLines();
		AXIS_ARRAY & yLines = pCtrl->GetYBindLines();

		if(pOwner->m_bXLineCatched ||pOwner->m_bYLineCatched
			|| pCtrl->m_nFocusXHandle >= 0||pCtrl->m_nFocusYHandle >= 0)
		{
			if( pCtrl->m_nFocusXHandle >= 0)
			{
				this->nXLine = pCtrl->m_nFocusXHandle ;
				this->nXLineData = xLines[this->nXLine ];
				pCtrl->RemoveXRulerLine(this->nXLine);
				pCtrl->m_nFocusXHandle = -1;
			}
			
			if(pOwner->m_bXLineCatched)
			{
				this->nXLine = pOwner->m_nXCatchedLine ;
				this->nXLineData = xLines[this->nXLine ];
				pCtrl->RemoveXRulerLine(pOwner->m_nXCatchedLine);
				pOwner->m_bXLineCatched = FALSE;
			}

			if( pCtrl->m_nFocusYHandle >= 0)
			{
				this->nYLine = pCtrl->m_nFocusYHandle ;
				this->nYLineData = yLines[this->nYLine ];
				pCtrl->RemoveYRulerLine(pCtrl->m_nFocusYHandle);
				pCtrl->m_nFocusYHandle = -1;
			}

			if(pOwner->m_bYLineCatched)
			{
				this->nYLine = pOwner->m_nYCatchedLine ;
				this->nYLineData = yLines[this->nYLine];
				pCtrl->RemoveYRulerLine(pOwner->m_nYCatchedLine);
				pOwner->m_bYLineCatched = FALSE;
			}
			
			pOwner->Invalidate();

		}else
			return FAILURE;

		return SUCCESS;
	}

	int bindLineDeleteCmd::redo()
	{
		__super::redo();

		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		ASSERT(pOwner);
		CRulerCtrl *pCtrl =(CRulerCtrl*)::GetWindowLongPtr(pOwner->m_hWnd,GWLP_USERDATA);
		ASSERT(pCtrl);

		AXIS_ARRAY & xLines = pCtrl->GetXBindLines();
		AXIS_ARRAY & yLines = pCtrl->GetYBindLines();

		if(this->nXLine < 0 && this->nYLine <0 )
			return FAILURE;

		if(this->nXLine >=  0)
		{
			AXIS_ARRAY::iterator it = xLines.begin();
			int i = 0;
			for(;it != xLines.end();it++,i++)
			{
				if( i == this->nXLine)
				{
					xLines.erase(it);
					break;
				}
			}
		}

		if(this->nYLine >= 0)
		{
			AXIS_ARRAY::iterator it = yLines.begin();
			int i = 0;
			for(;it != yLines.end();it++,i++)
			{
				if( i == this->nYLine)
				{
					yLines.erase(it);
					break;
				}
			}
		}

		pOwner->Invalidate();
		return SUCCESS;
	}

	int bindLineDeleteCmd::undo()
	{
		__super::undo();

		CLayoutDesignerCtrl *pOwner= (CLayoutDesignerCtrl *)this->m_pTargetWnd;
		ASSERT(pOwner);
		CRulerCtrl *pCtrl =(CRulerCtrl*)::GetWindowLongPtr(pOwner->m_hWnd,GWLP_USERDATA);
		ASSERT(pCtrl);

		AXIS_ARRAY & xLines = pCtrl->GetXBindLines();
		AXIS_ARRAY & yLines = pCtrl->GetYBindLines();

		if(this->nXLine < 0 && this->nYLine <0 )
			return FAILURE;
		if(this->nXLine >= 0)
		{
			//xLines[this->nXLine] = this->nXLineData;
			if(this->nXLine == 0)
				xLines.insert(xLines.begin(),this->nXLineData);
			else if((size_t)this->nXLine >= xLines.size())
				xLines.insert(xLines.end(),this->nXLineData);
			else
			{
				AXIS_ARRAY::iterator it = xLines.begin();
			
				xLines.insert(it + this->nXLine,nXLineData);
			}

		}
		if(this->nYLine >= 0)
		{
			if(this->nYLine == 0)
				yLines.insert(yLines.begin(),this->nYLineData);
			else if((size_t)this->nXLine >= yLines.size())
				yLines.insert(yLines.end(),this->nYLineData);
			else
			{
				AXIS_ARRAY::iterator it = yLines.begin();
			
				yLines.insert(it + this->nYLine,nYLineData);
			}
		}

		pOwner->Invalidate();
		return SUCCESS;
	}

	//
	//These two cmds to hold cmds status table of property dialog.
	//
	properLayoutCmd::properLayoutCmd(int nID,CWnd *pTarget):_baseCmd(nID,pTarget){
	};

	properLayoutCmd::~properLayoutCmd(){

	};
	int properLayoutCmd::redo()
	{
		__super::redo();
		if(this->m_subCmds.empty())
			return FAILURE;
		for(SUB_CMDS::iterator it = m_subCmds.begin();
			it != m_subCmds.end();it++)
		{
			_baseCmd *pCmd = *it;
			pCmd->redo();
		}
		return SUCCESS;
	};
	int properLayoutCmd::undo()
	{
		__super::undo();
		if(this->m_subCmds.empty())
			return FAILURE;
		for(int i = 0;i < m_subCmds.size();i++)
		{
			_baseCmd *pCmd = m_subCmds[i];
			pCmd->undo();
		}
		return SUCCESS;
	};
	int properLayoutCmd::ondo()
	{
		__super::ondo();
		if(this->m_subCmds.empty())
			return FAILURE;
		for(int i = 0;i < m_subCmds.size();i++)
		{
			_baseCmd *pCmd = m_subCmds[i];
			if(IS_FAILURE(pCmd->ondo()))//一些过程检查集中在ondo实现，类似(init)，该处可能存在失败
				return FAILURE;
		}
		return SUCCESS;
	};

	//
	//property for plugin instance .

	properInstCmd::properInstCmd(int nID,CWnd *pTarget):_baseCmd(nID,pTarget){
	};
	properInstCmd::~properInstCmd(){};
	
	int properInstCmd::redo()
	{
		__super::redo();
		if(this->m_subCmds.empty())
			return FAILURE;
		for(SUB_CMDS::iterator it = m_subCmds.begin();
			it != m_subCmds.end();it++)
		{
			_baseCmd *pCmd = *it;
			pCmd->redo();
		}
		return SUCCESS;
	};
	int properInstCmd::undo()
	{
		__super::undo();
		if(this->m_subCmds.empty())
			return FAILURE;
		for(SUB_CMDS::iterator it = m_subCmds.begin();
			it != m_subCmds.end();it++)
		{
			_baseCmd *pCmd = *it;
			pCmd->undo();
		}
		return SUCCESS;
	};
	int properInstCmd::ondo()
	{
		__super::ondo();
		if(this->m_subCmds.empty())
			return FAILURE;
		for(SUB_CMDS::iterator it = m_subCmds.begin();
			it != m_subCmds.end();it++)
		{
			_baseCmd *pCmd = *it;
			if(IS_FAILURE(pCmd->ondo()))
				return FAILURE;
		}
		return SUCCESS;
	};
	

	//
	//LayoutCmdEngine
	//
	bool LayoutCmdEngine::bindTarget(CWnd *pTargetWnd)
	{ 
		__super::bindTarget(pTargetWnd);
		m_pFactory->bind(this);//Need bind to instance of "XXXCmdEngine" class.
		return true;
	}

	//
	//LayoutCmdFactory
	//
	LayoutCmdFactory::LayoutCmdFactory(){
		m_pBuffCmd = NULL;
	}
	LayoutCmdFactory::~LayoutCmdFactory(){}
	
	CWnd * LayoutCmdFactory::getTargetWnd()
	{
		return m_pOwner->getTargetWnd();
	}

	_baseCmd * LayoutCmdFactory::newCmdBegin(int nID)
	{
		if(m_pBuffCmd) //last cmd obj isnt finished.
		{
			TRACE(_T("A Suspend Cmd obj exist!"));
			return NULL;
		}

		m_pBuffCmd = newCmd(nID);
		return (_baseCmd *)m_pBuffCmd;
	}

	_baseCmd * LayoutCmdFactory::getBuffedCmd()
	{
		return (_baseCmd *)m_pBuffCmd;
	}

	_baseCmd * LayoutCmdFactory::newCmdEnd()
	{
		_baseCmd *tp = m_pBuffCmd;
		m_pBuffCmd = NULL;
		return tp;
	}

	_baseCmd * LayoutCmdFactory::newCmd(int nID)
	{
		_baseCmd *tp = NULL;
		CWnd *pTargetWnd = this->getTargetWnd();
		ASSERT(pTargetWnd);
		switch((cmds)nID)
		{
		case CMD_OBJBUILD:
			tp = new objBuildCmd(nID,pTargetWnd); 
			break;
		case CMD_OBJDELETE:
			tp = new objDeleteCmd(nID,pTargetWnd);
			break;
		case CMD_OBJMOVE:
			tp = new objMoveCmd(nID,pTargetWnd);
			break;
		case CMD_OBJSIZE:
			tp = new objSizeCmd(nID,pTargetWnd);
			break;
		case CMD_OBJTOFRONT:
			tp = new objToFrontCmd(nID,pTargetWnd);
			break;
		case CMD_OBJTOBACK:
			tp = new objToBackCmd(nID,pTargetWnd);
			break;
		case CMD_OBJSGROUPED:
			tp = new objsGroupedCmd(nID,pTargetWnd);
			break;
		case CMD_OBJSCOLONE:
			tp = new objsColoneCmd(nID,pTargetWnd);
			break;
		case CMD_OBJSGROUPMOVE:
			tp = new objsGroupMoveCmd(nID,pTargetWnd);
			break;
		case CMD_OBJSGROUPDELETE:
			tp = new objsGroupDeleteCmd(nID,pTargetWnd);
			break;
		case CMD_WNDSCROLL:
			tp = new wndScrollCmd(nID,pTargetWnd);
			break;
		case CMD_LAYOUTSIZE:
			tp = new layoutSizeCmd(nID,pTargetWnd);
			break;  
		case CMD_LAYOUTCLR:
			tp = new layoutClrCmd(nID,pTargetWnd);
			break;
		case CMD_LAYOUTZOOM:
			tp = new layoutZoomCmd(nID,pTargetWnd);
			break;
		case CMD_BINDLINE:
			tp = new bindLineCmd(nID,pTargetWnd);
			break;
		case CMD_BINDLINEMOVE :
			tp = new bindLineMoveCmd(nID,pTargetWnd);
			break;
		case CMD_BINDLINEDELETE:
			tp = new bindLineDeleteCmd(nID,pTargetWnd);
			break;
		case CMD_PROPERLAYOUT:
			tp = new properLayoutCmd(nID,pTargetWnd);
			break;
		case CMD_PROPERINST:
			tp = new properInstCmd(nID,pTargetWnd);
			break;
		default:
			ASSERT(FALSE);
			break;
		}  
		ASSERT(tp);
		return tp;
	}

	LPCTSTR  LayoutCmdFactory::getCmdName(_baseCmd *pCmd)
	{
		if(NULL != pCmd)
		{
			switch( pCmd->getID())
			{
			case CMD_OBJBUILD:
				return _T("Create Plugin Instance");
			case CMD_OBJMOVE:
				return _T("Move Plugin Instance");
			case CMD_OBJDELETE:
				return _T("Delete Obj");
			case CMD_OBJSIZE:
				return _T("Change Plugin Instance Size");
			case CMD_OBJTOFRONT:
				return _T("Set Plugin Instance To Front");
			case CMD_OBJTOBACK:
				return _T("Set Plugin Instance Back");
			case CMD_OBJSGROUPED:
				return _T("Multiple Plugin Instances Selection ");
			case CMD_OBJSGROUPMOVE:
				return _T("Move Multiple Plugin Instances");
			case CMD_OBJSGROUPDELETE:
				return _T("Delete Multiple Plugin Instances");
			case CMD_OBJSCOLONE:
				return _T("Colone Objs");
			case CMD_WNDSCROLL:
				return _T("Scroll Designer Window");
			case CMD_LAYOUTSIZE:
				return _T("Set Layout Size");
			case CMD_LAYOUTCLR:
				return _T("Set Layout Color");
			case CMD_LAYOUTZOOM:
				return _T("Zoom Layout");
			case CMD_BINDLINE:
				return _T("Create Bindline");
			case CMD_BINDLINEMOVE :
				return _T("Move Bindline");
			case CMD_BINDLINEDELETE:
				return _T("Delete Bindline");
			case CMD_PROPERLAYOUT:
				return _T("Layout Property");
			case CMD_PROPERINST:
				return _T("Instance Property");
			default:
				break;
			}
		}
		return _T("");
	}

	bool LayoutCmdFactory::delCmd(_baseCmd *pCmd)
	{
		__super::delCmd(pCmd);
		ASSERT(pCmd != NULL);

		switch(pCmd->getID())
		{
			case CMD_OBJBUILD:
				delete ( objBuildCmd *)pCmd;
				break;
			case CMD_OBJMOVE:
				delete ( objMoveCmd *)pCmd;
				break;
			case CMD_OBJDELETE:
				delete (objDeleteCmd *)pCmd;
				break;
			case CMD_OBJSIZE:
				delete ( objSizeCmd *)pCmd;
				break;
			case CMD_OBJTOFRONT:
				delete ( objToFrontCmd *)pCmd;
				break;
			case CMD_OBJTOBACK:
				delete ( objToBackCmd *)pCmd;
				break;
			case CMD_OBJSGROUPED:
				delete ( objsGroupedCmd *)pCmd;
				break;
			case CMD_OBJSCOLONE:
				delete (objsColoneCmd *)pCmd;
				break;
			case CMD_OBJSGROUPMOVE:
				delete (objsGroupMoveCmd *)pCmd;
				break;
			case CMD_OBJSGROUPDELETE:
				delete (objsGroupDeleteCmd *)pCmd;
				break;
			case CMD_WNDSCROLL:
				delete (wndScrollCmd *) pCmd;
				break;
			case CMD_LAYOUTSIZE:
				delete (layoutSizeCmd *)pCmd;
				break;
			case CMD_LAYOUTCLR:
				delete (layoutClrCmd *)pCmd;
				break;
			case CMD_LAYOUTZOOM:
				delete (layoutZoomCmd *)pCmd;
				break;
			case CMD_BINDLINE:
				delete (bindLineCmd *)pCmd;
				break;
			case CMD_BINDLINEMOVE :
				delete (bindLineMoveCmd *)pCmd;
				break;
			case CMD_BINDLINEDELETE:
				delete (bindLineDeleteCmd *)pCmd;
				break;
			case CMD_PROPERINST:
				delete (properInstCmd *)pCmd;
				break;

			case CMD_PROPERLAYOUT:  
				delete (properLayoutCmd *)pCmd;
				break;

			default:
				TRACE("Try to release unknown class !\n");
				//delete pCmd; //support property sub cmds.
				FreeBaseCmd(pCmd);
				break;

		}
		return true;
	}
};  
