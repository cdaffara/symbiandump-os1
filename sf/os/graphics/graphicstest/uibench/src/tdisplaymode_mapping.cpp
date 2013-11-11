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
	_L("EUidPixelFormatUnknown"),
	_L("EUidPixelFormatXRGB_8888"),
	_L("EUidPixelFormatARGB_8888"),
	_L("EUidPixelFormatARGB_8888_PRE"),
	_L("EUidPixelFormatRGB_888"),
	_L("EUidPixelFormatRGB_565"),
	_L("EUidPixelFormatP_8"),
	_L("EUidPixelFormatA_8"),
	_L("EUidPixelFormatP_2"),
	_L("EUidPixelFormatP_1"),
	};

/**
Convert the string of pixel format to TDisplayMode
@param aPixelformatString The input string of pixel format
@return On return, it contains a display mode enum value
 */
TDisplayMode TDisplayModeMapping::ConvertPixelformatStringToDisplayMode(const TPtrC aPixelformatString)
	{
	TUidPixelFormat format = ConvertPixelformatStringToPixelFormat(aPixelformatString);
	
	return MapPixelFormatToDisplayMode(format);	
	}

/**
Convert the string of pixel format to TUidPixelFormat
@param aPixelformatString The input string of pixel format
@return On return, it contains a pixel format enum value
 */
TUidPixelFormat TDisplayModeMapping::ConvertPixelformatStringToPixelFormat(const TPtrC aPixelformatString)
	{
	TUidPixelFormat format = EUidPixelFormatUnknown;
	
	if (aPixelformatString == KPixelFormatNames[1])
		{
		format =  EUidPixelFormatXRGB_8888;
		}
	if (aPixelformatString == KPixelFormatNames[2])
		{
		format =  EUidPixelFormatARGB_8888;
		}
	if (aPixelformatString == KPixelFormatNames[3])
		{
		format =  EUidPixelFormatARGB_8888_PRE;
		}
	if (aPixelformatString == KPixelFormatNames[4])
		{
		format =  EUidPixelFormatRGB_888;
		}
	if (aPixelformatString == KPixelFormatNames[5])
		{
		format =  EUidPixelFormatRGB_565;
		}
	if (aPixelformatString == KPixelFormatNames[6])
		{
		format =  EUidPixelFormatP_8;
		}
	if (aPixelformatString == KPixelFormatNames[7])
		{
		format =  EUidPixelFormatA_8;
		}
	if (aPixelformatString == KPixelFormatNames[8])
		{
		format =  EUidPixelFormatP_2;
		}
	if (aPixelformatString == KPixelFormatNames[9])
		{
		format =  EUidPixelFormatP_1;
		}
	
	return format;
	}

/*
Convert the pixel format enum to a string
@param aFormat The pixel format enum
@return On return, it contains the pixel format string
 */
TPtrC TDisplayModeMapping::ConvertPixelformatToPixelFormatString(const TUidPixelFormat aFormat)
	{
	TPtrC pixelFormatString(KPixelFormatNames[0]);
	
	if (aFormat == EUidPixelFormatXRGB_8888)
		{
		pixelFormatString.Set(KPixelFormatNames[1]);
		}
	if (aFormat == EUidPixelFormatARGB_8888)
		{
		pixelFormatString.Set(KPixelFormatNames[2]);
		}
	if (aFormat == EUidPixelFormatARGB_8888_PRE)
		{
		pixelFormatString.Set(KPixelFormatNames[3]);
		}
	if (aFormat == EUidPixelFormatRGB_888)
		{
		pixelFormatString.Set(KPixelFormatNames[4]);
		}
	if (aFormat == EUidPixelFormatRGB_565)
		{
		pixelFormatString.Set(KPixelFormatNames[5]);
		}
	if (aFormat == EUidPixelFormatP_8)
		{
		pixelFormatString.Set(KPixelFormatNames[6]);
		}
	if (aFormat == EUidPixelFormatA_8)
		{
		pixelFormatString.Set(KPixelFormatNames[7]);
		}
	if (aFormat == EUidPixelFormatP_2)
		{
		pixelFormatString.Set(KPixelFormatNames[8]);
		}
	if (aFormat == EUidPixelFormatP_1)
		{
		pixelFormatString.Set(KPixelFormatNames[9]);
		}
	
	return pixelFormatString;
	}

