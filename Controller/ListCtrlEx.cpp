#include "stdafx.h"
#include <assert.h>
#include "ListCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx

IMPLEMENT_DYNCREATE(CListCtrlEx, CMFCListCtrl)

BEGIN_MESSAGE_MAP(CListCtrlEx, CMFCListCtrl)
	//{{AFX_MSG_MAP(CListCtrlEx)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx construction/destruction

CListCtrlEx::CListCtrlEx()
{
	m_clrText = ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrBkgnd = ::GetSysColor(COLOR_WINDOW);
	m_clrHText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_clrHBkgnd = ::GetSysColor(COLOR_HIGHLIGHT);
	m_clrPercent = ::GetSysColor(COLOR_HOTLIGHT);
	m_clrHPercent = ::GetSysColor(COLOR_BTNFACE);
}

CListCtrlEx::~CListCtrlEx()
{
}

void CListCtrlEx::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;

    switch(lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
	   *pResult = CDRF_NOTIFYSUBITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
 	    *pResult = CDRF_NOTIFYSUBITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT|CDDS_SUBITEM:
		{
			int iCol = lplvcd->iSubItem;
			int iRow = lplvcd->nmcd.dwItemSpec;
			CString sItem = GetItemText(iRow, iCol);
			CRect rcItem;
			GetCellRect(iRow, iCol, LVIR_BOUNDS, rcItem);
			
			CDC *pDC= CDC::FromHandle(lplvcd->nmcd.hdc);
			
			int State = ListView_GetItemState(m_hWnd, iRow, LVIS_CUT|LVIS_SELECTED|LVIS_FOCUSED);

			pDC->FillSolidRect(&rcItem, (State &LVIS_SELECTED) ? m_clrHBkgnd : m_clrBkgnd);
		 
			if(sItem.Right(1) == '%')
			{
				rcItem.DeflateRect(2, 1);

                CString strMain;// strUpper, strLower;
                AfxExtractSubString(strMain, (LPCTSTR)sItem, 0, _T('%'));
                //AfxExtractSubString(strUpper, (LPCTSTR)sItem, 1, _T('%'));
                //AfxExtractSubString(strLower, (LPCTSTR)sItem, 2, _T('%'));

                INT nMain = S3S2I(strMain); //nUpper = 100, nLower = 100;

				pDC->Rectangle(rcItem);
				rcItem.DeflateRect(1, 1);
				CRect rcLeft, rcRight;// rcLeftUp, rcRightUp, rcLeftLow, rcRightLow;
				rcLeft = rcRight = rcItem;
				rcLeft.right = rcLeft.left + MulDiv(nMain, rcItem.Width(), 100);
				rcRight.left = rcLeft.right;

                //rcLeft.top += rcItem.Height() / 5;
                //rcLeft.bottom -= rcItem.Height() / 5;
                //rcRight.top += rcItem.Height() / 5;
                //rcRight.bottom -= rcItem.Height() / 5;

               /* rcLeftUp = rcRightUp = rcItem;
                rcRightUp.left = rcLeftUp.right;
                rcLeftUp.bottom -= rcItem.Height() * 4 / 5;
                rcRightUp.bottom -= rcItem.Height()* 4 / 5;

                rcLeftLow = rcRightLow = rcItem;
                rcRightLow.left = rcLeftLow.right;
                rcLeftLow.top += rcItem.Height() * 4 / 5;
                rcRightLow.top += rcItem.Height() * 4 / 5;

                if (!strUpper.IsEmpty())
                {
                    nUpper = S3S2I(strUpper);
                    rcLeftUp.right = rcLeftUp.left + MulDiv(nUpper, rcItem.Width(), 100);
                    rcRightUp.left = rcLeftUp.right;
                }

                if (!strLower.IsEmpty())
                {
                    nLower = S3S2I(strLower);
                    rcLeftLow.right = rcLeftLow.left + MulDiv(nLower, rcItem.Width(), 100);
                    rcRightLow.left = rcLeftLow.right;
                }*/

                CString StrDisplay;

/*
                if (nMain == 100 && nLower != 100)
                {
                    StrDisplay = _T("99.99%");
                }
                else*/
                {
                    StrDisplay = strMain + _T("%");
                }

				if((State &LVIS_SELECTED))
				{
					//pDC->FillSolidRect(rcLeftUp, m_clrHPercent);
					//pDC->FillSolidRect(rcRightUp, m_clrBkgnd);

                    pDC->FillSolidRect(rcLeft, m_clrHPercent);
                    pDC->FillSolidRect(rcRight, m_clrBkgnd);

                    //pDC->FillSolidRect(rcLeftLow, m_clrHPercent);
                    //pDC->FillSolidRect(rcRightLow, m_clrBkgnd);

					pDC->SetTextColor(m_clrText);
					pDC->DrawText(StrDisplay, rcItem, DT_VCENTER|DT_CENTER|DT_SINGLELINE);
				}
				else
				{
                    //pDC->FillSolidRect(rcLeftUp, m_clrPercent);
                    //pDC->FillSolidRect(rcRightUp, m_clrBkgnd);

                    pDC->FillSolidRect(rcLeft, m_clrPercent);
                    pDC->FillSolidRect(rcRight, m_clrBkgnd);

                    //pDC->FillSolidRect(rcLeftLow, m_clrPercent);
                    //pDC->FillSolidRect(rcRightLow, m_clrBkgnd);

					//CRgn rgn;
                    //CRect rcLeftText, rcRightText;
                    //rcLeftText = rcRightText = rcItem;
                    //rcLeftText.right = rcLeft.right;
                    //rcRightText.left = rcLeft.right;

					//rgn.CreateRectRgnIndirect(rcLeftText);
					//pDC->SelectClipRgn(&rgn);
					//pDC->SetTextColor(m_clrBkgnd);
                    pDC->SetTextColor(RGB(153,217,234));
					pDC->DrawText(StrDisplay, rcItem, DT_VCENTER|DT_CENTER|DT_SINGLELINE);

					//rgn.SetRectRgn(rcItem);
					//pDC->SelectClipRgn(&rgn);
					//pDC->SetTextColor(m_clrText);
					//pDC->DrawText(StrDisplay, rcItem, DT_VCENTER|DT_CENTER|DT_SINGLELINE);
                    //pDC->SelectClipRgn (NULL);
				}
			}
			else
			{
				rcItem.OffsetRect(2,0); 
				pDC->SetTextColor((State & LVIS_SELECTED) ? m_clrHText:m_clrText);
				pDC->DrawText(sItem, -1, &rcItem, DT_LEFT | DT_VCENTER);
			}
		}
		*pResult= CDRF_SKIPDEFAULT;
		break;
	}
}

BOOL CListCtrlEx::GetCellRect(int iRow, int iCol, int nArea, CRect &rect)
{
	if(iCol)
		return GetSubItemRect(iRow, iCol, nArea, rect);
	
	if(GetHeaderCtrl().GetItemCount() == 1)
		return GetItemRect(iRow, rect, nArea);
	
	CRect rCol1;
	if(!GetSubItemRect(iRow, 1, nArea, rCol1))
		return FALSE;
	
	if(!GetItemRect(iRow, rect, nArea))
		return FALSE;
	
	rect.right = rCol1.left;
	
	return TRUE;
}
/*
void CListCtrlEx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 16;
}

void CListCtrlEx::Init()
{
	// WM_WINDOWPOSCHANGED
	// WM_MEASUREITEM
	CRect rc;
	GetWindowRect( &rc );

	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rc.Width();
	wp.cy = rc.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage( WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp );
}
*/