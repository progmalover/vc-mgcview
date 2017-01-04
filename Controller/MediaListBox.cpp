// MediaListBox.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "MediaListBox.h"
#include <afxtagmanager.h>
#include "PropertyLayer.h"
#include "DesignerPage.h"
// wParam - no used, lParam - no used
UINT WM_ON_MEDIA_NUM_CHANGE = ::RegisterWindowMessage(_T("PROPERTY_MEDIA_NUM_CHANGE"));
UINT WM_ON_MEDIA_ITEM_CHANGE = ::RegisterWindowMessage(_T("WM_ON_MEDIA_ITEM_CHANGE"));

// CMediaListBox
class CPrvPage :public CDesignerPage
{
    friend class CMediaListBox;
};
class CPrvPropertyLayer:public CPropertyLayer
{
    friend class CMediaListBox;
};
class CPrvLayoutDesignerCtrl :public CLayoutDesignerCtrl
{
    friend class CMediaListBox;
} ; 

CMediaListBox::CMediaListBox()
{
    m_layerInfo.reset();
}

CMediaListBox::~CMediaListBox()
{
    m_layerInfo.reset();
}


BEGIN_MESSAGE_MAP(CMediaListBox, CVSListBox)
    ON_MESSAGE(WM_MFC_INITCTRL, &CMediaListBox::OnInitControl)
    ON_NOTIFY(LVN_ITEMCHANGED, 1, &CMediaListBox::OnItemChanged)
    ON_NOTIFY(LVN_ENDLABELEDIT, 1, &CMediaListBox::OnEndLabelEdit)
     //ON_REGISTERED_MESSAGE(WM_DND_DROP, OnDrop)
END_MESSAGE_MAP()



// CMediaListBox message handlers
void CMediaListBox::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    ENSURE(pNMHDR != NULL);

    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    ENSURE(pNMListView != NULL);
    pNMHDR->hwndFrom = this->m_hWnd;
    pNMHDR->idFrom = GetDlgCtrlID();

    GetOwner()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)pNMHDR);
    *pResult = 0;

    UpdateButtons();
}



LRESULT CMediaListBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: Add your specialized code here and/or call the base class
    ASSERT_VALID(this);

    LRESULT lResult;
    switch (message)
    {
    case WM_NOTIFY:
    case WM_COMMAND:
        // send these messages to the owner if not handled
        if (OnWndMsg(message, wParam, lParam, &lResult))
            return lResult;
        else
        {
            // try owner next
            lResult = GetOwner()->SendMessage(message, GetDlgCtrlID(), lParam);
            return lResult;
        }
    }

    // otherwise, just handle in default way

    return CVSListBox::WindowProc(message, wParam, lParam);
}

void CMediaListBox::SetContent(std::shared_ptr<LayerElement> layerInfo, int pos)
{
    //m_pLayer = pLayer;
	m_layerInfo = layerInfo;
	vector<std::shared_ptr<MediaElement>> &mediaArray = layerInfo->GetMediaList();
	vector<std::shared_ptr<MediaElement>>::iterator it;
	int p = 0;
	m_pWndList->DeleteAllItems();
	for (it = mediaArray.begin();it != mediaArray.end(); it++)
	{
		CString mediaFile = (*it)->GetMediaFile();
		CString mediaType = (*it)->GetMediaType();
		if (mediaType.CompareNoCase(_T("S3WebBrowser")) != 0)
		{
			mediaFile = (*it)->GetMediaName();
		}
		else
		{
			if (::PathFileExists(mediaFile))
			{
				p = mediaFile.ReverseFind('\\');
				if (p != -1)
				{
					mediaFile = mediaFile.Right(mediaFile.GetLength() - p - 1);
				}
			}
		}

		if (mediaType == szTypeName_Message)
		{
			p = mediaFile.ReverseFind('.');
			if (p != -1)
			{
				mediaFile = mediaFile.Left(p);
			}
		}
		int Index = AddItem(mediaFile);
		SetItemData(Index, (DWORD_PTR )&(*it));
	}

	if (pos >= 0 && pos < mediaArray.size())
	{
		SelectItem(pos);
	}
	UpdateButtons();


#if 0
    POSITION MediaPosition = pLayer->GetHeadPosition();

    int p = 0;
    m_pWndList->DeleteAllItems();
    for(int i = 0; i < pLayer->GetCount(); i++ )
    {
        MEDIASETTING *pMedia = pLayer->GetAt(MediaPosition);

        CString str = pMedia->MediaFile;
        if (pMedia->MediaType.CompareNoCase(_T("S3WebBrowser")) != 0)
        {
            str = pMedia->MediaName;
        }
        else
        {
            if (::PathFileExists(str))
            {
                p = str.ReverseFind('\\');
                if (p != -1)
                {
                    str = str.Right(str.GetLength() - p - 1);
                }
            }
        }

        if (pMedia->MediaType == szTypeName_Message)
        {
            p = str.ReverseFind('.');
            if (p != -1)
            {
                str = str.Left(p);
            }
        }
        int Index = AddItem(str);
        SetItemData(Index, (DWORD_PTR )pMedia);

        pLayer->GetNext(MediaPosition);
    }
    if (pos >= 0 && pos < pLayer->GetCount())
    {
        SelectItem(pos);
    }
    UpdateButtons();
/*
    if (m_pLayer->GetCount() > 0)
    {
        EnableButton(GetButtonNum(AFX_VSLISTBOX_BTN_DELETE_ID), TRUE);
    }
*/
#endif

}

