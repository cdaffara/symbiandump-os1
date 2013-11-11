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
*/

#include "egltestcommonsgimageinfo.h"

/* Class TSgImageInfoTest */
EXPORT_C TSgImageInfoTest::TSgImageInfoTest()
	{
	iSizeInPixels = KPixmapSize;
	iPixelFormat = KDefaultSourceFormat;
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	iUsage = ESgUsageBitOpenVgImage;
#else
	iUsage = ESgUsageOpenVgImage;
	iShareable = EFalse;
	iCpuAccess = ESgCpuAccessNone;
	iScreenId = KSgScreenIdMain;
	iUserAttributes = NULL;
	iUserAttributeCount = 0;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	}

EXPORT_C TSgImageInfoTest::TSgImageInfoTest(TUidPixelFormat aPixelFormat, TSize aSize)
	{
	iSizeInPixels = aSize;
	iPixelFormat = aPixelFormat;
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	iUsage = ESgUsageBitOpenVgSurface;
#else
	iUsage = ESgUsageOpenVgTarget;
	iShareable = EFalse;
	iCpuAccess = ESgCpuAccessNone;
	iScreenId = KSgScreenIdMain;
	iUserAttributes = NULL;
	iUserAttributeCount = 0;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	}

EXPORT_C TSgImageInfoTest::~TSgImageInfoTest()
	{
	}

void TSgImageInfoTest::operator=(const TSgImageInfo& aInfo)
	{
	iSizeInPixels = aInfo.iSizeInPixels;
	iPixelFormat = aInfo.iPixelFormat;
	iUsage = aInfo.iUsage;
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	iShareable = aInfo.iShareable;
	iCpuAccess = aInfo.iCpuAccess;
	iScreenId = aInfo.iScreenId;
	iUserAttributes = aInfo.iUserAttributes;
	iUserAttributeCount = aInfo.iUserAttributeCount;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	}

TBool TSgImageInfoTest::operator==(const TSgImageInfo& aInfo)
	{
	if((iSizeInPixels == aInfo.iSizeInPixels) &&
	   (iPixelFormat == aInfo.iPixelFormat) &&
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	   (iUsage == aInfo.iUsage))
#else
		(iUsage == aInfo.iUsage) &&
		(iShareable == aInfo.iShareable) &&
		(iCpuAccess == aInfo.iCpuAccess) &&
		(iScreenId == aInfo.iScreenId) &&
		(iUserAttributes == aInfo.iUserAttributes) &&
		(iUserAttributeCount == aInfo.iUserAttributeCount))
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
		{
		return ETrue;
		}

	return EFalse;
	}

/* OpenVgImage Child variant */
EXPORT_C TSgImageInfoOpenVgImage::TSgImageInfoOpenVgImage()
	{
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	iUsage = ESgUsageBitOpenVgImage;
#else
	iUsage = ESgUsageOpenVgImage;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	}

EXPORT_C TSgImageInfoOpenVgImage::TSgImageInfoOpenVgImage(TUidPixelFormat aPixelFormat, TSize aSize) : TSgImageInfoTest(aPixelFormat, aSize)
	{
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	iUsage = ESgUsageBitOpenVgImage;
#else
	iUsage = ESgUsageOpenVgImage;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	}

EXPORT_C TSgImageInfoOpenVgImage::~TSgImageInfoOpenVgImage()
	{
	}

/* OpenVgTarget Child variant */
EXPORT_C TSgImageInfoOpenVgTarget::TSgImageInfoOpenVgTarget()
	{
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	iUsage = ESgUsageBitOpenVgSurface;
#else
	iUsage = ESgUsageOpenVgTarget;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	}

EXPORT_C TSgImageInfoOpenVgTarget::TSgImageInfoOpenVgTarget(TUidPixelFormat aPixelFormat, TSize aSize) : TSgImageInfoTest(aPixelFormat, aSize)
	{
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	iUsage = ESgUsageBitOpenVgSurface;
#else
	iUsage = ESgUsageOpenVgTarget;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	}

EXPORT_C TSgImageInfoOpenVgTarget::~TSgImageInfoOpenVgTarget()
	{
	}
