// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSURFACEHELPER_H__
#define __TSURFACEHELPER_H__

#include <e32base.h>
#include <gdi.h>
#include <pixelformats.h>
#include <graphics/surface.h>
#include <graphics/surfacemanager.h>
#include "twindows.h"

class CSurfaceHelper : public CBase
	{
public:
	static CSurfaceHelper* NewL();
	~CSurfaceHelper();
	TSurfaceId CreateSurfaceL(const TSize& aSize, TUidPixelFormat aPixelFormat, TInt aStride, TInt aBuffers);
	void FillRectangleL(const TSurfaceId& aSurface, const TPoint& aStartPos, const TSize& aSize, const TRgb& aColor);
private:
	void ConstructL();
	void DestroySurfaces();
private:
	RSurfaceManager iManager;
	RArray<TSurfaceId> iSurfaces; 
	};



#endif //__TSURFACEHELPER_H__
