// F:\p4\sw\Demo\ContentManagement\Controller\Scheduler\LayoutListPane.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "LayoutManagePage.h"
#include "LayoutListPane.h"
#include "Resource.h"
#include "Localization/Localization.h"
#include "LayoutSettings.h"
#include "LayoutCategoryCtrl.h"
#include "LayoutSearchBar.h"
#include "Sortfuncs.h"
#include <tuple>

UINT LAYOUT_LIST_FILE			= ::RegisterWindowMessage (_T("LAYOUT_LIST_FILE"));

extern UINT BCGM_GRID_BEGINDRAG;
extern UINT BCGM_GRID_ITEM_DBLCLICK;

IMPLEMENT_DYNAMIC(CLayoutListTab, CMFCTabCtrl)

CLayoutListTab::CLayoutListTab()
{
    m_WorkingCategoryID = -1;
}

CLayoutListTab::~CLayoutListTab()
{
}


BEGIN_MESSAGE_MAP(CLayoutListTab, CMFCTabCtrl)
    ON_WM_CREATE()
    ON_NOTIFY(LVN_ITEMCHANGED, IDW_LIST_LAYOUT_LIST, &CLayoutListTab::OnLvnItemchangedListLayout)
    ON_NOTIFY(LVN_BEGINDRAG, IDW_LIST_LAYOUT_LIST, &CLayoutListTab::OnLvnBegindragListLayout)
    ON_NOTIFY(NM_DBLCLK, IDW_LIST_LAYOUT_LIST, &CLayoutListTab::OnNmDblClick)
    //ON_MESSAGE(USER_MSG_GET_LAYOUT_SUB_CATEGORY_FILE, OnUpdateFilesInCategory)
    ON_MESSAGE(USER_MSG_SEARCH_LAYOUT_LIST, OnUpdateFilesInSearch)

	ON_REGISTERED_MESSAGE(BCGM_GRID_BEGINDRAG, OnLvnBeginDragNew )
	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_DBLCLICK, OnNmDblClickNew )

END_MESSAGE_MAP()


int CLayoutListTab::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMFCTabCtrl::OnCreate(lpCreateStruct) == -1)
        return -1;

    RECT rect;
    GetClientRect(&rect);

	m_grid.SetBackgroundTask(g_backgroundTask);
	m_grid.Create(WS_CHILD|WS_VISIBLE|LVS_REPORT, rect, this, IDW_LIST_LAYOUT_LIST);
#define Translate(x) x
	wchar_t* names[] = {
		Translate(_T("Layout ID")),
		Translate(_T("Name")),
		Translate(_T("Description")),
		Translate(_T("Dimension")),
		Translate(_T("Duration")),
#ifdef DB_MYSQL
		Translate(_T("Audit")),
#endif
#ifndef STARTER_EDITION
		Translate(_T("Size")),
#endif
		Translate(_T("Reference Count")),
	};
#undef Translate
	static const int kColumnCount = sizeof(names) / sizeof(*names);
	int idx = 10;
	for (int i = 0; i < kColumnCount; ++i)
	{
		if (names[i] == _T("Name"))
		{
			idx = 40;
		}
		else
		{
			idx = 10;
		}
		m_grid.InsertColumn(i, Translate(names[i]), idx * wcslen(names[i]));
		m_grid.GetColumnsInfo().SetColumnWidthAutoSize(i, true);
	}
	m_grid.SetColumnCanSort(LAYOUT_REFERS,FALSE);
	m_grid.SetCachePageSize(50);
	m_grid.SetVirtualRows(m_grid.GetVirtualRowCount());
	m_grid.EnableDragSelection(TRUE);
	m_grid.EnableDragHeaderItems(TRUE);
	m_grid.SetWholeRowSel(TRUE);

	InsertTab((CWnd*)&m_grid, Translate(_T("Layout List")), 0);
	g_backgroundTask->startTimer(boost::bind(&CLayoutGrid::handle_timeout, &m_grid, _1, _2));


    m_wndSearchList = new CMagicSortListCtrl;
    m_wndSearchList->Create(WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_SINGLESEL, rect, this, IDW_LIST_LAYOUT_LIST);
    m_wndSearchList->SetExtendedStyle(m_wndSearchList->GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    m_wndSearchList->ShowScrollBar(SB_HORZ);

    // double buffer fix flash when do data update
    m_wndSearchList->SetExtendedStyle(m_wndSearchList->GetExtendedStyle()|LVS_EX_DOUBLEBUFFER);

    m_wndSearchList->InsertColumn(LAYOUT_INDEX, Translate(_T("Layout ID")), LVCFMT_LEFT, 80);
	m_wndSearchList->SetCompairFunPair(Translate(_T("Layout ID")),NumberStrComp);

    m_wndSearchList->InsertColumn(LAYOUT_NAME, Translate(_T("Name")), LVCFMT_LEFT, 100);
    m_wndSearchList->InsertColumn(LAYOUT_DESC, Translate(_T("Description")), LVCFMT_LEFT, 120);
	m_wndSearchList->SetCompairFunPair(Translate(_T("Description")),BaseStrComp);

    m_wndSearchList->InsertColumn(LAYOUT_DIMENSION, Translate(_T("Dimension")), LVCFMT_LEFT, 120);
    m_wndSearchList->InsertColumn(LAYOUT_DURATION, Translate(_T("Duration")), LVCFMT_LEFT, 120);
#ifdef SERVER_EDITION
    m_wndSearchList->InsertColumn(LAYOUT_AUDIT, Translate(_T("Audit")), LVCFMT_LEFT,100);
#endif
#ifndef STARTER_EDITION
	m_wndSearchList->InsertColumn(LAYOUT_SIZE, Translate(_T("Size")), LVCFMT_LEFT, 100);
#endif
	m_wndSearchList->InsertColumn(LAYOUT_REFERS, Translate(_T("Reference Count")), LVCFMT_LEFT, 110);
	m_wndSearchList->SetCompairFunPair(Translate(_T("Reference Count")),NumberStrComp);

    m_wndSearchList->ModifyStyle(LVS_SINGLESEL, 0);

    InsertTab(m_wndSearchList, Translate(_T("Search Result")), 1);

	m_LayoutDescCtrl.Create(this);
	m_LayoutDescCtrl.AddTool((CWnd *)&m_grid,Translate(_T("Description")));
	m_LayoutDescCtrl.AddTool((CWnd *)m_wndSearchList,Translate(_T("Description")));
    EnableTabSwap(FALSE);

    return 0;
}

