// CReportCltAvailDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ReportCltOnlineDlg.h"
#include "Localization/Localization.h"
#include "DatetimeCheck.h"
#include <odbcinst.h>
#include  <afxdb.h> 

#include  "LoadingTranslucentDlg.h"

// CReportCltOnlineDlg dialog

IMPLEMENT_DYNAMIC(CReportCltOnlineDlg, CDialogEx)

CReportCltOnlineDlg::CReportCltOnlineDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CReportCltOnlineDlg::IDD, pParent)
{

}

CReportCltOnlineDlg::~CReportCltOnlineDlg()
{
}

void CReportCltOnlineDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_REPORT7_KEYWORD, m_strDesc);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_REPORT7_START, m_timeStart);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_REPORT7_END, m_timeEnd);
    DDX_Control(pDX, IDC_COMBO_REPORT7_TYPE, m_ctlSearchType);
    DDX_Control(pDX, IDC_LIST_REPORT7, m_ReportCltOnlineList);
}


BEGIN_MESSAGE_MAP(CReportCltOnlineDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_REPORT7_SEARCH, &CReportCltOnlineDlg::OnClickedButtonSearch)
    ON_BN_CLICKED(IDC_BTN_REPORT7_EXPORT, &CReportCltOnlineDlg::OnClickedButtonExport)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_REPORT7_START, &CReportCltOnlineDlg::OnDtnDatetimechangeDatetimepickStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_REPORT7_END, &CReportCltOnlineDlg::OnDtnDatetimechangeDatetimepickEnd)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CReportCltOnlineDlg)
    EASYSIZE(IDC_STATIC_REPORT7_TYPE, ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_COMBO_REPORT7_TYPE,IDC_STATIC_REPORT7_TYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT7_START,IDC_COMBO_REPORT7_TYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_DATETIMEPICKER_REPORT7_START, IDC_STATIC_REPORT7_START, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
    EASYSIZE(IDC_BTN_REPORT7_SEARCH,IDC_DATETIMEPICKER_REPORT7_START,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT7_KEYWORD,ES_BORDER,IDC_STATIC_REPORT7_TYPE,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_EDIT_REPORT7_KEYWORD,IDC_STATIC_REPORT7_KEYWORD,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT7_END,IDC_EDIT_REPORT7_KEYWORD,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_DATETIMEPICKER_REPORT7_END, IDC_STATIC_REPORT7_END,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_BTN_REPORT7_EXPORT, IDC_DATETIMEPICKER_REPORT7_END,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_LIST_REPORT7, ES_BORDER, IDC_STATIC_REPORT7_KEYWORD, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP


// CReportCltOnlineDlg message handlers
BOOL CReportCltOnlineDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;

	GetDlgItem(IDC_DATETIMEPICKER_REPORT7_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_DATETIMEPICKER_REPORT7_END)->EnableWindow(TRUE);


	m_timeStart = COleDateTime::GetCurrentTime();
	m_timeEnd = COleDateTime::GetCurrentTime();

	UpdateSearchType();

	// sync data with GUI
	UpdateData(FALSE);

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CReportCltOnlineDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

void CReportCltOnlineDlg::OnClickedButtonSearch()
{
    GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
        (LPARAM)Translate(_T("Searching......")).GetString());

    UpdateData(TRUE);

    using namespace ADODB;
    _RecordsetPtr pRs;

    CString cond("");
    CString sql("");

    CComboBox *pCBKey = &m_ctlSearchType;
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
             cond.Format(_T("'%%%s%%', '%s', '%s'"), m_strDesc, tmp1, tmp2);
        }
        break;
    default:
        break;
    }
     sql.Format(_T("call sp_clientOnline(%s)"), cond);

     CWaitCursor wait;
     HRESULT hr = GetControllerInterface->QuerySQL(Condition(sql.GetString()), pRs);
     
     if (!SUCCEEDED(hr))
     {
         TRACE0("Failed to query sql by ClientPlaySum stored proc\n");
         GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
             (LPARAM)Translate(_T("Search Failed by QuerySQL")).GetString());
         return;
     }

     m_ReportCltOnlineList.DeleteAllItems();
     int i = 0;
     if (pRs && pRs->RecordCount > 0)
     {
         try
         {
             pRs->MoveFirst();
             CString szGroupID;
             CString szRate;
             while (!pRs->EndOfFile)
             {
                 int groupId = pRs->Fields->GetItem(_T("ClientGroupId"))->GetValue();
                 std::tstring groupName = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("ClientGroupName"))->GetValue();
                 float rate = pRs->Fields->GetItem(_T("OnlineRate"))->GetValue();

                 szGroupID.Format(_T("%d"), groupId);
                 szRate.Format(_T("%.2f%%"), rate * 100);
                 szRate = szRate + szRate + szRate;
                 m_ReportCltOnlineList.InsertItem(i, (LPCTSTR)szGroupID);
                 m_ReportCltOnlineList.SetItemText(i, 1, groupName.c_str());
                 m_ReportCltOnlineList.SetItemText(i, 2, szRate);

                  pRs->MoveNext();
                 ++i;
             }
         }
         catch (_com_error& e)
         {
             MessageBox(Translate(_T("Database error!")),Translate(_T("Error:Client online query")), MB_OK);
         }
     }
     else
     {
         MessageBox(Translate(_T("No records match the query!")),Translate(_T("Tips:Client online query")), MB_OK);
     }
     pRs = NULL;
     CString InfoStatus("");
     InfoStatus.Format(Translate(_T("Report System List Updated! the result records: %d")), i);

     GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
         (LPARAM)InfoStatus.GetBuffer());

}
void CReportCltOnlineDlg::SaveCsv(CFile& file, CString szFileName)
{
    // write BOM of UTF-16 LE file
    file.Write("\xff\xfe", 2);

    int nItemCount = m_ReportCltOnlineList.GetItemCount();

    LVCOLUMN columnData;
    CString columnName;
    int columnNum = 0;

    columnData.mask = LVCF_TEXT;
    columnData.cchTextMax = 100;
    columnData.pszText = columnName.GetBuffer(100);
    CString TableTitle;

    for (int j = 0; m_ReportCltOnlineList.GetColumn(j, &columnData); j++)
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
        for (int j = 0; m_ReportCltOnlineList.GetColumn(j, &columnData); j++)
        {
            CString strItemText =  m_ReportCltOnlineList.GetItemText(i, j);
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
    MessageBox(warningStr,Translate(_T("Tips:Client online query")), MB_OK);

}

void CReportCltOnlineDlg::OnClickedButtonExport()
{
    CFileDialog mFileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, _T("CSV Files (*.csv)|*.csv|"), this);
    CString szFileSavePath;
    CString szFileName;

    if (mFileDlg.DoModal() == IDOK)
    {
        
        if (m_ReportCltOnlineList.GetItemCount() > 0)
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
                if (MessageBox(strWarning, Translate(_T("Tips:Client online query")),MB_YESNO) == IDYES)
                {
                                        file.Close();
                    if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
                    {
                        MessageBox(Translate(_T("Create file failed!")),Translate(_T("Error:Client online query")), MB_OK);
                        return;
                    }

                    SaveCsv(file, szFileName);
                }
            }
            else
            {
                if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
                {
                    MessageBox(Translate(_T("Create file failed!")),Translate(_T("Error:Client online query")), MB_OK);
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
            MessageBox(Translate(_T("No data to save!")),Translate(_T("Tips:Client online query")), MB_OK);
        }            
    }
   
}


