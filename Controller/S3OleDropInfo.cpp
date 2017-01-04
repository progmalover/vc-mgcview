#include "StdAfx.h"
#include "S3OleDropInfo.h"

IMPLEMENT_DYNAMIC(CS3OleDropInfo, CObject);

CS3OleDropInfo::CS3OleDropInfo(void)
{
    m_pDataObject = NULL;
    m_dwKeyState = 0;
    m_dropEffect = DROPEFFECT_NONE;
    m_dropList = DROPEFFECT_NONE;

}

CS3OleDropInfo::CS3OleDropInfo(COleDataObject* pDataObject, DWORD dwKeyState,
    DROPEFFECT dropEffect, DROPEFFECT dropList, CPoint point)
{
    m_pDataObject = pDataObject;
    m_dwKeyState = dwKeyState;
    m_dropEffect = dropEffect;
    m_dropList = dropList;
    m_point = point;
}

CS3OleDropInfo::~CS3OleDropInfo(void)
{
    m_pDataObject = NULL;
}