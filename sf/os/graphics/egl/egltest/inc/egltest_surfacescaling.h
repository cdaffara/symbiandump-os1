// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef EGLTEST_SURFACESCALING_H
#define EGLTEST_SURFACESCALING_H

#include "eglteststep.h"
#include <test/egltestcommonsession.h>
#include <test/graphicsscreencomparison.h>

enum TEglTestScalingConfig
	{ 
	EWindowAttribs_NoScaling,
	EWindowAttribsColor16MU_Scaling 
	};

static const EGLint KScalingConfigAttribs[2][17] =
	{
		{
		//Window - Scaling not supported
		EGL_BUFFER_SIZE,    	 0,
		EGL_RED_SIZE,			 0,
		EGL_GREEN_SIZE, 		 0,
		EGL_BLUE_SIZE,			 0,
		EGL_ALPHA_SIZE, 		 0,
		EGL_RENDERABLE_TYPE,	 EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,		 EGL_WINDOW_BIT,
		EGL_SURFACE_SCALING_NOK, EGL_FALSE,
		EGL_NONE
		},
		{
		//EColor16MU - Window - Scaling supported
		EGL_BUFFER_SIZE,    	 24,
		EGL_RED_SIZE,			 8,
		EGL_GREEN_SIZE, 		 8,
		EGL_BLUE_SIZE,			 8,
		EGL_ALPHA_SIZE,     	 0,
		EGL_RENDERABLE_TYPE,	 EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,		 EGL_WINDOW_BIT,
		EGL_SURFACE_SCALING_NOK, EGL_TRUE,
		EGL_NONE
		},
	};

//base class for all surface scaling test cases
//all common functionality should go here
NONSHARABLE_CLASS(CEglTest_SurfaceScalingBase) : public CEglTestStep
    {
public:
    ~CEglTest_SurfaceScalingBase();

protected:
    // from CTestStep
    TVerdict doTestStepPreambleL();
    TVerdict doTestStepPostambleL();

    // helper methods
    void CheckBorderColorL(EGLint aExpectedRedChannelColor, EGLint aExpectedBlueChannelColor, EGLint aExpectedGreenChannelColor);
    void CheckScalingAttributesL(EGLint aExpectedSurfaceWidth, EGLint aExpectedSurfaceHeight, EGLint aExpectedExtentWidth, EGLint aExpectedExtentHeight, EGLint aExpectedOffsetX, EGLint aExpectedOffsetY);
    CFbsBitmap* CreateBitmapLC(const TSize& aSize, TInt aBorderTop, TInt aBorderBottom, TInt aBorderLeft, TInt aBorderRight, const TRgb& aBorderColor);
    void WritePixelsToSurfaceL(const CFbsBitmap& aBitmap);
    void CreateAndActivateWindowL(const TSize& aWindowSize);
    void CloseWindow();

protected:
    CWsScreenDevice* iScreenDevice;
    RWindow iWindow;
    // surface scaling extension functions
    TFPtrEglQuerySurfaceScalingCapabilityNok iPfnEglQuerySurfaceScalingCapabilityNOK;
    TFPtrEglSetSurfaceScalingNok iPfnEglSetSurfaceScalingNOK;
    // surface scaling attributes
    TInt iWindowWidth;
    TInt iWindowHeight;
    TInt iSurfaceWidth;
    TInt iSurfaceHeight;
    TInt iExtentWidth;
    TInt iExtentHeight;
    TInt iOffsetX;
    TInt iOffsetY;
    TRgb iBorderColor;
    // surface scaling related attributes for reference bitmap
    TInt iBorderTop;
    TInt iBorderBottom;
    TInt iBorderLeft;
    TInt iBorderRight;
    // image comparison
    CTGraphicsScreenComparison* iImageComparison;
    // properties of the particular surface scaling implementation under testing
    TBool iAllScalable;
    TSize iScreenSize;
    };


// EGL Surface Scaling tests
_LIT(KSurfaceScaling_Positive, "SurfaceScaling_Positive");
NONSHARABLE_CLASS(CEglTest_SurfaceScaling_Positive) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    TVerdict doTestPartialStepL();
    };

_LIT(KSurfaceScaling_WindowResize, "SurfaceScaling_WindowResize");
NONSHARABLE_CLASS(CEglTest_SurfaceScaling_WindowResize) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    TVerdict doTestPartialStepL();
    };

