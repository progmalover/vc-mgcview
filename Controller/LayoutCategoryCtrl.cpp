// LayoutCategoryCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "LayoutCategoryCtrl.h"
#include "LayoutManagePage.h"
#include "afxdialogex.h"
#include "Localization/Localization.h"
#include "SearchLayoutDlg.h"

#include <algorithm>
#include <tuple>
UINT LAYOUT_LIST_CATEGORY			= ::RegisterWindowMessage (_T("LAYOUT_LIST_CATEGORY"));
UINT LAYOUT_LIST_REFRESH			= ::RegisterWindowMessage (_T("LAYOUT_LIST_REFRESH"));
UINT LAYOUT_SETACTIVE_TAB           = ::RegisterWindowMessage(_T("LAYOUT_SETACTIVE_TAB"));

CLIPFORMAT CLayoutCategoryCtrl::s_LayoutCategoryFormat = ::RegisterClipboardFormat (_T("MAGICVIEW_LAYOUT_CATEGORY"));
CLIPFORMAT CLayoutCategoryCtrl::s_LayoutFormat = ::RegisterClipboardFormat (_T("MAGICVIEW_LAYOUT"));

// CLayoutCategoryCtrl dialog

IMPLEMENT_DYNAMIC(CLayoutCategoryCtrl, CDialogCtrlTreeBase)

CLayoutCategoryCtrl::CLayoutCategoryCtrl(CWnd* pParent /*=NULL*/)
	: CDialogCtrlTreeBase(CLayoutCategoryCtrl::IDD, pParent),m_newAddingItem(0)
{
    m_bFileMode = FALSE;
    m_bShowEmptyLayout = FALSE;
    m_bShowPowerOffLayout = FALSE;
    m_RootCategoryID = -10;
    m_FilterValue = _T("");
}

CLayoutCategoryCtrl::~CLayoutCategoryCtrl()
{
}

void CLayoutCategoryCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILTER_TREE, m_ctrlCategoryTree);
	DDX_Text(pDX, IDC_EDIT_FILTER, m_FilterValue);
	DDX_Control(pDX, IDC_FILTER_CATEGORY_NEW, m_NewBtn);
	DDX_Control(pDX, IDC_FILTER_CATEGORY_DEL, m_DelBtn);
	DDX_Control(pDX, IDC_FILTER_CATEGORY_RENAME, m_RenameBtn);
	DDX_Control(pDX, IDC_FILTER_CATEGORY_REFRESH, m_RefreshBtn);
	DDX_Control(pDX, IDC_FILTER_CATEGORY_FILTER, m_FilterBtn);
    DDX_Control(pDX, IDC_FILTER_CATEGORY_SEARCH, m_SearchBtn);
}


BEGIN_MESSAGE_MAP(CLayoutCategoryCtrl, CDialogCtrlTreeBase)
    ON_WM_SIZE()

    ON_BN_CLICKED(IDC_FILTER_CATEGORY_DEL, &CLayoutCategoryCtrl::OnClickedFilterCategoryDel)
    ON_BN_CLICKED(IDC_FILTER_CATEGORY_NEW, &CLayoutCategoryCtrl::OnClickedFilterCategoryNew)
    ON_BN_CLICKED(IDC_FILTER_CATEGORY_REFRESH, &CLayoutCategoryCtrl::OnClickedFilterCategoryRefresh)
    ON_BN_CLICKED(IDC_FILTER_CATEGORY_RENAME, &CLayoutCategoryCtrl::OnClickedFilterCategoryRename)
    ON_BN_CLICKED(IDC_FILTER_CATEGORY_SEARCH, &CLayoutCategoryCtrl::OnClickedFilterCategorySearch)
    ON_NOTIFY(TVN_SELCHANGED, IDC_FILTER_TREE, &CLayoutCategoryCtrl::OnSelchangedFilterTree)
    ON_NOTIFY(TVN_ITEMEXPANDING, IDC_FILTER_TREE, &CLayoutCategoryCtrl::OnItemexpandingFilterTree)
    ON_NOTIFY(TVN_DELETEITEM, IDC_FILTER_TREE, &CLayoutCategoryCtrl::OnDeleteitemFilterTree)
    ON_NOTIFY(NM_DBLCLK, IDC_FILTER_TREE, &CLayoutCategoryCtrl::OnDblclkFilterTree)
    ON_NOTIFY(TVN_ENDLABELEDIT, IDC_FILTER_TREE, &CLayoutCategoryCtrl::OnEndlabeleditFilterTree)
    ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_FILTER_TREE, &CLayoutCategoryCtrl::OnBeginlabeleditFilterTree)
    ON_NOTIFY(TVN_BEGINDRAG, IDC_FILTER_TREE, &CLayoutCategoryCtrl::OnBegindragFilterTree)
    ON_NOTIFY(NM_CLICK, IDC_FILTER_TREE, &CLayoutCategoryCtrl::OnClickedLayoutCategory)
    ON_MESSAGE(USER_MSG_GET_LAYOUT_ROOT_CATEGORY, OnUpdateRootCategory)
    ON_MESSAGE(USER_MSG_GET_LAYOUT_SUB_CATEGORY, OnUpdateSubCategory)

    ON_MESSAGE(USER_MSG_GET_LAYOUT_SUB_CATEGORY_FILE, OnUpdateSubCategoryAndFiles)

    ON_REGISTERED_MESSAGE(WM_DND_DRAGENTER, OnDragEnter)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGOVER, OnDragOver)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGLEAVE, OnDragLeave)
    ON_REGISTERED_MESSAGE(WM_DND_DROP, OnDrop)

    ON_BN_CLICKED(IDC_FILTER_CATEGORY_FILTER, &CLayoutCategoryCtrl::OnClickedFilterCategoryFilter)
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CLayoutCategoryCtrl)
    EASYSIZE(IDC_FILTER_TREE,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDC_EDIT_FILTER,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_FILTER_CATEGORY_FILTER,IDC_EDIT_FILTER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
END_EASYSIZE_MAP
// CLayoutCategoryCtrl message handlers

void CLayoutCategoryCtrl::EnableFileMode()
{
    m_bFileMode = TRUE;
}


BOOL CLayoutCategoryCtrl::OnInitDialog()
{
    CDialogEx::OnInitDialog();

	m_NewBtn.SetImage(IDB_NEW);
	m_DelBtn.SetImage(IDB_DELETE);
	m_RenameBtn.SetImage(IDB_RENAME);
	m_RefreshBtn.SetImage(IDB_REFRESH);
    m_SearchBtn.SetImage(IDB_SEARCH);
	m_FilterBtn.SetImage(IDB_FILTER);

    SetDefID(IDC_FILTER_CATEGORY_FILTER);

    if(m_bFileMode)
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
    // TODO:  Add extra initialization here

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

    InitRootCategory();

    // register drop target
    if ( !m_DropTarget.Register( this ) )
        return FALSE;

    // privilege
    if (!GetControllerInterface->CheckPrivilege(UserPrivilege_CreateLayoutFilter))
    {
        GetDlgItem(IDC_FILTER_CATEGORY_NEW)->EnableWindow(FALSE);
    }
    if (!GetControllerInterface->CheckPrivilege(UserPrivilege_DeleteLayoutFilter))
    {
        GetDlgItem(IDC_FILTER_CATEGORY_DEL)->EnableWindow(FALSE);

    }
    if (!GetControllerInterface->CheckPrivilege(UserPrivilege_EditLayoutFilter))
    {
        GetDlgItem(IDC_FILTER_CATEGORY_RENAME)->EnableWindow(FALSE);
    }


	m_LayoutToolTipCtrl.Create(this);
	CMFCToolTipInfo tipInfo;
	tipInfo.m_bDrawDescription = TRUE;
	tipInfo.m_bDrawIcon = TRUE;
	tipInfo.m_bVislManagerTheme = TRUE;

	m_LayoutToolTipCtrl.SetParams(&tipInfo);
	m_LayoutToolTipCtrl.AddTool(&this->m_ctrlCategoryTree, Translate(_T("Layout Info")));
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}


void CLayoutCategoryCtrl::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;

}


