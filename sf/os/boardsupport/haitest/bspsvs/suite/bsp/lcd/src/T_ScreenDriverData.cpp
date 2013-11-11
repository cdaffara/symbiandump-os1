/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "T_ScreenDriverData.h"

#define MICROSECS_TO_SECS(x)  (x*1000000)

const TInt	BPP8					=8;
const TInt	BPP12					=12;
const TInt	BPP16					=16;
const TInt	BPP24					=24;
const TInt	BPP32					=32;

const TInt	KDefaultBoxLength		=16;
const TInt	KDefaultBoxWidth		=16;
const TInt	KDefaultBoxStep			=16;
const TInt	KDefaultMargin			=16;
const TInt	KDefaultFrameDelay		=20000;
const TInt	KDefaultScreenDelay		=5;



//These are for masks and shifts for rgb 332 data format
/*@{*/
_LIT(KEnumDisplayState,						"EDisplayState");
_LIT(KEnumBacklightState,					"EBacklightState");
_LIT(KEnumBacklight,						"EBacklight");
_LIT(KEnumDisplayMaxBrightness,				"EDisplayBrightnessMax");
_LIT(KEnumDisplayBrightness,				"EDisplayBrightness");
_LIT(KEnumDisplayMaxContrast,				"EDisplayContrastMax");
_LIT(KEnumDisplayContrast,					"EDisplayContrast");
_LIT(KEnumDisplayMemoryAddress,				"EDisplayMemoryAddress");
_LIT(KEnumDisplayIsPixelOrderRGB,			"EDisplayIsPixelOrderRGB");
_LIT(KEnumDisplayIsPixelOrderLandscape,		"EDisplayIsPixelOrderLandscape");
_LIT(KEnumDisplayMode,						"EDisplayMode");
_LIT(KEnumDisplayNumModes,					"EDisplayNumModes");
_LIT(KEnumDisplayColors,					"EDisplayColors");
_LIT(KEnumDisplayPaletteEntry,				"EDisplayPaletteEntry");
_LIT(KEnumDisplayXPixels,					"EDisplayXPixels");
_LIT(KEnumDisplayYPixels,					"EDisplayYPixels");
_LIT(KEnumDisplayXTwips,					"EDisplayXTwips");
_LIT(KEnumDisplayYTwips,					"EDisplayYTwips");
_LIT(KEnumDisplayBitsPerPixel,				"EDisplayBitsPerPixel");
_LIT(KEnumDisplayOffsetToFirstPixel,		"EDisplayOffsetToFirstPixel");
_LIT(KEnumDisplayOffsetBetweenLines,		"EDisplayOffsetBetweenLines");
_LIT(KEnumDisplayIsPalettized,				"EDisplayIsPalettized");
_LIT(KEnumDisplayNumberOfScreens,			"EDisplayNumberOfScreens");
_LIT(KEnumNull,								"");

_LIT(KFormatEntryField,						"%S_%S");

_LIT(KFldColor,								"color");
_LIT(KFldColorBackground,					"background");
_LIT(KFldColorForeground,					"foreground");
_LIT(KFldColorIndex,						"color%d");
_LIT(KFldFrameDelay,						"framedelay");
_LIT(KFldEndIndex,							"endIndex");
_LIT(KFldLength,							"length");
_LIT(KFldMargin,							"margin");
_LIT(KFldMask,								"mask");
_LIT(KFldShift,								"shift");
_LIT(KFldStartIndex,						"startIndex");
_LIT(KFldStep,								"step");
_LIT(KFldWidth,								"width");

_LIT(KTagRgbRed,							"red");
_LIT(KTagRgbGreen,							"green");
_LIT(KTagRgbBlue,							"blue");
_LIT(KTagRgbAlpha,							"alpha");

_LIT(KCmdUtilityFillColor,					"utilityFillColor");
_LIT(KCmdUtilityFillMultiColor,				"utilityFillMultiColor");
_LIT(KCmdUtilityMovingRectangle,			"utilityMovingRectangle");
_LIT(KCmdUtilityFillHorizontalStrip,		"utilityFillHorizontalStrip");
_LIT(KCmdUtilityFillVerticalStrip,			"utilityFillVerticalStrip");
_LIT(KCmdUtilityPromptMessage,	  			  "utilityPromptMessage");

_LIT(KScreenDelay,					"screenDelay");

_LIT(KLogInvalidLength,						"Invalid length %d. Must be >0.");
_LIT(KLogInvalidMargin,						"Invalid margin %d. Must be >0.");
_LIT(KLogInvalidStep,						"Invalid step %d. Must be >0.");
_LIT(KLogInvalidWidth,						"Invalid width %d. Must be >0.");
_LIT(KLogMissingParameter,					"Missing parameter '%S'");
_LIT(KLogNotEnoughData,						"Not enough data");
/*@}*/

