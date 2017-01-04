#pragma once

#include "SkinDlg/TranslucentDialog.h"
#include "SkinDlg/TranslucentButton.h"
#include "SkinDlg/TranslucentProgressBar.h"
#include "LoadingStandardDlg.h"

// CLoadingTranslucentDlg dialog

class CLoadingTranslucentDlg : public CTranslucentDialog
{
	//DECLARE_DYNAMIC(CLoadingTranslucentDlg)

public:
	CLoadingTranslucentDlg(LPCTSTR lpszFile, CWnd* pParent = NULL);   // standard constructor
	CLoadingTranslucentDlg(UINT nImgID, LPCTSTR lpszType = _T("PNG"), HINSTANCE hResourceModule = NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoadingTranslucentDlg();

// Dialog Data
	enum { IDD = IDD_TRANSLUCENTDIALOG };

	virtual CStandardDialog* CreateStandardDialog();
	virtual void OnInitChildrenWnds();
    CLoadingStandardDlg* GetStandardDialog();

    virtual void GetTitle(CString& s);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

};


// CLoadingThread
class CWaitingWnd :public CWnd
{
public:
	CWaitingWnd(){};
	~CWaitingWnd(){};
protected:
	afx_msg void OnShowWindow(  BOOL bShow,UINT nStatus);
	afx_msg int  OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEreaseBkGnd(CDC *pDC);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
public:
	static HWND _hMainWnd;
private:
	static CPngImage m_pngImage;
	DECLARE_MESSAGE_MAP();
};

class CLoadingThread : public CWinThread
{
	DECLARE_DYNCREATE(CLoadingThread)

protected:
	CLoadingThread();           // protected constructor used by dynamic creation
	virtual ~CLoadingThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

   // CLoadingTranslucentDlg* m_pDlg;
protected:
	DECLARE_MESSAGE_MAP()
};


class CWaitDlg
{
public:
    CWaitDlg(HWND  hWnd = NULL )
    {
        AfxGetApp()->BeginWaitCursor();

        m_pWaitThread = AfxBeginThread(RUNTIME_CLASS(CLoadingThread));
		if(hWnd)
			::SetFocus(hWnd);
    }
    virtual ~CWaitDlg()
    {        
        AFX_BEGIN_DESTRUCTOR

        m_pWaitThread->PostThreadMessageW(WM_QUIT, NULL, NULL);
        
        AfxGetApp()->EndWaitCursor();

        AFX_END_DESTRUCTOR
    }

    // Operations
public:
    void Restore(){ AfxGetApp()->RestoreWaitCursor(); }

private:                              
    CWinThread*   m_pWaitThread;
};
