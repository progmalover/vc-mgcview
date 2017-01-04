// PropertyPlugin.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "PropertyPlugin.h"
#include "afxdialogex.h"
#include "PropertyCtrl.h"
#include "PropertyLayer.h"
#include "Localization/Localization.h"

extern UINT WM_PROPERTY_SETTING_CHANGE	;
UINT WM_ON_PLUGIN_THUMB_CHANGE = ::RegisterWindowMessage(_T("PROPERTY_PLUGIN_THUMB_CHANGE"));


#define CONTROL_ID_START 0x1000
#define CONTROL_ID_END   0x1fff
// CPropertyPlugin dialog

IMPLEMENT_DYNAMIC(CPropertyPlugin, CDialogEx)

CPropertyPlugin::CPropertyPlugin(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPropertyPlugin::IDD, pParent)
{

}

CPropertyPlugin::~CPropertyPlugin()
{
    CleanControls();
}

void CPropertyPlugin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropertyPlugin, CDialogEx)
    ON_WM_SIZE()
    ON_CONTROL_RANGE(EN_CHANGE, CONTROL_ID_START, CONTROL_ID_END, &CPropertyPlugin::OnChangeProperty)
    ON_CONTROL_RANGE(BN_CLICKED, CONTROL_ID_START, CONTROL_ID_END, &CPropertyPlugin::OnChangeProperty)
    ON_CONTROL_RANGE(CBN_SELCHANGE, CONTROL_ID_START, CONTROL_ID_END, &CPropertyPlugin::OnChangeProperty)

END_MESSAGE_MAP()

#define CONTROL_HEIGHT 23
#define CONTROL_SPACE 10

VOID CPropertyPlugin::CalcControlSize(int YPos, CRect &ControlRect, CRect &NameRect)
{
    CRect ClientRect;
    GetClientRect(ClientRect);

    ControlRect.left = CONTROL_SPACE + 60;
    ControlRect.top = YPos;
    ControlRect.right = ClientRect.Width() - CONTROL_SPACE;
    ControlRect.bottom = YPos + CONTROL_HEIGHT;

    NameRect.left = CONTROL_SPACE;
    NameRect.top = YPos;
    NameRect.right = 60;
    NameRect.bottom = YPos + CONTROL_HEIGHT;
}

VOID CPropertyPlugin::CalcControlSize(int YPos, CRect &ControlRect)
{
    CRect ClientRect;
    GetClientRect(ClientRect);

    ControlRect.left = CONTROL_SPACE;
    ControlRect.top = YPos;
    ControlRect.right = ClientRect.Width() - CONTROL_SPACE;
    ControlRect.bottom = YPos + CONTROL_HEIGHT;
}


