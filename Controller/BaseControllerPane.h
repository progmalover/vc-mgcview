#pragma once


// CBaseControllerPane

class CBaseControllerPane : public CDockablePane
{
    DECLARE_DYNAMIC(CBaseControllerPane)

public:
    CBaseControllerPane();

    void SetMainWnd(CWnd *pWnd);
    virtual void DrawCaption(CDC* pDC, CRect rectCaption);

protected:
    DECLARE_MESSAGE_MAP()
    CWnd                    *m_pMainWnd;
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


