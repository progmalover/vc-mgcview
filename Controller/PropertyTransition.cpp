// PropertyTransition.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "PropertyTransition.h"
#include "afxdialogex.h"
#include "PropertyCtrl.h"
#include "Localization/Localization.h"

// CPropertyTransition dialog
IMPLEMENT_DYNAMIC(CPropertyTransition, CDialogEx)
UINT WM_TRANS_UPDATE_BEGIN = ::RegisterWindowMessage(_T("WM_TRANS_UPDATE_BEGIN"));
UINT WM_TRANS_UPDATE_END = ::RegisterWindowMessage(_T("WM_TRANS_UPDATE_END"));

CPropertyTransition::CPropertyTransition(CWnd* pParent /*=NULL*/, BOOL inOut)
	: CDialogEx(CPropertyTransition::IDD, pParent), m_bInOut(inOut)
{
    m_pTransition = NULL;

    static const S3S_CONTENT_EFFECT_DIRECTION DIRECTION_NONE      [] = {
        S3S_EFFECT_DIRECTION_NONE
    };
    static const S3S_CONTENT_EFFECT_DIRECTION DIRECTION_RANDOM    [] = {
        S3S_EFFECT_DIRECTION_NONE
    };
    static const S3S_CONTENT_EFFECT_DIRECTION DIRECTION_WIPE      [] = {
        S3S_EFFECT_DIRECTION_UPPER,
        S3S_EFFECT_DIRECTION_UPPER_RIGHT,
        S3S_EFFECT_DIRECTION_RIGHT,
        S3S_EFFECT_DIRECTION_LOWER_RIGHT,
        S3S_EFFECT_DIRECTION_LOWER,
        S3S_EFFECT_DIRECTION_LOWER_LEFT,
        S3S_EFFECT_DIRECTION_LEFT,
        S3S_EFFECT_DIRECTION_UPPER_LEFT,
        S3S_EFFECT_DIRECTION_LEFT_RIGHT,
        S3S_EFFECT_DIRECTION_UP_DOWN,
        S3S_EFFECT_DIRECTION_RANDOM
    };
    static const S3S_CONTENT_EFFECT_DIRECTION DIRECTION_EXPAND    [] = {
        S3S_EFFECT_DIRECTION_UPPER,
        S3S_EFFECT_DIRECTION_UPPER_RIGHT,
        S3S_EFFECT_DIRECTION_RIGHT,
        S3S_EFFECT_DIRECTION_LOWER_RIGHT,
        S3S_EFFECT_DIRECTION_LOWER,
        S3S_EFFECT_DIRECTION_LOWER_LEFT,
        S3S_EFFECT_DIRECTION_LEFT,
        S3S_EFFECT_DIRECTION_UPPER_LEFT,
        S3S_EFFECT_DIRECTION_LEFT_RIGHT,
        S3S_EFFECT_DIRECTION_UP_DOWN,
        S3S_EFFECT_DIRECTION_RANDOM
    };
    static const S3S_CONTENT_EFFECT_DIRECTION DIRECTION_SLIDEIN   [] = {
        S3S_EFFECT_DIRECTION_UPPER,
        S3S_EFFECT_DIRECTION_UPPER_RIGHT,
        S3S_EFFECT_DIRECTION_RIGHT,
        S3S_EFFECT_DIRECTION_LOWER_RIGHT,
        S3S_EFFECT_DIRECTION_LOWER,
        S3S_EFFECT_DIRECTION_LOWER_LEFT,
        S3S_EFFECT_DIRECTION_LEFT,
        S3S_EFFECT_DIRECTION_UPPER_LEFT,
        S3S_EFFECT_DIRECTION_RANDOM
    };
    static const S3S_CONTENT_EFFECT_DIRECTION DIRECTION_BLIND     [] = {
        S3S_EFFECT_DIRECTION_UPPER,
        S3S_EFFECT_DIRECTION_RIGHT,
        S3S_EFFECT_DIRECTION_LOWER,
        S3S_EFFECT_DIRECTION_LEFT,
        S3S_EFFECT_DIRECTION_RANDOM
    };
    static const S3S_CONTENT_EFFECT_DIRECTION DIRECTION_TURNOVER  [] = {
        S3S_EFFECT_DIRECTION_UPPER,
        S3S_EFFECT_DIRECTION_RIGHT,
        S3S_EFFECT_DIRECTION_LOWER,
        S3S_EFFECT_DIRECTION_LEFT,
        S3S_EFFECT_DIRECTION_RANDOM
    };
    static const S3S_CONTENT_EFFECT_DIRECTION DIRECTION_ROLL      [] = {
        S3S_EFFECT_DIRECTION_UPPER,
        S3S_EFFECT_DIRECTION_RIGHT,
        S3S_EFFECT_DIRECTION_LOWER,
        S3S_EFFECT_DIRECTION_LEFT,
        S3S_EFFECT_DIRECTION_RANDOM
    };
    static const S3S_CONTENT_EFFECT_DIRECTION DIRECTION_WHEEL     [] = {
        S3S_EFFECT_DIRECTION_CLOCKWISE,
        S3S_EFFECT_DIRECTION_COUNTERCLOCKWISE,
        S3S_EFFECT_DIRECTION_RANDOM
    };
    static const S3S_CONTENT_EFFECT_DIRECTION DIRECTION_CLOCK     [] = {
        S3S_EFFECT_DIRECTION_CLOCKWISE,
        S3S_EFFECT_DIRECTION_COUNTERCLOCKWISE,
        S3S_EFFECT_DIRECTION_RANDOM
    };
    static const S3S_CONTENT_EFFECT_DIRECTION DIRECTION_WAVE      [] = {
        S3S_EFFECT_DIRECTION_UPPER_RIGHT,
        S3S_EFFECT_DIRECTION_LOWER_RIGHT,
        S3S_EFFECT_DIRECTION_LOWER_LEFT,
        S3S_EFFECT_DIRECTION_UPPER_LEFT,
        S3S_EFFECT_DIRECTION_RANDOM
    };
    static const S3S_CONTENT_EFFECT_DIRECTION DIRECTION_FADE      [] = {
        S3S_EFFECT_DIRECTION_NONE
    };
    static const S3S_CONTENT_EFFECT_DIRECTION DIRECTION_BLOCK     [] = {
        S3S_EFFECT_DIRECTION_NONE
    };
    static const S3S_CONTENT_EFFECT_DIRECTION DIRECTION_ROUND     [] = {
        S3S_EFFECT_DIRECTION_NONE
    };
    static const S3S_CONTENT_EFFECT_DIRECTION DIRECTION_SCREW     [] = {
        S3S_EFFECT_DIRECTION_CLOCKWISE,
        S3S_EFFECT_DIRECTION_COUNTERCLOCKWISE,
        S3S_EFFECT_DIRECTION_RANDOM
    };
    static const S3S_CONTENT_EFFECT_DIRECTION DIRECTION_PLUS      [] = {
        S3S_EFFECT_DIRECTION_NONE
    };

#define ARRAY_COUNT(x) (sizeof(x)/sizeof(*x))
#define FILTER_ENTRY(name) \
    m_filters[S3S_EFFECT_##name].directions = DIRECTION_##name;\
    m_filters[S3S_EFFECT_##name].directionCount = ARRAY_COUNT(DIRECTION_##name)

    FILTER_ENTRY(NONE);
    FILTER_ENTRY(RANDOM);
    FILTER_ENTRY(WIPE);
    FILTER_ENTRY(EXPAND);
    FILTER_ENTRY(SLIDEIN);
    FILTER_ENTRY(BLIND);
    FILTER_ENTRY(TURNOVER);
    FILTER_ENTRY(ROLL);
    FILTER_ENTRY(WHEEL);
    FILTER_ENTRY(CLOCK);
    FILTER_ENTRY(WAVE);
    FILTER_ENTRY(FADE);
    FILTER_ENTRY(BLOCK);
    FILTER_ENTRY(ROUND);
    FILTER_ENTRY(SCREW);
    FILTER_ENTRY(PLUS);
#undef ARRAY_COUNT
#undef FILTER_ENTRY
}

CPropertyTransition::~CPropertyTransition()
{
}

void CPropertyTransition::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    //  DDX_CBIndex(pDX, IDC_COMBO_DIRECTION, m_nDirection);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_DURATION, m_duration);
    DDX_Control(pDX, IDC_COMBO_DIRECTION, m_wndDirection);
    DDX_Control(pDX, IDC_COMBO_TYPE, m_wndEffect);
}


BEGIN_MESSAGE_MAP(CPropertyTransition, CDialogEx)
    ON_WM_SIZE()
    ON_CBN_SELCHANGE(IDC_COMBO_DIRECTION, &CPropertyTransition::OnSelchangeComboDirection)
    ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CPropertyTransition::OnSelchangeComboType)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_DURATION, &CPropertyTransition::OnDtnDatetimechangeDatetimepickerDuration)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CPropertyTransition)
    EASYSIZE(IDC_STATIC_TRANSITION,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_DATETIMEPICKER_DURATION,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_COMBO_DIRECTION,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
END_EASYSIZE_MAP

void CPropertyTransition::UpdateBegin()
{
    GetParent()->SendMessage(WM_TRANS_UPDATE_BEGIN,this->m_bInOut,0);
}

void CPropertyTransition::UpdateEnd()
{
    GetParent()->SendMessage(WM_TRANS_UPDATE_END,this->m_bInOut,0);
}
// CPropertyTransition message handlers
BOOL CPropertyTransition::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    INIT_EASYSIZE;



/*
    for (int i = 0; i < S3S_EFFECT_DIRECTION_TOTAL; ++i)
    {
        if (szEffectDirection[i] == _T(""))
            m_wndDirection.AddString(_T("N/A"));
        else
            m_wndDirection.AddString(szEffectDirection[i]);
    }
*/
    for (int i = 0; i < S3S_EFFECT_TOTAL; ++i)
    {
		INT index;
        if (szEffect[i] == _T(""))
           index = m_wndEffect.AddString(Translate(_T("N/A")));
        else
		{
           index = m_wndEffect.AddString(Translate(szEffect[i]));
		}

		m_wndEffect.SetItemData(index, i);
    }

    m_wndDirection.AddString(Translate(_T("N/A")));
    m_wndDirection.SetCurSel(0);


    CString caption;
    if (m_bInOut)
    {
        caption = Translate(_T("Transition In"));
    }
    else
    {
        caption = Translate(_T("Transition Out"));
    }
    GetDlgItem(IDC_STATIC_TRANSITION)->SetWindowText(caption.GetString());

    ((CDateTimeCtrl*)GetDlgItem(IDC_DATETIMEPICKER_DURATION))->SetFormat(_T("HH:mm:ss"));

	Translate(this);

	return FALSE;  // return TRUE  unless you set the focus to a control
}


