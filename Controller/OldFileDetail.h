#pragma once
#include "afxpropertygridctrl.h"


// COldFileDetail 对话框

namespace MediaInfoLib
{
    class MediaInfo;
}

class CMyPropertyGridCtrl : public CMFCPropertyGridCtrl
{
public:
    void SetLeftColumnWidth(int n)
    {
        m_nLeftColumnWidth = n;
    }
};

class COldFileDetail : public CDialogEx
{
	DECLARE_DYNAMIC(COldFileDetail)

public:
	COldFileDetail(MediaInfoLib::MediaInfo& mi, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COldFileDetail();

// 对话框数据
	enum { IDD = IDD_FILE_DETAIL };

    virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
    MediaInfoLib::MediaInfo& m_mi;
public:
    CMyPropertyGridCtrl m_wndPropList;
};
