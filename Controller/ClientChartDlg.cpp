// UserMonitorChartDlg.cpp : 
//

#include "stdafx.h"
#include "Controller.h"
#include "ClientChartDlg.h"
#include "afxdialogex.h"

#include "Graph.h"
#include "GraphSeries.h"
#include "GraphLegend.h"

#include "DatetimeCheck.h"


#define INVALID_DATA_VAL -1
#ifndef _OLD_CLIENT_CHART_VER_
// CUserMonitorChartDlg

IMPLEMENT_DYNAMIC(CClientChartDlg, CDialogEx)

#define Translate(x) x
CString mDataTypeTitleName[DATATYPE_MAX_NUM] =
{
	Translate(_T("CpuUsage")),
	Translate(_T("MemUsage")),
	Translate(_T("Free Disk")),
	Translate(_T("fps")), 
	Translate(_T("Fan Speed")), 
	Translate(_T("CPU Temperature")),  
	Translate(_T("MotherBoard Temperature")), 
	Translate(_T("HDD Temperature")),  
	Translate(_T("GPU Temperature")),
};
#undef Translate

CClientChartDlg::CClientChartDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientChartDlg::IDD, pParent)
{
	m_nInitFlag = FALSE;
	m_nScale = 1;
	m_nRecordNum = 0;
	m_Span.SetDateTimeSpan(7,0,0,0);

	m_dcWidth = 2000;
	//m_dcHeight = 350 * DATATYPE_MAX_NUM;
	m_dcHeight = 700;

	for(int ii = 0;ii< DATATYPE_MAX_NUM ; ii++)
	{
		MyGraph[ii] = NULL;
	}


}

CClientChartDlg::~CClientChartDlg()
{
	dcMem.DeleteDC(); //delete DC 
	bmp.DeleteObject(); //delete bitmap
}

void CClientChartDlg::SetClientId(int id)
{
	m_ClientId = id;
}

int CClientChartDlg::GetClientId()
{
	return m_ClientId;
}

void CClientChartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	/*DDX_Control(pDX, IDC_DATETIMEPICKER1, m_datatimectrl);*/
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ComboType);
	/*DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER4, m_starttime);*/
	/*DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER5, m_endtime);*/
}


BEGIN_MESSAGE_MAP(CClientChartDlg, CDialogEx)
    ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_BUTTON1, &CClientChartDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CClientChartDlg::OnBnClickedButton3)
	/*ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, &CClientChartDlg::OnDtnDatetimechangeDatetimepicker1)*/

	ON_MESSAGE(USER_MSG_GET_CLIENT_HISTORY, &CClientChartDlg::UpdateClientHistoryData)

	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CClientChartDlg::OnCbnSelchangeComboType)
	ON_BN_CLICKED(IDC_RADIO_ONE_WEEK, &CClientChartDlg::OnBnClickedRadioOneWeek)
	ON_BN_CLICKED(IDC_RADIO_ONE_MONTH, &CClientChartDlg::OnBnClickedRadioOneMonth)
	ON_BN_CLICKED(IDC_RADIO_THREE_MONTH, &CClientChartDlg::OnBnClickedRadioThreeMonth)
	ON_BN_CLICKED(IDC_RADIO_USER_DEFINE, &CClientChartDlg::OnBnClickedRadioUserDefine)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER4, &CClientChartDlg::OnDtnDatetimechangeDatetimepicker4)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER5, &CClientChartDlg::OnDtnDatetimechangeDatetimepicker5)
	ON_BN_CLICKED(IDC_BTN_SEARCH, &CClientChartDlg::OnBnClickedBtnSearch)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CClientChartDlg)
	//EASYSIZE(IDC_STATIC_BACKGROUND,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	//EASYSIZE(IDC_STATIC_FOREGROUND,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_BACKGROUND,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

// CUserMonitorChartDlg 


void CClientChartDlg::OnCancel()
{
}


void CClientChartDlg::OnOK()
{
}


void CClientChartDlg::OnClose()
{
/*
    CClientStateTab* parent = (CClientStateTab*)GetParent();
    parent->RemoveTab(parent->GetActiveTab());
*/
    CDialogEx::OnClose();
}

int CClientChartDlg::GetTypeData(PerfStatus  it,int dataTypeID)
{
	int rtn = INVALID_DATA_VAL;
	int data;
	if(dataTypeID == (int)DATATYPE_CPU_USAGE)
	{
		//rtn = it.GetCpuUsage()%100;
		data = it.GetCpuUsage();
		if(data < 0 || data > 100)
		{

		}
		else
		{
			rtn = data;
		}
	}
	else if(dataTypeID == (int)DATATYPE_MEM_USAGE)
	{
		//rtn = it.GetMemUsage()%100;
		data = it.GetMemUsage();
		if(data < 0 || data > 100)
		{

		}
		else
		{
			rtn = data;
		}
	}
	else if(dataTypeID == (int)DATATYPE_DISK_USAGE)
	{
		//rtn = it.GetHarddiskUsage()%100;
		data = it.GetHarddiskUsage();
		if(data < 0 || data > 100)
		{

		}
		else
		{
			rtn = data;
		}
	}
	else if(dataTypeID == (int)DATATYPE_FPS)
	{
		//rtn = it.GetFPS()%60;
		data = it.GetFPS();
		if(data < 0 || data > 60)
		{

		}
		else
		{
			rtn = data;
		}
	}
	else if(dataTypeID == (int)DATATYPE_FAN_SPEED)
	{
		data = it.GetFanSpeed();
		if(data < 0 || data > 10000)
		{

		}
		else
		{
			rtn = data;
		}
		//rtn = it.GetFanSpeed()%10000;
	}
	else if(dataTypeID == (int)DATATYPE_CPU_TEMP)
	{
		//rtn = it.GetCPUTemperature()%100;
		data = it.GetCPUTemperature();
		if(data < 0 || data > 100)
		{

		}
		else
		{
			rtn = data;
		}
	}
	else if(dataTypeID == (int)DATATYPE_MOTHERBOARD_TEMP)
	{
		//rtn = it.GetMBTemperature()%100;
		data = it.GetMBTemperature();
		if(data < 0 || data > 100)
		{

		}
		else
		{
			rtn = data;
		}
	}
	else if(dataTypeID == (int)DATATYPE_HDD_TEMP)
	{
		//rtn = it.GetHDDTemperature()%100;
		data = it.GetHDDTemperature();
		if(data < 0 || data > 100)
		{

		}
		else
		{
			rtn = data;
		}

	}
	else //if(typeid == (int)DATATYPE_GPU_TEMP)
	{
		//rtn = it.GetGPUTemperature()%100;
		data = it.GetGPUTemperature();
		if(data < 0 || data > 100)
		{

		}
		else
		{
			rtn = data;
		}
	}
	return rtn;
}

