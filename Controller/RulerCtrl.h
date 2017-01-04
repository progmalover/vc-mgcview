#pragma once
#include <vector>
#include "layoutDesignerCmds.h"
using namespace std;
using namespace DESIGNERCMDS;
typedef vector<int>  AXIS_ARRAY;

class CLayoutDesignerCtrl;
class CS3DrawObj;
class CRulerCtrl :public CWnd
{
	friend class CS3DrawObj;
	friend class CLayoutDesignerCtrl;
	DELARE_FRIEND_LAYOUT_CMDS()
	DECLARE_DYNAMIC(CRulerCtrl)
public:	
	CRulerCtrl();
	~CRulerCtrl();
public:
	WNDPROC m_orgProc;
protected:
	CMiniFrameWnd*			m_pCtrlInfo;
	CLayoutDesignerCtrl *	m_pWndSlibing ; // edit slibing wnd;
	FLOAT		m_AxisScal; //Axis Scal,according to Designer Zoom Scal.
	//axis local pos ,for draw ruler .
	int 		m_xStart;   //x local start
	int         m_yStart;   //y local start
	int         m_yRange;	
	int 		m_xRange;

	//axis Device pos ,for draw Ruler Band Area.
	int			m_cxStart;
	int			m_cyStart;
	int			m_cyHeight;
	int			m_cxWidth;

	//font
	CFont	m_RulerFont;

	//ruler :Bind lines
	BOOL		m_bEnableRuler;
	BOOL		m_bEnableFixBindLine; 
	BOOL		m_bEnableBindLine;
	BOOL		m_bSnapTopRuler;
	BOOL		m_bSnapLeftRuler;
	BOOL		m_bMouseOnLine;
	
	int			m_nSnapXLine;
	int			m_nSnapYLine;
	BOOL		m_bSnapXLine;
	BOOL		m_bSnapYLine;

	//ruler line handle
	int		m_nFocusXHandle;
	int		m_nFocusYHandle;

	AXIS_ARRAY	m_xRulerBindLines;
	AXIS_ARRAY	m_yRulerBindLines;
	AXIS_ARRAY	m_xLayBindLines;
	AXIS_ARRAY	m_yLayBindLines;
	//ruler widget: LayoutBinder
	int			m_y_Top_LayoutBinder;
	int			m_y_Btm_LayoutBinder;
	int			m_x_left_LayoutBinder;
	int			m_x_right_LayoutBinder;

	//
	int			m_nxCmPixelLen;
	int			m_nyCmPixelLen;
	int			m_nxSeed;
	int			m_nySeed;

public:
	BOOL DelayEvent(MSG *pMsg);
	
	CSize GetTextSize(LPCTSTR lpStr,CFont *pFont);
	void OnToolTipRelay(MSG *pMsg);
	void RecalcLayout();
	void RecalcRuler(BOOL bEreaseX,BOOL bEreaseY);
	void OnDrawRulerBkGnd(CDC *pDC);
	void OnDrawRulerWidgets(CDC *pDC);
	void OnDrawRulerFixWidget(CDC *pDC);
	void OnDrawRulerWidget(CDC *pDC);
	void OnDrawRulerPoint(CDC *pDC ,CRect rcPoint);
	void OnDrawTopRuler(CDC *pDC,CRect xfillRange);
	void OnDrawLeftRuler(CDC *pDC,CRect yfillRange);

	BOOL HitTestLineHandle(CPoint pt,int &nHitLine, BOOL bTop = TRUE);

	CPoint GetRulerToSlibingOffset();
	CPoint GetOffSetPoint();
	
	void RemoveYRulerLine( int yLine);
	void RemoveXRulerLine( int xLine);
	void ChangeSlibingClientPosToRuler(CPoint &pt);
	void ChangeRulerPosToSlibingClient(CPoint &pt);
	void DrawBindLineHandle(CRect rect,CDC *pDC,BOOL bFocus = FALSE);
	void InitRulerFont();
	void EnableBindLine(BOOL bEnable){m_bEnableBindLine = bEnable;};
	void ResortLines();
	void BeginHandleTrack();
	void SetXYSeed(int x,int y){m_nxSeed = x;m_nySeed = y;};

	CRect GetTopRulerDrawBorder();
	CRect GetLeftRulerDrawBorder();
	CRect GetTopRulerVisibleRect();
	CRect GetLeftRulerVisibleRect(); 
	CRect GetTopRulerLineHandleRect(int x ,CRect xDrawRect);
	CRect GetLeftRulerLineHandleRect(int y ,CRect yDrawRect);

	AXIS_ARRAY & GetXBindLines();
	AXIS_ARRAY & GetYBindLines();

public:
	BOOL Create(CWnd *pOwner,CLayoutDesignerCtrl *pWndSlibing);
	BOOL DoScrollChange();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	
};