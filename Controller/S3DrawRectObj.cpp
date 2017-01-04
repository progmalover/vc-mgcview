/// F:\p4\sw\Demo\ContentManagement\Controller\Designer\S3DrawRectObj.cpp : implementation file
///

#include "StdAfx.h"
#include "S3DrawRectObj.h"



IMPLEMENT_DYNAMIC(CS3DrawRectObj, CS3DrawObj)

CS3DrawRectObj::CS3DrawRectObj()
{

}

CS3DrawRectObj::~CS3DrawRectObj()
{
}

CS3DrawRectObj::CS3DrawRectObj(const FRect logicPos, const CRect position, const UINT depth, const PROPERTY_TYPE propertyType)
    : CS3DrawObj(logicPos, position, depth, propertyType)
{
    m_logpen.lopnColor  = RGB(0, 0, 0);                  /// Line color: black

    m_logbrush.lbColor  = RGB(167, 224, 180);            /// Rect color

    m_Handle            = 8;
    m_nDragHandle       = 0;
    m_nDragLine         = 0;

    m_nHandleSize       = 8;
    m_nLineSize         = 4;
    m_nHandleLogicSize  = 8;
    m_nLineLogicSize    = 4;

}

/// Draw in physical coordinate
void CS3DrawRectObj::Draw(CDC* pDC)
{
    CBrush brush;
    if (!brush.CreateBrushIndirect(&m_logbrush))
    {
        return;
    }

    CPen pen;
    if (!pen.CreatePenIndirect(&m_logpen))
    {
        return;
    }

    CBrush* pOldBrush;
    CPen* pOldPen;

    if (m_bBrush)
    {
        pOldBrush = pDC->SelectObject(&brush);
    }
    else
    {
        pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
    }

    if (m_bPen)
    {
        pOldPen = pDC->SelectObject(&pen);
    }
    else
    {
        pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);
    }

    CRect rect = m_Position;
    pDC->Rectangle(rect);

    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);

}

