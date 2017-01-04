// F:\p4\sw\Demo\ContentManagement\Controller\MediaFileListCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "MediaFileListCtrl.h"
#include "Localization/Localization.h"

// CMediaFileListCtrl

IMPLEMENT_DYNAMIC(CMediaFileListCtrl, CMFCShellListCtrl)

CMediaFileListCtrl::CMediaFileListCtrl()
{
}

CMediaFileListCtrl::~CMediaFileListCtrl()
{
}

void CMediaFileListCtrl::SetPluginSupportFiles(std::map<CString, PLUGIN_SUPPORTFILES*>* pPluginFiles)
{
    m_PluginFiles = pPluginFiles;
}


BEGIN_MESSAGE_MAP(CMediaFileListCtrl, CMFCShellListCtrl)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_NOTIFY_REFLECT(NM_CLICK, OnNMClickFile)
    ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnLvnBeginDrag)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGENTER, OnDragEnter)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGOVER, OnDragOver)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGLEAVE, OnDragLeave)
    ON_REGISTERED_MESSAGE(WM_DND_DROP, OnDrop)
END_MESSAGE_MAP()


HRESULT CMediaFileListCtrl::EnumObjects(LPSHELLFOLDER pParentFolder, LPITEMIDLIST pidlParent)
{
    ASSERT_VALID(this);
    ASSERT_VALID(GetControllerApp->GetShellManager());

    LPENUMIDLIST pEnum = NULL;
    HRESULT hRes = pParentFolder->EnumObjects(NULL, m_nTypes, &pEnum);

    if (SUCCEEDED(hRes) && pEnum != NULL)
    {
        LPITEMIDLIST pidlTemp;
        DWORD dwFetched = 1;
        LPAFX_SHELLITEMINFO pItem;

        //enumerate the item's PIDLs
        while (pEnum->Next(1, &pidlTemp, &dwFetched) == S_OK && dwFetched)
        {
            LVITEM lvItem;
            ZeroMemory(&lvItem, sizeof(lvItem));

            //fill in the TV_ITEM structure for this item
            lvItem.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;

            //AddRef the parent folder so it's pointer stays valid
            pParentFolder->AddRef();

            //put the private information in the lParam
            pItem = (LPAFX_SHELLITEMINFO)GlobalAlloc(GPTR, sizeof(AFX_SHELLITEMINFO));

            pItem->pidlRel = pidlTemp;
            pItem->pidlFQ = GetControllerApp->GetShellManager()->ConcatenateItem(pidlParent, pidlTemp);

            pItem->pParentFolder = pParentFolder;
            lvItem.lParam = (LPARAM)pItem;

            lvItem.pszText = _T("");
            lvItem.iImage = OnGetItemIcon(GetItemCount(), pItem);

            //determine if the item is shared
            DWORD dwAttr = SFGAO_FOLDER | SFGAO_DISPLAYATTRMASK;
            pParentFolder->GetAttributesOf(1, (LPCITEMIDLIST*)&pidlTemp, &dwAttr);

            if (dwAttr & SFGAO_SHARE)
            {
                lvItem.mask |= LVIF_STATE;
                lvItem.stateMask |= LVIS_OVERLAYMASK;
                lvItem.state |= INDEXTOOVERLAYMASK(1); //1 is the index for the shared overlay image
            }

            if (dwAttr & SFGAO_GHOSTED)
            {
                lvItem.mask |= LVIF_STATE;
                lvItem.stateMask |= LVIS_CUT;
                lvItem.state |= LVIS_CUT;
            }

            if (dwAttr & SFGAO_FOLDER)
            {
                continue;
            }
            else
            {
                //SHFILEINFO sFI;
                TCHAR szPath[MAX_PATH];
                CString szFile;
                CString szExt;
                BOOL bFilter = FALSE;

                if (SHGetPathFromIDList(pItem->pidlFQ, szPath))
                {
                    szExt = ::PathFindExtension(szPath);
                    int n = szExt.ReverseFind('.');
                    if (n != -1)
                    {
                        szExt = szExt.Mid(n + 1);
                        for (UINT i = 0; i < nVideoSupport; i++)
                        {
                            if (szExt.CompareNoCase(szVideoSupportType[i]) == 0)
                            {
                                bFilter = TRUE;
                                break;
                            }
                        }

                        if (!bFilter)
                        {
                            for (UINT i = 0; i < nAudioSupport; i++)
                            {
                                if (szExt.CompareNoCase(szAudioSupportType[i]) == 0)
                                {
                                    bFilter = TRUE;
                                    break;
                                }
                            }
                        }

                        if (!bFilter)
                        {
                            CString szPluginName;
                            PPLUGIN_SUPPORTFILES pSupportList = NULL;
                           
                            for (auto it = m_PluginFiles->begin(); it != m_PluginFiles->end(); ++it)
                            {
                                szPluginName = it->first;
                                pSupportList = it->second;
                                for (DWORD i = 0; i < pSupportList->nCount; i++)
                                {
                                    if (szExt.CompareNoCase(pSupportList->pSupportFile[i].FileExt) == 0)
                                    {
                                        bFilter = TRUE;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                if (!bFilter)
                    continue;
            }

            int iItem = InsertItem(&lvItem);
            if (iItem >= 0)
            {
                // Set columns:
                const int nColumns = m_wndHeader.GetItemCount();
                for (int iColumn = 0; iColumn < nColumns; iColumn++)
                {
                    SetItemText(iItem, iColumn, OnGetItemText(iItem, iColumn, pItem));
                }
            }

            dwFetched = 0;
        }

        pEnum->Release();
    }

    return hRes;
}


// CMediaFileListCtrl message handlers
int CMediaFileListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CMFCShellListCtrl::OnCreate(lpCreateStruct) == -1)
        return -1;

    // register drop target
    if ( m_DropTarget.Register( this ) )
        return 0;
    else
        return -1;

    return 0;
}

void CMediaFileListCtrl::OnDestroy()
{
    CMFCShellListCtrl::OnDestroy();
}

void CMediaFileListCtrl::OnNMClickFile(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    POSITION pos = GetFirstSelectedItemPosition();
    int nItem = GetNextSelectedItem(pos); 

    CString strCurDragedItemDesc = NULL;
    CString strFileName = NULL;
    int nLength = 0;
    DWORD64 nFileSize = 0;
    LARGE_INTEGER fileSize;
    HANDLE hFile = NULL;
    CString sMessage;

    if (nItem >= 0)
    {
        GetItemPath(strCurDragedItemDesc, nItem);

        if (::PathFileExists(strCurDragedItemDesc))
        {
            hFile = ::CreateFile(strCurDragedItemDesc, FILE_GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == INVALID_HANDLE_VALUE)
            {
                sMessage.Format(Translate(_T("Failed to get information of file: %s")), strCurDragedItemDesc);
            }
            else
            {
                if (::GetFileSizeEx(hFile, &fileSize))
                {
                    nFileSize = fileSize.QuadPart;
                }

                CloseHandle(hFile);

                strFileName = ::PathFindFileName(strCurDragedItemDesc);

				CString fileStr, sizeStr;
				fileStr.Format(_T("\" %s \""), strFileName);
				
                if(nFileSize < 1024)
                {
                    sizeStr.Format(_T("%lld"), nFileSize); 
				    sMessage = Translate(_T("Select media file:")) + fileStr +  Translate(_T("the size is:")) + sizeStr + Translate(_T(" bytes."));
                }
                else if(nFileSize < 1024 * 1024)
                {
					sizeStr.Format(_T("%0.0f"), (float)nFileSize/1024.0f); 
				    sMessage = Translate(_T("Select media file:")) + fileStr +  Translate(_T("the size is:")) + sizeStr + _T(" KB."); 
                }
                else
                {
					sizeStr.Format(_T("%0.0f"), (float)nFileSize/1024.0f/1024.0f); 
				    sMessage = Translate(_T("Select media file:")) + fileStr +  Translate(_T("the size is:")) + sizeStr + _T(" MB.");
                }

                GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, (LPARAM)sMessage.GetString());
            }
        }
    }

    pResult = 0;

}

void CMediaFileListCtrl::OnLvnBeginDrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    CString strCurDragedItemDesc = _T("");
    int nCurDragItemIndex = 0;
    int nLength = 0;

    LPNMITEMACTIVATE pNMItemActive = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    for (int i = 0; i < GetItemCount(); i++)
    {
        if (GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
        {
            GetItemPath(strCurDragedItemDesc, i);
            nCurDragItemIndex = i;
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


void CMediaFileListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{

    CMFCShellListCtrl::OnLButtonDown(nFlags, point);
}

void CMediaFileListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{

    CMFCShellListCtrl::OnLButtonUp(nFlags, point);
}

void CMediaFileListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{

    CMFCShellListCtrl::OnMouseMove(nFlags, point);
}

LRESULT CMediaFileListCtrl::OnDrop(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1;

    return lr;
}

LRESULT CMediaFileListCtrl::OnDragEnter(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = DROPEFFECT_COPY;

    return lr;
}

LRESULT CMediaFileListCtrl::OnDragOver(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = DROPEFFECT_COPY;

    return lr;
}

LRESULT CMediaFileListCtrl::OnDragLeave(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0;

    return lr;
}