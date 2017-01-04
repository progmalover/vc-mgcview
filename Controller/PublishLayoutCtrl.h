#pragma once
#include "afxcmn.h"
#include "LayoutSettings.h"
#include "SimpleBrowser.h"


// CPublishLayoutCtrl dialog

class CPublishLayoutCtrl : public CDialogEx
{
    DECLARE_DYNAMIC(CPublishLayoutCtrl)
    DECLARE_EASYSIZE
public:
    CPublishLayoutCtrl(CWnd* pParent = NULL);   // standard constructor
    virtual ~CPublishLayoutCtrl();

    // Dialog Data
    enum { IDD = IDD_LAYOUT_PUBLISH };

#if 0
    HRESULT GenerateLayoutInfo(LayoutElement* pLayout, CString& szInfo);
    HRESULT UpdateLayoutFile(LayoutElement* pLayout, CString& szInfo);
#endif 

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);

    DECLARE_MESSAGE_MAP()

public:

    CWnd*                       m_pParent;
    SimpleBrowser               m_Browser;
    CString                     m_LayoutInfo;
    CString                     m_SummryInfo;
    CString                     m_ErrorInfo;
    DWORD64                     m_nLayoutSize;
    DWORD                       m_nContents;
    CMap<CString, LPCTSTR, CString, CString&>   m_MediaFiles;
    CStringArray                m_arrErrorFiles;
};