void CPropertyTransition::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

BOOL CPropertyTransition::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Add your specialized code here and/or call the base class

    CDialogEx::PreCreateWindow(cs);

    cs.style |= WS_CLIPCHILDREN;
    return TRUE;
}


VOID CPropertyTransition::SetContent(S3SIGNAGE_TRANSIION *pTransition)
{
    if (pTransition)
    {
        m_pTransition = pTransition;
        int DirectionIndex = 0;
        int DirectionSelIndex = 0;
        for (int i = 0; i < S3S_EFFECT_DIRECTION_TOTAL; ++i)
        {
            if (m_pTransition->Direction == szEffectDirection[i])
            {
                DirectionIndex = i;
            }
        }
        if (m_pTransition->Direction == _T(""))
        {
            DirectionSelIndex = 0;
        }


        m_wndDirection.ResetContent();
        for (int i = 0; i < S3S_EFFECT_TOTAL; ++i)
        {
            if (szEffect[i] == m_pTransition->Name)
            {
                for (size_t j = 0; j < m_filters[i].directionCount; ++j)
                {
                    if(m_filters[i].directions[j] == DirectionIndex)
                    {
                        DirectionSelIndex = j;
                    }

					int index;

                    if (szEffectDirection[m_filters[i].directions[j]] == _T(""))
                    {
                        index = m_wndDirection.AddString(Translate(_T("N/A")));
                    }
                    else
                    {
                        index = m_wndDirection.AddString(Translate(szEffectDirection[m_filters[i].directions[j]]));
                    }

					m_wndDirection.SetItemData(index, j);
                }
            }
        }

        m_wndDirection.SetCurSel(DirectionSelIndex);

        int EffectIndex = 0;

        for (int i = 0; i < S3S_EFFECT_TOTAL; ++i)
        {
            if (m_pTransition->Name == szEffect[i])
            {
                EffectIndex = i;
            }
        }
        if (m_pTransition->Name == _T(""))
        {
            EffectIndex = 0;
        }
        m_wndEffect.SetCurSel(EffectIndex);
        m_strCurEffect = m_pTransition->Name;

        int h, m, s;
        h = m_pTransition->Duration / 3600;
        m = (m_pTransition->Duration - h * 3600) / 60;
        s = (m_pTransition->Duration % 60);
        m_duration.SetTime(h, m, s);

        UpdateData(FALSE);
    }
}

