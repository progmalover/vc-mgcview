#include "stdafx.h"

#include "BORROW_Pro.h"
#include "BORROW_CalendarBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//*************************************************************************************
void OnDrawControlBorder (CDC* pDC, CRect rect, CWnd* pWndCtrl, BOOL bDrawOnGlass)
{
    ASSERT_VALID (pDC);

    if (bDrawOnGlass)
    {
        CDrawingManager dm (*pDC);
        dm.DrawRect (rect, (COLORREF)-1, afxGlobalData.clrBarShadow);

        rect.DeflateRect (1, 1);

        dm.DrawRect (rect, (COLORREF)-1, afxGlobalData.clrWindow);
    }
    else
    {
        if (pWndCtrl->GetSafeHwnd () != NULL &&
            (pWndCtrl->GetStyle () & WS_POPUP))
        {
            pDC->Draw3dRect (rect, afxGlobalData.clrBarShadow, afxGlobalData.clrBarShadow);
        }
        else
        {
            pDC->Draw3dRect (rect, afxGlobalData.clrBarDkShadow, afxGlobalData.clrBarHilite);
        }

        rect.DeflateRect (1, 1);
        pDC->Draw3dRect (rect, afxGlobalData.clrWindow, afxGlobalData.clrWindow);
    }
}


//*************************************************************************************
void OnDrawControlBorder (CWnd* pWndCtrl)
{
    ASSERT_VALID (pWndCtrl);

    CWindowDC dc (pWndCtrl);

    CRect rect;
    pWndCtrl->GetWindowRect (rect);

    rect.bottom -= rect.top;
    rect.right -= rect.left;
    rect.left = rect.top = 0;

    OnDrawControlBorder (&dc, rect, pWndCtrl, TRUE/*CBORROW_ToolBarImages::m_bIsDrawOnGlass*/);
}

//*************************************************************************************
void GetCalendarColors (const CBORROW_Calendar* /*pCalendar*/,
    CBORROW_CalendarColors& colors)
{
    colors.clrCaption = afxGlobalData.clrBtnFace;
    colors.clrCaptionText = afxGlobalData.clrBtnText;
    colors.clrSelected = afxGlobalData.clrBtnFace;
    colors.clrSelectedText = afxGlobalData.clrBtnText;
    colors.clrTodayBorder = RGB (187, 85, 3);
}


//**********************************************************************************
void OnDrawCheckBoxEx (CDC *pDC, CRect rect, 
                                            int nState,
                                            BOOL bHighlighted, 
                                            BOOL /*bPressed*/,
                                            BOOL bEnabled)
{
    if (TRUE/*CBORROW_ToolBarImages::m_bIsDrawOnGlass*/)
    {
        CDrawingManager dm (*pDC);

        rect.DeflateRect (1, 1);

        dm.DrawRect (rect, 
            bEnabled ? afxGlobalData.clrWindow : afxGlobalData.clrBarFace, 
            afxGlobalData.clrBarShadow);

        /*if (nState == 1)
        {
            CBORROW_MenuImages::Draw (pDC, CBORROW_MenuImages::IdCheck, rect,
                                CBORROW_MenuImages::ImageBlack);
        }*/

        return;
    }

    if (bHighlighted)
    {
        pDC->DrawFocusRect (rect);
    }

    rect.DeflateRect (1, 1);
    pDC->FillSolidRect (&rect, bEnabled ? afxGlobalData.clrWindow :
        afxGlobalData.clrBarFace);

    pDC->Draw3dRect (&rect, 
                    afxGlobalData.clrBarDkShadow,
                    afxGlobalData.clrBarHilite);
    
    rect.DeflateRect (1, 1);
    pDC->Draw3dRect (&rect,
                    afxGlobalData.clrBarShadow,
                    afxGlobalData.clrBarLight);

    /*if (nState == 1)
    {
        CBORROW_MenuImages::Draw (pDC, CBORROW_MenuImages::IdCheck, rect,
                            CBORROW_MenuImages::ImageBlack);
    }
    else */if (nState == 2)
    {
        rect.DeflateRect (1, 1);
        
        WORD HatchBits [8] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };

        CBitmap bmp;
        bmp.CreateBitmap (8, 8, 1, 1, HatchBits);

        CBrush br;
        br.CreatePatternBrush (&bmp);

        pDC->FillRect (rect, &br);
    }
}

//**********************************************************************************
void OnDrawCheckBox (CDC *pDC, CRect rect, 
                                         BOOL bHighlighted, 
                                         BOOL bChecked,
                                         BOOL bEnabled)
{
    OnDrawCheckBoxEx (pDC, rect, bChecked ? 1 : 0, bHighlighted, FALSE, bEnabled);
}

