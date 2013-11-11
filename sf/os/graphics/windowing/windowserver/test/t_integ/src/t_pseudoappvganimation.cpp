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

#include "t_pseudoappvganimation.h"

//for the inner square
const TReal32 KTlx = 0;
const TReal32 KTly = 0;
const TReal32 KSzx = 60;
const TReal32 KSzy = 60;

void CVgAnimation::DrawToEglL()
	{
    // clear background
    VGfloat bgColor[] = {0.0, 0.0, 0.0, 0.0};
    vgSetfv(VG_CLEAR_COLOR, 4, bgColor);
    vgClear(0, 0, iSurfaceSize.iWidth, iSurfaceSize.iHeight);

	//paint creation and setting.
	vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);
	vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_BETTER);

	VGPaint fillPaint=vgCreatePaint();
	CheckVgErrorL(_L("vgCreatePaint failed"));

	//the fill paint
	VGfloat fillColor[4];
	fillColor[0] = 0.0f/255.0f;  //blue
	fillColor[1] = 255.0f/255.0f; //red
	fillColor[2] = 0.0f/255.0f; //green
	fillColor[3] = 255.0f/255.0f; //alpha = required to be 1.0

	vgSetParameteri(fillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
	CheckVgErrorL(_L("fillPaint VG_PAINT_TYPE failed"));
	vgSetParameterfv(fillPaint, VG_PAINT_COLOR, 4, fillColor);
	CheckVgErrorL(_L("fillPaint VG_PAINT_COLOR failed"));

	VGPath	bgPath;
	bgPath= vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_S_32, 1.0f, 0.0f, 4, 4, (unsigned int)VG_PATH_CAPABILITY_ALL);
	CheckVgErrorL(_L("vgCreatePath failed"));

	VGUErrorCode vguerr=vguEllipse(bgPath, KTlx, KTly, KSzx, KSzy);
	TestL(vguerr==VGU_NO_ERROR, _L("vguRect failed"));
	CheckVgErrorL(_L("vguRect failed"));

	vgSetPaint(fillPaint, VG_FILL_PATH);
	CheckVgErrorL(_L("VG_FILL_PATH failed"));

	vgLoadIdentity();

    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
	CheckVgErrorL(_L("vgLoadIdentity failed"));

	vgTranslate(iX, iY);

	vgDrawPath(bgPath, VG_FILL_PATH);
	CheckVgErrorL(_L("vgDrawPath failed"));

	vgDestroyPath(bgPath);
	vgDestroyPaint(fillPaint);

	vgFinish();

	TInt ret=eglSwapBuffers(iDisplay,iSurface);
	TestL(ret,_L("eglSwapBuffers failed"));
	}

void CVgAnimation::DrawL(CTestSurfaceRasterizer*  /*aSurfaceRasterizer*/, SurfaceDetails& /*aSurfDetails*/)
	{
	DrawToEglL();

	if (iY >= iSurfaceSize.iHeight || iX >= iSurfaceSize.iWidth )
		{
		iX=0;
		iY=0;
		}
	else
		{
		iX+=iHorizontalRate;
		iY+=iVerticalRate;
		}
	}

CVgAnimation* CVgAnimation::NewL(RWindow* aWin, const TDisplayMode& aMode, const TSize& aSurfaceSize,
																		  const TInt aHorizontalRate,
																		  const TInt aVerticalRate)
	{
	RDebug::Print(_L("Creating CVgAnimation class\n"));
    CVgAnimation* self = new (ELeave) CVgAnimation(aMode,aSurfaceSize,aHorizontalRate,aVerticalRate);
    CleanupStack::PushL(self);
    self->ConstructL(aWin);
    CleanupStack::Pop(); // self;
    return self;
	}

CVgAnimation::CVgAnimation(const TDisplayMode& aMode, const TSize& aSurfaceSize, const TInt aHorizontalRate, const TInt aVerticalRate)
: CEglBase(aMode,aSurfaceSize,aHorizontalRate,aVerticalRate)
	{
	}

void CVgAnimation::ConstructL(RWindow* aWin)
	{
	BaseConstructL(aWin);

	// eglBindAPI
	TInt ret = eglBindAPI(EGL_OPENVG_API);
	TestL(ret, _L("eglBindAPI failed"));

	__ASSERT_DEBUG(iUseConfig>0, User::Panic(_L("CVgAnimation"), KErrArgument));

	// eglCreateContext
	iContext = eglCreateContext(iDisplay, iUseConfig, EGL_NO_CONTEXT, NULL);
	TestL(iContext!=EGL_NO_CONTEXT, _L("eglCreateContext failed"));

	// Make surface and context current on the display object
	ret = eglMakeCurrent(iDisplay, iSurface, iSurface, iContext);
	TestL(ret, _L("eglMakeCurrent failed"));

	CheckVgErrorL(_L("VG_Problems creating second surface"));
	}


CVgAnimation::~CVgAnimation()
	{
	}

void CVgAnimation::CheckVgErrorL(const TDesC& aMessage)
	{
	VGErrorCode err=vgGetError();
	if (err != VG_NO_ERROR)
		{
		RDebug::Print(_L("Error:0x%x - %S"),err,&aMessage);
		User::Leave(KErrUnknown);
		}
	}
