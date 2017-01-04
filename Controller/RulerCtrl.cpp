#include "stdafx.h"
#include "RulerCtrl.h"
#include "Controller.h"
#include "LayoutDesignerCtrl.h"


const int	TOP_RULER__HEIGHT  = 25;
const int	LEFT_RULER_WIDTH =  25;
const int	WND_EDG_THIEK  = 28;
const int   COMMON_SEED_NUM  = 5;
const int	CM_LEN = 20;
const int	POINT_SIZE = 1000 ; // the min unit is :pixel / POINT_SIZE
const int	OFF_P = 3;
const int   RULER_OFF_P = 2;
const float	SCAL_MAX = 16.0;
const float SCAL_MIN = 0.02;

const int	handleBaseSize = 2; // s pixel.
const int	scal_pixel = 6;
const int   num_scal_len = 7;
const int   num_txt_off = 6;
IMPLEMENT_DYNAMIC(CRulerCtrl, CWnd)
LRESULT APIENTRY  SlibingWindowProc( HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
	 
	CRulerCtrl *pRulerInst = (CRulerCtrl *)::GetWindowLongPtr(hwnd,GWLP_USERDATA);
	if(pRulerInst && ::IsWindow(pRulerInst->m_hWnd))
	{
		MSG msg;
		msg.hwnd = hwnd;
		msg.message = uMsg;
		msg.lParam = lParam;
		msg.wParam = wParam;
		LRESULT  lResult =   ::CallWindowProc(pRulerInst->m_orgProc,hwnd,uMsg,wParam,lParam);
	 
		pRulerInst->DelayEvent(&msg);
		return lResult ;
		
	}
	return ::DefWindowProc(hwnd,uMsg,wParam,lParam);
}
 
BOOL CRulerCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	this->DelayEvent(pMsg);
	
	//send hotkey to designerctrl.
	CLayoutDesignerCtrl *pDesigner = dynamic_cast<CLayoutDesignerCtrl *>(this->m_pWndSlibing);
	LayoutCmdEngine *pCmdEngine = pDesigner->GetCmdEngine();
	pCmdEngine->TransMessage(pMsg);

	return CWnd::PreTranslateMessage(pMsg);
}



CRulerCtrl::CRulerCtrl()
{
	m_nSnapXLine = -1;  //snap line number in array.
	m_nSnapYLine = -1;
	//m_bSnapAxis = FALSE; // Snaped line in whitch array.
	m_pWndSlibing = NULL ;

	m_xStart = 0;   //x number start
	m_yStart = 0;   //y number start
	m_yRange = 0;	
	m_xRange = 0;
	m_AxisScal = 1.0; //Axis Scal,according to Designer Zoom Scal.

	//
	m_cxStart = 0;
	m_cyStart = 0;
	m_cyHeight = 0;
	m_cxWidth = 0;
	 
	m_bSnapTopRuler  = FALSE;
	m_bSnapLeftRuler = FALSE;
	m_bSnapYLine = FALSE;
	m_bSnapXLine = FALSE;

	m_nxCmPixelLen = 0;
	m_nyCmPixelLen = 0;
	m_nxSeed = 0;
	m_nySeed = 0;
	m_bEnableFixBindLine = FALSE;
	m_bEnableBindLine = TRUE;
	m_bEnableRuler = TRUE;
	m_nFocusYHandle = -1;
	m_nFocusXHandle = -1;
    
	m_bMouseOnLine = FALSE;
	m_y_Top_LayoutBinder = 0;
	m_y_Btm_LayoutBinder = 0;
	m_x_left_LayoutBinder = 0;
	m_x_right_LayoutBinder = 0;

}

CRulerCtrl::~CRulerCtrl()
{

}


 void CRulerCtrl::InitRulerFont()
 {
	// this->m_RulerFont;
	// CFont *cf = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	 LOGFONT lg;
	 afxGlobalData.fontSmall.GetLogFont(&lg);
	 //lg.lfHeight = 9;
	 //_tcscpy( lg.lfFaceName,_T("Courier"));
	 this->m_RulerFont.CreateFontIndirect(&lg);
	// this->m_RulerFont = afxGlobalData.fontSmall;S
 }

BOOL CRulerCtrl::Create(CWnd *pOwner,CLayoutDesignerCtrl *pWndSlibing)
{
	//set default use dialog wndproc.
	this->m_pWndSlibing = pWndSlibing;
	pWndSlibing->ModifyStyleEx(0,WS_EX_CLIENTEDGE );
	DWORD nStyle = WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN ;

	CString strClassName = ::AfxRegisterWndClass(CS_SAVEBITS, ::LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_BTNFACE + 1), NULL);
	BOOL bRet =  __super::Create(strClassName,_T("Ruler"),nStyle,CRect(0,0,0,0),pOwner,32770,NULL);
	if(bRet)
	{
		this->m_orgProc = (WNDPROC)::SetWindowLongPtr(pWndSlibing->m_hWnd,GWLP_WNDPROC,(LONG_PTR)SlibingWindowProc);
		::SetWindowLongPtr(pWndSlibing->m_hWnd,GWLP_USERDATA,(LONG_PTR)this);
	}
	return bRet;
	
}

BOOL CRulerCtrl::DoScrollChange()
{
	ASSERT(this->m_pWndSlibing != NULL);

	if(!m_pWndSlibing || !::IsWindow(m_pWndSlibing->m_hWnd))
	{
		TRACE(_T("m_pWndSlibing is not window ! \n"));
		return FALSE;
	}

	//CLayoutDesignerCtrl *pLayoutCtrl = dynamic_cast<CLayoutDesignerCtrl*>(m_pWndSlibing);
	m_AxisScal = m_pWndSlibing->GetScale(); // same scal to Layout Ctrl.

	SCROLLINFO  scrlInfo;

	ZeroMemory(&scrlInfo, sizeof(scrlInfo));
    scrlInfo.cbSize = sizeof(scrlInfo);
    scrlInfo.fMask = SIF_PAGE|SIF_POS|SIF_RANGE;

	m_pWndSlibing->GetScrollInfo( SB_HORZ,&scrlInfo);

	int oldStart = m_xStart;
	m_xStart = max(scrlInfo.nPos,m_pWndSlibing->m_HscrollPos);
	m_xRange = scrlInfo.nMax - scrlInfo.nMin;
	m_xStart -= scrlInfo.nMin;
	
	//if(m_xRange <= scrlInfo.nPage)//-zoom 
	//{

	//}else
	//{

	//}

	BOOL bEreaseXAxia = (oldStart != m_xStart)? TRUE:FALSE;
	m_pWndSlibing->GetScrollInfo( SB_VERT,&scrlInfo);

	oldStart = m_yStart;

	m_yStart = max(scrlInfo.nPos,m_pWndSlibing->m_VscrollPos);
	m_yRange = scrlInfo.nMax - scrlInfo.nMin;
	m_yStart -= scrlInfo.nMin;

	/*if(m_yRange <= scrlInfo.nPage)
	{

	}else
	{

	}*/

 	BOOL bEreaseYAxia = (oldStart != m_yStart)? TRUE:FALSE;
	 
	//this->Invalidate();
	RecalcRuler(bEreaseXAxia,bEreaseYAxia);
	return TRUE;
}

