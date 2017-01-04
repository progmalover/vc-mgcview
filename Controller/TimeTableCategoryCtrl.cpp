// TimeTableCategoryCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "TimeTableCategoryCtrl.h"
#include "afxdialogex.h"
#include "atlconv.h"
#include "Localization/Localization.h"
#include "TimeTablePage.h"
#include <algorithm>


// CTimeTableCategoryCtrl dialog

UINT TIMETABLE_CATEGORY_SEL_CHANGE			= ::RegisterWindowMessage (_T("TIMETABLE_CATEGORY_SEL_CHANGE"));
UINT TIMETABLE_CATEGORY_DBCLICK			= ::RegisterWindowMessage (_T("TIMETABLE_CATEGORY_DBCLICK"));

CLIPFORMAT CTimeTableCategoryCtrl::s_TimeTableCategoryFormat = ::RegisterClipboardFormat (_T("MAGICVIEW_TIMETABLE_CATEGORY"));
CLIPFORMAT CTimeTableCategoryCtrl::s_TimeTableFormat = ::RegisterClipboardFormat (_T("MAGICVIEW_TIMETABLE"));



IMPLEMENT_DYNAMIC(CTimeTableCategoryCtrl, CDialogCtrlTreeBase)

CTimeTableCategoryCtrl::CTimeTableCategoryCtrl(CWnd* pParent /*=NULL*/, BOOL bReadOnly)
	: CDialogCtrlTreeBase(CTimeTableCategoryCtrl::IDD, pParent), m_bEnableToolBar (bReadOnly), m_RootCategoryID (-10)
	,m_newAddingItem(0)
{

    m_FilterValue = _T("");
}

CTimeTableCategoryCtrl::~CTimeTableCategoryCtrl()
{
		delete m_pToolTipCtrl;
}

void CTimeTableCategoryCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_FILTER_TREE, m_ctrlCategoryTree);
    DDX_Control(pDX, IDC_FILTER_CATEGORY_NEW, m_ctrlCategoryNew);
    DDX_Control(pDX, IDC_FILTER_CATEGORY_DEL, m_ctrlCategoryDelete);
    DDX_Control(pDX, IDC_FILTER_CATEGORY_RENAME, m_ctrlCategoryRename);
    DDX_Control(pDX, IDC_FILTER_CATEGORY_REFRESH, m_ctrlCategoryRefresh);
	DDX_Control(pDX, IDC_FILTER_CATEGORY_FILTER,m_ctrlCategoryFilter);
    DDX_Text(pDX, IDC_EDIT_FILTER, m_FilterValue);
}


BEGIN_MESSAGE_MAP(CTimeTableCategoryCtrl, CDialogCtrlTreeBase)
    ON_WM_SIZE()

    ON_REGISTERED_MESSAGE(WM_DND_DRAGENTER, OnDragEnter)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGOVER, OnDragOver)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGLEAVE, OnDragLeave)
    ON_REGISTERED_MESSAGE(WM_DND_DROP, OnDrop)

    ON_MESSAGE(USER_MSG_GET_TIMETABLE_ROOT_CATEGORY, UpdateRootCategory)
    ON_MESSAGE(USER_MSG_GET_TIMETABLE_SUB_CATEGORY_FILE, UpdateSubCategoryAndFiles)

    ON_BN_CLICKED(IDC_FILTER_CATEGORY_DEL, &CTimeTableCategoryCtrl::OnClickedFilterCategoryDel)
    ON_BN_CLICKED(IDC_FILTER_CATEGORY_NEW, &CTimeTableCategoryCtrl::OnClickedFilterCategoryNew)
    ON_BN_CLICKED(IDC_FILTER_CATEGORY_REFRESH, &CTimeTableCategoryCtrl::OnClickedFilterCategoryRefresh)
    ON_BN_CLICKED(IDC_FILTER_CATEGORY_RENAME, &CTimeTableCategoryCtrl::OnClickedFilterCategoryRename)
    ON_NOTIFY(TVN_SELCHANGED, IDC_FILTER_TREE, &CTimeTableCategoryCtrl::OnSelchangedFilterTree)
    ON_NOTIFY(TVN_BEGINDRAG, IDC_FILTER_TREE, &CTimeTableCategoryCtrl::OnBegindragFilterTree)
    ON_NOTIFY(TVN_DELETEITEM, IDC_FILTER_TREE, &CTimeTableCategoryCtrl::OnDeleteitemFilterTree)
    ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_FILTER_TREE, &CTimeTableCategoryCtrl::OnBeginlabeleditFilterTree)
    ON_NOTIFY(TVN_ENDLABELEDIT, IDC_FILTER_TREE, &CTimeTableCategoryCtrl::OnEndlabeleditFilterTree)
    ON_NOTIFY(TVN_ITEMEXPANDING, IDC_FILTER_TREE, &CTimeTableCategoryCtrl::OnItemexpandingFilterTree)
    ON_NOTIFY(NM_DBLCLK, IDC_FILTER_TREE, &CTimeTableCategoryCtrl::OnDblclkFilterTree)
    ON_BN_CLICKED(IDC_FILTER_CATEGORY_FILTER, &CTimeTableCategoryCtrl::OnClickedFilterCategoryFilter)
	ON_WM_WINDOWPOSCHANGED()
	ON_NOTIFY(NM_CLICK, IDC_FILTER_TREE, &CTimeTableCategoryCtrl::OnNMClickFilterTree)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CTimeTableCategoryCtrl)
