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
#include <iniparser.h>

#include "egltest_vgimagetosurfaces.h"

#include <test/egltestcommonsgimageinfo.h>
#include <test/egltestcommonconversion.h>
#include <test/egltestcommoninisettings.h>



/**
@SYMTestCaseID GRAPHICS-EGL-0170

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Check that it is possible to draw a VGImage (created from an RSgImage) to a pBuffer Surface.

@SYMTestActions
Create a reference Bitmap
Create and fully construct an RSgImage object having the same content as the reference bitmap 
•	Set the iUsage bit to ESgUsageBitOpenVgImage
Create and make current and EGL context bound to OVG APIs and linked to a pBuffer surface.
Pass the RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that eglCreateImageKHR() does NOT return EGL_NO_IMAGE_KHR
Use vgCreateEGLImageTargetKHR() to construct a VGImage object from the EGLImage.
•	Check for errors
Use OpenVG to draw the just created VGImage to the pBuffer surface currently linked to the context.
Call eglWaitClient() to finish the above drawing instructions synchronously.
Destroy the original image data
•	Pass the VGImage into vgDestroyImage()
•	Pass the EGLImage into eglDestroyImageKHR()
•	Close the RSgImage 
Check that the pBuffer surface matches the content from the reference bitmap.
Destroy the pBuffer surface
Check for memory and handle leaks

@SYMTestExpectedResults
pBuffer surface matches the content from the reference bitmap.
The test will loop through all the possible PixelFormat available for source RSgImages and Destination Surfaces
•	EUidPixelFormatRGB_565
•	EUidPixelFormatXRGB_8888
•	EUidPixelFormatARGB_8888 (source only)
•	EUidPixelFormatARGB_8888_PRE
•	EUidPixelFormatA_8  (source only)
*/
TVerdict CEglTest_EGL_Image_VgImage_To_pBuffer::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0170"));
	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	CEglTestCommonIniSettings* iniParser = CEglTestCommonIniSettings::NewL();
	CleanupStack::PushL(iniParser);

	TInt numPBufferSurfaceFormats = iniParser->GetNumberOfFormats(KSectionPBufferSurfaceFormats);
	TInt numImageSourceFormats = iniParser->GetNumberOfFormats(KSectionImageSourceFormats);
	if(!numImageSourceFormats && !numPBufferSurfaceFormats)
		{
		ERR_PRINTF1(_L("No formats to iterate through"));
		User::Leave(KErrArgument);
		}
	for(TUint j=0; j < numPBufferSurfaceFormats; j++)
		{
		iSurfaceFormat = iniParser->GetVgFormat(KSectionPBufferSurfaceFormats,j);
		for(TUint i=0; i < numImageSourceFormats; i++)
			{
			iSourceFormat = iniParser->GetPixelFormat(KSectionImageSourceFormats,i);
			if (iSourceFormat == EUidPixelFormatARGB_8888 && (iSurfaceFormat == VG_sARGB_8888_PRE || iSurfaceFormat == VG_sARGB_8888))
				{
				// Don't perform the test for this particular format combination
				//  Causes issues converting pixel values from non-pre to pre
				continue;
				}
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
			// A_8 related tests are only performed for SgImage-Lite
			if (iSourceFormat == EUidPixelFormatA_8)
				continue;
#endif	
			doTestPartialStepL();
			}
		}

	CleanupStack::PopAndDestroy(iniParser);
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