// CPropertyPlugin message handlers
VOID CPropertyPlugin::SetContent(std::shared_ptr<IS3RODesigner2>& pDesigner2)
{
    DWORD PropertyCount = 0;
    RO_PROPERTY_INFO *pPropertyInfo = NULL;

    m_pDesigner = pDesigner2;
    pDesigner2->GetProperties(GetUserDefaultLangID(), &PropertyCount, pPropertyInfo);

    pPropertyInfo = new RO_PROPERTY_INFO[PropertyCount];

    pDesigner2->GetProperties(GetUserDefaultLangID(), &PropertyCount, pPropertyInfo);



    CleanControls();

    CRect ClientRect;
    GetClientRect(ClientRect);

    int ControlIDs = CONTROL_ID_START;
    int CurrentYPos = CONTROL_SPACE;

    for(DWORD i=0; i< PropertyCount; i++)
    {
        PROPERTY_ENTRY PropertyEntry;
        PropertyEntry.PropertyInfo = pPropertyInfo[i];
        PropertyEntry.pPropertyControl = NULL;
        PropertyEntry.pPropertyName = NULL;

		CString str; 
		str = Translate(PropertyEntry.PropertyInfo.InfoString);
		memset(PropertyEntry.PropertyInfo.InfoString,'\0',str.GetLength());
		if (str.GetLength() > 1024)
		{
			str = str.Left(1024);
		}
		_tcscpy(PropertyEntry.PropertyInfo.InfoString,str);

        switch(pPropertyInfo[i].PType)
        {
        case S3RO_PT_INT:
            {
                PropertyEntry.pPropertyControl = new CEdit();
                PropertyEntry.ControlID = ControlIDs;
                CRect ControlRect, NameRect;
                CalcControlSize(CurrentYPos, ControlRect, NameRect);

                ((CEdit *)PropertyEntry.pPropertyControl)->Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, ControlRect, this, ControlIDs++);
                ((CEdit *)PropertyEntry.pPropertyControl)-> ModifyStyleEx(0,   WS_EX_CLIENTEDGE | ES_NUMBER,   SWP_DRAWFRAME);
                
                CString DefaultValue;
                DefaultValue.Format(_T("%d"), PropertyEntry.PropertyInfo.defaultI);
                ((CEdit *)PropertyEntry.pPropertyControl)->SetWindowText(DefaultValue);
                
                PropertyEntry.pPropertyName = new CStatic();
                PropertyEntry.pPropertyName->Create(Translate(PropertyEntry.PropertyInfo.InfoString),WS_CHILD|WS_VISIBLE|SS_RIGHT|WS_TABSTOP   ,NameRect, this);
                }
            break;
        case S3RO_PT_FLOAT:
            {
                PropertyEntry.pPropertyControl = new CEdit();
                PropertyEntry.ControlID = ControlIDs;
                CRect ControlRect, NameRect;
                CalcControlSize(CurrentYPos, ControlRect, NameRect);

                ((CEdit *)PropertyEntry.pPropertyControl)->Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, ControlRect, this, ControlIDs++);
                ((CEdit *)PropertyEntry.pPropertyControl)-> ModifyStyleEx(0,   WS_EX_CLIENTEDGE,   SWP_DRAWFRAME);

                CString DefaultValue;
                DefaultValue.Format(_T("%f"), PropertyEntry.PropertyInfo.defaultF);
                ((CEdit *)PropertyEntry.pPropertyControl)->SetWindowText(DefaultValue);

                PropertyEntry.pPropertyName = new CStatic();
                PropertyEntry.pPropertyName->Create(PropertyEntry.PropertyInfo.InfoString,WS_CHILD|WS_VISIBLE|SS_RIGHT|WS_TABSTOP   ,NameRect, this);
            }
            break;
        case S3RO_PT_STR:
            {
                PropertyEntry.pPropertyControl = new CEdit();
                PropertyEntry.ControlID = ControlIDs;
                CRect ControlRect, NameRect;
                CalcControlSize(CurrentYPos, ControlRect, NameRect);

                ((CEdit *)PropertyEntry.pPropertyControl)->Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, ControlRect, this, ControlIDs++);
                ((CEdit *)PropertyEntry.pPropertyControl)-> ModifyStyleEx(0,   WS_EX_CLIENTEDGE,   SWP_DRAWFRAME);
                ((CEdit *)PropertyEntry.pPropertyControl)->SetWindowText(PropertyEntry.PropertyInfo.defaultStr);


                PropertyEntry.pPropertyName = new CStatic();
                PropertyEntry.pPropertyName->Create(PropertyEntry.PropertyInfo.InfoString,WS_CHILD|WS_VISIBLE|SS_RIGHT|WS_TABSTOP   ,NameRect, this);
            }
            break;
        case S3RO_PT_INTB:
            {
                PropertyEntry.pPropertyControl = new CButton();
                PropertyEntry.ControlID = ControlIDs;
                CRect ControlRect;
                CalcControlSize(CurrentYPos, ControlRect);
                ((CButton *)PropertyEntry.pPropertyControl)->Create(PropertyEntry.PropertyInfo.InfoString, WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_TABSTOP, ControlRect, this, ControlIDs++);
                ((CButton *)PropertyEntry.pPropertyControl)->SetCheck(PropertyEntry.PropertyInfo.defaultI);
            }
            break;
        case S3RO_PT_INTLIST:
            {
                PropertyEntry.pPropertyControl = new CComboBox();
                PropertyEntry.ControlID = ControlIDs;
                CRect ControlRect, NameRect;
                CalcControlSize(CurrentYPos, ControlRect, NameRect);

                ((CComboBox *)PropertyEntry.pPropertyControl)->Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | CBS_DROPDOWNLIST   , ControlRect, this, ControlIDs++);
                ((CComboBox *)PropertyEntry.pPropertyControl)->ModifyStyleEx(0,   WS_EX_CLIENTEDGE,   SWP_DRAWFRAME);
                ((CComboBox *)PropertyEntry.pPropertyControl)->SetCurSel(PropertyEntry.PropertyInfo.defaultI);

                CString InfoString = PropertyEntry.PropertyInfo.InfoString;
                CString TitleString;
                int ParserIndex = InfoString.Find(_T("|"), 0);
                TitleString = InfoString.Mid(0, ParserIndex);
                ParserIndex ++;

                while(ParserIndex < InfoString.GetLength())
                {
                    int NextSeperate = InfoString.Find(_T("|"), ParserIndex);

                    CString SubString = InfoString.Mid(ParserIndex, NextSeperate);
                    int ValueStart = SubString.Find(_T(" "), 0);
                    int ValueNumb = _tstoi(SubString.Mid(0, ValueStart));
                    ParserIndex = NextSeperate + 1;

                    CString ValueName = SubString.Mid(ValueStart + 1, SubString.GetLength());

                    int ComboIndex = ((CComboBox *)PropertyEntry.pPropertyControl)->AddString(ValueName);
                    ((CComboBox *)PropertyEntry.pPropertyControl)->SetItemData(ComboIndex, ValueNumb);
                }


                PropertyEntry.pPropertyName = new CStatic();
                PropertyEntry.pPropertyName->Create(TitleString,WS_CHILD|WS_VISIBLE|SS_RIGHT|WS_TABSTOP, NameRect, this);
            }
            break;
        default:
            continue;
            break;
        }
        
        PropertyEntry.pPropertyControl->SetFont(GetFont() );

        m_ControlList.Add(PropertyEntry);
        CurrentYPos += CONTROL_SPACE + CONTROL_HEIGHT;
		
    }

    if (pPropertyInfo)
    {
        delete[] pPropertyInfo;
        pPropertyInfo = NULL;
    }

    // resize window to fit property controls
    MoveWindow(ClientRect.left, ClientRect.top, ClientRect.Width(), CurrentYPos);

}

