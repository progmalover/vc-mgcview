// MFCButtonEx.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "MFCButtonEx.h"
#include "Localization/Localization.h"

// CMFCButtonEx

IMPLEMENT_DYNAMIC(CMFCButtonEx, CMFCButton)

CMFCButtonEx::CMFCButtonEx()
{

}

CMFCButtonEx::~CMFCButtonEx()
{
}


BEGIN_MESSAGE_MAP(CMFCButtonEx, CMFCButton)
ON_MESSAGE(WM_MFC_INITCTRL, OnInitControl)
END_MESSAGE_MAP()

LRESULT CMFCButtonEx::OnInitControl(WPARAM,LPARAM)
{
	CString csText;
	this->GetWindowText(csText);
	csText = Translate(csText);

	this->SetTooltip(csText);
	this->SetWindowText(_T(""));

	return S_OK;
}

// CMFCButtonEx message handlers


