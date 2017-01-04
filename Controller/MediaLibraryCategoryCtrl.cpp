// MediaLibraryCategory.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "MediaLibraryCategoryCtrl.h"
#include "Localization/Localization.h"
#include "Medialibrarypage.h"
#include <algorithm>
#include "BorrowSkin/ControllerVisualManager.h"

UINT MEDIA_LIST_CATEGORY			= ::RegisterWindowMessage (_T("MEDIA_LIST_CATEGORY"));
//UINT MEDIA_CATEGORY_REFRESH         = ::RegisterWindowMessage(_T("MEDIA_CATEGORY_REFRESH"));
UINT MEDIA_SETACTIVE_TAB            = ::RegisterWindowMessage(_T("MEDIA_SETACTIVE_TAB"));


CLIPFORMAT CMediaLibraryCategoryCtrl::s_MediaFormat = ::RegisterClipboardFormat(_T("MAGICVIEW_MEDIA"));
CLIPFORMAT CMediaLibraryCategoryCtrl::s_MediaCategoryFormat = ::RegisterClipboardFormat (_T("MAGICVIEW_MEDIA_CATEGORY"));
// CMediaLibraryCategoryCtrl dialog
static HTREEITEM  g_CurItem;
IMPLEMENT_DYNAMIC(CMediaLibraryCategoryCtrl, CDialogCtrlTreeBase)

CMediaLibraryCategoryCtrl::CMediaLibraryCategoryCtrl(CWnd* pParent /*=NULL*/, BOOL bShowFiles)
	: CDialogCtrlTreeBase(CMediaLibraryCategoryCtrl::IDD, pParent),
    m_bShowFiles (bShowFiles) , m_RootCategoryID (-10),m_newAddingItem(0)
{

}

CMediaLibraryCategoryCtrl::~CMediaLibraryCategoryCtrl()
{

}

void CMediaLibraryCategoryCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_FILTER_TREE, m_ctrlCategoryTree);
    DDX_Control(pDX, IDC_FILTER_CATEGORY_NEW, m_ctrlCategoryNew);
    DDX_Control(pDX, IDC_FILTER_CATEGORY_DEL, m_ctrlCategoryDelete);
    DDX_Control(pDX, IDC_FILTER_CATEGORY_RENAME, m_ctrlCategoryRename);
    DDX_Control(pDX, IDC_FILTER_CATEGORY_REFRESH, m_ctrlCategoryRefresh);
    DDX_Control(pDX, IDC_FILTER_CATEGORY_FILTER,m_ctrlCategoryFilter);
	DDX_Control(pDX, IDC_FILTER_CATEGORY_SEARCH, m_ctrlCategorySearch);
    DDX_Text(pDX, IDC_EDIT_FILTER, m_FilterValue);
}


BEGIN_MESSAGE_MAP(CMediaLibraryCategoryCtrl, CDialogCtrlTreeBase)
    ON_WM_SIZE()

    ON_REGISTERED_MESSAGE(WM_DND_DRAGENTER, OnDragEnter)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGOVER, OnDragOver)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGLEAVE, OnDragLeave)
    ON_REGISTERED_MESSAGE(WM_DND_DROP, OnDrop)

    ON_MESSAGE(USER_MSG_GET_MEDIA_ROOT_CATEGORY, UpdateRootCategory)
    ON_MESSAGE(USER_MSG_GET_MEDIA_SUB_CATEGORY, UpdateSubCategory)

    ON_MESSAGE(USER_MSG_GET_MEDIA_SUB_CATEGORY_FILE, UpdateSubCategoryAndFiles)

    ON_NOTIFY(NM_CLICK, IDC_MEDIALIB_TREE, &CMediaLibraryCategoryCtrl::OnClickedMediaCategory)
    ON_NOTIFY(TVN_SELCHANGED, IDC_MEDIALIB_TREE, &CMediaLibraryCategoryCtrl::OnTvnSelchangedMedialibTree)
    ON_NOTIFY(TVN_ITEMEXPANDING, IDC_MEDIALIB_TREE, &CMediaLibraryCategoryCtrl::OnTvnItemexpandingMedialibTree)
    ON_NOTIFY(TVN_DELETEITEM, IDC_MEDIALIB_TREE, &CMediaLibraryCategoryCtrl::OnTvnDeleteitemMedialibTree)
    ON_NOTIFY(NM_DBLCLK, IDC_MEDIALIB_TREE, &CMediaLibraryCategoryCtrl::OnNMDblclkMedialibTree)
    ON_BN_CLICKED(IDC_MEDIALIB_CATEGORY_NEW, &CMediaLibraryCategoryCtrl::OnBnClickedMedialibCategoryNew)
    ON_BN_CLICKED(IDC_MEDIALIB_CATEGORY_DEL, &CMediaLibraryCategoryCtrl::OnBnClickedMedialibCategoryDel)
    ON_BN_CLICKED(IDC_MEDIALIB_CATEGORY_RENAME, &CMediaLibraryCategoryCtrl::OnBnClickedMedialibCategoryRename)
    ON_NOTIFY(TVN_ENDLABELEDIT, IDC_MEDIALIB_TREE, &CMediaLibraryCategoryCtrl::OnTvnEndlabeleditMedialibTree)
	ON_NOTIFY(TVN_BEGINLABELEDIT ,IDC_MEDIALIB_TREE, &CMediaLibraryCategoryCtrl::OnTvnBeginlabeleditMedialibTree)
    ON_NOTIFY(TVN_BEGINDRAG, IDC_MEDIALIB_TREE, &CMediaLibraryCategoryCtrl::OnTvnBegindragMedialibTree)
    ON_BN_CLICKED(IDC_FILTER_CATEGORY_REFRESH, &CMediaLibraryCategoryCtrl::OnClickedFilterCategoryRefresh)
    ON_BN_CLICKED(IDC_FILTER_CATEGORY_SEARCH, &CMediaLibraryCategoryCtrl::OnBnClickedFilterCategorySearch)
    ON_BN_CLICKED(IDC_FILTER_CATEGORY_FILTER, &CMediaLibraryCategoryCtrl::OnClickedFilterCategoryFilter)

	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CMediaLibraryCategoryCtrl)
    EASYSIZE(IDC_FILTER_TREE,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDC_EDIT_FILTER,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_FILTER_CATEGORY_FILTER,IDC_EDIT_FILTER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)

END_EASYSIZE_MAP


INT CMediaLibraryCategoryCtrl::GetSelectedCategory(ITEM_TYPE& ItemType)
{
    HTREEITEM hItem = g_CurItem;// m_ctrlCategoryTree.GetSelectedItem();
    if (hItem != NULL)
    {
        INT categoryID = m_ctrlCategoryTree.GetItemData(hItem);

        int nImage, nSelectedImage;
        if( m_ctrlCategoryTree.GetItemImage(hItem, nImage,nSelectedImage) )
        {
            if (CATEGORY_INDEX == nImage)
            {
                ItemType = ITEM_CATEGORY;
            }
            else if (FILE_INDEX == nImage || UNAPPROVED_FILE_INDEX == nImage)
            {
                ItemType = ITEM_FILE;
            }
        }

        return categoryID;
    }
    return -1;
}

INT CMediaLibraryCategoryCtrl::GetItemCategory(HTREEITEM hItem,ITEM_TYPE& ItemType)
{
	ItemType = (ITEM_TYPE)0;
    if (hItem != NULL)
    {
        INT categoryID = m_ctrlCategoryTree.GetItemData(hItem);

        int nImage, nSelectedImage;
        if( m_ctrlCategoryTree.GetItemImage(hItem, nImage,nSelectedImage) )
        {
            if (CATEGORY_INDEX == nImage)
            {
                ItemType = ITEM_CATEGORY;
            }
            else if (FILE_INDEX == nImage || UNAPPROVED_FILE_INDEX == nImage)
            {
                ItemType = ITEM_FILE;
            }
        }

        return categoryID;
    }
    return -1;
}

BOOL CMediaLibraryCategoryCtrl::SetMediaFilterID(int mediaFilterID)
{
    HTREEITEM hItem = m_ctrlCategoryTree.GetSelectedItem();
    if (hItem != NULL)
    {
        int nImage, nSelectedImage;
        if( m_ctrlCategoryTree.GetItemImage(hItem, nImage,nSelectedImage) )
        {
            if (CATEGORY_INDEX == nImage)
            {
                if (m_ctrlCategoryTree.SetItemData(hItem, mediaFilterID))
                {
                    return TRUE;
                }
            }
            else if (FILE_INDEX == nImage)
            {

            }
        }
    }
    return FALSE;
}

