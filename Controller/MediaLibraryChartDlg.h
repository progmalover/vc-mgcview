#pragma once


// CUserMonitorChartDlg 对话框

class CMediaLibraryChartDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMediaLibraryChartDlg)

public:
	CMediaLibraryChartDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMediaLibraryChartDlg();

// 对话框数据
	enum { IDD = IDD_CLIENT_MONITOR_CHART };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
    virtual void OnCancel();
    virtual void OnOK();
public:
    afx_msg void OnClose();
};
