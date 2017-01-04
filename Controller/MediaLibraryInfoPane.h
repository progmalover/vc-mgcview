#pragma once
#include "MediaLibraryInfoCtrl.h"

// CMediaLibraryInfoPane

class CMediaLibraryInfoPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CMediaLibraryInfoPane)

public:
    CMediaLibraryInfoPane();
    virtual ~CMediaLibraryInfoPane();

public:
    CMediaLibraryInfoCtrl   m_MediaLibraryInfoCtrl;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


