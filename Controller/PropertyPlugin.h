#pragma once
#include "CommonLib/RenderableObject.h"
#include "FocusEx.h"

extern UINT WM_ON_PLUGIN_THUMB_CHANGE;

typedef struct PROPERTY_ENTRY
{
    RO_PROPERTY_INFO PropertyInfo;
    CWnd            *pPropertyControl;
    CStatic         *pPropertyName;
    DWORD             ControlID;
}PROPERTY_ENTRY;
// CPropertyPlugin dialog

class CPropertyPlugin : public CDialogEx
{
	DECLARE_DYNAMIC(CPropertyPlugin)

public:
	CPropertyPlugin(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPropertyPlugin();
    VOID SetContent(std::shared_ptr<IS3RODesigner2>& pDesigner2);

    std::shared_ptr<IS3RODesigner2> m_pDesigner;

// Dialog Data
	enum { IDD = IDD_PROPERTY_PLUGIN };
    CArray<PROPERTY_ENTRY, PROPERTY_ENTRY &> m_ControlList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnChangeProperty(UINT uID);
    virtual void OnOK(){}
    virtual void OnCancel(){}
	DECLARE_MESSAGE_MAP()

    void CleanControls();
    VOID CalcControlSize(int YPos, CRect &ControlRect, CRect &NameRect);
    VOID CalcControlSize(int YPos, CRect &ControlRect);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    static HWND GetFocusableWindow(int nPosition, LPVOID lParam, BOOL shift);
    CFocusEx m_focus;

};
