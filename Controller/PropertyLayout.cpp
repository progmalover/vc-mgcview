// PropertyLayout.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "PropertyLayout.h"
#include "afxdialogex.h"
#include "DialogChooseClientSize.h"
#include "GroupEditor.h"
#include "LayoutDesignerCtrl.h"
#include "DesignerPage.h"
#include "PropertyCtrl.h"
#include "Localization/Localization.h"
#include "CommonUI/BaseEdit.h"

extern UINT WM_PROPERTY_SETTING_CHANGE	;
UINT WM_ON_LAYOUT_SIZE_CHANGE = ::RegisterWindowMessage(_T("PROPERTY_LAYOUT_SIZE_CHANGE"));

UINT WM_ON_GROUP_CHANGE = ::RegisterWindowMessage(_T("PROPERTY_GROUP_CHANGE"));

// wParam - point of CS3DrawObj, lParam - no used
UINT WM_ON_LAYER_SEL_CHANGE = ::RegisterWindowMessage(_T("PROPERTY_LAYER_SEL_CHANGE"));
UINT WM_SET_LAYOUT_COLOR    = ::RegisterWindowMessage(_T("WM_SET_LAYOUT_COLOR"));
UINT WM_LAYOUT_TEXT_CHANGE  = ::RegisterWindowMessage(_T("WM_LAYOUT_TEXT_CHANGE"));
// CPropertyLayout dialog

#define EFFECT_LAYOUT_EDITOR
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

extern CLayoutDesignerCtrl *GetCurrentDesignerCtrl();
BOOL SetCurrentLayoutSize( CSize szLayout, std::vector<CRect> *pMonitorRc = NULL, FLOAT fRotateDegree = 0.0f, BOOL bNotify = TRUE)
{
     CLayoutDesignerCtrl *pCtrl = GetCurrentDesignerCtrl();
  //  ASSERT(pCtrl);
    if(!pCtrl)
    {
        TRACE(_T("SetCurrentLayoutSize:get \"CLayoutDesignerCtrl\" instance error!\n"));
        return FALSE;
    }
    LayoutWidget *pWidget = (LayoutWidget *)pCtrl->GetWidget( LAYOUT_Z_0 );
    ASSERT(pWidget);
    if(!pWidget)
    {
        TRACE(_T("LayoutDesigner Init Widget:LAYOUT_Z_0 error!\n"));
        return FALSE;
    }
    if( 0 >= szLayout.cx || 0>= szLayout.cy)
        return FALSE;
    pWidget->SetClientLayout(szLayout,pMonitorRc, bNotify,fRotateDegree);
    return TRUE;
}

CSize GetCurrentLayoutSize()
{
    CLayoutDesignerCtrl *pCtrl = GetCurrentDesignerCtrl();
    ASSERT(pCtrl);
    if(!pCtrl)
        return FALSE;
    return pCtrl->GetLayoutSize();
}
IMPLEMENT_DYNAMIC(CPropertyLayout, CDialogEx)

CPropertyLayout::CPropertyLayout(CWnd* pParent /*=NULL*/)
    : CDialogEx(CPropertyLayout::IDD, pParent)
{
    m_layoutInfo = NULL;
    m_bInChangeSize = false;
    m_fAspectRatio = 1.0f;
    m_bOpenClient = false;
}

CPropertyLayout::~CPropertyLayout()
{
    m_LayerList.RemoveAll();
}

void CPropertyLayout::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_SIZE, m_wndPresetSize);
    DDX_Control(pDX, IDC_LIST_LAYERS, m_wndLayerList);
}


