#pragma once
#include "S3DrawObj.h"
#include "CommonLib/RenderableObject.h"

enum S3S_CONTENT_EFFECT{
    S3S_EFFECT_NONE     = 0,
    S3S_EFFECT_RANDOM   = 1,
    S3S_EFFECT_WIPE     = 2,
    S3S_EFFECT_EXPAND   = 3,
    S3S_EFFECT_SLIDEIN  = 4,
    S3S_EFFECT_BLIND    = 5,
    S3S_EFFECT_TURNOVER = 6,
    S3S_EFFECT_ROLL     = 7,
    S3S_EFFECT_WHEEL    = 8,
    S3S_EFFECT_CLOCK    = 9,
    S3S_EFFECT_WAVE     = 10,
    S3S_EFFECT_FADE     = 11,
    S3S_EFFECT_BLOCK    = 12,
    S3S_EFFECT_ROUND    = 13,
    S3S_EFFECT_SCREW    = 14,
    S3S_EFFECT_PLUS     = 15,
    S3S_EFFECT_TOTAL    = S3S_EFFECT_PLUS + 1,
};

enum S3S_CONTENT_EFFECT_DIRECTION{
    S3S_EFFECT_DIRECTION_NONE                   = 0,
    S3S_EFFECT_DIRECTION_RANDOM                 = 1,
    S3S_EFFECT_DIRECTION_UPPER                  = 2,
    S3S_EFFECT_DIRECTION_UPPER_LEFT             = 3,
    S3S_EFFECT_DIRECTION_UPPER_RIGHT            = 4,
    S3S_EFFECT_DIRECTION_LOWER                  = 5,
    S3S_EFFECT_DIRECTION_LOWER_LEFT             = 6,
    S3S_EFFECT_DIRECTION_LOWER_RIGHT            = 7,
    S3S_EFFECT_DIRECTION_LEFT                   = 8,
    S3S_EFFECT_DIRECTION_RIGHT                  = 9,
    S3S_EFFECT_DIRECTION_LEFT_RIGHT             = 10,
    S3S_EFFECT_DIRECTION_RIGHT_LEFT             = 11,
    S3S_EFFECT_DIRECTION_UP_DOWN                = 12,
    S3S_EFFECT_DIRECTION_DOWN_UP                = 13,
    S3S_EFFECT_DIRECTION_CLOCKWISE              = 14,
    S3S_EFFECT_DIRECTION_COUNTERCLOCKWISE       = 15,
    S3S_EFFECT_DIRECTION_TOTAL                  = S3S_EFFECT_DIRECTION_COUNTERCLOCKWISE + 1,
};

enum S3S_EMERGENCY_TEXT_DIRECTION{
	S3S_EMERGENCY_TEXT_DIRECTION_NONE                     = 0,
	S3S_EMERGENCY_TEXT_DIRECTION_LEFT_RIGHT               = 1,
	S3S_EMERGENCY_TEXT_DIRECTION_RIGHT_LEFT               = 2,
	S3S_EMERGENCY_TEXT_DIRECTION_TOTAL					  = S3S_EMERGENCY_TEXT_DIRECTION_RIGHT_LEFT + 1,
};

enum S3S_TEXT_DIRECTION{
    S3S_TEXT_DIRECTION_NONE                     = 0,
    S3S_TEXT_DIRECTION_LEFT_RIGHT               = 1,
    S3S_TEXT_DIRECTION_RIGHT_LEFT               = 2,
    S3S_TEXT_DIRECTION_UP_DOWN                  = 3,
    S3S_TEXT_DIRECTION_DOWN_UP                  = 4,
    S3S_TEXT_DIRECTION_RANDOM                   = 5,
    S3S_TEXT_DIRECTION_TOTAL                    = S3S_TEXT_DIRECTION_RANDOM + 1,
};

enum S3S_TEXT_ALIGNMENT{
    S3S_TEXT_ALIGNMENT_NONE                     = 0,
    S3S_TEXT_ALIGNMENT_LEFT                     = 1,
    S3S_TEXT_ALIGNMENT_CENTER                   = 2,
    S3S_TEXT_ALIGNMENT_RIGHT                    = 3,
    S3S_TEXT_ALIGNMENT_RANDOM                   = 4,
    S3S_TEXT_ALIGNMENT_TOTAL                    = S3S_TEXT_ALIGNMENT_RANDOM + 1,
};

