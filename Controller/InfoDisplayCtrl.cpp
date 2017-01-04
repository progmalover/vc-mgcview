// InfoDisplayCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "InfoDisplayCtrl.h"


// CInfoDisplayCtrl

IMPLEMENT_DYNAMIC(CInfoDisplayCtrl, CStatic)

CInfoDisplayCtrl::CInfoDisplayCtrl()
{

}

CInfoDisplayCtrl::~CInfoDisplayCtrl()
{
}


BEGIN_MESSAGE_MAP(CInfoDisplayCtrl, CStatic)
    ON_WM_PAINT()
END_MESSAGE_MAP()



// CInfoDisplayCtrl message handlers


void CInfoDisplayCtrl::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: Add your message handler code here
    // Do not call CStatic::OnPaint() for painting messages

    dc.SelectObject(GetFont());
    int OldMode = dc.GetBkMode();
    dc.SetBkMode(TRANSPARENT);

    CRect rect;
    GetClientRect(&rect);
	dc.FillRect(rect,&afxGlobalData.brBtnFace);
    int ColumeWidth = 200;
    int LinesCount = 0;
    int LineHeight = dc.GetTextExtent(_T("Ag")).cy + 1;
    int LinesPerColume = rect.Height()/LineHeight;

    for(int i=0; i< m_Infos.GetSize(); i++)
    {
        CSize TextSize = dc.GetTextExtent(m_Infos[i].InfoName + m_Infos[i].InfoValue);

        LinesCount += (TextSize.cx + ColumeWidth - 1) / ColumeWidth;
    }

    int LineIndex = 0;
    int ColumeID = 0;
    int ColumeStartX = 5;
    int CurrentColumeWidth = 0;
    for(int i=0; i< m_Infos.GetSize(); i++)
    {
        CSize TextSize = dc.GetTextExtent(m_Infos[i].InfoName + m_Infos[i].InfoValue);

        CurrentColumeWidth = max(CurrentColumeWidth, TextSize.cx);

		CRect drawRect(ColumeStartX,(LineIndex%LinesPerColume) * LineHeight,
			TextSize.cx + ColumeStartX,(LineIndex%LinesPerColume) * LineHeight + TextSize.cy);
		if(drawRect.right > rect.right)
			drawRect.right = rect.right - 1;
		dc.DrawText(m_Infos[i].InfoName + m_Infos[i].InfoValue,drawRect,DT_SINGLELINE);
       // dc.TextOutW(ColumeStartX, (LineIndex%LinesPerColume) * LineHeight, m_Infos[i].InfoName + m_Infos[i].InfoValue);
        LineIndex ++;
        if(LineIndex/LinesPerColume != ColumeID)
        {
            ColumeID ++;
            ColumeStartX += CurrentColumeWidth + 10;
            CurrentColumeWidth = 0;
        }
    }

    dc.SetBkMode(OldMode);
}


void CInfoDisplayCtrl::ClearInfo()
{
    m_Infos.RemoveAll();
    CRect rect;
    GetClientRect(&rect);
    InvalidateRect(&rect);
    SetWindowText(_T(""));
}
void CInfoDisplayCtrl::AddInfo(InfoDisplayItem &DisplayItem)
{
    m_Infos.Add(DisplayItem);
    Invalidate(TRUE);
}

