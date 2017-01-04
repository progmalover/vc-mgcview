#pragma once


// CRemoveStorage dialog

class CRemoveStorage : public CDialogEx
{
	DECLARE_DYNAMIC(CRemoveStorage)

public:
	CRemoveStorage(vector<CString> &szPathList, CWnd* pParent = NULL, CString WindowName = Translate(_T("Select Removeable Disk")));   // standard constructor
	virtual ~CRemoveStorage();

// Dialog Data
	enum { IDD = IDD_REMOVEABLE_STORAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	vector<CString> m_PathList;
	CString m_szWindowsName;
	CComboBox m_RemoveStoragePath;
	CString m_szPath;
public:
	afx_msg void OnBnClickedOk();
	void GetSelectPath(CString& path) { path = m_szPath; };
};
