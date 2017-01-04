// PropertyCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "PropertyCtrl.h"

#include "LayoutPropertiesPane.h"
#include "DesignerPage.h"

#include "PropertyLayout.h"
#include "PropertyLayer.h"
#include "PropertyMessage.h"
#include "PropertyBasicContent.h"
#include "PropertyTransition.h"
#include "PropertyPlugin.h"
#include "PropertySimplePlugin.h"


// wParam - no used, lParam - no used
UINT WM_ON_TEXTSETTING_CHANGE = ::RegisterWindowMessage(_T("WM_ON_TEXTSETTING_CHANGE"));
// CPropertyCtrl

IMPLEMENT_DYNAMIC(CPropertyCtrl, CWnd)

    CPropertyCtrl::CPropertyCtrl()
{
    m_StartY = 0;

    m_layoutInfo = NULL;
    m_layerInfo.reset();
    m_pLayoutGroup = NULL;
    m_pDrawObj= NULL;

    m_pPropertyLayout           = NULL;
    m_pPropertyLayer            = NULL;
    m_pPropertyBasicContent     = NULL;
    m_pPropertyTransitionIn     = NULL;
    m_pPropertyTransitionOut    = NULL;
    m_pPropertyMessage          = NULL;
    m_pPropertyPlugin           = NULL;
    m_pPropertySimplePlugin     = NULL;

}

CPropertyCtrl::~CPropertyCtrl()
{
}


BEGIN_MESSAGE_MAP(CPropertyCtrl, CWnd)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_VSCROLL()
    ON_WM_MOUSEWHEEL()
    ON_WM_ERASEBKGND()
    ON_REGISTERED_MESSAGE(WM_ON_MEDIA_SEL_CHANGE, OnMediaChanged)
    ON_REGISTERED_MESSAGE(WM_ON_TIME_CHANGE, OnLayerTimeChanged)
	ON_REGISTERED_MESSAGE(WM_ON_TEXTSETTING_CHANGE, OnTextSettingChanged)
END_MESSAGE_MAP()



// CPropertyCtrl message handlers
int CPropertyCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    ModifyStyle(0, WS_CLIPCHILDREN);
    m_pPropertyLayout       = new CPropertyLayout(this);
    m_pPropertyLayer        = new CPropertyLayer(this);
    m_pPropertyBasicContent = new CPropertyBasicContent(this);
    m_pPropertyTransitionIn = new CPropertyTransition(this, TRUE);
    m_pPropertyTransitionOut = new CPropertyTransition(this, FALSE);
    m_pPropertyMessage      = new CPropertyMessage(this);
    m_pPropertyPlugin       = new CPropertyPlugin(this);
    m_pPropertySimplePlugin = new CPropertySimplePlugin(this);

    m_pPropertyLayout->Create(CPropertyLayout::IDD,this);
    m_pPropertyLayer->Create(CPropertyLayer::IDD,this);
    m_pPropertyBasicContent->Create(CPropertyBasicContent::IDD, this);
    m_pPropertyTransitionIn->Create(CPropertyTransition::IDD,this);
    m_pPropertyTransitionOut->Create(CPropertyTransition::IDD,this);
    m_pPropertyMessage->Create(CPropertyMessage::IDD,this);
    m_pPropertyPlugin->Create(CPropertyPlugin::IDD, this);
    m_pPropertySimplePlugin->Create(CPropertySimplePlugin::IDD,this);

    m_pPropertyLayout->ShowWindow(SW_HIDE);
    m_pPropertyLayer->ShowWindow(SW_HIDE);
    m_pPropertyBasicContent->ShowWindow(SW_HIDE);
    m_pPropertyTransitionIn->ShowWindow(SW_HIDE);
    m_pPropertyTransitionOut->ShowWindow(SW_HIDE);
    m_pPropertyMessage->ShowWindow(SW_HIDE);
    m_pPropertyPlugin->ShowWindow(SW_HIDE);
    m_pPropertySimplePlugin->ShowWindow(SW_HIDE);

    m_pPropertyLayout->ModifyStyle(0, WS_CLIPSIBLINGS);
    m_pPropertyLayer->ModifyStyle(0, WS_CLIPSIBLINGS);
    m_pPropertyBasicContent->ModifyStyle(0, WS_CLIPSIBLINGS);
    m_pPropertyTransitionIn->ModifyStyle(0, WS_CLIPSIBLINGS);
    m_pPropertyTransitionOut->ModifyStyle(0, WS_CLIPSIBLINGS);
    m_pPropertyMessage->ModifyStyle(0, WS_CLIPSIBLINGS);
    m_pPropertyPlugin->ModifyStyle(0, WS_CLIPSIBLINGS);
    m_pPropertySimplePlugin->ModifyStyle(0, WS_CLIPSIBLINGS);


    return 0;
}