void CLayoutCategoryCtrl::OnClickedFilterCategoryDel()
{
    // TODO: Add your control notification handler code here
    if (MessageBox(Translate(_T("Are you sure to delete the selected Category?")),Translate(_T("Tips:Delete category")), MB_YESNO|MB_ICONQUESTION) != IDYES)
    {
        return;
    }

    HTREEITEM SeletectItem = m_ctrlCategoryTree.GetSelectedItem();

    if (SeletectItem)
    {
        INT CategoryID = m_ctrlCategoryTree.GetItemData(SeletectItem);

        if (CategoryID == 0xffffffff)
        {
            MessageBox(Translate(_T("the root category should not been deleted!")), Translate(_T("Error:Delete category")), MB_OK|MB_ICONERROR);
            return;
        }

        LayoutGroup Category;
        HRESULT hr = GetControllerInterface->GetLayoutFilter(CategoryID, Category);
        if (!SUCCEEDED(hr))
        {
            TRACE1("Failed to get layout filter %d\n", CategoryID);
            MessageBox(Translate(_T("Failed to delete the selected Category")), Translate(_T("Error:Delete category")), MB_OK|MB_ICONERROR);
            return;
        }

        vector<LayoutGroup> FailedSubCategorys;
        vector<Layout> FailedSubItems;
        if (!DeleteCategory(Category, FailedSubCategorys, FailedSubItems))
        {
            TRACE1("Failed to delete layout filter %d\n", CategoryID);

            CString szLayouts;
            for (auto it = FailedSubItems.begin(); it != FailedSubItems.end(); it++)
            {
                szLayouts += (it->GetLayoutName() + _T(";")).c_str();
            }
            CString ErrorInfo;
#ifdef STARTER_EDITION
			ErrorInfo.Format(Translate(_T("Failed to delete the selected Category, there are some layouts used in the schedules!(%s)")), szLayouts);
#else
            ErrorInfo.Format(Translate(_T("Failed to delete the selected Category, there are some layouts used in the schedules of client groups!!(%s)")), szLayouts);
#endif 
            MessageBox(ErrorInfo, Translate(_T("Error:Delete category")), MB_OK|MB_ICONERROR);
            return;
        }

         m_ctrlCategoryTree.DeleteItem(SeletectItem);

    }
}


void CLayoutCategoryCtrl::OnClickedFilterCategoryNew()
{
    // TODO: Add your control notification handler code here

    HTREEITEM SeletectItem = m_ctrlCategoryTree.GetSelectedItem();

    HTREEITEM NewItem;
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

    LayoutGroup Category;

    CString CategoryName = Translate(_T("New Category"));
    int TryCount = 1;
    HRESULT hr = S_OK;
    while(TryCount < 100)
    {
        if(TryCount > 1)
        {
            CategoryName.Format(Translate(_T("New Category(%d)")), TryCount);
        }

        hr = GetControllerInterface->NewLayoutGroup(CategoryName.GetString(), _T(""), ParentID, Category);
        if (!SUCCEEDED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                GetControllerApp->OnControllerServerOffline();
                return;
            }
            TryCount ++;
        }else
        {
            break;
        }

    }


    if(FAILED(hr))
    {
        MessageBox(Translate(_T("Fail to create new category")), Translate(_T("Error:Create category")), MB_OK|MB_ICONERROR);
        return;
    }

    m_LayoutGroupMap[Category.GetGroupID()] = Category;
    m_newAddingItem = m_ctrlCategoryTree.InsertItem(CategoryName, CATEGORY_INDEX, CATEGORY_INDEX,
        SeletectItem);

    m_ctrlCategoryTree.SetItemData(m_newAddingItem, Category.GetGroupID());

	if(m_ctrlCategoryTree.GetItemState(SeletectItem,TVIS_EXPANDED) & TVIS_EXPANDED)
	{
		 m_ctrlCategoryTree.EditLabel(m_newAddingItem);
		 return;
	}
    m_ctrlCategoryTree.Expand(SeletectItem,  TVE_EXPAND);


}