// 	EASYSIZE(IDC_FILTER_CATEGORY_DEL,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
//
// 	EASYSIZE(IDC_FILTER_CATEGORY_NEW,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
//     EASYSIZE(IDC_FILTER_CATEGORY_RENAME,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    if (m_bEnableToolBar)
    {
        EASYSIZE(IDC_EDIT_FILTER,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
        EASYSIZE(IDC_FILTER_CATEGORY_FILTER,IDC_EDIT_FILTER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
        EASYSIZE(IDC_FILTER_TREE,ES_BORDER,IDC_FILTER_CATEGORY_FILTER,ES_BORDER,ES_BORDER,0)
    }
    else
    {
        EASYSIZE(IDC_EDIT_FILTER,ES_BORDER,IDC_FILTER_CATEGORY_RENAME,ES_BORDER,ES_KEEPSIZE,0)
        EASYSIZE(IDC_FILTER_CATEGORY_FILTER,IDC_EDIT_FILTER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
        EASYSIZE(IDC_FILTER_TREE,ES_BORDER,IDC_FILTER_CATEGORY_FILTER,ES_BORDER,ES_BORDER,0)
    }

END_EASYSIZE_MAP
// CTimeTableCategoryCtrl message handlers


void CTimeTableCategoryCtrl::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;

    // TODO: Add your message handler code here
}



BOOL CTimeTableCategoryCtrl::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    // TODO:  Add extra initialization here
	m_ctrlCategoryNew.SetImage(IDB_NEW);
    m_ctrlCategoryDelete.SetImage(IDB_DELETE);
    m_ctrlCategoryRename.SetImage(IDB_RENAME);
    m_ctrlCategoryRefresh.SetImage(IDB_REFRESH);
	m_ctrlCategoryFilter.SetImage(IDB_FILTER);


    SetDefID(IDC_FILTER_CATEGORY_FILTER);
    
	m_pToolTipCtrl = new CMFCToolTipCtrl();
	m_pToolTipCtrl->Create(AfxGetMainWnd());

	CMFCToolTipInfo tipInf;
	tipInf.m_bDrawIcon = FALSE;
	tipInf.m_bVislManagerTheme = TRUE;  
	tipInf.m_bDrawDescription =TRUE;

	m_pToolTipCtrl->SetParams(&tipInf);
	m_pToolTipCtrl->AddTool(&m_ctrlCategoryTree,Translate(_T("TimeTable Information")));

    GetDlgItem(IDC_FILTER_CATEGORY_SEARCH)->ShowWindow(SW_HIDE);

    if(m_bEnableToolBar)
    {
        RECT rect;
        GetClientRect(&rect);

        GetDlgItem(IDC_FILTER_CATEGORY_DEL)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FILTER_CATEGORY_NEW)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FILTER_CATEGORY_RENAME)->ShowWindow(SW_HIDE);
       

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
//		GetDlgItem(IDC_EDIT_FILTER)->ShowWindow(SW_HIDE);
//		GetDlgItem(IDC_FILTER_CATEGORY_FILTER)->ShowWindow(SW_HIDE);

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

        RECT OrigFilterRect;
        GetDlgItem(IDC_FILTER_CATEGORY_FILTER)->GetWindowRect(&OrigFilterRect);
        ScreenToClient(&OrigFilterRect);

        int ButtomHeight = OrigFilterRect.bottom - OrigFilterRect.top + 1;
        OrigFilterRect.top += ButtomHeight;
        OrigFilterRect.bottom += ButtomHeight;
        OrigFilterRect.left = rect.right - width;
        OrigFilterRect.right = rect.right;
        GetDlgItem(IDC_FILTER_CATEGORY_FILTER)->MoveWindow(&OrigFilterRect);

        RECT OrigEditRect;
        GetDlgItem(IDC_EDIT_FILTER)->GetWindowRect(&OrigEditRect);
        ScreenToClient(&OrigEditRect);

        OrigEditRect.left = 0;
        OrigEditRect.top += ButtomHeight;
        OrigEditRect.bottom += ButtomHeight;
        OrigEditRect.right = OrigFilterRect.left - 5;
        GetDlgItem(IDC_EDIT_FILTER)->MoveWindow(&OrigEditRect);

        RECT OrigTreeRect;
        GetDlgItem(IDC_FILTER_TREE)->GetWindowRect(&OrigTreeRect);
        ScreenToClient(&OrigTreeRect);

        OrigTreeRect.top += ButtomHeight;
        GetDlgItem(IDC_FILTER_TREE)->MoveWindow(&OrigTreeRect);

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

    InitRootCategory();

    // register drop target
    if ( !m_DropTarget.Register( this ) )
        return FALSE;

    // privilege
    if (!GetControllerInterface->CheckPrivilege(UserPrivilege_CreateTimetableFilter))
    {
        GetDlgItem(IDC_FILTER_CATEGORY_NEW)->EnableWindow(FALSE);
    }
    if (!GetControllerInterface->CheckPrivilege(UserPrivilege_DeleteTimetableFilter))
    {
        GetDlgItem(IDC_FILTER_CATEGORY_DEL)->EnableWindow(FALSE);

    }
    if (!GetControllerInterface->CheckPrivilege(UserPrivilege_EditTimetableFilter))
    {
        GetDlgItem(IDC_FILTER_CATEGORY_RENAME)->EnableWindow(FALSE);
    }



    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTimeTableCategoryCtrl::InitRootCategory()
{
    HRESULT hr = S_OK;
    MediaFilter root;
    DWORD MessageID = USER_MSG_GET_TIMETABLE_ROOT_CATEGORY;

    if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,
        (WPARAM)this->GetSafeHwnd(), NULL))//post thread msg
    {
        TRACE1("post message failed,errno:%d\n",::GetLastError());
    }

    GetDlgItem(IDC_FILTER_CATEGORY_DEL)->EnableWindow(FALSE);
    GetDlgItem(IDC_FILTER_CATEGORY_RENAME)->EnableWindow(FALSE);

    m_LayoutMap.clear();
    m_TimetableMap.clear();

    return TRUE;
}


LRESULT CTimeTableCategoryCtrl::UpdateRootCategory (WPARAM wp, LPARAM lp)
{
    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf< TimetableFilter > MessageBuf;
    if (!WorkThreadDataManager<TimetableFilter>
        ::GetDataBuf(nMessageBufID, MessageBuf))
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////

	//added by RaymondLiu
	m_ExpandedCategoryList.clear();

    const TimetableFilter& Category = *MessageBuf.Get();

    m_RootCategoryID = Category.GetTimetableFilterID();

    m_ctrlCategoryTree.DeleteAllItems();

    HTREEITEM itemRoot = m_ctrlCategoryTree.InsertItem(Translate(_T("Time Table Root Category")), CATEGORY_INDEX, CATEGORY_INDEX);
	HTREEITEM lastItem = NULL;
    m_ctrlCategoryTree.SetItemData(itemRoot, Category.GetTimetableFilterID());


    if (Category.GetHaveChildren() || Category.GetHaveChildrenFilter())
    {
       lastItem = m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, itemRoot);
    }

    m_ctrlCategoryTree.Expand(itemRoot, TVE_EXPAND);

	//
	//zxy
	//
	if(m_newAddingItem && lastItem)
	{
		m_newAddingItem = NULL;
		m_ctrlCategoryTree.EditLabel(lastItem);
	}
    return 0;
}


