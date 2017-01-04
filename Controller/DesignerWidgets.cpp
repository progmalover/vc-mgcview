#include "stdafx.h"
#include "LayoutDesignerCtrl.h"
#include "DesignerPage.h"
#include "LayoutDesignerCmds.h"
extern UINT WM_ON_LAYOUT_SIZE_CHANGE;
UINT WM_ON_LAYOUT_SIZE_CHANGE_BEGIN = ::RegisterWindowMessage(_T("WM_ON_LAYOUT_SIZE_CHANGE_BEGIN"));
#define WM_ON_LAYOUT_SIZE_CHANGE_END WM_ON_LAYOUT_SIZE_CHANGE 
//
//global function
//

CLayoutDesignerCtrl *GetCurrentDesignerCtrl()
{
    CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
   // ASSERT(pPage);
    if(!pPage)
        return NULL;

    CLayoutDesignerCtrl *pCtrl = (CLayoutDesignerCtrl *)SearchWndObj( pPage,RUNTIME_CLASS(CLayoutDesignerCtrl));
   // ASSERT(pCtrl);
    return pCtrl;
}
//line test
 bool XLineCanGravityXLine(LayoutWidget::LINE *line1,LayoutWidget::LINE *line2)
{
    int tstVal1 = line1->ptStart.y - line2->ptStart.y;
    int tstVal2 = line1->ptStart.y - line2->ptEnd.y;
    int tstVal3 = line1->ptEnd.y - line2->ptStart.y;
    int tstVal4 = line1->ptEnd.y - line2->ptEnd.y;
                                                                                                                    
    if(tstVal1 * tstVal2 < 0 || tstVal3 * tstVal4 < 0)
        return true;
    if(tstVal1 * tstVal4 < 0 || tstVal3 *tstVal2 < 0)
        return true;
    
    return false;
}

 //
 //
bool YLineCanGravityYLine(LayoutWidget::LINE *line1,LayoutWidget::LINE *line2)
{
    int tstVal1 = line1->ptStart.x - line2->ptStart.x;
    int tstVal2 = line1->ptStart.x - line2->ptEnd.x;
    int tstVal3 = line1->ptEnd.x - line2->ptStart.x;
    int tstVal4 = line1->ptEnd.x - line2->ptEnd.x;

    if(tstVal1 * tstVal2 < 0 || tstVal3 * tstVal4 < 0)
        return true;
    if(tstVal1 * tstVal4 < 0 || tstVal3 *tstVal2 < 0)
        return true;
    return false;
}

bool IntersectLines(LayoutWidget::LINE &lineDesk,LayoutWidget::LINE &lineSrc)
{
    if(lineDesk.ptEnd.x == lineSrc.ptEnd.x && 
        lineDesk.ptEnd.x == lineDesk.ptEnd.x) // x line ,and can intersect.
    {
        if(XLineCanGravityXLine(&lineDesk,&lineSrc))
        {
            if(lineDesk.ptStart.y > lineSrc.ptStart.y)
                lineDesk.ptStart.y = lineSrc.ptStart.y;
            if(lineDesk.ptEnd.y < lineSrc.ptEnd.y)
                lineDesk.ptEnd.y = lineSrc.ptEnd.y;
            return true;
        }
    }

    if(lineDesk.ptEnd.y == lineSrc.ptEnd.y && 
        lineDesk.ptEnd.y == lineDesk.ptEnd.y) // y line ,and can intersect. 
    {
        if(YLineCanGravityYLine(&lineDesk,&lineSrc))
        {
            if(lineDesk.ptStart.x > lineSrc.ptStart.x)
                lineDesk.ptStart.x = lineSrc.ptStart.x;
            if(lineDesk.ptEnd.x < lineSrc.ptEnd.x)
                lineDesk.ptEnd.x = lineSrc.ptEnd.x;
            return true;
        }
    }
    return false;
}

void  AddSingleLine(LayoutWidget::LINES &lines,LayoutWidget::LINE &line)
{
    for(int i = 0;i < lines.size();i++)
    {
        LayoutWidget::LINE &itLine = lines[i];
        if(itLine == line)
          return ;
        //Merge lines.
        if(memcmp( &itLine.ptEnd ,&line.ptStart,sizeof(POINT)) == 0)
        {
            itLine.ptEnd = line.ptEnd;
            return;
        }

        if(memcmp( &itLine.ptStart ,&line.ptEnd,sizeof(POINT)) == 0)
        {
            itLine.ptStart = line.ptStart;
            return;
        }

        if(IntersectLines(itLine,line))
            return;
    }

    lines.push_back(line);
}


