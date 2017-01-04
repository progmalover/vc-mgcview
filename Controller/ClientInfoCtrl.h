#pragma once
#include "afxwin.h"
#include "Controller.h"
#include "InfoDisplayCtrl.h"

// CClientInfoCtrl dialog

class CClientInfoCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CClientInfoCtrl)
    DECLARE_EASYSIZE
public:
	CClientInfoCtrl(CWnd* pParent = NULL);   // standard constructor
    virtual ~CClientInfoCtrl();     

    BOOL    SetInfo(const ClientInfo& Info, CString szIP = _T(""));
    BOOL    SetThumbnail(const CImage& Thumbnail);

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
    CInfoDisplayCtrl    m_Info;
    CStatic             m_thumbnail;
    CImage              m_ThumbnailImage;
    CImageList          m_LargeImageList;
};
