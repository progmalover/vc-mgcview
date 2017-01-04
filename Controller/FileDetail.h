#pragma once
#include "afxpropertygridctrl.h"   
#include "InfoDisplayCtrl.h"
#include "afxwin.h"


// CFileDetail 对话框


class CFileDetail : public CDialogEx
{
	DECLARE_DYNAMIC(CFileDetail)

public:
	CFileDetail(const Media& MediaInfo, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFileDetail();

// 对话框数据
	enum { IDD = IDD_FILE_DETAIL_INFO };

    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);

    BOOL SetMediaInfo(const Media& MediaInfo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    BOOL SetMediaThumbnail(const CImage& Thumbnail);

    DECLARE_EASYSIZE
	DECLARE_MESSAGE_MAP()

    CInfoDisplayCtrl    m_mediaInfo;

    vector< Media >     m_MediaInfoStruct;
public:

    CStatic             m_MediaTumbnailCtrl;

    CImage              m_ThumbnailImage;
    CEdit               m_DetailInfoCtrl;
};
