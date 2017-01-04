#pragma once
#include <afx.h>
#include "resource.h"
#include <windows.h>
#include <cstdio>
#include <process.h>


//////////////////////////////////////////////////////////////////////////
class CBackgroundTask;
extern boost::shared_ptr<CBackgroundTask> g_backgroundTask;

//////////////////////////////////////////////////////////////////////////

#define WORKTHREAD_DATA_LIFE_CYCLE    15000

#define MEDIA_SEARCH_LIST    0
#define MEDIA_CATEGORY_LIST  1

//////////////////////////////////////////////////////////////////////////
// Multi Thread Message related, used for async data process
// Post message like below, and remember release the dynamic msg in the WorkThread
//
////////////////////////////////////////////////////////////////////////////
//  //How to new a buf and post it to work thread
//  UINT BufSize = 20;
//  UINT nBufID;
//  char* pInfo = WorkThreadDataManager<char>::NewDataBuf(nBufID, BufSize); //create dynamic msg
//  if (pInfo == NULL)
//  {
//      break;
//  }
//  //Do any thing with your new buf
//  pInfo[0] = 'S';
//
// if(!PostThreadMessage(GetControllerApp->m_WorkThread.m_nThreadID,USER_MSG,(WPARAM)hWnd,(LPARAM)nBufID))//post thread msg
// {
//     printf("post message failed,errno:%d\n",::GetLastError());
// }
//////////////////////////////////////////////////////////////////////////
//// below is a sample for how to receive message in work thread
////////////////////////////////////////////////////////////////////////////
//// How to get a buf
//  UINT nMessageBufID = (UINT)msg.lParam;
//  HWND   hWnd  = (HWND)msg.wParam;
//
//  DataBuf<char> MessageBuf;
//  if (!WorkThreadDataManager<char>::GetDataBuf(nMessageBufID, MessageBuf))
//  {
//      break;
//  }
////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
//  // do anything with your buf
//  const char * pConstInfo = MessageBuf.Get();
//  printf("recv %s\n",pConstInfo);
////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
//  // How to new a buf
//  UINT BufSize = 20;
//  UINT nBufID;
//  char* pInfo = WorkThreadDataManager<char>::NewDataBuf(BufSize, nBufID); //create dynamic msg
//  if (pInfo == NULL)
//  {
//      break;
//  }
////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
//  // Do any thing with your new buf
//  pInfo[0] = 'S';
////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
//  // How to Post a message
//  ::PostMessage(hWnd/*AfxGetMainWnd()->m_hWnd*/,USER_MSG,(WPARAM)nBufID, NULL);// send back
////////////////////////////////////////////////////////////////////////////

#define USER_MSG_EXIT_THREAD                        WM_USER + 99
#define USER_MSG                                    WM_USER + 100
#define USER_MSG_CONTROLLER_OFFLINE                 WM_USER + 101
#define USER_MSG_SERRCH_CLIENT                      WM_USER + 102
#define USER_MSG_UPDATE_CLIENT_STATUS               WM_USER + 103
#define USER_MSG_ID_UPDATE_CLIENT_STATUS            WM_USER + 104
#define USER_MSG_UPDATE_GROUP_STATUS                WM_USER + 105
#define USER_MSG_GET_CLIENTS_IN_GROUP               WM_USER + 106
#define USER_MSG_GET_MEDIA_ROOT_CATEGORY            WM_USER + 107
#define USER_MSG_GET_MEDIA_SUB_CATEGORY             WM_USER + 108
//#define USER_MSG_GET_MEDIA_ROOT_CATEGORY_FILE       WM_USER + 109
#define USER_MSG_GET_MEDIA_SUB_CATEGORY_FILE        WM_USER + 110
#define USER_MSG_GET_LAYOUT_ROOT_CATEGORY           WM_USER + 111
#define USER_MSG_GET_LAYOUT_SUB_CATEGORY            WM_USER + 112
//#define USER_MSG_GET_LAYOUT_ROOT_CATEGORY_FILE      WM_USER + 113
#define USER_MSG_GET_LAYOUT_SUB_CATEGORY_FILE       WM_USER + 114
#define USER_MSG_GET_TIMETABLE_ROOT_CATEGORY   WM_USER + 115
#define USER_MSG_GET_TIMETABLE_SUB_CATEGORY_FILE    WM_USER + 116
#define USER_MSG_SEARCH_MEDIA_LIST                  WM_USER + 117
#define USER_MSG_MEDIA_CATEGORY_LIST                WM_USER + 118
#define USER_MSG_MEDIALIBRARY_GET_MEDIALIST         WM_USER + 119
#define USER_MSG_SERVER_MEDIALIST                   WM_USER + 120
#define USER_MSG_GET_CLIENT_HISTORY                 WM_USER + 121 //add by LiuSong
#define USER_MSG_UPLOAD_MEDIA                       WM_USER + 122
#define USER_MSG_SEARCH_LAYOUT_LIST                 WM_USER + 123
#define  WM_TRANSLATE_BEFORE                        WM_USER + 313
#define USER_MSG_UPDATE_MEDIA_CATEGORY_LIST         WM_USER + 314 //add by xiangyang
#define USER_MSG_UPDATE_SERVER_MEDIALIST            WM_USER + 315