const CT_HALData::THalTableLookup	CT_ScreenDriverData::iTableLookup[] =
	{
//	Enum as a descriptor				Enum									Prepare input	Prepare paramters prior	Validation of		Store data returned after a			Flag indicating
//																				value prior to	to HAL::Get call		value returned		successful HAL::Get or				if validation
//																				HAL::Set call							after a successful	HAL::Set call						is mandatory
//																														HAL::Get call
	KEnumDisplayState,					HALData::EDisplayState,					SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayState,					EFalse,
	KEnumBacklightState,				HALData::EBacklightState,				SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetBacklightState,					EFalse,
	KEnumBacklight,					HALData::EBacklight,					SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetBacklight,						EFalse,
	KEnumDisplayBrightness,			HALData::EDisplayBrightness,			SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayMaxBrightness,			EFalse,
	KEnumDisplayMaxBrightness,			HALData::EDisplayBrightnessMax,			SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayBrightness,				EFalse,
	KEnumDisplayContrast,				HALData::EDisplayContrast,				SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayMaxContrast,				EFalse,
	KEnumDisplayMaxContrast,			HALData::EDisplayContrastMax,			SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayContrast,					EFalse,
	KEnumDisplayMemoryAddress,			HALData::EDisplayMemoryAddress,			SetPrepareUint,	GetPrepareDisplayMode,	GetValidationUint,	SetDisplayMemoryAddress,			EFalse,
	KEnumDisplayIsPixelOrderLandscape,	HALData::EDisplayIsPixelOrderLandscape,	SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayIsPixelOrderRGB,			EFalse,
	KEnumDisplayIsPixelOrderRGB,		HALData::EDisplayIsPixelOrderRGB,		SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayIsPixelOrderLandscape,	EFalse,
	KEnumDisplayMode,					HALData::EDisplayMode,					SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayMode,						EFalse,
	KEnumDisplayNumModes,				HALData::EDisplayNumModes,				SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayNumModes,					EFalse,
	KEnumDisplayColors,				HALData::EDisplayColors,				SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayColors,					EFalse,
	KEnumDisplayPaletteEntry,			HALData::EDisplayPaletteEntry,			SetPrepareInt,	GetPrepareColorIndex,	GetValidationInt,	SetDisplayPaletteEntry,				EFalse,
	KEnumDisplayXPixels,				HALData::EDisplayXPixels,				SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayXPixels,					EFalse,
	KEnumDisplayYPixels,				HALData::EDisplayYPixels,				SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayYPixels,					EFalse,
	KEnumDisplayXTwips,				HALData::EDisplayXTwips,				SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayXTwips,					EFalse,
	KEnumDisplayYTwips,				HALData::EDisplayYTwips,				SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayYTwips,					EFalse,
	KEnumDisplayBitsPerPixel,			HALData::EDisplayBitsPerPixel,			SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayBitsPerPixel,				EFalse,
	KEnumDisplayOffsetToFirstPixel,	HALData::EDisplayOffsetToFirstPixel,	SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayOffsetToFirstPixel,		EFalse,
	KEnumDisplayOffsetBetweenLines,	HALData::EDisplayOffsetBetweenLines,	SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayOffsetBetweenLines,		EFalse,
	KEnumDisplayIsPalettized,			HALData::EDisplayIsPalettized,			SetPrepareInt,	GetPrepareDisplayMode,	GetValidationBool,	SetDisplayIsPalettized,				EFalse,
	KEnumDisplayNumberOfScreens,		HALData::EDisplayNumberOfScreens,		SetPrepareInt,	GetPrepareDisplayMode,	GetValidationInt,	SetDisplayNumberOfScreens,			EFalse,
	KEnumNull,							(HALData::TAttribute)0,					NULL,			NULL,					NULL,				NULL,								NULL
	};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_ScreenDriverData* CT_ScreenDriverData::NewL()
	{
	CT_ScreenDriverData*	ret=new (ELeave) CT_ScreenDriverData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


CT_ScreenDriverData::CT_ScreenDriverData()
:	CT_HALData(iTableLookup)
,	iDisplayState(-1)
,	iBacklightState(-1)
,	iBacklight(-1)
,	iDisplayMaxBrightness(-1)
,	iDisplayBrightness(-1)
,	iDisplayMaxContrast(-1)
,	iDisplayContrast(-1)
,	iDisplayMemoryAddress((TUint8*)(-1))
,	iDisplayIsPixelOrderRGB(-1)
,	iDisplayIsPixelOrderLandscape(-1)
,	iDisplayMode(-1)
,	iDisplayNumModes(-1)
,	iDisplayColors(-1)
,	iDisplayPaletteEntry(-1)
,	iDisplayXPixels(-1)
,	iDisplayYPixels(-1)
,	iDisplayXTwips(-1)
,	iDisplayYTwips(-1)
,	iDisplayBitsPerPixel(-1)
,	iDisplayOffsetToFirstPixel(-1)
,	iDisplayOffsetBetweenLines(-1)
,	iDisplayIsPalettized(EFalse)
,	iDisplayNumberOfScreens(-1)
	{
	}

void CT_ScreenDriverData::ConstructL()
/**
 * Second phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
	{
	}

CT_ScreenDriverData::~CT_ScreenDriverData()
/**
 * Public destructor
 */
	{
	iPalette.Close();
	}

TAny* CT_ScreenDriverData::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
	{
	return NULL;
	}

//	Prepare the range of values that are getting as color indexs
TBool CT_ScreenDriverData::GetPrepareColorIndex(CDataWrapperBase* aThis, const TDesC& aSection, TInt& aValueStart, TInt& aValueEnd)
	{
	CT_ScreenDriverData*	thisPtr=static_cast<CT_ScreenDriverData*>(aThis);
	TBool	ret=thisPtr->GetIntFromConfig(aSection, KFldStartIndex(), aValueStart);
	if ( !ret )
		{
		thisPtr->ERR_PRINTF2(KLogMissingParameter, &KFldStartIndex());
		thisPtr->SetBlockResult(EFail);
		}

	aValueEnd=aValueStart;
	thisPtr->GetIntFromConfig(aSection, KFldEndIndex(), aValueEnd);

	return ret;
	}

//	Prepare the range of values that are getting as just the display mode
TBool CT_ScreenDriverData::GetPrepareDisplayMode(CDataWrapperBase* aThis, const TDesC& /*aSection*/, TInt& aValueStart, TInt& aValueEnd)
	{
	aValueStart=aValueEnd=static_cast<CT_ScreenDriverData*>(aThis)->iDisplayMode;

	return ETrue;
	}

//	Store the display state
void CT_ScreenDriverData::SetDisplayState(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayState=aValue;
	}

//	Store the backlight state
void CT_ScreenDriverData::SetBacklightState(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iBacklightState=aValue;
	}

//	Store the backlight
void CT_ScreenDriverData::SetBacklight(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iBacklight=aValue;
	}

//	Store the displays maximum brightness
void CT_ScreenDriverData::SetDisplayMaxBrightness(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayMaxBrightness=aValue;
	}

//	Store the displays brightness
void CT_ScreenDriverData::SetDisplayBrightness(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayBrightness=aValue;
	}

//	Store the displays maximum contrast
void CT_ScreenDriverData::SetDisplayMaxContrast(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayMaxContrast=aValue;
	}

//	Store the displays contrast
void CT_ScreenDriverData::SetDisplayContrast(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayContrast=aValue;
	}

//	Store the displays memory address
void CT_ScreenDriverData::SetDisplayMemoryAddress(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayMemoryAddress=(TUint8*)(aValue);
	}

//	Store the displays pixel RGB order flag
void CT_ScreenDriverData::SetDisplayIsPixelOrderRGB(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayIsPixelOrderRGB=aValue;
	}

//	Store the displays landscape pixel order flag
void CT_ScreenDriverData::SetDisplayIsPixelOrderLandscape(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayIsPixelOrderLandscape=aValue;
	}

//	Store the displays mode
void CT_ScreenDriverData::SetDisplayMode(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayMode=aValue;
	}

//	Store the displays supported number of modes
void CT_ScreenDriverData::SetDisplayNumModes(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayNumModes=aValue;
	}

//	Store the displays supported number of colors
void CT_ScreenDriverData::SetDisplayColors(CDataWrapperBase* aThis, TInt aValue)
	{
	CT_ScreenDriverData*	thisPtr=static_cast<CT_ScreenDriverData*>(aThis);
	thisPtr->iDisplayColors=aValue;
	thisPtr->iPalette.Reset();
	thisPtr->iPalette.Reserve(aValue);
	}

//	Store the an entry in the displays color palette
void CT_ScreenDriverData::SetDisplayPaletteEntry(CDataWrapperBase* aThis, TInt aValue)
	{
	CT_ScreenDriverData*	thisPtr=static_cast<CT_ScreenDriverData*>(aThis);
	thisPtr->iDisplayPaletteEntry=aValue;
	if ( thisPtr->iPalette.Count()<thisPtr->iValue )
		{
		thisPtr->iPalette[thisPtr->iValue]=aValue;
		}
	}

//	Store the displays width in pixels
void CT_ScreenDriverData::SetDisplayXPixels(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayXPixels=aValue;
	}

//	Store the displays height in pixels
void CT_ScreenDriverData::SetDisplayYPixels(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayYPixels=aValue;
	}

//	Store the displays width in twips
void CT_ScreenDriverData::SetDisplayXTwips(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayXTwips=aValue;
	}

//	Store the displays height in twips
void CT_ScreenDriverData::SetDisplayYTwips(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayYTwips=aValue;
	}

//	Store the displays number of bits per pixel
void CT_ScreenDriverData::SetDisplayBitsPerPixel(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayBitsPerPixel=aValue;
	}

//	Store the displays memory address offset to the first pixel
void CT_ScreenDriverData::SetDisplayOffsetToFirstPixel(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayOffsetToFirstPixel=aValue;
	}

//	Store the displays memory address offset between successive lines
void CT_ScreenDriverData::SetDisplayOffsetBetweenLines(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayOffsetBetweenLines=aValue;
	}

//	Store the displays flag indicating if a palette is used
void CT_ScreenDriverData::SetDisplayIsPalettized(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayIsPalettized=(aValue!=0);
	}

//	Store the devices number of screens supported
void CT_ScreenDriverData::SetDisplayNumberOfScreens(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_ScreenDriverData*>(aThis)->iDisplayNumberOfScreens=aValue;
	}
TBool CT_ScreenDriverData::GetRgbFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUint& aRed, TUint& aGreen, TUint& aBlue, TUint& aAlpha)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	tempStore.Format(KFormatEntryField, &aKeyName, &KTagRgbRed);
	TBool	ret=GetUintFromConfig(aSectName, tempStore, aRed);

	tempStore.Format(KFormatEntryField, &aKeyName, &KTagRgbGreen);
	if ( !GetUintFromConfig(aSectName, tempStore, aGreen) )
		{
		ret=EFalse;
		}

	tempStore.Format(KFormatEntryField, &aKeyName, &KTagRgbBlue);
	if ( !GetUintFromConfig(aSectName, tempStore, aBlue) )
		{
		ret=EFalse;
		}

	aAlpha=0;
	tempStore.Format(KFormatEntryField, &aKeyName, &KTagRgbAlpha);
	GetUintFromConfig(aSectName, tempStore, aAlpha);

	return ret;
	}