BEGIN_MESSAGE_MAP(CPropertyLayout, CDialogEx)
    ON_WM_SIZE()
    ON_EN_CHANGE(IDC_EDIT_NAME, &CPropertyLayout::OnEnChangeEditName)
    ON_CBN_SELCHANGE(IDC_COMBO_GROUP, &CPropertyLayout::OnCbnSelchangeComboGroup)
    ON_EN_CHANGE(IDC_EDIT_LDESC, &CPropertyLayout::OnEnChangeEditLdesc)
    ON_BN_CLICKED(IDC_BUTTON_CHOOSE_CLIENT, &CPropertyLayout::OnBnClickedButtonChooseClient)
    ON_EN_CHANGE(IDC_EDIT_WIDTH, &CPropertyLayout::OnEnChangeEditWidth)
    ON_EN_CHANGE(IDC_EDIT_HEIGHT, &CPropertyLayout::OnEnChangeEditHeight)
    ON_BN_CLICKED(IDC_CHECK_KEEP_ASPACT, &CPropertyLayout::OnBnClickedCheckKeepAspact)
    ON_CBN_SELCHANGE(IDC_COMBO_SIZE, &CPropertyLayout::OnCbnSelchangeComboSize)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_MFCCOLORBUTTON_BG, &CPropertyLayout::OnBnClickedMfccolorbuttonBg)
    ON_UPDATE_COMMAND_UI(IDC_MFCCOLORBUTTON_BG,&CPropertyLayout::OnUpdateBGBtnCommandUI)
    ON_REGISTERED_MESSAGE(WM_ON_GROUP_CHANGE, &CPropertyLayout::OnGroupChange)
    ON_BN_CLICKED(IDC_EDIT_GROUP, &CPropertyLayout::OnBnClickedEditGroup)
    ON_LBN_SELCHANGE(IDC_LIST_LAYERS, &CPropertyLayout::OnLbnSelchangeListLayers)
    ON_MESSAGE(WM_IDLEUPDATECMDUI,OnIdleUpdateUI)
    
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CPropertyLayout)
    EASYSIZE(IDC_EDIT_LAYOUT_DURATION,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_BASIC,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_EDIT_NAME,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_COMBO_GROUP,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_EDIT_GROUP,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_EDIT_LDESC,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_BUTTON_CHOOSE_CLIENT,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_HCENTER)
    EASYSIZE(IDC_COMBO_SIZE,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    //EASYSIZE(IDC_STATIC_MULTIPLY,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_HCENTER)

	EASYSIZE(IDC_X_LABEL,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_Y_LABEL,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)

	EASYSIZE(IDC_EDIT_WIDTH,IDC_X_LABEL,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_EDIT_HEIGHT,IDC_Y_LABEL,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)

	EASYSIZE(IDC_STATIC_X,IDC_EDIT_WIDTH,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_Y,IDC_EDIT_HEIGHT,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)

    //EASYSIZE(IDC_STATIC_X,ES_BORDER,ES_BORDER,IDC_STATIC_MULTIPLY,ES_KEEPSIZE,0)
	//EASYSIZE(IDC_STATIC_X,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	//EASYSIZE(IDC_STATIC_Y,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    //EASYSIZE(IDC_EDIT_HEIGHT,IDC_STATIC_MULTIPLY,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	//EASYSIZE(IDC_EDIT_HEIGHT,ES_KEEPSIZE,ES_BORDER,IDC_STATIC_Y,ES_KEEPSIZE,0)
    //EASYSIZE(IDC_STATIC_Y,IDC_EDIT_HEIGHT,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_CHECK_KEEP_ASPACT,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_HCENTER)
    EASYSIZE(IDC_MFCCOLORBUTTON_BG,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_BG_AUDIO,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_LIST_LAYERS,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_STATIC_SIZECOLOR,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)

END_EASYSIZE_MAP

//should be larger than 18
static SIZE kPresetSize[30] = 
{
	{1366, 768},
    {1920, 1080}, 
	{2304, 1366},
	{2732, 1536},
	{2732, 2304},
	{3072, 1366},
	{3072, 2732},
	{3240, 1920}, 
    {3840, 2160}, 
	{3840, 3240},
	{4098, 2304},
	{4098, 3072},
	{4320, 1920},
	{4320, 3840}, 
	{5464, 3072},
    {5760, 3240}, 
    {5760, 4320}, 
    {7680, 4320}, 
};

static UINT g_PresetCnt = 18;

LRESULT CPropertyLayout::OnIdleUpdateUI(WPARAM ,LPARAM)
{
    this->UpdateDialogControls( this ,TRUE);
   
    return S_OK;
}
// CPropertyLayout message handlers
BOOL CPropertyLayout::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    INIT_EASYSIZE;

    CRect rect;
    m_wndLayerList.GetClientRect(rect);
    m_wndLayerList.SetColumnWidth(rect.Width());

    CString str;           
#ifdef STARTER_EDITION

    DEVMODE DevMode;
    INT i=-1;

    std::map<CString, INT> DevPool;

    g_PresetCnt = 0;

    BOOL bResult=EnumDisplaySettings(NULL,i,&DevMode); 
    while(bResult)  
    {  
        str.Format(TEXT("%d*%d"),DevMode.dmPelsWidth,
            DevMode.dmPelsHeight);

        std::map<CString, INT>::iterator itFind = DevPool.find(str);
        if (itFind == DevPool.end())
        {       
            DevPool[str] = g_PresetCnt;

            kPresetSize[g_PresetCnt].cx = DevMode.dmPelsWidth;
            kPresetSize[g_PresetCnt].cy = DevMode.dmPelsHeight;
            
            g_PresetCnt++;

            //m_wndPresetSize.AddString(str.GetString());
        }

        bResult=EnumDisplaySettings(NULL,++i,&DevMode);  
    } 

	SIZE temp;
	for(int jj = 0;jj < g_PresetCnt;jj++)
	{
		for(int kk = jj;kk < g_PresetCnt;kk++)
		{
			if(kPresetSize[kk].cx < kPresetSize[jj].cx)
			{
				temp.cx = kPresetSize[kk].cx;
				temp.cy = kPresetSize[kk].cy;
				kPresetSize[kk].cx = kPresetSize[jj].cx;
				kPresetSize[kk].cy = kPresetSize[jj].cy;
				kPresetSize[jj].cx = temp.cx;
				kPresetSize[jj].cy = temp.cy;
			}
			else if(kPresetSize[kk].cx == kPresetSize[jj].cx)
			{
				if(kPresetSize[kk].cy < kPresetSize[jj].cy)
				{
					temp.cx = kPresetSize[kk].cx;
					temp.cy = kPresetSize[kk].cy;
					kPresetSize[kk].cx = kPresetSize[jj].cx;
					kPresetSize[kk].cy = kPresetSize[jj].cy;
					kPresetSize[jj].cx = temp.cx;
					kPresetSize[jj].cy = temp.cy;
				}
			}
		}
		str.Format(TEXT("%d*%d"),kPresetSize[jj].cx,kPresetSize[jj].cy);
		m_wndPresetSize.AddString(str.GetString());
	}
#else
    for (int i = 0; i < g_PresetCnt/*sizeof(kPresetSize)/sizeof(*kPresetSize)*/; ++i)
    {
        str.Format(_T("%d x %d"), kPresetSize[i].cx, kPresetSize[i].cy);
        m_wndPresetSize.AddString(str.GetString());
    }

#endif
    

	m_wndPresetSize.AddString(Translate(_T("custom size...")));
#if !defined(STARTER_EDITION) && !defined(STANDALONE_EDITION)
	m_wndPresetSize.AddString(Translate(_T("Choose client size...")));
#endif
    m_wndPresetSize.SetCurSel(0);

    GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(FALSE);

    GetDlgItem(IDC_COMBO_GROUP)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_GROUP)->EnableWindow(FALSE);

    CFont font;
    LOGFONT lf;
    CWnd* content = (CWnd*)GetDlgItem(IDC_STATIC_LAYOUT);
    content->GetFont()->GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;
    font.CreateFontIndirect(&lf);
    content->SetFont(&font);
    font.Detach();

	m_layoutName.SubclassDlgItem (IDC_EDIT_NAME, this);
	m_layoutName.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_layoutName.SetRegEx(_T("[^\\\\/:*?\"<>|]*"));

	m_EditWidth.SubclassDlgItem (IDC_EDIT_WIDTH, this);
	m_EditWidth.SetRange(1, 50000);
	m_EditWidth.LimitText(5);
	m_EditWidth.SetWavyLineColour (RGB (255, 255, 255));
	m_EditWidth.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_EditWidth.SetForegroundColourError (RGB (255, 255, 255));
	m_EditWidth.SetBackgroundColourError (RGB (255, 0, 0));

	m_EditHeight.SubclassDlgItem (IDC_EDIT_HEIGHT, this);
	m_EditHeight.SetRange(1, 50000);
	m_EditHeight.LimitText(5);
	m_EditHeight.SetWavyLineColour (RGB (255, 255, 255));
	m_EditHeight.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_EditHeight.SetForegroundColourError (RGB (255, 255, 255));
	m_EditHeight.SetBackgroundColourError (RGB (255, 0, 0));
   