VOID CPropertyPlugin::CleanControls()
{
    for(int i=0; i< m_ControlList.GetCount(); i++)
    {
        if (m_ControlList[i].pPropertyControl != NULL)
        {
            if (m_ControlList[i].pPropertyControl->GetSafeHwnd())
            {
                m_ControlList[i].pPropertyControl->DestroyWindow();
            }

            delete m_ControlList[i].pPropertyControl;
        }
        m_ControlList[i].pPropertyControl = NULL;
    }

    m_ControlList.RemoveAll();
}

void CPropertyPlugin::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    int CurrentYPos = CONTROL_SPACE;
    for(int i=0; i<m_ControlList.GetCount(); i++ )
    {
        switch(m_ControlList[i].PropertyInfo.PType)
        {
            case S3RO_PT_INT:
            case S3RO_PT_FLOAT:
            case S3RO_PT_STR:
            case S3RO_PT_INTLIST:
                {
                    CRect ControlRect, NameRect;
                    CalcControlSize(CurrentYPos, ControlRect, NameRect);
                    m_ControlList[i].pPropertyControl->MoveWindow(ControlRect);
                    m_ControlList[i].pPropertyName->MoveWindow(NameRect);
                }
                break;
            case S3RO_PT_INTB:
                {
                    CRect ControlRect;
                    CalcControlSize(CurrentYPos, ControlRect);
                    m_ControlList[i].pPropertyControl->MoveWindow(ControlRect);
                }
                break;
        }

        CurrentYPos += CONTROL_SPACE + CONTROL_HEIGHT;
    }

}

