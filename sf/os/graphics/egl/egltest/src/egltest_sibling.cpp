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
#include "egltest_sibling.h"

#include <test/egltestcommonconversion.h>
#include <test/egltestcommonsgimageinfo.h>

/**
@SYMTestCaseID GRAPHICS-EGL-0135

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Calling CreatePBufferFromClient with VGImage which is a EGLImage sibilind has to fail.
On the contrary if the VGImage comes from a vgCreateImage() call it has to succeed

@SYMTestActions
Create and fully construct an RSgImage object
Pass the RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that eglCreateImageKHR() does NOT return EGL_NO_IMAGE_KHR 
Use vgCreateEGLImageTargetKHR() to construct a VGImage object from the EGLImage.
•	Check for errors
Call eglCreatePBufferFromClient passing this VGImage as argument and EGL OPENVG IMAGE as bufftype and a NULL attribute list
Check that we get EGL NO SURFACE and an EGL BAD ACCESS error is generated.
Destroy the image data
•	Pass the EGLImage into eglDestroyImageKHR()
•	Close RSgImage
Create a VGImage by calling vgCreateImage().
Pass this VGImage to eglCreatePBufferFromClient, this call is expected to succeed.
Pass the VGImage to vgDestroyImage()
Check for memory and handle leaks

@SYMTestExpectedResults
vgCreatePBufferFromClient () returns EGL NO SURFACE
•	EGL BAD ACCESS error is generated in the first case
•	No error is raised in the second case..
No memory or handle leaks.
*/
TVerdict CEglTest_EGL_Image_CreatePBufferFromClient_With_Sibling_VGImage::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0135"));
	INFO_PRINTF1(_L("CEglTest_CreatePBufferFromClient_With_Sibling_VGImage::doTestStepL"));

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

	RSgImage sgImage;
	CleanupClosePushL(sgImage);

	// Create a reference bitmap which we use to init the SgImage (we use index=8)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 8);
	CleanupStack::PushL(bitmap);
	
	INFO_PRINTF1(_L("Creating one RSgImage"));	
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(iSourceFormat, KPixmapSize);
	ASSERT_EQUALS(sgImage.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride()), KErrNone);
	CleanupStack::PopAndDestroy(bitmap);

	INFO_PRINTF1(_L("Creating one EGLImage from it"));
	EGLImageKHR imageKHR = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR != EGL_NO_IMAGE_KHR);
	CleanupStack::PopAndDestroy(&sgImage);
	
	//Create a Surface and Link it to a Context bound to OpenVG
	TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
	TSgImageInfoOpenVgTarget imageInfo2 = TSgImageInfoOpenVgTarget(pixelFormat, KPixmapSize);
	iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo2, CTestEglSession::EResourceCloseSgImageEarly);

	INFO_PRINTF1(_L("Creating one VGImage from it"));
	VGImage vgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR);
	ASSERT_VG_TRUE(vgImage != VG_INVALID_HANDLE);

	INFO_PRINTF1(_L("Attempt to Create a PBuffer linked to this VGImage"));
	TEglTestConfig pbufferFormat = EglTestConversion::VgFormatToPBufferSurfaceFormat(iSurfaceFormat);
	EGLConfig currentPbufferConfig = 0;
	TRAPD(res, currentPbufferConfig=iEglSess->GetConfigExactMatchL(pbufferFormat));
	if(res == KTestNoMatchingConfig)
	    {
		WARN_PRINTF2(_L("Can't create pbuffer, format %d. Terminating the test without failing"), pbufferFormat);
		// destroy eglimage and vgimage
		vgDestroyImage(vgImage);
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
		ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
	    }
	User::LeaveIfError(res); // leave here if any other error was raised other than the one above.
	EGLSurface surface = eglCreatePbufferFromClientBuffer(iDisplay,EGL_OPENVG_IMAGE,vgImage,currentPbufferConfig,NULL);
	ASSERT_EGL_TRUE(surface == EGL_NO_SURFACE);
	ASSERT_EGL_ERROR(EGL_BAD_ACCESS);

	// destroy eglimage and vgimage
	vgDestroyImage(vgImage);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));
	
	//Positive test
	INFO_PRINTF1(_L("Attempt to Create a PBuffer linked to a VGImage obtained from vgCreateImage"));
	VGImage vgImage2 = vgCreateImage(KDefaultSurfaceFormat,KPixmapSize.iWidth,KPixmapSize.iHeight,VG_IMAGE_QUALITY_BETTER);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	surface = eglCreatePbufferFromClientBuffer(iDisplay,EGL_OPENVG_IMAGE,vgImage2,currentPbufferConfig,NULL);
	ASSERT_EGL_TRUE(surface != EGL_NO_SURFACE);
	ASSERT_EGL_ERROR(EGL_SUCCESS);
	ASSERT_EGL_TRUE(eglDestroySurface(iDisplay,surface));

	//cleanup
	vgDestroyImage(vgImage2);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	CleanAll();

	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0136

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Attempt to create an EGLImage out of a valid RSgImage that is already linked to another EGLImage has to fail.
Destroy the EGLImage, then re-create an EGLImage from the same RSgImage, and this attempt is expected to succeed. 