TVerdict CEglTest_EGL_Image_VgImage_To_pBuffer::doTestPartialStepL()
	{
	INFO_PRINTF1(_L("CEglTest_EGL_Image_VgImage_To_pBuffer::doTestPartialStepL"));
	PrintUsedPixelConfiguration();

	// Create display object
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	// Create a reference bitmap which we use to init the SgImage (we use index=7)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 7);
	CleanupStack::PushL(bitmap);

	INFO_PRINTF1(_L("Creating 1 RSgImage"));
	RSgImage sgImage;
	CleanupClosePushL(sgImage);
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(iSourceFormat, KPixmapSize);
	TInt ret = sgImage.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride());
	ASSERT_EQUALS(ret, KErrNone);
	
	// If create pbuffer surface fails, don't fail if there's no matching config (but fail in any other situation):
    //		Support for a window surface is not mandated any pixel format
    //		We should not enforce it in the tests
    //		Silently terminate the test without failing.
	TEglTestConfig pbufferFormat = EglTestConversion::VgFormatToPBufferSurfaceFormat(iSurfaceFormat);
	EGLConfig currentConfig = 0;
	TRAPD(res, currentConfig = iEglSess->GetConfigExactMatchL( pbufferFormat ));
	if(res == KTestNoMatchingConfig)
	    {
        INFO_PRINTF1(_L("Creation of pBuffer has failed, the test case for that configuration will be skipped"));
		CleanupStack::PopAndDestroy(2, bitmap); // bitmap, sgimage
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
	    }
	User::LeaveIfError(res); // leave here if any other error was raised other than the one above.
	iEglSess->CreatePbufferSurfaceAndMakeCurrentL(currentConfig, KPixmapSize, EGL_OPENVG_API);

	INFO_PRINTF1(_L("Creating 1 EGLImage from the RSgImage"));
	EGLImageKHR imageKHR = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR != EGL_NO_IMAGE_KHR);

	INFO_PRINTF1(_L("Creating 1 VGImage from the EGLImage"));
	VGImage vgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR);
	ASSERT_VG_TRUE(vgImage != VG_INVALID_HANDLE);

	// At this point we draw the VGImage created from the SgImage to the current surface.
	//	# if the source is a A_8, the VGImage acts as a mask and the target surface must contain 
	//		as a result the pen colour set above blended with the mask
	//	# otherwise, drawing the VGImage is just a simple copy via vgSetPixels (no blending required) 
	INFO_PRINTF1(_L("Copying the VGImage to the surface"));
	if (iSourceFormat == EUidPixelFormatA_8)
		{
		// clear surface background
		VGfloat bgColor[] = {0.0, 0.0, 0.0, 1.0}; // opaque black
		vgSetfv(VG_CLEAR_COLOR, 4, bgColor);
		vgClear(0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);

		// fill paint
		VGPaint fillPaint = vgCreatePaint();
		vgSetPaint(fillPaint, VG_FILL_PATH);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		vgSetParameteri(fillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
		VGuint fillColor = 0x00ff00ff; // opaque green
		vgSetColor(fillPaint, fillColor);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		
		vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_STENCIL);
		vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);
		vgDrawImage(vgImage);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		eglWaitClient();
	    vgDestroyPaint(fillPaint);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		}
	else
		{
		vgSetPixels(0, 0, vgImage, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
		eglWaitClient();
		}
	
	// destroy eglimage and vgimage
	vgDestroyImage(vgImage);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));

	// Check that the surface contains the expected pixels
	//  # if the source is a A_8, to compare the surface with a reference bitmap, the following is needed:
	//    a) a reference bitmap needs to be cleared to black (same colour as the surface was cleared to)
	//    b) a Pen bitmap, that we clear to green (same colour as the fillPaint used to draw to the surface)
	//    c) a mask bitmap, which is the reference bitmap used to create the SgImage
	//  # otherwise, the surface must contain the same pixels as the bitmap used to create the SgImage 
	if (iSourceFormat == EUidPixelFormatA_8)
		{
		// we need a reference bitmap with the same pixel format as the target surface
		TUidPixelFormat format = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
		TDisplayMode refbitmapMode = EglTestConversion::PixelFormatToDisplayMode(format);

		CFbsBitmap* refBitmap = iEglSess->CreateReferenceMaskedBitmapL(refbitmapMode, KRgbGreen, bitmap);
		CleanupStack::PushL(refBitmap);
		
		// compare the obtained reference bitmap with the surface drawn
		iEglSess->CheckVgDrawingL(iSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(refBitmap);
		}
	else
		{
		iEglSess->CheckVgDrawingL(iSurfaceFormat, bitmap);
		}
	INFO_PRINTF1(_L("Content is as expected"));
	
	//cleanup
	CleanupStack::PopAndDestroy(2, bitmap); // bitmap, sgImage
	CleanAll();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0171

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Check that it is possible to draw a VGImage (created from an RSgImage) to a Window Surface.

@SYMTestActions
Create a reference Bitmap
Create an RWindow.
Create and fully construct an RSgImage object having the same content as the reference bitmap 
•	Set the iUsage bit to ESgUsageBitOpenVgImage
Create and make current and EGL context bound to OVG APIs and linked to a Window Surface 
(linked to the previously created RWindow).
Pass the RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that eglCreateImageKHR() does NOT return EGL_NO_IMAGE_KHR
Use vgCreateEGLImageTargetKHR() to construct a VGImage object from the EGLImage.
•	Check for errors
Use OpenVG to draw the just drawn VGImage to the Window surface currently linked to the context.
Call eglWaitClient() to finish the above drawing instructions synchronously.
Destroy the original image data
•	Pass the VGImage into vgDestroyImage()
•	Pass the EGLImage into eglDestroyImageKHR()
•	Close the RSgImage 
Check that the Window surface matches the content from the reference bitmap.
Destroy the Window surface
Check for memory and handle leaks

@SYMTestExpectedResults
Window surface matches the content from the reference bitmap.
The test will loop through all the possible PixelFormat available for source 
RSgImages and Destination Surfaces (accordingly to whether or not the test 
is being run on hardware where just the 16MU format is supported for window surfaces)
•	EUidPixelFormatRGB_565
•	EUidPixelFormatXRGB_8888
•	EUidPixelFormatARGB_8888 (source only)
•	EUidPixelFormatARGB_8888_PRE
•	EUidPixelFormatA_8  (source only)
No memory or handle leaks.
*/
TVerdict CEglTest_EGL_Image_VgImage_To_Window::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0171"));
	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	CEglTestCommonIniSettings* iniParser = CEglTestCommonIniSettings::NewL();
	CleanupStack::PushL(iniParser);

	TInt numWindowSurfaceFormats = iniParser->GetNumberOfFormats(KSectionWindowSurfaceFormats);
	TInt numImageSourceFormats = iniParser->GetNumberOfFormats(KSectionImageSourceFormats);
	if(!numImageSourceFormats && !numWindowSurfaceFormats)
		{
		ERR_PRINTF1(_L("No formats to iterate through"));
		User::Leave(KErrArgument);
		}
	for(TUint j=0; j < numWindowSurfaceFormats; j++)
		{
		iSurfaceFormat = iniParser->GetVgFormat(KSectionWindowSurfaceFormats,j);
		for(TUint i=0; i < numImageSourceFormats; i++)
			{
			iSourceFormat = iniParser->GetPixelFormat(KSectionImageSourceFormats,i);
			if (iSourceFormat == EUidPixelFormatARGB_8888 && (iSurfaceFormat == VG_sARGB_8888_PRE || iSurfaceFormat == VG_sARGB_8888))
				{
				// Don't perform the test for this particular format combination
				//  Causes issues converting pixel values from non-pre to pre
				continue;
				}
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
			// A_8 related tests are only performed for SgImage-Lite
			if (iSourceFormat == EUidPixelFormatA_8)
				continue;
#endif	
			doTestPartialStepL();
			}
		}

	CleanupStack::PopAndDestroy(iniParser);
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

TVerdict CEglTest_EGL_Image_VgImage_To_Window::doTestPartialStepL()
	{
	INFO_PRINTF1(_L("CEglTest_EGL_Image_VgImage_To_Window::doTestPartialStepL"));
	PrintUsedPixelConfiguration();
	
	OpenWsSessionL(KDefaultWindowGroupId);
	RWindow window;
	ConstructWindowL(window, KPixmapSize);
	CleanupClosePushL(window);

	// Create display object
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	// Create a reference bitmap which we use to init the SgImage (we use index=2)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 2);
	CleanupStack::PushL(bitmap);

	INFO_PRINTF1(_L("Creating 1 RSgImage"));
	RSgImage sgImage;
	CleanupClosePushL(sgImage);
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(iSourceFormat, KPixmapSize);
	TInt ret = sgImage.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride());
	ASSERT_EQUALS(ret, KErrNone);

	// If create window surface fails, don't fail if there's no matching config (but fail in any other situation):
    //		Support for a window surface is not mandated any pixel format
    //		We should not enforce it in the tests
    //		Silently terminate the test without failing.
	TEglTestConfig windowFormat = EglTestConversion::VgFormatToWindowSurfaceFormat(iSurfaceFormat);
	EGLConfig config = 0;
	TRAPD(res, config = iEglSess->GetConfigExactMatchL( windowFormat ));
	if(res == KTestNoMatchingConfig)
		{
		INFO_PRINTF1(_L("Creation of window surface has failed, the test case for that configuration will be skipped"));
		CleanupStack::PopAndDestroy(3, &window); // window, bitmap, sgimage
		CloseWsSession();
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}
    User::LeaveIfError(res); // leave here if any other error was raised other than the one above.
	iEglSess->CreateWindowSurfaceAndMakeCurrentL(config, window, windowFormat == EWindowAttribsColor16MAP);

	INFO_PRINTF1(_L("Creating 1 EGLImage from the RSgImage"));
	EGLImageKHR imageKHR = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR != EGL_NO_IMAGE_KHR);

	INFO_PRINTF1(_L("Creating 1 VGImage from the EGLImage"));
	VGImage vgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR);
	ASSERT_VG_TRUE(vgImage != VG_INVALID_HANDLE);
	    
	// At this point we draw the VGImage created from the SgImage to the current surface.
	//	# if the source is a A_8, the VGImage acts as a mask and the target surface must contain 
	//		as a result the pen colour set above blended with the mask
	//	# otherwise, drawing the VGImage is just a simple copy via vgSetPixels (no blending required) 
	INFO_PRINTF1(_L("Copying the VGImage to the surface"));
	if (iSourceFormat == EUidPixelFormatA_8)
		{
		// clear surface background
		VGfloat bgColor[] = {0.0, 0.0, 0.0, 1.0}; // opaque black
		vgSetfv(VG_CLEAR_COLOR, 4, bgColor);
		vgClear(0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);

		// fill paint
		VGPaint fillPaint = vgCreatePaint();
		vgSetPaint(fillPaint, VG_FILL_PATH);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		vgSetParameteri(fillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
		VGuint fillColor = 0x00ff00ff; // opaque green
		vgSetColor(fillPaint, fillColor);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		
		vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_STENCIL);
		vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);
		vgDrawImage(vgImage);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		eglWaitClient();
	    vgDestroyPaint(fillPaint);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		}
	else
		{
		vgSetPixels(0, 0, vgImage, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
		eglWaitClient();
		}
	
	// destroy eglimage and vgimage
	vgDestroyImage(vgImage);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));

	// Check that the surface contains the expected pixels
	//  # if the source is a A_8, to compare the surface with a reference bitmap, the following is needed:
	//    a) a reference bitmap needs to be cleared to black (same colour as the surface was cleared to)
	//    b) a Pen bitmap, that we clear to green (same colour as the fillPaint used to draw to the surface)
	//    c) a mask bitmap, which is the reference bitmap used to create the SgImage
	//  # otherwise, the surface must contain the same pixels as the bitmap used to create the SgImage 
	if (iSourceFormat == EUidPixelFormatA_8)
		{
		// we need a reference bitmap with the same pixel format as the target surface
		TUidPixelFormat format = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
		TDisplayMode refbitmapMode = EglTestConversion::PixelFormatToDisplayMode(format);

		CFbsBitmap* refBitmap = iEglSess->CreateReferenceMaskedBitmapL(refbitmapMode, KRgbGreen, bitmap);
		CleanupStack::PushL(refBitmap);
		
		// compare the obtained reference bitmap with the surface drawn
		iEglSess->CheckVgDrawingL(iSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(refBitmap);
		}
	else
		{
		iEglSess->CheckVgDrawingL(iSurfaceFormat, bitmap);
		}
	INFO_PRINTF1(_L("Content is as expected"));
    eglSwapBuffers(iDisplay,iEglSess->Surface());
	
    // cleanup, destroy the window surface before native window.
    iEglSess->CleanupSurfaceAndContextL();
	CleanupStack::PopAndDestroy(3, &window); // window, bitmap, sgimage
	CloseWsSession();
	CleanAll();
	return TestStepResult();
	}


