// S3VisualManager.cpp : implementation file
//

#include "stdafx.h"
#include <afxtagmanager.h>
#include <afxglobalutils.h>
#include "ControllerVisualManager.h"

#include "BORROW_ScrollBar.h"
#include "BORROW_Group.h"
#include "BORROW_CalendarBar.h"
#include "BORROW_Button.h"
#include "BORROW_GridCtrl.h"

#define S3_SIGNAGE_MANAGEMEN_STYLE  _T("S3_SIGNAGE_MANAGEMEN_STYLE")
#define S3_STYLE_XML                _T("S3_STYLE_XML")


// S3VisualManager

IMPLEMENT_DYNCREATE(CControllerVisualManager, CMFCVisualManagerOffice2007)

static HICON GetWndIcon(CWnd* pWnd)
{
    ASSERT_VALID(pWnd);

    if (pWnd->GetSafeHwnd() == NULL)
    {
        return NULL;
    }

    HICON hIcon = pWnd->GetIcon(FALSE);

    if (hIcon == NULL)
    {
        hIcon = pWnd->GetIcon(TRUE);

        if (hIcon != NULL)
        {
            CImageList il;
            il.Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 1);
            il.Add(hIcon);

            if (il.GetImageCount() == 1)
            {
                hIcon = il.ExtractIcon(0);
            }
        }
    }

    if (hIcon == NULL)
    {
        hIcon = (HICON)(LONG_PTR)::GetClassLongPtr(pWnd->GetSafeHwnd(), GCLP_HICONSM);
    }

    if (hIcon == NULL)
    {
        hIcon = (HICON)(LONG_PTR)::GetClassLongPtr(pWnd->GetSafeHwnd(), GCLP_HICON);
    }

    return hIcon;
}

CControllerVisualManager::CControllerVisualManager()
: CMFCVisualManagerOffice2007()
{                          
    // we use style ObsidianBlack
    // This strange codes is because, the default style is ObsidianBlack, but it is different with the style set by ObsidianBlack, 
    // And if directly set ObsidianBlack, it will not change anything, so change it to another, then change back.
    SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
    SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);

    m_hThemeToolTip = NULL;


}

void CControllerVisualManager::CleanUp()
{
    CMFCVisualManagerOffice2007::CleanUp();

    m_ctrlSliderThumb[0].CleanUp();
    m_ctrlSliderThumb[1].CleanUp();
    m_ctrlSliderThumb[2].CleanUp();
    m_ctrlSliderThumb[3].CleanUp();
    m_ctrlSliderThumb[4].CleanUp();
    m_ctrlSliderThumb[5].CleanUp();

    int i, j;
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 2; j++)
        {
            m_ctrlScrollBar_Back[i][j].CleanUp ();
            m_ctrlScrollBar_Item[i][j].CleanUp ();
            m_ctrlScrollBar_ThumbBack[i][j].CleanUp ();
            m_ctrlScrollBar_ThumbIcon[i][j].CleanUp ();
        }
    }

    m_ctrlRibbonBtnGroup.CleanUp();

    m_ctrlRibbonBtnRadio.CleanUp();
    m_ctrlRibbonBtnPush.CleanUp();
    m_ctrlRibbonBtnGroup.CleanUp();

    m_clrDlgBackground = (COLORREF)-1;
    m_brDlgBackground.DeleteObject ();
}

void CControllerVisualManager::OnUpdateSystemColors()
{
    CleanUp();            

    SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);

    CMFCVisualManagerOffice2007::OnUpdateSystemColors();


    if (afxGlobalData.IsHighContrastMode() || afxGlobalData.m_nBitsPerPixel <= 8)
    {
        return;
    }

    m_nMenuBorderSize = 1;

    CTagManager tm;

    if (!tm.LoadFromResource(S3_SIGNAGE_MANAGEMEN_STYLE, S3_STYLE_XML))
    {
        DWORD x = GetLastError();

        return;
    }

    {
        CString strStyle;
        tm.ExcludeTag(_T("STYLE"), strStyle);
        tm.SetBuffer(strStyle);
    }

    CString strItem;

    m_nType = 20;

    CString oldStyle = m_strStylePrefix;
    if (!tm.IsEmpty())
    {
        int nVersion = 0;

        if (tm.ExcludeTag(_T("VERSION"), strItem))
        {
            CTagManager tmItem(strItem);

            tmItem.ReadInt(_T("NUMBER"), nVersion);

            if (nVersion == 2011)
            {
                tmItem.ReadInt(_T("TYPE"), m_nType);

                if (m_nType < 10)
                {
                    m_nType *= 10;
                }

                m_bLoaded = TRUE;
            }

            if (m_bLoaded)
            {
                if (tmItem.ExcludeTag(_T("ID_PREFIX"), strItem))
                {
                    strItem.Trim();
                    m_strStylePrefix = strItem;
                }
            }
        }
    }
    if (tm.ExcludeTag(_T("GLOBALS"), strItem))
    {
        ParseStyleXMLGlobal(strItem);
    }
/*
    if (tm.ExcludeTag (_T("MAINWND"), strItem))
    {
        ParseStyleXMLMainWnd(strItem);
    }
*/
    if (tm.ExcludeTag (_T("SLIDER"), strItem))
    {
        ParseStyleXMLSlider(strItem);
    }
    if (tm.ExcludeTag (_T("SCROLLBAR"), strItem))
    {
        ParseStyleXMLScrollBar(strItem);
    }
    if (tm.ExcludeTag (_T("BUTTONS"), strItem))
    {
        ParseStyleXMLButtonGroup(strItem);
    }
    m_strStylePrefix = oldStyle;
}

BOOL CControllerVisualManager::ParseStyleXMLSlider(const CString& strItem)
{
    if (strItem.IsEmpty ())
    {
        return FALSE;
    }

    CTagManager tmItem (strItem);

    tmItem.ReadControlRenderer(_T("THUMB_B"), m_ctrlSliderThumb[0], MakeResourceID(_T("SLIDER_THUMB_B")));
    tmItem.ReadControlRenderer(_T("THUMB_H"), m_ctrlSliderThumb[1], MakeResourceID(_T("SLIDER_THUMB_H")));
    tmItem.ReadControlRenderer(_T("THUMB_T"), m_ctrlSliderThumb[2], MakeResourceID(_T("SLIDER_THUMB_T")));
    tmItem.ReadControlRenderer(_T("THUMB_R"), m_ctrlSliderThumb[3], MakeResourceID(_T("SLIDER_THUMB_R")));
    tmItem.ReadControlRenderer(_T("THUMB_V"), m_ctrlSliderThumb[4], MakeResourceID(_T("SLIDER_THUMB_V")));
    tmItem.ReadControlRenderer(_T("THUMB_L"), m_ctrlSliderThumb[5], MakeResourceID(_T("SLIDER_THUMB_L")));

    return TRUE;

}

