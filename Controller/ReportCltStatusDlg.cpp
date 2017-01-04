// MediaLibrarySearchBarCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ReportCltStatusDlg.h"
#include "Localization/Localization.h"
#include "Utilities/StringUtility.h"
#include "DatetimeCheck.h"
#include <algorithm>
#include <odbcinst.h>
#include  <afxdb.h> 

#include  "LoadingTranslucentDlg.h"

// CMediaLibrarySearchBarCtrl dialog

IMPLEMENT_DYNAMIC(CReportCltStatusDlg, CDialogEx)

CReportCltStatusDlg::CReportCltStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CReportCltStatusDlg::IDD, pParent)
{

}

CReportCltStatusDlg::~CReportCltStatusDlg()
{
}

void CReportCltStatusDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_REPORT1_KEYWORD, m_strDesc);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_REPORT1_START, m_timeStart);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_REPORT1_END, m_timeEnd);
    DDX_Control(pDX, IDC_COMBO_REPORT1_TYPE, m_ctlType);
    DDX_Control(pDX, IDC_LIST_REPORT1, m_ReportCltStatusList);
}


BEGIN_MESSAGE_MAP(CReportCltStatusDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_REPORT1_SEARCH, &CReportCltStatusDlg::OnClickedButtonSearch)
    ON_BN_CLICKED(IDC_BTN_REPORT1_EXPORT, &CReportCltStatusDlg::OnClickedButtonExport)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_REPORT1_START, &CReportCltStatusDlg::OnDtnDatetimechangeDatetimepickStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_REPORT1_END, &CReportCltStatusDlg::OnDtnDatetimechangeDatetimepickEnd)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CReportCltStatusDlg)
    EASYSIZE(IDC_STATIC_REPORT1_TYPE, ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_COMBO_REPORT1_TYPE,IDC_STATIC_REPORT1_TYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT1_START,IDC_COMBO_REPORT1_TYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_BTN_REPORT1_SEARCH,IDC_DATETIMEPICKER_REPORT1_START,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT1_KEYWORD,ES_BORDER,IDC_STATIC_REPORT1_TYPE,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_EDIT_REPORT1_KEYWORD,IDC_STATIC_REPORT1_KEYWORD,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT1_END,IDC_EDIT_REPORT1_KEYWORD,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_DATETIMEPICKER_REPORT1_END, IDC_STATIC_REPORT1_END,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_BTN_REPORT1_EXPORT, IDC_DATETIMEPICKER_REPORT1_END,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_LIST_REPORT1, ES_BORDER, IDC_STATIC_REPORT1_KEYWORD, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP


// CMediaLibrarySearchBarCtrl message handlers
BOOL CReportCltStatusDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;

	GetDlgItem(IDC_DATETIMEPICKER_REPORT1_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_DATETIMEPICKER_REPORT1_END)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_REPORT1_KEYWORD)->SetFocus();

	m_timeStart = COleDateTime::GetCurrentTime();
	m_timeEnd = COleDateTime::GetCurrentTime();


    UpdateType();

	// sync data with GUI
	UpdateData(FALSE);

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CReportCltStatusDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

 // this search function search data by stored proc
void CReportCltStatusDlg::OnClickedButtonSearch()
{
    GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
        (LPARAM)Translate(_T("Searching......")).GetString());

    UpdateData(TRUE);


    using namespace ADODB;
    _RecordsetPtr pRs;

    int clientID;
    CString cond;

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
            if (m_strDesc == _T(""))
            {
                cond.Format(_T("'%%%%', '%%%%', '%%%%', '%s', '%s'"), tmp1, tmp2);
            }
            else
            {
                char   ch; 
                for(int i = 0; i < m_strDesc.GetLength();i++) 
                { 
                    ch  =  m_strDesc.GetAt(i);       
                    if((ch   <   '0'   ||   ch   >   '9')  && (ch != '-'))
                    {
                        MessageBox(Translate(_T("Please input integer!")), Translate(_T("Warning:Client status query")),MB_OK|MB_ICONEXCLAMATION);
                         GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
                        (LPARAM)Translate(_T("Input conditions are not valid!")).GetString());
                        return;
                    }
                } 
                clientID = _tstoi((LPCTSTR)m_strDesc);
                CString tmp;
                tmp.Format(_T("%d"), clientID);
                cond.Format(_T("'%%%s%%', '%%%%', '%%%%', '%s', '%s'"), tmp, tmp1, tmp2);
            }  
        }
        break;
    case 0:
        {   m_strDesc.MakeLower();
            cond.Format(_T("'%%%%', '%%%s%%', '%%%%',  '%s', '%s'"), m_strDesc, tmp1, tmp2);
        }
        break;
    case 1:
        {
            m_strDesc.MakeLower();
             cond.Format(_T("'%%%%', '%%%%', '%%%s%%',  '%s', '%s'"), m_strDesc, tmp1, tmp2);
        }
        break;
    default:
        break;
    }


    CString sql("");
    //select ClientID, ClientName, Status, start_time, end_time from client,schedule_item where ClientID = schedule_item.id and ClientID = '1' and start_time >= '2011/06/16 00:00:00' 
   // and end_time <= '2011/06/16 23:59:59';

    sql.Format(_T("call sp_clientstatus(%s)"), cond);
    
    // do search now
    CWaitCursor wait;

    HRESULT hr = GetControllerInterface->QuerySQL(Condition(sql.GetString()), pRs);

    if (!SUCCEEDED(hr))
    {
        TRACE0("Failed to query sql by ClientPlaySum stored proc\n");
        GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
            (LPARAM)Translate(_T("Search Failed by QuerySQL")).GetString());
        return;
    }
   
    m_ReportCltStatusList.DeleteAllItems();
    int i = 0;
    if (pRs && pRs->RecordCount > 0)
    {
        try
        {
            pRs->MoveFirst();
            CString szClientID;
            while (!pRs->EndOfFile)
            {
                int clientId = pRs->Fields->GetItem(_T("ClientId"))->GetValue();
                std::tstring clientName = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("ClientName"))->GetValue();
                std::tstring groupName = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("GroupName"))->GetValue();
                std::tstring status = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("ClientStatus"))->GetValue();
                COleDateTime reportTime = (DATE)pRs->Fields->GetItem(_T("ReportTime"))->GetValue();
                CString szReportTime(reportTime.Format(_T("%Y-%m-%d %H:%M:%S")));
                //report time
                //insert record
                szClientID.Format(_T("%d"), clientId);
                m_ReportCltStatusList.InsertItem(i, (LPCTSTR)szClientID);
                m_ReportCltStatusList.SetItemText(i, 1, clientName.c_str());
                m_ReportCltStatusList.SetItemText(i, 2, groupName.c_str());
                m_ReportCltStatusList.SetItemText(i, 3, Translate(status.c_str()));
                m_ReportCltStatusList.SetItemText(i, 4, szReportTime);
                pRs->MoveNext();

                ++i;
            }
        }
        catch (_com_error& e)
        {
            MessageBox(Translate(_T("Database error!")),Translate(_T("Error:Client status query")),MB_OK|MB_ICONERROR);
        }
    }
    else
    {
        MessageBox(Translate(_T("No records match the query!")),Translate(_T("Tips:Client status query")),MB_OK);
    }
    pRs = NULL;
    
    CString InfoStatus("");
    InfoStatus.Format(Translate(_T("Report System List Updated! the result records: %d")), i);

    GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
        (LPARAM)InfoStatus.GetBuffer());
}

