#pragma once

/// CBaseControllerPage
/// Container to bind the ribbon category and the client window

extern UINT CLOSE_CURRENT_PAGE;

typedef struct _PANE_POS
{
    UINT    Width;              // The Pane's default width percentage in the main frame client area
    UINT    Height;             // The Pane's default height percentage in the main frame client area
    UINT    MinWidth;           // The Pane's minimal width percentage in the main frame client area during sizing
    UINT    MinHeight;          // The Pane's minimal heighe percentage in the main frame client area during sizing
} PANE_POS;
typedef struct _PANE_POS *PPANE_POS;

// wParam - not used, lParam - points to string that use to be set to the state bar (TCHAR*)
extern UINT MAIN_APP_STATE_BAR_INFO;

class CBaseControllerPage : public CFrameWndEx
{
    DECLARE_DYNAMIC(CBaseControllerPage)

public:
    CBaseControllerPage();
    virtual ~CBaseControllerPage();

    // Close the main window
    virtual void Close() = 0;

    // Bind the ribbon category
    virtual void BindRibbonCategory(CMFCRibbonCategory* pCategory) { m_pCategory = pCategory; };

    // Create the ribbon buttons
    virtual BOOL CreateRibbonButtons() = 0;

    virtual void OnUpdateRibbonBtnPreview(CCmdUI* pCmdUI) { };

    virtual BOOL IsEnable(){return m_bEnable;}
    virtual void SetEnable(BOOL bEnable = TRUE)
    {
        m_bEnable = bEnable;
        EnableWindow(bEnable);
    }

    virtual BOOL RefreshPage(){return TRUE;}

protected:
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnNcPaint();
    DECLARE_MESSAGE_MAP()

    CWnd                    *m_pMainWnd;
    CMFCRibbonCategory      *m_pCategory;

    void                    SetMainWnd(CWnd *pWnd);

    virtual void AdjustClientArea();

    BOOL                    m_bEnable;
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