void  CRulerCtrl:: RecalcRuler(BOOL bEreaseX,BOOL bEreaseY)
{
	 if(bEreaseX)
		 this->InvalidateRect(GetTopRulerDrawBorder());
	 if(bEreaseY)
		 this->InvalidateRect(GetLeftRulerDrawBorder());
}

//change slibing client pos to ruler  pos.
CPoint   CRulerCtrl::GetRulerToSlibingOffset()
{
	CPoint pos1(0,0);
	CPoint pos2(0,0);
	m_pWndSlibing->ScreenToClient(&pos1);
	ScreenToClient(&pos2);

	return pos2 - pos1;
}

CPoint CRulerCtrl ::GetOffSetPoint()
{
	CPoint pos1(0,0);
	CPoint pos2(0,0);
	
	m_pWndSlibing->ScreenToClient(&pos1);
	ScreenToClient(&pos2);

	return pos2 - pos1;
}

void  CRulerCtrl::ChangeSlibingClientPosToRuler( CPoint &pt) //转换为1
{
	pt.y =  pt.y/this->m_AxisScal + m_yStart  ;
	pt.x =  pt.x/this->m_AxisScal + m_xStart ;
}

void CRulerCtrl::ChangeRulerPosToSlibingClient(CPoint &pt)
{
	pt.y = pt.y * this->m_AxisScal - m_yStart;
	pt.x = pt.x * this->m_AxisScal - m_xStart;
}

//
//recalculate wnd size.
void CRulerCtrl::RecalcLayout()
{
	ASSERT(this->m_pWndSlibing != NULL);
	CRect OwnerCliRect;
	GetParent()->GetClientRect(OwnerCliRect);

	CRect SlibingRect = OwnerCliRect;
	if(this->m_bEnableRuler)
	{
		if((GetStyle()&WS_VISIBLE)!=WS_VISIBLE)
			ShowWindow(SW_SHOW);
	SlibingRect.DeflateRect(WND_EDG_THIEK/2,WND_EDG_THIEK/2);
	SlibingRect.OffsetRect(WND_EDG_THIEK/2,WND_EDG_THIEK/2);

	SetWindowPos(&wndBottom ,OwnerCliRect.left,OwnerCliRect.top,
		OwnerCliRect.Width(),OwnerCliRect.Height(),SWP_SHOWWINDOW|SWP_SHOWWINDOW|SWP_NOCOPYBITS);
	}
	else
	{
		if((GetStyle()&WS_VISIBLE)==WS_VISIBLE)
			ShowWindow(SW_HIDE);
	}
	m_pWndSlibing->MoveWindow( SlibingRect.left, SlibingRect.top, SlibingRect.Width(), SlibingRect.Height());
	//m_pWndSlibing->MoveWindow(SlibingRect.left,SlibingRect.top,SlibingRect.Width(),SlibingRect.Height());

	CRect wndRect;
	m_pWndSlibing->GetWindowRect(wndRect);
	this->ScreenToClient(wndRect);

	m_cxStart = wndRect.left;
	m_cyStart = wndRect.top;

	/*Draw x Axis*/
	m_cxWidth = wndRect.Width();
	m_cyHeight = wndRect.Height();

}

BEGIN_MESSAGE_MAP(CRulerCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


void CRulerCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	//CMemDC   memDC(&dc,this);
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages
	
	//OnDrawRulerWidget(&dc);
}


BOOL CRulerCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CRect cliRect;
	GetClientRect(cliRect);

	CMemDC memDC(*pDC,this);
	 
	CDC &dc = memDC.GetDC();
	
	dc.SetMapMode(MM_TEXT);
	dc.FillRect(cliRect,&afxGlobalData.brBtnFace);

	OnDrawRulerBkGnd(&dc);
	OnDrawRulerWidgets(&dc);
	//OnDrawRulerBkGnd(&dc);
	return TRUE;
}

void CRulerCtrl::DrawBindLineHandle(CRect rect,CDC *pDC,BOOL bFocus)
{
	CDC mTransDC;
	CBitmap mTransBmp;
	mTransDC.CreateCompatibleDC(pDC);
	mTransBmp.CreateCompatibleBitmap(pDC,rect.Width(),rect.Height());
	mTransDC.SelectObject(mTransBmp);

	CBrush		fillBrush ,edgBrush;
	COLORREF    fillColor,edgColor;

	fillColor = RGB(0,0,160);
	edgColor  = bFocus ? RGB(255,0,0):RGB(0,160,0);

	
	fillBrush.CreateSolidBrush(fillColor);
	edgBrush.CreateSolidBrush(edgColor);

	if(rect.IsRectEmpty())
	{
		rect.InflateRect(1,1);
		pDC->FillRect(rect,&edgBrush);

		return;
	}

	CRect drawRect = rect;
	drawRect.OffsetRect(-drawRect.TopLeft());
	mTransDC.FillRect(drawRect,&fillBrush);
	
	mTransDC.FillRect(CRect(drawRect.left,drawRect.top,drawRect.right,drawRect.top + 2),&edgBrush);
	mTransDC.FillRect(CRect(drawRect.left,drawRect.top,drawRect.left + 2,drawRect.bottom),&edgBrush);
	mTransDC.FillRect(CRect(drawRect.left,drawRect.bottom - 2,drawRect.right,drawRect.bottom),&edgBrush);
	mTransDC.FillRect(CRect(drawRect.right - 2,drawRect.top,drawRect.right,drawRect.bottom),&edgBrush);
	

	BLENDFUNCTION bldFunc ={0,0,160,0};
	pDC->AlphaBlend(rect.left,rect.top,rect.Width(),
			rect.Height(),&mTransDC,0,0,drawRect.Width(),drawRect.Height(),bldFunc);
}