LRESULT CClientChartDlg::UpdateClientHistoryData(WPARAM wp,LPARAM lp)
{
	//////////////////////////////////////////////////////////////////////////
	// How to get a buf
	UINT nMessageBufID = (UINT)wp;

	//retrieve data finish
	bIsRetrievingData = false;
	((CButton *)GetDlgItem(IDC_COMBO_TYPE))->EnableWindow(true);
	((CButton *)GetDlgItem(IDC_RADIO_ONE_WEEK))->EnableWindow(true);
	((CButton *)GetDlgItem(IDC_RADIO_ONE_MONTH))->EnableWindow(true);
	((CButton *)GetDlgItem(IDC_RADIO_THREE_MONTH))->EnableWindow(true);
	((CButton *)GetDlgItem(IDC_RADIO_USER_DEFINE))->EnableWindow(true);
	if(m_nLastRadioBoxIdx == 3)
	{
		((CButton *)GetDlgItem(IDC_DATETIMEPICKER4))->EnableWindow(true);
		((CButton *)GetDlgItem(IDC_DATETIMEPICKER5))->EnableWindow(true);
		((CButton *)GetDlgItem(IDC_BTN_SEARCH))->EnableWindow(true);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_DATETIMEPICKER4))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_DATETIMEPICKER5))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_BTN_SEARCH))->EnableWindow(false);
	}
	
	DataBuf< ClientStatus> MessageBuf;
	if (!WorkThreadDataManager< ClientStatus >
		::GetDataBuf(nMessageBufID, MessageBuf))
	{
		return 1;
	}


	ClientStatus *pClientStatus = (ClientStatus *)MessageBuf.Get();
	
	pClientStatus->GetPerfHistory(m_ClientRTStatisticsList);

	//set current data start time
	m_CurDataStartTime = m_LastRetrieveStartTime ;

	m_nRecordNum = m_ClientRTStatisticsList.size();

	//release graphic data object
	ReleaseGraphicsData();

	//create graphic data object to show chart
	int ii = 0;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(20);
	MyGraph[ii]->SetTickRange(100);
	MyGraph[ii]->SetCurColorIndex(0);
	MyGraph[ii]->SetXAxisAlignment(300);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("%"));

	ii++;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(20);
	MyGraph[ii]->SetTickRange(100);
	MyGraph[ii]->SetCurColorIndex(1);
	MyGraph[ii]->SetXAxisAlignment(300);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("%"));
	
	ii++;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(20);
	MyGraph[ii]->SetTickRange(100);
	MyGraph[ii]->SetCurColorIndex(2);
	MyGraph[ii]->SetXAxisAlignment(300);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("%"));
	
	ii++;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(10);
	MyGraph[ii]->SetTickRange(60);
	MyGraph[ii]->SetCurColorIndex(3);
	MyGraph[ii]->SetXAxisAlignment(300);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("fps"));

	ii++;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(1000);
	MyGraph[ii]->SetTickRange(10000);
	MyGraph[ii]->SetCurColorIndex(4);
	MyGraph[ii]->SetXAxisAlignment(300);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("RPM"));

	for (int i = 0; i < 4; ++i)
	{
		ii++;
		MyGraph[ii] = new CGraph();
		MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
		MyGraph[ii]->SetGraphType(1);
		MyGraph[ii]->SetScale(m_nScale);
		MyGraph[ii]->SetTickSpace(10);
		MyGraph[ii]->SetTickRange(100);
		MyGraph[ii]->SetCurColorIndex(5 + i);
		MyGraph[ii]->SetXAxisAlignment(300);
		MyGraph[ii]->SetXAxisLabel(_T("T"));
//		MyGraph[ii]->SetYAxisLabel(_T("¡æ"));
		MyGraph[ii]->SetYAxisLabel(_T("\x2103"));
	}

	int mm;
	if(m_nRecordNum > 0x00) 
	{
		//int j = 0;
		S3Time rdtime(0,0,0,0,0,0);
		const COleDateTime oletime;
		CString tickLabel;
		//SYSTEM_STATUS_DESC  stSystemStatusDesc;
		CGraphSeries* tmpSeries;
		std::vector<PerfStatus>::iterator it;

		int nAverageArray[DATATYPE_MAX_NUM];
		//memset(nAverageArray,0,sizeof(nAverageArray));
		// calculate the average 
		for( mm = 0; mm< DATATYPE_MAX_NUM ; mm++)
		{
			nAverageArray[mm] = INVALID_DATA_VAL;
		}
		
		for(it = m_ClientRTStatisticsList.begin(); it < m_ClientRTStatisticsList.end(); ++it)
		{
			for( mm = 0; mm< DATATYPE_MAX_NUM ; mm++)
			{
				int data = GetTypeData(*it,mm);
				if(data != INVALID_DATA_VAL)
				{
					if(nAverageArray[mm] == INVALID_DATA_VAL)
					{
						nAverageArray[mm] = data;
					}
					else
					{
						nAverageArray[mm] += data;
						nAverageArray[mm] /= 2;
					}
				}
			}
		}

		COleDateTime comparetime = m_LastRetrieveStartTime;
		COleDateTimeSpan onedayspan;
		onedayspan.SetDateTimeSpan(1,0,0,0);

		#define DATA_MAX_NUM 10
		int nDataNum[DATATYPE_MAX_NUM];
		int nData[DATATYPE_MAX_NUM][DATA_MAX_NUM];

		int nDataIndex[DATATYPE_MAX_NUM] ;
		int nDataArray[DATATYPE_MAX_NUM][DATA_MAX_NUM];
		memset(nDataNum,0,sizeof(nDataNum));
		memset(nDataIndex,0,sizeof(nDataIndex));
		for( mm = 0; mm< DATATYPE_MAX_NUM ; mm++)
		{
			for(int mmm = 0; mmm < DATA_MAX_NUM ; mmm++)
				nDataArray[mm][mmm] = INVALID_DATA_VAL;//nAverageArray[mm];
		}
		for(it = m_ClientRTStatisticsList.begin(); it < m_ClientRTStatisticsList.end(); ++it)
		{
			rdtime = it->GetSubmitTime();
			
			const COleDateTime oletime = rdtime.ToCOleDateTime();
			//tickLabel.Format(_T("%02d-%02d %02d:%02d"), oletime.GetMonth(),oletime.GetDay(),oletime.GetHour(),oletime.GetMinute());
			
			if(oletime < comparetime ) 
			{
				continue;
			}

			while(oletime >= comparetime + onedayspan /*|| nDataIndex >= 10-1*/)
			{
				tickLabel.Format(_T("%02d-%02d"), comparetime.GetMonth(),comparetime.GetDay());
							
				for(int nType = 0; nType < DATATYPE_MAX_NUM ; nType++)
				{
					tmpSeries = new CGraphSeries();
					tmpSeries->SetLabel(tickLabel);
					if(nDataNum[nType] == 0)
					{
						for(int jj = 0; jj < DATA_MAX_NUM ; jj++)
						{
							tmpSeries->SetData(jj, INVALID_DATA_VAL);
						}
					}
					else if (nDataNum[nType] > 0 && nDataNum[nType] < DATA_MAX_NUM)
					{
						for(int jj = 0; jj < nDataNum[nType] ; jj++)
						{
							tmpSeries->SetData(jj, nData[nType][jj]);
						}
					}
					else
					{
						int jj;
						for(jj = 0; jj< DATA_MAX_NUM - nDataIndex[nType];jj++)
						{
							tmpSeries->SetData(jj, nData[nType][jj]);
						}
						for(int kk = 0; kk < nDataIndex[nType] ; kk++)
						{
							tmpSeries->SetData(jj, nDataArray[nType][kk]);
							jj++;
						}
					}
					
					MyGraph[nType]->AddSeries(tmpSeries);
				}
				
				//nDataIndex = 0;
				memset(nDataNum,0,sizeof(nDataNum));
				memset(nDataIndex,0,sizeof(nDataIndex));
				//memset(nDataArray,-1,sizeof(nDataArray));
				for(int kk = 0; kk< DATATYPE_MAX_NUM ; kk++)
				{
					for(int kkk = 0; kkk < DATA_MAX_NUM ; kkk++)
						nDataArray[kk][kkk] = INVALID_DATA_VAL;//nAverageArray[kk];
				}
				comparetime += onedayspan;
				if(comparetime >= m_LastRetrieveEndTime)
				{
					break;
				}
			}
			int LastVal;
			for(int nn = 0; nn < DATATYPE_MAX_NUM ; nn++)
			{
				int nCurVal = GetTypeData(*it,nn);
				if(nCurVal != INVALID_DATA_VAL && nDataNum[nn] < DATA_MAX_NUM )
				{
					nData[nn][nDataNum[nn]] = nCurVal;
					nDataNum[nn] ++ ;//suppose never exceed
				}
				
				if(nCurVal != INVALID_DATA_VAL && nDataNum[nn] >= DATA_MAX_NUM )
				{
					if(nDataIndex[nn] < DATA_MAX_NUM)
					{
						if(nDataIndex[nn] == 0) 
						{
							LastVal = nAverageArray[nn];
						}
						else
						{
							LastVal = nDataArray[nn][nDataIndex[nn]-1];
						}

						//int nCurVal = GetTypeData(*it,nn);
						//if( abs(nCurVal - LastVal) * 20 > LastVal )
						if( abs(nCurVal - LastVal) * 20 > nAverageArray[nn] )
						{
							nDataArray[nn][nDataIndex[nn]] = nCurVal;
							nDataIndex[nn]++;
						}
					}
				}
			}
		}
		while(comparetime < m_LastRetrieveEndTime)
		{
			tickLabel.Format(_T("%02d-%02d"), comparetime.GetMonth(),comparetime.GetDay());
			for(int nType = 0; nType < DATATYPE_MAX_NUM ; nType++)
			{
				tmpSeries = new CGraphSeries();
				tmpSeries->SetLabel(tickLabel);
				if(nDataNum[nType] == 0)
				{
					for(int jj = 0; jj < DATA_MAX_NUM ; jj++)
					{
						tmpSeries->SetData(jj, INVALID_DATA_VAL);
					}
				}
				else if (nDataNum[nType] > 0 && nDataNum[nType] < DATA_MAX_NUM)
				{
					int jj;
					for(jj = 0; jj < nDataNum[nType] ; jj++)
					{
						tmpSeries->SetData(jj, nData[nType][jj]);
					}
					for(jj = nDataNum[nType]; jj< DATA_MAX_NUM ;jj++)
					{
						tmpSeries->SetData(jj, INVALID_DATA_VAL);
					}
				}
				else
				{
					int jj;
					for(jj = 0; jj< DATA_MAX_NUM - nDataIndex[nType];jj++)
					{
						tmpSeries->SetData(jj, nData[nType][jj]);
					}
					for(int kk = 0; kk < nDataIndex[nType] ; kk++)
					{
						tmpSeries->SetData(jj, nDataArray[nType][kk]);
						jj++;
					}
				}
			
				MyGraph[nType]->AddSeries(tmpSeries);
			}
			memset(nDataNum,0,sizeof(nDataNum));
			memset(nDataIndex,0,sizeof(nDataIndex));

			//memset(nDataArray,-1,sizeof(nDataArray));
			comparetime += onedayspan;
		}
	}
	
	DrawGraphicBackBuffer();
	UpdateGraphicsDisplay();

	return 0x01;
}


