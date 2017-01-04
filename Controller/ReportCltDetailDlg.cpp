// CReportCltDetailDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ReportCltDetailDlg.h"
#include "Localization/Localization.h"
#include <odbcinst.h>
#include  <afxdb.h> 

#include  "LoadingTranslucentDlg.h"

// CReportCltDetailDlg dialog

IMPLEMENT_DYNAMIC(CReportCltDetailDlg, CDialogEx)

CReportCltDetailDlg::CReportCltDetailDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CReportCltDetailDlg::IDD, pParent)
{

}

CReportCltDetailDlg::~CReportCltDetailDlg()
{
}

void CReportCltDetailDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_REPORT5_KEYWORD, m_strDesc);
    DDX_Control(pDX, IDC_COMBO_REPORT5_TYPE, m_ctlType);
    DDX_Control(pDX, IDC_LIST_REPORT5, m_ReportCltDetailList);
}


BEGIN_MESSAGE_MAP(CReportCltDetailDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_REPORT5_SEARCH, &CReportCltDetailDlg::OnClickedButtonSearch)
    ON_BN_CLICKED(IDC_BTN_REPORT5_EXPORT_EXL, &CReportCltDetailDlg::OnClickedButtonExport)
    ON_BN_CLICKED(IDC_BTN_REPORT5_EXPORT_TXT, &CReportCltDetailDlg::OnClickedButtonExportTxt)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CReportCltDetailDlg)
    EASYSIZE(IDC_STATIC_REPORT5_TYPE, ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_COMBO_REPORT5_TYPE,IDC_STATIC_REPORT5_TYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_BTN_REPORT5_SEARCH,IDC_COMBO_REPORT5_TYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_BTN_REPORT5_EXPORT_EXL, IDC_BTN_REPORT5_SEARCH,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_BTN_REPORT5_EXPORT_TXT, IDC_BTN_REPORT5_EXPORT_EXL,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_REPORT5_KEYWORD,ES_BORDER,IDC_STATIC_REPORT5_TYPE,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_EDIT_REPORT5_KEYWORD,IDC_STATIC_REPORT5_KEYWORD,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)  
    EASYSIZE(IDC_LIST_REPORT5, ES_BORDER, IDC_STATIC_REPORT5_KEYWORD, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP


// CReportCltDetailDlg message handlers
BOOL CReportCltDetailDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    
    INIT_EASYSIZE;



    UpdateType();

	// sync data with GUI
	UpdateData(FALSE);

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CReportCltDetailDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}


