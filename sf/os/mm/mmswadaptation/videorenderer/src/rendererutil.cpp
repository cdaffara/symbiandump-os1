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

#include <mmf/devvideo/devvideobase.h>
#include "rendererutil.h"

/** Convert TVideoRendererPixelFormat pixel format from resource file to uncompressed video format */
TUncompressedVideoFormat VideoRendererUtil::ConvertPixelFormatToUncompressedVideoFormatL(TVideoRendererPixelFormat aPixelFormat)
	{
	TUncompressedVideoFormat ret;
	
	switch (aPixelFormat)
		{
	case EVideoRendererPixelFormatXRGB_8888:
		ret.iDataFormat = ERgbRawData;
		ret.iRgbFormat = ERgb32bit888;
		break;
	case EVideoRendererPixelFormatRGB_565:
		ret.iDataFormat = ERgbRawData;
		ret.iRgbFormat = ERgb16bit565;
		break;
	case EVideoRendererPixelFormatXRGB_4444:
		ret.iDataFormat = ERgbRawData;
		ret.iRgbFormat = ERgb16bit444;
		break;
	default:
		DEBUGPRINT2(_L("VideoRendererUtil::ConvertPixelFormatToUncompressedVideoFormatL aPixelFormat=%d"), aPixelFormat);
		User::Leave(KErrNotSupported);
		}
	
	return ret;
	}

/** Convert TUidPixelFormat pixel format to uncompressed video format */
TUncompressedVideoFormat VideoRendererUtil::ConvertUidPixelFormatToUncompressedVideoFormatL(TUidPixelFormat aUidPixelFormat)
	{
	return ConvertPixelFormatToUncompressedVideoFormatL(static_cast<TVideoRendererPixelFormat>(aUidPixelFormat));
	}

/** Convert uncompressed video format to TUidPixelFormat pixel format */
TUidPixelFormat VideoRendererUtil::ConvertUncompressedVideoFormatToUidPixelFormatL(const TUncompressedVideoFormat& aFormat)
	{
	TUidPixelFormat ret = EUidPixelFormatUnknown;
	
	if (aFormat.iDataFormat != ERgbRawData)
		{
		DEBUGPRINT2(_L("VideoRendererUtil::ConvertUncompressedVideoFormatToUidPixelFormat iDataFormat=%d"), aFormat.iDataFormat);
		User::Leave(KErrNotSupported);
		}
	
	switch (aFormat.iRgbFormat)
		{
	case ERgb32bit888:
		ret = EUidPixelFormatXRGB_8888;
		break;
	case ERgb16bit565:
		ret = EUidPixelFormatRGB_565;
		break;
	case ERgb16bit444:
		ret = EUidPixelFormatXRGB_4444;
		break;
	default:
		DEBUGPRINT2(_L("VideoRendererUtil::ConvertUncompressedVideoFormatToUidPixelFormatL iRgbFormat=%d"), aFormat.iRgbFormat);
		User::Leave(KErrNotSupported);
		}

	return ret;
	}

/** return the bytes per pixel for a uid pixel format */
TInt VideoRendererUtil::BytesPerPixelL(TUidPixelFormat aPixelFormat)
	{
	switch (aPixelFormat)
		{
	case EUidPixelFormatXRGB_4444:
	case EUidPixelFormatRGB_565:
		return 2;
	case EUidPixelFormatXRGB_8888:
		return 4;
	default:
		DEBUGPRINT2(_L("VideoRendererUtil::BytesPerPixelL aPixelFormat=%d"), aPixelFormat);
		User::Leave(KErrNotSupported);
		}
	
	return 0; // to remove compile warning
	}