void CClientChartDlg::RetrieveBufferedData(const COleDateTime & starttime, const COleDateTime & endtime )
{
	m_nRecordNum = m_ClientRTStatisticsList.size();

	//release graphic data object
	ReleaseGraphicsData();

	//create graphic data object to show chart
	int ii = 0;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(20);
	MyGraph[ii]->SetTickRange(100);
	MyGraph[ii]->SetCurColorIndex(0);
	MyGraph[ii]->SetXAxisAlignment(300);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("%"));

	ii++;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(20);
	MyGraph[ii]->SetTickRange(100);
	MyGraph[ii]->SetCurColorIndex(1);
	MyGraph[ii]->SetXAxisAlignment(300);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("%"));

	ii++;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(20);
	MyGraph[ii]->SetTickRange(100);
	MyGraph[ii]->SetCurColorIndex(2);
	MyGraph[ii]->SetXAxisAlignment(300);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("%"));

	ii++;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(10);
	MyGraph[ii]->SetTickRange(60);
	MyGraph[ii]->SetCurColorIndex(3);
	MyGraph[ii]->SetXAxisAlignment(300);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("fps"));

	ii++;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(1000);
	MyGraph[ii]->SetTickRange(10000);
	MyGraph[ii]->SetCurColorIndex(4);
	MyGraph[ii]->SetXAxisAlignment(300);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("RPM"));

	for (int i = 0; i < 4; ++i)
	{
		ii++;
		MyGraph[ii] = new CGraph();
		MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
		MyGraph[ii]->SetGraphType(1);
		MyGraph[ii]->SetScale(m_nScale);
		MyGraph[ii]->SetTickSpace(10);
		MyGraph[ii]->SetTickRange(100);
		MyGraph[ii]->SetCurColorIndex(5 + i);
		MyGraph[ii]->SetXAxisAlignment(300);
		MyGraph[ii]->SetXAxisLabel(_T("T"));
		//		MyGraph[ii]->SetYAxisLabel(_T("¡æ"));
		MyGraph[ii]->SetYAxisLabel(_T("\x2103"));
	}

	int mm;
	if(m_nRecordNum > 0x00) 
	{
		//int j = 0;
		S3Time rdtime(0,0,0,0,0,0);
		const COleDateTime oletime;
		CString tickLabel;
		//SYSTEM_STATUS_DESC  stSystemStatusDesc;
		CGraphSeries* tmpSeries;
		std::vector<PerfStatus>::iterator it;
		int nAverageArray[DATATYPE_MAX_NUM];
		//memset(nAverageArray,0,sizeof(nAverageArray));
		// calculate the average 
		for( mm = 0; mm< DATATYPE_MAX_NUM ; mm++)
		{
			nAverageArray[mm] = INVALID_DATA_VAL;
		}

		for(it = m_ClientRTStatisticsList.begin(); it < m_ClientRTStatisticsList.end(); ++it)
		{
			for( mm = 0; mm< DATATYPE_MAX_NUM ; mm++)
			{
				int data = GetTypeData(*it,mm);
				if(data != INVALID_DATA_VAL)
				{
					if(nAverageArray[mm] == INVALID_DATA_VAL)
					{
						nAverageArray[mm] = data;
					}
					else
					{
						nAverageArray[mm] += data;
						nAverageArray[mm] /= 2;
					}
				}
			}
		}

		COleDateTime comparetime = starttime;
		COleDateTimeSpan onedayspan;
		onedayspan.SetDateTimeSpan(1,0,0,0);

		#define DATA_MAX_NUM 10
		int nDataNum[DATATYPE_MAX_NUM];
		int nData[DATATYPE_MAX_NUM][DATA_MAX_NUM];

		int nDataIndex[DATATYPE_MAX_NUM] ;
		int nDataArray[DATATYPE_MAX_NUM][DATA_MAX_NUM];
		memset(nDataNum,0,sizeof(nDataNum));
		memset(nDataIndex,0,sizeof(nDataIndex));
		for( mm = 0; mm< DATATYPE_MAX_NUM ; mm++)
		{
			for(int mmm = 0; mmm < DATA_MAX_NUM ; mmm++)
				nDataArray[mm][mmm] = INVALID_DATA_VAL;//nAverageArray[mm];
		}
		for(it = m_ClientRTStatisticsList.begin(); it < m_ClientRTStatisticsList.end(); ++it)
		{
			rdtime = it->GetSubmitTime();

			const COleDateTime oletime = rdtime.ToCOleDateTime();
			if(oletime < comparetime ) 
			{
				continue;
			}
			//tickLabel.Format(_T("%02d-%02d %02d:%02d"), oletime.GetMonth(),oletime.GetDay(),oletime.GetHour(),oletime.GetMinute());

			while(oletime >= comparetime + onedayspan /*|| nDataIndex >= 10-1*/)
			{
				tickLabel.Format(_T("%02d-%02d"), comparetime.GetMonth(),comparetime.GetDay());

				for(int nType = 0; nType < DATATYPE_MAX_NUM ; nType++)
				{
					tmpSeries = new CGraphSeries();
					tmpSeries->SetLabel(tickLabel);
					if(nDataNum[nType] == 0)
					{
						for(int jj = 0; jj < DATA_MAX_NUM ; jj++)
						{
							tmpSeries->SetData(jj, INVALID_DATA_VAL);
						}
					}
					else if (nDataNum[nType] > 0 && nDataNum[nType] < DATA_MAX_NUM)
					{
						for(int jj = 0; jj < nDataNum[nType] ; jj++)
						{
							tmpSeries->SetData(jj, nData[nType][jj]);
						}
					}
					else
					{
						int jj;
						for(jj = 0; jj< DATA_MAX_NUM - nDataIndex[nType];jj++)
						{
							tmpSeries->SetData(jj, nData[nType][jj]);
						}
						for(int kk = 0; kk < nDataIndex[nType] ; kk++)
						{
							tmpSeries->SetData(jj, nDataArray[nType][kk]);
							jj++;
						}
					}
					MyGraph[nType]->AddSeries(tmpSeries);
				}

				//nDataIndex = 0;
				memset(nDataNum,0,sizeof(nDataNum));
				memset(nDataIndex,0,sizeof(nDataIndex));
				//memset(nDataArray,-1,sizeof(nDataArray));
				for(int kk = 0; kk< DATATYPE_MAX_NUM ; kk++)
				{
					for(int kkk = 0; kkk < DATA_MAX_NUM ; kkk++)
						nDataArray[kk][kkk] = INVALID_DATA_VAL;//nAverageArray[kk];
				}
				comparetime += onedayspan;
				if(comparetime >= endtime)
				{
					break;
				}
			}

			int LastVal;
			for(int nn = 0; nn < DATATYPE_MAX_NUM ; nn++)
			{
				int nCurVal = GetTypeData(*it,nn);
				if(nCurVal != INVALID_DATA_VAL && nDataNum[nn] < DATA_MAX_NUM )
				{
					nData[nn][nDataNum[nn]] = nCurVal;
					nDataNum[nn] ++ ;//suppose never exceed
				}
				
				if(nCurVal != INVALID_DATA_VAL && nDataNum[nn] >= DATA_MAX_NUM )
				{
					if(nDataIndex[nn] < DATA_MAX_NUM)
					{
						if(nDataIndex[nn] == 0) 
						{
							LastVal = nAverageArray[nn];
						}
						else
						{
							LastVal = nDataArray[nn][nDataIndex[nn]-1];
						}

						//int nCurVal = GetTypeData(*it,nn);
						//if( abs(nCurVal - LastVal) * 20 > LastVal )
						if( abs(nCurVal - LastVal) * 20 > nAverageArray[nn] )
						{
							nDataArray[nn][nDataIndex[nn]] = nCurVal;
							nDataIndex[nn]++;
						}
					}
				}
			}
		}
		while(comparetime < endtime)
		{
			tickLabel.Format(_T("%02d-%02d"), comparetime.GetMonth(),comparetime.GetDay());
			for(int nType = 0; nType < DATATYPE_MAX_NUM ; nType++)
			{
				tmpSeries = new CGraphSeries();
				tmpSeries->SetLabel(tickLabel);
				if(nDataNum[nType] == 0)
				{
					for(int jj = 0; jj < DATA_MAX_NUM ; jj++)
					{
						tmpSeries->SetData(jj, INVALID_DATA_VAL);
					}
				}
				else if (nDataNum[nType] > 0 && nDataNum[nType] < DATA_MAX_NUM)
				{
					int jj;
					for(jj = 0; jj < nDataNum[nType] ; jj++)
					{
						tmpSeries->SetData(jj, nData[nType][jj]);
					}
					for(jj = nDataNum[nType]; jj< DATA_MAX_NUM ;jj++)
					{
						tmpSeries->SetData(jj, INVALID_DATA_VAL);
					}
				}
				else
				{
					int jj;
					for(jj = 0; jj< DATA_MAX_NUM - nDataIndex[nType];jj++)
					{
						tmpSeries->SetData(jj, nData[nType][jj]);
					}
					for(int kk = 0; kk < nDataIndex[nType] ; kk++)
					{
						tmpSeries->SetData(jj, nDataArray[nType][kk]);
						jj++;
					}
				}
			
				MyGraph[nType]->AddSeries(tmpSeries);
			}
			memset(nDataNum,0,sizeof(nDataNum));
			memset(nDataIndex,0,sizeof(nDataIndex));
			//memset(nDataArray,-1,sizeof(nDataArray));
			comparetime += onedayspan;
		}
	}
	DrawGraphicBackBuffer();
	UpdateGraphicsDisplay();

	return ;
}

LRESULT CClientChartDlg::UpdatePartialClientHistoryData(COleDateTime starttime,bool bFreshFlag)
{
	return 0x01;
}

void CClientChartDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 

	AfxGetMainWnd()->Invalidate(); 
	AfxGetMainWnd()->UpdateWindow(); 

	UpdateGraphicsDisplay();
}


void CClientChartDlg::DrawGraphicBackBuffer()
{
	CWnd  *hwnd;
	CRect rect;
	int nWidth,nHeight;
	int StartYPos = 0;
	int nGraphHeight;

	hwnd = GetDlgItem(IDC_STATIC_BACKGROUND);
	hwnd->GetWindowRect(&rect);
#if 0
	nWidth = rect.Width();
	nGraphHeight = m_dcHeight/DATATYPE_MAX_NUM ; 
	dcMem.FillSolidRect(0,0,m_dcWidth,m_dcHeight,afxGlobalData.GetColor(COLOR_BTNFACE));
	for(int ii = 0 ; ii< DATATYPE_MAX_NUM;ii++)
	{
		MyGraph[ii]->DrawGraph(&dcMem,0,StartYPos, nWidth,nGraphHeight);
		StartYPos += nGraphHeight;
	}
#else
	nWidth = rect.Width();
	nHeight = rect.Height(); 
	dcMem.FillSolidRect(0,0,nWidth,nHeight,afxGlobalData.GetColor(COLOR_BTNFACE));
	
	int curDataTypeIdx = m_ComboType.GetCurSel();
	MyGraph[curDataTypeIdx]->DrawGraph(&dcMem,0,0, nWidth,nHeight);
#endif
}

void CClientChartDlg::RetrieveGraphicsData()
{
#if 0
	//COleDateTime olestarttime;
	//COleDateTime oleendtime;
	COleDateTimeSpan timespan;         // days,hours,minutes,seconds

	timespan.SetDateTimeSpan(7,0,0,0); //retrive 7-days data
	//m_datatimectrl.GetTime(oleendtime);
	//olestarttime = oleendtime - timespan;	

	DWORD MessageID = USER_MSG_GET_CLIENT_HISTORY;

	ST_CLIENT_RT_STA_PARA * stParameter =  new ST_CLIENT_RT_STA_PARA;
	stParameter->clientid = m_ClientId;
	m_datatimectrl.GetTime(stParameter->endtime);
	stParameter->starttime = stParameter->endtime - timespan;

	m_LastRetrieveStartTime = stParameter->starttime;

	if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,	(WPARAM)this->GetSafeHwnd(), (LPARAM)stParameter))
	{
		TRACE1("post message failed,errno:%d\n",::GetLastError());
	}
#endif 
}

