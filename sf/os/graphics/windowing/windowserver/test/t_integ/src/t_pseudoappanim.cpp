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

#include <e32debug.h> //RDebug

#include "t_pseudoappanim.h"
#include "t_pseudoappeng.h"

EXPORT_C CTestAnimation::CTestAnimation() : iRotationSupported(EFalse)
	{
	}

EXPORT_C void CTestAnimation::DrawL(CTestSurfaceRasterizer* /* aSurfaceRasterizer */, SurfaceDetails& /* aSurfDetails */)
	{
	}

//If rotation is supported by a derived class, this function will need to be overridden
EXPORT_C void CTestAnimation::SetFrameNumber(TInt /* aFrameNumber */)
	{
	RDebug::Print(_L("Rotation is not supported for this animation type"));
	User::Panic(KTPseudoAppPanicTxt, KErrNotSupported);
	}

EXPORT_C TBool CTestAnimation::RotationSupported()
	{
	return iRotationSupported;
	}

CTestAnimation::~CTestAnimation()
	{
	}
