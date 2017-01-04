#include "StdAfx.h"
#include "shlobj.h"
#include "FolderDialog.h"

int CALLBACK BrowseCallbackProc( HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData );


BOOL MyFolderDialog::ShowDialog(HWND hParent, LPCTSTR szRoot, LPCTSTR szTitle)
{
    _tcscpy_s(m_szRootPath, MAX_PATH, szRoot);
    m_szPath[0] = _T('\0');

    BROWSEINFO bi;
    bi.hwndOwner = hParent;     // Owner of created dialog box
    bi.pidlRoot = 0;                    // Unused
    bi.pszDisplayName = 0;              // Buffer to receive name displayed by Folder (not a valid path)
    bi.lpszTitle = szTitle;           // Title is ¡°Browse for Folder¡±, this is an instruction
    bi.lpfn = BrowseCallbackProc;       // Callback routine called when dialog has been initialized
    bi.lParam = (LPARAM)this;       // Passed to callback routine
    bi.ulFlags =
        BIF_RETURNONLYFSDIRS |          // Only allow user to select a directory
        BIF_STATUSTEXT |                // Create status text field we will be writing to in callback
        // BIF_BROWSEFORCOMPUTER|       // only allow user to select a computer
        // BIF_BROWSEFORPRINTER |       // only allow user to select a printer
        // BIF_BROWSEINCLUDEFILES|      // displays files too which user can pick
        // BIF_DONTGOBELOWDOMAIN|       // when user is exploring the ¡°Entire Network¡± they are not allowed into any domain
        BIF_NEWDIALOGSTYLE | 
        0;
    // Open dialog.
    LPITEMIDLIST lpItemId = ::SHBrowseForFolder( &bi );

    // Get full path.
    if ( lpItemId != NULL )
    {
        ::SHGetPathFromIDList( lpItemId, m_szPath );
        ::GlobalFree( lpItemId );
        return TRUE;
    }
    else
    {
        m_szPath[0] = _T('\0'); 
    }

    return FALSE;
}

int CALLBACK BrowseCallbackProc( HWND hwnd, UINT msg, LPARAM lp, LPARAM pData )
{
    LPTSTR  szBuf;

    switch(msg)
    {
    // When dialog is first initialized, change directory to one chosen above
    case BFFM_INITIALIZED:
        szBuf = (LPTSTR)((MyFolderDialog *)pData)->GetRootPath();
        ::SendMessage( hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)szBuf);
        break;

    // If you picked BIF_STATUSTEXT above, you can fill status here
    case BFFM_SELCHANGED:
        szBuf = (LPTSTR)((MyFolderDialog *)pData)->GetPath();
        if ( ::SHGetPathFromIDList((LPITEMIDLIST)lp, szBuf))
            SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szBuf);
        break;
    }
    return 0;
}