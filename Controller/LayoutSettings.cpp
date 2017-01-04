#include "stdafx.h"
#include "LayoutSettings.h"


int ConvertStr2Duration(CString str)
{
    int Hour, Minute, Second;

    _stscanf_s(str, _T("%d:%d:%d"), &Hour, &Minute, &Second);

    int TotalSecond = (Hour * 60 + Minute) * 60 + Second;

    return TotalSecond;
}

BOOL ConvertStr2BOOL(CString str)
{
    BOOL ReturnVal = FALSE;

    if(str.CompareNoCase(_T("TRUE")) == 0)
    {
        ReturnVal = TRUE;
    }

    return ReturnVal;
}

void RotatePoint(POINTF ptCenter, POINTF& ptR, float Degree)
{
    POINTF ptO = ptR;

    ptR.x = (ptO.x - ptCenter.x) * cos(Degree) 
        + (ptO.y - ptCenter.y)*sin(Degree) + ptCenter.x;
    ptR.y = - (ptO.x - ptCenter.x) * sin(Degree) 
        + (ptO.y - ptCenter.y)*cos(Degree) + ptCenter.y;
}


CRect GetRotatedScreen(FLOAT width, FLOAT height, FLOAT Degree)
{
    POINTF ptCenter;

    ptCenter.x = width/2;
    ptCenter.y = height/2; 

    // lb, lt, rb, rt
    POINTF ptLB, ptLT, ptRB, ptRT;
    ptLB.x = 0; ptLB.y = height;
    ptLT.x = 0; ptLT.y = 0;
    ptRB.x = width; ptRB.y = height;
    ptRT.x = width; ptRT.y = 0;

    RotatePoint(ptCenter, ptLB, Degree);
    RotatePoint(ptCenter, ptLT, Degree);
    RotatePoint(ptCenter, ptRB, Degree);
    RotatePoint(ptCenter, ptRT, Degree);


    CRect rect;
    rect.top = min(min(min(ptLT.y, ptLB.y), ptRT.y), ptRB.y);
    rect.bottom = max(max(max(ptLT.y, ptLB.y), ptRT.y), ptRB.y);
    rect.left = min(min(min(ptLT.x, ptLB.x), ptRT.x), ptRB.x);
    rect.right = max(max(max(ptLT.x, ptLB.x), ptRT.x), ptRB.x);

    return rect;
}