CRect  CRulerCtrl::GetTopRulerLineHandleRect(int x ,CRect xDrawRect)
{
	int halfWidth = handleBaseSize *this->m_AxisScal;
	CRect rect(x ,xDrawRect.top,x,xDrawRect.Height());
	rect.InflateRect(halfWidth,0);
	rect.OffsetRect(xDrawRect.left + OFF_P,0); 
	return rect;
}


CRect  CRulerCtrl::GetLeftRulerLineHandleRect(int y,CRect yDrawRect)
{

	int halfWidth = handleBaseSize *this->m_AxisScal;
	CRect rect(yDrawRect.left,y ,yDrawRect.Width(),y);
	rect.InflateRect(0,halfWidth);
	rect.OffsetRect(0,yDrawRect.top+OFF_P);

	return rect;
}
void CRulerCtrl::OnDrawRulerWidgets(CDC *pDC)
{
	//
	//if fix mode is enabled,can't draw normal bindlines.
	//
	if(m_bEnableBindLine)
	{
		OnDrawRulerWidget(pDC);
		return;
	}

	if(m_bEnableFixBindLine)
	{
		OnDrawRulerFixWidget(pDC);
	}
}
//
//
//
void CRulerCtrl::OnDrawRulerFixWidget(CDC *pDC)
{

}
//
//
//
void CRulerCtrl::OnDrawRulerWidget(CDC *pDC)
{
	CRect cliRect;
	this->GetClientRect(&cliRect);

	CPoint pt;
	GetCursorPos(&pt);
	
	this->ScreenToClient(&pt);
	//draw halftone line , to show mouse position.
	CRect xRange = GetTopRulerDrawBorder();
	CRect yRange = GetLeftRulerDrawBorder();

	//int oldRop = pDC->SetROP2(R2_NOT);
	if(yRange.top < pt.y &&  pt.y < yRange.bottom)	
		pDC->FillRect( CRect(yRange.left,pt.y,TOP_RULER__HEIGHT,pt.y +2),pDC->GetHalftoneBrush());
	if(xRange.left < pt.x &&  pt.x < xRange.right )	
		pDC->FillRect( CRect(pt.x,xRange.top,pt.x + 2,TOP_RULER__HEIGHT),pDC->GetHalftoneBrush());
	//pDC->SetROP2(oldRop);
	CRgn yrgn;
	yrgn.CreateRectRgn(yRange.left,yRange.top + 2,yRange.right,yRange.bottom);
	pDC->SelectObject(&yrgn);
	//ruler lines widget on ruler y directration
	if(this->m_yRulerBindLines.size())
	{
		for( int i = 0;i < m_yRulerBindLines.size();i++)
		{

			CPoint pt(0,0);
			pt.y = m_yRulerBindLines[i];
			m_pWndSlibing->TransRulerPosToClient(pt);
			CRect yDrawRect = this->GetLeftRulerDrawBorder();
			FLOAT nShowRange = yDrawRect.Height();
			//nShowRange /=  this->m_AxisScal;	
			
			BOOL bFocusLine = FALSE;
			if(m_bSnapYLine && i == m_nSnapYLine || i == this->m_nFocusYHandle)
				bFocusLine = TRUE;

			//y =  ((FLOAT)(y - this->m_yStart) *this->m_AxisScal + 0.5);//卷动坐标运算
			nShowRange = max(nShowRange,this->m_yRange);
			if(pt.y > 0 && pt.y < nShowRange)
			{
				CRect rect = GetLeftRulerLineHandleRect(pt.y,yDrawRect);
				DrawBindLineHandle(rect,pDC,bFocusLine);
				
				TRACE(_T("bind y :%d \n"),pt.y);
			}
		}
	}

	//ruler lines widget on ruler x  directration
	CRgn xrgn;
	xrgn.CreateRectRgn(xRange.left + 2,xRange.top,xRange.right,xRange.bottom);
	pDC->SelectClipRgn(&xrgn);

	if(this->m_xRulerBindLines.size())
	{
		CRect xDrawRange = this->GetTopRulerDrawBorder();
		int xStart = this->m_xStart;
		int xEnd = min(this->m_xRange,xDrawRange.Width() + xStart);
		for( int i = 0 ;i < m_xRulerBindLines.size();i++ )
		{
			CPoint pt(0,0);
			pt.x = m_xRulerBindLines[i];
			m_pWndSlibing->TransRulerPosToClient(pt);
			CRect xDrawRect = this->GetTopRulerDrawBorder();
			FLOAT nShowRange = xDrawRect.Width();
			//nShowRange /=  this->m_AxisScal;	
			
			BOOL bFocusLine = FALSE;
			if(m_bSnapXLine && i == m_nSnapXLine ||i == this->m_nFocusXHandle)
				bFocusLine = TRUE;

			//x =  ((x - this->m_xStart) *this->m_AxisScal + 0.5);//卷动坐标运算
			nShowRange = max(nShowRange,this->m_xRange);
			if(pt.x > 0 && pt.x < nShowRange)
			{
				CRect rect = GetTopRulerLineHandleRect(pt.x,xDrawRect);
				DrawBindLineHandle(rect,pDC,bFocusLine);
			
				TRACE(_T("bind x :%d \n"),pt.x);
			}
		}
	}


}

