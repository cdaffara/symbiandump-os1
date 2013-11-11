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

#ifndef EGLTESTCOMMONSGIMAGEINFO_H
#define EGLTESTCOMMONSGIMAGEINFO_H

#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <sgresource/sgimage.h>
#else
#include <graphics/sgimage.h>
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <test/egltestcommonsession.h>

/* Common base class, TSgImageInfoTest contains constructors,     */
/* copy constructor and a comparison operaror                     */
/* Constructors can be used to modify the default pixelformat and */
/* pixmap size if desired                                         */
/* The concept is that the base class will initialise the bulk of */
/* the TSgImageInfo member data and each derived class can modify */
/* the iUsage variable to be specific to it's particular type     */
/* The base class can be instantiated in the rare situation that  */
/* the iUsage member var does not match that of any derived class */
/* e.g iUsage = ESgUsageBitOpenVgImage | ESgUsageBitOpenVgSurface */

//Common base class
class TSgImageInfoTest : public TSgImageInfo
	{
public:
	IMPORT_C TSgImageInfoTest();
	IMPORT_C TSgImageInfoTest(TUidPixelFormat aPixelformat, TSize aSize = KPixmapSize);
	IMPORT_C ~TSgImageInfoTest();
	void operator=(const TSgImageInfo& aInfo);
	TBool operator==(const TSgImageInfo& aInfo);
	};

/* OpenVgImage Child variant */
class TSgImageInfoOpenVgImage : public TSgImageInfoTest
	{
public:
	IMPORT_C TSgImageInfoOpenVgImage();
	IMPORT_C TSgImageInfoOpenVgImage(TUidPixelFormat aPixelFormat, TSize aSize = KPixmapSize);
	IMPORT_C ~TSgImageInfoOpenVgImage();
	};

/* OpenVgTarget Child variant */
class TSgImageInfoOpenVgTarget : public TSgImageInfoTest
	{
public:
	IMPORT_C TSgImageInfoOpenVgTarget();
	IMPORT_C TSgImageInfoOpenVgTarget(TUidPixelFormat aPixelFormat, TSize aSize = KPixmapSize);
	IMPORT_C ~TSgImageInfoOpenVgTarget();
	};

#endif // EGLTESTCOMMONSGIMAGEINFO_H
