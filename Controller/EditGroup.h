#pragma once


// CEditGroup dialog

class CEditGroup : public CDialogEx
{
	DECLARE_DYNAMIC(CEditGroup)

public:
	CEditGroup(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditGroup();
	BOOL InitGroup(INT GroupID);
	// Dialog Data
	enum { IDD = IDD_EDIT_GROUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
protected:
	BOOL	  GetGroupInfo(ClientGroup &GroupInfo);
public:
	ClientGroup                 m_CurGroup;
	CString						m_GroupName;
	CString						m_GroupDesc;
	INT                         m_InitGroupID;
public:

	virtual BOOL				OnInitDialog();
	const ClientGroup&			GetCurGroup() const {return m_CurGroup;}
	afx_msg void				OnBnClickedOk();

};