void CClientChartDlg::RetrieveGraphicsDataNew(const COleDateTime & starttime, const COleDateTime & endtime )
{
	
	if(starttime >= m_LastRetrieveStartTime && endtime <= m_LastRetrieveEndTime)
	{
		bIsRetrievingData = true;

		((CButton *)GetDlgItem(IDC_RADIO_ONE_WEEK))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_RADIO_ONE_MONTH))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_RADIO_THREE_MONTH))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_RADIO_USER_DEFINE))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_DATETIMEPICKER4))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_DATETIMEPICKER5))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_BTN_SEARCH))->EnableWindow(false);

		RetrieveBufferedData(starttime,endtime);

		bIsRetrievingData = false;
		((CButton *)GetDlgItem(IDC_RADIO_ONE_WEEK))->EnableWindow(true);
		((CButton *)GetDlgItem(IDC_RADIO_ONE_MONTH))->EnableWindow(true);
		((CButton *)GetDlgItem(IDC_RADIO_THREE_MONTH))->EnableWindow(true);
		((CButton *)GetDlgItem(IDC_RADIO_USER_DEFINE))->EnableWindow(true);
		if(m_nLastRadioBoxIdx == 3)
		{
			((CButton *)GetDlgItem(IDC_DATETIMEPICKER4))->EnableWindow(true);
			((CButton *)GetDlgItem(IDC_DATETIMEPICKER5))->EnableWindow(true);
			((CButton *)GetDlgItem(IDC_BTN_SEARCH))->EnableWindow(true);
		}
		
		m_starttime = starttime;
		m_endtime = endtime;
		UpdateData(FALSE);
		return;
	}

	DWORD MessageID = USER_MSG_GET_CLIENT_HISTORY;
	
	ST_CLIENT_RT_STA_PARA * stParameter =  new ST_CLIENT_RT_STA_PARA;
	stParameter->clientid = m_ClientId;
	stParameter->starttime = starttime;
	stParameter->endtime = endtime;
	
	m_LastRetrieveStartTime = starttime;
	m_LastRetrieveEndTime = endtime;
	
	m_starttime = starttime;
	m_endtime = endtime;
	UpdateData(FALSE);

	if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,	(WPARAM)this->GetSafeHwnd(), (LPARAM)stParameter))
	{
		TRACE1("post message failed,errno:%d\n",::GetLastError());
	}
	else
	{
		//retrieve data begin
		bIsRetrievingData = true;
		
		((CButton *)GetDlgItem(IDC_COMBO_TYPE))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_RADIO_ONE_WEEK))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_RADIO_ONE_MONTH))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_RADIO_THREE_MONTH))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_RADIO_USER_DEFINE))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_DATETIMEPICKER4))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_DATETIMEPICKER5))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_BTN_SEARCH))->EnableWindow(false);
	}
}


void CClientChartDlg::ReleaseGraphicsData()
{
	for(int ii = 0;ii < DATATYPE_MAX_NUM; ii++)
	{
		if(MyGraph[ii])
		{
			delete MyGraph[ii];
			MyGraph[ii] = NULL;
		}
	}
}

void CClientChartDlg::UpdateGraphicsDisplay()
{
	CWnd  *hwnd;
	CDC   *mydc;
	CRect rect;

	hwnd = GetDlgItem(IDC_STATIC_BACKGROUND);
	mydc = hwnd->GetDC();
	hwnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
#if 0
	mydc->BitBlt(2,2,rect.Width()-4,rect.Height()-4,&dcMem,0,m_StartY,SRCCOPY);
#else
	mydc->BitBlt(2,2,rect.Width()-4,rect.Height()-4,&dcMem,0,0,SRCCOPY);
#endif
}


void CClientChartDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;
	
	if(m_nInitFlag)
	{
		CalcLayout();
		//modified by LiuSong
		//DrawGraphicBackBuffer();

		//UpdateGraphicsDisplay();
	}
	// TODO: 
}

void CClientChartDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	/*
	int nCurPos = GetScrollPos(SB_HORZ);
	int nPrevHScrollBarPos = nPos;
	*/
#if 0	
	int nCurPos = m_nCurHScrollBarPos;
	
	SCROLLINFO Info;
	GetScrollInfo(SB_HORZ, &Info, SIF_ALL);

	switch (nSBCode)
	{
	case SB_TOP:
		nCurPos = Info.nMin;
		break;
	case SB_BOTTOM:
		nCurPos = Info.nMax;
		break;
	case SB_LINELEFT:
		nCurPos = max(Info.nMin, nCurPos - 10);
		break;
	case SB_LINERIGHT:
		nCurPos = min(Info.nMax, nCurPos + 10);
		break;
	case SB_PAGELEFT:
		nCurPos = max(Info.nMin, nCurPos - (int)Info.nPage);
		break;
	case SB_PAGERIGHT:
		nCurPos = min(Info.nMax, nCurPos + (int)Info.nPage);
		break;
	//case SB_THUMBPOSITION:
	//	nCurPos = nPos;
	//	break;
	case SB_THUMBTRACK:
		nCurPos = nPos;
		break;
	case SB_ENDSCROLL:
		break;
	}

	// TODO: 
	COleDateTime endtime;
	COleDateTime starttime;
	COleDateTimeSpan timespan;//(2,0,0,0); // days,hours,minutes,seconds
	COleDateTimeSpan span(7,0,0,0);

	DWORD dwResult = m_datatimectrl.GetTime(endtime); 
	//if   (dwResult == GDT_VALID) 
	{
		if(nCurPos > m_nCurHScrollBarPos)
		{
			timespan.SetDateTimeSpan((nCurPos-m_nCurHScrollBarPos)/24,0,0,0);
			endtime += timespan;	
			m_datatimectrl.SetTime(endtime);
			
			starttime = endtime - m_Span;
			if(starttime - m_CurDataStartTime  <= span)
			{
				UpdatePartialClientHistoryData(starttime,true);
			}
			else
			{
				starttime.SetDateTime(0,0,0,0,0,0); 
				UpdatePartialClientHistoryData(starttime,false);
			}

		}
		else if(nCurPos < m_nCurHScrollBarPos)
		{
			timespan.SetDateTimeSpan((m_nCurHScrollBarPos - nCurPos)/24,0,0,0);
			endtime -= timespan;	
			m_datatimectrl.SetTime(endtime);

			starttime = endtime - m_Span;
			if( m_CurDataStartTime - endtime <= span)
			{
				UpdatePartialClientHistoryData(starttime,true);
			}
			else
			{
				starttime.SetDateTime(0,0,0,0,0,0); 
				UpdatePartialClientHistoryData(starttime,false);
			}
		}
	}

	switch (nSBCode)
	{
	case SB_THUMBTRACK:
		m_nCurHScrollBarPos = nCurPos;

		break;
	case SB_ENDSCROLL:
		RetrieveGraphicsData();
		break;
	default:
		m_nCurHScrollBarPos = Info.nMax/2;
		break;

	}
	
	//SetScrollPos(SB_HORZ, nCurPos);
#endif 
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CClientChartDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
#if 0
	int nCurPos = GetScrollPos(SB_VERT);
	SCROLLINFO Info;
	GetScrollInfo(SB_VERT, &Info, SIF_ALL);

	switch (nSBCode)
	{
	case SB_TOP:
		nCurPos = Info.nMin;
		break;
	case SB_BOTTOM:
		nCurPos = Info.nMax;
		break;
	case SB_ENDSCROLL:
		break;
	case SB_LINEUP:
		nCurPos = max(Info.nMin, nCurPos - 10);
		break;
	case SB_LINEDOWN:
		nCurPos = min(Info.nMax, nCurPos + 10);
		break;
	case SB_PAGEUP:
		nCurPos = max(Info.nMin, nCurPos - (int)Info.nPage);
		break;
	case SB_PAGEDOWN:
		nCurPos = min(Info.nMax, nCurPos + (int)Info.nPage);
		break;
	case SB_THUMBPOSITION:
		nCurPos = nPos;
		break;
	case SB_THUMBTRACK:
		nCurPos = nPos;
		break;
	}

	SetScrollPos(SB_VERT, nCurPos);

	if (m_StartY != nCurPos)
	{
		m_StartY = nCurPos;

		CalcLayout();
		UpdateGraphicsDisplay();
	}
#endif 
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
	
}

VOID CClientChartDlg::CalcLayout()
{
#if 0
	CRect ClientRect,Rect;
	//GetClientRect(&ClientRect);
	
	//GetDlgItem(IDC_STATIC_BACKGROUND)->GetWindowRect(ClientRect);
	//ScreenToClient(&ClientRect);

	GetDlgItem(IDC_STATIC_FOREGROUND)->GetWindowRect(Rect);
	ScreenToClient(&Rect);

	int TotalLength = m_dcHeight;

	int nCurPos = GetScrollPos(SB_VERT);
	//m_StartY = -nCurPos;
	m_StartY = nCurPos;

	if(TotalLength > Rect.Height())
	{
		EnableScrollBarCtrl(SB_VERT, TRUE);
		EnableScrollBar(SB_VERT, ESB_ENABLE_BOTH );

		SCROLLINFO ScrollInfo;
		ScrollInfo.cbSize = sizeof(SCROLLINFO);
		ScrollInfo.nMin = 0;
		ScrollInfo.nMax = TotalLength;
		ScrollInfo.nPage = Rect.Height();
		ScrollInfo.fMask = SIF_RANGE | SIF_PAGE;

		SetScrollInfo(SB_VERT,&ScrollInfo);

		//m_StartY = max(m_StartY, Rect.Height() - TotalLength );
		m_StartY = min(m_StartY, TotalLength - Rect.Height());
	}
	else
	{
		EnableScrollBarCtrl(SB_VERT, FALSE);
		m_StartY = 0;
	}
#else
	m_StartY = 0;
#endif
}


