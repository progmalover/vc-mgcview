#pragma once
#include "MediaLibrarySearchBar.h"

// CMediaLibrarySearchBarPane

class CMediaLibrarySearchBarPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CMediaLibrarySearchBarPane)

public:
    CMediaLibrarySearchBarPane();
    virtual ~CMediaLibrarySearchBarPane();

public:
    CMediaLibrarySearchBarCtrl   m_MediaLibrarySearchBarCtrl;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


