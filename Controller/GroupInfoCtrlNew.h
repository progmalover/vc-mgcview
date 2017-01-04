#pragma once


// CGroupInfoCtrlNew dialog

class CGroupInfoCtrlNew : public CDialogEx
{
	DECLARE_DYNAMIC(CGroupInfoCtrlNew)

public:
	CGroupInfoCtrlNew(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGroupInfoCtrlNew();

// Dialog Data
	enum { IDD = IDD_GROUP_INFO_NEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	ClientGroup                 m_CurGroup;
	INT                         m_InitGroupID;

	DECLARE_MESSAGE_MAP()
public:
	virtual void SetCurGroupID(int groupid); 

	virtual BOOL SetCurGroup(const ClientGroup& group);
	virtual const ClientGroup& GetCurGroup() const {return m_CurGroup;}

	afx_msg void OnBnClickedOk();
	CString m_SelectedGroupName;
	CString m_SelectedGroupDesc;


	afx_msg void OnEnChangeEditGname();
	afx_msg void OnEnChangeEditGdesc();
	BOOL      m_bModified;

	int m_volume;
	COleDateTime m_startTime;
	COleDateTime m_endTime;
	afx_msg void OnDtnDatetimechangeDatetimepickerFrom(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepickerTo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderGroupVolume(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckAlwaysWork();
	BOOL m_bAlwaysWork;
};
