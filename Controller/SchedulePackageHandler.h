#pragma once
#include "Appointment.h"
#include "SchedulerPage.h"
#include <tinyxml/tinyxml.h>

#include <atlbase.h>
#include <atlconv.h>

#define SCHEDULEPACKAGE_POSTFIX_SIZE 4
//#define SCHEDULEPACKAGE_POSTFIX _T(".scl")
#define SCHEDULEPACKAGE_POSTFIX _T(".xml")

class SchedulePackageHandler
{
public:
    SchedulePackageHandler(void);
    virtual ~SchedulePackageHandler(void);

    BOOL    ImportPackage(CString &GroupName, AppointmentArray& AppArray,  AppointmentArray& DefaultAppArray, 
        const CString& PackagePathName, ScheduleCtrl* pScheduleCtrl);
    BOOL    ExportPackage(AppointmentArray& AppArray,  AppointmentArray& DefaultAppArray, std::vector<Timetable> TimetableArray, ScheduleCtrl* pScheduleCtrl,
        const CString& PackagePathName = _T(""), CSchedulerPage* pPage = NULL);

protected:
	VOID ExportAppointmentThumbnail(INT LayoutID, TiXmlElement *pLayout);
	VOID ImportAppointmentThumbnail(TiXmlElement* pEntry, Appointment &pApp, ScheduleCtrl* pScheduleCtrl);
private:
    CString m_CurrentPathName;
    TiXmlDocument* m_pCurDoc;
};