void CInfoDisplayCtrl::AddAuditInfo(Audit audit)
{
    HRESULT hr;
    User Requester;
    GetControllerInterface->GetUserInfo(audit.GetRequestUserID(), Requester, true);

    User Approver;
    GetControllerInterface->GetUserInfo(audit.GetApproveUserID(), Approver, true);

#ifdef SERVER_EDITION

    InfoDisplayItem InfoItem;

    InfoItem.InfoName = Translate(_T("Status: "));
    InfoItem.InfoValue = Translate(audit.GetStatusStr().c_str());
    AddInfo(InfoItem);

    switch(audit.GetStatus())
    {
    case Audit::CREATEBUTNOTAPPROVED:
        InfoItem.InfoName = Translate(_T("Request User: "));
        InfoItem.InfoValue = Requester.GetAccount().GetUserNameW().c_str();
        AddInfo(InfoItem);


        InfoItem.InfoName = Translate(_T("Request Time: "));

        InfoItem.InfoValue = audit.GetRequestTime().ToString().c_str();;
        AddInfo(InfoItem);
        break;
    case Audit::REJECTED:
        InfoItem.InfoName = Translate(_T("Request User: "));
        InfoItem.InfoValue = Requester.GetAccount().GetUserNameW().c_str();
        AddInfo(InfoItem);


        InfoItem.InfoName = Translate(_T("Request Time: "));

        InfoItem.InfoValue = audit.GetRequestTime().ToString().c_str();;
        AddInfo(InfoItem);


        InfoItem.InfoName = Translate(_T("Reject User: "));

        InfoItem.InfoValue = Approver.GetAccount().GetUserNameW().c_str();
        AddInfo(InfoItem);


        InfoItem.InfoName = Translate(_T("Reject Time: "));

        InfoItem.InfoValue = audit.GetApproveTime().ToString().c_str();
        AddInfo(InfoItem);

        break;
    case Audit::APPROVED:
        InfoItem.InfoName = Translate(_T("Request User: "));
        InfoItem.InfoValue = Requester.GetAccount().GetUserNameW().c_str();
        AddInfo(InfoItem);


        InfoItem.InfoName = Translate(_T("Request Time: "));

        InfoItem.InfoValue = audit.GetRequestTime().ToString().c_str();;
        AddInfo(InfoItem);


        InfoItem.InfoName = Translate(_T("Approve User: "));

        InfoItem.InfoValue = Approver.GetAccount().GetUserNameW().c_str();
        AddInfo(InfoItem);


        InfoItem.InfoName = Translate(_T("Approve Time: "));

        InfoItem.InfoValue = audit.GetApproveTime().ToString().c_str();
        AddInfo(InfoItem);
        break;
    case Audit::RETIRED:
        InfoItem.InfoName = Translate(_T("Request User: "));
        InfoItem.InfoValue = Requester.GetAccount().GetUserNameW().c_str();
        AddInfo(InfoItem);


        InfoItem.InfoName = Translate(_T("Request Time: "));

        InfoItem.InfoValue = audit.GetRequestTime().ToString().c_str();;
        AddInfo(InfoItem);


        InfoItem.InfoName = Translate(_T("Retired User: "));

        InfoItem.InfoValue = Approver.GetAccount().GetUserNameW().c_str();
        AddInfo(InfoItem);


        InfoItem.InfoName = Translate(_T("Retired Time: "));

        InfoItem.InfoValue = audit.GetApproveTime().ToString().c_str();
        AddInfo(InfoItem);
        break;
    case Audit::DELETED:
        InfoItem.InfoName = Translate(_T("Request User: "));
        InfoItem.InfoValue = Requester.GetAccount().GetUserNameW().c_str();
        AddInfo(InfoItem);


        InfoItem.InfoName = Translate(_T("Request Time: "));

        InfoItem.InfoValue = audit.GetRequestTime().ToString().c_str();;
        AddInfo(InfoItem);


        InfoItem.InfoName = Translate(_T("Delete User: "));

        InfoItem.InfoValue = Approver.GetAccount().GetUserNameW().c_str();
        AddInfo(InfoItem);


        InfoItem.InfoName = Translate(_T("Delete Time: "));

        InfoItem.InfoValue = audit.GetApproveTime().ToString().c_str();
        AddInfo(InfoItem);
        break;
    case Audit::RESERVED:
        break;
    case Audit::UNEDITABLE:
        break;
    case Audit::NONE:
        break;
    }

#endif

}