BOOL CControllerVisualManager::ParseStyleXMLScrollBar(const CString& strItem)
{
    if (strItem.IsEmpty ())
    {
        return FALSE;
    }

    CTagManager tmItem(strItem);

    LPCTSTR szSBName [] = {_T("HORZ"), _T("VERT")};
    LPCTSTR szSBNameRes [] = {_T("H"), _T("V")};

    CString strSB;
    for (int i = 0; i < 2; i++)
    {
        if (tmItem.ExcludeTag (szSBName[i], strSB))
        {
            CTagManager tmSB (strSB);

            CString strName (_T("SB_"));
            strName += szSBNameRes[i];

            tmSB.ReadControlRenderer (_T("BACK_1"), m_ctrlScrollBar_Back[i][0], MakeResourceID(strName + _T("_BACK_1")));
            tmSB.ReadControlRenderer (_T("ITEM_1"), m_ctrlScrollBar_Item[i][0], MakeResourceID(strName + _T("_ITEM_1")));
            tmSB.ReadControlRenderer (_T("THUMB_BACK_1"), m_ctrlScrollBar_ThumbBack[i][0], MakeResourceID(strName + _T("_THUMB_BACK_1")));
            tmSB.ReadControlRenderer (_T("THUMB_ICON_1"), m_ctrlScrollBar_ThumbIcon[i][0], MakeResourceID(strName + _T("_THUMB_ICON_1")));

            tmSB.ReadControlRenderer (_T("BACK_2"), m_ctrlScrollBar_Back[i][1], MakeResourceID(strName + _T("_BACK_2")));
            tmSB.ReadControlRenderer (_T("ITEM_2"), m_ctrlScrollBar_Item[i][1], MakeResourceID(strName + _T("_ITEM_2")));
            tmSB.ReadControlRenderer (_T("THUMB_BACK_2"), m_ctrlScrollBar_ThumbBack[i][1], MakeResourceID(strName + _T("_THUMB_BACK_2")));
            tmSB.ReadControlRenderer (_T("THUMB_ICON_2"), m_ctrlScrollBar_ThumbIcon[i][1], MakeResourceID(strName + _T("_THUMB_ICON_2")));
        }
    }

    return TRUE;
}

BOOL CControllerVisualManager::ParseStyleXMLGlobal(const CString& strItem)
{
    if (strItem.IsEmpty ())
    {
        return FALSE;
    }

    // dialog background
    CTagManager tmItem (strItem);

    m_clrDlgBackground = afxGlobalData.clrBarLight;
    tmItem.ReadColor (_T("DlgBackColor"), m_clrDlgBackground);
    m_brDlgBackground.DeleteObject ();
    m_brDlgBackground.CreateSolidBrush (m_clrDlgBackground);

    return TRUE;
}

BOOL CControllerVisualManager::ParseStyleXMLMainWnd(const CString& strItem)
{
    if (strItem.IsEmpty ())
    {
        return FALSE;
    }

    CTagManager tmItem (strItem);

    // caption
    CString strCaption;
    if (tmItem.ExcludeTag (_T("CAPTION"), strCaption))
    {
        CTagManager tmCaption (strCaption);

        NONCLIENTMETRICS ncm;
        if (afxGlobalData.GetNonClientMetrics  (ncm))
        {
            tmCaption.ReadFont (_T("FONT"), ncm.lfCaptionFont);
            m_AppCaptionFont.DeleteObject ();
            m_AppCaptionFont.CreateFontIndirect (&ncm.lfCaptionFont);
        }

        tmCaption.ReadColor (_T("ActiveStart"), m_clrAppCaptionActiveStart);
        tmCaption.ReadColor (_T("ActiveFinish"), m_clrAppCaptionActiveFinish);
        tmCaption.ReadColor (_T("InactiveStart"), m_clrAppCaptionInactiveStart);
        tmCaption.ReadColor (_T("InactiveFinish"), m_clrAppCaptionInactiveFinish);
        tmCaption.ReadColor (_T("ActiveText"), m_clrAppCaptionActiveText);
        tmCaption.ReadColor (_T("InactiveText"), m_clrAppCaptionInactiveText);
        tmCaption.ReadColor (_T("ActiveTitleText"), m_clrAppCaptionActiveTitleText);
        tmCaption.ReadColor (_T("InactiveTitleText"), m_clrAppCaptionInactiveTitleText);

        tmCaption.ReadBool (_T("TextCenter"), m_bNcTextCenter);

        tmCaption.ReadControlRenderer (_T("BORDER"), m_ctrlMainBorderCaption, MakeResourceID(_T("MAINBRD_CAPTION")));

        m_szNcBtnSize[0] = CSize (::GetSystemMetrics (SM_CXSIZE),
            ::GetSystemMetrics (SM_CYSIZE));
        m_szNcBtnSize[1] = CSize (::GetSystemMetrics (SM_CXSMSIZE),
            ::GetSystemMetrics (SM_CYSMSIZE));

        // buttons
        CString strButtons;
        if (tmCaption.ExcludeTag (_T("BUTTONS"), strButtons))
        {
            CTagManager tmButtons (strButtons);

            for (int i = 0; i < 2; i++)
            {
                CString str;
                CString suffix;
                if (i == 1)
                {
                    suffix = _T("_S");
                }
                if (tmButtons.ExcludeTag (i == 0 ? _T("NORMAL") : _T("SMALL"), str))
                {
                    CTagManager tmBtn (str);

                    tmBtn.ReadSize (_T("ConstSize"), m_szNcBtnSize[i]);

                    CSize sizeIcon (0, 0);
                    if (tmBtn.ReadSize (_T("IconSize"), sizeIcon))
                    {
                        m_SysBtnClose[i].Clear ();
                        m_SysBtnClose[i].SetPreMultiplyAutoCheck (TRUE);
                        m_SysBtnClose[i].SetImageSize (sizeIcon);
                        m_SysBtnClose[i].LoadStr (MakeResourceID(_T("SYS_BTN_CLOSE") + suffix));

                        m_SysBtnRestore[i].Clear ();
                        m_SysBtnRestore[i].SetPreMultiplyAutoCheck (TRUE);
                        m_SysBtnRestore[i].SetImageSize (sizeIcon);
                        m_SysBtnRestore[i].LoadStr (MakeResourceID(_T("SYS_BTN_RESTORE") + suffix));

                        m_SysBtnMaximize[i].Clear ();
                        m_SysBtnMaximize[i].SetPreMultiplyAutoCheck (TRUE);
                        m_SysBtnMaximize[i].SetImageSize (sizeIcon);
                        m_SysBtnMaximize[i].LoadStr (MakeResourceID(_T("SYS_BTN_MAXIMIZE") + suffix));

                        m_SysBtnMinimize[i].Clear ();
                        m_SysBtnMinimize[i].SetPreMultiplyAutoCheck (TRUE);
                        m_SysBtnMinimize[i].SetImageSize (sizeIcon);
                        m_SysBtnMinimize[i].LoadStr (MakeResourceID(_T("SYS_BTN_MINIMIZE") + suffix));

                        m_SysBtnHelp[i].Clear ();
                        m_SysBtnHelp[i].SetPreMultiplyAutoCheck (TRUE);
                        m_SysBtnHelp[i].SetImageSize (sizeIcon);
                        m_SysBtnHelp[i].LoadStr (MakeResourceID(_T("SYS_BTN_HELP") + suffix));
                    }

                    CTagManager::ParseControlRenderer (tmBtn.GetBuffer (), 
                        m_SysBtnBack[i], MakeResourceID(_T("SYS_BTN_BACK")));
                    CTagManager::ParseControlRenderer (tmBtn.GetBuffer (), 
                        m_SysBtnBackC[i], MakeResourceID(_T("SYS_BTN_CLOSE_BACK")));
                }
            }
        }
    }

    // border
    tmItem.ReadControlRenderer (_T("BORDER"), m_ctrlMainBorder, MakeResourceID(_T("MAINBRD")));
    tmItem.ReadControlRenderer (_T("BORDER_MDICHILD"), m_ctrlMDIChildBorder, MakeResourceID(_T("MDICHILDBRD")));
    tmItem.ReadControlRenderer (_T("BORDER_DIALOG"), m_ctrlDialogBorder, MakeResourceID(_T("DIALOGBRD")));

    if (tmItem.ReadColor (_T("MainClientArea"), m_clrMainClientArea))
    {
        m_brMainClientArea.DeleteObject ();
        m_brMainClientArea.CreateSolidBrush (m_clrMainClientArea);
    }

    return TRUE;
}

