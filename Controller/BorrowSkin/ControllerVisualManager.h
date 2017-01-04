#pragma once

// CControllerVisualManager command target

class CBORROW_SliderCtrl;
class CBORROW_ScrollBar;
class CBORROW_Group;
class CBORROW_DateTimeCtrl;
class CBORROW_Calendar;
class CBORROW_CalendarColors;
class CBorrow_Button;
class CBCGPGridCtrl;
class CBCGPToolTipCtrl;
struct BCGP_GRID_COLOR_DATA;

#define visualManager ((CControllerVisualManager*)CMFCVisualManager::GetInstance())


class CControllerVisualManager : public CMFCVisualManagerOffice2007
{
	DECLARE_DYNCREATE(CControllerVisualManager)

public:
	CControllerVisualManager();

    virtual void OnUpdateSystemColors();

    virtual BOOL IsOwnerDrawCaption() { return FALSE; }
    virtual void CleanUp();

    //------------------------------------------------
    // slider control
    // draw slider control channel
    virtual void OnDrawSliderChannel(CDC* pDC, CBORROW_SliderCtrl* pSlider, 
        BOOL bVert, CRect rect, BOOL bDrawOnGlass);
    virtual void OnDrawSliderChannelFallback(CDC* pDC,
        CBORROW_SliderCtrl* pSlider, BOOL bVert, CRect rect, BOOL bDrawOnGlass);

    // draw slider control thumb
    virtual void OnDrawSliderThumb(CDC* pDC, CBORROW_SliderCtrl* pSlider, 
        CRect rect, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled,
        BOOL bVert, BOOL bLeftTop, BOOL bRightBottom,
        BOOL bDrawOnGlass);
    virtual void OnDrawSliderThumbFallback(CDC* pDC, CBORROW_SliderCtrl* pSlider, 
        CRect rect, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled,
        BOOL bVert, BOOL bLeftTop, BOOL bRightBottom,
        BOOL bDrawOnGlass);

    //------------------------------------------------
    // scroll bar control
    virtual BOOL IsOwnerDrawScrollBar() const
    {
        return CanDrawImage ();
    }
    virtual void OnScrollBarDrawThumb(CDC* pDC, CBORROW_ScrollBar* pScrollBar, CRect rect, 
        BOOL bHorz, BOOL bHighlighted, BOOL bPressed, BOOL bDisabled);

    virtual void OnScrollBarDrawButton(CDC* pDC, CBORROW_ScrollBar* pScrollBar, CRect rect, 
        BOOL bHorz, BOOL bHighlighted, BOOL bPressed, BOOL bFirst, BOOL bDisabled);

    virtual void OnScrollBarFillBackground(CDC* pDC, CBORROW_ScrollBar* pScrollBar, CRect rect, 
        BOOL bHorz, BOOL bHighlighted, BOOL bPressed, BOOL bFirst, BOOL bDisabled);

    //------------------------------------------------
    // group control
    void OnDrawGroup(CDC* pDC, CBORROW_Group* pGroup, CRect rect, const CString& strName);
    void OnDrawGroupFallback(CDC* pDC, CBORROW_Group* pGroup, CRect rect, const CString& strName);

    //------------------------------------------------
    // calendar control
    virtual void OnDrawDateTimeDropButton (CDC* pDC, CRect rect, BOOL bDisabled, BOOL bPressed, BOOL bHighlighted, CBORROW_DateTimeCtrl* pCtrl);
    virtual void OnDrawDateTimeDropButtonFallback (CDC* pDC, CRect rect, BOOL bDisabled, BOOL bPressed, BOOL bHighlighted, CBORROW_DateTimeCtrl* pCtrl);
    virtual void GetCalendarColors (const CBORROW_Calendar* pCalendar, CBORROW_CalendarColors& colors);

    //------------------------------------------------
    // button
    virtual CSize GetCheckRadioDefaultSize();
    virtual CSize GetCheckRadioDefaultSizeFallback();
    virtual void OnDrawCheckBoxEx (CDC *pDC, CRect rect, 
        int nState,
        BOOL bHighlighted, 
        BOOL bPressed,
        BOOL bEnabled);

    virtual void OnDrawRadioButton (CDC *pDC, CRect rect, 
        BOOL bOn,
        BOOL bHighlighted, 
        BOOL bPressed,
        BOOL bEnabled);

    virtual BOOL OnDrawPushButton (CDC* pDC, CRect rect, CBorrow_Button* pButton, COLORREF& clrText);
    virtual BOOL OnDrawPushButtonFallback (CDC* pDC, CRect rect, CBorrow_Button* pButton, COLORREF& clrText);

    virtual BOOL OnFillDialog (CDC* pDC, CWnd* pDlg, CRect rect);
    virtual BOOL OnFillDialogFallback (CDC* pDC, CWnd* pDlg, CRect rect);

    virtual CBrush& GetDlgBackBrush (CWnd* pDlg)
    {
        return m_brDlgBackground;
    }

