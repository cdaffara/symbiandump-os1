// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @prototype
*/

#ifndef SGUTILS_H
#define SGUTILS_H

#include <gdi.h>
#include <pixelformats.h>


/**
@publishedPartner
@prototype
@deprecated

Set of utility functions. These functions are related to a number of Graphics APIs 
and provide functionality for converting from one pixel format to another, 
mapping from TDisplayMode to TUidPixelFormat and viceversa, and calculating data strides.
*/
class SgUtils
	{
public:
	IMPORT_C static TUidPixelFormat DisplayModeToPixelFormat(TDisplayMode aDisplayMode);
	IMPORT_C static TDisplayMode PixelFormatToDisplayMode(TUidPixelFormat aPixelFormat);
	IMPORT_C static TInt MinDataStride(TInt aWidth, TUidPixelFormat aPixelFormat);
	IMPORT_C static TInt TransferPixels(TAny* aDataAddressDest, TInt aDataStrideDest, TUidPixelFormat aPixelFormatDest,
	                                    const TAny* aDataAddressSrc, TInt aDataStrideSrc, TUidPixelFormat aPixelFormatSrc,
	                                    const TRect& aRectSource);
	};


#endif // SGUTILS_H
