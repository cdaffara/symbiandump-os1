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

#ifndef __T_PSEUDOAPPANIM_H__
#define __T_PSEUDOAPPANIM_H__

#include <e32base.h>
#include "t_testsurfacerasterizer.h"

class CTestAnimation : public CBase
	{
public:
	IMPORT_C virtual void DrawL(CTestSurfaceRasterizer* aSurfaceRasterizer, SurfaceDetails& aSurfDetails) = 0;
	IMPORT_C virtual void SetFrameNumber(TInt aFrameNumber); //Override in derived class if rotation supported
	IMPORT_C TBool RotationSupported();
	~CTestAnimation();

protected:
	IMPORT_C CTestAnimation();

protected:
	TPtrC iBitmapAnimFile;
	TInt  iHorizontalRate;
	TInt  iVerticalRate;
	TSize iAnimSize;
	TSize iSurfaceSize;
	TBool iRotationSupported;
	};

#endif //__T_PSEUDOAPPANIM_H__

