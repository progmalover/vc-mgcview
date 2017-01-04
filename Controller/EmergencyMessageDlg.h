#pragma once
#include "afxwin.h"
#include "atlcomtime.h"
#include "afxfontcombobox.h"
#include "afxcmn.h"
#include "afxdtctl.h"
#include "afxcolorbutton.h"

// CScheduleItemCtrl dialog

#define NEW_MSG_INDEX (0xabcdef)
class EmergencyMessageDlg : public CDialogEx
{
    DECLARE_DYNAMIC(EmergencyMessageDlg)
    DECLARE_EASYSIZE
public:
    EmergencyMessageDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~EmergencyMessageDlg();

    BOOL SetMsgHistory(std::vector<EmergencyMessage>& history);

    BOOL GetNewMsg(EmergencyMessage& Msg){Msg = m_newEmessage; return TRUE;}

// Dialog Data
    enum { IDD = IDD_EMERGENCY_MESSAGE };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);


    DECLARE_MESSAGE_MAP()
public:

private:
    BOOL UpdateNewMsg();

    BOOL UpdateCtrls(INT index);

	LRESULT CalculateDuration(COleDateTimeSpan &timeSpan, BOOL bEditFont = TRUE);

	void UpdateDuration(BOOL bEditFont = TRUE);
public:



    CString m_MessageContent;
    COleDateTime m_Date;
    COleDateTime m_StartTime;
    COleDateTime m_Duration;
    CMFCFontComboBox m_ctrlFont;
    COLORREF m_Color;
    int m_SizeIndex;
	int m_Size;
    BOOL m_bBold;
    BOOL m_bItalic;
    BOOL m_bUnderline;
    CString m_strDirection;
    int m_Speed;
    int m_Transparency;
//	int m_ShowTimes;
    CComboBox m_ctrlFontSize;
    CComboBox m_ctrlDirection;
    CSliderCtrl m_ctrlSpeed;
    CSliderCtrl m_ctrlTransparency;
    CString m_Font;
    afx_msg void OnBnClickedOk();
	CMFCColorButton m_ColorControl;

    INT m_curMsgIndex; 
    EmergencyMessage m_newEmessage;
    std::vector<EmergencyMessage> m_MsgHistory;
    CListCtrl m_ctrlListMsgHistory;
    afx_msg void OnLvnItemchangedListMsgHistory(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDtnDatetimechangeEmgmDuration(NMHDR *pNMHDR, LRESULT *pResult);
    CDateTimeCtrl m_ctrlDuration;

    bool m_bInit;
    afx_msg void OnCbnEditchangeComboSize();
	afx_msg void OnBnClickedStop();
	afx_msg void OnSelchangeComboDirection();
//	afx_msg void OnEnKillfocusTimes();
	afx_msg void OnEnChangeTimes();
	afx_msg void OnEnChangeEmgmText();
	afx_msg void OnCbnSelchangeEmgmFont();
	afx_msg void OnCbnSelchangeComboSize();
	afx_msg void OnBnClickedCheckBold();
	afx_msg void OnBnClickedCheckItalic();
//	afx_msg void OnTRBNThumbPosChangingSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMReleasedcaptureSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckUnderline();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString m_ShowTimes;
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
