#ifndef _SORT_FUNCS
#define _SORT_FUNCS
extern int BaseStrComp(CString strItem1,CString strItem2);
extern int MediaSizeComp(CString strItem1,CString strItem2);
extern int MediaDurationComp(CString strItem1,CString strItem2);
extern int NumberStrComp(CString strItem1,CString strItem2);
extern int MediaDimentionComp(CString strItem1,CString strItem2);
extern int IPAdressComp(CString strItem1,CString strItem2);
extern int VersionComp(CString strItem1,CString strItem2);

typedef int (*BaseCompairFunc)(CString strItem1,CString strItem2);
#endif