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

#ifndef __T_PSEUDOAPPVGANIMATION_H__
#define __T_PSEUDOAPPVGANIMATION_H__

#include <VG/vgu.h>
#include "t_pseudoappeglbase.h"

/**
* Creates an animation (making use of OpenVG API) of a circle 
* which moves across the screen on x and y axis
*/
class CVgAnimation : public CEglBase
	{
public:
   	static CVgAnimation* NewL(RWindow* aWin, const TDisplayMode& aMode, const TSize& aSurfaceSize,
									                        const TInt aHorizontalRate,
									                        const TInt aVerticalRate);
   	~CVgAnimation();
	void DrawL(CTestSurfaceRasterizer* aSurfaceRasterizer, SurfaceDetails& aSurfDetails);

protected:
    CVgAnimation(const TDisplayMode& aMode, const TSize& aSurfaceSize, const TInt aHorizontalRate, const TInt aVerticalRate);
	void DrawToEglL();
    
private:
    void ConstructL(RWindow* aWin);
	void CheckVgErrorL(const TDesC& aMessage);	
	};

#endif //__T_PSEUDOAPPVGANIMATION_H__