enum S3S_TEXT_LAYOUT{
    S3S_TEXT_LAYOUT_H                           = 0,
    S3S_TEXT_LAYOUT_V                           = 1,
    S3S_TEXT_LAYOUT_TOTAL                       = S3S_TEXT_LAYOUT_V + 1,
};

enum S3S_VERTICAL_TEXT_DIRECTION{
	S3S_VERTICAL_TEXT_DIRECTION_NONE                     = 0,
	//S3S_VERTICAL_TEXT_DIRECTION_LEFT_RIGHT				 = 1,
	//S3S_VERTICAL_TEXT_DIRECTION_RIGHT_LEFT				 = 2,
	S3S_VERTICAL_TEXT_DIRECTION_UP_DOWN                  = 1,
	S3S_VERTICAL_TEXT_DIRECTION_DOWN_UP                  = 2,
	S3S_VERTICAL_TEXT_DIRECTION_RANDOM                   = 3,
	S3S_VERTICAL_TEXT_DIRECTION_TOTAL                    = S3S_VERTICAL_TEXT_DIRECTION_RANDOM + 1,
};

//------------ Layer definition begin --------------------------------//
static const CString szTypeName_Layer        = _T("Layer");
static const CString szTypeName_Message      = _T("Message");
static const CString szTypeName_Video        = _T("Movie");
static const CString szTypeName_Audio        = _T("Audio");
static const CString szTypeName_EmptyContent = _T("EmptyContent");
//------------ Layer definition end ----------------------------------//
static const CString szTypeName_Text         = _T("ScrollTextFile");
static const CString szTypeName_TextFile     = _T("TextFile");


static const CString szMediaCount   = _T("MediaCount");
static const CString szLayoutSize   = _T("LayoutSize");
static const CString szLayoutDesc   = _T("LayoutDescription");
static const CString szLayoutWidth  = _T("LayoutWidth");
static const CString szLayoutHeight = _T("LayoutHeight");

static const UINT nVideoSupport     = 9;
static const UINT nAudioSupport     = 3;

static const CString szVideoSupportType[nVideoSupport] = 
{
    _T("avi"), _T("wmv"), _T("asf"), _T("mpeg"),
    _T("mpg"), _T("mov"), _T("mp4"), _T("flv"), _T("mkv"),
};

static const CString szAudioSupportType[nAudioSupport] =
{
    _T("wma"), _T("mp3"), _T("wav")
};

#define Translate(x) x
static const CString szEffect[S3S_EFFECT_TOTAL] = 
{
    _T(""), Translate(_T("Random")), Translate(_T("Wipe")), Translate(_T("Expand")), 
    Translate(_T("Slide")), Translate(_T("Blind")), Translate(_T("TurnOver")), Translate(_T("Roll")), 
    Translate(_T("Wheel")), Translate(_T("Clock")), Translate(_T("Wave")), Translate(_T("Fade")), 
    Translate(_T("Block")), Translate(_T("Round")), Translate(_T("Screw")), Translate(_T("Plus"))
};

static const CString szEffectDirection[S3S_EFFECT_DIRECTION_TOTAL] = 
{
    _T(""), Translate(_T("Random")), Translate(_T("Upper")), Translate(_T("UpperLeft")), 
    Translate(_T("UpperRight")), Translate(_T("Lower")), Translate(_T("LowerLeft")), Translate(_T("LowerRight")), 
    Translate(_T("Left")), Translate(_T("Right")), Translate(_T("LeftRight")), Translate(_T("RightLeft")), 
    Translate(_T("UpDown")), Translate(_T("DownUp")), Translate(_T("CW")), Translate(_T("CCW"))
};

static const CString szEmergencyTextDirection[S3S_EMERGENCY_TEXT_DIRECTION_TOTAL] = 
{
	Translate(_T("Center")), Translate(_T("LeftRight")), Translate(_T("RightLeft"))
};

static const CString szTextDirection[S3S_TEXT_DIRECTION_TOTAL] = 
{
    Translate(_T("Center")), Translate(_T("LeftRight")), Translate(_T("RightLeft")), Translate(_T("UpDown")), Translate(_T("DownUp")), Translate(_T("Random"))
};

