#pragma once
#include "MagicSortListCtrl.h"
extern UINT LAYOUT_LIST_FILE;
extern UINT LAYOUTSEARCH_BUTTON;

#include "AsyncGrid.h"
#include <boost/format.hpp>

struct SLayoutFilter {
	
	int LayoutCategoryID;
	unsigned long count;
	unsigned long maxId;

	SLayoutFilter() : LayoutCategoryID(-1),count(0),maxId(0)
	{

	}
	void setCount(unsigned long cnt)
	{
		count = cnt;
	}
	void setCategoryId(int cgrId)
	{
		LayoutCategoryID = cgrId;
	}
	void setMaxId(int md)
	{
		maxId = md;
	}
	SLayoutFilter& operator = (const SLayoutFilter& o)
	{
		if (&o != this)
		{
			LayoutCategoryID = o.LayoutCategoryID;
			count = o.count;
			maxId = o.maxId;
		}
		return *this;
	}

	std::tstring ToString() const
	{
		std::tstring str;
		str = _T(" and LayoutFilterId = ");
		str += StringUtility::itos(LayoutCategoryID).c_str();
		return str;
	}
};

class CLayoutGrid : public CAsyncGrid2
{
public:
	virtual ~CLayoutGrid();

	virtual void GetData(int start, int end, const sort_and_filter_t& sf, std::map<int, std::vector<_variant_t> >& data);
	virtual int GetVirtualRowCount();
	virtual sort_and_filter_t BuildSort();
	void	RefreshLayoutGrid();
	unsigned long GetMaxRowId();
	int		GetSelectRowLayoutID();
	virtual void OnSelChanged(const CBCGPGridRange &range, BOOL bSelect);
	int JudgeReferCount();
	void SetFilter(const SLayoutFilter& filter);
	SLayoutFilter  GetFilter();
	void OnTimerRefresh();
	void handle_timeout(const boost::system::error_code& ec, boost::shared_ptr<boost::asio::deadline_timer> timer);

private:
	virtual void Refresh(int count, unsigned long maxId);
	boost::recursive_mutex	m_filterMutex;
	SLayoutFilter			m_layoutFilter;
};


class CLayoutListTab : public CMFCTabCtrl
{
    DECLARE_DYNAMIC(CLayoutListTab)

	CLayoutGrid m_grid;
    //CMagicSortListCtrl*       m_wndMainList;
    CMagicSortListCtrl*       m_wndSearchList;
public:
    CLayoutListTab();
    virtual ~CLayoutListTab();

        enum{
        LAYOUT_INDEX = 0,
        LAYOUT_NAME,
        LAYOUT_DESC,
        LAYOUT_DIMENSION,
        LAYOUT_DURATION,
#ifdef SERVER_EDITION
        LAYOUT_AUDIT,
#endif
#ifndef STARTER_EDITION
		LAYOUT_SIZE,
#endif	
		LAYOUT_REFERS,
	
    };

    HRESULT SetLayoutCategory(INT CategoryID);
    HRESULT Refresh();
    HRESULT GetSelectedLayoutList(std::vector<Layout> &SelectedLayoutList);

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLvnItemchangedListLayout(NMHDR *pNMHDR, LRESULT *pResult);
    
	afx_msg void OnLvnBegindragListLayout(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNmDblClick(NMHDR *pNMHDR, LRESULT *pResult);
	
	LRESULT OnLvnBeginDragNew(WPARAM wParam, LPARAM lParam);
	LRESULT OnNmDblClickNew(WPARAM wParam, LPARAM lParam);

     virtual void FireChangeActiveTab(int nNewTab);

    //afx_msg LRESULT OnUpdateFilesInCategory (WPARAM wp, LPARAM lp);
    afx_msg LRESULT OnUpdateFilesInSearch (WPARAM wp, LPARAM lp);
	virtual BOOL  PreTranslateMessage( MSG* pMsg );
    DECLARE_MESSAGE_MAP()

    //BOOL    SerializeTo (CFile& file, INT LayoutID);
	BOOL SerializeTo (CFile& file, const std::vector<INT>& LayoutIDList, const std::map<INT, Layout>* layoutMap);

    INT                     m_WorkingCategoryID;

    std::map<INT, Layout>   m_LayoutMap;
	std::map<INT, Layout>   m_SearchLayoutMap;
    COleDataSource          m_OleDataSource;
	CMFCToolTipCtrl			m_LayoutDescCtrl;
};



// CLayoutListPane

class CLayoutListPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CLayoutListPane)

public:
    CLayoutListPane();
    virtual ~CLayoutListPane();
    CLayoutListTab   m_wndLayoutListTab;   
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()



public:
    afx_msg void OnDestroy();
    

};


