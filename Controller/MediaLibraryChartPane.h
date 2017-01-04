#pragma once
#include "MediaLibraryChartDlg.h"

// CMediaLibraryChartPane

class CMediaLibraryChartPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CMediaLibraryChartPane)

public:
    CMediaLibraryChartPane();
    virtual ~CMediaLibraryChartPane();

    CMediaLibraryChartDlg m_wndMediaLibraryChart;
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

};