void CMediaListBox::OnClickButton(int iButton)
{
    ASSERT(m_layerInfo);
    if (m_layerInfo)
    {
        int pos = -1;
        UINT uiBtnID = GetButtonID(iButton);
        int iSelItem = GetSelItem();

        if(uiBtnID == AFX_VSLISTBOX_BTN_NEW_ID)
        {
			std::shared_ptr<MediaElement> NewMediaEle;
			m_layerInfo->CreateMedia(NewMediaEle);
			NewMediaEle->SetMediaType(szTypeName_EmptyContent);
			NewMediaEle->SetMediaFile(szTypeName_EmptyContent);
			NewMediaEle->SetMediaName(szTypeName_EmptyContent);
			NewMediaEle->SetTextSetting(NULL);
			NewMediaEle->SetDesigner2(NULL);
			NewMediaEle->SetDesigner(NULL);
			NewMediaEle->SetBGColor(RGB(0,0,0));

            if (iSelItem == -1)
            {
				m_layerInfo->AddTailMediaElement(NewMediaEle);
                //m_pLayer->AddTail(pMediaSetting);
            }
            else
            {
                //m_pLayer->InsertAfter(m_pLayer->FindIndex(iSelItem), pMediaSetting);
				m_layerInfo->InsertAfter(NewMediaEle, iSelItem);
            }
            
            pos = iSelItem + 1;
            GetOwner()->SendMessage(WM_ON_MEDIA_NUM_CHANGE, (WPARAM)ACTION_ADD, (LPARAM)&NewMediaEle);
        }
        else if (iSelItem >=0 && m_layerInfo->GetMediasCount() > iSelItem)
        {
            if (uiBtnID == AFX_VSLISTBOX_BTN_DELETE_ID)
            {
				std::shared_ptr<MediaElement> MediaEle = m_layerInfo->GetMediaElement(iSelItem);			
				m_layerInfo->RemoveAt(iSelItem);
                GetOwner()->SendMessage(WM_ON_MEDIA_NUM_CHANGE, (WPARAM)MAKEWPARAM(ACTION_DEL,iSelItem), (LPARAM)&MediaEle);
                pos = iSelItem - 1;
            }
            else if (uiBtnID == AFX_VSLISTBOX_BTN_UP_ID)
            {
				
                GetOwner()->SendMessage(WM_ON_MEDIA_ITEM_CHANGE, ACTION_ITEM_MOVE_UP, 0);
                SwapLayer(iSelItem, iSelItem - 1);
                pos = iSelItem - 1;
				
            }
            else if (uiBtnID == AFX_VSLISTBOX_BTN_DOWN_ID)
            {
                GetOwner()->SendMessage(WM_ON_MEDIA_ITEM_CHANGE, ACTION_ITEM_MOVE_DOWN, 0);
                SwapLayer(iSelItem, iSelItem + 1);
                pos = iSelItem + 1;
				//GetOwner()->SendMessage(WM_ON_MEDIA_ITEM_CHANGE, 3, 0);
            }
        }
        UpdateButtons();
        SetContent(m_layerInfo, pos);
        if (pos == -1)
        {
            LPARAM x;
            ((CPropertyLayer*)GetParent())->OnLvnItemchangedMediaList(NULL, &x);
        }
    }

    //CVSListBox::OnClickButton(iButton);
}

