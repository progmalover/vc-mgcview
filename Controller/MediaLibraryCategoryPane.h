#pragma once
#include "MediaLibraryCategoryCtrl.h"

// CMediaLibraryCategoryPane

class CMediaLibraryCategoryPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CMediaLibraryCategoryPane)

public:
    CMediaLibraryCategoryPane();
    virtual ~CMediaLibraryCategoryPane();

    CMediaLibraryCategoryCtrl m_MediaLibraryCategory;
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


