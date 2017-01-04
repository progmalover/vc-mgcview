#pragma once
//#include "afxole.h"

class CS3OleDropInfo : public CObject
{
    DECLARE_DYNAMIC(CS3OleDropInfo)

public:

    CS3OleDropInfo(void);
    CS3OleDropInfo(COleDataObject* pDataObject, DWORD dwKeyState, DROPEFFECT dropEffect, DROPEFFECT dropList, CPoint point);

    virtual ~CS3OleDropInfo(void);


public:
    COleDataObject*     m_pDataObject;
    DWORD               m_dwKeyState;
    DROPEFFECT          m_dropEffect;
    DROPEFFECT          m_dropList;
    CPoint              m_point;

};