unsigned __stdcall WorkThreadFun(void *param);

unsigned __stdcall ObserverThreadFun(void *param);

template<class T>
class DataBuf
{
public:
    DataBuf();
    DataBuf(T* pBuf, UINT size);
    DataBuf(DataBuf&);
    ~DataBuf();

    inline BOOL New(UINT size);
    
    inline void operator=(_In_ const DataBuf& src) throw();

    const T* Get(){return m_pDataBuf;}
    const UINT Size(){return m_nBufSize;}

    void Swap(DataBuf&);
	void Free(){
				if(m_pDataBuf) delete m_pDataBuf;
				m_pDataBuf = NULL;
	};
    void SetTickStamp(){m_nTickCount = GetTickCount();}

    DWORD GetTickStamp(){return m_nTickCount;}

    void SetID(DWORD ID){m_nID = ID;}

    DWORD GetID(){return m_nID;}

    DWORD GetThreadID(){return m_nThreadID;}

private:
    UINT  m_nBufSize;
    T*    m_pDataBuf;
    DWORD m_nTickCount;
    DWORD m_nThreadID;
    DWORD m_nID;
};

class ManagerBase;


class ManagerBase
{
public:
    ManagerBase();
    virtual ~ManagerBase();
    virtual void CollectGarbage(DWORD Tick);
    virtual void DoGarbage(DWORD Tick);
    UINT GetNewID();
    
private:

};

template<class T>
class WorkThreadDataManager : public ManagerBase
{
public:

    static WorkThreadDataManager<T> * GetInstance()
    {
        static WorkThreadDataManager<T> m_Instance;

        return &m_Instance;
    }

    // used to new a buffer
    static T* NewDataBuf(UINT& nBufID, const UINT nElemSize = 1);
    
    // used to get a buffer, it will delete the data buffer in the manager automatically
    static BOOL GetDataBuf(const UINT nBufID, DataBuf<T>& Buf);

protected:

    WorkThreadDataManager();
    ~WorkThreadDataManager();

    static void  DeleteDataBuf(const UINT nBufID);
    virtual void CollectGarbage(DWORD Tick);
    static void  ReleaseDataBuf(const UINT nBufID);

    std::map<UINT, DataBuf<T> >  m_WorkThreadBufMap;
    CMutex m_WorkThreadDataMutex;

private:

};



template<class T>
DataBuf<T>::DataBuf():
m_nBufSize (0),
    m_pDataBuf (NULL)
{
    m_nTickCount = GetTickCount();
    m_nThreadID = GetCurrentThreadId();
}

template<class T>
DataBuf<T>::DataBuf(T* pBuf, UINT size)
{
    m_nBufSize = size;

    if (pBuf && m_nBufSize > 0)
    {
        m_pDataBuf = new T[m_nBufSize];
        for (UINT i = 0; i < m_nBufSize; i++)
        {
            m_pDataBuf[i] = pBuf[i];
        }
    }
    else
    {
        m_pDataBuf = NULL;
    }
    m_nTickCount = GetTickCount();
    m_nThreadID = GetCurrentThreadId();
}

template<class T>
DataBuf<T>::DataBuf(DataBuf& Buf)
{
    m_nBufSize = Buf.m_nBufSize;

    if (Buf.m_pDataBuf && m_nBufSize > 0)
    {
        m_pDataBuf = new T[m_nBufSize];
        for (UINT i = 0; i < m_nBufSize; i++)
        {
            m_pDataBuf[i] = Buf.m_pDataBuf[i];
        }
    }
    else
    {
        m_pDataBuf = NULL;
    }
    m_nTickCount = GetTickCount();
    m_nThreadID = GetCurrentThreadId();
}

template<class T>
DataBuf<T>::~DataBuf()
{
    m_nBufSize = 0;
    if (m_pDataBuf)
    {
        delete [] m_pDataBuf;

        m_pDataBuf = NULL;
    }
}

template<class T>
inline void DataBuf<T>::operator=(_In_ const DataBuf& src) throw()
{
    m_nBufSize = 0;
    if (m_pDataBuf)
    {
        delete [] m_pDataBuf;
    }

    m_nBufSize = src.m_nBufSize;

    if (m_nBufSize > 0)
    {
        m_pDataBuf = new T[m_nBufSize];

        for (UINT i = 0; i < m_nBufSize; i++)
        {
            m_pDataBuf[i] = src.m_pDataBuf[i];
        }
    }
    else
    {
        m_pDataBuf = NULL;
    }

    m_nTickCount = src.m_nTickCount;
    m_nThreadID = src.m_nThreadID;
    m_nID = src.m_nID;
}

