#if !defined(AFX_BCGPSCROLLBAR_H__A96CA752_C9F3_4FEB_8D6F_9057D17337A0__INCLUDED_)
#define AFX_BCGPSCROLLBAR_H__A96CA752_C9F3_4FEB_8D6F_9057D17337A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BORROW_Pro.h"
#include "ControllerVisualManager.h"

BORROW_DLLEXPORT extern UINT BCGM_ON_DRAW_SCROLLBAR;	// wParam - scrollbar ID, lParam - pointer to BCGPSCROLLBAR_DRAW_INFO

/////////////////////////////////////////////////////////////////////////////
// BCGPSCROLLBAR_DRAW_INFO struct, used in BCGM_ON_DRAW_SCROLLBAR message

class CBORROW_ScrollBar;

enum BCGPSB_PART
{
	BORROW_SCROLLBAR_BACKGROUND	= 0,
	BORROW_SCROLLBAR_BUTTON		= 1,
	BORROW_SCROLLBAR_THUMB		= 2,
};

struct BORROW_DLLEXPORT BCGPSCROLLBAR_DRAW_INFO
{
	CDC*			m_pDC;
	CBORROW_ScrollBar*	m_pScrollBar;
	BCGPSB_PART		m_part;
	CRect			m_rect;
	BOOL			m_bFirst;
	BOOL			m_bHorz;
	BOOL			m_bHighlighted;
	BOOL			m_bPressed;
	BOOL			m_bDisabled;