#ifdef EFFECT_LAYOUT_EDITOR
    ((CMFCColorButton *)GetDlgItem(IDC_MFCCOLORBUTTON_BG))->SetColor(RGB(255,255,255));
#endif 

	Translate(this);

#ifdef STARTER_EDITION
    GetDlgItem(IDC_BUTTON_CHOOSE_CLIENT)->ShowWindow(SW_HIDE);
#endif
    return FALSE;  // return TRUE  unless you set the focus to a control
}


void CPropertyLayout::OnUpdateBGBtnCommandUI(CCmdUI *pCmd)
{
#ifdef EFFECT_LAYOUT_EDITOR
    COLORREF curClr =  ((CMFCColorButton *)GetDlgItem(IDC_MFCCOLORBUTTON_BG))->GetColor();
    CLayoutDesignerCtrl *pCtrl = GetCurrentDesignerCtrl();
    if(!pCtrl)
        return;
    if(curClr != pCtrl->GetLayoutColor())
        pCtrl->SetLayoutColor(curClr);
  /*  if(this->m_pLayout)
    {
        m_pLayout->BGColor = curClr;
    }*/

#endif
}

void CPropertyLayout::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);
    UPDATE_EASYSIZE;
}

BOOL CPropertyLayout::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Add your specialized code here and/or call the base class

    CDialogEx::PreCreateWindow(cs);

    cs.style |= WS_CLIPCHILDREN;
    return TRUE;
}