void CPropertyTransition::Clear()
{
    m_pTransition = NULL;
}

void CPropertyTransition::UpdateContent()
{
    UpdateData(TRUE);
    if (m_pTransition)
    {
        UpdateBegin();
		DWORD SelEffectIndex = m_wndEffect.GetItemData(m_wndEffect.GetCurSel());
		m_pTransition->Name = szEffect[SelEffectIndex];

        int DirectionIndex = m_wndDirection.GetCurSel();
        if(DirectionIndex < 0)
        {
            m_wndDirection.SetCurSel(0);
            DirectionIndex = 0;
        }

		DWORD SelDirIndex = m_wndDirection.GetItemData(DirectionIndex);
		m_pTransition->Direction = szEffectDirection[m_filters[SelEffectIndex].directions[SelDirIndex]];

        // backup old duration and check new duration
        DWORD oldDuration = m_pTransition->Duration;
        m_pTransition->Duration = m_duration.GetHour() * 3600 + m_duration.GetMinute() * 60 + m_duration.GetSecond();

        // wrong duration, revert to old duration
        if (!((CPropertyCtrl*)GetParent())->CheckTransitionTime())
        {
            m_pTransition->Duration = oldDuration;

            int h, m, s;
            h = m_pTransition->Duration / 3600;
            m = (m_pTransition->Duration - h * 3600) / 60;
            s = (m_pTransition->Duration % 60);
            m_duration.SetTime(h, m, s);

            MessageBox(Translate(_T("Transition duration needs to be shorter than media content's duration")),
                Translate(_T("Warning:Check value")), MB_OK | MB_ICONEXCLAMATION);
            UpdateData(FALSE);
        }
       
        UpdateEnd();
    }
}


