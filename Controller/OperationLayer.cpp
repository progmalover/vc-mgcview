#include "StdAfx.h"
#include "OperationLayer.h"
#include "ScheduleView.h"

OperationLayer::OperationLayer(void)
{
}


OperationLayer::~OperationLayer(void)
{
}
void OperationLayer::SetRect(CRect rect)
{
    m_rectDraw = rect;
    //m_rectDraw.InflateRect(c_HT_PREC, c_HT_PREC);
}
void OperationLayer::OnDraw (CDC* pDC, ScheduleView* pView)
{
    ASSERT_VALID (pDC);
    //ASSERT_VALID (pView);

    if (pDC == NULL || pDC->GetSafeHdc () == NULL || pView == NULL)
    {
        return;
    }
    OperationLayerApperance AA;

    AA.m_Rect = m_rectDraw;

    pView->DrawOperationLayer (pDC, this, &AA);
}


OperationLayer::OPLAYER_HITTEST OperationLayer::HitTest (const CPoint& point) const
{
    OPLAYER_HITTEST hit = OPLAYER_HITTEST_NOWHERE;

    CRect TestRect(m_rectDraw);

    //TestRect.InflateRect(c_HT_PREC, c_HT_PREC);

    if (TestRect.PtInRect (point))
    {
        CRect rt (m_rectDraw);

        hit = OPLAYER_HITTEST_INSIDE;

        if ((rt.top - c_HT_PREC) <= point.y && point.y <= (rt.top + c_HT_PREC)
            && (rt.left + rt.Width()/2 - c_HT_PREC) <= point.x
            && (rt.right - rt.Width()/2 + c_HT_PREC) >= point.x)
        {
            hit = OPLAYER_HITTEST_TOP;
        }
        else if (point.y <= (rt.bottom + c_HT_PREC) && (rt.bottom - c_HT_PREC) <= point.y
            && (rt.left + rt.Width()/2 - c_HT_PREC) <= point.x
            && (rt.right - rt.Width()/2 + c_HT_PREC) >= point.x)
        {
            hit = OPLAYER_HITTEST_BOTTOM;
        }
        else if (point.x <= (rt.left + c_HT_PREC) && (rt.left - c_HT_PREC) <= point.x
            && (rt.top + rt.Height()/2 - c_HT_PREC) <= point.y
            && (rt.bottom - rt.Height()/2 + c_HT_PREC) >= point.y)
        {
            hit = OPLAYER_HITTEST_LEFT;
        }
        else if (point.x <= (rt.right + c_HT_PREC) && (rt.right - c_HT_PREC) <= point.x
            && (rt.top + rt.Height()/2 - c_HT_PREC) <= point.y
            && (rt.bottom - rt.Height()/2 + c_HT_PREC) >= point.y)
        {
            hit = OPLAYER_HITTEST_RIGHT;
        }

        if (hit == OPLAYER_HITTEST_INSIDE)
        {
            hit = OPLAYER_HITTEST_MOVE;
        }
    }

    return hit;
}