@SYMTestActions
Create a reference Bitmap
Create and fully construct an RSgImage object having the same content as the reference bitmap 
•	Set the iUsage bit to ESgUsageBitOpenVgImage
Pass the RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that eglCreateImageKHR() does NOT return EGL_NO_IMAGE_KHR
Pass again the same RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that eglCreateImageKHR() returns EGL_NO_IMAGE_KHR and the error generated is EGL_BAD_ACCESS
Pass the EGLImage into eglDestroyImageKHR()
Pass again the same RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that eglCreateImageKHR() does NOT return EGL_NO_IMAGE_KHR
Destroy the image data
•	Pass the EGLImage into eglDestroyImageKHR()
•	Close RSgImage
Check for memory and handle leaks

@SYMTestExpectedResults
eglCreateImageKHR() returns EGL_NO_IMAGE_KHR (and an EGL_BAD_ACCESS error is generated) when the first EGLImage
has not yet been destroyed. Once it is destroyed, creating aother EGLImage succeeds.
No memory or handle leaks
*/
TVerdict CEglTest_EGL_Image_Sibling_Basic::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0136"));
	INFO_PRINTF1(_L("CEglTest_EGL_Image_Sibling_Basic::doTestStepL"));

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
	
	INFO_PRINTF1(_L("Trying to create another EGLImage from the same RSgImage"));
	EGLImageKHR imageKHR2 = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR2 == EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_BAD_ACCESS);
	
	INFO_PRINTF1(_L("Destroy first EGLImage and try to create another EGLImage from the same RSgImage"));
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));		
	EGLImageKHR imageKHR3 = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR3 != EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_SUCCESS);

	//cleanup
	CleanupStack::PopAndDestroy(&sgImage);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR3));		
	CleanAll();

	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0138

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Attempt to create an EGLImage out of a valid RSgImage that is already linked to another VGImage (after the original EGLImage has been destroyed) has to fail.
This is testing the EGLImage sibling rules from the EGLImage spec.

@SYMTestActions
This test will check for the “VG_KHR_EGL_image” extension, if it is not supported on this platform then the test will return immediately without failure.
Create a reference Bitmap
Create and fully construct an RSgImage object having the same content as the reference bitmap 
•	Set the iUsage bit to ESgUsageBitOpenVgImage
Pass the RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that eglCreateImageKHR() does NOT return EGL_NO_IMAGE_KHR
Create and make current and EGL context bound to OVG APIs and linked to a pixmap surface.
•	Set the iUsage bit of the underlying RSgImage to ESgUsageBitOpenVgSurface.
Use vgCreateEGLImageTargetKHR() to construct a VGImage object from the EGLImage.
•	Check for errors
Pass the EGLImage into eglDestroyImageKHR()
Pass again the same RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that eglCreateImageKHR() returns EGL_NO_IMAGE_KHR and the error generated is EGL_BAD_ACCESS
Pass the VGImage into vgDestroyImage()
Close RSgImage
Check for memory and handle leaks

