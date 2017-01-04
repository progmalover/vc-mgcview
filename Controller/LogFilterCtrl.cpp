// LogFilterCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "LogFilterCtrl.h"
#include "afxdialogex.h"
#include "LogSystemPage.h"
#include "Localization/Localization.h"
#include "DatetimeCheck.h"
#include <odbcinst.h>
#include  <afxdb.h>

#include <algorithm>
#include <functional>

#define CHECK_RETVAL(hr, additional) \
    if(hr != S_OK) \
    {\
        std::tstring ErrorMessage;\
        GetControllerInterface->GetLastError(ErrorMessage);\
        MessageBox(Translate(ErrorMessage.c_str()), Translate(_T("Error")), MB_OK);\
        additional\
    }\



// CLogFilterCtrl dialog

IMPLEMENT_DYNAMIC(CLogFilterCtrl, CDialogEx)

CLogFilterCtrl::CLogFilterCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLogFilterCtrl::IDD, pParent)
{

}

CLogFilterCtrl::~CLogFilterCtrl()
{
}

void CLogFilterCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHECK_DATE, m_bUseDate);
    DDX_Check(pDX, IDC_CHECK_OPERATOR, m_bUseOperator);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START_LOG, m_timeStart);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END_LOG, m_timeEnd);
    DDX_Control(pDX, IDC_COMBO_OPERATOR, m_ctlOperators);
    DDX_Control(pDX, IDC_BTN_REFRESH_USER, m_ctrlRefresh);
}


BEGIN_MESSAGE_MAP(CLogFilterCtrl, CDialogEx)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CLogFilterCtrl::OnBnClickedButtonSearch)
    ON_BN_CLICKED(IDC_BTN_EXPORT_CSV, &CLogFilterCtrl::OnBnClickedButtonExportCsv)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_END_LOG, &CLogFilterCtrl::OnDtnDatetimechangeDatetimepickerEndLog)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_START_LOG, &CLogFilterCtrl::OnDtnDatetimechangeDatetimepickerStartLog)
	ON_BN_CLICKED(IDC_CHECK_DATE, &CLogFilterCtrl::OnBnClickedCheckDate)
	ON_BN_CLICKED(IDC_CHECK_OPERATOR, &CLogFilterCtrl::OnBnClickedCheckOperator)
    ON_BN_CLICKED(IDC_BTN_REFRESH_USER, &CLogFilterCtrl::OnBnClickedRefreshUser)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CLogFilterCtrl)
    EASYSIZE(IDC_DATETIMEPICKER_START_LOG,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_DATETIMEPICKER_END_LOG,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_COMBO_OPERATOR,ES_BORDER,ES_BORDER,ES_BORDER,IDC_BTN_REFRESH_USER,0)
    EASYSIZE(IDC_BTN_REFRESH_USER,IDC_COMBO_OPERATOR,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_SEARCH,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_BTN_EXPORT_CSV,IDC_BUTTON_SEARCH,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
END_EASYSIZE_MAP


// CLogFilterCtrl message handlers
BOOL CLogFilterCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    m_ctrlRefresh.SetImage(IDB_REFRESH);

    INIT_EASYSIZE;

	if (!IsDlgButtonChecked(IDC_CHECK_DATE))
	{
		GetDlgItem(IDC_DATETIMEPICKER_START_LOG)->EnableWindow(FALSE);
		GetDlgItem(IDC_DATETIMEPICKER_END_LOG)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_DATETIMEPICKER_START_LOG)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATETIMEPICKER_END_LOG)->EnableWindow(TRUE);
	}

	if (!IsDlgButtonChecked(IDC_CHECK_OPERATOR))
	{
		GetDlgItem(IDC_COMBO_OPERATOR)->EnableWindow(FALSE);
        GetDlgItem(IDC_BTN_REFRESH_USER)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_OPERATOR)->EnableWindow(TRUE);
        GetDlgItem(IDC_BTN_REFRESH_USER)->EnableWindow(TRUE);
	}

    if (!GetControllerInterface->CheckPrivilege(UserPrivilege_ListLog))
    {
        GetDlgItem(IDC_CHECK_DATE)->EnableWindow(FALSE);
        GetDlgItem(IDC_DATETIMEPICKER_START_LOG)->EnableWindow(FALSE);
        GetDlgItem(IDC_DATETIMEPICKER_END_LOG)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHECK_OPERATOR)->EnableWindow(FALSE);
        GetDlgItem(IDC_COMBO_OPERATOR)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);
        return TRUE;
    }

    // init time filter controls
    m_timeStart = COleDateTime::GetCurrentTime();
    m_timeEnd = COleDateTime::GetCurrentTime();

    // init combobox of operator style, so we can multi-select operators

    // get operator list from control server
    HRESULT hr = GetControllerInterface->GetAllUsers(m_vUserList);

    CHECK_RETVAL(hr, return TRUE;);

    // put operator list into
    auto it = m_vUserList.begin(), ite = m_vUserList.end();
    int i = 0;
    for (; it != ite; ++it, i++)
    {
		if(it->GetUserID() >= 0x00)
		{
			m_ctlOperators.AddString(it->GetAccount().GetUserName().c_str());
		}
    }
    UntranslateWnd(m_ctlOperators.GetSafeHwnd());

    // sync data with GUI
    UpdateData(FALSE);

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CLogFilterCtrl::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

