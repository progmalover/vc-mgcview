#pragma once
#include "afxole.h"
#include "S3OleDropInfo.h"

// self define message, used for call drag/drop function in each cwnd

// wParam - CS3OleDropInfo point, lParam - no used
extern UINT WM_DND_DRAGENTER;

// wParam - CS3OleDropInfo point, lParam - no used
extern UINT WM_DND_DRAGOVER;

// wParam - CS3OleDropInfo point, lParam - no used
extern UINT WM_DND_DRAGLEAVE;

// wParam - CS3OleDropInfo point, lParam - no used
extern UINT WM_DND_DRAGSCROLL;

// wParam - CS3OleDropInfo point, lParam - no used
extern UINT WM_DND_DROPEX;

// wParam - CS3OleDropInfo point, lParam - no used
extern UINT WM_DND_DROP;



class CBaseDropTarget :
    public COleDropTarget
{
public:
    CBaseDropTarget(void);
    virtual ~CBaseDropTarget(void);

    // Overrides
public:
    virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    virtual void OnDragLeave(CWnd* pWnd);
    virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    virtual DROPEFFECT OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point);
    virtual DROPEFFECT OnDropEx(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropList, CPoint point);
    virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
};