BOOL CClientChartDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  
	INIT_EASYSIZE;
	SCROLLINFO ScrollInfo;
	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.nMin = 0;
	ScrollInfo.nMax = 24*365*2;//can get two year's data
	ScrollInfo.nPage = 24;     //24 hours
	ScrollInfo.fMask = SIF_RANGE | SIF_PAGE;
	SetScrollInfo(SB_HORZ,&ScrollInfo);
	//SetScrollRange (SB_HORZ, 0, 50, FALSE);
	SetScrollPos(SB_HORZ,ScrollInfo.nMax/2);
	m_nCurHScrollBarPos = GetScrollPos(SB_HORZ);

	for(int ii = 0; ii < DATATYPE_MAX_NUM ; ii++)
	{
		m_ComboType.AddString(Translate(mDataTypeTitleName[ii]));
	}
	m_ComboType.SetCurSel(0);

	((CButton *)GetDlgItem(IDC_RADIO_ONE_WEEK))->SetWindowText(Translate(_T("7 Days")));
	((CButton *)GetDlgItem(IDC_RADIO_ONE_MONTH))->SetWindowText(Translate(_T("30 Days")));
	((CButton *)GetDlgItem(IDC_RADIO_THREE_MONTH))->SetWindowText(Translate(_T("90 Days")));
	((CButton *)GetDlgItem(IDC_RADIO_USER_DEFINE))->SetWindowText(Translate(_T("User Define")));
	((CButton *)GetDlgItem(IDC_BTN_SEARCH))->SetWindowText(Translate(_T("Search")));

	bIsRetrievingData = false;
	m_nLastRadioBoxIdx = 0;
	((CButton *)GetDlgItem(IDC_RADIO_ONE_WEEK))->SetCheck(true);
	((CButton *)GetDlgItem(IDC_RADIO_ONE_MONTH))->SetCheck(false);
	((CButton *)GetDlgItem(IDC_RADIO_THREE_MONTH))->SetCheck(false);
	((CButton *)GetDlgItem(IDC_DATETIMEPICKER4))->EnableWindow(false);
	((CButton *)GetDlgItem(IDC_DATETIMEPICKER5))->EnableWindow(false);
	((CButton *)GetDlgItem(IDC_BTN_SEARCH))->EnableWindow(false);
	
	m_LastRetrieveStartTime.SetDate(1970,1,1);
	m_LastRetrieveEndTime.SetDate(1970,1,1);

	COleDateTime tmp = COleDateTime::GetCurrentTime() ;
	//tmp += 1;
	COleDateTime starttime,endtime;
	COleDateTimeSpan timespan;         // days,hours,minutes,seconds
	timespan.SetDateTimeSpan(6,23,59,59); //retrive 7-days data
	endtime.SetDateTime(tmp.GetYear(),tmp.GetMonth(),tmp.GetDay(),23,59,59);
	starttime = endtime - timespan;
	RetrieveGraphicsDataNew(starttime,endtime);
	
	CWnd  *hwnd;
	CDC   *mydc;
	CRect rect;

	hwnd = GetDlgItem(IDC_STATIC_BACKGROUND);
	hwnd->GetWindowRect(&rect);
	mydc = hwnd->GetDC();

	dcMem.CreateCompatibleDC(mydc); 
	bmp.CreateCompatibleBitmap(mydc,m_dcWidth,m_dcHeight);
	dcMem.SelectObject(&bmp); 
	dcMem.FillSolidRect(0,0,m_dcWidth,m_dcHeight,afxGlobalData.GetColor(COLOR_BTNFACE));
	GetDlgItem(IDC_BUTTON1)->SetWindowText(Translate(_T("zoom in")));
	GetDlgItem(IDC_BUTTON3)->SetWindowText(Translate(_T("zoom out")));
	CalcLayout();

	UpdateGraphicsDisplay();

	m_nInitFlag = true;

	return TRUE;  // return TRUE unless you set the focus to a control
}




BOOL CClientChartDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 
#if 0
	if(zDelta < 0)
	{
		OnVScroll(SB_LINEDOWN, GetScrollPos(SB_VERT), GetScrollBarCtrl(SB_VERT));
	}
	else if (zDelta > 0)
	{
		OnVScroll(SB_LINEUP, GetScrollPos(SB_VERT), GetScrollBarCtrl(SB_VERT));
	}
#endif
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CClientChartDlg::OnBnClickedButton1()
{
	int nCount = m_ClientRTStatisticsList.size();

	if(nCount /m_nScale > 10)
	{
		m_nScale *=2;

		for(int ii=0;ii<DATATYPE_MAX_NUM;ii++)
		{
			MyGraph[ii]->SetScale(m_nScale);
		}

		DrawGraphicBackBuffer();
		UpdateGraphicsDisplay();
	}
}

void CClientChartDlg::OnBnClickedButton3()
{
	// TODO: 
	if(m_nScale >= 2)
	{
		m_nScale /= 2;

		for(int ii=0;ii<DATATYPE_MAX_NUM;ii++)
		{
			MyGraph[ii]->SetScale(m_nScale);
		}

		DrawGraphicBackBuffer();
		UpdateGraphicsDisplay();
	}
}


void CClientChartDlg::OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: 
	*pResult = 0;
#if 0
	RetrieveGraphicsData();
#endif
}

void CClientChartDlg::OnCbnSelchangeComboType()
{
	// TODO: Add your control notification handler code here
	DrawGraphicBackBuffer();
	UpdateGraphicsDisplay();
}

void CClientChartDlg::OnBnClickedRadioOneWeek()
{
	// TODO: Add your control notification handler code here
	if(bIsRetrievingData)
	{
		return;
	}

	if( m_nLastRadioBoxIdx != 0)
	{
		m_nLastRadioBoxIdx = 0;

		((CButton *)GetDlgItem(IDC_RADIO_ONE_WEEK))->SetCheck(true);
		((CButton *)GetDlgItem(IDC_RADIO_ONE_MONTH))->SetCheck(false);
		((CButton *)GetDlgItem(IDC_RADIO_THREE_MONTH))->SetCheck(false);
		((CButton *)GetDlgItem(IDC_DATETIMEPICKER4))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_DATETIMEPICKER5))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_BTN_SEARCH))->EnableWindow(false);

		COleDateTime tmp = COleDateTime::GetCurrentTime();
		//tmp += 1;
		COleDateTime starttime,endtime;
		COleDateTimeSpan timespan;         // days,hours,minutes,seconds
		timespan.SetDateTimeSpan(6,23,59,59); //retrieve 7-days data
		endtime.SetDateTime(tmp.GetYear(),tmp.GetMonth(),tmp.GetDay(),23,59,59);
		starttime = endtime - timespan ;
		RetrieveGraphicsDataNew(starttime,endtime);
	}
}


void CClientChartDlg::OnBnClickedRadioOneMonth()
{
	// TODO: Add your control notification handler code here
	if(bIsRetrievingData)
	{
		return;
	}

	if( m_nLastRadioBoxIdx != 1)
	{
		m_nLastRadioBoxIdx = 1;

		((CButton *)GetDlgItem(IDC_RADIO_ONE_WEEK))->SetCheck(false);
		((CButton *)GetDlgItem(IDC_RADIO_ONE_MONTH))->SetCheck(true);
		((CButton *)GetDlgItem(IDC_RADIO_THREE_MONTH))->SetCheck(false);
		((CButton *)GetDlgItem(IDC_DATETIMEPICKER4))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_DATETIMEPICKER5))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_BTN_SEARCH))->EnableWindow(false);

		COleDateTime tmp = COleDateTime::GetCurrentTime();
		//tmp += 1;
		COleDateTime starttime,endtime;
		COleDateTimeSpan timespan;         // days,hours,minutes,seconds
		timespan.SetDateTimeSpan(29,23,59,59); //retrieve 1-month data
		endtime.SetDateTime(tmp.GetYear(),tmp.GetMonth(),tmp.GetDay(),23,59,59);
		starttime = endtime - timespan;
		RetrieveGraphicsDataNew(starttime,endtime);
	}
}


void CClientChartDlg::OnBnClickedRadioThreeMonth()
{
	// TODO: Add your control notification handler code here
	if(bIsRetrievingData)
	{
		return;
	}

	if( m_nLastRadioBoxIdx != 2)
	{
		m_nLastRadioBoxIdx = 2;

		((CButton *)GetDlgItem(IDC_RADIO_ONE_WEEK))->SetCheck(false);
		((CButton *)GetDlgItem(IDC_RADIO_ONE_MONTH))->SetCheck(false);
		((CButton *)GetDlgItem(IDC_RADIO_THREE_MONTH))->SetCheck(true);
		((CButton *)GetDlgItem(IDC_DATETIMEPICKER4))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_DATETIMEPICKER5))->EnableWindow(false);
		((CButton *)GetDlgItem(IDC_BTN_SEARCH))->EnableWindow(false);

		COleDateTime tmp = COleDateTime::GetCurrentTime();
		//tmp += 1;
		COleDateTime starttime,endtime;
		COleDateTimeSpan timespan;         // days,hours,minutes,seconds
		timespan.SetDateTimeSpan(89,23,59,59); //retrieve 3-months data
		endtime.SetDateTime(tmp.GetYear(),tmp.GetMonth(),tmp.GetDay(),23,59,59);
		starttime = endtime - timespan;
		RetrieveGraphicsDataNew(starttime,endtime);
	}
}


void CClientChartDlg::OnBnClickedRadioUserDefine()
{
	// TODO: Add your control notification handler code here
	if(bIsRetrievingData)
	{
		return;
	}

	if( m_nLastRadioBoxIdx != 3)
	{
		m_nLastRadioBoxIdx = 3;

		((CButton *)GetDlgItem(IDC_RADIO_ONE_WEEK))->SetCheck(false);
		((CButton *)GetDlgItem(IDC_RADIO_ONE_MONTH))->SetCheck(false);
		((CButton *)GetDlgItem(IDC_RADIO_THREE_MONTH))->SetCheck(false);
		((CButton *)GetDlgItem(IDC_DATETIMEPICKER4))->EnableWindow(true);
		((CButton *)GetDlgItem(IDC_DATETIMEPICKER5))->EnableWindow(true);
		((CButton *)GetDlgItem(IDC_BTN_SEARCH))->EnableWindow(true);
	}
}

