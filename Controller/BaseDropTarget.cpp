#include "StdAfx.h"
#include "BaseDropTarget.h"


// wParam - CS3OleDropInfo point, lParam - no used
UINT WM_DND_DRAGENTER = ::RegisterWindowMessage(_T("DND_DRAGENTER"));

// wParam - CS3OleDropInfo point, lParam - no used
UINT WM_DND_DRAGOVER = ::RegisterWindowMessage(_T("DND_DRAGOVER"));

// wParam - CS3OleDropInfo point, lParam - no used
UINT WM_DND_DRAGLEAVE = ::RegisterWindowMessage(_T("DND_DRAGLEAVE"));

// wParam - CS3OleDropInfo point, lParam - no used
UINT WM_DND_DRAGSCROLL = ::RegisterWindowMessage(_T("DND_DRAGSCROLL"));

// wParam - CS3OleDropInfo point, lParam - no used
UINT WM_DND_DROPEX = ::RegisterWindowMessage(_T("DND_DROPEX"));

// wParam - CS3OleDropInfo point, lParam - no used
UINT WM_DND_DROP = ::RegisterWindowMessage(_T("DND_DROP"));


CBaseDropTarget::CBaseDropTarget(void)
{
}

CBaseDropTarget::~CBaseDropTarget(void)
{
}

DROPEFFECT CBaseDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
    ASSERT_VALID(this);
    ASSERT(::IsWindow(pWnd->m_hWnd));

    // If CView, call base class
    if (pWnd->IsKindOf(RUNTIME_CLASS(CView)))
    {
        return COleDropTarget::OnDragEnter(pWnd, pDataObject, dwKeyState, point);
    }

    CS3OleDropInfo DropInfo(pDataObject, dwKeyState, 0, 0, point);

    DROPEFFECT dropEffect = DROPEFFECT_NONE;
    dropEffect = (DROPEFFECT)::SendMessage(pWnd->GetSafeHwnd(), WM_DND_DRAGENTER, (WPARAM)&DropInfo, 0);

    return dropEffect;
}

void CBaseDropTarget::OnDragLeave(CWnd* pWnd)
{
    ASSERT_VALID(this);
    ASSERT(::IsWindow(pWnd->m_hWnd));

    // If CView, call base class
    if (pWnd->IsKindOf(RUNTIME_CLASS(CView)))
    {
        COleDropTarget::OnDragLeave(pWnd);
        return;
    }

    ::SendMessage(pWnd->GetSafeHwnd(), WM_DND_DRAGLEAVE, 0, 0);

    return;
}

DROPEFFECT CBaseDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
    ASSERT_VALID(this);
    ASSERT(::IsWindow(pWnd->m_hWnd));

    // If CView, call base class
    if (pWnd->IsKindOf(RUNTIME_CLASS(CView)))
    {
        return COleDropTarget::OnDragOver(pWnd, pDataObject, dwKeyState, point);
    }

    CS3OleDropInfo DropInfo(pDataObject, dwKeyState, 0, 0, point);

    DROPEFFECT dropEffect = DROPEFFECT_NONE;
    dropEffect = (DROPEFFECT)::SendMessage(pWnd->GetSafeHwnd(), WM_DND_DRAGOVER, (WPARAM)&DropInfo, 0);

    return dropEffect;
}

DROPEFFECT CBaseDropTarget::OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point)
{
    return DROPEFFECT_NONE;
}

DROPEFFECT CBaseDropTarget::OnDropEx(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropList, CPoint point)
{
    ASSERT_VALID(this);
    ASSERT(::IsWindow(pWnd->m_hWnd));

    // If CView, call base class
    if (pWnd->IsKindOf(RUNTIME_CLASS(CView)))
    {
        return COleDropTarget::OnDropEx(pWnd, pDataObject, dropEffect, dropList, point);
    }

    CS3OleDropInfo DropInfo(pDataObject, 0, dropEffect, dropList, point);

    DROPEFFECT effect = (DROPEFFECT) - 1;
    //effect = (DROPEFFECT)::SendMessage(pWnd->GetSafeHwnd(), WM_DND_DROPEX, (WPARAM)&DropInfo, 0);

    return effect;
}

BOOL CBaseDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
    ASSERT_VALID(this);
    ASSERT(::IsWindow(pWnd->m_hWnd));

    // If CView, call base class
    if (pWnd->IsKindOf(RUNTIME_CLASS(CView)))
    {
        return COleDropTarget::OnDrop(pWnd, pDataObject, dropEffect, point);
    }

    CS3OleDropInfo DropInfo(pDataObject, 0, dropEffect, 0, point);

    BOOL bDrop = FALSE;
    bDrop = (BOOL)::SendMessage(pWnd->GetSafeHwnd(), WM_DND_DROP, (WPARAM)&DropInfo, 0);

    return bDrop;
}