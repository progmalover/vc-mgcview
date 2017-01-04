// RemoveStorage.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "RemoveStorage.h"
#include "afxdialogex.h"


// CRemoveStorage dialog

IMPLEMENT_DYNAMIC(CRemoveStorage, CDialogEx)

CRemoveStorage::CRemoveStorage(vector<CString> &szPathList, CWnd* pParent /*=NULL*/, CString WindowName)
	: CDialogEx(CRemoveStorage::IDD, pParent)
{
	m_PathList      = szPathList;
	m_szWindowsName = WindowName;
}

CRemoveStorage::~CRemoveStorage()
{
}

void CRemoveStorage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REMOVEABLEPATH, m_RemoveStoragePath);
}


BEGIN_MESSAGE_MAP(CRemoveStorage, CDialogEx)
	ON_BN_CLICKED(IDOK, &CRemoveStorage::OnBnClickedOk)
END_MESSAGE_MAP()


// CRemoveStorage message handlers


BOOL CRemoveStorage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	Translate(this);
	SetWindowTextW(m_szWindowsName);

	vector<CString>::iterator it = m_PathList.begin();

	for (; it != m_PathList.end(); it++)
	{
		m_RemoveStoragePath.AddString(*it);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CRemoveStorage::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	INT iIndex = 0;
	iIndex = m_RemoveStoragePath.GetCurSel();
	if (iIndex ==  CB_ERR)
	{
		MessageBox(Translate(_T("Please select removable storage firstly.")), Translate(_T("Error:Schedule page")),  MB_ICONERROR);
		return;
	}
	m_RemoveStoragePath.GetLBText(iIndex, m_szPath);
	CDialogEx::OnOK();
}
