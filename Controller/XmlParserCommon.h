#ifndef XML_PARSER_COMMON_H
#define XML_PARSER_COMMON_H
#pragma once

namespace Xml_Parser_Common
{
#if 0
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

	//------------ Layer definition begin --------------------------------//
	static const CString szTypeName_Layer        = _T("Layer");
	static const CString szTypeName_Message      = _T("Message");
	static const CString szTypeName_Video        = _T("Movie");
	static const CString szTypeName_Audio        = _T("Audio");
	static const CString szTypeName_EmptyContent = _T("EmptyContent");
	//------------ Layer definition end ----------------------------------//
	static const CString szTypeName_Text         = _T("ScrollTextFile");
	static const CString szTypeName_TextFile     = _T("TextFile");

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
#undef Translate


	typedef struct S3SIGNAGE_TRANSIION
	{
		S3SIGNAGE_TRANSIION()
		{
			Name        = szEffect[S3S_EFFECT_NONE];
			Direction   = szEffectDirection[S3S_EFFECT_DIRECTION_NONE];
			Duration    = 0;
		}

		CString     Name;
		DWORD       Duration;
		CString     Direction;
	}S3SIGNAGE_TRANSIION;


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
			return *this=*pMediaTextSetting;
		};
		bool operator=(struct S3SIGNAGE_TEXT_SETTING &mediaTextSetting)
		{
			this->szMessage = mediaTextSetting.szMessage;

			if(pFontInfo) delete pFontInfo;

			//copy font
			LOGFONT lf;
			pFontInfo = new CFont;
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

	typedef enum {
		BACKGROUND          = 0,
		LAYER               = 1,
		MESSAGE             = 2,
		AUDIO               = 3,
		USER_DEFINED_PLUGIN = 4,
		ALIGNDW             = 0xFFFFFFFF,
	} XML_PROPERTY_TYPE;

	typedef struct _S3SIGNAGE_TRANSITION_SETTING
	{
		S3S_CONTENT_EFFECT              Effect;
		ULONG                           EffectDuration;
		S3S_CONTENT_EFFECT_DIRECTION    EffectDirection;
	}S3SIGNAGE_TRANSITION_SETTING, *PS3SIGNAGE_TRANSITION_SETTING;

	typedef enum {
		BACKGROUND          = 0,
		LAYER               = 1,
		MESSAGE             = 2,
		AUDIO               = 3,
		USER_DEFINED_PLUGIN = 4,
		ALIGNDW             = 0xFFFFFFFF,
	} XML_PROPERTY_TYPE;
#endif 
	void ConvertBOOL2Str(BOOL bValue, char* pValue);
	void ConvertDuration2Str(DWORD duration, char* pValue);

};





#endif