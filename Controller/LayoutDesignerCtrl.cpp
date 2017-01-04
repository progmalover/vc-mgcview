// F:\p4\sw\Demo\ContentManagement\Controller\Designer\LayoutDesignerWnd.cpp : implementation file
//

#include "stdafx.h"
#include "LayoutDesignerCtrl.h"
#include "LayoutSettings.h"
#include "Controller.h"

#include "S3DrawRectObj.h"

#include "DesignerPage.h"
#include "rulerctrl.h"

// wParam - no used, lParam - float point to the zoom factor
UINT WM_ON_LAYOUT_ZOOM = ::RegisterWindowMessage(_T("LAYOUT_ZOOM"));

// wParam - TRUE - selected, otherwise - unselected, lParam - point to the CS3DrawObjList
UINT WM_ON_LAYTOUT_SELECT_OBJ = ::RegisterWindowMessage(_T("LAYOUT_SELECT_OBJ"));

// wParam - TRUE - removed, otherwise - not removed, lParam - point to the CS3DrawObjList
UINT WM_ON_LAYOUT_REMOVE_OBJ = ::RegisterWindowMessage(_T("LAYOUT_REMOVE_OBJ"));

// wParam - point of OBJINFO, include the default position and property type, lParam - Array of added media files' name, if the array count is 0, mean insert a component
UINT WM_ON_LAYOUT_INSERT_OBJ = ::RegisterWindowMessage(_T("LAYOUT_INSERT_OBJ"));

// wParam - TRUE - action on the layout design, otherwise - no action, lParam - point to the action structure
UINT WM_ON_LAYOUT_ACTION = ::RegisterWindowMessage(_T("LAYOUT_ACTION"));

// wParam - point to selected CS3DrawObj, lParam - Array of added media files' name, if the array count is 0, mean insert a component
UINT WM_ON_OBJ_ADDMEDIA = ::RegisterWindowMessage(_T("OBJ_ADDMEDIA"));

// wParam - client ID
UINT WM_ON_LAYTOUT_SELECT_CLIENT = ::RegisterWindowMessage(_T("LAYOUT_SELECT_CLIENT"));

// wParam, lParam - see WM_KEYDOWN
UINT DESIGNER_KEYDOWN		= ::RegisterWindowMessage (_T("DESIGNER_KEYDOWN"));
// CLayoutDesignerCtrl

IMPLEMENT_DYNAMIC(CLayoutDesignerCtrl, CWnd)

CLayoutDesignerCtrl::CLayoutDesignerCtrl():m_bNotifyParent	  (TRUE)
{
    m_BKColor           = RGB(192, 192, 192);
    m_LayoutColor       = RGB(255, 255, 255);
    m_LayoutShadowColor = RGB(128, 128, 128);
    m_BorderColor       = RGB(0, 0, 0);
    m_GridColor         = RGB(192, 192, 192);

    m_nRatioX = 100;
    m_nRatioY = 100;

    m_LayoutSize.SetSize(0, 0);
    m_LayoutRect.SetRectEmpty();
   // m_VirtualLayoutRect.SetRectEmpty();
    m_ViewportRect.SetRectEmpty();
    m_bVirtualRect = FALSE;

    m_Scale     = 1.0f;
    m_XTrans    = 0.0f;
    m_XTrans    = 0.0f;

    m_nSelectMode   = SEL_MODE_NONE;
    m_nDownFlags    = 0;
    m_downPoint.SetPoint(-1, -1);
    m_lastPoint.SetPoint(-1, -1);
    m_lastDown.SetPoint(-1, -1);
    m_lastScalPoint.SetPoint(0, 0);
    m_bSelection    = FALSE;
    m_lastAlignPoint.SetPoint(0, 0);
    m_lastClonePoint.SetPoint(0, 0);

    m_selectRect.SetRectEmpty();
    m_pSelectObj = NULL;

    m_pAudioObj = NULL;

    m_HscrollPos        = 0;
    m_VscrollPos        = 0;

    m_nSelectRectLength = 7;

    m_fAlignFactor      = 0.4f;
    m_bAlignGrid        = FALSE;
    m_bShowGrid         = TRUE;
	m_bEnableBindLay	= FALSE;
    m_bLBtnDblClk       = FALSE;

    m_prevDropEffect    = DROPEFFECT_NONE;

	m_bSetupingScrollBars = FALSE;

	m_nXCatchedLine = -1;
	m_nYCatchedLine = -1;
	m_bYLineCatched = FALSE;
	m_bXLineCatched = FALSE;
	
    m_Widgets.Init(this);
    UpdateOptions();

}

CLayoutDesignerCtrl::~CLayoutDesignerCtrl()
{
    m_drawObjList.RemoveAll();
    m_selectObjList.RemoveAll();
    RemoveGridLine();

    if (m_pSelectObj != NULL)
    {
        //delete m_pSelectObj;
        m_pSelectObj = NULL;
    }

    m_pAudioObj = NULL;
	m_Widgets.clear();
    m_DragMediaFiles.clear();
}


BEGIN_MESSAGE_MAP(CLayoutDesignerCtrl, CWnd)
    ON_WM_CREATE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_GETDLGCODE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_MOUSEWHEEL()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_SETFOCUS()
    ON_WM_DESTROY()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
	ON_WM_KEYDOWN()
    ON_REGISTERED_MESSAGE(WM_DND_DRAGENTER, OnDragEnter)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGOVER, OnDragOver)
    ON_REGISTERED_MESSAGE(WM_DND_DRAGLEAVE, OnDragLeave)
    ON_REGISTERED_MESSAGE(WM_DND_DROP, OnDrop)
	ON_WM_NCHITTEST()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_EDIT_UNDO,OnUndo)
	ON_COMMAND(ID_EDIT_REDO,OnRedo)
END_MESSAGE_MAP()

 void CLayoutDesignerCtrl::OnUndo()
{
	m_CmdEngine.undo();
}

 void CLayoutDesignerCtrl::OnRedo()
{
	m_CmdEngine.redo();
}

