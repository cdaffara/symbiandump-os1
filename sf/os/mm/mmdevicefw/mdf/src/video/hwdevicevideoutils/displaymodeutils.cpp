// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "displaymodeutils.h"

/*
 Converts a TRgbFormat into a TDisplayMode.
	 
 @param		The TRgbFormat.
 @return	The corresponding TDisplayMode, or ERgb if no match found.
*/
TDisplayMode TMMFDisplayModeUtils::DisplayMode(TRgbFormat aRgbFormat) 
	{
	TDisplayMode theDisplayMode = ERgb;
	
	switch(aRgbFormat) 
		{
		case ERgb16bit444:
			theDisplayMode = EColor4K;
			break;
		case ERgb16bit565:
			theDisplayMode = EColor64K;
			break;
		case ERgb32bit888:
			theDisplayMode = EColor16MU;
			break;
		case EFbsBitmapColor4K:
			theDisplayMode = EColor4K;
			break;
		case EFbsBitmapColor64K:
			theDisplayMode = EColor64K;
			break;
		case EFbsBitmapColor16M:
			theDisplayMode = EColor16M;
			break;
		case EFbsBitmapColor16MU:
			theDisplayMode = EColor16MU;
			break;
		default:
			break;			
		}
	return theDisplayMode;
	}

/*
 Converts a TRgbFormat into a the corresponding number of bits per pixel.
	 
 @param		The TRgbFormat.
 @return	The corresponding number of bits per pixel,
			or KErrNotSupported if no match found.
*/		
TInt TMMFDisplayModeUtilsBytesPerPixel(TRgbFormat aRgbFormat)
	{
	TInt bpp = KErrNotSupported;
	
	switch(aRgbFormat) 
		{
		case ERgb16bit444:
			bpp = 2;
			break;
		case ERgb16bit565:
			bpp = 2;
			break;
		case ERgb32bit888:
			bpp = 4;
			break;
		case EFbsBitmapColor4K:
			bpp = 2;
			break;
		case EFbsBitmapColor64K:
			bpp = 2;
			break;
		case EFbsBitmapColor16M:
			bpp = 3;
			break;
		case EFbsBitmapColor16MU:
			bpp = 4;
			break;
		default:
			break;
		}
	return bpp;
	}