void CLayoutCategoryCtrl::RefreshSubCategory(LayoutGroup & Category, HTREEITEM FatherItem)
{
	HRESULT hr;
	if(m_bFileMode)
	{
		if (FatherItem)
		{
			std::vector<LayoutGroup> SubCategoryList;
			hr = GetControllerInterface->GetSubLayoutFilters(Category, SubCategoryList);
			if (!SUCCEEDED(hr))
			{
				//TRACE1("Failed to get layout filter %d\n", CategoryID);
				return;
			}

			for (auto it = SubCategoryList.begin(); it != SubCategoryList.end(); it++)
			{
				HTREEITEM item = m_ctrlCategoryTree.InsertItem(it->GetGroupName().c_str(), CATEGORY_INDEX, CATEGORY_INDEX, FatherItem);
				m_ctrlCategoryTree.SetItemData(item, it->GetGroupID());

				if (it->GetHaveChildrenFilter() || it->GetHaveChildren())
				{
					//m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);
					
					std::vector<INT>::iterator iter = find(m_ExpandedCategoryList.begin(), m_ExpandedCategoryList.end(), it->GetGroupID());
					if (iter != m_ExpandedCategoryList.end())
					{
						m_ctrlCategoryTree.SetItemState(item,TVIS_EXPANDED,TVIS_EXPANDED);
						RefreshSubCategory(*it,item);
					}
					else
					{
						m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);
					}
					
				}
				

				m_LayoutGroupMap[it->GetGroupID()] = *it;
			}

			vector<Layout> Layouts;
			hr = GetControllerInterface->GetLayoutInFilter(Category, Layouts);
			if (!SUCCEEDED(hr))
			{
				//TRACE1("Failed to get layout files %d\n", CategoryID);
				return;
			}

			vector<pair<Layout, vector<INT> > > LayouList;
			for (auto it = Layouts.begin(); it != Layouts.end(); it++)
			{
				S3Time TempTime(1900,1,1,1,1,1);
				vector<int> Groups, ClientGroupIDList; 
				vector<int> refTimetableIDList;
				hr = GetControllerInterface->CheckLayoutRef(it->GetLayoutID(), TempTime, Groups, ClientGroupIDList,refTimetableIDList);
				if (!SUCCEEDED(hr))
				{
					TRACE1("Failed to get layout ref %d\n", it->GetLayoutID());
					break;
				}

				LayouList.push_back(make_pair(*it, Groups));
			}

			//const vector<pair<Layout, vector<INT> > >& LayouList = MessageBuf.Get()->second.second;
			vector<pair<Layout, vector<INT> > > MergedLayoutList = LayouList;

			//if(MessageBuf.Get()->first == S3NET_ROOT_FILTER_ID)
			if(Category.GetGroupID() == S3NET_ROOT_FILTER_ID )
			{
				if(m_bShowEmptyLayout)
				{
					Layout layout;
					pair<Layout, vector<INT>> LayoutPair;
					HRESULT hr = GetControllerInterface->GetLayout(S3NET_EMPTY_LAYOUT_ID, layout);

					LayoutPair.first = layout;
					if(SUCCEEDED(hr))
					{
						MergedLayoutList.push_back(LayoutPair);
					}
				}

				if(m_bShowPowerOffLayout)
				{
					Layout layout;
					pair<Layout, vector<INT>> LayoutPair;
					HRESULT hr = GetControllerInterface->GetLayout(S3NET_TURN_OFF_LAYOUT_ID, layout);
					LayoutPair.first = layout;
					if(SUCCEEDED(hr))
					{
						MergedLayoutList.push_back(LayoutPair);
					}
				}
			}

			for (auto it = MergedLayoutList.begin(); it != MergedLayoutList.end(); it++)
			{
				INT nImage = FILE_INDEX;
#ifndef PROFESSIONAL_EDITION
				if (it->first.GetAudit().GetStatus() != Audit::APPROVED)
				{
					nImage = UNAPPROVED_FILE_INDEX;
				}
#endif

				if(it->first.GetLayoutID() == S3NET_TURN_OFF_LAYOUT_ID || it->first.GetLayoutID() == S3NET_EMPTY_LAYOUT_ID)
				{
					nImage = FILE_INDEX;
				}

				if(m_FilterValue != _T(""))
				{
					CString TimetableName = it->first.GetLayoutName().c_str();
					if(TimetableName.MakeLower().Find(m_FilterValue.MakeLower()) <0)
					{
						continue;
					}
				}
				HTREEITEM item = m_ctrlCategoryTree.InsertItem(it->first.GetLayoutName().c_str(), nImage, nImage, FatherItem);
				m_ctrlCategoryTree.SetItemData(item, it->first.GetLayoutID());

				m_LayoutMap[it->first.GetLayoutID()] = it->first;
			}

		}
	}
	else
	{
		if (FatherItem)
		{
			std::vector<LayoutGroup> SubCategoryList;
			hr = GetControllerInterface->GetSubLayoutFilters(Category, SubCategoryList);
			if (!SUCCEEDED(hr))
			{
				//TRACE1("Failed to get layout filter %d\n", CategoryID);
				return;
			}

			for (auto it = SubCategoryList.begin(); it != SubCategoryList.end(); it++)
			{
				HTREEITEM item = m_ctrlCategoryTree.InsertItem(it->GetGroupName().c_str(), CATEGORY_INDEX, CATEGORY_INDEX, FatherItem);
				m_ctrlCategoryTree.SetItemData(item, it->GetGroupID());

				if (it->GetHaveChildrenFilter())
				{
					//m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);
					
					std::vector<INT>::iterator iter = find(m_ExpandedCategoryList.begin(), m_ExpandedCategoryList.end(), it->GetGroupID());
					if (iter != m_ExpandedCategoryList.end())
					{
						m_ctrlCategoryTree.SetItemState(item,TVIS_EXPANDED,TVIS_EXPANDED);
						RefreshSubCategory(*it,item);
					}
					else
					{
						m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);
					}
					
				}

				m_LayoutGroupMap[it->GetGroupID()] = *it;
			}
		
		}
	}
}

INT CLayoutCategoryCtrl::GetSelectedCategory(ITEM_TYPE_LAYOUT& ItemType)
{
    HTREEITEM hItem = m_ctrlCategoryTree.GetSelectedItem();
    if (hItem != NULL)
    {
        INT categoryID = m_ctrlCategoryTree.GetItemData(hItem);

        int nImage, nSelectedImage;
        if( m_ctrlCategoryTree.GetItemImage(hItem, nImage,nSelectedImage) )
        {
            if (CATEGORY_INDEX == nImage)
            {
                ItemType = CATEGORY_INDEX;
            }
            else if (FILE_INDEX == nImage || UNAPPROVED_FILE_INDEX == nImage)
            {
                ItemType = FILE_INDEX;
            }
            else
            {
               ItemType = UNAPPROVED_FILE_INDEX; 
            }
        }

        return categoryID;
    }
    return -1;
}

void CLayoutCategoryCtrl::OnClickedFilterCategoryRefresh()
{
	UpdateData(TRUE);

    //InitRootCategory();
	int SelectedID;
	HTREEITEM SelectedItem = m_ctrlCategoryTree.GetSelectedItem();
	if (SelectedItem )
	{
		SelectedID = m_ctrlCategoryTree.GetItemData(SelectedItem);
	}

	LayoutGroup RootCategory;
	HRESULT hr = GetControllerInterface->GetRootLayoutFilter(RootCategory);
	if (!SUCCEEDED(hr))
	{
		//TRACE1("Failed to get layout filter %d\n", CategoryID);
		return;
	}

	m_LayoutMap.clear();
	m_LayoutGroupMap.clear();

	m_RootCategoryID = RootCategory.GetGroupID();

	m_ctrlCategoryTree.DeleteAllItems();

	HTREEITEM root = m_ctrlCategoryTree.InsertItem(Translate(_T("Layout Root Category")), CATEGORY_INDEX, CATEGORY_INDEX);

	m_ctrlCategoryTree.SetItemData(root, RootCategory.GetGroupID());

	if (m_bFileMode && (RootCategory.GetHaveChildren() || RootCategory.GetHaveChildrenFilter()))
	{
		//m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, root);
	}
	else if (!m_bFileMode && RootCategory.GetHaveChildrenFilter())
	{
		//m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, root);
	}

	//m_ctrlCategoryTree.Expand(root, TVE_EXPAND);

	//m_ctrlCategoryTree.SetItemState(root,TVIS_EXPANDED,TVIS_EXPANDED);
	m_ctrlCategoryTree.SetItemState(root,TVIS_EXPANDED,TVIS_EXPANDED);
	//HTREEITEM item = m_ctrlCategoryTree.GetRootItem();
	RefreshSubCategory(RootCategory,root);
	m_ctrlCategoryTree.Invalidate(true);

	if(SelectedItem)
	{
		HTREEITEM FoundItem = FindCategoryItem(root,SelectedID);
		if(FoundItem)
		{
			m_ctrlCategoryTree.SetItemState(FoundItem,TVIS_SELECTED,TVIS_SELECTED);
			m_ctrlCategoryTree.SelectItem(FoundItem);
			//GetParent()->SendMessage(LAYOUT_LIST_CATEGORY, SelectedID, (LPARAM)&m_ctrlCategoryTree);
		}
	}
}


