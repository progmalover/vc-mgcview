#pragma once

#include  <afxvisualmanager.h>
#include <math.h>

#define VisualManager	CMFCVisualManager::GetInstance ()

enum
{
    _SCHEDULE_APPOINTMENT_SPACE = 3
};	

enum OPRATION_MODE
{
    OPERATION_FIRST = 0,
    OPERATION_NONE  = OPERATION_FIRST,
    OPERATION_SELECT  = 1,
    OPERATION_DRAG = 2,
    OPERATION_COPY = 3,
   OPERATION_LAST  = OPERATION_DRAG
};	

enum _SVIEW_STYLE
{
    _SVIEW_STYLE_FIRST     = 0,
    _SVIEW_STYLE_ALPHA       = _SVIEW_STYLE_FIRST,
    _SVIEW_STYLE_LAST      = _SVIEW_STYLE_ALPHA
};

enum _SVIEW_OPERATION
{
    _SVIEW_OPERATION_FIRST = 0,
    _SVIEW_OPERATION_NONE  = _SVIEW_OPERATION_FIRST,
    _SVIEW_OPERATION_CUT   = 1,
    _SVIEW_OPERATION_PASTE = 2,
    _SVIEW_OPERATION_DRAG  = 3,
    _SVIEW_OPERATION_DROP  = 4,
    _SVIEW_OPERATION_LAST  = _SVIEW_OPERATION_DROP
};



enum _SVIEW_HITTEST
{
    _SVIEW_HITTEST_FIRST              =  0,
    _SVIEW_HITTEST_NOWHERE            =  _SVIEW_HITTEST_FIRST, // 
    _SVIEW_HITTEST_TIMEBAR            =  1, // day view timebar area
    _SVIEW_HITTEST_WEEKBAR            =  2, // month view weekbar area
    _SVIEW_HITTEST_HEADER             =  3, // day view or month view header area
    _SVIEW_HITTEST_HEADER_TODAY       =  4, // day view today header area
    _SVIEW_HITTEST_DEFAULT_APP        =  5, // Used for default appointment setting
    _SVIEW_HITTEST_CLIENT             =  6, // client area
    _SVIEW_HITTEST_DAY_CAPTION        =  7, // caption of the day in week and month views
    _SVIEW_HITTEST_WEEK_CAPTION       =  8, // caption of the week in month view
    _SVIEW_HITTEST_ICON_UP            =  9, // up icons
    _SVIEW_HITTEST_ICON_DOWN          = 10, // down icons
    // this is appointment hit tests. include only new appointments enums
    _SVIEW_HITTEST_APPOINTMENT        = 11, // appointment area
    _SVIEW_HITTEST_APPOINTMENT_MOVE   = 12, // appointment move area (left side)
    _SVIEW_HITTEST_APPOINTMENT_TOP    = 13, // day view appointment top area (top side)
    _SVIEW_HITTEST_APPOINTMENT_BOTTOM = 14, // day view appointment bottom area (bottom side)
    _SVIEW_HITTEST_APPOINTMENT_LEFT   = 15, // day view appointment left area (left side)
    _SVIEW_HITTEST_APPOINTMENT_RIGHT  = 16, // day view appointment right area (right side)
    //
    _SVIEW_HITTEST_OPLAYER            = 17, // appointment area
    _SVIEW_HITTEST_OPLAYER_MOVE       = 18, // appointment move area (left side)
    _SVIEW_HITTEST_OPLAYER_TOP        = 19, // day view appointment top area (top side)
    _SVIEW_HITTEST_OPLAYER_BOTTOM     = 20, // day view appointment bottom area (bottom side)
    _SVIEW_HITTEST_OPLAYER_LEFT       = 21, // day view appointment left area (left side)
    _SVIEW_HITTEST_OPLAYER_RIGHT      = 22, // day view appointment right area (right side)
    _SVIEW_HITTEST_LAST               = _SVIEW_HITTEST_APPOINTMENT_RIGHT
};

enum _SVIEW_TIME_DELTA
{
    _SVIEW_TIME_DELTA_FIRST = 0,
    _SVIEW_TIME_DELTA_60    = _SVIEW_TIME_DELTA_FIRST,
    _SVIEW_TIME_DELTA_30    = 1,
    _SVIEW_TIME_DELTA_20    = 2,
    _SVIEW_TIME_DELTA_15    = 3,
    _SVIEW_TIME_DELTA_10    = 4,
    _SVIEW_TIME_DELTA_6     = 5,
    _SVIEW_TIME_DELTA_5     = 6,
    _SVIEW_TIME_DELTA_4     = 7,
    _SVIEW_TIME_DELTA_3     = 8,
    _SVIEW_TIME_DELTA_2     = 9,
    _SVIEW_TIME_DELTA_1     = 10,
	_SVIEW_TIME_DELTA_30S   = 11,
    _SVIEW_TIME_DELTA_15S   = 12,
    _SVIEW_TIME_DELTA_5S    = 13,
    _SVIEW_TIME_DELTA_1S    = 14,
    _SVIEW_TIME_DELTA_LAST  = _SVIEW_TIME_DELTA_1S
};

enum COPY_DIRECTION
{
    COPY_FIRST = 0,
    COPY_NONE  = COPY_FIRST,
    COPY_LEFT  = 1,
    COPY_RIGHT = 2,
    COPY_UP    = 3,
    COPY_DOWN  = 4,
    COPY_LAST  = COPY_DOWN
};	

#define MULTICOPY_SUNDAY        0x1
#define MULTICOPY_MONDAY        0x2
#define MULTICOPY_TUESDAY       0x4
#define MULTICOPY_WEDNESDAY     0x8
#define MULTICOPY_THURSDAY      0x10
#define MULTICOPY_FRIDAY        0x20
#define MULTICOPY_SATURDAY      0x40
#define MULTICOPY_ALLWEEK       0xFF

int ROUND(double val);