// CMediaLibraryCategoryCtrl message handlers
BOOL CMediaLibraryCategoryCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_ctrlCategoryNew.SetImage(IDB_NEW);
    m_ctrlCategoryDelete.SetImage(IDB_DELETE);
    m_ctrlCategoryRename.SetImage(IDB_RENAME);
    m_ctrlCategoryRefresh.SetImage(IDB_REFRESH);
    m_ctrlCategoryFilter.SetImage(IDB_FILTER);
	m_ctrlCategorySearch.SetImage(IDB_SEARCH);

    SetDefID(IDC_FILTER_CATEGORY_FILTER);

	m_MediaInfoCtrl.Create(AfxGetMainWnd());
	
	CMFCToolTipInfo  ttInfo;
	ttInfo.m_bDrawIcon = TRUE;
	ttInfo.m_bDrawDescription=TRUE;
	ttInfo.m_bVislManagerTheme = TRUE;
    ttInfo.m_nMaxDescrWidth = 1920;
	 
	m_MediaInfoCtrl.SetParams( &ttInfo );
    ((CControllerVisualManager*)(CMFCVisualManager::GetInstance()))->SetToolTipInfo(ttInfo);

	m_MediaInfoCtrl.AddTool( &m_ctrlCategoryTree,_T("Media Information"));

	
    if(m_bShowFiles)
    {
        RECT rect;
        GetClientRect(&rect);

        GetDlgItem(IDC_FILTER_CATEGORY_DEL)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FILTER_CATEGORY_NEW)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FILTER_CATEGORY_RENAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FILTER_CATEGORY_SEARCH)->ShowWindow(SW_HIDE);


        RECT OriginalRect;
        GetDlgItem(IDC_FILTER_CATEGORY_REFRESH)->GetWindowRect(&OriginalRect);
        ScreenToClient(&OriginalRect);

        OriginalRect.right = rect.left + (OriginalRect.right - OriginalRect.left);
        OriginalRect.left = rect.left;
        GetDlgItem(IDC_FILTER_CATEGORY_REFRESH)->MoveWindow(&OriginalRect);

        RECT OrigFilterRect;
        GetDlgItem(IDC_FILTER_CATEGORY_FILTER)->GetWindowRect(&OrigFilterRect);
        ScreenToClient(&OrigFilterRect);

        OrigFilterRect.right = rect.right;
        OrigFilterRect.left = rect.right - (OriginalRect.right - OriginalRect.left);
        GetDlgItem(IDC_FILTER_CATEGORY_FILTER)->MoveWindow(&OrigFilterRect);

        RECT OrigEditRect;
        GetDlgItem(IDC_EDIT_FILTER)->GetWindowRect(&OrigEditRect);
        ScreenToClient(&OrigEditRect);

        OrigEditRect.right = OrigFilterRect.left - 5;
        OrigEditRect.left = OriginalRect.right + 5;
        GetDlgItem(IDC_EDIT_FILTER)->MoveWindow(&OrigEditRect);
    }else
    {
        GetDlgItem(IDC_EDIT_FILTER)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FILTER_CATEGORY_FILTER)->ShowWindow(SW_HIDE);

        RECT rect;
        GetClientRect(&rect);

        RECT OrigNewRect;
        GetDlgItem(IDC_FILTER_CATEGORY_NEW)->GetWindowRect(&OrigNewRect);
        ScreenToClient(&OrigNewRect);

        int width =  (rect.right - rect.left) / 5;

        OrigNewRect.right = rect.left + width;
        OrigNewRect.left = rect.left;
        GetDlgItem(IDC_FILTER_CATEGORY_NEW)->MoveWindow(&OrigNewRect);

        RECT OrigDelRect;
        GetDlgItem(IDC_MEDIALIB_CATEGORY_DEL)->GetWindowRect(&OrigDelRect);
        ScreenToClient(&OrigDelRect);

        OrigDelRect.left = OrigNewRect.right + 5;
        OrigDelRect.right = OrigDelRect.left + width;
        GetDlgItem(IDC_MEDIALIB_CATEGORY_DEL)->MoveWindow(&OrigDelRect);

        RECT OrigRenameRect;
        GetDlgItem(IDC_MEDIALIB_CATEGORY_RENAME)->GetWindowRect(&OrigRenameRect);
        ScreenToClient(&OrigRenameRect);

        OrigRenameRect.left = OrigDelRect.right + 5;
        OrigRenameRect.right = OrigRenameRect.left + width;
        GetDlgItem(IDC_MEDIALIB_CATEGORY_RENAME)->MoveWindow(&OrigRenameRect);

        RECT OrigRefreshRect;
        GetDlgItem(IDC_FILTER_CATEGORY_REFRESH)->GetWindowRect(&OrigRefreshRect);
        ScreenToClient(&OrigRefreshRect);

        OrigRefreshRect.left = OrigRenameRect.right + 5;
        OrigRefreshRect.right = OrigRefreshRect.left + width;
        GetDlgItem(IDC_FILTER_CATEGORY_REFRESH)->MoveWindow(&OrigRefreshRect);

		RECT OrigSearchRect;
        GetDlgItem(IDC_FILTER_CATEGORY_SEARCH)->GetWindowRect(&OrigSearchRect);
        ScreenToClient(&OrigSearchRect);

        OrigSearchRect.left = OrigRefreshRect.right + 5;
        OrigSearchRect.right = OrigSearchRect.left + width;
        GetDlgItem(IDC_FILTER_CATEGORY_SEARCH)->MoveWindow(&OrigSearchRect);
    }


    INIT_EASYSIZE;


    m_ImageList.Create(16, 16, ILC_COLOR32|ILC_MASK, 1, 1);
    m_ImageList.SetImageCount(3);

    CBitmap bitmap;
    bitmap.LoadBitmap(IDB_CATEGORY_OPENED);
    m_ImageList.Replace(CATEGORY_INDEX, &bitmap, NULL);
    bitmap.DeleteObject();
    bitmap.LoadBitmap(IDB_ITEMSTATUS_APPROVED);
    m_ImageList.Replace(FILE_INDEX, &bitmap, NULL);
    bitmap.DeleteObject();
    bitmap.LoadBitmap(IDB_ITEMSTATUS_UNAPPROVED);
    m_ImageList.Replace(UNAPPROVED_FILE_INDEX, &bitmap, NULL);
    bitmap.DeleteObject();

    m_ImageList.SetOverlayImage(3, 1);

    m_ctrlCategoryTree.SetImageList(&m_ImageList,LVSIL_NORMAL);

    Translate(this);

    if (!CP(UserPrivilege_CreateMediaFilter))
    {
        GetDlgItem(IDC_FILTER_CATEGORY_NEW)->EnableWindow(FALSE);
    }
    if (!CP(UserPrivilege_DeleteMediaFilter))
    {
        GetDlgItem(IDC_FILTER_CATEGORY_DEL)->EnableWindow(FALSE);
    }
    if (!CP(UserPrivilege_EditMediaFilter))
    {
        GetDlgItem(IDC_FILTER_CATEGORY_RENAME)->EnableWindow(FALSE);
    }


    InitRootCategory();

    // register drop target
    if ( !m_DropTarget.Register( this ) )
        return FALSE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CMediaLibraryCategoryCtrl::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    UPDATE_EASYSIZE;


    // TODO: Add your message handler code here
}



void CMediaLibraryCategoryCtrl::OnCancel()
{

}


void CMediaLibraryCategoryCtrl::OnOK()
{
}

BOOL CMediaLibraryCategoryCtrl::InitRootCategory()
{
    HRESULT hr = S_OK;
    MediaFilter root;
    DWORD MessageID = USER_MSG_GET_MEDIA_ROOT_CATEGORY;

    if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,
        (WPARAM)this->GetSafeHwnd(), NULL))//post thread msg
    {
        TRACE1("post message failed,errno:%d\n",::GetLastError());
    }
    GetDlgItem(IDC_FILTER_CATEGORY_DEL)->EnableWindow(FALSE);
    GetDlgItem(IDC_FILTER_CATEGORY_RENAME)->EnableWindow(FALSE);

    return TRUE;
}


LRESULT CMediaLibraryCategoryCtrl::UpdateRootCategory (WPARAM wp, LPARAM lp)
{
    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf< MediaFilter > MessageBuf;
    if (!WorkThreadDataManager< MediaFilter >
        ::GetDataBuf(nMessageBufID, MessageBuf))
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    m_MediaMap.clear();
	//added by RaymondLiu 
	m_MediaCategoryMap.clear();

	m_ExpandedCategoryList.clear();
    MediaFilter Category = *MessageBuf.Get();
    m_RootCategoryID = Category.GetMediaFilterID();

    m_ctrlCategoryTree.DeleteAllItems();

    HTREEITEM itemRoot = m_ctrlCategoryTree.InsertItem(Translate(_T("Media Root Category")), CATEGORY_INDEX, CATEGORY_INDEX);
    m_ctrlCategoryTree.SetItemData(itemRoot, Category.GetMediaFilterID());

    if (m_bShowFiles && (Category.GetHaveChildren() || Category.GetHaveChildrenFilter()))
    {
        m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, itemRoot);
    }
    else if (!m_bShowFiles && Category.GetHaveChildrenFilter())
    {
        m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, itemRoot);
    }

    m_ctrlCategoryTree.Expand(itemRoot, TVE_EXPAND);

    return 0;
}

void CMediaLibraryCategoryCtrl::OnTvnSelchangedMedialibTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    g_CurItem   =   pNMTreeView->itemNew.hItem;
    INT CategoryID = m_ctrlCategoryTree.GetItemData(g_CurItem);

    if (CP(UserPrivilege_DeleteMediaFilter))
    {
        GetDlgItem(IDC_FILTER_CATEGORY_DEL)->EnableWindow(TRUE);
    }
    if (CP(UserPrivilege_EditMediaFilter))
    {
        GetDlgItem(IDC_FILTER_CATEGORY_RENAME)->EnableWindow(TRUE);
    }
	
    if (CP(UserPrivilege_ListMediaFilter))
    {
        GetParent()->SendMessage(MEDIA_LIST_CATEGORY, CategoryID, 0L);
		CWnd * p = AfxGetMainWnd();
		if (p != NULL)
		{
			CWnd *pWnd = SearchWndObj(p,RUNTIME_CLASS(CMediaLibraryListTab));
			if (pWnd != NULL)
			{
				CMediaLibraryListTab* pTab = (CMediaLibraryListTab*)pWnd;
				if (pTab != NULL)
				{
					pTab->SetActiveTab(CATEGORY_LIST_TAB4);
					SMediaFilter filter;
					filter = pTab->m_mediaGrid.GetFilter();
					filter.setCategoryId(CategoryID);
					pTab->m_mediaGrid.SetFilter(filter);		
					pTab->m_mediaGrid.RefreshMediaGrid();
				}	
			}	
		}
    }
	

    // root category should not be deleted or renamed
    if (CategoryID == 0xFFFFFFFF)
    {
        GetDlgItem(IDC_FILTER_CATEGORY_DEL)->EnableWindow(FALSE);
        GetDlgItem(IDC_FILTER_CATEGORY_RENAME)->EnableWindow(FALSE);
    }
//    ((CMediaLibraryPage*)(GetParent()->GetParent()))->m_wndMediaLibraryList.SendMessage(MEDIA_LIST_CATEGORY, CategoryID, (LPARAM)&m_ctrlCategoryTree);
	
}


