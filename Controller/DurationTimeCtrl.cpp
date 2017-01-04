// DataTimeCtrlEx.cpp : implementation file
//

#include "stdafx.h"
//#include "testDemo.h"
#include "DurationTimeCtrl.h"
#include "DurationCtrlTheme.h"


void AFXAPI DDX_DurationDateTimeCtrl(CDataExchange* pDX, int nIDC, COleDateTimeSpan& value)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	CWnd* pWnd =  CWnd::FromHandle(hWndCtrl);
	ENSURE(pWnd);

	if(!pWnd->IsKindOf(RUNTIME_CLASS(CDurationTimeCtrl)))
		return;
	CDurationTimeCtrl *pDurationCtrl = (CDurationTimeCtrl *)pWnd;
	if (pDX->m_bSaveAndValidate)
	{
		pDurationCtrl->GetTime(value);
	}
	else
	{
		pDurationCtrl->SetTime(value);
	}
}


IMPLEMENT_DYNAMIC(CDurationTimeCtrl, CWnd)
static CString strDurationCtrlClass;
CDurationTimeCtrl::CDurationTimeCtrl()
{
	 
	m_DurationString = _T("000h:00m:00s");
	this->m_iFocusSec = SS_SECTION;
	m_bEnughSpace = TRUE;

	if(strDurationCtrlClass.IsEmpty())
	 strDurationCtrlClass = AfxRegisterWndClass(
      CS_VREDRAW | CS_HREDRAW,
      ::LoadCursor(NULL, IDC_ARROW),
      (HBRUSH) ::GetStockObject(WHITE_BRUSH),
      ::LoadIcon(NULL, IDI_APPLICATION));

}

CDurationTimeCtrl::~CDurationTimeCtrl()
{
}


BEGIN_MESSAGE_MAP(CDurationTimeCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
//	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_ENABLE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CDurationTimeCtrl message handlers


bool  CDurationTimeCtrl::CreateFromStatic(CWnd *pOwner,int nID)
{
	CWnd *pWnd = pOwner->GetDlgItem(nID);
	if(!pWnd)
		return false;

	CRect wndRect;
	pWnd->GetWindowRect(&wndRect);
	pOwner->ScreenToClient(&wndRect);

	pWnd->DestroyWindow();
	BOOL bRet = this->Create(strDurationCtrlClass,_T("S3DurationCtrl"),
							WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_TABSTOP,wndRect,pOwner,nID);
	return (bRet == TRUE);

}


int CDurationTimeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_SpinBtn.Create( WS_CHILD|WS_VISIBLE  ,CRect(0,0,0,0),this,0x3E8))
		return -1;
	//m_SpinBtn.SetBuddy(this);
	
	GetDrawArea(this->m_DrawArea);
	ReBuildDataSecs();
	return 0;
}


BOOL CDurationTimeCtrl::PreTranslateMessage(MSG* pMsg)
{
	return CWnd::PreTranslateMessage(pMsg);
}

void CDurationTimeCtrl::GetDrawArea(CRect &area)
{
 
	GetClientRect(&area);
	 
	CRect spinArea = area;
	spinArea.left = spinArea.right - 22;
	spinArea.DeflateRect(2,2,2,2);
	spinArea.OffsetRect(1,0);
	spinArea.bottom += 1;
	area.right = spinArea.left;

	this->m_SpinBtn.MoveWindow(&spinArea);

	CDC *pDC = GetDC();
	 
	pDC->SaveDC();
	pDC->SelectObject(&afxGlobalData.fontRegular);

	CSize textSize = pDC->GetTextExtent( m_DurationString);
	int upMargin = (textSize.cy - area.Height())/2 ;
	area.InflateRect(CSize(upMargin,upMargin));
	area.right = (area.left + textSize.cx) > area.right? area.right -1:
		area.left + textSize.cx;

	m_bEnughSpace = (textSize.cx - area.Width()) ? FALSE:TRUE;

	pDC->RestoreDC(0);
	ReleaseDC(pDC);
}

bool CDurationTimeCtrl::GetDateFieldVal(int & val,int iSec)
{
	CString strVal;
	GetDateField(strVal, iSec);
	if(strVal != _T("none"))
	{
		val = StrToInt(strVal);
		return true;
	}
	return false;
}