BOOL CControllerVisualManager::ParseStyleXMLButtonGroup(const CString& strItem)
{
    if (strItem.IsEmpty ())
    {
        return FALSE;
    }

    CTagManager tmButtons (strItem);

    tmButtons.ReadControlRenderer (_T("BUTTON_CHECK"), m_ctrlRibbonBtnCheck, MakeResourceID(_T("RB_BTN_CHECK")));
    tmButtons.ReadControlRenderer (_T("BUTTON_RADIO"), m_ctrlRibbonBtnRadio, MakeResourceID(_T("RB_BTN_RADIO")));
    tmButtons.ReadControlRenderer (_T("BUTTON_PUSH"), m_ctrlRibbonBtnPush, MakeResourceID(_T("RB_BTN_PUSH")));
    tmButtons.ReadControlRenderer (_T("BUTTON_GROUP"), m_ctrlRibbonBtnGroup, MakeResourceID(_T("RB_BTN_GRP")));

    m_ctrlRibbonBtnCheck.SmoothResize (afxGlobalData.GetRibbonImageScale ());
    m_ctrlRibbonBtnRadio.SmoothResize (afxGlobalData.GetRibbonImageScale ());

    return TRUE;
}
//////////////////////////////////////////////////////////////////////////
void CControllerVisualManager::OnDrawSliderChannel(CDC* pDC, CBORROW_SliderCtrl* pSlider, BOOL bVert, CRect rect, BOOL bDrawOnGlass)
{
    if (!CanDrawImage())
    {
        OnDrawSliderChannelFallback(pDC, pSlider, bVert, rect, bDrawOnGlass);
        return;
    }

    ASSERT_VALID (pDC);
    DrawSeparator(pDC, rect, m_penSeparatorDark, m_penSeparator2, !bVert);
}

void CControllerVisualManager::OnDrawSliderChannelFallback(CDC* pDC, CBORROW_SliderCtrl* /*pSlider*/, BOOL bVert, CRect rect, BOOL bDrawOnGlass)
{
    ASSERT_VALID (pDC);

    if (bVert)
    {
        if (rect.Width () < 3)
        {
            rect.right++;
        }
    }
    else
    {
        if (rect.Height () < 3)
        {
            rect.bottom++;
        }
    }

    if (m_hThemeTrack != NULL && afxGlobalData.m_nBitsPerPixel > 8 && !afxGlobalData.IsHighContrastMode ())
    {
        (*m_pfDrawThemeBackground) (m_hThemeTrack, pDC->GetSafeHdc(), 
            bVert ? TKP_TRACKVERT : TKP_TRACK,
            1, &rect, 0);
        return;
    }

    pDC->Draw3dRect (rect, afxGlobalData.clrBarShadow, afxGlobalData.clrBarHilite);
}

void CControllerVisualManager::OnDrawSliderThumb (CDC* pDC, CBORROW_SliderCtrl* pSlider, 
    CRect rect, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled,
    BOOL bVert, BOOL bLeftTop, BOOL bRightBottom,
    BOOL bDrawOnGlass)
{
    if (!CanDrawImage ())
    {
        OnDrawSliderThumbFallback(
            pDC, pSlider, rect, bIsHighlighted, bIsPressed, bIsDisabled,
            bVert, bLeftTop, bRightBottom, bDrawOnGlass);
        return;
    }

    ASSERT_VALID (pDC);

    int indexRen = 0;

    if (bLeftTop && bRightBottom)
    {
        indexRen = 1;
    }
    else if (bLeftTop)
    {
        indexRen = 2;
    }

    if (bVert)
    {
        indexRen += 3;
    }

    CMFCControlRenderer* pRenderer = &m_ctrlSliderThumb[indexRen];

    if (pRenderer != NULL)
    {
        int index = 0;
        if (bIsDisabled)
        {
            index = 3;
        }
        else	
        {
            if (bIsPressed)
            {
                index = 2;
            }
            else if (bIsHighlighted)
            {
                index = 1;
            }
        }

        CRect rectSides = pRenderer->GetParams ().m_rectSides;

        if ((rectSides.left + rectSides.right) >= rect.Width () || rectSides.top + rectSides.bottom >= rect.Height ())
        {

            OnDrawSliderThumbFallback(
                pDC, pSlider, rect, bIsHighlighted, bIsPressed, bIsDisabled,
                bVert, bLeftTop, bRightBottom, bDrawOnGlass);

        }
        else
        {
            pRenderer->Draw(pDC, rect, index);
        }
    }
}

void CControllerVisualManager::OnDrawSliderThumbFallback(CDC* pDC, CBORROW_SliderCtrl* pSlider, CRect rect, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled, BOOL bVert, BOOL bLeftTop, BOOL bRightBottom, BOOL bDrawOnGlass)
{

}

void CControllerVisualManager::OnScrollBarDrawThumb(CDC* pDC, CBORROW_ScrollBar* pScrollBar, CRect rect, BOOL bHorz, BOOL bHighlighted, BOOL bPressed, BOOL bDisabled)
{
    if (!CanDrawImage ())
    {
        return;
    }

    int nScroll = bHorz ? 0 : 1;
    BOOL bIsFrame = pScrollBar->GetVisualStyle () == CBORROW_ScrollBar::BORROW_SBSTYLE_VISUAL_MANAGER_FRAME;
    int nIndex  = bIsFrame ? 0 : 1;

    m_ctrlScrollBar_Back[nScroll][nIndex].Draw (pDC, rect, 0);

    if (!bDisabled)
    {
        if (!IsBeta ())
        {
            if (bHorz)
            {
                rect.DeflateRect (0, 1);
            }
            else
            {
                rect.DeflateRect (1, 0);
            }
        }

        m_ctrlScrollBar_ThumbBack[nScroll][nIndex].Draw (pDC, rect, bPressed ? 2 : bHighlighted ? 1 : 0);

        if (rect.Width () - 4 > m_ctrlScrollBar_ThumbIcon[nScroll][nIndex].GetParams ().m_rectImage.Width () &&
            rect.Height () - 4 > m_ctrlScrollBar_ThumbIcon[nScroll][nIndex].GetParams ().m_rectImage.Height ())
        {
            rect.OffsetRect (1, 1);
            m_ctrlScrollBar_ThumbIcon[nScroll][nIndex].FillInterior (pDC, rect, 
                CMFCToolBarImages::ImageAlignHorzCenter, CMFCToolBarImages::ImageAlignVertCenter,
                bPressed ? 2 : bHighlighted ? 1 : 0);
        }
    }
}

void CControllerVisualManager::OnScrollBarDrawButton(CDC* pDC, CBORROW_ScrollBar* pScrollBar, CRect rect, BOOL bHorz, BOOL bHighlighted, BOOL bPressed, BOOL bFirst, BOOL bDisabled)
{
    if (!CanDrawImage ())
    {
        return;
    }

    int nScroll = bHorz ? 0 : 1;
    BOOL bIsFrame = pScrollBar->GetVisualStyle () == CBORROW_ScrollBar::BORROW_SBSTYLE_VISUAL_MANAGER_FRAME;
    int nIndex  = bIsFrame ? 0 : 1;

    m_ctrlScrollBar_Back[nScroll][nIndex].Draw (pDC, rect, 0);

    if (!bDisabled)
    {
        if (!IsBeta ())
        {
            if (bHorz)
            {
                rect.DeflateRect (0, 1);
            }
            else
            {
                rect.DeflateRect (1, 0);
            }
        }

        m_ctrlScrollBar_Item[nScroll][nIndex].Draw (pDC, rect, bPressed ? 3 : bHighlighted ? 2 : pScrollBar->IsActive () ? 1 : 0);
    }

    CMenuImages::IMAGES_IDS ids;
    if (bHorz)
    {
        ids = bFirst ? CMenuImages::IdArrowLeftTab3d : CMenuImages::IdArrowRightTab3d;
    }
    else
    {
        ids = bFirst ? CMenuImages::IdArrowUpLarge : CMenuImages::IdArrowDownLarge;
    }

    CMenuImages::IMAGE_STATE state = bDisabled ? CMenuImages::ImageGray : CMenuImages::ImageBlack2;

/*
    if (!pScrollBar->IsActive () && m_Style == VS2007_ObsidianBlack && bIsFrame)
    {
        state = CMenuImages::ImageLtGray;
    }
*/

    CMenuImages::Draw (pDC, ids, rect, state);

}