void  AddLine(LayoutWidget::LINES &lines,LayoutWidget::LINE &line)
{
    for(int i = 0;i < lines.size();i++)
    {
        LayoutWidget::LINE &itLine = lines[i];
        if(itLine == line)
            return ;
    }

    lines.push_back(line);
}
//
//widgets manager
//
void WidgetsManager::OnDraw(CDC *pDC)
{
    ASSERT(m_pDesigner);
    if(size() <= 0) return;
    for( int i = 0 ;i < size();i++)
    {
        DrawWidget *p = this->operator[](i);
        if(p)
        p->OnDraw(pDC);
    }

     m_pDesigner->DrawLayers(pDC);

    /// paint the select lines
     m_pDesigner->DrawSelectLine(pDC);

     //support floating widgets.
     m_pDesigner->DrawFloatingWidgets(pDC);
};

DrawWidget * WidgetsManager::AddWidget(int zOrder,DrawWidget *pWidget)
{
    if(zOrder == LAYOUT_Z_FLOATING)
        zOrder = LAYOUT_Z_MAX;

    ASSERT(zOrder >= LAYOUT_Z_0 && zOrder <= LAYOUT_Z_MAX );
    if(zOrder < LAYOUT_Z_0 || zOrder > LAYOUT_Z_MAX)return NULL;
    pWidget->m_pCtrl = m_pDesigner;
    DrawWidget *pOld = this->operator[](zOrder);
    this->operator[](zOrder) = pWidget;
    return pOld;
}

void WidgetsManager::Init(CLayoutDesignerCtrl *pCtrl)
{
    m_pDesigner = pCtrl;
    if(NULL == m_pDesigner)
        m_pDesigner = GetCurrentDesignerCtrl();
    resize(LAYOUT_Z_MAX + 1);//max 10 lays.
    AddWidget(LAYOUT_Z_0, new LayoutWidget()); //default layout widget.
}
    
void WidgetsManager::clear()
{
    for(int i = 0;i <size();i++)
    {
        switch(i)
        {
        case LAYOUT_Z_0:
            delete  (LayoutWidget *)this->operator[](i) ;
        default:
            break;
        }
    }
}

void WidgetsManager::RelayEvent(MSG *pMsg)
{
    if(size() <= 0) return;
    for( int i = 0 ;i < size();i++)
    {
        DrawWidget *p = this->operator[](i);
        if(p)
        p->OnEvent(pMsg);
    }
}
//
//CLayoutDesignerCtrl::DrawWidget *GetWidget(int i)
//

DrawWidget * CLayoutDesignerCtrl::GetWidget(int i)
{
    if( i < LAYOUT_Z_0 || i > LAYOUT_Z_FLOATING)
        return NULL;
    return m_Widgets[i];
}
//
//line operator
//
bool LayoutWidget::line::operator==(LayoutWidget::line &line)
{
    if(memcmp(this,&line,sizeof(LINE)) == 0)
        return true;
    return false;
}

void  LayoutWidget:: operator=(LayoutWidget &obj)
{
    this->m_MonitorRects.clear();
    this->m_Edge.clear();     
    this->m_VHBravityEdge.clear();
    
    this->m_MonitorRects.assign(obj.m_MonitorRects.begin(),obj.m_MonitorRects.end());
    this->m_Edge.xLines.assign(obj.m_Edge.xLines.begin(),obj.m_Edge.xLines.end());
    this->m_Edge.yLines.assign(obj.m_Edge.yLines.begin(),obj.m_Edge.yLines.end());        
    this->m_Edge.freeLines.assign(obj.m_Edge.freeLines.begin(),obj.m_Edge.freeLines.end());

    this->m_VHBravityEdge.xLines.assign(obj.m_VHBravityEdge.xLines.begin(),obj.m_VHBravityEdge.xLines.end());  
    this->m_VHBravityEdge.yLines.assign(obj.m_VHBravityEdge.yLines.begin(),obj.m_VHBravityEdge.yLines.end());   
    this->m_VHBravityEdge.freeLines.assign(obj.m_VHBravityEdge.freeLines.begin(),obj.m_VHBravityEdge.freeLines.end()); 
}