bool  CDurationTimeCtrl::GetDateFieldRect(int iSec,CRect &rect)
{
	if( iSec < 0 || iSec >= this->m_DateSecs.size())
		return false;
	DATE_SEC_STR &dateSec = m_DateSecs[iSec];
	GetClientRect(&rect);
	rect.left = dateSec.startx;
	rect.right = dateSec.endx;
	rect.DeflateRect(0,1,0,1);
	return true;
}

bool CDurationTimeCtrl::SetDateFieldVal(int val,int iSec)
{
	if(val < 0)
		return false;

	CString strVal;
	strVal.Format(_T("%d"),val);
	return SetDateField(strVal,iSec);
}

bool CDurationTimeCtrl::GetDateField(CString &str,int iSec)
{
	if(iSec >= 0 &&iSec < this->m_DateSecs.size())		
	{
		str = this->m_DateSecs[iSec].strSec;
		return true;
	}
	else
	{
		str = _T("none");
	}

	return false;
}

 
bool  CDurationTimeCtrl::GetFoucusDateField(CString &str)
{
	return GetDateField(str,this->m_iFocusSec);

}

bool  CDurationTimeCtrl::SetDateField(CString str ,int iSec)
{
	if(iSec >= 0 &&iSec < this->m_DateSecs.size())
	{
		CString strCurDate ;
		this->GetDateField(strCurDate,iSec);
		if(strCurDate == str)
		{
			return false;
		}

		// modify date_sec String
		int len = str.GetLength();
		int secLen = m_DateSecs[iSec].strSec.GetLength()-1;
		int inLoc = m_DateSecs[iSec].endLocal  - len;
		
		if(inLoc < 0)
			return false;

		for(int i = 0;i < secLen ;i++)
		{
			m_DurationString.SetAt( m_DateSecs[iSec].startLocal + i,'0');
		}

	
		for( int i = 0;i < len;i++)
		{
			m_DurationString.SetAt(inLoc  + i,str[i]);
		}
	
			
		ReBuildDataSecs();
		this->InvalidateRect(this->m_DrawArea);
		
		return true;
	}
	return false;
}

bool  CDurationTimeCtrl::OnKeyDownSetFocusDateSec(CString str)
{
	bool bret = SetFoucusDateField(str);
	if(bret)
	{
		this->m_SpinBtn.SetPos(StrToInt(str));
		PostDateTimeChangeNCMessage();
	}
	return bret;
}

bool  CDurationTimeCtrl::SetFoucusDateField(CString str)
{ 
	return SetDateField(str, this->m_iFocusSec);	 
}

bool  CDurationTimeCtrl::PtToDateSec(CPoint pt)
{ 
	if(this->m_DrawArea.IsRectEmpty())
		return false;
	ReBuildDataSecs(); 
	// x in draw area range.	
	int val= -1;
	if( m_DrawArea.left <= pt.x && pt.x <= m_DrawArea.right)
	{
		for(int i = 0;i < this->m_DateSecs.size();i++)
		{
			if(m_DateSecs[i].startx <= pt.x && pt.x <= m_DateSecs[i].endx)
			{
			    m_iFocusSec = i;
				val = StrToInt(m_DateSecs[i].strSec);
				if( i != 0)
					m_SpinBtn.SetRange(0,60);
				else
					m_SpinBtn.SetRange(0,999);
				 
			}
		}
	}else
	{
		if( pt.x > m_DrawArea.right)
		{
			m_iFocusSec  = SS_SECTION;
			m_SpinBtn.SetRange(0,60);
			val = StrToInt(m_DateSecs[m_iFocusSec].strSec);
		}

		if(pt.x < m_DrawArea.left)
		{
			m_iFocusSec = HH_SECTION;
			m_SpinBtn.SetRange(0,999);
			val = StrToInt(m_DateSecs[m_iFocusSec].strSec);
		}

	}
	if(val != -1)
		m_SpinBtn.SetPos(val);
	//this->InvalidateRect(m_DrawArea);
	this->Invalidate();
	return true;
}