	BCGPSCROLLBAR_DRAW_INFO (
		CDC* pDC, CBORROW_ScrollBar* pScrollBar, BCGPSB_PART part, 
		CRect rect, BOOL bHorz, BOOL bHighlighted, BOOL bPressed, 
		BOOL bFirst, BOOL bDisabled)
	{
		m_pDC = pDC;
		m_pScrollBar = pScrollBar;
		m_part = part;
		m_rect = rect;
		m_bHorz = bHorz;
		m_bHighlighted = bHighlighted;
		m_bPressed = bPressed;
		m_bFirst = bFirst;
		m_bDisabled = bDisabled;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CBORROW_ScrollBar window

class BORROW_DLLEXPORT CBORROW_ScrollBar : public CScrollBar
{
	DECLARE_DYNAMIC(CBORROW_ScrollBar)

// Construction
public:
	CBORROW_ScrollBar();

	enum BORROW_SB_STYLE
	{
		BORROW_SBSTYLE_FIRST                  = 0,
		BORROW_SBSTYLE_DEFAULT				= BORROW_SBSTYLE_FIRST,	// Windows default style,
		BORROW_SBSTYLE_VISUAL_MANAGER			= 1,	// Defined by currently activated Visual Manager
		BORROW_SBSTYLE_VISUAL_MANAGER_FRAME	= 2,	// Used in frame window,
		BORROW_SBSTYLE_OWNER_DRAW				= 3,	// Developer should handle BCGM_ON_DRAW_SCROLLBAR message,
		BORROW_SBSTYLE_LAST                   = BORROW_SBSTYLE_OWNER_DRAW
	};

// Attributes
public:
	void SetVisualStyle (BORROW_SB_STYLE style, BOOL bIsRedaw = TRUE)
	{
		ASSERT (BORROW_SBSTYLE_FIRST <= style && style <= BORROW_SBSTYLE_LAST);
		m_Style = style;

		if (bIsRedaw && GetSafeHwnd () != NULL)
		{
			RedrawWindow ();
		}
	}

	BORROW_SB_STYLE GetVisualStyle () const
	{
		return m_Style;
	}

	BOOL IsActive () const
	{
		return m_HitTest != BORROW_HT_NOWHERE;
	}

	BOOL	m_bAutoDestoyWindow;

protected:
	enum BORROW_SB_HITTEST
	{
		BORROW_HT_NOWHERE       = 0,
		BORROW_HT_BUTTON_LEFT   = 1,
		BORROW_HT_BUTTON_UP     = BORROW_HT_BUTTON_LEFT,
		BORROW_HT_BUTTON_LTUP   = BORROW_HT_BUTTON_LEFT,
		BORROW_HT_BUTTON_RIGHT  = 2,
		BORROW_HT_BUTTON_DOWN   = BORROW_HT_BUTTON_RIGHT,
		BORROW_HT_BUTTON_RTDN   = BORROW_HT_BUTTON_RIGHT,
		BORROW_HT_THUMB         = 3,
		BORROW_HT_CLIENT_LEFT   = 4,
		BORROW_HT_CLIENT_UP     = BORROW_HT_CLIENT_LEFT,
		BORROW_HT_CLIENT_LTUP   = BORROW_HT_CLIENT_LEFT,
		BORROW_HT_CLIENT_RIGHT  = 5,
		BORROW_HT_CLIENT_DOWN   = BORROW_HT_CLIENT_RIGHT,
		BORROW_HT_CLIENT_RTDN   = BORROW_HT_CLIENT_RIGHT,
		BORROW_HT_CLIENT        = 6
	};

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBORROW_ScrollBar)
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage (MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBORROW_ScrollBar();

	// Generated message map functions
protected:
	virtual BORROW_SB_HITTEST HitTest (const CPoint& point) const;

	//{{AFX_MSG(CBORROW_ScrollBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnCancelMode();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM,LPARAM);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	virtual void DoPaint (CDC* pDC);

	virtual void DoPaintThumb (CDC* pDC, CRect rect, BOOL bHorz,
								   BOOL bHighlighted, BOOL bPressed, BOOL bDisabled)
	{
		if (m_Style == BORROW_SBSTYLE_OWNER_DRAW)
		{
			BCGPSCROLLBAR_DRAW_INFO info (
				pDC, this, BORROW_SCROLLBAR_THUMB, rect, bHorz, bHighlighted, bPressed, FALSE, bDisabled);

			ASSERT_VALID (GetOwner ());

			GetOwner ()->SendMessage (BCGM_ON_DRAW_SCROLLBAR, (WPARAM) GetDlgCtrlID (), (LPARAM) &info);
			return;
		}

		visualManager->OnScrollBarDrawThumb (pDC, this, rect, 
			bHorz, bHighlighted, bPressed, bDisabled);
	}

	virtual void DoPaintButton (CDC* pDC, CRect rect, BOOL bHorz,
								BOOL bHighlighted, BOOL bPressed, BOOL bFirst, BOOL bDisabled)
	{
		if (m_Style == BORROW_SBSTYLE_OWNER_DRAW)
		{
			BCGPSCROLLBAR_DRAW_INFO info (
				pDC, this, BORROW_SCROLLBAR_BUTTON, rect, bHorz, bHighlighted, bPressed, bFirst, bDisabled);

			ASSERT_VALID (GetOwner ());

			GetOwner ()->SendMessage (BCGM_ON_DRAW_SCROLLBAR, (WPARAM) GetDlgCtrlID (), (LPARAM) &info);
			return;
		}

		visualManager->OnScrollBarDrawButton (pDC, this, rect, bHorz, 
								bHighlighted, bPressed, bFirst, bDisabled);
	}

	virtual void DoPaintBackground (CDC* pDC, CRect rect, BOOL bHorz, 
									BOOL bHighlighted, BOOL bPressed, BOOL bFirst, BOOL bDisabled)
	{
		if (m_Style == BORROW_SBSTYLE_OWNER_DRAW)
		{
			BCGPSCROLLBAR_DRAW_INFO info (
				pDC, this, BORROW_SCROLLBAR_BACKGROUND, rect, bHorz, bHighlighted, bPressed, bFirst, bDisabled);

			ASSERT_VALID (GetOwner ());

			GetOwner ()->SendMessage (BCGM_ON_DRAW_SCROLLBAR, (WPARAM) GetDlgCtrlID (), (LPARAM) &info);
			return;
		}

		visualManager->OnScrollBarFillBackground(pDC, this, rect, bHorz, 
								bHighlighted, bPressed, bFirst, bDisabled);
	}

	virtual void Process ();

	CRect GetThumbRect (const CRect& rect, BOOL bHorz) const;
	int   PosFromThumb (int thumb, const CSize& szClamp);

protected:
	BORROW_SB_STYLE	m_Style;
	BOOL			m_bTracked;
	BORROW_SB_HITTEST	m_HitTest;
	BORROW_SB_HITTEST	m_HitPressed;
	CWnd*			m_pLastCapture;
	
	DWORD			m_dwDisabledArrows;

	UINT_PTR		m_TimerDown;
	UINT_PTR		m_Timer;

	CPoint			m_ptTrackStart;
	CRect			m_rectTrackThumb;
	CRect			m_rectTrackThumbStart;
	int				m_TrackPos;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPSCROLLBAR_H__A96CA752_C9F3_4FEB_8D6F_9057D17337A0__INCLUDED_)
