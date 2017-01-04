#ifndef __BORROW_RO_H
#define __BORROW_RO_H

#define BORROW_DLLEXPORT

#define COMPILE_MULTIMON_STUBS
#define _BORROW_CALENDAR_STANDALONE
#define  _BORROW_SUITE_
//#include "BORROW_globals.h"
class CBORROW_LocalResource  
{
public:
    CBORROW_LocalResource() {}
    virtual ~CBORROW_LocalResource() {}

protected:
    HINSTANCE m_hInstOld;
};

#if _MSC_VER >= 1400
#define BCGNcHitTestType	LRESULT
#else
#define BCGNcHitTestType	UINT
#endif

#define BCG_GET_X_LPARAM(lp)		(MAKEPOINTS(lp).x)
#define BCG_GET_Y_LPARAM(lp)		(MAKEPOINTS(lp).y)

//
//
//// Smart docking theme:
//
typedef enum BORROW__SMARTDOCK_THEME
{
    BORROW__SDT_DEFAULT = 0,		// Visual Manager-specific theme
    BORROW__SDT_VS2005 = 1,		// VS 2005-style
    BORROW__SDT_VS2008 = 2,		// VS 2008-style
    BORROW__SDT_VS2010 = 3			// VS 2010-style
};

//*************************************************************************************
void OnDrawControlBorder (CDC* pDC, CRect rect, CWnd* pWndCtrl, BOOL bDrawOnGlass);

//*************************************************************************************
void OnDrawControlBorder (CWnd* pWndCtrl);

class CBORROW_Calendar;
class CBORROW_CalendarColors;
//*************************************************************************************
void GetCalendarColors (const CBORROW_Calendar* /*pCalendar*/,
    CBORROW_CalendarColors& colors);


//**********************************************************************************
void OnDrawCheckBoxEx (CDC *pDC, CRect rect, 
                                            int nState,
                                            BOOL bHighlighted, 
                                            BOOL /*bPressed*/,
                                            BOOL bEnabled);

//**********************************************************************************
void OnDrawCheckBox (CDC *pDC, CRect rect, 
                                         BOOL bHighlighted, 
                                         BOOL bChecked,
                                         BOOL bEnabled);

BOOL EnableEditCtrlAutoComplete (HWND hwndEdit, BOOL bDirsOnly);
BOOL StringFromDecimal(CString& str, DECIMAL& decimal);
BOOL StringFromCy(CString& str, CY& cy);
BOOL CyFromString(CY& cy, LPCTSTR psz);
BOOL DecimalFromString(DECIMAL& decimal, LPCTSTR psz);

#endif // __BORROW_RO_H
