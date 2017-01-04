#pragma once
#include "afxwin.h"
#include "LayoutSettings.h"
#include "LayoutElement.h"

// CGroupEditor 对话框

class CPropertyLayout;

class CGroupEditor : public CDialogEx
{
	DECLARE_DYNAMIC(CGroupEditor)

public:
	CGroupEditor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGroupEditor();

    void SetContent(LayoutElement* m_layoutInfo, LAYOUTGROUP_LIST* groups, CPropertyLayout* p);

    void UpdateGroupList();

// 对话框数据
	enum { IDD = IDD_GROUP_EDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

    LAYOUTGROUP_LIST* m_pLayoutGroup;
    //LAYOUT* m_pLayout;
	LayoutElement* m_layoutInfo;
    CPropertyLayout* m_pPropertyLayout;
    CListBox m_wndGroupList;

    int m_nSelected;
public:
    afx_msg void OnBnClickedButtonNewGroup();
    afx_msg void OnBnClickedButtonDeleteGroup();
    afx_msg void OnLbnSelchangeListGroup();
	CString m_szCategoryDesc;

	std::map<int, CString> m_GroupDescMap;
};
