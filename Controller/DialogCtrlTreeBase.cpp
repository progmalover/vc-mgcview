#include "StdAfx.h"
#include "DialogCtrlTreeBase.h"
#include "Resource.h"

IMPLEMENT_DYNAMIC(CDialogCtrlTreeBase,CDialogEx)

BEGIN_MESSAGE_MAP(CDialogCtrlTreeBase,CDialogEx)
ON_NOTIFY(TVN_BEGINLABELEDIT ,IDC_MEDIALIB_TREE, OnTvnBeginlabelEdit)
//ON_NOTIFY(TVN_BEGINLABELEDIT ,IDC_MEDIALIB_TREE, OnTvnBeginlabeledit)
//ON_NOTIFY(TVN_BEGINLABELEDIT ,IDC_MEDIALIB_TREE, OnTvnBeginlabeledit)
END_MESSAGE_MAP()

 
CDialogCtrlTreeBase::CDialogCtrlTreeBase(void)
{
	m_pTree = NULL;
}

CDialogCtrlTreeBase::CDialogCtrlTreeBase( UINT nIDTemplate, CWnd *pParent /*= NULL*/ )
    : CDialogEx(nIDTemplate, pParent)
{
	m_pTree = NULL;
}


CDialogCtrlTreeBase::~CDialogCtrlTreeBase(void)
{
}


void CDialogCtrlTreeBase::OnTvnBeginlabelEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	  ASSERT(pNMHDR != NULL);
	  LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	  if(NULL == pTVDispInfo)
		  return;
	  if(m_pTree == NULL)
		  m_pTree = (CPrivateTreeCtrl *)CTreeCtrl::FromHandle(pTVDispInfo->hdr.hwndFrom);
	  if(!m_pTree->IsKindOf(RUNTIME_CLASS(CTreeCtrl)))
	  {
		  m_pTree = NULL;
		  return;
	  }
	  m_csItemStr = m_pTree->GetItemText(pTVDispInfo->item.hItem);
}

BOOL CDialogCtrlTreeBase::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
#if 0
	if(pMsg->message >= WM_MOUSEFIRST || pMsg->message <= WM_MOUSELAST)
	{
		if(
			pMsg->message == WM_NCHITTEST )
			goto L_END;

		CPoint pt;
		::GetCursorPos(&pt);
		CWnd *pWnd = CWnd ::WindowFromPoint(pt);

		if(m_pTree && m_pTree->GetWindow(GW_CHILD)) //m_pTree´¦ÓÚ±à¼­×´Ì¬
			goto L_END;
		if(pWnd != CWnd ::GetFocus() && this != CWnd::GetFocus())
		{
			//pWnd->SetActiveWindow();
			if( 
			    !pWnd->IsKindOf(RUNTIME_CLASS(CMFCButton))&&
				 !pWnd->IsKindOf(RUNTIME_CLASS(CButton)) &&
				 !pWnd->IsKindOf(RUNTIME_CLASS(CMFCRibbonBar))
				)
			{
				pWnd->SetFocus();
			}
			//TRACE("Get Mouse Message here!\n");
		}
	}
#endif
	if(m_pTree)
	if(pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{		
		
		if(pMsg->wParam == VK_ESCAPE)
		{
			if(m_csItemStr.IsEmpty() && !m_pTree)
				return TRUE;

			CWnd *pWnd = m_pTree->GetWindow(GW_CHILD);
			if(!pWnd)return TRUE;
			pWnd->SetWindowText(m_csItemStr);

			m_pTree->PostMessage(WM_LBUTTONDOWN,0,0);
			//m_csItemStr.Empty();
			//return TRUE;
		}
		if(pMsg->wParam == VK_RETURN)
		{
			CWnd *pWnd = m_pTree->GetWindow(GW_CHILD);
			if(!pWnd)return TRUE;
			CString csEditText;
			pWnd->GetWindowText(csEditText);
			if(csEditText.IsEmpty())
				pWnd->SetWindowText(m_csItemStr);

			m_pTree->PostMessage(WM_LBUTTONDOWN,0,0);
			//return TRUE;
		}
	}
	L_END:
	return __super::PreTranslateMessage(pMsg);
}