void CLogFilterCtrl::OnBnClickedButtonSearch()
{
    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("Log List Updating!")).GetString());

    // retrieve data from dialog
    UpdateData();
    S3Time startTime(S3Time::NULLTime), endTime(S3Time::OneDayAfter());
    std::vector<int> userIds;
    std::vector<Log> logList;

    ATL::COleDateTime t1, t2(2999, 12, 31, 23, 59, 59);
    if (m_bUseDate)
    {
        t1.SetDateTime(m_timeStart.GetYear(), m_timeStart.GetMonth(), m_timeStart.GetDay(), 0, 0, 0);
        t2.SetDateTime(m_timeEnd.GetYear(), m_timeEnd.GetMonth(), m_timeEnd.GetDay(), 23, 59, 59);

        startTime = S3Time::CreateTime(t1);
        endTime = S3Time::CreateTime(t2);
    }

    CString userName;
    // get operator from control
    if (m_bUseOperator)
    {
        int iPos;
        if ((iPos = m_ctlOperators.GetCurSel()) != CB_ERR)
        {
            m_ctlOperators.GetLBText(iPos, userName);

            //
            auto it = m_vUserList.begin(), ite = m_vUserList.end();
            for (; it != ite; ++it)
            {
                if (it->GetAccount().GetUserName() == userName.GetString())
                {
                    userIds.push_back(it->GetUserID());
                }
            }
        }
    }


    // do search now
//     HRESULT hr = GetControllerInterface->ViewLogList(startTime, endTime, userIds, logList);
//     CHECK_RETVAL(hr, return;);

    // Log search result
    CLogSystemPage* logSystemPage = dynamic_cast<CLogSystemPage*>((GetParent()->GetParent()));
    CLogListPane& logListPane = logSystemPage->m_wndLogList;

    //logListPane.SetLogList(logList);
    SLogFilter filter;
    filter.startTime = t1;
    filter.endTime = t2;
    filter.userIds = userIds;

    logListPane.SetFilter(filter);


    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("Log List Updated!")).GetString());
}


void CLogFilterCtrl::OnBnClickedButtonExportCsv()
{
	CFileDialog mFileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, _T("CSV Files (*.csv)|*.csv|"), this);
	CString szFileSavePath;
	CString szFileName;

	if (mFileDlg.DoModal() == IDOK)
	{
		CLogSystemPage* logSystemPage = dynamic_cast<CLogSystemPage*>((GetParent()->GetParent()));
		CLogListPane& logListPane = logSystemPage->m_wndLogList;
		unsigned long len =logSystemPage->m_wndLogList.m_grid.GetVirtualRowCount();

		if (len > 0)
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
				if (MessageBox(strWarning,Translate(_T("Tips:Export csv")) ,MB_YESNO|MB_ICONQUESTION) == IDYES)
				{
					file.Close();
					if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
					{
						MessageBox(Translate(_T("Create file failed!")),Translate(_T("Tips:Export csv")), MB_OK|MB_ICONERROR);
						return;
					}
					SaveCsv(file, szFileName, len);
				}
			}
			else
			{
				if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
				{
					MessageBox(Translate(_T("Create file failed!")), Translate(_T("Tips:Export csv")), MB_OK|MB_ICONERROR);
					return;
				}
				else
				{
					SaveCsv(file, szFileName, len);
				}
			}
		}
		else
		{
			MessageBox(Translate(_T("No data to save!")),Translate(_T("Tips:Export csv")), MB_OK|MB_ICONINFORMATION);
			return;
		}
		
	}

		
	
