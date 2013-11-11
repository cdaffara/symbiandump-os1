// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "tdisplaymode_mapping.h"

const TPtrC KPixelFormatNames[] = 
	{
	_L("EUidPixelFormatUnknown"),              //0
	_L("EUidPixelFormatXRGB_8888"),            //1
	_L("EUidPixelFormatARGB_8888"),            //2
	_L("EUidPixelFormatARGB_8888_PRE"),        //3
	_L("EUidPixelFormatRGB_888"),              //4
	_L("EUidPixelFormatA_8"),                  //5
	_L("EUidPixelFormatRGB_565"),              //6
	_L("EUidPixelFormatXRGB_4444"),            //7
	_L("EUidPixelFormatP_8"),                  //8
	_L("EUidPixelFormatP_4"),                  //9
	_L("EUidPixelFormatL_8"),                  //10
	_L("EUidPixelFormatL_4"),                  //11
	_L("EUidPixelFormatL_2"),                  //12
	_L("EUidPixelFormatL_1"),                  //13
	};

const TPtrC KShortPixelFormatNames[] = 
	{
	_L("Unknown"),              //0
	_L("XRGB_8888"),            //1
	_L("ARGB_8888"),            //2
	_L("ARGB_8888_PRE"),        //3
	_L("RGB_888"),              //4
	_L("A_8"),                  //5
	_L("RGB_565"),              //6
	_L("XRGB_4444"),            //7
	_L("P_8"),                  //8
	_L("P_4"),                  //9
	_L("L_8"),                  //10
	_L("L_4"),                  //11
	_L("L_2"),                  //12
	_L("L_1"),                  //13
	};

/**
Convert the string of pixel format to TDisplayMode
@param aPixelformatString The input string of pixel format
@return On return, it contains a display mode enum value
 */
TDisplayMode TDisplayModeMapping::ConvertPixelFormatStringToDisplayMode(const TPtrC aPixelformatString)
	{
	TUidPixelFormat format = ConvertPixelFormatStringToPixelFormat(aPixelformatString);
	
	return MapPixelFormatToDisplayMode(format);	
	}

/**
Converts a pixel format string to TUidPixelFormat
@see ConvertPixelformatToShortPixelFormatString()
@param aPixelformatString The pixel format string to convert
@return Returns a pixel format enum value TUidPixelFormat
 */
TUidPixelFormat TDisplayModeMapping::ConvertPixelFormatStringToPixelFormat(const TPtrC aPixelformatString)
	{
	TUidPixelFormat format = EUidPixelFormatUnknown;
	
	if (aPixelformatString == KPixelFormatNames[1])
		{
		format =  EUidPixelFormatXRGB_8888;
		}
	else if (aPixelformatString == KPixelFormatNames[2])
		{
		format =  EUidPixelFormatARGB_8888;
		}
	else if (aPixelformatString == KPixelFormatNames[3])
		{
		format =  EUidPixelFormatARGB_8888_PRE;
		}
	else if (aPixelformatString == KPixelFormatNames[4])
		{
		format =  EUidPixelFormatRGB_888;
		}
	else if (aPixelformatString == KPixelFormatNames[5])
		{
		format =  EUidPixelFormatA_8;
		}
	else if (aPixelformatString == KPixelFormatNames[6])
		{
		format =  EUidPixelFormatRGB_565;
		}
	else if (aPixelformatString == KPixelFormatNames[7])
		{
		format =  EUidPixelFormatXRGB_4444;
		}
	else if (aPixelformatString == KPixelFormatNames[8])
		{
		format =  EUidPixelFormatP_8;
		}
	else if (aPixelformatString == KPixelFormatNames[9])
		{
		format =  EUidPixelFormatP_4;
		}
	else if (aPixelformatString == KPixelFormatNames[10])
		{
		format =  EUidPixelFormatL_8;
		}
	else if (aPixelformatString == KPixelFormatNames[11])
		{
		format =  EUidPixelFormatL_4;
		}
	else if (aPixelformatString == KPixelFormatNames[12])
		{
		format =  EUidPixelFormatL_2;
		}
	else if (aPixelformatString == KPixelFormatNames[13])
		{
		format =  EUidPixelFormatL_1;
		}
	
	return format;
	}

/*
Convert the pixel format enum to a string
@param aFormat The pixel format enum
@return On return, it contains the pixel format string
 */