void CMediaListBox::UpdateButtons()
{
    // enable/disable up/down button
    int i = GetSelItem();

    EnableButton(GetButtonNum(AFX_VSLISTBOX_BTN_DELETE_ID), TRUE);
    EnableButton(GetButtonNum(AFX_VSLISTBOX_BTN_UP_ID), TRUE);
    EnableButton(GetButtonNum(AFX_VSLISTBOX_BTN_DOWN_ID), TRUE);
    EnableButton(GetButtonNum(AFX_VSLISTBOX_BTN_NEW_ID), TRUE);

    // nothing selected
    if (i == -1)
    {
        EnableButton(GetButtonNum(AFX_VSLISTBOX_BTN_DELETE_ID), FALSE);
        EnableButton(GetButtonNum(AFX_VSLISTBOX_BTN_UP_ID), FALSE);
        EnableButton(GetButtonNum(AFX_VSLISTBOX_BTN_DOWN_ID), FALSE);
    }
    // disable to move up
    if (i == 0)
    {
        EnableButton(GetButtonNum(AFX_VSLISTBOX_BTN_UP_ID), FALSE);
    }
    // disable to move down
    if (i == GetCount() - 1)
    {
        EnableButton(GetButtonNum(AFX_VSLISTBOX_BTN_DOWN_ID), FALSE);
    }
}

void CMediaListBox::SwapLayer(int pos1, int pos2)
{
    ASSERT(pos1 != pos2 && m_layerInfo);
    
    m_layerInfo->SwapMedia(pos1, pos2);

}

BOOL CMediaListBox::EditItem(int iIndex)
{
#ifdef STARTER_EDITION
	std::shared_ptr<MediaElement> mediaEle = m_layerInfo->GetMediaElement(iIndex);
	CString mediaType = mediaEle->GetMediaType();
	if (mediaType.CompareNoCase(_T("S3WebBrowser")) == 0)
	{
		return CVSListBox::EditItem(iIndex);
	}
#endif
    return TRUE;
}

void CMediaListBox::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    CVSListBox::OnEndLabelEdit(pNMHDR, pResult);
    *pResult = 0;

    int iIndex = GetSelItem();
    if (m_layerInfo)
    {
		std::shared_ptr<MediaElement> mediaEle = m_layerInfo->GetMediaElement(iIndex);
        if(NULL != mediaEle)
		{
			CString mediaType = mediaEle->GetMediaType();
			if (mediaType.CompareNoCase(_T("S3WebBrowser")) == 0)
			{
				mediaEle->SetMediaFile(GetItemText(iIndex));
				CString mediaFile = mediaEle->GetMediaFile();
				if(mediaFile.Find(_T(':')) == -1)
				{
					mediaFile = _T("http://") + mediaFile;
					mediaEle->SetMediaFile(mediaFile);
				}
			}
		}
    }

    UpdateButtons();
}

LRESULT CMediaListBox::OnInitControl(WPARAM wParam, LPARAM lParam)
{
    DWORD dwSize = (DWORD)wParam;
    BYTE* pbInitData = (BYTE*)lParam;

    CString strDst;
    CMFCControlContainer::UTF8ToString((LPSTR)pbInitData, strDst, dwSize);

    CTagManager tagManager(strDst);

    BOOL bBrowseButton = TRUE;
    if (CMFCControlContainer::ReadBoolProp(tagManager, PS_MFCVSListbox_BrowseButton, bBrowseButton))
    {
        EnableBrowseButton(bBrowseButton);
    }

    UINT uiBtns = 0;

    BOOL bNewButton = FALSE;
    if (CMFCControlContainer::ReadBoolProp(tagManager, PS_MFCVSListbox_NewButton, bNewButton))
    {
        if (bNewButton && GetButtonNum(AFX_VSLISTBOX_BTN_NEW_ID) == -1)
        {
            uiBtns |= AFX_VSLISTBOX_BTN_NEW;
        }
    }

    BOOL bRemoveButton = FALSE;
    if (CMFCControlContainer::ReadBoolProp(tagManager, PS_MFCVSListbox_RemoveButton, bRemoveButton))
    {
        if (bRemoveButton && GetButtonNum(AFX_VSLISTBOX_BTN_DELETE_ID) == -1)
        {
            uiBtns |= AFX_VSLISTBOX_BTN_DELETE;
        }
    }

    BOOL bUpButton = FALSE;
    if (CMFCControlContainer::ReadBoolProp(tagManager, PS_MFCVSListbox_UpButton, bUpButton))
    {
        if (bUpButton && GetButtonNum(AFX_VSLISTBOX_BTN_UP_ID) == -1)
        {
            uiBtns |= AFX_VSLISTBOX_BTN_UP;
        }
    }

    BOOL bDownButton = FALSE;
    if (CMFCControlContainer::ReadBoolProp(tagManager, PS_MFCVSListbox_DownButton, bDownButton))
    {
        if (bDownButton && GetButtonNum(AFX_VSLISTBOX_BTN_DOWN_ID) == -1)
        {
            uiBtns |= AFX_VSLISTBOX_BTN_DOWN;
        }
    }

    if (uiBtns != 0)
    {
        MySetStandardButtons(uiBtns);
    }

	m_DragTarget.Register(this);
    m_BaseDragTarget.Register(this);
    return 0;
}