//
//edge
//
void LayoutWidget::edge::clear()
{
    xLines.clear();
    yLines.clear();
    freeLines.clear();
}
//
//Layoug Widget
//
void LayoutWidget::OnDraw(CDC *pDC)
{
    ASSERT(m_pCtrl);   
    m_pCtrl->DrawLayout(pDC);
    if(m_MonitorRects.size())
    {
        DrawMonitorRegions(pDC);
        OnEdgeBravity();
    }
}


//
//these values is same as "RecalcRulerLinesAttachment"
//
const int nAttachSize =30;
const int nLeftAttached = 0x1;
const int nRightAtteched = 0x2;
const int nTopAttached = 0x4;
const int nBottomAttached = 0x8;
void LayoutWidget::OnEdgeBravity()
{
    ASSERT(m_pCtrl);
    if(m_VHBravityEdge.xLines.size() <= 0 || m_VHBravityEdge.yLines.size() <= 0)
        return;
    CS3DrawObjList  &objList = m_pCtrl->m_drawObjList;

    POSITION pos = objList.GetHeadPosition();
    while (pos != NULL)
    {
        CS3DrawObj *pObj = objList.GetNext(pos);
        PROPERTY_TYPE proType;
        pObj->GetPropertyType(proType);
        if (proType != BACKGROUND)
        {
            FRect rectObj;
            FRect rect;
            pObj->GetLogicPos(rectObj);
            m_pCtrl->LayoutToClient(rectObj);
            rect = rectObj;

            DWORD dwAttchStatus = 0;
            int storeX1,storeX2;
            int storeY1,storeY2;

            
            int nLeftCacherLine = -1;
            for( int i = 0 ;i < m_VHBravityEdge.xLines.size();i++)
            {
                //pt.x = m_VHBravityEdge.xLines[i];
                LINE xLine = m_VHBravityEdge.xLines[i];
                m_pCtrl->LayoutToClient(*(CRect *)&xLine);
                LINE tstLine ;
                tstLine.ptStart = CPoint(rect.left,rect.top);
                tstLine.ptEnd = CPoint(rect.left,rect.bottom);
                if(XLineCanGravityXLine(&xLine,&tstLine))
                if(abs(xLine.ptStart.x - rect.left)<= nAttachSize)
                {
                    storeX1 = xLine.ptStart.x;
                    nLeftCacherLine = i;
                    dwAttchStatus |= nLeftAttached;
                    break;
                }
            }


            for( int i = 0 ;i <  m_VHBravityEdge.xLines.size();i++)
            {
                LINE xLine = m_VHBravityEdge.xLines[i];
                m_pCtrl->LayoutToClient(*(CRect *)&xLine);
                LINE tstLine ;
                tstLine.ptStart = CPoint(rect.right,rect.top);
                tstLine.ptEnd = CPoint(rect.right,rect.bottom);
                if(XLineCanGravityXLine(&xLine,&tstLine))
                if(abs(xLine.ptStart.x - rect.right)<= nAttachSize)
                {
                    if(nLeftCacherLine == i)
                        continue;

                    storeX2 = xLine.ptStart.x;
                    dwAttchStatus |= nRightAtteched;
                    break;
                }
            }

            int nTopCacherLine = -1;
            for( int i = 0 ;i < m_VHBravityEdge.yLines.size();i++)
            {
                LINE yLine = m_VHBravityEdge.yLines[i];
                m_pCtrl->LayoutToClient(*(CRect *)&yLine);
                LINE tstLine ;
                tstLine.ptStart = CPoint(rect.left,rect.top);
                tstLine.ptEnd = CPoint(rect.right,rect.top);
                if(YLineCanGravityYLine(&yLine,&tstLine))
                if(abs(yLine.ptStart.y - rect.top)<= nAttachSize)
                {
                    storeY1 = yLine.ptStart.y;
                    nTopCacherLine = i;
                    dwAttchStatus |= nTopAttached;
                    break;
                }
            }


            for( int i = 0 ;i <m_VHBravityEdge.yLines.size();i++)
            {
                LINE yLine = m_VHBravityEdge.yLines[i];
                m_pCtrl->LayoutToClient(*(CRect *)&yLine);
                LINE tstLine ;
                tstLine.ptStart = CPoint(rect.left,rect.bottom);
                tstLine.ptEnd = CPoint(rect.right,rect.bottom);
                if(YLineCanGravityYLine(&yLine,&tstLine))
                if(abs(yLine.ptStart.y  - rect.bottom)<= nAttachSize)
                {

                    if(nTopCacherLine == i)
                        continue;

                    storeY2 = yLine.ptStart.y;
                    dwAttchStatus |= nBottomAttached;
                    break;
                }
            }

            FRect tpRect = rect;
            if(((dwAttchStatus & nLeftAttached) == nLeftAttached) &&  
                ((dwAttchStatus & nRightAtteched) == nRightAtteched))
            {
                tpRect.left  = storeX1;
                tpRect.right = storeX2;
            }else
            if(dwAttchStatus & nLeftAttached)
            {
                tpRect.OffsetRect(storeX1 - tpRect.left,0);
            }else
            if(dwAttchStatus & nRightAtteched)
            {
                tpRect.OffsetRect(storeX2 - tpRect.right,0);
            }

            if( ((dwAttchStatus & nTopAttached) == nTopAttached)
                &&(( dwAttchStatus & nBottomAttached ) == nBottomAttached))
            {
                tpRect.top  = storeY1;
                tpRect.bottom = storeY2;
            }else
            if(dwAttchStatus & nTopAttached)
            {
                tpRect.OffsetRect(0,storeY1 - tpRect.top);
            }else
            if(dwAttchStatus & nBottomAttached)
            {
                tpRect.OffsetRect(0,storeY2 - tpRect.bottom);
            }

            if(tpRect != rect && dwAttchStatus)
            {
                m_pCtrl->ClientToLayout(tpRect);
                pObj->SetLogicPos( tpRect);
              //  m_pCtrl->Invalidate();

            }
        }

    }
}

