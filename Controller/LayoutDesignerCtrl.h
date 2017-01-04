#pragma once

#include "S3DrawObj.h"
#include "BaseDropTarget.h"
#include "RulerCtrl.h"
#include "cmds.h"
#include "LayoutDesignerCmds.h"
#include "PropertySubCmds.h"
#include <list>
// CLayoutDesignerCtrl


typedef struct _GRIDLINE
{
    CPoint startPoint;
    CPoint endPoint;
}GRIDLINE;
typedef struct _GRIDLINE *PGRIDLINE;

typedef enum SelectMode
{
    SEL_MODE_NONE,
    SEL_MODE_MOVE,
    SEL_MODE_SIZE,
    SEL_MODE_MULTI_SELECT
};

// self define message, used for notify some info to the designer main wnd

// wParam - no used, lParam - float point to the zoom factor
extern UINT WM_ON_LAYOUT_ZOOM;

// wParam - -1 - no select, 0 - select obj, 1 - select background, lParam - point to the CS3DrawObjList
extern UINT WM_ON_LAYTOUT_SELECT_OBJ;

// wParam - TRUE - removed, otherwise - not removed, lParam - point to the CS3DrawObjList
extern UINT WM_ON_LAYOUT_REMOVE_OBJ;

// wParam - point of OBJINFO, include the default position and property type, lParam - Array of added media files' name, if the array count is 0, mean insert a component
extern UINT WM_ON_LAYOUT_INSERT_OBJ;

// wParam - TRUE - action on the layout design, otherwise - no action, lParam - point to the action structure
extern UINT WM_ON_LAYOUT_ACTION;

// wParam - point to selected CS3DrawObj, lParam - Array of added media files' name, if the array count is 0, mean insert a component
extern UINT WM_ON_OBJ_ADDMEDIA;

// wParam - client ID
extern UINT WM_ON_LAYTOUT_SELECT_CLIENT;

// wParam, lParam - see WM_KEYDOWN
extern UINT DESIGNER_KEYDOWN;

using namespace std;
using namespace DESIGNERCMDS;
using namespace LAYOUTPROPERTYCMDS;
class WidgetsManager;
class DrawWidget
{
    friend class WidgetsManager;
public:
   virtual void OnDraw(CDC *pDC) = 0;
   virtual void OnEvent(MSG *msg){};
protected:
   CLayoutDesignerCtrl *m_pCtrl;
};

class LayoutWidget :public DrawWidget
{
    DELARE_FRIEND_PROPERTY_CMDS()
public:
    LayoutWidget()
    {
        m_pCtrl = NULL;
        m_fRotateDegree = 0.0f;
        m_xTrans = 0;
        m_yTrans = 0;
    };
    ~LayoutWidget(){};

    typedef struct line{
        POINT ptStart;
        POINT ptEnd;
        bool operator==(struct line &);
    }LINE;

    typedef  vector<LINE>   LINES;
    typedef  struct edge{
        LINES  xLines;
        LINES  yLines; 
        LINES  freeLines;
        void   clear();
    }EDGE;

    typedef  vector<CRect>  RECTS;

    typedef struct layoutmask
    {
        CBitmap maskBmp;
        CRgn    maskRgn;
    }LAYOUTMASK;

protected:
    EDGE   m_Edge;     
    EDGE   m_VHBravityEdge;
    RECTS  m_MonitorRects; 
    FLOAT  m_fRotateDegree;
    FLOAT  m_xTrans;
    FLOAT  m_yTrans;
    CSize  m_LayoutSize;
protected:
   virtual void   CauclateMonitorRegionEdiges(FLOAT xTrans = 0.0f, FLOAT yTrans = 0.0f);
   virtual void   DrawMonitorText(CDC *pDC);
   virtual void   DrawMonitorRegions(CDC *pDC);
   virtual void   DrawEdges(CDC *pDC);
   virtual void   OnDraw(CDC *pDC);
   virtual void   OnEdgeBravity(void);
   virtual HRGN   CreateMaskRgn(CRect layoutRc);
   virtual void   OnEvent(MSG *msg);
   virtual void   operator=(LayoutWidget &obj);
   
public:
   void SetClientLayout(CSize &szClient ,std::vector<CRect> *rcMonitors =NULL, BOOL bNotify = TRUE,FLOAT fRotateDegree = 0.0f);
   RECTS & GetMonitorRects(){return m_MonitorRects;};
};
 
