#pragma once

#include "LayoutDesignerCtrl.h"
#include "RulerCtrl.h"
// CLayoutDesignerPane

class CLayoutDesignerPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CLayoutDesignerPane)

public:
    CLayoutDesignerPane();
    virtual ~CLayoutDesignerPane();


public:
    CLayoutDesignerCtrl  m_wndDesign;
	CRulerCtrl			 m_Ruler;
	BOOL				 m_bEnableCaption;
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};


