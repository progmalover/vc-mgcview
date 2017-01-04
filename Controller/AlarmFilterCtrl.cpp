// AlarmFilterCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "AlarmFilterCtrl.h"
#include "afxdialogex.h"
#include "AlarmSystemPage.h"
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



// CAlarmFilterCtrl dialog

IMPLEMENT_DYNAMIC(CAlarmFilterCtrl, CDialogEx)

CAlarmFilterCtrl::CAlarmFilterCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAlarmFilterCtrl::IDD, pParent)
{

}

CAlarmFilterCtrl::~CAlarmFilterCtrl()
{
}

void CAlarmFilterCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHECK_DATE, m_bUseDate);
    DDX_Check(pDX, IDC_CHECK_OPERATOR, m_bUseOperator);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START_LOG, m_timeStart);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END_LOG, m_timeEnd);
    DDX_Control(pDX, IDC_COMBO_OPERATOR, m_ctlOperators);
    DDX_Control(pDX, IDC_BTN_REFRESH_USER, m_ctrlRefresh);
}


BEGIN_MESSAGE_MAP(CAlarmFilterCtrl, CDialogEx)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CAlarmFilterCtrl::OnBnClickedButtonSearch)
    ON_BN_CLICKED(IDC_BTN_EXPORT_CSV, &CAlarmFilterCtrl::OnBnClickedButtonExportCsv)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_END_LOG, &CAlarmFilterCtrl::OnDtnDatetimechangeDatetimepickerEndLog)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_START_LOG, &CAlarmFilterCtrl::OnDtnDatetimechangeDatetimepickerStartLog)
	ON_BN_CLICKED(IDC_CHECK_DATE, &CAlarmFilterCtrl::OnBnClickedCheckDate)
	ON_BN_CLICKED(IDC_CHECK_OPERATOR, &CAlarmFilterCtrl::OnBnClickedCheckOperator)
    ON_BN_CLICKED(IDC_BTN_REFRESH_USER, &CAlarmFilterCtrl::OnBnClickedRefreshUser)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CAlarmFilterCtrl)
    EASYSIZE(IDC_DATETIMEPICKER_START_LOG,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_DATETIMEPICKER_END_LOG,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_COMBO_OPERATOR,ES_BORDER,ES_BORDER,ES_BORDER,IDC_BTN_REFRESH_USER,0)
    EASYSIZE(IDC_BTN_REFRESH_USER,IDC_COMBO_OPERATOR,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_SEARCH,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_BTN_EXPORT_CSV,IDC_BUTTON_SEARCH,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
END_EASYSIZE_MAP


// CAlarmFilterCtrl message handlers
BOOL CAlarmFilterCtrl::OnInitDialog()
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
	m_ctlOperators.ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_OPERATOR)->ShowWindow(FALSE);
	GetDlgItem(IDC_BTN_REFRESH_USER)->ShowWindow(FALSE);

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


void CAlarmFilterCtrl::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

void CAlarmFilterCtrl::OnBnClickedButtonSearch()
{
    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("Alarm List Updating!")).GetString());

    // retrieve data from dialog
    UpdateData();
    S3Time startTime(S3Time::NULLTime), endTime(S3Time::OneDayAfter());
    std::vector<int> userIds;
    std::vector<Alarm> AlarmList;

	ATL::COleDateTime t1, t2(2999, 12, 31, 23, 59, 59);
    if (m_bUseDate)
    {
        //ATL::COleDateTime t1, t2;
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
    //HRESULT hr = GetControllerInterface->ViewAlarmList(startTime, endTime, userIds, AlarmList);
    //CHECK_RETVAL(hr, return;);

    // Alarm search result
    CAlarmSystemPage* AlarmSystemPage = dynamic_cast<CAlarmSystemPage*>((GetParent()->GetParent()));
    CAlarmListPane& AlarmListPane = AlarmSystemPage->m_wndAlarmList;

    //AlarmListPane.SetAlarmList(AlarmList);
	SAlarmFilter filter;
	filter.startTime = t1;
	filter.endTime = t2;
	filter.userIds = userIds;

	AlarmListPane.m_grid.SetFilter(filter);

    GetParent()->SendMessage(MAIN_APP_STATE_BAR_INFO, NULL,
        (LPARAM)Translate(_T("Alarm List Updated!")).GetString());
}