BOOL CDurationTimeCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= (WS_EX_CLIENTEDGE|WS_CLIPCHILDREN);
	return CWnd::PreCreateWindow(cs);
}

typedef HRESULT (*PDRAWTHEMEEDGE)(HTHEME hTheme,
    HDC hdc,
    int iPartId,
    int iStateId,
    LPCRECT pDestRect,
    UINT uEdge,
    UINT uFlags,
    LPRECT pContentRect
);

void CDurationTimeCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages
	BOOL bEnableWnd = this->IsWindowEnabled();
	BOOL bIsFoucs = (CWnd::GetFocus() == this) ? TRUE:FALSE;

	CDrawingManager gm(dc);

	CRect cliRect;
	GetClientRect(&cliRect);

	CDurationCtrlTheme obTheme(GetOwner());
	AFX_GLOBAL_DATA_PRIVATE *pGlobalDataPri = (AFX_GLOBAL_DATA_PRIVATE *)&afxGlobalData;
	if(obTheme.IsAppThemed())
	{
		//CMFCVisualManagerOffice2007Private  *pVisual = (CMFCVisualManagerOffice2007Private *) CMFCVisualManager::GetInstance();
		// PDRAWTHEMEEDGE pDrawEdge = (PDRAWTHEMEEDGE)GetProcAddress( pGlobalDataPri->m_hinstUXThemeDLL,"DrawThemeEdge");
		 
		if(bEnableWnd){ 				
			//gm.DrawRect(cliRect,RGB(255,255,255),pVisual->m_clrEditSelection);
			//(*pDrawEdge)(pVisual->m_hThemeComboBox,dc.GetSafeHdc(),CP_BORDER,CBB_FOCUSED,&cliRect,EDGE_SUNKEN,BF_RECT,&cliRect);
			obTheme.DrawBackground(&dc,cliRect,bIsFoucs);
		}
		else{
		 
			//gm.DrawRect(cliRect,afxGlobalData.clrBtnFace,pVisual->m_clrEditSelection);
			obTheme.DrawBackground(&dc,cliRect,0,0);
		}
	}else
	{
		if(!bEnableWnd)
			dc.FillRect(cliRect,&pGlobalDataPri->brBtnFace);
		dc.DrawEdge(cliRect,EDGE_SUNKEN,BF_TOPLEFT|BF_BOTTOMRIGHT);
	}
	dc.SaveDC();
	dc.SelectObject(&afxGlobalData.fontRegular);

	if(!bEnableWnd)
		dc.SetTextColor(RGB(168,168,168));
	dc.SetBkMode(TRANSPARENT);

	DWORD nAlign = DT_LEFT;
#if 0
	
	//CSize txtSize = dc.GetTextExtent(m_DurationString,m_DurationString.GetLength());
	if(!m_bEnughSpace)// not enugh space.
	{
		if( this->m_iFocusSec == SS_SECTION)
			nAlign = DT_RIGHT;
		if(this->m_iFocusSec == MM_SECTION)
			nAlign = DT_CENTER;
	}
