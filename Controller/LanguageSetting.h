#pragma once

// LanguageSetting

class LanguageSetting : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(LanguageSetting)

public:
	LanguageSetting();
	virtual ~LanguageSetting();

	// Dialog Data
	enum { IDD = IDD_LANGUAGE_SETTING };

	CString m_SelLanguage;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected:
	DECLARE_MESSAGE_MAP()
public:
	 afx_msg void OnLbnSelchangeListLanguage();

	 CListBox m_LanguageList;
};