void CMediaLibraryCategoryCtrl::OnTvnItemexpandingMedialibTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

	UpdateData(TRUE);

    HTREEITEM   ExpandingItem   =   pNMTreeView-> itemNew.hItem;

    // collapse
    if (pNMTreeView->action == TVE_EXPAND)
    {
        // Retrieves the images associated with the given item.
        int nImage, nSelectedImage;
        if( m_ctrlCategoryTree.GetItemImage(ExpandingItem, nImage,nSelectedImage) )
        {
            if (CATEGORY_INDEX == nImage)
            {
                INT CategoryID = m_ctrlCategoryTree.GetItemData(ExpandingItem);

				//added by RaymondLiu
				m_ExpandedCategoryList.push_back(CategoryID);

                DWORD MessageID = USER_MSG_GET_MEDIA_SUB_CATEGORY;
                if (m_bShowFiles)
                {
                    MessageID = USER_MSG_GET_MEDIA_SUB_CATEGORY_FILE;
                }

                if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,
                    (WPARAM)this->GetSafeHwnd(), LPARAM(CategoryID)))//post thread msg
                {
                    TRACE1("post message failed,errno:%d\n",::GetLastError());
                }
            }
        }

        return;
    }
	else
	{
		int nImage, nSelectedImage;
		if( m_ctrlCategoryTree.GetItemImage(ExpandingItem, nImage,nSelectedImage) )
		{
			if (CATEGORY_INDEX == nImage)
			{
				INT CategoryID = m_ctrlCategoryTree.GetItemData(ExpandingItem);

				//added by RaymondLiu
				std::vector<INT>::iterator iter = find(m_ExpandedCategoryList.begin(), m_ExpandedCategoryList.end(), CategoryID);
				if (iter != m_ExpandedCategoryList.end())
				{
					m_ExpandedCategoryList.erase(iter);
				}
				
			}
		}

		//return;
		
	}
}

LRESULT CMediaLibraryCategoryCtrl::UpdateSubCategory (WPARAM wp, LPARAM lp)
{
    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf< std::pair< INT, std::vector<MediaFilter> > > MessageBuf;
    if (!WorkThreadDataManager< std::pair< INT, std::vector<MediaFilter> > >
        ::GetDataBuf(nMessageBufID, MessageBuf))
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////

    HTREEITEM Item = FindCategoryItem(m_ctrlCategoryTree.GetRootItem(), MessageBuf.Get()->first);
	HTREEITEM lastItem = NULL;
    if (Item)
    {
        DeleteChildrens(Item);

        vector<MediaFilter> SubCategoryList = MessageBuf.Get()->second;

        for (auto it = SubCategoryList.begin(); it != SubCategoryList.end(); it++)
        {
            HTREEITEM item = m_ctrlCategoryTree.InsertItem(it->GetMediaFilterName().c_str(), CATEGORY_INDEX, CATEGORY_INDEX, Item);
            m_ctrlCategoryTree.SetItemData(item, it->GetMediaFilterID());

            if (it->GetHaveChildrenFilter())
            {
               m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);
            }

            m_MediaCategoryMap[it->GetMediaFilterID()] = *it;
			lastItem = item;
        }
    }
	//
	//if add new item,and parent item is expanding..... zxy
	//
	if(this->m_newAddingItem && lastItem)
	{
	this->m_ctrlCategoryTree.EditLabel(lastItem);
	m_newAddingItem = NULL;
	}
    return 0;
}


LRESULT CMediaLibraryCategoryCtrl::UpdateSubCategoryAndFiles (WPARAM wp, LPARAM lp)
{
    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf< pair< INT, pair< vector<MediaFilter>, vector<Media> > > > MessageBuf;
    if (!WorkThreadDataManager< pair< INT, pair< vector<MediaFilter>, vector<Media> > > >
        ::GetDataBuf(nMessageBufID, MessageBuf))
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////

    HTREEITEM Item = FindCategoryItem(m_ctrlCategoryTree.GetRootItem(), MessageBuf.Get()->first);
    if (Item)
    {
        DeleteChildrens(Item);

        const vector<MediaFilter>& SubCategoryList = MessageBuf.Get()->second.first;

        for (auto it = SubCategoryList.begin(); it != SubCategoryList.end(); it++)
        {
            HTREEITEM item = m_ctrlCategoryTree.InsertItem(it->GetMediaFilterName().c_str(), CATEGORY_INDEX, CATEGORY_INDEX, Item);
            m_ctrlCategoryTree.SetItemData(item, it->GetMediaFilterID());

            if (it->GetHaveChildrenFilter() || it->GetHaveChildren())
            {
                m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);
            }

            m_MediaCategoryMap[it->GetMediaFilterID()] = *it;
        }

        const vector<Media>& SubMediaList = MessageBuf.Get()->second.second;

        for (auto it = SubMediaList.begin(); it != SubMediaList.end(); it++)
        {
            INT nImage = FILE_INDEX;
#ifndef PROFESSIONAL_EDITION
            if (it->GetAudit().GetStatus() != Audit::APPROVED)
            {
                nImage = UNAPPROVED_FILE_INDEX;
            }
#endif

            if(m_FilterValue != _T(""))
            {
                CString TimetableName = it->GetName().c_str();
                if(TimetableName.MakeLower().Find(m_FilterValue.MakeLower()) <0)
                {
                    continue;
                }
            }

            HTREEITEM item = m_ctrlCategoryTree.InsertItem(it->GetName().c_str(), nImage, nImage, Item);
            m_ctrlCategoryTree.SetItemData(item, it->GetMeidaID());

            m_MediaMap[it->GetMeidaID()] = *it;
        }
    }

    return 0;
}

void CMediaLibraryCategoryCtrl::OnTvnDeleteitemMedialibTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    HTREEITEM   Item   =   pNMTreeView->itemOld.hItem;

}


void CMediaLibraryCategoryCtrl::OnNMDblclkMedialibTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    *pResult = 0;
}


void CMediaLibraryCategoryCtrl::OnBnClickedMedialibCategoryNew()
{
    // TODO: Add your control notification handler code here

    HTREEITEM SeletectItem = m_ctrlCategoryTree.GetSelectedItem();

  //  HTREEITEM NewItem;
    INT ParentID;
    if (!SeletectItem)
    {
        SeletectItem = m_ctrlCategoryTree.GetRootItem();
    }

    HTREEITEM ParentItem = m_ctrlCategoryTree.GetParentItem(SeletectItem);

    if (SeletectItem)
    {
        ParentID = m_ctrlCategoryTree.GetItemData(SeletectItem);
    }
    else
    {
        ParentID = -1;
    }

    MediaFilter Category;
    INT CategoryID = -1;
    Category.ChangeParentFilterID(ParentID);

    CString CategoryName = Translate(_T("New Category"));
    int TryCount = 1;
    HRESULT hr = S_OK;
    while(TryCount < 100)
    {
        if(TryCount > 1)
        {
            CategoryName.Format(Translate(_T("New Category(%d)")), TryCount);
        }

        Category.Rename(CategoryName.GetString());

        hr = GetControllerInterface->AddMediaFilter(Category, CategoryID);
        if (!SUCCEEDED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                TRACE0("Failed to add media filter\n");
                GetControllerApp->OnControllerServerOffline();
                return;
            }
            TryCount ++;
        }else
        {
            break;
        }

    }

    hr = GetControllerInterface->GetMediaFilter(CategoryID, Category);
    if (!SUCCEEDED(hr))
    {
        TRACE0("Failed to get media filter\n");
        return;
    }

    m_MediaCategoryMap[CategoryID] = Category;

    m_newAddingItem = m_ctrlCategoryTree.InsertItem(CategoryName, CATEGORY_INDEX, CATEGORY_INDEX,
        SeletectItem);

  	m_ctrlCategoryTree.SetItemData(m_newAddingItem, CategoryID);
	if(m_ctrlCategoryTree.GetItemState(SeletectItem,TVIS_EXPANDED) & TVIS_EXPANDED )
	{
		m_ctrlCategoryTree.EditLabel(m_newAddingItem);
		return;
	}
	 m_ctrlCategoryTree.Expand(SeletectItem,  TVE_EXPAND);
	//m_ctrlCategoryTree.EditLabel(m_newAddingItem);

}


void CMediaLibraryCategoryCtrl::OnBnClickedMedialibCategoryDel()
{

    HTREEITEM SeletectItem = m_ctrlCategoryTree.GetSelectedItem();

    int nImage, nSelectedImage;
    int i = 0;
    if( m_ctrlCategoryTree.GetItemImage(SeletectItem, nImage,nSelectedImage) )
    {
        if (CATEGORY_INDEX == nImage)
        {

        }
        else if (FILE_INDEX == nImage)
        {
            return;
        }
    }

	// TODO: Add your control notification handler code here
    if (MessageBox(Translate(_T("Are you sure to delete the selected Category?")), Translate(_T("Tips:Delete category")),MB_YESNO|MB_ICONQUESTION) != IDYES)
    {
        return;
    }

    if (SeletectItem)
    {
        INT CategoryID = m_ctrlCategoryTree.GetItemData(SeletectItem);


        if (CategoryID == 0xffffffff)
        {
            MessageBox(Translate(_T("the root category should not been deleted!")), Translate(_T("Error:Delete category")), MB_ICONERROR);
            return;
        }

		CMediaLibraryCategoryPane *pPane = (CMediaLibraryCategoryPane *)GetParent();
		CMediaLibraryPage *pPage = (CMediaLibraryPage *)pPane->GetParent();

		if ( pPage->IsCategoryCanBeDel(CategoryID) <= 0)
		{
			MessageBox(Translate(_T("the category should not been deleted, there are uploading tasks!")), Translate(_T("Error:Delete category")), MB_ICONERROR);
			return;
		}

        MediaFilter Category;
        HRESULT hr = GetControllerInterface->GetMediaFilter(CategoryID, Category);
        if (!SUCCEEDED(hr))
        {
            TRACE1("Failed to get Media filter %d\n", CategoryID);
            MessageBox(Translate(_T("Failed to delete the selected Category")), Translate(_T("Error:Delete category")), MB_ICONERROR);
            return;
        }

        vector<MediaFilter> FailedSubCategorys;
        vector<Media> FailedSubItems;
        if (!DeleteCategory(Category, FailedSubCategorys, FailedSubItems))
        {
            TRACE1("Failed to delete Media filter %d\n", CategoryID);

            CString szMedias;
            for (auto it = FailedSubItems.begin(); it != FailedSubItems.end() && i < 5; it++, i++)
            {
                szMedias += (it->GetName() + _T("\n")).c_str();
            }
            CString ErrorInfo;
            ErrorInfo.Format(Translate(_T("Failed to delete the selected Category, there are some medias used in exist layouts! \n%sEtc.")), szMedias);
            MessageBox(ErrorInfo, Translate(_T("Error:Delete category")), MB_ICONERROR);
            return;
        }

        m_ctrlCategoryTree.DeleteItem(SeletectItem);
    }

}


