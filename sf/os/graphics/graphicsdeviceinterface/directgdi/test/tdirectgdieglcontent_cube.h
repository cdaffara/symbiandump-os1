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
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TDIRECTGDIEGLCONTENT_CUBE_H__
#define __TDIRECTGDIEGLCONTENT_CUBE_H__

#include <e32base.h>
#include <pixelformats.h>
#include <graphics/sgimage.h>
#include <graphics/sgimagecollection.h>
#include <GLES/egl.h>

const TInt KEglContentBuffers = 2;

/**
This class contains cube GL renderer.
CGLCube initializes the EGL environment and renders off-screen pixmap surfaces. 
Creation of the pixmap is based on RSgImage resource which is initialized 
with parameter  TsgImageInfo.iUsage set to EsgUsageOpenGlesTarget|ESgUsageDirectGdiSource.  
OpenGL ES API is used to reneder content of the pixmap. The result is available as a TSgDrawableId object.
*/
class CGLCube : public CBase
	{
public:
	static CGLCube* NewL(TUidPixelFormat aPixelFormat, const TSize& aSize);
	static CGLCube* NewLC(TUidPixelFormat aPixelFormat, const TSize& aSize);
	virtual ~CGLCube();

	void Render(TInt aFrame);
	void GetImage(TSgDrawableId& aId);

private:
	CGLCube();
	void ConstructL(TUidPixelFormat aPixelFormat, const TSize& aSize);
	void InitEglL(TUidPixelFormat aPixelFormat, const TSize& aSize);
	void DeInitEgl();

private:
	/** Display where the OpenGL ES window surface resides. */
	EGLDisplay iEglDisplay;

	/** Describes the format, type and size of the color buffers and ancillary buffers for EGLSurface. */
	EGLConfig iConfig;

	/** The image where the framebuffer is drawn. */
	TSgImageInfo iImageInfo;
	RSgImage iImages[KEglContentBuffers];

	/** Current image buffer number */
	TInt iCurrentImage;
	TInt iLastImage;

	/** Pixmap surface (that uses the iPixmap) that is the target of OpenGL ES graphics rendering. */
	EGLSurface iEglSurfaces[KEglContentBuffers];

	/** OpenGL ES rendering context. */
	EGLContext iEglContext;

	/** OpenGL ES Texture id */
	GLuint iTexId;
	};

#endif