static const CString szTextAlignment[S3S_TEXT_ALIGNMENT_TOTAL] =
{
    _T(""), Translate(_T("Left")), Translate(_T("Center")), Translate(_T("Right")), Translate(_T("Random"))
};

static const CString szTextLayout[S3S_TEXT_LAYOUT_TOTAL] = 
{
    Translate(_T("Horizontal")), Translate(_T("Vertical"))
};

static const CString szVerticalTextDirection[S3S_VERTICAL_TEXT_DIRECTION_TOTAL] = 
{
	Translate(_T("Center")), Translate(_T("UpDown")), Translate(_T("DownUp")), Translate(_T("Random"))
};
#undef Translate

typedef struct _LAYOUTGROUP_SETTING
{
    DWORD           GroupID;
    CString         GroupName;
	CString         GroupDesc;
} LAYOUTGROUP_SETTING;

typedef struct _PLUGIN_SUPPORTFILES
{
    DWORD               nCount;
    PRO_SUPPORTEDFILE   pSupportFile;
}PLUGIN_SUPPORTFILES;

typedef struct _PLUGIN_SUPPORTFILES *PPLUGIN_SUPPORTFILES;

typedef CList<LAYOUTGROUP_SETTING*, LAYOUTGROUP_SETTING*> LAYOUTGROUP_LIST;

typedef struct _S3SIGNAGE_TRANSITION_SETTING
{
    S3S_CONTENT_EFFECT              Effect;
    ULONG                           EffectDuration;
    S3S_CONTENT_EFFECT_DIRECTION    EffectDirection;
}S3SIGNAGE_TRANSITION_SETTING, *PS3SIGNAGE_TRANSITION_SETTING;

typedef struct S3SIGNAGE_TRANSIION
{
    S3SIGNAGE_TRANSIION()
    {
        Name        = szEffect[S3S_EFFECT_NONE];
        Direction   = szEffectDirection[S3S_EFFECT_DIRECTION_NONE];
        Duration    = 0;
    }
    bool operator=(struct S3SIGNAGE_TRANSIION &transObj)
    {
        Name = transObj.Name;
        Direction = transObj.Direction;
        Duration = transObj.Duration;
        return true;
    }
   /* bool operator=(struct S3SIGNAGE_TRANSIION *ptransObj)
    {
        if(this != ptransObj && ptransObj)
			 *this=*ptransObj;
		return true;
    }*/
    bool operator==( struct S3SIGNAGE_TRANSIION &transObj)
    {
        if(Name == transObj.Name &&
           Duration == transObj.Duration &&
           Direction == transObj.Direction)
           return true;

        return false;
    }
    CString     Name;
    DWORD       Duration;
    CString     Direction;
}S3SIGNAGE_TRANSIION;

typedef struct TRANSFORM_VALUE
{
    DWORD       Time;
    float       XValue;
    float       YValue;
}TRANSFORM_VALUE;

typedef CList<TRANSFORM_VALUE*, TRANSFORM_VALUE*> S3SIGNAGE_TRANSFORM;

