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

#include "t_pseudoappperformanceanim.h"
#include "t_testsurfacerasterizer.h"

EXPORT_C CPerformanceAnimation* CPerformanceAnimation::NewL()
	{
	RDebug::Print(_L("Creating CPerformanceAnimation class\n"));
    CPerformanceAnimation* self = new (ELeave) CPerformanceAnimation();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self;
    return self;
	}

void CPerformanceAnimation::ConstructL()
	{
	}

CPerformanceAnimation::CPerformanceAnimation() : CTestAnimation()
	{
	}

CPerformanceAnimation::~CPerformanceAnimation()
	{
	}

EXPORT_C void CPerformanceAnimation::DrawL(CTestSurfaceRasterizer* aSurfaceRasterizer, SurfaceDetails& /* aSurfDetails */)
	{
	aSurfaceRasterizer->PerfTestL(iFrameCounter);
	iFrameCounter++;
	}
