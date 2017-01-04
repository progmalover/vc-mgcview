#pragma once

#include "LayoutSettings.h"
#include "BaseDropTarget.h"

// CMediaFileListCtrl

class CMediaFileListCtrl : public CMFCShellListCtrl
{
    DECLARE_DYNAMIC(CMediaFileListCtrl)

public:
    CMediaFileListCtrl();
    virtual ~CMediaFileListCtrl();

    void SetPluginSupportFiles(std::map<CString, PLUGIN_SUPPORTFILES*>* pPluginFiles);

protected:
    CBaseDropTarget         m_DropTarget;                   /// for drop target functionality
    COleDataSource          m_OleDataSource;
    std::map<CString, PLUGIN_SUPPORTFILES*>*  m_PluginFiles;

protected:
    virtual HRESULT EnumObjects(LPSHELLFOLDER pParentFolder, LPITEMIDLIST pidlParent);


protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnNMClickFile(NMHDR *pNMHDR, LRESULT *pResult);
    //afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnBeginDrag(NMHDR *pNMHDR, LRESULT *pResult);

    afx_msg LRESULT OnDragEnter(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragOver(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDragLeave(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDrop(WPARAM wParam, LPARAM lParam);


    DECLARE_MESSAGE_MAP()


public:
    afx_msg void OnDestroy();

};