BOOL EnableEditCtrlAutoComplete (HWND hwndEdit, BOOL bDirsOnly)
{
    typedef HRESULT (CALLBACK* LPFNAUTOCOMPLETE) (HWND ,DWORD);
    enum
    {
        shacf_usetab = 0x00000008,
        shacf_filesys_only = 0x00000010,
        shacf_filesys_dirs = 0x00000020,
        shacf_autosuggest_force_on = 0x10000000
    };

    BOOL bRes = FALSE;

    HINSTANCE hInst = ::LoadLibrary (_T("shlwapi.dll"));
    if(hInst != NULL)
    {
        LPFNAUTOCOMPLETE lpfnAutoComplete = (LPFNAUTOCOMPLETE)::GetProcAddress (hInst, "SHAutoComplete");
        if (lpfnAutoComplete != NULL)
        {
            DWORD dwFlags = shacf_usetab | shacf_autosuggest_force_on;
            dwFlags |= (bDirsOnly) ? shacf_filesys_dirs : shacf_filesys_only;

            if (SUCCEEDED(lpfnAutoComplete (hwndEdit, dwFlags)))
            {
                bRes = TRUE;
            }
        }

        ::FreeLibrary (hInst);
    }

    return bRes;
}

//------------------------------------------------------------------------//
BOOL StringFromDecimal(CString& str, DECIMAL& decimal)
{
    VARIANTARG varDecimal;
    VARIANTARG varBstr;
    AfxVariantInit(&varDecimal);
    AfxVariantInit(&varBstr);
    V_VT(&varDecimal) = VT_DECIMAL;
    V_DECIMAL(&varDecimal) = decimal;
    if (FAILED(VariantChangeType(&varBstr, &varDecimal, 0, VT_BSTR)))
    {
        VariantClear(&varDecimal);
        VariantClear(&varBstr);
        return FALSE;
    }
    str = V_BSTR(&varBstr);
    VariantClear(&varDecimal);
    VariantClear(&varBstr);
    return TRUE;
}
//------------------------------------------------------------------------//
BOOL DecimalFromString(DECIMAL& decimal, LPCTSTR psz)
{
    USES_CONVERSION;

    if (psz == NULL || _tcslen (psz) == 0)
    {
        psz = _T("0");
    }

    VARIANTARG varBstr;
    VARIANTARG varDecimal;
    AfxVariantInit(&varBstr);
    AfxVariantInit(&varDecimal);
    V_VT(&varBstr) = VT_BSTR;
    V_BSTR(&varBstr) = SysAllocString(T2COLE(psz));
    if (FAILED(VariantChangeType(&varDecimal, &varBstr, 0, VT_DECIMAL)))
    {
        VariantClear(&varBstr);
        VariantClear(&varDecimal);
        return FALSE;
    }
    decimal = V_DECIMAL(&varDecimal);
    VariantClear(&varBstr);
    VariantClear(&varDecimal);
    return TRUE;
}
BOOL StringFromCy(CString& str, CY& cy)
{
    VARIANTARG varCy;
    VARIANTARG varBstr;
    AfxVariantInit(&varCy);
    AfxVariantInit(&varBstr);
    V_VT(&varCy) = VT_CY;
    V_CY(&varCy) = cy;
    if (FAILED(VariantChangeType(&varBstr, &varCy, 0, VT_BSTR)))
    {
        VariantClear(&varCy);
        VariantClear(&varBstr);
        return FALSE;
    }
    str = V_BSTR(&varBstr);
    VariantClear(&varCy);
    VariantClear(&varBstr);
    return TRUE;
}
//------------------------------------------------------------------------//
BOOL CyFromString(CY& cy, LPCTSTR psz)
{
    USES_CONVERSION;

    if (psz == NULL || _tcslen (psz) == 0)
    {
        psz = _T("0");
    }

    VARIANTARG varBstr;
    VARIANTARG varCy;
    AfxVariantInit(&varBstr);
    AfxVariantInit(&varCy);
    V_VT(&varBstr) = VT_BSTR;
    V_BSTR(&varBstr) = SysAllocString(T2COLE(psz));
    if (FAILED(VariantChangeType(&varCy, &varBstr, 0, VT_CY)))
    {
        VariantClear(&varBstr);
        VariantClear(&varCy);
        return FALSE;
    }
    cy = V_CY(&varCy);
    VariantClear(&varBstr);
    VariantClear(&varCy);
    return TRUE;
}