BOOL CMediaListBox::MySetStandardButtons(UINT uiBtns /*= AFX_VSLISTBOX_BTN_NEW | AFX_VSLISTBOX_BTN_DELETE | AFX_VSLISTBOX_BTN_UP | AFX_VSLISTBOX_BTN_DOWN*/)
{
    if (GetSafeHwnd() == NULL)
    {
        ASSERT(FALSE);
        return FALSE;
    }

    CString strButton;
    if (uiBtns & AFX_VSLISTBOX_BTN_NEW)
    {
        strButton = Translate(_T("Insert Empty Content"));
        ENSURE(AddButton(afxGlobalData.Is32BitIcons() ? IDB_AFXBARRES_NEW32 : IDB_AFXBARRES_NEW, strButton, VK_INSERT,0,AFX_VSLISTBOX_BTN_NEW_ID));
    }

    if (uiBtns & AFX_VSLISTBOX_BTN_DELETE)
    {
       // ENSURE(strButton.LoadString(IDS_AFXBARRES_DELETE));
		strButton = Translate(_T("Delete"));
        ENSURE(AddButton(afxGlobalData.Is32BitIcons() ? IDB_AFXBARRES_DELETE32 : IDB_AFXBARRES_DELETE, strButton, VK_DELETE, 0, AFX_VSLISTBOX_BTN_DELETE_ID));
    }

    if (uiBtns & AFX_VSLISTBOX_BTN_UP)
    {
      //  ENSURE(strButton.LoadString(IDS_AFXBARRES_MOVEUP));
		strButton = Translate(_T("Move to Top"));
        ENSURE(AddButton(afxGlobalData.Is32BitIcons() ? IDB_AFXBARRES_UP32 : IDB_AFXBARRES_UP, strButton, VK_UP, FALT, AFX_VSLISTBOX_BTN_UP_ID));
    }

    if (uiBtns & AFX_VSLISTBOX_BTN_DOWN)
    {
       // ENSURE(strButton.LoadString(IDS_AFXBARRES_MOVEDN));
		strButton = Translate(_T("Move to Bottom"));
        ENSURE(AddButton(afxGlobalData.Is32BitIcons() ? IDB_AFXBARRES_DOWN32 : IDB_AFXBARRES_DOWN, strButton, VK_DOWN, FALT, AFX_VSLISTBOX_BTN_DOWN_ID));
    }

    m_uiStandardBtns |= uiBtns;
    return TRUE;
}

// Add DragDrop code here, by Xiangyang
 DROPEFFECT  CMediaListBox::CanAccept(CString strId)
 {
     CString RefObjName;
     CPrvPropertyLayer *pOwner = (CPrvPropertyLayer *)GetOwner();
     pOwner->m_pDrawObj->GetName(RefObjName);
     RefObjName.MakeUpper();

     if( StringUtility::IsDigitalStr(strId.GetBuffer()))
         return DROPEFFECT_COPY;

     strId.MakeUpper();

     if(strId == RefObjName)
          return DROPEFFECT_COPY;

     if(strId == _T("EMPTYCONTENT"))
     {
         return DROPEFFECT_COPY;
     }

     if(strId == _T("MESSAGE"))
     {
         if(RefObjName == _T("LAYER"))
            return  DROPEFFECT_NONE;
     }

    
     return DROPEFFECT_NONE;
 }

