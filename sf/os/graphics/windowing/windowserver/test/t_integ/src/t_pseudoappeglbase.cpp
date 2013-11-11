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

#include <e32debug.h>
#include "t_pseudoappeglbase.h"

const TInt KMaxConfigs = 1024;

CEglBase::CEglBase(const TDisplayMode& aMode,const TSize& aSurfaceSz, const TInt aHRate, const TInt aVRate)
			: CTestAnimation(),iDispMode(aMode)
	{
	iSurfaceSize=aSurfaceSz;
	iHorizontalRate=aHRate;
	iVerticalRate=aVRate;
	}

void CEglBase::BaseConstructL(RWindow* aWin)
	{
	// eglGetDisplay
	iDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	TestL(iDisplay!=EGL_NO_DISPLAY, _L("eglGetDisplay failed"));

	// eglInitialize
	TInt ret = eglInitialize(iDisplay, NULL, NULL);
	TestL(ret, _L("eglInitialize failed"));

	// eglGetConfigs
	EGLint numConfigs;
	EGLConfig configs[KMaxConfigs];
	ret = eglGetConfigs(iDisplay, configs, KMaxConfigs, &numConfigs);
	TestL(ret, _L("eglGetConfigs failed"));
	TestL(numConfigs>0, _L("eglGetConfigs returned no configs"));

	const EGLint* attribList = NULL;

	// Select attribs based on current display mode
	switch (iDispMode)
		{
		case EColor4K:	attribList = KColor4KAttribList;	break;
		case EColor64K:	attribList = KColor64KAttribList;	break;
		case EColor16M:	attribList = KColor16MAttribList;	break;
		case EColor16MA:attribList = KColor16MAAttribList;	break;
		default: 		User::Panic(_L("CEglBase"), KErrArgument);
		}
	
	RDebug::Print(_L("Display Mode: %d"), iDispMode );

	// eglChooseConfig
	ret=eglChooseConfig(iDisplay, attribList, configs, KMaxConfigs, &numConfigs);
	TestL(ret, _L("eglChooseConfig failed"));
	TestL(numConfigs>0, _L("eglChooseConfig returned no configs"));
 
    if (attribList)
    	{
    	TInt configIndex = GetSuitableConfig(configs, numConfigs, attribList);
		TestL(configIndex!=KErrNotFound, _L("No suitable configuration found") );
		iUseConfig = configs[configIndex];	
    	}

	// eglCreateWindowSurface
	iSurface = eglCreateWindowSurface(iDisplay, iUseConfig, aWin, NULL);
	TestL(iSurface!=EGL_NO_SURFACE, _L("eglCreateWindowSurface failed") );
	}

CEglBase::~CEglBase()
	{
	// Clean everything allocated.
	eglMakeCurrent(iDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(iDisplay, iContext);
	eglDestroySurface(iDisplay, iSurface);
	eglTerminate(iDisplay);
	}

TInt CEglBase::GetSuitableConfig(const EGLConfig* aConfigs, EGLint aNumConfigs, const EGLint* attribs)
	{
	TInt configIndex = KErrNotFound;
	for (TInt i=0; i<aNumConfigs; i++)
		{
		EGLConfig config = aConfigs[i];
		EGLint value;

		TInt ret=eglGetConfigAttrib(iDisplay, config, attribs[0], &value);
		RDebug::Print(_L("eglGetConfigAttrib returned %d"), ret );
		if (ret && value != attribs[1])
			{
			continue;
			}
		ret=eglGetConfigAttrib(iDisplay, config, attribs[2], &value);
		if (ret && value != attribs[3])
			{
			continue;
			}
		ret=eglGetConfigAttrib(iDisplay, config, attribs[4], &value);
		if (ret && value != attribs[5])
			{
			continue;
			}
		ret=eglGetConfigAttrib(iDisplay, config, attribs[6], &value);
		if (ret && value != attribs[7])
			{
			continue;
			}
		ret=eglGetConfigAttrib(iDisplay, config, attribs[8], &value);
		if (ret && value != attribs[9])
			{
			continue;
			}

		// Success
		configIndex = i;
		break;
		}

	return configIndex;
	}

void CEglBase::TestL(const TInt aRet, const TDesC& aMessage)
	{
	if (!aRet)
		{
		RDebug::Print(_L("Error: %d - %S"), aRet, &aMessage);
		User::Leave(KErrUnknown);
		}
	}