@SYMTestExpectedResults
eglCreateImageKHR() returns EGL_NO_IMAGE_KHR 
•	And an EGL_BAD_ACCESS error is generated
No memory or handle leaks.
*/
TVerdict CEglTest_EGL_Image_Sibling_VGImage::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0138"));
	INFO_PRINTF1(_L("CEglTest_EGL_Image_Sibling_VGImage::doTestStepL"));

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

	INFO_PRINTF1(_L("Creating one VGImage from it"));
	VGImage vgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR);
	ASSERT_VG_TRUE(vgImage != VG_INVALID_HANDLE);
	
	INFO_PRINTF1(_L("Destroying the EGLImage but retain the handle value"));
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));		
	
	INFO_PRINTF1(_L("Attemptimg to create another VGImage from an invalid handle"));
	VGImage failingVgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR);	
	ASSERT_VG_TRUE(failingVgImage == VG_INVALID_HANDLE);
	ASSERT_TRUE(vgGetError()==VG_ILLEGAL_ARGUMENT_ERROR);

	INFO_PRINTF1(_L("Setting the handle to EGL_NO_IMAGE_KHR"));
	imageKHR = EGL_NO_IMAGE_KHR;

	INFO_PRINTF1(_L("Trying to create another EGLImage from the same RSgImage"));
	EGLImageKHR imageKHR2 = GetEglSess()->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&sgImage,NULL);
	ASSERT_EGL_TRUE(imageKHR2 == EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_BAD_ACCESS);

	//cleanup
	CleanupStack::PopAndDestroy(&sgImage);
	vgDestroyImage(vgImage);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	CleanAll();

	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0139

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Attempt to create an EGLImage out of a valid RSgImage that is already linked 
to two VGImages (after the original EGLImage has been destroyed) has to fail.
This is testing the EGLImage sibling rules from the EGLImage spec.

@SYMTestActions
This test will check for the “VG_KHR_EGL_image” extension, if it is not supported 
on this platform then the test will return immediately without failure.
Create a reference Bitmap
Create and fully construct an RSgImage object having the same content as the 
reference bitmap 
•	Set the iUsage bit to ESgUsageBitOpenVgImage
Pass the RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that eglCreateImageKHR() does NOT return EGL_NO_IMAGE_KHR
Create and make current and EGL context bound to OVG APIs and linked to a pixmap surface.
•	Set the iUsage bit of the underlying RSgImage to ESgUsageBitOpenVgSurface 
Use vgCreateEGLImageTargetKHR() to construct a VGImage object from the EGLImage.
•	Check for errors
Use vgCreateEGLImageTargetKHR() to construct another VGImage object from the EGLImage.
•	Check for errors
Pass the EGLImage into eglDestroyImageKHR()
Pass again the same RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that eglCreateImageKHR() returns EGL_NO_IMAGE_KHR and the error generated is EGL_BAD_ACCESS
Pass the first VGImage into vgDestroyImage()
Pass the second VGImage into vgDestroyImage()
Close RSgImage
Check for memory and handle leaks.

@SYMTestExpectedResults
eglCreateImageKHR() returns EGL_NO_IMAGE_KHR
•	And an EGL_BAD_ACCESS error is generated
No memory or handle leaks.
*/
TVerdict CEglTest_EGL_Image_Sibling_Two_VGImages::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0139"));
	INFO_PRINTF1(_L("CEglTest_EGL_Image_Sibling_Two_VGImages::doTestStepL"));

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

	INFO_PRINTF1(_L("Creating two VGImages from it"));
	VGImage vgImage1 = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR);
	ASSERT_VG_TRUE(vgImage1 != VG_INVALID_HANDLE);
	VGImage vgImage2 = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR);
	ASSERT_VG_TRUE(vgImage2 != VG_INVALID_HANDLE);
	
	INFO_PRINTF1(_L("Destroying the EGLImage"));
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));		

	INFO_PRINTF1(_L("Trying to create an EGLImage from the same RSgImage (linked to the two VGImages)"));
	EGLImageKHR imageKHR2 = iEglSess->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&sgImage,NULL);
	ASSERT_EGL_TRUE(imageKHR2 == EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_BAD_ACCESS);

	INFO_PRINTF1(_L("Destroying the first VgImage"));
	vgDestroyImage(vgImage1);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	
	INFO_PRINTF1(_L("Trying to create an EGLImage from the same RSgImage (linked now to just one VGImage)"));
	imageKHR2 = iEglSess->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&sgImage,NULL);
	ASSERT_EGL_TRUE(imageKHR2 == EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_BAD_ACCESS);

	INFO_PRINTF1(_L("Destroying the second VgImage"));
	vgDestroyImage(vgImage2);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);

	INFO_PRINTF1(_L("Trying to create an EGLImage from the same RSgImage (linked now to just one VGImage)"));
	imageKHR2 = iEglSess->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&sgImage,NULL);
	ASSERT_EGL_TRUE(imageKHR2 != EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_SUCCESS);

	//cleanup
	CleanupStack::PopAndDestroy(&sgImage);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR2));		
	CleanAll();

	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0141

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Attempt to create an EGLImage out of a valid RSgImage after that:
•	It has been created a VGImage from it (hence an EGLImage has been created too)
•	A Child VGImage has been created from the previous VGImage
•	the EGLImage has been destroyed too
has to fail
Another attempt to create an EGLImage from the same RSgImage has to fail even if we destroy the parent Image.
Creation of an EGLImage from the same RSgImage has to succeed if the child VgImage has also been destroyed.