void CLayoutCategoryCtrl::OnClickedFilterCategoryRename()
{

    HTREEITEM SeletectItem = m_ctrlCategoryTree.GetSelectedItem();

    if (SeletectItem && m_ctrlCategoryTree.GetItemData(SeletectItem) != 0xffffffff)
    {
        m_ctrlCategoryTree.EditLabel(SeletectItem);
    }
}


void CLayoutCategoryCtrl::OnClickedFilterCategorySearch()
{
    CSearchLayoutDlg dlg(this);
    dlg.DoModal();
}

void CLayoutCategoryCtrl::OnSelchangedFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
{

    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    HTREEITEM   ChangingItem   =   pNMTreeView->itemNew.hItem;
    INT CategoryID = m_ctrlCategoryTree.GetItemData(ChangingItem);

    if (GetControllerInterface->CheckPrivilege(UserPrivilege_DeleteLayoutFilter))
    {
        GetDlgItem(IDC_FILTER_CATEGORY_DEL)->EnableWindow(TRUE);
    }
    if (GetControllerInterface->CheckPrivilege(UserPrivilege_EditLayoutFilter))
    {
        GetDlgItem(IDC_FILTER_CATEGORY_RENAME)->EnableWindow(TRUE);
    }

    // root category should not be deleted
    if (CategoryID == 0xFFFFFFFF)
    {
        GetDlgItem(IDC_FILTER_CATEGORY_DEL)->EnableWindow(FALSE);
        GetDlgItem(IDC_FILTER_CATEGORY_RENAME)->EnableWindow(FALSE);
    }
	CWnd * p = AfxGetMainWnd();
	if (p != NULL)
	{
		CWnd *pWnd = SearchWndObj(p,RUNTIME_CLASS(CLayoutListTab));
		if (pWnd != NULL)
		{
			CLayoutListTab* pTab = (CLayoutListTab*)pWnd;
			if (pTab != NULL)
			{
				pTab->SetActiveTab(0);
				SLayoutFilter filter;
				filter = pTab->m_grid.GetFilter();
				filter.setCategoryId(CategoryID);
				pTab->m_grid.SetFilter(filter);		
				pTab->m_grid.RefreshLayoutGrid();
			}	
		}	
	}
    GetParent()->SendMessage(LAYOUT_LIST_CATEGORY, CategoryID, (LPARAM)&m_ctrlCategoryTree);
}


void CLayoutCategoryCtrl::OnItemexpandingFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

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

                DWORD MessageID = USER_MSG_GET_LAYOUT_SUB_CATEGORY;
                if (m_bFileMode)
                {
                    MessageID = USER_MSG_GET_LAYOUT_SUB_CATEGORY_FILE;
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


void CLayoutCategoryCtrl::OnDeleteitemFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    HTREEITEM   Item   =   pNMTreeView->itemOld.hItem;
}


void CLayoutCategoryCtrl::OnDblclkFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    *pResult = 0;
}

void CLayoutCategoryCtrl::OnBeginlabeleditFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	__super::OnTvnBeginlabelEdit(pNMHDR,pResult) ;

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

void CLayoutCategoryCtrl::OnEndlabeleditFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
{

    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;



    HTREEITEM ChangedItem = pTVDispInfo->item.hItem;

    CString ChangedLabel = pTVDispInfo->item.pszText;
    INT     CategoryID   = m_ctrlCategoryTree.GetItemData(ChangedItem);

    if(!(pTVDispInfo->item.mask & TVIF_TEXT)) return;

	if(ChangedLabel.IsEmpty())
		ChangedLabel = m_ctrlCategoryTree.GetItemText(ChangedItem);
    // Retrieves the images associated with the given item.
    int nImage, nSelectedImage;
    if( m_ctrlCategoryTree.GetItemImage(ChangedItem, nImage,nSelectedImage) )
    {
        if (CATEGORY_INDEX == nImage)
        {
            LayoutGroup Category;
            HRESULT hr = GetControllerInterface->GetLayoutFilter(CategoryID, Category);
            if (!SUCCEEDED(hr))
            {
                if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                {
                    GetControllerApp->OnControllerServerOffline();
                }
                else
                {
                    MessageBox(Translate(_T("Failed to get the selected Category.")), Translate(_T("Error:Layout category")), MB_OK|MB_ICONERROR);
                }
                return;
            }
            Category.SetGroupName(ChangedLabel.GetString());



            hr = GetControllerInterface->EditLayoutGroup(Category);
            if (!SUCCEEDED(hr))
            {
                if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
                {
                    GetControllerApp->OnControllerServerOffline();
                }
                else
                {
                    std::tstring desc;
                    GetControllerInterface->GetLastError(desc);
                    MessageBox(Translate(desc.c_str()), Translate(_T("Error:Layout category")), MB_OK|MB_ICONERROR);
                    //MessageBox(Translate(_T("Failed to rename the selected Category.")), Translate(_T("Error")), MB_ICONERROR);
                }
                return;
            }

            m_ctrlCategoryTree.SetItemText(ChangedItem, ChangedLabel);

        }
    }

}


