// PropertyLayer.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "PropertyLayer.h"
#include "afxdialogex.h"
#include "DesignerPage.h"
#include "Localization/Localization.h"
#include "LayoutDesignerCtrl.h"
#include "CommonUI/UIntRangeEdit.h"
#include "CommonUI/IntRangeEdit.h"
#include "CommonUI/BaseEdit.h"
extern UINT WM_PROPERTY_SETTING_CHANGE	;
// wParam - point of CS3DrawObj, lParam - point of POS_CHANGE_INFO, include the change type(one of the x, y, w, h) and FRect, the changed x, y, w, h


UINT WM_ON_SIZE_POS_CHANGE2 = ::RegisterWindowMessage(_T("PROPERTY_SIZE_POS_CHANGE2"));

// wParam - no used, lParam - no used
UINT WM_ON_MEDIA_SEL_CHANGE = ::RegisterWindowMessage(_T("PROPERTY_MEDIA_SEL_CHANGE"));

// wParam - point of CS3DrawObj, lParam - no used
UINT WM_ON_LAYER_LOOP_CHANGE = ::RegisterWindowMessage(_T("PROPERTY_LAYER_LOOP_CHANGE"));
UINT WM_ON_LAYER_KEEP_ASPECT_CHANGE = ::RegisterWindowMessage(_T("WM_ON_LAYER_KEEP_ASPECT_CHANGE"));
UINT WM_ON_LAYER_NAME_CHANGE = ::RegisterWindowMessage(_T("PROPERTY_LAYER_NAME_CHANGE"));
typedef struct
{
    UINT controlID;
    int minValue;
    int maxValue;

    UINT buddyControlID;
    int minBuddyValue;
    int maxBuddyValue;

    const CString name;
} ValueCheck_t;

#define ID_NONE -1


// CPropertyLayer dialog

IMPLEMENT_DYNAMIC(CPropertyLayer, CDialogEx)

CPropertyLayer::CPropertyLayer(CWnd* pParent /*=NULL*/)
    : CDialogEx(CPropertyLayer::IDD, pParent)
    , m_bAlign2Grid(FALSE)
	, m_szXaxis(_T("0"))
	, m_szYaxis(_T("0"))
	, m_szWidth(_T("0"))
	, m_szHeight(_T("0"))
{
    m_layerInfo.reset();
    m_pDrawObj = NULL;
    m_bInChangeSize = false;
    memset(&m_ChangePos, 0, sizeof(POS_CHANGE_INFO));
    m_Duration = 0;
    m_LayoutDuration = 0;
}

CPropertyLayer::~CPropertyLayer()
{
}

void CPropertyLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);


	//fix a memory leak caused by MFC feature pack. When attach CVSlistbox,
	//MFC need to detach and free the old created CSListBox, but then child
	//control of the VSListbox was not freed.. Now add patch to free these
	//child control when attach CVSListBox.

	if(m_MediaList.m_hWnd == NULL)
	{
		CVSListBox *pUnbindedVSListBox = (CVSListBox*)GetDlgItem(IDC_MFCVSLISTBOX_MEDIA);

		if(pUnbindedVSListBox)
		{

			CArray<CWnd *, CWnd *> ChildWndList;
			CWnd *pChild = pUnbindedVSListBox->GetWindow(GW_CHILD);

			while(pChild)
			{
				ChildWndList.Add(pChild);
				pChild = pChild->GetNextWindow();
			}

			for(int i=0; i< ChildWndList.GetCount(); i++)
			{
				ChildWndList[i]->DestroyWindow();
				//delete ChildWndList[i];
			}

			ChildWndList.RemoveAll();
		}
	}

	DDX_Control(pDX, IDC_MFCVSLISTBOX_MEDIA, m_MediaList);
	DDX_Text(pDX, IDC_EDIT_X, m_szXaxis);
	DDX_Text(pDX, IDC_EDIT_Y, m_szYaxis);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_szWidth);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_szHeight);
}