BOOL CLayoutDesignerCtrl::PreTranslateMessage( MSG* pMsg)
{
	this->m_CmdEngine.TransMessage(pMsg);
    this->m_Widgets.RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CLayoutDesignerCtrl::LayoutToClient(FRect& rect)
{
    rect.left   = rect.left * m_Scale + m_XTrans;
    rect.top    = rect.top * m_Scale + m_YTrans;
    rect.right  = rect.right * m_Scale + m_XTrans;
    rect.bottom = rect.bottom * m_Scale + m_YTrans;
}

void CLayoutDesignerCtrl::LayoutToClient(CRect& rect)
{
    rect.left   = (int)(rect.left * m_Scale + m_XTrans + 0.5f);
    rect.top    = (int)(rect.top * m_Scale + m_YTrans + 0.5f);
    rect.right  = (int)(rect.right * m_Scale + m_XTrans + 0.5f);;
    rect.bottom = (int)(rect.bottom * m_Scale + m_YTrans + 0.5f);;
}

void CLayoutDesignerCtrl::LayoutToClient(CPoint& point)
{
    point.x = (int)(point.x * m_Scale + m_XTrans + 0.5f);
    point.y = (int)(point.y * m_Scale + m_YTrans + 0.5f);
}

void CLayoutDesignerCtrl::LayoutToClient(FPoint& point)
{
    point.x = point.x * m_Scale + m_XTrans;
    point.y = point.y * m_Scale + m_YTrans;
}

void CLayoutDesignerCtrl::LayoutToClient(int& nLength)
{
    nLength = (int)(nLength * m_Scale + 0.5f);
}

void CLayoutDesignerCtrl::LayoutToClient(UINT& nLength)
{
    nLength = (UINT)(nLength * m_Scale + 0.5f);
}

void CLayoutDesignerCtrl::ClientToLayout(UINT& nLength)
{
    nLength = (UINT)(nLength / m_Scale + 0.5f);
}

void CLayoutDesignerCtrl::ClientToLayout(int& nLength)
{
    nLength = (int)(nLength / m_Scale + 0.5f);
}

void CLayoutDesignerCtrl::ClientToLayout(CPoint& point)
{
    point.x = (int)((point.x - m_XTrans) / m_Scale + 0.5f);
    point.y = (int)((point.y - m_YTrans) / m_Scale + 0.5f);
}

void CLayoutDesignerCtrl::ClientToLayout(FPoint& point)
{
    point.x = (point.x - m_XTrans) / m_Scale;
    point.y = (point.y - m_YTrans) / m_Scale;
}

void CLayoutDesignerCtrl::ClientToLayout(CRect& rect)
{
	rect.left   = (int)((rect.left - m_XTrans) / m_Scale + 0.5f);
	rect.top    = (int)((rect.top - m_YTrans) / m_Scale + 0.5f);
	rect.right  = (int)((rect.right - m_XTrans) / m_Scale + 0.5f);
	rect.bottom = (int)((rect.bottom - m_YTrans) / m_Scale + 0.5f);
}

void CLayoutDesignerCtrl::ClientToLayout(FRect& rect)
{
    rect.left   = (rect.left - m_XTrans) / m_Scale;
    rect.top    = (rect.top - m_YTrans) / m_Scale;
    rect.right  = (rect.right - m_XTrans) / m_Scale;
    rect.bottom = (rect.bottom - m_YTrans) / m_Scale;
}

/// In logic coordinate
void CLayoutDesignerCtrl::SetLayoutSize(const CSize layoutSize)
{
    FRect rect(0.0f, 0.0f, 0.0f, 0.0f);
    CPoint point;
    point = rect.CenterPoint();

    m_LayoutSize.SetSize(layoutSize.cx, layoutSize.cy);

    m_MaxDesignerWidth     = layoutSize.cx * 2;
    m_MaxDesignerHeight    = layoutSize.cy * 2;

    m_LayoutRect.left      = 0.0f;
    m_LayoutRect.right     = (FLOAT)m_LayoutSize.cx;
    m_LayoutRect.top       = 0.0f;
    m_LayoutRect.bottom    = (FLOAT)m_LayoutSize.cy;

	m_ViewportRect.left    = 0.0f - m_LayoutSize.cx / 2;
    m_ViewportRect.top     = 0.0f - m_LayoutSize.cy / 2;
    m_ViewportRect.right   = (FLOAT)(m_LayoutSize.cx + m_LayoutSize.cx / 2);
    m_ViewportRect.bottom  = (FLOAT)(m_LayoutSize.cy + m_LayoutSize.cy / 2);

    RemoveGridLine();
    SetupVirtualGrid();

    CalcFitScale();
    CalcTransform();
	CalcBindLayLines(m_LayoutRect);
    m_bVirtualRect = TRUE;

}

void CLayoutDesignerCtrl::CalcFitScale()
{
	CRect rect;
	GetClientRect(rect);

    int nWidth = abs(rect.Width() - 4);
    int nHeight= abs(rect.Height() - 4);

    
	if ((FLOAT)m_LayoutSize.cx / (FLOAT)m_LayoutSize.cy > (FLOAT)nWidth / (FLOAT)nHeight)
    {
        m_Scale = (FLOAT)nWidth / (FLOAT)m_LayoutSize.cx;
    }
    else
    {
        m_Scale = (FLOAT)nHeight / (FLOAT)m_LayoutSize.cy;
    }

 /*   int nScal = 100 * m_Scale;
    if(nScal)
        m_Scale = (FLOAT)nScal / 100;*/

    if(m_Scale <= 0.00001)
        m_Scale = 0.01;
    GetParent()->SendMessage(WM_ON_LAYOUT_ZOOM, 0, (LPARAM)&m_Scale);
}

void CLayoutDesignerCtrl::OnFitScreen()
{
    CalcFitScale();
    CalcTransform();
}

/// In logic coordinate
BOOL CLayoutDesignerCtrl::AlignToGrid(FPoint& point, const FPoint clickPoint)
{
    CPoint position;
    position.SetPoint(static_cast<int>(point.x), static_cast<int>(point.y));

    return FALSE;
}

/// In logic coordinate
BOOL CLayoutDesignerCtrl::AlignToGridByPoint(FRect& rect, const FPoint posDelta, POINT_POS refPoint)
{
    FRect position = rect;
    // Calc the align distance, if min distance should be 1
    int nAlignGapX = (int)(m_nRatioX + 0.5f);
    nAlignGapX = (nAlignGapX - 1) < 0 ? 1 : nAlignGapX;

    int nAlignGapY = (int)(m_nRatioY + 0.5f);
    nAlignGapY = (nAlignGapY - 1) < 0 ? 1 : nAlignGapY;

    if (refPoint == TOP_LEFT)
    {
        position.OffsetRect(posDelta);

        if ((((static_cast<int>(position.top) % nAlignGapY) + (nAlignGapY>>2))%nAlignGapY) < (nAlignGapY>>1))
        {
            float OffsetV = static_cast<float>(position.top + nAlignGapY/2.0)
                - (static_cast<int>(position.top + nAlignGapY/2.0) % nAlignGapY);
            OffsetV -= position.top;

            position.top += OffsetV;
            position.bottom += OffsetV;
        }

        if ((((static_cast<int>(position.left) % nAlignGapX) + (nAlignGapX>>2))%nAlignGapX) < (nAlignGapX>>1))
        {
            float OffsetH = static_cast<float>(position.left + nAlignGapX/2.0)
                - (static_cast<int>(position.left + nAlignGapX/2.0) % nAlignGapX);
            OffsetH -= position.left;

            position.left += OffsetH;
            position.right += OffsetH;
        }
    }

    rect = position;
    return TRUE;
}

/// In logic coordinate
BOOL CLayoutDesignerCtrl::AlignToGridResizeByLine(FRect& rect, LINE_POS refLine, BOOL bKeepAspect)
{
    FRect position = rect;
    INT Radio = 0;

    float* pMyLine = NULL;
    float* pAspectLine = NULL;


    switch (refLine)
    {
    case TOP:
        {
            pMyLine = &position.top;
            Radio = m_nRatioY;

            if (bKeepAspect)
            {
                pAspectLine = &position.right;
            }
        }
    	break;
    case RIGHT:
        {
            pMyLine = &position.right;
            Radio = m_nRatioX;

            if (bKeepAspect)
            {
                pAspectLine = &position.bottom;
            }
        }
        break;
    case BOTTOM:
        {
            pMyLine = &position.bottom;
            Radio = m_nRatioY;

            if (bKeepAspect)
            {
                pAspectLine = &position.right;
            }
        }
        break;
    case LEFT:
        {
            pMyLine = &position.left;
            Radio = m_nRatioX;

            if (bKeepAspect)
            {
                pAspectLine = &position.bottom;
            }
        }
        break;
    default:
        ASSERT(0);
    }

    // Calc the align distance, if min distance should be 1
    int nAlignGap = (int)(Radio + 0.5f);
    nAlignGap = (nAlignGap - 1) < 0 ? 1 : nAlignGap;

    float Offset = 0;

    if ((((static_cast<int>(*pMyLine) % nAlignGap) + (nAlignGap>>2))%nAlignGap) < (nAlignGap>>1))
    {
        Offset = static_cast<float>(*pMyLine + nAlignGap/2.0)
            - (static_cast<int>(*pMyLine + nAlignGap/2.0) % nAlignGap);
        Offset -= *pMyLine;
    }

    float OffsetAspect = 0;
    if (bKeepAspect)
    {
        switch (refLine)
        {
        case TOP:
            {
                double radixY = Offset/position.Height();
                OffsetAspect = -(float)(position.Width()*radixY);
            }
            break;
        case RIGHT:
            {
                double radixX = Offset/position.Width();
                OffsetAspect = (float)(position.Width()*radixX);
            }
            break;
        case BOTTOM:
            {
                double radixY = Offset/position.Height();
                OffsetAspect = (float)(position.Width()*radixY);
            }
            break;
        case LEFT:
            {
                double radixX = Offset/position.Width();
                OffsetAspect = -(float)(position.Width()*radixX);
            }
            break;
        default:
            ASSERT(0);
        }

    }

    *pMyLine += Offset;
    if (pAspectLine && bKeepAspect)
    {
        *pAspectLine += OffsetAspect;
    }

    rect = position;
    return TRUE;
}

/// In logic coordinate
BOOL CLayoutDesignerCtrl::AlignToGridResizeByPoint(FRect& rect, POINT_POS refPoint, BOOL bKeepAspect)
{
    FRect position = rect;
    // Calc the align distance, if min distance should be 1
    int nAlignGapX = (int)(m_nRatioX + 0.5f);
    nAlignGapX = (nAlignGapX - 1) < 0 ? 1 : nAlignGapX;

    int nAlignGapY = (int)(m_nRatioY + 0.5f);
    nAlignGapY = (nAlignGapY - 1) < 0 ? 1 : nAlignGapY;

    float* pMyLineV = NULL;
    float* pMyLineH = NULL;

    switch (refPoint)
    {
    case TOP_LEFT:
        {
            pMyLineH = &position.top;
            pMyLineV = &position.left;
        }
        break;
    case TOP_CENTER:
        {
            pMyLineH = &position.top;
            if (bKeepAspect)
            {
                pMyLineV = &position.right;
            }
        }
        break;
    case TOP_RIGHT:
        {
            pMyLineH = &position.top;
            pMyLineV = &position.right;
        }
        break;
    case RIGHT_CENTER:
        {
            pMyLineV = &position.right;
            if (bKeepAspect)
            {
                pMyLineH = &position.bottom;
            }
        }
        break;
    case BOTTOM_RIGHT:
        {
            pMyLineH = &position.bottom;
            pMyLineV = &position.right;
        }
        break;
    case BOTTOM_CENTER:
        {
            pMyLineH = &position.bottom;
            if (bKeepAspect)
            {
                pMyLineV = &position.right;
            }
        }
        break;
    case BOTTOM_LEFT:
        {
            pMyLineH = &position.bottom;
            pMyLineV = &position.left;
        }
        break;
    case LEFT_CENTER:
        {
            pMyLineV = &position.left;
            if (bKeepAspect)
            {
                pMyLineH = &position.bottom;
            }
        }
        break;
    default:
        ASSERT(0);
        break;
    }

    float OffsetY = 0;
    float OffsetX = 0;
    if(pMyLineH &&
        ((((static_cast<int>(*pMyLineH) % nAlignGapY) + (nAlignGapY>>2))%nAlignGapY) < (nAlignGapY>>1)))
    {
        OffsetY = static_cast<float>(*pMyLineH + nAlignGapY/2.0)
            - (static_cast<int>(*pMyLineH + nAlignGapY/2.0) % nAlignGapY);
        OffsetY -= *pMyLineH;
    }

    if(pMyLineV &&
        ((((static_cast<int>(*pMyLineV) % nAlignGapX) + (nAlignGapX>>2))%nAlignGapX) < (nAlignGapX>>1)))
    {
        OffsetX = static_cast<float>(*pMyLineV + nAlignGapX/2.0)
            - (static_cast<int>(*pMyLineV + nAlignGapX/2.0) % nAlignGapX);
        OffsetX -= *pMyLineV;

    }

    if (bKeepAspect)
    {
        double radixX = OffsetX/position.Width();
        double radixY = OffsetY/position.Height();


        switch (refPoint)
        {
        case TOP_LEFT:
        case BOTTOM_RIGHT:
            {
                if (radixX > radixY)
                {
                    OffsetY = (float)(position.Height()*radixX);
                }
                else
                {
                    OffsetX = (float)(position.Width()*radixY);
                }
            }
            break;
        case BOTTOM_LEFT:
        case TOP_RIGHT:
            {
                if (radixX > radixY)
                {
                    OffsetY = -(float)(position.Height()*radixX);
                }
                else
                {
                    OffsetX = -(float)(position.Width()*radixY);
                }
            }
            break;
        case TOP_CENTER:
        case LEFT_CENTER:
            {
                OffsetX = -(float)(position.Width()*radixY);
            }
            break;
        case BOTTOM_CENTER:
        case RIGHT_CENTER:
            {
                OffsetY = (float)(position.Height()*radixX);
            }
            break;
        default:
            ASSERT(0);
            break;
        }

    }

    if (pMyLineH)
    {
        *pMyLineH += OffsetY;
    }
    if (pMyLineV)
    {
        *pMyLineV += OffsetX;
    }

    rect = position;
    return TRUE;
}

void CLayoutDesignerCtrl::SelectDrawObj(CS3DrawObj* pObj, BOOL bAdd)
{
    if (!bAdd)
    {
        POSITION pos = m_selectObjList.GetHeadPosition();
        while (pos != NULL)
        {
            InvalidateDrawObj(m_selectObjList.GetNext(pos));
        }
        m_selectObjList.RemoveAll();
		if(this->m_pSelectObj)
		{
			m_pSelectObj  =NULL;
			m_selectRect.SetRectEmpty();
			this->Invalidate();	 
		}
    }

    if (pObj == NULL || IsSelected(pObj))
    {
        return;
    }

    m_selectObjList.AddTail(pObj);

//	if(bAdd)GetParent()->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 0, (LPARAM)&m_selectObjList);
    InvalidateDrawObj(pObj);
}

void CLayoutDesignerCtrl::SelectDrawObjWithinRect(FRect& rect, BOOL bAdd)
{
    FRect selectRect;
    selectRect.SetRectEmpty();
    FRect rectObj;
    int nHit = -1;

    if (!bAdd)
        SelectDrawObj(NULL);

    POSITION posObj = m_drawObjList.GetHeadPosition();
    while (posObj != NULL)
    {
        CS3DrawObj* pObj = m_drawObjList.GetNext(posObj);
        PROPERTY_TYPE proType;
        pObj->GetPropertyType(proType);
        if (proType != BACKGROUND)
        {
            if (pObj->Intersects(rect))
            {
                SelectDrawObj(pObj, TRUE);

                pObj->GetLogicPos(rectObj);

                nHit += 1;
                MergeSelectRect(selectRect, rectObj);
            }
        }
    }

    if (nHit > 0)
    {
        rect = selectRect;
    }
    else
    {
        rect.SetRectEmpty();
    }
}

void CLayoutDesignerCtrl::DeselectDrawObj(CS3DrawObj* pObj)
{
    POSITION pos = m_selectObjList.Find(pObj);
    if (pos != NULL)
    {
        InvalidateDrawObj(pObj);
        m_selectObjList.RemoveAt(pos);
    }
}

void CLayoutDesignerCtrl::PrepareClone()
{
    m_lastClonePoint.SetPoint(0, 0);
}

void CLayoutDesignerCtrl::CloneSelection(CS3DrawObjList* pClipList)
{

	LayoutCmdFactory *pFactor = (LayoutCmdFactory *)this->m_CmdEngine.getFactory();
	objsColoneCmd *pCmd = (objsColoneCmd*)pFactor->newCmd(CMD_OBJSCOLONE);
	pCmd->pCopyObjsList = pClipList;
	if(!m_CmdEngine.execCmd(pCmd))
		pFactor->delCmd(pCmd);
#if 0
    FPoint clipPoint(m_lastClonePoint);
    ClientToLayout(clipPoint);
    FPoint pointOffset(0, 0);
    FPoint pointOrigin(0, 0);
    FRect rectLogic;
    CRect rectPos;
    BOOL bStartPoint = FALSE;
    POSITION pos = pClipList->GetHeadPosition();
    while (pos != NULL)
    {
        CS3DrawObj* pObj = pClipList->GetNext(pos);

        if (!bStartPoint)
        {
            pObj->GetLogicPos(rectLogic);
            pointOrigin.x = rectLogic.left;
            pointOrigin.y = rectLogic.top;
            pointOffset = clipPoint;
        }
        else
        {
            pObj->GetLogicPos(rectLogic);
            pointOffset.x = clipPoint.x + (rectLogic.left - pointOrigin.x);
            pointOffset.y = clipPoint.y + (rectLogic.top - pointOrigin.y);
        }

        pObj->MoveTo(pointOffset);
        pObj->GetLogicPos(rectLogic);
        LayoutToClient(rectLogic);
        rectLogic.ConvertToCRect(rectPos);
        pObj->SetPosition(rectPos);
        bStartPoint = TRUE;

        InsertDrawObj(pObj);
    }
#endif
}

void CLayoutDesignerCtrl::DeleteObj()
{
	LayoutCmdFactory *pFactor = m_CmdEngine.getFactory();
	bindLineDeleteCmd *pBindCmd = 
				dynamic_cast<bindLineDeleteCmd *>(pFactor->newCmd(CMD_BINDLINEDELETE));

	if(m_CmdEngine.execCmd(pBindCmd))
			return;
	else
		pFactor->delCmd(pBindCmd);

	objsGroupDeleteCmd *pDelGroupCmd = 
				dynamic_cast<objsGroupDeleteCmd *>(pFactor->newCmd(CMD_OBJSGROUPDELETE));

	if(m_CmdEngine.execCmd(pDelGroupCmd))
			return;
	else
		pFactor->delCmd(pDelGroupCmd);

	objDeleteCmd *pCmd = 
				dynamic_cast<objDeleteCmd *>(pFactor->newCmd(CMD_OBJDELETE));
	pCmd->pCtrlSelList = &m_selectObjList;
	if(!m_CmdEngine.execCmd(pCmd))
		pFactor->delCmd(pCmd);

   // Invalidate();
}

void CLayoutDesignerCtrl::TabChange()
    {
    POSITION posCurrSelect;
    POSITION posNextSelect;
    CS3DrawObj* pCurrObj = NULL;
    CS3DrawObj* pNextObj = NULL;
    PROPERTY_TYPE proType;
    if (m_selectObjList.GetCount() == 1)
    {

        if (m_drawObjList.GetCount() >= 1)
        {
            pCurrObj = m_selectObjList.GetHead();  
            SelectDrawObj(pCurrObj, FALSE);
            posCurrSelect = m_drawObjList.Find(pCurrObj);
            if(posCurrSelect != NULL)
            {
                pNextObj = m_drawObjList.GetNext(posCurrSelect);
                if (posCurrSelect != NULL)
                {
                    pNextObj = m_drawObjList.GetNext(posCurrSelect);                 
                }
                else
                {             
                    posCurrSelect = m_drawObjList.GetHeadPosition();
                    pNextObj = m_drawObjList.GetNext(posCurrSelect);
                }
                pNextObj->GetPropertyType(proType);
                if (proType == BACKGROUND)
                {
                    if (posCurrSelect != NULL)
                    {
                        pNextObj = m_drawObjList.GetNext(posCurrSelect);
                    }       
                }
                m_selectObjList.RemoveAll();
                m_selectObjList.AddTail(pNextObj);
                InvalidateDrawObj(m_selectObjList.GetHead());
            }        
        }
    }
    else if (m_selectObjList.GetCount() == 0)
    {
        if (m_drawObjList.GetCount() >= 1)
        {
            posCurrSelect = m_drawObjList.GetHeadPosition();
            pNextObj = m_drawObjList.GetNext(posCurrSelect);

            pNextObj->GetPropertyType(proType);
            if (proType == BACKGROUND)
            {
                if (posCurrSelect != NULL)
                {
                    pNextObj = m_drawObjList.GetNext(posCurrSelect);
                }
                else
                {
                    posCurrSelect = m_drawObjList.GetHeadPosition();
                    pNextObj = m_drawObjList.GetNext(posCurrSelect);
                }
            }
            pNextObj->GetPropertyType(proType);
            if (proType != BACKGROUND)
            {
                m_selectObjList.RemoveAll();
                m_selectObjList.AddTail(pNextObj);
                InvalidateDrawObj(m_selectObjList.GetHead());
            }
        }
    }
}

void CLayoutDesignerCtrl::MoveLeft()
{
    CS3DrawObj* pObj = NULL;
    FRect posOrig;
    FPoint delta(5, 0);
    FPoint dstPoint;

    if (m_selectObjList.GetCount() == 1)
    {
        pObj = m_selectObjList.GetHead();
        pObj->GetLogicPos(posOrig);
        
        FPoint origPoint(posOrig.left, posOrig.top);

        dstPoint = (FPoint)(origPoint - delta);
        pObj->MoveTo(dstPoint);
      
        GetParent()->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 3, (LPARAM)&m_selectObjList);
        RedrawWindow();
    }
    else if (m_selectObjList.GetCount() > 1)
    {

    }
}

void CLayoutDesignerCtrl::MoveRight()
{
    CS3DrawObj* pObj = NULL;
    FRect posOrig;
    FPoint delta(-5, 0);
    FPoint dstPoint;

    if (m_selectObjList.GetCount() == 1)
    {
        pObj = m_selectObjList.GetHead();
        pObj->GetLogicPos(posOrig);

        FPoint origPoint(posOrig.left, posOrig.top);

        dstPoint = (FPoint)(origPoint - delta);
        pObj->MoveTo(dstPoint);

        GetParent()->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 3, (LPARAM)&m_selectObjList);
        RedrawWindow();
    }
    else if (m_selectObjList.GetCount() > 1)
    {

    }
}

void CLayoutDesignerCtrl::MoveUp()
{
    CS3DrawObj* pObj = NULL;
    FRect posOrig;
    FPoint delta(0, 5);
    FPoint dstPoint;

    if (m_selectObjList.GetCount() == 1)
    {
        pObj = m_selectObjList.GetHead();
        pObj->GetLogicPos(posOrig);

        FPoint origPoint(posOrig.left, posOrig.top);

        dstPoint = (FPoint)(origPoint - delta);
        pObj->MoveTo(dstPoint);

        GetParent()->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 3, (LPARAM)&m_selectObjList);
        RedrawWindow();
    }
    else if (m_selectObjList.GetCount() > 1)
    {

    }
}

void CLayoutDesignerCtrl::MoveDown()
{
    CS3DrawObj* pObj = NULL;
    FRect posOrig;
    FPoint delta(0, -5);
    FPoint dstPoint;

    if (m_selectObjList.GetCount() == 1)
    {
        pObj = m_selectObjList.GetHead();
        pObj->GetLogicPos(posOrig);

        FPoint origPoint(posOrig.left, posOrig.top);

        dstPoint = (FPoint)(origPoint - delta);
        pObj->MoveTo(dstPoint);

        GetParent()->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, 3, (LPARAM)&m_selectObjList);
        RedrawWindow();
    }
    else if (m_selectObjList.GetCount() > 1)
    {

    }
}

BOOL CLayoutDesignerCtrl::IsSelected(const CS3DrawObj* pObj) const
{
    CS3DrawObj* pDraw = (CS3DrawObj*)pObj;
    return m_selectObjList.Find(pDraw) != NULL;
}

