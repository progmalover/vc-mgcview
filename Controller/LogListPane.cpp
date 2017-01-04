// F:\p4\sw\Demo\ContentManagement\Controller\Log\LogListPane.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "LogListPane.h"
#include "Localization/Localization.h"


// CLogListPane

IMPLEMENT_DYNAMIC(CLogListPane, CBaseControllerPane)

CLogListPane::CLogListPane()
{

}

CLogListPane::~CLogListPane()
{
}


BEGIN_MESSAGE_MAP(CLogListPane, CBaseControllerPane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CLogListPane message handlers
int CLogListPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CBaseControllerPane::OnCreate(lpCreateStruct) == -1)
        return -1;

    RECT rect;
    GetClientRect(&rect);

    m_grid.SetBackgroundTask(g_backgroundTask);
    m_grid.Create(WS_CHILD|WS_VISIBLE|LVS_REPORT, rect, this, IDW_LIST_LOG);
#define Translate(x) x
    wchar_t* colnames[] = {
        //_T("User ID"),
        Translate(_T("User Name")),
        Translate(_T("Login IP")),
        Translate(_T("Operation Time")),
        Translate(_T("Operation")),
    };
#undef Translate
    static const int kColumnCount = sizeof(colnames) / sizeof(*colnames);
    for (int i = 0; i < kColumnCount; ++i)
    {
        //m_wndComponentList.InsertColumn(i, Translate(colnames[i]), LVCFMT_LEFT, 10 * wcslen(colnames[i]));
        m_grid.InsertColumn(i, Translate(colnames[i]), 10 * wcslen(colnames[i]));
        //m_grid.SetColumnName(i, names[i]);
    }
    m_grid.SetVirtualRows(m_grid.GetVirtualRowCount());
    m_grid.EnableDragSelection(TRUE);
    m_grid.EnableDragHeaderItems(TRUE);
    m_grid.SetWholeRowSel(TRUE);
    SetMainWnd(&m_grid);
    return 0;
}


void CLogListPane::OnDestroy()
{
    CBaseControllerPane::OnDestroy();

}

void CLogListPane::ClearList()
{
    //m_wndComponentList.DeleteAllItems();
}


void CLogListPane::SetFilter(const SLogFilter& filter)
{
    CWaitCursor cursor;
    m_filter = filter;

    m_grid.UpdateAllData();
}

SLogFilter CLogListPane::GetFilter()
{
    return m_filter;
}

const wchar_t* pLogColFields[] = {
	_T("UserName"),
	_T("OperateIP"),
	_T("TimeStamp"),
	_T("Operation"),
};
//////////////////////////////////////////////////////////////////////////
sort_and_filter_t CLogGrid::BuildSort()
{
    sort_and_filter_t sf;
    int n = m_Columns.GetColumnCount();
    for (int i = 0; i < n; ++i)
    {
        int s = m_Columns.GetColumnState(i);
        if (s == 1)
        {
            sf.AddSort(/*m_Columns.GetColumnName(i)*/ pLogColFields[i], 1);
        }
        else if (s == -1)
        {
            sf.AddSort(/*m_Columns.GetColumnName(i)*/pLogColFields[i], -1);
        }
        else
        {
            sf.AddSort(/*m_Columns.GetColumnName(i)*/pLogColFields[i], 0);
        }
    }

    sf.SetFilter(((CLogListPane*)GetParent())->GetFilter());

    return sf;
}

int CLogGrid::GetVirtualRowCount()
{
    std::tstring sql = _T("select count(*) as cnt from Log");
    sql += ((CLogListPane*)GetParent())->GetFilter().ToString();

    //sql += m_sortClause;

    ADODB::_RecordsetPtr rs;
    GetControllerInterface->QuerySQL(sql, rs);

    rs->MoveFirst();
    return rs->Fields->Item["cnt"]->Value;
}

void CLogGrid::GetData(int start, int end, const sort_and_filter_t& sf, std::map<int, std::vector<_variant_t> >& data)
{
    std::map<int, std::vector<_variant_t> > temp;
    CString limit;
    std::tstring sql, clause;

    sql = _T("select username, operateip, timestamp, operation from Log");
    SLogFilter filter;
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

#if defined(DB_MYSQL) || defined(DB_SQLITE)
    limit.Format(_T(" limit %d, %d"), start, end - start);
#elif defined(DB_ACCESS)
    limit.Format(_T(" between %d and %d"), start, end - 1);
#endif
    sql += limit.GetString();

    ADODB::_RecordsetPtr rs;
    if (S_OK != GetControllerInterface->QuerySQL(sql, rs))
        return;

    n = start;
    while (!rs->EndOfFile)
    {
        std::vector<_variant_t> row;
        for (short i = 0; i < rs->Fields->Count; ++i)
        {
            row.push_back(rs->Fields->Item[i]->Value);
        }
        temp[n] = row;
        ++n;
        rs->MoveNext();
    }

    data.swap(temp);
}

CLogGrid::~CLogGrid()
{

}