void CMediaLibraryCategoryCtrl::OnBnClickedMedialibCategoryRename()
{
    // TODO: Add your control notification handler code here

    HTREEITEM SeletectItem = m_ctrlCategoryTree.GetSelectedItem();

    if (SeletectItem && m_ctrlCategoryTree.GetItemData(SeletectItem) != 0xFFFFFFFF)
    {
        m_ctrlCategoryTree.EditLabel(SeletectItem);
    }
}


void CMediaLibraryCategoryCtrl::OnTvnBeginlabeleditMedialibTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	__super::OnTvnBeginlabelEdit(pNMHDR,pResult);

    ASSERT(pNMHDR != NULL);
    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    if(NULL == pTVDispInfo)
        return;
    if (m_ctrlCategoryTree.GetItemData(pTVDispInfo->item.hItem) == 0xffffffff)
    {
        *pResult = TRUE;
    }
    else
    {
        *pResult = FALSE;
    }
}

void CMediaLibraryCategoryCtrl::OnTvnEndlabeleditMedialibTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;


    HTREEITEM ChangedItem = pTVDispInfo->item.hItem;

    //CString ChangedLabel = pTVDispInfo->item.pszText;
    CString ChangedLabel = pTVDispInfo->item.pszText ? pTVDispInfo->item.pszText : m_ctrlCategoryTree.GetItemText(ChangedItem);
    INT     CategoryID   = m_ctrlCategoryTree.GetItemData(ChangedItem);

    // Retrieves the images associated with the given item.
    int nImage, nSelectedImage;
    if( m_ctrlCategoryTree.GetItemImage(ChangedItem, nImage,nSelectedImage) )
    {
        if (CATEGORY_INDEX == nImage)
        {
            MediaFilter Category;
            HRESULT hr = GetControllerInterface->GetMediaFilter(CategoryID, Category);
            if (!SUCCEEDED(hr))
            {
                TRACE1("Failed to get media filter %d\n", CategoryID);
                return;
            }
            Category.Rename(ChangedLabel.GetString());

            hr = GetControllerInterface->EditMediaFilter(Category);
            if (!SUCCEEDED(hr))
            {
                TRACE2("Failed to edit media filter %d, %s\n", CategoryID, Category.GetMediaFilterName().c_str());
                //MessageBox(Translate(_T("Failed to rename the selected Category.")), Translate(_T("Error")), MB_ICONERROR);
                std::tstring desc;
                GetControllerInterface->GetLastError(desc);
                MessageBox(Translate(desc.c_str()), Translate(_T("Error:Media library category")), MB_ICONERROR);
                return;
            }

            m_MediaCategoryMap[Category.GetMediaFilterID()] = Category;

            m_ctrlCategoryTree.SetItemText(ChangedItem, ChangedLabel);

        }
        else if (FILE_INDEX == nImage || UNAPPROVED_FILE_INDEX == nImage)
        {
            vector<Media> myMedia;
            vector<INT> MediaIDs;
            MediaIDs.push_back(CategoryID);
            HRESULT hr = GetControllerInterface->GetMedias(MediaIDs, myMedia);
            if (!SUCCEEDED(hr) || myMedia.size() < 1)
            {
                TRACE1("Failed to get media %d\n", CategoryID);
                return;
            }

            myMedia[0].SetName(ChangedLabel.GetString());

            hr = GetControllerInterface->EditMedia(myMedia[0]);
            if (!SUCCEEDED(hr))
            {
                TRACE2("Failed to edit media %d, %s\n", CategoryID, myMedia[0].GetName().c_str());
                return;
            }

            m_MediaMap[myMedia[0].GetMeidaID()] = myMedia[0];

            m_ctrlCategoryTree.SetItemText(ChangedItem, ChangedLabel);
        }
    }
}


HTREEITEM CMediaLibraryCategoryCtrl::FindCategoryItem(HTREEITEM item, INT ID)
{
    HTREEITEM hFind;
    if(item == NULL)
    {
        return NULL;
    }

    while(item!=NULL)
    {
        // Retrieves the images associated with the given item.
        int nImage, nSelectedImage;
        if( m_ctrlCategoryTree.GetItemImage(item, nImage,nSelectedImage) )
        {
            if(nImage == CATEGORY_INDEX &&
                m_ctrlCategoryTree.GetItemData(item) == ID)
            {
                return item;
            }
        }

        if(m_ctrlCategoryTree.ItemHasChildren(item))
        {
            item = m_ctrlCategoryTree.GetChildItem(item);
            hFind = FindCategoryItem(item,ID);
            if(hFind)
            {
                return hFind;
            }
            else
            {
                item = m_ctrlCategoryTree.GetNextSiblingItem(m_ctrlCategoryTree.GetParentItem(item));
            }
        }
        else
        {
            item = m_ctrlCategoryTree.GetNextSiblingItem(item);
            if(item == NULL)
            {
                return NULL;
            }
        }
    }

    return item;
}

void CMediaLibraryCategoryCtrl::DeleteChildrens(HTREEITEM item)
{
    if(item == NULL)
    {
        return;
    }

    HTREEITEM itemChild = m_ctrlCategoryTree.GetChildItem(item);

    while(itemChild!=NULL)
    {
        if(m_ctrlCategoryTree.ItemHasChildren(itemChild))
        {
            DeleteChildrens(itemChild);
        }
        else
        {
            HTREEITEM itemOld = itemChild;
            itemChild = m_ctrlCategoryTree.GetNextSiblingItem(itemChild);
            m_ctrlCategoryTree.DeleteItem(itemOld);
        }
    }

    return;
}

BOOL CMediaLibraryCategoryCtrl::SerializeMediaTo(CFile& file, INT MediaID)
{
    auto it = m_MediaMap.find(MediaID);
    if (it == m_MediaMap.end())
    {
        return FALSE;
    }

    CString XMLString = it->second.ToXML().c_str();

    CArchive Archive (&file, CArchive::store);

    Archive << XMLString;

    Archive.Close();

    return TRUE;
}

LRESULT CMediaLibraryCategoryCtrl::OnDrop(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1L;

    HGLOBAL hGlobal = NULL;
    HDROP hDrop = NULL;

    if (m_bShowFiles) return 0;

    CS3OleDropInfo* pInfo = (CS3OleDropInfo*)wParam;

    CPoint point;

    point = pInfo->m_point;
    ClientToScreen(&point);
    m_ctrlCategoryTree.ScreenToClient(&point);

    UINT uFlags;
    HTREEITEM hItem = m_ctrlCategoryTree.HitTest(point, &uFlags);

    int nImage, nSelectedImage;
    if (m_ctrlCategoryTree.GetItemImage(hItem, nImage, nSelectedImage))
    {
        if (nImage != CATEGORY_INDEX)
        {
            return 0;
        }

        m_ctrlCategoryTree.SelectDropTarget(NULL);

        if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
        {
            MediaFilter NewMediaFilter;

            DWORD MediaCategoryID = m_ctrlCategoryTree.GetItemData(hItem);

            HRESULT hr = GetControllerInterface->GetMediaFilter(MediaCategoryID, NewMediaFilter);
            if (!SUCCEEDED(hr))
            {
                TRACE1("Failed to Get Media Filter %d!\n", MediaCategoryID);
            }

            if (pInfo->m_pDataObject->IsDataAvailable(CMediaLibraryCategoryCtrl::s_MediaFormat))
            {
                CFile* pFile;
                pFile = pInfo->m_pDataObject->GetFileData(CMediaLibraryCategoryCtrl::s_MediaFormat);

                std::vector<Media> mediaListTemp;

                if (pFile == NULL)
                {
                    return FALSE;
                }
                SerializeMediaFrom(*pFile, mediaListTemp);

                delete pFile;

                if (MessageBox(Translate(_T("Are you sure to move the selected media?")),Translate(_T("Tips:Drop media")), MB_YESNO|MB_ICONQUESTION) != IDYES)
                {
                    return 0 ;
                }

				// NOTE: do we need to re-get media?
                std::vector<Media> mediaList;
                std::vector<int> mediaIDList;
				for(auto it = mediaListTemp.begin() ; it != mediaListTemp.end(); ++it)
					mediaIDList.push_back(it->GetMeidaID());

				if(mediaIDList.size())
				{
					hr = GetControllerInterface->GetMedias(mediaIDList, mediaList);
					if (!SUCCEEDED(hr) || mediaList.size() < 1)
					{
						MessageBox(Translate(_T("Failed to get the Media!")), Translate(_T("Error:Media library category")), MB_ICONERROR);
						return 0;
					}
				}

				for(auto iter = mediaList.begin() ; iter != mediaList.end(); ++iter)
				{
					// change media filter
					//mediaList[0].ChangeMediaFilter(NewMediaFilter);
					iter->ChangeMediaFilter(NewMediaFilter);

					//hr = GetControllerInterface->EditMedia(mediaList[0]);
					hr = GetControllerInterface->EditMedia(*iter);

					if (!SUCCEEDED(hr))
					{
						if (!SUCCEEDED(hr))
						{
							if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
							{
								GetControllerApp->OnControllerServerOffline();
							}
							else
							{
                                std::tstring desc;
                                unsigned int err = GetControllerInterface->GetLastError(desc);
								MessageBox(Translate(_T("Failed to move the selected media.")) + Translate(desc.c_str()), Translate(_T("Error:Media library category")), MB_ICONERROR);
							}
							return 0 ;
						}
					}

					//m_MediaMap[mediaList[0].GetMeidaID()] = mediaList[0];
					m_MediaMap[iter->GetMeidaID()] = *iter;
				}
         //       GetParent()->PostMessage(MEDIA_CATEGORY_REFRESH, NULL, NULL); 
				//refresh category ????
				CWnd *p = AfxGetMainWnd();
				if (p != NULL)
				{
					CWnd *pWnd = SearchWndObj(p,RUNTIME_CLASS(CMediaLibraryListTab));
					if (pWnd != NULL)
					{
						CMediaLibraryListTab* pTab = (CMediaLibraryListTab*)pWnd;
						if (pTab != NULL)
						{
							pTab->SetActiveTab(CATEGORY_LIST_TAB4);
							ITEM_TYPE ItemType;
							pTab->m_mediaGrid.GetFilter().setCategoryId(GetSelectedCategory(ItemType));
							pTab->m_mediaGrid.RefreshMediaGrid();
						}	
					}		
				}		
                return 1;
            }

            if (pInfo->m_pDataObject->IsDataAvailable(CMediaLibraryCategoryCtrl::s_MediaCategoryFormat))
            {
                CFile* pFile;
                pFile = pInfo->m_pDataObject->GetFileData (CMediaLibraryCategoryCtrl::s_MediaCategoryFormat);

                MediaFilter mediaCategory;

                if (pFile == NULL)
                {
                    return FALSE;
                }
                SerializeMediaCategoryFrom(*pFile, mediaCategory);
                delete pFile;

                if (MessageBox(Translate(_T("Are you sure to move the selected Category?")),Translate(_T("Tips:Drop media")) ,MB_YESNO|MB_ICONQUESTION) != IDYES)
                {
                    return 0 ;
                }

                MediaFilter Category;
                HRESULT hr = GetControllerInterface->GetMediaFilter(mediaCategory.GetMediaFilterID(), Category);
                if (!SUCCEEDED(hr))
                {
                    TRACE1("Failed to get media filter %d\n", mediaCategory.GetMediaFilterID());
                    return 0;
                }

                Category.ChangeParentFilterID(MediaCategoryID);

                hr = GetControllerInterface->EditMediaFilter(Category);
                if (!SUCCEEDED(hr))
                {
                    if (!SUCCEEDED(hr))
                    {
                        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                        {
                            GetControllerApp->OnControllerServerOffline();
                        }
                        else
                        {
                            MessageBox(Translate(_T("Failed to move the selected Category.")), Translate(_T("Error:Media library category")), MB_ICONERROR);
                        }
                        return 0 ;
                    }
                }

                m_MediaCategoryMap[Category.GetMediaFilterID()] = Category;

                InitRootCategory();
                return 1;
            }

        }
    }

    return 0;
}

