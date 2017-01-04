// CReportCltPlayDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ReportCltPlaySumDlg.h"
#include "Localization/Localization.h"
#include "DatetimeCheck.h"
#include <odbcinst.h>
#include  <afxdb.h> 

#include  "LoadingTranslucentDlg.h"

// CReportCltPlaySumDlg dialog

IMPLEMENT_DYNAMIC(CReportCltPlaySumDlg, CDialogEx)

CReportCltPlaySumDlg::CReportCltPlaySumDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CReportCltPlaySumDlg::IDD, pParent)
{

}

CReportCltPlaySumDlg::~CReportCltPlaySumDlg()
{
}

void CReportCltPlaySumDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_REPORT4_KEYWORD, m_strDesc);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_REPORT4_START, m_timeStart);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_REPORT4_END, m_timeEnd);
    DDX_Control(pDX, IDC_COMBO_REPORT4_TYPE, m_ctlType);
    DDX_Control(pDX, IDC_LIST_REPORT4, m_ReportCltPlaySumList);
}


BEGIN_MESSAGE_MAP(CReportCltPlaySumDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_REPORT4_SEARCH, &CReportCltPlaySumDlg::OnClickedButtonSearch)
    ON_BN_CLICKED(IDC_BTN_REPORT4_EXPORT, &CReportCltPlaySumDlg::OnClickedButtonExport)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_REPORT4_START, &CReportCltPlaySumDlg::OnDtnDatetimechangeDatetimepickStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_REPORT4_END, &CReportCltPlaySumDlg::OnDtnDatetimechangeDatetimepickEnd)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CReportCltPlaySumDlg)
    EASYSIZE(IDC_STATIC_REPORT4_TYPE, ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_COMBO_REPORT4_TYPE,IDC_STATIC_REPORT4_TYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT4_START,IDC_COMBO_REPORT4_TYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_DATETIMEPICKER_REPORT4_START, IDC_STATIC_REPORT4_START, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
    EASYSIZE(IDC_BTN_REPORT4_SEARCH,IDC_DATETIMEPICKER_REPORT4_START,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT4_KEYWORD,ES_BORDER,IDC_STATIC_REPORT4_TYPE,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_EDIT_REPORT4_KEYWORD,IDC_STATIC_REPORT4_KEYWORD,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT4_END,IDC_EDIT_REPORT4_KEYWORD,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_DATETIMEPICKER_REPORT4_END, IDC_STATIC_REPORT4_END,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_BTN_REPORT4_EXPORT, IDC_DATETIMEPICKER_REPORT4_END,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_LIST_REPORT4, ES_BORDER, IDC_STATIC_REPORT4_KEYWORD, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP


// CReportCltPlaySumDlg message handlers
BOOL CReportCltPlaySumDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;

	GetDlgItem(IDC_DATETIMEPICKER_REPORT4_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_DATETIMEPICKER_REPORT4_END)->EnableWindow(TRUE);


	m_timeStart = COleDateTime::GetCurrentTime();
	m_timeEnd = COleDateTime::GetCurrentTime();


    UpdateType();

	// sync data with GUI
	UpdateData(FALSE);

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CReportCltPlaySumDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

void CReportCltPlaySumDlg::OnClickedButtonSearch()
{
    GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
        (LPARAM)Translate(_T("Searching......")).GetString());

    UpdateData(TRUE);


    using namespace ADODB;
    _RecordsetPtr pRs;

    CString cond("");
    CString sql("");

    CComboBox *pCBKey = &m_ctlType;
    int KeywordID = pCBKey->GetItemData(pCBKey->GetCurSel());

    S3Time startTime(S3Time::NULLTime), endTime(S3Time::OneDayAfter());
    ATL::COleDateTime t1, t2;


    t1.SetDateTime(m_timeStart.GetYear(),m_timeStart.GetMonth(), m_timeStart.GetDay(),0, 0,0);
    t2.SetDateTime(m_timeEnd.GetYear(), m_timeEnd.GetMonth(), m_timeEnd.GetDay(), 23,59,59);

    startTime = S3Time::CreateTime(t1);
    endTime = S3Time::CreateTime(t2);

    std::wstring tmpStart = startTime.ToString();
    std::wstring tmpEnd = endTime.ToString();
    CString tmp1(tmpStart.c_str());
    CString tmp2(tmpEnd.c_str());


    switch(KeywordID)
    {
    case -1:
        {
            m_strDesc.MakeLower();
            cond.Format(_T("'%%%s%%','%s','%s'"), m_strDesc, tmp1, tmp2);
        }
        break;
    default:
        break;
    }
    sql.Format(_T("call sp_ClientPlaySum(%s)"), cond);

    CWaitCursor wait;
    HRESULT hr = GetControllerInterface->QuerySQL(Condition(sql.GetString()), pRs);

     if (!SUCCEEDED(hr))
    {
        TRACE0("Failed to query sql by ClientPlaySum stored proc\n");
        GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
            (LPARAM)Translate(_T("Search Failed by QuerySQL")).GetString());
        return;
    }

    m_ReportCltPlaySumList.DeleteAllItems();
    int i = 0;
    if (pRs && pRs->RecordCount > 0)
    {
        try
        {
            pRs->MoveFirst();
            CString szPlayTaskID;
            CString szTimes;
            while(!pRs->EndOfFile)
            {
                std::tstring TaskName = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("PlayTaskName"))->GetValue();
                std::tstring TaskType = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("PlayTaskType"))->GetValue();
                float TaskTimes = pRs->Fields->GetItem(_T("PlayTaskTimes"))->GetValue();

                szTimes.Format(_T("%.2f"), TaskTimes);

                m_ReportCltPlaySumList.InsertItem(i, TaskName.c_str());
                m_ReportCltPlaySumList.SetItemText(i, 1, TaskType.c_str());
                m_ReportCltPlaySumList.SetItemText(i, 2, szTimes);

                pRs->MoveNext();
                ++i;
            }

        }
        catch (_com_error& e)
        {
            MessageBox(Translate(_T("Database error!")),Translate(_T("Error:Client playsum query")),MB_OK|MB_ICONERROR);
        }
    }
    else
    {
        MessageBox(Translate(_T("No records match the query!")),Translate(_T("Tips:Client playsum query")),MB_OK|MB_ICONEXCLAMATION);
    }
    pRs = NULL;
    CString InfoStatus("");
    InfoStatus.Format(Translate(_T("Report System List Updated! the result records: %d")), i);

    GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
        (LPARAM)InfoStatus.GetBuffer());
}