//
//z-order in layout.
//
#define LAYOUT_Z_0   0 //base layout order.
#define LAYOUT_Z_1   1
#define LAYOUT_Z_2   2
#define LAYOUT_Z_3   3
#define LAYOUT_Z_4   4
#define LAYOUT_Z_MAX 10
#define LAYOUT_Z_BOTTOM    LAYOUT_Z_1
#define LAYOUT_Z_FLOATING  0xffff // use this add to lay max.

typedef vector<DrawWidget *> WIDGETS;
class WidgetsManager:public WIDGETS
{
public:
    WidgetsManager(){};
    ~WidgetsManager(){};
protected:
    CLayoutDesignerCtrl *m_pDesigner;
public:
    virtual void OnDraw(CDC *pDC);
    DrawWidget * AddWidget(int zOrder, DrawWidget *); // when widget exist ,return old "DrawWidget",and keep new one.
    void Init(CLayoutDesignerCtrl *pCtrl = NULL);
    void clear();
    void RelayEvent(MSG *pMsg);
};

class CLayoutDesignerCtrl : public CWnd
{
    DECLARE_DYNAMIC(CLayoutDesignerCtrl)
	//commands
	DELARE_FRIEND_LAYOUT_CMDS()
	DELARE_FRIEND_PROPERTY_CMDS()
	DECLARE_FRIEND_CLASS(CRulerCtrl)
    DECLARE_FRIEND_CLASS(WidgetsManager)
    DECLARE_FRIEND_CLASS(LayoutWidget)


public:
    CLayoutDesignerCtrl();
    virtual ~CLayoutDesignerCtrl();

    enum POINT_POS
    {
        TOP_LEFT,
        TOP_CENTER,
        TOP_RIGHT,
        RIGHT_CENTER,
        BOTTOM_RIGHT,
        BOTTOM_CENTER,
        BOTTOM_LEFT,
        LEFT_CENTER
    };
       
    enum LINE_POS
    {
        TOP,
        RIGHT,
        BOTTOM,
        LEFT
    };

    enum DragMediaType
    {
        MEDIA_FILE,
        MEDIA_URL
    };
    struct DragMediaInfo
    {
		DragMediaInfo(){};
		DragMediaInfo(const struct DragMediaInfo & _In)
		{
			this->MediaID = _In.MediaID;
			this->MediaName = _In.MediaName;
			this->Duration = _In.Duration;
			this->MediaType = _In.MediaType;
			this->RefreshInteval = _In.RefreshInteval;
			this->nHeight = _In.nHeight;
			this->nWidth = _In.nWidth;
		};
        INT             MediaID;
        CString         MediaName;
        INT             Duration;
        INT             RefreshInteval;
        DragMediaType   MediaType;
        DWORD           nWidth;
        DWORD           nHeight;
    };
public:
    DrawWidget *GetWidget(int i);
    CSize GetLayoutSize(){return m_LayoutSize;};
    BOOL UpdateOptions();

    /// Set up the layout virtual coordinates, calc the m_XTrans, m_YTrans, m_Scale to transform between
    /// the layout logical size and CWnd client area physical size. Also set ratio to decide how to draw
    /// the virtual dot lines.
    void CalcTransform(BOOL bResize = FALSE);
    void CalcFitScale();
    void CalcScrollRange();
    void LayoutToClient(int& nLength);
    void LayoutToClient(UINT& nLength);
    void LayoutToClient(CRect& rect);
    void LayoutToClient(FRect& rect);
    void LayoutToClient(FPoint& point);
    void ClientToLayout(FPoint& point);
    void LayoutToClient(CPoint& point);
    void ClientToLayout(CPoint& point);
    void ClientToLayout(CRect& rect);
    void ClientToLayout(FRect& rect);
    void ClientToLayout(int& nLength);
    void ClientToLayout(UINT& nLength);
    void SetLayoutSize(const CSize layoutSize);
	void UpDataDrawObject();
    BOOL AlignToGrid(FPoint& point, const FPoint clickPoint);
    BOOL AlignToGridByPoint(FRect& rect, const FPoint posDelta, POINT_POS refPoint = TOP_LEFT);// it will do the delta thing and align to..
    BOOL AlignToGridResizeByLine(FRect& rect, LINE_POS refLine = RIGHT, BOOL bKeepAspect = FALSE);// it just used to adjust the lines
    BOOL AlignToGridResizeByPoint(FRect& rect, POINT_POS refPoint = TOP_LEFT, BOOL bKeepAspect = FALSE);// it just used to adjust the lines
    void ZoomImage(FLOAT zoomFactor);
    FLOAT GetZoomFactor();