/*
Map pixel format enum to display mode enum
@param aFormat The pixel format enum
@return On return, it contains the display mode enum
 */
TDisplayMode TDisplayModeMapping::MapPixelFormatToDisplayMode(const TUidPixelFormat aFormat)
	{
	TDisplayMode mode;
	
	switch(aFormat)
	{
	/** Display mode with alpha (24bpp colour plus 8bpp alpha) */
	case EUidPixelFormatXRGB_8888:
	//case EUidPixelFormatBGRX_8888:
	//case EUidPixelFormatXBGR_8888: 
		mode = EColor16MU; 
		break;
	
	/** Display mode with alpha (24bpp colour plus 8bpp alpha) */
	case EUidPixelFormatARGB_8888:
	//case EUidPixelFormatBGRA_8888:
	//case EUidPixelFormatABGR_8888: 
		mode = EColor16MA; 
		break;

	/** Pre-multiplied Alpha display mode (24bpp color multiplied with the alpha channel value, plus 8bpp alpha) */
	case EUidPixelFormatARGB_8888_PRE:
	//case EUidPixelFormatABGR_8888_PRE:
	//case EUidPixelFormatBGRA_8888_PRE: 
		mode = EColor16MAP; 
		break;

	/** True colour display mode (24 bpp) */
	//case EUidPixelFormatBGR_888:
	case EUidPixelFormatRGB_888:
		mode = EColor16M; 
		break;
	
	/** 64,000 colour display mode (16 bpp) */
	case EUidPixelFormatRGB_565:
	//case EUidPixelFormatBGR_565:
	//case EUidPixelFormatARGB_1555:
	//case EUidPixelFormatXRGB_1555:
	//case EUidPixelFormatARGB_4444:
	//case EUidPixelFormatARGB_8332:
	//case EUidPixelFormatBGRX_5551:
	//case EUidPixelFormatBGRA_5551:	
	//case EUidPixelFormatBGRA_4444:
	//case EUidPixelFormatBGRX_4444:
	//case EUidPixelFormatAP_88:
	//case EUidPixelFormatXRGB_4444:
	//case EUidPixelFormatXBGR_4444:
		mode = EColor64K; 
		break;
	
	/** 256 colour display mode (8 bpp) */
	//case EUidPixelFormatRGB_332:
	//case EUidPixelFormatBGR_332:
	case EUidPixelFormatP_8:
		mode = EColor256;
		break;
	
	/** 256 grayscales display mode (8 bpp) */	
	case EUidPixelFormatA_8:
		mode = EGray256;
		break;

	/** Low colour EGA 16 colour display mode (4 bpp) */
	//case EUidPixelFormatP_4:
	//	mode = EColor16;
	//  mode = EGray16 ???
	//	break;
		
	/** Four grayscales display mode (2 bpp) */
	case EUidPixelFormatP_2:
		mode = EGray4;
		break;
		
	/** Monochrome display mode (1 bpp) */
	case EUidPixelFormatP_1:
	//case EUidPixelFormatBW_1:
		mode = EGray2;
		break;

	// Unknown display mode
	default:
	case EUidPixelFormatUnknown:
	//case EUidPixelFormatYUV_444Interleaved:
	//case EUidPixelFormatYUV_444Planar:
	//case EUidPixelFormatYUV_420Interleaved:
	//case EUidPixelFormatYUV_420Planar:
	//case EUidPixelFormatYUV_420PlanarReversed:
	//case EUidPixelFormatYUV_420SemiPlanar:
	//case EUidPixelFormatYUV_422Interleaved:
	//case EUidPixelFormatYUV_422Planar:
	//case EUidPixelFormatYUV_422Reversed:
	//case EUidPixelFormatYUV_422SemiPlanar:
	//case EUidPixelFormatYUV_422InterleavedReversed:
	//case EUidPixelFormatYYUV_422Interleaved:
	//case EUidPixelFormatARGB_2101010:
	//case EUidPixelFormatABGR_2101010:
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
	//case EUidPixelFormatBGRX_8888:
	//case EUidPixelFormatXBGR_8888: 
		format = EUidPixelFormatXRGB_8888; 
		break;
	
	/** Display mode with alpha (24bpp colour plus 8bpp alpha) */
	case EColor16MA:
	//case EUidPixelFormatBGRA_8888:
	//case EUidPixelFormatABGR_8888: 
		format = EUidPixelFormatARGB_8888; 
		break;

	/** Pre-multiplied Alpha display mode (24bpp color multiplied with the alpha channel value, plus 8bpp alpha) */
	case EColor16MAP:
	//case EUidPixelFormatABGR_8888_PRE:
	//case EUidPixelFormatBGRA_8888_PRE: 
		format = EUidPixelFormatARGB_8888_PRE; 
		break;

	/** True colour display mode (24 bpp) */
	//case EUidPixelFormatBGR_888:
	case EColor16M:
		format = EUidPixelFormatRGB_888; 
		break;
	
	/** 64,000 colour display mode (16 bpp) */
	case EColor64K:
	//case EUidPixelFormatBGR_565:
	//case EUidPixelFormatARGB_1555:
	//case EUidPixelFormatXRGB_1555:
	//case EUidPixelFormatARGB_4444:
	//case EUidPixelFormatARGB_8332:
	//case EUidPixelFormatBGRX_5551:
	//case EUidPixelFormatBGRA_5551:	
	//case EUidPixelFormatBGRA_4444:
	//case EUidPixelFormatBGRX_4444:
	//case EUidPixelFormatAP_88:
	//case EUidPixelFormatXRGB_4444:
	//case EUidPixelFormatXBGR_4444:
		format = EUidPixelFormatRGB_565; 
		break;
	
	/** 256 colour display mode (8 bpp) */
	//case EUidPixelFormatRGB_332:
	//case EUidPixelFormatBGR_332:
	case EColor256:
		format = EUidPixelFormatP_8;
		break;
	
	/** 256 grayscales display mode (8 bpp) */	
	case EGray256:
		format = EUidPixelFormatA_8;
		break;

	/** Low colour EGA 16 colour display mode (4 bpp) */
	//case EUidPixelFormatP_4:
	//	mode = EColor16;
	//  mode = EGray16 ???
	//	break;
		
	/** Four grayscales display mode (2 bpp) */
	case EGray4:
		format = EUidPixelFormatP_2;
		break;
		
	/** Monochrome display mode (1 bpp) */
	case EGray2:
	//case EUidPixelFormatBW_1:
		format = EUidPixelFormatP_1;
		break;

	// Unknown display mode
	default:
	case ENone:
	//case EUidPixelFormatYUV_444Interleaved:
	//case EUidPixelFormatYUV_444Planar:
	//case EUidPixelFormatYUV_420Interleaved:
	//case EUidPixelFormatYUV_420Planar:
	//case EUidPixelFormatYUV_420PlanarReversed:
	//case EUidPixelFormatYUV_420SemiPlanar:
	//case EUidPixelFormatYUV_422Interleaved:
	//case EUidPixelFormatYUV_422Planar:
	//case EUidPixelFormatYUV_422Reversed:
	//case EUidPixelFormatYUV_422SemiPlanar:
	//case EUidPixelFormatYUV_422InterleavedReversed:
	//case EUidPixelFormatYYUV_422Interleaved:
	//case EUidPixelFormatARGB_2101010:
	//case EUidPixelFormatABGR_2101010:
		format = EUidPixelFormatUnknown; 
		break;
	}
	
	return format;
	}

/**
 Converts pixel format strings to pixel format enums.
 @param aPixelFormatInput The pixel format string for testing
 @param aPixelFormatArray An array of TUidPixelFormats as output.
 */
void TDisplayModeMapping::ConvertPixelFormats(TPtrC aPixelFormatInput, RArray<TUidPixelFormat>& aPixelFormatArray)
	{
	TPtrC tempBuf = aPixelFormatInput;
	TInt position = tempBuf.Find(_L(","));
			
	while(KErrNotFound != position)
		{
		aPixelFormatArray.Append(TDisplayModeMapping::ConvertPixelformatStringToPixelFormat(tempBuf.Left(position)));
		tempBuf.Set(tempBuf.Mid(position + 2));
		position = tempBuf.Find(_L(","));
		}	

	if (position == KErrNotFound)
		{
		aPixelFormatArray.Append(TDisplayModeMapping::ConvertPixelformatStringToPixelFormat(tempBuf));
		}
	}