void CPropertyCtrl::OnDestroy()
{
    ClearControlList();

    m_pPropertyLayout->DestroyWindow();
    m_pPropertyLayer->DestroyWindow();
    m_pPropertyBasicContent->DestroyWindow();
    m_pPropertyTransitionIn->DestroyWindow();
    m_pPropertyTransitionOut->DestroyWindow();
    m_pPropertyMessage->DestroyWindow();
    m_pPropertyPlugin->DestroyWindow();
    m_pPropertySimplePlugin->DestroyWindow();

    delete m_pPropertyLayout;
    delete m_pPropertyLayer;
    delete m_pPropertyBasicContent;
    delete m_pPropertyTransitionIn;
    delete m_pPropertyTransitionOut;
    delete m_pPropertyMessage;
    delete m_pPropertyPlugin;
    delete m_pPropertySimplePlugin;

    CWnd::OnDestroy();
}

VOID CPropertyCtrl::ClearControlList()
{
    for(int i=0; i< m_ControlList.GetCount(); i++)
    {
        //m_ControlList[i]->DestroyWindow();
        //delete m_ControlList[i];
        m_ControlList[i]->ShowWindow(SW_HIDE);

        // Clear the sub control point
        if(m_ControlList[i]->IsKindOf( RUNTIME_CLASS( CPropertyLayer )))
        {
            ((CPropertyLayer *)m_ControlList[i])->Clear();
        }
        else if(m_ControlList[i]->IsKindOf( RUNTIME_CLASS( CPropertyLayout )))
        {
            ((CPropertyLayout *)m_ControlList[i])->Clear();
        }
        else if(m_ControlList[i]->IsKindOf( RUNTIME_CLASS( CPropertyMessage )))
        {
            ((CPropertyMessage *)m_ControlList[i])->Clear();
        }
        else if(m_ControlList[i]->IsKindOf( RUNTIME_CLASS( CPropertyTransition )))
        {
            ((CPropertyTransition *)m_ControlList[i])->Clear();
        }
    }

    m_ControlList.RemoveAll();
    m_StartY = 0;
}

VOID CPropertyCtrl::CalcLayout()
{
    CRect ClientRect;
    GetClientRect(&ClientRect);

    int TotalLength = 0;
    for(int i=0; i<m_ControlList.GetCount(); i++ )
    {
        CRect Rect;
        m_ControlList[i]->GetWindowRect(Rect);

        TotalLength += Rect.Height();
    }

    int nCurPos = GetScrollPos(SB_VERT);
    m_StartY = -nCurPos;

    if(TotalLength > ClientRect.Height())
    {
        EnableScrollBarCtrl(SB_VERT, TRUE);
        EnableScrollBar(SB_VERT, ESB_ENABLE_BOTH );

        SCROLLINFO ScrollInfo;
        ScrollInfo.cbSize = sizeof(SCROLLINFO);
        ScrollInfo.nMin = 0;
        ScrollInfo.nMax = TotalLength;
        ScrollInfo.nPage = ClientRect.Height();
        ScrollInfo.fMask = SIF_RANGE | SIF_PAGE;

        SetScrollInfo(SB_VERT,&ScrollInfo);

        m_StartY = max(m_StartY, ClientRect.Height() - TotalLength );
    }else
    {
        EnableScrollBarCtrl(SB_VERT, FALSE);
        m_StartY = 0;
    }

    GetClientRect(&ClientRect);


    int StartY = m_StartY;
    for(int i=0; i<m_ControlList.GetCount(); i++ )
    {
        CRect oldRect, newRect;
        m_ControlList[i]->GetWindowRect(oldRect);
        m_ControlList[i]->MoveWindow(0, StartY, ClientRect.Width(), oldRect.Height(), TRUE);

        StartY += oldRect.Height();
    }

}


