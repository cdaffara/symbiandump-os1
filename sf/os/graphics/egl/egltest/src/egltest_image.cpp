// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "egltest_image.h"

#include <iniparser.h>
#include <test/tefunit.h> // for ASSERT macros

#include <test/egltestcommonconversion.h>
#include <test/egltestcommoninisettings.h>
#include <test/egltestcommonsgimageinfo.h>


/**
@SYMTestCaseID GRAPHICS-EGL-0107

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Tests the use of an EglImageKHR object with OpenVG, using the “EGL_IMAGE_PRESERVED” attribute.
This means we can draw to the source RSgImage as soon as it is created.

@SYMTestActions
This test will check for the “VG_KHR_EGL_image” extension, if it is not supported on this platform then the test will return immediately without failure.
Create a reference Bitmap
Create and fully construct an RSgImage object having the same content as the reference bitmap
•	Set the iUsage bit to ESgUsageBitOpenVgImage
Pass the RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use the “EGL_IMAGE_PRESERVED” attribute
Check that eglCreateImageKHR() does NOT return EGL_NO_IMAGE_KHR
Create and make current and EGL context bound to OVG APIs and linked to a pixmap surface.
•	Set the iUsage bit of the underlying RSgImage to ESgUsageBitOpenVgSurface 
Use vgCreateEGLImageTargetKHR() to construct a VGImage object from the EGLImage.
•	Check for errors
Use OpenVG to copy  the VGImage created from the EGLImage to the new pixmap surface currently linked to the context.
Call eglWaitClient() to finish the above drawing instructions synchronously.
Destroy the original image data
•	Pass the VGImage into vgDestroyImage()
•	Pass the EGLImage into eglDestroyImageKHR()
•	Close the RSgImage
Check that the pixmap contains expected pixel values.
Destroy the pixmap
Check for memory and handle leaks

@SYMTestExpectedResults
eglCreateImageKHR() does NOT return EGL_NO_IMAGE_KHR
After each call to an OpenVG method, check that vgGetError() returns VG_NO_ERROR
The pixmap contains expected pixel values after the original image data has been destroyed
No memory or handle leaks
*/
TVerdict CEglTest_EGL_Image_RSgImage_UseOpenVG_PersistImageData::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0107"));
	INFO_PRINTF1(_L("CEglTest_EGL_Image_RSgImage_UseOpenVG::doTestStepL"));

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

	// Create RSgImage's attributes.
	//This image will become a VGImage. It will be Cleared via vgClear
	TSgImageInfoTest imageInfo = TSgImageInfoTest(iSourceFormat, KPixmapSize);
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	imageInfo.iUsage = 	ESgUsageBitOpenVgImage | ESgUsageBitOpenVgSurface;
#else
	imageInfo.iUsage =  ESgUsageOpenVgImage | ESgUsageOpenVgTarget;	
#endif	
	INFO_PRINTF1(_L("Creating an RSgImage"));
	RSgImage sgImage;
	CleanupClosePushL(sgImage);
	ret = sgImage.Create(imageInfo, NULL, NULL);
	ASSERT_EQUALS(ret, KErrNone);

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

	//Drawing to the current surface (and hence to the RSgImage) to test that the contents are preserved
	//create a reference bitmap to use the values (we use index=1) 
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 1);
	CleanupStack::PushL(bitmap);
    // Mind the fact that CFbsBitmap and VGImages use different coordinates origin!
	TSize bitmapSize = bitmap->SizeInPixels();
	TUint8* address = reinterpret_cast<TUint8*>(bitmap->DataAddress());
	TInt stride = bitmap->DataStride();
	address += (bitmapSize.iHeight - 1) * stride;
	vgWritePixels(address, -stride, KDefaultSurfaceFormat,0,0, bitmapSize.iWidth, bitmapSize.iHeight);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	eglWaitClient();   // wait for writing to finish
   	ASSERT_EGL_TRUE(eglDestroySurface(iDisplay, surface));				//Destroying Surface handle

	INFO_PRINTF1(_L("Create a EGLImage out of the SgImage"));
	EGLImageKHR imageKHR = iEglSess->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&sgImage,KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR != EGL_NO_IMAGE_KHR);

	INFO_PRINTF1(_L("Create a VGImage out of the EGLImage"));
	VGImage vgImageTarget = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR);
	ASSERT_VG_TRUE(vgImageTarget != VG_INVALID_HANDLE);

	//Create an OffScreen Pixmap to be used as an OpenVg target using the same config of the previous RSgImage
	//except for Usage
	imageInfo.iSizeInPixels = KPixmapSize;
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	imageInfo.iUsage = ESgUsageBitOpenVgSurface;
#else
	imageInfo.iUsage = ESgUsageOpenVgTarget;
