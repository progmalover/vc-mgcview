// MsgWnd.cpp : implementation file
// Download by http://www.codefans.net

#include "stdafx.h"
#include "MsgWnd.h"
#include "resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//---------------------------------------------------------------------------
#define ID_TIMER_POP_WINDOW		10
#define ID_TIMER_CLOSE_WINDOW	11
#define ID_TIMER_DISPLAY_DELAY	12
#define WIN_WIDTH	181
#define WIN_HEIGHT	116
/////////////////////////////////////////////////////////////////////////////
// CMsgWnd

CMsgWnd::CMsgWnd()
{
	m_Bitmap.LoadBitmap(MAKEINTRESOURCE(IDB_ALARM_MSG)); //Load Bitmap
	m_Bitmap.GetBitmap(&bmBitmap);         //Get Bitmap Info
	m_bFlag=false;
	m_strMessage= Translate(_T("New Alarm received!"));
	m_strCaption= Translate(_T("Warning"));
	
}

CMsgWnd::~CMsgWnd()
{
}


BEGIN_MESSAGE_MAP(CMsgWnd, CWnd)
	//{{AFX_MSG_MAP(CMsgWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMsgWnd message handlers
void CMsgWnd::CreateMsgWindow()
{
//	RECT rect;
//	SystemParametersInfo(SPI_GETWORKAREA,0,&rect,0);
//	int y=rect.bottom-rect.top;
//	int x=rect.right-rect.left;
//	x=x-WIN_WIDTH;
//	y=y-WIN_HEIGHT;
	CreateEx(0,
		     AfxRegisterWndClass(
			 0,
			 ::LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_AFXBARRES_DELETE_COLUMN)),(HBRUSH)(COLOR_DESKTOP+1),NULL),
			 _T(""),
			 WS_POPUP|WS_EX_TOPMOST,
			 0,
			 0,
			 0,//bmBitmap.bmWidth,  //Bitmap Width = Splash Window Width
			 0,//bmBitmap.bmHeight, //Bitmap Height = Splash Window Height
			 NULL,//AfxGetMainWnd()->GetSafeHwnd(),
			 NULL,
			 NULL);
	SetTimer(ID_TIMER_POP_WINDOW,20,NULL);
}
void CMsgWnd::SetPromptMessage(CString lpszMsg)
{
	//lstrcpy(m_strMessage,lpszMsg);
	m_strMessage = lpszMsg;

}

void CMsgWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CDC dcMemory;
	CRect rect;
	GetClientRect(&rect);
    dcMemory.CreateCompatibleDC(NULL);
	dcMemory.SelectObject(&m_Bitmap);
	dc.StretchBlt(0,
		0,
		rect.right-rect.left,//bmBitmap.bmWidth,
		rect.bottom-rect.top,//bmBitmap.bmHeight,    
		&dcMemory, 
		0,
		0,
		bmBitmap.bmWidth,    
		bmBitmap.bmHeight,
		SRCCOPY);	
	CFont font;
	font.CreatePointFont(90,_T("Impact"));
	dc.SelectObject(&font);
	dc.SetTextColor(RGB(0,64,128));
    dc.SetBkMode(TRANSPARENT);
	dc.TextOut(30,10,m_strCaption);
	rect.top=30;
	//dc.DrawText(m_strMessage,-1,&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
	dc.DrawText(m_strMessage,&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER);

	// Do not call CWnd::OnPaint() for painting messages
}

void CMsgWnd::OnTimer(UINT_PTR nIDEvent) 
{	
	static int nHeight=0;
 	int cy=GetSystemMetrics(SM_CYSCREEN);
	int cx=GetSystemMetrics(SM_CXSCREEN);
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rect,0);
	int y=rect.bottom-rect.top;
	int x=rect.right-rect.left;
	x=x-WIN_WIDTH;
	
	switch(nIDEvent)
	{
	case ID_TIMER_POP_WINDOW:
		if(nHeight<=WIN_HEIGHT)
		{
			++nHeight;			
			MoveWindow(x,
				y-nHeight,
				WIN_WIDTH,
				WIN_HEIGHT);
			
		    Invalidate(FALSE);
		}
		else
		{
			KillTimer(ID_TIMER_POP_WINDOW);
			SetTimer(ID_TIMER_DISPLAY_DELAY,5000,NULL);
		}
		break;
	case ID_TIMER_CLOSE_WINDOW:
		if(nHeight>=0)
		{
			nHeight--;
			MoveWindow(x,
				y-nHeight,
				WIN_WIDTH,
				nHeight);
		}
		else
		{
			KillTimer(ID_TIMER_CLOSE_WINDOW);
			SendMessage(WM_CLOSE);
		}
		break;
	case ID_TIMER_DISPLAY_DELAY:
		KillTimer(ID_TIMER_DISPLAY_DELAY);
		SetTimer(ID_TIMER_CLOSE_WINDOW,20,NULL);
		break;
	}
	
	CWnd::OnTimer(nIDEvent);
}

int CMsgWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	
	return 0;
}



void CMsgWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetClientRect(&rect);
	if(rect.PtInRect(point))
	{	m_bFlag=true;
		KillTimer(ID_TIMER_DISPLAY_DELAY);
	}	
	CWnd::OnMouseMove(nFlags, point);
}

void CMsgWnd::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	if(m_bFlag)
	SetTimer(ID_TIMER_DISPLAY_DELAY,3000,NULL);
	// TODO: Add your message handler code here
	
}

void CMsgWnd::SetPromptCaption(CString lpszCaption)
{
	//lstrcpy(m_strCaption,lpszCaption);
	m_strCaption = lpszCaption;
}
