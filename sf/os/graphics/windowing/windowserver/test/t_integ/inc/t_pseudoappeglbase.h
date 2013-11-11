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

#ifndef __T_PSEUDOAPPEGLBASE_H__
#define __T_PSEUDOAPPEGLBASE_H__

#include <eikenv.h>
#include <gdi.h>
#include <EGL/egl.h>
#include "t_pseudoappanim.h"

	/** Attributes for an EColor4K window */
	const EGLint KColor4KAttribList[] =
			{
			EGL_RED_SIZE,		4,
			EGL_GREEN_SIZE,		4,
			EGL_BLUE_SIZE,		4,
			EGL_DEPTH_SIZE,		16,
			EGL_SURFACE_TYPE,	EGL_WINDOW_BIT,
			EGL_NONE
			};

	/** Attributes for an EColor64K window */
	const EGLint KColor64KAttribList[] =
			{
			EGL_RED_SIZE,		5,
			EGL_GREEN_SIZE,		6,
			EGL_BLUE_SIZE,		5,
			EGL_DEPTH_SIZE,		16,
			EGL_SURFACE_TYPE,	EGL_WINDOW_BIT,
			EGL_NONE
			};

	/** Attributes for an EColor16M window */
	const EGLint KColor16MAttribList[] =
			{
			EGL_RED_SIZE,		8,
			EGL_GREEN_SIZE,		8,
			EGL_BLUE_SIZE,		8,
			EGL_DEPTH_SIZE,		16,
			EGL_SURFACE_TYPE,	EGL_WINDOW_BIT,
			EGL_NONE
			};

	const EGLint KColor16MAAttribList[] =
			{
		EGL_BUFFER_SIZE,    32,
		EGL_RED_SIZE,		8,
		EGL_GREEN_SIZE, 	8,
		EGL_BLUE_SIZE,		8,
		EGL_ALPHA_SIZE, 	8,
		EGL_RENDERABLE_TYPE,EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT,
		EGL_SURFACE_TYPE,	EGL_WINDOW_BIT,
		EGL_NONE
			};	

class CEglBase : public CTestAnimation
	{
public:
	virtual ~CEglBase();
	
protected:
	CEglBase(const TDisplayMode& aMode,const TSize& aSurfaceSz,const TInt aHRate,const TInt aVRate);
	void BaseConstructL(RWindow* aWin);

	virtual void DrawToEglL()=0;
	void TestL(const TInt aRet, const TDesC& aMessage);
	
private:
	TInt GetSuitableConfig(const EGLConfig* aConfigs, EGLint aNumConfigs, const EGLint* attribs);
	

protected: // Data
	EGLDisplay 			iDisplay;
	EGLSurface 			iSurface;
	EGLContext 			iContext;
	EGLConfig 			iUseConfig;
	float				iX;
	float				iY;

private: //Data
	TDisplayMode		iDispMode;
	};

#endif //__T_PSEUDOAPPEGLBASE_H__