template<class T>
inline BOOL DataBuf<T>::New(UINT size)
{
    m_nBufSize = size;

    if (m_pDataBuf)
    {
        delete [] m_pDataBuf;
    }

    if (m_nBufSize > 0)
    {
        m_pDataBuf = new T[m_nBufSize];
    }
    else
    {
        m_pDataBuf = NULL;
    }

    return TRUE;
}

template<class T>
void DataBuf<T>::Swap(DataBuf& Buf)
{
    UINT Size = m_nBufSize;
    T* pBuf = m_pDataBuf;

    m_nBufSize = Buf.m_nBufSize;
    m_pDataBuf = Buf.m_pDataBuf;

    Buf.m_nBufSize = Size;
    Buf.m_pDataBuf = pBuf;
}

template<class T>
WorkThreadDataManager<T>::WorkThreadDataManager():
    m_WorkThreadDataMutex (FALSE, NULL)
{

}

template<class T>
WorkThreadDataManager<T>::~WorkThreadDataManager()
{
    m_WorkThreadBufMap.clear();
}

template<class T>
void WorkThreadDataManager<T>::CollectGarbage(DWORD Tick)
{
    //DWORD TickCount = GetTickCount();
    DWORD ThreadID = GetCurrentThreadId();

    m_WorkThreadDataMutex.Lock();
    map< UINT, DataBuf<T> >::iterator it = m_WorkThreadBufMap.begin();
    for(;it!=m_WorkThreadBufMap.end();) 
    { 
        //if((TickCount - it->second.GetTickStamp()) > WORKTHREAD_DATA_LIFE_CYCLE)
        //if(it->second.GetThreadID() != ThreadID && it->second.GetTickStamp() < Tick)
        if(it->second.GetThreadID() != ThreadID && it->second.GetID() < Tick)
        {
            it = m_WorkThreadBufMap.erase(it); 
        } 
        else 
        {
            ++it; 
        } 
    }
    m_WorkThreadDataMutex.Unlock();
}

template<class T>
T* WorkThreadDataManager<T>::NewDataBuf(UINT& nBufID, const UINT nBufSize)
{
    WorkThreadDataManager<T>* pInstance = WorkThreadDataManager<T>::GetInstance();

    DataBuf<T> Buf;
    Buf.New(nBufSize);

    pInstance->m_WorkThreadDataMutex.Lock();

    nBufID = pInstance->GetNewID();
    pInstance->m_WorkThreadBufMap[nBufID] = Buf;
    pInstance->m_WorkThreadBufMap[nBufID].SetID(nBufID);

    pInstance->m_WorkThreadDataMutex.Unlock();

    return const_cast<T*>(pInstance->m_WorkThreadBufMap[nBufID].Get());
}

template<class T>
void  WorkThreadDataManager<T>::ReleaseDataBuf(const UINT nBufID)
{
    WorkThreadDataManager<T>* pInstance = WorkThreadDataManager<T>::GetInstance();

    pInstance->m_WorkThreadDataMutex.Lock();


    map< UINT, DataBuf<T> >::iterator it = pInstance->m_WorkThreadBufMap.find(nBufID);
    if (it == pInstance->m_WorkThreadBufMap.end() ) 
    {
        pInstance->m_WorkThreadDataMutex.Unlock();
        return;
    }

    pInstance->m_WorkThreadBufMap[nBufID].SetTickStamp();

    pInstance->m_WorkThreadDataMutex.Unlock();
}

template<class T>
void  WorkThreadDataManager<T>::DeleteDataBuf(const UINT nBufID)
{
    WorkThreadDataManager<T>* pInstance = WorkThreadDataManager<T>::GetInstance();

    pInstance->m_WorkThreadDataMutex.Lock();


    map< UINT, DataBuf<T> >::iterator it = pInstance->m_WorkThreadBufMap.find(nBufID);
    if (it == pInstance->m_WorkThreadBufMap.end() ) 
    {
        pInstance->m_WorkThreadDataMutex.Unlock();
        return;
    }

    pInstance->m_WorkThreadBufMap.erase(nBufID);

    pInstance->m_WorkThreadDataMutex.Unlock();
}

template<class T>
BOOL WorkThreadDataManager<T>::GetDataBuf(const UINT nBufID, DataBuf<T>& Buf)
{
    WorkThreadDataManager<T>* pInstance = WorkThreadDataManager<T>::GetInstance();

    pInstance->m_WorkThreadDataMutex.Lock();

    map< UINT, DataBuf<T> >::iterator it = pInstance->m_WorkThreadBufMap.find(nBufID);
    if (it == pInstance->m_WorkThreadBufMap.end() ) 
    {
        pInstance->m_WorkThreadDataMutex.Unlock();
        return FALSE;
    }

    Buf = pInstance->m_WorkThreadBufMap[nBufID];

    pInstance->m_WorkThreadDataMutex.Unlock();

    pInstance->DeleteDataBuf(nBufID);

    //pInstance->DoGarbage(Buf.GetTickStamp());
    pInstance->DoGarbage(Buf.GetID());

    return TRUE;
}

