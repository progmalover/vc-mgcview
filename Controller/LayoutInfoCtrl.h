#pragma once
#include "resource.h"
#include "Controller.h"
#include "InfoDisplayCtrl.h"
// CLayoutInfoCtrl dialog

class CLayoutInfoCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CLayoutInfoCtrl)
    DECLARE_EASYSIZE

public:
	CLayoutInfoCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLayoutInfoCtrl();

    BOOL    SetMediaInfo(const Layout& LayoutInfo);
    BOOL    SetMediaThumbnail(const CImage& Thumbnail);

    BOOL    SetCategoryInfo(const LayoutGroup& CategoryInfo);

    BOOL    ClearInfo();

// Dialog Data
	enum { IDD = IDD_MEDIA_LIB_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnOK() {}
    afx_msg void OnCancel() {}



	DECLARE_MESSAGE_MAP()



public:
    CInfoDisplayCtrl     m_mediaInfo;

    CStatic     m_thumbnail;

    CString     m_szInfo;
    CImage      m_ThumbnailImage;


    CString     m_szRejectReason;
    CButton     m_ctrlRejectReason;
    afx_msg void OnBnClickedBtnRejectReason();
    CRect       m_thumbRect;
};