#endif
	RSgImage sgImageTarget;
	CleanupClosePushL(sgImageTarget);
	ret = sgImageTarget.Create(imageInfo, NULL, NULL);
	ASSERT_EQUALS(ret, KErrNone);

	surface = eglCreatePixmapSurface(iDisplay, configWithPre, &sgImageTarget, KPixmapAttribsVgAlphaFormatPre);
	ASSERT_EGL_TRUE(surface != EGL_NO_SURFACE);

	INFO_PRINTF1(_L("Calling eglMakeCurrent"));
	ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, surface, surface, context));

    //Copy the source VGImage to the surface
	vgSetPixels(0, 0, vgImageTarget, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	eglWaitClient();

	vgDestroyImage(vgImageTarget);										//Destroying VGImage handle
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));		//Destroying EGLImage handle
	sgImage.Close();													//Destroying SgImage and so the actual data

	// we can now compare the VgImage to the one we would expect for this particular process
	iEglSess->CheckVgDrawingL(iSurfaceFormat, bitmap);
	
	ASSERT_EGL_TRUE(eglDestroyContext(iDisplay, context));				//Closing eglContext
	ASSERT_EGL_TRUE(eglDestroySurface(iDisplay,surface));				//Destroying Target Surface handle
	sgImageTarget.Close();												//Destroying 2nd RSgImage handle
	
	//cleanup
	CleanupStack::PopAndDestroy(3,&sgImage); // sgImage, bitmap, sgImageTarget 
	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0108

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Ensure that if there are more than one EGLImage and some of them are destroyed,
the other images remain workable

@SYMTestActions
Create a reference Bitmap
Create and fully construct 4 RSgImage objects having the same content as the reference bitmap
•	Set the iUsage bit to ESgUsageBitOpenVgImage
Pass the 4 RSgImage objects into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that those calls to eglCreateImageKHR() do NOT return EGL_NO_IMAGE_KHR
Create and make current and EGL context bound to OVG APIs and linked to a pixmap surface.
•	Set the iUsage bit of the underlying RSgImage to ESgUsageBitOpenVgSurface
Use vgCreateEGLImageTargetKHR() to construct VGImage objects from the just created EGLImages.
•	Check for errors
Destroy all the RSgImages.
Pass the 2nd and the 3rd EGLImeges to eglDestroyImageKHR().
Call vgDestroyImage on the 2nd VGImage too.
Use OpenVG to draw the VGImage created from the 4th EGLImage to the new pixmap surface currently linked to the context.
Call eglWaitClient() to finish the above drawing instructions synchronously.
Check that the pixmap contains expected pixel values.
Clear the destination surface to the default background color
Use OpenVG to draw the VGImage created from the 3rd EGLImage to the new pixmap surface currently linked to the context.
Call eglWaitClient() to finish the above drawing instructions synchronously.
Check that the pixmap contains expected pixel values.
Clear the destination surface to the default background color
Destroy and restore the 4th VGImage and copy it to the pixmap surface.
Call eglWaitClient() to finish the above drawing instructions synchronously.
Check that the pixmap contains expected pixel values.
Pass all the VGImages left into vgDestroyImage()
Pass the EGLImage left into eglDestroyImageKHR()
Destroy the pixmap
Check for memory and handle leaks

@SYMTestExpectedResults
Pixmap surface has the expected contents
No memory or handle leaks