void  LayoutWidget::DrawEdges(CDC *pDC)
{
    ASSERT(pDC);
    if(m_Edge.freeLines.size() <= 0)
        return;
    if(!pDC->GetSafeHdc())return;
    pDC->SaveDC();

    CPen ep;
    ep.CreatePen(PS_DASH,1,RGB(0,0,255));
    pDC->SelectObject(ep);

    for(int j = 0;j < m_Edge.freeLines.size();j++)
    {
        LINE &line = m_Edge.freeLines[j];
        CPoint pt = line.ptStart;
        m_pCtrl->LayoutToClient(pt);
        pDC->MoveTo(pt);
        pt = line.ptEnd;
        m_pCtrl->LayoutToClient(pt);
        pDC->LineTo(pt);
    }


    CPen ep2;
    ep2.CreatePen(PS_DOT,1,RGB(53,180,234));
    pDC->SelectObject(ep2);

    for(int i = 0;i < m_VHBravityEdge.xLines.size();i++)
    {
        LINE &line = m_VHBravityEdge.xLines[i];
        CPoint pt = line.ptStart;
        m_pCtrl->LayoutToClient(pt);
        pDC->MoveTo(pt);
        pt = line.ptEnd;
        m_pCtrl->LayoutToClient(pt);
        pDC->LineTo(pt);
    }

    for(int j = 0;j < m_VHBravityEdge.yLines.size();j++)
    {
        LINE &line = m_VHBravityEdge.yLines[j];
        CPoint pt = line.ptStart;
        m_pCtrl->LayoutToClient(pt);
        pDC->MoveTo(pt);
        pt = line.ptEnd;
        m_pCtrl->LayoutToClient(pt);
        pDC->LineTo(pt);
    }

    pDC->RestoreDC(-1);
}