/// Draw in physical coordinate
void CS3DrawRectObj::DrawTracker(CDC* pDC)
{
    UINT nHandleCount = GetHandleCount();
    int x, y;
    int left, top, right, bottom;

    left    = m_Position.left;
    right   = m_Position.right;
    top     = m_Position.top;
    bottom  = m_Position.bottom;

    for (UINT nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
    {
        CPoint handle = GetPhysicalHandle(nHandle);
        if (handle.x == left)
        {
            x = handle.x;
        }
        else if (handle.x == right)
        {
            x = handle.x - m_nHandleSize;
        }
        else
        {
            x = handle.x - m_nHandleSize / 2;
        }

        if (handle.y == top)
        {
            y = handle.y;
        }
        else if (handle.y == bottom)
        {
            y = handle.y - m_nHandleSize;
        }
        else
        {
            y = handle.y - m_nHandleSize / 2;
        }

        pDC->PatBlt(x, y, m_nHandleSize, m_nHandleSize, DSTINVERT);
    }
}

void CS3DrawRectObj::HighlightEdgeLine(CDC* pDC)
{
    COLORREF highlightLine = RGB(255, 0, 255);
    CPen penLine;
    penLine.CreatePen(PS_SOLID, 2, highlightLine);
    pDC->SelectObject(&penLine);

    CRect rect = m_Position;
    pDC->MoveTo(rect.left, rect.top);
    pDC->LineTo(rect.right, rect.top);
    pDC->LineTo(rect.right, rect.bottom);
    pDC->LineTo(rect.left, rect.bottom);
    pDC->LineTo(rect.left, rect.top);

}

/// In logic coordinate
BOOL CS3DrawRectObj::Intersects(const FRect& rect)
{
    FRect fixed = m_LogicPos;
    FRect rectT = rect;
    rectT.NormalizeRect();
    return !(rectT & fixed).IsRectEmpty();
}

/// Note: return value: -1 = no hit, 0 = hit(anywhere), 
/// the handle return value to decide if hit the handle, 0 = no hit, > 0 = hit handle, start from the left top, and increment clockwise
/// the Line return value to decide if hit the rect line, 0 = no hit, > 0 = hit line, start from the left, and increment clockwise

/// In logic coordinate
int CS3DrawRectObj::HitTest(FPoint point, UINT& nHandle, UINT& nLine)
{
    int nHit = -1;
    UINT nHandleCount = GetHandleCount();
    nHandle = 0;
    nLine   = 0;
    

    for (UINT i = 1; i <= m_Handle; i += 1)
    {
        // GetHandleRect returns in logic coordinate
        FRect rc = GetHandleRect(i);
        if (point.x >= rc.left && point.x <= rc.right && point.y >= rc.top && point.y <= rc.bottom)
        {
            nHandle = i;
            break;
        }
    }

    // The handle has the higher hit priority, if the hit the handle, then skip the line hit test
    if (nHandle > 0)
    {
        return 0;
    }

    if (point.x >= m_LogicPos.left && point.x <= m_LogicPos.right && point.y > m_LogicPos.top && point.y <= (int)(m_LogicPos.top + m_nLineLogicSize))
    {
        nLine = 1;
    }
    else if (point.x >= (int)(m_LogicPos.right - m_nLineLogicSize) && point.x < m_LogicPos.right && point.y >= m_LogicPos.top && point.y <= m_LogicPos.bottom)
    {
        nLine = 2;
    }
    else if (point.x >= m_LogicPos.left && point.x <= m_LogicPos.right && point.y >= (int)(m_LogicPos.bottom - m_nLineLogicSize) && point.y < m_LogicPos.bottom)
    {
        nLine = 3;
    }
    else if (point.x > m_LogicPos.left && point.x <= (int)(m_LogicPos.left + m_nLineLogicSize) && point.y >= m_LogicPos.top && point.y <= m_LogicPos.bottom)
    {
        nLine = 4;
    }

    if (point.x > m_LogicPos.left && point.x < m_LogicPos.right && point.y > m_LogicPos.top && point.y < m_LogicPos.bottom)
    {
        return 0;
    }

    return -1;
}

/// In physical coordinate, return the handle's point
CPoint CS3DrawRectObj::GetPhysicalHandle(UINT nHandle)
{
    int x, y, xCenter, yCenter;

    //this gets the center regardless of left/right and top/bottom ordering
    xCenter = m_Position.left + m_Position.Width() / 2;
    yCenter = m_Position.top + m_Position.Height() / 2;

    switch (nHandle)
    {
    default:
        ASSERT(FALSE);

    case 1:
        x = m_Position.left;
        y = m_Position.top;
        break;

    case 2:
        x = xCenter;
        y = m_Position.top;
        break;

    case 3:
        x = m_Position.right;
        y = m_Position.top;
        break;

    case 4:
        x = m_Position.right;
        y = yCenter;
        break;

    case 5:
        x = m_Position.right;
        y = m_Position.bottom;
        break;

    case 6:
        x = xCenter;
        y = m_Position.bottom;
        break;

    case 7:
        x = m_Position.left;
        y = m_Position.bottom;
        break;

    case 8:
        x = m_Position.left;
        y = yCenter;
        break;
    }

    return CPoint(x, y);
}

/// In logic coordinate
FPoint CS3DrawRectObj::GetHandle(UINT nHandle)
{
    float x, y, xCenter, yCenter;

    xCenter = m_LogicPos.left + m_LogicPos.Width() / 2;
    yCenter = m_LogicPos.top + m_LogicPos.Height() / 2;

    switch (nHandle)
    {
    default:
        ASSERT(FALSE);

    case 1:
        x = m_LogicPos.left;
        y = m_LogicPos.top;
        break;

    case 2:
        x = xCenter;
        y = m_LogicPos.top;
        break;

    case 3:
        x = m_LogicPos.right;
        y = m_LogicPos.top;
        break;

    case 4:
        x = m_LogicPos.right;
        y = yCenter;
        break;

    case 5:
        x = m_LogicPos.right;
        y = m_LogicPos.bottom;
        break;

    case 6:
        x = xCenter;
        y = m_LogicPos.bottom;
        break;

    case 7:
        x = m_LogicPos.left;
        y = m_LogicPos.bottom;
        break;

    case 8:
        x = m_LogicPos.left;
        y = yCenter;
        break;
    }

    return FPoint(x, y);
}

/// In logic coordinate, return the handle's tracker rect
FRect CS3DrawRectObj::GetHandleRect(UINT nHandleID)
{
    FRect rect;

    FPoint point = GetHandle(nHandleID);

    switch (nHandleID)
    {
    default:
        ASSERT(FALSE);

    /// Top Left
    case 1:
        rect.SetRect(point.x, point.y, point.x + m_nHandleLogicSize, point.y + m_nHandleLogicSize);
        break;

    /// Top Center
    case 2:
        rect.SetRect(point.x - m_nHandleLogicSize / 2, point.y, point.x + m_nHandleLogicSize / 2, point.y + m_nHandleLogicSize);
        break;

    /// Top Right
    case 3:
        rect.SetRect(point.x - m_nHandleLogicSize, point.y, point.x, point.y + m_nHandleLogicSize);
        break;

    /// Right Center
    case 4:
        rect.SetRect(point.x - m_nHandleLogicSize, point.y - m_nHandleLogicSize / 2, point.x, point.y + m_nHandleLogicSize / 2);
        break;

    /// Right Bottom
    case 5:
        rect.SetRect(point.x - m_nHandleLogicSize, point.y - m_nHandleLogicSize, point.x, point.y);
        break;

    /// Bottom Center
    case 6:
        rect.SetRect(point.x - m_nHandleLogicSize / 2, point.y - m_nHandleLogicSize, point.x + m_nHandleLogicSize / 2, point.y);
        break;

    /// Bottom Left
    case 7:
        rect.SetRect(point.x, point.y - m_nHandleLogicSize, point.x + m_nHandleLogicSize, point.y);
        break;

    /// Left Center
    case 8:
        rect.SetRect(point.x, point.y - m_nHandleLogicSize / 2, point.x + m_nHandleLogicSize, point.y + m_nHandleLogicSize / 2);
        break;
    }

    return rect;
}


HCURSOR CS3DrawRectObj::GetHandleCursor(UINT nHandle)
{
    LPCTSTR id;
    switch (nHandle)
    {
    default:
        ASSERT(FALSE);

    case 1:
    case 5:
        id = IDC_SIZENWSE;
        break;

    case 2:
    case 6:
        id = IDC_SIZENS;
        break;

    case 3:
    case 7:
        id = IDC_SIZENESW;
        break;

    case 4:
    case 8:
        id = IDC_SIZEWE;
        break;
    }

    return AfxGetApp()->LoadStandardCursor(id);
}

HCURSOR CS3DrawRectObj::GetLineCursor(UINT nLine)
{
    LPCTSTR id;
    switch (nLine)
    {
    default:
        ASSERT(FALSE);

    case 1:
    case 3:
        id = IDC_SIZENS;
        break;

    case 2:
    case 4:
        id = IDC_SIZEWE;
        break;
    }

    return AfxGetApp()->LoadStandardCursor(id);
}

/// In logic coordinate, the min size should be 1
/// If fScale == 0.0f, just move the obj by itself, no use the originPoint, lastPoint
/// If fScale != 0.0f, the obj is inside the select obj, use the originPoint, lastPoint
FRect CS3DrawRectObj::MoveHandleTo(const UINT nHandle, const FPoint dstPoint, const FPoint originPoint, const FLOAT fScale, const FPoint lastPoint)
{
    BOOL bChange = TRUE;
    FRect position = m_LogicPos;
    FPoint origin = originPoint;
    FPoint delta = (FPoint)(dstPoint - origin);
    FPoint lefttop(position.left, position.top);
    FPoint righttop(position.right, position.top);
    FPoint leftbottom(position.left, position.bottom);
    FPoint rightbottom(position.right, position.bottom);
    FPoint last = lastPoint;
    FPoint dst = dstPoint;

    switch (nHandle)
    {
    default:
        ASSERT(FALSE);

        /// Top Left
    case 1:
        {
            if (fScale == 0.0f)
            {
                origin.SetPoint(m_LogicPos.right, m_LogicPos.bottom);
                delta = (FPoint)(dstPoint - origin);
            }

            if (delta.x >= 0 && delta.y >= 0)
            {
                bChange = FALSE;
            }

            if (bChange)
            {
                if (m_bKeepAspect)
                {
                    if (fScale != 0.0f)
                    {
                        CalcLeftTopPosition(origin, last, dst, fScale);
                        CalcPoint(origin, last, dst, lefttop);
                        CalcPoint(origin, last, dst, rightbottom);

                        position.left   = lefttop.x;
                        position.top    = lefttop.y;
                        position.right  = rightbottom.x;
                        position.bottom = rightbottom.y;
                    }
                    else
                    {
                        CalcLeftTopPosition(origin, lefttop, dst, 0.0f);
                        position.left   = dst.x;
                        position.top    = dst.y;
                    }
                }
                else
                {
                    position.left = origin.x + delta.x;
                    position.top = origin.y + delta.y;
                }

                position.left = ((position.right - 1) <= position.left) ? position.right - 1 : position.left;
                position.top = ((position.bottom - 1) <= position.top) ? position.bottom - 1 : position.top;
            }
        }
        break;

        /// Top Center
    case 2:
        {
            if (fScale == 0.0f)
            {
                origin.SetPoint(m_LogicPos.left, m_LogicPos.bottom);
                delta = (FPoint)(dstPoint - origin);
                delta.x = 0;
            }

            if (delta.y >= 0)
            {
                bChange = FALSE;
            }

            if (bChange)
            {
                if (m_bKeepAspect)
                {
                    if (fScale != 0.0f)
                    {
                        CalcVPosition(origin, last, dst, fScale);
                        CalcPoint(origin, last, dst, leftbottom);
                        CalcPoint(origin, last, dst, righttop);

                        position.left   = leftbottom.x;
                        position.top    = righttop.y;
                        position.right  = righttop.x;
                        position.bottom = leftbottom.y;
                    }
                    else
                    {
                        CalcVPosition(origin, righttop, dst, 0.0f);
                        position.right  = dst.x;
                        position.top    = dst.y;
                    }
                }
                else
                {
                    position.top = origin.y + delta.y;
                }

                position.top = ((position.bottom - 1) <= position.top) ? position.bottom - 1 : position.top;
                position.right  = ((position.left + 1) >= position.right) ? position.left + 1 : position.right;
            }
        }
        break;

        /// Top Right
    case 3:
        {
            if (fScale == 0.0f)
            {
                origin.SetPoint(m_LogicPos.left, m_LogicPos.bottom);
                delta = (FPoint)(dstPoint - origin);
            }

            if (delta.x <= 0 && delta.y >= 0)
            {
                bChange = FALSE;
            }

            if (bChange)
            {
                if (m_bKeepAspect)
                {
                    if (fScale != 0.0f)
                    {
                        CalcRightTopPosition(origin, last, dst, fScale);
                        CalcPoint(origin, last, dst, leftbottom);
                        CalcPoint(origin, last, dst, righttop);

                        position.left   = leftbottom.x;
                        position.top    = righttop.y;
                        position.right  = righttop.x;
                        position.bottom = leftbottom.y;
                    }
                    else
                    {
                        CalcRightTopPosition(origin, righttop, dst, 0.0f);
                        position.right  = dst.x;
                        position.top    = dst.y;
                    }
                }
                else
                {
                    position.right  = origin.x + delta.x;
                    position.top    = origin.y + delta.y;
                }

                position.right  = ((position.left + 1) >= position.right) ? position.left + 1 : position.right;
                position.top    = ((position.bottom - 1) <= position.top) ? position.bottom - 1 : position.top;
            }
        }
        break;

        /// Right Center
    case 4:
        {
            if (fScale == 0.0f)
            {
                origin.SetPoint(m_LogicPos.left, m_LogicPos.top);
                delta = (FPoint)(dstPoint - origin);
                delta.y = 0;
            }

            if (delta.x <= 1)
            {
                bChange = FALSE;
            }

            if (bChange)
            {
                if (m_bKeepAspect)
                {
                    if (fScale != 0.0f)
                    {
                        CalcHPosition(origin, last, dst, fScale);
                        CalcPoint(origin, last, dst, lefttop);
                        CalcPoint(origin, last, dst, rightbottom);

                        position.left   = lefttop.x;
                        position.top    = lefttop.y;
                        position.right  = rightbottom.x;
                        position.bottom = rightbottom.y;
                    }
                    else
                    {
                        CalcHPosition(origin, rightbottom, dst, 0.0f);
                        position.right  = dst.x;
                        position.bottom = dst.y;
                    }
                }
                else
                {
                    position.right  = origin.x + delta.x;
                }

                position.right  = ((position.left + 1) >= position.right) ? position.left + 1 : position.right;
                position.bottom = ((position.top + 1) >= position.bottom) ? position.top + 1 : position.bottom;
            }
        }
        break;

        /// Right Bottom
    case 5:
        {
            if (fScale == 0.0f)
            {
                origin.SetPoint(m_LogicPos.left, m_LogicPos.top);
                delta = (FPoint)(dstPoint - origin);
            }

            if (delta.x <= 0 && delta.y <= 0)
            {
                bChange = FALSE;
            }

            if (bChange)
            {
                if (m_bKeepAspect)
                {
                    if (fScale != 0.0f)
                    {
                        CalcPosition(origin, last, dst, fScale);
                        CalcPoint(origin, last, dst, lefttop);
                        CalcPoint(origin, last, dst, rightbottom);

                        position.left   = lefttop.x;
                        position.top    = lefttop.y;
                        position.right  = rightbottom.x;
                        position.bottom = rightbottom.y;
                    }
                    else
                    {
                        CalcPosition(origin, rightbottom, dst, 0.0f);
                        position.right  = dst.x;
                        position.bottom = dst.y;
                    }
                }
                else
                {
                    position.right  = origin.x + delta.x;
                    position.bottom = origin.y + delta.y;
                }

                position.right  = ((position.left + 1) >= position.right) ? position.left + 1 : position.right;
                position.bottom = ((position.top + 1) >= position.bottom) ? position.top + 1 : position.bottom;
            }
        }
        break;

        /// Bottom Center
    case 6:
        {
            if (fScale == 0.0f)
            {
                origin.SetPoint(m_LogicPos.left, m_LogicPos.top);
                delta = (FPoint)(dstPoint - origin);
                delta.x = 0;
            }

            if (delta.y <= 1)
            {
                bChange = FALSE;
            }

            if (bChange)
            {
                if (m_bKeepAspect)
                {
                    if (fScale != 0.0f)
                    {
                        CalcVPosition(origin, last, dst, fScale);
                        CalcPoint(origin, last, dst, lefttop);
                        CalcPoint(origin, last, dst, rightbottom);

                        position.left   = lefttop.x;
                        position.top    = lefttop.y;
                        position.right  = rightbottom.x;
                        position.bottom = rightbottom.y;
                    }
                    else
                    {
                        CalcVPosition(origin, rightbottom, dst, 0.0f);
                        position.right  = dst.x;
                        position.bottom = dst.y;
                    }
                }
                else
                {
                    position.bottom = origin.y + delta.y;
                }

                position.bottom = ((position.top + 1) >= position.bottom) ? position.top + 1 : position.bottom;
                position.right  = ((position.left + 1) >= position.right) ? position.left + 1 : position.right;
            }
        }
        break;

        /// Bottom Left
    case 7:
        {
            if (fScale == 0.0f)
            {
                origin.SetPoint(m_LogicPos.right, m_LogicPos.top);
                delta = (FPoint)(dstPoint - origin);
            }

            if (delta.x >= 0 && delta.y <= 0)
            {
                bChange = FALSE;
            }

            if (bChange)
            {
                if (m_bKeepAspect)
                {
                    if (fScale != 0.0f)
                    {
                        CalcLeftBottomPosition(origin, last, dst, fScale);
                        CalcPoint(origin, last, dst, righttop);
                        CalcPoint(origin, last, dst, leftbottom);

                        position.left   = leftbottom.x;
                        position.top    = righttop.y;
                        position.right  = righttop.x;
                        position.bottom = leftbottom.y;
                    }
                    else
                    {
                        CalcLeftBottomPosition(origin, leftbottom, dst, 0.0f);
                        position.left   = dst.x;
                        position.bottom = dst.y;
                    }
                }
                else
                {
                    position.left   = origin.x + delta.x;
                    position.bottom = origin.y + delta.y;
                }

                position.left   = ((position.right - 1) <= position.left) ? position.right - 1 : position.left;
                position.bottom = ((position.top + 1) >= position.bottom) ? position.top + 1 : position.bottom;
            }
        }
        break;

        /// Left Center
    case 8:
        {
            if (fScale == 0.0f)
            {
                origin.SetPoint(m_LogicPos.right, m_LogicPos.top);
                delta = (FPoint)(dstPoint - origin);
                delta.y = 0;
            }

            if (delta.x >= 0)
            {
                bChange = FALSE;
            }

            if (bChange)
            {
                if (m_bKeepAspect)
                {
                    if (fScale != 0.0f)
                    {
                        CalcHPosition(origin, last, dst, fScale);
                        CalcPoint(origin, last, dst, righttop);
                        CalcPoint(origin, last, dst, leftbottom);

                        position.left   = leftbottom.x;
                        position.top    = righttop.y;
                        position.right  = righttop.x;
                        position.bottom = leftbottom.y;
                    }
                    else
                    {
                        CalcHPosition(origin, leftbottom, dst, 0.0f);
                        position.left   = dst.x;
                        position.bottom = dst.y;
                    }
                }
                else
                {
                    position.left   = origin.x + delta.x;
                }

                position.left = ((position.right - 1) <= position.left) ? position.right - 1 : position.left;
                position.bottom = ((position.top + 1) >= position.bottom) ? position.top + 1 : position.bottom;
            }
        }
        break;
    }

    m_LogicPos = position;
    return position;
}

/// In logic coordinate, the min size should be 1, the parameter point is the moving delta point
/// If fScale == 0.0f, just move the obj by itself, no use the originPoint, lastPoint
/// If fScale != 0.0f, the obj is inside the select obj, use the originPoint, lastPoint
FRect CS3DrawRectObj::MoveLineTo(const UINT nLine, const FPoint dstPoint, const FPoint originPoint, const FLOAT fScale, const FPoint lastPoint)
{
    BOOL bChange = TRUE;
    FRect position = m_LogicPos;
    FPoint origin = originPoint;
    FPoint delta = (FPoint)(dstPoint - origin);
    FPoint lefttop(position.left, position.top);
    FPoint righttop(position.right, position.top);
    FPoint leftbottom(position.left, position.bottom);
    FPoint rightbottom(position.right, position.bottom);
    FPoint last = lastPoint;
    FPoint dst = dstPoint;

    switch (nLine)
    {
    default:
        ASSERT(FALSE);

    /// Top line
    case 1:
        {
            if (fScale == 0.0f)
            {
                origin.SetPoint(m_LogicPos.left, m_LogicPos.bottom);
                delta = (FPoint)(dstPoint - origin);
            }
            delta.x = 0;

            if (delta.y >= 0)
            {
                bChange = FALSE;
            }

            if (bChange)
            {
                if (m_bKeepAspect)
                {
                    if (fScale != 0.0f)
                    {
                        CalcVPosition(origin, last, dst, fScale);
                        CalcPoint(origin, last, dst, leftbottom);
                        CalcPoint(origin, last, dst, righttop);

                        position.left   = leftbottom.x;
                        position.top    = righttop.y;
                        position.right  = righttop.x;
                        position.bottom = leftbottom.y;
                    }
                    else
                    {
                        CalcVPosition(origin, righttop, dst, 0.0f);
                        position.right  = dst.x;
                        position.top    = dst.y;
                    }
                }
                else
                {
                    position.top = origin.y + delta.y;
                }

                position.top = ((position.bottom - 1) <= position.top) ? position.bottom - 1 : position.top;
                position.right  = ((position.left + 1) >= position.right) ? position.left + 1 : position.right;
            }
        }
        break;

    /// Right line
    case 2:
        {
            if (fScale == 0.0f)
            {
                origin.SetPoint(m_LogicPos.left, m_LogicPos.top);
                delta = (FPoint)(dstPoint - origin);
            }
            delta.y = 0;

            if (delta.x < 1)
            {
                bChange = FALSE;
            }

            if (bChange)
            {
                if (m_bKeepAspect)
                {
                    if (fScale != 0.0f)
                    {
                        CalcHPosition(origin, last, dst, fScale);
                        CalcPoint(origin, last, dst, lefttop);
                        CalcPoint(origin, last, dst, rightbottom);

                        position.left   = lefttop.x;
                        position.top    = lefttop.y;
                        position.right  = rightbottom.x;
                        position.bottom = rightbottom.y;
                    }
                    else
                    {
                        CalcHPosition(origin, rightbottom, dst, 0.0f);
                        position.right  = dst.x;
                        position.bottom = dst.y;
                    }
                }
                else
                {
                    position.right  = origin.x + delta.x;
                }

                position.right = ((position.left + 1) >= position.right) ? position.left + 1 : position.right;
                position.bottom = ((position.top + 1) >= position.bottom) ? position.top + 1 : position.bottom;
            }
        }
        break;

    /// Bottom line
    case 3:
        {
            if (fScale == 0.0f)
            {
                origin.SetPoint(m_LogicPos.left, m_LogicPos.top);
                delta = (FPoint)(dstPoint - origin);
            }
            delta.x = 0;

            if (delta.y < 1)
            {
                bChange = FALSE;
            }

            if (bChange)
            {
                if (m_bKeepAspect)
                {
                    if (fScale != 0.0f)
                    {
                        CalcVPosition(origin, last, dst, fScale);
                        CalcPoint(origin, last, dst, lefttop);
                        CalcPoint(origin, last, dst, rightbottom);

                        position.left   = lefttop.x;
                        position.top    = lefttop.y;
                        position.right  = rightbottom.x;
                        position.bottom = rightbottom.y;
                    }
                    else
                    {
                        CalcVPosition(origin, rightbottom, dst, 0.0f);
                        position.right  = dst.x;
                        position.bottom = dst.y;
                    }
                }
                else
                {
                    position.bottom = origin.y + delta.y;
                }

                position.bottom = ((position.top + 1) >= position.bottom) ? position.top + 1 : position.bottom;
                position.right = ((position.left + 1) >= position.right) ? position.left + 1 : position.right;
            }
        }
        break;

    /// Left line
    case 4:
        {
            if (fScale == 0.0f)
            {
                origin.SetPoint(m_LogicPos.right, m_LogicPos.top);
                delta = (FPoint)(dstPoint - origin);
            }
            delta.y = 0;

            if (delta.x >= 0)
            {
                bChange = FALSE;
            }

            if (bChange)
            {
                if (m_bKeepAspect)
                {
                    if (fScale != 0.0f)
                    {
                        CalcHPosition(origin, last, dst, fScale);
                        CalcPoint(origin, last, dst, righttop);
                        CalcPoint(origin, last, dst, leftbottom);

                        position.left   = leftbottom.x;
                        position.top    = righttop.y;
                        position.right  = righttop.x;
                        position.bottom = leftbottom.y;
                    }
                    else
                    {
                        CalcHPosition(origin, leftbottom, dst, 0.0f);
                        position.left   = dst.x;
                        position.bottom = dst.y;
                    }
                }
                else
                {
                    position.left   = origin.x + delta.x;
                }

                position.left = ((position.right - 1) <= position.left) ? position.right - 1 : position.left;
                position.bottom = ((position.top + 1) >= position.bottom) ? position.top + 1 : position.bottom;
            }
        }
        break;
    }

    m_LogicPos = position;

    return position;
}

void CS3DrawRectObj::SetLineColor(COLORREF color)
{
    m_logpen.lopnColor = color;

}

void CS3DrawRectObj::SetFillColor(COLORREF color)
{
    m_logbrush.lbColor = color;

}

void CS3DrawRectObj::SetLineWeight(int nWidth)
{
    if (m_bPen)
    {
        m_bPen = nWidth > 0;
    }

    if (m_bPen)
    {
        m_logpen.lopnWidth.x = nWidth;
        m_logpen.lopnWidth.y = nWidth;
    }

}

void CS3DrawRectObj::EnableFill(BOOL bEnable)
{
    m_bBrush = bEnable;

}

void CS3DrawRectObj::EnableLine(BOOL bEnable)
{
    m_bPen = bEnable;

}

void CS3DrawRectObj::CalcScale(const FPoint point, FLOAT fScale)
{
    FLOAT fW = point.x;
    FLOAT fH = point.y;
    if (point.x < 0)
    {
        fW = 0 - point.x;
    }
    if (point.y < 0)
    {
        fH = 0 - point.y;
    }

    if (fScale != -1.0f)
    {
        m_fScale = fScale;
    }
    else
    {
        m_fScale = max(fW / m_LogicPos.Width(), fH / m_LogicPos.Height());
    }
}

void CS3DrawRectObj::CalcAspectRatio()
{
    m_fAspectRatio = m_LogicPos.Width() / m_LogicPos.Height();
}

void CS3DrawRectObj::CalcPosition(const FPoint origin, const FPoint last, FPoint& point, FLOAT fScale)
{
    FLOAT fW = point.x - origin.x;
    FLOAT fH = point.y - origin.y;

    if (fScale == 0.0f)
    {
        fScale = m_fAspectRatio;
    }

    if ((fW / m_LogicPos.Width()) > (fH / m_LogicPos.Height()))
    {
        point.y = fW / fScale + origin.y;
    }
    else
    {
        point.x = fH * fScale + origin.x;
    }

}

void CS3DrawRectObj::CalcVPosition(const FPoint origin, const FPoint last, FPoint& point, FLOAT fScale)
{
    if (fScale == 0.0f)
    {
        fScale = m_fAspectRatio;
    }

    point.x = abs(point.y - origin.y) * fScale + origin.x;

}

void CS3DrawRectObj::CalcHPosition(const FPoint origin, const FPoint last, FPoint& point, FLOAT fScale)
{
    if (fScale == 0.0f)
    {
        fScale = m_fAspectRatio;
    }

    point.y = abs(point.x - origin.x) / fScale + origin.y;

}

void CS3DrawRectObj::CalcRightTopPosition(const FPoint origin, const FPoint last, FPoint& point, FLOAT fScale)
{
    FLOAT fW = point.x - origin.x;
    FLOAT fH = point.y - origin.y;

    if (fScale == 0.0f)
    {
        fScale = m_fAspectRatio;
    }

    if ((abs(fW) / m_LogicPos.Width()) > (abs(fH) / m_LogicPos.Height()))
    {
        if (fW <= 0)
        {
            point.x = origin.x - fH * fScale;
        }
        else
        {
            point.y = origin.y - fW / fScale;
        }
    }
    else
    {
        if (fH >= 0)
        {
            point.y = origin.y - fW / fScale;
        }
        else
        {
            point.x = origin.x - fH * fScale;
        }
    }

}

void CS3DrawRectObj::CalcLeftBottomPosition(const FPoint origin, const FPoint last, FPoint& point, FLOAT fScale)
{
    FLOAT fW = point.x - origin.x;
    FLOAT fH = point.y - origin.y;

    if (fScale == 0.0f)
    {
        fScale = m_fAspectRatio;
    }

    if ((abs(fW) / m_LogicPos.Width()) > (abs(fH) / m_LogicPos.Height()))
    {
        if (fW >= 0)
        {
            point.x = origin.x - fH * fScale;
        }
        else
        {
            point.y = origin.y - fW / fScale;
        }
    }
    else
    {
        if (fH >= 0)
        {
            point.x = origin.x - fH * fScale;
        }
        else
        {
            point.y = origin.y - fW / fScale;
        }
    }

}

void CS3DrawRectObj::CalcLeftTopPosition(const FPoint origin, const FPoint last, FPoint& point, FLOAT fScale)
{
    FLOAT fW = point.x - origin.x;
    FLOAT fH = point.y - origin.y;

    if (fScale == 0.0f)
    {
        fScale = m_fAspectRatio;
    }

    if ((abs(fW) / m_LogicPos.Width()) > (abs(fH) / m_LogicPos.Height()))
    {
        if ( fW <= 0)
        {
            point.y = origin.y + fW / fScale;
        }
        else
        {
            point.x = origin.x + fH * fScale;
        }
    }
    else
    {
        if (fH <= 0)
        {
            point.x = origin.x + fH * fScale;
        }
        else
        {
            point.y = origin.y + fW / fScale;
        }
    }

}

void CS3DrawRectObj::CalcPoint(const FPoint origin, const FPoint last, const FPoint lastChange, FPoint& point)
{
    point.x = origin.x + ((point.x - origin.x) / (last.x - origin.x)) * (lastChange.x - origin.x);
    point.y = origin.y + ((point.y - origin.y) / (last.y - origin.y)) * (lastChange.y - origin.y);
}