LRESULT CTimeTableCategoryCtrl::UpdateSubCategoryAndFiles (WPARAM wp, LPARAM lp)
{
    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf<  pair< TimetableFilter, pair< vector<TimetableFilter>,
        pair< vector<Timetable>, vector<Layout> > > >  > MessageBuf;
    if (!WorkThreadDataManager<  pair< TimetableFilter, pair< vector<TimetableFilter>,
        pair< vector<Timetable>, vector<Layout> > > >  >
        ::GetDataBuf(nMessageBufID, MessageBuf))
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////


    HTREEITEM Item = FindCategoryItem(m_ctrlCategoryTree.GetRootItem(), MessageBuf.Get()->first.GetTimetableFilterID());
	HTREEITEM lastItem = NULL;
    if (Item)
    {
        DeleteChildrens(Item);

        const vector<TimetableFilter>& SubCategoryList = MessageBuf.Get()->second.first;

        for (auto it = SubCategoryList.begin(); it != SubCategoryList.end(); it++)
        {
            lastItem = m_ctrlCategoryTree.InsertItem(it->GetName().c_str(), CATEGORY_INDEX, CATEGORY_INDEX, Item);
            m_ctrlCategoryTree.SetItemData(lastItem, it->GetTimetableFilterID());

            if (it->GetHaveChildrenFilter() || it->GetHaveChildren())
            {
               lastItem =  m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, lastItem);
            }

            m_TimetableCategoryMap[it->GetTimetableFilterID()] = *it;
        }

        const vector<Timetable>& SubItemList = MessageBuf.Get()->second.second.first;

        for (auto it = SubItemList.begin(); it != SubItemList.end(); it++)
        {
			INT nImage = FILE_INDEX;
#ifndef  PROFESSIONAL_EDITION
            if (it->GetAudit().GetStatus() != Audit::APPROVED)
            {
                nImage = UNAPPROVED_FILE_INDEX;
            }
#endif
            if(m_FilterValue != _T(""))
            {
                CString TimetableName = it->GetTimetableName().c_str();
                if(TimetableName.MakeLower().Find(m_FilterValue.MakeLower()) <0)
                {
                    continue;
                }
            }
            lastItem = m_ctrlCategoryTree.InsertItem(it->GetTimetableName().c_str(), nImage, nImage, Item);
            m_ctrlCategoryTree.SetItemData(lastItem, it->GetTimetableID());

            m_TimetableMap[it->GetTimetableID()] = *it;
        }

        const vector<Layout>& LayoutList = MessageBuf.Get()->second.second.second;

        for (auto it = LayoutList.begin(); it != LayoutList.end(); it++)
        {
            m_LayoutMap[it->GetLayoutID()] = *it;
        }
    }
	//
	//zxy
	//
	if(m_newAddingItem && lastItem)
	{
		m_newAddingItem = NULL;
		m_ctrlCategoryTree.EditLabel(lastItem);
	}
    return 0;
}


HTREEITEM CTimeTableCategoryCtrl::FindCategoryItem(HTREEITEM item, INT ID)
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


void CTimeTableCategoryCtrl::DeleteChildrens(HTREEITEM item)
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

void CTimeTableCategoryCtrl::OnClickedFilterCategoryDel()
{
    HTREEITEM SeletectItem = m_ctrlCategoryTree.GetSelectedItem();

    int nImage, nSelectedImage;
    if( m_ctrlCategoryTree.GetItemImage(SeletectItem, nImage,nSelectedImage) )
    {
        if (CATEGORY_INDEX == nImage)
        {

        }
        else
        {
            return;
        }
    }

    // TODO: Add your control notification handler code here
    if (MessageBox(Translate(_T("Are you sure to delete the selected Category?")),Translate(_T("Tips:Timetable category")), MB_YESNO|MB_ICONQUESTION) != IDYES)
    {
        return;
    }

    if (SeletectItem)
    {
        INT CategoryID = m_ctrlCategoryTree.GetItemData(SeletectItem);


        if (CategoryID == 0xffffffff)
        {
            MessageBox(Translate(_T("the root category should not been deleted!")),Translate(_T("Error:Timetable category")), MB_OK|MB_ICONERROR);
            return;
        }

        TimetableFilter Category;
        HRESULT hr = GetControllerInterface->GetTimetableFilter(CategoryID, Category);
        if (!SUCCEEDED(hr))
        {
            TRACE1("Failed to get timetable filter %d\n", CategoryID);
            MessageBox(Translate(_T("Failed to delete the selected Category")),Translate(_T("Error:Timetable category")), MB_OK|MB_ICONERROR);
            return;
        }

        vector<TimetableFilter> FailedSubCategorys;
        vector<Timetable> FailedSubTimetables;
        if (!DeleteCategory(Category, FailedSubCategorys, FailedSubTimetables))
        {
            TRACE1("Failed to delete timetable filter %d\n", CategoryID);

            CString szTimetables;
            for (auto it = FailedSubTimetables.begin(); it != FailedSubTimetables.end(); it++)
            {
                szTimetables += (it->GetTimetableName() + _T(";")).c_str();
            }
            CString ErrorInfo;
#ifdef STARTER_EDITION
            ErrorInfo.Format(Translate(_T("Failed to delete the selected Category, there are some timetables used in the schedules!(%s)")), szTimetables);
#else
            ErrorInfo.Format(Translate(_T("Failed to delete the selected Category, there are some timetables used in the schedules of client groups!(%s)")), szTimetables);
#endif;
            MessageBox(ErrorInfo,Translate(_T("Error:Timetable category")), MB_OK|MB_ICONERROR);
            return;
        }
         m_ctrlCategoryTree.DeleteItem(SeletectItem);
    }
}


void CTimeTableCategoryCtrl::OnClickedFilterCategoryNew()
{
    // TODO: Add your control notification handler code here

    HTREEITEM SeletectItem = m_ctrlCategoryTree.GetSelectedItem();

    int nImage, nSelectedImage;
    if (m_ctrlCategoryTree.GetItemImage(SeletectItem, nImage, nSelectedImage))
    {
        if (nImage != CATEGORY_INDEX)
        {
            return;
        }
    }   

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

    TimetableFilter Category;
    INT CategoryID = -1;
    Category.SetParentFilterID(ParentID);

    CString CategoryName = Translate(_T("New Category"));
    int TryCount = 1;
    HRESULT hr = S_OK;
    while(TryCount < 100)
    {
        if(TryCount > 1)
        {
            CategoryName.Format(Translate(_T("New Category(%d)")), TryCount);
        }
        Category.SetName(CategoryName.GetString());

        hr = GetControllerInterface->AddTimetableFilter(Category, CategoryID);
        if (!SUCCEEDED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                TRACE0("Failed to add timetable filter\n");
                GetControllerApp->OnControllerServerOffline();
                return;
            }
            TryCount ++;
        }else
        {
            break;
        }

    }

    hr = GetControllerInterface->GetTimetableFilter(CategoryID, Category);
    if (!SUCCEEDED(hr))
    {
        TRACE0("Failed to get timetable filter\n");
        return;
    }

    m_TimetableCategoryMap[CategoryID] = Category;

    this->m_newAddingItem = m_ctrlCategoryTree.InsertItem(CategoryName, CATEGORY_INDEX, CATEGORY_INDEX,
        SeletectItem);

    m_ctrlCategoryTree.SetItemData(m_newAddingItem, CategoryID);

	if(m_ctrlCategoryTree.GetItemState(SeletectItem,TVIS_EXPANDED) & TVIS_EXPANDED)
	{
		 m_ctrlCategoryTree.EditLabel(m_newAddingItem);
		 return;
	}
    m_ctrlCategoryTree.Expand(SeletectItem,  TVE_EXPAND);

    
}