VOID CPropertyCtrl::AddLayoutControl(LayoutElement *layoutInfo, LAYOUTGROUP_LIST* pLayoutGroup, const CS3DrawObjList* pObjList)
{
    CPropertyLayout *pLayout = m_pPropertyLayout;
    pLayout->ShowWindow(SW_SHOWNOACTIVATE);
    pLayout->SetLayerList(pObjList);
    pLayout->SetContent(layoutInfo, pLayoutGroup);
    m_ControlList.Add(pLayout);


}

VOID CPropertyCtrl::AddLayerControl(std::shared_ptr<LayerElement> layerInfo, CS3DrawObj *pDrawObj)
{
    CPropertyLayer *pLayer = m_pPropertyLayer;
    pLayer->ShowWindow(SW_SHOWNOACTIVATE);
    m_ControlList.Add(pLayer);
    pLayer->SetContent(layerInfo, pDrawObj);

}

VOID CPropertyCtrl::AddBasicContentControl(std::shared_ptr<MediaElement> mediaInfo)
{
    CPropertyBasicContent *pContent = m_pPropertyBasicContent;

	if (mediaInfo->GetMediaType().CompareNoCase(_T("S3WebBrowser")) == 0)
	{
		CString str;
		if (pContent->GetDlgItemText(IDC_DURATION, str), str == Translate(_T("Duration")))
		{
			pContent->GetDlgItem(IDC_DURATION)->SetWindowText(Translate(_T("Refresh Frequency")));
		}
		
	}
	else
	{
		CString str;
		if (pContent->GetDlgItemText(IDC_DURATION, str), str == Translate(_T("Refresh Frequency")))
		{
			pContent->GetDlgItem(IDC_DURATION)->SetWindowText(Translate(_T("Duration")));
		}
	}

    pContent->ShowWindow(SW_SHOWNOACTIVATE);
    pContent->SetContent(mediaInfo);
    m_ControlList.Add(pContent);

}

VOID CPropertyCtrl::AddMessageControl(std::shared_ptr<S3SIGNAGE_TEXT_SETTING> &MessageSetting)
{
    CPropertyMessage *pMessage = m_pPropertyMessage;
    pMessage->ShowWindow(SW_SHOWNOACTIVATE);
    pMessage->SetContent(MessageSetting);
    m_ControlList.Add(pMessage);

}


VOID CPropertyCtrl::AddTransitionControl(S3SIGNAGE_TRANSIION *pTransitionSetting, BOOL inOut)
{
    CPropertyTransition *pTransition;
    if (inOut)
    {
        pTransition = m_pPropertyTransitionIn;
    }
    else
    {
        pTransition = m_pPropertyTransitionOut;
    }

    pTransition->ShowWindow(SW_SHOWNOACTIVATE);
    pTransition->SetContent(pTransitionSetting);
    m_ControlList.Add(pTransition);
}

VOID CPropertyCtrl::AddPluginControl(std::shared_ptr<IS3RODesigner2>& pDesigner2)
{
    CPropertyPlugin *pPlugin = m_pPropertyPlugin;
    pPlugin->ShowWindow(SW_SHOWNOACTIVATE);
    pPlugin->SetContent(pDesigner2);
    m_ControlList.Add(pPlugin);
}

VOID CPropertyCtrl::AddBasicPluginControl(std::shared_ptr<IS3RODesigner>& pDesigner,CS3DrawObj *pDrawObj)
{
    CPropertySimplePlugin *pPlugin = m_pPropertySimplePlugin;
    pPlugin->ShowWindow(SW_SHOWNOACTIVATE);
    pPlugin->SetContent(pDesigner,pDrawObj);
    m_ControlList.Add(pPlugin);
}

void CPropertyCtrl::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    CalcLayout();
}


