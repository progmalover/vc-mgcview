
#include "stdafx.h"
#include "ClientStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CLIENT_STATUS GetClientStatus(INT ClientID, CString& strIP, const std::map< int,ClientRTStatistics>& Stats)
{
    if (!GetControllerApp->IsLogin())
    {
        return CLIENT_DISCONNECT;
    }

    std::map< int,ClientRTStatistics>::const_iterator cit = Stats.find(ClientID);
    if (cit == Stats.end())
    {
        return CLIENT_DISCONNECT;
    }

    return GetClientStatus(cit->second, strIP);
}

CLIENT_STATUS GetClientStatus(const ClientRTStatistics& Stats, CString& strIP)
{
    const ClientRTStatistics& Statistics = Stats;

    if (!Statistics.IsOnline())
    {
        return CLIENT_DISCONNECT;
    }

    strIP = Statistics.GetIPAddr().c_str();

    LAYOUT_DESC CurLayoutDesc, NextLayoutDesc;
    SCHEDULE_DESC ScheduleDesc;
    PLAYER_STATUS_DESC PlayerStatus;
    SYSTEM_STATUS_DESC SystemStatus;
    S3Time LastHeartBeat(0,0,0,0,0,0);

    Statistics.GetCurLayoutDesc(CurLayoutDesc);
    Statistics.GetNextLayoutDesc(NextLayoutDesc);
    Statistics.GetScheduleDesc(ScheduleDesc);
    Statistics.GetPlayerStatusDesc(PlayerStatus);
    Statistics.GetLastActiveTime(LastHeartBeat);
    Statistics.GetSysStatusDesc(SystemStatus);

    COleDateTimeSpan TimeGap = COleDateTime::GetCurrentTime() - LastHeartBeat.ToCOleDateTime();

    // Critical
    if (TimeGap > COleDateTimeSpan(0, 0, 10, 0))
    {
        return CLIENT_CRITICAL;
    }

    if(!CurLayoutDesc.LayoutName.empty() && CurLayoutDesc.DownloadedSize != CurLayoutDesc.LayoutSize)
    {
        return CLIENT_CRITICAL;
    }

    COleDateTimeSpan myTime = COleDateTime::GetCurrentTime() - NextLayoutDesc.StartTime.ToCOleDateTime();

    if ((NextLayoutDesc.LayoutSize - NextLayoutDesc.DownloadedSize) >
        (myTime.GetTotalSeconds() * SystemStatus.TransportRate))
    {
        return CLIENT_CRITICAL;
    }

    // Need attention

    if (PlayerStatus.FPS < 20)
    {
        return CLIENT_NEED_ATTENTION;
    }

    if (SystemStatus.DiskFreeSpace < 100)
    {
        return CLIENT_NEED_ATTENTION;
    }

    if (SystemStatus.CPUUsage > 90)
    {
        return CLIENT_NEED_ATTENTION;
    }

    if (SystemStatus.MemUsage > 90)
    {
        return CLIENT_NEED_ATTENTION;
    }

    return CLIENT_WELL;
}