DROPEFFECT CMediaListBox::OnDragOver(COleDataObject* pDataObject,
	DWORD /*dwKeyState*/, CPoint /*point*/)
{
	HGLOBAL hGlobal = pDataObject->GetGlobalData(CF_UNICODETEXT);

#ifdef  _UNICODE
	CString strId =(LPWSTR) GlobalLock(hGlobal);
#else
	CString strId = CW2A((LPWSTR)GlobalLock(hGlobal));
#endif

	GlobalUnlock(hGlobal);

    return  CanAccept(strId);
	
}
 
// zxy 2011.8.19
#define DEFAULT_LENGHT 256
LRESULT CMediaListBox::OnAddDropedObject(CS3OleDropInfo *pDrop ,CString strId)
{
     
    CPrvLayoutDesignerCtrl *pWnd = (CPrvLayoutDesignerCtrl*)
                                SearchWndObj(AfxGetMainWnd(),RUNTIME_CLASS(CLayoutDesignerCtrl));
    
    if(pWnd)
    {
        // temp solution ,issue is on hold.
        pWnd-> m_DragMediaFiles.clear();


 //this code call main page function to add media,but has  issue on layout z-order machin.        
		POBJINFO pObjInfo = new OBJINFO();
        pObjInfo->proType  = LAYER;

        DWORD nWidth = DEFAULT_LENGHT, nHeight = DEFAULT_LENGHT;
         pObjInfo->proType = USER_DEFINED_PLUGIN;

         if (strId.CompareNoCase(szTypeName_Message) == 0)
                pObjInfo->proType = MESSAGE;
            
	    else if (strId.CompareNoCase(szTypeName_EmptyContent) == 0)
				pObjInfo->proType = LAYER;
   
        CPrvPropertyLayer *pOwner = (CPrvPropertyLayer *)GetOwner(); 
        FRect pos ;
        pOwner->m_pDrawObj->GetLogicPos(pos);
       
        CPoint pt(pos.left,pos.top);
        pWnd->LayoutToClient(pt);
        pt.x += 1;
        pt.y += 1;
		
        pWnd->OnAddObject( pObjInfo, pt,nWidth,nHeight);
	//	GetOwner()->SendMessage(WM_ON_MEDIA_NUM_CHANGE, 0, 0);
		
		return S_OK;

#if 0
		MEDIASETTING *pNewMedia = new MEDIASETTING;
        int mediaID ;
        if (strId.CompareNoCase(szTypeName_Message) == 0)
            mediaID = MESSAGE;
        else if (strId.CompareNoCase(szTypeName_Layer) == 0)
            mediaID = LAYER;
        else
            mediaID = USER_DEFINED_PLUGIN;

        pNewMedia->MediaType = strId ;// DropMedias[0].GetName().c_str();
        pNewMedia->MediaName = strId;
        pNewMedia->nMediaID = mediaID;
      
        if(mediaID == MESSAGE)
        {
            pNewMedia->MediaType = szTypeName_Text;
            pNewMedia->pTextSetting = new S3SIGNAGE_TEXT_SETTING;
        }
        
        else if(mediaID == USER_DEFINED_PLUGIN)
        {
            POBJINFO pObjInfo = new OBJINFO();
            pObjInfo->proType  = USER_DEFINED_PLUGIN;

            DWORD nWidth = DEFAULT_LENGHT, nHeight = DEFAULT_LENGHT;
            CPrvPropertyLayer *pOwner = (CPrvPropertyLayer *)GetOwner(); 
            FRect pos ;
            pOwner->m_pDrawObj->GetLogicPos(pos);

            CPoint pt(pos.left,pos.top);
            pWnd->LayoutToClient(pt);
            pt.x += 1;
            pt.y += 1;
            pWnd->OnAddObject( pObjInfo, pt,nWidth,nHeight);
            if (pObjInfo)
            {
                delete pObjInfo;
                pObjInfo = NULL;
            }
            return S_OK;
        }
		CString objTypeName;
//		CPrvPropertyLayer *pOwner = (CPrvPropertyLayer *)GetOwner();
		pOwner->m_pDrawObj->GetName(objTypeName);
	
		PROPERTY_TYPE propertyType;
		pOwner->m_pDrawObj->GetPropertyType( propertyType);
		objTypeName.MakeUpper();

		m_pLayer->AddTail(pNewMedia);
		int nItem = this->AddItem(pNewMedia->MediaName);
		this->SetItemData(nItem,(DWORD_PTR)pNewMedia);
#endif
    }
    return S_OK;
}