void CLayoutCategoryCtrl::OnBegindragFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
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
                DWORD LayoutID = m_ctrlCategoryTree.GetItemData(DragItem);

                auto it = m_LayoutMap.find(LayoutID);
                if (it == m_LayoutMap.end() ||
                    (it->second.GetAudit().GetStatus() != Audit::APPROVED &&
                    it->first != S3NET_EMPTY_LAYOUT_ID &&
                    it->first != S3NET_TURN_OFF_LAYOUT_ID))
                {
                    MessageBox(Translate(_T("Only approved item could be draged to use!")), Translate(_T("Warning:Drag filter")), MB_OK|MB_ICONINFORMATION);
                    return;
                }

                BOOL bDrag = FALSE;
                try
                {
                    CSharedFile globFile;

                    bDrag =  SerializeLayoutTo(globFile, LayoutID);

                    if (bDrag)
                    {
                        m_OleDataSource.CacheGlobalData (s_LayoutFormat,
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
                    = m_OleDataSource.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE);

                m_OleDataSource.Empty();

            }
            else if (CATEGORY_INDEX == nImage)
            {
                DWORD CategoryID = m_ctrlCategoryTree.GetItemData(DragItem);

                if(CategoryID == 0xffffffff) return;
                if (!GetControllerInterface->CheckPrivilege(UserPrivilege_EditLayoutFilter))
                {
                    MessageBox(Translate(_T("You don't have privilege to rename/move layout category")), Translate(_T("Warning:Drag filter")), MB_OK|MB_ICONEXCLAMATION);
                    return;
                }

                BOOL bDrag = FALSE;
                try
                {
                    CSharedFile globFile;

                    bDrag =  SerializeLayoutGroupTo(globFile, CategoryID);

                    if (bDrag)
                    {
                        m_OleDataSource.CacheGlobalData (s_LayoutCategoryFormat,
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
                MessageBox(Translate(_T("Only approved item could be draged to use!")), Translate(_T("Warning:Drag filter")), MB_OK|MB_ICONEXCLAMATION);
            }
        }
    }

}

void CLayoutCategoryCtrl::OnClickedLayoutCategory(NMHDR *pNMHDR, LRESULT *pResult)
{
    DWORD   dwpos = GetMessagePos();   
    TVHITTESTINFO ht = {0};  

    ht.pt.x = GET_X_LPARAM(dwpos);
    ht.pt.y = GET_Y_LPARAM(dwpos);
    ::MapWindowPoints(HWND_DESKTOP,pNMHDR->hwndFrom,&ht.pt,1); //把屏幕坐标转换成控件坐标

    HTREEITEM hItem = TreeView_HitTest(pNMHDR->hwndFrom,&ht);   //确定点击的是哪一项

	CWnd *pLayoutManagePage =  GetRuntimePageCtrl(RUNTIME_CLASS(CLayoutManagePage));  
	if(pLayoutManagePage && !pLayoutManagePage->IsWindowVisible())
	{
		int nImage,nSelectedImage;
		m_ctrlCategoryTree.GetItemImage(hItem,nImage,nSelectedImage);
        
		if(FILE_INDEX == nImage || UNAPPROVED_FILE_INDEX == nImage)
		{
			if(m_ctrlCategoryTree.GetSelectedItem() == hItem)
				*pResult = -1;
		}
	}

  //  CString strSelItemTest = m_ctrlCategoryTree.GetItemText(ht.hItem);
  //  m_ctrlCategoryTree.Select(ht.hItem, TVGN_CARET);
   // GetParent()->GetParent()->SendMessage(LAYOUT_SETACTIVE_TAB, 0, NULL); //layout tab = 0, search tab = 1
	CWnd *p = AfxGetMainWnd();
	if (p != NULL)
	{
		CWnd *pWnd = SearchWndObj(p,RUNTIME_CLASS(CLayoutListTab));
		if (pWnd != NULL)
		{
			CLayoutListTab* pTab = (CLayoutListTab*)pWnd;
			if (pTab != NULL)
			{
				pTab->SetActiveTab(0);
			}
		}
	}		
}

BOOL CLayoutCategoryCtrl::InitRootCategory()
{
    HRESULT hr = S_OK;
    MediaFilter root;
    DWORD MessageID = USER_MSG_GET_LAYOUT_ROOT_CATEGORY;

    if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,
        (WPARAM)this->GetSafeHwnd(), NULL))//post thread msg
    {
        TRACE1("post message failed,errno:%d\n",::GetLastError());
    }
    GetDlgItem(IDC_FILTER_CATEGORY_DEL)->EnableWindow(FALSE);
    GetDlgItem(IDC_FILTER_CATEGORY_RENAME)->EnableWindow(FALSE);

    return TRUE;
}

LRESULT CLayoutCategoryCtrl::OnUpdateRootCategory (WPARAM wp, LPARAM lp)
{

    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf<LayoutGroup> MessageBuf;
    if (!WorkThreadDataManager< LayoutGroup >
        ::GetDataBuf(nMessageBufID, MessageBuf))
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    m_LayoutMap.clear();
    m_LayoutGroupMap.clear();

	//added by RaymondLiu
	m_ExpandedCategoryList.clear();

    const LayoutGroup& RootCategory = *MessageBuf.Get();
    m_RootCategoryID = RootCategory.GetGroupID();

    m_ctrlCategoryTree.DeleteAllItems();

    HTREEITEM root = m_ctrlCategoryTree.InsertItem(Translate(_T("Layout Root Category")), CATEGORY_INDEX, CATEGORY_INDEX);

    m_ctrlCategoryTree.SetItemData(root, RootCategory.GetGroupID());

    if (m_bFileMode && (RootCategory.GetHaveChildren() || RootCategory.GetHaveChildrenFilter()))
    {
        m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, root);
    }
    else if (!m_bFileMode && RootCategory.GetHaveChildrenFilter())
    {
        m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, root);
    }

    m_ctrlCategoryTree.Expand(root, TVE_EXPAND);

    return 0;
}

LRESULT CLayoutCategoryCtrl::OnUpdateSubCategory (WPARAM wp, LPARAM lp)
{
    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf< std::pair< INT, std::vector<LayoutGroup> > > MessageBuf;
    if (!WorkThreadDataManager< std::pair< INT, std::vector<LayoutGroup> > >
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

        const vector<LayoutGroup>& SubCategoryList = MessageBuf.Get()->second;

        for (auto it = SubCategoryList.begin(); it != SubCategoryList.end(); it++)
        {
            lastItem = m_ctrlCategoryTree.InsertItem(it->GetGroupName().c_str(), CATEGORY_INDEX, CATEGORY_INDEX, Item);
            m_ctrlCategoryTree.SetItemData(lastItem, it->GetGroupID());

            if (it->GetHaveChildrenFilter())
            {
               lastItem =  m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, lastItem);
            }

            m_LayoutGroupMap[it->GetGroupID()] = *it;
        }
    }

	if(m_newAddingItem && lastItem)
	{
		m_newAddingItem = NULL;
		m_ctrlCategoryTree.EditLabel(lastItem);
	}

    return 0;
}

