// DemoTranslucentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "LoadingTranslucentDlg.h"
#include "LoadingStandardDlg.h"


// CLoadingTranslucentDlg dialog
//IMPLEMENT_DYNAMIC(CLoadingTranslucentDlg, CTranslucentDialog)
extern HWND GetMFCMainWndCrossThread();
CLoadingTranslucentDlg::CLoadingTranslucentDlg(LPCTSTR lpszFile, CWnd* pParent /*=NULL*/)
	: CTranslucentDialog(CLoadingTranslucentDlg::IDD, lpszFile, pParent)
{
}

CLoadingTranslucentDlg::CLoadingTranslucentDlg(UINT nImgID, LPCTSTR lpszType/* = _T("PNG")*/, HINSTANCE hResourceModule/* = NULL*/, CWnd* pParent /*=NULL*/)
	: CTranslucentDialog(CLoadingTranslucentDlg::IDD, nImgID, lpszType, hResourceModule, pParent)
{
}

CLoadingTranslucentDlg::~CLoadingTranslucentDlg()
{
}

void CLoadingTranslucentDlg::DoDataExchange(CDataExchange* pDX)
{
	CTranslucentDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoadingTranslucentDlg, CTranslucentDialog)
END_MESSAGE_MAP()


CStandardDialog* CLoadingTranslucentDlg::CreateStandardDialog()
{
	return ::new CLoadingStandardDlg(this);
}

CLoadingStandardDlg* CLoadingTranslucentDlg::GetStandardDialog()
{
    return static_cast<CLoadingStandardDlg*>(m_pStandardDlg);
}
void CLoadingTranslucentDlg::OnInitChildrenWnds()
{
}

void CLoadingTranslucentDlg::GetTitle(CString& s)
{
    s.Empty();
}


// CLoaidngThread

IMPLEMENT_DYNCREATE(CLoadingThread, CWinThread)

CLoadingThread::CLoadingThread()
{
}

CLoadingThread::~CLoadingThread()
{
   if(m_pMainWnd)
   delete m_pMainWnd;
}

BOOL CLoadingThread::InitInstance()
{
  /*  m_pDlg = new CLoadingTranslucentDlg(IDB_LOADING);
    m_pDlg->Create(CLoadingTranslucentDlg::IDD, NULL);
    m_pDlg->ShowWindow(SW_SHOW);*/

	//if(!CWaitingWnd::_hMainWnd)
		CWaitingWnd::_hMainWnd = GetMFCMainWndCrossThread();
	ASSERT(::IsWindow(CWaitingWnd::_hMainWnd ));
    m_pMainWnd= new CWaitingWnd();
	CString strClassName = ::AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW, ::LoadCursor(NULL, IDC_ARROW), 0, NULL);
 	m_pMainWnd->Create(strClassName,_T("progressbar"),0 ,CRect(0,0,0,0),CWnd::FromHandle(CWaitingWnd::_hMainWnd ),0,0);
    return TRUE;
}

int CLoadingThread::ExitInstance()
{
	if(m_pMainWnd)
	 m_pMainWnd->DestroyWindow();
    return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CLoadingThread, CWinThread)
END_MESSAGE_MAP()


//CWaitingWnd
///////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CWaitingWnd,CWnd)
ON_WM_PAINT()
ON_WM_ERASEBKGND()
ON_WM_CREATE()
ON_WM_SHOWWINDOW()
ON_WM_TIMER()
END_MESSAGE_MAP()
#define WND_WIDTH  300  
#define WND_HEIGHT 50
CPngImage   CWaitingWnd::m_pngImage;
HWND 		CWaitingWnd::_hMainWnd;
int nTimeCount;
void CWaitingWnd:: OnShowWindow(  BOOL bShow,UINT nStatus)
{
	 __super::OnShowWindow(bShow,nStatus);
	 nTimeCount = 0;
	 SetTimer(10,100,0);
}

int CWaitingWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if(__super::OnCreate(lpCreateStruct) < 0 )
		return -1;

	CRect wndRect;
	GetOwner()->GetWindowRect(wndRect);
	wndRect.OffsetRect(-wndRect.TopLeft());
	CRect barRect(0,0,WND_WIDTH,WND_HEIGHT);

	if(!m_pngImage.GetSafeHandle())
		m_pngImage.Load(IDB_LOADING);
	
	barRect.OffsetRect((wndRect.Width()-barRect.Width())/2,(wndRect.Height()-barRect.Height())/2);
	SetWindowPos(&CWnd::wndTopMost, 0,0,0,0,SWP_NOACTIVATE|SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
	MoveWindow(barRect,TRUE);
	ShowWindow(SW_SHOW);
	return 0;
}

void CWaitingWnd::OnDestroy()
{
	KillTimer(10);
}

void CWaitingWnd::OnTimer(UINT_PTR nIDEvent)
{
	static int nCount = 0;
	nCount++;
	if(nCount >= 5)
	{
		nTimeCount++;
		nCount = 0;
	}
	this->Invalidate();
}

BOOL CWaitingWnd::OnEreaseBkGnd(CDC *pDC)
{
	return 1;
	//return __super::OnEraseBkgnd(pDC);
}

void CWaitingWnd::OnPaint()
{
	//__super::OnPaint();

	CPaintDC dc(this);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	memDC.SaveDC();
	memDC.SelectObject(m_pngImage.GetSafeHandle());

	
	CDC rendDC;
	CBitmap rendBmp;
	CRect cliRect;
	GetClientRect(&cliRect);
	rendDC.CreateCompatibleDC(&dc);
	rendBmp.CreateCompatibleBitmap(&dc,cliRect.Width(),cliRect.Height());
	rendDC.SelectObject(rendBmp);
	rendDC.FillSolidRect(cliRect,RGB(255,255,255));

	BLENDFUNCTION func={AC_SRC_OVER,0,255,AC_SRC_ALPHA };
	rendDC.AlphaBlend(0,0,WND_WIDTH,WND_HEIGHT,&memDC,0,0,WND_WIDTH,WND_HEIGHT,func);
	rendDC.SetBkMode(TRANSPARENT);
	
	CString strCount;
	for(int i = 0;i < nTimeCount;i++)
		strCount += _T(".");

	CRect drawRect ;
	rendDC.DrawText(strCount,strCount.GetLength(),&drawRect, DT_CALCRECT);
	if(drawRect.Width() <= WND_WIDTH / 2)
	{
		CRect barRect;
		GetClientRect(&barRect);
		drawRect.OffsetRect((barRect.Width() - drawRect.Width())/2 ,(barRect.Height()-drawRect.Height())/2);

		rendDC.DrawText(strCount,strCount.GetLength(),&drawRect,DT_CENTER);
	}else
		nTimeCount = 0;
	
	dc.TransparentBlt(0,0,cliRect.Width(),cliRect.Height(),
				&rendDC,0,0,cliRect.Width(),cliRect.Height(),RGB(255,255,255));
	memDC.RestoreDC(-1);	//CRect cliRect;

	
}