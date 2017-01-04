// F:\p4\sw\Demo\ContentManagement\Controller\ComponentListCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ComponentListCtrl.h"
#include "LayoutSettings.h"
#include "Localization/Localization.h"

// CComponentListCtrl

IMPLEMENT_DYNAMIC(CComponentListCtrl, CListCtrl)

// CComponentListCtrl construction/destruction

CComponentListCtrl::CComponentListCtrl()
{
    m_nImageWidth   = 32;
    m_nImageHeight  = 32;
    m_nSelectedItem = 0;
	/* LAYOUT-DESIGNER-NEW-VER-RAYMONDLIU
    COMPONENT_INFO* pLayerInfo      = new COMPONENT_INFO;
    pLayerInfo->szName              = szTypeName_Layer;
    pLayerInfo->hIcon = AfxGetApp()->LoadIconW(IDI_LAYER);
	*/
    COMPONENT_INFO* pMessageInfo    = new COMPONENT_INFO;
    pMessageInfo->szName            = szTypeName_Message;
    pMessageInfo->hIcon = AfxGetApp()->LoadIconW(IDI_MESSAGE);
	/* LAYOUT-DESIGNER-NEW-VER-RAYMONDLIU
    COMPONENT_INFO* pAudio          = new COMPONENT_INFO;
    pAudio->szName                  = szTypeName_Audio;
    pAudio->hIcon = AfxGetApp()->LoadIconW(IDI_AUDIO);
	*/
    COMPONENT_INFO* pEmptyContent   = new COMPONENT_INFO;
    pEmptyContent->szName           = szTypeName_EmptyContent;
    pEmptyContent->hIcon = AfxGetApp()->LoadIconW(IDI_EMPTYCONTENT);
    /* LAYOUT-DESIGNER-NEW-VER-RAYMONDLIU
	m_Components.Add(pLayerInfo);
	*/
    m_Components.Add(pMessageInfo);
	/* LAYOUT-DESIGNER-NEW-VER-RAYMONDLIU
    m_Components.Add(pAudio);
	*/
    m_Components.Add(pEmptyContent);
}

CComponentListCtrl::~CComponentListCtrl()
{
    for (int i = 0; i < m_Components.GetCount(); i++)
    {
        COMPONENT_INFO* pInfo = (COMPONENT_INFO*)m_Components.GetAt(i);
        if (pInfo)
        {
            delete pInfo;
            pInfo = NULL;
        }
    }

    m_Components.RemoveAll();
}

BEGIN_MESSAGE_MAP(CComponentListCtrl, CListCtrl)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnLvnBeginDrag)
    ON_NOTIFY_REFLECT(LVN_GETINFOTIP, OnGetToolTip)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGENTER, OnDragEnter)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGOVER, OnDragOver)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGLEAVE, OnDragLeave)
    ON_REGISTERED_MESSAGE(WM_DND_DROP, OnDrop)
END_MESSAGE_MAP()



BOOL CComponentListCtrl::SetViewType(DWORD dwViewType)
{
    return(ModifyStyle(LVS_TYPEMASK, dwViewType & LVS_TYPEMASK));
}

DWORD CComponentListCtrl::GetViewType()
{
    return(GetStyle() & LVS_TYPEMASK);
}

// CComponentListCtrl message handlers
BOOL CComponentListCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style |= LVS_ICON | LVS_AUTOARRANGE;

    return CListCtrl::PreCreateWindow(cs);
}

int CComponentListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CListCtrl::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    SetExtendedStyle(GetExtendedStyle() | LVS_EX_INFOTIP);

    // register drop target
    if ( m_DropTarget.Register( this ) )
        return 0;
    else
        return -1;

    return 0;
}

void CComponentListCtrl::OnDestroy()
{
    CListCtrl::OnDestroy();
}

void CComponentListCtrl::InitPluginObj(std::vector<PLUGIN_INFO*>& pArrRenderableObj)
{
    HRESULT hr = S_OK;
    BOOL bPluginSupportFiles = FALSE;

    for (size_t i = 0; i < pArrRenderableObj.size(); i++)
    {
        bPluginSupportFiles = FALSE;
        COMPONENT_INFO* pInfo = new COMPONENT_INFO;
        PLUGIN_INFO* pPlugin = pArrRenderableObj[i];
        pInfo->szName = pPlugin->szPluginName;

        // Query the plugin support files' list, if the plugin support files, do not show it in component list
        PLUGIN_SUPPORTFILES* pSupportList = new PLUGIN_SUPPORTFILES;
        pSupportList->nCount = 0;
        pSupportList->pSupportFile = NULL;
        hr = pPlugin->pRenderableObj->GetSupportedFiles(&pSupportList->nCount, pSupportList->pSupportFile);
        if (SUCCEEDED(hr) && pSupportList->nCount > 0)
        {
            bPluginSupportFiles = TRUE;
        }

        if (pSupportList)
        {
            delete pSupportList;
            pSupportList = NULL;
        }

        if (!bPluginSupportFiles                               
#ifdef STARTER_EDITION
            || pInfo->szName.CompareNoCase(_T("S3WebBrowser")) == 0 
#endif
            || pInfo->szName.CompareNoCase(_T("S3HikCapture")) == 0 
            || pInfo->szName.CompareNoCase(_T("S3DaHuaCapture")) == 0
            || pInfo->szName.CompareNoCase(_T("S3Weather")) == 0)
        {
            hr = pPlugin->pRenderableObj->GetIcon((HANDLE*)&pInfo->hIcon);
            if (hr == S_OK)
            {
                m_Components.Add(pInfo);
            }else
            {
                delete pInfo;
            }
        }else
        {
            delete pInfo;
        }
    }
}