void CPropertyPlugin::OnChangeProperty(UINT uID)
{
    for(int i=0; i< m_ControlList.GetCount(); i++)
    {
        if(uID == m_ControlList[i].ControlID)
        {
            BOOL bNewThumbnail = FALSE;
            switch(m_ControlList[i].PropertyInfo.PType)
            {   
            case S3RO_PT_INT:
                {
                    CString StrValue;

                    ((CEdit *)m_ControlList[i].pPropertyControl)->GetWindowTextW(StrValue);
                    int ValueNumber = _tstoi(StrValue);

                    if(ValueNumber > m_ControlList[i].PropertyInfo.maxI)
                    {
                        ValueNumber = m_ControlList[i].PropertyInfo.maxI;
                        StrValue.Format(_T("%d"), ValueNumber);
                        ((CEdit *)m_ControlList[i].pPropertyControl)->SetWindowText(StrValue);
                    }

                    if(ValueNumber < m_ControlList[i].PropertyInfo.minI)
                    {
                        ValueNumber = m_ControlList[i].PropertyInfo.minI;
                        StrValue.Format(_T("%d"), ValueNumber);
                        ((CEdit *)m_ControlList[i].pPropertyControl)->SetWindowText(StrValue);
                    }

                    m_pDesigner->SetPropertyI(m_ControlList[i].PropertyInfo.PID, _tstoi(StrValue), &bNewThumbnail);
                }
                break;
            case S3RO_PT_FLOAT:
                {
                    CString StrValue;

                    ((CEdit *)m_ControlList[i].pPropertyControl)->GetWindowTextW(StrValue);
                    
                    float ValueNumber = (float)_tstof(StrValue);

                    if(ValueNumber > m_ControlList[i].PropertyInfo.maxF)
                    {
                        ValueNumber = m_ControlList[i].PropertyInfo.maxF;
                        StrValue.Format(_T("%f"), ValueNumber);
                        ((CEdit *)m_ControlList[i].pPropertyControl)->SetWindowText(StrValue);
                    }

                    if(ValueNumber < m_ControlList[i].PropertyInfo.minF)
                    {
                        ValueNumber = m_ControlList[i].PropertyInfo.minF;
                        StrValue.Format(_T("%f"), ValueNumber);
                        ((CEdit *)m_ControlList[i].pPropertyControl)->SetWindowText(StrValue);
                    }
                    
                    m_pDesigner->SetPropertyF(m_ControlList[i].PropertyInfo.PID, ValueNumber, &bNewThumbnail);
                }

                break;
            case S3RO_PT_STR:
                {
                    CString StrValue;

                    ((CEdit *)m_ControlList[i].pPropertyControl)->GetWindowTextW(StrValue);
                    m_pDesigner->SetPropertyS(m_ControlList[i].PropertyInfo.PID, StrValue, &bNewThumbnail);
                }
                break;
            case S3RO_PT_INTB:
                {
                    BOOL Checked;

                    Checked = ((CButton *)m_ControlList[i].pPropertyControl)->GetCheck();
                    m_pDesigner->SetPropertyI(m_ControlList[i].PropertyInfo.PID, Checked, &bNewThumbnail);
                }
                break;
            case S3RO_PT_INTLIST:
                {
                    int CurSelected;
                    int SelectedValue = 0;

                    CurSelected = ((CComboBox *)m_ControlList[i].pPropertyControl)->GetCurSel();
                    SelectedValue = ((CComboBox *)m_ControlList[i].pPropertyControl)->GetItemData(CurSelected);
                    m_pDesigner->SetPropertyI(m_ControlList[i].PropertyInfo.PID, SelectedValue, &bNewThumbnail);
                }

                break;
            }

            if(bNewThumbnail)
            {
                 GetParent()->PostMessage(WM_ON_PLUGIN_THUMB_CHANGE, 0, 0);
            }
            break;
        }
    }
    CleanControls();
    SetContent(m_pDesigner);
    GetParent()->SendMessage(WM_ON_MEDIA_SEL_CHANGE);
    CFocusEx::SetFocusToFirstWindow(GetSafeHwnd());
}

BOOL CPropertyPlugin::PreTranslateMessage(MSG* pMsg)
{
    m_focus.InitFocusEx(GetFocusableWindow, this);

    if (m_focus.ProcessKeyPressMessage(this, pMsg))
    {
        return TRUE;
    }
    if (pMsg->message == WM_MOUSEWHEEL)
    {
        GetParent()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
        return TRUE;
    }
    if (pMsg->message == WM_LBUTTONDOWN)
    {
        SetFocus();
    }

    return __super::PreTranslateMessage(pMsg);
}

HWND CPropertyPlugin::GetFocusableWindow(int nPoisition, LPVOID lParam, BOOL shift)
{
    CPropertyPlugin* pThis = (CPropertyPlugin*)lParam;
    switch (nPoisition)
    {
    case FOCUSABLEWINDOW_POSITION_FIRST:
        return CFocusEx::GetFirstFocusableWindow(pThis->GetSafeHwnd());
    case FOCUSABLEWINDOW_POSITION_LAST:
        return CFocusEx::GetLastFocusableWindow(pThis->GetSafeHwnd());
    case FOCUSABLEWINDOW_POSITION_FOCUSABLE:
        {
            CPropertyCtrl* pParent = (CPropertyCtrl*)(pThis->GetParent());
            return pParent->GetNextFocusWindow(pThis->GetSafeHwnd(), shift);
        }
    }
    return NULL;
}