void CControllerVisualManager::OnScrollBarFillBackground(CDC* pDC, CBORROW_ScrollBar* pScrollBar, CRect rect, BOOL bHorz, BOOL bHighlighted, BOOL bPressed, BOOL bFirst, BOOL bDisabled)
{
    if (!CanDrawImage ())
    {
        return;
    }

    int nScroll = bHorz ? 0 : 1;
    BOOL bIsFrame = pScrollBar->GetVisualStyle () == CBORROW_ScrollBar::BORROW_SBSTYLE_VISUAL_MANAGER_FRAME;
    int nIndex  = bIsFrame ? 0 : 1;

    m_ctrlScrollBar_Back[nScroll][nIndex].Draw (pDC, rect, bPressed ? 1 : 0);
}

void CControllerVisualManager::OnDrawGroup(CDC* pDC, CBORROW_Group* pGroup, CRect rect, const CString& strName)
{
    if (!CanDrawImage ())
    {
        OnDrawGroupFallback(pDC, pGroup, rect, strName);
        return;
    }

    ASSERT_VALID (pDC);
    ASSERT_VALID (pGroup);

    CSize sizeText = pDC->GetTextExtent (strName);

    CRect rectFrame = rect;
    rectFrame.top += sizeText.cy / 2;

    if (sizeText == CSize (0, 0))
    {
        rectFrame.top += pDC->GetTextExtent (_T("A")).cy / 2;
    }

    int xMargin = sizeText.cy / 2;

    CRect rectText = rect;
    rectText.left += xMargin;
    rectText.right = rectText.left + sizeText.cx + xMargin;
    rectText.bottom = rectText.top + sizeText.cy;

    if (!strName.IsEmpty ())
    {
        pDC->ExcludeClipRect (rectText);
    }

    m_ctrlRibbonBtnGroup.DrawFrame (pDC, rectFrame);

    pDC->SelectClipRgn (NULL);

    if (strName.IsEmpty ())
    {
        return;
    }

    DWORD dwTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_NOCLIP;

    {
        CString strCaption = strName;
        pDC->DrawText (strCaption, rectText, dwTextStyle);
    }
}

void CControllerVisualManager::OnDrawGroupFallback(CDC* pDC, CBORROW_Group* pGroup, CRect rect, const CString& strName)
{
    ASSERT_VALID (pDC);
    ASSERT_VALID (pGroup);

    CSize sizeText = pDC->GetTextExtent (strName);

    CRect rectFrame = rect;
    rectFrame.top += sizeText.cy / 2;

    if (sizeText == CSize (0, 0))
    {
        rectFrame.top += pDC->GetTextExtent (_T("A")).cy / 2;
    }

    int xMargin = sizeText.cy / 2;

    CRect rectText = rect;
    rectText.left += xMargin;
    rectText.right = rectText.left + sizeText.cx + xMargin;
    rectText.bottom = rectText.top + sizeText.cy;

    if (!strName.IsEmpty ())
    {
        pDC->ExcludeClipRect (rectText);
    }

    if (m_pfDrawThemeBackground != NULL && m_hThemeButton != NULL)
    {
        (*m_pfDrawThemeBackground) (m_hThemeButton, pDC->GetSafeHdc(), BP_GROUPBOX, 
            pGroup->IsWindowEnabled () ? GBS_NORMAL : GBS_DISABLED, &rectFrame, 0);
    }
    else
    {
        CDrawingManager dm (*pDC);
        dm.DrawRect (rectFrame, (COLORREF)-1, afxGlobalData.clrBarDkShadow);
    }

    pDC->SelectClipRgn (NULL);

    if (strName.IsEmpty ())
    {
        return;
    }

    DWORD dwTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_NOCLIP;

    {
        CString strCaption = strName;
        pDC->DrawText (strCaption, rectText, dwTextStyle);
    }
}

void CControllerVisualManager::OnDrawDateTimeDropButton(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bPressed, BOOL bHighlighted, CBORROW_DateTimeCtrl* pCtrl)
{
    if (!CanDrawImage ())
    {
        OnDrawDateTimeDropButtonFallback(pDC, rect, bDisabled, bPressed, bHighlighted, pCtrl);
        return;
    }

    ASSERT_VALID(pDC);
    ASSERT_VALID (this);

    COLORREF color1 = m_clrComboBtnStart;
    COLORREF color2 = m_clrComboBtnFinish;
    COLORREF colorBorder = m_clrComboBtnBorder;

    if (bDisabled)
    {
        color1 = m_clrComboBtnDisabledStart;
        color2 = m_clrComboBtnDisabledFinish;
        colorBorder = m_clrComboBtnBorderDisabled;
    }
    else if (bPressed)
    {
        color1 = m_clrComboBtnPressedStart;
        color2 = m_clrComboBtnPressedFinish;
        colorBorder = m_clrComboBtnBorderPressed;
    }
    else if (bHighlighted)
    {
        color1 = m_clrComboBtnHighlightedStart;
        color2 = m_clrComboBtnHighlightedFinish;
        colorBorder = m_clrComboBtnBorderHighlighted;
    }

    if (colorBorder != (COLORREF)(-1))
    {
        if (CMFCToolBarImages::m_bIsDrawOnGlass)
        {
            CDrawingManager dm (*pDC);
            dm.DrawRect (rect, (COLORREF)-1, colorBorder);
        }
        else
        {
            pDC->Draw3dRect (rect, colorBorder, colorBorder);
        }

        rect.DeflateRect (1, 1, 1, 1);
    }

    CDrawingManager dm (*pDC);
    dm.FillGradient (rect, color1, color2, TRUE);

    CMenuImages::Draw (pDC, CMenuImages::IdArrowDown, rect,
        bDisabled ? CMenuImages::ImageGray : CMenuImages::ImageBlack);

}

void CControllerVisualManager::OnDrawDateTimeDropButtonFallback(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bPressed, BOOL bHighlighted, CBORROW_DateTimeCtrl* pCtrl)
{
    ASSERT_VALID(pDC);
    ASSERT_VALID (this);

    if (bHighlighted || bPressed)
    {
        OnFillHighlightedArea (pDC, rect, bPressed ? &m_brHighlightDn : &m_brHighlight, NULL);
    }
    else
    {
        if (CMFCToolBarImages::m_bIsDrawOnGlass)
        {
            CDrawingManager dm (*pDC);
            dm.DrawRect (rect, 
                bDisabled ? afxGlobalData.clrBtnFace : afxGlobalData.clrBarFace, 
                afxGlobalData.clrBarHilite);
        }
        else
        {
            pDC->FillRect (rect, bDisabled ? &afxGlobalData.brBtnFace : &afxGlobalData.brBarFace);
            pDC->Draw3dRect (rect, afxGlobalData.clrBarHilite, afxGlobalData.clrBarHilite);
        }
    }

    CMenuImages::Draw (pDC, CMenuImages::IdArrowDown, rect,
        bDisabled ? CMenuImages::ImageGray : CMenuImages::ImageBlack);

    if (bHighlighted)
    {
        if (CMFCToolBarImages::m_bIsDrawOnGlass)
        {
            CDrawingManager dm (*pDC);
            dm.DrawRect (rect, (COLORREF)-1, m_clrMenuItemBorder);
        }
        else
        {
            pDC->Draw3dRect (rect, m_clrMenuItemBorder, m_clrMenuItemBorder);
        }
    }
}

