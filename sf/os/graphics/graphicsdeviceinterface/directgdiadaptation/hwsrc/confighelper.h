// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CONFIGHELPER_H_
#define CONFIGHELPER_H_

#include <e32def.h>
#include <EGL/egl.h>

class CFbsBitmap;

/**
Utility class to help in choosing an EGL config.

@internalComponent Reference implementation.
*/
NONSHARABLE_CLASS(TConfigHelper)
	{
	/**
	Enumeration of EGL Config Config Attribute indices into KSurfaceAttribs.
	*/
	enum 
		{
		EEglBufferSize = 0,
		EEglBufferType = 2,
		EEglRedSize = 4,
		EEglGreenSize = 6,
		EEglBlueSize = 8,
		EEglAlphaSize = 10,
		EEglLuminanceSize = 12,
		EEglSurfaceType = 14,
		EEglRenderableType = 16,
		ELast,
		};
	
	/**
	 Enumeration of EGL Config indices into KSurfaceAttribs.
	 */
	enum
		{ 
		EPixmapAttribsColor64K,
		EPixmapAttribsColor16MAP,
		EPixmapAttribsColor16MU,
		EPixmapAttribsColor16MA,
		EPixmapAttribsLuminance8L,
		EPixmapAttribsLuminance1L      
		};
	
public:
	static TInt GetSuitablePixmapConfigIndex(EGLDisplay aDisplay, EGLConfig *aConfigs, TInt aNumConfigs, TInt aColorAttrib);
	static TInt MatchPixelType(TInt aPixelType);
	static TInt GetConfig(TInt aAttributeArrayIndex, EGLConfig& aResult);
	static TInt GetConfigForFbsBitmap(const CFbsBitmap& aBitmap, EGLConfig& aResult);
	
#ifdef _DEBUG
	static void SupportedSurfacePixelFormatsL();
#endif

private:
	static TBool EglAttributeMatches(EGLint aAttribute, TInt aExpected, TInt aActual);
	
public:
	static const EGLint KSurfaceAttribs[][19];
	};

#endif /*CONFIGHELPER_H_*/
