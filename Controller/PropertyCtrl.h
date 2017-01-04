#pragma once
#include "LayoutSettings.h"
#include "FocusEx.h"
#include "layoutElement.h"
#include "layerElement.h"
// CPropertyCtrl

class CPropertyLayout;
class CPropertyLayer;
class CPropertyBasicContent;
class CPropertyTransition;
class CPropertyMessage;
class CPropertyPlugin;
class CPropertySimplePlugin;
class CLayoutDesignerCtrl;

class CPropertyCtrl : public CWnd
{
	DECLARE_DYNAMIC(CPropertyCtrl)

public:
    CPropertyCtrl();
    virtual ~CPropertyCtrl();

    VOID SetLayout(LayoutElement *layoutInfo, LAYOUTGROUP_LIST *pLayoutGroup, const CS3DrawObjList* pObjList);
    VOID SetLayer(std::shared_ptr<LayerElement> layerInfo, CS3DrawObj *pDrawObj);
    VOID SetLayoutDuration(DWORD duration);
    VOID Clear();

    // check whether transition in plus out duration shorter than media content's duration
    bool CheckTransitionTime();

    //LAYOUT *m_pLayout;
    //LAYERINFO *m_pLayer;
	LayoutElement* m_layoutInfo;
	std::shared_ptr<LayerElement> m_layerInfo;
    LAYOUTGROUP_LIST *m_pLayoutGroup;
    CS3DrawObj *m_pDrawObj;

protected:
	DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg LRESULT OnMediaChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTextSettingChanged(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLayerTimeChanged(WPARAM wParam, LPARAM lParam); 

    VOID ClearControlList();
    VOID AddLayoutControl(LayoutElement* layoutInfo, LAYOUTGROUP_LIST* pLayoutGroup, const CS3DrawObjList* pObjList);
    VOID AddLayerControl(std::shared_ptr<LayerElement> layerInfo, CS3DrawObj *pDrawObj);
    VOID AddBasicContentControl(std::shared_ptr<MediaElement> mediaInfo);
    VOID AddMessageControl(std::shared_ptr<S3SIGNAGE_TEXT_SETTING> &MessageSetting);
    VOID AddTransitionControl(S3SIGNAGE_TRANSIION *pTransition, BOOL inOut);
    VOID AddPluginControl(std::shared_ptr<IS3RODesigner2>& pDesigner2);
    VOID AddBasicPluginControl(std::shared_ptr<IS3RODesigner>& pDesigner,CS3DrawObj *pDrawObj);
    VOID UpdateLayerControl();
    VOID UpdateLayoutControl(const CS3DrawObjList* pObjList);

    VOID CalcLayout();

    CArray<CDialogEx *,CDialogEx*> m_ControlList;
    int m_TotalHeight;
    int m_StartY;
public:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

    CPropertyLayout*        m_pPropertyLayout;
    CPropertyLayer*         m_pPropertyLayer;
    CPropertyBasicContent*  m_pPropertyBasicContent;
    CPropertyMessage*       m_pPropertyMessage;
    CPropertyTransition*    m_pPropertyTransitionIn;
    CPropertyTransition*    m_pPropertyTransitionOut;
    CPropertyPlugin*        m_pPropertyPlugin;
    CPropertySimplePlugin*  m_pPropertySimplePlugin;

    HWND GetNextFocusWindow(HWND hWnd, BOOL shift = FALSE);
};


