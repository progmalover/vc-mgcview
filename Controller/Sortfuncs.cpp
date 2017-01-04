#include "stdafx.h"
static int StrToSize(CString &strVal)
{
	float fval;
	LPTSTR pDesk;
	strVal.MakeLower();

	fval = _tstof(strVal.GetBuffer());
	pDesk = ::StrStr(strVal,_T("gb"));
	if(pDesk)
		fval *= 1000000000;
	
	pDesk = ::StrStr(strVal,_T("mb"));
	if(pDesk)
		fval *= 1000000;

	pDesk = ::StrStr(strVal,_T("kb"));
	if(pDesk)
			fval *= 1000;

	return (int)fval;
}

static int StrToDuration(CString &strVal)
{
	 CString strSub;
	 int ntime = 0;
	 int i = 0;
	 while(::AfxExtractSubString(strSub,strVal,i,':'))
	 {
		 ntime += StrToInt(strSub);
		 if( i < 2)
			ntime *= 60;
		 i++;
	 }
	return ntime;
}

int BaseStrComp(CString strItem1,CString strItem2)
{
	return _tcsicmp(strItem1,strItem2);
}

int MediaSizeComp(CString strItem1,CString strItem2)
{
	int nSize1 = StrToSize(strItem1);
	int nSize2 = StrToSize(strItem2);

	return (nSize1 - nSize2);
}

int MediaDurationComp(CString strItem1,CString strItem2)
{
	int nDuration1 = StrToDuration(strItem1);
	int nDuration2 = StrToDuration(strItem2);
	return (nDuration1 - nDuration2);
}

int NumberStrComp(CString strItem1,CString strItem2)
{
	int nNumber1 = StrToInt(strItem1);
	int nNumber2 = StrToInt(strItem2);

	return (nNumber1 - nNumber2);
}

int MediaDimentionComp(CString strItem1,CString strItem2)
{
	int nDimention1,nDimention2;
	
	strItem1.MakeLower();
	strItem2.MakeLower();

	TCHAR ch = _T('*');
	if( strItem1.Find(ch) <= -1)
		ch = _T('x');
	CString strVal;
	AfxExtractSubString(strVal,strItem1,0,ch);
	nDimention1 = StrToInt(strVal);
	AfxExtractSubString(strVal,strItem1,1,ch);
	strVal.TrimLeft();
	nDimention1 *= StrToInt(strVal);

	AfxExtractSubString(strVal,strItem2,0,ch);
	nDimention2 = StrToInt(strVal);
	AfxExtractSubString(strVal,strItem2,1,ch);
	strVal.TrimLeft(); 
	nDimention2 *= StrToInt(strVal);

	return (nDimention1 - nDimention2);
}

int IPAdressComp(CString strItem1,CString strItem2)
{
	int nIPSec1,nIPSec2;
	
	for(int i = 0;i < 4;i++)
	{
		CString strSec;
		AfxExtractSubString(strSec,strItem1,i,_T('.'));
		nIPSec1 = StrToInt(strSec);
		AfxExtractSubString(strSec,strItem2,i,_T('.'));
		nIPSec2 = StrToInt(strSec);

		if( nIPSec1!=nIPSec2)
			return (nIPSec1 - nIPSec2) << (3 - i)*8;

	}

	return 0;
}

int  VersionComp(CString strItem1,CString strItem2)
{
	float fVal1,fVal2;
	int len = strItem1.GetLength();
	if(len < strItem2.GetLength())
		len = strItem2.GetLength();

	fVal1 = _tstof(strItem1);
	fVal2 = _tstof(strItem2);

	float fVal = fVal1 - fVal2;
	while( --len >= 0)
		fVal *= 10;

	return fVal;
}