void CTimeTableCategoryCtrl::RefreshSubCategory(TimetableFilter & Category, HTREEITEM FatherItem)
{
	HRESULT hr;
	if (FatherItem)
	{
		DeleteChildrens(FatherItem);

		vector<TimetableFilter> SubCategoryList;
		hr = GetControllerInterface->GetSubTimetableFilters(Category, SubCategoryList);
		if (!SUCCEEDED(hr))
		{
			//TRACE1("Failed to get timetable filter %d\n", CategoryID);
			return;
		}

		for (auto it = SubCategoryList.begin(); it != SubCategoryList.end(); it++)
		{
			HTREEITEM item = m_ctrlCategoryTree.InsertItem(it->GetName().c_str(), CATEGORY_INDEX, CATEGORY_INDEX, FatherItem);
			m_ctrlCategoryTree.SetItemData(item, it->GetTimetableFilterID());

			if (it->GetHaveChildrenFilter() || it->GetHaveChildren())
			{
				//m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);
				
				std::vector<INT>::iterator iter = find(m_ExpandedCategoryList.begin(), m_ExpandedCategoryList.end(), it->GetTimetableFilterID());
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

			m_TimetableCategoryMap[it->GetTimetableFilterID()] = *it;
		}

		vector<Timetable> SubItemList;
		hr = GetControllerInterface->GetTimetablesInFilter(Category, SubItemList);
		if (!SUCCEEDED(hr))
		{
			//TRACE1("Failed to get timetable %d\n", CategoryID);
			return;
		}

		for (auto it = SubItemList.begin(); it != SubItemList.end(); it++)
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
				CString TimetableName = it->GetTimetableName().c_str();
				if(TimetableName.MakeLower().Find(m_FilterValue.MakeLower()) <0)
				{
					continue;
				}
			}
			HTREEITEM item = m_ctrlCategoryTree.InsertItem(it->GetTimetableName().c_str(), nImage, nImage, FatherItem);
			m_ctrlCategoryTree.SetItemData(item, it->GetTimetableID());

			m_TimetableMap[it->GetTimetableID()] = *it;
		}

		//const vector<Timetable>& SubItemList = pInfo->second.second.first;
		vector<Layout> LayoutList;
		std::map<INT, INT> LayoutIDMap;

		for (auto it = SubItemList.begin(); it != SubItemList.end(); it++)
		{
			for (auto it2 = it->LayoutEntryList.begin(); it2 != it->LayoutEntryList.end(); it2++)
			{
				auto itTest = LayoutIDMap.find(it2->GetLayoutID());
				if (itTest == LayoutIDMap.end())
				{
					Layout layout;
					hr = GetControllerInterface->GetLayout(it2->GetLayoutID(), layout);
					if (!SUCCEEDED(hr))
					{
						//TRACE1("Failed to get layout files %d\n", CategoryID);
						break;
					}

					LayoutList.push_back(layout);

					LayoutIDMap[it2->GetLayoutID()] = 1;
				}
			}
		}

		for (auto it = LayoutList.begin(); it != LayoutList.end(); it++)
		{
			m_LayoutMap[it->GetLayoutID()] = *it;
		}
	}
}

void CTimeTableCategoryCtrl::OnClickedFilterCategoryRefresh()
{
    // TODO: Add your control notification handler code here

	UpdateData(TRUE);

    //InitRootCategory();

	int SelectedID;
	HTREEITEM SelectedItem = m_ctrlCategoryTree.GetSelectedItem();
	if (SelectedItem )
	{
		SelectedID = m_ctrlCategoryTree.GetItemData(SelectedItem);
	}

	TimetableFilter Category;
	HRESULT hr = GetControllerInterface->GetRootTimetableFilter(Category);
	if (!SUCCEEDED(hr))
	{
		//TRACE1("Failed to get timetable filter %d\n", CategoryID);
		return;
	}

	m_RootCategoryID = Category.GetTimetableFilterID();

	m_ctrlCategoryTree.DeleteAllItems();

	HTREEITEM itemRoot = m_ctrlCategoryTree.InsertItem(Translate(_T("Time Table Root Category")), CATEGORY_INDEX, CATEGORY_INDEX);
	m_ctrlCategoryTree.SetItemData(itemRoot, Category.GetTimetableFilterID());


	if (Category.GetHaveChildren() || Category.GetHaveChildrenFilter())
	{
		//m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, itemRoot);
	}

	//m_ctrlCategoryTree.Expand(itemRoot, TVE_EXPAND);

	//m_ctrlCategoryTree.SetItemState(itemRoot,TVIS_EXPANDED,TVIS_EXPANDED);
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
			//GetParent()->SendMessage(TIMETABLE_CATEGORY_SEL_CHANGE, SelectedID, (LPARAM)&m_ctrlCategoryTree);
		}
	}
}


void CTimeTableCategoryCtrl::OnClickedFilterCategoryRename()
{
    // TODO: Add your control notification handler code here

    HTREEITEM SeletectItem = m_ctrlCategoryTree.GetSelectedItem();

    if (SeletectItem && m_ctrlCategoryTree.GetItemData(SeletectItem) != 0xffffffff)
    {
        m_ctrlCategoryTree.EditLabel(SeletectItem);
    }
}


void CTimeTableCategoryCtrl::OnSelchangedFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    HTREEITEM   ChangingItem   =   pNMTreeView->itemNew.hItem;
    INT CategoryID = m_ctrlCategoryTree.GetItemData(ChangingItem);

	if (GetControllerInterface->CheckPrivilege(UserPrivilege_CreateTimetableFilter))
	{
		GetDlgItem(IDC_FILTER_CATEGORY_NEW)->EnableWindow(TRUE);
	}
    if (GetControllerInterface->CheckPrivilege(UserPrivilege_DeleteTimetableFilter))
    {
        GetDlgItem(IDC_FILTER_CATEGORY_DEL)->EnableWindow(TRUE);

    }
    if (GetControllerInterface->CheckPrivilege(UserPrivilege_EditTimetableFilter))
    {
        GetDlgItem(IDC_FILTER_CATEGORY_RENAME)->EnableWindow(TRUE);
    }

    // root category should not be deleted
    if (CategoryID == 0xFFFFFFFF)
    {
        GetDlgItem(IDC_FILTER_CATEGORY_DEL)->EnableWindow(FALSE);
        GetDlgItem(IDC_FILTER_CATEGORY_RENAME)->EnableWindow(FALSE);
    }


	Timetable Table;
	if (GetSelectedTimeTable(Table))
	{
		GetDlgItem(IDC_FILTER_CATEGORY_NEW)->EnableWindow(FALSE);
		GetDlgItem(IDC_FILTER_CATEGORY_DEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_FILTER_CATEGORY_RENAME)->EnableWindow(FALSE);
	}


    GetParent()->SendMessage(TIMETABLE_CATEGORY_SEL_CHANGE, CategoryID, (LPARAM)&m_ctrlCategoryTree);
}