LRESULT CLayoutCategoryCtrl::OnUpdateSubCategoryAndFiles (WPARAM wp, LPARAM lp)
{
    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf<pair< INT, pair< vector<LayoutGroup>, vector<tuple<Layout, vector<INT>, vector<INT> > > > > > MessageBuf;
    if (!WorkThreadDataManager< pair< INT, pair< vector<LayoutGroup>, vector<tuple<Layout, vector<INT>, vector<INT> > > > > >
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

        const vector<LayoutGroup>& SubCategoryList = MessageBuf.Get()->second.first;

        for (auto it = SubCategoryList.begin(); it != SubCategoryList.end(); it++)
        {
            HTREEITEM item = m_ctrlCategoryTree.InsertItem(it->GetGroupName().c_str(), CATEGORY_INDEX, CATEGORY_INDEX, Item);
            m_ctrlCategoryTree.SetItemData(item, it->GetGroupID());

            if (it->GetHaveChildren() || it->GetHaveChildrenFilter())
            {
               lastItem =  m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);
            }

            m_LayoutGroupMap[it->GetGroupID()] = *it;
        }

        const vector<tuple<Layout, vector<INT>, vector<INT> > >& LayouList = MessageBuf.Get()->second.second;
        vector<tuple<Layout, vector<INT>, vector<INT> > > MergedLayoutList = LayouList;

        if(MessageBuf.Get()->first == S3NET_ROOT_FILTER_ID)
        {
            if(m_bShowEmptyLayout)
            {
                Layout layout;
                tuple<Layout, vector<INT>, vector<INT> > LayoutPair;
                HRESULT hr = GetControllerInterface->GetLayout(S3NET_EMPTY_LAYOUT_ID, layout);

                std::get<0>(LayoutPair) = layout;
                if(SUCCEEDED(hr))
                {
                    MergedLayoutList.push_back(LayoutPair);
                }
            }

            if(m_bShowPowerOffLayout)
            {
                Layout layout;
                tuple<Layout, vector<INT>, vector<INT> > LayoutPair;
                HRESULT hr = GetControllerInterface->GetLayout(S3NET_TURN_OFF_LAYOUT_ID, layout);
                std::get<0>(LayoutPair) = layout;
                if(SUCCEEDED(hr))
                {
                    MergedLayoutList.push_back(LayoutPair);
                }
            }
        }

        for (auto it = MergedLayoutList.begin(); it != MergedLayoutList.end(); it++)
        {
            INT nImage = FILE_INDEX;
#ifndef PROFESSIONAL_EDITION
            if (std::get<0>(*it).GetAudit().GetStatus() != Audit::APPROVED)
            {
                nImage = UNAPPROVED_FILE_INDEX;
            }
#endif
            if(std::get<0>(*it).GetLayoutID() == S3NET_TURN_OFF_LAYOUT_ID || std::get<0>(*it).GetLayoutID() == S3NET_EMPTY_LAYOUT_ID)
            {
                nImage = FILE_INDEX;
            }

            if(m_FilterValue != _T(""))
            {
                CString TimetableName = std::get<0>(*it).GetLayoutName().c_str();
                if(TimetableName.MakeLower().Find(m_FilterValue.MakeLower()) <0)
                {
                    continue;
                }
            }
            lastItem= m_ctrlCategoryTree.InsertItem(std::get<0>(*it).GetLayoutName().c_str(), nImage, nImage, Item);
            m_ctrlCategoryTree.SetItemData(lastItem, std::get<0>(*it).GetLayoutID());

            m_LayoutMap[std::get<0>(*it).GetLayoutID()] = std::get<0>(*it);
        }
    }

	if(m_newAddingItem && lastItem)
	{
		m_newAddingItem = NULL;
		m_ctrlCategoryTree.EditLabel(lastItem);
	}
    return 0;
}

HTREEITEM CLayoutCategoryCtrl::FindCategoryItem(HTREEITEM item, INT ID)
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

void CLayoutCategoryCtrl::DeleteChildrens(HTREEITEM item)
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


BOOL CLayoutCategoryCtrl::SerializeLayoutGroupTo (CFile& file, INT LayoutGroupID)
{
    auto it = m_LayoutGroupMap.find(LayoutGroupID);
    if (it == m_LayoutGroupMap.end())
    {
        return FALSE;
    }

    CString XMLString = it->second.ToXML().c_str();

    CArchive Archive (&file, CArchive::store);

    Archive << XMLString;

    Archive.Close();

    return TRUE;
}

BOOL CLayoutCategoryCtrl::SerializeLayoutTo (CFile& file, INT LayoutID)
{
    auto it = m_LayoutMap.find(LayoutID);
    if (it == m_LayoutMap.end())
    {
        return FALSE;
    }

    CString XMLString = it->second.ToXML().c_str();

    CArchive Archive (&file, CArchive::store);

    Archive << XMLString;

    Archive.Close();

    return TRUE;
}

LRESULT CLayoutCategoryCtrl::OnDragLeave(WPARAM wParam, LPARAM lParam)
{
    m_ctrlCategoryTree.SelectDropTarget(NULL);
    return 0;

}

LRESULT CLayoutCategoryCtrl::OnDragEnter(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1L;

    if(m_bFileMode) return DROPEFFECT_NONE;

    CS3OleDropInfo* pInfo = (CS3OleDropInfo*)wParam;

    CPoint point;

    point = pInfo->m_point;
    ClientToScreen(&point);
    m_ctrlCategoryTree.ScreenToClient(&point);

    UINT uFlags;
    HTREEITEM hItem = m_ctrlCategoryTree.HitTest(point, &uFlags);

    int nImage, nSelectedImage;
    if( m_ctrlCategoryTree.GetItemImage(hItem, nImage,nSelectedImage) )
    {
        if(nImage != CATEGORY_INDEX)
        {
            m_ctrlCategoryTree.SelectDropTarget(NULL);
            return DROPEFFECT_NONE;
        }

        if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
        {
            m_ctrlCategoryTree.SelectDropTarget(hItem);

            if (pInfo->m_pDataObject->IsDataAvailable(CLayoutCategoryCtrl::s_LayoutFormat))
            {
                return DROPEFFECT_MOVE;
            }


            if (pInfo->m_pDataObject->IsDataAvailable(CLayoutCategoryCtrl::s_LayoutCategoryFormat))
            {
                return DROPEFFECT_MOVE;
            }
        }
    }


    return DROPEFFECT_NONE;
}

LRESULT CLayoutCategoryCtrl::OnDragOver(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1L;

    if(m_bFileMode) return DROPEFFECT_NONE;


    CS3OleDropInfo* pInfo = (CS3OleDropInfo*)wParam;

    CPoint point;

    point = pInfo->m_point;
    ClientToScreen(&point);
    m_ctrlCategoryTree.ScreenToClient(&point);

    UINT uFlags;
    HTREEITEM hItem = m_ctrlCategoryTree.HitTest(point, &uFlags);

    int nImage, nSelectedImage;
    if( m_ctrlCategoryTree.GetItemImage(hItem, nImage,nSelectedImage) )
    {
        if(nImage != CATEGORY_INDEX)
        {
            m_ctrlCategoryTree.SelectDropTarget(NULL);
            return DROPEFFECT_NONE;
        }

        if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
        {
            m_ctrlCategoryTree.SelectDropTarget(hItem);

            if (pInfo->m_pDataObject->IsDataAvailable(CLayoutCategoryCtrl::s_LayoutFormat))
            {
                return DROPEFFECT_MOVE;
            }


            if (pInfo->m_pDataObject->IsDataAvailable(CLayoutCategoryCtrl::s_LayoutCategoryFormat))
            {
                return DROPEFFECT_MOVE;
            }
        }
    }


    return DROPEFFECT_NONE;
}