void CPropertyTransition::OnSelchangeComboDirection()
{
    UpdateContent();
}


void CPropertyTransition::OnSelchangeComboType()
{
    ChangeEffect();
    UpdateContent();
}


void CPropertyTransition::OnDtnDatetimechangeDatetimepickerDuration(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    UpdateContent();
    *pResult = 0;
}

void CPropertyTransition::ChangeEffect()
{
    CString effect;
    UpdateData();

	DWORD EffectIndex = m_wndEffect.GetItemData(m_wndEffect.GetCurSel());
	effect = szEffect[EffectIndex];

    if (m_strCurEffect == effect)
        return;

    int DirectionIndex = 0;

    m_wndDirection.ResetContent();
    for (int i = 0; i < S3S_EFFECT_TOTAL; ++i)
    {
        if (szEffect[i] == effect)
        {

            DirectionIndex = m_filters[i].directionCount - 1;

            for (size_t j = 0; j < m_filters[i].directionCount; ++j)
            {
				int index;
                if (szEffectDirection[m_filters[i].directions[j]] == _T(""))
                {
                    index = m_wndDirection.AddString(Translate(_T("N/A")));
                }
                else
                {
                    index = m_wndDirection.AddString(Translate(szEffectDirection[m_filters[i].directions[j]]));
                }

				m_wndDirection.SetItemData(index, j);
            }
        }
    }

    m_wndDirection.SetCurSel(DirectionIndex);
    m_strCurEffect = effect;
}

BOOL CPropertyTransition::PreTranslateMessage(MSG* pMsg)
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

    return __super::PreTranslateMessage(pMsg);
}

HWND CPropertyTransition::GetFocusableWindow(int nPoisition, LPVOID lParam, BOOL shift)
{
    CPropertyTransition* pThis = (CPropertyTransition*)lParam;
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
