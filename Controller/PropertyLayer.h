#pragma once
#include "LayoutSettings.h"
#include "afxvslistbox.h"
#include "MediaListBox.h"
#include "FocusEx.h"
#include "PropertySimplePlugin.h"
#include "amsEdit.h"
//add by sophia
#include "CommonUI/UIntRangeEdit.h"
#include "CommonUI/IntRangeEdit.h"
#include "CommonUI/BaseEdit.h"
// CPropertyLayer dialog

extern UINT WM_ON_SIZE_POS_CHANGE2;
extern UINT WM_ON_MEDIA_SEL_CHANGE;

extern UINT WM_ON_LAYER_LOOP_CHANGE;
extern UINT WM_ON_LAYER_NAME_CHANGE;
typedef enum SIZE_CHANGING
{
    VALUE_TYPE_X,
    VALUE_TYPE_Y,
    VALUE_TYPE_W,
    VALUE_TYPE_H
} SIZE_CHANGING;

typedef struct POS_CHANGE_INFO
{
    SIZE_CHANGING   ChangeType;
    FRect           rectChanged;
} POS_CHANGE_INFO;


class CPropertyLayer : public CDialogEx
{
    DECLARE_DYNAMIC(CPropertyLayer)
    DECLARE_EASYSIZE

    enum VALUE_CHECK_TYPE
    {
        VALUE_CHECK_TYPE_X,
        VALUE_CHECK_TYPE_Y,
        VALUE_CHECK_TYPE_W,
        VALUE_CHECK_TYPE_H
    };
public:
    CPropertyLayer(CWnd* pParent = NULL);   // standard constructor
    virtual ~CPropertyLayer();

    VOID  SetContent(std::shared_ptr<LayerElement> layerInfo, CS3DrawObj *pDrawObj);
    std::shared_ptr<MediaElement>  GetSelectedMedia();
    VOID  SetDuration(DWORD duration);

    void ChangeLayerDuration();

    void CalcLayerDuration();

    void Clear();

    // Dialog Data
    enum { IDD = IDD_PROPERTY_LAYER };

    virtual BOOL PreTranslateMessage(MSG* pMsg);
    static HWND GetFocusableWindow(int nPosition, LPVOID lParam, BOOL shift);
    CFocusEx m_focus;


protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg LRESULT OnMediaNumChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPlugSizeChange(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    virtual void OnOK(){}
    virtual void OnCancel(){}

    void CheckInputValue(VALUE_CHECK_TYPE type, float ratio);

    //LAYERINFO *m_pLayer;
	std::shared_ptr<LayerElement> m_layerInfo;
    CS3DrawObj *m_pDrawObj;
    bool m_bInChangeSize;
    POS_CHANGE_INFO m_ChangePos;
    DWORD m_Duration;
    DWORD m_LayoutDuration;

//	CAMSNumericEdit m_EditX;
//	CAMSNumericEdit m_EditY;
//	CAMSNumericEdit m_EditWidth;
//	CAMSNumericEdit m_EditHeight;

	//add by sophia
	FilterEdit::CIntRangeEdit m_EditX;
	FilterEdit::CIntRangeEdit m_EditY;
	FilterEdit::CUIntRangeEdit m_EditWidth;
	FilterEdit::CUIntRangeEdit m_EditHeight;

	FilterEdit::CBaseEdit  m_Name;

public:
    afx_msg void OnEnChangeEditName();
    afx_msg void OnEnChangeEditX();
    afx_msg void OnEnChangeEditY();
    afx_msg void OnEnChangeEditWidth();
    afx_msg void OnEnChangeEditHeight();
    afx_msg void OnLvnItemchangedMediaList(NMHDR *pNMHDR, LRESULT *pResult);
    CMediaListBox m_MediaList;
    afx_msg void OnBnClickedCheckKeepAspact();
    afx_msg void OnBnClickedCheckLoopSync();
    BOOL m_bAlign2Grid;
    afx_msg void OnBnClickedCheckAlign2grid();
    afx_msg void OnStnClickedStaticY();
	CString m_szXaxis;
	CString m_szYaxis;
	CString m_szWidth;
	CString m_szHeight;
};
