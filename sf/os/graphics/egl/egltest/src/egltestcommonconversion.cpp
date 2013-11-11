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

#include "egltestcommonconversion.h"

EXPORT_C TEglTestConfig EglTestConversion::VgFormatToWindowSurfaceFormat(VGImageFormat aVgFormat)
	{
	switch(aVgFormat)
		{
		case VG_sRGB_565:
			return EWindowAttribsColor64K;		
		case VG_sARGB_8888:
			return EWindowAttribsColor16MA;
		case VG_sARGB_8888_PRE:
			return EWindowAttribsColor16MAP;
		default:
			return EEglTestConfigMax;
		}
	}

EXPORT_C TEglTestConfig EglTestConversion::VgFormatToPBufferSurfaceFormat(VGImageFormat aVgFormat)
	{
	switch(aVgFormat)
		{
		case VG_sRGB_565:
			return EPBufferAttribsColor64K;
		case VG_sXRGB_8888:
			return EPBufferAttribsColor16MU;					
		case VG_sARGB_8888:
			return EPBufferAttribsColor16MA;
		case VG_sARGB_8888_PRE:
			return EPBufferAttribsColor16MAP;
		default:
			return EEglTestConfigMax;
		}
	}

EXPORT_C TEglTestConfig EglTestConversion::VgFormatToPixmapSgSurfaceFormat(VGImageFormat aVgFormat)
	{
	switch(aVgFormat)
		{
		case VG_sRGB_565:
			return EPixmapAttribsColor64K;
		case VG_sXRGB_8888:
			return EPixmapAttribsColor16MU;
		case VG_sARGB_8888_PRE:
			return EPixmapAttribsColor16MAP;
		default:
			return EEglTestConfigMax;
		}
	}

EXPORT_C TUidPixelFormat EglTestConversion::VgFormatToSgPixelFormat(VGImageFormat aVgFormat)
	{
	switch(aVgFormat)
		{
		case VG_sRGB_565:
			return EUidPixelFormatRGB_565;
		case VG_sXRGB_8888:
			return EUidPixelFormatXRGB_8888;
		case VG_sARGB_8888:
			return EUidPixelFormatARGB_8888;
		case VG_sARGB_8888_PRE:
			return EUidPixelFormatARGB_8888_PRE;
		case VG_A_8:
			return EUidPixelFormatA_8;
		default:
			return EUidPixelFormatUnknown;
		}
	}

EXPORT_C VGImageFormat EglTestConversion::PixelFormatToVgFormat(TUidPixelFormat aPixelFormat)
	{
	switch(aPixelFormat)
		{
		case EUidPixelFormatRGB_565:
			return VG_sRGB_565;
		case EUidPixelFormatXRGB_8888:
			return VG_sXRGB_8888;
		case EUidPixelFormatARGB_8888:
			return VG_sARGB_8888;
		case EUidPixelFormatARGB_8888_PRE:
			return VG_sARGB_8888_PRE;
		case EUidPixelFormatA_8:
			return VG_A_8;
		default:
			return VG_IMAGE_FORMAT_INVALID;
		}
	}

EXPORT_C TDisplayMode EglTestConversion::PixelFormatToDisplayMode(TUidPixelFormat aPixelFormat)
	{
	switch(aPixelFormat)
		{
		case EUidPixelFormatRGB_565:
			return EColor64K;
		case EUidPixelFormatXRGB_8888:
			return EColor16MU;
		case EUidPixelFormatARGB_8888:
			return EColor16MA;
		case EUidPixelFormatARGB_8888_PRE:
			return EColor16MAP;
		case EUidPixelFormatA_8:
			return EGray256;
		default:
			return ENone;
		}
	}

EXPORT_C TDisplayMode EglTestConversion::VgFormatToDisplayMode(VGImageFormat aVgFormat)
	{
	return PixelFormatToDisplayMode( VgFormatToSgPixelFormat(aVgFormat) );
	}

EXPORT_C TInt EglTestConversion::BytePerPixel(TUidPixelFormat aPixelFormat)
    {
    switch(aPixelFormat)
        {
        case EUidPixelFormatRGB_565:
            return 2;
        case EUidPixelFormatXRGB_8888:
        case EUidPixelFormatARGB_8888:
        case EUidPixelFormatARGB_8888_PRE:
            return 4;
        case EUidPixelFormatA_8:
            return 1;
        default:
            return 0;
        }
    }