    void SelectDrawObj(CS3DrawObj* pObj, BOOL bAdd = FALSE);
    void SelectDrawObjWithinRect(CRect& rect, BOOL bAdd = FALSE);       /// If select rect hit some obj, return the rect include all selected obj, else return empty rect
    void SelectDrawObjWithinRect(FRect& rect, BOOL bAdd = FALSE);       /// If select rect hit some obj, return the rect include all selected obj, else return empty rect
    void DeselectDrawObj(CS3DrawObj* pObj);
    void CloneSelection(CS3DrawObjList* pClipList);
    void PrepareClone();
    void DeleteObj();
    void TabChange();
    void MoveLeft();
    void MoveRight();
    void MoveUp();
    void MoveDown();
    BOOL IsSelected(const CS3DrawObj* pObj) const;
    void SelectAllDrawObj(BOOL bUnSelAll = FALSE);

    void MoveBack();
    void MoveForward();
    void MoveToBack();
    void MoveToFront();

	
    void SetupScrollBars(BOOL bOnInit = FALSE);
    BOOL IsVirtualRect() { return m_bVirtualRect; };

    void InsertDrawObj(CS3DrawObj* pObj);
    void RemoveDrawObj(CS3DrawObj* pObj);
    void InvalidateDrawObj(CS3DrawObj* pObj);
	VOID DrawFloatingWidgets(CDC *pDC);
	VOID DrawRulerLine(CDC *pDC);
	VOID DrawBindLay(CDC *pDC);
	VOID RulerLinesHitTest(CPoint pt);
    VOID TransRulerPosToClient(CPoint &pt);
	VOID TransRulerPosToLogic(CPoint &pt);
	VOID RecalcRulerLinesAttachment();
	VOID EnableBindLay(BOOL bEnable = TRUE);
	VOID CalcBindLayLines(FRect LayoutSize );
    BOOL SortIntervalX(CS3DrawObjList *targetList);
    BOOL SortIntervalY(CS3DrawObjList *targetList);
	BOOL IsBindLayEnable(){return this->m_bEnableBindLay;};
	CRulerCtrl *GetSlibingRuler();
    CS3DrawObj* GetDrawObjAtPoint(const FPoint point, BOOL bSetSelect = TRUE);

    CS3DrawObjList* GetSelectDrawObj() { return &m_selectObjList; };
	CRect GetLayoutClientRect(){
		CRect rect;
		FRect flayout = m_LayoutRect;
		LayoutToClient(flayout);
		flayout.ConvertToCRect(rect);
		return rect;
	};
    HCURSOR GetDrawObjCursor(CS3DrawObj* pObj, UINT nHandle, UINT nLine);
    void OnFitScreen();
    HRESULT CloseLayout();

	LayoutCmdEngine *GetCmdEngine(){return &m_CmdEngine;};
    /// In logic coordinate, move and resize draw obj
    void MoveObjPos(CS3DrawObj* pObj, const FRect position, const FPoint clickPoint);
    void MoveObjPos(CS3DrawObj* pObj, const FPoint position, const FPoint clickPoint);
    void MoveObjHandlePos(CS3DrawObj* pObj, const UINT nHandle, const FPoint position);
    void ResizeObj(CS3DrawObj* pObj, const FPoint position, const UINT nHandle, BOOL bMouseMove = FALSE);
    void ResizeObjLine(CS3DrawObj* pObj, const FPoint deltaPosition, const UINT nLine, BOOL bMouseMove = FALSE);
     LRESULT OnAddObject(POBJINFO pObjInfo,CPoint pt,int nWidth,int nHeight);

    COLORREF GetLayoutColor(){return m_LayoutColor;};
    void     SetLayoutColor(COLORREF clr){ 
        m_LayoutColor = clr;
        this->Invalidate();
    };

    
    //arrange fuction
    void SameInterval(BOOL yOrder = FALSE);
    void FillLayout(void);
protected:
    void SetupVirtualGrid();
    void RemoveGridLine();

    void DrawGrid(CDC* pDC);
    void DrawRectObj(CS3DrawObj* pObj, CDC* pDC);
    void DrawObj(CS3DrawObj* pObj, CDC* pDC);
    void DrawSelectLine(CDC* pDC);
    void DrawLayers(CDC* pDC);
    void DrawLayout(CDC* pDC);

    void SetObjHitTestSize(CS3DrawObj* pObj);

    /// Return Value: True - obj's rect outside of the select rect, merge the select rect
    ///               False - obj's rect inside of the select rect, not change the select rect
    BOOL MergeSelectRect(FRect& selectRect, const FRect rectObj);
    int HitTestSelectRect(const FPoint point, UINT& nHandle, UINT& nLine);