LRESULT CTimeTableCategoryCtrl::OnDrop(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1L;

    //if (m_bShowFiles) return 0;

    HGLOBAL hGlobal = NULL;
    HDROP hDrop = NULL;

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
            TimetableFilter NewFilter;

            DWORD CategoryID = m_ctrlCategoryTree.GetItemData(hItem);

            HRESULT hr = GetControllerInterface->GetTimetableFilter(CategoryID, NewFilter);
            if (!SUCCEEDED(hr))
            {
                TRACE1("Failed to Get timetable Filter %d!\n", CategoryID);
            }

            if (pInfo->m_pDataObject->IsDataAvailable(s_TimeTableFormat))
            {
                CFile* pFile;
                pFile = pInfo->m_pDataObject->GetFileData(s_TimeTableFormat);

                Timetable Table;

                if (pFile == NULL)
                {
                    return FALSE;
                }
                SerializeTimeTableFrom(*pFile, Table);

                delete pFile;

                if (MessageBox(Translate(_T("Are you sure to move the selected timetable?")),Translate(_T("Tips:Timetable category")), MB_YESNO|MB_ICONQUESTION) != IDYES)
                {
                    return 0 ;
                }

                Timetable myTable;
                hr = GetControllerInterface->GetTimetable(Table.GetTimetableID(), myTable);
                if (!SUCCEEDED(hr))
                {
                    TRACE1("Failed to get timetable %d\n", Table.GetTimetableID());
                    return 0;
                }

                // change media filter
                myTable.SetParentFilterID(NewFilter.GetTimetableFilterID());

                hr = GetControllerInterface->EditTimetable(myTable);
                if (!SUCCEEDED(hr))
                {
                    std::tstring desc;
                    unsigned int err = GetControllerInterface->GetLastError(desc);
                    MessageBox(Translate(_T("Failed to move the selected timetable.")) + Translate(desc.c_str()),Translate(_T("Error:Timetable category")), MB_OK|MB_ICONERROR);
                    return 0 ;
                }

                m_TimetableMap[myTable.GetTimetableID()] = myTable;

                InitRootCategory();

                return 1;
            }

            if (pInfo->m_pDataObject->IsDataAvailable(CTimeTableCategoryCtrl::s_TimeTableCategoryFormat))
            {
                CFile* pFile;
                pFile = pInfo->m_pDataObject->GetFileData (CTimeTableCategoryCtrl::s_TimeTableCategoryFormat);

                TimetableFilter Category;

                if (pFile == NULL)
                {
                    return FALSE;
                }
                SerializeTimeTableCategoryFrom(*pFile, Category);
                delete pFile;

                if (MessageBox(Translate(_T("Are you sure to move the selected Category?")),Translate(_T("Tips:Timetable category")), MB_YESNO|MB_ICONQUESTION) != IDYES)
                {
                    return 0 ;
                }


                TimetableFilter ServerCategory;
                HRESULT hr = GetControllerInterface->GetTimetableFilter(Category.GetTimetableFilterID(), ServerCategory);
                if (!SUCCEEDED(hr))
                {
                    TRACE1("Failed to get timetable filter %d\n", CategoryID);
                    return 0;
                }

                ServerCategory.SetParentFilterID(CategoryID);

                hr = GetControllerInterface->EditTimetableFilter(ServerCategory);
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
                            MessageBox(Translate(_T("Failed to move the selected Category.")),Translate(_T("Error:Timetable category")), MB_OK|MB_ICONERROR);
                        }
                        return 0 ;
                    }
                }

                m_TimetableCategoryMap[ServerCategory.GetTimetableFilterID()] = ServerCategory;

                InitRootCategory();
                return 1;
            }

        }
    }

    return 0;
}

LRESULT CTimeTableCategoryCtrl::OnDragEnter(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1L;

    //if (m_bShowFiles) return DROPEFFECT_NONE;

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

            if (pInfo->m_pDataObject->IsDataAvailable(CTimeTableCategoryCtrl::s_TimeTableCategoryFormat))
            {
                return DROPEFFECT_MOVE;
            }

            if (pInfo->m_pDataObject->IsDataAvailable(CTimeTableCategoryCtrl::s_TimeTableFormat))
            {
                return DROPEFFECT_MOVE;
            }
        }
    }

    return DROPEFFECT_NONE;
}

LRESULT CTimeTableCategoryCtrl::OnDragOver(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 1L;

    //if (m_bShowFiles)   return DROPEFFECT_NONE;

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

            if (pInfo->m_pDataObject->IsDataAvailable(CTimeTableCategoryCtrl::s_TimeTableCategoryFormat))
            {
                return DROPEFFECT_MOVE;
            }

            if (pInfo->m_pDataObject->IsDataAvailable(CTimeTableCategoryCtrl::s_TimeTableFormat))
            {
                return DROPEFFECT_MOVE;
            }
        }
    }

    return DROPEFFECT_NONE;
}

LRESULT CTimeTableCategoryCtrl::OnDragLeave(WPARAM wParam, LPARAM lParam)
{
    m_ctrlCategoryTree.SelectDropTarget(NULL);
    return 0;
}

