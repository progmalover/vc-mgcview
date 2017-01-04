#pragma once

#include "Graph.h"
#include "GraphSeries.h"
#include "GraphLegend.h"
#include "afxdtctl.h"
#include "afxwin.h"
#include "atlcomtime.h"
// CUserMonitorChartDlg 

#ifndef _OLD_CLIENT_CHART_VER_

enum ENUM_DATATYPE
{
	DATATYPE_CPU_USAGE=0,
	DATATYPE_MEM_USAGE,
	DATATYPE_DISK_USAGE,
	DATATYPE_FPS,
	DATATYPE_FAN_SPEED,
	DATATYPE_CPU_TEMP,
	DATATYPE_MOTHERBOARD_TEMP,
	DATATYPE_HDD_TEMP,
	DATATYPE_GPU_TEMP,
	DATATYPE_MAX_NUM,
};

class CClientChartDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CClientChartDlg)
	DECLARE_EASYSIZE
public:
	CClientChartDlg(CWnd* pParent = NULL);   //
	virtual ~CClientChartDlg();

// 
	enum { IDD = IDD_CLIENT_MONITOR_CHART1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
    virtual void OnCancel();
    virtual void OnOK();
private:
	int m_nInitFlag;

	int m_StartY;     //start display y position 
	VOID CalcLayout();

	int m_ClientId;   //client id 

	int m_nRecordNum;  //Record number

	int m_nScale;      

	int m_dcWidth;
	int m_dcHeight;
	CDC dcMem;        // memory dc 
	CBitmap bmp; 

	void DrawGraphicBackBuffer(); //draw chart to graphic back buffer 
	void UpdateGraphicsDisplay(); //blt graphic back buffer to graphic buffer  

	BOOL bIsRetrievingData;
	void RetrieveGraphicsDataNew(const COleDateTime & starttime, const COleDateTime & endtime );

	void RetrieveGraphicsData();  //
	void ReleaseGraphicsData();   //

	COleDateTimeSpan m_Span;        //time range ,supposed to be 7-days
	std::vector<PerfStatus> m_ClientRTStatisticsList; //data that retrieved from database
	LRESULT UpdatePartialClientHistoryData(COleDateTime starttime,bool bFreshFlag); 

	CGraph * MyGraph[DATATYPE_MAX_NUM];
	
	int GetTypeData(PerfStatus it,int dataTypeID);

	COleDateTime m_CurDataStartTime;         //
	int m_nLastRadioBoxIdx; 
	COleDateTime m_LastRetrieveStartTime;    //
	COleDateTime m_LastRetrieveEndTime;    //
public:
    afx_msg void OnClose();
	afx_msg void OnPaint();

	void SetClientId(int id);
	int GetClientId();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	int m_nCurHScrollBarPos;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	CDateTimeCtrl m_datatimectrl;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg LRESULT UpdateClientHistoryData(WPARAM,LPARAM);
	void RetrieveBufferedData(const COleDateTime & starttime, const COleDateTime & endtime );

	CComboBox m_ComboType;
	afx_msg void OnCbnSelchangeComboType();
	afx_msg void OnBnClickedRadioOneWeek();
	afx_msg void OnBnClickedRadioOneMonth();
	afx_msg void OnBnClickedRadioThreeMonth();
	afx_msg void OnBnClickedRadioUserDefine();
	afx_msg void OnDtnDatetimechangeDatetimepicker4(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepicker5(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnSearch();
	COleDateTime m_starttime;
	COleDateTime m_endtime;
};

#else
enum GRAPH_DATATYPE
{
	CPU_USAGE_DATATYPE=0,
	MEM_USAGE_DATATYPE,
	DISK_USAGE_DATATYPE,
	FPS_DATATYPE,
	DISKFREESPACE_DATATYPE,
};


#define MY_GRAPH_NUM 9

class CClientChartDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CClientChartDlg)
	DECLARE_EASYSIZE
public:
	CClientChartDlg(CWnd* pParent = NULL);   //
	virtual ~CClientChartDlg();

	// 
	enum { IDD = IDD_CLIENT_MONITOR_CHART };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void OnOK();
private:
	int m_nInitFlag;

	//int m_TotalHeight;
	int m_StartY;     //start display y position 
	VOID CalcLayout();

	int m_ClientId;   //client id 

	int m_nRecordNum;  //Record number

	int m_nScale;      //magnification between 1 and 24

	int m_dcWidth;
	int m_dcHeight;
	CDC dcMem;        // memory dc 
	CBitmap bmp; 

	void DrawGraphicBackBuffer(); //draw chart to graphic back buffer 
	void UpdateGraphicsDisplay(); //blt graphic back buffer to graphic buffer  

	void RetrieveGraphicsData();  //
	void ReleaseGraphicsData();   //

	COleDateTimeSpan m_Span;        //time range ,supposed to be 7-days
	std::vector<PerfStatus> m_ClientRTStatisticsList; //data that retrieved from database
	LRESULT UpdatePartialClientHistoryData(COleDateTime starttime,bool bFreshFlag); 

	CGraph * MyGraph[MY_GRAPH_NUM];

	COleDateTime m_CurDataStartTime;         //
	COleDateTime m_LastRetrieveStartTime;         //
public:
	afx_msg void OnClose();
	afx_msg void OnPaint();

	void SetClientId(int id);
	int GetClientId();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	int m_nCurHScrollBarPos;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	CDateTimeCtrl m_datatimectrl;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg LRESULT UpdateClientHistoryData(WPARAM,LPARAM);
};

#endif