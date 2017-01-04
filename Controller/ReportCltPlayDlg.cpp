// CReportCltPlayDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ReportCltPlayDlg.h"
#include "Localization/Localization.h"
#include "DatetimeCheck.h"
#include <odbcinst.h>
#include  <afxdb.h> 

#include  "LoadingTranslucentDlg.h"

// CReportCltPlayDlg dialog

IMPLEMENT_DYNAMIC(CReportCltPlayDlg, CDialogEx)

CReportCltPlayDlg::CReportCltPlayDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CReportCltPlayDlg::IDD, pParent)
{

}

CReportCltPlayDlg::~CReportCltPlayDlg()
{
}

void CReportCltPlayDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_REPORT3_KEYWORD, m_strDesc);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_REPORT3_START, m_timeStart);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_REPORT3_END, m_timeEnd);
    DDX_Control(pDX, IDC_COMBO_REPORT3_TYPE, m_ctlType);
    DDX_Control(pDX, IDC_LIST_REPORT3, m_ReportCltPlayList);
}


BEGIN_MESSAGE_MAP(CReportCltPlayDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_REPORT3_SEARCH, &CReportCltPlayDlg::OnClickedButtonSearch)
    ON_BN_CLICKED(IDC_BTN_REPORT3_EXPORT, &CReportCltPlayDlg::OnClickedButtonExport)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_REPORT3_START, &CReportCltPlayDlg::OnDtnDatetimechangeDatetimepickStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_REPORT3_END, &CReportCltPlayDlg::OnDtnDatetimechangeDatetimepickEnd)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CReportCltPlayDlg)
    EASYSIZE(IDC_STATIC_REPORT3_TYPE, ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_COMBO_REPORT3_TYPE,IDC_STATIC_REPORT3_TYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT3_START,IDC_COMBO_REPORT3_TYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_DATETIMEPICKER_REPORT3_START, IDC_STATIC_REPORT3_START, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
    EASYSIZE(IDC_BTN_REPORT3_SEARCH,IDC_DATETIMEPICKER_REPORT3_START,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT3_KEYWORD,ES_BORDER,IDC_STATIC_REPORT3_TYPE,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_EDIT_REPORT3_KEYWORD,IDC_STATIC_REPORT3_KEYWORD,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT3_END,IDC_EDIT_REPORT3_KEYWORD,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_DATETIMEPICKER_REPORT3_END, IDC_STATIC_REPORT3_END,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_BTN_REPORT3_EXPORT, IDC_DATETIMEPICKER_REPORT3_END,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_LIST_REPORT3, ES_BORDER, IDC_STATIC_REPORT3_KEYWORD, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP


// CMediaLibrarySearchBarCtrl message handlers
BOOL CReportCltPlayDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;

	GetDlgItem(IDC_DATETIMEPICKER_REPORT3_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_DATETIMEPICKER_REPORT3_END)->EnableWindow(TRUE);


	m_timeStart = COleDateTime::GetCurrentTime();
	m_timeEnd = COleDateTime::GetCurrentTime();


    UpdateType();

	// sync data with GUI
	UpdateData(FALSE);

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CReportCltPlayDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

void CReportCltPlayDlg::OnClickedButtonSearch()
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
                cond.Format(_T("'%%%%', '%%%%', '%%%%', '%%%%', '%s', '%s'"), tmp1, tmp2);
            }
            else
            {
                char   ch; 
                for(int i = 0; i < m_strDesc.GetLength();i++) 
                { 
                    ch  =  m_strDesc.GetAt(i);       
                    if((ch   <   '0'   ||   ch   >   '9')  && (ch != '-'))
                    {
                        MessageBox(Translate(_T("Please input integer!")),Translate(_T("Warning:Client play query")),MB_OK|MB_ICONEXCLAMATION);
                         GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
                        (LPARAM)Translate(_T("Input conditions are not valid!")).GetString());
                        return;
                    }
                } 
                clientID = _tstoi((LPCTSTR)m_strDesc);
                CString tmp;
                tmp.Format(_T("%d"), clientID);
                cond.Format(_T("'%%%s%%', '%%%%', '%%%%', '%%%%', '%s', '%s'"), tmp, tmp1, tmp2);
            }
        }
        break;
    case 0:
        {
            m_strDesc.MakeLower();
            cond.Format(_T("'%%%%','%%%s%%','%%%%','%%%%','%s','%s'"), m_strDesc, tmp1, tmp2);
        }
        break;
    case 1:
        {
            m_strDesc.MakeLower();
            cond.Format(_T("'%%%%','%%%%','%%%s%%','%%%%','%s','%s'"), m_strDesc, tmp1, tmp2);
        }
        break;
    case 2:
        {
            m_strDesc.MakeLower();
            cond.Format(_T("'%%%%','%%%%','%%%%','%%%s%%','%s','%s'"), m_strDesc, tmp1, tmp2);
        }
        break;
    default:
        break;
    }
    CString sql("");
    sql.Format(_T("call sp_clientPlay(%s)"), cond);

    CWaitCursor wait;

    HRESULT hr = GetControllerInterface->QuerySQL(Condition(sql.GetString()), pRs);

    if (!SUCCEEDED(hr))
    {
        TRACE0("Failed to query sql by ClientPlaySum stored proc\n");
        GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
            (LPARAM)Translate(_T("Search Failed by QuerySQL")).GetString());
        return;
    }
    m_ReportCltPlayList.DeleteAllItems();
    int i = 0;

    if (pRs && pRs->RecordCount > 0)
    {
        try
        {
            pRs->MoveFirst();
            CString szClientID;
            CString szTimes;
            while (!pRs->EndOfFile)
            {
                int clientId = pRs->Fields->GetItem(_T("ClientId"))->GetValue();
                std::tstring clientName = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("ClientName"))->GetValue();
                std::tstring groupName = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("GroupName"))->GetValue();
                std::tstring taskName = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("PlayTaskName"))->GetValue();
                float taskTimes = pRs->Fields->GetItem(_T("PlayTaskTimes"))->GetValue();

                COleDateTime startTime = (DATE)pRs->Fields->GetItem(_T("PlayStartTime"))->GetValue();
                COleDateTime endTime = (DATE)pRs->Fields->GetItem(_T("PlayEndTime"))->GetValue();
                CString szStartTime(startTime.Format(_T("%Y-%m-%d %H:%M:%S")));
                CString szEndTime(endTime.Format(_T("%Y-%m-%d %H:%M:%S")));

                szClientID.Format(_T("%d"), clientId);
                szTimes.Format(_T("%.2f"), taskTimes);

                m_ReportCltPlayList.InsertItem(i, (LPCTSTR)szClientID);
                m_ReportCltPlayList.SetItemText(i, 1, clientName.c_str());
                m_ReportCltPlayList.SetItemText(i, 2, groupName.c_str());
                m_ReportCltPlayList.SetItemText(i, 3, taskName.c_str());
                m_ReportCltPlayList.SetItemText(i, 4, szTimes);
                m_ReportCltPlayList.SetItemText(i, 5, szStartTime);
                m_ReportCltPlayList.SetItemText(i, 6, szEndTime);
                pRs->MoveNext();

                ++i;

            }
        }
        catch (_com_error& e)
        {
            MessageBox(Translate(_T("Database error!")),Translate(_T("Error:Client play query")),MB_OK|MB_ICONERROR);
        }
    }
    else
    {
        MessageBox(Translate(_T("No records match the query!")),Translate(_T("Tips:Client play query")),MB_OK|MB_ICONEXCLAMATION);
    }
    pRs = NULL;

    CString InfoStatus("");
    InfoStatus.Format(Translate(_T("Report System List Updated! the result records: %d")), i);

    GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
        (LPARAM)InfoStatus.GetBuffer());
}

