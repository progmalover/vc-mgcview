#pragma once
#include <afx.h>
#include "resource.h"
#include "BaseDropTarget.h"
#include "Controller.h"

enum CLIENT_STATUS
{
    CLIENT_WELL = 0,
    CLIENT_NEED_ATTENTION = 1,
    CLIENT_CRITICAL = 2,
    CLIENT_DISCONNECT = 3,
    CLIENT_STATUS_CNT = 4,
};

enum GROUP_STATUS
{
    GROUP_EMPTY = 0,
    GROUP_STATUS_WELL = 1,
    GROUP_NEED_ATTENTION = 2,
    GROUP_CRITICAL = 3,
    GROUP_STATUS_CNT = 4,
};

CLIENT_STATUS GetClientStatus(INT ClientID, CString& strIP, const std::map< int,ClientRTStatistics>& Stats);

CLIENT_STATUS GetClientStatus(const ClientRTStatistics& Stats, CString& strIP);