TPtrC TDisplayModeMapping::ConvertPixelFormatToPixelFormatString(const TUidPixelFormat aFormat)
	{
	TPtrC pixelFormatString(KPixelFormatNames[0]);
	
	if (aFormat == EUidPixelFormatXRGB_8888)
		{
		pixelFormatString.Set(KPixelFormatNames[1]);
		}
	else if (aFormat == EUidPixelFormatARGB_8888)
		{
		pixelFormatString.Set(KPixelFormatNames[2]);
		}
	else if (aFormat == EUidPixelFormatARGB_8888_PRE)
		{
		pixelFormatString.Set(KPixelFormatNames[3]);
		}
	else if (aFormat == EUidPixelFormatRGB_888)
		{
		pixelFormatString.Set(KPixelFormatNames[4]);
		}
	else if (aFormat == EUidPixelFormatA_8)
		{
		pixelFormatString.Set(KPixelFormatNames[5]);
		}
	else if (aFormat == EUidPixelFormatRGB_565)
		{
		pixelFormatString.Set(KPixelFormatNames[6]);
		}
	else if (aFormat == EUidPixelFormatXRGB_4444)
		{
		pixelFormatString.Set(KPixelFormatNames[7]);
		}
	else if (aFormat == EUidPixelFormatP_8)
		{
		pixelFormatString.Set(KPixelFormatNames[8]);
		}
	else if (aFormat == EUidPixelFormatP_4)
		{
		pixelFormatString.Set(KPixelFormatNames[9]);
		}
	else if (aFormat == EUidPixelFormatL_8)
		{
		pixelFormatString.Set(KPixelFormatNames[10]);
		}
	else if (aFormat == EUidPixelFormatL_4)
		{
		pixelFormatString.Set(KPixelFormatNames[11]);
		}
	else if (aFormat == EUidPixelFormatL_2)
		{
		pixelFormatString.Set(KPixelFormatNames[12]);
		}
	else if (aFormat == EUidPixelFormatL_1)
		{
		pixelFormatString.Set(KPixelFormatNames[13]);
		}	
	
	return pixelFormatString;
	}

/*
Convert the pixel format enum to a shorter string than is achieved with ConvertPixelformatToPixelFormatString().
The EUidPixelFormat portion of the pixel format string is not added to the string. 
@see ConvertPixelformatToPixelFormatString()
@param aFormat The pixel format enum
@return On return, it contains the pixel format string
 */
TPtrC TDisplayModeMapping::ConvertPixelFormatToShortPixelFormatString(const TUidPixelFormat aFormat)
	{
	TPtrC pixelFormatString(KShortPixelFormatNames[0]);
	
	if (aFormat == EUidPixelFormatXRGB_8888)
		{
		pixelFormatString.Set(KShortPixelFormatNames[1]);
		}
	else if (aFormat == EUidPixelFormatARGB_8888)
		{
		pixelFormatString.Set(KShortPixelFormatNames[2]);
		}
	else if (aFormat == EUidPixelFormatARGB_8888_PRE)
		{
		pixelFormatString.Set(KShortPixelFormatNames[3]);
		}
	else if (aFormat == EUidPixelFormatRGB_888)
		{
		pixelFormatString.Set(KShortPixelFormatNames[4]);
		}
	else if (aFormat == EUidPixelFormatA_8)
		{
		pixelFormatString.Set(KShortPixelFormatNames[5]);
		}
	else if (aFormat == EUidPixelFormatRGB_565)
		{
		pixelFormatString.Set(KShortPixelFormatNames[6]);
		}
	else if (aFormat == EUidPixelFormatXRGB_4444)
		{
		pixelFormatString.Set(KShortPixelFormatNames[7]);
		}
	else if (aFormat == EUidPixelFormatP_8)
		{
		pixelFormatString.Set(KShortPixelFormatNames[8]);
		}
	else if (aFormat == EUidPixelFormatP_4)
		{
		pixelFormatString.Set(KShortPixelFormatNames[9]);
		}
	else if (aFormat == EUidPixelFormatL_8)
		{
		pixelFormatString.Set(KShortPixelFormatNames[10]);
		}
	else if (aFormat == EUidPixelFormatL_4)
		{
		pixelFormatString.Set(KShortPixelFormatNames[11]);
		}
	else if (aFormat == EUidPixelFormatL_2)
		{
		pixelFormatString.Set(KShortPixelFormatNames[12]);
		}
	else if (aFormat == EUidPixelFormatL_1)
		{
		pixelFormatString.Set(KShortPixelFormatNames[13]);
		}	
	
	return pixelFormatString;
	}

/*
Maps a pixel format enum to a display mode enum
@param aFormat The pixel format enum
@return Returns the display mode enum
 */
