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
 @internalComponent
*/

#ifndef __T_PSEUDOAPPCFBSBITMAPANIM_H__
#define __T_PSEUDOAPPCFBSBITMAPANIM_H__

#include <gdi.h>		//Display modes
#include <w32std.h>		//RWsSession, CWsScreenDevice etc

#include "t_pseudoappanim.h"
#include "t_pseudoappwindow.h"

class CCFbsBitmapAnimation : public CTestAnimation
	{
public:
	IMPORT_C static CCFbsBitmapAnimation* NewL(TDisplayMode aMode, const TSize& aAnimSize, const TSize& aSurfaceSize,
                                               TPtrC& aBitmapFile, TInt aHorizontalRate, TInt aVerticalRate,
                                               TPixelFormat aPixelFormat);
	IMPORT_C ~CCFbsBitmapAnimation();
	IMPORT_C void Rotate(TSize aScreenSize);
	IMPORT_C void DrawL(CTestSurfaceRasterizer* aSurfaceRasterizer, SurfaceDetails& aSurfDetails);
	IMPORT_C void SetFrameNumber(TInt aFrameNumber);

private:
	void ConstructL(TDisplayMode aMode, const TSize& aAnimSize, const TSize& aSurfaceSize,
                    TPtrC& aBitmapFile, TInt aHorizontalRate, TInt aVerticalRate,
                    TPixelFormat aPixelFormat);
	CCFbsBitmapAnimation();
	void RedrawSurfaceL();

protected:
	CFbsBitmap* iFullSurfaceBitmap;
	CFbsBitGc* iFullSurfaceBitmapGc;
	CFbsBitmapDevice* iFullSurfaceBitmapDevice;

	TPixelFormat iPixelFormat;
	TInt iFrameCounter;
	TInt iXPos;
	TInt iYPos;
	};

#endif //__T_PSEUDOAPPCFBSBITMAPANIM_H__