BOOL CComponentListCtrl::GetSelectedItemName(CString& itemName)
{
    BOOL bSelected = FALSE;

    COMPONENT_INFO* pInfo = (COMPONENT_INFO*)m_Components.GetAt(m_nSelectedItem);
    if (pInfo)
    {
        itemName = pInfo->szName;
        bSelected = TRUE;
    }

    return bSelected;
}

void CComponentListCtrl::GetComponentNames(CStringArray& itemNames)
{
    for (int i = 0; i < m_Components.GetCount(); i++)
    {
        COMPONENT_INFO* pInfo = (COMPONENT_INFO*)m_Components.GetAt(i);
        itemNames.Add(pInfo->szName);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CComponentListCtrl initialization

void CComponentListCtrl::OnInitialUpdate()
{
    int nCount = m_Components.GetCount();
    // set image lists

    m_LargeImageList.Create(m_nImageWidth, m_nImageHeight, ILC_COLOR32|ILC_MASK, nCount, 1);

    for (int i = 0; i < nCount; i++)
    {
        COMPONENT_INFO* pInfo = (COMPONENT_INFO*)m_Components.GetAt(i);
        m_LargeImageList.Add(pInfo->hIcon);
    }

    SetImageList(&m_LargeImageList, LVSIL_NORMAL);

    // insert items

    LV_ITEM lvi;

    int nIdx = 0;
    for (int i = 0; i < m_Components.GetCount(); i++)
    {
        lvi.mask = LVIF_TEXT | LVIF_IMAGE;
        lvi.iItem = nIdx;
        lvi.iSubItem = 0;
        COMPONENT_INFO* pInfo = (COMPONENT_INFO*)m_Components.GetAt(i);
        lvi.pszText = (LPWSTR)pInfo->szName.GetString();
        lvi.iImage = nIdx;

        InsertItem(&lvi);

        nIdx += 1;
    }

    if (GetViewType() != LVS_ICON)
        SetViewType(LVS_ICON);
}

void CComponentListCtrl::OnLvnBeginDrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    CString strCurDragedItemDesc = NULL;
    int nLength = 0;

    LPNMITEMACTIVATE pNMItemActive = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    for (int i = 0; i < GetItemCount(); i++)
    {
        if (GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
        {
            COMPONENT_INFO* pInfo = (COMPONENT_INFO*)m_Components.GetAt(i);
            strCurDragedItemDesc = pInfo->szName;
            m_nSelectedItem = i;
        }
    }

    if (strCurDragedItemDesc.GetLength() == 0)
    {
        return;
    }

    nLength = (strCurDragedItemDesc.GetLength() + 1) * sizeof(TCHAR);

    HGLOBAL hgData = GlobalAlloc(GHND|GMEM_SHARE, nLength);
    ASSERT(hgData!=NULL);

    TCHAR* lpData = (TCHAR*)::GlobalLock(hgData);
    ASSERT(lpData!=NULL);

    _tcscpy_s(lpData, strCurDragedItemDesc.GetLength() + 1, strCurDragedItemDesc);

    ::GlobalUnlock(hgData);

    // Cache the data, and initiate DragDrop
    m_OleDataSource.CacheGlobalData(CF_UNICODETEXT, hgData);

    DROPEFFECT dropEffect
        = m_OleDataSource.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE);

    m_OleDataSource.Empty();

}

void CComponentListCtrl::OnGetToolTip(NMHDR* pNMHDR, LRESULT *pResult)
{
    LPNMLVGETINFOTIPW pToolTip = (LPNMLVGETINFOTIPW)pNMHDR;

    CString szTip;
    COMPONENT_INFO* pInfo = (COMPONENT_INFO*)m_Components.GetAt(pToolTip->iItem);
    _tcscpy_s(pToolTip->pszText, INFOTIPSIZE, Translate((LPCWSTR)pInfo->szName));

    pResult = 0;

}

LRESULT CComponentListCtrl::OnDrop(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0;

    return lr;
}

LRESULT CComponentListCtrl::OnDragEnter(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = DROPEFFECT_COPY;

    return lr;
}

LRESULT CComponentListCtrl::OnDragOver(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = DROPEFFECT_COPY;

    return lr;
}

LRESULT CComponentListCtrl::OnDragLeave(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0;

    return lr;
}