//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of BCGControlBar Library Professional Edition
// Copyright (C) 1998-2010 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
// BCGPDlgImpl.cpp: implementation of the CBORROW_DlgImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "multimon.h"
// #include "bcgcbpro.h"

// #ifndef _BCGSUITE_
// #include "BCGPPopupMenu.h"
// #include "BCGPToolbarMenuButton.h"
// #else
#define FindDestBar	FindDestintationToolBar
// #endif

#include "BORROW_Dialog.h"
#include "BORROW_DlgImpl.h"
// #include "BORROW_PropertySheet.h"
#include "BORROW_Button.h"
// #include "BORROW_PropertyPage.h"
#include "BORROW_DlgImpl.h"
#include "BORROW_SliderCtrl.h"
#include "BORROW_ProgressCtrl.h"
#include "BORROW_Group.h"
#include "BORROW_Static.h"
// #include "BORROW_Edit.h"
#include "BORROW_ComboBox.h"
// #include "BORROW_VisualManager.h"
#include "BORROW_ScrollBar.h"
// #include "BORROW_SpinButtonCtrl.h"
// #include "BORROW_FormView.h"
// #include "BORROW_RibbonElementHostCtrl.h"
// #include "BORROW_RibbonBackstageView.h"
#define BCGP_EXCLUDE_RIBBON 1
/*
#ifndef _BCGSUITE_
#include "BCGPCaptionButton.h"
#include "BCGPFrameImpl.h"
#endif
*/

// #ifndef _BCGSUITE_
/*
#define visualManager	CControllerVisualManager::GetInstance ()
#define visualManager		CControllerVisualManager::GetInstance ()
*/
/*
#else
#define visualManager	CMFCVisualManager::GetInstance ()
#define visualManager		CBCGPVisualManager::GetInstance ()
#endif
*/

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

HHOOK CBORROW_DlgImpl::m_hookMouse = NULL;
CBORROW_DlgImpl* CBORROW_DlgImpl::m_pMenuDlgImpl = NULL;

UINT BCGM_ONSETCONTROLAERO = ::RegisterWindowMessage (_T("BCGM_ONSETCONTROLAERO"));
UINT BCGM_ONSETCONTROLVMMODE = ::RegisterWindowMessage (_T("BCGM_ONSETCONTROLVMMODE"));
UINT BCGM_ONSETCONTROLBACKSTAGEMODE = ::RegisterWindowMessage (_T("BCGM_ONSETCONTROBACKSTAGEMMODE"));

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBORROW_DlgImpl::CBORROW_DlgImpl(CWnd& dlg) :
	m_Dlg (dlg),
	m_bVisualManagerStyle (FALSE),
	m_bTransparentStaticCtrls (TRUE),
	m_bVisualManagerNCArea (FALSE),
	m_nHotSysButton (HTNOWHERE),
	m_nHitSysButton (HTNOWHERE),
	m_bWindowPosChanging (FALSE),
	m_bHasBorder (FALSE),
	m_bIsWindowRgn (FALSE),
	m_bIsWhiteBackground(FALSE),