#if 0
    CFileDialog mFileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, _T("CSV Files (*.csv)|*.csv|"), this);
    CString szFileSavePath;
    CString szFileName;

    if (mFileDlg.DoModal() == IDOK)
    {
        CLogSystemPage* logSystemPage = dynamic_cast<CLogSystemPage*>((GetParent()->GetParent()));
        CLogListPane& logListPane = logSystemPage->m_wndLogList;

        if (logListPane.m_wndComponentList.GetItemCount() > 0)
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
                if (MessageBox(strWarning,Translate(_T("Tips:Export csv")) ,MB_YESNO|MB_ICONQUESTION) == IDYES)
                {
                    file.Close();
                    if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
                    {
                        MessageBox(Translate(_T("Create file failed!")),Translate(_T("Tips:Export csv")), MB_OK|MB_ICONERROR);
                        return;
                    }
                    SaveCsv(file, szFileName);
                }
            }
            else
            {
                if (!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
                {
            	    MessageBox(Translate(_T("Create file failed!")), Translate(_T("Tips:Export csv")), MB_OK|MB_ICONERROR);
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
            MessageBox(Translate(_T("No data to save!")),Translate(_T("Tips:Export csv")), MB_OK|MB_ICONINFORMATION);
            return;
        }
    }
#endif
}

void CLogFilterCtrl::SaveCsv(CFile& file, CString szFileName, unsigned long len)
{
	file.Write("\xff\xfe", 2);
	CWaitCursor cursor;
	CLogSystemPage* logSystemPage = dynamic_cast<CLogSystemPage*>((GetParent()->GetParent()));
	CLogListPane& logListPane = logSystemPage->m_wndLogList;
	std::tstring sql, clause;
	sql = _T("select username, operateip, timestamp, operation from log");
	SLogFilter filter;
	sort_and_filter_t sf = logSystemPage->m_wndLogList.m_grid.BuildSort();
	filter = boost::any_cast<SLogFilter>(sf.GetFilter());
	sql += filter.ToString();

	clause = _T(" order by ");

	// sort clause
	sort_and_filter_t::sorts_t sorts;
	sorts = sf.GetSorts();
	int n = sorts.size();
	int i = 0;
	bool bFirst = true;
	bool bSort = false;
	for (i = 0; i < n; ++i)
	{
		int s = std::get<1>(sorts[i]);
		if (!bFirst && s != 0)
		{
			clause += _T(", ");
		}


		if (s == 1)
		{
			clause += std::get<0>(sorts[i]);
			clause += _T(" ASC");
			bSort = true;
			bFirst = false;
		}
		else if (s == -1)
		{
			clause += std::get<0>(sorts[i]);
			clause += _T(" DESC");
			bSort = true;
			bFirst = false;
		}
		else
		{
			continue;
		}

	}
	if (bSort > 0)
	{
		sql += clause;
	}

	for (int i = 0; i<len; i+=200)
	{
		std::tstring sqlEnd;
		CString limit;
		unsigned long endTmp = i+199;
		unsigned long end = min(endTmp, len);

#ifdef SERVER_EDITION
		limit.Format(_T(" limit %d, %d"), i, end);
#else
		limit.Format(_T(" between %d and %d"), i, end);
#endif

		sqlEnd = sql + limit.GetString();

		ADODB::_RecordsetPtr rs;
		if (S_OK != GetControllerInterface->QuerySQL(sql, rs))
			return;


		ADODB::FieldsPtr fds = rs->GetFields();
		int nFileds = fds->GetCount();
		VARIANT varIndex;
		VariantInit(&varIndex);
		varIndex.vt = VT_I2;

		CString TableTitle;
		for (int j = 0; j < nFileds; j++)
		{
			varIndex.iVal = j;
			_bstr_t fieldname = fds->Item[varIndex]->GetName();
			
			TableTitle += (LPCSTR)fieldname;
			TableTitle += _T(",");
		}
		TableTitle += _T("\r\n");
		//CW2A TableTitle2(TableTitle);
		file.Write(TableTitle, _tcslen(TableTitle) * sizeof(wchar_t));

		while (!rs->EndOfFile)
		{
			CString str;
			for(int ii = 0;ii <nFileds ; ii++)
			{
				varIndex.iVal = ii;
				_bstr_t fieldname = fds->Item[varIndex]->GetName();
				std::string fieldName = (char*)fieldname;
				str += (TCHAR*)(_bstr_t)rs->Fields->GetItem(fieldName.c_str())->GetValue();
				str += _T(",");

			}
			/*
			for (int j = 0; j < logListPane.m_grid.GetColumnCount(); j++)
			{
				int col = logListPane.m_grid.GetColumnsInfo().OrderToIndex(j);
				if (col != -1)
				{
					std::tstring fieldName = logListPane.m_grid.GetColumnName(col);
					str += (TCHAR*)(_bstr_t)rs->Fields->GetItem(fieldName.c_str())->GetValue();
					str += _T(",");
				}	
			}
			*/
			str += _T("\r\n");
		
			file.Write(str, _tcslen(str) * sizeof(wchar_t));
			rs->MoveNext();
		}

	}
	file.Close();
	CString warningStr;
	warningStr.Format(Translate(_T("Save file to %s!")), szFileName);
	MessageBox(warningStr,Translate(_T("Tips:Export csv")), MB_OK|MB_ICONINFORMATION);

#if 0
    // write BOM of UTF-16 LE file
    file.Write("\xff\xfe", 2);

    CLogSystemPage* logSystemPage = dynamic_cast<CLogSystemPage*>((GetParent()->GetParent()));
    CLogListPane& logListPane = logSystemPage->m_wndLogList;

    int nItemCount = logListPane.m_wndComponentList.GetItemCount();

    LVCOLUMN columnData;
    CString columnName;
    int columnNum = 0;

    columnData.mask = LVCF_TEXT;
    columnData.cchTextMax = 100;
    columnData.pszText = columnName.GetBuffer(100);
    CString TableTitle;

    for (int j = 0; logListPane.m_wndComponentList.GetColumn(j, &columnData); j++)
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
        for (int j = 0; logListPane.m_wndComponentList.GetColumn(j, &columnData); j++)
        {
            CString strItemText =  logListPane.m_wndComponentList.GetItemText(i, j);
            str += strItemText + _T(",");
        }
        str += _T("\r\n");
        //CW2A str2(str);
        file.Write(str, _tcslen(str) * sizeof(wchar_t));

    }
    file.Close();
    CString warningStr;
    warningStr.Format(Translate(_T("Save file to %s!")), szFileName);
    MessageBox(warningStr,Translate(_T("Tips:Export csv")), MB_OK|MB_ICONINFORMATION);
