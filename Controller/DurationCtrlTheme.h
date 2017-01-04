#pragma once
//COLOR_HIGHLIGHT

class CDurationCtrlTheme
{
	#define TEST_HANDLE() if(m_hDll == NULL) return 0
	struct AFX_GLOBAL_DATA_EX :public AFX_GLOBAL_DATA
	{
		friend class CDurationCtrlTheme ;
	};

	typedef HRESULT (__stdcall *PFNGETTHEMECOLOR)(HTHEME hTheme, int iPartId, 
		int iStateId, int iPropId,  COLORREF *pColor);
	typedef HTHEME(__stdcall *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
	typedef HRESULT(__stdcall *PFNCLOSETHEMEDATA)(HTHEME hTheme);
	typedef BOOL(__stdcall *PFNISAPPTHEMED)();
	typedef HRESULT (__stdcall *PFNENABLETHEMEDIALOGTEXTURE)(HWND hwnd, DWORD dwFlags);
	typedef HRESULT(__stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc, 
		int iPartId, int iStateId, const RECT *pRect,  const RECT *pClipRect);
public:
	CDurationCtrlTheme(CWnd *pOwner){
	  AFX_GLOBAL_DATA_EX *pGlobalData = (AFX_GLOBAL_DATA_EX *)&afxGlobalData;
	  m_hDll = pGlobalData->m_hinstUXThemeDLL ;
	  
	  ASSERT(pOwner != NULL);
	  m_pOwner = pOwner;
	 
	  m_hTheme =  OpenThemeData(m_pOwner->m_hWnd ,_T("DATEPICKER;EDIT;LISTBOX;COMBOBOX;WINDOW"));
	 
	   
	};
	~CDurationCtrlTheme(void){
	 if(m_hTheme)
		 CloseThemeData(m_hTheme);
	 
	};
public:
	HRESULT GetThemeColor(int iPartId,int iStateId, int iPropId,  COLORREF *pColor)
	{
		return GetThemeColor(this->m_hTheme,iPartId,iStateId,iPropId, pColor);
	}
	HRESULT DrawBackground(CDC *pDC , CRect rec ,BOOL bFocus = FALSE,BOOL bEnable = TRUE)
	{
		int nFoucs = (bFocus) ?EBWBS_FOCUSED :  EBWBS_NORMAL;
		if(bEnable == FALSE)
			nFoucs = EBWBS_DISABLED;
		return DrawThemeBackground(m_hTheme,pDC->GetSafeHdc(),EP_BACKGROUNDWITHBORDER,nFoucs,rec,0);
	}
public:
	BOOL IsAppThemed()
{
	TEST_HANDLE();
	PFNISAPPTHEMED pfnIsAppThemed = (PFNISAPPTHEMED)GetProcAddress(m_hDll,"IsAppThemed");
	return (*pfnIsAppThemed)();
}

HRESULT EnableThemeDialogTexture(HWND hwnd, DWORD dwFlags)
{
	TEST_HANDLE();
	PFNENABLETHEMEDIALOGTEXTURE pfn = (PFNENABLETHEMEDIALOGTEXTURE)GetProcAddress(m_hDll,"EnableThemeDialogTexture");
	return (*pfn)(hwnd, dwFlags);
}

HTHEME OpenThemeData(HWND hwnd, LPCWSTR pszClassList)
{
	TEST_HANDLE();
	PFNOPENTHEMEDATA pfnOpenThemeData = (PFNOPENTHEMEDATA)GetProcAddress(m_hDll,"OpenThemeData");
	return (*pfnOpenThemeData)(hwnd, pszClassList);
}

HRESULT CloseThemeData(HTHEME hTheme)
{
	TEST_HANDLE();
	PFNCLOSETHEMEDATA pfnCloseThemeData = (PFNCLOSETHEMEDATA)GetProcAddress(m_hDll,"CloseThemeData");
	return (*pfnCloseThemeData)(hTheme);
}

HRESULT GetThemeColor(HTHEME hTheme, int iPartId, 
									   int iStateId, int iPropId,  COLORREF *pColor)
{
	TEST_HANDLE();
	PFNGETTHEMECOLOR pfn = (PFNGETTHEMECOLOR)GetProcAddress(m_hDll,"GetThemeColor");
	return (*pfn)(hTheme, iPartId, iStateId, iPropId, pColor);
}

HRESULT DrawThemeBackground(HTHEME hTheme, HDC hdc, 
											 int iPartId, int iStateId, const RECT *pRect, const RECT *pClipRect)
{
	TEST_HANDLE();
	PFNDRAWTHEMEBACKGROUND pfnDrawThemeBackground = 
		(PFNDRAWTHEMEBACKGROUND)GetProcAddress(m_hDll,"DrawThemeBackground");
	return (*pfnDrawThemeBackground)(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}

private :
	HINSTANCE	m_hDll;
	HTHEME		m_hTheme;
	CWnd		*m_pOwner;
};