void CLayoutDesignerCtrl::InsertDrawObj(CS3DrawObj* pObj)
{
    m_drawObjList.AddTail(pObj);


    PROPERTY_TYPE proType;
    pObj->GetPropertyType(proType);
    if (proType == AUDIO)
    {
        ASSERT(m_pAudioObj == NULL);
        m_pAudioObj = pObj;
    }

    if (m_bAlignGrid/*pObj->m_AlignMode > CS3DrawObj::ALIGN_NONE*/)
    {
        FRect rect;
        pObj->GetLogicPos(rect);

        FPoint point(0,0);
        AlignToGridByPoint(rect, point);

        pObj->SetLogicPos(rect);
    }
}

void CLayoutDesignerCtrl::RemoveDrawObj(CS3DrawObj* pObj)
{
    POSITION pos = m_drawObjList.Find(pObj);
    if (pos != NULL)
    {
        m_drawObjList.RemoveAt(pos);
    }
}

HRESULT CLayoutDesignerCtrl::CloseLayout()
{
    HRESULT hr = S_OK;

    // Remove all draw obj in list
    m_drawObjList.RemoveAll();
    m_selectObjList.RemoveAll();
    RemoveGridLine();
    if (m_pSelectObj)
    {
        //delete m_pSelectObj;
        m_pSelectObj = NULL;
    }

    m_DragMediaFiles.clear();

    // Re init all parameters
    m_selectRect.SetRectEmpty();
    m_LayoutSize.SetSize(0, 0);
    m_LayoutRect.SetRectEmpty();

    m_ViewportRect.SetRectEmpty();

    m_Scale     = 1.0f;
    m_XTrans    = 0.0f;
    m_XTrans    = 0.0f;

    m_nSelectMode   = SEL_MODE_NONE;
    m_nDownFlags    = 0;
    m_downPoint.SetPoint(-1, -1);
    m_lastPoint.SetPoint(-1, -1);
    m_lastDown.SetPoint(-1, -1);
    m_lastScalPoint.SetPoint(0, 0);
    m_bSelection    = FALSE;
    m_lastAlignPoint.SetPoint(0, 0);
    m_lastClonePoint.SetPoint(0, 0);

    m_HscrollPos        = 0;
    m_VscrollPos        = 0;

    m_fAlignFactor      = 0.4f;
    m_bAlignGrid        = FALSE;

    m_pAudioObj         = NULL;

    return hr;
}

void CLayoutDesignerCtrl::SelectAllDrawObj(BOOL bUnSelAll)
{
  
    m_selectObjList.RemoveAll();
    if(bUnSelAll)
    {
        RedrawWindow();
        return;
    }

    FRect selectRect;
    FRect rectObj;
    POSITION posSelect = NULL;
    POSITION pos = m_drawObjList.GetHeadPosition();

    selectRect.SetRectEmpty();
    rectObj.SetRectEmpty();
    while (pos != NULL)
    {
        CS3DrawObj* pObj = m_drawObjList.GetNext(pos);

        PROPERTY_TYPE proType;
        pObj->GetPropertyType(proType);
        if (proType != BACKGROUND)
        {
            SelectDrawObj(pObj, TRUE);

            pObj->GetLogicPos(rectObj);

            MergeSelectRect(selectRect, rectObj);
        }
    }

    if (!selectRect.IsRectEmpty())
    {
        FRect rect;
        CRect rectposition;
        rect = selectRect;
        LayoutToClient(rect);
		rect.ConvertToCRect(rectposition);
		if(this->m_selectObjList.GetSize() > 1)
		{
			m_pSelectObj = new CS3DrawRectObj(selectRect, rectposition, 0, BACKGROUND);
			m_pSelectObj->SetHitTestSize(m_nHandleSize, m_nLineSize);
			m_pSelectObj->SetKeepAspect(TRUE);
			LayoutCmdFactory *pFactor = (LayoutCmdFactory *)this->m_CmdEngine.getFactory();
			objsGroupedCmd *pCmd = (objsGroupedCmd *)pFactor->newCmd(CMD_OBJSGROUPED);
			pCmd->pGroupBound = m_pSelectObj;
			pCmd->setSelObjs(&this->m_selectObjList);
			this->m_CmdEngine.execCmd(pCmd);
		}
    }

    RedrawWindow();
}

void CLayoutDesignerCtrl::InvalidateDrawObj(CS3DrawObj* pObj)
{
    FRect rect;
    CRect rectObj;
    pObj->GetLogicPos(rect);
    LayoutToClient(rect);
    rect.ConvertToCRect(rectObj);

    InvalidateRect(rectObj, FALSE);
}

/// The point is in the logic position
CS3DrawObj* CLayoutDesignerCtrl::GetDrawObjAtPoint(const FPoint point, BOOL bSetSelect)
{
    CS3DrawObj* pSelectObj = NULL;
    CS3DrawObj* pObj = NULL;
    int nHit = -1;
    UINT nHandle = 0;
    UINT nLine = 0;

    // Search from the tail of the draw obj list, the first hit one has the largest depth value, should return it
    POSITION pos = m_drawObjList.GetTailPosition();
    while(pos != NULL)
    {
        pObj = m_drawObjList.GetPrev(pos);
		FRect FRectObj;
		pObj->GetLogicPos(FRectObj);
		LayoutToClient(FRectObj);

		if(FRectObj.left > FRectObj.right)
		{
			float left = FRectObj.left;
			FRectObj.left = FRectObj.right;
			FRectObj.right = left;
		}

		if(FRectObj.top > FRectObj.bottom)
		{
			float top = FRectObj.top;
			FRectObj.top = FRectObj.bottom;
			FRectObj.bottom = top;
		}
		this->ClientToLayout(FRectObj);
		pObj->SetLogicPos(FRectObj);
        PROPERTY_TYPE proType;
        nHit = -1;
        nHandle = 0;
        nLine = 0;
        pObj->GetPropertyType(proType);
        if (proType != BACKGROUND)
        {
            SetObjHitTestSize(pObj);
            nHit = pObj->HitTest(point, nHandle, nLine);

            if (nHit == 0)
            {
                pSelectObj = pObj;

                if (bSetSelect)
                {
                    m_nSelectMode = SEL_MODE_MOVE;

                    if (nHandle > 0)
                    {
                        pSelectObj->SetDragHandle(nHandle);
                        m_nSelectMode = SEL_MODE_SIZE;
                    }
                    if (nLine > 0)
                    {
                        pSelectObj->SetDragLine(nLine);
                        m_nSelectMode = SEL_MODE_SIZE;
                    }

                    SetCursor(GetDrawObjCursor(pSelectObj, nHandle, nLine));
                }

                break;
            }
        }
    }

    return pSelectObj;
}

/// Draw in physical coordinate
void CLayoutDesignerCtrl::DrawGrid(CDC* pDC)
{
    if (!m_bShowGrid)
    {
        return;
    }

    // Major unit lines
    CPen penDot;
    penDot.CreatePen(PS_DOT, 0, m_GridColor);
    pDC->SelectObject(&penDot);

    PGRIDLINE gridLine;
    POSITION pos = m_GridLine.GetHeadPosition();
    while(pos != NULL)
    {
        gridLine = m_GridLine.GetNext(pos);
        CPoint startPoint = gridLine->startPoint;
        CPoint endPoint = gridLine->endPoint;
        LayoutToClient(startPoint);
        LayoutToClient(endPoint);

        pDC->MoveTo(startPoint);
        pDC->LineTo(endPoint);
    }
}

/// Draw in physical coordinate
void CLayoutDesignerCtrl::DrawRectObj(CS3DrawObj* pObj, CDC* pDC)
{
    CS3DrawRectObj* pRectObj = (CS3DrawRectObj*)pObj;
    pRectObj->Draw(pDC);
}

/// Draw in physical coordinate
void CLayoutDesignerCtrl::DrawObj(CS3DrawObj* pObj, CDC* pDC)
{
    pObj->Draw(pDC);
}

/// Draw in physical coordinate
void CLayoutDesignerCtrl::DrawSelectLine(CDC* pDC)
{
    CRect rect;

    // If multi select obj in the include rect, draw the include edge line,
    // else, draw the select rect edge line.
    if (m_pSelectObj)
    {
        /// The select obj's edge is the multi obj's union edge,
        /// should add delta value to display and hit test.
        FRect rectObj;
        m_pSelectObj->GetLogicPos(rectObj);
        rectObj.NormalizeRect();
        LayoutToClient(rectObj);
        rectObj.ConvertToCRect(rect);
        rect.left   -= m_nSelectRectLength;
        rect.top    -= m_nSelectRectLength;
        rect.right  += m_nSelectRectLength;
        rect.bottom += m_nSelectRectLength;
        m_pSelectObj->SetPosition(rect);
        pDC->DrawFocusRect(rect);
        m_pSelectObj->DrawTracker(pDC);
    }
    else
    {
		
        rect = m_selectRect;
		if(rect.IsRectEmpty())
			return;
        LayoutToClient(rect);
        pDC->DrawFocusRect(rect);
    }
}

/// Draw in physical coordinate
void CLayoutDesignerCtrl::DrawLayout(CDC* pDC)
{
    CBrush brush;
	if (!brush.CreateSolidBrush( m_LayoutColor))
        return;
    CBrush brushShadow;
    if (!brushShadow.CreateSolidBrush(m_LayoutShadowColor))
        return;

    brush.UnrealizeObject();
    FRect rect = m_LayoutRect;
    LayoutToClient(rect);
    CRect rectClient;
    rect.ConvertToCRect(rectClient);

    pDC->FillRect(rectClient, &brush);

    CRect ShadowRect;

    ShadowRect.left = rectClient.right;
    ShadowRect.right = rectClient.right + 5;
    ShadowRect.top = rectClient.top + 5;
    ShadowRect.bottom = rectClient.bottom + 5;
    pDC->FillRect(ShadowRect, &brushShadow);

    ShadowRect.left = rectClient.left + 5;
    ShadowRect.right = rectClient.right + 5;
    ShadowRect.top = rectClient.bottom;
    ShadowRect.bottom = rectClient.bottom + 5;
    pDC->FillRect(ShadowRect, &brushShadow);


    /// paint the grid
    DrawGrid(pDC);

    //// Outlines
    CPen penSolid;
    penSolid.CreatePen(PS_SOLID, 0, m_BorderColor);
    pDC->SelectObject(&penSolid);

    pDC->MoveTo(rectClient.left, rectClient.top);
    pDC->LineTo(rectClient.right, rectClient.top);
    pDC->LineTo(rectClient.right, rectClient.bottom);
    pDC->LineTo(rectClient.left, rectClient.bottom);
    pDC->LineTo(rectClient.left, rectClient.top);
}