    virtual void SetToolTipInfo(const CMFCToolTipInfo& info);
    //------------------------------------------------
    // Grid
    virtual void OnFillGridBackground (CDC* pDC, CRect rect);
    virtual void OnDrawGridExpandingBox (CDC* pDC, CRect rect, BOOL bIsOpened, COLORREF colorBox);
    virtual void OnFillGridHeaderBackground (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect);
    virtual BOOL OnDrawGridHeaderItemBorder (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect, BOOL bPressed);
    virtual void OnFillGridRowHeaderBackground (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect);
    virtual BOOL OnDrawGridRowHeaderItemBorder (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect, BOOL bPressed);
    virtual void OnFillGridSelectAllAreaBackground (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect, BOOL bPressed);
    virtual BOOL OnDrawGridSelectAllAreaBorder (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect, BOOL bPressed);
    virtual void OnDrawGridSortArrow (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rectArrow, BOOL bIsUp);
    virtual COLORREF OnFillGridGroupByBoxBackground (CDC* pDC, CRect rect);
    virtual COLORREF OnFillGridGroupByBoxTitleBackground (CDC* pDC, CRect rect);
    virtual COLORREF GetGridGroupByBoxLineColor () const;
    virtual void OnDrawGridGroupByBoxItemBorder (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect);
    virtual COLORREF GetGridLeftOffsetColor (CBCGPGridCtrl* pCtrl);
    virtual COLORREF GetGridItemSortedColor (CBCGPGridCtrl* pCtrl);
    virtual void OnFillGridGroupBackground (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rectFill);
    virtual void OnDrawGridGroupUnderline (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rectFill);
    virtual COLORREF OnFillGridRowBackground (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rectFill, BOOL bSelected);
    virtual COLORREF OnFillGridItem (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rectFill,
        BOOL bSelected, BOOL bActiveItem, BOOL bSortedColumn);
    virtual void OnDrawGridHeaderMenuButton (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect, 
        BOOL bHighlighted, BOOL bPressed, BOOL bDisabled);
    virtual void OnDrawGridSelectionBorder (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect);

    virtual BOOL OnSetGridColorTheme (CBCGPGridCtrl* pCtrl, BCGP_GRID_COLOR_DATA& theme);
    // Report control:
    virtual COLORREF GetReportCtrlGroupBackgoundColor ();
    virtual COLORREF OnFillReportCtrlRowBackground (CBCGPGridCtrl* pCtrl, CDC* pDC,
        CRect rectFill, BOOL bSelected, BOOL bGroup);

    // ToolTip
    virtual BOOL GetToolTipParams (CMFCToolTipInfo& params, UINT nType = (UINT)(-1));
    virtual void OnFillToolTip (CDC* pDC, CBCGPToolTipCtrl* pToolTip, CRect rect, COLORREF& clrText, COLORREF& clrLine);


    //------------------------------------------------
    // dialog
#if 0
    virtual BOOL OnNcPaint (CWnd* pWnd, const CObList& lstSysButtons, CRect rectRedraw);
    virtual BOOL OnNcPaintFallback (CWnd* pWnd, const CObList& lstSysButtons, CRect rectRedraw);
    
    virtual BOOL OnUpdateNcButtons (CWnd* pWnd, const CObList& lstSysButtons, CRect rectCaption);

    virtual BOOL OnNcActivate (CWnd* pWnd, BOOL bActive);

    virtual CBrush& GetDlgBackBrush (CWnd* pDlg)
    {
        return m_brDlgBackground;
    }
#endif
protected:
    virtual BOOL ParseStyleXMLGlobal(const CString& strItem);
    virtual BOOL ParseStyleXMLMainWnd(const CString& strItem);
    virtual BOOL ParseStyleXMLSlider(const CString& strItem);
    virtual BOOL ParseStyleXMLScrollBar(const CString& strItem);
    virtual BOOL ParseStyleXMLButtonGroup(const CString& strItem);

    CMFCToolBarImages   m_SysBtnHelp[2];

    CMFCControlRenderer	m_SysBtnBackC[2];

    CMFCControlRenderer	m_ctrlSliderThumb[6];

    CMFCControlRenderer	m_ctrlScrollBar_Back[2][2];
    CMFCControlRenderer	m_ctrlScrollBar_Item[2][2];
    CMFCControlRenderer	m_ctrlScrollBar_ThumbBack[2][2];
    CMFCControlRenderer	m_ctrlScrollBar_ThumbIcon[2][2];

    CMFCControlRenderer	m_ctrlRibbonBtnRadio;
    CMFCControlRenderer	m_ctrlRibbonBtnPush;
    CMFCControlRenderer	m_ctrlRibbonBtnGroup;

    COLORREF	m_clrReportGroupText;

    // dialog
    CMFCControlRenderer m_ctrlDialogBorder;
    COLORREF			m_clrDlgBackground;
    CBrush				m_brDlgBackground;

    HTHEME				m_hThemeToolTip;
};