/// In logic coordinate
void CPropertyCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    int nCurPos = GetScrollPos(SB_VERT);
    SCROLLINFO Info;
    GetScrollInfo(SB_VERT, &Info, SIF_ALL);


    switch (nSBCode)
    {
    case SB_TOP:
        nCurPos = Info.nMin;
        break;
    case SB_BOTTOM:
        nCurPos = Info.nMax;
        break;
    case SB_ENDSCROLL:
        break;
    case SB_LINEUP:
        nCurPos = max(Info.nMin, nCurPos - 10);
        break;
    case SB_LINEDOWN:
        nCurPos = min(Info.nMax, nCurPos + 10);
        break;
    case SB_PAGEUP:
        nCurPos = max(Info.nMin, nCurPos - (int)Info.nPage);
        break;
    case SB_PAGEDOWN:
        nCurPos = min(Info.nMax, nCurPos + (int)Info.nPage);
        break;
    case SB_THUMBPOSITION:
        nCurPos = nPos;
        break;
    case SB_THUMBTRACK:
        nCurPos = nPos;
        break;
    }

    SetScrollPos(SB_VERT, nCurPos);

    if (m_StartY != -nCurPos)
    {
        m_StartY = -nCurPos;

        CalcLayout();
    }


    CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CPropertyCtrl::OnEraseBkgnd(CDC* pDC)
{
    return CWnd::OnEraseBkgnd(pDC);
}


VOID CPropertyCtrl::SetLayout(LayoutElement *layoutInfo, LAYOUTGROUP_LIST *pLayoutGroup, const CS3DrawObjList* pObjList)
{
    if(m_layoutInfo != layoutInfo || m_pLayoutGroup != pLayoutGroup)
    {
        ClearControlList();
        AddLayoutControl(layoutInfo, pLayoutGroup, pObjList);
        CalcLayout();
    }else
    {
        UpdateLayoutControl(pObjList);
    }

    m_layoutInfo		= layoutInfo;
    m_pLayoutGroup		= pLayoutGroup;
    m_layerInfo			= NULL;
    m_pDrawObj			= NULL;

}

VOID CPropertyCtrl::SetLayer(std::shared_ptr<LayerElement> layerInfo, CS3DrawObj *pDrawObj)
{
    if(m_layerInfo.get() != layerInfo.get() || m_pDrawObj != pDrawObj)
    {
        ClearControlList();
        AddLayerControl(layerInfo, pDrawObj);
        CalcLayout();
    }else
    {
        UpdateLayerControl();
    }

    m_layoutInfo    = NULL;
    m_layerInfo		= layerInfo;
    m_pDrawObj		= pDrawObj;
}

VOID CPropertyCtrl::SetLayoutDuration(DWORD duration)
{
    CPropertyLayer *pLayer = NULL;
    // find layer control
    for(int i=0; i< m_ControlList.GetCount(); i++)
    {
        if(m_ControlList[i]->IsKindOf( RUNTIME_CLASS( CPropertyLayer )))
        {
            pLayer = (CPropertyLayer *)m_ControlList[i];
            break;
        }
    }

    if (pLayer)
    {
        pLayer->SetDuration(duration);
    }
}

VOID CPropertyCtrl::Clear()
{
    ClearControlList();
    m_layoutInfo	= NULL;
    m_pDrawObj		= NULL;
	m_layerInfo.reset();
    CalcLayout();
}

VOID CPropertyCtrl::UpdateLayerControl()
{
    for(int i=0; i< m_ControlList.GetCount(); i++)
    {
        if(m_ControlList[i]->IsKindOf( RUNTIME_CLASS( CPropertyLayer )))
        {
            ((CPropertyLayer *)m_ControlList[i])->SetContent(m_layerInfo, m_pDrawObj);
        }
    }
}


VOID CPropertyCtrl::UpdateLayoutControl(const CS3DrawObjList* pObjList)
{
    for(int i=0; i< m_ControlList.GetCount(); i++)
    {
        if(m_ControlList[i]->IsKindOf( RUNTIME_CLASS( CPropertyLayout )))
        {
            ((CPropertyLayout *)m_ControlList[i])->SetLayerList(pObjList);
            ((CPropertyLayout *)m_ControlList[i])->SetContent(m_layoutInfo, m_pLayoutGroup);
        }
    }

}