void CControllerVisualManager::GetCalendarColors(const CBORROW_Calendar* pCalendar, CBORROW_CalendarColors& colors)
{
    if (!CanDrawImage ())
    {
        return;
    }

    colors.clrCaption = m_clrCaptionBarGradientDark;
    colors.clrCaptionText = m_clrCaptionBarText;

    colors.clrSelected = m_clrHighlightGradientDark;
    colors.clrSelectedText = m_clrOutlookPageTextHighlighted;
}

CSize CControllerVisualManager::GetCheckRadioDefaultSize()
{
    if (!CanDrawImage ())
    {
        return GetCheckRadioDefaultSizeFallback ();
    }

    return m_ctrlRibbonBtnCheck.GetParams().m_rectImage.Size () + CSize (2, 2);
}

CSize CControllerVisualManager::GetCheckRadioDefaultSizeFallback()
{
    return CSize (::GetSystemMetrics (SM_CXMENUCHECK) + 1, ::GetSystemMetrics (SM_CYMENUCHECK) + 1);
}

void CControllerVisualManager::OnDrawCheckBoxEx(CDC *pDC, CRect rect, int nState, BOOL bHighlighted, BOOL bPressed, BOOL bEnabled)
{
    if (CMFCToolBarImages::m_bIsDrawOnGlass)
    {
        CDrawingManager dm (*pDC);

        rect.DeflateRect (1, 1);

        dm.DrawRect (rect, 
            bEnabled ? afxGlobalData.clrWindow : afxGlobalData.clrBarFace, 
            afxGlobalData.clrBarShadow);

        if (nState == 1)
        {
            CMenuImages::Draw (pDC, CMenuImages::IdCheck, rect,
                CMenuImages::ImageBlack);
        }

        return;
    }

    if (bHighlighted)
    {
        pDC->DrawFocusRect (rect);
    }

    rect.DeflateRect (1, 1);
    pDC->FillSolidRect (&rect, bEnabled ? afxGlobalData.clrWindow :
        afxGlobalData.clrBarFace);

    pDC->Draw3dRect (&rect, 
        afxGlobalData.clrBarDkShadow,
        afxGlobalData.clrBarHilite);

    rect.DeflateRect (1, 1);
    pDC->Draw3dRect (&rect,
        afxGlobalData.clrBarShadow,
        afxGlobalData.clrBarLight);

    if (nState == 1)
    {
        CMenuImages::Draw (pDC, CMenuImages::IdCheck, rect,
            CMenuImages::ImageBlack);
    }
    else if (nState == 2)
    {
        rect.DeflateRect (1, 1);

        WORD HatchBits [8] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };

        CBitmap bmp;
        bmp.CreateBitmap (8, 8, 1, 1, HatchBits);

        CBrush br;
        br.CreatePatternBrush (&bmp);

        pDC->FillRect (rect, &br);
    }
}

void CControllerVisualManager::OnDrawRadioButton(CDC *pDC, CRect rect, BOOL bOn, BOOL bHighlighted, BOOL bPressed, BOOL bEnabled)
{
    CDrawingManager dm (*pDC);

    rect.DeflateRect (1, 1);

    dm.DrawEllipse (rect,
        bEnabled ? afxGlobalData.clrBarHilite : afxGlobalData.clrBarFace,
        bHighlighted && bEnabled ? afxGlobalData.clrBarDkShadow : afxGlobalData.clrBarShadow);

    if (bOn)
    {
        rect.DeflateRect (rect.Width () / 3, rect.Width () / 3);

        dm.DrawEllipse (rect,
            bHighlighted && bEnabled ? afxGlobalData.clrBarDkShadow : afxGlobalData.clrBarShadow,
            (COLORREF)-1);
    }

}

BOOL CControllerVisualManager::OnDrawPushButton(CDC* pDC, CRect rect, CBorrow_Button* pButton, COLORREF& clrText)
{
    if (!CanDrawImage ())
    {
        return OnDrawPushButtonFallback (pDC, rect, pButton, clrText);
    }

    ASSERT_VALID (pDC);
    ASSERT_VALID (pButton);

    int index = 0;

    BOOL bDisabled    = !pButton->IsWindowEnabled ();
    BOOL bFocused     = pButton->GetSafeHwnd () == ::GetFocus ();
    BOOL bDefault     = pButton->IsDefaultButton ();
    BOOL bPressed     = pButton->IsPressed ();
    BOOL bChecked     = pButton->IsChecked ();
    BOOL bHighlighted = pButton->IsHighlighted ();

    bHighlighted |= bFocused;

    if (bDisabled)
    {
        index = 6;
    }
    else
    {
        if (bChecked)
        {
            index = 3;
        }
        else
        {
            if (bDefault && !bHighlighted)
            {
                index = 5;
            }
        }

        if (bPressed)
        {
            if (bHighlighted)
            {
                index = 2;
            }
        }
        else if (bHighlighted)
        {
            index++;
        }
    }

    afxGlobalData.DrawParentBackground (pButton, pDC);
    m_ctrlRibbonBtnPush.Draw (pDC, rect, index);

    if (bDisabled)
    {
        clrText = m_clrToolBarBtnTextDisabled;
    }
    else if (bHighlighted)
    {
        clrText = m_clrToolBarBtnTextHighlighted;
    }

    return TRUE;
}

BOOL CControllerVisualManager::OnDrawPushButtonFallback(CDC* pDC, CRect rect, CBorrow_Button* pButton, COLORREF& clrText)
{
    ASSERT_VALID (pDC);
    ASSERT_VALID (pButton);

    CDrawingManager dm (*pDC);

    COLORREF clrFace = afxGlobalData.clrBarFace;

    if (!pButton->IsWindowEnabled ())
    {
        {
            pDC->FillRect (rect, &afxGlobalData.brBarFace);
            pDC->Draw3dRect (rect, afxGlobalData.clrBarShadow, afxGlobalData.clrBarShadow);
        }
    }
    else if (pButton->IsPressed () || pButton->GetCheck ())
    {
        OnFillHighlightedArea (pDC, rect, &m_brHighlightDn, NULL);

        {
            pDC->Draw3dRect (rect, m_clrMenuItemBorder, m_clrMenuItemBorder);
        }

        clrFace = m_clrHighlightDn;
    }
    else if (pButton->IsHighlighted () || CWnd::GetFocus () == pButton)
    {
        OnFillHighlightedArea (pDC, rect, &m_brHighlight, NULL);

        {
            pDC->Draw3dRect (rect, m_clrMenuItemBorder, m_clrMenuItemBorder);
        }

        clrFace = m_clrHighlight;
    }
    else
    {
        {
            pDC->FillRect (rect, &afxGlobalData.brBarFace);
            pDC->Draw3dRect (rect, afxGlobalData.clrBarDkShadow, afxGlobalData.clrBarDkShadow);
        }
    }

    if (GetRValue (clrFace) <= 128 ||
        GetGValue (clrFace) <= 128 ||
        GetBValue (clrFace) <= 128)
    {
        clrText = RGB (255, 255, 255);
    }

    return TRUE;
}


BOOL CControllerVisualManager::OnFillDialog( CDC* pDC, CWnd* pDlg, CRect rect )
{
    if (!CanDrawImage () || m_brDlgBackground.GetSafeHandle () == NULL)
    {
        return OnFillDialogFallback (pDC, pDlg, rect);
    }

    ASSERT_VALID (pDC);

    pDC->FillRect (rect, &GetDlgBackBrush (pDlg));

    return TRUE;

}

BOOL CControllerVisualManager::OnFillDialogFallback( CDC* pDC, CWnd* pDlg, CRect rect )
{
    ASSERT_VALID (pDC);
    pDC->FillRect (rect, &GetDlgBackBrush (pDlg));

    return TRUE;
}

void CControllerVisualManager::SetToolTipInfo(const CMFCToolTipInfo& info)
{
    m_ToolTipParams = const_cast<CMFCToolTipInfo&>(info);
}