//
//Draw Ruler when m_AxisScal >= 1
//
void  CRulerCtrl::OnDrawTopRuler(CDC *pDC ,CRect xfillRange)
{ 
	CRect rulerRect = GetTopRulerVisibleRect();
	int xStart = rulerRect.left * POINT_SIZE;
	FLOAT fxCmPixelLen = m_nxCmPixelLen * m_AxisScal;
	int nSeedRange = (fxCmPixelLen < CM_LEN)? 10:1;
	int xCmPixelLen =  (int)(fxCmPixelLen * POINT_SIZE);
	int baseLoc = 0;

    if(xCmPixelLen <= 0)
        xCmPixelLen = 1;

	while(nSeedRange * fxCmPixelLen < CM_LEN)
		nSeedRange *=2;

	CRgn rulerRgn;
	rulerRect.InflateRect(5,0);
	rulerRect.OffsetRect(5,0);
	rulerRgn.CreateRoundRectRgn(rulerRect.left,rulerRect.top,rulerRect.right += fxCmPixelLen,rulerRect.bottom,10,10);

	pDC->SaveDC();
	pDC->SelectObject(rulerRgn);
	pDC->FillRect(rulerRect,CBrush::FromHandle((HBRUSH)::GetStockObject(WHITE_BRUSH)));
	
    int nUnitCount = (rulerRect.Width() * POINT_SIZE) / (xCmPixelLen * nSeedRange) + 1;
	int nSeedNum = (((xCmPixelLen * nSeedRange)/POINT_SIZE) / m_nxSeed) - 1;
	int iStep = m_nxSeed;				
	float fdx =  float(((xCmPixelLen* nSeedRange)/POINT_SIZE) % m_nxSeed)/(float)((nSeedNum == 1)?2:nSeedNum);
    for(int i = 0;i < nUnitCount;i++)
    {
        CString strNumber;
		strNumber.Format(_T("%d"), i * nSeedRange *m_nxCmPixelLen);
				
		CRect drawRect;
		pDC->SaveDC();
		pDC->SelectObject(this->m_RulerFont);
		pDC->DrawText(strNumber,&drawRect,DT_CALCRECT);
        
        baseLoc = i *xCmPixelLen * nSeedRange;
		drawRect.OffsetRect((float)baseLoc/POINT_SIZE + 0.5 + RULER_OFF_P + rulerRect.left,scal_pixel);
		
		CPoint pt  = drawRect.TopLeft();
		drawRect.OffsetRect(-drawRect.Width() / 2,num_txt_off);
	
		pDC->DrawText(strNumber,&drawRect,DT_SINGLELINE );
		pDC->MoveTo(pt.x,xfillRange.top);
		pDC->LineTo(pt.x,xfillRange.top + num_scal_len);
		
		pDC->RestoreDC(-1);
 
		float fStart = rulerRect.left + OFF_P + baseLoc / POINT_SIZE ; //logic to client 
		for( int j = 1;j <= nSeedNum;j++)
		{
			fStart +=  (float)iStep;
			fStart +=  fdx;  
			CRect ptRect = xfillRange;
			ptRect.left = xfillRange.top + fStart;
			ptRect.top += 3;
			ptRect.bottom = ptRect.top + 2;
			ptRect.right = ptRect.left + 2;
			ptRect.OffsetRect(-1,0);
			OnDrawRulerPoint(pDC ,ptRect);
		}
        
    }

	pDC->RestoreDC(-1);
	pDC->DrawEdge(xfillRange,BF_DIAGONAL_ENDTOPRIGHT,BF_LEFT|BF_TOP|BF_BOTTOMRIGHT);
	
}

CRect CRulerCtrl::GetTopRulerVisibleRect()
{

	CRect rulerRect = this->GetTopRulerDrawBorder();
	CRect  layOutRect = m_pWndSlibing->GetLayoutClientRect();
	if(layOutRect.IsRectEmpty())
		return rulerRect;

	rulerRect.DeflateRect(0,1);
	rulerRect.OffsetRect(0,-1);
	rulerRect.left = layOutRect.left;
	rulerRect.right = layOutRect.right;
	return rulerRect;
}

CRect CRulerCtrl::GetLeftRulerVisibleRect() 
{
	CRect rulerRect = this->GetLeftRulerDrawBorder();
	CRect  layOutRect = m_pWndSlibing->GetLayoutClientRect();
	if(layOutRect.IsRectEmpty())
		return rulerRect;

	rulerRect.DeflateRect(1,0);
	rulerRect.OffsetRect(-1,0);
	rulerRect.top = layOutRect.top;
	rulerRect.bottom = layOutRect.bottom;
	return rulerRect;

}

void  CRulerCtrl::OnDrawLeftRuler(CDC *pDC,CRect yfillRange)
{
	CRect rulerRect = GetLeftRulerVisibleRect();
	int yStart = rulerRect.top * POINT_SIZE;
	FLOAT fyCmPixelLen = m_nyCmPixelLen * m_AxisScal;
	int nSeedRange = (fyCmPixelLen < CM_LEN)? 10:1;
	int yCmPixelLen =  (int)(fyCmPixelLen * POINT_SIZE);
	int baseLoc = 0;

    if(yCmPixelLen <= 0)
        yCmPixelLen = 1;

	while(nSeedRange * fyCmPixelLen < CM_LEN)
		nSeedRange *=2;

	CRgn rulerRgn;
	rulerRect.InflateRect(0,5);
	rulerRect.OffsetRect(0,5);
	rulerRgn.CreateRoundRectRgn(rulerRect.left,rulerRect.top,rulerRect.right,rulerRect.bottom +=fyCmPixelLen,10,10);

	pDC->SaveDC();
	pDC->SelectObject(rulerRgn);
	pDC->FillRect(rulerRect,CBrush::FromHandle((HBRUSH)::GetStockObject(WHITE_BRUSH)));
	
    int nUnitCount = (rulerRect.Height()*POINT_SIZE) / (yCmPixelLen * nSeedRange) + 1;
    int nSeedNum = (((yCmPixelLen * nSeedRange)/POINT_SIZE) / m_nxSeed) - 1;
	int iStep = m_nySeed;
				
	float fdy =  float(((yCmPixelLen* nSeedRange)/POINT_SIZE) % m_nySeed)/(float)((nSeedNum == 1)?2:nSeedNum);
    for(int j = 0;j < nUnitCount;j++)
    {
        CString strNumber;
		strNumber.Format(_T("%d"), j * nSeedRange * m_nyCmPixelLen);
				
		CRect drawRect;
		pDC->SaveDC();
		pDC->SelectObject(this->m_RulerFont);
		pDC->DrawText(strNumber,&drawRect,DT_CALCRECT);

        baseLoc = j *yCmPixelLen * nSeedRange;

		drawRect.OffsetRect(scal_pixel,(float)baseLoc /POINT_SIZE + 0.5 + RULER_OFF_P + rulerRect.top);
		if(strNumber.GetLength() < 4)
		{
			drawRect.OffsetRect((LEFT_RULER_WIDTH - scal_pixel - drawRect.Width())/2,0 );
			//drawRect.OffsetRect(drawRect.Width()/2,0);
		}
		CPoint pt  = drawRect.TopLeft();
		pDC->DrawText(strNumber,&drawRect,DT_SINGLELINE );
		pDC->MoveTo(yfillRange.left,pt.y);
		pDC->LineTo(yfillRange.left + num_scal_len,pt.y);
		
		pDC->RestoreDC(-1);
 
		float fStart = rulerRect.top + OFF_P + baseLoc / POINT_SIZE ;
				 
		for( int j = 1;j <= nSeedNum;j++)
		{
			fStart +=  (float)iStep;
			fStart +=  fdy;
			CRect ptRect = yfillRange;
			ptRect.top = yfillRange.top + fStart ;
			ptRect.bottom = ptRect.top + 2;
			ptRect.left += 3;
			ptRect.right = ptRect.left + 2;
			//ptRect.OffsetRect( 2,0);
			OnDrawRulerPoint(pDC ,ptRect);

		}

    }

	pDC->RestoreDC(-1);
	pDC->DrawEdge(yfillRange,BF_DIAGONAL_ENDTOPRIGHT,BF_LEFT|BF_TOP|BF_BOTTOMRIGHT);
}