*/
TVerdict CEglTest_EGL_Image_Consistent_Linked_List::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0108"));
	INFO_PRINTF1(_L("CEglTest_EGL_Image_Consistent_Linked_List::doTestStepL"));

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

	// Create a reference bitmap which we use to init the SgImage (we use index=6)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 6);
	CleanupStack::PushL(bitmap);
	
	INFO_PRINTF1(_L("Creating 4 RSgImages"));
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(iSourceFormat, KPixmapSize);
	RSgImage sgImage1;
	CleanupClosePushL(sgImage1);
	ASSERT_EQUALS(sgImage1.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride()), KErrNone);
	RSgImage sgImage2;
	CleanupClosePushL(sgImage2);
	ASSERT_EQUALS(sgImage2.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride()), KErrNone);
	RSgImage sgImage3;
	CleanupClosePushL(sgImage3);
	ASSERT_EQUALS(sgImage3.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride()), KErrNone);
	RSgImage sgImage4;
	CleanupClosePushL(sgImage4);
	ASSERT_EQUALS(sgImage4.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride()), KErrNone);
	
	INFO_PRINTF1(_L("Creating 4 EGLImages"));
	EGLImageKHR imageKHR1 = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage1, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR1 != EGL_NO_IMAGE_KHR);
	EGLImageKHR imageKHR2 = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage2, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR2 != EGL_NO_IMAGE_KHR);
	EGLImageKHR imageKHR3 = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage3, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR3 != EGL_NO_IMAGE_KHR);
	EGLImageKHR imageKHR4 = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage4, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR4 != EGL_NO_IMAGE_KHR);
	
	//Create a Surface and Link it to a Context bound to OpenVG
	TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
	TSgImageInfoOpenVgTarget imageInfo2 = TSgImageInfoOpenVgTarget(pixelFormat, KPixmapSize);
	iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo2, CTestEglSession::EResourceCloseSgImageEarly);
	
    INFO_PRINTF1(_L("Creating 4 VGImages from the four EGLImages"));
	VGImage vgImage1 = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR1);
	ASSERT_VG_TRUE(vgImage1 != VG_INVALID_HANDLE);
	VGImage vgImage2 = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR2);
	ASSERT_VG_TRUE(vgImage2 != VG_INVALID_HANDLE);
	VGImage vgImage3 = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR3);
	ASSERT_VG_TRUE(vgImage3 != VG_INVALID_HANDLE);
	VGImage vgImage4 = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR4);
	ASSERT_VG_TRUE(vgImage4 != VG_INVALID_HANDLE);
	
	INFO_PRINTF1(_L("Destroying all RSgImage, the 2nd and the 3rd EGLImage, the 2nd VGImage"));
	CleanupStack::PopAndDestroy(4, &sgImage1); 							// closes the 4 RSgImages
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR2));	//Destroying 2nd EGLImage handle
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR3));	//Destroying 3rd EGLImage handle
	vgDestroyImage(vgImage2);											//Destroying 2nd VGImage handle
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	
	//Copy the source VGImage to the surface
	INFO_PRINTF1(_L("Drawing the 4th VGImage content to the surface"));
	vgSetPixels(0, 0, vgImage4, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	eglWaitClient();
    iEglSess->CheckVgDrawingL(iSurfaceFormat, bitmap);
	INFO_PRINTF1(_L("Content is as expected"));
	
	INFO_PRINTF1(_L("Clear the surface and draw the 3rd VGImage content to the surface"));
	VGfloat bgColor[] = {0.25, 0.50, 0.75, 1.0}; // random opaque colour
	vgSetfv(VG_CLEAR_COLOR, 4, bgColor);
	vgClear(0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
	ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
	vgSetPixels(0, 0, vgImage3, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	eglWaitClient();
    iEglSess->CheckVgDrawingL(iSurfaceFormat, bitmap);
	INFO_PRINTF1(_L("Content is as expected"));
	
	INFO_PRINTF1(_L("Clear the surface, destroy the 4th VGImage and, after having it set up again, copy its content to the surface"));
	vgSetfv(VG_CLEAR_COLOR, 4, bgColor);
	vgClear(0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
	ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
	vgDestroyImage(vgImage4);													//Destroying 4th VGImage handle
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	vgImage4 = VG_INVALID_HANDLE;
	vgImage4 = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR4);		//Creating 4th image again
	ASSERT_VG_TRUE(vgImage4 != VG_INVALID_HANDLE);
	vgSetPixels(0, 0, vgImage4, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	eglWaitClient();
    iEglSess->CheckVgDrawingL(iSurfaceFormat, bitmap);
	INFO_PRINTF1(_L("Content is as expected"));

	INFO_PRINTF1(_L("Destroying remaining images"));
	vgDestroyImage(vgImage1);											//Destroying 1st VGImage handle
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	vgDestroyImage(vgImage3);											//Destroying 3rd VGImage handle
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	vgDestroyImage(vgImage4);											//Destroying 4th VGImage handle
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR1));	//Destroying 1st EGLImage handle
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR4));	//Destroying 4th EGLImage handle

	//cleanup
	CleanupStack::PopAndDestroy(bitmap);
	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0112

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Ensure that the EGL implementation holds a handle to the SgDriver.
In this way all the resource may be used even though a call to SgDriver::Close() has been made.