BEGIN_MESSAGE_MAP(CPropertyLayer, CDialogEx)
    ON_WM_SIZE()
     ON_EN_CHANGE(IDC_EDIT_NAME, &CPropertyLayer::OnEnChangeEditName)
    ON_EN_CHANGE(IDC_EDIT_X, &CPropertyLayer::OnEnChangeEditX)
    ON_EN_CHANGE(IDC_EDIT_Y, &CPropertyLayer::OnEnChangeEditY)
    ON_EN_CHANGE(IDC_EDIT_WIDTH, &CPropertyLayer::OnEnChangeEditWidth)
    ON_EN_CHANGE(IDC_EDIT_HEIGHT, &CPropertyLayer::OnEnChangeEditHeight)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_MFCVSLISTBOX_MEDIA, &CPropertyLayer::OnLvnItemchangedMediaList)
    ON_BN_CLICKED(IDC_CHECK_KEEP_ASPACT, &CPropertyLayer::OnBnClickedCheckKeepAspact)
    ON_BN_CLICKED(IDC_CHECK_LOOPSYNC, &CPropertyLayer::OnBnClickedCheckLoopSync)
    ON_REGISTERED_MESSAGE(WM_ON_MEDIA_NUM_CHANGE, &CPropertyLayer::OnMediaNumChange)
	ON_REGISTERED_MESSAGE(WM_ON_SIMPLE_PLUGIN_SIZE_CHANGE, &CPropertyLayer::OnPlugSizeChange)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CPropertyLayer)
    EASYSIZE(IDC_EDIT_LAYERDURATION, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
    EASYSIZE(IDC_CHECK_LOOPSYNC, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_HCENTER)
    EASYSIZE(IDC_STATIC_APPEARANCE,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_CHECK_KEEP_ASPACT,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_HCENTER)
//     EASYSIZE(IDC_STATIC_Y,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_HCENTER)
//     EASYSIZE(IDC_STATIC_H,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_HCENTER)
    EASYSIZE(IDC_MFCVSLISTBOX_MEDIA,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
//     EASYSIZE(IDC_EDIT_Y,IDC_STATIC_Y,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
//     EASYSIZE(IDC_EDIT_HEIGHT,IDC_STATIC_Y,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
//     EASYSIZE(IDC_EDIT_X,ES_BORDER,ES_BORDER,IDC_STATIC_Y,ES_KEEPSIZE,0)
//     EASYSIZE(IDC_EDIT_WIDTH,ES_BORDER,ES_BORDER,IDC_STATIC_Y,ES_KEEPSIZE,0)
//     EASYSIZE(IDC_STATIC_PIXELS_Y, IDC_EDIT_Y, ES_BORDER, ES_BORDER, ES_BORDER, 0)
//     EASYSIZE(IDC_STATIC_PIXELS_H, IDC_EDIT_HEIGHT, ES_BORDER, ES_BORDER, ES_BORDER, 0)
//     
    EASYSIZE(IDC_STATIC_X, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
    EASYSIZE(IDC_STATIC_Y, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
    EASYSIZE(IDC_STATIC_H, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
    EASYSIZE(IDC_STATIC_W, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
    EASYSIZE(IDC_STATIC_NAME, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)

    EASYSIZE(IDC_EDIT_NAME, IDC_STATIC_NAME, ES_BORDER, ES_BORDER, ES_BORDER, 0)
    EASYSIZE(IDC_EDIT_X, IDC_STATIC_X, ES_BORDER, ES_BORDER, ES_BORDER, 0)
    EASYSIZE(IDC_EDIT_Y, IDC_STATIC_Y, ES_BORDER, ES_BORDER, ES_BORDER, 0)
    EASYSIZE(IDC_EDIT_WIDTH, IDC_STATIC_H, ES_BORDER, ES_BORDER, ES_BORDER, 0)
    EASYSIZE(IDC_EDIT_HEIGHT, IDC_STATIC_W, ES_BORDER, ES_BORDER, ES_BORDER, 0)


    EASYSIZE(IDC_STATIC_PIXELS_X, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_BORDER, 0)
    EASYSIZE(IDC_STATIC_PIXELS_Y, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_BORDER, 0)
    EASYSIZE(IDC_STATIC_PIXELS_H, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_BORDER, 0)
    EASYSIZE(IDC_STATIC_PIXELS_W, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_BORDER, 0)

END_EASYSIZE_MAP

// CPropertyLayer message handlers
BOOL CPropertyLayer::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    INIT_EASYSIZE;

    CFont font;
    LOGFONT lf;
    CWnd* content = (CWnd*)GetDlgItem(IDC_STATIC_LAYER);
    content->GetFont()->GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;
    font.CreateFontIndirect(&lf);
    content->SetFont(&font);
    font.Detach();

	

	//add by sophia
	m_Name.SubclassDlgItem (IDC_EDIT_NAME, this);
	m_Name.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_Name.SetRegEx(_T("[^\\\\/:*?\"<>|]*"));

	m_EditX.SubclassDlgItem(IDC_EDIT_X, this);
	m_EditX.SetRange(-30000,30000);
	m_EditX.LimitText(6);
	m_EditX.SetWavyLineColour (RGB (255, 255, 255));
	m_EditX.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_EditX.SetForegroundColourError (RGB (255, 255, 255));
	m_EditX.SetBackgroundColourError (RGB (255, 0, 0));

	m_EditY.SubclassDlgItem(IDC_EDIT_Y, this);
	m_EditY.SetRange(-30000,30000);
	m_EditY.LimitText(6);
	m_EditY.SetWavyLineColour (RGB (255, 255, 255));
	m_EditY.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_EditY.SetForegroundColourError (RGB (255, 255, 255));
	m_EditY.SetBackgroundColourError (RGB (255, 0, 0));


	m_EditWidth.SubclassDlgItem (IDC_EDIT_WIDTH, this);
	m_EditWidth.SetRange(1, 30000);
	m_EditWidth.LimitText(5);
	m_EditWidth.SetWavyLineColour (RGB (255, 255, 255));
	m_EditWidth.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_EditWidth.SetForegroundColourError (RGB (255, 255, 255));
	m_EditWidth.SetBackgroundColourError (RGB (255, 0, 0));

	
	m_EditHeight.SubclassDlgItem (IDC_EDIT_HEIGHT, this);
	m_EditHeight.SetRange(1, 30000);
	m_EditHeight.LimitText(5);
	m_EditHeight.SetWavyLineColour (RGB (255, 255, 255));
	m_EditHeight.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_EditHeight.SetForegroundColourError (RGB (255, 255, 255));
	m_EditHeight.SetBackgroundColourError (RGB (255, 0, 0));

	//m_EditX.SubclassWindow(GetDlgItem(IDC_EDIT_X)->GetSafeHwnd());
	//m_EditY.SubclassWindow(GetDlgItem(IDC_EDIT_Y)->GetSafeHwnd());
	//m_EditWidth.SubclassWindow(GetDlgItem(IDC_EDIT_WIDTH)->GetSafeHwnd());
	//m_EditHeight.SubclassWindow(GetDlgItem(IDC_EDIT_HEIGHT)->GetSafeHwnd());
	Translate(this);

    return FALSE;  // return TRUE  unless you set the focus to a control
}


void CPropertyLayer::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

BOOL CPropertyLayer::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Add your specialized code here and/or call the base class

    CDialogEx::PreCreateWindow(cs);

    cs.style |= WS_CLIPCHILDREN;
    return TRUE;
}

VOID  CPropertyLayer::SetContent(std::shared_ptr<LayerElement> layerInfo, CS3DrawObj *pDrawObj)
{
    if( NULL == layerInfo || NULL == pDrawObj)
    {
        TRACE(_T("Function SetContent() call is error!"));
        return ;
    }
	m_layerInfo = layerInfo;
    m_pDrawObj = pDrawObj;
    if(m_layerInfo.get() && m_pDrawObj)
    {
        //((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->SetCheck(pDrawObj->);
        //FRect Rect;
        CRect rectPos;
        //m_pDrawObj->GetPosition(rectPos);
        //CLayoutDesignerCtrl *pDesigner = NULL;
        //CDesignerPage *pPage =(CDesignerPage *) GetCurrentPageCtrl(RUNTIME_CLASS(CDesignerPage));
        //ASSERT(pPage);
        //if(pPage)
        //{
        //    pDesigner = (CLayoutDesignerCtrl *)SearchWndObj(pPage,RUNTIME_CLASS(CLayoutDesignerCtrl));
        //    ASSERT(pDesigner);
        //    if(pDesigner)
        //        pDesigner->ClientToLayout(rectPos);
        //}

        //if(!pPage || !pDesigner)
        //{
            FRect Rect;
            m_pDrawObj->GetLogicPos(Rect);
            Rect.ConvertToCRect(rectPos);
        //}

        CString sWidth, sHeight, sXPos, sYPos;
        int xPos    = 0;
        int yPos    = 0;
        int width   = 0;
        int height  = 0;
        GetDlgItem(IDC_EDIT_X)->GetWindowText(sXPos);
        GetDlgItem(IDC_EDIT_Y)->GetWindowText(sYPos);
        GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText(sWidth);
        GetDlgItem(IDC_EDIT_HEIGHT)->GetWindowText(sHeight);
        xPos    = _tstoi(sXPos);
        yPos    = _tstoi(sYPos);
        width   = _tstoi(sWidth);
        height  = _tstoi(sHeight);

#if 0
        if (rectPos.left != xPos)
        {
            sXPos.Format(_T("%d"), rectPos.left);
            GetDlgItem(IDC_EDIT_X)->SetWindowText(sXPos);
        }
        if (rectPos.top != yPos)
        {
            sYPos.Format(_T("%d"), rectPos.top);
            GetDlgItem(IDC_EDIT_Y)->SetWindowText(sYPos);
        }
        if (rectPos.Width() != width)
        {
            sWidth.Format(_T("%d"), rectPos.Width());
            GetDlgItem(IDC_EDIT_WIDTH)->SetWindowText(sWidth);
        }
        if (rectPos.Height() != height)
        {
            sHeight.Format(_T("%d"), rectPos.Height());
            GetDlgItem(IDC_EDIT_HEIGHT)->SetWindowText(sHeight);
        }
#else
		//xiangyang 
        CRect newRect;
        Rect.ConvertToCRect(newRect);
		if ( (newRect.left != xPos ) || (newRect.top != yPos) || (newRect.Width() != width) || (newRect.Height() != height) )
		{
			CString str;
			str.Format(_T("%d"), newRect.left);
			m_szXaxis = str;
			str.Format(_T("%d"), newRect.top );
			m_szYaxis = str;  
			str.Format(_T("%d"), newRect.Width()); 
			m_szWidth = str;
			str.Format(_T("%d"), newRect.Height());
			m_szHeight = str;
			UpdateData(FALSE);
		}
#endif
        ((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->SetCheck(m_pDrawObj->GetKeepAspect());
		BOOL keepAspectRatio = m_pDrawObj->GetKeepAspect();
		m_pDrawObj->SetKeepAspect(keepAspectRatio);
		m_pDrawObj->CalcAspectRatio();
		

        BOOL bLoop= FALSE;
        m_pDrawObj->GetLoopSync(bLoop);
        ((CButton *)GetDlgItem(IDC_CHECK_LOOPSYNC))->SetCheck(bLoop);

        CalcLayerDuration();

/*
        CDesignerPage* designerPage = (CDesignerPage*)(GetParent()->GetParent()->GetParent());
        float ratio = designerPage->GetLayerAspectRatio(m_pDrawObj);
        if (ratio > 0.000001f)
            ((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->SetCheck(TRUE);
        else
            ((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->SetCheck(FALSE);*/
        CString strSerialName;
        m_pDrawObj->GetSerialName(strSerialName);
        SetDlgItemText(IDC_EDIT_NAME,strSerialName);

        CString type;
        m_pDrawObj->GetName(type);
        if (type == szTypeName_Audio || 0 == type.CompareNoCase(_T("S3Clock")))
        {
            GetDlgItem(IDC_CHECK_KEEP_ASPACT)->EnableWindow(FALSE);
        }
        else 
        {
            GetDlgItem(IDC_CHECK_KEEP_ASPACT)->EnableWindow(TRUE);
        }

        m_MediaList.SetContent(m_layerInfo);
    }
}

VOID CPropertyLayer::SetDuration(DWORD duration)
{
    m_LayoutDuration = duration;
}

void CPropertyLayer::Clear()
{
    m_layerInfo.reset();
    m_pDrawObj = NULL;
    m_Duration = 0;
    m_LayoutDuration = 0;
}

void CPropertyLayer::OnEnChangeEditName()
{
  //  const CString::PCXSTR interpunction = _T("\\/:*?\"<>|");
    ASSERT(m_pDrawObj);
    CString strText;
    GetDlgItemText(IDC_EDIT_NAME,strText);
    strText.Trim();
    
    if(strText.IsEmpty())
    {
        MessageBox(Translate(_T("New Name should not Null!")), Translate(_T("Tips:Rename layout")), MB_OK|MB_ICONERROR);
        return;
    }
	GetOwner()->SendMessage(WM_ON_LAYER_NAME_CHANGE,(WPARAM)m_pDrawObj,(LPARAM)0);

}

void CPropertyLayer::OnEnChangeEditX()
{
    if (m_bInChangeSize) return;

    if(m_pDrawObj)
    {
        //CDesignerPage* designerPage = (CDesignerPage*)(GetParent()->GetParent()->GetParent());
        //CheckInputValue(VALUE_CHECK_TYPE_X, -1.0f);
        CString sXPos;

        GetDlgItem(IDC_EDIT_X)->GetWindowText(sXPos);
		int ipos = 0;
		while(true)
		{
			ipos = (sXPos.Find(_T('-'),ipos));
			if(ipos == -1)
			{
				break;
			}
			else if (ipos == 0x00)
			{
				ipos++;
			}
			else 
			{	
				UpdateData(FALSE);
				((CEdit *)(GetDlgItem(IDC_EDIT_X)))->SetFocus();
				//((CEdit *)(GetDlgItem(IDC_EDIT_X)))->SetSel(m_xaxis.GetLength(),m_xaxis.GetLength(),TRUE);
				return;
			}
		}
		
		//((CEdit *)(GetDlgItem(IDC_EDIT_X)))->SetSel(sXPos.GetLength(),sXPos.GetLength(),TRUE);
		//UpdateData(TRUE);

        FRect Rect;
        CRect rectPos;
        int xPos = _tstoi(sXPos);

		if(xPos < -30000 || xPos > 30000)
		{
			MessageBox(Translate(_T("Invalid value of the layer's left position. Please input the value in the range: -30000 - 30000!")), Translate(_T("Warning:Check value")), MB_OK|MB_ICONEXCLAMATION);
			UpdateData(FALSE);
			((CEdit *)(GetDlgItem(IDC_EDIT_X)))->SetFocus();
			return;
		}
		else
		{
            CString sX;
            sX.Format(_T("%d"),xPos);
            if (!m_szXaxis.Compare(sX))
            {
                m_szYaxis = sX;
                UpdateData(FALSE);
            }
            else
                UpdateData(TRUE);
		}

        m_pDrawObj->GetLogicPos(Rect);
        Rect.ConvertToCRect(rectPos);

        if (rectPos.left != xPos)
        {
            Rect.right  = (FLOAT)xPos + Rect.Width();
            Rect.left   = (FLOAT)xPos;
            //rectPos.right   = xPos + rectPos.Width();
            //rectPos.left    = xPos;
            //Rect = rectPos;
            //m_pDrawObj->SetLogicPos(Rect);
            m_ChangePos.ChangeType    = VALUE_TYPE_X;
            m_ChangePos.rectChanged   = Rect;
            GetParent()->PostMessage(WM_ON_SIZE_POS_CHANGE2, (WPARAM)m_pDrawObj, (LPARAM)&m_ChangePos);
        }
    }
}

void CPropertyLayer::OnEnChangeEditY()
{
    if (m_bInChangeSize) return;

    if(m_pDrawObj)
    {
        //CDesignerPage* designerPage = (CDesignerPage*)(GetParent()->GetParent()->GetParent());
        //CheckInputValue(VALUE_CHECK_TYPE_Y, -1.0f);

        CString sYPos;
		
        GetDlgItem(IDC_EDIT_Y)->GetWindowText(sYPos);
		int ipos = 0;
		while(true)
		{
			ipos = (sYPos.Find(_T('-'),ipos));
			if(ipos == -1)
			{
				break;
			}
			else if (ipos == 0x00)
			{
				ipos++;
			}
			else 
			{	
				UpdateData(FALSE);
				((CEdit *)(GetDlgItem(IDC_EDIT_Y)))->SetFocus();
				//((CEdit *)(GetDlgItem(IDC_EDIT_X)))->SetSel(m_yaxis.GetLength(),m_yaxis.GetLength(),TRUE);
				return;
			}
		}
		//((CEdit *)(GetDlgItem(IDC_EDIT_Y)))->SetSel(sYPos.GetLength(),sYPos.GetLength(),TRUE);
		//UpdateData(TRUE);
		
        FRect Rect;
        CRect rectPos;
        int yPos = _tstoi(sYPos);
		
		if(yPos < -30000 || yPos > 30000)
		{
            MessageBox(Translate(_T("Invalid value of the layer's top position. Please input the value in the range: -30000 - 30000!")), Translate(_T("Warning:Check value")), MB_OK|MB_ICONEXCLAMATION);
			UpdateData(FALSE);
			((CEdit *)(GetDlgItem(IDC_EDIT_Y)))->SetFocus();
			return;
		}
		else
		{
            CString sY;
            sY.Format(_T("%d"),yPos);
            if (!m_szYaxis.Compare(sY))
            {
                m_szYaxis = sY;
                UpdateData(FALSE);
            }
            else
			    UpdateData(TRUE);
		}

        m_pDrawObj->GetLogicPos(Rect);
        Rect.ConvertToCRect(rectPos);

        if (rectPos.top != yPos)
        {
            Rect.bottom = (FLOAT)yPos + Rect.Height();
            Rect.top    = (FLOAT)yPos;
            //rectPos.bottom = yPos + rectPos.Height();
            //rectPos.top = yPos;
            //Rect = rectPos;
            //m_pDrawObj->SetLogicPos(Rect);
            m_ChangePos.ChangeType    = VALUE_TYPE_Y;
            m_ChangePos.rectChanged   = Rect;
            GetParent()->PostMessage(WM_ON_SIZE_POS_CHANGE2, (WPARAM)m_pDrawObj, (LPARAM)&m_ChangePos);
        }
    }
}

void CPropertyLayer::OnEnChangeEditWidth()
{
    if (m_bInChangeSize) return;

    if(m_pDrawObj)
    {
        //CDesignerPage* designerPage = (CDesignerPage*)(GetParent()->GetParent()->GetParent());
        //CheckInputValue(VALUE_CHECK_TYPE_W, -1.0f);

        CString sWidth;
		CString sHeight;
        GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText(sWidth);
		GetDlgItem(IDC_EDIT_HEIGHT)->GetWindowText(sHeight);
		//((CEdit *)(GetDlgItem(IDC_EDIT_WIDTH)))->SetSel(sWidth.GetLength(),sWidth.GetLength(),TRUE);
		//UpdateData(TRUE);

        FRect Rect;
        CRect rectPos;
        m_pDrawObj->GetLogicPos(Rect);
        Rect.ConvertToCRect(rectPos);

        int width = _tstoi(sWidth);
		int height = _tstoi(sHeight);
		BOOL keepAspectRatio = ((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->GetCheck();
		{
			if(keepAspectRatio)
			{
				float ratio;
				m_pDrawObj->GetAspectRatio(ratio);
				height = (int)((float)width *1.0 / ratio +  0.5);
				if(0 == height) 
				{
					height = 1;
				}
			}
		}

		if(width < 1 || width > 30000)
		{
            MessageBox(Translate(_T("Invalid value of the layer's width. Please input the value in the range: 1 - 30000!")), Translate(_T("Warning:Check value")), MB_OK|MB_ICONEXCLAMATION);
			UpdateData(FALSE);
			((CEdit *)(GetDlgItem(IDC_EDIT_WIDTH)))->SetFocus();
			return;
		}
		else if(height < 1 || height > 30000)
		{
            MessageBox(Translate(_T("Invalid value of the layer's height. Please input the value in the range: 1 - 30000!")), Translate(_T("Warning:Check value")), MB_OK|MB_ICONEXCLAMATION);
			UpdateData(FALSE);
			((CEdit *)(GetDlgItem(IDC_EDIT_WIDTH)))->SetFocus();
			return;
		}
		else
		{
			//sHeight.Format(_T(""), height);
			//GetDlgItem(IDC_EDIT_HEIGHT)->SetWindowText(sHeight);
			//UpdateData(TRUE);

			sWidth.Format(_T("%d"),width);
			sHeight.Format(_T("%d"),height);
			m_szWidth = sWidth;
			m_szHeight = sHeight;
			UpdateData(FALSE);
		}

        if (rectPos.Width() != width)
        {
            Rect.right = Rect.left + (FLOAT)width;
			Rect.bottom = Rect.top + (FLOAT)height;
            //Rect.right = Rect.left + width;
            //m_pDrawObj->SetLogicPos(Rect);
            m_ChangePos.ChangeType    = VALUE_TYPE_W;
            m_ChangePos.rectChanged   = Rect;
            GetParent()->PostMessage(WM_ON_SIZE_POS_CHANGE2, (WPARAM)m_pDrawObj, (LPARAM)&m_ChangePos);
        }

		if (rectPos.Height() != height)
		{
			Rect.right = Rect.left + (FLOAT)width;
			Rect.bottom = rectPos.top + (FLOAT)height;
			//Rect.bottom = Rect.top + height;
			//m_pDrawObj->SetLogicPos(Rect);
			m_ChangePos.ChangeType    = VALUE_TYPE_H;
			m_ChangePos.rectChanged   = Rect;
			GetParent()->PostMessage(WM_ON_SIZE_POS_CHANGE2, (WPARAM)m_pDrawObj, (LPARAM)&m_ChangePos);
		}
    }
}

void CPropertyLayer::OnEnChangeEditHeight()
{
    if (m_bInChangeSize) return;

    if(m_pDrawObj)
    {
        //CDesignerPage* designerPage = (CDesignerPage*)(GetParent()->GetParent()->GetParent());
        //CheckInputValue(VALUE_CHECK_TYPE_H, -1.0f);

        CString sWidth;
		CString sHeight;

        GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText(sWidth);
		GetDlgItem(IDC_EDIT_HEIGHT)->GetWindowText(sHeight);

		//((CEdit *)(GetDlgItem(IDC_EDIT_HEIGHT)))->SetSel(sHeight.GetLength(),sHeight.GetLength(),TRUE);
		//UpdateData(TRUE);


        FRect Rect;
        CRect rectPos;
        m_pDrawObj->GetLogicPos(Rect);
        Rect.ConvertToCRect(rectPos);

		int width = _tstoi(sWidth);
        int height = _tstoi(sHeight);
		BOOL keepAspectRatio = ((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->GetCheck();
		{
			if(keepAspectRatio)
			{
				float ratio;
				m_pDrawObj->GetAspectRatio(ratio);
				width = (int)((float)height * ratio + 0.5);
				if(0 == width ) 
				{
					width = 1;
				}
			}
		}

		if(height < 1 || height > 30000)
		{
            MessageBox(Translate(_T("Invalid value of the layer's height. Please input the value in the range: 1 - 30000!")), Translate(_T("Warning:Check value")), MB_OK|MB_ICONEXCLAMATION);
			UpdateData(FALSE);
			((CEdit *)(GetDlgItem(IDC_EDIT_HEIGHT)))->SetFocus();
			return;
		}
		else if(width < 1 || width > 30000)
		{
            MessageBox(Translate(_T("Invalid value of the layer's width. Please input the value in the range: 1 - 30000!")), Translate(_T("Warning:Check value")), MB_OK|MB_ICONEXCLAMATION);
			UpdateData(FALSE);
			((CEdit *)(GetDlgItem(IDC_EDIT_WIDTH)))->SetFocus();
			return;
		}
		else
        {
			//sWidth.Format(_T(""), width);
			//GetDlgItem(IDC_EDIT_WIDTH)->SetWindowText(sWidth);
			//UpdateData(TRUE);
			sWidth.Format(_T("%d"),width);
			sHeight.Format(_T("%d"),height);
			m_szWidth = sWidth;
			m_szHeight = sHeight;
			UpdateData(FALSE);
		}

		if (rectPos.Width() != width)
		{
			Rect.right = Rect.left + (FLOAT)width;
			Rect.bottom = Rect.top + (FLOAT)height;
			//Rect.right = Rect.left + width;
			//m_pDrawObj->SetLogicPos(Rect);
			m_ChangePos.ChangeType    = VALUE_TYPE_W;
			m_ChangePos.rectChanged   = Rect;
			GetParent()->PostMessage(WM_ON_SIZE_POS_CHANGE2, (WPARAM)m_pDrawObj, (LPARAM)&m_ChangePos);
		}

        if (rectPos.Height() != height)
        {
			Rect.right = Rect.left + (FLOAT)width;
            Rect.bottom = rectPos.top + (FLOAT)height;
            //Rect.bottom = Rect.top + height;
            //m_pDrawObj->SetLogicPos(Rect);
            m_ChangePos.ChangeType    = VALUE_TYPE_H;
            m_ChangePos.rectChanged   = Rect;
            GetParent()->PostMessage(WM_ON_SIZE_POS_CHANGE2, (WPARAM)m_pDrawObj, (LPARAM)&m_ChangePos);
        }
    }
}


void CPropertyLayer::OnLvnItemchangedMediaList(NMHDR *pNMHDR, LRESULT *pResult)
{
    //LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTBOX>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
    GetOwner()->PostMessage(WM_ON_MEDIA_SEL_CHANGE);
}

//
//Get Media that selected in MediaList.
//Add safe verify. -zxy
//
std::shared_ptr<MediaElement> CPropertyLayer::GetSelectedMedia()
{
    int SelIndex = m_MediaList.GetSelItem();
    if(!::IsWindow(m_MediaList.GetSafeHwnd()))
        return NULL;
    int nItemCount = m_MediaList.GetCount();
    if(SelIndex >= nItemCount)
        return NULL;
    if(SelIndex >=0)
    {
        //return ((*(std::shared_ptr<MediaElement> *)(m_MediaList.GetItemData(SelIndex))));
        std::shared_ptr<MediaElement> * pPtr = (std::shared_ptr<MediaElement> *)m_MediaList.GetItemData(SelIndex);
        if(!pPtr)
            return NULL;
        
        return *pPtr;
    }

    return NULL;
}

//
//zxy -check box
//
void CPropertyLayer::OnBnClickedCheckKeepAspact()
{
    if(m_pDrawObj)
    {
        //BOOL keepAspectRatio = ((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->GetCheck();
       // m_pDrawObj->SetKeepAspect(keepAspectRatio);
       // m_pDrawObj->CalcAspectRatio();
        GetParent()->SendMessage(WM_ON_LAYER_KEEP_ASPECT_CHANGE,(WPARAM)m_pDrawObj, 0);
    }
}

//
//check box
//
void CPropertyLayer::OnBnClickedCheckLoopSync()
{
    if (m_pDrawObj)
    {
       // BOOL bLoop = ((CButton *)GetDlgItem(IDC_CHECK_LOOPSYNC))->GetCheck();
       // m_pDrawObj->SetLoopSync(bLoop);
       // BOOL bCmd = TRUE;
        GetParent()->SendMessage(WM_ON_LAYER_LOOP_CHANGE, (WPARAM)m_pDrawObj, 0);

        //CalcLayerDuration();
    }
}

void CPropertyLayer::CheckInputValue(VALUE_CHECK_TYPE type, float ratio)
{
    static const ValueCheck_t kValueCheck[] = {
        {IDC_EDIT_X, -50000, 50000, ID_NONE, -50000, 50000, _T("x")},
        {IDC_EDIT_Y, -50000, 50000, ID_NONE, -50000, 50000, _T("y")},
        {IDC_EDIT_WIDTH, 1, 50000, IDC_EDIT_HEIGHT, 1, 50000, Translate(_T("Width"))},
        {IDC_EDIT_HEIGHT, 1, 50000, IDC_EDIT_WIDTH, 1, 50000, Translate(_T("Height"))},
    };

    m_bInChangeSize = true;
    CString sValue;
    int nValue;
    GetDlgItem(kValueCheck[type].controlID)->GetWindowText(sValue);
    if (sValue == _T(""))
    {
        sValue = _T("0");
    }
    nValue = _tstoi(sValue.GetString());
#if 0
    // check ratio first
    if (ratio > 0.000001f && kValueCheck[type].buddyControlID != -1)
    {
        CString sBuddyValue;
        int nBuddyValue;

        GetDlgItem(kValueCheck[type].buddyControlID)->GetWindowText(sBuddyValue);
        nBuddyValue = _tstoi(sBuddyValue.GetString());

        int nExpectBuddyValue = ((float)nValue * ratio) + 0.5;
        // too large to change buddy control's value
		CString str = Translate(_T("Value is invalid to keep the aspect ratio.")) + _T("\n") +
			Translate(_T("Uncheck")) + _T(" \"") + Translate(_T("Keep Aspect Ratio")) + _T(" \"") + Translate(_T("or enter a new value"));
        if (nExpectBuddyValue > kValueCheck[type].maxBuddyValue)
        {
            MessageBox(str, Translate(_T("Error")), MB_OK|MB_ICONERROR );
            nValue = (float)nBuddyValue / ratio;
            sValue.Format(_T("%d"), nValue);
            GetDlgItem(kValueCheck[type].controlID)->SetWindowText(sValue);
        }
        // too small to change buddy control's value
        else if (nExpectBuddyValue < kValueCheck[type].minBuddyValue)
        {
            MessageBox(str, Translate(_T("Error")), MB_OK|MB_ICONERROR );
            nValue = (float)nBuddyValue / ratio;
            sValue.Format(_T("%d"), nValue);
            GetDlgItem(kValueCheck[type].controlID)->SetWindowText(sValue);
        }
        // can change buddy control's value
        else
        {
            GetDlgItem(kValueCheck[type].buddyControlID)->SetWindowText(StringUtility::itos(nExpectBuddyValue).c_str());

            FRect Rect;
            m_pDrawObj->GetLogicPos(Rect);
            if (kValueCheck[type].buddyControlID == IDC_EDIT_HEIGHT)
            {
                Rect.InflateRect(0.0f, 0.0f, nValue, nExpectBuddyValue - Rect.Height());
                m_pDrawObj->SetLogicPos(Rect);
            }
            else
            {
                Rect.InflateRect(0.0f, 0.0f, nExpectBuddyValue - Rect.Width(), nValue);
                m_pDrawObj->SetLogicPos(Rect);
            }
        }
    }
#endif
    // check value
    if (nValue > kValueCheck[type].maxValue)
    {
		CString msg, strName, strMin, strMax;
		strName.Format(_T("%s"), kValueCheck[type].name);
		strMin.Format(_T("%d"), kValueCheck[type].minValue);
		strMax.Format(_T("%d"), kValueCheck[type].maxValue);
        // TRANS:
		msg = Translate(_T("Value of ")) + strName + Translate(_T(" is invalid.")) + _T("\n") +
			Translate(_T("Please input number between")) + _T(" ") + strMin + _T(" ") + Translate(_T("and")) + _T(" ") + strMax;

        MessageBox(msg,  Translate(_T("Warning:Check value")), MB_OK|MB_ICONEXCLAMATION);

        nValue = kValueCheck[type].maxValue;
        sValue.Format(_T("%d"), nValue);
        GetDlgItem(kValueCheck[type].controlID)->SetWindowText(sValue);

    }
    else if (nValue < kValueCheck[type].minValue)
    {
       CString msg, strName, strMin, strMax;
		strName.Format(_T("%s"), kValueCheck[type].name);
		strMin.Format(_T("%d"), kValueCheck[type].minValue);
		strMax.Format(_T("%d"), kValueCheck[type].maxValue);
		msg = Translate(_T("Value of ")) + strName + Translate(_T(" is invalid.")) + _T("\n") +
			Translate(_T("Please input number between")) + _T(" ") + strMin + _T(" ") + Translate(_T("and")) + _T(" ") + strMax;

        MessageBox(msg,  Translate(_T("Warning:Check value")), MB_OK|MB_ICONEXCLAMATION);

        nValue = kValueCheck[type].minValue;
        sValue.Format(_T("%d"), nValue);
        GetDlgItem(kValueCheck[type].controlID)->SetWindowText(sValue);
    }

    m_bInChangeSize = false;
}

BOOL CPropertyLayer::PreTranslateMessage(MSG* pMsg)
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
        GetParent()->SetFocus();
    }

#if 0
	if(GetFocus() == GetDlgItem(IDC_EDIT_X) ||GetFocus() == GetDlgItem(IDC_EDIT_Y) 
		||GetFocus() == GetDlgItem(IDC_EDIT_WIDTH) ||GetFocus() == GetDlgItem(IDC_EDIT_HEIGHT))
	{
		if(pMsg->message == WM_KEYDOWN)  
		{
    		if( isdigit(pMsg->wParam) || pMsg->wParam == VK_OEM_MINUS )
			{
				;
			}
			else if( pMsg->wParam >= 0x00 && pMsg->wParam< 0x30)
			{
				;
			}
			else if (pMsg->wParam >= VK_NUMPAD0 && pMsg->wParam <= VK_NUMPAD9 )
			{
				;
			}
			else if (pMsg->wParam == VK_SUBTRACT) // -
			{
				;
			}
			else
			{
				CWnd * curFocus = GetFocus();
				MessageBox(Translate(_T("Please input integer!")),Translate(_T("Warning:Check value")), MB_OK|MB_ICONEXCLAMATION);
				curFocus->SetFocus();
				return TRUE;
			}
		}
	}
#endif
    return __super::PreTranslateMessage(pMsg);
}

HWND CPropertyLayer::GetFocusableWindow(int nPoisition, LPVOID lParam, BOOL shift)
{
    CPropertyCtrl* pThis = (CPropertyCtrl*)lParam;
    switch (nPoisition)
    {
    case FOCUSABLEWINDOW_POSITION_FIRST:
        return pThis->GetDlgItem(IDC_EDIT_X)->GetSafeHwnd();
    case FOCUSABLEWINDOW_POSITION_LAST:
        return ((CVSListBox*)(pThis->GetDlgItem(IDC_MFCVSLISTBOX_MEDIA)))->m_pWndList->GetSafeHwnd();
    case FOCUSABLEWINDOW_POSITION_FOCUSABLE:
        {
            CPropertyCtrl* pParent = (CPropertyCtrl*)(pThis->GetParent());
            return pParent->GetNextFocusWindow(pThis->GetSafeHwnd(), shift);
        }
    }
    return NULL;
}

void CPropertyLayer::CalcLayerDuration()
{
    if (m_layerInfo)
    {
        m_Duration = 0;
        CString szDuration;
		m_Duration = m_layerInfo->GetMediasTotalDuraiton(TRUE);


/*
        POSITION pos = m_pLayer->GetHeadPosition();
        while (pos != NULL)
        {
            MEDIASETTING* pMedia = m_pLayer->GetNext(pos);
            if (pMedia->MediaFile.GetLength() == 0)
            {
                continue;
            }


            m_Duration += pMedia->Duration;
        }*/

        BOOL bLoop = ((CButton *)GetDlgItem(IDC_CHECK_LOOPSYNC))->GetCheck();
        if (bLoop)
        {
            GetParent()->SendMessage(WM_ON_LAYER_LOOP_CHANGE, (WPARAM)m_pDrawObj, 0);

            m_Duration = m_LayoutDuration;
        }

        int h, m, s;
        h = m_Duration / 3600;
        m = (m_Duration - h * 3600) / 60;
        s = m_Duration % 60;
        szDuration.Format(_T("%02d:%02d:%02d"), h, m, s);
        GetDlgItem(IDC_EDIT_LAYERDURATION)->SetWindowText(szDuration);
    }
}

LRESULT CPropertyLayer::OnMediaNumChange(WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0L;

    GetParent()->SendMessage(WM_ON_LAYER_LOOP_CHANGE, (WPARAM)m_pDrawObj, 0);

    CalcLayerDuration();

    return lr;
}

LRESULT CPropertyLayer::OnPlugSizeChange(WPARAM wParam, LPARAM lParam)
{
	LRESULT lr = 0L;
	CString height,width;
	PRO_CONTENTINFO content = (PRO_CONTENTINFO)wParam;
	height.Format(_T("%d"),content->height);
	width.Format(_T("%d"),content->width);
	m_szHeight = height;
	m_szWidth = width;
	UpdateData(FALSE);

	return lr;
}
void CPropertyLayer::ChangeLayerDuration()
{
    CalcLayerDuration();
}