LRESULT CPropertyCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    ASSERT_VALID(this);

    LRESULT lResult;
    if(message >= 0xC000)
    {
        if (OnWndMsg(message, wParam, lParam, &lResult))
            return lResult;
        else
        {
            this->GetOwner()->SendMessage(message, wParam, lParam);
        }
    }

    return CWnd::WindowProc(message, wParam, lParam);
}

LRESULT CPropertyCtrl::OnTextSettingChanged(WPARAM wParam, LPARAM lParam)
{
	CPropertyLayer *pLayer = NULL;
	// find layer control
	for(int i=0; i< m_ControlList.GetCount(); i++)
	{
		if(m_ControlList[i]->IsKindOf( RUNTIME_CLASS( CPropertyLayer )))
		{
			pLayer = (CPropertyLayer *)m_ControlList[i];
			break;
		}
	}
	
	//copy from DesignerPage.cpp
	//HRESULT CDesignerPage::WriteLayoutFile(CString Filename, CString &ContentXML)
	std::shared_ptr<MediaElement> pMedia = pLayer->GetSelectedMedia();
	std::shared_ptr<S3SIGNAGE_TEXT_SETTING>  pTextSetting = pMedia->GetTextSetting();
	if(!(pMedia != NULL && pTextSetting))
	{    
		return S_FALSE;
	}

	// Write text message property settings
	if (!pTextSetting->pFontInfo)
	{
		return S_FALSE;
	}
		
	//split to multi-string by cr
	int index = -1, lastPosition = 0;
	CString szTmpMsg = pTextSetting->szMessage;
 	std::vector<CString> strs;
 	CString separate_str = _T("\x0D\x0A");
 	while (-1 != (index = szTmpMsg.Find(separate_str, lastPosition)))
 	{
 		strs.push_back(szTmpMsg.Mid(lastPosition, index - lastPosition));
 		lastPosition = index + separate_str.GetLength();
 	}
 	CString lastString = szTmpMsg.Mid(lastPosition);
 	if (0 != lastString.GetLength())
 	{
 		strs.push_back(lastString);
 	}

	LOGFONT logFont;
	
	pTextSetting->pFontInfo->GetLogFont(&logFont);
	int durationTime = 0;

	FRect LayerRect;
	m_pDrawObj->GetLogicPos(LayerRect);

// 	FLOAT fLayerScale;
// 	m_pDrawObj->GetScale(fLayerScale);

	//copy from S3ObjectContainer.cpp 
	//S3ObjectContainer::S3ObjectContainer(S3SIGNAGE_CONTENT  &Content, float Scale,  int nFPS)
	
// 	if (m_ctrlFont.GetSelFont() == NULL )
// 	{
// 		return S_FALSE;
// 	}
// 
// 	if (m_ShowTimes < 0)
// 	{
// 		m_ShowTimes = 0;
// 	}

// 	if (m_MessageContent.GetLength() == 0)
// 	{
// 		timeSpan = 0;
// 		return S_OK;
// 	}

// 	CString strFontSize;
// 	GetDlgItem(IDC_COMBO_SIZE)->GetWindowText(strFontSize);
// 
// 	ZeroMemory(&logFont, sizeof(LOGFONT));
// 	wcscpy_s(logFont.lfFaceName, m_ctrlFont.GetSelFont()->m_strName.GetString());
// 	logFont.lfWeight    = m_bBold ? FW_BOLD : FW_NORMAL;
// 	logFont.lfItalic    = m_bItalic;
// 	logFont.lfUnderline = m_bUnderline;
// 	logFont.lfHeight    = _tstoi(strFontSize.GetString());
// 	logFont.lfCharSet = DEFAULT_CHARSET;
// 	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
// 	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
// 	logFont.lfQuality = ANTIALIASED_QUALITY;
// 	logFont.lfPitchAndFamily = VARIABLE_PITCH;
	int totaldurationtime = 0;
	std::vector<CString>::iterator iter;
	for(iter = strs.begin(); iter < strs.end(); ++iter)
	{
		CString temp = *iter;
		
		//if (pMedia->pTextSetting->szMessage.GetLength() && pMedia->pTextSetting->FontSpeed > 0)
		if(temp.GetLength() && pTextSetting->FontSpeed > 0)
		{
			//FontInfo.CreateFontIndirect(&logFont);

			HDC  hDC       = CreateCompatibleDC( NULL );
			SIZE MessageSize;
			SelectObject(hDC, pTextSetting->pFontInfo->GetSafeHandle());

			GetTextExtentPoint32(hDC, temp, temp.GetLength(), &MessageSize);

			int iExtraWidth = 0;
			if (logFont.lfItalic == 1)
			{
				iExtraWidth = logFont.lfHeight/6 + 1;
			}

			//float width = GetSystemMetrics(SM_CXSCREEN);
			float width = LayerRect.Width();
			float calctime = (width + MessageSize.cx + iExtraWidth) / (float)pTextSetting->FontSpeed;
			durationTime = (int)(calctime*10);
			if(durationTime%10 > 4)
			{	
				durationTime /=10;
				durationTime += 1;
			}
			else
			{
				durationTime /=10;
			}
			totaldurationtime += durationTime;
			//durationTime = ceil((width + MessageSize.cx + iExtraWidth) / (float)pMedia->pTextSetting->FontSpeed);
			//durationTime = m_ShowTimes * durationTime;

		}
		else
		{
			durationTime = 0;
		}
	}
	

    CString str;
    str.Format(Translate(_T("Actual Run Time : %d seconds")), totaldurationtime);
    m_pPropertyMessage->SetActualRunTime(str);


	return S_OK;
}

