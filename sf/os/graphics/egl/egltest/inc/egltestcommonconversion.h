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

#ifndef __EGLTESTCOMMONCONVERSION_H__
#define __EGLTESTCOMMONCONVERSION_H__

#include <EGL/egl.h>
#include <VG/openvg.h>
#include <test/egltestcommonutils.h>


class EglTestConversion
	{
public:
	// all static methods
	IMPORT_C static TEglTestConfig VgFormatToWindowSurfaceFormat(VGImageFormat aVgFormat);
	IMPORT_C static TEglTestConfig VgFormatToPBufferSurfaceFormat(VGImageFormat aVgFormat);
	IMPORT_C static TEglTestConfig VgFormatToPixmapSgSurfaceFormat(VGImageFormat aVgFormat);
	
	IMPORT_C static TUidPixelFormat VgFormatToSgPixelFormat(VGImageFormat aVgFormat);
	IMPORT_C static VGImageFormat PixelFormatToVgFormat(TUidPixelFormat aPixelFormat);
	
	IMPORT_C static TDisplayMode VgFormatToDisplayMode(VGImageFormat aVgFormat);
	IMPORT_C static TDisplayMode PixelFormatToDisplayMode(TUidPixelFormat aPixelFormat);

    IMPORT_C static TInt BytePerPixel(TUidPixelFormat aPixelFormat);
	};

#endif