/*COleDateTime tmpStartTime,tmpEndTime;*/
void CClientChartDlg::OnDtnDatetimechangeDatetimepicker4(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	
	COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 0, 0, 0);
	CheckDateTime(this, IDC_DATETIMEPICKER4, IDC_DATETIMEPICKER5, date, true);

	/*
	COleDateTime starttime,endtime;
	//((CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER4))->GetTime(starttime);
	starttime.SetDate(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay);
	((CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER5))->GetTime(endtime);
	
	if(starttime == tmpStartTime)
		return;
	tmpStartTime = starttime;

	COleDateTimeSpan timespan;         // days,hours,minutes,seconds
	timespan.SetDateTimeSpan(90,0,0,0); //retrieve 3-months data
	if(endtime > starttime + timespan)
	{
		AfxMessageBox(Translate(_T("The period between start time and end time must less than 90 days!")));
		((CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER4))->SetTime(m_starttime);
		//UpdateData(FALSE);
		return;
	}
	else if (endtime <= starttime)
	{
		AfxMessageBox(Translate(_T("start time must less than end time!")));
		((CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER4))->SetTime(m_starttime);
		//UpdateData(FALSE);
		return;
	}
	else
	{
		m_starttime = starttime;
		//UpdateData(TRUE);
	}
	*/

	*pResult = 0;
	//((CButton *)GetDlgItem(IDC_DATETIMEPICKER5))->SetFocus();
}


void CClientChartDlg::OnDtnDatetimechangeDatetimepicker5(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here

	COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 0, 0, 0);
	CheckDateTime(this, IDC_DATETIMEPICKER4, IDC_DATETIMEPICKER5, date, false);
	/*
	COleDateTime starttime,endtime;
	((CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER4))->GetTime(starttime);
	//((CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER5))->GetTime(endtime);
	endtime.SetDate(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay);

	if(endtime == tmpEndTime)
		return;
	tmpEndTime = endtime;

	COleDateTimeSpan timespan;         // days,hours,minutes,seconds
	timespan.SetDateTimeSpan(90,0,0,0); //retrieve 3-months data
	if(endtime > starttime + timespan)
	{
		AfxMessageBox(Translate(_T("The period between start time and end time must less than 90 days!")));
		
		((CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER5))->SetTime(m_endtime);
		//UpdateData(FALSE);
	}
	else if (endtime <= starttime)
	{
		AfxMessageBox(Translate(_T("start time must less than end time!")));
		((CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER5))->SetTime(m_endtime);
		//UpdateData(FALSE);
	}
	else
	{
		m_endtime = endtime;
		//UpdateData(TRUE);
	}
	*/
	*pResult = 0;
}


void CClientChartDlg::OnBnClickedBtnSearch()
{
	// TODO: Add your control notification handler code here
	COleDateTime starttime,endtime;
	((CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER4))->GetTime(starttime);
	((CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER5))->GetTime(endtime);
	
	COleDateTimeSpan timespan;         // days,hours,minutes,seconds
	timespan.SetDateTimeSpan(89,23,59,59); //retrieve 3-months data
	endtime.SetDateTime(endtime.GetYear(),endtime.GetMonth(),endtime.GetDay(),23,59,59);
	//starttime = endtime - timespan;
	if(starttime < endtime - timespan)
	{
		MessageBox(Translate(_T("The period between start time and end time must less than 90 days!")), Translate(_T("Warning:Client monitor")), MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	/*
	COleDateTimeSpan timespan;         // days,hours,minutes,seconds
	timespan.SetDateTimeSpan(90,0,0,0); //retrieve 3-months data
	if(endtime > starttime + timespan)
	{
		AfxMessageBox(Translate(_T("The period between start time and end time must less than 90 days!")));
		return;
	}*/
	else if (endtime <= starttime)
	{
		MessageBox(Translate(_T("start time must less than end time!")), Translate(_T("Warning:Client monitor")), MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	else
	{
		starttime.SetDate(starttime.GetYear(),starttime.GetMonth(),starttime.GetDay());
		//endtime += 1;
		endtime.SetDateTime(endtime.GetYear(),endtime.GetMonth(),endtime.GetDay(),23,59,59);
		RetrieveGraphicsDataNew(starttime,endtime);
	}
}


#else

IMPLEMENT_DYNAMIC(CClientChartDlg, CDialogEx)

CString mDataTypeTitleName[MY_GRAPH_NUM]=
{
	_T("CpuUsage"),_T("MemUsage"),_T("DiskUsage"),_T("fps"), 
	_T("Fan Speed"), _T("CPU Temperature"),  _T("MotherBoard Temperature"), 
	_T("HDD Temperature"),  _T("GPU Temperature"),
};

CClientChartDlg::CClientChartDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientChartDlg::IDD, pParent)
{
	m_nInitFlag = FALSE;
	m_nScale = 1;
	m_nRecordNum = 0;
	m_Span.SetDateTimeSpan(7,0,0,0);

	m_dcWidth = 2000;
	m_dcHeight = 350 * MY_GRAPH_NUM;

	for(int ii = 0;ii< MY_GRAPH_NUM ; ii++)
	{
		MyGraph[ii] = NULL;
	}


}

CClientChartDlg::~CClientChartDlg()
{
	dcMem.DeleteDC(); //delete DC 
	bmp.DeleteObject(); //delete bitmap
}

void CClientChartDlg::SetClientId(int id)
{
	m_ClientId = id;
}

int CClientChartDlg::GetClientId()
{
	return m_ClientId;
}

void CClientChartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_datatimectrl);
}


BEGIN_MESSAGE_MAP(CClientChartDlg, CDialogEx)
    ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_BUTTON1, &CClientChartDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CClientChartDlg::OnBnClickedButton3)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, &CClientChartDlg::OnDtnDatetimechangeDatetimepicker1)

	ON_MESSAGE(USER_MSG_GET_CLIENT_HISTORY, &CClientChartDlg::UpdateClientHistoryData)

END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CClientChartDlg)
	EASYSIZE(IDC_STATIC_BACKGROUND,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_FOREGROUND,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	
END_EASYSIZE_MAP

// CUserMonitorChartDlg 


void CClientChartDlg::OnCancel()
{
}


void CClientChartDlg::OnOK()
{
}


void CClientChartDlg::OnClose()
{
/*
    CClientStateTab* parent = (CClientStateTab*)GetParent();
    parent->RemoveTab(parent->GetActiveTab());
*/
    CDialogEx::OnClose();
}

LRESULT CClientChartDlg::UpdateClientHistoryData(WPARAM wp,LPARAM lp)
{
	//////////////////////////////////////////////////////////////////////////
	// How to get a buf
	UINT nMessageBufID = (UINT)wp;

	DataBuf< ClientStatus> MessageBuf;
	if (!WorkThreadDataManager< ClientStatus >
		::GetDataBuf(nMessageBufID, MessageBuf))
	{
		return 1;
	}

	ClientStatus *pClientStatus = (ClientStatus *)MessageBuf.Get();
	
	pClientStatus->GetPerfHistory(m_ClientRTStatisticsList);

	//set current data start time
	m_CurDataStartTime = m_LastRetrieveStartTime ;

	m_nRecordNum = m_ClientRTStatisticsList.size();

	//release graphic data object
	ReleaseGraphicsData();

	//create graphic data object to show chart
	int ii = 0;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(20);
	MyGraph[ii]->SetTickRange(100);
	MyGraph[ii]->SetCurColorIndex(0);
	MyGraph[ii]->SetXAxisAlignment(270);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("%"));

	ii++;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(20);
	MyGraph[ii]->SetTickRange(100);
	MyGraph[ii]->SetCurColorIndex(1);
	MyGraph[ii]->SetXAxisAlignment(270);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("%"));
	
	ii++;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(20);
	MyGraph[ii]->SetTickRange(100);
	MyGraph[ii]->SetCurColorIndex(2);
	MyGraph[ii]->SetXAxisAlignment(270);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("%"));
	
	ii++;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(10);
	MyGraph[ii]->SetTickRange(60);
	MyGraph[ii]->SetCurColorIndex(3);
	MyGraph[ii]->SetXAxisAlignment(270);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("fps"));

	ii++;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(1000);
	MyGraph[ii]->SetTickRange(10000);
	MyGraph[ii]->SetCurColorIndex(4);
	MyGraph[ii]->SetXAxisAlignment(270);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("RPM"));

	for (int i = 0; i < 4; ++i)
	{
		ii++;
		MyGraph[ii] = new CGraph();
		MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
		MyGraph[ii]->SetGraphType(1);
		MyGraph[ii]->SetScale(m_nScale);
		MyGraph[ii]->SetTickSpace(10);
		MyGraph[ii]->SetTickRange(100);
		MyGraph[ii]->SetCurColorIndex(5 + i);
		MyGraph[ii]->SetXAxisAlignment(270);
		MyGraph[ii]->SetXAxisLabel(_T("T"));
//		MyGraph[ii]->SetYAxisLabel(_T("¡æ"));
		MyGraph[ii]->SetYAxisLabel(_T("\x2103"));
	}

	
	if(m_nRecordNum > 0x00) 
	{
		int j = 0;
		S3Time rdtime(0,0,0,0,0,0);
		const COleDateTime oletime;
		CString tickLabel;
		//SYSTEM_STATUS_DESC  stSystemStatusDesc;
		CGraphSeries* tmpSeries;
		std::vector<PerfStatus>::iterator it;
		for(it=m_ClientRTStatisticsList.begin(); it< m_ClientRTStatisticsList.end(); ++it,j++)
		{
			rdtime = it->GetSubmitTime();
		
			const COleDateTime oletime = rdtime.ToCOleDateTime();
			tickLabel.Format(_T("%02d-%02d %02d:%02d"), oletime.GetMonth(),oletime.GetDay(),oletime.GetHour(),oletime.GetMinute());

			tmpSeries = new CGraphSeries();
			//CString tickLabel;
			//tickLabel.Format(_T("%d"), j);
			tmpSeries->SetLabel(tickLabel);
			//tmpSeries->SetLabel(_T("day"));
			tmpSeries->SetData(0,it->GetCpuUsage()%100);
			MyGraph[0]->AddSeries(tmpSeries);

			tmpSeries = new CGraphSeries();
			//CString tickLabel;
			//tickLabel.Format(_T("%d"), j);
			tmpSeries->SetLabel(tickLabel);
			//tmpSeries->SetLabel(_T("day"));
			tmpSeries->SetData(0, it->GetMemUsage() %100);
			MyGraph[1]->AddSeries(tmpSeries);

			tmpSeries = new CGraphSeries();
			//CString tickLabel;
			//tickLabel.Format(_T("%d"), j);
			tmpSeries->SetLabel(tickLabel);
			//tmpSeries->SetLabel(_T("day"));
			tmpSeries->SetData(0, it->GetHarddiskUsage() %100);
			MyGraph[2]->AddSeries(tmpSeries);

			tmpSeries = new CGraphSeries();
			//CString tickLabel;
			//tickLabel.Format(_T("%d"), j);
			tmpSeries->SetLabel(tickLabel);
			//tmpSeries->SetLabel(_T("day"));
			tmpSeries->SetData(0, it->GetFPS() % 60);
			MyGraph[3]->AddSeries(tmpSeries);

			tmpSeries = new CGraphSeries();
			tmpSeries->SetLabel(tickLabel);
			tmpSeries->SetData(0, it->GetFanSpeed() % 10000);
			MyGraph[4]->AddSeries(tmpSeries);

			tmpSeries = new CGraphSeries();
			tmpSeries->SetLabel(tickLabel);
			tmpSeries->SetData(0, it->GetCPUTemperature() % 100);
			MyGraph[5]->AddSeries(tmpSeries);

			tmpSeries = new CGraphSeries();
			tmpSeries->SetLabel(tickLabel);
			tmpSeries->SetData(0, it->GetMBTemperature() % 100);
			MyGraph[6]->AddSeries(tmpSeries);

			tmpSeries = new CGraphSeries();
			tmpSeries->SetLabel(tickLabel);
			tmpSeries->SetData(0, it->GetHDDTemperature() % 100);
			MyGraph[7]->AddSeries(tmpSeries);

			tmpSeries = new CGraphSeries();
			tmpSeries->SetLabel(tickLabel);
			tmpSeries->SetData(0, it->GetGPUTemperature() % 100);
			MyGraph[8]->AddSeries(tmpSeries);
		}
	}
	DrawGraphicBackBuffer();
	UpdateGraphicsDisplay();

	return 0x01;
}