BOOL  CLayoutListTab::PreTranslateMessage(
	MSG* pMsg 
	)
{
	if(pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
	{
		
		CMagicSortListCtrl *pList = CMagicSortListCtrl::ConverPtr(CWnd::FromHandle(pMsg->hwnd));
		if(!pList)
			goto _OUT;
	
		LVHITTESTINFO htItemInf;
		memset(&htItemInf,0,sizeof(LVHITTESTINFO));
		::GetCursorPos(&htItemInf.pt);
		pList->ScreenToClient(&htItemInf.pt);
		
		static int nLastItem = -1;
		int nItem = pList->SubItemHitTest(&htItemInf);
		if(nItem != -1)
		{  
			
				if(htItemInf.iSubItem == LAYOUT_DESC )
				{

					CString strItemText = pList->GetItemText(nItem,htItemInf.iSubItem);
					if(strItemText.GetLength() <= 0)
						goto _OUT;
				
					m_LayoutDescCtrl.SetDescription(strItemText);
					if(m_LayoutDescCtrl.IsWindowVisible() && nLastItem != htItemInf.iItem)
					{
						m_LayoutDescCtrl.Update();
					}
					else
					m_LayoutDescCtrl.RelayEvent(pMsg);
					nLastItem = htItemInf.iItem;
					
					goto _OUT;
				}

		}
		
		HWND hwnd = pMsg->hwnd;
		pMsg->hwnd = this->m_hWnd;
		m_LayoutDescCtrl.RelayEvent(pMsg);
		pMsg->hwnd = hwnd ;
		nLastItem = -1;
		
	}
_OUT:
	return __super::PreTranslateMessage(pMsg);
}

void CLayoutListTab::OnLvnItemchangedListLayout(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
    int iCurrent = pNMLV->iItem;
    int tabIndex = GetActiveTab();

	if(0 == tabIndex)
	{
		INT selectCount = m_grid.GetSelectionCount();
		if (1 == selectCount)
		{
		   // get selected media id
			CBCGPGridRow* currow = m_grid.GetRow(iCurrent);
			CBCGPGridItem* curitem = currow->GetItem(0);
			std::tstring szValue = (TCHAR*)(_bstr_t)curitem->GetValue();
			GetParent()->SendMessage(LAYOUT_LIST_FILE, 0L, (LPARAM)(_tstoi(szValue.c_str())));

			//CString LayoutID = m_wndMainList->GetItemText(iCurrent, 0);

			//send selected message id as a message param
			//GetParent()->SendMessage(LAYOUT_LIST_FILE, 0L, (LPARAM)(_tstoi(LayoutID)));

		}
		else
		{
		   //send message to media info pane to clear media info desc
			GetParent()->SendMessage(LAYOUT_LIST_FILE, 0L, 0L);
		}
	}
	else
	{
		INT selectCount = m_wndSearchList->GetSelectedCount();
		if (1 == selectCount)
		{
			// get selected media id
			CString LayoutID = m_wndSearchList->GetItemText(iCurrent, 0);

			//send selected message id as a message param
			GetParent()->SendMessage(LAYOUT_LIST_FILE, 0L, (LPARAM)(_tstoi(LayoutID)));

		}
		else
		{
			//send message to media info pane to clear media info desc
			GetParent()->SendMessage(LAYOUT_LIST_FILE, 0L, 0L);
		}
	}
}

LRESULT CLayoutListTab::OnNmDblClickNew(WPARAM wParam, LPARAM lParam)
{
	if (!GetControllerInterface->CheckPrivilege(UserPrivilege_PublishLayout))
	{
		MessageBox(Translate(_T("You don't have privilege to move/rename/edit layout.")), Translate(_T("Warning: Edit layout")), MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}

	GetParent()->SendMessage(WM_COMMAND, ID_RINBON_BTN_LAYOUT_EDIT, (WPARAM)this->GetSafeHwnd());
	return 1;
}

void CLayoutListTab::FireChangeActiveTab(int nNewTab)
{
    CMFCTabCtrl::FireChangeActiveTab(nNewTab);

    CWnd* pWnd = GetActiveWnd();
    EnableActiveTabCloseButton(FALSE);
    if (pWnd == &m_grid)
    {
        m_grid.RefreshLayoutGrid();
    }
}

void CLayoutListTab::OnNmDblClick(NMHDR *pNMHDR, LRESULT *pResult)
{
    if (!GetControllerInterface->CheckPrivilege(UserPrivilege_PublishLayout))
    {
        MessageBox(Translate(_T("You don't have privilege to move/rename/edit layout.")), Translate(_T("Warning: Edit layout")), MB_OK|MB_ICONEXCLAMATION);
        return;
    }

    GetParent()->SendMessage(WM_COMMAND, ID_RINBON_BTN_LAYOUT_EDIT, (WPARAM)this->GetSafeHwnd());
}


LRESULT CLayoutListTab::OnLvnBeginDragNew(WPARAM wParam, LPARAM lParam)
{
	if (!GetControllerInterface->CheckPrivilege(UserPrivilege_PublishLayout))
	{
		MessageBox(Translate(_T("You don't have privilege to move/rename/edit layout.")), Translate(_T("Warning: Edit layout")), MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}
	std::map<INT, Layout>* pLayoutList = NULL;
	std::vector<int> selLayoutIDList;
	pLayoutList = &m_LayoutMap;

	m_LayoutMap.clear();
	INT selectCount = m_grid.GetSelectionCount();
	for(int i = 0 ;i < selectCount; i++)
	{
		CBCGPGridRange range;
		if (m_grid.GetSelection (i, range))
		{
			const int nRowOffset = range.m_nTop;
			const int nRowCount = range.m_nBottom - range.m_nTop + 1;

			for (int nRow = 0; nRow < nRowCount; nRow++)
			{
				int nRowIndex = nRow + nRowOffset;
				ASSERT(nRowIndex >= 0);

				CBCGPGridRow * currow = m_grid.GetRow(nRowIndex);

				CBCGPGridItem* curitem = currow->GetItem(0);
				std::tstring szValue = (TCHAR*)(_bstr_t)curitem->GetValue();
				int nValue = StringUtility::stoi(szValue);

				Layout layout;
				HRESULT hr = GetControllerInterface->GetLayout(nValue,layout);
				if (!SUCCEEDED(hr))
				{
					//return E_FAIL;
				}
				else
				{
					selLayoutIDList.push_back(nValue);
					m_LayoutMap[nValue] = layout;
				}
			}
		}
	}
	BOOL bDrag = FALSE;
	try
	{
		CSharedFile globFile;

		bDrag =  SerializeTo(globFile, selLayoutIDList, pLayoutList);

		if (bDrag)
		{
			m_OleDataSource.CacheGlobalData (CLayoutCategoryCtrl::s_LayoutFormat,
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
		return 0;
	}

	DROPEFFECT dropEffect
		= m_OleDataSource.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE);

	m_OleDataSource.Empty();
	return 1;
}

void CLayoutListTab::OnLvnBegindragListLayout(NMHDR *pNMHDR, LRESULT *pResult)
{
    if (!GetControllerInterface->CheckPrivilege(UserPrivilege_PublishLayout))
    {
        MessageBox(Translate(_T("You don't have privilege to move/rename/edit layout.")), Translate(_T("Warning: Edit layout")), MB_OK|MB_ICONEXCLAMATION);
        return;
    }


    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    int iCurrent = pNMLV->iItem;

    int tabIndex = GetActiveTab();
    std::map<INT, Layout>* pLayoutList = NULL;

	std::vector<int> selLayoutIDList;
    //if(tabIndex != 0 && tabIndex != 1) return ;
    if (tabIndex == 0)
    {
        pLayoutList = &m_LayoutMap;
		
		m_LayoutMap.clear();
		INT selectCount = m_grid.GetSelectionCount();
		for(int i = 0 ;i < selectCount; i++)
		{
			CBCGPGridRange range;
			if (m_grid.GetSelection (i, range))
			{
				const int nRowOffset = range.m_nTop;
				const int nRowCount = range.m_nBottom - range.m_nTop + 1;

				for (int nRow = 0; nRow < nRowCount; nRow++)
				{
					int nRowIndex = nRow + nRowOffset;
					ASSERT(nRowIndex >= 0);

					CBCGPGridRow * currow = m_grid.GetRow(nRowIndex);

					CBCGPGridItem* curitem = currow->GetItem(0);
					std::tstring szValue = (TCHAR*)(_bstr_t)curitem->GetValue();
					int nValue = StringUtility::stoi(szValue);

					Layout layout;
					HRESULT hr = GetControllerInterface->GetLayout(nValue,layout);
					if (!SUCCEEDED(hr))
					{
						//return E_FAIL;
					}
					else
					{
						selLayoutIDList.push_back(nValue);
						m_LayoutMap[nValue] = layout;
					}
				}
			}
		}
    }
    else if (tabIndex == 1)
    {
        pLayoutList = &m_SearchLayoutMap;

		POSITION pos = m_wndSearchList->GetFirstSelectedItemPosition();

		//std::vector<int> selLayoutIDList;
		while(pos)
		{
			int nItem = m_wndSearchList->GetNextSelectedItem(pos);
			CString szMediaID = m_wndSearchList->GetItemText(nItem, 0);
			int MediaID = _tstoi(szMediaID);

			auto  it = pLayoutList->find(MediaID);
			if (it == pLayoutList->end())
			{
				return ;
			}

			selLayoutIDList.push_back(MediaID);
		}

    }
	else
	{
		return;
	}

	BOOL bDrag = FALSE;
    try
    {
        CSharedFile globFile;

        bDrag =  SerializeTo(globFile, selLayoutIDList, pLayoutList);

        if (bDrag)
        {
            m_OleDataSource.CacheGlobalData (CLayoutCategoryCtrl::s_LayoutFormat,
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

BOOL CLayoutListTab::SerializeTo (CFile& file, const std::vector<INT>& LayoutIDList, const std::map<INT, Layout>* layoutMap)
{
#if 1 //modified by RaymondLiu
	CString XMLString;

	for(auto iter=LayoutIDList.begin();iter<LayoutIDList.end();++iter)
	{
		auto it = layoutMap->find(*iter);
		if (it == layoutMap->end())
		{
			return FALSE;
		}
		XMLString += it->second.ToXML().c_str();
	}
	CArchive Archive (&file, CArchive::store);

	Archive << XMLString;

	Archive.Close();

   return TRUE;
#endif
}


HRESULT CLayoutListTab::SetLayoutCategory(INT CategoryID)
{
    m_WorkingCategoryID = CategoryID;

    if(CategoryID < S3NET_ROOT_FILTER_ID) return S_OK;
    /*
	SLayoutFilter filter;
	filter.LayoutCategoryID = CategoryID;
	SetFilter(filter);
    */
    return S_OK;
}

HRESULT CLayoutListTab::Refresh()
{
    int tabIndex = GetActiveTab();
    if(0x00 != tabIndex)
    {
        GetParent()->SendMessage(LAYOUT_LIST_SEARCH);
    }
	return 0;
}

HRESULT CLayoutListTab::GetSelectedLayoutList(std::vector<Layout> &SelectedLayoutList)
{
	int tabIndex = GetActiveTab();
	if(0x00 == tabIndex)
	{
		INT selectCount = m_grid.GetSelectionCount();
		for(int i = 0 ;i < selectCount; i++)
		{
			CBCGPGridRange range;
			if (m_grid.GetSelection (i, range))
			{
				const int nRowOffset = range.m_nTop;
				const int nRowCount = range.m_nBottom - range.m_nTop + 1;

				for (int nRow = 0; nRow < nRowCount; nRow++)
				{
					int nRowIndex = nRow + nRowOffset;
					ASSERT(nRowIndex >= 0);

					CBCGPGridRow * currow = m_grid.GetRow(nRowIndex);

					CBCGPGridItem* curitem = currow->GetItem(0);
					std::tstring szValue = (TCHAR*)(_bstr_t)curitem->GetValue();
					int nValue = StringUtility::stoi(szValue);

					Layout layout;
					HRESULT hr = GetControllerInterface->GetLayout(nValue,layout);
					if (!SUCCEEDED(hr))
					{
						//return E_FAIL;
					}
					else
					{
						SelectedLayoutList.push_back(layout);
					}
				}
			}


			/*
			CList <CBCGPGridItem*, CBCGPGridItem*> selItem;
			m_grid.GetSelectedItems(selItem);
			// Enumerate the items in the list.
			POSITION pos = selItem.GetHeadPosition ();
			while (pos != NULL) 
			{
				CBCGPGridItem *item = selItem.GetNext (pos);
				CBCGPGridRow * currow = item->GetParentRow();
				CBCGPGridItem* curitem = currow->GetItem(0);
				std::tstring szValue = (TCHAR*)(_bstr_t)curitem->GetValue();
				int nValue = StringUtility::stoi(szValue);

				Layout layout;
				HRESULT hr = GetControllerInterface->GetLayout(nValue,layout);
				if (!SUCCEEDED(hr))
				{
					return E_FAIL;
				}

				SelectedLayoutList.push_back(layout);
			}
			*/
		}
		return S_OK;
	}
	else
	{
		POSITION pos = m_wndSearchList->GetFirstSelectedItemPosition();

		while(pos)
		{
			int nItem = m_wndSearchList->GetNextSelectedItem(pos);
			CString sLayoutID = m_wndSearchList->GetItemText(nItem, 0);
			int LayoutID = _tstoi(sLayoutID);

			auto it = m_SearchLayoutMap.find(LayoutID);
			if (it == m_SearchLayoutMap.end())
			{
				return E_FAIL;
			}

			SelectedLayoutList.push_back(it->second);
		}
		return S_OK;
	}
}

/*
LRESULT CLayoutListTab::OnUpdateFilesInCategory (WPARAM wp, LPARAM lp)
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

    // keep old selected layout
    int tabIndex = GetActiveTab();
    std::set<int> selectedLayoutId;
    if(0x00 == tabIndex)
    {
        POSITION pos = m_wndMainList->GetFirstSelectedItemPosition();

        while(pos)
        {
            int nItem = m_wndMainList->GetNextSelectedItem(pos);
            CString sLayoutID = m_wndMainList->GetItemText(nItem, 0);
            int LayoutID = _tstoi(sLayoutID);
            selectedLayoutId.insert(LayoutID);
        }
    }

    vector<tuple<Layout, vector<INT>, vector<INT> > > LayoutList = MessageBuf.Get()->second.second;

    m_wndMainList->DeleteAllItems();
    m_LayoutMap.clear();

    //insert media info in category list
    auto it = LayoutList.begin(), ite = LayoutList.end();
    int i = 0;

    for (; it != ite; ++it, ++i)
    {
        m_LayoutMap[std::get<0>(*it).GetLayoutID()] = std::get<0>(*it);

        CString szLayoutID;
        szLayoutID.Format(_T("%d"), std::get<0>(*it).GetLayoutID());

        CString szDuration;
        COleDateTimeSpan tmpDur;
        tmpDur.SetDateTimeSpan(0,0,0,std::get<0>(*it).GetDuration());
        szDuration = tmpDur.Format(_T("%H:%M:%S"));

        CString szDimenstion;
        szDimenstion.Format(_T("%s x %s"),
            std::get<0>(*it).GetCustomizedSetting((LPCTSTR)szLayoutWidth).c_str(),
            std::get<0>(*it).GetCustomizedSetting((LPCTSTR)szLayoutHeight).c_str());

        CString tmpName = std::get<0>(*it).GetLayoutName().c_str();
        tmpName.Replace(_T("&"), _T("&&"));
        if (tmpName.Right(1) == '%')
        {
            tmpName.AppendChar(_T(' '));
        }
        m_wndMainList->InsertItem(i, (LPCTSTR)szLayoutID);
        m_wndMainList->SetItemText(i, LAYOUT_NAME, tmpName);
        m_wndMainList->SetItemText(i, LAYOUT_DESC, std::get<0>(*it).GetCustomizedSetting((LPCTSTR)szLayoutDesc).c_str());
        m_wndMainList->SetItemText(i, LAYOUT_DIMENSION, szDimenstion);
        m_wndMainList->SetItemText(i, LAYOUT_DURATION, szDuration);
#ifdef SERVER_EDITION
        m_wndMainList->SetItemText(i, LAYOUT_AUDIT, Translate(std::get<0>(*it).GetAudit().GetStatusStr().c_str()));
#endif
        m_wndMainList->SetItemText(i, LAYOUT_REFERS, StringUtility::itos(std::get<1>(*it).size() + std::get<2>(*it).size()).c_str());
		
		CString layoutsize = std::get<0>(*it).GetCustomizedSetting((LPCTSTR)szLayoutSize).c_str();
		DWORD64 dwValue = 0;
		for(int ii = 0; ii < layoutsize.GetLength(); ii++)
		{
			dwValue *= 10;
			if(layoutsize[ii] >= 0x30 && layoutsize[ii] <= 0x39)
			{
				dwValue += (layoutsize[ii]-0x30);
			}
		}
		
		double num1 = (dwValue * 1.0) ;
		if(dwValue > 0)
		{
			num1 /= (1024*1024);
		}
		num1 += 1;
		layoutsize.Format(_T("%.1f Mb"),num1);
#ifndef STARTER_EDITION
		m_wndMainList->SetItemText(i, LAYOUT_SIZE, layoutsize);
#endif


        if (selectedLayoutId.find(std::get<0>(*it).GetLayoutID()) != selectedLayoutId.end())
        {
            m_wndMainList->SetItemState(i, 0xffffffff, LVIS_SELECTED);
        }
    }

	m_wndMainList->Resort();
    GetParent()->SendMessage(LAYOUT_LIST_FILE);
    return 0;

}
*/

LRESULT CLayoutListTab::OnUpdateFilesInSearch (WPARAM wp, LPARAM lp)
{

    //////////////////////////////////////////////////////////////////////////
    // How to get a buf
    UINT nMessageBufID = (UINT)wp;

    DataBuf<vector<pair<Layout, INT > > > MessageBuf;
    if (!WorkThreadDataManager< vector<pair<Layout, INT > > >
        ::GetDataBuf(nMessageBufID, MessageBuf))
    {
        return 1;
    }

    vector<pair<Layout, INT > > LayoutList = *MessageBuf.Get();

    m_wndSearchList->DeleteAllItems();

    m_SearchLayoutMap.clear();

///////professional edition, judge size 
#ifdef  PROFESSIONAL_EDITION
        long ft = -1;
        long sd = -1;
        CLayoutSearchBarCtrl *pWnd = (CLayoutSearchBarCtrl*)
                            SearchWndObj(AfxGetMainWnd(),RUNTIME_CLASS(CLayoutSearchBarCtrl));
        if(pWnd)
        {
             CComboBox* pCBsize  = (CComboBox*)(&pWnd->m_ctlSize);
             int SizeId = pCBsize->GetItemData(pCBsize->GetCurSel());
             if (SizeId >= 0)
              {
                switch(SizeId)
                {
                case 0:
                    {
                         ft = 1;
                         sd =10*1024*1024;
                    }
                    break;
                case 1:
                    {
                        ft = 10*1024*1024;
                        sd = 16*1024*1024;
                    }
                    break;
                case 2:
                    {
                        ft = 16*1024*1024;
                        sd = 128*1024*1024;
                    }
                    break;
                case 3:
                    {
                        sd = 128*1024*1024;
                    }
                    break;
                default:
                    break;
                }
             }
       }

#endif

    //insert media info in category list

    auto it = LayoutList.begin(), ite = LayoutList.end();
    int iIndex = 0;

    for (; it != ite; ++it)
    {
		if(it->first.GetLayoutID() < 0) continue;

        CString szLayoutID;
        szLayoutID.Format(_T("%d"), it->first.GetLayoutID());

        //////////////////////////////////
		CString layoutsize;
		layoutsize.Format(_T("%lld"), it->first.GetLayoutSize());
		DWORD64 dwValue = 0;
		for(int ii = 0; ii < layoutsize.GetLength(); ii++)
		{
			dwValue *= 10;
			if(layoutsize[ii] >= 0x30 && layoutsize[ii] <= 0x39)
			{
				dwValue += (layoutsize[ii]-0x30);
			}
		}
		
		double num1 = (dwValue * 1.0) ;
		num1 += 1024*1024;

#ifdef PROFESSIONAL_EDITION
    if(ft < 0)
    {
        if(num1 <= sd)
        continue;
    }      
    else if(ft == 1)
    {
        if(num1 > sd)
        continue;
    }
    else
    {
        if(num1 <= ft || num1 >= sd)
        continue;
    }     
#endif
        if(dwValue >= 0)
		{
			num1 /= (1024*1024);
		}
		layoutsize.Format(_T("%.1f Mb"),num1);
        //////////////////////////////////////////////
        m_SearchLayoutMap[it->first.GetLayoutID()] = it->first;

        CString szDuration;
        COleDateTimeSpan tmpDur;
        tmpDur.SetDateTimeSpan(0,0,0,it->first.GetDuration());
        szDuration = tmpDur.Format(_T("%H:%M:%S"));

        CString szDimenstion;
		szDimenstion.Format(_T("%d x %d"), it->first.GetWidth(), it->first.GetHeight());
        m_wndSearchList->InsertItem(iIndex, (LPCTSTR)szLayoutID);
#ifndef STARTER_EDITION
		m_wndSearchList->SetItemText(iIndex, LAYOUT_SIZE, layoutsize);
#endif
        m_wndSearchList->SetItemText(iIndex, LAYOUT_NAME, it->first.GetLayoutName().c_str());
		m_wndSearchList->SetItemText(iIndex, LAYOUT_DESC, it->first.GetDescription().c_str());
        m_wndSearchList->SetItemText(iIndex, LAYOUT_DIMENSION, szDimenstion);
        m_wndSearchList->SetItemText(iIndex, LAYOUT_DURATION, szDuration);
#ifdef SERVER_EDITION
        m_wndSearchList->SetItemText(iIndex, LAYOUT_AUDIT, Translate(it->first.GetAudit().GetStatusStr().c_str()));
#endif
		m_wndSearchList->SetItemText(iIndex, LAYOUT_REFERS, StringUtility::itos(it->second).c_str());
		/* By RaymondLiu
	    layoutsize = it->first.GetCustomizedSetting((LPCTSTR)szLayoutSize).c_str();
		dwValue = 0;
		for(int ii = 0; ii < layoutsize.GetLength(); ii++)
		{
			dwValue *= 10;
			if(layoutsize[ii] >= 0x30 && layoutsize[ii] <= 0x39)
			{
				dwValue += (layoutsize[ii]-0x30);
			}
		}
		
		num1 = (dwValue * 1.0) ;
		if(dwValue > 0)
		{
			num1 /= (1024*1024);
		}
		num1 += 1;
		layoutsize.Format(_T("%.1f Mb"),num1);
		m_wndSearchList->SetItemText(iIndex, LAYOUT_SIZE, layoutsize);*/

        iIndex ++;
    }

	m_wndSearchList->Resort();

    CString InfoStatus("");
    InfoStatus.Format(Translate(_T("Search completed, the result records: %d")), iIndex);
    GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)(InfoStatus.GetBuffer()));
    return 0;

}

const wchar_t* pLayoutColFields[] = {
	_T("LayoutId"),
	_T("LayoutName"),
	_T("Description"),
	_T("Dimension"),
	_T("Duration"),
#ifdef SERVER_EDITION
	_T("Audit"),
#endif
#ifndef STARTER_EDITION
	_T("LayoutSize"),
#endif
	_T("Reference Count"),
};
//////////////////////////////////////////////////////////////////////////
sort_and_filter_t CLayoutGrid::BuildSort()
{
	sort_and_filter_t sf;
	std::tstring field;
	int n = m_Columns.GetColumnCount();
	for (int i = 0; i < n; ++i)
	{
		//CString szColumnName = m_Columns.GetColumnName(i);
		std::tstring colName = pLayoutColFields[i];
		int s = m_Columns.GetColumnState(i);

		field = colName;

		if ( _T("Dimension") == colName && s == -1)
		{
			field = _T("Width desc, Height");
		}
		else if ( _T("Dimension") == colName)
		{
			field = _T("Width, Height");
		}
#ifdef SERVER_EDITION
		if (_T("Audit") == colName)
		{
			field = _T("Status");
		}
#endif
		if( _T("Reference Count") == colName)
		{
			field = _T("");
		}

		if (field != _T(""))
		{
			int s = m_Columns.GetColumnState(i);
			if (s == 1)
			{
				sf.AddSort(/*m_Columns.GetColumnName(i)*/field, 1);
			}
			else if (s == -1)
			{
				sf.AddSort(/*m_Columns.GetColumnName(i)*/field, -1);
			}
			else
			{
				sf.AddSort(/*m_Columns.GetColumnName(i)*/field, 0);
			}
		}
	}
	
	sf.SetFilter(GetFilter());
	return sf;
}

void CLayoutGrid::SetFilter(const SLayoutFilter& filter)
{
	boost::recursive_mutex::scoped_lock lock(m_filterMutex);
	CWaitCursor cursor;
	m_layoutFilter = filter;
	UpdateAllData();
}


SLayoutFilter CLayoutGrid::GetFilter()
{
	boost::recursive_mutex::scoped_lock lock(m_filterMutex);
	return m_layoutFilter;
}

void CLayoutGrid::RefreshLayoutGrid()
{
	CWaitCursor cursor;
	SLayoutFilter filter;
	filter.setCategoryId(m_layoutFilter.LayoutCategoryID);
	filter.setCount(GetVirtualRowCount());
	filter.setMaxId(GetMaxRowId());
	SetFilter(filter);
}

unsigned long CLayoutGrid::GetMaxRowId()
{
	std::tstring sql = _T("select count(*) as c, max(layoutid) as maxId from layout Where status <> @AUDIT_DELETED@ ");
	sql += GetFilter().ToString();

	ADODB::_RecordsetPtr rs;
	if(S_OK != GetControllerInterface->QuerySQL(sql, rs))
		return 0;

	rs->MoveFirst();
	unsigned long ct = rs->Fields->Item["c"]->Value;
	if (ct == 0)
	{
		return 0;
	}
	else
		return rs->Fields->Item["maxId"]->Value;
}

int CLayoutGrid::GetSelectRowLayoutID()
{
	m_pSelRow = GetCurSel();
	if (!m_pSelRow)
	{
		return -1;
	}
	CBCGPGridItem* pName = m_pSelRow->GetItem(0);
	_variant_t id;
	if (pName)
	{
		id = pName->GetValue();
	}
	if (id.vt == VT_BSTR)
	{
		return -1;
	}
	else
		return id;
}

void CLayoutGrid::OnSelChanged(const CBCGPGridRange &range, BOOL bSelect)
{

	CBCGPGridCtrl::OnSelChanged(range, bSelect);
	CWnd * p = AfxGetMainWnd();
	if (p != NULL)
	{
		CWnd *pWnd = SearchWndObj(p,RUNTIME_CLASS(CLayoutManagePage));
		CLayoutManagePage* pPage= (CLayoutManagePage*)pWnd;
		int id = GetSelectRowLayoutID();
		if (id != -1)
		{
			// pPage->SendMessage(MEDIA_SHOW_INFO, 0L, (LPARAM)(id));
		}	
	}
}

int CLayoutGrid::JudgeReferCount()
{
	if (!m_lstSel.IsEmpty())
	{
		POSITION pos = m_lstSel.GetHeadPosition ();
		if (!pos)
		{
			return 1;
		}
		CBCGPGridRange* pSelRange = m_lstSel.GetNext (pos);
		ASSERT (pSelRange != NULL);
		CBCGPGridRow* pTopRow = (pSelRange->m_nTop != -1) ? GetRow (pSelRange->m_nTop) : NULL;
#if defined(SERVER_EDITION)
	int idex = 7;
#elif defined(PROFESSIONAL_EDITION)
	int idex = 6;
#else
	int idex = 5;
#endif
		if (m_pSelRow)
		{
			CBCGPGridItem *pIt = m_pSelRow->GetItem(idex);
			_variant_t v = pIt->GetValue();
			return v;
		}	
	}
	return 1;
}

void CLayoutGrid::OnTimerRefresh()
{
    try
    {
        unsigned long maxId = GetMaxRowId();
        int count = GetVirtualRowCount();
        Refresh_TH(count, maxId);
    }
    catch (NetworkException&)
    {	
    } 
}



void CLayoutGrid::handle_timeout(const boost::system::error_code& ec, boost::shared_ptr<boost::asio::deadline_timer> timer)
{
	if (ec)
		return;

	CString szRefreshDuration;
	GetConfig->Get(CLIENT_MONITOR_REFRESH, szRefreshDuration);

	if (IsWindowVisible())
	{
		OnTimerRefresh();
	}
	timer->expires_from_now(boost::posix_time::seconds( StringUtility::stoi(szRefreshDuration.GetString()) ));
	timer->async_wait(boost::bind(&CLayoutGrid::handle_timeout, this, boost::asio::placeholders::error, timer));
}

void CLayoutGrid::Refresh(int count, unsigned long maxId)
{
	if (count != m_nVirtualRows || maxId != m_layoutFilter.maxId)
	{
		RefreshLayoutGrid();
	}
	else
	{
		ForceUpdateOneRecord(m_idActive);
	}
}

int CLayoutGrid::GetVirtualRowCount()
{
	std::tstring sql = _T("select count(*) as c from Layout where status <> @AUDIT_DELETED@ ");
	//sql += ((CLayoutListTab*)GetParent())->GetFilter().ToString();
	sql += GetFilter().ToString();
	//sql += m_sortClause;

	ADODB::_RecordsetPtr rs;
	try
	{
		if(S_OK != GetControllerInterface->QuerySQL(sql, rs))
			return 0;
	}
	catch(...)
	{
		return 0;
	}

	rs->MoveFirst();
	return rs->Fields->Item["c"]->Value;
}

TCHAR* AuditStatus2[] = {
	_T("REQUESTED"),
	_T("REJECTED"),
	_T("APPROVED"),
	_T("RETIRED"),
	_T("DELETED")
};


void CLayoutGrid::GetData(int start, int end, const sort_and_filter_t& sf, std::map<int, std::vector<_variant_t> >& data){
	std::map<int, std::vector<_variant_t> > temp;
	CString limit;
	std::tstring sql, clause;

	int idxStatus = -1;
	int idxRefers = -1;
	int idxSize = -1;
    int idxDur = 4;
#if defined(DB_MYSQL) 
	idxStatus = 5;
	idxRefers = 7;
	idxSize = 6;
#elif defined(DB_SQLITE)
#ifdef STARTER_EDITION
	idxRefers = 5;
#else
	idxSize = 5;
	idxRefers = 6;
#endif;
#endif;

#if defined(DB_MYSQL)
	sql = _T("select LayoutId, LayoutName, Description ,CONCAT(cast(width as char),'*', cast(height as char))as Dimension, Duration ,Status,  LayoutSize, mediaCount from layout where status <> @AUDIT_DELETED@ ");
#elif defined(DB_SQLITE)
#ifndef STARTER_EDITION
	sql = _T("select LayoutId, LayoutName, Description , (cast(width as char)|| '*'|| cast(height as char)) as Dimension, Duration , LayoutSize, MediaCount from layout where status <> @AUDIT_DELETED@ ");
#else
	sql = _T("select LayoutId, LayoutName, Description , (cast(width as char)|| '*'|| cast(height as char)) as Dimension, Duration , MediaCount from layout where status <> @AUDIT_DELETED@ ");//access
#endif;
#endif;
	SLayoutFilter filter;
	filter = boost::any_cast<SLayoutFilter>(sf.GetFilter());
	sql += filter.ToString();

	clause = _T(" order by ");

	// sort clause
	sort_and_filter_t::sorts_t sorts;
	sorts = sf.GetSorts();
	int n = sorts.size();
	int i = 0;
	bool bFirst = true;
	bool bSort = false;

#ifndef DB_ACCESS
	for (i = 0; i < n; ++i)
	{
		int s = std::get<1>(sorts[i]);
		if (!bFirst && s != 0)
		{
			clause += _T(", ");
		}


		if (s == 1)
		{
			clause += std::get<0>(sorts[i]);
			clause += _T(" ASC");
			bSort = true;
			bFirst = false;
		}
		else if (s == -1)
		{
			clause += std::get<0>(sorts[i]);
			clause += _T(" DESC");
			bSort = true;
			bFirst = false;
		}
		else
		{
			continue;
		}

	}
	if (bSort > 0)
	{
		sql += clause;
	}

	limit.Format(_T(" limit %d, %d"), start, end - start);
	sql += limit.GetString();
#else
	sql =_T("");//limit.Format(_T(" between %d and %d"), start, end - 1); // need modify access 
#endif

	ADODB::_RecordsetPtr rs;
	ADODB::_RecordsetPtr rs1;
	if (S_OK != GetControllerInterface->QuerySQL(sql, rs))
		return;

	n = start;

	while (!rs->EndOfFile)
	{
		std::tstring szLayoutId = (TCHAR*)(_bstr_t)rs->Fields->GetItem(_T("LayoutId"))->GetValue();
		int iLayoutId = StringUtility::stoi(szLayoutId);
		std::vector<_variant_t> row;
		for(short i = 0; i < rs->Fields->Count; ++i)
		{
            CString szStr;
			if(idxStatus > 0 && i == idxStatus)
			{
                szStr = Translate(AuditStatus2[long(rs->Fields->Item[i]->Value)]);
				row.push_back((_variant_t)(_bstr_t)szStr.GetString());
			}
            else if (i == idxDur)
            {
                CString szDuration;
                CTimeSpan tmpDur(unsigned __int64(rs->Fields->GetItem(_T("Duration"))->GetValue()));
                //Total days: %D, hours: %H, mins: %M, secs: %S
                if (tmpDur.GetDays() > 0)
                {
                    long hours = tmpDur.GetTotalHours();

                    szDuration = tmpDur.Format(_T("%M:%S"));//%D:%H:%M:%S
                    CString sz;
                    sz.Format(_T("%d:"),hours);
                    szDuration = sz + szDuration;
                }
                else
                {
                    szDuration = tmpDur.Format(_T("%H:%M:%S"));
                }
                row.push_back((_variant_t)(_bstr_t)szDuration.GetString());
            }
			else if (idxSize > 0 && i == idxSize)
			{
				std::tstring strLayoutSize = (TCHAR*)(_bstr_t)rs->Fields->GetItem(_T("LayoutSize"))->GetValue();
				DWORD64 dwValue = 0;
				for(int ii = 0; ii < strLayoutSize.length(); ii++)
				{
					dwValue *= 10;
					if(strLayoutSize[ii] >= 0x30 && strLayoutSize[ii] <= 0x39)
					{
						dwValue += (strLayoutSize[ii]-0x30);
					}
				}

				double num1 = (dwValue * 1.0) ;
				if(dwValue > 0)
				{
					num1 /= (1024*1024);
				}
				num1 += 1;
				CString layoutsize;
				layoutsize.Format(_T("%.1f Mb"),num1);
				row.push_back((_variant_t)(_bstr_t)layoutsize.GetString());
			}
			else if (idxRefers > 0 && i == idxRefers) //layout refers replace of mediacount
			{
				CString tsql;
				tsql.Format(_T("select m1.c1+m2.c2+m3.c3 as c FROM (SELECT count(s.scheduleitemid) as c1 FROM ScheduleItem s, layout l  WHERE s.EndTime >'%s' and l.LayoutId = s.ItemId and s.ItemId = %d and ")               
					_T("s.ItemType = 0 and s.Status <> @AUDIT_DELETED@ and l.Status <> @AUDIT_DELETED@) m1,  (select count(*) as c2 from ClientGroup  where DefaultLayoutId = 1 and Status <> @AUDIT_DELETED@) m2,")
					_T("(select count(te.TimetableId) as c3 from TimetableEntry te, Timetable t where te.LayoutId = %d and te.TimetableId = t.TimetableId and t.Status <> @AUDIT_DELETED@) m3"),S3Time::Now().ToString().c_str(), iLayoutId, iLayoutId);
				
				std::tstring sql1 = tsql.GetString();
				
				HRESULT hr1 = GetControllerInterface->QuerySQL(sql1, rs1);
				if (S_OK != hr1)
				{
					row.push_back((_variant_t)(_bstr_t)_T(""));
					break;
				}
				else{
					while(!rs1->EndOfFile)
						{
							row.push_back(rs1->Fields->Item["c"]->Value);
							break;
						}
				}
				/*
				S3Time TempTime(1900,1,1,1,1,1);
				vector<int> Groups; 
				std::vector<int> ClientGroupIDList;
				std::vector<int> refTimetableIDList;
				HRESULT hr = GetControllerInterface->CheckLayoutRef(iLayoutId, TempTime, Groups, ClientGroupIDList ,refTimetableIDList);
				if (!SUCCEEDED(hr))
				{
					//TRACE1("Failed to get layout ref %d\n", iLayoutId);
					row.push_back((_variant_t)(_bstr_t)_T(""));
					break;
				}
				else
				{
					std::tstring szRefer;
					szRefer = StringUtility::itos(Groups.size() + ClientGroupIDList.size() + refTimetableIDList.size() );
					row.push_back((_variant_t)(_bstr_t)szRefer.c_str());
				}
				*/
			}
			else
			{
				row.push_back(rs->Fields->Item[i]->Value);
			}
		}
		temp[n] = row;
		++n;
		rs->MoveNext();
	}
	data.swap(temp);
}

CLayoutGrid::~CLayoutGrid()
{

}



// CLayoutTemplates

IMPLEMENT_DYNAMIC(CLayoutListPane, CBaseControllerPane)

CLayoutListPane::CLayoutListPane()
{

}

CLayoutListPane::~CLayoutListPane()
{
}


BEGIN_MESSAGE_MAP(CLayoutListPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



// CLayoutListPane message handlers
int CLayoutListPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;


    RECT rect;
    GetClientRect(&rect);

    m_wndLayoutListTab.Create(CMFCTabCtrl::STYLE_3D_VS2005, rect, this, IDW_LIST_LAYOUT_LIST);
    m_wndLayoutListTab.SetLocation(CMFCBaseTabCtrl::LOCATION_TOP);
    m_wndLayoutListTab.EnableActiveTabCloseButton(FALSE);
    m_wndLayoutListTab.AutoDestroyWindow(TRUE);

    SetMainWnd(&m_wndLayoutListTab);

    return 0;
}


void CLayoutListPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

 



