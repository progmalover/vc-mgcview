#ifndef _CLISTCTRLEX_H_
#define _CLISTCTRLEX_H_

#ifdef _UNICODE
#define S3S2I(X)  _wtoi(X)
#define S3S2I64(X)  _wtoi64(X)
#else
#define S3S2I(X)  atoi(X)
#define S3S2I64(X)  _wtoi64(X)
#endif

class CListCtrlEx : public CMFCListCtrl
{
//	typedef CListCtrl inherited;

	DECLARE_DYNCREATE(CListCtrlEx)

// Construction
public:
	CListCtrlEx();
	virtual ~CListCtrlEx();

// Overrides
	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CFontCombo)     
	public:
	virtual void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
//	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
//	void Init();
	COLORREF m_clrText;
	COLORREF m_clrBkgnd;
	COLORREF m_clrHText;
	COLORREF m_clrHBkgnd;
	COLORREF m_clrPercent;
    COLORREF m_clrHPercent;
	BOOL GetCellRect(int iRow, int iCol, int nArea, CRect &rect);

// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlEx)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
