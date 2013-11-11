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
 @prototype
*/

#ifndef SUBTITLECOMMONUTILS_H
#define SUBTITLECOMMONUTILS_H

#include <bitmaptransforms.h>
#include <fbs.h>
#include <imageconversion.h>
#include <msvapi.h>

class SubtitleCommonUtils
	{
public:
	static void Png2BmpL(RFs& aFs, const TDesC& aSourcePngFilename, CFbsBitmap& aDestBitmap);
	};

#endif // SUBTITLECOMMONUTILS_H
