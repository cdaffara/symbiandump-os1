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
*/

#include <test/tefunit.h> // for ASSERT macros

#include "egltest_image_negative.h"

#include <test/egltestcommonconversion.h>
#include <test/egltestcommonsgimageinfo.h>

/**
@SYMTestCaseID GRAPHICS-EGL-0126

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Test that eglCreateImageKHR() fails and returns the correct error
when one of the parameters other than “Target” has an invalid value.
The value of the “Target” parameter must always be EGL_NATIVE_PIXMAP_KHR

@SYMTestActions
•	Call eglCreateImageKHR() with NULL instead of a valid RSgImage handle
•	Create a not fully constructed RSgImage object (i.e. do not call RSgImage::Create() )and pass its handle when calling eglCreateImageKHR().
•	Fully construct an RSgImage and call eglCreateImageKHR() with a valid egl context
•	Call eglCreateImageKHR() with a non valid target (meaningless number instead of EGL_NATIVE_PIXMAP_KHR)
•	Call eglCreateImageKHR() with diplay = EGL_NO_DISPLAY
•	Call eglCreateImageKHR() with a meaningless number in place of iDisplay
Destroy the RSgImage object


@SYMTestExpectedResults
eglCreateImageKHR() returns EGL_NO_IMAGE_KHR in all cases
•	And an EGL_BAD_PARAMETER error is generated in all cases but the last
•	The last case should generate EGL_BAD_DISPLAY error
No memory or handle leaks
*/
TVerdict CEglTest_EGL_Image_eglCreateImage_Bad_Parameter::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0126"));
	INFO_PRINTF1(_L("CEglTest_EGL_Image_eglCreateImage_Bad_Parameter::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	// This test is performed for default pixel format
	PrintUsedPixelConfiguration();

	// Create display object
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	INFO_PRINTF1(_L("Creating one EGLImage from a NULL RSgImage"));
	EGLImageKHR imageKHR = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, NULL, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR == EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);

	INFO_PRINTF1(_L("Creating one EGLImage from a not fully constructed RSgImage"));
	RSgImage sgImage;
	CleanupClosePushL(sgImage);
	imageKHR = iEglSess->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&sgImage,KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR == EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);

	// Create an RSgImage with proper attributes
	TSgImageInfo imageInfo;
	imageInfo.iSizeInPixels = KPixmapSize;
	imageInfo.iPixelFormat = iSourceFormat;
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	imageInfo.iUsage = ESgUsageBitOpenVgImage;
#else
	imageInfo.iUsage = ESgUsageOpenVgImage;
	imageInfo.iShareable = EFalse;
	imageInfo.iCpuAccess = ESgCpuAccessReadWrite;
	//imageInfo.iMutable = ETrue;
	imageInfo.iScreenId = KSgScreenIdMain;
	imageInfo.iUserAttributes = NULL;
	imageInfo.iUserAttributeCount=0;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE

	ASSERT_EQUALS(sgImage.Create(imageInfo, NULL, NULL), KErrNone);

	//Creating a Valid Context
	INFO_PRINTF1(_L("Calling eglBindAPI(EGL_OPENVG_API)"));
	ASSERT_EGL_TRUE(eglBindAPI(EGL_OPENVG_API));

	INFO_PRINTF1(_L("Calling eglCreateContext"));
	TEglTestConfig pixmapFormat = EglTestConversion::VgFormatToPixmapSgSurfaceFormat(iSurfaceFormat);
	EGLConfig currentConfig = iEglSess->GetConfigExactMatchL(pixmapFormat);
	EGLContext context = eglCreateContext(iDisplay, currentConfig, EGL_NO_CONTEXT, NULL);
	ASSERT_EGL_TRUE(context != EGL_NO_CONTEXT);

	INFO_PRINTF1(_L("Calling with a valid context instead of EGL_NO_CONTEXT"));
	imageKHR = iEglSess->eglCreateImageKhrL(iDisplay,context,EGL_NATIVE_PIXMAP_KHR,&sgImage,KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR == EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);

	INFO_PRINTF1(_L("Calling with a non valid target, using registered by meaningless in this context number - EGL_RENDERABLE_TYPE"));
	imageKHR = iEglSess->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT, EGL_RENDERABLE_TYPE,&sgImage,KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR == EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);

	INFO_PRINTF1(_L("Calling with a non valid target, using registered by meaningless in this context number - EGL_RENDERABLE_TYPE, and a valid context too"));
	imageKHR = iEglSess->eglCreateImageKhrL(iDisplay,context,EGL_RENDERABLE_TYPE,&sgImage,KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR == EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);

	INFO_PRINTF1(_L("Calling with a display set to EGL_NO_DISPLAY"));
	imageKHR = iEglSess->eglCreateImageKhrL(EGL_NO_DISPLAY,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&sgImage,KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR == EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_BAD_DISPLAY);

	INFO_PRINTF1(_L("Calling with a a \"random\" value instead of a Display"));
	imageKHR = iEglSess->eglCreateImageKhrL(iDisplay+3,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&sgImage,KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR == EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_BAD_DISPLAY);

	INFO_PRINTF1(_L("Calling with an invalid attribute list"));
	EGLint KEglImageAttribsInvalid[] =
		{
		EGL_SURFACE_TYPE, EGL_PIXMAP_BIT,
		EGL_NONE
		};
	imageKHR = iEglSess->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&sgImage,KEglImageAttribsInvalid);
	ASSERT_EGL_TRUE(imageKHR == EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);

	INFO_PRINTF1(_L("Calling with a corrupt attribute list (fail to provide an EGL_NONE attribute at the end of the list)"));
	EGLint KEglImageAttribsCorrupt[] =
		{
		EGL_IMAGE_PRESERVED_KHR, EGL_TRUE
		};
	imageKHR = iEglSess->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&sgImage,KEglImageAttribsCorrupt);
	ASSERT_EGL_TRUE(imageKHR == EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);

	//cleanup
	ASSERT_EGL_TRUE(eglDestroyContext(iDisplay, context));
	CleanupStack::PopAndDestroy(&sgImage);
	CleanAll();

	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0127

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Check that iUsage bits are enforced.
It’s not possible to create a VGImage from an RSgImage can’t be used as a VGImage source.

