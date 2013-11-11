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
// Graphics Resource - image inline functions
//

#ifndef SGIMAGE_INL
#define SGIMAGE_INL

inline TSgImageInfo::TSgImageInfo()
	{
	}

inline TSgImageInfo::TSgImageInfo(const TSize& aSizeInPixels, TInt aPixelFormat, TUint32 aUsage)
	: iSizeInPixels(aSizeInPixels), iPixelFormat(aPixelFormat), iUsage(aUsage)
	{
	}

inline RSgImage::RSgImage()
	: RSgDrawable(KSgImageTypeUid)
	{
	}

#endif // SGIMAGE_INL
