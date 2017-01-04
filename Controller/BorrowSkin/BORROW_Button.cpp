// SkinButton.cpp : 实现文件
//

#include "stdafx.h"
#include "BORROW_Button.h"
#include "ControllerVisualManager.h"


static const COLORREF clrDefault = (COLORREF) -1;

// CSkinButton

IMPLEMENT_DYNAMIC(CBorrow_Button, CMFCButton)

CBorrow_Button::CBorrow_Button()
{
    m_bDefaultButton	= FALSE;
    m_bVisualManagerStyle = FALSE;
}

CBorrow_Button::~CBorrow_Button()
{
}


BEGIN_MESSAGE_MAP(CBorrow_Button, CMFCButton)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void CBorrow_Button::DoDrawItem(CDC* pDCPaint, CRect rectClient, UINT itemState)
{
    CMemDC memDC (*pDCPaint, this);
    CDC* pDC = &memDC.GetDC ();

    m_clrText = clrDefault;

    CString text;
    GetWindowText(text);

    BOOL bDefaultDraw = TRUE;
    BOOL bDefaultCheckRadio = (m_bCheckButton || m_bRadioButton) && (GetStyle () & BS_PUSHLIKE) == 0;

    if (!m_bTransparent)
    {
        if (bDefaultCheckRadio)
        {
            afxGlobalData.DrawParentBackground (this, pDC);

            //----------------
            // Draw check box:
            //----------------
            CRect rectCheck = rectClient;
            const CSize sizeCheck = visualManager->GetCheckRadioDefaultSize ();

            {
                rectCheck.left = rectCheck.right - sizeCheck.cx;
            }

            BOOL bMultiLine = (GetStyle () & BS_MULTILINE) != 0;

            if (bMultiLine)
            {
                if ((GetStyle () & BS_VCENTER) == BS_VCENTER)
                {
                    rectCheck.top = rectCheck.CenterPoint ().y - sizeCheck.cy / 2;
                }
                else if ((GetStyle () & BS_BOTTOM) == BS_BOTTOM)
                {
                    rectCheck.top = rectCheck.bottom - sizeCheck.cy;
                }
                else if ((GetStyle () & BS_TOP) == BS_TOP)
                {
                }
                else
                {
                    rectCheck.top = rectCheck.CenterPoint ().y - sizeCheck.cy / 2;
                }
            }
            else
            {
                rectCheck.top = rectCheck.CenterPoint ().y - sizeCheck.cy / 2;
            }

            rectCheck.bottom = rectCheck.top + sizeCheck.cy;

            const BOOL bIsHighlighted = IsHighlighted () && IsWindowEnabled ();

            if (m_bCheckButton)
            {
                int nCheck = IsChecked () ? 1 : 0;

                visualManager->OnDrawCheckBoxEx (pDC,
                    rectCheck, nCheck,
                    bIsHighlighted, IsPressed () && bIsHighlighted, IsWindowEnabled ());
            }
            else
            {
                visualManager->OnDrawRadioButton (pDC,
                    rectCheck, IsChecked () || (IsPressed () && bIsHighlighted),
                    bIsHighlighted, IsPressed () && bIsHighlighted, IsWindowEnabled ());
            }

            {
                rectClient.right = rectCheck.left;
                rectClient.left++;
            }

            bDefaultDraw = FALSE;
        }
        else
        {
            if (visualManager->OnDrawPushButton (pDC, rectClient, this, m_clrText))
            {
                rectClient.DeflateRect (2, 2);
                bDefaultDraw = FALSE;
            }
        }
    }

    if (bDefaultDraw)
    {
        OnFillBackground (pDC, rectClient);
        OnDrawBorder (pDC, rectClient, itemState);
    }

    //---------------------
    // Draw button content:
    //---------------------
    OnDraw (pDC, rectClient, itemState);

    if ((itemState & ODS_FOCUS) && m_bDrawFocus)
    {
        OnDrawFocusRect (pDC, rectClient);
    }

}

BOOL CBorrow_Button::IsDefaultButton()
{
    return m_bDefaultButton;
}