LRESULT CPropertyCtrl::OnMediaChanged(WPARAM wParam, LPARAM lParam)
{
    CPropertyLayer *pLayer = NULL;
    // find layer control
    for(int i=0; i< m_ControlList.GetCount(); i++)
    {
        if(m_ControlList[i]->IsKindOf( RUNTIME_CLASS( CPropertyLayer )))
        {
            pLayer = (CPropertyLayer *)m_ControlList[i];
            break;
        }
    }
	if(NULL == pLayer)
		return FALSE;
    // remove all other layer
    for(int i=0; i< m_ControlList.GetCount(); i++)
    {
        if(!m_ControlList[i]->IsKindOf( RUNTIME_CLASS( CPropertyLayer )))
        {
            //m_ControlList[i]->DestroyWindow();
            //delete m_ControlList[i];
            m_ControlList[i]->ShowWindow(SW_HIDE);
            m_ControlList.RemoveAt(i);
            i--;
        }
    }

    std::shared_ptr<MediaElement> mediaInfo = pLayer->GetSelectedMedia();

    if(mediaInfo.get() != NULL)
    {
        AddBasicContentControl(mediaInfo);
        if(mediaInfo->GetTextSetting())
        {
            AddMessageControl(mediaInfo->GetTextSetting());
			PostMessage(WM_ON_TEXTSETTING_CHANGE, 0, 0);
        }

        CString szMediaType = mediaInfo->GetMediaType();
        if (szMediaType.CompareNoCase(_T("S3ImageViewer")) != 0 &&
            szMediaType.CompareNoCase(_T("S3PPT")) != 0 &&
            szMediaType.CompareNoCase(_T("S3EXE")) != 0 &&
            szMediaType.CompareNoCase(_T("S3FlashPlayer")) != 0 &&
            szMediaType.CompareNoCase(_T("S3WebBrowser")) != 0)
        {
            if (mediaInfo->GetDesigner2())
            {
                AddPluginControl(mediaInfo->GetDesigner2());
            }
            else if (mediaInfo->GetDesigner())
            {
                AddBasicPluginControl(mediaInfo->GetDesigner(),m_pDrawObj);
            }

        }
        if (szMediaType != szTypeName_Audio && szMediaType != szTypeName_EmptyContent)
        {
            AddTransitionControl(&mediaInfo->GetTransIn(), TRUE);
            AddTransitionControl(&mediaInfo->GetTransOut(), FALSE);
        }
    }
    CalcLayout();
    return TRUE;
}