/// Draw in physical coordinate
void CLayoutDesignerCtrl::DrawLayers(CDC* pDC)
{
    int nCount = 0;
    CString szNameType;
    CString szFrame;
    CRect rectText;

    CS3DrawObjList  drawEdgeList;
    CS3DrawObj* pObj = NULL;
    CRect rect;
    FRect rectObj;
    FRect rectCover;
    FRect rectTest;
    FRect rectKeep;
    POSITION posSelect = NULL;
    POSITION posCover = NULL;
    CS3DrawObj* pObjCover = NULL;
    BOOL bCover = FALSE;
    POSITION pos = m_drawObjList.GetHeadPosition();
    while (pos != NULL)
    {
        pObj = m_drawObjList.GetNext(pos);
        PROPERTY_TYPE proType;
        pObj->GetPropertyType(proType);
        if (proType != BACKGROUND)
        {
            pObj->GetLogicPos(rectObj);
            LayoutToClient(rectObj);
            //rectObj.ConvertToCRect(rect);
            rect.left = rectObj.left+ 0.5;
            rect.top  = rectObj.top + 0.5;
            rect.right = rectObj.right + 0.5;
            rect.bottom = rectObj.bottom + 0.5;
         
            pObj->SetPosition(rect);
            pObj->Draw(pDC);

            if (proType != AUDIO)
            {
                //szFrame.AppendFormat(_T("%d"), nCount);
				pObj->GetSerialName(szFrame);
				if(szFrame.IsEmpty())
				{
					pObj->GetName(szFrame);
					pObj->GetTypeCount(nCount);
					szFrame.AppendFormat(_T("%d"), nCount);
					pObj->SetSerialName(szFrame);
				}
            }else
				pObj->GetName(szFrame);

            rectText.left = rect.left + 4;
            rectText.top = rect.top + 4;
            rectText.right = rect.right - 4;
            rectText.bottom = rect.bottom - 4;

            rectText.right = rectText.right > rect.right ? rect.right : rectText.right;
            rectText.bottom = rectText.bottom > rect.bottom ? rect.bottom : rectText.bottom;

            CFont RenderFont;

            RenderFont.CreateFont(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                VARIABLE_PITCH,
                _T("Calibri"));

            CFont* pOldFont = pDC->SelectObject (&RenderFont);

            pDC->DrawText(szFrame, rectText, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

            nCount++;

            pDC->SelectObject(pOldFont);

            RenderFont.DeleteObject();
        }

        bCover = FALSE;
        posSelect = m_selectObjList.Find(pObj);
        if (posSelect != NULL)
        {
            pObj->GetLogicPos(rectObj);
            rectTest = rectObj;
            rectKeep = rectObj;
            LayoutToClient(rectObj);
            rectObj.ConvertToCRect(rect);
            pObj->SetPosition(rect);

            posCover = m_drawObjList.GetTailPosition();
            while (posCover != NULL)
            {
                pObjCover = m_drawObjList.GetPrev(posCover);

                if (pObj == pObjCover)
                {
                    break;
                }
                pObjCover->GetPropertyType(proType);
                if (proType == BACKGROUND)
                {
                    continue;
                }

                pObjCover->GetLogicPos(rectCover);
                if (rectTest.IntersectRect(&rectKeep, &rectCover))
                {
                    bCover = TRUE;
                    break;
                }
            }

            if (m_pSelectObj == NULL && !bCover)
            {
                pObj->DrawTracker(pDC);
            }
            else
            {
                drawEdgeList.AddTail(pObj);
            }
        }

    }

    // Let highlight edge line be the last draw, so that even the obj be covered by other obj, still draw the edge line to show where is the obj
	posSelect = drawEdgeList.GetHeadPosition();
	while (posSelect != NULL)
	{
		pObj = drawEdgeList.GetNext(posSelect);
		pObj->HighlightEdgeLine(pDC);
	}

    drawEdgeList.RemoveAll();

}

/// Change the draw obj list index, to change the z order
void CLayoutDesignerCtrl::MoveBack()
{
 	LayoutCmdFactory *pFactor = (LayoutCmdFactory *)m_CmdEngine.getFactory();
	objToBackCmd *pCmd = (objToBackCmd *)pFactor->newCmd(CMD_OBJTOBACK);
	if(!m_CmdEngine.execCmd(pCmd))
		pFactor->delCmd(pCmd);

}

/// Change the draw obj list index, to change the z order
void CLayoutDesignerCtrl::MoveForward()
{
	LayoutCmdFactory *pFactor = (LayoutCmdFactory *)m_CmdEngine.getFactory();
	objToBackCmd *pCmd = (objToBackCmd *)pFactor->newCmd(CMD_OBJTOFRONT);
	if(!m_CmdEngine.execCmd(pCmd))
		pFactor->delCmd(pCmd);

}

/// Change the draw obj list index, to change the z order
void CLayoutDesignerCtrl::MoveToBack()
{
	LayoutCmdFactory *pFactor = (LayoutCmdFactory *)m_CmdEngine.getFactory();
	objToBackCmd *pCmd = (objToBackCmd *)pFactor->newCmd(CMD_OBJTOBACK);
	pCmd->nType = BIG_STEP;
	if(!m_CmdEngine.execCmd(pCmd))
		pFactor->delCmd(pCmd);
}

/// Change the draw obj list index, to change the z order
void CLayoutDesignerCtrl::MoveToFront()
{
	LayoutCmdFactory *pFactor = (LayoutCmdFactory *)m_CmdEngine.getFactory();
	objToBackCmd *pCmd = (objToBackCmd *)pFactor->newCmd(CMD_OBJTOFRONT);
	pCmd->nType = BIG_STEP;
	if(!m_CmdEngine.execCmd(pCmd))
		pFactor->delCmd(pCmd);
}

/// In logic coordinate
void CLayoutDesignerCtrl::MoveObjPos(CS3DrawObj* pObj, const FPoint posDelta, const FPoint mousePoint)
{
    if (posDelta.x == 0 && posDelta.y == 0)
    {
        return;
    }

    BOOL bAlign = FALSE;
    FRect rectObj;
    //pObj->GetLogicPos(rectObj);
    //rectObj.OffsetRect(posDelta);

    rectObj = pObj->m_OrigPosition;
    FPoint posDeltaOrig = mousePoint - FPoint(pObj->m_OrigPoint);

    if (m_bAlignGrid/*pObj->m_AlignMode == CS3DrawObj::ALIGN_TO_GRID*/)
    {
        AlignToGridByPoint(rectObj, posDeltaOrig);
    }
    else
    {
        rectObj.OffsetRect(posDeltaOrig);
    }

    pObj->SetLogicPos(rectObj);
    Invalidate();

}

/// In logic coordinate
void CLayoutDesignerCtrl::MoveObjPos(CS3DrawObj* pObj, const FRect position, const FPoint clickPoint)
{
    FRect rect;
    pObj->GetLogicPos(rect);
    if (position == rect)
        return;

    rect = position;
    BOOL bAlign = FALSE;

    //if (m_bAlignGrid)
    //{
    //    CPoint point;
    //    if (m_objAlignPoint.Lookup(pObj, point))
    //    {
    //        // Calc the align distance, min distance should be 1
    //        int nAlignGap = (int)(m_nRatio * m_fAlignFactor + 0.5f);
    //        nAlignGap = (nAlignGap - 1) < 0 ? 1 : nAlignGap;

    //        if (abs(position.left - point.x) > nAlignGap || abs(position.top - point.y) > nAlignGap)
    //        {
    //            m_objAlignPoint.RemoveKey(pObj);
    //        }
    //    }
    //    else
    //    {
    //        bAlign = TRUE;
    //    }

    //    if (bAlign)
    //    {
    //        if (AlignToGrid(rect, clickPoint))
    //        {
    //            point.SetPoint(rect.left, rect.top);
    //            m_objAlignPoint.SetAt(pObj, point);
    //        }
    //    }
    //}

    pObj->SetLogicPos(rect);
    Invalidate();

}

/// In logic coordinate
void CLayoutDesignerCtrl::MoveObjHandlePos(CS3DrawObj* pObj, const UINT nHandle, const FPoint position)
{
    FRect rect;
    FPoint origin(0.0f, 0.0f);
    FLOAT fScale = -1.0f;
    rect = pObj->MoveHandleTo(nHandle, position, origin, fScale, position);

    MoveObjPos(pObj, rect, position);

}

/// In logic coordinate
void CLayoutDesignerCtrl::ResizeObj(CS3DrawObj* pObj, const FPoint position, const UINT nHandle, BOOL bMouseMove)
{
    if (nHandle <= 0)
    {
        return;
    }

    FPoint origin(0.0f, 0.0f);
    FPoint lastPoint(0.0f, 0.0f);
    FLOAT fScale = 0.0f;
    if (pObj != m_pSelectObj && m_pSelectObj != NULL)
    {
        GetSelectRectOrigin(nHandle, 0, origin);
        GetSelectRectLast(nHandle, 0, lastPoint);
        //m_pSelectObj->GetScale(fScale);
        m_pSelectObj->GetAspectRatio(fScale);
    }

    FRect rect;
    FRect rectLogic;
    BOOL bAlign = FALSE;
    pObj->GetLogicPos(rectLogic);
    if (nHandle > 0)
    {
        rect = pObj->MoveHandleTo(nHandle, position, origin, fScale, lastPoint);
    }

    if (rect == rectLogic)
    {
        return;
    }

    if (m_bAlignGrid/*pObj->m_AlignMode > CS3DrawObj::ALIGN_NONE*/ && bMouseMove)
    {
        POINT_POS refPoint;
        switch(nHandle)
        {
            /// Top Left
        case 1:
            {
                refPoint = TOP_LEFT;
            }
            break;

            /// Top Center
        case 2:
            {
                refPoint = TOP_CENTER;
            }
            break;

            /// Top Right
        case 3:
            {
                refPoint = TOP_RIGHT;
            }
            break;

            /// Right Center
        case 4:
            {
                refPoint = RIGHT_CENTER;
            }
            break;

            /// Right Bottom
        case 5:
            {
                refPoint = BOTTOM_RIGHT;
            }
            break;

            /// Bottom Center
        case 6:
            {
                refPoint = BOTTOM_CENTER;
            }
            break;

            /// Bottom Left
        case 7:
            {
                refPoint = BOTTOM_LEFT;
            }
            break;

            /// Left Center
        case 8:
            {
                refPoint = LEFT_CENTER;
            }
            break;
        default:
            ASSERT(FALSE);
            break;

        }
        AlignToGridResizeByPoint(rect, refPoint, pObj->GetKeepAspect());
    }

    pObj->SetLogicPos(rect);
    Invalidate();
}

/// In logic coordinate
void CLayoutDesignerCtrl::ResizeObjLine(CS3DrawObj* pObj, const FPoint deltaPosition, const UINT nLine, BOOL bMouseMove)
{
    if (nLine <= 0)
    {
        return;
    }

    FPoint lastPoint(0.0f, 0.0f);
    FPoint origin(0.0f, 0.0f);
    FLOAT fScale = 0.0f;
    if (pObj != m_pSelectObj && m_pSelectObj != NULL)
    {
        GetSelectRectOrigin(0, nLine, origin);
        GetSelectRectLast(0, nLine, lastPoint);
        m_pSelectObj->GetAspectRatio(fScale);
    }

    FRect rect;
    FRect rectLogic;
    BOOL bAlign = FALSE;
    pObj->GetLogicPos(rectLogic);

    if (nLine > 0)
    {
        rect = pObj->MoveLineTo(nLine, deltaPosition, origin, fScale, lastPoint);
    }

    if (rect == rectLogic)
    {
        return;
    }

    if (m_bAlignGrid/*pObj->m_AlignMode > CS3DrawObj::ALIGN_NONE*/ && bMouseMove)
    {
        LINE_POS refLine;
        switch(nLine)
        {
        case 1:
            refLine = TOP;
            break;
        case 2:
            refLine = RIGHT;
            break;
        case 3:
            refLine = BOTTOM;
            break;
        case 4:
            refLine = LEFT;
            break;
        default:
            ASSERT(0);
        }
        AlignToGridResizeByLine(rect, refLine, pObj->GetKeepAspect());
    }

    pObj->SetLogicPos(rect);
    Invalidate();
}

/// The select obj's logic pos is the union edge of the included ojbs,
/// but the edge should has a delta in display and hit test
int CLayoutDesignerCtrl::HitTestSelectRect(const FPoint point, UINT& nHandle, UINT& nLine)
{
    int nHit = -1;

    FRect rectSelect;
    FRect rectTest;
    CRect rect;
    m_pSelectObj->GetLogicPos(rectSelect);
    rectTest = rectSelect;
    LayoutToClient(rectTest);
    rectTest.ConvertToCRect(rect);
    rect.left   -= m_nSelectRectLength;
    rect.top    -= m_nSelectRectLength;
    rect.right  += m_nSelectRectLength;
    rect.bottom += m_nSelectRectLength;
    m_pSelectObj->SetPosition(rect);
    ClientToLayout(rect);
    rectTest = rect;
    m_pSelectObj->SetLogicPos(rectTest);
    nHit = m_pSelectObj->HitTest(point, nHandle, nLine);
    m_pSelectObj->SetLogicPos(rectSelect);

    return nHit;
}

void CLayoutDesignerCtrl::GetSelectRectOrigin(const UINT nHandle, const UINT nLine, FPoint& originPoint)
{
    FPoint origin(0.0f, 0.0f);
    FRect position;
    position.SetRectEmpty();

    if (m_pSelectObj)
    {
        m_pSelectObj->GetLogicPos(position);
    }

    switch (nLine)
    {
    case 1:
        origin.SetPoint(position.left, position.bottom);
        break;
    case 2:
    case 3 :
        origin.SetPoint(position.left, position.top);
        break;
    case 4:
        origin.SetPoint(position.right, position.top);
        break;

    default:
        break;
    }

    switch (nHandle)
    {
    case 1:
        origin.SetPoint(position.right, position.bottom);
        break;
    case 2:
    case 3:
        origin.SetPoint(position.left, position.bottom);
        break;
    case 4:
    case 5:
    case 6:
        origin.SetPoint(position.left, position.top);
        break;
    case 7:
    case 8:
        origin.SetPoint(position.right, position.top);
        break;

    default:
        break;
    }

    originPoint = origin;
}

void CLayoutDesignerCtrl::GetSelectRectLast(const UINT nHandle, const UINT nLine, FPoint& point)
{
    FPoint last(0.0f, 0.0f);
    FRect position;
    position.SetRectEmpty();

    if (m_pSelectObj)
    {
        m_pSelectObj->GetLogicPos(position);
    }

    switch (nLine)
    {
    case 1:
        last.SetPoint(position.right, position.top);
        break;
    case 2:
    case 3 :
        last.SetPoint(position.right, position.bottom);
        break;
    case 4:
        last.SetPoint(position.left, position.bottom);
        break;

    default:
        break;
    }

    switch (nHandle)
    {
    case 1:
        last.SetPoint(position.left, position.top);
        break;
    case 2:
    case 3:
        last.SetPoint(position.right, position.top);
        break;
    case 4:
    case 5:
    case 6:
        last.SetPoint(position.right, position.bottom);
        break;
    case 7:
    case 8:
        last.SetPoint(position.left, position.bottom);
        break;

    default:
        break;
    }

    point = last;
}

HCURSOR CLayoutDesignerCtrl::GetDrawObjCursor(CS3DrawObj* pObj, UINT nHandle, UINT nLine)
{
    if (pObj == NULL)
    {
        return AfxGetApp()->LoadStandardCursor(IDC_ARROW);
    }

    if (nHandle > 0)
    {
        return pObj->GetHandleCursor(nHandle);
    }
    if (nLine > 0)
    {
        return pObj->GetLineCursor(nLine);
    }

    return AfxGetApp()->LoadStandardCursor(IDC_HAND);
}

/// In logic coordinate
void CLayoutDesignerCtrl::SetupScrollBars(BOOL bOnInit)
{
	if (m_bSetupingScrollBars)
	{
		return;
	}

	m_bSetupingScrollBars = TRUE;

    SCROLLINFO   siH;
    SCROLLINFO   siV;

	CRect rectClient;
	GetClientRect(&rectClient);
	ClientToLayout(rectClient);

	siH.fMask       = SIF_ALL;
	siH.nMin        = m_ViewportRect.left;
	siH.nMax        = m_ViewportRect.right;
	
	siH.nPage       = rectClient.Width();
	if (bOnInit)
	{
		siH.nPos        = (siH.nMax + siH.nMin)/2  - siH.nPage/2;
	}
	else
	{
		SCROLLINFO   OrigsiH;
		GetScrollInfo(SB_HORZ, &OrigsiH);
		siH.nPos        = max(OrigsiH.nPos, siH.nMin) - (int)((int)siH.nPage - (int)OrigsiH.nPage)/2;
	}

	m_HscrollPos = siH.nPos;
	SetScrollInfo(SB_HORZ, &siH, TRUE);

	siV.fMask       = SIF_ALL;
	siV.nMin        = m_ViewportRect.top;
	siV.nMax        = m_ViewportRect.bottom;

	siV.nPage       = rectClient.Height();
	if (bOnInit)
	{
		siV.nPos        = (siV.nMax + siV.nMin)/2 - siV.nPage/2;
	}
	else
	{
		SCROLLINFO   OrigsiV;
		GetScrollInfo(SB_VERT, &OrigsiV);
		siV.nPos        = max(OrigsiV.nPos, siV.nMin) - (int)((int)siV.nPage - (int)OrigsiV.nPage)/2;
	}

	m_VscrollPos = siV.nPos;
	SetScrollInfo(SB_VERT, &siV, TRUE);

	CRulerCtrl *pCtrl =GetSlibingRuler();
	if(NULL != pCtrl)
		pCtrl->DoScrollChange();
	m_bSetupingScrollBars = FALSE;

	this->RecalcRulerLinesAttachment();
}


void CLayoutDesignerCtrl::ZoomImage(FLOAT zoomFactor)
{
    m_Scale = zoomFactor;
	SetupScrollBars();

    Invalidate(FALSE);


}

FLOAT CLayoutDesignerCtrl::GetZoomFactor()
{
    return m_Scale;
}


void CLayoutDesignerCtrl::CalcTransform(BOOL bResize)  
{
    m_XTrans = (0 - m_HscrollPos * m_Scale);
    m_YTrans = (0 - m_VscrollPos * m_Scale);
    TRACE(_T("m_HscrollPos :%d\n"),m_HscrollPos);
    TRACE(_T("m_VscrollPos :%d\n"),m_VscrollPos);
}


BOOL CLayoutDesignerCtrl::MergeSelectRect(FRect& selectRect, const FRect rectObj)
{
    BOOL bChanged = FALSE;
    int nLine = 0;

    if (selectRect.IsRectEmpty())
    {
        bChanged = TRUE;
        selectRect = rectObj;
    }
    else
    {
        if (selectRect.left > rectObj.left)
        {
            bChanged = TRUE;
            selectRect.left = rectObj.left;
        }
        if (selectRect.top > rectObj.top)
        {
            bChanged = TRUE;
            selectRect.top = rectObj.top;
        }
        if (selectRect.right < rectObj.right)
        {
            bChanged = TRUE;
            selectRect.right = rectObj.right;
        }
        if (selectRect.bottom < rectObj.bottom)
        {
            bChanged = TRUE;
            selectRect.bottom = rectObj.bottom;
        }
    }

    return bChanged;
}

void CLayoutDesignerCtrl::CalcScrollRange()
{
    // If move or resize the obj's position out of the last layout virtual coordinate rect
    // should re-calc the layout virtual coordinate rect
    CRect rectClient;
    GetClientRect(rectClient);
    ClientToLayout(rectClient);
    FRect largestLayoutRect = m_ViewportRect; //m_VirtualLayoutRect;
    POSITION pos = m_drawObjList.GetHeadPosition();
    while (pos != NULL)
    {
        CS3DrawObj* pObj = m_drawObjList.GetNext(pos);
        FRect rectPos;
        pObj->GetLogicPos(rectPos);

        largestLayoutRect.left = largestLayoutRect.left > rectPos.left ? rectPos.left : largestLayoutRect.left;
        largestLayoutRect.top = largestLayoutRect.top > rectPos.top ? rectPos.top : largestLayoutRect.top;
        largestLayoutRect.right = largestLayoutRect.right < rectPos.right ? rectPos.right : largestLayoutRect.right;
        largestLayoutRect.bottom = largestLayoutRect.bottom < rectPos.bottom ? rectPos.bottom : largestLayoutRect.bottom;
    }

    if (largestLayoutRect.left > rectClient.left && largestLayoutRect.right < rectClient.right
        && largestLayoutRect.top > rectClient.top && largestLayoutRect.bottom < rectClient.bottom)
    {
        m_ViewportRect = largestLayoutRect;
    }
    else
    {
        m_ViewportRect.left = largestLayoutRect.left < rectClient.left ? largestLayoutRect.left : rectClient.left;
        m_ViewportRect.right = largestLayoutRect.right > rectClient.right ? largestLayoutRect.right : rectClient.right;
        m_ViewportRect.top = largestLayoutRect.top < rectClient.top ? largestLayoutRect.top : rectClient.top;
        m_ViewportRect.bottom = largestLayoutRect.bottom > rectClient.bottom ? largestLayoutRect.bottom : rectClient.bottom;
    }

    SetupScrollBars();

}

/// In logic coordinate
void CLayoutDesignerCtrl::SetupVirtualGrid()
{
    INT Rx, Ry;
    GetConfig->Get(DESIGNER_GRID_X, Rx);
    m_nRatioX = Rx;
    GetConfig->Get(DESIGNER_GRID_Y, Ry);
    m_nRatioY = Ry;

    m_GridLine.RemoveAll();

    int left = (int)(m_LayoutRect.left + 0.5f);
    int top = (int)(m_LayoutRect.top + 0.5f);
    int right = (int)(m_LayoutRect.right + 0.5f);
    int bottom = (int)(m_LayoutRect.bottom + 0.5f);

    for (int x = left; x < right; x += m_nRatioX)
    {
        PGRIDLINE gridLine = new GRIDLINE;
        gridLine->startPoint.x   = x;
        gridLine->startPoint.y   = top;
        gridLine->endPoint.x     = x;
        gridLine->endPoint.y     = bottom;

        m_GridLine.AddTail(gridLine);
    }
    PGRIDLINE gridLine = new GRIDLINE;
    gridLine->startPoint.x   = right;
    gridLine->startPoint.y   = top;
    gridLine->endPoint.x     = right;
    gridLine->endPoint.y     = bottom;
    m_GridLine.AddTail(gridLine);

    for (int y = top; y < bottom; y += m_nRatioY)
    {
        PGRIDLINE gridLine = new GRIDLINE;
        gridLine->startPoint.x   = left;
        gridLine->startPoint.y   = y;
        gridLine->endPoint.x     = right;
        gridLine->endPoint.y     = y;

        m_GridLine.AddTail(gridLine);
    }
    PGRIDLINE gridLineBottom = new GRIDLINE;
    gridLine->startPoint.x   = left;
    gridLine->startPoint.y   = bottom;
    gridLine->endPoint.x     = left;
    gridLine->endPoint.y     = bottom;
    m_GridLine.AddTail(gridLineBottom);
}

/// Set the Obj's hit test handle, line size in the logic coordinate
void CLayoutDesignerCtrl::SetObjHitTestSize(CS3DrawObj* pObj)
{
    UINT nHandleSize = 0;
    UINT nLineSize = 0;

    pObj->GetHitHandleSize(nHandleSize);
    pObj->GetHitLineSize(nLineSize);
    ClientToLayout(nHandleSize);
    ClientToLayout(nLineSize);
    pObj->SetHitTestLogicSize(nHandleSize, nLineSize);
}

void CLayoutDesignerCtrl::RemoveGridLine()
{
    POSITION pos = m_GridLine.GetHeadPosition();
    while (pos != NULL)
    {
        delete m_GridLine.GetNext(pos);
    }

    m_GridLine.RemoveAll();
}

BOOL CLayoutDesignerCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
    CWnd::PreCreateWindow(cs);

    return TRUE;
}


