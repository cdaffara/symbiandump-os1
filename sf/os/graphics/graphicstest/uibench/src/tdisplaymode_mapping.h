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
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TDISPLAYMODEMAPPING_H__
#define __TDISPLAYMODEMAPPING_H__

#include <e32const.h>
#include <pixelformats.h>
#include <gdi.h>

// a helper class for mapping between display modes and pixel formats in directgdi tests
class TDisplayModeMapping
 	{
public:
	static TDisplayMode MapPixelFormatToDisplayMode(const TUidPixelFormat aFormat);
	static TUidPixelFormat MapDisplayModeToPixelFormat(const TDisplayMode aDisplayMode);
	static TDisplayMode ConvertPixelformatStringToDisplayMode(const TPtrC aPixelformatString);
	static TUidPixelFormat ConvertPixelformatStringToPixelFormat(const TPtrC aPixelformatString);
	static TPtrC ConvertPixelformatToPixelFormatString(const TUidPixelFormat aFormat);
	static TPtrC ConvertDisplayModeToString(const TDisplayMode aFormat);
	static void ConvertPixelFormats(TPtrC aPixelFormatInput, RArray<TUidPixelFormat>& aPixelFormatArray);
	};

#endif	//__TDISPLAYMODEMAPPING_H__
