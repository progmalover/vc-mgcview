// CReportCltDldDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ReportCltDldDlg.h"
#include "Localization/Localization.h"
#include "DatetimeCheck.h"
#include <odbcinst.h>
#include  <afxdb.h> 

#include  "LoadingTranslucentDlg.h"

// CReportCltDldDlg dialog

IMPLEMENT_DYNAMIC(CReportCltDldDlg, CDialogEx)

CReportCltDldDlg::CReportCltDldDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CReportCltDldDlg::IDD, pParent)
{

}

CReportCltDldDlg::~CReportCltDldDlg()
{
}

void CReportCltDldDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_REPORT2_KEYWORD, m_strDesc);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_REPORT2_START, m_timeStart);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_REPORT2_END, m_timeEnd);
    DDX_Control(pDX, IDC_COMBO_REPORT2, m_ctlType);
    DDX_Control(pDX, IDC_LIST_REPORT2, m_ReportCltDldList);
}


BEGIN_MESSAGE_MAP(CReportCltDldDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_REPORT2_SEARCH, &CReportCltDldDlg::OnClickedButtonSearch)
    ON_BN_CLICKED(IDC_BTN_REPORT2_EXPORT, &CReportCltDldDlg::OnClickedButtonExport)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_REPORT2_START, &CReportCltDldDlg::OnDtnDatetimechangeDatetimepickStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_REPORT2_END, &CReportCltDldDlg::OnDtnDatetimechangeDatetimepickEnd)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CReportCltDldDlg)
    EASYSIZE(IDC_STATIC_REPORT2_TYPE, ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_COMBO_REPORT2_TYPE,IDC_STATIC_REPORT2_TYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT2_START,IDC_COMBO_REPORT2_TYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_DATETIMEPICKER_REPORT2_START, IDC_STATIC_REPORT2_START, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
    EASYSIZE(IDC_BTN_REPORT2_SEARCH,IDC_DATETIMEPICKER_REPORT2_START,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT2_KEYWORD,ES_BORDER,IDC_STATIC_REPORT2_TYPE,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_EDIT_REPORT2_KEYWORD,IDC_STATIC_REPORT2_KEYWORD,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT2_END,IDC_EDIT_REPORT2_KEYWORD,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_DATETIMEPICKER_REPORT2_END, IDC_STATIC_REPORT2_END,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_BTN_REPORT2_EXPORT, IDC_DATETIMEPICKER_REPORT2_END,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_LIST_REPORT2, ES_BORDER, IDC_STATIC_REPORT2_KEYWORD, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP


// CMediaLibrarySearchBarCtrl message handlers
BOOL CReportCltDldDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;

	GetDlgItem(IDC_DATETIMEPICKER_REPORT2_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_DATETIMEPICKER_REPORT2_END)->EnableWindow(TRUE);


	m_timeStart = COleDateTime::GetCurrentTime();
	m_timeEnd = COleDateTime::GetCurrentTime();


    UpdateType();

	// sync data with GUI
	UpdateData(FALSE);

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CReportCltDldDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}


void CReportCltDldDlg::OnClickedButtonSearch()
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
                         MessageBox(Translate(_T("Please input integer!")), Translate(_T("Warning:Client download query")), MB_OK|MB_ICONEXCLAMATION);
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
        {
            m_strDesc.MakeLower();
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

     sql.Format(_T("call sp_clientdownload(%s)"), cond);

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

     m_ReportCltDldList.DeleteAllItems();
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
                 std::tstring downloadFile = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("DownloadFile"))->GetValue();
                 COleDateTime reportTime = (DATE)pRs->Fields->GetItem(_T("ReportTime"))->GetValue();
                 CString szReportTime(reportTime.Format(_T("%Y-%m-%d %H:%M:%S"))); 
                 std::tstring downStatus = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("DownloadStatus"))->GetValue();

                 szClientID.Format(_T("%d"), clientId);
                 m_ReportCltDldList.InsertItem(i, (LPCTSTR)szClientID);
                 m_ReportCltDldList.SetItemText(i, 1, clientName.c_str());
                 m_ReportCltDldList.SetItemText(i, 2, groupName.c_str());
                 m_ReportCltDldList.SetItemText(i, 3, downloadFile.c_str());
                 m_ReportCltDldList.SetItemText(i, 4, szReportTime);
                 m_ReportCltDldList.SetItemText(i, 5, Translate(downStatus.c_str()));
                 pRs->MoveNext();

                 ++i;
             
             }
         }
         catch (_com_error& e)
         {
             MessageBox(Translate(_T("Database error!")), Translate(_T("Error:Client download query")), MB_OK);
         }
     }
     else
     {
         MessageBox(Translate(_T("No records match the query!")),Translate(_T("Tips:Client download query")), MB_OK);
     }
     pRs = NULL;

     CString InfoStatus("");
     InfoStatus.Format(Translate(_T("Report System List Updated! the result records: %d")), i);

     GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
         (LPARAM)InfoStatus.GetBuffer());

}

