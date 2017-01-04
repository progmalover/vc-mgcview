#pragma once

#include "SkinDlg/TranslucentDialog.h"
#include "SkinDlg/TranslucentButton.h"
#include "SkinDlg/TranslucentProgressBar.h"
#include "DemoStandardDlgSetting.h"

// CDemoTranslucentDlgSetting dialog

class CDemoTranslucentDlgSetting : public CTranslucentDialog
{
	//DECLARE_DYNAMIC(CDemoTranslucentDlgSetting)

public:
	CDemoTranslucentDlgSetting(LPCTSTR lpszFile, CWnd* pParent = NULL);   // standard constructor
	CDemoTranslucentDlgSetting(UINT nImgID, LPCTSTR lpszType = _T("PNG"), HINSTANCE hResourceModule = NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDemoTranslucentDlgSetting();

// Dialog Data
	enum { IDD = IDD_TRANSLUCENTDIALOG };

	virtual CStandardDialog* CreateStandardDialog();
	virtual void OnInitChildrenWnds();
    CDemoStandardDlgSetting* GetStandardDialog();

    virtual void GetTitle(CString& s);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

};