void CReportCltPlaySumDlg::SaveCsv(CFile& file, CString szFileName)
{
    // write BOM of UTF-16 LE file
    file.Write("\xff\xfe", 2);

    int nItemCount = m_ReportCltPlaySumList.GetItemCount();

    LVCOLUMN columnData;
    CString columnName;
    int columnNum = 0;

    columnData.mask = LVCF_TEXT;
    columnData.cchTextMax = 100;
    columnData.pszText = columnName.GetBuffer(100);
    CString TableTitle;

    for (int j = 0; m_ReportCltPlaySumList.GetColumn(j, &columnData); j++)
    {
        TableTitle += columnData.pszText;
        TableTitle += _T(",");
    }
    TableTitle += _T("\r\n");
    //CW2A TableTitle2(TableTitle);
    file.Write(TableTitle, _tcslen(TableTitle) * sizeof(wchar_t));

    for(int i = 0; i < nItemCount; i++)
    {
        CString str;
        for (int j = 0; m_ReportCltPlaySumList.GetColumn(j, &columnData); j++)
        {
            CString strItemText =  m_ReportCltPlaySumList.GetItemText(i, j);
            str += strItemText + _T(",");
        }
        str += _T("\r\n");
        //CW2A str2(str);
        file.Write(str, _tcslen(str) * sizeof(wchar_t));

    }
    file.Close();
    CString warningStr;
    warningStr.Format(Translate(_T("Save file to %s!")),szFileName);
    MessageBox(warningStr,Translate(_T("Tips:Client play query")),MB_OK|MB_ICONEXCLAMATION);

}

void CReportCltPlaySumDlg::OnClickedButtonExport()
{
 
    CFileDialog mFileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY , _T("CSV Files (*.csv)|*.csv|"), this);
    CString szFileSavePath;
    CString szFileName;

    if (mFileDlg.DoModal() == IDOK)
    {
        
        if (m_ReportCltPlaySumList.GetItemCount() > 0)
        { 
            szFileSavePath = mFileDlg.GetPathName();

            CString ext = szFileSavePath.Right(4);

            if (ext.CompareNoCase(_T(".csv")) != 0)
            {
                szFileName = szFileSavePath + _T(".csv");
            }
            else
            {
                szFileName = szFileSavePath;
            }

            CFile file;
            if (file.Open(szFileName, CFile::modeWrite))
            {
                CString strWarning;
                strWarning.Format(Translate(_T("The file %s have exist! Are you want to overwrite?")), szFileName);
                if (MessageBox(strWarning, Translate(_T("Tips:Client playsum query")),MB_YESNO) == IDYES)
                {
                                        file.Close();
                    if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
                    {
                        MessageBox(Translate(_T("Create file failed!")),Translate(_T("Error:Client playsum query")),MB_OK|MB_ICONERROR);
                        return;
                    }

                    SaveCsv(file, szFileName);
                }
            }
            else
            {

                if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
                {
                    MessageBox(Translate(_T("Create file failed!")),Translate(_T("Error:Client play query")),MB_OK|MB_ICONERROR);
                    return;
                }
                else
                {
                    SaveCsv(file, szFileName);
                }
            }
        }
        else
        {
            MessageBox(Translate(_T("No data to save!")),Translate(_T("Tips:Client play query")),MB_OK|MB_ICONEXCLAMATION);
            return;
        }            
    }
   
}


void CReportCltPlaySumDlg::UpdateType()
{
    
    m_ctlType.ResetContent();

    m_ctlType.InsertString(0, Translate(_T("Task Name")));
    m_ctlType.SetItemData(0, -1);

    m_ctlType.SetCurSel(0);
}

void CReportCltPlaySumDlg::OnDtnDatetimechangeDatetimepickEnd(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_REPORT4_START, IDC_DATETIMEPICKER_REPORT4_END, date, false);
    *pResult = 0;
}

void CReportCltPlaySumDlg::OnDtnDatetimechangeDatetimepickStart(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_REPORT4_START, IDC_DATETIMEPICKER_REPORT4_END, date, true);
    *pResult = 0;
}