LRESULT CLayoutCategoryCtrl::OnDrop(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1L;

    HGLOBAL hGlobal = NULL;
    HDROP hDrop = NULL;

    if(m_bFileMode) return 0;


    CS3OleDropInfo* pInfo = (CS3OleDropInfo*)wParam;

    CPoint point;

    point = pInfo->m_point;
    ClientToScreen(&point);
    m_ctrlCategoryTree.ScreenToClient(&point);

    UINT uFlags;
    HTREEITEM hItem = m_ctrlCategoryTree.HitTest(point, &uFlags);

    int nImage, nSelectedImage;
    if( m_ctrlCategoryTree.GetItemImage(hItem, nImage,nSelectedImage) )
    {
        if(nImage != CATEGORY_INDEX)
        {

            return 0;
        }

        m_ctrlCategoryTree.SelectDropTarget(NULL);

        if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
        {


            DWORD LayoutCategoryID = m_ctrlCategoryTree.GetItemData(hItem);

            if (pInfo->m_pDataObject->IsDataAvailable(CLayoutCategoryCtrl::s_LayoutFormat))
            {
                CFile* pFile;
                pFile = pInfo->m_pDataObject->GetFileData (CLayoutCategoryCtrl::s_LayoutFormat);

                //Layout layout;
				std::vector<Layout> layoutListTemp;

				if (pFile == NULL)
				{
					return FALSE;
				}
				SerializeLayoutFrom(*pFile, layoutListTemp);

                delete pFile;

                if (MessageBox(Translate(_T("Are you sure to move the selected layout?")), Translate(_T("Tips:Drag layout")),MB_YESNO|MB_ICONQUESTION) != IDYES)
                {
                    return 0 ;
                }

				// NOTE: do we need to re-get media?
				std::vector<Layout> layoutList;
				std::vector<int> layoutIDList;
				for(auto it = layoutListTemp.begin() ; it != layoutListTemp.end(); ++it)
					layoutIDList.push_back(it->GetLayoutID());

				if(layoutIDList.size())
				{
					HRESULT hr = GetControllerInterface->GetLayouts(layoutIDList, layoutList);
					if (!SUCCEEDED(hr) || layoutList.size() < 1)
					{
						MessageBox(Translate(_T("Failed to get the layout!")), Translate(_T("Error:Layout category")), MB_OK|MB_ICONERROR);
						return 0;
					}
				}
				for(auto iter = layoutList.begin() ; iter != layoutList.end(); ++iter)
				{
					iter->SetGroupID(LayoutCategoryID);

					HRESULT hr = GetControllerInterface->EditLayout(*iter);
					if (!SUCCEEDED(hr))
					{
						std::tstring errStr;
                        int err = GetControllerInterface->GetLastError(errStr);
						MessageBox(Translate(_T("Failed to move the selected layout.")) + Translate(errStr.c_str()), _T("Error:Layout category"), MB_OK|MB_ICONERROR);
					}
				}
                GetParent()->PostMessage(LAYOUT_LIST_REFRESH, NULL, NULL);

                return 1;
            }


            if (pInfo->m_pDataObject->IsDataAvailable(CLayoutCategoryCtrl::s_LayoutCategoryFormat))
            {
                CFile* pFile;
                pFile = pInfo->m_pDataObject->GetFileData (CLayoutCategoryCtrl::s_LayoutCategoryFormat);

                LayoutGroup layoutgroup;

                if (pFile == NULL)
                {
                    return FALSE;
                }
                SerializeLayoutGroupFrom(*pFile, layoutgroup);
                delete pFile;

                if (MessageBox(Translate(_T("Are you sure to move the selected Category?")),Translate(_T("Tips:Drag category")), MB_YESNO|MB_ICONQUESTION) != IDYES)
                {
                    return 0 ;
                }
                HRESULT hr = GetControllerInterface->GetLayoutFilter(layoutgroup.GetGroupID(), layoutgroup);

                layoutgroup.SetParentID(LayoutCategoryID);

                hr = GetControllerInterface->EditLayoutGroup(layoutgroup);
                if (!SUCCEEDED(hr))
                {
                    MessageBox(Translate(_T("Failed to move the selected Category.")), Translate(_T("Error:Layout category")), MB_OK|MB_ICONERROR);
                }

                InitRootCategory();
                return 1;
            }
        }
    }

    return 0;
}

#if 0
BOOL CLayoutCategoryCtrl::SerializeLayoutFrom (CFile& file, Layout &layout)
{
    CArchive Archive (&file, CArchive::load);
    CString XMLString;
    Archive >> XMLString;


	TiXmlDocument *pDoc = new TiXmlDocument();

	std::string xml = StringUtility::ConvertUnicodeToUTF8(XMLString.GetBuffer(0));
	pDoc->Parse(reinterpret_cast<LPCSTR>(xml.c_str()), NULL, TIXML_ENCODING_UTF8);

    TiXmlElement* pLayoutElement = pDoc->RootElement();

	if(pLayoutElement == NULL)
    {
        delete pDoc;
        return FALSE;
    }

    BOOL bResult = FALSE;

    bResult = layout.LoadFromXML(pLayoutElement);

    Archive.Close();
    delete pDoc;

    return bResult;
}
#else

BOOL CLayoutCategoryCtrl::SerializeLayoutFrom (CFile& file, std::vector<Layout> &layoutList)
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

	TiXmlElement* pLayoutElement = pDoc->RootElement();

	if(pLayoutElement == NULL)
	{
		delete pDoc;
		return FALSE;
	}

	BOOL bResult = FALSE;

	Layout layout;
	bResult = layout.LoadFromXML(pLayoutElement);
	layoutList.push_back(layout);
	while(pLayoutElement=pLayoutElement->NextSiblingElement())
	{
		bResult = layout.LoadFromXML(pLayoutElement);
		layoutList.push_back(layout);
	}

	Archive.Close();
	delete pDoc;

	return bResult;
}

#endif

BOOL CLayoutCategoryCtrl::SerializeLayoutGroupFrom (CFile& file, LayoutGroup &layoutgroup)
{
    CArchive Archive (&file, CArchive::load);
    CString XMLString;
    Archive >> XMLString;


	TiXmlDocument *pDoc = new TiXmlDocument();

	std::string xml = StringUtility::ConvertUnicodeToUTF8(XMLString.GetBuffer(0));
	pDoc->Parse(reinterpret_cast<LPCSTR>(xml.c_str()), NULL, TIXML_ENCODING_UTF8);

    TiXmlElement* pLayoutElement = pDoc->RootElement();

	if(pLayoutElement == NULL)
    {
        delete pDoc;
        return FALSE;
    }

    BOOL bResult = FALSE;

    bResult = layoutgroup.LoadFromXML(pLayoutElement);

    Archive.Close();
    delete pDoc;

    return bResult;
}