void CTimeTableCategoryCtrl::OnBegindragFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
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
            if (FILE_INDEX == nImage || UNAPPROVED_FILE_INDEX == nImage)
            {

                DWORD TimeTableID = m_ctrlCategoryTree.GetItemData(DragItem);

                auto it = m_TimetableMap.find(TimeTableID);
/*
                if (it == m_TimetableMap.end() || it->second.GetAudit().GetStatus() != Audit::APPROVED)
                {
                    AfxMessageBox(_T("Only approved item could be draged to use!"));
                    return;
                }
*/
                BOOL bDrag = FALSE;
                try
                {
                    CSharedFile globFile;

                    bDrag =  SerializeTo(globFile, TimeTableID);

                    if (bDrag)
                    {
                        m_OleDataSource.CacheGlobalData (s_TimeTableFormat,
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

                if(CategoryID == m_RootCategoryID) return;

                if (!GetControllerInterface->CheckPrivilege(UserPrivilege_EditTimetableFilter))
                {
                    MessageBox(Translate(_T("You don't have privilege to rename/move time table category")),Translate(_T("Warning:Timetable category")), MB_OK|MB_ICONEXCLAMATION);
                    return;
                }

                BOOL bDrag = FALSE;
                try
                {
                    CSharedFile globFile;

                    bDrag =  SerializeTimeTableCategoryTo(globFile, CategoryID);

                    if (bDrag)
                    {
                        m_OleDataSource.CacheGlobalData (s_TimeTableCategoryFormat,
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
                MessageBox(_T("Only approved item could be draged to use!"),Translate(_T("Warning:Timetable category")), MB_OK|MB_ICONEXCLAMATION);
            }

        }
    }
}


void CTimeTableCategoryCtrl::OnDeleteitemFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
}


void CTimeTableCategoryCtrl::OnBeginlabeleditFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	__super::OnTvnBeginlabelEdit(pNMHDR,pResult);
    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    HTREEITEM ChangedItem = pTVDispInfo->item.hItem;

    CString ChangedLabel = pTVDispInfo->item.pszText;
    INT     CategoryID   = m_ctrlCategoryTree.GetItemData(ChangedItem);
    if (CategoryID == 0xffffffff)
    {
        *pResult = TRUE;
        return;
    }

    // Retrieves the images associated with the given item.
    int nImage, nSelectedImage;
    if( m_ctrlCategoryTree.GetItemImage(ChangedItem, nImage,nSelectedImage) )
    {
        if (CATEGORY_INDEX == nImage)
        {
            if (!GetControllerInterface->CheckPrivilege(UserPrivilege_EditTimetableFilter))
            {
                *pResult = TRUE;
                return;
            }
        }
        else if (FILE_INDEX == nImage || UNAPPROVED_FILE_INDEX == nImage)
        {
            if (!GetControllerInterface->CheckPrivilege(UserPrivilege_PublishTimetable))
            {
                *pResult = TRUE;
                return;
            }
        }
    }
}

void CTimeTableCategoryCtrl::OnEndlabeleditFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    HTREEITEM ChangedItem = pTVDispInfo->item.hItem;

    CString ChangedLabel = pTVDispInfo->item.pszText;
    INT     CategoryID   = m_ctrlCategoryTree.GetItemData(ChangedItem);

	if(ChangedLabel.IsEmpty())
		ChangedLabel = m_ctrlCategoryTree.GetItemText(ChangedItem);
    // Retrieves the images associated with the given item.
    int nImage, nSelectedImage;
    if( m_ctrlCategoryTree.GetItemImage(ChangedItem, nImage,nSelectedImage) )
    {
        if (CATEGORY_INDEX == nImage)
        {
            TimetableFilter Category;
            HRESULT hr = GetControllerInterface->GetTimetableFilter(CategoryID, Category);
            if (!SUCCEEDED(hr))
            {
                TRACE1("Failed to get timetable filter %d\n", CategoryID);
                return;
            }

            Category.SetName(ChangedLabel.GetString());

            hr = GetControllerInterface->EditTimetableFilter(Category);
            if (!SUCCEEDED(hr))
            {
                TRACE1("Failed to edit timetable filter %d\n", CategoryID);
				MessageBox(Translate(_T("Failed to rename the selected Category.")),Translate(_T("Error:Timetable category")), MB_OK|MB_ICONERROR);
                return;
            }

            m_TimetableCategoryMap[Category.GetTimetableFilterID()] = Category;

            m_ctrlCategoryTree.SetItemText(ChangedItem, ChangedLabel);

        }
        else if (FILE_INDEX == nImage || UNAPPROVED_FILE_INDEX == nImage)
        {
            Timetable myTable;
            HRESULT hr = GetControllerInterface->GetTimetable(CategoryID, myTable);
            if (!SUCCEEDED(hr))
            {
                TRACE1("Failed to get timetable %d\n", CategoryID);
                return;
            }

            myTable.SetTimetableName(ChangedLabel.GetString());

            hr = GetControllerInterface->EditTimetable(myTable);
            if (!SUCCEEDED(hr))
            {
                TRACE2("Failed to edit timetable %d, %s\n", CategoryID, myTable.GetTimetableName());
				MessageBox(Translate(_T("Failed to rename the selected timetable!")),Translate(_T("Error:Timetable category")), MB_OK|MB_ICONERROR);
				
                return;
            }

            m_TimetableMap[myTable.GetTimetableID()] = myTable;

            m_ctrlCategoryTree.SetItemText(ChangedItem, ChangedLabel);
        }
    }
}


void CTimeTableCategoryCtrl::OnItemexpandingFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
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

                DWORD MessageID = USER_MSG_GET_TIMETABLE_SUB_CATEGORY_FILE;

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


void CTimeTableCategoryCtrl::OnDblclkFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    *pResult = 0;

    GetParent()->SendMessage(TIMETABLE_CATEGORY_DBCLICK, 0, 0);
}

bool CTimeTableCategoryCtrl::isCategorySelected()
{
	HTREEITEM hItem = m_ctrlCategoryTree.GetSelectedItem();
	if (hItem != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT  CTimeTableCategoryCtrl::GetCurCategoryID()
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

            }
            else if (FILE_INDEX == nImage || UNAPPROVED_FILE_INDEX == nImage)
            {
                categoryID = m_ctrlCategoryTree.GetItemData(m_ctrlCategoryTree.GetParentItem(hItem));
            }
        }

        return categoryID;
    }

    return m_ctrlCategoryTree.GetItemData(m_ctrlCategoryTree.GetRootItem());
}

void  CTimeTableCategoryCtrl::RefreshCurCategory()
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

            }
            else if (FILE_INDEX == nImage || UNAPPROVED_FILE_INDEX == nImage)
            {
                hItem = m_ctrlCategoryTree.GetParentItem(hItem);
            }
        }
    }

    // Retrieves the images associated with the given item.
    int nImage, nSelectedImage;
    if( m_ctrlCategoryTree.GetItemImage(hItem, nImage,nSelectedImage) )
    {
        if (CATEGORY_INDEX == nImage)
        {
            INT CategoryID = m_ctrlCategoryTree.GetItemData(hItem);

            DWORD MessageID = USER_MSG_GET_TIMETABLE_SUB_CATEGORY_FILE;

            if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,
                (WPARAM)this->GetSafeHwnd(), LPARAM(CategoryID)))//post thread msg
            {
                TRACE1("post message failed,errno:%d\n",::GetLastError());
            }
        }
    }
}