TBool CT_ScreenDriverData::GetRgbFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aRed, TInt& aGreen, TInt& aBlue, TInt& aAlpha)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	tempStore.Format(KFormatEntryField, &aKeyName, &KTagRgbRed);
	TBool	ret=GetIntFromConfig(aSectName, tempStore, aRed);

	tempStore.Format(KFormatEntryField, &aKeyName, &KTagRgbGreen);
	if ( !GetIntFromConfig(aSectName, tempStore, aGreen) )
		{
		ret=EFalse;
		}

	tempStore.Format(KFormatEntryField, &aKeyName, &KTagRgbBlue);
	if ( !GetIntFromConfig(aSectName, tempStore, aBlue) )
		{
		ret=EFalse;
		}

	aAlpha=0;
	tempStore.Format(KFormatEntryField, &aKeyName, &KTagRgbAlpha);
	GetIntFromConfig(aSectName, tempStore, aAlpha);

	return ret;
	}

TBool CT_ScreenDriverData::GetColorFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUint& aColor, TPtrC& aLogColor)
	{
	TBool	dataOk=ETrue;
	
	if ( iDisplayIsPalettized )
		{
		WARN_PRINTF1(_L("Testing of Palettes not supported\n"));
		dataOk=EFalse;
		}

	TUint	colorRed;
	TUint	colorGreen;
	TUint	colorBlue;
	TUint	colorAlpha;
	if ( !GetRgbFromConfig(aSectName, aKeyName, colorRed, colorGreen, colorBlue, colorAlpha) )
		{
		WARN_PRINTF2(KLogMissingParameter, &aKeyName);
		dataOk=EFalse;
		}

	if ( !GetStringFromConfig(aSectName, aKeyName, aLogColor) )
		{
		WARN_PRINTF2(KLogMissingParameter, &aKeyName);
		dataOk=EFalse;
		}

	TUint	maskRed;
	TUint	maskGreen;
	TUint	maskBlue;
	TUint	maskAlpha;
	if ( !GetRgbFromConfig(aSectName, KFldMask(), maskRed, maskGreen, maskBlue, maskAlpha) )
		{
		WARN_PRINTF2(KLogMissingParameter, &KFldMask());
		dataOk=EFalse;
		}

	TInt	shiftRed;
	TInt	shiftGreen;
	TInt	shiftBlue;
	TInt	shiftAlpha;
	if ( !GetRgbFromConfig(aSectName, KFldShift(), shiftRed, shiftGreen, shiftBlue, shiftAlpha) )
		{
		WARN_PRINTF2(KLogMissingParameter, &KFldShift());
		dataOk=EFalse;
		}

	aColor=0;

	//	Apply Red
	colorRed	&= maskRed;
	if ( shiftRed<0 )
		{
		aColor |= (colorRed >> -shiftRed);
		}
	else
		{
		aColor |= (colorRed << shiftRed);
		}

	//	Apply Green
	colorGreen	&= maskGreen;
	if ( shiftGreen<0 )
		{
		aColor |= (colorGreen >> -shiftGreen);
		}
	else
		{
		aColor |= (colorGreen << shiftGreen);
		}

	//	Apply Blue
	colorBlue	&= maskBlue;
	if ( shiftBlue<0 )
		{
		aColor |= (colorBlue >> -shiftBlue);
		}
	else
		{
		aColor |= (colorBlue << shiftBlue);
		}

	//	Apply Alpha
	colorAlpha	&= maskAlpha;
	if ( shiftAlpha<0 )
		{
		aColor |= (colorAlpha >> -shiftAlpha);
		}
	else
		{
		aColor |= (colorAlpha << shiftAlpha);
		}

	return dataOk;
	}