LRESULT CMediaLibraryCategoryCtrl::OnDragEnter(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1L;

    if (m_bShowFiles) return DROPEFFECT_NONE;

    CS3OleDropInfo* pInfo = (CS3OleDropInfo*)wParam;

    CPoint point;

    point = pInfo->m_point;
    ClientToScreen(&point);
    m_ctrlCategoryTree.ScreenToClient(&point);

    UINT uFlags;
    HTREEITEM hItem = m_ctrlCategoryTree.HitTest(point, &uFlags);

    int nImage, nSelectedImage;
    if (m_ctrlCategoryTree.GetItemImage(hItem, nImage, nSelectedImage))
    {
        if (nImage != CATEGORY_INDEX)
        {
            m_ctrlCategoryTree.SelectDropTarget(NULL);
            return DROPEFFECT_NONE;
        }

        if ((hItem != NULL) && (TVHT_ONITEM) & uFlags)
        {
            m_ctrlCategoryTree.SelectDropTarget(hItem);
            if (pInfo->m_pDataObject->IsDataAvailable(CMediaLibraryCategoryCtrl::s_MediaFormat))
            {
                return DROPEFFECT_MOVE;
            }

            if (pInfo->m_pDataObject->IsDataAvailable(CMediaLibraryCategoryCtrl::s_MediaCategoryFormat))
            {
                return DROPEFFECT_MOVE;
            }
        }
    }

    return DROPEFFECT_NONE;
}

LRESULT CMediaLibraryCategoryCtrl::OnDragOver(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1L;

    if (m_bShowFiles)   return DROPEFFECT_NONE;

    CS3OleDropInfo* pInfo = (CS3OleDropInfo*)wParam;

    CPoint point;

    point = pInfo->m_point;
    ClientToScreen(&point);
    m_ctrlCategoryTree.ScreenToClient(&point);

    UINT uFlags;
    HTREEITEM hItem = m_ctrlCategoryTree.HitTest(point, &uFlags);

    int nImage, nSelectedImage;
    if (m_ctrlCategoryTree.GetItemImage(hItem, nImage,nSelectedImage))
    {
        if (nImage != CATEGORY_INDEX)
        {
            m_ctrlCategoryTree.SelectDropTarget(NULL);
            return DROPEFFECT_NONE;
        }

        if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
        {
            m_ctrlCategoryTree.SelectDropTarget(hItem);

            if (pInfo->m_pDataObject->IsDataAvailable(CMediaLibraryCategoryCtrl::s_MediaFormat))
            {
                return DROPEFFECT_MOVE;
            }

            if (pInfo->m_pDataObject->IsDataAvailable(CMediaLibraryCategoryCtrl::s_MediaCategoryFormat))
            {
                return DROPEFFECT_MOVE;
            }
        }
    }

    return DROPEFFECT_NONE;
}

LRESULT CMediaLibraryCategoryCtrl::OnDragLeave(WPARAM wParam, LPARAM lParam)
{
   m_ctrlCategoryTree.SelectDropTarget(NULL);
   return 0;
}

void CMediaLibraryCategoryCtrl::OnTvnBegindragMedialibTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;


    HTREEITEM DragItem = pNMTreeView->itemNew.hItem;

    if (DragItem)
    {
        // Retrieves the images associated with the given item.
        int nImage, nSelectedImage;
        if( m_ctrlCategoryTree.GetItemImage(DragItem, nImage,nSelectedImage) )
        {
            if (FILE_INDEX == nImage)
            {
                DWORD MediaID = m_ctrlCategoryTree.GetItemData(DragItem);

                auto it = m_MediaMap.find(MediaID);
                if (it == m_MediaMap.end() || it->second.GetAudit().GetStatus() != Audit::APPROVED)
                {
                    MessageBox(Translate(_T("Only approved item could be draged to use!")),Translate(_T("Warning:Drop media")) ,MB_OK|MB_ICONEXCLAMATION);
                    return;
                }

                CString szMediaID = StringUtility::itos(MediaID).c_str();
                DWORD nLength = (szMediaID.GetLength() + 1) * sizeof(TCHAR);

                HGLOBAL hgData = GlobalAlloc(GHND|GMEM_SHARE, nLength);
                ASSERT(hgData!=NULL);

                TCHAR* lpData = (TCHAR*)::GlobalLock(hgData);
                ASSERT(lpData!=NULL);

                _tcscpy_s(lpData, szMediaID.GetLength() + 1, szMediaID);

                ::GlobalUnlock(hgData);

                // Cache the data, and initiate DragDrop
                m_OleDataSource.CacheGlobalData(CF_UNICODETEXT, hgData);

                DROPEFFECT dropEffect
                    = m_OleDataSource.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE);

                m_OleDataSource.Empty();

            }
            else if (CATEGORY_INDEX == nImage)
            {
                DWORD CategoryID = m_ctrlCategoryTree.GetItemData(DragItem);

                if(CategoryID == 0xffffffff) return;

                if (!GetControllerInterface->CheckPrivilege(UserPrivilege_EditMediaFilter))
                {
                    MessageBox(Translate(_T("You don't have privilege to rename/move media category")),Translate(_T("Warning:Drop media")) ,MB_OK|MB_ICONEXCLAMATION);
                    return;
                }


                BOOL bDrag = FALSE;
                try
                {
                    CSharedFile globFile;

                    bDrag =  SerializeMediaCategoryTo(globFile, CategoryID);

                    if (bDrag)
                    {
                        m_OleDataSource.CacheGlobalData (s_MediaCategoryFormat,
                            globFile.Detach());
                    }

                }
                catch (COleException* pEx)
                {
                    TRACE(_T(" DragDrop. OLE exception: %x\r\n"),
                        pEx->m_sc);
                    pEx->Delete ();

                    bDrag = FALSE;
                }
                catch (CNotSupportedException *pEx)
                {
                    TRACE(_T(" DragDrop. \"Not Supported\" exception\r\n"));
                    pEx->Delete ();

                    bDrag = FALSE;
                }

                if (!bDrag)
                {
                    return;
                }

                DROPEFFECT dropEffect
                    = m_OleDataSource.DoDragDrop(DROPEFFECT_MOVE);

                m_OleDataSource.Empty();

            }
            else
            {
                MessageBox(Translate(_T("Only approved item could be draged to use!")),Translate(_T("Warning:Drop media")) ,MB_OK|MB_ICONEXCLAMATION);
            }
        }
    }

}

