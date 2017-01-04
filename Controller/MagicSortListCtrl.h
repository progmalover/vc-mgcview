#pragma once


// CMagicSortListCtrl
#include "ListCtrlex.h"
#include <map>
#include "Sortfuncs.h"
using namespace std;

class CMagicSortListCtrl : public CListCtrlEx
{
	DECLARE_DYNAMIC(CMagicSortListCtrl)
private:
	class CSortFunctions
	{
		friend class CMagicSortListCtrl;
		typedef int (*COMPARE_LISTITEM_FUN)(CString ,CString);
		typedef map<CString ,COMPARE_LISTITEM_FUN> SORT_FUNS;

	public:
		CSortFunctions(){};
		~CSortFunctions(){};

		COMPARE_LISTITEM_FUN GetSortFunc(CString strColName);
		bool AddFuncPair(CString,COMPARE_LISTITEM_FUN);
		int	 GetFuncsNumber();
	private:
		SORT_FUNS m_SortFunsMap;
	} ;
	static CSortFunctions m_xFuns;
public:
	CMagicSortListCtrl();
	virtual ~CMagicSortListCtrl();
	// sort function
	virtual void Resort();
	int			 InsertItem( int iItem ,LPCTSTR strItem);
	bool		 SetCompairFunPair(LPCTSTR strkey,BaseCompairFunc func);
protected:
	DECLARE_MESSAGE_MAP()
protected:
	int FindParamItem(CMagicSortListCtrl *plist, LPARAM lparam );
	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn);
	virtual BOOL InitSortFuncs();
	virtual void OnCustomDraw(NMHDR *,LRESULT *);
public:
	static CMagicSortListCtrl * ConverPtr(CWnd *);
public:
	afx_msg void OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

};