/**
@SYMTestCaseID GRAPHICS-EGL-0172

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Check that it is possible to draw a VGImage (created from an RSgImage) to a pixmap surface created from an RSGImage

@SYMTestActions
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
Use OpenVG to copy the VGImage to the pixmap surface currently linked to the context.
Call eglWaitClient() to finish the above drawing instructions synchronously.
Destroy the original image data
•	Pass the VGImage into vgDestroyImage()
•	Pass the EGLImage into eglDestroyImageKHR()
•	Close the RSgImage 
Check that the pixmap surface matches the content from the reference bitmap.
Destroy the pixmap surface
Destroy the RSgImage
Check for memory and handle leaks

@SYMTestExpectedResults
pixmap surface matches the content from the reference bitmap 
The test will loop through all the possible PixelFormat available for source 
RSgImages and Destination Surfaces
•	EUidPixelFormatRGB_565
•	EUidPixelFormatXRGB_8888
•	EUidPixelFormatARGB_8888 (source only)
•	EUidPixelFormatARGB_8888_PRE
•	EUidPixelFormatA_8  (source only)
No memory or handle leaks
*/
TVerdict CEglTest_EGL_Image_VgImage_To_Pixmap::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0172"));
	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	CEglTestCommonIniSettings* iniParser = CEglTestCommonIniSettings::NewL();
	CleanupStack::PushL(iniParser);

	TInt numPixmapSgSurfaceFormats = iniParser->GetNumberOfFormats(KSectionPixmapSgSurfaceFormats);
	TInt numImageSourceFormats = iniParser->GetNumberOfFormats(KSectionImageSourceFormats);
	if(!numImageSourceFormats && !numPixmapSgSurfaceFormats)
		{
		ERR_PRINTF1(_L("No formats to iterate through"));
		User::Leave(KErrArgument);
		}
	for(TUint j=0; j < numPixmapSgSurfaceFormats; j++)
		{
		iSurfaceFormat = iniParser->GetVgFormat(KSectionPixmapSgSurfaceFormats,j);
		for(TUint i=0; i < numImageSourceFormats; i++)
			{
			iSourceFormat = iniParser->GetPixelFormat(KSectionImageSourceFormats,i);
			if (iSourceFormat == EUidPixelFormatARGB_8888 && (iSurfaceFormat == VG_sARGB_8888_PRE || iSurfaceFormat == VG_sARGB_8888))
				{
				// Don't perform the test for this particular format combination
				//  Causes issues converting pixel values from non-pre to pre
				continue;
				}
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
			// A_8 related tests are only performed for SgImage-Lite
			if (iSourceFormat == EUidPixelFormatA_8)
				continue;
#endif	
			doTestPartialStepL();
			}
		}

	CleanupStack::PopAndDestroy(iniParser);
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

