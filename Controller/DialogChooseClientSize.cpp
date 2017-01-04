// DialogChooseClientSize.cpp : 实现文件
//

#include "stdafx.h"
#include "Controller.h"
#include "DialogChooseClientSize.h"
#include "afxdialogex.h"
#include "LayoutDesignerCtrl.h"
#include "Localization/Localization.h"

// CDialogChooseClientSize 对话框

IMPLEMENT_DYNAMIC(CDialogChooseClientSize, CDialogEx)

CDialogChooseClientSize::CDialogChooseClientSize(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogChooseClientSize::IDD, pParent)
{
    
}

CDialogChooseClientSize::~CDialogChooseClientSize()
{
}

void CDialogChooseClientSize::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_CLIENT, m_wndClientSize);
}


BEGIN_MESSAGE_MAP(CDialogChooseClientSize, CDialogEx)
END_MESSAGE_MAP()


// CDialogChooseClientSize 消息处理程序


void CDialogChooseClientSize::OnOK()
{
    int pos = m_wndClientSize.GetNextItem(-1, LVNI_SELECTED);
    if (pos != -1)
    {
        //m_clientID = m_Clients[pos].m_clientID;
        m_rectSelectedLayout = m_Clients[pos];
        CDialogEx::OnOK();
    }
    else
    {
        MessageBox(Translate(_T("Please select a client layout")), Translate(_T("Warning:Choose client size")), MB_ICONEXCLAMATION|MB_OK);
    }
}


BOOL CDialogChooseClientSize::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_wndClientSize.SetExtendedStyle(m_wndClientSize.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

    m_wndClientSize.InsertColumn(0, Translate(_T("Client Name")), LVCFMT_LEFT, 80);
    m_wndClientSize.InsertColumn(1, Translate(_T("Client Layout width")), LVCFMT_LEFT, 60);
    m_wndClientSize.InsertColumn(2, Translate(_T("Client Layout height")), LVCFMT_LEFT, 60);
    m_wndClientSize.InsertColumn(3,Translate(_T("Client IP address")), LVCFMT_LEFT, 100); 
    m_wndClientSize.InsertColumn(4, Translate(_T("Screen Rotation")), LVCFMT_LEFT, 60);
    m_wndClientSize.InsertColumn(5, Translate(_T("Rotated Width")), LVCFMT_LEFT, 60);
    m_wndClientSize.InsertColumn(6, Translate(_T("Rotated Height")), LVCFMT_LEFT, 60);

    Condition cond(_T("ClientName like '%%'"));
    std::vector<int> clientIDList;

    HRESULT hr = GetControllerInterface->SearchClient(cond, clientIDList);
    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
        {
            GetControllerApp->OnControllerServerOffline();
        }
        else
        {
            TRACE0("Failed to Search Client!\n");
            m_wndClientSize.InsertItem(0,_T(" "));
            m_wndClientSize.SetItemText(0,1,_T("Failed to Search Client!\n"));
        }
        return FALSE;
    }

    CRect rect;
    auto it = clientIDList.begin(), ite = clientIDList.end();
    for (int i = 0; it != ite; ++it, ++i)
    {
        CClientDesc clientDesc;

        ClientInfo info;
        hr = GetControllerInterface->GetClient(*it, info);
        if (SUCCEEDED(hr))
        {
            std::tstring str = info.GetDisplayLayout();
            clientDesc.m_clientID = *it;

            std::string utfStr = StringUtility::ConvertUnicodeToUTF8(str);
            TiXmlDocument xmlDoc;
            xmlDoc.Parse(utfStr.c_str(), NULL, TIXML_ENCODING_UTF8);

            TiXmlElement* root = xmlDoc.RootElement();
            if( NULL == root)
            {
                i--;
                continue;
            }
           
            TiXmlAttribute* pAttribute = root->FirstAttribute();
            ASSERT(pAttribute);
            while(pAttribute)
            {
                std::string strName =  pAttribute->Name();
                std::string strVal  =  pAttribute->Value();

                if(strName == "left")
                    clientDesc.left = stoi(strVal);
                if(strName == "top")
                    clientDesc.top = stoi(strVal);
                if(strName == "right")
                    clientDesc.right = stoi(strVal);
                if(strName == "bottom")
                    clientDesc.bottom = stoi(strVal);
                pAttribute = pAttribute->Next();
            }

            TiXmlElement *pSubElement = root->FirstChildElement();
            while(pSubElement)
            {
                TiXmlAttribute *pSubAttribute = pSubElement->FirstAttribute();
                ASSERT(pSubAttribute);
                CRect monitorRect;
                while(pSubAttribute)
                {
                    std::string strName = pSubAttribute->Name();
                    std::string strVal  = pSubAttribute->Value();

                    if(strName == "left")
                        monitorRect.left = stoi(strVal);
                    if(strName == "top")
                        monitorRect.top = stoi(strVal);
                    if(strName == "right")
                        monitorRect.right = stoi(strVal);
                    if(strName == "bottom")
                        monitorRect.bottom = stoi(strVal);
                      
                    pSubAttribute = pSubAttribute->Next();
                } 
                clientDesc.m_MonitorRects.push_back(monitorRect);
                pSubElement = pSubElement->NextSiblingElement();
            }
            if(clientDesc.Width() <= 0 || clientDesc.Height() <= 0)//client size can't be null!
            {
                ASSERT(0);
            }
            //offset to 0,0 
            CPoint ptLeftTop = clientDesc.TopLeft();
            clientDesc.OffsetRect(-ptLeftTop);
            clientDesc.right += ptLeftTop.x;
            clientDesc.bottom += ptLeftTop.y;

            clientDesc.m_fRotateDegree = PI * info.GetRotateDegree() / 180.0;
            m_Clients.push_back(clientDesc);

            m_wndClientSize.InsertItem(i, info.GetClientDesc().c_str());
            m_wndClientSize.SetItemText(i, 1, StringUtility::itos(clientDesc.Width()).c_str());
            m_wndClientSize.SetItemText(i, 2, StringUtility::itos(clientDesc.Height()).c_str());


            CSize szClient(clientDesc.Width(),clientDesc.Height());
            FLOAT fRotateDegree = clientDesc.m_fRotateDegree; 

            CRect rctScreen = GetRotatedScreen(szClient.cx,szClient.cy, fRotateDegree);

            m_wndClientSize.SetItemText(i, 4, (StringUtility::itos(info.GetRotateDegree()) + _T("°")).c_str());
            m_wndClientSize.SetItemText(i, 5, StringUtility::itos(rctScreen.Width()).c_str());
            m_wndClientSize.SetItemText(i, 6, StringUtility::itos(rctScreen.Height()).c_str());
                
            ClientRTStatistics rtStat;
            hr = GetControllerInterface->GetClientRTStatistics(*it, rtStat);
            if (SUCCEEDED(hr))
            {
                if (rtStat.IsOnline())
                    m_wndClientSize.SetItemText(i, 3, rtStat.GetIPAddr().c_str());
                else if (!rtStat.IsOnline())
                    m_wndClientSize.SetItemText(i, 3, Translate(_T("Offline")));
            }
            else
                m_wndClientSize.SetItemText(i, 3, Translate(_T("Unknown")));

            
        }
        else
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
            {
                GetControllerApp->OnControllerServerOffline();
            }
            else
            {
                TRACE0("Failed to Get Client!\n");
            }
            return FALSE;
        }
    }

	Translate(this);
    return TRUE;  // return TRUE unless you set the focus to a control
}