int CMediaLibraryCategoryCtrl::SearchSubCategory(MediaFilter & Category, HTREEITEM FatherItem, CString str )
{
	HRESULT hr;
	bool isFound = false;

#if 1 //there is a problem ,sub category can not expand
	if(m_bShowFiles)
	{
		if (FatherItem)
		{
			//DeleteChildrens(Item);

			std::vector<MediaFilter> SubCategoryList;
			hr = GetControllerInterface->GetSubMediaFilters(Category, SubCategoryList);
			if (!SUCCEEDED(hr))
			{
				//TRACE0("Failed to get sub media filters\n");
				return false;
			}

			for (auto it = SubCategoryList.begin(); it != SubCategoryList.end(); it++)
			{
				HTREEITEM item = m_ctrlCategoryTree.InsertItem(it->GetMediaFilterName().c_str(), CATEGORY_INDEX, CATEGORY_INDEX, FatherItem);
				m_ctrlCategoryTree.SetItemData(item, it->GetMediaFilterID());

				CString strName = it->GetMediaFilterName().c_str();
				if(strName.MakeLower().Find(str.MakeLower()) >= 0)
				//if(memicmp(it->GetMediaFilterName().c_str(),str,str.GetLength()) == 0)
				{
					isFound = true;
					m_ctrlCategoryTree.SetItemState(item,TVIS_SELECTED,TVIS_SELECTED);
				}

#if 0
				if (it->GetHaveChildrenFilter() || it->GetHaveChildren())
				{
					//m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);

					m_ctrlCategoryTree.SetItemState(item,TVIS_EXPANDED,TVIS_EXPANDED);
					if(SearchSubCategory(*it,item,str) == true)
					{	
						//m_MediaCategoryMap[it->GetMediaFilterID()] = *it;
						//m_ctrlCategoryTree.SetItemState(item,TVIS_SELECTED,TVIS_SELECTED);
						//return true;
						isFound = true;
					}
					else
					{
						std::vector<INT>::iterator iter = find(m_ExpandedCategoryList.begin(), m_ExpandedCategoryList.end(), it->GetMediaFilterID());
						if (iter == m_ExpandedCategoryList.end())
						{
							//m_ctrlCategoryTree.SetItemState(item,0,TVIS_EXPANDED);
							m_ctrlCategoryTree.Expand(item,TVE_COLLAPSE);
						}
					}
					//m_ctrlCategoryTree.Expand(item, TVE_EXPAND);
				}
#else
				if (it->GetHaveChildrenFilter() || it->GetHaveChildren())
				{
					std::vector<INT>::iterator iter = find(m_ExpandedCategoryList.begin(), m_ExpandedCategoryList.end(), it->GetMediaFilterID());
					if (iter == m_ExpandedCategoryList.end())
					{
						m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);
					}
					else
					{
						m_ctrlCategoryTree.SetItemState(item,TVIS_EXPANDED,TVIS_EXPANDED);
						if(SearchSubCategory(*it,item,str) == true)
						{	
							//m_MediaCategoryMap[it->GetMediaFilterID()] = *it;
							//m_ctrlCategoryTree.SetItemState(item,TVIS_SELECTED,TVIS_SELECTED);
							//return true;
							isFound = true;
						}
					}
				}
				//m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);
#endif
				m_MediaCategoryMap[it->GetMediaFilterID()] = *it;
			}

			std::vector<Media> SubMediaList;
			hr = GetControllerInterface->GetMediaInFoler(Category, SubMediaList);
			if (!SUCCEEDED(hr))
			{
				//TRACE0("Failed to get sub media filters\n");
				return false;
			}

			for (auto it = SubMediaList.begin(); it != SubMediaList.end(); it++)
			{
				INT nImage = FILE_INDEX;
#ifndef PROFESSIONAL_EDITION
				if (it->GetAudit().GetStatus() != Audit::APPROVED)
				{
					nImage = UNAPPROVED_FILE_INDEX;
				}
#endif

				if(str != _T(""))
				{
					CString szName = it->GetName().c_str();
					if(szName.MakeLower().Find(str.MakeLower()) <0)
					{
						continue;
					}
				}
				
				isFound = true;

				HTREEITEM item = m_ctrlCategoryTree.InsertItem(it->GetName().c_str(), nImage, nImage, FatherItem);
				m_ctrlCategoryTree.SetItemData(item, it->GetMeidaID());

				m_MediaMap[it->GetMeidaID()] = *it;
			}
		}
	}
	else
	{
		if (FatherItem)
		{
			//DeleteChildrens(Item);

			std::vector<MediaFilter> SubCategoryList;
			hr = GetControllerInterface->GetSubMediaFilters(Category, SubCategoryList);
			if (!SUCCEEDED(hr))
			{
				//TRACE0("Failed to get sub media filters\n");
				return false;
			}

			for (auto it = SubCategoryList.begin(); it != SubCategoryList.end(); it++)
			{
				HTREEITEM item = m_ctrlCategoryTree.InsertItem(it->GetMediaFilterName().c_str(), CATEGORY_INDEX, CATEGORY_INDEX, FatherItem);
				m_ctrlCategoryTree.SetItemData(item, it->GetMediaFilterID());

				CString strName = it->GetMediaFilterName().c_str();
				if(strName.MakeLower().Find(m_FilterValue.MakeLower()) >=0)
				//if(memicmp(it->GetMediaFilterName().c_str(),str,str.GetLength()) == 0)
				{
					isFound = true;
					m_ctrlCategoryTree.SetItemState(item,TVIS_SELECTED,TVIS_SELECTED);
				}
#if 0
				if (it->GetHaveChildrenFilter())
				{
					//m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);

					m_ctrlCategoryTree.SetItemState(item,TVIS_EXPANDED,TVIS_EXPANDED);
					if(SearchSubCategory(*it,item,str) == true)
					{	
						//m_MediaCategoryMap[it->GetMediaFilterID()] = *it;
						//m_ctrlCategoryTree.SetItemState(item,TVIS_SELECTED,TVIS_SELECTED);
						//return true;
						isFound = true;
					}
					else
					{
						std::vector<INT>::iterator iter = find(m_ExpandedCategoryList.begin(), m_ExpandedCategoryList.end(), it->GetMediaFilterID());
						if (iter == m_ExpandedCategoryList.end())
						{
							//m_ctrlCategoryTree.SetItemState(item,0,TVIS_EXPANDED);
							m_ctrlCategoryTree.Expand(item,TVE_COLLAPSE);
						}
					}
					//m_ctrlCategoryTree.Expand(item, TVE_COLLAPSE);
				}
#else
				if (it->GetHaveChildrenFilter() )
				{
					std::vector<INT>::iterator iter = find(m_ExpandedCategoryList.begin(), m_ExpandedCategoryList.end(), it->GetMediaFilterID());
					if (iter == m_ExpandedCategoryList.end())
					{
						m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);
					}
					else
					{
						m_ctrlCategoryTree.SetItemState(item,TVIS_EXPANDED,TVIS_EXPANDED);
						if(SearchSubCategory(*it,item,str) == true)
						{	
							//m_MediaCategoryMap[it->GetMediaFilterID()] = *it;
							//m_ctrlCategoryTree.SetItemState(item,TVIS_SELECTED,TVIS_SELECTED);
							//return true;
							isFound = true;
						}
					}
				}
				
#endif
				m_MediaCategoryMap[it->GetMediaFilterID()] = *it;
			}
		}
	}

	return isFound;
#endif
}


void CMediaLibraryCategoryCtrl::RefreshSubCategory(MediaFilter & Category, HTREEITEM FatherItem)
{
	HRESULT hr;

#if 1 //there is a problem ,sub category can not expand
	if(m_bShowFiles)
	{
		if (FatherItem)
		{
			//DeleteChildrens(Item);

			std::vector<MediaFilter> SubCategoryList;
			hr = GetControllerInterface->GetSubMediaFilters(Category, SubCategoryList);
			if (!SUCCEEDED(hr))
			{
				//TRACE0("Failed to get sub media filters\n");
				return;
			}

			for (auto it = SubCategoryList.begin(); it != SubCategoryList.end(); it++)
			{
				HTREEITEM item = m_ctrlCategoryTree.InsertItem(it->GetMediaFilterName().c_str(), CATEGORY_INDEX, CATEGORY_INDEX, FatherItem);
				m_ctrlCategoryTree.SetItemData(item, it->GetMediaFilterID());

				if (it->GetHaveChildrenFilter() || it->GetHaveChildren())
				{
					//m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);
					
					
					std::vector<INT>::iterator iter = find(m_ExpandedCategoryList.begin(), m_ExpandedCategoryList.end(), it->GetMediaFilterID());
					if (iter != m_ExpandedCategoryList.end())
					{
						m_ctrlCategoryTree.SetItemState(item,TVIS_EXPANDED,TVIS_EXPANDED);
						RefreshSubCategory(*it,item);
					}
					else
					{
						m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);
					}
					
					//m_ctrlCategoryTree.Expand(item, TVE_EXPAND);
				}

				m_MediaCategoryMap[it->GetMediaFilterID()] = *it;
			}

			std::vector<Media> SubMediaList;
			hr = GetControllerInterface->GetMediaInFoler(Category, SubMediaList);
			if (!SUCCEEDED(hr))
			{
				//TRACE0("Failed to get sub media filters\n");
				return;
			}

			for (auto it = SubMediaList.begin(); it != SubMediaList.end(); it++)
			{
				INT nImage = FILE_INDEX;
#ifndef PROFESSIONAL_EDITION
				if (it->GetAudit().GetStatus() != Audit::APPROVED)
				{
					nImage = UNAPPROVED_FILE_INDEX;
				}
#endif

				if(m_FilterValue != _T(""))
				{
					CString TimetableName = it->GetName().c_str();
					if(TimetableName.MakeLower().Find(m_FilterValue.MakeLower()) <0)
					{
						continue;
					}
				}

				HTREEITEM item = m_ctrlCategoryTree.InsertItem(it->GetName().c_str(), nImage, nImage, FatherItem);
				m_ctrlCategoryTree.SetItemData(item, it->GetMeidaID());

				m_MediaMap[it->GetMeidaID()] = *it;
			}
		}
	}
	else
	{
		if (FatherItem)
		{
			//DeleteChildrens(Item);

			std::vector<MediaFilter> SubCategoryList;
			hr = GetControllerInterface->GetSubMediaFilters(Category, SubCategoryList);
			if (!SUCCEEDED(hr))
			{
				//TRACE0("Failed to get sub media filters\n");
				return;
			}

			for (auto it = SubCategoryList.begin(); it != SubCategoryList.end(); it++)
			{
				HTREEITEM item = m_ctrlCategoryTree.InsertItem(it->GetMediaFilterName().c_str(), CATEGORY_INDEX, CATEGORY_INDEX, FatherItem);
				m_ctrlCategoryTree.SetItemData(item, it->GetMediaFilterID());

				if (it->GetHaveChildrenFilter())
				{
					//m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);
					
					
					std::vector<INT>::iterator iter = find(m_ExpandedCategoryList.begin(), m_ExpandedCategoryList.end(), it->GetMediaFilterID());
					if (iter != m_ExpandedCategoryList.end())
					{
						m_ctrlCategoryTree.SetItemState(item,TVIS_EXPANDED,TVIS_EXPANDED);
						RefreshSubCategory(*it,item);
					}
					else
					{
						m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);
					}
					
					//m_ctrlCategoryTree.Expand(item, TVE_EXPAND);
				}

				m_MediaCategoryMap[it->GetMediaFilterID()] = *it;
			}
		}
	}