void CReportCltOnlineDlg::UpdateSearchType()
{
    m_ctlSearchType.ResetContent();
    m_ctlSearchType.InsertString(0,  Translate(_T("Group Name")));
    m_ctlSearchType.SetItemData(0, -1);        // if you need, you can modify SetItemData second param

    m_ctlSearchType.SetCurSel(0);
}

void CReportCltOnlineDlg::OnDtnDatetimechangeDatetimepickEnd(NMHDR *pNMHDR, LRESULT *pResult)
{
#if 0
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	COleDateTime end(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
	COleDateTime start;
	((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT7_START))->GetTime(start);

	start.SetDateTime(start.GetYear(), start.GetMonth(), start.GetDay(), 0, 0, 0);

	if (end < start)
	{
		MessageBox(Translate(_T("Can't set end date before start date")), Translate(_T("Error")), MB_OK|MB_ICONERROR);
		end.SetDateTime(start.GetYear(), start.GetMonth(), start.GetDay(), 23, 59, 59);
		((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT7_END))->SetTime(end);
		*pResult = 1;
	}
	else
	{
		((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT7_END))->SetTime(end);
		*pResult = 0;
	}
#endif // 0
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_REPORT7_START, IDC_DATETIMEPICKER_REPORT7_END, date, false);
    *pResult = 0;
}

void CReportCltOnlineDlg::OnDtnDatetimechangeDatetimepickStart(NMHDR *pNMHDR, LRESULT *pResult)
{
#if 0
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	COleDateTime start(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay,23, 59, 59);
	COleDateTime end;
	((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT7_END))->GetTime(end);

	end.SetDateTime(end.GetYear(), end.GetMonth(), end.GetDay(),23, 59, 59);
	if (end < start)
	{
		MessageBox(Translate(_T("Can't set start date after end date")), Translate(_T("Error")), MB_OK|MB_ICONERROR);
		start.SetDateTime(end.GetYear(), end.GetMonth(), end.GetDay(), 0, 0, 0);
		((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT7_START))->SetTime(start);
	}
	else
	{
		((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_REPORT7_START))->SetTime(start);
	}
	*pResult = 0;
#endif // 0
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_REPORT7_START, IDC_DATETIMEPICKER_REPORT7_END, date, true);
    *pResult = 0;
}