//
//Draw ruler when m_AxisScal >= 1
//
void  CRulerCtrl::OnDrawRulerPoint(CDC *pDC ,CRect rcPoint)
{
	//COLORREF clrGnd = RGB(161,161,161),clrFGnd = RGB(0,0,161);
	CBrush brGnd,brFGnd;
	brGnd.CreateSolidBrush(RGB(140,140,140));
	brFGnd.CreateSolidBrush(RGB(210,210,210));

	pDC->FillRect(rcPoint,&brGnd);
	rcPoint.top++;
	rcPoint.right--;
	pDC->FillRect(rcPoint,&brFGnd);
}

 
CRect CRulerCtrl::GetTopRulerDrawBorder()
{
	return CRect(m_cxStart,2,m_cxStart + m_cxWidth,LEFT_RULER_WIDTH + 4);
}

CRect CRulerCtrl::GetLeftRulerDrawBorder()
{
	return CRect(2,m_cyStart,LEFT_RULER_WIDTH + 4 ,m_cyHeight + m_cyStart);
}

void CRulerCtrl::OnDrawRulerBkGnd(CDC *pDC)
{
	
	//cx ruler
	CBitmap cxMemBmp;
	cxMemBmp.CreateCompatibleBitmap(pDC,m_cxWidth,LEFT_RULER_WIDTH * 2);
	CDC cxMemDC;
	cxMemDC.CreateCompatibleDC(pDC);
	cxMemDC.SelectObject(cxMemBmp);

	CRect xfillRange = GetTopRulerDrawBorder();
	CRect xfillRect = xfillRange;
	xfillRect.OffsetRect(-xfillRange.TopLeft());

	CBrush borderBrush;  
	borderBrush.CreateSolidBrush(afxGlobalData.clrActiveCaption);


	cxMemDC.FillRect(xfillRect,/*&afxGlobalData.brBtnFace*/&borderBrush);
	OnDrawTopRuler(&cxMemDC,xfillRect);
	pDC->BitBlt(xfillRange.left,xfillRange.top,xfillRange.Width(),xfillRange.Height(),&cxMemDC,0,0,SRCCOPY);

	//cy ruler
	CBitmap cyMemBmp;
	cyMemBmp.CreateCompatibleBitmap(pDC,LEFT_RULER_WIDTH * 2,m_cyHeight);
	CDC cyMemDC;
	cyMemDC.CreateCompatibleDC(pDC);
	cyMemDC.SelectObject(cyMemBmp);

	CRect yfillRange = GetLeftRulerDrawBorder();
	CRect yfillRect = yfillRange;
	yfillRect.OffsetRect(-yfillRange.TopLeft());

	cyMemDC.FillRect(yfillRect,/*&afxGlobalData.brBtnFace*/&borderBrush);
	OnDrawLeftRuler(&cyMemDC,yfillRect);
	pDC->BitBlt(yfillRange.left,yfillRange.top,yfillRange.Width(),yfillRange.Height(),&cyMemDC,0,0,SRCCOPY);

}

int CRulerCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	InitRulerFont();

	m_pCtrlInfo = new CMiniFrameWnd();
	if(!m_pCtrlInfo->Create(_T("Static"),_T("Info Pane"),WS_POPUP ,CRect(0,0,0,0),GetOwner(),0))
		return -1;
	m_pCtrlInfo->ModifyStyle(0,WS_BORDER );
	m_pCtrlInfo->SetFont(&afxGlobalData.fontRegular);
	
	HDC hDC = ::GetDC(0); 
    int xLogPixels = ::GetDeviceCaps(hDC,LOGPIXELSX);
	int yLogPixels = ::GetDeviceCaps(hDC,LOGPIXELSY);
	::ReleaseDC(0,hDC);

	FLOAT fxCmLen  = (xLogPixels * 10.0)/25;
	m_nxCmPixelLen = (int)((fxCmLen / 10.0) + 0.5)*10;

	for( int i = COMMON_SEED_NUM;i > 1;i--)
	{
		if( (m_nxCmPixelLen % i) == 0)
		{
			m_nxSeed = m_nxCmPixelLen/i;
			break;
		}
	}

	FLOAT fyCmLem  = (yLogPixels * 10.0)/25;
	m_nyCmPixelLen = (int)((fxCmLen / 10.0) + 0.5)*10;   
	
	for( int i = COMMON_SEED_NUM;i > 1;i--)
	{
		if( (m_nyCmPixelLen % i) == 0)
		{
			m_nySeed = m_nyCmPixelLen/i;
			break;
		}
	}

	return 0;
}

void CRulerCtrl::RemoveYRulerLine( int yLine)
{
	if(yLine >= 0 && yLine < m_yRulerBindLines.size())
	{
		AXIS_ARRAY::iterator it = m_yRulerBindLines.begin();
		m_yRulerBindLines.erase(it + yLine);
		/*AXIS_ARRAY  yarray;
		for( int i = 0;i < m_yRulerBindLines.size();i++)
		{
			if( i != yLine)
				yarray.push_back(m_yRulerBindLines.at(i));
		}
		m_yRulerBindLines.clear();
		m_yRulerBindLines.assign(yarray.begin(),yarray.end());*/

	}
}

