/// F:\p4\sw\Demo\ContentManagement\Controller\Designer\S3DrawObj.cpp : implementation file
///

#include "StdAfx.h"
#include "S3DrawObj.h"


//IMPLEMENT_SERIAL(CS3DrawObj, CObject, 0)
IMPLEMENT_DYNAMIC(CS3DrawObj, CObject)

CS3DrawObj::CS3DrawObj():m_dwLayerId(0)
{
}

CS3DrawObj::~CS3DrawObj()
{
}

CS3DrawObj::CS3DrawObj(const FRect logicPos, const CRect position, const UINT depth, const PROPERTY_TYPE propertyType)
{
	m_dwLayerId				= 0;
    m_nCountType            = 0;
    m_szNameType            = _T("");
    m_LogicPos              = logicPos;
    m_Position              = position;
    m_Depth                 = depth;
    m_Type                  = propertyType;
    m_Handle                = 4;
    m_nDragHandle           = 0;
    m_nDragLine             = 0;
    m_nHandleSize           = 0;
    m_nLineSize             = 0;

    m_fScale                = 1.0f;
    m_fAspectRatio          = 1.0f;

    m_bPen                  = TRUE;
    m_logpen.lopnStyle      = PS_INSIDEFRAME;
    m_logpen.lopnWidth.x    = 1;
    m_logpen.lopnWidth.y    = 1;
    m_logpen.lopnColor      = RGB(0, 0, 0);

    m_bBrush                = TRUE;
    m_logbrush.lbStyle      = BS_SOLID;
    m_logbrush.lbColor      = RGB(192, 192, 192);
    m_logbrush.lbHatch      = HS_HORIZONTAL;

    m_bLoopSync             = FALSE;
    m_bKeepAspect           = FALSE;
    m_AlignMode             = ALIGN_NONE;
    m_OrigPoint             = CPoint(); 
    m_OrigPosition          = FRect();
}

/// In logic coordinate
BOOL CS3DrawObj::MoveTo(const FPoint& position)
{
    if (position.x == 0 && position.y == 0)
    {
        return FALSE;
    }

    //m_LogicPos.OffsetRect (position);
    m_LogicPos.MoveToXY(position);
    return TRUE;
}

/// In logic coordinate
BOOL CS3DrawObj::MoveTo(const FRect& position)
{
    if (position == m_LogicPos)
    {
        return FALSE;
    }

    m_LogicPos = position;
    return TRUE;
}

/// Note: return value: -1 = no hit, 0 = hit(anywhere), 
/// the handle return value to decide if hit the handle, 0 = no hit, > 0 = hit handle, start from the left top, and increment clockwise
/// the Line return value to decide if hit the rect line, 0 = no hit, > 0 = hit line, start from the left, and increment clockwise

/// In logic coordinate
int CS3DrawObj::HitTest(FPoint point, UINT& nHandle, UINT& nLine)
{

    return 0;
}

/// return the draw obj handle count
UINT CS3DrawObj::GetHandleCount()
{
    return m_Handle;
}

/// In logic coordinate
FPoint CS3DrawObj::GetHandle(UINT Handle)
{
    FPoint point;


    return point;
}

/// In logic coordinate
FRect CS3DrawObj::GetHandleRect(UINT nHandleID)
{
    FRect rect;


    return rect;
}


/// In physical coordinate
CPoint CS3DrawObj::GetPhysicalHandle(UINT nHandle)
{
    CPoint point;

    return point;
}

void CS3DrawObj::Draw(CDC* pDC)
{

}

void CS3DrawObj::DrawTracker(CDC* pDC)
{

}

void CS3DrawObj::HighlightEdgeLine(CDC* pDC)
{

}

/// In logic coordinate
FRect CS3DrawObj::MoveHandleTo(const UINT nHandle, const FPoint dstPoint, const FPoint originPoint, const FLOAT fScale, const FPoint lastPoint)
{
    FRect rect;
    rect.SetRectEmpty();

    return rect;
}

/// In logic coordinate
FRect CS3DrawObj::MoveLineTo(const UINT nLine, const FPoint dstPoint, const FPoint originPoint, const FLOAT fScale, const FPoint lastPoint)
{
    FRect rect;
    rect.SetRectEmpty();

    return rect;
}

/// In logic coordinate
BOOL CS3DrawObj::Intersects(const FRect& rect)
{

    return TRUE;
}

HCURSOR CS3DrawObj::GetHandleCursor(UINT nHandle)
{

    return AfxGetApp()->LoadStandardCursor(0);
}

HCURSOR CS3DrawObj::GetLineCursor(UINT nLine)
{

    return AfxGetApp()->LoadStandardCursor(0);
}


void CS3DrawObj::SetLogicPos(const FRect rect)
{
    m_LogicPos = rect;
}

void CS3DrawObj::SetLogicPos(const CRect rect)
{
    FRect rectF(rect);
    m_LogicPos = rectF;
}

void CS3DrawObj::CalcScale(const FPoint point, FLOAT fScale)
{
    m_fScale = 1.0f;
}

void CS3DrawObj::CalcAspectRatio()
{
    m_fAspectRatio = 1.0f;
}

void CS3DrawObj::CalcPosition(const FPoint origin, const FPoint last, FPoint& point, FLOAT fScale)
{

}

void CS3DrawObj::CalcPoint(const FPoint origin, const FPoint last, const FPoint lastChange, FPoint& point)
{

}

void CS3DrawObj::SetKeepAspect(BOOL v)
{
    m_bKeepAspect = v;
}

BOOL CS3DrawObj::GetKeepAspect()
{
    return m_bKeepAspect;
}