void CReportCltDetailDlg::OnClickedButtonSearch()
{

    GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
        (LPARAM)Translate(_T("Searching......")).GetString());
    UpdateData(TRUE);

    using namespace ADODB;
    _RecordsetPtr pRs;

    CComboBox *pCBKey = &m_ctlType;
    int KeywordID = pCBKey->GetItemData(pCBKey->GetCurSel());
    CString Conditons = _T("");
    CString sql("");


    sql = _T("select DISTINCT client.clientid, clientname, hardwareinfo, location, phone, displaylayout, clientgroup.name from \
             client, clientgroup where client.groupid = clientgroup.clientgroupid");

    switch(KeywordID)
    {
    case -1:
        {
            if (m_strDesc != _T(""))
            {
                char   ch; 
                for(int i = 0; i < m_strDesc.GetLength();i++) 
                { 
                    ch  =  m_strDesc.GetAt(i);       
                    if((ch   <   '0'   ||   ch   >   '9')  && (ch != '-'))
                    {
                         MessageBox(Translate(_T("Please input integer!")),Translate(_T("Warning:Client availablity query")), MB_OK|MB_ICONEXCLAMATION);
                         GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
                        (LPARAM)Translate(_T("Input conditions are not valid!")).GetString());
                        return;
                    }
                } 
                int clientID = _tstoi((LPCTSTR)m_strDesc);
                CString tmp;
                tmp.Format(_T("%d"), clientID);
                Conditons += _T(" and client.clientid like '%") + tmp + _T("%' ");
            }    
        }
        break;
    case 0:
        {
            m_strDesc.MakeLower();
            Conditons += _T(" and LOWER(clientname) like '%") + m_strDesc + _T("%' ");
        }
        break;
    case 1:
        {
            m_strDesc.MakeLower();
            Conditons += _T(" and LOWER(clientgroup.name) like '%") + m_strDesc + _T("%' ");
        }
        break;
    default:
        break;
    }
    sql += Conditons;

    CWaitCursor wait;
    HRESULT hr  = GetControllerInterface->QuerySQL(Condition(sql.GetString()), pRs);

    if (!SUCCEEDED(hr))
    {
        TRACE0("Failed to query sql by ClientPlaySum stored proc\n");
        GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
            (LPARAM)Translate(_T("Search Failed by QuerySQL")).GetString());
        return;
    }

    m_ReportCltDetailList.DeleteAllItems();
    int i = 0;
    CString header = _T("CPU:  ");
    if (pRs && pRs->RecordCount > 0)
    {
        try
        {
            pRs->MoveFirst();
            CString szClientID;
            while(!pRs->EndOfFile)
            {
                int clientId = pRs->Fields->GetItem(_T("ClientId"))->GetValue();
                std::tstring clientName = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("ClientName"))->GetValue();
                std::tstring diskInfo = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("hardwareInfo"))->GetValue();
                std::tstring location = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("location"))->GetValue();
                std::tstring phone = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("phone"))->GetValue();
                std::tstring displaylayout = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("displaylayout"))->GetValue();
                std::tstring groupName = (TCHAR*)(_bstr_t)pRs->Fields->GetItem(_T("name"))->GetValue(); //groupname

                CString info = diskInfo.c_str();
                CString rs = info.Mid(header.GetLength(),info.GetLength()-header.GetLength());
                rs.TrimLeft(_T(" "));
                rs.TrimRight(_T(" "));
                CString hardwareInfo = header + rs;
                //insert record
                szClientID.Format(_T("%d"), clientId);
                m_ReportCltDetailList.InsertItem(i, (LPCTSTR)szClientID);
                m_ReportCltDetailList.SetItemText(i, 1, clientName.c_str());
                m_ReportCltDetailList.SetItemText(i, 2, hardwareInfo);
                m_ReportCltDetailList.SetItemText(i, 3, location.c_str());
                m_ReportCltDetailList.SetItemText(i, 4, phone.c_str());
               // m_ReportCltDetailList.SetItemText(i, 5, displaylayout.c_str());
                m_ReportCltDetailList.SetItemText(i, 5, groupName.c_str());
                pRs->MoveNext();

                ++i;
            }
        }
        catch (_com_error& e)
        {
            MessageBox(Translate(_T("Database error!")), Translate(_T("Error:Client availablity query")), MB_OK);
        }
    }
    else
    {
        MessageBox(Translate(_T("No records match the query!")),Translate(_T("Tips:Client availablity query")), MB_OK);
    }
    pRs = NULL;

    CString InfoStatus("");
    InfoStatus.Format(Translate(_T("Report System List Updated! the result records: %d")), i);

    GetParent()->GetParent()->GetParent()->GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL, 
        (LPARAM)InfoStatus.GetBuffer());
}


void CReportCltDetailDlg::SaveCsv(CFile& file, CString szFileName)
{
    // write BOM of UTF-16 LE file
    file.Write("\xff\xfe", 2);

    int nItemCount = m_ReportCltDetailList.GetItemCount();

    LVCOLUMN columnData;
    CString columnName;
    int columnNum = 0;

    columnData.mask = LVCF_TEXT;
    columnData.cchTextMax = 100;
    columnData.pszText = columnName.GetBuffer(100);
    CString TableTitle;

    for (int j = 0; m_ReportCltDetailList.GetColumn(j, &columnData); j++)
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
        for (int j = 0; m_ReportCltDetailList.GetColumn(j, &columnData); j++)
        {
            CString strItemText =  m_ReportCltDetailList.GetItemText(i, j);
            strItemText.Replace('\n', ' ');
            str += strItemText + _T(",");
        }
        str += _T("\r\n");
        //CW2A str2(str);
        file.Write(str, _tcslen(str) * sizeof(wchar_t));
    }
    file.Close();
    CString warningStr;
    warningStr.Format(Translate(_T("Save file to %s!")),szFileName);
    MessageBox(warningStr, Translate(_T("Tips:Client availablity query")), MB_OK);
}

