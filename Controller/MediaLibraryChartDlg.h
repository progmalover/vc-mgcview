#pragma once


// CUserMonitorChartDlg �Ի���

class CMediaLibraryChartDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMediaLibraryChartDlg)

public:
	CMediaLibraryChartDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMediaLibraryChartDlg();

// �Ի�������
	enum { IDD = IDD_CLIENT_MONITOR_CHART };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
    virtual void OnCancel();
    virtual void OnOK();
public:
    afx_msg void OnClose();
};