@SYMTestActions
Create a RSgImage with the same content as the reference bitmap.
Create a EGLImage from the RSgImage.
Close the SgDriver.
Create a pbuffer surface, a context and make them current.
Create a VGImage from the eglImage.
Copy the VGImage to the surface.
Check the contents
Release all resources.

@SYMTestExpectedResults
If creation of pbuffer fails, silently terminate the test without failing, as it is not mandatory to support
a pbuffer format.
The surface contains the expected contents. 
No memory or handle leaks.
*/
TVerdict CEglTest_EGL_Image_SgDriverHandle::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0112"));
	INFO_PRINTF1(_L("CEglTest_EGL_Image_SgDriverHandle::doTestStepL"));

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

	// Create a reference bitmap which we use to init the SgImage (we use index=6)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 6);
	CleanupStack::PushL(bitmap);
	
	INFO_PRINTF1(_L("Creating 1 RSgImage"));
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(iSourceFormat, KPixmapSize);
	ASSERT_EQUALS(sgImage.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride()), KErrNone);
	CleanupStack::PopAndDestroy(bitmap);
	
	INFO_PRINTF1(_L("Creating 1 EGLImage from the RSgImage"));
	EGLImageKHR imageKHR = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR != EGL_NO_IMAGE_KHR);

	//close SgImage before closing the SgDriver
	CleanupStack::PopAndDestroy(&sgImage);

	INFO_PRINTF1(_L("Closing the SgDriver now should cause no difference"));
	iEglSess->CloseSgDriver();
	
	// If create pbuffer surface fails, don't fail if there's no matching config (but fail in any other situation):
    //		Support for a pbuffer surface is not mandated for any pixel format
    //		We should not enforce it in the tests
    //		Silently terminate the test without failing.
	TEglTestConfig pbufferFormat = EglTestConversion::VgFormatToPBufferSurfaceFormat(iSurfaceFormat);
	EGLConfig currentConfig = 0;
	TRAPD(res, currentConfig = iEglSess->GetConfigExactMatchL( pbufferFormat ));
	if(res == KTestNoMatchingConfig)
	    {
		WARN_PRINTF2(_L("Can't create pbuffer, format %d. Terminating the test without failing"), pbufferFormat);
		ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
	    }
    User::LeaveIfError(res); // leave here if any other error was raised other than the one above.
	iEglSess->CreatePbufferSurfaceAndMakeCurrentL(currentConfig, KPixmapSizeBigger, EGL_OPENVG_API);
	
	INFO_PRINTF1(_L("Creating 1 VGImage from the EGLImage"));
	VGImage vgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR);
	ASSERT_VG_TRUE(vgImage != VG_INVALID_HANDLE);

    INFO_PRINTF1(_L("Copying the VGImage to the surface"));
	vgSetPixels(0, 0, vgImage, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	eglWaitClient();
	
	// Re-create a reference bitmap which we use to init the SgImage (we use index=6)
	CFbsBitmap* refBitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 6);
	CleanupStack::PushL(refBitmap);
	iEglSess->CheckVgDrawingL(iSurfaceFormat, refBitmap);
	CleanupStack::PopAndDestroy(refBitmap);
	INFO_PRINTF1(_L("Content is as expected"));

	// destroy eglimage and vgimage
	vgDestroyImage(vgImage);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));

	//cleanup
	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

