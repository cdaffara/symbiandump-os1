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
// Graphics Resource - constants
//

#ifndef SGCONST_H
#define SGCONST_H

#include <pixelformats.h>
#include <sgresource/sgerror.h>

/**
The constants defined in TSgUsageBits correspond to the different usages for which 
an image can be created.  They can be combined using the bitwise OR operator to 
specify more than one usage.
*/
enum TSgUsageBits
	{
	/** Usage as a VGImage with OpenVG via EGLImage.*/
	ESgUsageBitOpenVgImage        = 0x0001,
	/** Usage as a two-dimensional texture with OpenGL ES 1.x via EGLImage.*/
	ESgUsageBitOpenGlesTexture2D  = 0x0002,
	/** Usage as a two-dimensional texture with OpenGL ES 2.x via EGLImage.*/
	ESgUsageBitOpenGles2Texture2D = 0x0004,
	/** Usage as a target of an OpenVG rendering context.*/
	ESgUsageBitOpenVgSurface      = 0x0010,
	/** Usage as a target of an OpenGL ES 1.x rendering context.*/
	ESgUsageBitOpenGlesSurface    = 0x0020,
	/** Usage as a target of an OpenGL ES 2.x rendering context.*/
	ESgUsageBitOpenGles2Surface   = 0x0040,
	/** Usage as a target of an OpenGL rendering context.*/
	ESgUsageBitOpenGlSurface      = 0x0080
	};

/**
The subset of pixel formats defined in pixelformats.h which are guaranteed to be 
supported by all implementations of the Graphics Resource API.
*/
enum TSgPixelFormat
	{
	ESgPixelFormatARGB_8888_PRE = EUidPixelFormatARGB_8888_PRE,
	ESgPixelFormatARGB_8888     = EUidPixelFormatARGB_8888,
	ESgPixelFormatXRGB_8888     = EUidPixelFormatXRGB_8888,
	ESgPixelFormatRGB_565       = EUidPixelFormatRGB_565,
	ESgPixelFormatA_8           = EUidPixelFormatA_8
	};

#endif // SGCONST_H
