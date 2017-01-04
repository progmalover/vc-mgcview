#pragma once

#include "SkinDlg/TranslucentDialog.h"
#include "SkinDlg/TranslucentButton.h"
#include "SkinDlg/TranslucentProgressBar.h"
#include "DemoStandardDlg.h"

// CDemoTranslucentDlg dialog

class CDemoTranslucentDlg : public CTranslucentDialog
{
	//DECLARE_DYNAMIC(CDemoTranslucentDlg)

public:
	CDemoTranslucentDlg(LPCTSTR lpszFile, CWnd* pParent = NULL);   // standard constructor
	CDemoTranslucentDlg(UINT nImgID, LPCTSTR lpszType = _T("PNG"), HINSTANCE hResourceModule = NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDemoTranslucentDlg();

// Dialog Data
	enum { IDD = IDD_TRANSLUCENTDIALOG };

	virtual CStandardDialog* CreateStandardDialog();
	virtual void OnInitChildrenWnds();
    CDemoStandardDlg* GetStandardDialog();

    virtual void GetTitle(CString& s);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

};