void CPropertyLayout::OnEnChangeEditName()
{
    if(m_layoutInfo)
    {
        CString str;
        GetDlgItem(IDC_EDIT_NAME)->GetWindowText(str);
        str.Trim();
        if (str.IsEmpty())
        {
            MessageBox(Translate(_T("New Name should not Null!")), Translate(_T("Tips:Rename layout")), MB_OK|MB_ICONERROR);
            return;
        }

        GetParent()->SendMessage(WM_LAYOUT_TEXT_CHANGE,1,0); //1:
       
    }
}


void CPropertyLayout::OnCbnSelchangeComboGroup()
{
    int i = ((CComboBox*)GetDlgItem(IDC_COMBO_GROUP))->GetCurSel();
    CString str;
    ((CComboBox*)GetDlgItem(IDC_COMBO_GROUP))->GetLBText(i, str);

    for (int i = 0; i < m_pLayoutGroup->GetCount(); ++i)
    {
        POSITION pos = m_pLayoutGroup->FindIndex(i);
        LAYOUTGROUP_SETTING* group = m_pLayoutGroup->GetAt(pos);
        if (group->GroupName == str)
        {
            m_layoutInfo->SetGroupID(group->GroupID);
        }
    }
}


void CPropertyLayout::OnEnChangeEditLdesc()
{
    if(m_layoutInfo)
    {
        //GetDlgItem(IDC_EDIT_LDESC)->GetWindowText(m_layoutInfo->GetDescription());
		 GetParent()->SendMessage(WM_LAYOUT_TEXT_CHANGE,2,0);//2:special for description.
    }
}


void CPropertyLayout::OnBnClickedButtonChooseClient()
{
    CDialogChooseClientSize dlg;
    if (dlg.DoModal() == IDOK && m_layoutInfo)
    {
        m_bOpenClient = TRUE;

        CSize szClient(dlg.m_rectSelectedLayout.Width(),dlg.m_rectSelectedLayout.Height());
        FLOAT fRotateDegree = dlg.m_rectSelectedLayout.m_fRotateDegree; 

        CRect rctScreen = GetRotatedScreen(szClient.cx,szClient.cy, fRotateDegree);
        FLOAT xTrans = (rctScreen.Width() - szClient.cx) / 2.0;
        FLOAT yTrans = (rctScreen.Height() - szClient.cy) / 2.0;

        CSize szRotatedLayout(rctScreen.Width(), rctScreen.Height());

        m_layoutInfo->SetLayoutSize(szRotatedLayout);
        m_layoutInfo->SetLayoutRotateDegree(fRotateDegree);
        SetContent(m_layoutInfo, m_pLayoutGroup);
      
        SetCurrentLayoutSize(szRotatedLayout,
            &dlg.m_rectSelectedLayout.m_MonitorRects, fRotateDegree);
        return;
    }
}


void CPropertyLayout::OnEnChangeEditWidth()
{
    if (m_bInChangeSize)
        return;

    if(m_layoutInfo)
    {
        // first check value and keep aspect ratio
        if (m_layoutInfo->GetbKeepAspect())
        {
            CheckInputValue(VALUE_CHECK_TYPE_W, 1/m_fAspectRatio);
        }
        else
        {
            CheckInputValue(VALUE_CHECK_TYPE_W, -1.0f);
        }

		CString sWidth;
		CString sHeight;
		GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText(sWidth);
		int width = _tstoi(sWidth);
		GetDlgItem(IDC_EDIT_HEIGHT)->GetWindowText(sHeight);
		int height = _tstoi(sHeight);

        if(!GetDlgItem(IDC_EDIT_WIDTH)->IsWindowEnabled()||
            !GetDlgItem(IDC_EDIT_HEIGHT)->IsWindowEnabled())
            return;
        if( m_wndPresetSize.GetCurSel() != 18) //custom size...
            return;
		if (m_layoutInfo->GetLayoutSize().cx != _tstoi(sWidth)  || m_layoutInfo->GetLayoutSize().cy != _tstoi(sHeight) )
		{
			m_layoutInfo->SetLayoutSize(CSize(_tstoi(sWidth), _tstoi(sHeight)));
            SetCurrentLayoutSize(m_layoutInfo->GetLayoutSize());
			//GetParent()->PostMessage(WM_ON_LAYOUT_SIZE_CHANGE, 0, 0);
		}
    }
}