void CAlarmFilterCtrl::OnBnClickedButtonExportCsv()
{
	CFileDialog mFileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, _T("CSV Files (*.csv)|*.csv|"), this);
	CString szFileSavePath;
	CString szFileName;

	if (mFileDlg.DoModal() == IDOK)
	{
		CAlarmSystemPage* alarmSystemPage = dynamic_cast<CAlarmSystemPage*>((GetParent()->GetParent()));
		CAlarmListPane& alarmListPane = alarmSystemPage->m_wndAlarmList;
		unsigned long len =alarmSystemPage->m_wndAlarmList.m_grid.GetVirtualRowCount();

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
        CAlarmSystemPage* AlarmSystemPage = dynamic_cast<CAlarmSystemPage*>((GetParent()->GetParent()));
        CAlarmListPane& AlarmListPane = AlarmSystemPage->m_wndAlarmList;

        if (AlarmListPane.m_wndComponentList.GetItemCount() > 0)
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

void CAlarmFilterCtrl::SaveCsv(CFile& file, CString szFileName, unsigned long len)
{
	file.Write("\xff\xfe", 2);
	CWaitCursor cursor;
	CAlarmSystemPage* alarmSystemPage = dynamic_cast<CAlarmSystemPage*>((GetParent()->GetParent()));
	CAlarmListPane& alarmListPane = alarmSystemPage->m_wndAlarmList;
	std::tstring sql, clause;
	sql = _T("select ClientId, AlarmLevel, Element,Value ,TimeStamp from alarm");
	SAlarmFilter filter;
	sort_and_filter_t sf = alarmSystemPage->m_wndAlarmList.m_grid.BuildSort();
	filter = boost::any_cast<SAlarmFilter>(sf.GetFilter());
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

	CString TableTitle;
	for (int j = 0; j < alarmListPane.m_grid.GetColumnCount(); j++)
	{
		int col = alarmListPane.m_grid.GetColumnsInfo().OrderToIndex(j);
		if (col != -1)
		{
			TableTitle += alarmListPane.m_grid.GetColumnName(col);
			TableTitle += _T(",");
		}	
	}
	TableTitle += _T("\r\n");
	//CW2A TableTitle2(TableTitle);
	file.Write(TableTitle, _tcslen(TableTitle) * sizeof(wchar_t));

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

		while (!rs->EndOfFile)
		{
			CString str;
			for (int j = 0; j < alarmListPane.m_grid.GetColumnCount(); j++)
			{
				int col = alarmListPane.m_grid.GetColumnsInfo().OrderToIndex(j);
				if (col != -1)
				{
					std::tstring fieldName = alarmListPane.m_grid.GetColumnName(col);
					str += (TCHAR*)(_bstr_t)rs->Fields->GetItem(fieldName.c_str())->GetValue();
					str += _T(",");
				}	
			}
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

    CAlarmSystemPage* AlarmSystemPage = dynamic_cast<CAlarmSystemPage*>((GetParent()->GetParent()));
    CAlarmListPane& AlarmListPane = AlarmSystemPage->m_wndAlarmList;

    int nItemCount = AlarmListPane.m_wndComponentList.GetItemCount();

    LVCOLUMN columnData;
    CString columnName;
    int columnNum = 0;

    columnData.mask = LVCF_TEXT;
    columnData.cchTextMax = 100;
    columnData.pszText = columnName.GetBuffer(100);
    CString TableTitle;

    for (int j = 0; AlarmListPane.m_wndComponentList.GetColumn(j, &columnData); j++)
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
        for (int j = 0; AlarmListPane.m_wndComponentList.GetColumn(j, &columnData); j++)
        {
            CString strItemText =  AlarmListPane.m_wndComponentList.GetItemText(i, j);
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

void CAlarmFilterCtrl::OnOK()
{
}


void CAlarmFilterCtrl::OnCancel()
{
}


void CAlarmFilterCtrl::OnDtnDatetimechangeDatetimepickerEndLog(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_START_LOG, IDC_DATETIMEPICKER_END_LOG, date, false);
    *pResult = 0;
}


void CAlarmFilterCtrl::OnDtnDatetimechangeDatetimepickerStartLog(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    COleDateTime date(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
    CheckDateTime(this, IDC_DATETIMEPICKER_START_LOG, IDC_DATETIMEPICKER_END_LOG, date, true);
    *pResult = 0;
}


void CAlarmFilterCtrl::OnBnClickedCheckDate()
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


void CAlarmFilterCtrl::OnBnClickedCheckOperator()
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

void CAlarmFilterCtrl::OnBnClickedRefreshUser()
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

