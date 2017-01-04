#pragma once
#include "LayoutSettings.h"
#include "afxwin.h"
#include "MediaElement.h"
#include "layoutElement.h"
//add by sophia
#include "CommonUI/BaseEdit.h"
#include "CommonUI/UIntRangeEdit.h"

extern UINT WM_ON_LAYOUT_SIZE_CHANGE;
extern UINT WM_ON_GROUP_CHANGE;

extern UINT WM_ON_LAYER_SEL_CHANGE;


class CLayoutDesignerCtrl;
// CPropertyLayout dialog

class CPropertyLayout : public CDialogEx
{
    DECLARE_DYNAMIC(CPropertyLayout)
    DECLARE_EASYSIZE

    enum VALUE_CHECK_TYPE
    {
        VALUE_CHECK_TYPE_W,
        VALUE_CHECK_TYPE_H
    };
public:
    CPropertyLayout(CWnd* pParent = NULL);   // standard constructor
    virtual ~CPropertyLayout();

    void SetLayerList(const CS3DrawObjList* pObjList);
    VOID SetContent(LayoutElement* layoutInfo, LAYOUTGROUP_LIST* pLayoutGroup);

    void Clear();

// Dialog Data
    enum { IDD = IDD_PROPERTY_LAYOUT };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnOK(){}
    virtual void OnCancel(){}

    void CheckInputValue(VALUE_CHECK_TYPE type, float ratio);
public:
    afx_msg void OnEnChangeEditName();
    afx_msg void OnCbnSelchangeComboGroup();
    afx_msg void OnEnChangeEditLdesc();
    afx_msg void OnBnClickedButtonChooseClient();
    afx_msg void OnEnChangeEditWidth();
    afx_msg void OnEnChangeEditHeight();
    afx_msg void OnBnClickedCheckKeepAspact();
    afx_msg void OnCbnSelchangeComboSize();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnBnClickedMfccolorbuttonBg();
    afx_msg void OnUpdateBGBtnCommandUI(CCmdUI *pCmd);
    afx_msg LRESULT CPropertyLayout::OnIdleUpdateUI(WPARAM ,LPARAM);
    afx_msg LRESULT OnGroupChange(WPARAM wParam, LPARAM lParam);

    CS3DrawObjList m_LayerList;
    //LAYOUT *m_pLayout;
	LayoutElement* m_layoutInfo;
    LAYOUTGROUP_LIST* m_pLayoutGroup;
    float m_fAspectRatio;

    BOOL m_bOpenClient;
    bool m_bInChangeSize;
    CComboBox m_wndPresetSize;
    CListBox m_wndLayerList;
	//add by sophia
	FilterEdit::CBaseEdit  m_layoutName;
	FilterEdit::CUIntRangeEdit m_EditWidth;
	FilterEdit::CUIntRangeEdit m_EditHeight;

    afx_msg void OnBnClickedEditGroup();
    afx_msg void OnLbnSelchangeListLayers();

    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedStaticLayerList();
};
