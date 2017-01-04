#pragma once
#include "afxwin.h"
#include "InfoDisplayCtrl.h"


// CMediaLibraryInfoCtrl dialog

class CMediaLibraryInfoCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CMediaLibraryInfoCtrl)
    DECLARE_EASYSIZE
public:
	CMediaLibraryInfoCtrl(CWnd* pParent = NULL);   // standard constructor
    virtual ~CMediaLibraryInfoCtrl();

    BOOL    SetMediaInfo(const Media& MediaInfo);
    BOOL    SetMediaInfo(const TASK_DETAIL_INFO& MediaInfo);
    BOOL    SetMediaThumbnail(const CImage& Thumbnail);

    BOOL    SetCategoryInfo(const MediaFilter& CategoryInfo);

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
protected:


public:
    CInfoDisplayCtrl m_mediaInfo;

    CStatic     m_thumbnail;

    CString     m_szInfo;
    CImage      m_ThumbnailImage;

    CString     m_szRejectReason;
    CButton     m_ctrlRejectReason;
    afx_msg void OnBnClickedBtnRejectReason();

    CRect       m_thumbRect;
};