#endif
}

void CMediaLibraryCategoryCtrl::OnClickedFilterCategoryRefresh()
{
	UpdateData(TRUE);
    //InitRootCategory();

	int SelectedID ;
	HTREEITEM SelectedItem = m_ctrlCategoryTree.GetSelectedItem();
	if (SelectedItem )
	{
		SelectedID = m_ctrlCategoryTree.GetItemData(SelectedItem);
	}

	MediaFilter Category;
	HRESULT hr = GetControllerInterface->GetRootMediaFilter(Category);
	if (!SUCCEEDED(hr))
	{
		return;
	}


	m_MediaMap.clear();
	m_MediaCategoryMap.clear();

	m_RootCategoryID = Category.GetMediaFilterID();

	m_ctrlCategoryTree.DeleteAllItems();

	HTREEITEM itemRoot = m_ctrlCategoryTree.InsertItem(Translate(_T("Media Root Category")), CATEGORY_INDEX, CATEGORY_INDEX);
	m_ctrlCategoryTree.SetItemData(itemRoot, Category.GetMediaFilterID());

	if (m_bShowFiles && (Category.GetHaveChildren() || Category.GetHaveChildrenFilter()))
	{
		//m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, itemRoot);
	}
	else if (!m_bShowFiles && Category.GetHaveChildrenFilter())
	{
		//m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, itemRoot);
	}

	//m_ctrlCategoryTree.Expand(itemRoot, TVE_EXPAND);
	
	m_ctrlCategoryTree.SetItemState(itemRoot,TVIS_EXPANDED,TVIS_EXPANDED);
	//HTREEITEM item = m_ctrlCategoryTree.GetRootItem();
	RefreshSubCategory(Category,itemRoot);
	m_ctrlCategoryTree.Invalidate(true);

	if(SelectedItem)
	{
		HTREEITEM FoundItem = FindCategoryItem(itemRoot,SelectedID);
		if(FoundItem)
		{
			m_ctrlCategoryTree.SetItemState(FoundItem,TVIS_SELECTED,TVIS_SELECTED);
			m_ctrlCategoryTree.SelectItem(FoundItem);
			//GetParent()->SendMessage(MEDIA_LIST_CATEGORY, SelectedID, 0L);
		}
	}
// 	m_ctrlCategoryTree.ModifyStyle(TVS_TRACKSELECT,0);
// 	m_ctrlCategoryTree.ModifyStyle(TVS_TRACKSELECT,TVIS_SELECTED);
}

void CMediaLibraryCategoryCtrl::OnClickedMediaCategory(NMHDR *pNMHDR, LRESULT *pResult)
{
    DWORD   dwpos = GetMessagePos();   
    TVHITTESTINFO ht = {0};  

    ht.pt.x = GET_X_LPARAM(dwpos);
    ht.pt.y = GET_Y_LPARAM(dwpos);
    ::MapWindowPoints(HWND_DESKTOP,pNMHDR->hwndFrom,&ht.pt,1); //把屏幕坐标转换成控件坐标

    HTREEITEM hItem = TreeView_HitTest(pNMHDR->hwndFrom,&ht);   //确定点击的是哪一项

	CWnd *pMediaLibraryPage =  GetRuntimePageCtrl(RUNTIME_CLASS(CMediaLibraryPage));  
	if(pMediaLibraryPage && !pMediaLibraryPage->IsWindowVisible())
	{
		int nImage,nSelectedImage;
		m_ctrlCategoryTree.GetItemImage(hItem,nImage,nSelectedImage);
        
		if(FILE_INDEX == nImage || UNAPPROVED_FILE_INDEX == nImage)
		{
			if(m_ctrlCategoryTree.GetSelectedItem() == hItem)
				*pResult = -1;
		}
	}
   //GetItemIcon(hItem)
  //  CString strSelItemTest = m_ctrlCategoryTree.GetItemText(ht.hItem);
  //  m_ctrlCategoryTree.Select(ht.hItem, TVGN_CARET);
  //  GetParent()->GetParent()->SendMessage(MEDIA_SETACTIVE_TAB, CATEGORY_LIST_TAB4, NULL);
		CWnd *p = AfxGetMainWnd();
		if (p != NULL)
		{
			CWnd *pWnd = SearchWndObj(p,RUNTIME_CLASS(CMediaLibraryListTab));
			if (pWnd != NULL)
			{
				CMediaLibraryListTab* pTab = (CMediaLibraryListTab*)pWnd;
				if (pTab != NULL)
				{
					pTab->SetActiveTab(CATEGORY_LIST_TAB4);
				}
			}
		}		
}

BOOL CMediaLibraryCategoryCtrl::SerializeMediaFrom (CFile& file, std::vector<Media> &mediaList)
{
    CArchive Archive (&file, CArchive::load);
    CString XMLString;
    Archive >> XMLString;


    TiXmlDocument *pDoc = new TiXmlDocument();

	std::string xml = StringUtility::ConvertUnicodeToUTF8(XMLString.GetBuffer(0));
	pDoc->Parse(xml.c_str(), NULL, TIXML_ENCODING_UTF8);
    if (pDoc->Error())
	{
		return FALSE;
	}

    TiXmlElement* pMediaElement = pDoc->RootElement();

    if(pMediaElement == NULL)
    {
        delete pDoc;
        return FALSE;
    }

    BOOL bResult = FALSE;

	Media media;
    bResult = media.LoadFromXML(pMediaElement);
	mediaList.push_back(media);
	while(pMediaElement=pMediaElement->NextSiblingElement())
	{
		bResult = media.LoadFromXML(pMediaElement);
		mediaList.push_back(media);
	}

    Archive.Close();
    delete pDoc;

    return bResult;
}


BOOL CMediaLibraryCategoryCtrl::SerializeMediaCategoryTo (CFile& file, INT CategoryID)
{
    auto it = m_MediaCategoryMap.find(CategoryID);
    if (it == m_MediaCategoryMap.end())
    {
        return FALSE;
    }

    CString XMLString = it->second.ToXML().c_str();

    CArchive Archive (&file, CArchive::store);

    Archive << XMLString;

    Archive.Close();

    return TRUE;
}



BOOL CMediaLibraryCategoryCtrl::SerializeMediaCategoryFrom (CFile& file, MediaFilter &mediaCategory)
{
    CArchive Archive (&file, CArchive::load);
    CString XMLString;
    Archive >> XMLString;


    TiXmlDocument *pDoc = new TiXmlDocument();

	std::string xml = StringUtility::ConvertUnicodeToUTF8(XMLString.GetBuffer(0));
	pDoc->Parse(reinterpret_cast<LPCSTR>(xml.c_str()), NULL, TIXML_ENCODING_UTF8);

    TiXmlElement* pElement = pDoc->RootElement();

    if(pElement == NULL)
    {
        delete pDoc;
        return FALSE;
    }

    BOOL bResult = FALSE;

    bResult = mediaCategory.LoadFromXML(pElement);

    Archive.Close();
    delete pDoc;

    return bResult;
}

void CMediaLibraryCategoryCtrl::OnClickedFilterCategoryFilter()
{
    UpdateData(TRUE);
    //InitRootCategory();
	CString str;
	GetDlgItem(IDC_EDIT_FILTER)->GetWindowText(str);
	int len = str.GetLength();
	if(len <= 0)
	{
		//OnClickedFilterCategoryRefresh();
		InitRootCategory();
		return;
	}

	int SelectedID ;
	HTREEITEM SelectedItem = m_ctrlCategoryTree.GetSelectedItem();
	if (SelectedItem )
	{
		SelectedID = m_ctrlCategoryTree.GetItemData(SelectedItem);
	}

	MediaFilter Category;
	HRESULT hr = GetControllerInterface->GetRootMediaFilter(Category);
	if (!SUCCEEDED(hr))
	{
		return;
	}


	m_MediaMap.clear();
	m_MediaCategoryMap.clear();

	m_RootCategoryID = Category.GetMediaFilterID();

	m_ctrlCategoryTree.DeleteAllItems();

	HTREEITEM itemRoot = m_ctrlCategoryTree.InsertItem(Translate(_T("Media Root Category")), CATEGORY_INDEX, CATEGORY_INDEX);
	m_ctrlCategoryTree.SetItemData(itemRoot, Category.GetMediaFilterID());

	if (m_bShowFiles && (Category.GetHaveChildren() || Category.GetHaveChildrenFilter()))
	{
		//m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, itemRoot);
	}
	else if (!m_bShowFiles && Category.GetHaveChildrenFilter())
	{
		//m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, itemRoot);
	}

	//m_ctrlCategoryTree.Expand(itemRoot, TVE_EXPAND);

	m_ctrlCategoryTree.SetItemState(itemRoot,TVIS_EXPANDED,TVIS_EXPANDED);
	//HTREEITEM item = m_ctrlCategoryTree.GetRootItem();
	int rtn = SearchSubCategory(Category,itemRoot,str);
	m_ctrlCategoryTree.Invalidate(true);

	if(rtn == true)
	{//found a specified item	
		return;
	}

	if(SelectedItem)
	{
		HTREEITEM FoundItem = FindCategoryItem(itemRoot,SelectedID);
		if(FoundItem)
		{
			m_ctrlCategoryTree.SetItemState(FoundItem,TVIS_SELECTED,TVIS_SELECTED);
			m_ctrlCategoryTree.SelectItem(FoundItem);
			//GetParent()->SendMessage(MEDIA_LIST_CATEGORY, SelectedID, 0L);
		}
	}

}


