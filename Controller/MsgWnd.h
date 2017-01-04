#if !defined(AFX_MSGWND_H__1DC7047D_8675_4C80_B335_54F78F3BBD76__INCLUDED_)
#define AFX_MSGWND_H__1DC7047D_8675_4C80_B335_54F78F3BBD76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsgWnd window

class CMsgWnd : public CWnd
{
// Construction
public:
	CMsgWnd();

// Attributes
protected:
    CBitmap m_Bitmap;
	BITMAP bmBitmap;
	BOOL m_bFlag;
	
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgWnd)
	//}}AFX_VIRTUAL

// Implementation
public:	
	CString m_strMessage;//LPTSTR m_strMessage;
	CString m_strCaption;//LPTSTR m_strCaption;
	virtual ~CMsgWnd();
    void CreateMsgWindow();
	//void SetPromptMessage(LPCTSTR lpszMsg);
	void SetPromptMessage(CString lpszMsg);
	//void SetPromptCaption(LPCTSTR lpszCaption);
	void SetPromptCaption(CString lpszCaption);

	// Generated message map functions
protected:
	//{{AFX_MSG(CMsgWnd)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGWND_H__1DC7047D_8675_4C80_B335_54F78F3BBD76__INCLUDED_)
