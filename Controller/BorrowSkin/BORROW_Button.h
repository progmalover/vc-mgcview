#pragma once


// CSkinButton

class CBorrow_Button : public CMFCButton
{
    friend class CControllerVisualManager;

	DECLARE_DYNAMIC(CBorrow_Button)

public:
	CBorrow_Button();
	virtual ~CBorrow_Button();

    virtual void DoDrawItem(CDC* pDCPaint, CRect rectClient, UINT itemState);
    virtual BOOL IsDefaultButton();

    void MyInitStyle (DWORD dwStyle);
    BOOL			m_bVisualManagerStyle;

protected:
	DECLARE_MESSAGE_MAP()
    virtual void OnDraw(CDC* pDC, const CRect& rect, UINT uiState);
    afx_msg void OnPaint();

    virtual void PreSubclassWindow();

    COLORREF m_clrText;
    BOOL m_bDefaultButton;
};


