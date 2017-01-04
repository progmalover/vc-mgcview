#pragma once

#include  <afxvisualmanager.h>
#include <math.h>

#define VisualManager	CMFCVisualManager::GetInstance ()
enum
{
	_TIMETABLE_APPOINTMENT_SPACE = 3
};	

enum _TOPRATION_MODE
{
	_TOPERATION_FIRST = 0,
	_TOPERATION_NONE  = _TOPERATION_FIRST,
	_TOPERATION_SELECT  = 1,
	_TOPERATION_DRAG = 2,
	_TOPERATION_COPY = 3,
	_TOPERATION_LAST  = _TOPERATION_COPY
};	

enum _TVIEW_STYLE
{
	_TVIEW_STYLE_FIRST     = 0,
	_TVIEW_STYLE_ALPHA       = _TVIEW_STYLE_FIRST,
	_TVIEW_STYLE_LAST      = _TVIEW_STYLE_ALPHA
};

enum _TVIEW_TIME_DELTA
{
	_TVIEW_TIME_DELTA_FIRST = 0,
	_TVIEW_TIME_DELTA_60    = _TVIEW_TIME_DELTA_FIRST,
	_TVIEW_TIME_DELTA_30    = 1,
	_TVIEW_TIME_DELTA_20    = 2,
	_TVIEW_TIME_DELTA_15    = 3,
	_TVIEW_TIME_DELTA_10    = 4,
	_TVIEW_TIME_DELTA_6     = 5,
	_TVIEW_TIME_DELTA_5     = 6,
	_TVIEW_TIME_DELTA_4     = 7,
	_TVIEW_TIME_DELTA_3     = 8,
	_TVIEW_TIME_DELTA_2     = 9,
    _TVIEW_TIME_DELTA_1     = 10,
    _TVIEW_TIME_DELTA_30S   = 11,
    _TVIEW_TIME_DELTA_15S   = 12,
    _TVIEW_TIME_DELTA_5S    = 13,
    _TVIEW_TIME_DELTA_1S    = 14,
	_TVIEW_TIME_DELTA_LAST  = _TVIEW_TIME_DELTA_1S
};

enum _TVIEW_HITTEST
{
    _TVIEW_HITTEST_FIRST              =  0,
    _TVIEW_HITTEST_NOWHERE            =  _TVIEW_HITTEST_FIRST, // 
    _TVIEW_HITTEST_TIMEBAR            =  1, // day view timebar area
    _TVIEW_HITTEST_CLIENT             =  6, // client area
    _TVIEW_HITTEST_ICON_UP            =  9, // up icons
    _TVIEW_HITTEST_ICON_DOWN          = 10, // down icons
    // this is appointment hit tests. include only new appointments enums
    _TVIEW_HITTEST_APPOINTMENT        = 11, // appointment area
    _TVIEW_HITTEST_APPOINTMENT_MOVE   = 12, // appointment move area (left side)
    _TVIEW_HITTEST_APPOINTMENT_TOP    = 13, // day view appointment top area (top side)
    _TVIEW_HITTEST_APPOINTMENT_BOTTOM = 14, // day view appointment bottom area (bottom side)
    _TVIEW_HITTEST_APPOINTMENT_LEFT   = 15, // day view appointment left area (left side)
    _TVIEW_HITTEST_APPOINTMENT_RIGHT  = 16, // day view appointment right area (right side)
    //
    _TVIEW_HITTEST_OPLAYER            = 17, // appointment area
    _TVIEW_HITTEST_OPLAYER_MOVE       = 18, // appointment move area (left side)
    _TVIEW_HITTEST_OPLAYER_TOP        = 19, // day view appointment top area (top side)
    _TVIEW_HITTEST_OPLAYER_BOTTOM     = 20, // day view appointment bottom area (bottom side)
    _TVIEW_HITTEST_OPLAYER_LEFT       = 21, // day view appointment left area (left side)
    _TVIEW_HITTEST_OPLAYER_RIGHT      = 22, // day view appointment right area (right side)
    _TVIEW_HITTEST_LAST               = _TVIEW_HITTEST_OPLAYER_RIGHT
};

int TROUND(double val);
