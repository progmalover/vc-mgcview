#pragma once
#include "LayoutSettings.h"
#include "FocusEx.h"
#include "PropertySubCmds.h"
//add by sophia
#include "CommonUI/BaseEdit.h"
// CPropertyMessage dialog
 using namespace LAYOUTPROPERTYCMDS;
class CPropertyMessage : public CDialogEx
{
    DECLARE_DYNAMIC(CPropertyMessage)
    DECLARE_EASYSIZE
    DELARE_FRIEND_PROPERTY_CMDS()
public:
    CPropertyMessage(CWnd* pParent = NULL);   // standard constructor
    virtual ~CPropertyMessage();

    VOID SetContent(std::shared_ptr<S3SIGNAGE_TEXT_SETTING> &MessageSetting);
    void UpdateContent();
    void UpdateFont(bool isSizeEdit = true);

    void Clear();

    // Dialog Data
    enum { IDD = IDD_PROPERTY_MESSAGE };

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
    }	DECLARE_MESSAGE_MAP()
        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    std::shared_ptr<S3SIGNAGE_TEXT_SETTING> m_pMessage;
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    static HWND GetFocusableWindow(int nPosition, LPVOID lParam, BOOL shift);
    CFocusEx m_focus;

public:
    CString m_strMessage;
    //    int m_nFontSize;
    CMFCColorButton m_wndFontColor;
    BOOL m_bFontBold;
    BOOL m_bFontItalic;
    BOOL m_bFontUnderline;
    CMFCFontComboBox m_wndFont;
    int m_nSpeed;
	CSliderCtrl m_ctrlSpeed;
	//     int m_nTransparency;
    CString m_strFontSize;
    CComboBox m_wndDirection;
    CComboBox m_wndAlignment;
    CComboBox m_wndLayout;
    afx_msg void OnChangeEditMessage();
    //    afx_msg void OnEditchangeMfcfontcomboText();
    afx_msg void OnEditchangeComboSize();
    afx_msg void OnSelchangeComboAlignment();
    afx_msg void OnSelchangeComboDirection();
    afx_msg void OnSelchangeComboLayout();
    afx_msg void OnClickedCheckBold();
    afx_msg void OnClickedCheckItalic();
    afx_msg void OnClickedCheckUnderline();
    afx_msg void OnSelchangeMfcfontcomboText();
    afx_msg void OnClickedMfccolorbuttonText();
    afx_msg void OnSelchangeComboSize();
    afx_msg void OnReleasedcaptureSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult);
//     afx_msg void OnReleasedcaptureSliderTransparency(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_fontDirection;
	afx_msg void OnCbnSelchangeComboFontdirection();
	afx_msg void SetActualRunTime(CString str);

	FilterEdit::CBaseEdit  m_strMsg;
private:
	void UpdateTextDirCtrl();
    bool IsMsgLenValid();
private:
	BOOL m_bVerticalText;
};