@SYMTestActions
Create a reference Bitmap
Create and fully construct an RSgImage object having the same content as the reference bitmap
•	Set the iUsage bits to ESgUsageBitOpenGlesSurface
Pass the RSgImage objects into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that those calls to eglCreateImageKHR() returns EGL_NO_IMAGE_KHR
The following will only be exercised if OpenGL_ES2 is supported
•	Set the iUsage bits to ESgUsageBitOpenGles2Texture2D
Pass the RSgImage objects into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that those calls to eglCreateImageKHR() do NOT return EGL_NO_IMAGE_KHR
Create and make current and EGL context bound to OVG APIs and linked to a pixmap surface.
•	Set the iUsage bit of the underlying RSgImage to ESgUsageBitOpenVgSurface
Use vgCreateEGLImageTargetKHR() to construct a VGImage object from the just created EGLImage.
This call should return VG_UNSUPPORTED_IMAGE_FORMAT_ERROR since the underlying RSgImage needs iUsage of ESgOpenVgImage to make this call succeed.
Pass the EGLImage into eglDestroyImageKHR()
Close the RSgImage
Destroy the pixmap
Check for memory and handle leaks

@SYMTestExpectedResults
eglCreateImageKHR() does return EGL_BAD_PARAMETER
No memory or handle leaks
*/
TVerdict CEglTest_EGL_Image_UsageBits_Enforcement::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0127"));
	INFO_PRINTF1(_L("CEglTest_EGL_Image_UsageBits_Enforcement::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	// This test is performed for default pixel format
	PrintUsedPixelConfiguration();

	// Create display object
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	// Create a reference bitmap which we use to init the SgImage (we use index=8)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 8);
	CleanupStack::PushL(bitmap);

	// Create RSgImage's attributes.
	TSgImageInfoTest imageInfo;
	imageInfo.iSizeInPixels = KPixmapSize;
	imageInfo.iPixelFormat = iSourceFormat;
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	imageInfo.iUsage = ESgUsageBitOpenGlesSurface;
#else
	imageInfo.iUsage = ESgUsageDirectGdiSource;
	imageInfo.iShareable = EFalse;
	imageInfo.iCpuAccess = ESgCpuAccessNone;
	imageInfo.iScreenId = KSgScreenIdMain;
	imageInfo.iUserAttributes = NULL;
	imageInfo.iUserAttributeCount = 0;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE

	RSgImage sgImage;
	CleanupClosePushL(sgImage);
	ASSERT_EQUALS(sgImage.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride()), KErrNone);

	//First Subtest: Attempted creation of an EGLImageKhr from an RSgImage with the wrong Usage should fail
	INFO_PRINTF1(_L("Attempt creation of an EGLImage from a RSgImage with incorrect iUsage ESgUsageBitOpenGlesSurface"));
	INFO_PRINTF1(_L("Correct iUsage needs to have at least one of ESgUsageBitOpenVgImage, ESgUsageOpenGlesTexture2D or ESgUsageOpenGles2Texture2D bits set"));

	EGLImageKHR imageKHR = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR == EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);
    CleanupStack::PopAndDestroy(&sgImage);

	if(iEglSess->IsOpenGLES2Supported())
	    {
	    TSgImageInfoTest imageInfo2;
		imageInfo2.iSizeInPixels = KPixmapSize;
		imageInfo2.iPixelFormat = iSourceFormat;
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
		imageInfo2.iUsage = ESgUsageBitOpenGles2Texture2D;
#else
		imageInfo2.iUsage = ESgUsageOpenGlesTexture2D;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE

	    // Create a SgImage
		RSgImage sgImage2;
		CleanupClosePushL(sgImage2);
		ASSERT_EQUALS(sgImage2.Create(imageInfo2, bitmap->DataAddress(), bitmap->DataStride()), KErrNone);

	    // The creation of an EGLImage from a RSgImage with correct usage (ESgUsageBitOpenGles2Texture2D or ESgUsageOpenGlesTexture2D) should pass
		EGLImageKHR imageKHR2 = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage2, KEglImageAttribsPreservedTrue);
		ASSERT_EGL_TRUE(imageKHR2 != EGL_NO_IMAGE_KHR);

	    //Create an OffScreen Pixmap, we need it to make a Context current
	    imageInfo2.iSizeInPixels = KPixmapSize;
	    imageInfo2.iPixelFormat = iSourceFormat;
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
		imageInfo2.iUsage = ESgUsageBitOpenVgSurface;