void CBorrow_Button::MyInitStyle(DWORD dwStyle)
{
    switch (dwStyle & 0x0F) 
    {
    case BS_CHECKBOX:
        m_bCheckButton = TRUE;
        break;
    case BS_AUTOCHECKBOX:
        m_bCheckButton = m_bAutoCheck = TRUE;
        break;
    case BS_AUTO3STATE:
        m_bCheckButton = m_bAutoCheck = TRUE;
        break;
    case BS_RADIOBUTTON:
        m_bRadioButton = TRUE;
        break;
    case BS_AUTORADIOBUTTON:
        m_bRadioButton = m_bAutoCheck = TRUE;
        break;
    case BS_DEFPUSHBUTTON:
        m_bDefaultButton = TRUE;
        break;
    }

    if (m_bCheckButton || m_bRadioButton)
    {
        switch (dwStyle & BS_CENTER) 
        {
        case BS_LEFT:
        default:
            m_nAlignStyle = CMFCButton::ALIGN_LEFT;
            break;

        case BS_RIGHT:
            m_nAlignStyle = CMFCButton::ALIGN_RIGHT;
            break;

        case BS_CENTER:
            m_nAlignStyle = CMFCButton::ALIGN_CENTER;
            break;
        }
    }

    CMFCButton::InitStyle(dwStyle);
}