void CControllerVisualManager::OnDrawGridSortArrow (CBCGPGridCtrl* /*pCtrl*/, CDC* pDC,
											  CRect rectArrow, BOOL bIsUp)
{
	DoDrawHeaderSortArrow (pDC, rectArrow, bIsUp, TRUE/*!pCtrl->IsControlBarColors ()*/);
}
//********************************************************************************
COLORREF CControllerVisualManager::OnFillGridGroupByBoxBackground (CDC* pDC, CRect rect)
{
	ASSERT_VALID (pDC);

	CBrush br (afxGlobalData.clrBtnShadow);
	pDC->FillRect (rect, &br);

	return afxGlobalData.clrBarText;
}
//********************************************************************************
COLORREF CControllerVisualManager::OnFillGridGroupByBoxTitleBackground (CDC* pDC, CRect rect)
{
	ASSERT_VALID (pDC);

	pDC->FillRect (rect, &afxGlobalData.brBtnFace);
	return afxGlobalData.clrBtnShadow;
}
//********************************************************************************
COLORREF CControllerVisualManager::GetGridGroupByBoxLineColor () const
{
	return afxGlobalData.clrBarText;
}
//********************************************************************************
void CControllerVisualManager::OnDrawGridGroupByBoxItemBorder (CBCGPGridCtrl* /*pCtrl*/,
														 CDC* pDC, CRect rect)
{
	ASSERT_VALID (pDC);
	pDC->FillRect (rect, &afxGlobalData.brBtnFace);

	pDC->Draw3dRect (rect, afxGlobalData.clrBarWindow, afxGlobalData.clrBtnText);
	rect.DeflateRect (0, 0, 1, 1);
	pDC->Draw3dRect (rect, afxGlobalData.clrBarWindow, afxGlobalData.clrBtnShadow);
}
//********************************************************************************
COLORREF CControllerVisualManager::GetGridLeftOffsetColor (CBCGPGridCtrl* pCtrl)
{
	ASSERT_VALID (pCtrl);

	COLORREF clrGray = (COLORREF)-1;

	if (afxGlobalData.m_nBitsPerPixel <= 8)
	{
		clrGray = pCtrl->IsControlBarColors () ?
			afxGlobalData.clrBarShadow : afxGlobalData.clrBtnShadow;
	}
	else
	{
		clrGray = CDrawingManager::PixelAlpha (
			pCtrl->IsControlBarColors () ? afxGlobalData.clrBarFace : afxGlobalData.clrBtnFace, 94);
	}

	return clrGray;
}
//********************************************************************************
COLORREF CControllerVisualManager::GetGridItemSortedColor (CBCGPGridCtrl* pCtrl)
{
	ASSERT_VALID (pCtrl);

	COLORREF clrSortedColumn = (COLORREF)-1;
	if (afxGlobalData.m_nBitsPerPixel <= 8)
	{
		clrSortedColumn = pCtrl->GetBkColor ();
	}
	else
	{
		clrSortedColumn = CDrawingManager::PixelAlpha (
				pCtrl->GetBkColor (), .97, .97, .97);
	}

	return clrSortedColumn;
}
//********************************************************************************
void CControllerVisualManager::OnFillGridGroupBackground (CBCGPGridCtrl* /*pCtrl*/, CDC* pDC, 
												   CRect rectFill)
{
	ASSERT_VALID (pDC);
	pDC->FillRect (rectFill, &afxGlobalData.brWindow);
}
//********************************************************************************
void CControllerVisualManager::OnDrawGridGroupUnderline (CBCGPGridCtrl* pCtrl, CDC* pDC, 
												   CRect rectFill)
{
	ASSERT_VALID (pDC);
	ASSERT_VALID (pCtrl);

	COLORREF clrOld = pDC->GetBkColor ();
	pDC->FillSolidRect (rectFill, 
		pCtrl->IsControlBarColors () ? afxGlobalData.clrBarShadow : afxGlobalData.clrBtnShadow);
	pDC->SetBkColor (clrOld);
}
//********************************************************************************
COLORREF CControllerVisualManager::OnFillGridRowBackground (CBCGPGridCtrl* pCtrl, 
												  CDC* pDC, CRect rectFill, BOOL bSelected)
{
	ASSERT_VALID (pDC);
	ASSERT_VALID (pCtrl);

	// Fill area:
	if (!pCtrl->IsFocused ())
	{
		pDC->FillRect (rectFill, 
			pCtrl->IsControlBarColors () ? &afxGlobalData.brBarFace : &afxGlobalData.brBtnFace);
	}
	else
	{
		pDC->FillRect (rectFill, &afxGlobalData.brHilite);
	}

	// Return text color:
	if (!pCtrl->IsHighlightGroups () && bSelected)
	{
		return (!pCtrl->IsFocused ()) ? afxGlobalData.clrHotLinkNormalText : afxGlobalData.clrTextHilite;
	}

	return pCtrl->IsHighlightGroups () ? 
		(pCtrl->IsControlBarColors () ? afxGlobalData.clrBarShadow : afxGlobalData.clrBtnShadow) :
		afxGlobalData.clrWindowText;
}
//********************************************************************************
COLORREF CControllerVisualManager::OnFillGridItem (CBCGPGridCtrl* pCtrl, 
											CDC* pDC, CRect rectFill,
											BOOL bSelected, BOOL bActiveItem, BOOL bSortedColumn)
{
	ASSERT_VALID (pDC);
	ASSERT_VALID (pCtrl);

	// Fill area:
	if (bSelected && !bActiveItem)
	{
		if (!pCtrl->IsFocused ())
		{
			pDC->FillRect (rectFill,
				pCtrl->IsControlBarColors () ? &afxGlobalData.brBarFace : &afxGlobalData.brBtnFace);
			return afxGlobalData.clrBtnText;
		}
		else
		{
			pDC->FillRect (rectFill, &afxGlobalData.brHilite);
			return afxGlobalData.clrTextHilite;
		}
	}
	else
	{
		if (bActiveItem)
		{
			pDC->FillRect (rectFill, &afxGlobalData.brWindow);
		}
		else if (bSortedColumn)
		{
			CBrush br (pCtrl->GetSortedColor ());
			pDC->FillRect (rectFill, &br);
		}
		else
		{
			// no painting
		}
	}

	return (COLORREF)-1;
}
//********************************************************************************
void CControllerVisualManager::OnDrawGridHeaderMenuButton (CBCGPGridCtrl* /*pCtrl*/, CDC* pDC, CRect rect, 
		BOOL bHighlighted, BOOL /*bPressed*/, BOOL /*bDisabled*/)
{
	ASSERT_VALID (pDC);

	rect.DeflateRect (1, 1);

	if (bHighlighted)
	{
		pDC->Draw3dRect (rect, afxGlobalData.clrBtnHilite, afxGlobalData.clrBtnDkShadow);
	}
}
//********************************************************************************
void CControllerVisualManager::OnDrawGridSelectionBorder (CBCGPGridCtrl* /*pCtrl*/, CDC* pDC, CRect rect)
{
	ASSERT_VALID (pDC);

	pDC->Draw3dRect (rect, afxGlobalData.clrBtnDkShadow, afxGlobalData.clrBtnDkShadow);
	rect.DeflateRect (1, 1);
	pDC->Draw3dRect (rect, afxGlobalData.clrBtnDkShadow, afxGlobalData.clrBtnDkShadow);
}
//********************************************************************************
BOOL CControllerVisualManager::OnSetGridColorTheme (CBCGPGridCtrl* /*pCtrl*/, BCGP_GRID_COLOR_DATA& /*theme*/)
{
	return TRUE;
}
//********************************************************************************
COLORREF CControllerVisualManager::OnFillReportCtrlRowBackground (CBCGPGridCtrl* pCtrl, 
												  CDC* pDC, CRect rectFill,
												  BOOL bSelected, BOOL bGroup)
{
	ASSERT_VALID (pDC);
	ASSERT_VALID (pCtrl);

	// Fill area:
	COLORREF clrText = (COLORREF)-1;

	if (bSelected)
	{
		if (!pCtrl->IsFocused ())
		{
			pDC->FillRect (rectFill, 
				pCtrl->IsControlBarColors () ? &afxGlobalData.brBarFace : &afxGlobalData.brBtnFace);

			clrText = m_clrReportGroupText;
		}
		else
		{
			pDC->FillRect (rectFill, &afxGlobalData.brHilite);
			clrText = afxGlobalData.clrTextHilite;
		}
	}
	else
	{
		if (bGroup)
		{
			// no painting
			clrText = m_clrReportGroupText;
		}
	}

	// Return text color:
	return clrText;
}
//********************************************************************************
COLORREF CControllerVisualManager::GetReportCtrlGroupBackgoundColor ()
{
	return afxGlobalData.clrBtnLight;
}
//********************************************************************************
void CControllerVisualManager::OnFillGridBackground (CDC* pDC, CRect rect)
{
	ASSERT_VALID (pDC);

	pDC->FillRect (rect, &afxGlobalData.brWindow);
}
//********************************************************************************
void CControllerVisualManager::OnDrawGridExpandingBox (CDC* pDC, CRect rect, BOOL bIsOpened, COLORREF colorBox)
{
	OnDrawExpandingBox (pDC, rect, bIsOpened, colorBox);
}
//********************************************************************************
void CControllerVisualManager::OnFillGridHeaderBackground (CBCGPGridCtrl* /*pCtrl*/, CDC* pDC, CRect rect)
{
	ASSERT_VALID (pDC);

	pDC->FillRect (rect, &afxGlobalData.brBtnFace);
}
//********************************************************************************
BOOL CControllerVisualManager::OnDrawGridHeaderItemBorder (CBCGPGridCtrl* /*pCtrl*/, CDC* pDC, CRect rect, BOOL /*bPressed*/)
{
	ASSERT_VALID (pDC);

	pDC->Draw3dRect (rect, afxGlobalData.clrBtnHilite, afxGlobalData.clrBtnDkShadow);
	return TRUE;
}
//********************************************************************************
void CControllerVisualManager::OnFillGridRowHeaderBackground (CBCGPGridCtrl* /*pCtrl*/, CDC* pDC, CRect rect)
{
	ASSERT_VALID (pDC);

	pDC->FillRect (rect, &afxGlobalData.brBtnFace);
}
//********************************************************************************
BOOL CControllerVisualManager::OnDrawGridRowHeaderItemBorder (CBCGPGridCtrl* /*pCtrl*/, CDC* pDC, CRect rect, BOOL /*bPressed*/)
{
	ASSERT_VALID (pDC);

	pDC->Draw3dRect (rect, afxGlobalData.clrBtnHilite, afxGlobalData.clrBtnDkShadow);
	return TRUE;
}
//********************************************************************************
void CControllerVisualManager::OnFillGridSelectAllAreaBackground (CBCGPGridCtrl* /*pCtrl*/, CDC* pDC, CRect rect, BOOL /*bPressed*/)
{
	ASSERT_VALID (pDC);

	pDC->FillRect (rect, &afxGlobalData.brBtnFace);
}
//********************************************************************************
BOOL CControllerVisualManager::OnDrawGridSelectAllAreaBorder (CBCGPGridCtrl* /*pCtrl*/, CDC* pDC, CRect rect, BOOL /*bPressed*/)
{
	ASSERT_VALID (pDC);

	pDC->Draw3dRect (rect, afxGlobalData.clrBtnHilite, afxGlobalData.clrBtnDkShadow);
	return TRUE;
}

