// CReportCltAvailDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ReportCltAvailDlg.h"
#include "Localization/Localization.h"
#include "DatetimeCheck.h"
#include <odbcinst.h>
#include  <afxdb.h> 

#include  "LoadingTranslucentDlg.h"

// CReportCltAvailDlg dialog

IMPLEMENT_DYNAMIC(CReportCltAvailDlg, CDialogEx)

CReportCltAvailDlg::CReportCltAvailDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CReportCltAvailDlg::IDD, pParent)
{

}

CReportCltAvailDlg::~CReportCltAvailDlg()
{
}

void CReportCltAvailDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_REPORT6_KEYWORD, m_strDesc);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_REPORT6_START, m_timeStart);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_REPORT6_END, m_timeEnd);
    DDX_Control(pDX, IDC_COMBO_REPORT6_TYPE, m_ctlType);
    DDX_Control(pDX, IDC_LIST_REPORT6, m_ReportCltAvailList);
}


BEGIN_MESSAGE_MAP(CReportCltAvailDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_REPORT6_SEARCH, &CReportCltAvailDlg::OnClickedButtonSearch)
    ON_BN_CLICKED(IDC_BTN_REPORT6_EXPORT, &CReportCltAvailDlg::OnClickedButtonExport)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_REPORT6_START, &CReportCltAvailDlg::OnDtnDatetimechangeDatetimepickStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_REPORT6_END, &CReportCltAvailDlg::OnDtnDatetimechangeDatetimepickEnd)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CReportCltAvailDlg)
    EASYSIZE(IDC_STATIC_REPORT6_TYPE, ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_COMBO_REPORT6_TYPE,IDC_STATIC_REPORT6_TYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT6_START,IDC_COMBO_REPORT6_TYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_DATETIMEPICKER_REPORT6_START, IDC_STATIC_REPORT6_START, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
    EASYSIZE(IDC_BTN_REPORT6_SEARCH,IDC_DATETIMEPICKER_REPORT6_START,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT6_KEYWORD,ES_BORDER,IDC_STATIC_REPORT6_TYPE,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_EDIT_REPORT6_KEYWORD,IDC_STATIC_REPORT6_KEYWORD,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT6_END,IDC_EDIT_REPORT6_KEYWORD,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_DATETIMEPICKER_REPORT6_END, IDC_STATIC_REPORT6_END,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_BTN_REPORT6_EXPORT, IDC_DATETIMEPICKER_REPORT6_END,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_LIST_REPORT6, ES_BORDER, IDC_STATIC_REPORT6_KEYWORD, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP


// CReportCltAvailDlg message handlers
BOOL CReportCltAvailDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;

	GetDlgItem(IDC_DATETIMEPICKER_REPORT6_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_DATETIMEPICKER_REPORT6_END)->EnableWindow(TRUE);


	m_timeStart = COleDateTime::GetCurrentTime();
	m_timeEnd = COleDateTime::GetCurrentTime();


    UpdateType();

	// sync data with GUI
	UpdateData(FALSE);

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CReportCltAvailDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

void CReportCltAvailDlg::OnClickedButtonSearch()
{
    GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
    (LPARAM)Translate(_T("Searching......")).GetString());

    UpdateData(TRUE);

    using namespace ADODB;
    _RecordsetPtr pRs;

    int clientID;
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
                        MessageBox(Translate(_T("Please input integer!")),Translate(_T("Warning:Search Client Availiablity")),MB_OK|MB_ICONEXCLAMATION);
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

    sql.Format(_T("call sp_clientAvail(%s)"), cond);

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

     m_ReportCltAvailList.DeleteAllItems();
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
                 float Avail = pRs->Fields->GetItem(_T("Availability"))->GetValue();
                 CString szAvail;
                 szAvail.Format(_T("%.2f"), Avail*100);
                 szAvail += _T("%");
                 szAvail = szAvail + szAvail + szAvail;
                 //report time
                 //insert record
                 szClientID.Format(_T("%d"), clientId);
                 m_ReportCltAvailList.InsertItem(i, (LPCTSTR)szClientID);
                 m_ReportCltAvailList.SetItemText(i, 1, clientName.c_str());
                 m_ReportCltAvailList.SetItemText(i, 2, groupName.c_str());
                 m_ReportCltAvailList.SetItemText(i, 3, szAvail);
                 pRs->MoveNext();

                 ++i;
             }
         }
         catch (_com_error& e)
         {
             MessageBox(Translate(_T("Database error!")), Translate(_T("Error:Search Client Availiablity")), MB_OK|MB_ICONERROR);
         }
     }
     else
     {
         MessageBox(Translate(_T("No records match the query!")),Translate(_T("Tips:Search Client Availiablity")), MB_OK|MB_ICONINFORMATION);
     }
     pRs = NULL;
 
     CString InfoStatus("");
     InfoStatus.Format(Translate(_T("Report System List Updated! the result records: %d")), i);

     GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
         (LPARAM)InfoStatus.GetBuffer());
}