void CLayoutCategoryCtrl::OnCancel()
{
}


void CLayoutCategoryCtrl::OnOK()
{
}

void CLayoutCategoryCtrl::OnClickedFilterCategoryFilter()
{
    UpdateData(TRUE);
    InitRootCategory();
}

void CLayoutCategoryCtrl::ShowPowerOffLayout()
{
    m_bShowPowerOffLayout = TRUE;
}

void CLayoutCategoryCtrl::ShowEmptyLayout()
{

    m_bShowEmptyLayout = TRUE;
}




BOOL CLayoutCategoryCtrl::DeleteCategory(const LayoutGroup& Category, vector<LayoutGroup>& FailedSubCategorys,
    vector<Layout>& FailedSubItems)
{
    HRESULT hr;

    vector<LayoutGroup> SubCategorys;
    hr = GetControllerInterface->GetSubLayoutFilters(Category, SubCategorys);
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to get sub layout filter %s\n", Category.GetGroupName());
        return FALSE;
    }

    BOOL bFailedToDeleteChildren = FALSE;
    for (auto it = SubCategorys.begin(); it != SubCategorys.end(); it++)
    {
        vector<LayoutGroup> TempFailedSubCategorys;
        vector<Layout> TempFailedSubItems;

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

    vector<Layout> SubItemList;
    hr = GetControllerInterface->GetLayoutInFilter(Category, SubItemList);
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to get sub layout %s\n", Category.GetGroupName());
        return FALSE;
    }

    for (auto it = SubItemList.begin(); it != SubItemList.end(); it++)
    {
        hr = GetControllerInterface->DeleteLayout(it->GetLayoutID());
        if (!SUCCEEDED(hr))
        {
            TRACE1("Failed to delete layout %d\n", it->GetLayoutID());
            FailedSubItems.push_back(*it);

            bFailedToDeleteChildren = TRUE;
        }
    }

    if (bFailedToDeleteChildren)
    {
        return FALSE;
    }

    hr = GetControllerInterface->DeleteLayoutGroup(Category.GetGroupID());
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to delete layout filter %d\n", Category);
        FailedSubCategorys.push_back(Category);
    }

    if (FailedSubCategorys.size() != 0 || FailedSubItems.size() != 0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

void CLayoutCategoryCtrl::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogCtrlTreeBase::OnWindowPosChanged(lpwndpos);

	// TODO: Add your message handler code here
		UPDATE_EASYSIZE;
}

BOOL CLayoutCategoryCtrl::GetLayoutInfoString(int nLayoutID,CString &strInfo)
{
	if(this->m_LayoutMap.size() <=0)
		return FALSE;

	auto it = m_LayoutMap.find(nLayoutID);
	if( it != m_LayoutMap.end())
	{
		Layout &layInf = it->second;
		strInfo += Translate(_T("Layout Name:"));
		strInfo += layInf.GetLayoutName().c_str();
		strInfo += _T("\n");

		//strInfo += Translate(_T("Media Count:"));
		//strInfo += layInf.GetCustomizedSetting(_T("MediaCount")).c_str();
		//strInfo += _T("\n");
		CString strItem;
		strItem.Format(_T("%d"),layInf.GetWidth());
		if(!strItem.IsEmpty())
		{
			strInfo += Translate(_T("Width: "));
			strInfo +=  strItem;
			strInfo += _T("\n");
		}
		strItem.Format(_T("%d"),layInf.GetHeight());
		if(!strItem.IsEmpty())
		{
			strInfo += Translate(_T("Height: "));
			strInfo += strItem ;
			strInfo += _T("\n");
		}
		strItem = layInf.GetDescription().c_str();
		if(!strItem.IsEmpty())
		{
			strInfo += Translate(_T("Description: "));
			strInfo +=  strItem;
			strInfo += _T("\n");
		}

		strItem.Format(_T("%lld"),layInf.GetLayoutSize());
		if(!strItem.IsEmpty())
		{
			LONGLONG val;
			::StrToInt64Ex(strItem,STIF_DEFAULT,&val);
			double fval =  val /(double)(1024 * 1024);
			fval += 1.0;

			strItem.Format(_T("%.1f Mb"),fval);
			strInfo += Translate(_T("Size: "));
			strInfo +=  strItem;
			strInfo += _T("\n");
		}
	 
		strInfo += Translate(_T("Duration: "));
		COleDateTimeSpan tmpDur;
		tmpDur.SetDateTimeSpan(0,0,0,layInf.GetDuration());
		strInfo += tmpDur.Format(_T("%H:%M:%S"));
		strInfo += _T("\n");

	#ifdef SERVER_EDITION
	   strInfo +=  Translate(_T("Owner user: "));
	   strInfo +=  layInf.GetOwner().c_str();
	   strInfo += _T("\n");
	#endif

	  strInfo +=Translate(_T("Create Time: "));
	  strInfo += layInf.GetCreateTime().c_str();
	  strInfo += _T("\n");
		//layInf.
	}else
		return FALSE;
	 
	return TRUE;
}
BOOL CLayoutCategoryCtrl::PreTranslateMessage(MSG* pMsg)
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
			CPoint pt ;
			::GetCursorPos(&pt);
			m_ctrlCategoryTree.ScreenToClient(&pt);

			HTREEITEM ht = m_ctrlCategoryTree.HitTest(pt);
			static HTREEITEM hTraceIt;
			if(ht)
			{
				int nImg,nSelectImg;
				m_ctrlCategoryTree.GetItemImage(ht,nImg,nSelectImg);
				if(nImg != FILE_INDEX && nImg != UNAPPROVED_FILE_INDEX)
					goto _JUMP;

				
				if(hTraceIt != ht)
				{	
					CString strLayoutInfo;
					int nID = m_ctrlCategoryTree.GetItemData(ht);
					this->GetLayoutInfoString(nID,strLayoutInfo);
					this->m_LayoutToolTipCtrl.SetDescription(strLayoutInfo);
					this->m_LayoutToolTipCtrl.UpdateTipText(Translate(_T("Layout Info")), &m_ctrlCategoryTree );
					this->m_LayoutToolTipCtrl.RelayEvent(pMsg);
					hTraceIt = ht;
				}

			}else
			{
_JUMP:
				HWND hWnd = pMsg->hwnd;
				pMsg->hwnd = AfxGetMainWnd()->m_hWnd;
				this->m_LayoutToolTipCtrl.RelayEvent(pMsg);
				pMsg->hwnd = hWnd;
				hTraceIt = NULL;
				
			}
		
		
		 
	}
	return CDialogCtrlTreeBase::PreTranslateMessage(pMsg);
}