@SYMTestActions
This test will check for the “VG_KHR_EGL_image” extension, if it is not supported on this platform then the test will return immediately without failure.
Create a reference Bitmap
Create and fully construct an RSgImage object having the same content as the reference bitmap 
•	Set the iUsage bit to ESgUsageBitOpenVgImage
Pass the RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that eglCreateImageKHR() does NOT return EGL_NO_IMAGE_KHR
Create and make current and EGL context bound to OVG APIs and linked to a pixmap surface.
•	Set the iUsage bit of the underlying RSgImage to ESgUsageBitOpenVgSurface 
Use vgCreateEGLImageTargetKHR() to construct a VGImage object from the EGLImage.
•	Check for errors
Use vgChildImage() to construct child VGImage object from the previous VGImage.
•	Check for errors
Pass the EGLImage into eglDestroyImageKHR()
Pass again the same RSgImage object into eglCreateImageKHR().
Check that eglCreateImageKHR() returns EGL_NO_IMAGE_KHR and the error generated is EGL_BAD_ACCESS
Pass the parent VGImage into vgDestroyImage()
Pass again the same RSgImage object into eglCreateImageKHR().
Check that eglCreateImageKHR() returns EGL_NO_IMAGE_KHR and the error generated is EGL_BAD_ACCESS
Pass the child VGImage into vgDestroyImage()
Pass again the same RSgImage object into eglCreateImageKHR().
Check that eglCreateImageKHR() does not return EGL_NO_IMAGE_KHR.
Pass the EGLImage into eglDestroyImageKHR()
Close RSgImage
Check for memory and handle leaks.

@SYMTestExpectedResults
eglCreateImageKHR() returns EGL_NO_IMAGE_KHR 
•	And an EGL_BAD_ACCESS error is generated
No memory or handle leaks.
*/
TVerdict CEglTest_EGL_Image_Sibling_VGImage_Child::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0141"));
	INFO_PRINTF1(_L("CEglTest_EGL_Image_Sibling_VGImage_Child::doTestStepL"));

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

	INFO_PRINTF1(_L("Creating one VGImage from it"));
	VGImage vgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR);
	ASSERT_VG_TRUE(vgImage != VG_INVALID_HANDLE);
	
	INFO_PRINTF1(_L("Creating a VGImage Child from the previous VGImage"));
	VGImage childVgImage = vgChildImage(vgImage,50,50,KPixmapSize.iWidth-50,KPixmapSize.iHeight-50);
	ASSERT_EGL_TRUE(childVgImage != VG_INVALID_HANDLE);

	INFO_PRINTF1(_L("Destroying the EGLImage"));
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));		
	
	INFO_PRINTF1(_L("Trying to create an EGLImage from the same RSgImage (linked to both Parent and Child VGImage)"));
	EGLImageKHR imageKHR2 = iEglSess->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&sgImage,NULL);
	ASSERT_EGL_TRUE(imageKHR2 == EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_BAD_ACCESS);

	INFO_PRINTF1(_L("Destroying the Parent VGImage"));
	vgDestroyImage(vgImage);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);

	INFO_PRINTF1(_L("Trying to create an EGLImage from the same RSgImage (linked to the Child VGImage)"));
	imageKHR2 = iEglSess->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&sgImage,NULL);
	ASSERT_EGL_TRUE(imageKHR2 == EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_BAD_ACCESS);
	
	INFO_PRINTF1(_L("Destroying the Child VGImage"));
	vgDestroyImage(childVgImage);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);

	INFO_PRINTF1(_L("Trying to create an EGLImage and a new VGImage from the same RSgImage (now it's free)"));
	imageKHR2 = iEglSess->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&sgImage,NULL);
	ASSERT_EGL_TRUE(imageKHR2 != EGL_NO_IMAGE_KHR);
	ASSERT_EGL_ERROR(EGL_SUCCESS);

	//cleanup
	CleanupStack::PopAndDestroy(&sgImage);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR2));		
	CleanAll();

	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}