LRESULT CClientChartDlg::UpdatePartialClientHistoryData(COleDateTime starttime,bool bFreshFlag)
{
	S3Time stTime = S3Time::CreateTime(starttime);

	if(m_nRecordNum <= 0x00) 
	{
		return 0x00;
	}

	//release graphic data object
	ReleaseGraphicsData();

	//create graphic data object to show chart
	int ii = 0;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(10);
	MyGraph[ii]->SetTickRange(100);
	MyGraph[ii]->SetCurColorIndex(0);
	MyGraph[ii]->SetXAxisAlignment(270);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("%"));

	ii++;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(10);
	MyGraph[ii]->SetTickRange(100);
	MyGraph[ii]->SetCurColorIndex(1);
	MyGraph[ii]->SetXAxisAlignment(270);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("%"));

	ii++;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(10);
	MyGraph[ii]->SetTickRange(100);
	MyGraph[ii]->SetCurColorIndex(2);
	MyGraph[ii]->SetXAxisAlignment(270);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("%"));

	ii++;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(10);
	MyGraph[ii]->SetTickRange(60);
	MyGraph[ii]->SetCurColorIndex(3);
	MyGraph[ii]->SetXAxisAlignment(270);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("%"));

	ii++;
	MyGraph[ii] = new CGraph();
	MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
	MyGraph[ii]->SetGraphType(1);
	MyGraph[ii]->SetScale(m_nScale);
	MyGraph[ii]->SetTickSpace(1000);
	MyGraph[ii]->SetTickRange(10000);
	MyGraph[ii]->SetCurColorIndex(4);
	MyGraph[ii]->SetXAxisAlignment(270);
	MyGraph[ii]->SetXAxisLabel(_T("T"));
	MyGraph[ii]->SetYAxisLabel(_T("RPM"));

	for (int i = 0; i < 4; ++i)
	{
		ii++;
		MyGraph[ii] = new CGraph();
		MyGraph[ii]->SetGraphTitle(Translate(mDataTypeTitleName[ii]));
		MyGraph[ii]->SetGraphType(1);
		MyGraph[ii]->SetScale(m_nScale);
		MyGraph[ii]->SetTickSpace(10);
		MyGraph[ii]->SetTickRange(100);
		MyGraph[ii]->SetCurColorIndex(5 + i);
		MyGraph[ii]->SetXAxisAlignment(270);
		MyGraph[ii]->SetXAxisLabel(_T("T"));
//		MyGraph[ii]->SetYAxisLabel(_T("¡æ"));
		MyGraph[ii]->SetYAxisLabel(_T("\x2103"));
	}

	//if(starttime.m_dt > 0)
	if(true == bFreshFlag)
	{
		int j = 0;
		S3Time rdtime(0,0,0,0,0,0);
		const COleDateTime oletime;
		CString tickLabel;
		//SYSTEM_STATUS_DESC  stSystemStatusDesc;
		CGraphSeries* tmpSeries;
		std::vector<PerfStatus>::iterator it;
		for(it=m_ClientRTStatisticsList.begin(); it< m_ClientRTStatisticsList.end(); ++it,j++)
		{
			rdtime = it->GetSubmitTime();

			if(rdtime < stTime)
			{
				continue ;// this is not what i want 
			}

			const COleDateTime oletime = rdtime.ToCOleDateTime();
			tickLabel.Format(_T("%d-%d %d:%d"), oletime.GetMonth(),oletime.GetDay(),oletime.GetHour(),oletime.GetMinute());

			tmpSeries = new CGraphSeries();
			//CString tickLabel;
			//tickLabel.Format(_T("%d"), j);
			tmpSeries->SetLabel(tickLabel);
			//tmpSeries->SetLabel(_T("day"));
			tmpSeries->SetData(0,it->GetCpuUsage()%100);
			MyGraph[0]->AddSeries(tmpSeries);

			tmpSeries = new CGraphSeries();
			//CString tickLabel;
			//tickLabel.Format(_T("%d"), j);
			tmpSeries->SetLabel(tickLabel);
			//tmpSeries->SetLabel(_T("day"));
			tmpSeries->SetData(0, it->GetMemUsage() %100);
			MyGraph[1]->AddSeries(tmpSeries);

			tmpSeries = new CGraphSeries();
			//CString tickLabel;
			//tickLabel.Format(_T("%d"), j);
			tmpSeries->SetLabel(tickLabel);
			//tmpSeries->SetLabel(_T("day"));
			tmpSeries->SetData(0, it->GetHarddiskUsage() %100);
			MyGraph[2]->AddSeries(tmpSeries);

			tmpSeries = new CGraphSeries();
			//CString tickLabel;
			//tickLabel.Format(_T("%d"), j);
			tmpSeries->SetLabel(tickLabel);
			//tmpSeries->SetLabel(_T("day"));
			tmpSeries->SetData(0, it->GetFPS() % 60);
			MyGraph[3]->AddSeries(tmpSeries);

			tmpSeries = new CGraphSeries();
			tmpSeries->SetLabel(tickLabel);
			tmpSeries->SetData(0, it->GetFanSpeed() % 10000);
			MyGraph[4]->AddSeries(tmpSeries);

			tmpSeries = new CGraphSeries();
			tmpSeries->SetLabel(tickLabel);
			tmpSeries->SetData(0, it->GetCPUTemperature() % 100);
			MyGraph[5]->AddSeries(tmpSeries);

			tmpSeries = new CGraphSeries();
			tmpSeries->SetLabel(tickLabel);
			tmpSeries->SetData(0, it->GetMBTemperature() % 100);
			MyGraph[6]->AddSeries(tmpSeries);

			tmpSeries = new CGraphSeries();
			tmpSeries->SetLabel(tickLabel);
			tmpSeries->SetData(0, it->GetHDDTemperature() % 100);
			MyGraph[7]->AddSeries(tmpSeries);

			tmpSeries = new CGraphSeries();
			tmpSeries->SetLabel(tickLabel);
			tmpSeries->SetData(0, it->GetGPUTemperature() % 100);
			MyGraph[8]->AddSeries(tmpSeries);
		}
	}
	
	DrawGraphicBackBuffer();
	UpdateGraphicsDisplay();

	return 0x01;
}

void CClientChartDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 

	AfxGetMainWnd()->Invalidate(); 
	AfxGetMainWnd()->UpdateWindow(); 

	UpdateGraphicsDisplay();
}


void CClientChartDlg::DrawGraphicBackBuffer()
{
	CWnd  *hwnd;
	CRect rect;
	int nWidth,nHeight;
	int StartYPos = 0;
	int nGraphHeight;

	hwnd = GetDlgItem(IDC_STATIC_BACKGROUND);
	hwnd->GetWindowRect(&rect);

	nWidth = rect.Width();
	nGraphHeight = m_dcHeight/MY_GRAPH_NUM ; 
	dcMem.FillSolidRect(0,0,m_dcWidth,m_dcHeight,afxGlobalData.GetColor(COLOR_BTNFACE));
	for(int ii = 0 ; ii< MY_GRAPH_NUM;ii++)
	{
		MyGraph[ii]->DrawGraph(&dcMem,0,StartYPos, nWidth,nGraphHeight);
		StartYPos += nGraphHeight;
	}
}

void CClientChartDlg::RetrieveGraphicsData()
{
	//COleDateTime olestarttime;
	//COleDateTime oleendtime;
	COleDateTimeSpan timespan;         // days,hours,minutes,seconds

	timespan.SetDateTimeSpan(7,0,0,0); //retrive 7-days data
	//m_datatimectrl.GetTime(oleendtime);
	//olestarttime = oleendtime - timespan;	
	
	DWORD MessageID = USER_MSG_GET_CLIENT_HISTORY;
	
	ST_CLIENT_RT_STA_PARA * stParameter =  new ST_CLIENT_RT_STA_PARA;
	stParameter->clientid = m_ClientId;
	m_datatimectrl.GetTime(stParameter->endtime);
	stParameter->starttime = stParameter->endtime - timespan;

	m_LastRetrieveStartTime = stParameter->starttime;
	
	if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID, MessageID,	(WPARAM)this->GetSafeHwnd(), (LPARAM)stParameter))
	{
		TRACE1("post message failed,errno:%d\n",::GetLastError());
	}
}