/// CLayoutDesignerCtrl Message Handle
int CLayoutDesignerCtrl::OnCreate(LPCREATESTRUCT lpcs)
{
    if (CWnd::OnCreate(lpcs) == -1)
        return -1;
    m_Scale     = 1.0f;
    m_XTrans    = 0.0f;
    m_XTrans    = 0.0f;
	
	m_CmdEngine.bindTarget(this);
    // register drop target
	if ( m_DropTarget.Register( this ))
        return 0;
    else
        return -1;

}

void CLayoutDesignerCtrl::RecalcMultiSelectMergeRect()
{
	FRect MergeRect(0,0,0,0);
    POSITION pos = m_selectObjList.GetHeadPosition();
    while (pos != NULL)
    {
        CS3DrawObj *pObj = m_selectObjList.GetNext(pos);
      
        FRect myLogicalRect;
        pObj->GetLogicPos(myLogicalRect);

        if (MergeRect.IsRectEmpty())
        {
            MergeRect = myLogicalRect;
        }
        else
        {
            MergeRect.UnionRect(&MergeRect, &myLogicalRect);
        }
    }
    if (m_pSelectObj)
    {
        m_pSelectObj->SetLogicPos(MergeRect);
    }
}
/// In logic coordinate, transfer the mouse point to logic coordinate, and do operations
void CLayoutDesignerCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetFocus();
	if(this->m_bYLineCatched || this->m_bXLineCatched)
	{
		LayoutCmdFactory *pFactor = dynamic_cast<LayoutCmdFactory *>(m_CmdEngine.getFactory());
		pFactor->newCmdEnd(); //click on bindline,last cmd is ended.
		_baseCmd *pCmd = pFactor->newCmdBegin(CMD_BINDLINEMOVE);
		m_CmdEngine.execCmd(pCmd);
		return;
	}

    CS3DrawObj* pObj;
    m_nSelectMode = SEL_MODE_NONE;
	int nHit = -1;
	UINT nLine = 0;
    UINT nHandle = 0;
    BOOL bInclude = FALSE;
    FRect selectRect;
    FRect selectPhysicalRect;
    FRect rectObj;
    FPoint layoutPoint(point);
    ClientToLayout(layoutPoint);

    selectRect.SetRectEmpty();
    selectPhysicalRect.SetRectEmpty();
    rectObj.SetRectEmpty();

    // Check for resizing(only allowed on single selections)
    if (m_selectObjList.GetCount() == 1)
    {
        pObj = m_selectObjList.GetHead();
        SetObjHitTestSize(pObj);
        nHit = pObj->HitTest(layoutPoint, nHandle, nLine);
        if (nHandle > 0 || nLine > 0)
        {
            m_nSelectMode = SEL_MODE_SIZE;
            nHandle = nHandle > 0 ? nHandle : 0;
            pObj->SetDragHandle((UINT)nHandle);
            nLine = nLine > 0 ? nLine : 0;
            pObj->SetDragLine((UINT)nLine);

			LayoutCmdFactory *pFactor = (LayoutCmdFactory *)this->m_CmdEngine.getFactory();
			//pFactor->newCmdEnd();//clear last action cmd.
			objSizeCmd *pSizeCmd =(objSizeCmd*) pFactor->newCmd(CMD_OBJSIZE);
			ASSERT(pSizeCmd);
			this->m_CmdEngine.execCmd(pSizeCmd);
        }
    }

    // Check if point in the select rect
    nHit = -1;
    nLine = 0;
    nHandle = 0;
    if (m_pSelectObj)
    {
        m_pSelectObj->SetDragHandle(0);
        m_pSelectObj->SetDragLine(0);

        SetObjHitTestSize(m_pSelectObj);
        nHit = HitTestSelectRect(layoutPoint, nHandle, nLine);
        if (nHit == 0)
        {
            m_nSelectMode = SEL_MODE_MOVE;
        }
        if (nHandle > 0 || nLine > 0)
        {
            m_nSelectMode = SEL_MODE_SIZE;
			//group size here !
			
			LayoutCmdFactory *pFactor = m_CmdEngine.getFactory();
			objsGroupMoveCmd *pCmd = dynamic_cast<objsGroupMoveCmd *>(pFactor->newCmd(CMD_OBJSGROUPMOVE));
			if(!m_CmdEngine.execCmd(pCmd))
				pFactor->delCmd(pCmd);

            if (nHandle > 0)
            {
                m_pSelectObj->SetDragHandle(nHandle);
            }
            if (nLine > 0)
            {
                m_pSelectObj->SetDragLine(nLine);
            }
            SetCursor(GetDrawObjCursor(m_pSelectObj, nHandle, nLine));
        }
    }

    // See if the click was on an object, select and start move if so
    if (m_nSelectMode == SEL_MODE_NONE)
    {
        pObj = GetDrawObjAtPoint(layoutPoint);

        if (pObj == NULL)
        {
            m_nSelectMode = SEL_MODE_NONE;
        }
        else
        {
            if (!IsSelected(pObj))
            {
				LayoutCmdFactory *pFactor = m_CmdEngine.getFactory();
				objsGroupedCmd *pCmd = dynamic_cast<objsGroupedCmd *>(pFactor->getBuffedCmd());
                if ((nFlags & MK_CONTROL) != 0)
                {
                    if (m_pSelectObj)
                    {
                        m_pSelectObj->GetLogicPos(selectRect);
                        pObj->GetLogicPos(rectObj);

                        bInclude = MergeSelectRect(selectRect, rectObj);

                        m_pSelectObj->SetLogicPos(selectRect);
                    }
                    else
                    {
                        if (m_selectObjList.GetCount() > 0)
                        {
                            CS3DrawObj* pLastObj = m_selectObjList.GetTail();
                            pLastObj->GetLogicPos(rectObj);
                            MergeSelectRect(selectRect, rectObj);

                            pObj->GetLogicPos(rectObj);

                            bInclude = MergeSelectRect(selectRect, rectObj);

                            CRect rectposition;
                            selectPhysicalRect = selectRect;
                            LayoutToClient(selectPhysicalRect);
                            selectPhysicalRect.ConvertToCRect(rectposition);

							
							
							pCmd = dynamic_cast<objsGroupedCmd *>(pFactor->newCmdBegin(CMD_OBJSGROUPED));

                            m_pSelectObj = new CS3DrawRectObj(selectRect, rectposition, 0, BACKGROUND);
							pCmd->pGroupBound = m_pSelectObj;
                            m_pSelectObj->SetHitTestSize(m_nHandleSize, m_nLineSize);
                            m_pSelectObj->SetKeepAspect(TRUE);
							m_CmdEngine.execCmd(pCmd);

                        }
                    }
                    SelectDrawObj(pObj, TRUE);

					//m_CmdEngine.execCmd(pCmd);
                }
                else
                {

					//当直接点击一个对象，但未按下 "Ctrl"
					LayoutCmdFactory *pFactor = m_CmdEngine.getFactory();
					pFactor->newCmdEnd();

                    if (m_pSelectObj)
                    {
                    //    delete m_pSelectObj;
						this->m_selectRect.SetRectEmpty();
                        m_pSelectObj = NULL;
                    }

                    SelectDrawObj(pObj, FALSE);
                }
            }
        }
    }

    // Click on background, start a rect-selection
    if (m_nSelectMode == SEL_MODE_NONE)
    {

		//直接点击 "Layout"
		LayoutCmdFactory *pFactor = m_CmdEngine.getFactory();
		pFactor->newCmdEnd();

        if ((nFlags & MK_CONTROL) == 0)
        {
            SelectDrawObj(NULL);
        }

        m_nSelectMode = SEL_MODE_MULTI_SELECT;
    }


    // record the selected Layer's original point
    // Check for resizing(only allowed on single selections)
    POSITION pos = m_selectObjList.GetHeadPosition();
    while (pos != NULL)
    {
        CS3DrawObj* pObj = m_selectObjList.GetNext(pos);

        pObj->m_OrigPoint = point;
        ClientToLayout(pObj->m_OrigPoint);
        pObj->GetLogicPos(pObj->m_OrigPosition);
    }

    if (m_pSelectObj)
    {
        m_pSelectObj->m_OrigPoint = point;
        ClientToLayout(m_pSelectObj->m_OrigPoint);
        m_pSelectObj->GetLogicPos(m_pSelectObj->m_OrigPosition);
    }

    m_lastPoint     = point;
    SetCapture();
    m_nDownFlags    = nFlags;
    m_downPoint     = point;
    m_lastDown      = point;

    CWnd::OnLButtonDown(nFlags, point);
}

/// In logic coordinate, transfer the mouse point to logic coordinate, and do operations
void CLayoutDesignerCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{

	this->RecalcRulerLinesAttachment();

	LayoutCmdFactory *pFactor = dynamic_cast<LayoutCmdFactory *>(m_CmdEngine.getFactory());
	_baseCmd *pCmd = pFactor->getBuffedCmd();
	if(pCmd)
	{

			if(pCmd->isObjOf(CMD_OBJSGROUPED) && 
						this->m_pSelectObj )
			{
				objsGroupedCmd *pGroup = (objsGroupedCmd *)pCmd;
				pGroup->setSelObjs(&this->m_selectObjList);

				if((nFlags & MK_CONTROL) != 0)
					return ;
			}
			
			if(pCmd->isObjOf(CMD_OBJMOVE))
			{
				objMoveCmd *pMoveCmd = (objMoveCmd *)pCmd;
				pMoveCmd->addBoundObj(this->m_pSelectObj);
			}

			if(!pCmd->isObjOf(CMD_BINDLINEMOVE))
				m_CmdEngine.execCmd(pCmd);

			pFactor->newCmdEnd();
			
	}

	if (GetCapture() == this)
    {
        if (m_nSelectMode == SEL_MODE_MULTI_SELECT)
        {
            FRect selectRect;
            FPoint downPoint(m_downPoint);
            FPoint lastDown(m_lastDown);
            ClientToLayout(downPoint);
            ClientToLayout(lastDown);
            selectRect.SetRect(downPoint.x, downPoint.y, lastDown.x, lastDown.y);
            selectRect.NormalizeRect();

            SelectDrawObjWithinRect(selectRect, TRUE);

            if (m_pSelectObj)//zxy
            {
               // delete m_pSelectObj;
                m_pSelectObj = NULL;
            }

            if (!selectRect.IsRectEmpty())
            {
                FRect rect;
                CRect rectposition;
                rect = selectRect;
                LayoutToClient(rect);
                rect.ConvertToCRect(rectposition);
				
				LayoutCmdFactory *pFactor = m_CmdEngine.getFactory();
				objsGroupedCmd *pCmd = dynamic_cast<objsGroupedCmd *>(pFactor->newCmdBegin(CMD_OBJSGROUPED));
                m_pSelectObj = new CS3DrawRectObj(selectRect, rectposition, 0, BACKGROUND);
                m_pSelectObj->SetHitTestSize(m_nHandleSize, m_nLineSize);
                m_pSelectObj->SetKeepAspect(TRUE);
				pCmd->pGroupBound = m_pSelectObj;
				//pCmd->setSelObjs( &this->m_selectObjList);
				//pCmd->objsSelList = this->m_selectObjList;
				m_CmdEngine.execCmd(pCmd);
            }
            else
            {
                selectRect.ConvertToCRect(m_selectRect);
            }
        }

        if (m_nSelectMode != SEL_MODE_NONE)
        {
            Invalidate(FALSE);
        }

        m_lastClonePoint = point;
    }

    if (!m_bLBtnDblClk)
    {
        int nSelect = -1;
        FPoint hitPoint(point);
        ClientToLayout(hitPoint);
        if (m_selectObjList.GetCount() > 0)
        {
            nSelect = 0;
        }
        else if (m_LayoutRect.PtInRect(hitPoint))
        {
            nSelect = 1;
        }

        GetParent()->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, nSelect, (LPARAM)&m_selectObjList);
    }
    else
    {
        m_bLBtnDblClk = FALSE;
    }

    ReleaseCapture();

    if (point == m_downPoint)
    {
        m_bSelection = TRUE;
    }

	 
    CWnd::OnLButtonUp(nFlags, point);
}