void CPropertyLayout::OnEnChangeEditHeight()
{
    if (m_bInChangeSize)
        return;

    if(m_layoutInfo)
    {
        // first check value and keep aspect ratio
        if (m_layoutInfo->GetbKeepAspect())
        {
            CheckInputValue(VALUE_CHECK_TYPE_H, m_fAspectRatio);
        }
        else
        {
            CheckInputValue(VALUE_CHECK_TYPE_H, -1.0f);
        }
		
		CString sWidth;
		CString sHeight;
		GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText(sWidth);
		int width = _tstoi(sWidth);
		GetDlgItem(IDC_EDIT_HEIGHT)->GetWindowText(sHeight);
		int height = _tstoi(sHeight);

        if(!GetDlgItem(IDC_EDIT_WIDTH)->IsWindowEnabled()||
           !GetDlgItem(IDC_EDIT_HEIGHT)->IsWindowEnabled())
           return;

        if( m_wndPresetSize.GetCurSel() != 18) //custom size...
            return;
		if (m_layoutInfo->GetLayoutSize().cx != _tstoi(sWidth)  || m_layoutInfo->GetLayoutSize().cy != _tstoi(sHeight) )
		{
			m_layoutInfo->SetLayoutSize(CSize(_tstoi(sWidth), _tstoi(sHeight)));

            SetCurrentLayoutSize(m_layoutInfo->GetLayoutSize());
			//GetParent()->PostMessage(WM_ON_LAYOUT_SIZE_CHANGE, 0, 0);
		}
    }
}