#endif

	dc.DrawText(m_DurationString,m_DrawArea,nAlign);
	
	if( bIsFoucs  && bEnableWnd)
	if(m_iFocusSec >= HH_SECTION && (m_iFocusSec < this->m_DateSecs.size()))
	{
		CRect secRect(m_DateSecs[m_iFocusSec].startx,m_DrawArea.top,m_DateSecs[m_iFocusSec].endx,this->m_DrawArea.bottom);
		
		dc.SetBkMode(TRANSPARENT);
		COLORREF textClr = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
		/* I Can't get theme color of text selected.
		ETS_ASSIST, ETS_CUEBANNER, ETS_DISABLED, ETS_FOCUSED, ETS_HOT, ETS_NORMAL, ETS_READONLY, ETS_SELECTED
		HRESULT hres = obTheme.GetThemeColor(EP_EDITTEXT,ETS_FOCUSED,TMT_TEXTCOLOR,&textClr);
		hres = obTheme.GetThemeColor(EP_EDITTEXT,ETS_HOT,TMT_TEXTCOLOR,&textClr);
		hres = obTheme.GetThemeColor(EP_EDITTEXT,ETS_NORMAL,TMT_TEXTCOLOR,&textClr);
		hres = obTheme.GetThemeColor(EP_EDITTEXT,ETS_SELECTED,TMT_TEXTCOLOR,&textClr);
		*/
		dc.SetTextColor(textClr);
		// not enugh space
#if 0
		if(nAlign != DT_LEFT)
		{
			if(nAlign == DT_RIGHT)
				secRect.OffsetRect( m_DrawArea.right - secRect.right,0);
			
			if(nAlign == DT_CENTER)
			{
				int secWdith = secRect.Width();
				secRect.left = m_DrawArea.left + (m_DrawArea.Width() -secWdith )/2;
				secRect.right = secRect.left  + secWdith;

			}
		}
#endif
		CBrush fillBrush;
		COLORREF fillColor;
		fillColor = ::GetSysColor(COLOR_HIGHLIGHT);
		fillBrush.CreateSolidBrush(fillColor);
		dc.FillRect(secRect,&fillBrush);

		CString &strSec = m_DateSecs[m_iFocusSec].strSec;
		secRect.right = (secRect.right > m_DrawArea.right) ?m_DrawArea.right:secRect.right;
		dc.DrawText(strSec,strSec.GetLength(),secRect,nAlign);
	}
 
	 
	dc.RestoreDC(0);
	this->m_SpinBtn.Invalidate();
}


void CDurationTimeCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	TRACE(_T("Pos : %d \n"),nPos);
	CString strSec;
	this->SetFocus();
	if(this->m_iFocusSec < HH_SECTION)
		m_iFocusSec = HH_SECTION;
	if(nSBCode != 4)
		return;

	GetFoucusDateField(strSec);
	int val = StrToInt(strSec);
	 
	int nRange = this->m_SpinBtn.GetRange();
	if( 0 <= nRange && nRange <= 60)
	{
		CSpinButtonCtrl *pSpin = (CSpinButtonCtrl *) pScrollBar;
		int pinPos = pSpin->GetPos();
		if( pinPos == 65536) // 65539 : dec  
		{
			if(val == 0 && CanDec(m_iFocusSec - 1))
			{
				nPos = 59;
				if(m_iFocusSec > HH_SECTION)
					DecDateSec(m_iFocusSec - 1);
			}
		}

		if(pinPos == 65596)// 65596:inc
		{
			if(nPos == 60 && CanInc(m_iFocusSec - 1))
			{
				nPos = 0;
				if(m_iFocusSec > 0)
				   IncDateSec(m_iFocusSec - 1);
			}
		}
	}

	 
	strSec.Format(_T("%d"),nPos);
	if(this->SetFoucusDateField(strSec))
	{
		this->m_SpinBtn.SetPos(nPos);
		PostDateTimeChangeNCMessage();
	}
}

void CDurationTimeCtrl::PostDateTimeChangeNCMessage()
{
	CWnd *pOwner = GetOwner();
	DWORD dwID = this->GetDlgCtrlID();
	NMHDR hdr;
	hdr.code = DTN_DATETIMECHANGE;  
	hdr.hwndFrom = this->m_hWnd;
	hdr.idFrom = dwID;
	pOwner->SendMessage(WM_NOTIFY,(WPARAM)dwID, (LPARAM)&hdr);
}

void CDurationTimeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint pt;
	::GetCursorPos(&pt);
	this->ScreenToClient(&pt);

	CRect spinRect;
	this->m_SpinBtn.GetWindowRect(spinRect);
	this->ScreenToClient(&spinRect);
	spinRect.InflateRect(2,2,2,2);
	if(::PtInRect(spinRect,pt))
		return;
	 
	this->PtToDateSec(pt);
	//this->InvalidateRect(m_DrawArea);
	this->SetFocus();
	//CWnd::OnLButtonDown(nFlags, point);
}