// 	m_pLayout(NULL),
	m_bBackstageMode(FALSE),
	m_bResizeBox(FALSE)
{
	m_AeroMargins.cxLeftWidth = 0;
	m_AeroMargins.cxRightWidth = 0;
	m_AeroMargins.cyTopHeight = 0;
	m_AeroMargins.cyBottomHeight = 0;

	m_rectRedraw.SetRectEmpty ();
	m_rectResizeBox.SetRectEmpty ();

	ZeroMemory(&m_LayoutMMI, sizeof(MINMAXINFO));
}
//*******************************************************************************************
CBORROW_DlgImpl::~CBORROW_DlgImpl()
{
	//------------------------------
	// Clear caption system buttons:
	//------------------------------
	while (!m_lstCaptionSysButtons.IsEmpty ())
	{
		delete m_lstCaptionSysButtons.RemoveHead ();
	}

/*
	if (m_pLayout != NULL)
	{
		delete m_pLayout;
		m_pLayout = NULL;
	}
*/
}
//*******************************************************************************************
BOOL CBORROW_DlgImpl::ProcessMouseClick (POINT pt)
{
	if (!CMFCToolBar::IsCustomizeMode () &&
		CMFCPopupMenu::GetActiveMenu() != NULL &&
		::IsWindow (CMFCPopupMenu::GetActiveMenu()->m_hWnd))
	{
		CMFCPopupMenu::MENUAREA_TYPE clickArea = CMFCPopupMenu::GetActiveMenu()->CheckArea (pt);

		if (clickArea == CMFCPopupMenu::OUTSIDE)
		{
			// Click outside of menu

			//--------------------------------------------
			// Maybe secondary click on the parent button?
			//--------------------------------------------
			CMFCToolBarMenuButton* pParentButton = 
				CMFCPopupMenu::GetActiveMenu()->GetParentButton ();
			if (pParentButton != NULL)
			{
				CWnd* pWndParent = pParentButton->GetParentWnd ();
				if (pWndParent != NULL)
				{
					CMFCPopupMenuBar* pWndParentPopupMenuBar = 
						DYNAMIC_DOWNCAST (CMFCPopupMenuBar, pWndParent);

					CPoint ptClient = pt;
					pWndParent->ScreenToClient (&ptClient);

					if (pParentButton->Rect ().PtInRect (ptClient))
					{
						//-------------------------------------------------------
						// If user clicks second time on the parent button,
						// we should close an active menu on the toolbar/menubar
						// and leave it on the popup menu:
						//-------------------------------------------------------
						if (pWndParentPopupMenuBar == NULL &&
							!CMFCPopupMenu::GetActiveMenu()->InCommand ())
						{
							//----------------------------------------
							// Toolbar/menu bar: close an active menu!
							//----------------------------------------
							CMFCPopupMenu::GetActiveMenu()->SendMessage (WM_CLOSE);
						}

						return TRUE;
					}

					if (pWndParentPopupMenuBar != NULL)
					{
						pWndParentPopupMenuBar->CloseDelayedSubMenu ();
						
						CMFCPopupMenu* pWndParentPopupMenu = 
							DYNAMIC_DOWNCAST (CMFCPopupMenu, 
							pWndParentPopupMenuBar->GetParent ());

						if (pWndParentPopupMenu != NULL)
						{
							CMFCPopupMenu::MENUAREA_TYPE clickAreaParent = 
								pWndParentPopupMenu->CheckArea (pt);

							switch (clickAreaParent)
							{
							case CMFCPopupMenu::MENU:
							case CMFCPopupMenu::TEAROFF_CAPTION:
							case CMFCPopupMenu::LOGO:
								return FALSE;

							case CMFCPopupMenu::SHADOW_RIGHT:
							case CMFCPopupMenu::SHADOW_BOTTOM:
								pWndParentPopupMenu->SendMessage (WM_CLOSE);
								m_Dlg.SetFocus ();

								return TRUE;
							}
						}
					}
				}
			}

			if (!CMFCPopupMenu::GetActiveMenu()->InCommand ())
			{
				CMFCPopupMenu::GetActiveMenu()->SendMessage (WM_CLOSE);

				CWnd* pWndFocus = CWnd::GetFocus ();
				if (pWndFocus != NULL && pWndFocus->IsKindOf (RUNTIME_CLASS (CMFCToolBar)))
				{
					m_Dlg.SetFocus ();
				}

				if (clickArea != CMFCPopupMenu::OUTSIDE)	// Click on shadow
				{
					return TRUE;
				}
			}
		}
		else if (clickArea == CMFCPopupMenu::SHADOW_RIGHT ||
				clickArea == CMFCPopupMenu::SHADOW_BOTTOM)
		{
			CMFCPopupMenu::GetActiveMenu()->SendMessage (WM_CLOSE);
			m_Dlg.SetFocus ();

			return TRUE;
		}
	}

	return FALSE;
}
//*******************************************************************************************
BOOL CBORROW_DlgImpl::ProcessMouseMove (POINT pt)
{
	if (m_bBackstageMode && m_Dlg.GetParent() != NULL)
	{
		m_Dlg.GetParent()->SendMessage (WM_MOUSEMOVE, 0, MAKELPARAM (-1, -1));
	}

	if (!CMFCToolBar::IsCustomizeMode () &&
		CMFCPopupMenu::GetActiveMenu() != NULL)
	{
		CRect rectMenu;
		CMFCPopupMenu::GetActiveMenu()->GetWindowRect (rectMenu);

		if (rectMenu.PtInRect (pt) ||
			CMFCPopupMenu::GetActiveMenu()->GetMenuBar ()->FindDestBar (pt) != NULL)
		{
			return FALSE;	// Default processing
		}

		return TRUE;		// Active menu "capturing"
	}

	return FALSE;	// Default processing
}
//**************************************************************************************
BOOL CBORROW_DlgImpl::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message)
	{
	case WM_SYSKEYDOWN:
	case WM_CONTEXTMENU:
		if (CMFCPopupMenu::GetActiveMenu() != NULL &&
			::IsWindow (CMFCPopupMenu::GetActiveMenu()->m_hWnd) &&
			pMsg->wParam == VK_MENU)
		{
			CMFCPopupMenu::GetActiveMenu()->SendMessage (WM_CLOSE);
			return TRUE;
		}
		break;

	case WM_SYSKEYUP:
		if (CMFCPopupMenu::GetActiveMenu() != NULL &&
			::IsWindow (CMFCPopupMenu::GetActiveMenu()->m_hWnd))
		{
			return TRUE;	// To prevent system menu opening
		}
		break;

	case WM_KEYDOWN:
		//-----------------------------------------
		// Pass keyboard action to the active menu:
		//-----------------------------------------
		if (CMFCPopupMenu::GetActiveMenu() != NULL &&
			::IsWindow (CMFCPopupMenu::GetActiveMenu()->m_hWnd))
		{
			CMFCPopupMenu::GetActiveMenu()->SendMessage (WM_KEYDOWN, (int) pMsg->wParam);
			return TRUE;
		}

#ifndef BCGP_EXCLUDE_RIBBON
		if (m_bBackstageMode && (pMsg->wParam == VK_HOME || pMsg->wParam == VK_ESCAPE))
		{
			for (CWnd* pWndParent = m_Dlg.GetParent(); pWndParent != NULL; pWndParent = pWndParent->GetParent())
			{
				CBCGPRibbonBackstageView* pView = DYNAMIC_DOWNCAST(CBCGPRibbonBackstageView, pWndParent);
				if (pView != NULL)
				{
					if (pMsg->wParam == VK_HOME)
					{
						pView->SetFocus();
					}
					else	// Escape
					{
						pView->SendMessage(WM_CLOSE);
					}
					return TRUE;
				}
			}
			return TRUE;
		}
#endif
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		{
			CPoint pt (GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
			CWnd* pWnd = CWnd::FromHandle (pMsg->hwnd);

			if (pWnd != NULL && ::IsWindow (pMsg->hwnd))
			{
				pWnd->ClientToScreen (&pt);
			}

			if (ProcessMouseClick (pt))
			{
				return TRUE;
			}

			if (!::IsWindow (pMsg->hwnd))
			{
				return TRUE;
			}
		}
		break;

	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONDOWN:
	case WM_NCRBUTTONUP:
	case WM_NCMBUTTONDOWN:
	case WM_NCMBUTTONUP:
		if (ProcessMouseClick (CPoint (GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam))))
		{
			return TRUE;
		}

		if (pMsg->message == WM_NCRBUTTONUP && pMsg->hwnd == m_Dlg.GetSafeHwnd () && IsOwnerDrawCaption ())
		{
			CPoint pt (GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));

			UINT nHit = OnNcHitTest (pt);

			if (nHit == HTCAPTION || nHit == HTSYSMENU)
			{
				CMenu* pMenu = m_Dlg.GetSystemMenu (FALSE);
				if (pMenu->GetSafeHmenu () != NULL && ::IsMenu (pMenu->GetSafeHmenu ()))
				{
					UINT uiRes = ::TrackPopupMenu (pMenu->GetSafeHmenu(), TPM_LEFTBUTTON | TPM_RETURNCMD, 
						pt.x, pt.y, 0, m_Dlg.GetSafeHwnd (), NULL);

					if (uiRes != 0)
					{
						m_Dlg.SendMessage (WM_SYSCOMMAND, uiRes);
						return TRUE;
					}
				}

			}
		}
		break;

	case WM_MOUSEWHEEL:
		if (CMFCPopupMenu::GetActiveMenu() != NULL &&
			::IsWindow (CMFCPopupMenu::GetActiveMenu()->m_hWnd) &&
			CMFCPopupMenu::GetActiveMenu()->IsScrollable ())
		{
			CMFCPopupMenu::GetActiveMenu()->SendMessage (WM_MOUSEWHEEL,
				pMsg->wParam, pMsg->lParam);

		}
#ifndef BCGP_EXCLUDE_RIBBON
		else
		{
			CPoint ptCursor;
			::GetCursorPos (&ptCursor);

			CWnd* pWnd = CWnd::WindowFromPoint(ptCursor);

			CBCGPRibbonGalleryCtrl* pGalleryCtrl = DYNAMIC_DOWNCAST(CBCGPRibbonGalleryCtrl, pWnd);

			if (pGalleryCtrl != NULL)
			{
				pGalleryCtrl->SendMessage (WM_MOUSEWHEEL, pMsg->wParam, pMsg->lParam);
				return TRUE;
			}
		}