void CRulerCtrl::RemoveXRulerLine( int xLine)
{
	if(xLine >= 0 && xLine < m_xRulerBindLines.size())
	{
		AXIS_ARRAY::iterator it = m_xRulerBindLines.begin();
		m_xRulerBindLines.erase(it + xLine);
		/*AXIS_ARRAY  xarray;
		for( int i = 0;i < m_xRulerBindLines.size();i++)
		{
			if( i != xLine)
				xarray.push_back(m_xRulerBindLines.at(i));
		}
		m_xRulerBindLines.clear();
		m_xRulerBindLines.assign(xarray.begin(),xarray.end());*/
	}
}

void CRulerCtrl::ResortLines()
{
	return;
}

//process mouse feeling ,and interactive with Edit Layoutwnd.
BOOL  CRulerCtrl::HitTestLineHandle(CPoint pt,int & nHitLine ,BOOL bTop)
{ 
	nHitLine = -1;
	if(bTop)
	{
		CRect xrect = this->GetTopRulerDrawBorder(); 
		if(xrect.PtInRect(pt) && m_xRulerBindLines.size())
		{
			int nSz = m_xRulerBindLines.size();
			for(int i = 0;i < nSz ;i++)
			{
				CPoint xtp(m_xRulerBindLines[i],0);
				m_pWndSlibing->TransRulerPosToClient(xtp);
				//x =  ((FLOAT)(x - this->m_xStart) *this->m_AxisScal + 0.5);//卷动坐标运算
				if( xtp.x <= 0)
					continue;
							
				CRect handleRect = this->GetTopRulerLineHandleRect(xtp.x,xrect);	
				if(handleRect.IsRectEmpty())
					handleRect.InflateRect(1,1);
				if(handleRect.PtInRect(pt))
				{
					nHitLine = i;
					TRACE(_T("nHitLien = %d \n"),i);
					return TRUE;
				}
							
			}
		}
	}else
	{
		CRect yrect = this->GetLeftRulerDrawBorder(); 
		if(yrect.PtInRect(pt) && m_yRulerBindLines.size())
		{
			int nSz = m_yRulerBindLines.size();
			for(int i = 0;i < nSz ;i++)
			{
				CPoint ypt(0,m_yRulerBindLines[i]);
				m_pWndSlibing->TransRulerPosToClient(ypt);
				//y =  ((FLOAT)(y - this->m_yStart) *this->m_AxisScal + 0.5);//卷动坐标运算
				if( ypt.y <= 0)
					continue;
							
				CRect handleRect = this->GetLeftRulerLineHandleRect(ypt.y,yrect);	
				if(handleRect.IsRectEmpty())
					handleRect.InflateRect(1,1);
				if(handleRect.PtInRect(pt))
				{
					nHitLine = i;
					return TRUE;
				}
							
			}
		}
	}

	return FALSE;
}


CSize CRulerCtrl::GetTextSize(LPCTSTR lpStr ,CFont *pFont)
{
	ASSERT(pFont != NULL);
	HDC hDC = ::GetDC(NULL);
	CRect txtRC;

	::SaveDC(hDC);
	::SelectObject(hDC,pFont->GetSafeHandle());
	::DrawText(hDC,lpStr,_tcslen(lpStr),&txtRC,DT_CALCRECT);
	::RestoreDC(hDC,-1);
	::ReleaseDC(NULL,hDC);
	txtRC.InflateRect(1,2);
    return CSize(txtRC.Width(),txtRC.Height());
}

void  CRulerCtrl::OnToolTipRelay(MSG *pMsg)
{
	if(pMsg->hwnd != this->m_hWnd && 
			pMsg->hwnd != m_pWndSlibing->m_hWnd)
		return;
	if(!m_bEnableBindLine)
		return;

	if(pMsg->message >= WM_MOUSEFIRST && 
				pMsg->message <= WM_MOUSELAST)
	{
		if(m_bSnapYLine || m_bSnapXLine
			||m_nFocusXHandle >= 0
			||m_nFocusYHandle >= 0)
		{
			//m_CtrlTip.RelayEvent(pMsg);
			bool bShow = ((m_pCtrlInfo->GetStyle() & WS_VISIBLE) ? true:false);
			switch(pMsg->message)
			{
			case WM_MOUSEMOVE:
				{
					CPoint Pos;
					Pos.x = GET_X_LPARAM(pMsg->lParam); 
					Pos.y = GET_Y_LPARAM(pMsg->lParam); 

					CPoint tpPos = Pos;
					if(pMsg->hwnd == this->m_hWnd)
						tpPos -= GetOffSetPoint();
					CRect topRulerVisibleRect = GetTopRulerVisibleRect();
					CRect leftRulerVisibleRect =GetLeftRulerVisibleRect();
					
			#define  SHOW_X		0x1
			#define  SHOW_Y		0x2
					// can show ?
					int nEnableType = SHOW_X | SHOW_Y;
					if( !m_bSnapXLine ||
						(tpPos.x < topRulerVisibleRect.left || tpPos.x > topRulerVisibleRect.right))
						nEnableType &= ~SHOW_X;

					if( m_nFocusXHandle >= 0 && 
						(tpPos.x > topRulerVisibleRect.left && tpPos.x < topRulerVisibleRect.right))
						nEnableType |= SHOW_X;

					if( !m_bSnapYLine || 
						(tpPos.y < leftRulerVisibleRect.top || tpPos.y > leftRulerVisibleRect.bottom))
						nEnableType &= ~SHOW_Y;

					if( m_nFocusYHandle >= 0 && 
						(tpPos.y > leftRulerVisibleRect.top && tpPos.y < leftRulerVisibleRect.bottom))
						nEnableType |= SHOW_Y;

					if(nEnableType <= 0)
					{
						m_pCtrlInfo->ShowWindow(SW_HIDE);
						return;
					}
					
					CString strPos;
					FLOAT fxCmPixelLen = m_nxCmPixelLen  ;
					FLOAT fyCmPixelLen = m_nyCmPixelLen  ;

					float  fx = 0.0,fy = 0.0;
					if(nEnableType == (SHOW_X | SHOW_Y))
					{
						 fx = (float)this->m_xRulerBindLines[ m_nSnapXLine];
						 fy = (float)this->m_yRulerBindLines[ m_nSnapYLine];
					 
						 fx = (int)fx ;
						 fy = (int)fy ;
						 strPos.Format(_T("%3.1fx%3.1f"),fx  ,fy );

					}else
					{
						if(nEnableType == SHOW_X)
						{
							int n =  m_bSnapXLine?m_nSnapXLine:m_nFocusXHandle;
							fx = (float)this->m_xRulerBindLines[n];
							fx = (int)fx  ;
							strPos.Format(_T("%3.1f"),fx  );
						}else
						{  
							int n =  m_bSnapYLine?m_nSnapYLine:m_nFocusYHandle;
							fy = (float)this->m_yRulerBindLines[ n];
							fy = (int)fy  ;
							strPos.Format(_T("%3.1f"),fy  );
						}

					}
				 
					CSize sz = GetTextSize(strPos,m_pCtrlInfo->GetFont());
					ClientToScreen(&Pos);
					if(!bShow)
					{
						m_pCtrlInfo->MoveWindow(Pos.x + 60,Pos.y  + 10,sz.cx,sz.cy,TRUE);
						m_pCtrlInfo->SetWindowText(strPos);
						m_pCtrlInfo->ShowWindow(SW_SHOW);

					}else
					{
						m_pCtrlInfo->SetWindowText(strPos);
						m_pCtrlInfo->MoveWindow(Pos.x + 60,Pos.y  + 10,sz.cx,sz.cy,TRUE);
					}
				}
			}
		}else
			m_pCtrlInfo->ShowWindow(SW_HIDE);
	}
}