/// In logic coordinate, transfer the mouse point to logic coordinate, and do operations
void CLayoutDesignerCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
    int nHit = -1;
    UINT nLine = 0;
    UINT nHandle = 0;
    UINT nDragHandle = 0;
    UINT nDragLine = 0;
    FRect position;
    FRect rect;
    CS3DrawObj* pObj = NULL;
    BOOL bHit = FALSE;
    FPoint layoutPoint(point);
    ClientToLayout(layoutPoint);

	if(this->m_bXLineCatched || this->m_bYLineCatched)
	{ 
		return;
	}

/*
	if(GetFocus() != this)
		SetFocus();
*/

    if (GetCapture() != this)
    {
        if (m_selectObjList.GetCount() == 1)
        {
            CS3DrawObj* pHitObj = NULL;
            CS3DrawObj* pObj = m_selectObjList.GetHead();
            SetObjHitTestSize(pObj);
            nHit = pObj->HitTest(layoutPoint, nHandle, nLine);

            // If the hit point covered by more than 1 layer obj, do not set the drag handle or line
            if (nHit == 0)
            {
                pHitObj = GetDrawObjAtPoint(layoutPoint, FALSE);
            }

            if (pHitObj != NULL && pHitObj != pObj)
            {
                return;
            }

            if (nHandle > 0)
            {
                bHit = TRUE;
                pObj->SetDragHandle(nHandle);
            }
            if (nLine > 0)
            {
                bHit = TRUE;
                pObj->SetDragLine(nLine);
            }
            SetCursor(GetDrawObjCursor(pObj, nHandle, nLine));
            if (bHit)
            {
                return;
            }
        }

        if (m_pSelectObj)
        {
            nHit = -1;
            nLine = 0;
            nHandle = 0;
            SetObjHitTestSize(m_pSelectObj);
            nHit = HitTestSelectRect(layoutPoint, nHandle, nLine);

            if (nHit == 0)
            {
                m_nSelectMode = SEL_MODE_MOVE;
            }

            if (nHandle > 0)
            {
                bHit = TRUE;
                m_pSelectObj->SetDragHandle(nHandle);
            }
            if (nLine > 0)
            {
                bHit = TRUE;
                m_pSelectObj->SetDragLine(nLine);
            }
            SetCursor(GetDrawObjCursor(m_pSelectObj, nHandle, nLine));
            if (bHit)  
            {
                return;
            }
        }

        m_lastDown = point;
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

        return;
    }

    if (m_nSelectMode == SEL_MODE_MULTI_SELECT)
    {
        CRect selectRect;
        CPoint downpoint = m_downPoint;
        CPoint curpoint = point;
        ClientToLayout(downpoint);
        ClientToLayout(curpoint);
        selectRect.SetRect(downpoint.x, downpoint.y, curpoint.x, curpoint.y);
        selectRect.NormalizeRect();

        m_selectRect = selectRect;

        m_lastDown = point;
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

        Invalidate();

        return;
    }

    FPoint lastPoint(m_lastPoint);
    ClientToLayout(lastPoint);
    FPoint delta = (FPoint)(layoutPoint - lastPoint);
    BOOL bKeepAspect = FALSE;
    BOOL bChangeAspect = FALSE;
    BOOL bResize = FALSE;

	LayoutCmdFactory *pFactor = this->m_CmdEngine.getFactory();
	_baseCmd *pCmd = pFactor->getBuffedCmd();

	if(pCmd && pCmd->isObjOf(CMD_OBJSGROUPED) 
			&& (nFlags & MK_CONTROL)) //点选状态下 ，产生 move cmd.
	{
		return;
	}
	objMoveCmd *pMoveCmd = NULL;
	if(SEL_MODE_SIZE != m_nSelectMode)
	{
		if(pCmd == NULL || !pCmd->isObjOf(CMD_OBJMOVE))
		{
			pFactor->newCmdEnd();
			pMoveCmd = dynamic_cast<objMoveCmd *>(pFactor->newCmdBegin(CMD_OBJMOVE));
		}
		else
			pMoveCmd = dynamic_cast<objMoveCmd *>(pCmd);
	}
 

    if (m_pSelectObj)
    {
		if(pMoveCmd)
			pMoveCmd->addBoundObj(m_pSelectObj);
		
        nDragHandle = m_pSelectObj->GetDragHandle();
        nDragLine   = m_pSelectObj->GetDragLine();
        if (m_nSelectMode == SEL_MODE_MOVE)
        {
			 
            MoveObjPos(m_pSelectObj, delta, layoutPoint);
        }
        else if (nDragHandle > 0 || nDragLine > 0)
        {
            bResize = TRUE;
            bKeepAspect = TRUE;
            m_pSelectObj->CalcAspectRatio();
            m_pSelectObj->SetKeepAspect(TRUE);

            POSITION pos = m_selectObjList.GetHeadPosition();
            while (pos != NULL)
            {
                pObj = m_selectObjList.GetNext(pos);
                pObj->SetDragHandle(0);
                pObj->SetDragLine(0);
                if (nDragHandle > 0)
                {
                    pObj->SetDragHandle(nDragHandle);
                }
                if (nDragLine > 0)
                {
                    pObj->SetDragLine(nDragLine);
                }
            }
        }
    }

    FRect MergeRect(0,0,0,0);
    POSITION pos = m_selectObjList.GetHeadPosition();
    while (pos != NULL)
    {
        pObj = m_selectObjList.GetNext(pos);
        nDragHandle = pObj->GetDragHandle();
        nDragLine   = pObj->GetDragLine();
        
		if (m_nSelectMode == SEL_MODE_MOVE)
        {
			if(pMoveCmd)
				pMoveCmd->addObj(pObj);
            MoveObjPos(pObj, delta, layoutPoint);
        }
        else
        {
            bChangeAspect = FALSE;
            if (nDragHandle > 0)
            {
                if (!pObj->GetKeepAspect() && bKeepAspect)
                {
                    bChangeAspect = TRUE;
                    pObj->SetKeepAspect(bKeepAspect);
                }

                ResizeObj(pObj, layoutPoint, nDragHandle, TRUE);

                if (bChangeAspect)
                {
                    pObj->SetKeepAspect(FALSE);
                }
            }
            if (nDragLine > 0)
            {
                if (!pObj->GetKeepAspect() && bKeepAspect)
                {
                    bChangeAspect = TRUE;
                    pObj->SetKeepAspect(bKeepAspect);
                }

                ResizeObjLine(pObj, layoutPoint, nDragLine, TRUE);

                if (bChangeAspect)
                {
                    pObj->SetKeepAspect(FALSE);
                }
            }
        }


        FRect myLogicalRect;
        pObj->GetLogicPos(myLogicalRect);

        if (MergeRect.IsRectEmpty())
        {
            MergeRect = myLogicalRect;
        }
        else
        {
            MergeRect.UnionRect(&MergeRect, &myLogicalRect);
        }
    }

    if (m_pSelectObj && bResize)
    {
        if (nDragHandle > 0)
        {
            ResizeObj(m_pSelectObj, layoutPoint, nDragHandle);
        }
        if (nDragLine > 0)
        {
            ResizeObjLine(m_pSelectObj, layoutPoint, nDragLine);
        }
    }

    if (m_pSelectObj)
    {
        m_pSelectObj->SetLogicPos(MergeRect);
    }

    CalcScrollRange();
    //Invalidate();

    m_lastPoint = point;

    if (m_nSelectMode == SEL_MODE_SIZE)
    {
        m_lastDown = point;
        pObj = m_selectObjList.GetHead();
        nDragHandle = pObj->GetDragHandle();
        if (nDragHandle > 0)
        {
            SetCursor(pObj->GetHandleCursor(nDragHandle));
        }
        if (nDragLine > 0)
        {
            SetCursor(pObj->GetLineCursor(nDragLine));
        }
        return;
    }

    m_lastDown = point;
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

}

BOOL CLayoutDesignerCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    FLOAT fScale = m_Scale;
	//m_lastScalPoint = pt;
	//ScreenToClient(&m_lastScalPoint);
	CRect rectClient;
	GetClientRect(rectClient);
	//if (rectClient.PtInRect(m_lastScalPoint))
    {

		SHORT skey = GetAsyncKeyState(VK_CONTROL);
        if (skey & 0x8000)
        {
            if (zDelta > 0)
            {
                if (fScale <= 8.0f)
                {
                    fScale *= 2.0f;
                }
            }
            else
            {
                if (fScale >= 0.04f)
                {
                    fScale /= 2.0f;
                }
            }
			ZoomImage(fScale);
			CalcTransform();
			GetParent()->SendMessage(WM_ON_LAYOUT_ZOOM, 0, (LPARAM)&fScale);
        }
        else
        {
            if ((nFlags & MK_SHIFT) != 0)
            {
                if (zDelta > 0)
                {
                    OnHScroll(SB_LINERIGHT, 0, NULL);
                }
                else
                {
                    OnHScroll(SB_LINELEFT, 0, NULL);
                }
            }
            else
            {
                if (zDelta > 0)
                {
                    OnVScroll(SB_LINEUP, 0, NULL);
                }
                else
                {
                    OnVScroll(SB_LINEDOWN, 0, NULL);
                }
            }
        }

    }

    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CLayoutDesignerCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    int nSelect = -1;
    FPoint hitPoint(point);
    ClientToLayout(hitPoint);
    if (m_selectObjList.GetCount() > 0)
    {
        nSelect = 0;
    }
    else if (m_LayoutRect.PtInRect(hitPoint))
    {
        nSelect = 1;
    }
    else
    {
        nSelect = 1;
    }

    GetParent()->SendMessage(WM_ON_LAYTOUT_SELECT_OBJ, nSelect, (LPARAM)&m_selectObjList);
    m_bLBtnDblClk = TRUE;

    CWnd::OnLButtonDblClk(nFlags, point);
}

void CLayoutDesignerCtrl::OnSize(UINT nType, int cx, int cy)
{

    CWnd::OnSize(nType, cx, cy);

	SetupScrollBars();
    CalcTransform();

}

BOOL CLayoutDesignerCtrl::OnEraseBkgnd(CDC* pDC)
{

    return TRUE;
}

void CLayoutDesignerCtrl::OnSetFocus(CWnd* pOldWnd)
{
    CWnd::OnSetFocus(pOldWnd);

}

void CLayoutDesignerCtrl::OnDestroy()
{
    CWnd::OnDestroy();

}

CRulerCtrl *CLayoutDesignerCtrl::GetSlibingRuler()
{
    if(!IsWindow(m_hWnd))
           return (CRulerCtrl *)NULL;
    CRulerCtrl *pCtrl = (CRulerCtrl*)::GetWindowLongPtr(m_hWnd,GWLP_USERDATA);
    if(pCtrl && ::IsWindow(pCtrl->m_hWnd))
         return pCtrl;
    return NULL;
}

VOID CLayoutDesignerCtrl::EnableBindLay(BOOL bEnable)
{
	m_bEnableBindLay = bEnable;
	CRulerCtrl *pRuler = GetSlibingRuler();
    if(!pRuler)return;
	ASSERT(pRuler);
	pRuler->m_bEnableBindLine = !bEnable;
	pRuler->Invalidate();
	this->Invalidate();
}

VOID CLayoutDesignerCtrl::DrawFloatingWidgets(CDC *pDC)
{
	CRulerCtrl *pRuler = GetSlibingRuler();
    if(!pRuler)
        return;
	if(pRuler->m_bEnableBindLine && pRuler->m_bEnableRuler)
		DrawRulerLine(pDC);
	
	if(m_bEnableBindLay)
	{
		DrawBindLay(pDC);
	}
}

//
//LayoutRect : layout local coordnary.
//
#define w 400
#define h 300
VOID  CLayoutDesignerCtrl::CalcBindLayLines(FRect LayoutRect )
{
	//static FRect bkLayout ;

	//if(bkLayout == LayoutRect)
	//	 return ;
	//bkLayout = LayoutRect;
    if(LayoutRect.Width() <= w || LayoutRect.Height() <= h)
        return ;
	CRulerCtrl *pCtrl = GetSlibingRuler();
    if(!pCtrl)
        return;

	float yStart = LayoutRect.top;
	float xStart = LayoutRect.left;

	pCtrl->m_yLayBindLines.clear();
	int ySecs = LayoutRect.Height() / h ;
	int ym	  = (int)LayoutRect.Height() % h;
	float yd  = (float)ym / (float)ySecs;

	while(yStart <= LayoutRect.bottom)
	{
		pCtrl->m_yLayBindLines.push_back(floor(yStart));
		yStart += (float)h + yd;
	}
	if(ySecs >= pCtrl->m_yLayBindLines.size())
		pCtrl->m_yLayBindLines.push_back(LayoutRect.bottom);

	pCtrl->m_xLayBindLines.clear();
	int xSecs = LayoutRect.Width() / w ;
	int xm	  = (int)LayoutRect.Width() % w;
	float xd  = (float)xm / (float)xSecs;
	while(xStart <= LayoutRect.right)
	{
		pCtrl->m_xLayBindLines.push_back(floor(xStart));
		xStart += w + xd;
	}
	if(xSecs >= pCtrl->m_xLayBindLines.size())
		pCtrl->m_xLayBindLines.push_back(LayoutRect.right);
	//this->Invalidate();
	 
}