//*******************************************************************************
BOOL CControllerVisualManager::GetToolTipParams (CMFCToolTipInfo& params, 
										   UINT /*nType*/ /*= (UINT)(-1)*/)
{
	CMFCToolTipInfo dummy;
	params = dummy;

	return TRUE;
}
//*******************************************************************************
void CControllerVisualManager::OnFillToolTip (CDC* pDC, CBCGPToolTipCtrl* /*pToolTip*/, CRect rect,
										COLORREF& clrText, COLORREF& clrLine)
{
	if (m_pfDrawThemeBackground != NULL && m_hThemeToolTip != NULL)
	{
		(*m_pfDrawThemeBackground) (m_hThemeToolTip, pDC->GetSafeHdc(), TTP_STANDARD, 0, &rect, 0);

		if (m_pfGetThemeColor != NULL)
		{
			(*m_pfGetThemeColor) (m_hThemeToolTip, TTP_STANDARD, 0, TMT_TEXTCOLOR, &clrText);
			(*m_pfGetThemeColor) (m_hThemeToolTip, TTP_STANDARD, 0, TMT_EDGEDKSHADOWCOLOR, &clrLine);
		}
	}
	else
	{
		::FillRect (pDC->GetSafeHdc (), rect, ::GetSysColorBrush (COLOR_INFOBK));
	}
}

