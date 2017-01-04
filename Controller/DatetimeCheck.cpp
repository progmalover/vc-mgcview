#include "stdafx.h"
void CheckDateTime(CWnd* pWnd, int startId, int endId, COleDateTime newDate, bool isStart)
{
    if (isStart)
    {
        if (LOBYTE(LOWORD(GetVersion())) < 6) // Win XP and below only
        {
            static bool isSecondTime = false;
            // We are changing the date in the Month Control, we only handle the second notification
            if (NULL != DateTime_GetMonthCal(pWnd->GetDlgItem(startId)->GetSafeHwnd()))
            {
                if (!isSecondTime)
                {
                    isSecondTime = !isSecondTime;
                    return;
                }
                else
                {
                    isSecondTime = !isSecondTime;
                }
            }
            else
            {
                isSecondTime = false;
            }
        }

        COleDateTime end;
        ((CDateTimeCtrl*)pWnd->GetDlgItem(endId))->GetTime(end);

        end.SetDateTime(end.GetYear(), end.GetMonth(), end.GetDay(),23, 59, 59);
        if (end < newDate)
        {
			ShowXMessageBox(pWnd->GetSafeHwnd(), Translate(_T("Can't set start date after end date")), Translate(_T("Error: Check datetime")), MB_OK|MB_ICONERROR);
            //pWnd->MessageBox(Translate(_T("Can't set start date after end date")), Translate(_T("Error: Check datetime")), MB_OK|MB_ICONERROR);
            newDate.SetDateTime(end.GetYear(), end.GetMonth(), end.GetDay(), 0, 0, 0);
            ((CDateTimeCtrl*)pWnd->GetDlgItem(startId))->SetTime(newDate);
        }
        else
        {
            ((CDateTimeCtrl*)pWnd->GetDlgItem(startId))->SetTime(newDate);
        }
    }
    else
    {
        if (LOBYTE(LOWORD(GetVersion())) < 6) // Win XP and below only
        {
            static bool isSecondTime = false;
            // We are changing the date in the Month Control, we only handle the second notification
            if (NULL != DateTime_GetMonthCal(pWnd->GetDlgItem(endId)->GetSafeHwnd()))
            {
                if (!isSecondTime)
                {
                    isSecondTime = !isSecondTime;
                    return;
                }
                else
                {
                    isSecondTime = !isSecondTime;
                }
            }
            else
            {
                isSecondTime = false;
            }
        }

//         LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
//         COleDateTime end(pDTChange->st.wYear, pDTChange->st.wMonth, pDTChange->st.wDay, 23, 59, 59);
        COleDateTime start;
        ((CDateTimeCtrl*)pWnd->GetDlgItem(startId))->GetTime(start);

        start.SetDateTime(start.GetYear(), start.GetMonth(), start.GetDay(), 0, 0, 0);

        if (newDate < start)
        {
			ShowXMessageBox(pWnd->GetSafeHwnd(),Translate(_T("Can't set end date before start date")), Translate(_T("Error:Check datetime")), MB_OK|MB_ICONERROR);
            //pWnd->MessageBox(Translate(_T("Can't set end date before start date")), Translate(_T("Error:Check datetime")), MB_OK|MB_ICONERROR);
            newDate.SetDateTime(start.GetYear(), start.GetMonth(), start.GetDay(), 23, 59, 59);
            ((CDateTimeCtrl*)pWnd->GetDlgItem(endId))->SetTime(newDate);
        }
        else
        {
            ((CDateTimeCtrl*)pWnd->GetDlgItem(endId))->SetTime(newDate);
        }
    }
}