void CReportCltAvailDlg::SaveCsv(CFile& file, CString szFileName)
{
    // write BOM of UTF-16 LE file
    file.Write("\xff\xfe", 2);

    int nItemCount = m_ReportCltAvailList.GetItemCount();

    LVCOLUMN columnData;
    CString columnName;
    int columnNum = 0;

    columnData.mask = LVCF_TEXT;
    columnData.cchTextMax = 100;
    columnData.pszText = columnName.GetBuffer(100);
    CString TableTitle;

    for (int j = 0; m_ReportCltAvailList.GetColumn(j, &columnData); j++)
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
        for (int j = 0; m_ReportCltAvailList.GetColumn(j, &columnData); j++)
        {
            CString strItemText =  m_ReportCltAvailList.GetItemText(i, j);
            int len = strItemText.Find('%');
            if ( len!= -1)
            {
                strItemText.Delete(len + 1, strItemText.GetLength() - 1);
            }
            str += strItemText + _T(",");
        }
        str += _T("\r\n");
        //CW2A str2(str);
        file.Write(str, _tcslen(str) * sizeof(wchar_t));
    }
    file.Close();
    CString warningStr;
    warningStr.Format(Translate(_T("Save file to %s!")),szFileName);
    MessageBox(warningStr,Translate(_T("Save csv")), MB_OK);

}


void CReportCltAvailDlg::OnClickedButtonExport()
{
                   
    CFileDialog mFileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY,  _T("CSV Files (*.csv)|*.csv|"), this);
    CString szFileSavePath;
    CString szFileName;

    if (mFileDlg.DoModal() == IDOK)
    {
        if (m_ReportCltAvailList.GetItemCount() > 0)
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
                if (MessageBox(strWarning, Translate(_T("Tips:Client Availablity")), MB_YESNO) == IDYES)
                {
                    file.Close();
                    if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
                    {
                        MessageBox(Translate(_T("Create file failed!")), Translate(_T("Error:Client availablity query")), MB_OK);
                        return;
                    }
                    SaveCsv(file, szFileName);
                }
            }
            else
            {
                if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
                {
                    MessageBox(Translate(_T("Create file failed!")),Translate(_T("Error:Client availablity query")), MB_OK);
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
            MessageBox(Translate(_T("No data to save!")),Translate(_T("Tips:Client availablity query")), MB_OK);
            return;
        }            
    }
    
}


void CReportCltAvailDlg::UpdateType()
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

void CReportCltAvailDlg::OnDtnDatetimechangeDatetimepickEnd(NMHDR *pNMHDR, LRESULT *pResult)
{
#if 0
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	COleDateTime end(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
	COleDateTime start;
	((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT6_START))->GetTime(start);

	start.SetDateTime(start.GetYear(), start.GetMonth(), start.GetDay(), 0, 0, 0);

	if (end < start)
	{
		MessageBox(Translate(_T("Can't set end date before start date")), Translate(_T("Error")), MB_OK|MB_ICONERROR);
		end.SetDateTime(start.GetYear(), start.GetMonth(), start.GetDay(), 23, 59, 59);
		((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT6_END))->SetTime(end);
		*pResult = 1;
	}
	else
	{
		((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT6_END))->SetTime(end);
		*pResult = 0;
	}
#else
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_REPORT6_START, IDC_DATETIMEPICKER_REPORT6_END, date, false);
    *pResult = 0;
#endif
}

void CReportCltAvailDlg::OnDtnDatetimechangeDatetimepickStart(NMHDR *pNMHDR, LRESULT *pResult)
{
#if 0
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	COleDateTime start(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay,23, 59, 59);
	COleDateTime end;
	((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT6_END))->GetTime(end);

	end.SetDateTime(end.GetYear(), end.GetMonth(), end.GetDay(),23, 59, 59);
	if (end < start)
	{
		MessageBox(Translate(_T("Can't set start date after end date")), Translate(_T("Error")), MB_OK|MB_ICONERROR);
		start.SetDateTime(end.GetYear(), end.GetMonth(), end.GetDay(), 0, 0, 0);
		((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT6_START))->SetTime(start);
	}
	else
	{
		((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT6_START))->SetTime(start);
	}
	*pResult = 0;
#else
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_REPORT6_START, IDC_DATETIMEPICKER_REPORT6_END, date, true);
    *pResult = 0;
#endif // 0
}