#else
		imageInfo2.iUsage = ESgUsageOpenVgTarget;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	    iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo2,CTestEglSession::EResourceCloseSgImageEarly);

	    // Create a VGImage from the EGLImage
	    //Second Subtest: creation of an VGImage from an EGLImage whose RSgImage has NOT ESgUsageBitOpenVgImage as usage will fail
	    VGImage vgImageTarget = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR2);
	    ASSERT_VG_TRUE(vgImageTarget == VG_INVALID_HANDLE);
	    ASSERT_TRUE(vgGetError()==VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);

        CleanupStack::PopAndDestroy(&sgImage2);
        ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR2));
	    }

	//cleanup
	CleanupStack::PopAndDestroy(bitmap);
	CleanAll();

	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0128

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
eglDestroyImageKHR handles correctly errors when given wrong input parameters.

@SYMTestActions
Create a reference Bitmap
Create and fully construct an RSgImage object having the same content as the reference bitmap
•	Set the iUsage bit to ESgUsageBitOpenVgImage
Pass the RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that eglCreateImageKHR() does NOT return EGL_NO_IMAGE_KHR
Call eglDestroyImageKHR() sequentially with the following parameters:
1.	Display equal to EGL_NO_DISPLAY
2.	The EGLImageKHR is not a valid EGLImage  handle
Destroy the image data
•	Pass the EGLImage into eglDestroyImageKHR()
•	Close RSgImage
Check for memory and handle leaks