void CReportCltDldDlg::SaveCsv(CFile& file, CString szFileName)
{
    // write BOM of UTF-16 LE file
    file.Write("\xff\xfe", 2);

    int nItemCount = m_ReportCltDldList.GetItemCount();

    LVCOLUMN columnData;
    CString columnName;
    int columnNum = 0;

    columnData.mask = LVCF_TEXT;
    columnData.cchTextMax = 100;
    columnData.pszText = columnName.GetBuffer(100);
    CString TableTitle;

    for (int j = 0; m_ReportCltDldList.GetColumn(j, &columnData); j++)
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
        for (int j = 0; m_ReportCltDldList.GetColumn(j, &columnData); j++)
        {
            CString strItemText =  m_ReportCltDldList.GetItemText(i, j);
            str += strItemText + _T(",");
        }
        str += _T("\r\n");
        //CW2A str2(str);
        file.Write(str, _tcslen(str) * sizeof(wchar_t));
    }
    file.Close();
    CString warningStr;
    warningStr.Format(Translate(_T("Save file to %s!")),szFileName);
    MessageBox(warningStr, Translate(_T("Tips:Client download query")), MB_OK);

}

void CReportCltDldDlg::OnClickedButtonExport()
{
    
    CFileDialog mFileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, _T("CSV Files (*.csv)|*.csv|"), this);
    CString szFileSavePath;
    CString szFileName;

    if (mFileDlg.DoModal() == IDOK)
    {
       
        if (m_ReportCltDldList.GetItemCount() > 0)
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
                if (MessageBox(strWarning, Translate(_T("Tips:Client download query")),MB_YESNO) == IDYES)
                {
                                        file.Close();
                    if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
                    {
                        MessageBox(Translate(_T("Create file failed!")), Translate(_T("Error:Client download query")), MB_OK);
                        return;
                    }

                    SaveCsv(file, szFileName);
                }
            }
            else
            {
                if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
                {
                    MessageBox(Translate(_T("Create file failed!")), Translate(_T("Error:Client download query")), MB_OK);
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
            MessageBox(Translate(_T("No data to save!")),Translate(_T("Tips:Client download query")), MB_OK);
            return;
        }            
    }
    
}

void CReportCltDldDlg::UpdateType()
{
    m_ctlType.ResetContent();
    m_ctlType.InsertString(0, Translate(_T("Client ID")));
    m_ctlType.SetItemData(0, -1);        // if you need, you can modify SetItemData second param

    m_ctlType.InsertString(1, Translate(_T("Client Name")));
    m_ctlType.SetItemData(1, 0);

    m_ctlType.InsertString(2, Translate(_T("Group Name")));
    m_ctlType.SetItemData(2, 1);

    m_ctlType.SetCurSel(0);
}


void CReportCltDldDlg::OnDtnDatetimechangeDatetimepickEnd(NMHDR *pNMHDR, LRESULT *pResult)
{
#if 0
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	COleDateTime end(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
	COleDateTime start;
	((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT2_START))->GetTime(start);

	start.SetDateTime(start.GetYear(), start.GetMonth(), start.GetDay(), 0, 0, 0);

	if (end < start)
	{
		MessageBox(Translate(_T("Can't set end date before start date")), Translate(_T("Error")), MB_OK|MB_ICONERROR);
		end.SetDateTime(start.GetYear(), start.GetMonth(), start.GetDay(), 23, 59, 59);
		((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT2_END))->SetTime(end);
		*pResult = 1;
	}
	else
	{
		((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT2_END))->SetTime(end);
		*pResult = 0;
	}
#else
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_REPORT2_START, IDC_DATETIMEPICKER_REPORT2_END, date, false);
    *pResult = 0;
#endif // 0
}

void CReportCltDldDlg::OnDtnDatetimechangeDatetimepickStart(NMHDR *pNMHDR, LRESULT *pResult)
{
#if 0
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	COleDateTime start(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay,23, 59, 59);
	COleDateTime end;
	((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT2_END))->GetTime(end);

	end.SetDateTime(end.GetYear(), end.GetMonth(), end.GetDay(),23, 59, 59);
	if (end < start)
	{
		MessageBox(Translate(_T("Can't set start date after end date")), Translate(_T("Error")), MB_OK|MB_ICONERROR);
		start.SetDateTime(end.GetYear(), end.GetMonth(), end.GetDay(), 0, 0, 0);
		((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT2_START))->SetTime(start);
	}
	else
	{
		((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT2_START))->SetTime(start);
	}
	*pResult = 0;
#else
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_REPORT2_START, IDC_DATETIMEPICKER_REPORT2_END, date, true);
    *pResult = 0;
#endif // 0
}
