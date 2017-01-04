// PublishLayoutDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "PublishLayoutDialog.h"
#include "afxdialogex.h"

// CPublishLayoutDialog dialog

IMPLEMENT_DYNAMIC(CPublishLayoutDialog, CDialogEx)

CPublishLayoutDialog::CPublishLayoutDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPublishLayoutDialog::IDD, pParent)
	, m_CurCategoryID(-1)
{

}

CPublishLayoutDialog::~CPublishLayoutDialog()
{
}

void CPublishLayoutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CATEGORY_TREE_NEW, m_ctrlCategoryTree);
}


BEGIN_MESSAGE_MAP(CPublishLayoutDialog, CDialogEx)

	ON_NOTIFY(TVN_SELCHANGED, IDC_CATEGORY_TREE_NEW, &CPublishLayoutDialog::OnSelchangedFilterTree)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_CATEGORY_TREE_NEW, &CPublishLayoutDialog::OnItemexpandingFilterTree)

	ON_NOTIFY(NM_DBLCLK, IDC_CATEGORY_TREE_NEW, &CPublishLayoutDialog::OnDblclkFilterTree)

	ON_MESSAGE(USER_MSG_GET_LAYOUT_ROOT_CATEGORY, OnUpdateRootCategory)
	ON_MESSAGE(USER_MSG_GET_LAYOUT_SUB_CATEGORY, OnUpdateSubCategory)

	ON_BN_CLICKED(IDOK, &CPublishLayoutDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CPublishLayoutDialog message handlers
BOOL CPublishLayoutDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPublishLayoutDialog::OnSelchangedFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
{

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

}


void CPublishLayoutDialog::OnItemexpandingFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
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

				DWORD MessageID = USER_MSG_GET_LAYOUT_SUB_CATEGORY;
				
				if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,
					(WPARAM)this->GetSafeHwnd(), LPARAM(CategoryID)))//post thread msg
				{
					TRACE1("post message failed,errno:%d\n",::GetLastError());
				}
			}
		}

		return;
	}
}

HTREEITEM CPublishLayoutDialog::FindCategoryItem(HTREEITEM item, INT ID)
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

void CPublishLayoutDialog::DeleteChildrens(HTREEITEM item)
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

void CPublishLayoutDialog::OnDblclkFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

BOOL CPublishLayoutDialog::InitRootCategory()
{
	HRESULT hr = S_OK;
	MediaFilter root;
	DWORD MessageID = USER_MSG_GET_LAYOUT_ROOT_CATEGORY;

	if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,
		(WPARAM)this->GetSafeHwnd(), NULL))//post thread msg
	{
		TRACE1("post message failed,errno:%d\n",::GetLastError());
	}

	return TRUE;
}

LRESULT CPublishLayoutDialog::OnUpdateRootCategory (WPARAM wp, LPARAM lp)
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

	const LayoutGroup& RootCategory = *MessageBuf.Get();
	m_RootCategoryID = RootCategory.GetGroupID();

	m_ctrlCategoryTree.DeleteAllItems();

	HTREEITEM root = m_ctrlCategoryTree.InsertItem(Translate(_T("Layout Root Category")), CATEGORY_INDEX, CATEGORY_INDEX);

	m_ctrlCategoryTree.SetItemData(root, RootCategory.GetGroupID());

	if (RootCategory.GetHaveChildrenFilter())
	{
		m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, root);
	}

	m_ctrlCategoryTree.Expand(root, TVE_EXPAND);

	return 0;
}

LRESULT CPublishLayoutDialog::OnUpdateSubCategory (WPARAM wp, LPARAM lp)
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
	if (Item)
	{
		DeleteChildrens(Item);

		const vector<LayoutGroup>& SubCategoryList = MessageBuf.Get()->second;

		for (auto it = SubCategoryList.begin(); it != SubCategoryList.end(); it++)
		{
			HTREEITEM item = m_ctrlCategoryTree.InsertItem(it->GetGroupName().c_str(), CATEGORY_INDEX, CATEGORY_INDEX, Item);
			m_ctrlCategoryTree.SetItemData(item, it->GetGroupID());

			if (it->GetHaveChildrenFilter())
			{
				m_ctrlCategoryTree.InsertItem(UPDATING, CATEGORY_INDEX, CATEGORY_INDEX, item);
			}

			m_LayoutGroupMap[it->GetGroupID()] = *it;
		}
	}

	return 0;
}

int CPublishLayoutDialog::GetCurCategoryID(void)
{
	return m_CurCategoryID;
}

void CPublishLayoutDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	HTREEITEM item = m_ctrlCategoryTree.GetSelectedItem();
	if(item == NULL)
	{
		MessageBox(Translate(_T("Please select a category!")), Translate(_T("Warning:Publish layout")),MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	m_CurCategoryID = m_ctrlCategoryTree.GetItemData(item);

	if(m_CurCategoryID == -1)
	{
		MessageBox(Translate(_T("Please select a category!")),Translate(_T("Warning:Publish layout")), MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	CDialogEx::OnOK();
}