void CDurationTimeCtrl::ReBuildDataSecs(void)
{
	this->m_DateSecs.clear();
	CDC *pDC = GetDC();

	pDC->SaveDC();
	pDC->SelectObject(&afxGlobalData.fontRegular);
	// default is : 00:00:00 ,caculate date sec leng and size.
	CSize signSize = pDC->GetTextExtent( _T(":"));
	CSize textSize;
	CString strSub;
	DATE_SEC_STR  dateSec;
	::AfxExtractSubString(strSub,m_DurationString,0,':');
	textSize = pDC->GetTextExtent( strSub);

	dateSec.startLocal = 0;
	dateSec.endLocal = strSub.GetLength() - 1;
	dateSec.startx = this->m_DrawArea.left;
	dateSec.endx = dateSec.startx + textSize.cx;
	dateSec.strSec = strSub;
	m_DateSecs.push_back(dateSec);

	::AfxExtractSubString(strSub,m_DurationString,1,':');
	textSize = pDC->GetTextExtent( strSub);
	dateSec.startLocal = m_DateSecs[0].endLocal  + 2;
	dateSec.endLocal = dateSec.startLocal  + strSub.GetLength()-1;
	dateSec.startx = m_DateSecs[0].endx + signSize.cx;
	dateSec.endx = dateSec.startx + textSize.cx;
	dateSec.strSec = strSub;
	m_DateSecs.push_back(dateSec);

	::AfxExtractSubString(strSub,m_DurationString,2,':');
	textSize = pDC->GetTextExtent( strSub);
	dateSec.startLocal = m_DateSecs[1].endLocal  + 2;
	dateSec.endLocal =dateSec.startLocal  + strSub.GetLength()-1;
	dateSec.startx = m_DateSecs[1].endx + signSize.cx;
	dateSec.endx = dateSec.startx + textSize.cx;
	dateSec.strSec = strSub;
	m_DateSecs.push_back(dateSec);
	
	pDC->RestoreDC(0);
	ReleaseDC(pDC);
}

bool  CDurationTimeCtrl::DecDateSec(int iSec)
{
	int val;
	if( !GetDateFieldVal(val,iSec))
		return false;
	val--;
	if(iSec > 0 && val < 0)
	{
	   if( CanDec(iSec))
	   {
		   val = 59;
		   SetDateFieldVal(val,iSec);
		   return DecDateSec(iSec-1);
	   }
	   else 
		   val = 0;
	   return SetDateFieldVal(val,iSec);
        
	}
	if(iSec == 0)
	{
		if(val < 0)
			val = 0;
	}
	 
	return SetDateFieldVal(val,iSec);
	 
}

bool  CDurationTimeCtrl::IncDateSec(int iSec)
{
	int val;
	if(!GetDateFieldVal(val,iSec))
		return  false;
 
	val++;
	if(iSec > 0 && val >=60)
	{
		if(CanInc(iSec))
		{
			val = 0;
			SetDateFieldVal(val,iSec);
			return IncDateSec(iSec - 1);
		}else
		 val = 59;
		return SetDateFieldVal(val,iSec);
		 
	}
	if(iSec == 0)
	{
		if(val > 999)
			val=999;
	}
	 
	return SetDateFieldVal(val,iSec);
	 
}

bool  CDurationTimeCtrl::CanDec(int iSec)
{
	int val;
	if( !GetDateFieldVal(val,iSec))
		return false;
	
	if(iSec == 0)
	{
		if(val <= 0)
			return false;
		return true;
	}
	if( iSec > 0)
	{
		if( val <= 0)
			return CanDec(iSec - 1);
		else
			return true;
	}
	return true;
}

bool  CDurationTimeCtrl::CanInc(int iSec)
{
	int val;
	if( !GetDateFieldVal(val,iSec))
		return false;
	
	if(iSec == 0)
	{
		if(val >= 999)
			return false;
		return true;
	}
	if( iSec > 0)
	{
		if( val >= 59)
			return CanInc(iSec - 1);
		else
			return true;
	}
	return true;
	
}

void CDurationTimeCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	//m_InputTracker.Track(nChar,this);
	CWnd::OnChar(nChar, nRepCnt, nFlags);
}


// keyboard input tracker
bool CDurationTimeCtrl::CKeyBoardInputTrack::Filter(int& key)
{
	if(key <= 0)
		return false;
	if(!::isalpha(key) && ::isalnum(key) )
		return true;
	return false;
}