_LIT(KSurfaceScaling_ExtentPositionChange, "SurfaceScaling_ExtentPositionChange");
NONSHARABLE_CLASS(CEglTest_SurfaceScaling_ExtentPositionChange) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    TVerdict doTestPartialStepL(const CFbsBitmap& aRefBitmap);
private:
    TInt iRefBitmapOffset;
    };

_LIT(KSurfaceScaling_ExtentSizeChange, "SurfaceScaling_ExtentSizeChange");
NONSHARABLE_CLASS(CEglTest_SurfaceScaling_ExtentSizeChange) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    TVerdict doTestPartialStepL();
    };

_LIT(KSurfaceScaling_SwapBuffers, "SurfaceScaling_SwapBuffers");
NONSHARABLE_CLASS(CEglTest_SurfaceScaling_SwapBuffers) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    TVerdict doTestPartialStepL();
    };

_LIT(KSurfaceScaling_WindowSurface_Check, "SurfaceScaling_WindowSurface_Check");
NONSHARABLE_CLASS(CEglTest_SurfaceScaling_WindowSurface_Check) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    };

_LIT(KSurfaceScaling_Negative_CreateWindowSurface, "SurfaceScaling_Negative_CreateWindowSurface");
NONSHARABLE_CLASS(CEglTest_SurfaceScaling_Negative_CreateWindowSurface) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    };

_LIT(KSurfaceScaling_Negative_FixedSize_NonWindowSurface, "SurfaceScaling_Negative_FixedSize_NonWindowSurface");
NONSHARABLE_CLASS(CEglTest_SurfaceScaling_Negative_FixedSize_NonWindowSurface) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    };

_LIT(KSurfaceScalingDefaultBorderColor, "SurfaceScalingDefaultBorderColor");
NONSHARABLE_CLASS(CEglTest_SurfaceScalingDefaultBorderColor) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    };

_LIT(KSurfaceScalingModifyingBorderColor, "SurfaceScalingModifyingBorderColor");
NONSHARABLE_CLASS(CEglTest_SurfaceScalingModifyingBorderColor) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    };

_LIT(KSurfaceScalingModifyingBorderColorNonFixed, "SurfaceScalingModifyingBorderColorNonFixed");
NONSHARABLE_CLASS(CEglTest_SurfaceScalingModifyingBorderColorNonFixed) : public CEglTest_SurfaceScalingBase
    {
public:
	TVerdict doTestStepL();
    };

_LIT(KSurfaceScalingModifyingInvalidBorderColor, "SurfaceScalingModifyingInvalidBorderColor");
NONSHARABLE_CLASS(CEglTest_SurfaceScalingModifyingInvalidBorderColor) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    };

_LIT(KSurfaceScalingModifyingExtent, "SurfaceScalingModifyingExtent");
NONSHARABLE_CLASS(CEglTest_SurfaceScalingModifyingExtent) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    };

_LIT(KSurfaceScalingModifyingExtentNonFixed, "SurfaceScalingModifyingExtentNonFixed");
NONSHARABLE_CLASS(CEglTest_SurfaceScalingModifyingExtentNonFixed) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    };

_LIT(KSurfaceScalingQuerySurface, "SurfaceScalingQuerySurface");
NONSHARABLE_CLASS(CEglTest_SurfaceScalingQuerySurface) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    };

_LIT(KSurfaceScalingQuerySurfaceNonFixed, "SurfaceScalingQuerySurfaceNonFixed");
NONSHARABLE_CLASS(CEglTest_SurfaceScalingQuerySurfaceNonFixed) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    };

_LIT(KSurfaceScalingCapability, "SurfaceScalingCapability");
NONSHARABLE_CLASS(CEglTest_SurfaceScalingCapability) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    };

_LIT(KSurfaceScalingSet, "SurfaceScalingSet");
NONSHARABLE_CLASS(CEglTest_SurfaceScalingSet) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    };

_LIT(KSurfaceScalingSetNonFixed, "SurfaceScalingSetNonFixed");
NONSHARABLE_CLASS(CEglTest_SurfaceScalingSetNonFixed) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    };

_LIT(KSurfaceScalingSetInvalidAttributes, "SurfaceScalingSetInvalidAttributes");
NONSHARABLE_CLASS(CEglTest_SurfaceScalingSetInvalidAttributes) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    };

_LIT(KSurfaceScalingNotInitialized, "SurfaceScalingNotInitialized");
NONSHARABLE_CLASS(CEglTest_SurfaceScalingNotInitialized) : public CEglTest_SurfaceScalingBase
    {
public:
    TVerdict doTestStepL();
    };

#endif // EGLTEST_SURFACESCALING_H
