#pragma once

#include <tchar.h>

#define DEFAULT_ROOTPATH    _T("c:\\")
#define Translate(x) x
#define DEFAULT_TITLE       Translate(_T("Please select the output directory"))
#undef Translate

class MyFolderDialog
{
public:
    BOOL ShowDialog( HWND hParent = NULL,
        LPCTSTR lpszRoot = DEFAULT_ROOTPATH, LPCTSTR lpszTitle = Translate(DEFAULT_TITLE));

    LPCTSTR GetPath() { return m_szPath; };
    LPTSTR GetRootPath() { return m_szRootPath; };

protected:
    TCHAR m_szPath[MAX_PATH];
    TCHAR m_szRootPath[MAX_PATH];
};
