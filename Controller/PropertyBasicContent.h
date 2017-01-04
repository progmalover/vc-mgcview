#pragma once
#include "LayoutSettings.h"
#include "afxcmn.h"
#include "FocusEx.h"

#include "MediaElement.h"
#include "propertysubcmds.h"

// CPropertyBasicContent dialog
extern UINT WM_ON_TIME_CHANGE;
using namespace LAYOUTPROPERTYCMDS;
class CPropertyBasicContent : public CDialogEx
{
    DECLARE_DYNAMIC(CPropertyBasicContent)
    DECLARE_EASYSIZE
    DELARE_FRIEND_PROPERTY_CMDS();
public:
    CPropertyBasicContent(CWnd* pParent = NULL);   // standard constructor
    virtual ~CPropertyBasicContent();
    VOID    SetContent(std::shared_ptr<MediaElement> MediaInfo);
    void UpdateContent();
    void UpdateBegin();
    void UpdateEnd();
    void CheckDuration();
	void CheckRefreshInterval();
    void Clear();

    // Dialog Data
    enum { IDD = IDD_PROPERTY_BASIC_CONTENT };

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

    //MEDIASETTING *m_pMedia;
	std::shared_ptr<MediaElement> m_MediaInfo;
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    static HWND GetFocusableWindow(int nPosition, LPVOID lParam, BOOL shift);
    CFocusEx m_focus;

public:
    //    afx_msg void OnHotitemchangeMfccolorbuttonBg(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnClickedMfccolorbuttonBg();
    afx_msg void OnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult);
    CMFCColorButton m_wndColorBG;
    COleDateTime m_time;

    CSliderCtrl m_wndVolume;
    afx_msg void OnNMReleasedcaptureSliderVolume(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedCheckKeepAspectRatio();
    afx_msg void OnBnClickedButtonDetail();
    CSliderCtrl m_ctlTransparency;
    DWORD m_nTransparency;
    afx_msg void OnNMReleasedcaptureSliderTransparency(NMHDR *pNMHDR, LRESULT *pResult);
	COleDateTime m_refreshTime;
	afx_msg void OnDtnDatetimechangeRefreshDatetimepicker(NMHDR *pNMHDR, LRESULT *pResult);
};