VOID  CLayoutDesignerCtrl::DrawBindLay(CDC *pDC)
{
	CRulerCtrl *pCtrl = GetSlibingRuler();
    if(!pCtrl)
        return;

	AXIS_ARRAY &yArray = pCtrl->GetYBindLines();
	AXIS_ARRAY &xArray = pCtrl->GetXBindLines();

	CPen bluePen;
	bluePen.CreatePen(PS_DASHDOT,1,RGB(0,0,255));
	pDC->SaveDC();
	pDC->SelectObject(bluePen);
	pDC->SetROP2(R2_NOTXORPEN);
	CRect LayoutRect = GetLayoutClientRect();
	for(int i = 0;i < xArray.size();i++)
	{
		CPoint ptStart(xArray[i],0);
		this->LayoutToClient( ptStart);
		ptStart.y = LayoutRect.top;

		CPoint ptEnd = ptStart;
		ptEnd.y = LayoutRect.bottom;

		pDC->MoveTo(ptStart);
		pDC->LineTo(ptEnd);

	}

	for(int j = 0;j < yArray.size();j++)
	{
		CPoint ptStart(0,yArray[j]);
		this->LayoutToClient(ptStart);
		ptStart.x = LayoutRect.left;

		CPoint ptEnd = ptStart;
		ptEnd.x = LayoutRect.right;

		pDC->MoveTo(ptStart);
		pDC->LineTo(ptEnd);
	}

	//
	//Write text here!
	//
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(0,0,255));
	for(int i = 0;i < xArray.size()-1;i++)
	{
		for(int j = 0;j < yArray.size()-1;j++)
		{
			CRect rc(xArray[i],yArray[j],xArray[i + 1],yArray[j + 1] );
			this->LayoutToClient(rc);

			CString strPmt;
			strPmt.Format(Translate(_T("Client slot %dx%d")),i,j);
			CSize txtSize = pDC->GetTextExtent(strPmt);

			if(txtSize.cx <= rc.Width() && txtSize.cy <= rc.Height())
				pDC->DrawText(strPmt,&rc,DT_VCENTER);
		}
	}
	pDC->RestoreDC(-1);
}
//zxy designed for ruler. 2011.10.
VOID  CLayoutDesignerCtrl::DrawRulerLine(CDC *pDC)
{
	CRect rectClient;

	GetClientRect(&rectClient);

	CRulerCtrl *pCtrl = GetSlibingRuler();
    if(!pCtrl)return;

	pCtrl->RedrawWindow(NULL);
	AXIS_ARRAY &yArray = pCtrl->GetYBindLines();

	pDC->SaveDC();
	pDC->SetROP2(R2_NOTXORPEN);
	CPen	penBlue;
	CPen	penRed;
	 

	penBlue.CreatePen(PS_SOLID,1,RGB(0,0,255));
	penRed.CreatePen(PS_SOLID,1,RGB(255,0,0));

	if( yArray.size())
	{
		for(int i = 0 ;i < yArray.size();i++)
		{
			CPoint rulerYPt(0,yArray[i]);
			this->TransRulerPosToClient(rulerYPt);
			if(rulerYPt.y <=0 )
				continue;
		 
			if( m_nYCatchedLine == i&& (this->m_bYLineCatched || pCtrl->m_bSnapYLine))
				pDC->SelectObject(penRed);
			else
				pDC->SelectObject(penBlue);

			pDC->MoveTo(0,rulerYPt.y);
			pDC->LineTo(rectClient.Width(),rulerYPt.y);
		}
	}
	//pDC->RestoreDC(-1);
	AXIS_ARRAY &xArray = pCtrl->GetXBindLines();
	if(xArray.size())
	{
		for(int i = 0 ;i < xArray.size();i++)
		{
			CPoint rulerXPt(xArray[i],0);
			this->TransRulerPosToClient(rulerXPt);
			if(rulerXPt.x <=0 )
				continue;
			 
			if( m_nXCatchedLine == i && (this->m_bXLineCatched || pCtrl->m_bSnapXLine))
				pDC->SelectObject(penRed);
			else
				pDC->SelectObject(penBlue);

			pDC->MoveTo(rulerXPt.x,0);
			pDC->LineTo(rulerXPt.x,rectClient.Height());
		}
	}
	pDC->RestoreDC(-1);
}

void CLayoutDesignerCtrl::OnPaint()
{
    CRect rectClient;
    GetClientRect(&rectClient);

    CPaintDC dc(this);
    CDC memDC;
    CBitmap memBitmap;
    CPaintDC* pDrawDC = &dc;

    if (memDC.CreateCompatibleDC(pDrawDC))
    {
        if (memBitmap.CreateCompatibleBitmap (pDrawDC, rectClient.Width (), rectClient.Height ()))
        {
            memDC.SelectObject (&memBitmap);
        }
        else
        {
            memDC.DeleteDC();
        }
    }

    BOOL bMemDC = memDC.GetSafeHdc () != NULL;

    CDC* pDC = bMemDC ? &memDC : pDrawDC;

    CBrush brushBK;
    if (!brushBK.CreateSolidBrush(m_BKColor))
        return;

    brushBK.UnrealizeObject();
    pDC->FillRect(rectClient, &brushBK);

    if (m_bVirtualRect)
    {
        pDC->SetBkMode (TRANSPARENT);

        /// paint background color
        //DrawLayout(pDC);
        m_Widgets.OnDraw(pDC);
    }

    if (bMemDC)
    {
        BOOL bBlt = pDrawDC->BitBlt (rectClient.left, rectClient.top, rectClient.Width (), rectClient.Height (),
            &memDC, 0, 0, SRCCOPY);
    }

}

/// In logic coordinate
void CLayoutDesignerCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	DWORD dwStyle = this->GetStyle() ;
	if((dwStyle & WS_HSCROLL)  != WS_HSCROLL)
		return;

    int nCurPos = GetScrollPos(SB_HORZ);
    SCROLLINFO Info;
    GetScrollInfo(SB_HORZ, &Info, SIF_ALL);

    int LayoutLength = 10;
    ClientToLayout(LayoutLength);
    LayoutLength = max(LayoutLength, 10);

    switch (nSBCode)
    {
    case SB_LEFT:
        nCurPos = Info.nMin;
        break;
    case SB_RIGHT:
        nCurPos = Info.nMax;
        break;
    case SB_ENDSCROLL:
        break;
    case SB_LINELEFT:
        nCurPos = max(Info.nMin, nCurPos - LayoutLength);
        break;
    case SB_LINERIGHT:
        nCurPos = min(Info.nMax, nCurPos + LayoutLength);
        break;
    case SB_PAGELEFT:
        nCurPos = max(Info.nMin, nCurPos - (int)Info.nPage);
        break;
    case SB_PAGERIGHT:
        nCurPos = min(Info.nMax, nCurPos + (int)Info.nPage);
        break;
    case SB_THUMBPOSITION:
        nCurPos = nPos;
        break;
    case SB_THUMBTRACK:
        nCurPos = nPos;
        break;
    }

    SetScrollPos(SB_HORZ, nCurPos);

    if (m_HscrollPos != nCurPos)
    {
        m_HscrollPos = nCurPos;

        CalcTransform();
    }

    Invalidate();

    CWnd::OnHScroll(nSBCode, nPos, pScrollBar);

}

/// In logic coordinate
void CLayoutDesignerCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	DWORD dwStyle = this->GetStyle() ;
	if((dwStyle & WS_VSCROLL)  != WS_VSCROLL)
		return;
    int nCurPos = GetScrollPos(SB_VERT);
    SCROLLINFO Info;
    GetScrollInfo(SB_VERT, &Info, SIF_ALL);

    int LayoutLength = 20;
    ClientToLayout(LayoutLength);
    //LayoutLength = max(LayoutLength, 10);

    switch (nSBCode)
    {
    case SB_TOP:
        nCurPos = Info.nMin;
        break;
    case SB_BOTTOM:
        nCurPos = Info.nMax;
        break;
    case SB_ENDSCROLL:
        break;
    case SB_LINEUP:
        nCurPos = max(Info.nMin, nCurPos - LayoutLength);
        break;
    case SB_LINEDOWN:
        nCurPos = min(Info.nMax, nCurPos + LayoutLength);
        break;
    case SB_PAGEUP:
        nCurPos = max(Info.nMin, nCurPos - (int)Info.nPage);
        break;
    case SB_PAGEDOWN:
        nCurPos = min(Info.nMax, nCurPos + (int)Info.nPage);
        break;
    case SB_THUMBPOSITION:
        nCurPos = nPos;
        break;
    case SB_THUMBTRACK:
        nCurPos = nPos;
        break;
    }

    SetScrollPos(SB_VERT, nCurPos);

    if (m_VscrollPos != nCurPos)
    {
        m_VscrollPos = nCurPos;

        CalcTransform();
    }

    Invalidate();

    CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

#define DEFAULT_LENGHT 256
 LRESULT CLayoutDesignerCtrl::OnAddObject(POBJINFO pObjInfo,CPoint pt,int nWidth,int nHeight)
 {
     CS3DrawObj* pObj;
     BOOL bInclude = FALSE;
     FRect rectObj;
     FPoint layoutPoint(pt);
     ClientToLayout(layoutPoint);

     rectObj.SetRectEmpty();

     SelectDrawObj(NULL);

     pObj = GetDrawObjAtPoint(layoutPoint);
	 //zxy
     if (m_pSelectObj)
     {
       //  delete m_pSelectObj;
         m_pSelectObj = NULL;
     }

     SelectDrawObj(pObj, FALSE);

     if (pObj == NULL)//The new obj isnt Add to exist ojb.
     {
         DWORD ClipWidth = max(1, nWidth);
         DWORD ClipHeight = max(1,nHeight);
         if (ClipWidth < DEFAULT_LENGHT)
         {
             FLOAT fScale = (FLOAT)(DEFAULT_LENGHT / (FLOAT)ClipWidth);
             ClipWidth   = DEFAULT_LENGHT;
             ClipHeight *= fScale;
         }
         else if (ClipHeight < DEFAULT_LENGHT)
         {
             FLOAT fScale = (FLOAT)(DEFAULT_LENGHT / (FLOAT)ClipHeight);
             ClipHeight   = DEFAULT_LENGHT;
             ClipWidth *= fScale;
         }

         rectObj.left    = layoutPoint.x;
         rectObj.right   = layoutPoint.x + ClipWidth;
         rectObj.top     = layoutPoint.y;
         rectObj.bottom  = layoutPoint.y + ClipHeight;
         CRect rect;
         rectObj.ConvertToCRect(rect);
         pObjInfo->position = rect;

         // Empty content layer can not create new CS3DrawObj
         if (pObjInfo->proType != ALIGNDW)
         {
             if (m_pAudioObj == NULL || pObjInfo->proType != AUDIO)
             {
                return GetParent()->SendMessage(WM_ON_LAYOUT_INSERT_OBJ, (WPARAM)pObjInfo, (LPARAM)&m_DragMediaFiles);
             }
             else
             {
                return GetParent()->SendMessage(WM_ON_OBJ_ADDMEDIA, (WPARAM)m_pAudioObj, (LPARAM)&m_DragMediaFiles);
             }
         }
     }
     else //add to exist obj.
     {
         if (pObjInfo->proType == AUDIO)
         {
			 PROPERTY_TYPE pty;
			 pObj->GetPropertyType(pty);
             if (m_pAudioObj != NULL && pty == AUDIO)
             {
                return GetParent()->SendMessage(WM_ON_OBJ_ADDMEDIA, (WPARAM)m_pAudioObj, (LPARAM)&m_DragMediaFiles);
             }
			 else
			 {
				 //added by RaymondLiu
				  MessageBox(Translate(_T("Only audio layer can input audio files!")), Translate(_T("Error:Layout designer")), MB_ICONERROR);
				  return S_FALSE;
			 }
         }
         else
         {
            return GetParent()->SendMessage(WM_ON_OBJ_ADDMEDIA, (WPARAM)pObj, (LPARAM)&m_DragMediaFiles);
         }
     }
    return S_OK;
 }
 
LRESULT CLayoutDesignerCtrl::OnDrop(WPARAM wParam, LPARAM lParam)  
{
    // Did not have a layout, do not handle the drop message

    HGLOBAL			hGlobal = NULL;
    HDROP			hDrop = NULL;
	CS3OleDropInfo* pInfo; 

	if (m_LayoutSize.cx == 0 && m_LayoutSize.cy == 0)
	{
		return (LRESULT)0;
	}

    pInfo = (CS3OleDropInfo*)wParam;
	if(!pInfo->m_pDataObject->IsDataAvailable(CF_UNICODETEXT))
	{
		return  0;
	}  
   
	hGlobal = pInfo->m_pDataObject->GetGlobalData(CF_UNICODETEXT);
	m_DragMediaFiles.clear();

	LayoutCmdFactory *pFactor = m_CmdEngine.getFactory();
	_baseCmd *pBaseCmd = pFactor->newCmdEnd(); //clear last option.
	if(pBaseCmd && pBaseCmd->isObjOf(CMD_OBJSGROUPED))
	{
		this->m_pSelectObj = NULL;
		this->m_selectRect.SetRectEmpty();
		//delete group cmd needed
	}
	objBuildCmd *pCmd = (objBuildCmd *)pFactor->newCmd(CMD_OBJBUILD);

	//save context to cmd obj.
	pCmd->nHeight = DEFAULT_LENGHT;
	pCmd->nWidth  = DEFAULT_LENGHT;
	pCmd->strMediaFile = (LPTSTR)GlobalLock(hGlobal);
	pCmd->dragPoint = pInfo->m_point;//drag point ,used to put down object.
	GlobalUnlock(hGlobal);

	if(!m_CmdEngine.execCmd(pCmd))
		pFactor->delCmd(pCmd);

	this->SetFocus();
	this->RecalcRulerLinesAttachment();
	this->Invalidate();
    return (LRESULT)1;
}

LRESULT CLayoutDesignerCtrl::OnDragEnter(WPARAM wParam, LPARAM lParam)
{
    CS3OleDropInfo* pInfo = (CS3OleDropInfo*)wParam;
    if (pInfo->m_pDataObject->IsDataAvailable(CF_UNICODETEXT))
    {
        return DROPEFFECT_COPY;
    }
    else
    {
        return DROPEFFECT_NONE;
    }
}

LRESULT CLayoutDesignerCtrl::OnDragOver(WPARAM wParam, LPARAM lParam)
{
    CS3OleDropInfo* pInfo = (CS3OleDropInfo*)wParam;
    if (pInfo->m_pDataObject->IsDataAvailable(CF_UNICODETEXT))
    {
        return DROPEFFECT_COPY;
    }
    else
    {
        return DROPEFFECT_NONE;
    }
}

LRESULT CLayoutDesignerCtrl::OnDragLeave(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0l;
    return lr;
}

BOOL CLayoutDesignerCtrl::UpdateOptions()
{
    SetupVirtualGrid();

    CString strAlign;
    GetConfig->Get(DESIGNER_SNAP_GRID, strAlign);
    strAlign.MakeUpper();
    m_bAlignGrid = (strAlign == _T("TRUE") ? TRUE : FALSE);
    m_bAlignGrid = FALSE;

    CString strShowGrid;
    GetConfig->Get(DESIGNER_GRID_SHOW, strShowGrid);
    strShowGrid.MakeUpper();
    m_bShowGrid = (strShowGrid == _T("TRUE") ? TRUE : FALSE);


    if (GetSafeHwnd())
    {
        RedrawWindow();
    }

    return TRUE;
}

void CLayoutDesignerCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (m_bNotifyParent && GetSafeHwnd () != NULL)
    {
        GetParent ()->SendMessage (DESIGNER_KEYDOWN, nChar, MAKELPARAM(nRepCnt, nFlags));
    }
}

UINT CLayoutDesignerCtrl::OnGetDlgCode() 
{
    return DLGC_WANTARROWS | DLGC_WANTTAB;
}
//zxy designed for ruler. 2011.10.
VOID  CLayoutDesignerCtrl::TransRulerPosToLogic(CPoint &pt)
{
	ClientToLayout(pt);
}
////zxy designed for ruler. 2011.10.
VOID  CLayoutDesignerCtrl::TransRulerPosToClient(CPoint &pt)
{
	LayoutToClient(pt);
}