#if 0
BOOL CControllerVisualManager::OnNcPaint(CWnd* pWnd, const CObList& lstSysButtons, CRect rectRedraw)
{
    if (!CanDrawImage ())
    {
        return OnNcPaintFallback (pWnd, lstSysButtons, rectRedraw);
    }

    ASSERT_VALID (pWnd);

    if (pWnd->GetSafeHwnd () == NULL)
    {
        return FALSE;
    }

    CWindowDC dc (pWnd);

    if (dc.GetSafeHdc () != NULL)
    {
        CRgn rgn;
        if (!rectRedraw.IsRectEmpty ())
        {
            rgn.CreateRectRgnIndirect (rectRedraw);
            dc.SelectClipRgn (&rgn);
        }

        CMFCRibbonBar* pBar = GetRibbonBar (pWnd);
        BOOL bRibbonCaption  = pBar != NULL && 
            pBar->IsWindowVisible () &&
            pBar->IsReplaceFrameCaption ();

        CRect rtWindow;
        pWnd->GetWindowRect (rtWindow);
        pWnd->ScreenToClient (rtWindow);

        CRect rtClient;
        pWnd->GetClientRect (rtClient);

        rtClient.OffsetRect (-rtWindow.TopLeft ());
        dc.ExcludeClipRect (rtClient);

        rtWindow.OffsetRect (-rtWindow.TopLeft ());

        BOOL bActive = IsWindowActive (pWnd);

        CRect rectCaption (rtWindow);
        CSize szSysBorder (GetSystemBorders (bRibbonCaption));

        BOOL bDialog = pWnd->IsKindOf (RUNTIME_CLASS (CDialogEx))/* || pWnd->IsKindOf (RUNTIME_CLASS (CBCGPPropertySheet))*/;
        if (bDialog && (pWnd->GetStyle () & WS_THICKFRAME) == 0)
        {
            szSysBorder.cy = ::GetSystemMetrics (SM_CYFIXEDFRAME);
        }

        if (!pWnd->IsIconic ())
        {
            rectCaption.bottom = rectCaption.top + szSysBorder.cy;
        }

        const DWORD dwStyle = pWnd->GetStyle ();
        BOOL bMaximized = (dwStyle & WS_MAXIMIZE) == WS_MAXIMIZE;

        if (!bRibbonCaption)
        {
            const DWORD dwStyleEx = pWnd->GetExStyle ();
            const BOOL bIsSmallCaption = (dwStyleEx & WS_EX_TOOLWINDOW) != 0;

            if (!pWnd->IsIconic ())
            {
                rectCaption.bottom += bIsSmallCaption ? ::GetSystemMetrics (SM_CYSMCAPTION) : ::GetSystemMetrics (SM_CYCAPTION);
            }
            else
            {
                rectCaption.bottom -= 1;
            }

            HICON hIcon = GetWndIcon (pWnd);

            CString strText;
            pWnd->GetWindowText (strText);

            CString strTitle (strText);
            CString strDocument;

            BOOL bPrefix = FALSE;
            if ((dwStyle & FWS_ADDTOTITLE) == FWS_ADDTOTITLE)
            {
                bPrefix = (dwStyle & FWS_PREFIXTITLE) == FWS_PREFIXTITLE;
                CFrameWnd* pFrameWnd = DYNAMIC_DOWNCAST(CFrameWnd, pWnd);

                if (pFrameWnd != NULL)
                {
                    strTitle = pFrameWnd->GetTitle();

                    if (!strTitle.IsEmpty ())
                    {
                        if (strText.GetLength () >= strTitle.GetLength ())
                        {
                            if (bPrefix)
                            {
                                int pos = strText.Find (strTitle, strText.GetLength () - strTitle.GetLength ());
                                if (pos != -1)
                                {
                                    strTitle = strText.Right (strTitle.GetLength () + 3);
                                    strDocument = strText.Left (strText.GetLength () - strTitle.GetLength ());
                                }
                            }
                            else
                            {
                                int pos = strText.Find (strTitle);
                                if (pos != -1)
                                {
                                    strTitle = strText.Left (strTitle.GetLength () + 3);
                                    strDocument = strText.Right (strText.GetLength () - strTitle.GetLength ());
                                }	
                            }
                        }
                    }
                    else
                    {
                        strDocument = strText;
                    }
                }
            }

            if (bMaximized)
            {
                rectCaption.InflateRect (szSysBorder.cx, szSysBorder.cy, szSysBorder.cx, 0);
            }

            DrawNcCaption (&dc, rectCaption, dwStyle, dwStyleEx, 
                strTitle, strDocument, hIcon, bPrefix, bActive, m_bNcTextCenter,
                lstSysButtons);


            if (bMaximized)
            {
                return TRUE;
            }
        }
        else
        {
            if (bMaximized)
            {
                return TRUE;
            }

            rectCaption.bottom += pBar->GetCaptionHeight ();

            if (IsBeta ())
            {
                CRect rectBorder (m_ctrlMainBorderCaption.GetParams ().m_rectSides);

                COLORREF clr1  = bActive 
                    ? m_clrAppCaptionActiveStart 
                    : m_clrAppCaptionInactiveStart;
                COLORREF clr2  = bActive 
                    ? m_clrAppCaptionActiveFinish 
                    : m_clrAppCaptionInactiveFinish;

                CRect rectCaption2 (rectCaption);
                rectCaption2.DeflateRect (rectBorder.left, rectBorder.top, 
                    rectBorder.right, rectBorder.bottom);

                {
                    CDrawingManager dm (dc);
                    dm.Fill4ColorsGradient (rectCaption2, clr1, clr2, clr2, clr1, FALSE);
                }

                m_ctrlMainBorderCaption.DrawFrame (&dc, rectCaption, bActive ? 0 : 1);
            }
            else
            {
                m_ctrlMainBorderCaption.Draw (&dc, rectCaption, bActive ? 0 : 1);
            }
        }

        rtWindow.top = rectCaption.bottom;

        dc.ExcludeClipRect (rectCaption);

        if (pWnd->IsKindOf (RUNTIME_CLASS (CMDIChildWnd)) ||
            (bDialog && !m_ctrlDialogBorder.IsValid ()))
        {
            if (bDialog)
            {
                CRect rtDialog (rtWindow);
                rtDialog.DeflateRect (1, 0, 1, 1);
                dc.FillRect (rtDialog, &GetDlgBackBrush (pWnd));

                dc.ExcludeClipRect (rtDialog);
            }

            m_ctrlMDIChildBorder.DrawFrame (&dc, rtWindow, bActive ? 0 : 1);
        }
        else if (bDialog)
        {
            m_ctrlDialogBorder.DrawFrame (&dc, rtWindow, bActive ? 0 : 1);
        }
        else
        {
            m_ctrlMainBorder.DrawFrame (&dc, rtWindow, bActive ? 0 : 1);
        }

        if (bDialog)
        {
            dc.SelectClipRgn (NULL);
            return TRUE;
        }

        //-------------------------------
        // Find status bar extended area:
        //-------------------------------
        CRect rectExt (0, 0, 0, 0);
        BOOL bExtended    = FALSE;
        BOOL bBottomFrame = FALSE;
        BOOL bIsStatusBar = FALSE;

        CWnd* pStatusBar = pWnd->GetDescendantWindow (AFX_IDW_STATUS_BAR, TRUE);

        if (pStatusBar->GetSafeHwnd () != NULL && pStatusBar->IsWindowVisible ())
        {
            CMFCStatusBar* pClassicStatusBar = DYNAMIC_DOWNCAST (
                CMFCStatusBar, pStatusBar);
            if (pClassicStatusBar != NULL)
            {
                bExtended = pClassicStatusBar->GetExtendedArea (rectExt);
                bIsStatusBar = TRUE;
            }
            else
            {
                CMFCRibbonStatusBar* pRibbonStatusBar = DYNAMIC_DOWNCAST (
                    CMFCRibbonStatusBar, pStatusBar);
                if (pRibbonStatusBar != NULL)
                {
                    bExtended    = pRibbonStatusBar->GetExtendedArea (rectExt);
                    bBottomFrame = pRibbonStatusBar->IsBottomFrame ();
                    bIsStatusBar = TRUE;
                }
            }
        }

        if (bIsStatusBar)
        {
            CRect rectStatus;
            pStatusBar->GetClientRect (rectStatus);

            int nHeight = rectStatus.Height ();
            rectStatus.bottom = rtWindow.bottom;
            rectStatus.top    = rectStatus.bottom - nHeight - (bBottomFrame ? -1 : szSysBorder.cy);
            rectStatus.left   = rtWindow.left;
            rectStatus.right  = rtWindow.right;

            if (bExtended)
            {
                rectExt.left   = rectStatus.right - rectExt.Width () - szSysBorder.cx;
                rectExt.top    = rectStatus.top;
                rectExt.bottom = rectStatus.bottom;
                rectExt.right  = rtWindow.right;
            }

            m_ctrlStatusBarBack.Draw (&dc, rectStatus, bActive ? 0 : 1);

            if (bExtended)
            {
                rectExt.left -= m_ctrlStatusBarBack_Ext.GetParams ().m_rectCorners.left;

                m_ctrlStatusBarBack_Ext.Draw (&dc, rectExt, bActive ? 0 : 1);
            }
        }

        dc.SelectClipRgn (NULL);

        return TRUE;
    }
    return OnNcPaintFallback (pWnd, lstSysButtons, rectRedraw);
}

BOOL CControllerVisualManager::OnNcPaintFallback(CWnd* pWnd, const CObList& lstSysButtons, CRect rectRedraw)
{
    return TRUE;
}

BOOL CControllerVisualManager::OnNcActivate(CWnd* pWnd, BOOL bActive)
{
    ASSERT_VALID (pWnd);

    if (pWnd->GetSafeHwnd () == NULL)
    {
        return FALSE;
    }

    if (afxGlobalData.DwmIsCompositionEnabled ())
    {
        return FALSE;
    }

    // stay active if WF_STAYACTIVE bit is on
    if (pWnd->m_nFlags & WF_STAYACTIVE)
    {
        bActive = TRUE;
    }

    // but do not stay active if the window is disabled
    if (!pWnd->IsWindowEnabled())
    {
        bActive = FALSE;
    }

    m_ActivateFlag[pWnd->GetSafeHwnd ()] = bActive;
    pWnd->SendMessage (WM_NCPAINT, 0, 0);

    return TRUE;
}

BOOL CControllerVisualManager::OnUpdateNcButtons(CWnd* pWnd, const CObList& lstSysButtons, CRect rectCaption)
{
    return FALSE;
}




#endif