void CClientChartDlg::ReleaseGraphicsData()
{
	for(int ii = 0;ii < MY_GRAPH_NUM; ii++)
	{
		if(MyGraph[ii])
		{
			delete MyGraph[ii];
			MyGraph[ii] = NULL;
		}
	}
}

void CClientChartDlg::UpdateGraphicsDisplay()
{
	CWnd  *hwnd;
	CDC   *mydc;
	CRect rect;

	hwnd = GetDlgItem(IDC_STATIC_FOREGROUND);
	mydc = hwnd->GetDC();
	hwnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	mydc->BitBlt(2,2,rect.Width()-4,rect.Height()-4,&dcMem,0,m_StartY,SRCCOPY);//½«ÄÚ´æDCÉÏµÄÍ¼Ïó¿½±´µ½Ç°Ì¨ 
}


void CClientChartDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;
	
	if(m_nInitFlag)
	{
		CalcLayout();
		//modified by LiuSong
		//DrawGraphicBackBuffer();

		//UpdateGraphicsDisplay();
	}
	// TODO: 
}

void CClientChartDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	/*
	int nCurPos = GetScrollPos(SB_HORZ);
	int nPrevHScrollBarPos = nPos;
	*/
	
	int nCurPos = m_nCurHScrollBarPos;
	
	SCROLLINFO Info;
	GetScrollInfo(SB_HORZ, &Info, SIF_ALL);

	switch (nSBCode)
	{
	case SB_TOP:
		nCurPos = Info.nMin;
		break;
	case SB_BOTTOM:
		nCurPos = Info.nMax;
		break;
	case SB_LINELEFT:
		nCurPos = max(Info.nMin, nCurPos - 10);
		break;
	case SB_LINERIGHT:
		nCurPos = min(Info.nMax, nCurPos + 10);
		break;
	case SB_PAGELEFT:
		nCurPos = max(Info.nMin, nCurPos - (int)Info.nPage);
		break;
	case SB_PAGERIGHT:
		nCurPos = min(Info.nMax, nCurPos + (int)Info.nPage);
		break;
	//case SB_THUMBPOSITION:
	//	nCurPos = nPos;
	//	break;
	case SB_THUMBTRACK:
		nCurPos = nPos;
		break;
	case SB_ENDSCROLL:
		break;
	}

	// TODO: 
	COleDateTime endtime;
	COleDateTime starttime;
	COleDateTimeSpan timespan;//(2,0,0,0); // days,hours,minutes,seconds
	COleDateTimeSpan span(7,0,0,0);

	DWORD dwResult = m_datatimectrl.GetTime(endtime); 
	//if   (dwResult == GDT_VALID) 
	{
		if(nCurPos > m_nCurHScrollBarPos)
		{
			timespan.SetDateTimeSpan((nCurPos-m_nCurHScrollBarPos)/24,0,0,0);
			endtime += timespan;	
			m_datatimectrl.SetTime(endtime);
			
			starttime = endtime - m_Span;
			if(starttime - m_CurDataStartTime  <= span)
			{
				UpdatePartialClientHistoryData(starttime,true);
			}
			else
			{
				starttime.SetDateTime(0,0,0,0,0,0); 
				UpdatePartialClientHistoryData(starttime,false);
			}

		}
		else if(nCurPos < m_nCurHScrollBarPos)
		{
			timespan.SetDateTimeSpan((m_nCurHScrollBarPos - nCurPos)/24,0,0,0);
			endtime -= timespan;	
			m_datatimectrl.SetTime(endtime);

			starttime = endtime - m_Span;
			if( m_CurDataStartTime - endtime <= span)
			{
				UpdatePartialClientHistoryData(starttime,true);
			}
			else
			{
				starttime.SetDateTime(0,0,0,0,0,0); 
				UpdatePartialClientHistoryData(starttime,false);
			}
		}
	}

	switch (nSBCode)
	{
	case SB_THUMBTRACK:
		m_nCurHScrollBarPos = nCurPos;

		break;
	case SB_ENDSCROLL:
		RetrieveGraphicsData();
		break;
	default:
		m_nCurHScrollBarPos = Info.nMax/2;
		break;

	}
	
	//SetScrollPos(SB_HORZ, nCurPos);
	
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CClientChartDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nCurPos = GetScrollPos(SB_VERT);
	SCROLLINFO Info;
	GetScrollInfo(SB_VERT, &Info, SIF_ALL);

	switch (nSBCode)
	{
	case SB_TOP:
		nCurPos = Info.nMin;
		break;
	case SB_BOTTOM:
		nCurPos = Info.nMax;
		break;
	case SB_ENDSCROLL:
		break;
	case SB_LINEUP:
		nCurPos = max(Info.nMin, nCurPos - 10);
		break;
	case SB_LINEDOWN:
		nCurPos = min(Info.nMax, nCurPos + 10);
		break;
	case SB_PAGEUP:
		nCurPos = max(Info.nMin, nCurPos - (int)Info.nPage);
		break;
	case SB_PAGEDOWN:
		nCurPos = min(Info.nMax, nCurPos + (int)Info.nPage);
		break;
	case SB_THUMBPOSITION:
		nCurPos = nPos;
		break;
	case SB_THUMBTRACK:
		nCurPos = nPos;
		break;
	}

	SetScrollPos(SB_VERT, nCurPos);

	if (m_StartY != nCurPos)
	{
		m_StartY = nCurPos;

		CalcLayout();
		UpdateGraphicsDisplay();
	}

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
	
}

VOID CClientChartDlg::CalcLayout()
{
	CRect ClientRect,Rect;
	//GetClientRect(&ClientRect);
	
	//GetDlgItem(IDC_STATIC_BACKGROUND)->GetWindowRect(ClientRect);
	//ScreenToClient(&ClientRect);

	GetDlgItem(IDC_STATIC_FOREGROUND)->GetWindowRect(Rect);
	ScreenToClient(&Rect);

	int TotalLength = m_dcHeight;

	int nCurPos = GetScrollPos(SB_VERT);
	//m_StartY = -nCurPos;
	m_StartY = nCurPos;

	if(TotalLength > Rect.Height())
	{
		EnableScrollBarCtrl(SB_VERT, TRUE);
		EnableScrollBar(SB_VERT, ESB_ENABLE_BOTH );

		SCROLLINFO ScrollInfo;
		ScrollInfo.cbSize = sizeof(SCROLLINFO);
		ScrollInfo.nMin = 0;
		ScrollInfo.nMax = TotalLength;
		ScrollInfo.nPage = Rect.Height();
		ScrollInfo.fMask = SIF_RANGE | SIF_PAGE;

		SetScrollInfo(SB_VERT,&ScrollInfo);

		//m_StartY = max(m_StartY, Rect.Height() - TotalLength );
		m_StartY = min(m_StartY, TotalLength - Rect.Height());
	}
	else
	{
		EnableScrollBarCtrl(SB_VERT, FALSE);
		m_StartY = 0;
	}
}


BOOL CClientChartDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  
	INIT_EASYSIZE;
	SCROLLINFO ScrollInfo;
	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.nMin = 0;
	ScrollInfo.nMax = 24*365*2;//can get two year's data
	ScrollInfo.nPage = 24;     //24 hours
	ScrollInfo.fMask = SIF_RANGE | SIF_PAGE;
	SetScrollInfo(SB_HORZ,&ScrollInfo);
	//SetScrollRange (SB_HORZ, 0, 50, FALSE);
	SetScrollPos(SB_HORZ,ScrollInfo.nMax/2);
	m_nCurHScrollBarPos = GetScrollPos(SB_HORZ);

	RetrieveGraphicsData();
	
	CWnd  *hwnd;
	CDC   *mydc;
	CRect rect;

	hwnd = GetDlgItem(IDC_STATIC_BACKGROUND);
	hwnd->GetWindowRect(&rect);
	mydc = hwnd->GetDC();

	dcMem.CreateCompatibleDC(mydc); 
	bmp.CreateCompatibleBitmap(mydc,m_dcWidth,m_dcHeight);
	dcMem.SelectObject(&bmp); 
	dcMem.FillSolidRect(0,0,m_dcWidth,m_dcHeight,afxGlobalData.GetColor(COLOR_BTNFACE));
	GetDlgItem(IDC_BUTTON1)->SetWindowText(Translate(_T("zoom in")));
	GetDlgItem(IDC_BUTTON3)->SetWindowText(Translate(_T("zoom out")));
	CalcLayout();

	UpdateGraphicsDisplay();

	m_nInitFlag = true;

	return TRUE;  // return TRUE unless you set the focus to a control
}




BOOL CClientChartDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 
	if(zDelta < 0)
	{
		OnVScroll(SB_LINEDOWN, GetScrollPos(SB_VERT), GetScrollBarCtrl(SB_VERT));
	}
	else if (zDelta > 0)
	{
		OnVScroll(SB_LINEUP, GetScrollPos(SB_VERT), GetScrollBarCtrl(SB_VERT));
	}
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CClientChartDlg::OnBnClickedButton1()
{
	int nCount = m_ClientRTStatisticsList.size();

	if(nCount /m_nScale > 10)
	{
		m_nScale *=2;

		for(int ii=0;ii<MY_GRAPH_NUM;ii++)
		{
			MyGraph[ii]->SetScale(m_nScale);
		}

		DrawGraphicBackBuffer();
		UpdateGraphicsDisplay();
	}
}

void CClientChartDlg::OnBnClickedButton3()
{
	// TODO: 
	if(m_nScale >= 2)
	{
		m_nScale /= 2;

		for(int ii=0;ii<MY_GRAPH_NUM;ii++)
		{
			MyGraph[ii]->SetScale(m_nScale);
		}

		DrawGraphicBackBuffer();
		UpdateGraphicsDisplay();
	}
}


void CClientChartDlg::OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: 
	*pResult = 0;
	RetrieveGraphicsData();
}


#endif


