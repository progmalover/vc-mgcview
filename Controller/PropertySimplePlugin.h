#pragma once
#include "CommonLib/RenderableObject.h"
#include "FocusEx.h"
#include "LayoutSettings.h"

extern UINT WM_ON_SIMPLE_PLUGIN_THUMB_CHANGE;
extern UINT WM_ON_SIMPLE_PLUGIN_SIZE_CHANGE;

// CPropertySimplePlugin dialog

class CPropertySimplePlugin : public CDialogEx
{
	DECLARE_DYNAMIC(CPropertySimplePlugin)
    DECLARE_EASYSIZE
public:
	CPropertySimplePlugin(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPropertySimplePlugin();
    VOID SetContent(std::shared_ptr<IS3RODesigner>& pDesigner,CS3DrawObj *pDarwObj);

    std::shared_ptr<IS3RODesigner> m_pDesigner;
	CS3DrawObj *m_pDrawObj;
// Dialog Data
	enum { IDD = IDD_PROPERTY_SIMPLE_PLUGIN };
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    static HWND GetFocusableWindow(int nPosition, LPVOID lParam, BOOL shift);
    CFocusEx m_focus;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnOK(){}
    virtual void OnCancel(){}
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg LRESULT OnTranslateBefore(WPARAM, LPARAM);
    afx_msg LRESULT OnSelJudge(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonConfigure();

private:
    BOOL  m_bSelJudge;
};