/**
@SYMTestCaseID GRAPHICS-EGL-0142

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Ensure that the when using a child VGImage contents of the RSgImage are preserved even if the parent VGImage and the EGLImage have been destroyed.

@SYMTestActions
This test will check for the “VG_KHR_EGL_image” extension, if it is not supported on this platform then the test will return immediately without failure.
Create a reference Bitmap
Create and fully construct an RSgImage object having the same content as the reference bitmap 
•	Set the iUsage bit to ESgUsageBitOpenVgImage
Pass the RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that eglCreateImageKHR() does NOT return EGL_NO_IMAGE_KHR
Create and make current and EGL context bound to OVG APIs and linked to a window surface.
Use vgCreateEGLImageTargetKHR() to construct a VGImage object from the EGLImage.
•	Check for errors
Use vgChildImage() to construct child VGImage object from the previous VGImage.
•	Check for errors
Pass the parent VGImage into vgDestroyImage()
Pass the EGLImage into eglDestroyImageKHR()
Use OpenVG APIs to draw the previous child VGImage to this surface.
Check Contents.
Pass the child VGImage into vgDestroyImage()
Close RSgImage
Check for memory and handle leaks.

@SYMTestExpectedResults
What is drawn to the surface is exactly what is expected to be inside the child VGImage.
No memory or handle leaks.
*/
TVerdict CEglTest_EGL_Image_Sibling_VGImage_Child_CheckContents::doTestStepL()
	{	
	INFO_PRINTF1(_L("CEglTest_EGL_Image_Sibling_VGImage_Child_CheckContents::doTestStepL"));
	SetTestStepID(_L("GRAPHICS-EGL-0142"));

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

	RSgImage sgImage;
	CleanupClosePushL(sgImage);

	// Create a reference bitmap which we use to init the SgImage (we use index=8)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 8);
	CleanupStack::PushL(bitmap);

	INFO_PRINTF1(_L("Creating one RSgImage"));	
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(iSourceFormat, KPixmapSize);
	ASSERT_EQUALS(sgImage.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride()), KErrNone);
	CleanupStack::PopAndDestroy(bitmap);
	bitmap = NULL;

	INFO_PRINTF1(_L("Creating one EGLImage from it"));
	EGLImageKHR imageKHR = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR != EGL_NO_IMAGE_KHR);

	//Create a Surface and Link it to a Context bound to OpenVG
	TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
	TSgImageInfoOpenVgTarget imageInfo2 = TSgImageInfoOpenVgTarget(pixelFormat, KPixmapSize);
	iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo2, CTestEglSession::EResourceCloseSgImageEarly);

	INFO_PRINTF1(_L("Creating one VGImage from it"));
	VGImage vgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR);
	ASSERT_VG_TRUE(vgImage != VG_INVALID_HANDLE);
	
	INFO_PRINTF1(_L("Creating a VGImage Child from the previous VGImage"));
	VGImage childVgImage = vgChildImage(vgImage,0,0,KPixmapSize.iWidth,KPixmapSize.iHeight);
	ASSERT_EGL_TRUE(childVgImage != VG_INVALID_HANDLE);

	INFO_PRINTF1(_L("Destroying the RSgImage, the EGLImage, and the parent VGImage"));
	CleanupStack::PopAndDestroy(&sgImage);								// closes RSgImage
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));		// closes EGLImage
	vgDestroyImage(vgImage);											// closes VGImage (parent)
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);

	INFO_PRINTF1(_L("Draw the child VGImage to the surface and check contents"));
	vgSetPixels(0, 0, childVgImage, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	eglWaitClient();
	
	// To check the contents we need to repopulate the reference bitmap (with same index used during init)
	TDisplayMode refBitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
	CFbsBitmap* refBitmap = iEglSess->CreateReferenceBitmapL(refBitmapMode, KPixmapSize, 8);
	CleanupStack::PushL(refBitmap);
	iEglSess->CheckVgDrawingL(iSurfaceFormat, refBitmap);
	CleanupStack::PopAndDestroy(refBitmap);
	INFO_PRINTF1(_L("Content is as expected"));

	//cleanup
	vgDestroyImage(childVgImage);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	CleanAll();
	
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}