void CPropertyLayout::OnBnClickedCheckKeepAspact()
{
    if(m_layoutInfo)
    {
        m_layoutInfo->SetbKeepAspect(((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->GetCheck());
        m_fAspectRatio = (float)m_layoutInfo->GetLayoutSize().cx / (float)m_layoutInfo->GetLayoutSize().cy;
    }
}

void CPropertyLayout::OnCbnSelchangeComboSize()
{
    UpdateData(FALSE);
    //((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->SetCheck(FALSE);

    int num = m_wndPresetSize.GetCurSel();

    INT ScreenModeCnt = g_PresetCnt;//sizeof(kPresetSize)/sizeof(*kPresetSize);
#ifdef STARTER_EDITION
    ScreenModeCnt = g_PresetCnt;
#endif

	if(num == ScreenModeCnt)
	{
		if(m_layoutInfo)
		{
			((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->SetCheck(m_layoutInfo->GetbKeepAspect());
			//m_pLayout->bKeepAspect = false;
			((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->EnableWindow(true);
			m_fAspectRatio = (float)m_layoutInfo->GetLayoutSize().cx / (float)m_layoutInfo->GetLayoutSize().cy;
		}

		GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(TRUE);
        m_bOpenClient = FALSE;
        CSize sz = GetCurrentLayoutSize();
        SetCurrentLayoutSize(sz,
                 NULL);
	}
	else if(num == ScreenModeCnt +1)
	{
		GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(FALSE);

		if(m_layoutInfo)
		{
			((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->SetCheck(false);
			m_layoutInfo->SetbKeepAspect(false);
			((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->EnableWindow(false);
			m_fAspectRatio = (float)m_layoutInfo->GetLayoutSize().cx / (float)m_layoutInfo->GetLayoutSize().cy;
		}

        /*CDialogChooseClientSize dlg;
        if (dlg.DoModal() == IDOK && m_pLayout)
        {
        m_pLayout->LayoutSize.cx = dlg.m_rectSelectedLayout.Width();
        m_pLayout->LayoutSize.cy = dlg.m_rectSelectedLayout.Height();
        SetContent(m_pLayout, m_pLayoutGroup);
        GetParent()->PostMessage(WM_ON_LAYOUT_SIZE_CHANGE, 0, 0);
        return;
        }*/
        OnBnClickedButtonChooseClient();
	}
	else
	{
		if(m_layoutInfo)
		{
			((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->SetCheck(false);
			m_layoutInfo->SetbKeepAspect(false);
			((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->EnableWindow(false);
			m_fAspectRatio = (float)m_layoutInfo->GetLayoutSize().cx / (float)m_layoutInfo->GetLayoutSize().cy;
		}

		GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(FALSE);

		m_bInChangeSize = true;
		GetDlgItem(IDC_EDIT_HEIGHT)->SetWindowText(StringUtility::itos(kPresetSize[num].cy).c_str());
		GetDlgItem(IDC_EDIT_WIDTH)->SetWindowText(StringUtility::itos(kPresetSize[num].cx).c_str());

		if (m_layoutInfo != NULL)
		{
			if (m_layoutInfo->GetLayoutSize() != kPresetSize[num])
			{
				m_layoutInfo->SetLayoutSize(kPresetSize[num]);
                m_bOpenClient = FALSE;
                SetCurrentLayoutSize(m_layoutInfo->GetLayoutSize());
				//GetParent()->PostMessage(WM_ON_LAYOUT_SIZE_CHANGE, 0, 0);
			}
		}

		m_bInChangeSize = false;
	}
}


void CPropertyLayout::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: Add your message handler code here and/or call default

    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CPropertyLayout::OnBnClickedMfccolorbuttonBg()
{
    if(m_layoutInfo)
    {
         //m_pLayout->BGColor = ((CMFCColorButton *)GetDlgItem(IDC_MFCCOLORBUTTON_BG))->GetColor();
         GetParent()->SendMessage(WM_SET_LAYOUT_COLOR,0,0);
    }
}


VOID CPropertyLayout::SetContent(LayoutElement* layoutInfo, LAYOUTGROUP_LIST* pLayoutGroup)
{
	//m_pLayout = pLayout;
	m_layoutInfo = layoutInfo;
    m_pLayoutGroup = pLayoutGroup;
    if(layoutInfo && pLayoutGroup)
    {
        GetDlgItem(IDC_EDIT_NAME)->SetWindowText(m_layoutInfo->GetLayoutName());
        GetDlgItem(IDC_EDIT_LDESC)->SetWindowText(m_layoutInfo->GetDescription());

        ((CComboBox*)GetDlgItem(IDC_COMBO_GROUP))->ResetContent();

        int selectedGroupIndex = 0; 
        for (int i = 0; i < m_pLayoutGroup->GetCount(); ++i)
        {
            LAYOUTGROUP_SETTING* layoutSetting = m_pLayoutGroup->GetAt(m_pLayoutGroup->FindIndex(i));
            ((CComboBox*)GetDlgItem(IDC_COMBO_GROUP))->AddString(layoutSetting->GroupName);
            if (m_layoutInfo->GetGroupID() == layoutSetting->GroupID)
            {
                selectedGroupIndex = i;
            }
        }
        ((CComboBox*)GetDlgItem(IDC_COMBO_GROUP))->SetCurSel(selectedGroupIndex);
 
		//((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->SetCheck(m_pLayout->bKeepAspect);

        CString sWidth, sHeight;
		CSize layoutsize = m_layoutInfo->GetLayoutSize();
        m_fAspectRatio = (float)layoutsize.cx / (float)layoutsize.cy;
        sWidth.Format(_T("%d"), layoutsize.cx);
        sHeight.Format(_T("%d"), layoutsize.cy);

        GetDlgItem(IDC_EDIT_WIDTH)->SetWindowText(sWidth);
        GetDlgItem(IDC_EDIT_HEIGHT)->SetWindowText(sHeight);

		int ii;
		int num = g_PresetCnt;//sizeof(kPresetSize)/sizeof(*kPresetSize);
#ifdef STARTER_EDITION
		num = g_PresetCnt;
#endif
		for (ii = 0; ii < num; ++ii)
		{
			if(layoutsize.cx == kPresetSize[ii].cx &&  layoutsize.cy == kPresetSize[ii].cy)
			{
				break;
			}	
		}
        if(!m_bOpenClient)
		    m_wndPresetSize.SetCurSel(ii);
        
		if(ii == num )
		{
			GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(TRUE);
			//if(m_pLayout)
			{
				((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->SetCheck(m_layoutInfo->GetbKeepAspect());
				//m_pLayout->bKeepAspect = false;
				((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->EnableWindow(true);
				m_fAspectRatio = (float)layoutsize.cx / (float)layoutsize.cy;
			}
		}
		else if(ii == num+1 )
		{
			GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(FALSE);
			//if(m_pLayout)
			{
				((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->SetCheck(false);
				m_layoutInfo->SetbKeepAspect(FALSE);
				((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->EnableWindow(false);
				m_fAspectRatio = (float)layoutsize.cx / (float)layoutsize.cy;
			}
		}
		else
		{
			GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(FALSE);
			//if(m_pLayout)
			{
				((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->SetCheck(false);
				m_layoutInfo->SetbKeepAspect(FALSE);
				((CButton *)GetDlgItem(IDC_CHECK_KEEP_ASPACT))->EnableWindow(false);
				m_fAspectRatio = (float)layoutsize.cx / (float)layoutsize.cy;
			}
		}

        ((CMFCColorButton *)GetDlgItem(IDC_MFCCOLORBUTTON_BG))->SetColor(m_layoutInfo->GetBGColor());

        GetDlgItem(IDC_EDIT_WIDTH)->SetWindowText(sWidth);

        if(m_bOpenClient)
        {
            GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(FALSE);
        }
        // set layer list
        m_wndLayerList.ResetContent();

        CString szTypeName;
        CString szSerialName;
        int nCount = 0;
        int i = 0;
		m_layoutInfo->SetLayoutDuration(0);

		POSITION posMedia;
		PROPERTY_TYPE propertyType;
		DWORD layerDuration = 0;
		
		POSITION pos = m_LayerList.GetTailPosition();
		while (pos != NULL)
        {
            CS3DrawObj* pObj = m_LayerList.GetPrev(pos);
            pObj->GetName(szTypeName);
            pObj->GetSerialName(szSerialName);

            pObj->GetTypeCount(nCount);
			pObj->GetPropertyType(propertyType);

			if (propertyType != BACKGROUND)
			{
				layerDuration = 0;
				BOOL bFound = FALSE;
				std::shared_ptr<LayerElement> layerInfo;
				layerInfo = m_layoutInfo->GetLayerInfo(pObj->GetLayerId(), bFound);
				if (bFound && layerInfo.get() != NULL)
				{
					layerDuration = layerInfo->GetMediasTotalDuraiton();
/*
					list<std::shared_ptr<MediaElement>> medias = layerInfo->GetMedias();
					list<std::shared_ptr<MediaElement>>::iterator it;
					for (it = medias.begin(); it != medias.end(); it++)
					{
						layerDuration += (*it)->GetDuration();
					}*/

				}

				if (m_layoutInfo->GetLayoutDuration() < layerDuration)
				{
					m_layoutInfo->SetLayoutDuration(layerDuration);
				}
			}
			
			/*if (szTypeName.CompareNoCase(szTypeName_Audio) != 0)
            {
                szTypeName.Format(_T("%s%d"), szTypeName.GetString(), nCount);
            }*/
            if(szSerialName.IsEmpty())
            {
                szSerialName.Format(_T("%s%d"), szTypeName.GetString(), nCount);
            }
            szSerialName.AppendFormat(_T(" (%s)"),szTypeName);
            m_wndLayerList.InsertString(i, szSerialName); // serialname+(type) -zxy
            m_wndLayerList.SetItemDataPtr(i, pObj);
            ++i;
        }

        int h, m, s;
        CString szDuration;

		DWORD dwLayoutDuration = m_layoutInfo->GetLayoutDuration();

        h = dwLayoutDuration / 3600;
        m = (dwLayoutDuration - h * 3600) / 60;
        s = dwLayoutDuration % 60;
        szDuration.Format(_T("%02d:%02d:%02d"), h, m, s);
        GetDlgItem(IDC_EDIT_LAYOUT_DURATION)->SetWindowText(szDuration);

    }
}

void CPropertyLayout::Clear()
{
    m_LayerList.RemoveAll();
    //m_pLayoutGroup should not remove, it is not belong to this ctrl
    m_layoutInfo = NULL;
    m_fAspectRatio = 1.0f;
}

void CPropertyLayout::CheckInputValue(VALUE_CHECK_TYPE type, float ratio)
{
    const ValueCheck_t kValueCheck[] = {
        {IDC_EDIT_WIDTH, 1, 50000, IDC_EDIT_HEIGHT, 1, 50000, Translate(_T("Width"))},
        {IDC_EDIT_HEIGHT, 1, 50000, IDC_EDIT_WIDTH, 1, 50000, Translate(_T("Height"))},
    };

    m_bInChangeSize = true;
    CString sValue;
    int nValue = 0;
    GetDlgItem(kValueCheck[type].controlID)->GetWindowText(sValue);
    if (sValue == _T(""))
    {
        sValue = _T("0");
    }

    nValue = _tstoi(sValue.GetString());

    // check ratio first
    if (ratio > 0.000001f && kValueCheck[type].buddyControlID != -1)
    {
        CString sBuddyValue;
        int nBuddyValue;

        GetDlgItem(kValueCheck[type].buddyControlID)->GetWindowText(sBuddyValue);
        nBuddyValue = _tstoi(sBuddyValue.GetString());

        int nExpectBuddyValue = (int)(((float)nValue * ratio) + 0.5);
        // TRANS:
        // too large to change buddy control's value
		CString str = Translate(_T("Value is invalid to keep the aspect ratio.")) + _T("\n") +
				Translate(_T("Uncheck")) +_T(" \"") + Translate(_T("Keep Aspect Ratio")) + _T(" \"") + Translate(_T("or enter a new value"));
        if (nExpectBuddyValue > kValueCheck[type].maxBuddyValue)
        {
            MessageBox(str, Translate(_T("Warning:Check value")), MB_OK|MB_ICONEXCLAMATION );
            nValue = (int)((float)nBuddyValue / ratio);
            sValue.Format(_T("%d"), nValue);
            GetDlgItem(kValueCheck[type].controlID)->SetWindowText(sValue);
        }
        // too small to change buddy control's value
        else if (nExpectBuddyValue < kValueCheck[type].minBuddyValue)
        {
            MessageBox(str, Translate(_T("Warning:Check value")), MB_OK|MB_ICONEXCLAMATION );
            nValue = (int)((float)nBuddyValue / ratio);
            sValue.Format(_T("%d"), nValue);
            GetDlgItem(kValueCheck[type].controlID)->SetWindowText(sValue);
        }
        // can change buddy control's value
        else
        {
            GetDlgItem(kValueCheck[type].buddyControlID)->SetWindowText(StringUtility::itos(nExpectBuddyValue).c_str());
        }
    }

    // check value
    if (nValue > kValueCheck[type].maxValue)
    {
        CString msg, strName, strMin, strMax, strValue;
        strName.Format(_T("%s"), kValueCheck[type].name);
        strMin.Format(_T("%d"), kValueCheck[type].minValue);
        strMax.Format(_T("%d"), kValueCheck[type].maxValue);
        strValue.Format(_T("%d"), nValue);
        msg = Translate(_T("Value of ")) + strName + Translate(_T(" is invalid.")) + _T("\n") +
            Translate(_T("Please input number between")) + _T(" ") + strMin + _T(" ") + Translate(_T("and")) + _T(" ") + strMax;

        MessageBox(msg.GetString(),  Translate(_T("Value Invalid")), MB_OK|MB_ICONEXCLAMATION);

        nValue = kValueCheck[type].maxValue;
        sValue.Format(_T("%d"), nValue);
        GetDlgItem(kValueCheck[type].controlID)->SetWindowText(sValue);

    }
    else if (nValue < kValueCheck[type].minValue)
    {
        CString msg, strName, strMin, strMax, strValue;
		strName.Format(_T("%s"), kValueCheck[type].name);
		strMin.Format(_T("%d"), kValueCheck[type].minValue);
		strMax.Format(_T("%d"), kValueCheck[type].maxValue);
        strValue.Format(_T("%d"), nValue);
		msg = Translate(_T("Value of ")) + strName + Translate(_T(" is invalid.")) + _T("\n") +
			Translate(_T("Please input number between")) + _T(" ") + strMin + _T(" ") + Translate(_T("and")) + _T(" ") + strMax;

        MessageBox(msg.GetString(),  Translate(_T("Value Invalid")), MB_OK|MB_ICONEXCLAMATION);

        nValue = kValueCheck[type].minValue;
        sValue.Format(_T("%d"), nValue);
        GetDlgItem(kValueCheck[type].controlID)->SetWindowText(sValue);
    }

    m_bInChangeSize = false;
}

LRESULT CPropertyLayout::OnGroupChange( WPARAM wParam, LPARAM lParam )
{
    return 0;
}

void CPropertyLayout::OnBnClickedEditGroup()
{
    CGroupEditor dlg;
    dlg.SetContent(m_layoutInfo, m_pLayoutGroup, this);

    dlg.DoModal();

    SetContent(m_layoutInfo, m_pLayoutGroup);
}

void CPropertyLayout::OnLbnSelchangeListLayers()
{
    UpdateData(TRUE);
    int i = m_wndLayerList.GetCurSel();
    if (i == -1)
    {
        GetParent()->SendMessage(WM_ON_LAYER_SEL_CHANGE, NULL, 0);
        return;
    }

    CS3DrawObj* obj = (CS3DrawObj*)m_wndLayerList.GetItemDataPtr(i);
    GetParent()->SendMessage(WM_ON_LAYER_SEL_CHANGE, (WPARAM)obj, 0);

	BOOL bFound = FALSE;
    ((CPropertyCtrl*)GetParent())->SetLayer(m_layoutInfo->GetLayerInfo(obj->GetLayerId(), bFound), obj);
}


BOOL CPropertyLayout::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_MOUSEWHEEL)
    {
        GetParent()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
        return TRUE;
    }
    if (pMsg->message == WM_LBUTTONDOWN)
    {
        GetParent()->SetFocus();
    }
	/*
	if(GetFocus() == GetDlgItem(IDC_EDIT_WIDTH) ||GetFocus() == GetDlgItem(IDC_EDIT_HEIGHT) )
	{
		if(pMsg->message == WM_KEYDOWN)  
		{
			if( isdigit(pMsg->wParam) )//|| pMsg->wParam == VK_OEM_MINUS 
			{
				;
			}
			else if (pMsg->wParam >= VK_NUMPAD0 && pMsg->wParam <= VK_NUMPAD9 )
			{
				;
			}
			else if(pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_DELETE 
				|| pMsg->wParam == VK_BACK || pMsg->wParam == VK_TAB)
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
	*/
    return CDialogEx::PreTranslateMessage(pMsg);
}

void CPropertyLayout::SetLayerList(const CS3DrawObjList* pObjList)
{
    m_LayerList.RemoveAll();

    PROPERTY_TYPE propertyType;
    POSITION pos = pObjList->GetHeadPosition();
    while (pos != NULL)
    {
        CS3DrawObj* pObj = pObjList->GetNext(pos);
        pObj->GetPropertyType(propertyType);

        if (propertyType == BACKGROUND)
        {
            continue;
        }

        m_LayerList.AddTail(pObj);
    }
}
