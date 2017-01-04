#pragma once

#include "S3Types.h"

typedef enum {
	BACKGROUND          = 0,
	LAYER               = 1,
	MESSAGE             = 2,
	AUDIO               = 3,
	USER_DEFINED_PLUGIN = 4,
	ALIGNDW             = 0xFFFFFFFF,
} PROPERTY_TYPE;//it can be regarded as LAYER_TYPE


/////////////////////////////////////////////////////////////////////////////
/// CS3DrawObj - base class for all 'drawable objects'
typedef struct _OBJINFO
{
    CRect           position;
    PROPERTY_TYPE   proType;
} OBJINFO, *POBJINFO;

static const UINT m_nHandleSize = 6;
static const UINT m_nLineSize = 4;


class CS3DrawObj : public CObject
{
protected:
    //DECLARE_SERIAL(CS3DrawObj);
    DECLARE_DYNAMIC(CS3DrawObj);

    enum ALIGN_MODE{
        ALIGN_NONE,
        ALIGN_TO_GRID,
    };

    // Constructors
public:
    CS3DrawObj();
    CS3DrawObj(const FRect logicPos, const CRect position, const UINT depth, const PROPERTY_TYPE propertyType);
    virtual ~CS3DrawObj();

    /// Attributes
    virtual UINT GetHandleCount();

    // Display tip info of the obj
    virtual void SetName(const CString szName) { m_szNameType = szName; };
    virtual void GetName(CString& szName) { szName = m_szNameType; };
	virtual void GetSerialName(CString &szName){szName = m_szSerialName;};
	virtual void SetSerialName(CString szName){m_szSerialName = szName;};
    virtual void SetTypeCount(const int nCount) { m_nCountType = nCount; };
    virtual void GetTypeCount(int& nCount) { nCount = m_nCountType; };

    /// in logic position
    virtual FPoint GetHandle(UINT nHandle);
    virtual FRect GetHandleRect(UINT nHandleID);
    virtual HCURSOR GetHandleCursor(UINT nHandle);
    virtual HCURSOR GetLineCursor(UINT nLine);

    virtual UINT GetDragHandle() { return m_nDragHandle; };
    virtual void SetDragHandle(UINT nDragHandle) { m_nDragHandle = nDragHandle; };
    virtual UINT GetDragLine() { return m_nDragLine; };
    virtual void SetDragLine(UINT nDragLine) { m_nDragLine = nDragLine; };

    /// in physical position
    virtual void GetPosition(CRect& position) { position = m_Position; };
    virtual void SetPosition(const CRect position) { m_Position = position; };

    /// in logic position
    virtual void GetLogicPos(FRect& rect) { rect = m_LogicPos; };
    virtual void SetLogicPos(const FRect rect);
    virtual void SetLogicPos(const CRect rect);
    virtual void GetDepth(UINT& depth) { depth = m_Depth; };
    virtual void GetPropertyType(PROPERTY_TYPE& propertyType) { propertyType = m_Type; };

    // Operations
    /// Get handle in physical position, need this function to help draw handle trackers
    virtual CPoint GetPhysicalHandle(UINT nHandle);

    /// in physical position
    virtual void Draw(CDC* pDC);
    virtual void DrawTracker(CDC* pDC);                     /// draw handle tracker, pDC - is the design wnd client paint dc
    virtual void HighlightEdgeLine(CDC* pDC);               /// Highlight edge line when the obj be selected

    /// in logic position
    virtual BOOL MoveTo(const FPoint& position);            /// Return False: same position, no move; Return True: move to the setting position
    virtual BOOL MoveTo(const FRect& positon);              /// Return False: same position, no move; Return True: move to the setting position
    virtual int HitTest(FPoint point, UINT& nHandle, UINT& nLine);
    virtual FRect MoveHandleTo(const UINT nHandle, const FPoint dstPoint, const FPoint originPoint, const FLOAT fScale, const FPoint lastPoint);
    virtual FRect MoveLineTo(const UINT nLine, const FPoint dstPoint, const FPoint originPoint, const FLOAT fScale, const FPoint lastPoint);
    virtual BOOL Intersects(const FRect& rect);