void CBorrow_Button::OnDraw(CDC* pDC, const CRect& rect, UINT uiState)
{
    CRect rectText = rect;
    CRect rectImage = rect;

    CString strText;
    GetWindowText (strText);

    if (m_sizeImage.cx != 0)
    {
        if (!strText.IsEmpty ())
        {
/*
            if (m_bTopImage)
            {
                rectImage.bottom = rectImage.top + m_sizeImage.cy + GetVertMargin ();
                rectText.top = rectImage.bottom;
                rectText.bottom -= GetVertMargin ();
            }
            else if (m_bRighImage)
            {
                rectText.right -= m_sizeImage.cx + GetImageHorzMargin () / 2;
                rectImage.left = rectText.right;
                rectImage.right -= GetImageHorzMargin () / 2;
            }
            else
*/
            {
                rectText.left +=  m_sizeImage.cx + GetImageHorzMargin () / 2;
                rectImage.left += GetImageHorzMargin () / 2;
                rectImage.right = rectText.left;
            }
        }

        // Center image:
        rectImage.DeflateRect ((rectImage.Width () - m_sizeImage.cx) / 2,
            max (0, (rectImage.Height () - m_sizeImage.cy) / 2));
    }
    else
    {
        rectImage.SetRectEmpty ();
    }

    //-----------
    // Draw text:
    //-----------
    CFont* pOldFont = SelectFont (pDC);
    ASSERT(pOldFont != NULL);

    pDC->SetBkMode (TRANSPARENT);

    UINT uiDTFlags = DT_END_ELLIPSIS;
    BOOL bIsSingleLine = FALSE;

    BOOL bDefaultCheckRadio = (m_bCheckButton || m_bRadioButton) && (GetStyle () & BS_PUSHLIKE) == 0;
    BOOL bMultiLine = (GetStyle () & BS_MULTILINE) != 0;

    COLORREF clrTextDefault = (bDefaultCheckRadio && m_bVisualManagerStyle) ? afxGlobalData.clrBarText : afxGlobalData.clrBtnText;
    COLORREF clrText = m_clrRegular == clrDefault ? clrTextDefault : m_clrRegular;

    if (m_bHighlighted && m_clrHover != clrDefault)
    {
        clrText = m_clrHover;
    }

    if (bDefaultCheckRadio && !bMultiLine)
    {
        uiDTFlags |= DT_VCENTER | DT_SINGLELINE;
        bIsSingleLine = TRUE;
    }
    else
    {
        if (bMultiLine)
        {
            uiDTFlags |= DT_WORDBREAK;

            if (!bDefaultCheckRadio)
            {
                rectText.DeflateRect (GetVertMargin () / 2, GetVertMargin () / 2);
            }

            CRect rectCalc = rectText;
            pDC->DrawText (strText, rectCalc, DT_CALCRECT | DT_WORDBREAK);

            if ((GetStyle () & BS_VCENTER) == BS_VCENTER)
            {
                rectText.top = rectText.CenterPoint ().y - rectCalc.Height () / 2;
            }
            else if ((GetStyle () & BS_BOTTOM) == BS_BOTTOM)
            {
                rectText.top = rectText.bottom - rectCalc.Height ();
            }
            else if ((GetStyle () & BS_TOP) == BS_TOP)
            {
            }
            else
            {
                rectText.top = rectText.CenterPoint ().y - rectCalc.Height () / 2;
            }

            rectText.bottom = rectText.top + rectCalc.Height ();
        }
        else if (strText.Find (_T('\n')) < 0)
        {
            uiDTFlags |= DT_VCENTER | DT_SINGLELINE;
            bIsSingleLine = TRUE;
        }
        else
        {
            rectText.DeflateRect (0, GetVertMargin () / 2);
        }

        switch (m_nAlignStyle)
        {
        case ALIGN_LEFT:
            uiDTFlags |= DT_LEFT;

            if (!bDefaultCheckRadio)
            {
                rectText.left += GetImageHorzMargin () / 2;
            }

            break;

        case ALIGN_RIGHT:
            uiDTFlags |= DT_RIGHT;

            if (!bDefaultCheckRadio)
            {
                rectText.right -= GetImageHorzMargin () / 2;
            }

            break;

        case ALIGN_CENTER:
            uiDTFlags |= DT_CENTER;
            break;
        }
    }

    if (GetExStyle() & WS_EX_LAYOUTRTL)
    {
        uiDTFlags |= DT_RTLREADING;
    }

    if ((uiState & ODS_DISABLED) && m_bGrayDisabled)
    {
//         if (!m_bOnGlass)
        {
            pDC->SetTextColor (afxGlobalData.clrBtnHilite);

            CRect rectShft = rectText;
            rectShft.OffsetRect (1, 1);
            OnDrawText (pDC, rectShft, strText, uiDTFlags, uiState);
        }

        pDC->SetTextColor (afxGlobalData.clrGrayedText);
    }
    else
    {
        if (m_clrText == clrDefault)
        {
            pDC->SetTextColor (clrText);
        }
        else
        {
            pDC->SetTextColor (m_clrText);
        }
    }

    if (m_bDelayFullTextTooltipSet)
    {
        BOOL bIsFullText = pDC->GetTextExtent (strText).cx <= rectText.Width ();
        SetTooltip (bIsFullText || !bIsSingleLine ? NULL : (LPCTSTR) strText);
        m_bDelayFullTextTooltipSet = FALSE;
    }

    OnDrawText (pDC, rectText, strText, uiDTFlags, uiState);

    //------------
    // Draw image:
    //------------
    if (!rectImage.IsRectEmpty ())
    {
        if (m_nStdImageId != (CMenuImages::IMAGES_IDS) -1)
        {
            CMenuImages::IMAGES_IDS id = m_nStdImageId;

            if ((uiState & ODS_DISABLED) && m_bGrayDisabled &&
                m_nStdImageDisabledId != 0)
            {
                id = m_nStdImageDisabledId;
            }

            CMenuImages::Draw (pDC, id, rectImage.TopLeft (), m_StdImageState);
        }
        else
        {
            BOOL bIsDisabled = (uiState & ODS_DISABLED) && m_bGrayDisabled;

            CMFCToolBarImages& imageChecked = 
                (bIsDisabled && m_ImageCheckedDisabled.GetCount () != 0) ?
                    m_ImageCheckedDisabled :
                        (m_bHighlighted && m_ImageCheckedHot.GetCount () != 0) ?
                            m_ImageCheckedHot : m_ImageChecked;

            CMFCToolBarImages& image = 
                (bIsDisabled && m_ImageDisabled.GetCount () != 0) ?
                    m_ImageDisabled :
                        (m_bHighlighted && m_ImageHot.GetCount () != 0) ?
                            m_ImageHot : m_Image;

            if (m_bChecked && imageChecked.GetCount () != 0)
            {
                CAfxDrawState ds;

                imageChecked.PrepareDrawImage (ds);
                imageChecked.Draw (pDC, rectImage.left, rectImage.top, 0, FALSE, 
                    bIsDisabled && m_ImageCheckedDisabled.GetCount () == 0);
                imageChecked.EndDrawImage (ds);
            }
            else if (image.GetCount () != 0)
            {
                CAfxDrawState ds;

                image.PrepareDrawImage (ds);
                image.Draw (pDC, rectImage.left, rectImage.top, 0, FALSE, 
                    bIsDisabled && m_ImageDisabled.GetCount () == 0);
                image.EndDrawImage (ds);
            }
        }
    }

    pDC->SelectObject (pOldFont);
}

void CBorrow_Button::OnPaint()
{
    const BOOL bIsRadio = m_bRadioButton || m_bCheckButton;

/*
    if (!bIsRadio)
    {
        Default ();
        return;
    }
*/

    CPaintDC dc(this); // device context for painting

    CRect rectClient;
    GetClientRect (&rectClient);

    DoDrawItem(&dc, rectClient, (CWnd::GetFocus () == this) ? ODS_FOCUS : 0);

}

void CBorrow_Button::PreSubclassWindow()
{
    MyInitStyle(GetStyle());

    CMFCButton::InitStyle(GetStyle());

    ModifyStyle(BS_DEFPUSHBUTTON, BS_OWNERDRAW);
    CButton::PreSubclassWindow();
}




// CSkinButton 消息处理程序