//zxy designed for ruler. 2011.10
VOID CLayoutDesignerCtrl::RulerLinesHitTest(CPoint pt)
{
	CRulerCtrl *pCtrl =GetSlibingRuler();
    if(!pCtrl)
        return;
	if(!pCtrl->m_bEnableBindLine || !pCtrl->m_bEnableRuler)
		return;
	AXIS_ARRAY &yArray = pCtrl->GetYBindLines();

	if(pCtrl->m_bSnapXLine || pCtrl->m_bSnapYLine)
		return;
	int nXCatchedLine = m_nXCatchedLine;
	int nYCatchedLine = m_nYCatchedLine;

	BOOL bCachedY = m_bYLineCatched;
	BOOL bCachedX = m_bXLineCatched;

	m_bYLineCatched = FALSE;
	m_bXLineCatched = FALSE;

	m_nXCatchedLine = -1;
	m_nYCatchedLine = -1;

	
	ScreenToClient( &pt);

	CPoint rulerPos(0,0);
	CRect ylineRect(0,0,m_ViewportRect.Width()/*this->m_VirtualLayoutRect.Width()*/, 0);

	if(yArray.size())
	{
		for( int i = 0;i < yArray.size();i++)
		{
			rulerPos.y = yArray[i];
			TransRulerPosToClient( rulerPos );

			ylineRect.top = rulerPos.y - 2;
			ylineRect.bottom = rulerPos.y + 2;

			if(ylineRect.PtInRect(pt) )
			{
				m_bYLineCatched = TRUE;
				m_nYCatchedLine = i;
				pCtrl->m_bSnapYLine = TRUE;
				//pCtrl->m_bSnapXLine = FALSE;
				pCtrl->m_nSnapYLine = i;
				if( i != nYCatchedLine || FALSE == bCachedY)
					this->InvalidateRect(ylineRect);
				break;
			}
		}

		if(nYCatchedLine >= 0 && m_nYCatchedLine < 0 && bCachedY)
		{
			this->Invalidate();
		}
	}


	CRect xlineRect(0,0,0, this->m_ViewportRect.Height());
	AXIS_ARRAY &xArray = pCtrl->GetXBindLines();

	if(xArray.size())
	{
		for( int i = 0;i < xArray.size();i++)
		{
			rulerPos.x = xArray[i];
			TransRulerPosToClient( rulerPos );

			xlineRect.left = rulerPos.x - 2;
			xlineRect.right = rulerPos.x + 2;

			if(xlineRect.PtInRect(pt))
			{
				m_bXLineCatched = TRUE;
				pCtrl->m_bSnapXLine = TRUE;
				pCtrl->m_nSnapXLine = i;
				if( i != nXCatchedLine ||FALSE == bCachedX)
				this->InvalidateRect(xlineRect);
				m_nXCatchedLine = i;
				break;
			}
		}
		
		if( nXCatchedLine >= 0  &&  m_nXCatchedLine < 0 && bCachedX)
		{
			this->Invalidate();
		}
				
	}

}

LRESULT CLayoutDesignerCtrl::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	RulerLinesHitTest(point);
	
	return CWnd::OnNcHitTest(point);
}

//zxy designed for ruler. 2011.10.
BOOL CLayoutDesignerCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	if(this->m_bXLineCatched || this->m_bYLineCatched)
	{
		if(CWnd::GetFocus() != this)
			this->SetFocus();
	}
	if(this->m_bXLineCatched && this->m_bYLineCatched)
	{
		HCURSOR hCursor = ::LoadCursor(NULL,IDC_CROSS);
		::SetCursor(hCursor);
		return TRUE;
	}

	if(this->m_bXLineCatched)
	{
		HCURSOR hCursor = ::LoadCursor(NULL,IDC_SIZEWE);
		::SetCursor(hCursor);
		return TRUE;
	}

	if(this->m_bYLineCatched)
	{
		HCURSOR hCursor = ::LoadCursor(NULL,IDC_SIZENS);
		::SetCursor(hCursor);
		return TRUE;
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

//calculate line attachment,and change their pos,or size etc...
const int nAttachSize =30;
const int nLeftAttached = 0x1;
const int nRightAtteched = 0x2;
const int nTopAttached = 0x4;
const int nBottomAttached = 0x8;
VOID CLayoutDesignerCtrl::RecalcRulerLinesAttachment( )
{  
	CRulerCtrl *pCtrl =GetSlibingRuler();
	if(!pCtrl || !::IsWindow(pCtrl->m_hWnd))
		return;

	AXIS_ARRAY &yArray = pCtrl->GetYBindLines();
	AXIS_ARRAY &xArray = pCtrl->GetXBindLines();

	int nYSz = yArray.size();
	int nXSz = xArray.size();

	//Get Current Cmd ojb .
	LayoutCmdFactory *pFactor = dynamic_cast<LayoutCmdFactory *>(this->m_CmdEngine.getFactory());
	bindLineMoveCmd *pCmd = dynamic_cast<bindLineMoveCmd *>(pFactor->getBuffedCmd());
	if( pCmd && !pCmd->isObjOf(CMD_BINDLINEMOVE))
		pCmd = NULL;

	POSITION pos = m_drawObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CS3DrawObj *pObj = m_drawObjList.GetNext(pos);
		PROPERTY_TYPE proType;
		pObj->GetPropertyType(proType);
		if (proType != BACKGROUND)
		{
			FRect rectObj;
			FRect rect;
			pObj->GetLogicPos(rectObj);
			LayoutToClient(rectObj);
			rect = rectObj;

			DWORD dwAttchStatus = 0;
			int storeX1,storeX2;
			int storeY1,storeY2;

			CPoint pt(0,0);
			int nLeftCacherLine = -1;
			for( int i = 0 ;i < xArray.size();i++)
			{
				pt.x = xArray[i];
				this->TransRulerPosToClient(pt);
				if(abs(pt.x - rect.left)<= nAttachSize)
				{
					storeX1 = pt.x;
					nLeftCacherLine = i;
					dwAttchStatus |= nLeftAttached;

					if(pCmd && pCmd->nXLine == i)
						pCmd->addAttach(pObj);
					break;
				}
			}
			

			for( int i = 0 ;i < xArray.size();i++)
			{
				pt.x = xArray[i];
				this->TransRulerPosToClient(pt);
				if(abs(pt.x - rect.right)<= nAttachSize)
				{
					if(pCmd && pCmd->nXLine == i)
						pCmd->addAttach(pObj);

					if(nLeftCacherLine == i)
						continue;

					storeX2 = pt.x;
					dwAttchStatus |= nRightAtteched;
					break;
				}
			}

			int nTopCacherLine = -1;
			for( int i = 0 ;i < yArray.size();i++)
			{
				pt.y = yArray[i];
				this->TransRulerPosToClient(pt);
				if(abs(pt.y - rect.top)<= nAttachSize)
				{
					if(pCmd && pCmd->nYLine == i)
						pCmd->addAttach(pObj);

					storeY1 = pt.y;
					nTopCacherLine = i;
					dwAttchStatus |= nTopAttached;
					break;
				}
			}
			

			for( int i = 0 ;i < yArray.size();i++)
			{
				pt.y = yArray[i];
				this->TransRulerPosToClient(pt);
				if(abs(pt.y - rect.bottom)<= nAttachSize)
				{

					if(pCmd && pCmd->nYLine == i)
						pCmd->addAttach(pObj);

					if(nTopCacherLine == i)
						continue;

					storeY2 = pt.y;
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
				this->ClientToLayout(tpRect);
				pObj->SetLogicPos( tpRect);
			}
		}

	}
	RecalcMultiSelectMergeRect();
}


void CLayoutDesignerCtrl::UpDataDrawObject()
{
	POSITION pos = m_drawObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CS3DrawObj *pObj = m_drawObjList.GetNext(pos);
		PROPERTY_TYPE proType;
		pObj->GetPropertyType(proType);
		if (proType != BACKGROUND)
		{
			FRect rectObj;
			FRect rect;
			pObj->GetLogicPos(rectObj);
			LayoutToClient(rectObj);
			
			if(rectObj.left > rectObj.right)
			{
				int left = rectObj.left;
				rectObj.left = rectObj.right;
				rectObj.right = left;
			}

			if(rectObj.top > rectObj.bottom)
			{
				int top = rectObj.top;
				rectObj.top = rectObj.bottom;
				rectObj.bottom = top;
			}

			ClientToLayout(rectObj);
			pObj->SetLogicPos(rectObj);
		}

	}
}

//this is pre-order function ,interval will reference first two objects.
BOOL  CLayoutDesignerCtrl::SortIntervalX(CS3DrawObjList *targetList)
{
    //first sort by x,and then calculate x margin range ,and make same Interval
    float fInterval = 0;
    float fHoldRange = 0;
    FRect stPos,endPos;

    int nCount = targetList->GetCount(); //get item number.
    std::vector<CS3DrawObj *> sortList;
    //Sort all objects from left to right,and calculate "can hold range"
    POSITION pos = targetList->GetHeadPosition();
    while(pos)
    {
        CS3DrawObj *p = targetList->GetAt(pos);
        p->GetLogicPos(stPos);
       // LayoutToClient(stPos);

        fHoldRange += stPos.Width();
        BOOL bInsert = FALSE;
        //find a place to insert.
        if(sortList.size())
        {
            std::vector<CS3DrawObj *>::iterator it = sortList.begin();
            //BOOST_FOREACH(auto it,sortList)
            for(;it != sortList.end();it++)
            {
                CRect pos2;
                CS3DrawObj *p1= *it;
                p1->GetLogicPos(endPos);
                if(endPos.left >= stPos.left)
                {
                    sortList.insert(it,p);
                    bInsert = TRUE;
                    break;
                }
            }
        }
        //not found ,push back!
        if(!bInsert)
        {
            sortList.push_back(p);
        }
        targetList->GetNext(pos);
    }

    //space is enough to hold all objects ?
    sortList[0]->GetLogicPos(stPos);
    sortList[sortList.size()-1]->GetLogicPos(endPos);
    //calculate interval value
    fInterval =  (endPos.right - stPos.left - fHoldRange) / (float)(nCount-1);
    //reposition all object here !
    CS3DrawObj *pPre = NULL;
    std::vector<CS3DrawObj *>::iterator it = sortList.begin();

    BOOL bPosChange = false;
    for(;it != sortList.end();it++)
    {
        CS3DrawObj *pObj = *it;
        if(pPre == NULL)
        {
            pPre = pObj;
            continue;
        }
        pPre->GetLogicPos(stPos);
        pObj->GetLogicPos(endPos);
     
        float newLeft = stPos.right + fInterval;
        if(newLeft != endPos.left)
            bPosChange = true;
        endPos.OffsetRect(newLeft -  endPos.left,0);
        pObj->SetLogicPos(endPos);
        pPre = pObj;
    }

    return bPosChange;

}

BOOL  CLayoutDesignerCtrl::SortIntervalY(CS3DrawObjList *targetList)
{
    //first sort by y
    float fInterval = 0;
    float fHoldRange = 0;
    FRect stPos,endPos;

    int nCount = targetList->GetCount(); //get item number.
    std::vector<CS3DrawObj *> sortList;
    //Sort all objects from left to right,and calculate "can hold range"
    POSITION pos = targetList->GetHeadPosition();
    while(pos)
    {
        CS3DrawObj *p = targetList->GetAt(pos);
        p->GetLogicPos(stPos);
      
        fHoldRange += stPos.Height();
        BOOL bInsert = FALSE;
        //find a place to insert.
        if(sortList.size())
        {
            std::vector<CS3DrawObj *>::iterator it = sortList.begin();
            for(;it != sortList.end();it++)
            {
                CRect pos2;
                CS3DrawObj *p1= *it;
                p1->GetLogicPos(endPos);

                if(endPos.top >= stPos.top)
                {
                    sortList.insert(it,p);
                    bInsert = TRUE;
                    break;
                }
            }
        }
        //not found ,push back!
        if(!bInsert)
        {
            sortList.push_back(p);
        }
        targetList->GetNext(pos);
    }

    //space is enough to hold all objects ?
    sortList[0]->GetLogicPos(stPos);
    sortList[sortList.size()-1]->GetLogicPos(endPos);
   
    //calculate interval value
    fInterval =  (endPos.bottom - stPos.top - fHoldRange) / (float)(nCount-1);
    //reposition all object here !
    CS3DrawObj *pPre = NULL;
    //BOOST_FOREACH(auto it,sortList)
    std::vector<CS3DrawObj *>::iterator it = sortList.begin();
    BOOL bPosChange = false;
    for(;it != sortList.end();it++)
    {
        CS3DrawObj *pObj = *it;
        if(pPre == NULL)
        {
            pPre = pObj;
            continue;
        }
        pPre->GetLogicPos(stPos);
        pObj->GetLogicPos(endPos);

        float newTop = stPos.bottom + fInterval;
        if(newTop != endPos.top)
            bPosChange = true;
        
        endPos.OffsetRect(0,newTop - endPos.top);
        pObj->SetLogicPos(endPos);
        pPre = pObj;
    }
    return bPosChange;
}

void CLayoutDesignerCtrl::SameInterval(BOOL yOrder)
{
    if(m_selectObjList.GetCount()<= 2)
        return ;

    LayoutCmdFactory *pFactor = m_CmdEngine.getFactory();
    objsGroupMoveCmd *pCmd = dynamic_cast<objsGroupMoveCmd *>(pFactor->newCmd(CMD_OBJSGROUPMOVE));
    if(!m_CmdEngine.execCmd(pCmd))
    {
        pFactor->delCmd(pCmd);
        return;
    }

    bool bDone = false;
    if(yOrder)
    {
        if(bDone = SortIntervalY(&m_selectObjList))
        {
            this->Invalidate(); 
        }
    }
    else
    {
        if(bDone = SortIntervalX(&m_selectObjList))  
        {
           this->Invalidate();
        }
    }
    if(!bDone)//remove unused cmd - i reused "group move cmd " ,so ,need doing thing here!.
    {
        CMD_STACK &stack = m_CmdEngine.GetUndoCmds();
        stack.pop();
        pFactor->delCmd(pCmd);
    }
}

//
void CLayoutDesignerCtrl::FillLayout(void)
{
    if(m_selectObjList.GetCount()<= 0)
        return ;
   // if(m_pl)
    LayoutCmdFactory *pFactor = m_CmdEngine.getFactory();
    objSizeCmd *pCmd = dynamic_cast<objSizeCmd *>(pFactor->newCmd(CMD_OBJSIZE));
    if(!m_CmdEngine.execCmd(pCmd))
        pFactor->delCmd(pCmd);

    BOOL bDone = FALSE;
    POSITION pos = m_selectObjList.GetHeadPosition();
    while(pos)
    {
        CS3DrawObj *p = m_selectObjList.GetAt(pos);
        FRect fRct;
        p->GetLogicPos(fRct);
        if(fRct != m_ViewportRect)
        {
            bDone = TRUE;
            p->SetLogicPos(m_LayoutRect);
        }
        m_selectObjList.GetNext(pos);
    }
    if(!bDone)//remove unused cmd - i reused "group move cmd " ,so ,need doing thing here!.
    {
        CMD_STACK &stack = m_CmdEngine.GetUndoCmds();
        stack.pop();
        pFactor->delCmd(pCmd);
    }
    this->Invalidate();
}