@SYMTestExpectedResults
eglDestroyImageKHR generates the correct error code:
•	EGL_BAD_DISPLAY in the both case
*/
TVerdict CEglTest_EGL_Image_DestroyImageKHR::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0128"));
	INFO_PRINTF1(_L("CEglTest_EGL_Image_DestroyImageKHR::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	// This test is performed for default pixel format
	PrintUsedPixelConfiguration();

	// Create display object
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	INFO_PRINTF1(_L("Creating one RSgImage"));
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(iSourceFormat, KPixmapSize);
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	imageInfo.iCpuAccess = ESgCpuAccessReadWrite;
#endif
	RSgImage sgImage;
	CleanupClosePushL(sgImage);
	ASSERT_EQUALS(sgImage.Create(imageInfo, NULL, NULL), KErrNone);

	INFO_PRINTF1(_L("Creating one EGLImage from it"));
	EGLImageKHR imageKHR = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR != EGL_NO_IMAGE_KHR);

	INFO_PRINTF1(_L("Calling eglDestroyImageKHR with EGL_NO_DISPLAY"));
	ASSERT_EGL_TRUE(!iEglSess->DestroyEGLImage(EGL_NO_DISPLAY, imageKHR));
	ASSERT_EGL_ERROR(EGL_BAD_DISPLAY);

	INFO_PRINTF1(_L("Calling eglDestroyImageKHR with a random number (but not 1) as Display"));
	ASSERT_EGL_TRUE(!iEglSess->DestroyEGLImage(7, imageKHR));
	ASSERT_EGL_ERROR(EGL_BAD_DISPLAY);

	INFO_PRINTF1(_L("Calling eglDestroyImageKHR with valid EGLImage handle (so, that should succeed)"));
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));

	INFO_PRINTF1(_L("Calling eglDestroyImageKHR with invalid handle"));
	ASSERT_EGL_TRUE(!iEglSess->DestroyEGLImage(iDisplay, imageKHR));
	ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);

	//cleanup
    CleanupStack::PopAndDestroy(&sgImage);
	CleanAll();

	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0129

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Any attemp to create a VGImage from a bad EGLImage handle has to fail.

@SYMTestActions
Create a reference Bitmap
Create and fully construct an RSgImage object having the same content as the reference bitmap
•	Set the iUsage bit to ESgUsageBitOpenVgImage
Pass the RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that eglCreateImageKHR() does NOT return EGL_NO_IMAGE_KHR.
Create a surface and a current context.
Destroy the EGLImage but retain the handle value.
Try to create a VGImage from this invalid handle.
Check that the error VG_ILLEGAL_ARGUMENT_ERROR is raised.
Check for memory and handle leaks.

@SYMTestExpectedResults
vgCreateImageTargetKHR raises a VG_ILLEGAL_ARGUMENT_ERROR error.
Check for memory and handle leaks.
*/
TVerdict CEglTest_EGL_Image_VGImage_From_Invalid_EGLHandle::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0129"));
	INFO_PRINTF1(_L("CEglTest_VGImage_From_Invalid_EGLHandle::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	// This test is performed for default pixel format
	PrintUsedPixelConfiguration();

	// Create display object
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	INFO_PRINTF1(_L("Creating one RSgImage"));
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(iSourceFormat, KPixmapSize);
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	imageInfo.iCpuAccess = ESgCpuAccessReadWrite;
#endif
	RSgImage sgImage;
	CleanupClosePushL(sgImage);
	ASSERT_EQUALS(sgImage.Create(imageInfo, NULL, NULL), KErrNone);

	INFO_PRINTF1(_L("Creating one EGLImage from it"));
	EGLImageKHR imageKHR = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR != EGL_NO_IMAGE_KHR);

	//Create a Surface and Link it to a Context bound to OpenVG
	TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
	TSgImageInfoOpenVgTarget imageInfo2 = TSgImageInfoOpenVgTarget(pixelFormat, KPixmapSize);
	iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo2, CTestEglSession::EResourceCloseSgImageEarly);

	INFO_PRINTF1(_L("Destroying the EGLImage but retain the handle value"));
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));

	INFO_PRINTF1(_L("Attemptimg to create a VGImage from an invalid handle"));
	VGImage vgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR);
	ASSERT_VG_TRUE(vgImage == VG_INVALID_HANDLE);
	ASSERT_TRUE(vgGetError()==VG_ILLEGAL_ARGUMENT_ERROR);

	vgDestroyImage(vgImage);
	ASSERT_TRUE(vgGetError()==VG_BAD_HANDLE_ERROR);

	//cleanup
    CleanupStack::PopAndDestroy(&sgImage);
	CleanAll();

	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0130

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
When a RSgImage is used as both the source and target of a draw operation, then the operation should not panic.
However the outcome is undefined.

@SYMTestActions
Create and fully construct an RSgImage object
Pass the RSgImage objects into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that those calls to eglCreateImageKHR() do NOT return EGL_NO_IMAGE_KHR
Use vgCreateEGLImageTargetKHR() to construct a VGImage object from the just created EGLImage.
•	Check for errors
Create Pixmap Surface from the previous RSgImage and make it current in a way that is compatible as a target for the VGImage to be drawn to.
•	Set the iUsage bit to ESgUsageBitOpenVgSurface and ESgUsageBitOpenGlesSurface
Use OpenVG to draw a single patern to the left half of the VGImage created from the EGLImage.
Try to draw this VGImage to the right half of the pixmap surface currently linked to the context.
Call eglWaitClient() to finish the above drawing instructions synchronously.
Check that the pixmap contains expected pixel values.
Pass the VGImage into vgDestroyImage()
Pass the EGLImage into eglDestroyImageKHR()
Close the RSgImage
Destroy the pixmap
Check for memory and handle leaks

