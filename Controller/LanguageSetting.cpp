// LanguageSetting: implementation file

#include "stdafx.h"
#include "Controller.h"
#include "LanguageSetting.h"
#include "Localization/Localization.h"

// LanguageSetting

IMPLEMENT_DYNAMIC(LanguageSetting, CMFCPropertyPage)

LanguageSetting::LanguageSetting()
	: CMFCPropertyPage(LanguageSetting::IDD)
{
	
}

LanguageSetting::~LanguageSetting()
{
}

BOOL LanguageSetting::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	//m_LanguageList.AddString(_T("English"));
	map<CString ,CString> LanNameMap = GetLanguageNameMap();

	if(LanNameMap.size())
	{
		map<CString ,CString>::iterator it;
        int index = 0;
		for(it = LanNameMap.begin();it != LanNameMap.end();it++,index++)
		{
			m_LanguageList.AddString(it->first);
            if(m_SelLanguage.CompareNoCase(ChangeLocToEngLanName(it->first)) == 0)
            {
                m_LanguageList.SetItemData(index, LVIS_SELECTED);
                m_LanguageList.SetCurSel(index);
            }
		}
	}
	//m_LanguageList.AddString(Translate(_T("Chinese")));
    
	UpdateData(FALSE);

	UntranslateWnd(GetDlgItem(IDC_LIST_LANGUAGE)->m_hWnd);
	Translate(this);
	ClearUntranslateWnd(); // call this function to clear hWnd list.

	return TRUE;  
}

BEGIN_MESSAGE_MAP(LanguageSetting, CMFCPropertyPage)
 ON_LBN_SELCHANGE(IDC_LIST_LANGUAGE, &LanguageSetting::OnLbnSelchangeListLanguage)
END_MESSAGE_MAP()

void LanguageSetting::DoDataExchange( CDataExchange* pDX )
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LANGUAGE, m_LanguageList);

}

void LanguageSetting::OnLbnSelchangeListLanguage()
{
	// TODO: Add your control notification handler code here
	//Translate(AfxGetApp()->m_pMainWnd);
	int index = m_LanguageList.GetCurSel();
	if (index == -1)
	{
		return;
	}

	CString szLocLanguage;
	m_LanguageList.GetText(index, szLocLanguage);
	m_SelLanguage = ChangeLocToEngLanName(szLocLanguage);
	SetModified();

}