//
//open interface to interactive with schedulerctrl.
//
BOOL  CRulerCtrl::DelayEvent(MSG *pMsg)
{
	if(0 == ::IsWindow(m_hWnd))
		return FALSE;
	if(pMsg->message == WM_NCCALCSIZE|| pMsg->message == WM_SIZE)
	{
		RecalcLayout();
		return TRUE;
	}
	if(!this->m_bEnableBindLine)
		return TRUE;
	 
 
	OnToolTipRelay(pMsg);

	switch(pMsg->message)
	{
		case WM_MOUSEMOVE:
			{
				CLayoutDesignerCtrl *pLayout = m_pWndSlibing;
				LayoutCmdEngine *pEngine = pLayout->GetCmdEngine();
				LayoutCmdFactory *pFactory = dynamic_cast<LayoutCmdFactory*>(pEngine->getFactory());

				InvalidateRect(GetTopRulerDrawBorder());
				InvalidateRect(GetLeftRulerDrawBorder());

				// if mouse out off layout erea.
				// this action is "add line action" when pressing lbutton or rbutton on ruler.
				// so,ignore this mousemove message.
				
				if(pMsg->hwnd == m_pWndSlibing->m_hWnd
					|| pMsg->hwnd == this->m_hWnd) //Move On Slibing wnd.
				{
				
					if((m_bSnapTopRuler || m_bSnapLeftRuler) 
						&& pMsg->hwnd  == m_pWndSlibing->m_hWnd)//snap on line.
					{
						if( m_bSnapTopRuler)
						{
							bindLineCmd *pCmd = dynamic_cast<bindLineCmd *>(pFactory->newCmd(CMD_BINDLINE));
							m_yRulerBindLines.push_back(0);
							m_nSnapYLine = m_yRulerBindLines.size() - 1;
							pCmd->nYIndex = m_nSnapYLine;
							pEngine->execCmd(pCmd);
							m_bSnapYLine = TRUE;
							m_bSnapXLine = FALSE;
						}
						
						if( m_bSnapLeftRuler)
						{
							bindLineCmd *pCmd = dynamic_cast<bindLineCmd *>(pFactory->newCmd(CMD_BINDLINE));
							m_xRulerBindLines.push_back(0);
							m_nSnapXLine = m_xRulerBindLines.size() - 1;
							pCmd->nXIndex = m_nSnapXLine;
							pEngine->execCmd(pCmd);
							m_bSnapXLine = TRUE;
							m_bSnapYLine = FALSE;
						}
					
						m_bSnapLeftRuler = FALSE;
						m_bSnapTopRuler = FALSE;
					}

					if(m_bSnapYLine || m_bSnapXLine)//snap a bind line.
					{
						//clear buffer data keep last press status.
						if(pMsg->wParam == MK_LBUTTON  || 
									pMsg->wParam == MK_RBUTTON)
						{
							CPoint pt;
							::GetCursorPos(&pt);

							m_pWndSlibing->ScreenToClient(&pt);
							pLayout->TransRulerPosToLogic(pt); 

							if( m_bSnapYLine)
							{
								m_yRulerBindLines[m_nSnapYLine] = pt.y ; //offset val of layout
								this->ResortLines();
								pLayout->m_nYCatchedLine = m_nSnapYLine;
								pLayout->m_bYLineCatched = TRUE;
								pLayout->RecalcRulerLinesAttachment();
							}
							if( m_bSnapXLine)
							{
								m_xRulerBindLines[m_nSnapXLine] = pt.x ;
								this->ResortLines();
								pLayout->m_nXCatchedLine = m_nSnapXLine;
								pLayout->m_bXLineCatched = TRUE;
								pLayout->RecalcRulerLinesAttachment();
							}
							
							pLayout->Invalidate();

						}else
						{
							m_bSnapYLine = FALSE;
							m_bSnapXLine = FALSE;
						}

					}
				}
				
			}
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			{
				CLayoutDesignerCtrl *pLayout = m_pWndSlibing;

				CPoint pt;
				::GetCursorPos(&pt);	
				m_pWndSlibing->ScreenToClient(&pt);			
				//ChangeSlibingClientPosToRuler(pt);

				m_bSnapYLine = FALSE;
				m_bSnapXLine = FALSE;
			
				pLayout->TransRulerPosToLogic(pt);
				if(pMsg->hwnd == m_hWnd)
				{
					LayoutCmdEngine *pEngine = pLayout->GetCmdEngine();
					LayoutCmdFactory *pFactory = pEngine->getFactory();
					bindLineCmd *pCmd = dynamic_cast<bindLineCmd *>(pFactory->newCmd(CMD_BINDLINE));

					if(m_bSnapLeftRuler)
					{
						m_yRulerBindLines.push_back(0);
						m_nSnapYLine = m_yRulerBindLines.size() - 1;
						 
						m_yRulerBindLines[m_nSnapYLine] = pt.y ; 
						this->ResortLines();
						pLayout->m_nYCatchedLine = m_nSnapYLine;
						pLayout->RecalcRulerLinesAttachment();
						pCmd->nYIndex = m_nSnapYLine;
						pCmd->nYLineData = pt.y;

					}

					if(m_bSnapTopRuler)
					{
						m_xRulerBindLines.push_back(0);
						m_nSnapXLine = m_xRulerBindLines.size() - 1;

						m_xRulerBindLines[m_nSnapXLine] = pt.x ; 
						this->ResortLines();
						pLayout->m_nXCatchedLine = m_nSnapXLine;
						pLayout->RecalcRulerLinesAttachment();
						pCmd->nXIndex = m_nSnapXLine;
						pCmd->nXLineData = pt.x;
					}
					pEngine->execCmd(pCmd);

				}else
				{
					 this->m_nFocusXHandle = -1;
					 this->m_nFocusYHandle = -1;
				}
				m_bSnapTopRuler = FALSE;
				m_bSnapLeftRuler = FALSE;
				pLayout->Invalidate();
				 
			}break;
	 
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			{
				// create snap line here.
				if(this->m_nFocusXHandle >= 0 || this->m_nFocusYHandle >= 0)
					return FALSE;

				if(pMsg->hwnd == this->m_hWnd)
				{
					if(this->m_bEnableBindLine)
					{
						CPoint pt;
						GetCursorPos(&pt);
						this->ScreenToClient(&pt);

						CRect xRuler = this->GetTopRulerDrawBorder();
						CRect yRuler = this->GetLeftRulerDrawBorder();

						m_bSnapTopRuler = FALSE;
						m_bSnapLeftRuler = FALSE;
						if(xRuler.PtInRect(pt))
						{
							this->m_bSnapTopRuler = TRUE;
							this->m_bSnapLeftRuler = FALSE;
						}
						if(yRuler.PtInRect(pt))
						{
							this->m_bSnapLeftRuler =TRUE;
							this->m_bSnapTopRuler = FALSE;
						
						}
					}
				}
			}
			break;
		case WM_KEYDOWN:
			{
				if(pMsg->hwnd == m_hWnd ||
					this->m_nFocusYHandle >= 0
					|| this->m_nFocusXHandle >= 0 )
				{
					if(pMsg->wParam  == VK_DELETE)
						m_pWndSlibing->SendMessage(WM_KEYDOWN,pMsg->wParam,pMsg->lParam);
				}
			}break;
		case WM_NCHITTEST:
			{
				/*if(pMsg->hwnd == m_pWndSlibing->m_hWnd)*/
				m_nFocusYHandle = -1;
				m_nFocusXHandle = -1;
				
			}
			break;
		case WM_VSCROLL:
		case WM_HSCROLL:
		case WM_MOUSEWHEEL:
			{
				DoScrollChange();
			}
			break;
		
	}

	return FALSE;
}