typedef struct S3SIGNAGE_TEXT_SETTING
{
    S3SIGNAGE_TEXT_SETTING()
    {
        szMessage       = _T("");
        FontColor       = RGB(0,0,0);
        FontSpeed       = 0;
        Transparency    = 50;
        fScale          = 1.0f;
        TextDirection   = szTextDirection[S3S_TEXT_DIRECTION_NONE];
        Alignment       = szTextAlignment[S3S_TEXT_ALIGNMENT_NONE];
        TextLayout      = szTextLayout[S3S_TEXT_LAYOUT_H];
		FontDirection   = szTextLayout[S3S_TEXT_LAYOUT_H];
        pFontInfo = new CFont;
        pFontInfo->CreateFont(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, 
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            ANTIALIASED_QUALITY,
            VARIABLE_PITCH,
            _T("Arial"));
    }

	bool operator=(struct S3SIGNAGE_TEXT_SETTING * pMediaTextSetting)
	{	
		if(this != pMediaTextSetting && pMediaTextSetting)
			 *this=*pMediaTextSetting;
		return true;
	};

	bool operator=(struct S3SIGNAGE_TEXT_SETTING &mediaTextSetting)
	{
		this->szMessage = mediaTextSetting.szMessage;
		
		if(pFontInfo->GetSafeHandle())pFontInfo->DeleteObject();
		
		//copy font
		LOGFONT lf;
		mediaTextSetting.pFontInfo->GetLogFont(&lf);
		pFontInfo->CreateFontIndirect(&lf);

		FontColor = mediaTextSetting.FontColor;
		FontSpeed = mediaTextSetting.FontSpeed;
		Transparency = mediaTextSetting.Transparency;
		TextDirection = mediaTextSetting.TextDirection;
		Alignment = mediaTextSetting.Alignment;
		TextLayout = mediaTextSetting.TextLayout;   
		FontDirection = mediaTextSetting.FontDirection;
		fScale = mediaTextSetting.fScale;
		szMultiLineMessage = mediaTextSetting.szMultiLineMessage;
		return true;
	};

    //operator ==  for compare "txt setting" data - zxy
    bool operator==(struct S3SIGNAGE_TEXT_SETTING &mediaTextSetting)
    {
        if(szMessage == mediaTextSetting.szMessage &&
           FontColor == mediaTextSetting.FontColor &&
           FontSpeed == mediaTextSetting.FontSpeed &&
           Transparency == mediaTextSetting.Transparency &&
           TextDirection == mediaTextSetting.TextDirection &&
           Alignment == mediaTextSetting.Alignment &&
           TextLayout == mediaTextSetting.TextLayout &&
           FontDirection == mediaTextSetting.FontDirection &&  
           fScale == mediaTextSetting.fScale &&
           szMultiLineMessage == mediaTextSetting.szMultiLineMessage)
        {
            //null point compare.
            if( NULL == this->pFontInfo || NULL == mediaTextSetting.pFontInfo)
            {
                if(this->pFontInfo  == mediaTextSetting.pFontInfo)
                    return true;
                return false;
            }

            LOGFONT lg1,lg2;
            this->pFontInfo->GetLogFont(&lg1);
            mediaTextSetting.pFontInfo->GetLogFont(&lg2);

            if(memcmp(&lg1, &lg2 ,sizeof(LOGFONT) - 32 *sizeof(TCHAR)) == 0 && 
                0 ==_tcscmp(lg1.lfFaceName ,lg2.lfFaceName))
                return true;
        }

        return false;
    };

    CString                     szMessage;
    CFont*                      pFontInfo;
    COLORREF                    FontColor;
    DWORD                       FontSpeed;
    DWORD                       Transparency;
    CString                     TextDirection;
    CString                     Alignment; // useless
    CString                     TextLayout;
    CString                     FontDirection;
    FLOAT                       fScale;
	CString                     szMultiLineMessage;
}S3SIGNAGE_TEXT_SETTING, *PS3SIGNAGE_TEXT_SETTING;

