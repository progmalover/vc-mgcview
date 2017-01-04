#include "stdafx.h"
#include "BORROW_Pro.h"
#include "BORROW_TrackMouse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

VOID CALLBACK BORROW_TrackMouseTimerProc (HWND hWnd, 
									  UINT /*uMsg*/,
									  UINT idEvent, 
									  DWORD /*dwTime*/)
{
	RECT	rect;
	POINT	pt;
	
	::GetClientRect (hWnd, &rect);
	::MapWindowPoints (hWnd, NULL, (LPPOINT)&rect, 2);

	::GetCursorPos (&pt);
	if (!::PtInRect (&rect, pt) || (WindowFromPoint(pt) != hWnd)) 
	{
		if (!::KillTimer (hWnd, idEvent))
		{
			// Error killing the timer!
		}
		
		::PostMessage (hWnd,WM_MOUSELEAVE, 0, 0);
	}
}
//************************************************************************************
BOOL BORROW_TrackMouse (LPTRACKMOUSEEVENT ptme)
{
	ASSERT (ptme != NULL);
	if (ptme->cbSize < sizeof (TRACKMOUSEEVENT))
	{
		ASSERT (FALSE);
		return FALSE;
	}
	
	if (!::IsWindow(ptme->hwndTrack)) 
	{
		ASSERT (FALSE);
		return FALSE;
	}
	
	if (!(ptme->dwFlags & TME_LEAVE)) 
	{
		ASSERT (FALSE);
		return FALSE;
	}
	
	return (BOOL) ::SetTimer (ptme->hwndTrack, ptme->dwFlags, 100,
			(TIMERPROC) BORROW_TrackMouseTimerProc);
}