void CReportCltDetailDlg::OnClickedButtonExport()
{  
    CFileDialog mFileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY , _T("CSV Files (*.csv)|*.csv|"), this);
    CString szFileSavePath;
    CString szFileName;

    if (mFileDlg.DoModal() == IDOK)
    {
       
        if (m_ReportCltDetailList.GetItemCount() > 0)
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
                if (MessageBox(strWarning, Translate(_T("Tips:Client detail query")), MB_YESNO) == IDYES)
                {
                    SaveCsv(file, szFileName);
                }
            }
            else
            {
                if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
                {
                    MessageBox(Translate(_T("Create file failed!")),Translate(_T("Error:Client detail query")), MB_OK);
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
            MessageBox(Translate(_T("No data to save!")), Translate(_T("Tips:Client detail query")), MB_OK);
            return;
        }            
    }
    
}

void CReportCltDetailDlg::SaveTxt(CFile& file, CString szFileName)
{
    int nItemCount = m_ReportCltDetailList.GetItemCount();

    LVCOLUMN columnData;
    CString columnName;
    int columnNum = 0;


    columnData.mask = LVCF_TEXT;
    columnData.cchTextMax =100;
    columnData.pszText = columnName.GetBuffer (100);
    CString Tabletitle;
    for (int j = 0; m_ReportCltDetailList.GetColumn(j, &columnData); j++)
    {
        Tabletitle += columnData.pszText;
        Tabletitle += _T("\t");
    }
    Tabletitle += _T("\n");
    CW2A TableTitle2(Tabletitle);
    file.Write(TableTitle2, strlen(TableTitle2));

    for (int i = 0; i < nItemCount; i++)
    {
        CString str;
        for (int j = 0; m_ReportCltDetailList.GetColumn(j, &columnData); j++)
        {
            CString strItemText = m_ReportCltDetailList.GetItemText(i, j);
            strItemText.Remove('\n');
            str += strItemText + _T(",");
        }
        str.Delete(str.GetLength()-1, str.GetLength());
        str += _T("\n");
        CW2A str2(str);
        file.Write(str2, strlen(str2));
    }
    file.Close(); 
    CString warningStr;
    warningStr.Format(Translate(_T("Save file to %s!")),szFileName);
    MessageBox(warningStr, Translate(_T("Tips:Client detail query")), MB_OK); // MESSAGEBOX:
}

void CReportCltDetailDlg::OnClickedButtonExportTxt()
{
    CString szFileSavePath;
    CString szFileName;

    CFileDialog exportTxtDlg(FALSE,
        NULL, NULL,
        OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT,
        _T("TXT Files (*.txt)|*.txt||"), this);

    if (exportTxtDlg.DoModal() == IDOK)
    {
        if (m_ReportCltDetailList.GetItemCount() > 0)
        {
            szFileSavePath = exportTxtDlg.GetPathName();
            CString ext = szFileSavePath.Right(4);

            if (ext.CompareNoCase(_T(".txt")) != 0)
            {
                szFileName = szFileSavePath + _T(".txt");
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
                if (MessageBox(strWarning, Translate(_T("Tips:Client Detail Query")),MB_YESNO) == IDYES)
                {
                    file.Close();
                    if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
                    {
                        MessageBox(Translate(_T("Create file failed!")), Translate(_T("Error:Client detail query")), MB_OK);
                        return;
                    }

                    SaveTxt(file, szFileName);
                }

            }
            else
            {
                if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
                {
                    MessageBox(Translate(_T("Create file failed!")), Translate(_T("Error:Client detail query")), MB_OK);
                    return;
                }
                else
                {
                    SaveTxt(file, szFileName);
                }
            }
        }
        else
        {
            MessageBox(Translate(_T("no data for save!")),Translate(_T("Tips:Client detail query")), MB_OK);
            return;
        }

    }
}

void CReportCltDetailDlg::UpdateType()
{
    m_ctlType.ResetContent();
    m_ctlType.InsertString(0,Translate(_T("Client ID")));
    m_ctlType.SetItemData(0, -1);        // if you need, you can modify SetItemData second param

    m_ctlType.InsertString(1, Translate(_T("Client Name")));
    m_ctlType.SetItemData(1, 0);

    m_ctlType.InsertString(2, Translate(_T("Group Name")));
    m_ctlType.SetItemData(2, 1);

    m_ctlType.SetCurSel(0);
}