    void GetSelectRectOrigin(const UINT nHandle, const UINT nLine, FPoint& originPoint);
    void GetSelectRectLast(const UINT nHandle, const UINT nLine, FPoint& point);
	FLOAT GetScale(void ) {return m_Scale;}
	void RecalcMultiSelectMergeRect();
    /// Implementation
    /// For drop-target functionality
  
protected:
    CBaseDropTarget     m_DropTarget;                   /// for drop target functionality
    CPoint              m_DragPoint;                    /// current position
    CSize               m_DragSize;                     /// size of dragged object
    CSize               m_DragOffset;                   /// offset between pt and drag object corner
    DROPEFFECT          m_prevDropEffect;
    BOOL                m_bDragDataAcceptable;

	BOOL				m_bYLineCatched;
	BOOL				m_bXLineCatched;
	int					m_nXCatchedLine;
	int					m_nYCatchedLine;

    std::vector<std::pair<CString, DragMediaInfo> >    m_DragMediaFiles;   //<FilePath, Duration>

	LayoutCmdEngine		m_CmdEngine; // zxy 
    WidgetsManager      m_Widgets; // zxy 
protected:

    COLORREF            m_BKColor;                      
    COLORREF            m_LayoutColor;
    COLORREF            m_LayoutShadowColor;
    COLORREF            m_BorderColor;
    COLORREF            m_GridColor;                    /// Grid Color
    CSize               m_LayoutSize;                   /// The layout's size
    FRect               m_LayoutRect;                   /// The layout rect, calc by the view rect and setting layout size
    UINT                m_nRatioX;                       /// Ratio of the view rect with the setting layout size
    UINT                m_nRatioY;                       /// Ratio of the view rect with the setting layout size
    BOOL                m_bVirtualRect;                 /// BOOL value to decide if there's virtual rect to decide draw
    CS3DrawObjList      m_drawObjList;                  /// List of draw obj
    CS3DrawObjList      m_selectObjList;                /// List of selected draw obj
    BOOL                m_bAlignGrid;                   /// BOOL value to decide if align to grid line
    BOOL                m_bShowGrid;                    /// BOOL value to decide show grid line or not
    CS3DrawObj*         m_pAudioObj;                    /// Audio layer, support only one audio layer in one layout, the obj create/destroy by
                                                        /// designer page, do not try to delete the point in this class
    FLOAT               m_fAlignFactor;                 /// The align factor, if the distance between obj's line and grid line smaller than it,
                                                        /// align to the grid line
    CList<PGRIDLINE, PGRIDLINE&>  m_GridLine;           /// The grid line list

    /// Calc logic size to physical size
    FLOAT               m_Scale;
    FLOAT               m_XTrans;
    FLOAT               m_YTrans;

    /// Mouse down/up point
    CPoint              m_downPoint;
    UINT                m_nDownFlags;
    CPoint              m_lastPoint;
    CPoint              m_lastDown;
    SelectMode          m_nSelectMode;
    BOOL                m_bSelection;
    CRect               m_selectRect;                   /// Draw the select rect line in mouse move,
                                                        /// if the rect include some draw obj, do not draw this rect
    CS3DrawObj*         m_pSelectObj;                   /// It's a rect draw obj for the select region rect operation
    CPoint              m_lastAlignPoint;
    CPoint              m_lastScalPoint;
    CPoint              m_lastClonePoint;               /// The point of the clone obj's left top
    CMap<CS3DrawObj*, CS3DrawObj*, CPoint, CPoint>  m_objAlignPoint;

    BOOL                m_bLBtnDblClk;
	BOOL				m_bNotifyParent;
	BOOL				m_bEnableBindLay;
    /// Scroll bar
    int                 m_HscrollPos;
    int                 m_VscrollPos;
    int                 m_MaxDesignerWidth;
    int                 m_MaxDesignerHeight;
    FRect               m_ViewportRect;
    int                 m_nSelectRectLength;

	BOOL				m_bSetupingScrollBars;
	
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage( MSG* pMsg);
public:
    DECLARE_MESSAGE_MAP()

    afx_msg int OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnDestroy();
    afx_msg void OnPaint();
	afx_msg void OnUndo();
	afx_msg void OnRedo();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

    afx_msg LRESULT OnDragLeave(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDrop(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragEnter(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragOver(WPARAM wParam, LPARAM lParam);

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg UINT OnGetDlgCode();

	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};