void CReportCltPlayDlg::SaveCsv(CFile& file, CString szFileName)
{
    // write BOM of UTF-16 LE file
    file.Write("\xff\xfe", 2);

    int nItemCount = m_ReportCltPlayList.GetItemCount();

    LVCOLUMN columnData;
    CString columnName;
    int columnNum = 0;

    columnData.mask = LVCF_TEXT;
    columnData.cchTextMax = 100;
    columnData.pszText = columnName.GetBuffer(100);
    CString TableTitle;

    for (int j = 0; m_ReportCltPlayList.GetColumn(j, &columnData); j++)
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
        for (int j = 0; m_ReportCltPlayList.GetColumn(j, &columnData); j++)
        {
            CString strItemText =  m_ReportCltPlayList.GetItemText(i, j);
            str += strItemText + _T(",");
        }
        str += _T("\r\n");
        //CW2A str2(str);
        file.Write(str, _tcslen(str) * sizeof(wchar_t));
    }
    file.Close();
    CString warningStr;
    warningStr.Format(Translate(_T("Save file to %s!")),szFileName);
    MessageBox(warningStr,Translate(_T("Tips:Client play query")),MB_OK);
}

void CReportCltPlayDlg::OnClickedButtonExport()
{ 
    CFileDialog mFileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY,  _T("CSV Files (*.csv)|*.csv|"), this);
    CString szFileSavePath;
    CString szFileName;

    if (mFileDlg.DoModal() == IDOK)
    {
        
        if (m_ReportCltPlayList.GetItemCount() > 0)
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
                if (MessageBox(strWarning, Translate(_T("Tips:Client play query")),MB_YESNO) == IDYES)
                {
                                        file.Close();
                    if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
                    {
                        MessageBox(Translate(_T("Create file failed!")),Translate(_T("Error:Client play query")),MB_OK|MB_ICONERROR);
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

void CReportCltPlayDlg::UpdateType()
{
    m_ctlType.ResetContent();
    m_ctlType.InsertString(0, Translate(_T("Client ID")));
    m_ctlType.SetItemData(0, -1);        // if you need, you can modify SetItemData second param

    m_ctlType.InsertString(1, Translate(_T("Client Name")));
    m_ctlType.SetItemData(1, 0);

    m_ctlType.InsertString(2,Translate(_T("Group Name")));
    m_ctlType.SetItemData(2, 1);

    m_ctlType.InsertString(3, Translate(_T("Task Name")));
    m_ctlType.SetItemData(3, 2);

    m_ctlType.SetCurSel(0);
}

void CReportCltPlayDlg::OnDtnDatetimechangeDatetimepickEnd(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_REPORT3_START, IDC_DATETIMEPICKER_REPORT3_END, date, false);
    *pResult = 0;
}

void CReportCltPlayDlg::OnDtnDatetimechangeDatetimepickStart(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_REPORT3_START, IDC_DATETIMEPICKER_REPORT3_END, date, true);
    *pResult = 0;
}