#endif
}

void CLogFilterCtrl::OnOK()
{
}


void CLogFilterCtrl::OnCancel()
{
}


void CLogFilterCtrl::OnDtnDatetimechangeDatetimepickerEndLog(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_START_LOG, IDC_DATETIMEPICKER_END_LOG, date, false);
    *pResult = 0;
}


void CLogFilterCtrl::OnDtnDatetimechangeDatetimepickerStartLog(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_START_LOG, IDC_DATETIMEPICKER_END_LOG, date, true);
    *pResult = 0;
}


void CLogFilterCtrl::OnBnClickedCheckDate()
{
	// TODO: Add your control notification handler code here
	if (IsDlgButtonChecked(IDC_CHECK_DATE))
	{
		GetDlgItem(IDC_DATETIMEPICKER_START_LOG)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATETIMEPICKER_END_LOG)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_DATETIMEPICKER_START_LOG)->EnableWindow(FALSE);
		GetDlgItem(IDC_DATETIMEPICKER_END_LOG)->EnableWindow(FALSE);
	}
}


void CLogFilterCtrl::OnBnClickedCheckOperator()
{
	// TODO: Add your control notification handler code here
	if (IsDlgButtonChecked(IDC_CHECK_OPERATOR))
	{
		GetDlgItem(IDC_COMBO_OPERATOR)->EnableWindow(TRUE);
        GetDlgItem(IDC_BTN_REFRESH_USER)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_OPERATOR)->EnableWindow(FALSE);
        GetDlgItem(IDC_BTN_REFRESH_USER)->EnableWindow(FALSE);
	}
}

void CLogFilterCtrl::OnBnClickedRefreshUser()
{
    CWaitCursor cursor;
	int iPos;
	CString userName = _T("");
	if ((iPos = m_ctlOperators.GetCurSel()) != CB_ERR)
	{
		m_ctlOperators.GetLBText(iPos, userName);
	}

    m_ctlOperators.ResetContent();
    // get operator list from control server
    m_vUserList.clear();
    HRESULT hr = GetControllerInterface->GetAllUsers(m_vUserList);

    CHECK_RETVAL(hr, return;);

    // put operator list into
    auto it = m_vUserList.begin(), ite = m_vUserList.end();
    int i = 0;
    for (; it != ite; ++it, i++)
    {
        if(it->GetUserID() >= 0x00)
        {
            m_ctlOperators.AddString(it->GetAccount().GetUserName().c_str());
        }
    }

	if(userName != _T(""))
	{
		int pos = m_ctlOperators.FindString(0,userName);
		if(pos)
		{
			m_ctlOperators.SetCurSel(pos);
		}
		else
		{
			m_ctlOperators.SetCurSel(0);
		}
	}
}

