#pragma once
#include "afx.h"

class ScheduleView;


typedef struct OperationLayerApperance
{
    CRect	        m_Rect;
    BOOL            m_Visible;
}OperationLayerApperance;


class OperationLayer :
    public CObject
{

public:
    OperationLayer(void);
    virtual ~OperationLayer(void);

    enum OPLAYER_HITTEST
    {
        OPLAYER_HITTEST_FIRST   = 0,
        OPLAYER_HITTEST_NOWHERE = OPLAYER_HITTEST_FIRST,
        OPLAYER_HITTEST_INSIDE  = 1, // OPLAYER area
        OPLAYER_HITTEST_MOVE    = 2, // OPLAYER move area (IN side)
        OPLAYER_HITTEST_TOP     = 3, // OPLAYER top area (top side)
        OPLAYER_HITTEST_BOTTOM  = 4, // OPLAYER bottom area (bottom side)
        OPLAYER_HITTEST_LEFT    = 5, // OPLAYER left area (left side)
        OPLAYER_HITTEST_RIGHT   = 6, // OPLAYER right area (right side)
        OPLAYER_HITTEST_LAST    = OPLAYER_HITTEST_RIGHT
    };

    CRect GetRect(){return m_rectDraw;}
    void SetRect(CRect rect);

    virtual void OnDraw (CDC* pDC, ScheduleView* pView);

    OperationLayer::OPLAYER_HITTEST HitTest (const CPoint& point) const;

private:
    CRect			m_rectDraw;
};