TBool CT_ScreenDriverData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdUtilityFillColor )
		{
		DoCmdUtilityFillColorL(aSection);
		}
	else if ( aCommand==KCmdUtilityFillMultiColor )
		{
		DoCmdUtilityFillMultiColorL(aSection);
		}
	else if ( aCommand==KCmdUtilityMovingRectangle )
		{
		DoCmdUtilityMovingRectangleL(aSection);
		}
	else if ( aCommand==KCmdUtilityFillHorizontalStrip )
		{
		DoCmdUtilityFillHorizontalStripL(aSection);	
		}
	else if ( aCommand==KCmdUtilityFillVerticalStrip)
		{
		DoCmdUtilityFillVerticalStripL(aSection);	
		}
	else if ( aCommand==KCmdUtilityPromptMessage)
		{
		ret=CDataWrapperBase::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}
	else
		{
		ret=CT_HALData::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	return ret;
	}

/**
 * Fill the Screen in a single color by using screen base memory address
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_ScreenDriverData::DoCmdUtilityFillColorL(const TDesC& aSection)
	{
	
	TUint	 color;
	TPtrC	 logColor;
	TBool	 dataOk=GetColorFromConfig(aSection, KFldColor, color, logColor);
	if ( !dataOk )
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldColor());
		SetBlockResult(EFail);
		}
	else
	 	{
		TInt	pixelsX=iDisplayXPixels;
		TInt	pixelsY=iDisplayYPixels;
		TUint8*	memoryAddress=iDisplayMemoryAddress;
		memoryAddress+=iDisplayOffsetToFirstPixel;

		TInt	line=0;
		for ( line=0; line<pixelsY; ++line )
			{
			DrawLine(color, memoryAddress, 0, pixelsX);
			memoryAddress+=iDisplayOffsetBetweenLines;
			}

		}	

	//delays the image on the screen to allow for the user to verify
	TInt screenDelay;	
	TBool screenDelayOk = GetIntFromConfig(aSection, KScreenDelay(), screenDelay);
	if(!screenDelayOk)
		{

			INFO_PRINTF1(_L("defualt screen delay value used"));
			screenDelay = KDefaultScreenDelay;
		}
	CDataWrapperBase::Timedelay(MICROSECS_TO_SECS(screenDelay));
	}

/**
 * Fill the Screen color with multiple nested rectangles by using screen base memory address
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_ScreenDriverData::DoCmdUtilityFillMultiColorL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	RArray<TUint>	color;
	CleanupClosePushL(color);

	RArray<TDesC>	colorLog;
	CleanupClosePushL(colorLog);

	TBool	moreData=ETrue;
	TBool	index=0;
	while ( moreData )
		{
		tempStore.Format(KFldColorIndex(), ++index);
		TUint	colorTemp;
		TPtrC	colorLogTemp;

		moreData=GetColorFromConfig(aSection, tempStore, colorTemp, colorLogTemp);
		if ( moreData )
			{
			color.AppendL(colorTemp);
			colorLog.AppendL(colorLogTemp);
			}
		}

	//	Must have more than one color for this to work
	TBool	dataOk=color.Count()>1;
	if ( !dataOk )
		{
		ERR_PRINTF1(KLogNotEnoughData);
		SetBlockResult(EFail);
		}

	TInt	margin=KDefaultMargin;
	GetIntFromConfig(aSection, KFldMargin(), margin);
	if ( margin<=0 )
		{
		ERR_PRINTF2(KLogInvalidMargin, margin);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}

	if ( dataOk )
	 	{
		TInt	pixelsX=iDisplayXPixels;
		TInt	pixelsY=iDisplayYPixels;
		TUint8*	memoryAddress=iDisplayMemoryAddress;
		memoryAddress+=iDisplayOffsetToFirstPixel;

		TInt	colorIndex=0;
		TInt	left=0;
		TInt	pixels=pixelsX;
		TInt	top=0;
		TInt	bottom=pixelsY;
		TInt	line=0;
		while ( (left<pixels) && (top<bottom) )
			{
			TUint8*	address=memoryAddress + top*iDisplayOffsetBetweenLines;
			for ( line=top; line<bottom; ++line )
				{
				DrawLine(color[colorIndex], address, left, pixels);
				address+=iDisplayOffsetBetweenLines;
				}
			left += margin;
			pixels -= margin;
			top += margin;
			bottom -= margin;
			++colorIndex;
			if ( colorIndex>=color.Count() )
				{
				colorIndex=0;
				}
			}

		}
	CleanupStack::PopAndDestroy(2, &color);
	
	TInt screenDelay;	
	TBool screenDelayOk = GetIntFromConfig(aSection, KScreenDelay(), screenDelay);
	if(!screenDelayOk)
		{
			screenDelay = KDefaultScreenDelay;
			INFO_PRINTF1(_L("defualt screen delay value used"));
		}

	CDataWrapperBase::Timedelay(MICROSECS_TO_SECS(screenDelay));
	}

/**
 * Fill the Screen in a color and animate a rectangle moving around the perimeter of the screen
 * by using screen base memory address
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_ScreenDriverData::DoCmdUtilityMovingRectangleL(const TDesC& aSection)
	{
	TBool	dataOk=ETrue;

	TUint	colorBackground;
	TPtrC	logColorBackground;
	if ( !GetColorFromConfig(aSection, KFldColorBackground, colorBackground, logColorBackground) )
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldColorBackground());
		SetBlockResult(EFail);
		dataOk=EFalse;
		}

	TUint	colorForeground;
	TPtrC	logColorForeground;
	if ( !GetColorFromConfig(aSection, KFldColorForeground, colorForeground, logColorForeground) )
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldColorForeground());
		SetBlockResult(EFail);
		dataOk=EFalse;
		}

	TInt	boxLength=KDefaultBoxLength;
	GetIntFromConfig(aSection, KFldLength(), boxLength);
	if ( boxLength<=0 )
		{
		ERR_PRINTF2(KLogInvalidLength, boxLength);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}

	TInt	boxWidth=KDefaultBoxWidth;
	GetIntFromConfig(aSection, KFldWidth(), boxWidth);
	if ( boxWidth<=0 )
		{
		ERR_PRINTF2(KLogInvalidWidth, boxWidth);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}

	TInt	step=KDefaultBoxStep;
	GetIntFromConfig(aSection, KFldStep(), step);
	if ( step<=0 )
		{
		ERR_PRINTF2(KLogInvalidStep, step);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}

	TInt	frameDelay=KDefaultFrameDelay;
	GetIntFromConfig(aSection, KFldFrameDelay(), frameDelay);
	if ( frameDelay<=0 )
		{
		ERR_PRINTF2(KLogInvalidStep, frameDelay);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}

	if ( dataOk )
	 	{
		TInt	pixelsX=iDisplayXPixels;
		TInt	pixelsY=iDisplayYPixels;
		TUint8*	memoryAddress=iDisplayMemoryAddress;
		memoryAddress+=iDisplayOffsetToFirstPixel;

		//	Fill the background
		INFO_PRINTF1(_L("-->Fill the background"));
		TUint8*	address=memoryAddress;
		TInt	x=0;
		TInt	y=0;
		TInt	line=0;
		for ( line=0; line<pixelsY; ++line )
			{
			DrawLine(colorBackground, address, 0, pixelsX);
			address+=iDisplayOffsetBetweenLines;
			}
		INFO_PRINTF1(_L("<--Fill the background"));

		//	Draw box moving from top left to top right
		TInt	max=pixelsX-boxLength;
		for ( x=0; x<max; x+=step )
			{
			address=memoryAddress;
			for ( line=0; line<boxWidth; ++line )
				{
				DrawLine(colorForeground, address, x, boxLength+x);
				address+=iDisplayOffsetBetweenLines;
				}
			Timedelay(frameDelay);

			address=memoryAddress;
			for ( line=0; line<boxWidth; ++line )
				{
				DrawLine(colorBackground, address, x, boxLength+x);
				address+=iDisplayOffsetBetweenLines;
				}
			}

		//	Draw box moving from top right to bottom right
		//	Missing out first as it was draw at the end of the previous draw
		max=pixelsY-boxWidth;
		x=pixelsX-boxLength;
		for ( y=step; y<max; y+=step )
			{
			address=memoryAddress+y*iDisplayOffsetBetweenLines;
			for ( line=0; line<boxWidth; ++line )
				{
				DrawLine(colorForeground, address, x, pixelsX);
				address+=iDisplayOffsetBetweenLines;
				}
			Timedelay(frameDelay);

			address=memoryAddress+y*iDisplayOffsetBetweenLines;
			for ( line=0; line<boxWidth; ++line )
				{
				DrawLine(colorBackground, address, x, pixelsX);
				address+=iDisplayOffsetBetweenLines;
				}
			}

		//	Draw box moving from bottom right to bottom left
		//	Missing out first as it was draw at the end of the previous draw
		for ( x=pixelsX-boxLength; x>0; x-=step )
			{
			address=memoryAddress+pixelsY*iDisplayOffsetBetweenLines;
			for ( line=0; line<boxWidth; ++line )
				{
				address-=iDisplayOffsetBetweenLines;
				DrawLine(colorForeground, address, x, boxLength+x);
				}
			Timedelay(frameDelay);

			address=memoryAddress+pixelsY*iDisplayOffsetBetweenLines;
			for ( line=0; line<boxWidth; ++line )
				{
				address-=iDisplayOffsetBetweenLines;
				DrawLine(colorBackground, address, x, boxLength+x);
				}
			}

		//	Draw box moving from bottom right bottom left to top right
		//	Missing out first as it was draw at the end of the previous draw
		for ( y=pixelsY-boxWidth; y>0; y-=step )
			{
			address=memoryAddress+y*iDisplayOffsetBetweenLines;
			for ( line=0; line<boxWidth; ++line )
				{
				DrawLine(colorForeground, address, 0, boxLength);
				address+=iDisplayOffsetBetweenLines;
				}
			Timedelay(frameDelay);

			address=memoryAddress+y*iDisplayOffsetBetweenLines;
			for ( line=0; line<boxWidth; ++line )
				{
				DrawLine(colorBackground, address, 0, boxLength);
				address+=iDisplayOffsetBetweenLines;
				}
			}
		}
	}

/**
Utility function to Fill color in a 16pixels*16 lines area at the given xb,yb position in the screen
@param ColorVal color to be filled in .For 4 bpp mode ColorVal contains only 4 bits of information, the upper 4 bits are empty
	For 8 bpp mode all the 8 bits are filled in , the value is taken from the standard OS palette
	For all other modes ColorVal holds data in rgb 332 format.

@param aColor			Color to draw in
@param aMemoryAddress	Address offset to start of line
@param aLeftMargin		Left margin to not draw in
@param aPixelsX			Number of pixels to draw on line
*/
void CT_ScreenDriverData::DrawLine(TUint aColor, TUint8* aMemoryAddress, TInt aLeftMargin, TInt aPixelsX)
	{

	switch ( iDisplayBitsPerPixel )
		{
	case BPP32:
	case BPP24:
		{
		TUint32*	pixelPtr=(TUint32*)aMemoryAddress;
		TUint32		color=aColor;
		pixelPtr += aLeftMargin;
		for ( TInt x=aLeftMargin; x<aPixelsX; ++x )
			{
			*pixelPtr++=color;
			}
		}
		break;
		
	case BPP16:
	case BPP12:
		{
		TUint16*	pixelPtr=(TUint16*)aMemoryAddress;
		TUint16		color=aColor;
		pixelPtr += aLeftMargin;
		for ( TInt x=aLeftMargin; x<aPixelsX; ++x )
			{
			*pixelPtr++=color;
			}
		}
		break;
	case BPP8:
		{
		TUint8*	pixelPtr=aMemoryAddress;
		TUint8	color=aColor;
		pixelPtr += aLeftMargin;
		for ( TInt x=aLeftMargin; x<aPixelsX; ++x )
			{
			*pixelPtr++=color;
			}
		}
		break;
	default:
		break;
		}
	}

