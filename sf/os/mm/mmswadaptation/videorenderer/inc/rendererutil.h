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
 @internalComponent
*/

#ifndef RENDERERUTIL_H
#define RENDERERUTIL_H

#include <e32def.h>
#include <e32const.h>
#include <pixelformats.h>
#include "videorenderer.hrh"

// class declaration
class TUncompressedVideoFormat;

#ifdef _DEBUG
#include <e32debug.h>

#define DEBUGPRINT1(_p1) RDebug::Print(_p1)
#define DEBUGPRINT2(_p1,_p2) RDebug::Print(_p1,_p2)
#define DEBUGPRINT3(_p1,_p2,_p3) RDebug::Print(_p1,_p2,_p3)
#define DEBUGPRINT4(_p1,_p2,_p3,_p4) RDebug::Print(_p1,_p2,_p3,_p4)
#else
#define DEBUGPRINT1(_p1)
#define DEBUGPRINT2(_p1,_p2)
#define DEBUGPRINT3(_p1,_p2,_p3)
#define DEBUGPRINT4(_p1,_p2,_p3,_p4)
#endif


/** Utiility functions for video renderer */
class VideoRendererUtil
	{
public:
	static TUncompressedVideoFormat ConvertPixelFormatToUncompressedVideoFormatL(TVideoRendererPixelFormat aPixelFormat);
	static TUncompressedVideoFormat ConvertUidPixelFormatToUncompressedVideoFormatL(TUidPixelFormat aUidPixelFormat);
	static TUidPixelFormat ConvertUncompressedVideoFormatToUidPixelFormatL(const TUncompressedVideoFormat& aFormat);
	static TInt BytesPerPixelL(TUidPixelFormat aPixelFormat);
	};



#endif /*RENDERERUTIL_H*/
