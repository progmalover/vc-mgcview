#pragma once

/////////////////////////////////////////////////////////////////////////////
/// CS3DrawRectObj - draw Rectagles

#include "S3DrawObj.h"


class CS3DrawRectObj : public CS3DrawObj
{
protected:
    DECLARE_DYNAMIC(CS3DrawRectObj);

    // Constructors
public:
    CS3DrawRectObj();
    CS3DrawRectObj(const FRect logicPos, const CRect position, const UINT depth, const PROPERTY_TYPE propertyType);
    virtual ~CS3DrawRectObj();

    /// Attributes
    virtual FPoint GetHandle(UINT nHandle);
    virtual HCURSOR GetHandleCursor(UINT nHandle);
    virtual HCURSOR GetLineCursor(UINT nLine);
    virtual FRect GetHandleRect(UINT nHandleID);
    virtual int HitTest(FPoint point, UINT& nHandle, UINT& nLine);

    virtual void CalcAspectRatio();
    virtual void CalcScale(const FPoint point, FLOAT fScale = -1.0f);
    virtual void CalcPosition(const FPoint origin, const FPoint last, FPoint& point, FLOAT fScale = 0.0f);
    virtual void CalcPoint(const FPoint origin, const FPoint last, const FPoint lastChange, FPoint& point);
    virtual void CalcVPosition(const FPoint origin, const FPoint last, FPoint& point, FLOAT fScale = 0.0f);
    virtual void CalcHPosition(const FPoint origin, const FPoint last, FPoint& point, FLOAT fScale = 0.0f);
    virtual void CalcRightTopPosition(const FPoint origin, const FPoint last, FPoint& point, FLOAT fScale = 0.0f);
    virtual void CalcLeftBottomPosition(const FPoint origin, const FPoint last, FPoint& point, FLOAT fScale = 0.0f);
    virtual void CalcLeftTopPosition(const FPoint origin, const FPoint last, FPoint& point, FLOAT fScale = 0.0f);

    // Operations
    virtual void Draw(CDC* pDC);
    virtual void DrawTracker(CDC* pDC);
    virtual void HighlightEdgeLine(CDC* pDC);
    virtual FRect MoveHandleTo(const UINT nHandle, const FPoint dstPoint, const FPoint originPoint, const FLOAT fScale, const FPoint lastPoint);
    virtual FRect MoveLineTo(const UINT nLine, const FPoint dstPoint, const FPoint originPoint, const FLOAT fScale, const FPoint lastPoint);
    virtual BOOL Intersects(const FRect& rect);

    virtual CPoint GetPhysicalHandle(UINT nHandle);

    virtual void SetLineColor(COLORREF color);
    virtual void SetFillColor(COLORREF color);
    virtual void SetLineWeight(int nWidth);
    virtual void EnableFill(BOOL bEnable);
    virtual void EnableLine(BOOL bEnable);

};
