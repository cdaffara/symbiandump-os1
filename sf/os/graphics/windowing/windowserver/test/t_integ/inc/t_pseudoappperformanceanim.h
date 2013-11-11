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

#ifndef __T_PSEUDOAPPPERFORMANCEANIM_H__
#define __T_PSEUDOAPPPERFORMANCEANIM_H__

#include <gdi.h>		//Display modes
#include <w32std.h>		//RWsSession, CWsScreenDevice etc

#include "t_pseudoappanim.h"

class CPerformanceAnimation : public CTestAnimation
	{
public:
	IMPORT_C static CPerformanceAnimation* NewL();
	IMPORT_C void DrawL(CTestSurfaceRasterizer* aSurfaceRasterizer, SurfaceDetails& aSurfDetails);
	~CPerformanceAnimation();

private:
	void ConstructL();
	CPerformanceAnimation();

protected:
	TInt iFrameCounter;
	};

#endif //__T_PSEUDOAPPPERFORMANCEANIM_H__