/**
@SYMTestCaseID GRAPHICS-EGL-0143

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Ensure that when using a child VGImage contents of the RSgImage are preserved even 
if the parent VGImage, the EGLImage and the original RSgImage have been destroyed.
This test that the link of references fron the child VGImage to the actual content is 
valid even when it is the only reference left to the data.

@SYMTestActions
Perform the exact action described in GRAPHICS-EGL-0213 but close the RSgImage too.

@SYMTestExpectedResults
What is drawn to the surface is exactly what is expected to be inside the child VGImage.
No memory or handle leaks.
*/
TVerdict CEglTest_EGL_Image_Sibling_VGImage_Child_CheckContents_CloseSgEarlier::doTestStepL()
	{	
	INFO_PRINTF1(_L("CEglTest_EGL_Image_Sibling_VGImage_Child_CheckContents::doTestStepL"));
	SetTestStepID(_L("GRAPHICS-EGL-0142"));

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

	RSgImage sgImage;
	CleanupClosePushL(sgImage);

	// Create a reference bitmap which we use to init the SgImage (we use index=8)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 8);
	CleanupStack::PushL(bitmap);

	INFO_PRINTF1(_L("Creating one RSgImage"));	
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(iSourceFormat, KPixmapSize);
	ASSERT_EQUALS(sgImage.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride()), KErrNone);
	CleanupStack::PopAndDestroy(bitmap);
	bitmap = NULL;

	INFO_PRINTF1(_L("Creating one EGLImage from it"));
	EGLImageKHR imageKHR = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR != EGL_NO_IMAGE_KHR);

	//Close the SgDriver EGL Implemtation should have its own reference to it
	CleanupStack::PopAndDestroy(&sgImage);								// closes RSgImage
	iEglSess->CloseSgDriver();
	
	//Create a Surface and Link it to a Context bound to OpenVG
	TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
	TSgImageInfoOpenVgTarget imageInfo2 = TSgImageInfoOpenVgTarget(pixelFormat, KPixmapSize);
	iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo2, CTestEglSession::EResourceCloseSgImageEarly);

	INFO_PRINTF1(_L("Creating one VGImage from it"));
	VGImage vgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR);
	ASSERT_VG_TRUE(vgImage != VG_INVALID_HANDLE);
	
	INFO_PRINTF1(_L("Creating a VGImage Child from the previous VGImage"));
	VGImage childVgImage = vgChildImage(vgImage,0,0,KPixmapSize.iWidth,KPixmapSize.iHeight);
	ASSERT_EGL_TRUE(childVgImage != VG_INVALID_HANDLE);

	INFO_PRINTF1(_L("Destroying the RSgImage, the EGLImage, and the parent VGImage"));
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));		// closes EGLImage
	vgDestroyImage(vgImage);											// closes VGImage (parent)
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);

	INFO_PRINTF1(_L("Draw the child VGImage to the surface and check contents"));
	vgSetPixels(0, 0, childVgImage, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	eglWaitClient();
	
	// To check the contents we need to repopulate the reference bitmap (with same index used during init)
	TDisplayMode refBitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
	CFbsBitmap* refBitmap = iEglSess->CreateReferenceBitmapL(refBitmapMode, KPixmapSize, 8);
	CleanupStack::PushL(refBitmap);
	iEglSess->CheckVgDrawingL(iSurfaceFormat, refBitmap);
	CleanupStack::PopAndDestroy(refBitmap);
	INFO_PRINTF1(_L("Content is as expected"));

	//cleanup
	vgDestroyImage(childVgImage);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	CleanAll();
	
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}