bool  CDurationTimeCtrl::CKeyBoardInputTrack::Track(int key, CWnd *pOwner)
{
	if( CWnd::GetFocus() != pOwner)
		return false;
	if( !Filter(key))
		return false;
	CDurationTimeCtrl *pDurationCtrl = (CDurationTimeCtrl*) pOwner;
	pDurationCtrl->GetFoucusDateField(this->m_OldString);
	

	CString keyStr((TCHAR)key);
	pDurationCtrl->OnKeyDownSetFocusDateSec(keyStr);

	for(;;)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

	/*	if (CWnd::GetCapture() != pWnd)
			break;*/

		if(CWnd::GetFocus() != pOwner)
			break;
	    
		switch (msg.message)
		{
			case WM_KEYDOWN:
				{ 
					int nIEnableNum = (pDurationCtrl->m_iFocusSec) ?2:3;
					int nRange = (pDurationCtrl->m_iFocusSec) ?60:1000;
					unsigned short nflag = HIWORD(msg.lParam);
					int  ch = msg.wParam;

					//change extern keyboard.
					if((nflag & 0x40) == 0x40)
						ch -= 48;
					if(Filter(ch))
					{
						keyStr += (char)ch;
						if(StrToInt(keyStr) >= nRange)
							keyStr = (char)ch;

						if(keyStr.GetLength() < nIEnableNum)
							pDurationCtrl->OnKeyDownSetFocusDateSec(keyStr);
						else 
						{
							while(keyStr.GetLength() - nIEnableNum)
							{
								keyStr.Delete(0,1);
							}
							pDurationCtrl->OnKeyDownSetFocusDateSec(keyStr);
							goto EXIT;
						}
					}
				
				}
				break;
			case WM_LBUTTONDOWN:
				{
					int iFocusItem = pDurationCtrl->m_iFocusSec;
					DispatchMessage(&msg);

					if(iFocusItem != pDurationCtrl->m_iFocusSec)
						goto EXIT;
					  
				}
				break;
			case WM_CHAR:
				break;
			
			default:
				{
					DispatchMessage(&msg);
				}
			break;
		}
	}
EXIT:
	return true;
}



void CDurationTimeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == 189)
	{
		this->m_iFocusSec++;
		if(m_iFocusSec > 2)
			m_iFocusSec = 0;
		this->InvalidateRect(m_DrawArea);
	}
	 
	unsigned short sflag = nFlags;
	if((sflag & 0x40) == 0x40)
		nChar -= 48;
	m_InputTracker.Track(nChar,this);
	
}


void CDurationTimeCtrl::OnKillFocus(CWnd* pNewWnd)
{
		CWnd::OnKillFocus(pNewWnd);
	//pNewWnd->SetFocus();
	// TODO: Add your message handler code here
	this->InvalidateRect(m_DrawArea);
}


void CDurationTimeCtrl::OnEnable(BOOL bEnable)
{
	CWnd::OnEnable(bEnable);
	this->m_SpinBtn.EnableWindow(bEnable);
	this->Invalidate();
	// TODO: Add your message handler code here
}


void CDurationTimeCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
	this->Invalidate();
	// TODO: Add your message handler code here
}


void CDurationTimeCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if(cx <= 0 || cy <= 0)
		return ;
	GetDrawArea(this->m_DrawArea);
	// TODO: Add your message handler code here
}

// public interface ,for GetTime and SetTime
void CDurationTimeCtrl::SetTime(COleDateTimeSpan oleDate)
{
	int nD = oleDate.GetDays();
	int nH = oleDate.GetHours();
	int nM = oleDate.GetMinutes();
	int nS = oleDate.GetSeconds();

	if(nD > 0)
		nH += nD * 24;
	this->SetDateFieldVal(nH,HH_SECTION);
	this->SetDateFieldVal(nM,MM_SECTION);
	this->SetDateFieldVal(nS,SS_SECTION);

}

void CDurationTimeCtrl::GetTime(COleDateTimeSpan &oleDate)
{
	int nD = 0,nH = 0,nM = 0,nS = 0;
	
	this->GetDateFieldVal(nH,HH_SECTION);
	this->GetDateFieldVal(nM,MM_SECTION);
	this->GetDateFieldVal(nS,SS_SECTION);

	nD = nH / 24;
	nH = nH % 24;
	oleDate.SetDateTimeSpan(nD,nH,nM,nS);

}