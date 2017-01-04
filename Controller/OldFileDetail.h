#pragma once
#include "afxpropertygridctrl.h"


// COldFileDetail �Ի���

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
	COldFileDetail(MediaInfoLib::MediaInfo& mi, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~COldFileDetail();

// �Ի�������
	enum { IDD = IDD_FILE_DETAIL };

    virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
    MediaInfoLib::MediaInfo& m_mi;
public:
    CMyPropertyGridCtrl m_wndPropList;
};