    /// Handle and Line physical size, for hit test and draw tracker
    virtual void SetHitTestSize(UINT nHandleSize, UINT nLineSize) { m_nHandleSize = nHandleSize; m_nLineSize = nLineSize; };
    virtual void GetHitHandleSize(UINT& nHandleSize) { nHandleSize = m_nHandleSize; };
    virtual void GetHitLineSize(UINT& nLineSize) { nLineSize = m_nLineSize; };

    /// Handle and Line logic size, for hit test and draw tracker
    virtual void SetHitTestLogicSize(UINT nHandleSize, UINT nLineSize) { m_nHandleLogicSize = nHandleSize; m_nLineLogicSize = nLineSize; };
    virtual void GetHitHandleLogicSize(UINT& nLogicSize) { nLogicSize = m_nHandleLogicSize; };
    virtual void GetHitLineLogicSize(UINT& nLogicSize) { nLogicSize = m_nLineLogicSize; };

    virtual void GetAspectRatio(FLOAT& fAspectRatio)    { fAspectRatio = m_fAspectRatio; };
    virtual void CalcAspectRatio();
    virtual void GetScale(FLOAT& fScale) { fScale = m_fScale; };
    virtual void CalcScale(const FPoint point, FLOAT fScale = 0.0f);
    virtual void CalcPosition(const FPoint origin, const FPoint last, FPoint& point, FLOAT fScale = 0.0f);
    virtual void CalcPoint(const FPoint origin, const FPoint last, const FPoint lastChange, FPoint& point);

    virtual void SetKeepAspect(BOOL v);
    virtual BOOL GetKeepAspect();

    virtual void SetLoopSync(BOOL bLoop) { m_bLoopSync = bLoop; };
    virtual void GetLoopSync(BOOL& bLoop) { bLoop = m_bLoopSync; };
	void SetLayerId(DWORD dwLayerId){m_dwLayerId = dwLayerId;};
	DWORD GetLayerId(){return m_dwLayerId;};
    // implementation data
protected:
    /// Attributes
	DWORD			m_dwLayerId;
    int             m_nCountType;       /// The obj type count
    CString         m_szNameType;       /// The obj type name
	CString			m_szSerialName;		/// The name from creating order.
    CRect           m_Position;         /// The physical position info in the draw wnd
    FRect           m_LogicPos;         /// The logical rect info set by user
    UINT            m_Depth;
    PROPERTY_TYPE   m_Type;
    UINT            m_Handle;
    UINT            m_nDragHandle;      /// Drag Handle in resizing the draw obj, the value start from left top, and increment clockwise, 0 = not drag handle
    UINT            m_nDragLine;        /// Drag line in resizing the draw obj, the value start from left top, and increment clockwise, 0 = not drag line
    UINT            m_nHandleSize;      /// The handle size to hit test handle and draw handle tracker, in logic coords
    UINT            m_nLineSize;        /// The line size to hit test line, and set line cursor, in logic coords
    UINT            m_nHandleLogicSize; /// The handle size to hit test handle and draw handle tracker, in physical coords
    UINT            m_nLineLogicSize;   /// The line size to hit test line, and set line cursor, in physical coords

    /// Calculate the point position in the keep aspect ratio mode
    FLOAT           m_fScale;           /// The max value of point's W/H and obj's W/H, in logic coordinate
    FLOAT           m_fAspectRatio;     /// The aspect ratio in logic coordinate

    /// Drawing Data
    BOOL            m_bPen;
    LOGPEN          m_logpen;
    BOOL            m_bBrush;
    LOGBRUSH        m_logbrush;

    // Layer size need keep aspect ratio
    BOOL            m_bKeepAspect;

    // Layer loop sync the layout or not
    BOOL            m_bLoopSync;

public:
    ALIGN_MODE      m_AlignMode;
    // logic point or rect
    CPoint          m_OrigPoint; // OrigPoint when mouse lb down.
    FRect           m_OrigPosition; /// The physical position info in the draw wnd when mouse lb down.
};

typedef CTypedPtrList<CObList, CS3DrawObj*> CS3DrawObjList;