/**
 * Fill the Screen color with multicoloured horizontal strips using screen base memory address
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_ScreenDriverData::DoCmdUtilityFillHorizontalStripL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	RArray<TUint>	color;
	CleanupClosePushL(color);
	
	RArray<TDesC>	colorLog;
	CleanupClosePushL(colorLog);

	TBool		moreData=ETrue;
	TBool		index=0;
	while ( moreData )
		{
		tempStore.Format(KFldColorIndex(), ++index);
		TUint	colorTemp;
		TPtrC	colorLogTemp;

		moreData=GetColorFromConfig(aSection, tempStore, colorTemp, colorLogTemp);
		if ( moreData )
			{
			color.AppendL(colorTemp);
			colorLog.AppendL(colorLogTemp);
			}
		}

	//	Must have more than one color for this to work
	TBool	dataOk=color.Count()>1;
	if ( !dataOk )
		{
		ERR_PRINTF1(KLogNotEnoughData);
		SetBlockResult(EFail);
		}

	TInt	margin=KDefaultMargin;
	GetIntFromConfig(aSection, KFldMargin(), margin);
	if ( margin<=0 )
		{
		ERR_PRINTF2(KLogInvalidMargin, margin);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}

	if ( dataOk )
		{
		TInt	pixelsX=iDisplayXPixels;
		TInt	pixelsY=iDisplayYPixels;
		TUint8*	memoryAddress=iDisplayMemoryAddress;
		memoryAddress+=iDisplayOffsetToFirstPixel;
				
		TInt	count=0;
		TInt 	colorIndex=0;
		for ( TInt liney=0; liney<pixelsY; liney++ )
			{
			DrawLine(color[colorIndex], memoryAddress, 0, pixelsX);
			memoryAddress+=iDisplayOffsetBetweenLines;
			
			++count;
			if( count>margin )
				{
				++colorIndex;
				count=0;
				}
			
			if ( colorIndex>=color.Count() )
				{
				colorIndex=0;
				}
			}

		}
	CleanupStack::PopAndDestroy(2, &color);

	TInt screenDelay;	
	TBool screenDelayOk = GetIntFromConfig(aSection, KScreenDelay(), screenDelay);
	if(!screenDelayOk)
		{
			screenDelay = KDefaultScreenDelay;
			INFO_PRINTF1(_L("defualt screen delay value used"));
		}
	CDataWrapperBase::Timedelay(MICROSECS_TO_SECS(screenDelay));

	}

/**
 * Fill the Screen color with multicoloured vertical strips using screen based memory address
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_ScreenDriverData::DoCmdUtilityFillVerticalStripL(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	RArray<TUint>	color;
	CleanupClosePushL(color);

	RArray<TDesC>	colorLog;
	CleanupClosePushL(colorLog);

	TBool	moreData=ETrue;
	TBool	index=0;
	while ( moreData )
		{
		tempStore.Format(KFldColorIndex(), ++index);
		TUint	colorTemp;
		TPtrC	colorLogTemp;

		moreData=GetColorFromConfig(aSection, tempStore, colorTemp, colorLogTemp);
		if ( moreData )
			{
			color.AppendL(colorTemp);
			colorLog.AppendL(colorLogTemp);
			}
		}

	//	Must have more than one color for this to work
	TBool	dataOk=color.Count()>1;
	if ( !dataOk )
		{
		ERR_PRINTF1(KLogNotEnoughData);
		SetBlockResult(EFail);
		}

	TInt	margin=KDefaultMargin;
	GetIntFromConfig(aSection, KFldMargin(), margin);
	if ( margin<=0 )
		{
		ERR_PRINTF2(KLogInvalidMargin, margin);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}

	if ( dataOk )
	 	{
		TInt	pixelsX=iDisplayXPixels;
		TInt	pixelsY=iDisplayYPixels;
		TUint8*	memoryAddress=iDisplayMemoryAddress;
		memoryAddress+=iDisplayOffsetToFirstPixel;

		TInt	colorIndex=0;
		TUint8*	address;
		TInt	right=0;
		for ( TInt left=0; left<pixelsX; left+=margin )
			{
			address = memoryAddress;
			right += margin;
			if ( right>=pixelsX )
				{
				right=pixelsX-1;
				}
			for ( TInt liney=0; liney<pixelsY; liney++ )
				{
				DrawLine(color[colorIndex], address, left, right);
				address+=iDisplayOffsetBetweenLines;
				}
			++colorIndex;	
			if ( colorIndex>=color.Count() )
				{
				colorIndex=0;
				}
			}

		}
	CleanupStack::PopAndDestroy(2, &color);
	
	TInt screenDelay;	
	TBool screenDelayOk = GetIntFromConfig(aSection, KScreenDelay(), screenDelay);
	if(!screenDelayOk)
		{
			screenDelay = KDefaultScreenDelay;
			INFO_PRINTF1(_L("defualt screen delay value used"));
		}
	CDataWrapperBase::Timedelay(MICROSECS_TO_SECS(screenDelay));

	}