LRESULT CRulerCtrl::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
    CPoint pt = point;
	ScreenToClient(&pt);

	HitTestLineHandle(pt,m_nFocusXHandle); 
	HitTestLineHandle(pt,m_nFocusYHandle,FALSE); // mouse on line handle ?
	if(m_nFocusXHandle >= 0 || m_nFocusYHandle >= 0)
	{
		if(GetFocus() != this)
			this->SetFocus();
	}
	return CWnd::OnNcHitTest(point);
}


void CRulerCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(	m_nFocusXHandle >= 0 || m_nFocusYHandle >= 0)
	{
		BeginHandleTrack();
	}
	CWnd::OnLButtonDown(nFlags, point);
}


BOOL CRulerCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	if(	m_nFocusXHandle >= 0 || m_nFocusYHandle >= 0)
	{
		SetCursor(LoadCursor(NULL,IDC_HAND));
		return TRUE;
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CRulerCtrl::BeginHandleTrack()
{
	::SetCapture(m_hWnd);

	CLayoutDesignerCtrl *pLayout = m_pWndSlibing;
	MSG msg;
	while(GetMessage( &msg, 0, 0, 0 ) != 0)
	{
		OnToolTipRelay(&msg);

		switch(msg.message)
		{
		case WM_MOUSEMOVE:
			{
				 
				CPoint pt;
				::GetCursorPos(&pt);	
				m_pWndSlibing->ScreenToClient(&pt);			
				pLayout->TransRulerPosToLogic(pt);

				if(m_nFocusXHandle >= 0)
				{
					m_bSnapXLine = TRUE;
					m_nSnapXLine = m_nFocusXHandle;
				}
				if(m_nFocusYHandle >= 0)
				{
					m_bSnapYLine = TRUE;
					m_nSnapYLine = m_nFocusYHandle;
				}

				if(m_bSnapYLine)
				{
					m_yRulerBindLines[m_nSnapYLine] = pt.y ; //offset val of layout
					this->ResortLines();
					pLayout->m_nYCatchedLine = m_nSnapYLine;
					pLayout->m_bYLineCatched = TRUE;
					pLayout->RecalcRulerLinesAttachment();
				}
				if(m_bSnapXLine)
				{
					m_xRulerBindLines[m_nSnapXLine] = pt.x ;
					this->ResortLines();
					pLayout->m_nXCatchedLine = m_nSnapXLine;
					pLayout->m_bXLineCatched = TRUE;
					pLayout->RecalcRulerLinesAttachment();
				}
					pLayout->Invalidate();
				 
			}break;
		
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
			{
				this->m_nFocusXHandle = -1;
				this->m_nFocusYHandle = -1;
				m_bSnapXLine = FALSE;
				m_nSnapXLine = -1;
				m_bSnapYLine = FALSE;
				m_nSnapYLine = -1;
				pLayout->m_bXLineCatched = FALSE;
				pLayout->m_bYLineCatched = FALSE;

				pLayout->Invalidate();
				goto _Out;
			}break;
	    default:
			{
				//::DefWindowProc(m_hWnd,msg.message,msg.wParam,msg.lParam);
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}break;
		}

		 
	}

	_Out:
	::ReleaseCapture();

}

 
AXIS_ARRAY & CRulerCtrl:: GetXBindLines()
{
	if(m_pWndSlibing->m_bEnableBindLay)
		return m_xLayBindLines;
	return m_xRulerBindLines;
};

AXIS_ARRAY & CRulerCtrl::GetYBindLines()
{
	if(m_pWndSlibing->m_bEnableBindLay)
		return m_yLayBindLines;
	return m_yRulerBindLines;
};