TDisplayMode TDisplayModeMapping::MapPixelFormatToDisplayMode(const TUidPixelFormat aFormat)
	{
	TDisplayMode mode;
	
	switch(aFormat)
	{
	/** Display mode with alpha (24bpp colour plus 8bpp alpha) */
	case EUidPixelFormatXRGB_8888:
		mode = EColor16MU; 
		break;
	
	/** Display mode with alpha (24bpp colour plus 8bpp alpha) */
	case EUidPixelFormatARGB_8888:
		mode = EColor16MA; 
		break;

	/** Pre-multiplied Alpha display mode (24bpp color multiplied with the alpha channel value, plus 8bpp alpha) */
	case EUidPixelFormatARGB_8888_PRE:
		mode = EColor16MAP; 
		break;

	/** True colour display mode (24 bpp) */
	case EUidPixelFormatRGB_888:
		mode = EColor16M; 
		break;
	
	case EUidPixelFormatXRGB_4444:
		mode = EColor4K; 
		break;

	/** 64,000 colour display mode (16 bpp) */
	case EUidPixelFormatRGB_565:
		mode = EColor64K; 
		break;
		
	/** 256 grayscales display mode (8 bpp) */	
	case EUidPixelFormatA_8:
		mode = EGray256;
		break;

	/** 256 grayscales (8 bpp) */
	case EUidPixelFormatL_8:
		mode = EGray256;
		break;
			
	/** Eight grayscales (4 bpp) */
	case EUidPixelFormatL_4:
	  	mode = EGray16;
		break;
		
	/** Four grayscales display mode (2 bpp) */
	case EUidPixelFormatL_2:
		mode = EGray4;
		break;
		
	/** Two grayscales (black and white) display mode (1 bpp) */
	case EUidPixelFormatL_1:
		mode = EGray2;
		break;

	/** 256 colour display mode (8 bpp) */
	case EUidPixelFormatP_8:
		mode = EColor256;
		break;
			
	/** Low colour EGA 16 colour display mode (4 bpp) */
	case EUidPixelFormatP_4:
	  	mode = EColor16;
		break;
		
	/** Monochrome display mode (1 bpp) */
	case EUidPixelFormatP_1:
		mode = EGray2;
		break;
		
	// Unknown display mode
	default:
	case EUidPixelFormatUnknown:
		mode = ENone; 
		break;
	}
	
	return mode;
	}

/*
Map display mode enum to pixel formation enum
@param aDisplayMode The display mode enum
@return On return, it contains the pixel format enum
 */
TUidPixelFormat TDisplayModeMapping::MapDisplayModeToPixelFormat(const TDisplayMode aDisplayMode)
	{
	TUidPixelFormat format;
	
	switch(aDisplayMode)
	{
	/** Display mode with alpha (24bpp colour plus 8bpp alpha) */
	case EColor16MU:
		format = EUidPixelFormatXRGB_8888; 
		break;
	
	/** Display mode with alpha (24bpp colour plus 8bpp alpha) */
	case EColor16MA:
		format = EUidPixelFormatARGB_8888; 
		break;

	/** Pre-multiplied Alpha display mode (24bpp color multiplied with the alpha channel value, plus 8bpp alpha) */
	case EColor16MAP:
		format = EUidPixelFormatARGB_8888_PRE; 
		break;

	/** True colour display mode (24 bpp) */
	case EColor16M:
		format = EUidPixelFormatRGB_888; 
		break;
	
	case EColor4K:
		format = EUidPixelFormatXRGB_4444;
		break;

	/** 64,000 colour display mode (16 bpp) */
	case EColor64K:
		format = EUidPixelFormatRGB_565; 
		break;
	
		/** 256 grayscale display mode (8 bpp) */
	case EGray256:
		format = EUidPixelFormatL_8;
		break;
		
	/** Sixteen grayscales display mode (4 bpp) */
	case EGray16:
		format = EUidPixelFormatL_4;
		break;

	/** Four grayscales display mode (2 bpp) */
	case EGray4:
		format = EUidPixelFormatL_2;
		break;

	/** Monochrome display mode (1 bpp) */
	case EGray2:
		format = EUidPixelFormatL_1;
		break;
		
	/** 256 Color paletised (8 bpp) */
	case EColor256:
		format = EUidPixelFormatP_8;
		break;

	/** Sixteen Colour paletised (4 bpp)*/
	case EColor16:
		format = EUidPixelFormatP_4;
		break;

	// Unknown display mode
	default:
	case ENone:
		format = EUidPixelFormatUnknown; 
		break;
	}
	
	return format;
	}