#if 0
typedef struct _MEDIASETTING
{
    _MEDIASETTING()
    {
        nMediaID            = -1;
        MediaName           = _T("Media");
        MediaType           = _T("Movie");
        MediaFile           = _T("");
        MessageFile         = _T("");
        Position.SetRectEmpty();
        StartTime           = 0;
        Duration            = 30;
		RefreshInterval     = 600;
        LoopCount           = 0;
        VolumeCount         = 100;
        bKeepAspect         = FALSE;
        BGColor             = 0xFFFFFFFF;
        pTextSetting        = NULL;
        pDesigner2          = NULL;
        pDesigner           = NULL;
    }
	bool operator=(struct _MEDIASETTING *pMediaSetting)
	{
		if(this != pMediaSetting && pMediaSetting)
			 *this = *pMediaSetting;
		return true;
	};
	bool operator=(struct _MEDIASETTING &mediaSetting)
	{
		nMediaID = mediaSetting.nMediaID;
		MediaName = mediaSetting.MediaName;
		MediaType = mediaSetting.MediaType;
		MediaFile = mediaSetting.MediaFile;
		MessageFile = mediaSetting.MessageFile;
		Position = mediaSetting.Position;
		StartTime = mediaSetting.StartTime;
		Duration = mediaSetting.Duration;
		RefreshInterval = mediaSetting.RefreshInterval;
		LoopCount = mediaSetting.LoopCount;
		VolumeCount = mediaSetting.VolumeCount;
		bKeepAspect = mediaSetting.bKeepAspect;
		BGColor = mediaSetting.BGColor;
		TransIn = mediaSetting.TransIn;
		TransOut = mediaSetting.TransOut;
		pTextSetting = mediaSetting.pTextSetting;
		pDesigner2 = mediaSetting.pDesigner2;
		pDesigner = mediaSetting.pDesigner;
		return true;
	};

    bool operator==(struct _MEDIASETTING &mediaSetting)
    {
        if(nMediaID == mediaSetting.nMediaID &&
            MediaName == mediaSetting.MediaName &&
            MediaType == mediaSetting.MediaType &&
            MediaFile == mediaSetting.MediaFile &&
            MessageFile == mediaSetting.MessageFile &&
            Position == mediaSetting.Position &&
            StartTime == mediaSetting.StartTime &&
            Duration == mediaSetting.Duration &&
            RefreshInterval == mediaSetting.RefreshInterval &&
            LoopCount == mediaSetting.LoopCount &&
            VolumeCount == mediaSetting.VolumeCount &&
            bKeepAspect == mediaSetting.bKeepAspect &&
            BGColor == mediaSetting.BGColor &&
            TransIn == mediaSetting.TransIn &&
            TransOut == mediaSetting.TransOut &&
            pTextSetting == mediaSetting.pTextSetting)
        {
           // if(pDesigner2 == mediaSetting.pDesigner2 &&  //how to judge  pDesigner is equral ?
               // pDesigner == mediaSetting.pDesigner )
            return true;

             
        }
        return false;
    };

    INT                             nMediaID;
    CString                         MediaName;
    CString                         MediaType;
    CString                         MediaFile;
    CString                         MessageFile;
    CRect                           Position;
    DWORD                           StartTime;
    DWORD                           Duration;
	DWORD                           RefreshInterval;
    DWORD                           LoopCount;
    DWORD                           VolumeCount;
    BOOL                            bKeepAspect;
    COLORREF                        BGColor;
    S3SIGNAGE_TRANSIION             TransIn;
    S3SIGNAGE_TRANSIION             TransOut;
    PS3SIGNAGE_TEXT_SETTING         pTextSetting;
    std::shared_ptr<IS3RODesigner2>         pDesigner2;
    std::shared_ptr<IS3RODesigner>          pDesigner;
} MEDIASETTING;

typedef CList<MEDIASETTING*, MEDIASETTING*> LAYERINFO;


typedef CMap<CS3DrawObj*, CS3DrawObj*, LAYERINFO*, LAYERINFO*> S3SIGNAGE_LAYER;

typedef struct _LAYOUT
{
    _LAYOUT()
    {
        LayoutName          = Translate(_T("Default"));
        COleDateTime createLayoutTime = COleDateTime::GetCurrentTime();
        CString tmp(createLayoutTime.Format(_T("_%Y%m%d_%H%M%S")));
        LayoutName += tmp;
        Description = LayoutName;
        LayoutDuration      = 0;
	//	RefreshInterval     = 0;
        GroupID             = 1;
        //Description         = Translate(_T("Default layout"));
        ClientID            = 0;
        LayoutSize.SetSize(1920, 1080);
        bKeepAspect         = FALSE;
        BGColor             = RGB(255,255,255);
        LayoutFile          = _T("");
        MediaContentSize    = 0;
        MediaCount          = 0;
    }

    CString         LayoutName;
    DWORD           LayoutDuration;
 //   DWORD           RefreshInterval;
    DWORD           GroupID;
    CString         Description;
    DWORD           ClientID;
    CSize           LayoutSize;
    BOOL            bKeepAspect;
    COLORREF        BGColor;
    CString         LayoutFile;
    DWORD64         MediaContentSize;
    DWORD           MediaCount;
    S3SIGNAGE_LAYER Layers;
} LAYOUT;
#endif

#define PI 3.1415926535897932384626433832795
#define PI_2 1.5707963267948966192313216916398
#define PI_3 1.0471975511965977461542144610932

void RotatePoint(POINTF ptCenter, POINTF& ptR, float Degree);
CRect GetRotatedScreen(FLOAT width, FLOAT height, FLOAT Degree);

int ConvertStr2Duration(CString str);
BOOL ConvertStr2BOOL(CString str);