TVerdict CEglTest_EGL_Image_VgImage_To_Pixmap::doTestPartialStepL()
	{
	INFO_PRINTF1(_L("CEglTest_EGL_Image_VgImage_To_Pixmap::doTestPartialStepL"));
	PrintUsedPixelConfiguration();

	// Create display object
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	// Create a reference bitmap which we use to init the SgImage (we use index=7)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 7);
	CleanupStack::PushL(bitmap);

	INFO_PRINTF1(_L("Creating 1 RSgImage"));
	RSgImage sgImage;
	CleanupClosePushL(sgImage);
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(iSourceFormat, KPixmapSize);
	TInt ret = sgImage.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride());
	ASSERT_EQUALS(ret, KErrNone);
	
	INFO_PRINTF1(_L("Creating 1 EGLImage from it"));
	EGLImageKHR imageKHR = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR != EGL_NO_IMAGE_KHR);

	TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
	TSgImageInfoOpenVgTarget imageInfo2 = TSgImageInfoOpenVgTarget(pixelFormat, KPixmapSize);
	// Create a pixmap surface matching the native image pixel format
	iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo2, CTestEglSession::EResourceCloseSgImageEarly);
 
	INFO_PRINTF1(_L("Creating 1 VGImage from the EGLImage"));
	VGImage vgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR);
	ASSERT_VG_TRUE(vgImage != VG_INVALID_HANDLE);

	// At this point we draw the VGImage created from the SgImage to the current surface.
	//	# if the source is a A_8, the VGImage acts as a mask and the target surface must contain 
	//		as a result the pen colour set above blended with the mask
	//	# otherwise, drawing the VGImage is just a simple copy via vgSetPixels (no blending required) 
	INFO_PRINTF1(_L("Copying the VGImage to the surface"));
	if (iSourceFormat == EUidPixelFormatA_8)
		{
		// clear surface background
		VGfloat bgColor[] = {0.0, 0.0, 0.0, 1.0}; // opaque black
		vgSetfv(VG_CLEAR_COLOR, 4, bgColor);
		vgClear(0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);

		// fill paint
		VGPaint fillPaint = vgCreatePaint();
		vgSetPaint(fillPaint, VG_FILL_PATH);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		vgSetParameteri(fillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
		VGuint fillColor = 0x00ff00ff; // opaque green
		vgSetColor(fillPaint, fillColor);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		
		vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_STENCIL);
		vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);
		vgDrawImage(vgImage);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		eglWaitClient();
	    vgDestroyPaint(fillPaint);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		}
	else
		{
		vgSetPixels(0, 0, vgImage, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
		eglWaitClient();
		}
	
	// destroy eglimage and vgimage
	vgDestroyImage(vgImage);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));

	// Check that the surface contains the expected pixels
	//  # if the source is a A_8, to compare the surface with a reference bitmap, the following is needed:
	//    a) a reference bitmap needs to be cleared to black (same colour as the surface was cleared to)
	//    b) a Pen bitmap, that we clear to green (same colour as the fillPaint used to draw to the surface)
	//    c) a mask bitmap, which is the reference bitmap used to create the SgImage
	//  # otherwise, the surface must contain the same pixels as the bitmap used to create the SgImage 
	if (iSourceFormat == EUidPixelFormatA_8)
		{
		// we need a reference bitmap with the same pixel format as the target surface
		TUidPixelFormat format = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
		TDisplayMode refbitmapMode = EglTestConversion::PixelFormatToDisplayMode(format);

		CFbsBitmap* refBitmap = iEglSess->CreateReferenceMaskedBitmapL(refbitmapMode, KRgbGreen, bitmap);
		CleanupStack::PushL(refBitmap);
		
		// compare the obtained reference bitmap with the surface drawn
		iEglSess->CheckVgDrawingL(iSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(refBitmap);
		}
	else
		{
		iEglSess->CheckVgDrawingL(iSurfaceFormat, bitmap);
		}
	INFO_PRINTF1(_L("Content is as expected"));
	
	//cleanup
	CleanupStack::PopAndDestroy(2, bitmap); // bitmap, sgImage
	CleanAll();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0173

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Check that it is possible to draw a VGImage (created from an RSgImage) to a pixmap surface based upon a CFbsBitmap, created from a CFbsBitmap Surface,.

@SYMTestActions
Create a reference Bitmap
Create and fully construct an RSgImage object having the same content as the reference bitmap 
•	Set the iUsage bit to ESgUsageBitOpenVgImage
Create and make current and EGL context bound to OVG APIs and linked to a pixmap surface based upon a CFbsBitmap.
Pass the RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that eglCreateImageKHR() does NOT return EGL_NO_IMAGE_KHR
Use vgCreateEGLImageTargetKHR() to construct a VGImage object from the EGLImage.
•	Check for errors
Use OpenVG to copy the VGImage to the pixmap surface currently linked to the context.
Call eglWaitClient() to finish the above drawing instructions synchronously.
Destroy the original image data
•	Pass the VGImage into vgDestroyImage()
•	Pass the EGLImage into eglDestroyImageKHR()
•	Close the RSgImage 
Check that the pixmap surface matches the content from the reference bitmap.
Destroy the pixmap surface
Destroy the CFbsBitmap
Check for memory and handle leaks

@SYMTestExpectedResults
pixmap surface matches the content from the reference bitmap
The test will loop through all the possible PixelFormat available for source RSgImages and Destination Surfaces
•	EUidPixelFormatRGB_565
•	EUidPixelFormatXRGB_8888
•	EUidPixelFormatARGB_8888 (source only)
•	EUidPixelFormatARGB_8888_PRE
•	EUidPixelFormatA_8  (source only)
No memory or handle leaks
*/
TVerdict CEglTest_EGL_Image_VGImage_To_Pixmap_CFbs::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0173"));
	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	CEglTestCommonIniSettings* iniParser = CEglTestCommonIniSettings::NewL();
	CleanupStack::PushL(iniParser);

	TInt numPixmapFbsSurfaceFormats = iniParser->GetNumberOfFormats(KSectionPixmapFbsSurfaceFormats);
	TInt numImageSourceFormats = iniParser->GetNumberOfFormats(KSectionImageSourceFormats);
	if(!numImageSourceFormats && !numPixmapFbsSurfaceFormats)
		{
		ERR_PRINTF1(_L("No formats to iterate through"));
		User::Leave(KErrArgument);
		}
	for(TUint j=0; j < numPixmapFbsSurfaceFormats; j++)
		{
		iSurfaceFormat = iniParser->GetVgFormat(KSectionPixmapSgSurfaceFormats,j);
		for(TUint i=0; i < numImageSourceFormats; i++)
			{
			iSourceFormat = iniParser->GetPixelFormat(KSectionImageSourceFormats,i);
			if (iSourceFormat == EUidPixelFormatARGB_8888 && (iSurfaceFormat == VG_sARGB_8888_PRE || iSurfaceFormat == VG_sARGB_8888))
				{
				// Don't perform the test for this particular format combination
				//  Causes issues converting pixel values from non-pre to pre
				continue;
				}
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
			// A_8 related tests are only performed for SgImage-Lite
			if (iSourceFormat == EUidPixelFormatA_8)
				continue;
#endif	
			doTestPartialStepL();
			}
		}

	CleanupStack::PopAndDestroy(iniParser);
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

TVerdict CEglTest_EGL_Image_VGImage_To_Pixmap_CFbs::doTestPartialStepL()
	{
	INFO_PRINTF1(_L("CEglTest_EGL_Image_VGImage_To_Pixmap_CFBS::doTestPartialStepL"));
	PrintUsedPixelConfiguration();

	// Create display object
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();


	// Create a reference bitmap which we use to init the SgImage (we use index=7)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 7);
	CleanupStack::PushL(bitmap);

	INFO_PRINTF1(_L("Creating 1 RSgImage"));
	RSgImage sgImage;
	CleanupClosePushL(sgImage);
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(iSourceFormat, KPixmapSize);
	TInt ret = sgImage.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride());
	ASSERT_EQUALS(ret, KErrNone);
	
	// If create pixmap fails, don't fail if there's no matching config (but fail in any other situation):
    //		Support for a pixmap surface from CFbsBitmap is not mandatory
    //		We should not enforce it in the tests
    //		Silently terminate the test without failing.
	TDisplayMode surfaceBitmapMode = EglTestConversion::VgFormatToDisplayMode(iSurfaceFormat);
	TRAPD(res, iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(KPixmapSize, surfaceBitmapMode, EGL_OPENVG_API));
    if(res == KTestNoMatchingConfig)
		{
	    WARN_PRINTF2(_L("Can't create pixmap surface based on bitmap in mode %d, the test will be skipped"), surfaceBitmapMode);
		CleanupStack::PopAndDestroy(bitmap); 
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}
    User::LeaveIfError(res); // leave here if any other error was raised other than the one above.

	INFO_PRINTF1(_L("Creating 1 EGLImage from the RSgImage"));
	// Create an EGLImage from the RSgImage
	EGLImageKHR imageKHR = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImage, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(imageKHR != EGL_NO_IMAGE_KHR);

	INFO_PRINTF1(_L("Creating 1 VGImage from the EGLImage"));
	VGImage vgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)imageKHR);
	ASSERT_VG_TRUE(vgImage != VG_INVALID_HANDLE);

	// At this point we draw the VGImage created from the SgImage to the current surface.
	//	# if the source is a A_8, the VGImage acts as a mask and the target surface must contain 
	//		as a result the pen colour set above blended with the mask
	//	# otherwise, drawing the VGImage is just a simple copy via vgSetPixels (no blending required) 
	INFO_PRINTF1(_L("Copying the VGImage to the surface"));
	if (iSourceFormat == EUidPixelFormatA_8)
		{
		// clear surface background
		VGfloat bgColor[] = {0.0, 0.0, 0.0, 1.0}; // opaque black
		vgSetfv(VG_CLEAR_COLOR, 4, bgColor);
		vgClear(0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);

		// fill paint
		VGPaint fillPaint = vgCreatePaint();
		vgSetPaint(fillPaint, VG_FILL_PATH);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		vgSetParameteri(fillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
		VGuint fillColor = 0x00ff00ff; // opaque green
		vgSetColor(fillPaint, fillColor);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		
		vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_STENCIL);
		vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);
		vgDrawImage(vgImage);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		eglWaitClient();
	    vgDestroyPaint(fillPaint);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		}
	else
		{
		vgSetPixels(0, 0, vgImage, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
		eglWaitClient();
		}
	
	// destroy eglimage and vgimage
	vgDestroyImage(vgImage);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, imageKHR));

	// Check that the surface contains the expected pixels
	//  # if the source is a A_8, to compare the surface with a reference bitmap, the following is needed:
	//    a) a reference bitmap needs to be cleared to black (same colour as the surface was cleared to)
	//    b) a Pen bitmap, that we clear to green (same colour as the fillPaint used to draw to the surface)
	//    c) a mask bitmap, which is the original reference bitmap used to create the SgImage
	//  # otherwise, the surface must contain the same pixels as the bitmap used to create the SgImage 
	if (iSourceFormat == EUidPixelFormatA_8)
		{
		// we need a reference bitmap with the same pixel format as the target surface
		TUidPixelFormat format = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
		TDisplayMode refbitmapMode = EglTestConversion::PixelFormatToDisplayMode(format);

		CFbsBitmap* refBitmap = iEglSess->CreateReferenceMaskedBitmapL(refbitmapMode, KRgbGreen, bitmap);
		CleanupStack::PushL(refBitmap);
		
		// compare the obtained reference bitmap with the surface drawn
		iEglSess->CheckVgDrawingL(iSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(refBitmap);
		}
	else
		{
		iEglSess->CheckVgDrawingL(iSurfaceFormat, bitmap);
		}
	INFO_PRINTF1(_L("Content is as expected"));
	
	//cleanup
	CleanupStack::PopAndDestroy(2, bitmap); //bitmap, sgImage 
	CleanAll();
	return TestStepResult();
	}
