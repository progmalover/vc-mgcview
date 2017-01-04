#pragma once


class LayoutFileUtility
{

public:
    static HRESULT PreviewLayout(const CString& szLayoutFile, const vector<int>& MediaIDs,
        const int nWidth, const int nHeight, CString szLayoutThumbnail = _T(""));

public:
    LayoutFileUtility();
    virtual ~LayoutFileUtility();
};