@SYMTestExpectedResults
This test is not supposed to panic.
The contents, though, are undefined since we are reading from and writing to the same memory
No memory or handle leaks.
*/
TVerdict CEglTest_EGL_Image_Self_Drawing::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0130"));
	INFO_PRINTF1(_L("CEglTest_EGL_Image_Self_Drawing::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	// This test is performed for default pixel format
	PrintUsedPixelConfiguration();

	// Create display object
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	// Create a reference bitmap which we use to init the SgImage (we use index=8)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 8);
	CleanupStack::PushL(bitmap);

	INFO_PRINTF1(_L("Creating one RSgImage"));
	TSgImageInfoTest imageInfo;
	imageInfo.iSizeInPixels = KPixmapSize;
	imageInfo.iPixelFormat = iSourceFormat;
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	imageInfo.iUsage = ESgUsageBitOpenVgImage | ESgUsageBitOpenVgSurface;
#else
	imageInfo.iUsage = ESgUsageOpenVgImage | ESgUsageOpenVgTarget;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	RSgImage sgImage;
	CleanupClosePushL(sgImage);
	ASSERT_EQUALS(sgImage.Create(imageInfo, bitmap->DataAddress(),bitmap->DataStride()), KErrNone);

	INFO_PRINTF1(_L("Creating one EGLImage from it"));
	EGLImageKHR imageKHR = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR != EGL_NO_IMAGE_KHR);

	INFO_PRINTF1(_L("Calling eglBindAPI(EGL_OPENVG_API)"));
	ASSERT_EGL_TRUE(eglBindAPI(EGL_OPENVG_API));

    EGLint numConfigsWithPre = 0;
    EGLConfig configWithPre;
    const EGLint KAttribImagePre[] = { EGL_MATCH_NATIVE_PIXMAP,  reinterpret_cast<EGLint>(&sgImage),
                                       EGL_RENDERABLE_TYPE,      EGL_OPENVG_BIT,
                                       EGL_SURFACE_TYPE,         EGL_PIXMAP_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT,
                                       EGL_NONE };
    ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KAttribImagePre, &configWithPre, 1, &numConfigsWithPre));

	// Create a pixmap surface from the native image
	INFO_PRINTF1(_L("Calling eglCreatePixmapSurface"));
	EGLSurface surface = eglCreatePixmapSurface(iDisplay, configWithPre, &sgImage, KPixmapAttribsVgAlphaFormatPre );
	ASSERT_EGL_TRUE(surface != EGL_NO_SURFACE);

	// Create a context for drawing to/reading from the pixmap surface and make it current
	INFO_PRINTF1(_L("Calling eglCreateContext"));
	EGLContext context = eglCreateContext(iDisplay, configWithPre, EGL_NO_CONTEXT, NULL);
	ASSERT_EGL_TRUE(context != EGL_NO_CONTEXT);

	INFO_PRINTF1(_L("Calling eglMakeCurrent"));
	ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, surface, surface, context));

	// Create a VGImage from the EGLImage
	INFO_PRINTF1(_L("Creating 1 VGImage from the EGLImage"));
	VGImage vgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR);
	ASSERT_VG_TRUE(vgImage != VG_INVALID_HANDLE);

    //Copy the source VGImage to the surface
	vgSetPixels(0, 0, vgImage, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	eglWaitClient();

	//cleanup
	vgDestroyImage(vgImage);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));
	CleanupStack::PopAndDestroy(2, bitmap); // bitmap, sgImage
	//This test doesn't check the drawing because the content of the image are undefined
	//since we are using the same buffer both as target and as source
	//The main purpose of this test is to ensure we don't get a panic
	ASSERT_EGL_TRUE(eglDestroyContext(iDisplay, context));					//Closing eglContext
	context = EGL_NO_CONTEXT;
	ASSERT_EGL_TRUE(eglDestroySurface(iDisplay,surface));					//Destroying Target Surface handle
	CleanAll();

	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}