BOOL  CTimeTableCategoryCtrl::GetItemTimeTable(HTREEITEM ht,Timetable& Table)
{
    if (ht != NULL)
    {
        INT categoryID = m_ctrlCategoryTree.GetItemData(ht);

        int nImage, nSelectedImage;
        if( m_ctrlCategoryTree.GetItemImage(ht, nImage,nSelectedImage) )
        {
            if (CATEGORY_INDEX == nImage)
            {
                return FALSE;
            }
            else if (FILE_INDEX == nImage || UNAPPROVED_FILE_INDEX == nImage)
            {
                auto it = m_TimetableMap.find(categoryID);
                if (it == m_TimetableMap.end())
                {
                    return FALSE;
                }
                Table = it->second;

                return TRUE;
            }
        }
    }

    return FALSE;
}
BOOL  CTimeTableCategoryCtrl::GetSelectedTimeTable(Timetable& Table)
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
                return FALSE;
            }
            else if (FILE_INDEX == nImage || UNAPPROVED_FILE_INDEX == nImage)
            {
                auto it = m_TimetableMap.find(categoryID);
                if (it == m_TimetableMap.end())
                {
                    return FALSE;
                }
                Table = it->second;

                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL CTimeTableCategoryCtrl::AddTimetable(const Timetable& Table)
{
    HTREEITEM hPrarentItem = FindCategoryItem(m_ctrlCategoryTree.GetRootItem(), Table.GetParentFilterID());

    INT nImage = FILE_INDEX;
#ifndef PROFESSIONAL_EDITION
    if (Table.GetAudit().GetStatus() != Audit::APPROVED)
    {
        nImage = UNAPPROVED_FILE_INDEX;
    }
#endif
    HTREEITEM item = m_ctrlCategoryTree.InsertItem(Table.GetTimetableName().c_str(), nImage, nImage, hPrarentItem);
    m_ctrlCategoryTree.SetItemData(item, Table.GetTimetableID());

    m_TimetableMap[Table.GetTimetableID()] = Table;

    return TRUE;
}



BOOL CTimeTableCategoryCtrl::SerializeTo (CFile& file, INT TimeTabletID)
{

    auto itTimeTable = m_TimetableMap.find(TimeTabletID);
    if (itTimeTable == m_TimetableMap.end())
    {
        MessageBox(Translate(_T("Failed to get timetable from server")),Translate(_T("Error:Timetable category")), MB_OK|MB_ICONERROR);
        return FALSE;
    }


    CString XMLString = itTimeTable->second.ToXML().c_str();

    CArchive Archive (&file, CArchive::store);

    Archive << XMLString;

    Archive.Close();

    return TRUE;
}

BOOL CTimeTableCategoryCtrl::SerializeTimeTableFrom (CFile& file, Timetable&Table)
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

    bResult = Table.LoadFromXML(pElement);

    Archive.Close();
    delete pDoc;

    return bResult;
}


BOOL CTimeTableCategoryCtrl::SerializeTimeTableCategoryTo (CFile& file, INT CategoryID)
{
    auto it = m_TimetableCategoryMap.find(CategoryID);
    if (it == m_TimetableCategoryMap.end())
    {
        return FALSE;
    }

    CString XMLString = it->second.ToXML().c_str();

    CArchive Archive (&file, CArchive::store);

    Archive << XMLString;

    Archive.Close();

    return TRUE;
}



BOOL CTimeTableCategoryCtrl::SerializeTimeTableCategoryFrom (CFile& file, TimetableFilter &Category)
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

    bResult = Category.LoadFromXML(pElement);

    Archive.Close();
    delete pDoc;

    return bResult;
}

void CTimeTableCategoryCtrl::OnCancel()
{
}


void CTimeTableCategoryCtrl::OnOK()
{
}

void CTimeTableCategoryCtrl::OnClickedFilterCategoryFilter()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    InitRootCategory();
}

BOOL CTimeTableCategoryCtrl::DeleteCategory(const TimetableFilter& Category, vector<TimetableFilter>& FailedSubCategorys,
    vector<Timetable>& FailedSubTimetables)
{
    HRESULT hr;

    vector<TimetableFilter> SubCategorys;
    hr = GetControllerInterface->GetSubTimetableFilters(Category, SubCategorys);
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to get sub timetable filter %s\n", Category.GetName());
        return FALSE;
    }

    BOOL bFailedToDeleteChildren = FALSE;
    for (auto it = SubCategorys.begin(); it != SubCategorys.end(); it++)
    {
        vector<TimetableFilter> TempFailedSubCategorys;
        vector<Timetable> TempFailedSubTimetables;

        if (!DeleteCategory(*it, TempFailedSubCategorys, TempFailedSubTimetables))
        {
            for (auto itcategory = TempFailedSubCategorys.begin();
                itcategory != TempFailedSubCategorys.end(); itcategory++)
            {
                FailedSubCategorys.push_back(*itcategory);
            }

            for (auto itTimetable = TempFailedSubTimetables.begin();
                itTimetable != TempFailedSubTimetables.end(); itTimetable++)
            {
                FailedSubTimetables.push_back(*itTimetable);
            }

            bFailedToDeleteChildren = TRUE;
        }
    }

    vector<Timetable> SubItemList;
    hr = GetControllerInterface->GetTimetablesInFilter(Category, SubItemList);
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to get sub timetable %s\n", Category.GetName());
        return FALSE;
    }

    for (auto it = SubItemList.begin(); it != SubItemList.end(); it++)
    {
        hr = GetControllerInterface->DeleteTimetable(it->GetTimetableID());
        if (!SUCCEEDED(hr))
        {
            TRACE1("Failed to delete timetable %d\n", it->GetTimetableID());
            FailedSubTimetables.push_back(*it);

            bFailedToDeleteChildren = TRUE;
        }
    }

    if (bFailedToDeleteChildren)
    {
        return FALSE;
    }

    hr = GetControllerInterface->DeleteTimetableFilter(Category.GetTimetableFilterID());
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to delete timetable filter %d\n", Category);
        FailedSubCategorys.push_back(Category);
    }

    if (FailedSubCategorys.size() != 0 || FailedSubTimetables.size() != 0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

void CTimeTableCategoryCtrl::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogCtrlTreeBase::OnWindowPosChanged(lpwndpos);
	UPDATE_EASYSIZE;
	// TODO: Add your message handler code here
}