void LayoutWidget::DrawMonitorText(CDC *pDC)
{
    if(this->m_MonitorRects.size() <= 0)
        return;
    pDC->SaveDC();
    pDC->SetTextColor(RGB(0,0,255));
    pDC->SetBkMode(TRANSPARENT);

    CRect Canvas;
    for(int i = 0;i < m_MonitorRects.size();i++)
    {
        Canvas.UnionRect(&Canvas,&m_MonitorRects[i]);
    }

    for( int i = 0;i  < m_MonitorRects.size();i++)
    {
        POINTF ptCenter;
        ptCenter.x = Canvas.CenterPoint().x;   
        ptCenter.y = Canvas.CenterPoint().y;

        // lb, lt,  rt, rb
        POINTF pt[4];
        pt[0].x = m_MonitorRects[i].left; pt[0].y = m_MonitorRects[i].bottom;
        pt[1].x = m_MonitorRects[i].left; pt[1].y = m_MonitorRects[i].top;
        pt[2].x = m_MonitorRects[i].right; pt[2].y = m_MonitorRects[i].top;   
        pt[3].x = m_MonitorRects[i].right; pt[3].y = m_MonitorRects[i].bottom;

        RotatePoint(ptCenter, pt[0], m_fRotateDegree);
        RotatePoint(ptCenter, pt[1], m_fRotateDegree);
        RotatePoint(ptCenter, pt[2], m_fRotateDegree);
        RotatePoint(ptCenter, pt[3], m_fRotateDegree);

        CRect rc;

        rc.top = m_yTrans + min(min(min(pt[0].y, pt[1].y), pt[2].y), pt[3].y);
        rc.bottom = m_yTrans + max(max(max(pt[0].y, pt[1].y), pt[2].y), pt[3].y);
        rc.left = m_xTrans + min(min(min(pt[0].x, pt[1].x), pt[2].x), pt[3].x);
        rc.right = m_xTrans + max(max(max(pt[0].x, pt[1].x), pt[2].x), pt[3].x);

        m_pCtrl->LayoutToClient(rc);
        pDC->DrawText(Translate(_T("Monitor Slot")),rc,DT_CENTER | DT_VCENTER|DT_SINGLELINE);
    }
    pDC->RestoreDC(-1);
}

HRGN LayoutWidget::CreateMaskRgn(CRect layoutRc)
{
    CRgn boundRgn;
    boundRgn.CreateRectRgn(layoutRc.left,layoutRc.top,layoutRc.right,layoutRc.bottom);

    CRgn maskRgn;
    CRgn tmpRgn;
    CRgn *pSrcRgn = &boundRgn;

    CRect Canvas;
    for(int i = 0;i < m_MonitorRects.size();i++)
    {
        Canvas.UnionRect(&Canvas,&m_MonitorRects[i]);
    }

    tmpRgn.CreateRectRgn(0,0,0,0);
    for( int i = 0;i  < m_MonitorRects.size();i++)
    {
        CRect rc = m_MonitorRects[i];

        POINTF ptCenter;
        ptCenter.x = Canvas.CenterPoint().x;   
        ptCenter.y = Canvas.CenterPoint().y;

        // lb, lt,  rt, rb
        POINTF pt[4];
        pt[0].x = m_MonitorRects[i].left; pt[0].y = m_MonitorRects[i].bottom;
        pt[1].x = m_MonitorRects[i].left; pt[1].y = m_MonitorRects[i].top;
        pt[2].x = m_MonitorRects[i].right; pt[2].y = m_MonitorRects[i].top;   
        pt[3].x = m_MonitorRects[i].right; pt[3].y = m_MonitorRects[i].bottom;

        RotatePoint(ptCenter, pt[0], m_fRotateDegree);
        RotatePoint(ptCenter, pt[1], m_fRotateDegree);
        RotatePoint(ptCenter, pt[2], m_fRotateDegree);
        RotatePoint(ptCenter, pt[3], m_fRotateDegree);

        CPoint cpt[4];
        cpt[0].x = pt[0].x;  cpt[0].y = pt[0].y;  
        cpt[1].x = pt[1].x;  cpt[1].y = pt[1].y;
        cpt[2].x = pt[2].x;  cpt[2].y = pt[2].y;
        cpt[3].x = pt[3].x;  cpt[3].y = pt[3].y;

        cpt[0].Offset(m_xTrans, m_yTrans);   
        cpt[1].Offset(m_xTrans, m_yTrans);
        cpt[2].Offset(m_xTrans, m_yTrans);
        cpt[3].Offset(m_xTrans, m_yTrans);

        CRgn uniRgn;

        m_pCtrl->LayoutToClient(cpt[0]);         
        m_pCtrl->LayoutToClient(cpt[1]);
        m_pCtrl->LayoutToClient(cpt[2]);
        m_pCtrl->LayoutToClient(cpt[3]);

        if(uniRgn.CreatePolygonRgn(cpt, 4, ALTERNATE))
        {
            tmpRgn.CombineRgn(pSrcRgn,&uniRgn,RGN_XOR);
            if(maskRgn.GetSafeHandle() != NULL)
                maskRgn.DeleteObject();

            maskRgn.Attach(tmpRgn.Detach());
            tmpRgn.CreateRectRgn(0,0,0,0);
            pSrcRgn = &maskRgn;
        }
    }
    return (HRGN)maskRgn.Detach();
}