LRESULT CPropertyCtrl::OnLayerTimeChanged(WPARAM wParam, LPARAM lParam)
{
    CPropertyLayer *pLayer = NULL;
    // find layer control
    for(int i=0; i< m_ControlList.GetCount(); i++)
    {
        if(m_ControlList[i]->IsKindOf( RUNTIME_CLASS( CPropertyLayer )))
        {
            pLayer = (CPropertyLayer *)m_ControlList[i];
            break;
        }
    }

    if (pLayer)
    {
        pLayer->ChangeLayerDuration();
    }

    return 0;
}

HWND CPropertyCtrl::GetNextFocusWindow(HWND hWnd, BOOL shift)
{
    if (m_ControlList.IsEmpty())
    {
        return NULL;
    }

    int listSize = m_ControlList.GetCount();
    if (shift)
    {
        for (int i = 0; i < listSize; ++i)
        {
            if (m_ControlList.GetAt(i)->GetSafeHwnd() == hWnd)
            {
                // first
                if (i == 0)
                {
                    return m_ControlList.GetAt(listSize - 1)->GetSafeHwnd();
                }
                else
                {
                    CDialogEx* dlg = (CDialogEx*)(m_ControlList.GetAt(i-1));
                    if (dlg->IsKindOf(RUNTIME_CLASS(CPropertyPlugin)))
                    {
                        CPropertyPlugin* pluginDlg = (CPropertyPlugin*)dlg;
                        if (pluginDlg->m_ControlList.GetCount() == 0)
                        {
                            return m_ControlList.GetAt(i-2)->GetSafeHwnd();
                        }
                        else
                        {
                            return m_ControlList.GetAt(i-1)->GetSafeHwnd();
                        }
                    }
                    else
                    {
                        return m_ControlList.GetAt(i-1)->GetSafeHwnd();
                    }
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < listSize; ++i)
        {
            if (m_ControlList.GetAt(i)->GetSafeHwnd() == hWnd)
            {
                if (i + 1 < listSize)
                {
                    CDialogEx* dlg = (CDialogEx*)(m_ControlList.GetAt(i+1));
                    if (dlg->IsKindOf(RUNTIME_CLASS(CPropertyPlugin)))
                    {
                        CPropertyPlugin* pluginDlg = (CPropertyPlugin*)dlg;
                        if (pluginDlg->m_ControlList.GetCount() == 0)
                        {
                            return m_ControlList.GetAt(i+2)->GetSafeHwnd();
                        }
                        else
                        {
                            return m_ControlList.GetAt(i+1)->GetSafeHwnd();
                        }
                    }
                    else
                    {
                        return m_ControlList.GetAt(i+1)->GetSafeHwnd();
                    }
                }
                // last one
                else
                {
                    return m_ControlList.GetAt(0)->GetSafeHwnd();
                }
            }
        }
    }
    return NULL;
}

BOOL CPropertyCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    if(zDelta < 0)
    {
        OnVScroll(SB_LINEDOWN, GetScrollPos(SB_VERT), GetScrollBarCtrl(SB_VERT));
    }
    else if (zDelta > 0)
    {
        OnVScroll(SB_LINEUP, GetScrollPos(SB_VERT), GetScrollBarCtrl(SB_VERT));
    }
    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

bool CPropertyCtrl::CheckTransitionTime()
{
    CPropertyLayer *pLayer = NULL;
    // find layer control
    for(int i=0; i< m_ControlList.GetCount(); i++)
    {
        if(m_ControlList[i]->IsKindOf( RUNTIME_CLASS( CPropertyLayer )))
        {
            pLayer = (CPropertyLayer *)m_ControlList[i];
            break;
        }
    }

    if (pLayer)
    {
        std::shared_ptr<MediaElement> mediaSetting = pLayer->GetSelectedMedia();
        if (mediaSetting)
        {
			S3SIGNAGE_TRANSIION transIn = mediaSetting->GetTransIn();
			S3SIGNAGE_TRANSIION transOut = mediaSetting->GetTransOut();
            if (mediaSetting->GetDuration() >= transIn.Duration + transOut.Duration)
                return true;
            else
                return false;
        }
    }
    else
    {
        ASSERT(0);
    }

    return true;
}
