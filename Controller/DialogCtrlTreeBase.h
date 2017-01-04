#pragma once
#include "afxdialogex.h"
class CDialogCtrlTreeBase :
	public CDialogEx
{
	class CPrivateTreeCtrl :public CTreeCtrl
	{
		friend class CDialogCtrlTreeBase;
	};
public:
	DECLARE_DYNAMIC(CDialogCtrlTreeBase);
	DECLARE_MESSAGE_MAP();

	CDialogCtrlTreeBase(void);
	CDialogCtrlTreeBase( UINT nIDTemplate, CWnd *pParent /*= NULL*/ );
	~CDialogCtrlTreeBase(void);
   virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	CPrivateTreeCtrl *m_pTree;
	CString m_csItemStr;
protected:
	afx_msg void OnTvnBeginlabelEdit(NMHDR *pNMHDR,LRESULT *pResult);
};