BOOL CMediaLibraryCategoryCtrl::DeleteCategory(const MediaFilter& Category, vector<MediaFilter>& FailedSubCategorys,
    vector<Media>& FailedSubItems)
{
    HRESULT hr;

    vector<MediaFilter> SubCategorys;
    hr = GetControllerInterface->GetSubMediaFilters(Category, SubCategorys);
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to get sub media filter %s\n", Category.GetMediaFilterName());
        return FALSE;
    }

    BOOL bFailedToDeleteChildren = FALSE;
    for (auto it = SubCategorys.begin(); it != SubCategorys.end(); it++)
    {
        vector<MediaFilter> TempFailedSubCategorys;
        vector<Media> TempFailedSubItems;

        if (!DeleteCategory(*it, TempFailedSubCategorys, TempFailedSubItems))
        {
            for (auto itcategory = TempFailedSubCategorys.begin();
                itcategory != TempFailedSubCategorys.end(); itcategory++)
            {
                FailedSubCategorys.push_back(*itcategory);
            }

            for (auto itItem = TempFailedSubItems.begin();
                itItem != TempFailedSubItems.end(); itItem++)
            {
                FailedSubItems.push_back(*itItem);
            }

            bFailedToDeleteChildren = TRUE;
        }
    }

    vector<Media> SubItemList;
    hr = GetControllerInterface->GetMediaInFoler(Category, SubItemList);
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to get sub media %s\n", Category.GetMediaFilterName());
        return FALSE;
    }

	MEDIA_LIST MediaDelTask;

	for (auto it = SubItemList.begin(); it != SubItemList.end(); it++)
    {
   		vector<INT> LayoutIDs;
		HRESULT hr = GetControllerInterface->GetMediaRefInLayout(it->GetMeidaID(), LayoutIDs);
		if (!LayoutIDs.empty())
		{
			return FALSE;
		}
		MediaDelTask.AddMedia(it->GetMeidaID());
	}

	hr = GetMediaLibraryInterface->ClearLocalMedia(MediaDelTask);
	if (!SUCCEEDED(hr))
	{
		TRACE0("Failed to clear local media!\n");
		return FALSE;
	}

	hr = GetMediaLibraryInterface->DeleteServerMedia(MediaDelTask);
	if (!SUCCEEDED(hr))
	{
		TRACE0("Failed to delete server media!\n");
		return FALSE;
	}

/*
    for (auto it = SubItemList.begin(); it != SubItemList.end(); it++)
    {
        hr = GetControllerInterface->DeleteMedia(it->GetMeidaID());
        if (!SUCCEEDED(hr))
        {
            TRACE1("Failed to delete media %d\n", it->GetMeidaID());
            FailedSubItems.push_back(*it);

            bFailedToDeleteChildren = TRUE;
        }
		MediaDelTask.AddMedia(it->GetMeidaID());

    }


    if (bFailedToDeleteChildren)
    {
        return FALSE;
    }*/

    hr = GetControllerInterface->DeleteMediaFilter(Category.GetMediaFilterID());
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to delete media filter %d\n", Category);
        FailedSubCategorys.push_back(Category);
    }

    if (FailedSubCategorys.size() != 0 || FailedSubItems.size() != 0)
    {
        return FALSE;
    }

	return TRUE;
}

BOOL CMediaLibraryCategoryCtrl::GetMediaInfoString(int nMediaID,CString &strInfo )
{
	int mediaID = nMediaID;
    std::vector<int> mediaSingleList;
    std::vector<Media> mediaSingleInfo;
    mediaSingleList.push_back(mediaID);
    //get media thumbnail
#if 0
    MediaThumbnail thumbnail;
    HRESULT hr = GetControllerInterface->GetMediaThumbnail(mediaID, thumbnail);
    if (!SUCCEEDED(hr))
    {
        TRACE0("Failed to Get Media Thumbnail!\n");
    }
    //show media thumbnail in picture control


    vector<BYTE> data;
    thumbnail.GetThumbnail(data);
    MLThumbnail thumbnailImage(data);
    thumbnailImage.InitFromData(data);

    m_wndMediaInfo.m_MediaLibraryInfoCtrl.SetMediaThumbnail(thumbnailImage.GetImage());
#endif
    //get media detail info
    HRESULT hr = GetControllerInterface->GetMedias(mediaSingleList, mediaSingleInfo);
    if (!SUCCEEDED(hr) || mediaSingleInfo.size() < 1)
    {
        TRACE0("Get media detail info fail!\n");
        return E_FAIL;
    }

   // m_wndMediaInfo.m_MediaLibraryInfoCtrl.SetMediaInfo(*mediaSingleInfo.begin());
	Media MediaInfo = *mediaSingleInfo.begin();
    strInfo = Translate(_T("Media Name: "));
    strInfo += MediaInfo.GetName().c_str();
    strInfo += _T("\n");


    strInfo += Translate(_T("Size: "));
    strInfo += FormatSizeString(MediaInfo.GetFileSize());
    strInfo += _T("\n");

    strInfo += Translate(_T("Type: "));
    strInfo +=  MediaInfo.GetMediaType().c_str();
    strInfo += _T("\n");


	CString strItem;
	strItem.Format(_T(" %d"),MediaInfo.GetWidth());
    strInfo += Translate(_T("Width: "));
    strInfo += strItem;
    strInfo += _T("\n");


    strInfo += Translate(_T("Height: "));
	strItem.Format(_T("%d"),MediaInfo.GetHeight());
    strInfo += strItem;
    strInfo += _T("\n");

    strInfo += Translate(_T("Category: "));
    strInfo += MediaInfo.GetMediaFilter().GetMediaFilterName().c_str();
    strInfo += _T("\n");

    strInfo += Translate(_T("Duration: "));
    COleDateTimeSpan tmpDur;
    tmpDur.SetDateTimeSpan(0,0,0,MediaInfo.GetDuration() / 1000);
    strInfo += tmpDur.Format(_T("%H:%M:%S"));
    strInfo += _T("\n");


    User Creator;
    hr = GetControllerInterface->GetUserInfo(MediaInfo.GetUploadUserID(), Creator);


     strInfo +=  Translate(_T("Upload User: "));
     strInfo +=  Creator.GetAccount().GetUserNameW().c_str();
     strInfo += _T("\n");

     strInfo +=  Translate(_T("Upload Time: "));
    if (MediaInfo.GetUploadTime() == S3Time::NULLTime)
    {
          strInfo += Translate(_T("Upload Status: "));
          strInfo +=  Translate(_T("Uploading"));
    }
    else
    {
         strInfo +=  MediaInfo.GetUploadTime().ToString().c_str();
    }
     strInfo += _T("\n");

#if 0
    m_mediaInfo.AddAuditInfo(MediaInfo.GetAudit());

    if (MediaInfo.GetAudit().GetStatus() == Audit::REJECTED)
    {
        CRect ThumbnailRect = m_thumbRect;
        ScreenToClient(&ThumbnailRect);

        CRect BtnRect = ThumbnailRect;
        BtnRect.DeflateRect(0, ThumbnailRect.Height() * 3 / 4, 0, 0);
        ThumbnailRect.DeflateRect(0, 0, 0, ThumbnailRect.Height() / 4);
        m_thumbnail.MoveWindow(ThumbnailRect);
        m_ctrlRejectReason.MoveWindow(BtnRect);

        m_ctrlRejectReason.ShowWindow(SW_SHOW);

        m_szRejectReason = MediaInfo.GetAudit().GetComment().c_str();
    }
    else
    {
        CRect ThumbnailRect = m_thumbRect;
        ScreenToClient(&ThumbnailRect);

        CRect InfoRect;
        m_mediaInfo.GetWindowRect(&InfoRect);
        ScreenToClient(&InfoRect);

        ThumbnailRect.top = InfoRect.top;
        ThumbnailRect.bottom = InfoRect.bottom;

        m_thumbnail.MoveWindow(ThumbnailRect);
        m_ctrlRejectReason.MoveWindow(ThumbnailRect);

        m_ctrlRejectReason.ShowWindow(SW_HIDE);

        m_szRejectReason = _T("");
    }
#endif
	return FALSE;
}

BOOL CMediaLibraryCategoryCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		CWnd *pTreeSubWnd = m_ctrlCategoryTree.GetWindow(GW_CHILD);
		if(pTreeSubWnd)
		{
			CString  className;
			LPTSTR   lpBuff = className.GetBufferSetLength(255);
			::GetClassName(pTreeSubWnd->GetSafeHwnd(),lpBuff,255);
			if(className == _T("Edit"))
			{
				m_ctrlCategoryTree.SendMessage(WM_LBUTTONDOWN,0,0);
				return TRUE;
			}
		}
	}

	if(pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
	{
		CPoint pt;
		::GetCursorPos(&pt);
		m_ctrlCategoryTree.ScreenToClient(&pt);

		HTREEITEM ht = m_ctrlCategoryTree.HitTest(pt);

		CMediaLibraryCategoryCtrl::ITEM_TYPE ItemType;
		int nID = this->GetItemCategory(ht,ItemType);
		static HTREEITEM hTraceItem;
		if( ht &&  ItemType )
		{
			CString strMediaInfo;
			if(hTraceItem != ht)
			{
				this->GetMediaInfoString(nID,strMediaInfo);
				m_MediaInfoCtrl.SetDescription(strMediaInfo);
				m_MediaInfoCtrl.UpdateTipText(Translate(_T("Media Info")),&m_ctrlCategoryTree);
				this->m_MediaInfoCtrl.RelayEvent(pMsg);
				hTraceItem = ht;
			}
			 
		}
		else
		{
			HWND hWnd = pMsg->hwnd;
			pMsg->hwnd = AfxGetMainWnd()->m_hWnd;
			this->m_MediaInfoCtrl.RelayEvent(pMsg);
			pMsg->hwnd = hWnd;
			hTraceItem = NULL;
		}
	}
	return CDialogCtrlTreeBase::PreTranslateMessage(pMsg);
}

void CMediaLibraryCategoryCtrl::OnBnClickedFilterCategorySearch()
{
    CSearchFileDlg dlg(this);

    dlg.DoModal();
}
