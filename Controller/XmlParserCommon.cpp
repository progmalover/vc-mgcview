#include "StdAfx.h"
#include "XmlParserCommon.h"

void Xml_Parser_Common :: ConvertBOOL2Str(BOOL bValue, char* pValue)
{
	if (bValue)
	{
		strcpy_s(pValue, MAX_PATH, "TRUE");
	}
	else
	{
		strcpy_s(pValue, MAX_PATH, "FALSE");
	}
}

void Xml_Parser_Common::ConvertDuration2Str(DWORD duration, char* pValue)
{
	int Hour, Minute, Second;
	Hour    = duration / (60 * 60);
	Minute  = (duration % (60 * 60)) / 60;
	Second  = (duration % (60 * 60)) % 60;

	sprintf_s(pValue, MAX_PATH, "%02d:%02d:%02d", Hour, Minute, Second);
}