void LayoutWidget::DrawMonitorRegions(CDC *pDC)
{
    ASSERT(pDC);
    ASSERT(m_pCtrl);

    CRect layoutRc = m_pCtrl->GetLayoutClientRect();
    CDC memDC;
    memDC.CreateCompatibleDC(pDC);
    if(memDC.GetSafeHdc() != NULL)
    {
        //
        //create / draw layout mask.
        //
        memDC.SaveDC();
        pDC->SaveDC(); 

        CRect cliRect;
        m_pCtrl->GetClientRect(&cliRect);
        layoutRc.IntersectRect(layoutRc,cliRect);  

        LAYOUTMASK mask;
     
        mask.maskBmp.CreateCompatibleBitmap(pDC,layoutRc.Width(),layoutRc.Height());
        memDC.SelectObject(&mask.maskBmp);
        memDC.FillSolidRect(0,0,layoutRc.Width(),layoutRc.Height(),RGB(40,100,40));

        mask.maskRgn.Attach(CreateMaskRgn(layoutRc));
       
        pDC->SelectClipRgn(&mask.maskRgn);
        //memDC.SelectClipRgn(&maskRgn);
        BLENDFUNCTION blend = {0,0,100,0};
        pDC->AlphaBlend(layoutRc.left,layoutRc.top,layoutRc.Width(),
            layoutRc.Height(),&memDC,0,0,layoutRc.Width(),layoutRc.Height(),blend);

        memDC.RestoreDC(-1);
        pDC->RestoreDC(-1);

    }
    
    //Draw Edge
    DrawEdges(pDC);
    
    //DrawText
    DrawMonitorText(pDC);
}

