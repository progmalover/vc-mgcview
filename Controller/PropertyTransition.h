#pragma once
#include "LayoutSettings.h"
#include "FocusEx.h"
#include "propertysubcmds.h"
// CPropertyTransition dialog
using namespace LAYOUTPROPERTYCMDS;
class CPropertyTransition : public CDialogEx
{
    DECLARE_DYNAMIC(CPropertyTransition)
    DECLARE_EASYSIZE
    DELARE_FRIEND_PROPERTY_CMDS()
public:
    CPropertyTransition(CWnd* pParent = NULL, BOOL inOut = TRUE);   // standard constructor
    virtual ~CPropertyTransition();

    VOID SetContent(S3SIGNAGE_TRANSIION *pTransition);
    void UpdateContent();

    void Clear();
    void UpdateEnd();
    void UpdateBegin();
    void ChangeEffect();

    typedef struct {
        const S3S_CONTENT_EFFECT_DIRECTION* directions;
        size_t directionCount;
    } S3S_CONTENT_EFFECT_FILTER;

    S3S_CONTENT_EFFECT_FILTER m_filters[S3S_EFFECT_TOTAL];

    // Dialog Data
    enum { IDD = IDD_PROPERTY_TRANSITION };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual void OnOK(){}
    virtual void OnCancel()
    {
#ifdef DEBUG
        UpdateContent();
#endif
    }
    DECLARE_MESSAGE_MAP()
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    S3SIGNAGE_TRANSIION *m_pTransition;
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    static HWND GetFocusableWindow(int nPosition, LPVOID lParam, BOOL shift);
    CFocusEx m_focus;

public:
    //    int m_nDirection;
    COleDateTime m_duration;
    CComboBox m_wndDirection;
    CComboBox m_wndEffect;
    BOOL m_bInOut;
    CString m_strCurEffect;
    afx_msg void OnSelchangeComboDirection();
    afx_msg void OnSelchangeComboType();
    afx_msg void OnDtnDatetimechangeDatetimepickerDuration(NMHDR *pNMHDR, LRESULT *pResult);
};