void CReportCltStatusDlg::SaveCsv(CFile& file, CString szFileName)
{
    // write BOM of UTF-16 LE file
    file.Write("\xff\xfe", 2);

    int nItemCount = m_ReportCltStatusList.GetItemCount();

    LVCOLUMN columnData;
    CString columnName;
    int columnNum = 0;

    columnData.mask = LVCF_TEXT;
    columnData.cchTextMax = 100;
    columnData.pszText = columnName.GetBuffer(100);
    CString TableTitle;

    for (int j = 0; m_ReportCltStatusList.GetColumn(j, &columnData); j++)
    {
        TableTitle += columnData.pszText;
        TableTitle += _T(",");
    }
    TableTitle.TrimRight(_T(",")); 
    TableTitle += _T("\r\n");
   // CW2A TableTitle2(TableTitle);
  
    file.Write(TableTitle, _tcslen(TableTitle) * sizeof(wchar_t));
 

    for(int i = 0; i < nItemCount; i++)
    {
        CString str;
        for (int j = 0; m_ReportCltStatusList.GetColumn(j, &columnData); j++)
        {
            CString strItemText =  m_ReportCltStatusList.GetItemText(i, j);
            str += strItemText + _T(",");
        }
        str.TrimRight(_T(","));
        str += _T("\r\n");
      //  CW2A str2(str);
        file.Write(str, _tcslen(str) * sizeof(wchar_t));
    }
    file.Close();
    CString warningStr;
    warningStr.Format(Translate(_T("Save file to %s!")),szFileName);
    MessageBox(warningStr, Translate(_T("Tips:Client status query")),MB_OK);
    
}

void CReportCltStatusDlg::OnClickedButtonExport()
{

    CFileDialog mFileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, _T("CSV Files (*.csv)|*.csv|"), this);
    CString szFileSavePath;
    CString szFileName;

    if (mFileDlg.DoModal() == IDOK)
    {
      
        if (m_ReportCltStatusList.GetItemCount() > 0)
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
                if (MessageBox(strWarning,Translate(_T("Tips:Client status query")), MB_YESNO) == IDYES)
                {
                                        file.Close();
                    if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
                    {
                        MessageBox(Translate(_T("Create file failed!")), Translate(_T("Error:Client status query")),MB_OK|MB_ICONERROR);
                        return;
                    }

                    SaveCsv(file, szFileName);
                }
            }
            else
            {
                if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
                {
                    MessageBox(Translate(_T("Create file failed!")), Translate(_T("Error:Client status query")),MB_OK|MB_ICONERROR);
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
            MessageBox(Translate(_T("No data to save!")),Translate(_T("Tips:Client status query")),MB_OK|MB_ICONEXCLAMATION);
            return;
        }            
    }
    
}


void CReportCltStatusDlg::UpdateType()
{
    m_ctlType.ResetContent();
    m_ctlType.InsertString(0,  Translate(_T("Client ID")));
    m_ctlType.SetItemData(0, -1);        // if you need, you can modify SetItemData second param

    m_ctlType.InsertString(1,  Translate(_T("Client Name")));
    m_ctlType.SetItemData(1, 0);

    m_ctlType.InsertString(2,  Translate(_T("Group Name")));
    m_ctlType.SetItemData(2, 1);

    m_ctlType.SetCurSel(0);
}

void CReportCltStatusDlg::OnDtnDatetimechangeDatetimepickEnd(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_REPORT1_START, IDC_DATETIMEPICKER_REPORT1_END, date, false);
    *pResult = 0;
}

void CReportCltStatusDlg::OnDtnDatetimechangeDatetimepickStart(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_REPORT1_START, IDC_DATETIMEPICKER_REPORT1_END, date, true);
    *pResult = 0;
}