#endif
		break;

	case WM_MOUSEMOVE:
		{
			CPoint pt (GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
			CWnd* pWnd = CWnd::FromHandle (pMsg->hwnd);

			if (pWnd != NULL)
			{
				pWnd->ClientToScreen (&pt);
			}

			if (ProcessMouseMove (pt))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}
//**********************************************************************************
LRESULT CALLBACK CBORROW_DlgImpl::BCGDlgMouseProc (int nCode, WPARAM wParam, LPARAM lParam)
{
	if (m_pMenuDlgImpl != NULL)
	{
		switch (wParam)
		{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_NCLBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
		case WM_NCMBUTTONDOWN:
			{
				CPoint ptCursor;
				::GetCursorPos (&ptCursor);

				CRect rectWindow;
				m_pMenuDlgImpl->m_Dlg.GetWindowRect (rectWindow);

				if (!rectWindow.PtInRect (ptCursor))
				{
					m_pMenuDlgImpl->ProcessMouseClick (ptCursor);
				}
			}
		}
	}

	return CallNextHookEx (m_hookMouse, nCode, wParam, lParam);
}
//****************************************************************************************
void CBORROW_DlgImpl::SetActiveMenu (CMFCPopupMenu* pMenu)
{
/*
#ifndef _BCGSUITE_
	CMFCPopupMenu::m_pActivePopupMenu = pMenu;
#else
*/
	class CMFCPopupMenuDummy : public CMFCPopupMenu
	{
		friend class CBORROW_DlgImpl;
	};

	CMFCPopupMenuDummy::m_pActivePopupMenu = pMenu;

// #endif

	if (pMenu != NULL)
	{
		if (m_hookMouse == NULL)
		{
			m_hookMouse = ::SetWindowsHookEx (WH_MOUSE, BCGDlgMouseProc, 
				0, GetCurrentThreadId ());
		}

		m_pMenuDlgImpl = this;
	}
	else 
	{
		if (m_hookMouse != NULL)
		{
			::UnhookWindowsHookEx (m_hookMouse);
			m_hookMouse = NULL;
		}

		m_pMenuDlgImpl = NULL;
	}

}
//****************************************************************************************
void CBORROW_DlgImpl::OnDestroy ()
{
	for (int i = 0; i < m_arSubclassedCtrls.GetSize (); i++)
	{
		delete m_arSubclassedCtrls [i];
	}

	m_arSubclassedCtrls.RemoveAll ();
	if (m_pMenuDlgImpl != NULL &&
		m_pMenuDlgImpl->m_Dlg.GetSafeHwnd () == m_Dlg.GetSafeHwnd ())
	{
		m_pMenuDlgImpl = NULL;
	}
}
//****************************************************************************************
BOOL CBORROW_DlgImpl::OnCommand (WPARAM wParam, LPARAM /*lParam*/)
{
	if (HIWORD (wParam) == 1)
	{
		UINT uiCmd = LOWORD (wParam);

		CMFCToolBar::AddCommandUsage (uiCmd);

		//---------------------------
		// Simmulate ESC keystroke...
		//---------------------------
		if (CMFCPopupMenu::GetActiveMenu() != NULL &&
			::IsWindow (CMFCPopupMenu::GetActiveMenu()->m_hWnd))
		{
			CMFCPopupMenu::GetActiveMenu()->SendMessage (WM_KEYDOWN, VK_ESCAPE);
			return TRUE;
		}

/*
		if (g_pUserToolsManager != NULL &&
			g_pUserToolsManager->InvokeTool (uiCmd))
		{
			return TRUE;
		}
*/
	}

	return FALSE;
}
//***************************************************************************************
void CBORROW_DlgImpl::OnNcActivate (BOOL& bActive)
{
	//----------------------------------------
	// Stay active if WF_STAYACTIVE bit is on:
	//----------------------------------------
	if (m_Dlg.m_nFlags & WF_STAYACTIVE)
	{
		bActive = TRUE;
	}

	//--------------------------------------------------
	// But do not stay active if the window is disabled:
	//--------------------------------------------------
	if (!m_Dlg.IsWindowEnabled ())
	{
		bActive = FALSE;
	}

	if (IsOwnerDrawCaption ())
	{
		visualManager->OnNcActivate (&m_Dlg, bActive);
		m_Dlg.RedrawWindow (CRect (0, 0, 0, 0), NULL,
			RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
	}
}
//****************************************************************************************
void CBORROW_DlgImpl::OnActivate(UINT nState, CWnd* pWndOther)
{
	m_Dlg.m_nFlags &= ~WF_STAYACTIVE;

	//--------------------------------------------------
	// Determine if this window should be active or not:
	//--------------------------------------------------
	CWnd* pWndActive = (nState == WA_INACTIVE) ? pWndOther : &m_Dlg;
	if (pWndActive != NULL)
	{
		BOOL bStayActive = (pWndActive->GetSafeHwnd () == m_Dlg.GetSafeHwnd () ||
			pWndActive->SendMessage (WM_FLOATSTATUS, FS_SYNCACTIVE));

		if (bStayActive)
		{
			m_Dlg.m_nFlags |= WF_STAYACTIVE;
		}
	}
	else 
	{
		//------------------------------------------
		// Force painting on our non-client area....
		//------------------------------------------
		m_Dlg.SendMessage (WM_NCPAINT, 1);
	}

	if (nState == WA_INACTIVE && IsOwnerDrawCaption ())
	{
		m_Dlg.RedrawWindow(NULL, NULL, RDW_FRAME | RDW_UPDATENOW);
	} 
}
//*************************************************************************************
void CBORROW_DlgImpl::EnableVisualManagerStyle (BOOL bEnable, BOOL bNCArea, const CList<UINT,UINT>* plstNonSubclassedItems)
{
	m_bVisualManagerStyle = bEnable;
	m_bVisualManagerNCArea = bNCArea;

	if (m_Dlg.GetSafeHwnd () == NULL)
	{
		return;
	}

	CWnd* pWndChild = m_Dlg.GetWindow (GW_CHILD);

	while (pWndChild != NULL)
	{
		ASSERT_VALID (pWndChild);

		if (plstNonSubclassedItems != NULL && plstNonSubclassedItems->Find (pWndChild->GetDlgCtrlID ()) != NULL)
		{
			pWndChild = pWndChild->GetNextWindow ();
			continue;
		}

		if (m_lstNonSubclassedItems.Find (pWndChild->GetDlgCtrlID ()) != NULL)
		{
			pWndChild = pWndChild->GetNextWindow ();
			continue;
		}

		CBorrow_Button* pButton = DYNAMIC_DOWNCAST(CBorrow_Button, pWndChild);
		if (pButton != NULL)
		{
			ASSERT_VALID (pButton);
			pButton->m_bVisualManagerStyle = m_bVisualManagerStyle;
		}

		if (m_bVisualManagerStyle &&
			CWnd::FromHandlePermanent (pWndChild->GetSafeHwnd ()) == NULL)
		{
			#define MAX_CLASS_NAME		255
			#define STATIC_CLASS		_T("Static")
			#define BUTTON_CLASS		_T("Button")
			#define EDIT_CLASS			_T("Edit")
			#define	COMBOBOX_CLASS		_T("ComboBox")
			#define SCROLLBAR_CLASS		_T("ScrollBar")

			TCHAR lpszClassName [MAX_CLASS_NAME + 1];

			::GetClassName (pWndChild->GetSafeHwnd (), lpszClassName, MAX_CLASS_NAME);
			CString strClass = lpszClassName;

			CWnd* pWndSubclassedCtrl = NULL;

			if (strClass == STATIC_CLASS)
			{
				pWndSubclassedCtrl = new CBORROW_Static;
			}
			else if (strClass == BUTTON_CLASS)
			{
				if ((pWndChild->GetStyle () & 0xF) == BS_GROUPBOX)
				{
					pWndSubclassedCtrl = new CBORROW_Group;
				}
				else
				{
					pWndSubclassedCtrl = new CBorrow_Button;
				}
			}
			else if (strClass == PROGRESS_CLASS)
			{
				pWndSubclassedCtrl = new CBORROW_ProgressCtrl;
			}
			else if (strClass == TRACKBAR_CLASS)
			{
				pWndSubclassedCtrl = new CBORROW_SliderCtrl;
			}
/* TODO:
			else if (strClass == EDIT_CLASS)
			{
				pWndSubclassedCtrl = new CBorrow_Edit;
			}
*/
			else if (strClass == COMBOBOX_CLASS)
			{
				pWndSubclassedCtrl = new CBORROW_ComboBox;
			}
			else if (strClass == SCROLLBAR_CLASS)
			{
				pWndSubclassedCtrl = new CBORROW_ScrollBar;
			}
/*
			else if (strClass == UPDOWN_CLASS)
			{
				pWndSubclassedCtrl = new CBORROW_SpinButtonCtrl;
			}
*/

			if (pWndSubclassedCtrl != NULL)
			{
				m_arSubclassedCtrls.Add (pWndSubclassedCtrl);
				pWndSubclassedCtrl->SubclassWindow (pWndChild->GetSafeHwnd ());
			}
		}

		pWndChild->SendMessage (BCGM_ONSETCONTROLVMMODE, (WPARAM) bEnable);
		pWndChild = pWndChild->GetNextWindow ();
	}

	OnChangeVisualManager ();

	if (m_Dlg.IsWindowVisible ())
	{
		m_Dlg.RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
	}
}
//********************************************************************************************************
BOOL CBORROW_DlgImpl::EnableAero (AFX_MARGINS& margins)
{
	m_AeroMargins = margins;

	if (HasAeroMargins () && !m_bVisualManagerStyle)
	{
		EnableVisualManagerStyle (TRUE);
	}

	if (m_Dlg.GetSafeHwnd () == NULL)
	{
		return TRUE;
	}

	BOOL bRes = afxGlobalData.DwmExtendFrameIntoClientArea (m_Dlg.GetSafeHwnd (), &m_AeroMargins);
	BOOL bIsAeroEnabled = afxGlobalData.DwmIsCompositionEnabled ();

	CRect rectClient;
	m_Dlg.GetClientRect (rectClient);

	CWnd* pWndChild = m_Dlg.GetWindow (GW_CHILD);

	while (pWndChild != NULL)
	{
		ASSERT_VALID (pWndChild);

		BOOL bGlass = FALSE;

		if (bIsAeroEnabled && HasAeroMargins ())
		{
			CRect rectChild;
			pWndChild->GetWindowRect (&rectChild);
			m_Dlg.ScreenToClient (&rectChild);

			CRect rectInter;

			if (m_AeroMargins.cxLeftWidth != 0)
			{
				CRect rectAero = rectClient;
				rectAero.right = rectAero.left + m_AeroMargins.cxLeftWidth;

				if (rectInter.IntersectRect (rectAero, rectChild))
				{
					bGlass = TRUE;
				}
			}

			if (!bGlass && m_AeroMargins.cxRightWidth != 0)
			{
				CRect rectAero = rectClient;
				rectAero.left = rectAero.right - m_AeroMargins.cxRightWidth;

				if (rectInter.IntersectRect (rectAero, rectChild))
				{
					bGlass = TRUE;
				}
			}

			if (!bGlass && m_AeroMargins.cyTopHeight != 0)
			{
				CRect rectAero = rectClient;
				rectAero.bottom = rectAero.top + m_AeroMargins.cyTopHeight;

				if (rectInter.IntersectRect (rectAero, rectChild))
				{
					bGlass = TRUE;
				}
			}

			if (!bGlass && m_AeroMargins.cyBottomHeight != 0)
			{
				CRect rectAero = rectClient;
				rectAero.top = rectAero.bottom - m_AeroMargins.cyBottomHeight;

				if (rectInter.IntersectRect (rectAero, rectChild))
				{
					bGlass = TRUE;
				}
			}
		}

		pWndChild->SendMessage (BCGM_ONSETCONTROLAERO, (WPARAM) bGlass);
		pWndChild = pWndChild->GetNextWindow ();
	}

	if (m_Dlg.IsWindowVisible ())
	{
		m_Dlg.RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
	}

	return bRes;
}
//********************************************************************************************************
void CBORROW_DlgImpl::GetAeroMargins (AFX_MARGINS& margins) const
{
	margins = m_AeroMargins;
}
//********************************************************************************************************
BOOL CBORROW_DlgImpl::HasAeroMargins () const
{
	return 	m_AeroMargins.cxLeftWidth != 0 ||
			m_AeroMargins.cxRightWidth != 0 ||
			m_AeroMargins.cyTopHeight != 0 ||
			m_AeroMargins.cyBottomHeight != 0;
}
//********************************************************************************************************
void CBORROW_DlgImpl::ClearAeroAreas (CDC* pDC)
{
	if (!HasAeroMargins () || m_Dlg.GetSafeHwnd () == NULL ||
		!afxGlobalData.DwmIsCompositionEnabled ())
	{
		return;
	}

	CRect rectClient;
	m_Dlg.GetClientRect (rectClient);

	if (m_AeroMargins.cxLeftWidth != 0)
	{
		CRect rectAero = rectClient;
		rectAero.right = rectAero.left + m_AeroMargins.cxLeftWidth;

		pDC->FillSolidRect (rectAero, RGB (0, 0, 0));
	}

	if (m_AeroMargins.cxRightWidth != 0)
	{
		CRect rectAero = rectClient;
		rectAero.left = rectAero.right - m_AeroMargins.cxRightWidth;

		pDC->FillSolidRect (rectAero, RGB (0, 0, 0));
	}

	if (m_AeroMargins.cyTopHeight != 0)
	{
		CRect rectAero = rectClient;
		rectAero.bottom = rectAero.top + m_AeroMargins.cyTopHeight;

		pDC->FillSolidRect (rectAero, RGB (0, 0, 0));
	}

	if (m_AeroMargins.cyBottomHeight != 0)
	{
		CRect rectAero = rectClient;
		rectAero.top = rectAero.bottom - m_AeroMargins.cyBottomHeight;

		pDC->FillSolidRect (rectAero, RGB (0, 0, 0));
	}
}
//********************************************************************************************************
void CBORROW_DlgImpl::OnDWMCompositionChanged ()
{
	if (!HasAeroMargins () || m_Dlg.GetSafeHwnd () == NULL)
	{
		return;
	}

	EnableAero (m_AeroMargins);
	OnChangeVisualManager ();
	m_Dlg.RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
}
//********************************************************************************************************
HBRUSH CBORROW_DlgImpl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	#define MAX_CLASS_NAME	255
	#define STATIC_CLASS	_T("Static")
	#define BUTTON_CLASS	_T("Button")

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		TCHAR lpszClassName [MAX_CLASS_NAME + 1];

		::GetClassName (pWnd->GetSafeHwnd (), lpszClassName, MAX_CLASS_NAME);
		CString strClass = lpszClassName;

		if (strClass == STATIC_CLASS)
		{
			pDC->SetBkMode(TRANSPARENT);

			if (m_bVisualManagerStyle)
			{
				pDC->SetTextColor (afxGlobalData.clrBarText);
			}

			if (m_bTransparentStaticCtrls && (pWnd->GetStyle () & SS_ICON) != SS_ICON)
			{
				return (HBRUSH) ::GetStockObject (HOLLOW_BRUSH);
			}

			if (m_bIsWhiteBackground)
			{
				return (HBRUSH) ::GetStockObject (WHITE_BRUSH);
			}

			return (HBRUSH) visualManager->GetDlgBackBrush (&m_Dlg).GetSafeHandle ();
		}

		if (strClass == BUTTON_CLASS)
		{
			DWORD dwStyle = pWnd->GetStyle ();

			if (dwStyle & BS_GROUPBOX)
			{
				if (m_bVisualManagerStyle)
				{
					pDC->SetTextColor (afxGlobalData.clrBarText);
					pDC->SetBkMode(TRANSPARENT);
					return (HBRUSH) visualManager->GetDlgBackBrush (&m_Dlg).GetSafeHandle ();
				}
			}

			if ((dwStyle & BS_CHECKBOX) == 0)
			{
				pDC->SetBkMode(TRANSPARENT);
			}

			return (HBRUSH) ::GetStockObject (m_bIsWhiteBackground ? WHITE_BRUSH : HOLLOW_BRUSH);
		}
	}

	return NULL;
}
//*********************************************************************************************************
BOOL CBORROW_DlgImpl::OnNcPaint ()
{
/*
#ifndef _BCGSUITE_
	if (afxGlobalData.m_bInSettingsChange || !IsOwnerDrawCaption ())
	{
		return FALSE;
	}
#else
*/
	if (!IsOwnerDrawCaption ())
	{
		return FALSE;
	}
// #endif

	return visualManager->OnNcPaint (&m_Dlg,
		m_lstCaptionSysButtons, m_rectRedraw);
}
//********************************************************************************
BOOL CBORROW_DlgImpl::OnNcCalcSize(BOOL /*bCalcValidRects*/, NCCALCSIZE_PARAMS FAR* lpncsp)
{
	ASSERT (lpncsp != NULL);

	if ((m_Dlg.GetStyle () & WS_CAPTION) == 0)
	{
		m_bVisualManagerNCArea = FALSE;
	}

	if (IsOwnerDrawCaption ())
	{
		lpncsp->rgrc[0].top += GetCaptionHeight ();
	}

	return (m_Dlg.GetStyle () & WS_MAXIMIZE) == WS_MAXIMIZE && IsOwnerDrawCaption ();
}
//********************************************************************************
CRect CBORROW_DlgImpl::GetCaptionRect ()
{
	CSize szSystemBorder (::GetSystemMetrics (SM_CXSIZEFRAME), ::GetSystemMetrics (SM_CYSIZEFRAME));

	if (m_Dlg.IsIconic () || 
		(m_Dlg.GetStyle () & WS_MAXIMIZE) == WS_MAXIMIZE)
	{
		szSystemBorder = CSize (0, 0);
	}
	else if ((m_Dlg.GetStyle () & WS_THICKFRAME) == 0)
	{
		szSystemBorder = CSize (::GetSystemMetrics (SM_CXFIXEDFRAME), ::GetSystemMetrics (SM_CXFIXEDFRAME));
	}
	

	CRect rectWnd;
	m_Dlg.GetWindowRect (&rectWnd);

	m_Dlg.ScreenToClient (&rectWnd);

	int cyOffset = szSystemBorder.cy;
	if (!m_Dlg.IsIconic ())
	{
		cyOffset += GetCaptionHeight ();
	}

	rectWnd.OffsetRect (szSystemBorder.cx, cyOffset);

	CRect rectCaption (	rectWnd.left + szSystemBorder.cx, 
						rectWnd.top + szSystemBorder.cy, 
						rectWnd.right - szSystemBorder.cx, 
						rectWnd.top + szSystemBorder.cy + GetCaptionHeight ());

	if (m_Dlg.IsIconic ())
	{
		rectCaption.top += ::GetSystemMetrics (SM_CYSIZEFRAME);
		rectCaption.right -= ::GetSystemMetrics (SM_CXSIZEFRAME);
	}

	return rectCaption;
}
//*************************************************************************
void CBORROW_DlgImpl::UpdateCaption ()
{
	if (!IsOwnerDrawCaption ())
	{
		return;
	}

	if (m_lstCaptionSysButtons.IsEmpty ())
	{
		//------------------------
		// Create caption buttons:
		//------------------------
		const DWORD dwStyle = m_Dlg.GetStyle ();
		HMENU hSysMenu = NULL;
		CMenu* pSysMenu = m_Dlg.GetSystemMenu (FALSE);

		if (pSysMenu != NULL && ::IsMenu (pSysMenu->m_hMenu))
		{
			hSysMenu = pSysMenu->GetSafeHmenu ();
			if (!::IsMenu (hSysMenu) || (m_Dlg.GetStyle () & WS_SYSMENU) == 0)
			{
				hSysMenu = NULL;
			}
		}

		if (hSysMenu != NULL)
		{
			m_lstCaptionSysButtons.AddTail (new CMFCCaptionButtonEx (AFX_HTCLOSE));

			if ((dwStyle & WS_MAXIMIZEBOX) == WS_MAXIMIZEBOX)
			{
				m_lstCaptionSysButtons.AddTail (new CMFCCaptionButtonEx (AFX_HTMAXBUTTON));
			}

			if ((dwStyle & WS_MINIMIZEBOX) == WS_MINIMIZEBOX)
			{
				m_lstCaptionSysButtons.AddTail (new CMFCCaptionButtonEx (AFX_HTMINBUTTON));
			}

/*
#ifndef _BCGSUITE_
			if ((dwStyle & DS_CONTEXTHELP) == DS_CONTEXTHELP)
			{
				if (!m_Dlg.IsKindOf (RUNTIME_CLASS (CPropertySheet)))
				{
					m_lstCaptionSysButtons.AddTail (new CMFCCaptionButtonEx (HTHELPBUTTON_BCG));
				}
			}
#endif
*/
		}
	}

	CRect rectCaption = GetCaptionRect ();

// #ifndef _BCGSUITE_
    // TODO:
    //if (!visualManager->OnUpdateNcButtons(&m_Dlg, m_lstCaptionSysButtons, rectCaption))
// #endif
    {
	    CSize sizeButton = visualManager->GetNcBtnSize (FALSE);
	    sizeButton.cy = min (sizeButton.cy, rectCaption.Height () - 2);

	    int x = rectCaption.right - sizeButton.cx;
	    int y = rectCaption.top + max (0, (rectCaption.Height () - sizeButton.cy) / 2);

	    for (POSITION pos = m_lstCaptionSysButtons.GetHeadPosition (); pos != NULL;)
	    {
		    CMFCCaptionButtonEx* pButton = (CMFCCaptionButtonEx*)
			    m_lstCaptionSysButtons.GetNext (pos);
		    ASSERT_VALID (pButton);

		    pButton->SetRect (CRect (CPoint (x, y), sizeButton));

		    x -= sizeButton.cx;
	    }
    }

/*
#ifndef _BCGSUITE_
    m_Dlg.SendMessage (BCGM_ONAFTERUPDATECAPTION);
#endif
*/

	m_Dlg.RedrawWindow (NULL, NULL,
		RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
}
//*************************************************************************
void CBORROW_DlgImpl::UpdateCaptionButtons()
{
	if ((m_Dlg.GetStyle () & WS_SYSMENU) == 0)
	{
		return;
	}

	CMenu* pSysMenu = m_Dlg.GetSystemMenu (FALSE);

	if (pSysMenu == NULL || !::IsMenu (pSysMenu->m_hMenu))
	{
		return;
	}

	for (POSITION pos = m_lstCaptionSysButtons.GetHeadPosition (); pos != NULL;)
	{
		CMFCCaptionButtonEx* pButton = (CMFCCaptionButtonEx*)m_lstCaptionSysButtons.GetNext (pos);
		ASSERT_VALID (pButton);

		if (pButton->GetHit () == AFX_HTCLOSE)
		{
			BOOL bGrayed = pSysMenu->GetMenuState (SC_CLOSE, MF_BYCOMMAND) & MF_GRAYED;
			pButton->m_bEnabled = bGrayed ? FALSE : TRUE;
		}

		if (pButton->GetHit () == AFX_HTMAXBUTTON)
		{
			BOOL bGrayed = pSysMenu->GetMenuState (SC_MAXIMIZE, MF_BYCOMMAND) & MF_GRAYED;
			pButton->m_bEnabled = bGrayed ? FALSE : TRUE;
		}

		if (pButton->GetHit () == AFX_HTMINBUTTON)
		{
			BOOL bGrayed = pSysMenu->GetMenuState (SC_MINIMIZE, MF_BYCOMMAND) & MF_GRAYED;
			pButton->m_bEnabled = bGrayed ? FALSE : TRUE;
		}
	}

/*
#ifndef _BCGSUITE_
	m_Dlg.SendMessage (BCGM_ONAFTERUPDATECAPTION);
#endif
*/

	m_Dlg.RedrawWindow (NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
}
//*************************************************************************
UINT CBORROW_DlgImpl::OnNcHitTest (CPoint point)
{
	m_Dlg.ScreenToClient (&point);

	if (!m_rectResizeBox.IsRectEmpty ())
	{
		if (m_rectResizeBox.PtInRect(point))
		{
			BOOL bRTL = m_Dlg.GetExStyle() & WS_EX_LAYOUTRTL;
			return bRTL ? HTBOTTOMLEFT : HTBOTTOMRIGHT;
		}
	}

	if (!IsOwnerDrawCaption ())
	{
		return HTNOWHERE;
	}

	const CSize szSystemBorder(::GetSystemMetrics (SM_CXSIZEFRAME), ::GetSystemMetrics (SM_CYSIZEFRAME));

	int cxOffset = szSystemBorder.cx;
	int cyOffset = szSystemBorder.cy;
	if (!m_Dlg.IsIconic ())
	{
		cyOffset += GetCaptionHeight ();
	}

	if (m_Dlg.IsZoomed ())
	{
		cxOffset -= szSystemBorder.cx;
		cyOffset -= szSystemBorder.cy;
	}

	point.Offset (cxOffset, cyOffset);

	for (POSITION pos = m_lstCaptionSysButtons.GetHeadPosition (); pos != NULL;)
	{
		CMFCCaptionButtonEx* pButton = (CMFCCaptionButtonEx*)
			m_lstCaptionSysButtons.GetNext (pos);
		ASSERT_VALID (pButton);

		if (pButton->GetRect ().PtInRect (point))
		{
			return pButton->m_nHit;
		}
	}

	CRect rectCaption = GetCaptionRect ();
	if (rectCaption.PtInRect (point))
	{
		if ((m_Dlg.GetExStyle () & WS_EX_TOOLWINDOW) == 0)
		{
			CRect rectSysMenu = rectCaption;
			rectSysMenu.right = rectSysMenu.left + ::GetSystemMetrics (SM_CXSMICON) + 2 +
				(m_Dlg.IsZoomed () ? szSystemBorder.cx : 0);

			return rectSysMenu.PtInRect (point) ? HTSYSMENU : HTCAPTION;
		}

		return HTCAPTION;
	}

	return HTNOWHERE;
}
//*************************************************************************
void CBORROW_DlgImpl::OnNcMouseMove(UINT /*nHitTest*/, CPoint point)
{
	if (!IsOwnerDrawCaption ())
	{
		return;
	}

	OnTrackCaptionButtons (point);
}
//*************************************************************************
void CBORROW_DlgImpl::OnLButtonDown(CPoint /*point*/)
{
	if (m_nHotSysButton == HTNOWHERE)
	{
		return;
	}

	CMFCCaptionButtonEx* pBtn = GetSysButton (m_nHotSysButton);
	if (pBtn != NULL)
	{
		m_nHitSysButton = m_nHotSysButton;
		pBtn->m_bPushed = TRUE;
		RedrawCaptionButton (pBtn);
	}
}
//*************************************************************************
void CBORROW_DlgImpl::OnLButtonUp(CPoint /*point*/)
{
	if (!IsOwnerDrawCaption ())
	{
		return;
	}

	switch (m_nHitSysButton)
	{
	case AFX_HTCLOSE:
	case AFX_HTMAXBUTTON:
	case AFX_HTMINBUTTON:
/*
#ifndef _BCGSUITE_
	case HTHELPBUTTON_BCG:
#endif
*/
		{
			UINT nHot = m_nHotSysButton;
			UINT nHit = m_nHitSysButton;

			StopCaptionButtonsTracking ();

			if (nHot == nHit)
			{
				UINT nSysCmd = 0;

				switch (nHot)
				{
				case AFX_HTCLOSE:
					nSysCmd = SC_CLOSE;
					break;

				case AFX_HTMAXBUTTON:
					nSysCmd = 
						(m_Dlg.GetStyle () & WS_MAXIMIZE) == WS_MAXIMIZE ? SC_RESTORE : SC_MAXIMIZE;
					break;

				case AFX_HTMINBUTTON:
					nSysCmd = m_Dlg.IsIconic () ? SC_RESTORE : SC_MINIMIZE;
					break;

/*
#ifndef _BCGSUITE_
				case HTHELPBUTTON_BCG:
					nSysCmd = SC_CONTEXTHELP;
					break;
#endif
*/
				}

				m_Dlg.PostMessage (WM_SYSCOMMAND, nSysCmd);
			}
		}
	}
}
//*************************************************************************
void CBORROW_DlgImpl::OnMouseMove(CPoint point)
{
	if (!IsOwnerDrawCaption ())
	{
		return;
	}

	CPoint ptScreen = point;
	m_Dlg.ClientToScreen (&ptScreen);

	OnTrackCaptionButtons (ptScreen);
}
//*************************************************************************
CMFCCaptionButtonEx* CBORROW_DlgImpl::GetSysButton (UINT nHit)
{
	for (POSITION pos = m_lstCaptionSysButtons.GetHeadPosition (); pos != NULL;)
	{
		CMFCCaptionButtonEx* pButton = (CMFCCaptionButtonEx*)
			m_lstCaptionSysButtons.GetNext (pos);
		ASSERT_VALID (pButton);

		if (pButton->m_nHit == nHit)
		{
			return pButton;
		}
	}

	return NULL;
}
//*************************************************************************
void CBORROW_DlgImpl::SetHighlightedSysButton (UINT nHit)
{
	BOOL bRedraw = FALSE;

	for (POSITION pos = m_lstCaptionSysButtons.GetHeadPosition (); pos != NULL;)
	{
		CMFCCaptionButtonEx* pButton = (CMFCCaptionButtonEx*)
			m_lstCaptionSysButtons.GetNext (pos);
		ASSERT_VALID (pButton);

		if (pButton->m_nHit == nHit)
		{
			if (pButton->m_bFocused)
			{
				return;
			}

			pButton->m_bFocused = TRUE;
			bRedraw = TRUE;
		}
	}
}
//*************************************************************************************
void CBORROW_DlgImpl::OnTrackCaptionButtons (CPoint point)
{
	if (CMFCPopupMenu::GetActiveMenu () != NULL)
	{
		return;
	}

	UINT nHot = m_nHotSysButton;
	CMFCCaptionButtonEx* pBtn = GetSysButton (OnNcHitTest (point));

	if (pBtn != NULL && pBtn->m_bEnabled)
	{
		m_nHotSysButton = pBtn->GetHit ();
		pBtn->m_bFocused = TRUE;
	}
	else
	{
		m_nHotSysButton = HTNOWHERE;
	}

	if (m_nHotSysButton != nHot)
	{
		RedrawCaptionButton (pBtn);

		CMFCCaptionButtonEx* pBtnOld = GetSysButton (nHot);
		if (pBtnOld != NULL)
		{
			pBtnOld->m_bFocused = FALSE;
			RedrawCaptionButton (pBtnOld);
		}
	}

	if (m_nHitSysButton == HTNOWHERE)
	{
		if (nHot != HTNOWHERE && m_nHotSysButton == HTNOWHERE)
		{
			::ReleaseCapture();
		}
		else if (nHot == HTNOWHERE && m_nHotSysButton != HTNOWHERE)
		{
			m_Dlg.SetCapture ();
		}
	}
}
//************************************************************************************
void CBORROW_DlgImpl::StopCaptionButtonsTracking ()
{
	if (m_nHitSysButton != HTNOWHERE)
	{
		CMFCCaptionButtonEx* pBtn = GetSysButton (m_nHitSysButton);
		m_nHitSysButton = HTNOWHERE;

		ReleaseCapture ();
		if (pBtn != NULL)
		{
			pBtn->m_bPushed = FALSE;
			RedrawCaptionButton (pBtn);
		}
	}

	if (m_nHotSysButton != HTNOWHERE)
	{
		CMFCCaptionButtonEx* pBtn = GetSysButton (m_nHotSysButton);
		m_nHotSysButton = HTNOWHERE;

		ReleaseCapture ();
		if (pBtn != NULL)
		{
			pBtn->m_bFocused = FALSE;
			RedrawCaptionButton (pBtn);
		}
	}
}
//************************************************************************************
void CBORROW_DlgImpl::RedrawCaptionButton (CMFCCaptionButtonEx* pBtn)
{
	if (pBtn ==	NULL)
	{
		return;
	}

	ASSERT_VALID (pBtn);

	m_rectRedraw = pBtn->GetRect ();
	m_Dlg.SendMessage (WM_NCPAINT);
	m_rectRedraw.SetRectEmpty ();

	m_Dlg.UpdateWindow ();
}
//********************************************************************************
void CBORROW_DlgImpl::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos)
{
	if (m_bWindowPosChanging)
	{
		return;
	}

	if (((lpwndpos->flags & SWP_NOSIZE) == 0 || (lpwndpos->flags & SWP_FRAMECHANGED)) && 
		(IsOwnerDrawCaption ()))
	{
		m_bWindowPosChanging = TRUE;

		m_bIsWindowRgn = visualManager->OnSetWindowRegion (
			&m_Dlg, CSize (lpwndpos->cx, lpwndpos->cy));

		m_bWindowPosChanging = FALSE;
	}
}
//************************************************************************************
void CBORROW_DlgImpl::OnChangeVisualManager ()
{
	if (m_Dlg.GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rectWindow;
	m_Dlg.GetWindowRect (rectWindow);

	BOOL bZoomed = m_Dlg.IsZoomed ();

	if (IsOwnerDrawCaption ())
	{
		BOOL bChangeBorder = FALSE;

		if ((m_Dlg.GetStyle () & WS_BORDER) == WS_BORDER && m_bHasBorder)
		{
			bChangeBorder = TRUE;
			m_bWindowPosChanging = TRUE;
			m_Dlg.ModifyStyle (WS_BORDER, 0, SWP_FRAMECHANGED);
			m_bWindowPosChanging = FALSE;
		}

		m_bIsWindowRgn = visualManager->OnSetWindowRegion (
			&m_Dlg, rectWindow.Size ());

		if (bZoomed && bChangeBorder)
		{
			m_Dlg.ShowWindow (SW_MINIMIZE);
			m_Dlg.ShowWindow (SW_MAXIMIZE);
		}
	}
	else
	{
		BOOL bChangeBorder = FALSE;

		if ((m_Dlg.GetStyle () & WS_BORDER) == 0 && m_bHasBorder)
		{
			bChangeBorder = TRUE;
			m_bWindowPosChanging = TRUE;
			m_Dlg.ModifyStyle (0, WS_BORDER, SWP_FRAMECHANGED);
			m_bWindowPosChanging = FALSE;
		}

		if (m_bIsWindowRgn)
		{
			m_bIsWindowRgn = FALSE;
			m_Dlg.SetWindowRgn (NULL, TRUE);
		}

		if (bZoomed && bChangeBorder)
		{
			NCCALCSIZE_PARAMS params;
			ZeroMemory(&params, sizeof (NCCALCSIZE_PARAMS));
			params.rgrc[0].left   = rectWindow.left;
			params.rgrc[0].top    = rectWindow.top;
			params.rgrc[0].right  = rectWindow.right;
			params.rgrc[0].bottom = rectWindow.bottom;

			m_Dlg.CalcWindowRect (&params.rgrc[0], CFrameWnd::adjustBorder);

			if ((m_Dlg.GetStyle () & WS_CAPTION) == WS_CAPTION)
			{
				params.rgrc[0].top += GetCaptionHeight ();
			}

			m_Dlg.SetWindowPos (NULL, params.rgrc[0].left, params.rgrc[0].top, 
				params.rgrc[0].right - params.rgrc[0].left, params.rgrc[0].bottom - params.rgrc[0].top,
				SWP_NOACTIVATE | SWP_NOZORDER);
		}
		else
		{
			m_Dlg.SetWindowPos (NULL, -1, -1, rectWindow.Width () + 1, rectWindow.Height () + 1,
				SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
			m_Dlg.SetWindowPos (NULL, -1, -1, rectWindow.Width (), rectWindow.Height (),
				SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
		}
	}

	UpdateCaption ();
	UpdateCaptionButtons();
}
//**********************************************************************************************
int CBORROW_DlgImpl::GetCaptionHeight ()
{
	if (m_Dlg.GetSafeHwnd () == NULL)
	{
		return 0;
	}

	if (m_Dlg.GetExStyle () & WS_EX_TOOLWINDOW)
	{
		return ::GetSystemMetrics (SM_CYSMCAPTION);
	}
	
	return ::GetSystemMetrics (SM_CYCAPTION);
}
//********************************************************************************
void CBORROW_DlgImpl::OnGetMinMaxInfo (MINMAXINFO FAR* lpMMI)
{
	ASSERT (lpMMI != NULL);

	if ((m_Dlg.GetStyle () & WS_CAPTION) == 0 ||
		(m_Dlg.GetStyle () & WS_BORDER) == 0)
	{
		CRect rectWindow;
		m_Dlg.GetWindowRect (&rectWindow);

		if (m_Dlg.IsIconic ())
		{
			WINDOWPLACEMENT wp;
			wp.length = sizeof (WINDOWPLACEMENT);

			m_Dlg.GetWindowPlacement (&wp);

			rectWindow = wp.rcNormalPosition;
		}

		CRect rect (0, 0, 0, 0);

		MONITORINFO mi;
		mi.cbSize = sizeof (MONITORINFO);

		if (GetMonitorInfo (MonitorFromPoint (rectWindow.CenterPoint (), MONITOR_DEFAULTTONEAREST),
			&mi))
		{
			CRect rectWork = mi.rcWork;
			CRect rectScreen = mi.rcMonitor;

			rect.left = rectWork.left - rectScreen.left;
			rect.top = rectWork.top - rectScreen.top;

			rect.right = rect.left + rectWork.Width ();
			rect.bottom = rect.top + rectWork.Height ();
		}
		else
		{
			::SystemParametersInfo (SPI_GETWORKAREA, 0, &rect, 0);
		}

/*
#ifdef _BCGSUITE_
		int nShellAutohideBars = afxGlobalData.m_nShellAutohideBars;
#else
*/
		int nShellAutohideBars = afxGlobalData.GetShellAutohideBars ();
// #endif

		if (nShellAutohideBars & AFX_AUTOHIDE_BOTTOM)
		{
			rect.bottom -= 2;
		}

		if (nShellAutohideBars & AFX_AUTOHIDE_TOP)
		{
			rect.top += 2;
		}

		if (nShellAutohideBars & AFX_AUTOHIDE_RIGHT)
		{
			rect.right -= 2;
		}

		if (nShellAutohideBars & AFX_AUTOHIDE_LEFT)
		{
			rect.left += 2;
		}

		lpMMI->ptMaxPosition.x = rect.left;
		lpMMI->ptMaxPosition.y = rect.top;
		lpMMI->ptMaxSize.x = rect.Width ();
		lpMMI->ptMaxSize.y = rect.Height ();
	}

/*
	if (m_pLayout != NULL && 
		m_LayoutMMI.ptMinTrackSize.x > 0 && m_LayoutMMI.ptMinTrackSize.y > 0)
	{
		lpMMI->ptMinTrackSize = m_LayoutMMI.ptMinTrackSize;
	}
*/
}
//********************************************************************************
/*
void CBORROW_DlgImpl::EnableLayout(BOOL bEnable, CRuntimeClass* pRTC, BOOL bResizeBox)
{

	if (m_pLayout != NULL)
	{
		delete m_pLayout;
		m_pLayout = NULL;
	}


	if (!bEnable)
	{
		return;
	}

	if (pRTC == NULL)
	{
		pRTC = RUNTIME_CLASS (CBCGPStaticLayout);
	}
	else if (!pRTC->IsDerivedFrom (RUNTIME_CLASS (CBCGPControlsLayout)))
	{
		ASSERT(FALSE);
		return;
	}

	m_pLayout = (CBCGPControlsLayout*)pRTC->CreateObject ();
	m_bResizeBox = bResizeBox;
}
*/
//********************************************************************************
int CBORROW_DlgImpl::OnCreate()
{
/*
	if (m_pLayout == NULL)
	{
		return 0;
	}

	ASSERT_VALID(m_pLayout);

	if (!m_pLayout->Create(&m_Dlg))
	{
		delete m_pLayout;
		m_pLayout = NULL;
		return -1;
	}
*/

	if (afxGlobalData.m_bIsRTL)
	{
		m_Dlg.ModifyStyleEx (0, WS_EX_LAYOUTRTL);
	}

	const DWORD dwStyle = m_Dlg.GetStyle ();

	if (dwStyle & WS_CHILD)
	{
		m_bResizeBox = FALSE;
	}

	CBORROW_Dialog* pDialog = DYNAMIC_DOWNCAST(CBORROW_Dialog, &m_Dlg);
	CMFCPropertySheet* pPropSheet = DYNAMIC_DOWNCAST(CMFCPropertySheet, &m_Dlg);

	if (pDialog != NULL || pPropSheet != NULL)
	{
		if ((dwStyle & WS_CHILD) != WS_CHILD)
		{
			CRect rect;
			m_Dlg.GetClientRect(&rect);

			m_Dlg.ModifyStyle(DS_MODALFRAME, WS_POPUP | WS_THICKFRAME);

			::AdjustWindowRectEx(&rect, m_Dlg.GetStyle (), 
				::IsMenu(m_Dlg.GetMenu()->GetSafeHmenu()), m_Dlg.GetExStyle ());

			if (m_bVisualManagerNCArea && IsOwnerDrawCaption() && !afxGlobalData.DwmIsCompositionEnabled ())
			{
				int nHeight = ::GetSystemMetrics (SM_CYCAPTION);
				rect.top -= nHeight / 2;
				rect.bottom += nHeight - nHeight / 2;
			}

			m_Dlg.SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_FRAMECHANGED | 
				SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

			if (pPropSheet == NULL)
			{
				m_Dlg.GetWindowRect (rect);
				m_LayoutMMI.ptMinTrackSize.x = rect.Width ();
				m_LayoutMMI.ptMinTrackSize.y = rect.Height ();
			}
		}
	}

	if (pPropSheet == NULL && DYNAMIC_DOWNCAST(CMFCPropertyPage, &m_Dlg) == NULL)
	{
		CRect rect;
		m_Dlg.GetClientRect (rect);
// 		m_pLayout->SetMinSize (rect.Size ());
	}

	return 0;
}
//********************************************************************************
/*
void CBORROW_DlgImpl::AdjustControlsLayout()
{
	if (m_pLayout != NULL)
	{
		BOOL bUpdate = FALSE;

		if (m_bResizeBox)
		{
			if (!m_rectResizeBox.IsRectEmpty ())
			{
				m_Dlg.InvalidateRect(m_rectResizeBox);
				bUpdate = TRUE;
			}

			CRect rectClient;
			m_Dlg.GetClientRect(rectClient);

			m_rectResizeBox = rectClient;
			m_rectResizeBox.left = m_rectResizeBox.right - ::GetSystemMetrics(SM_CXVSCROLL);
			m_rectResizeBox.top = m_rectResizeBox.bottom - ::GetSystemMetrics(SM_CYHSCROLL);

			if (!m_rectResizeBox.IsRectEmpty ())
			{
				m_Dlg.InvalidateRect(m_rectResizeBox);
				bUpdate = TRUE;
			}
		}
		else
		{
			if (!m_rectResizeBox.IsRectEmpty ())
			{
				m_rectResizeBox.SetRectEmpty();
				bUpdate = TRUE;
			}
		}

		if (bUpdate)
		{
			m_Dlg.UpdateWindow();
		}

		m_pLayout->AdjustLayout();
	}
}
*/
//********************************************************************************
void CBORROW_DlgImpl::DrawResizeBox(CDC* pDC)
{
	if (!m_rectResizeBox.IsRectEmpty())
	{
/*
#ifndef _BCGSUITE_
		visualManager->OnDrawDlgSizeBox (pDC, &m_Dlg, m_rectResizeBox);
#else
*/
		visualManager->OnDrawStatusBarSizeBox (pDC, NULL, m_rectResizeBox);
// #endif
	}
}
//****************************************************************************
void CBORROW_DlgImpl::EnableBackstageMode()
{
	m_bBackstageMode = TRUE;
	m_bTransparentStaticCtrls = FALSE;
	m_bIsWhiteBackground = TRUE;

	if (m_Dlg.GetSafeHwnd () == NULL)
	{
		return;
	}

	CWnd* pWndChild = m_Dlg.GetWindow (GW_CHILD);

	while (pWndChild != NULL)
	{
		ASSERT_VALID (pWndChild);

		pWndChild->SendMessage (BCGM_ONSETCONTROLBACKSTAGEMODE);
		pWndChild = pWndChild->GetNextWindow ();
	}
}