void LayoutWidget::CauclateMonitorRegionEdiges(FLOAT xTrans, FLOAT yTrans)
{
    if(m_MonitorRects.size() <= 0)
        return;

    CRect Canvas;
    for(int i = 0;i < m_MonitorRects.size();i++)
    {
        Canvas.UnionRect(&Canvas,&m_MonitorRects[i]);
    }

    POINTF ptCenter;
    ptCenter.x = Canvas.CenterPoint().x;   
    ptCenter.y = Canvas.CenterPoint().y;

    m_xTrans = xTrans;
    m_yTrans = yTrans;
    FLOAT fRotateDegree = m_fRotateDegree;

    m_VHBravityEdge.clear();

    for(int i = 0;i < m_MonitorRects.size();i++)
    {            
        CRect &rcMonitor = m_MonitorRects[i];
        LINE line;
        {   
            POINTF pt[4];
            pt[0].x = rcMonitor.left; pt[0].y = rcMonitor.bottom;
            pt[1].x = rcMonitor.left; pt[1].y = rcMonitor.top;
            pt[2].x = rcMonitor.right; pt[2].y = rcMonitor.top;   
            pt[3].x = rcMonitor.right; pt[3].y = rcMonitor.bottom;

            RotatePoint(ptCenter, pt[0], m_fRotateDegree);
            RotatePoint(ptCenter, pt[1], m_fRotateDegree);
            RotatePoint(ptCenter, pt[2], m_fRotateDegree);
            RotatePoint(ptCenter, pt[3], m_fRotateDegree);

            line.ptStart.x = pt[0].x + xTrans;             
            line.ptStart.y = pt[0].y + yTrans;
            line.ptEnd.x = pt[1].x + xTrans;             
            line.ptEnd.y = pt[1].y + yTrans;
            AddLine(m_Edge.freeLines,line);   
            line.ptStart.x = pt[1].x + xTrans;             
            line.ptStart.y = pt[1].y + yTrans;
            line.ptEnd.x = pt[2].x + xTrans;             
            line.ptEnd.y = pt[2].y + yTrans;
            AddLine(m_Edge.freeLines,line);
            line.ptStart.x = pt[2].x + xTrans;             
            line.ptStart.y = pt[2].y + yTrans;
            line.ptEnd.x = pt[3].x + xTrans;             
            line.ptEnd.y = pt[3].y + yTrans;
            AddLine(m_Edge.freeLines,line);
            line.ptStart.x = pt[3].x + xTrans;             
            line.ptStart.y = pt[3].y + yTrans;
            line.ptEnd.x = pt[0].x + xTrans;             
            line.ptEnd.y = pt[0].y + yTrans;
            AddLine(m_Edge.freeLines,line);

            CRect rc;
            rc.top = m_yTrans + min(min(min(pt[0].y, pt[1].y), pt[2].y), pt[3].y);
            rc.bottom = m_yTrans + max(max(max(pt[0].y, pt[1].y), pt[2].y), pt[3].y);
            rc.left = m_xTrans + min(min(min(pt[0].x, pt[1].x), pt[2].x), pt[3].x);
            rc.right = m_xTrans + max(max(max(pt[0].x, pt[1].x), pt[2].x), pt[3].x);

            line.ptStart.x = rc.left;          
            line.ptStart.y = rc.top;
            line.ptEnd.x = rc.right;         
            line.ptEnd.y = rc.top;
            AddSingleLine(m_VHBravityEdge.yLines,line);
            line.ptStart.x = rc.left;          
            line.ptStart.y = rc.bottom;
            line.ptEnd.x = rc.right;         
            line.ptEnd.y = rc.bottom;
            AddSingleLine(m_VHBravityEdge.yLines,line);
            line.ptStart.x = rc.left;          
            line.ptStart.y = rc.top;
            line.ptEnd.x = rc.left;         
            line.ptEnd.y = rc.bottom;
            AddSingleLine(m_VHBravityEdge.xLines,line);
            line.ptStart.x = rc.right;          
            line.ptStart.y = rc.top;
            line.ptEnd.x = rc.right;         
            line.ptEnd.y = rc.bottom;
            AddSingleLine(m_VHBravityEdge.xLines,line);
        }

    }

}

void  LayoutWidget::OnEvent(MSG *msg)
{
    switch(msg->message)
    {
    case WM_MOUSEMOVE:
        this->OnEdgeBravity();
        break;
    default:
        break;
    }
}

void LayoutWidget::SetClientLayout(CSize &szClient ,std::vector<CRect> *rcMonitors, BOOL bNotify,FLOAT fRotateDegree)
{
    ASSERT(m_pCtrl != NULL);
   /* if(szClient.cx == m_pCtrl->m_LayoutSize.cx  &&
        szClient.cy == m_pCtrl->m_LayoutSize.cy)
    {
       return;
    }*/

    CDesignerPage *pPage = (CDesignerPage *)GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));

    m_LayoutSize = szClient;
    m_Edge.clear();  
    
    if(bNotify)
        pPage->SendMessage(WM_ON_LAYOUT_SIZE_CHANGE_BEGIN,0,0); // 1: start size change....

    m_fRotateDegree = fRotateDegree;

    m_MonitorRects.clear();
    if(rcMonitors && rcMonitors->size())
    {         
        m_MonitorRects.assign(rcMonitors->begin(),rcMonitors->end());

        CRect Canvas;
        for(int i = 0;i < m_MonitorRects.size();i++)
        {
            Canvas.UnionRect(&Canvas,&m_MonitorRects[i]);
        }

        FLOAT xTrans = (szClient.cx - Canvas.Width()) / 2.0;
        FLOAT yTrans = (szClient.cy - Canvas.Height()) / 2.0;

        CauclateMonitorRegionEdiges(xTrans, yTrans);
    }

    if(bNotify)
    {    
        pPage->SendMessage(WM_ON_LAYOUT_SIZE_CHANGE_END,0,0);
    //m_pCtrl->Invalidate();
    }else
    {
        pPage->NewLayout(szClient.cx,szClient.cy, fRotateDegree);
    }
} 