BOOL CTimeTableCategoryCtrl::GetAuditInfoString(Audit adt,CString &strInfo)
{
	HRESULT hr;
    User Requester;
    GetControllerInterface->GetUserInfo(adt.GetRequestUserID(), Requester);

    User Approver;
    GetControllerInterface->GetUserInfo(adt.GetApproveUserID(), Approver);

#ifdef SERVER_EDITION

    //InfoDisplayItem InfoItem;

    strInfo += Translate(_T("Status: "));
    strInfo += Translate(adt.GetStatusStr().c_str());
    strInfo +=_T("\r\n"); 

    switch(adt.GetStatus())
    {
    case Audit::CREATEBUTNOTAPPROVED:
        strInfo += Translate(_T("Request User: "));
        strInfo += Requester.GetAccount().GetUserNameW().c_str();
        strInfo +=_T("\r\n");


        strInfo += Translate(_T("Request Time: "));

        strInfo += adt.GetRequestTime().ToString().c_str();;
        strInfo +=_T("\r\n");
        break;
    case Audit::REJECTED:
        strInfo += Translate(_T("Request User: "));
        strInfo += Requester.GetAccount().GetUserNameW().c_str();
        strInfo +=_T("\r\n");


        strInfo += Translate(_T("Request Time: "));

        strInfo += adt.GetRequestTime().ToString().c_str();;
        strInfo +=_T("\r\n");


        strInfo += Translate(_T("Reject User: "));

        strInfo  += Approver.GetAccount().GetUserNameW().c_str();
        strInfo +=_T("\r\n");


        strInfo += Translate(_T("Reject Time: "));

        strInfo += adt.GetApproveTime().ToString().c_str();
        strInfo +=_T("\r\n");

        break;
    case Audit::APPROVED:
        strInfo += Translate(_T("Request User: "));
        strInfo += Requester.GetAccount().GetUserNameW().c_str();
        strInfo +=_T("\r\n");


        strInfo += Translate(_T("Request Time: "));

		strInfo  += adt.GetRequestTime().ToString().c_str();;
        strInfo  +=_T("\r\n");


        strInfo  += Translate(_T("Approve User: "));

        strInfo  += Approver.GetAccount().GetUserNameW().c_str();
        strInfo  +=_T("\r\n");


        strInfo  += Translate(_T("Approve Time: "));

        strInfo  += adt.GetApproveTime().ToString().c_str();
        strInfo  +=_T("\r\n");
        break;
    case Audit::RETIRED:
        strInfo += Translate(_T("Request User: "));
        strInfo += Requester.GetAccount().GetUserNameW().c_str();
        strInfo +=_T("\r\n");


        strInfo += Translate(_T("Request Time: "));

        strInfo += adt.GetRequestTime().ToString().c_str();;
        strInfo +=_T("\r\n");


        strInfo += Translate(_T("Retired User: "));

        strInfo += Approver.GetAccount().GetUserNameW().c_str();
        strInfo +=_T("\r\n");


        strInfo += Translate(_T("Retired Time: "));

        strInfo += adt.GetApproveTime().ToString().c_str();
        strInfo +=_T("\r\n");
        break;
    case Audit::DELETED:
        strInfo += Translate(_T("Request User: "));
        strInfo +=  Requester.GetAccount().GetUserNameW().c_str();
        strInfo +=_T("\r\n");


       strInfo += Translate(_T("Request Time: "));

       strInfo += adt.GetRequestTime().ToString().c_str();;
        strInfo +=_T("\r\n");


        strInfo += Translate(_T("Delete User: "));

        strInfo += Approver.GetAccount().GetUserNameW().c_str();
        strInfo +=_T("\r\n");


        strInfo += Translate(_T("Delete Time: "));

        strInfo += adt.GetApproveTime().ToString().c_str();
        strInfo +=_T("\r\n");
        break;
    case Audit::RESERVED:
        break;
    case Audit::UNEDITABLE:
        break;
    case Audit::NONE:
        break;
    }

#endif
	return TRUE;
}
BOOL CTimeTableCategoryCtrl::GetTimetableInfoString(Timetable tb,CString &strInfo)
{
	
   strInfo = Translate(_T("Timetable Name: "));
   strInfo += tb.GetTimetableName().c_str();
   strInfo +=_T("\r\n");
   strInfo += Translate(_T("Duration: "));
   strInfo += StringUtility::itos(tb.GetDuration()).c_str();
   strInfo += _T(" ");
   strInfo += Translate(_T("seconds"));
   strInfo += _T("\r\n");


   int UserID = tb.GetCreateUserID();

    User Creator;
    HRESULT hr = GetControllerInterface->GetUserInfo(UserID, Creator);
    if (!SUCCEEDED(hr))
    {
        TRACE1("Failed to get user info %d", UserID);
        return FALSE;
    }
#ifdef SERVER_EDITION
    strInfo += Translate(_T("Creator: "));
    strInfo += Creator.GetAccount().GetUserNameW().c_str();
    strInfo += _T("\r\n");
#endif
    GetAuditInfoString(tb.GetAudit(),strInfo);

   /* if (Table.GetAudit().GetStatus() == Audit::REJECTED)
    {
        CRect ThumbnailRect;
        m_Info.GetWindowRect(&ThumbnailRect);
        ScreenToClient(&ThumbnailRect);

        CRect BtnRect;
        m_ctrlRejectReason.GetWindowRect(&BtnRect);
        ScreenToClient(&BtnRect);
        ThumbnailRect.top = BtnRect.bottom;

        m_Info.MoveWindow(ThumbnailRect);

        m_ctrlRejectReason.ShowWindow(SW_SHOW);

        m_szRejectReason = Table.GetAudit().GetComment().c_str();
    }
    else
    {
        CRect ThumbnailRect;
        m_Info.GetWindowRect(&ThumbnailRect);
        ScreenToClient(&ThumbnailRect);

        CRect BtnRect;
        m_ctrlRejectReason.GetWindowRect(&BtnRect);
        ScreenToClient(&BtnRect);
        ThumbnailRect.top = BtnRect.top;

        m_Info.MoveWindow(ThumbnailRect);

        m_ctrlRejectReason.ShowWindow(SW_HIDE);

        m_szRejectReason = _T("");
    }*/

	return TRUE;
}
BOOL CTimeTableCategoryCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(WM_MOUSEFIRST <= pMsg->message &&  pMsg->message <= WM_MOUSELAST)
   {
	   CPoint pt(GET_X_LPARAM(pMsg->lParam),GET_Y_LPARAM(pMsg->lParam));
	  
	  HTREEITEM ht = this->m_ctrlCategoryTree.HitTest(pt);
	  static HTREEITEM hTraceIt;
	  if(ht )
	  {		
		  if(hTraceIt != ht )
		 {
			Timetable Table;
			if(!GetItemTimeTable(ht,Table))
				goto  _JUMP;
			CString StrTimetableInfo;
			this->GetTimetableInfoString(Table,StrTimetableInfo);
			m_pToolTipCtrl->SetDescription( StrTimetableInfo);
			m_pToolTipCtrl->UpdateTipText(Translate(_T("TimeTable Information")),&m_ctrlCategoryTree );
			m_pToolTipCtrl->RelayEvent(pMsg);
			hTraceIt = ht; 
		 }
		 		   
	  }else
	  {
		  _JUMP:
		  HWND hwnd = pMsg->hwnd;
		  pMsg->hwnd  = AfxGetMainWnd()->m_hWnd;
		  m_pToolTipCtrl->RelayEvent(pMsg);
		  pMsg->hwnd = hwnd;
		  hTraceIt = NULL;
		  
	  }

   }  
	return CDialogCtrlTreeBase::PreTranslateMessage(pMsg);
}


void CTimeTableCategoryCtrl::OnNMClickFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	DWORD   dwpos = GetMessagePos();   
    TVHITTESTINFO ht = {0};  

    ht.pt.x = GET_X_LPARAM(dwpos);
    ht.pt.y = GET_Y_LPARAM(dwpos);
    ::MapWindowPoints(HWND_DESKTOP,pNMHDR->hwndFrom,&ht.pt,1); //把屏幕坐标转换成控件坐标

	HTREEITEM hItem = TreeView_HitTest(pNMHDR->hwndFrom,&ht);
	CWnd *pTimeTablePage =  GetRuntimePageCtrl(RUNTIME_CLASS(CTimeTablePage)); //value returned maybe is null,because this class is used in other page. 
	if(pTimeTablePage && !pTimeTablePage->IsWindowVisible())
	{
		int nImage,nSelectedImage;
		m_ctrlCategoryTree.GetItemImage(hItem,nImage,nSelectedImage);
        
		if(FILE_INDEX == nImage || UNAPPROVED_FILE_INDEX == nImage)
		{
			if(m_ctrlCategoryTree.GetSelectedItem() == hItem)
				*pResult = -1;
		}
	}


}
