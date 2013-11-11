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

#ifndef __T_PSEUDOAPPOPENGLANIMATION_H__
#define __T_PSEUDOAPPOPENGLANIMATION_H__

#include <GLES/gl.h>
#include "t_pseudoappeglbase.h"

/**
* Creates an animation (making use of OpenGL API) of a rotating cube 
* which moves across the screen on X-axis
*/
class COpenGLAnimation : public CEglBase
	{
public:
   	static COpenGLAnimation* NewL(RWindow* aWin, const TDisplayMode& aMode, const TSize& aSurfaceSize,
									                             const TInt aHorizontalRate,
									                             const TInt aVerticalRate);
   	~COpenGLAnimation();
	void DrawL(CTestSurfaceRasterizer* aSurfaceRasterizer, SurfaceDetails& aSurfDetails);

protected:
    COpenGLAnimation(const TDisplayMode& aMode, const TSize& aSurfaceSize, const TInt aHorizontalRate, const TInt aVerticalRate);
    void DrawToEglL();

private:
    void ConstructL(RWindow* aWin);
	void CheckGlErrorL(const TDesC& aMessage);
	
private:	//Data
	GLfloat 			iRotAngle;
	};

#endif //__T_PSEUDOAPPOPENGLANIMATION_H__