BOOL CMediaListBox::OnDrop(COleDataObject* pDataObject,
	DROPEFFECT dropEffect, CPoint /*point*/)
{
   //return OnDrop(WPARAM(pDataObject),LPARAM(dropEffect));
	HGLOBAL hGlobal = pDataObject->GetGlobalData(CF_UNICODETEXT);

#ifdef  _UNICODE
	CString strId =(LPWSTR) GlobalLock(hGlobal);
#else
	CString strId = CW2A((LPWSTR)GlobalLock(hGlobal));
#endif

	GlobalUnlock(hGlobal);

	if(!StringUtility::IsDigitalStr(strId.GetBuffer()))
		return OnAddDropedObject((CS3OleDropInfo *)pDataObject,strId);

	int mediaID = StringUtility::stoi(strId.GetBuffer());

	vector<int> Medias;
	Medias.push_back(mediaID);
	vector<Media> DropMedias;
	 

	HRESULT hr = GetControllerInterface->GetMedias(Medias, DropMedias);

	if(!SUCCEEDED(hr) || DropMedias.empty())
		return FALSE;

	//drag one file at once time.
	CString strFileName = DropMedias[0].GetName().c_str(); 

	 
	//continue to add file.

	std::shared_ptr<MediaElement> NewMediaEle;
	m_layerInfo->CreateMedia(NewMediaEle);
	NewMediaEle->SetMediaID(mediaID);
	NewMediaEle->SetMediaFile(DropMedias[0].GetMediaServerPath().c_str());
	NewMediaEle->SetMediaName(DropMedias[0].GetName().c_str());

	DWORD Duration = DropMedias[0].GetDuration() / 1000;
	if (0 == Duration)
	{
		Duration = 30;
	}
	NewMediaEle->SetDuration(Duration);
	CPrvPage *pPage = (CPrvPage *)GetRuntimePageCtrl(RUNTIME_CLASS(CDesignerPage));
	CString szMediaType;
	pPage->QueryMediaType(NewMediaEle->GetMediaFile(),szMediaType);
	NewMediaEle->SetMediaType(szMediaType);

	//type filter here.
	CString objTypeName;
	CPrvPropertyLayer *pOwner = (CPrvPropertyLayer *)GetOwner();
	pOwner->m_pDrawObj->GetName(objTypeName);
	
	PROPERTY_TYPE propertyType;
	pOwner->m_pDrawObj->GetPropertyType( propertyType);
    objTypeName.MakeUpper();
	if(objTypeName != _T("LAYER") ||propertyType == AUDIO || (NewMediaEle->GetMediaType().CompareNoCase(_T("AUDIO")) == 0))
	{
		if( objTypeName != NewMediaEle->GetMediaType())
		{
			/*TRACE(_T(" %s, %s \n"),objTypeName, pNewMedia->MediaType);*/
#define Translate(x) x
			CString strPromt;
			if(NewMediaEle->GetMediaType().CompareNoCase(_T("MESSAGE")) == 0)
			{
				strPromt = Translate(_T("Only Message or EmptyContent can add to Message layer!"));
            }
            else if(NewMediaEle->GetMediaType().CompareNoCase(_T("USER_DEFINED_PLUGIN")) == 0)
            {
				strPromt = Translate(_T("Only same type component or EmptyContent can add to the layer!"));
            }
            else if(NewMediaEle->GetMediaType().CompareNoCase(_T("AUDIO")) == 0)
            {
                strPromt = Translate(_T("Only audio files can be added to audio layers!"));
            }
#undef Translate
			MessageBox(Translate(strPromt), Translate(_T("Error:Media listbox")), MB_OK|MB_ICONERROR);
			return FALSE;
		}
		
	}
	m_layerInfo->AddTailMediaElement(NewMediaEle);
	int nItem = this->AddItem(NewMediaEle->GetMediaName());
	this->SetItemData(nItem,(DWORD_PTR)&NewMediaEle);
	this->UpdateButtons();
	GetOwner()->SendMessage(WM_ON_MEDIA_NUM_CHANGE, (LPARAM)ACTION_ADD, (WPARAM)&NewMediaEle);

	return TRUE;


   
}

 