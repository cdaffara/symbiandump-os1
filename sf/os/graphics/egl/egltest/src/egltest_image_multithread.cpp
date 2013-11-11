
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
#include "egltest_image_multithread.h"

#include <test/egltestcommonconversion.h>
#include <test/egltestcommonsgimageinfo.h>


CEglTest_MThread_Image_Base::CEglTest_MThread_Image_Base()
	{
	}

CEglTest_MThread_Image_Base::~CEglTest_MThread_Image_Base()
	{
	iSgImageShared.Close();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0155

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Check if EGL Implementation allows two threads to work in parallel.

@SYMTestActions
Run two threads that independently perform the same actions detailed below.
This test will check for the “VG_KHR_EGL_image” extension, if it is not 
supported on this platform then the test will return immediately without failure.
Create and fully construct an RSgImage object
•	Set the iUsage bits to ESgUsageBitOpenVgImage
Pass the RSgImage object into eglCreateImageKHR() with
•	The target parameter set to EGL_NATIVE_PIXMAP_KHR
•	Use the current display and EGL_NO_CONTEXT
•	Use a NULL attr_list
Check that eglCreateImageKHR() does NOT return EGL_NO_IMAGE_KHR
Use vgCreateEGLImageTargetKHR() to construct a VGImage object from the EGLImage.
•	Check for errors (VGInvalidHandle is not returned)
Create a second RSgImage, and use it to create a pixmap surface that is 
compatible as a target for the VGImage to be drawn to.
•	Set the iUsage bit to ESgUsageBitOpenVgSurface
•	Use the same pixel format as the RSgImage above.
Now that a eglContext and an OpenVG context have been created, use the 
OpenVG API vgClearImage to clear to a chosen colour the VGImage previously 
returned by eglCreateImageKHR.
Use OpenVG to draw the just drawn VGImage to the pixmap surface currently 
linked to the context.
Call eglWaitClient() to finish the above drawing instructions synchronously.
Destroy the original image data
•	Pass the VGImage into vgDestroyImage()
•	Pass the EGLImage into eglDestroyImageKHR()
•	Close the first RSgImage
•	Check that the pixmap surface contains expected pixel values using 
OpenVG APIs, vgReadPixels.
Close the second RSgImage and destroy the pixmap surface
Check for memory and handle leaks

@SYMTestExpectedResults
Pixmap surface has the expected contents in both threads (within tolerance).
No memory or handle leaks.
*/
TVerdict CEglTest_EGL_Image_Multi_Thread_Parallel::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0155"));
	INFO_PRINTF1(_L("Enter: CEglTest_EGL_Image_Multi_Thread_Parallel::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	Test_MultiThreadL(2, ETrue);
	TerminateDisplayL();

	INFO_PRINTF1(_L("Exit: CEglTest_EGL_Image_Multi_Thread_Parallel::doTestStepL"));
	RecordTestResultL();	
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

void CEglTest_EGL_Image_Multi_Thread_Parallel::doThreadFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Thread_Parallel::doThreadFunctionL, Thread %d"),aIdx);
	GetDisplayL();
	CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, aIdx);
	eglSess->InitializeL();	
	eglSess->OpenSgDriverL();
	
	// create a reference bitmap (we give index 7, as there's only 1 image in this test case)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
	CFbsBitmap* bitmap = eglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 7);
	CleanupStack::PushL(bitmap);

	// Create an RSgImage
	INFO_PRINTF2(_L("Thread %d, Creating a RSgImage having the reference bitmap's content"),aIdx);
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(KDefaultSourceFormat, KPixmapSize);
    RSgImage rSgImageLocal;
	CleanupClosePushL(rSgImageLocal);
	ASSERT_EQUALS(rSgImageLocal.Create(imageInfo,bitmap->DataAddress(),bitmap->DataStride()), KErrNone);

	INFO_PRINTF2(_L("Thread %d, Creating an EGLImage from the shared RSgImage"),aIdx);
	EGLImageKHR eglImageLocal = eglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &rSgImageLocal, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(eglImageLocal != EGL_NO_IMAGE_KHR);
	CleanupStack::PopAndDestroy(&rSgImageLocal); 	//transferring ownership of the buffer to the EGLImage
	CleanupStack::PopAndDestroy(bitmap);

	INFO_PRINTF2(_L("Thread %d, Creating a Surface and a Context bound to OpenVG"),aIdx);
	TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(KDefaultSurfaceFormat);
	TSgImageInfoOpenVgTarget imageInfo2 = TSgImageInfoOpenVgTarget(pixelFormat, KPixmapSize);
	// Create a pixmap surface matching the native image pixel format
	eglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo2,CTestEglSession::EResourceCloseSgImageEarly);
	
	INFO_PRINTF2(_L("Thread %d, Creating one VGImage from the EGLImage"),aIdx);
	VGImage vgImageLocal = eglSess->vgCreateImageTargetKHR((VGeglImageKHR)eglImageLocal);	
	ASSERT_VG_TRUE(vgImageLocal != VG_INVALID_HANDLE);
	ASSERT_EGL_TRUE(eglSess->DestroyEGLImage(iDisplay, eglImageLocal));
		
	// Copy the source VGImage to the surface
	vgSetPixels(0, 0, vgImageLocal, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	eglWaitClient();

	// destroy VGImage
	vgDestroyImage(vgImageLocal);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);

	// we can now compare the VgImage to the one we would expect for this particular thread
	CFbsBitmap* refBitmap = eglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 7);
	CleanupStack::PushL(refBitmap);
	eglSess->CheckVgDrawingL(KDefaultSurfaceFormat, refBitmap);
	CleanupStack::PopAndDestroy(refBitmap);
	INFO_PRINTF2(_L("Drawing successful, Thread %d"),aIdx);

	// cleanup
	eglSess->CloseSgDriver();
  	CleanupStack::PopAndDestroy(eglSess);	
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0156

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Check if EGL Implementation allows two threads to work in parallel.
EGLImage can’t be created from an RSgImage that already has been linked to an EGLImage by another thread.

@SYMTestActions
Main Thread: creates an RSgImage and starts thread1 and thread2.
Thread1: Creates an EGLImage from the RSgImage previous mentioned
--------
Thread2: Creates an EGLImage from the RSgImage previous mentioned and check that eglCreateImageKHR() does return EGL_NO_IMAGE_KHR
--------
Thread1: Closes the EGLImage
Main Thread: Closes the RSgImage.

@SYMTestExpectedResults
eglCreateImageKHR() does return EGL_NO_IMAGE_KHR in the 2nd thread and EGL_BAD_ACCESS error is generated.
No memory or handle leaks
*/
TVerdict CEglTest_EGL_Image_Multi_Thread_Sibling_Basic::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0156"));
	INFO_PRINTF1(_L("Enter: CEglTest_EGL_Image_Multi_Thread_Sibling_Basic::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	// Create display object
	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
    GetDisplayL();
    CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, 0);
	eglSess->InitializeL();

	// Make sure the driver is ready
	eglSess->OpenSgDriverL();

	// create a reference bitmap (we give index 0, as there's only 1 image in this test case)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
	CFbsBitmap* bitmap = eglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 0);
	CleanupStack::PushL(bitmap);
	
	// Create an RSgImage (member variable as it is 'shared' in the thread funtion...) 
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(KDefaultSourceFormat, KPixmapSize);
	ASSERT_EQUALS(iSgImageShared.Create(imageInfo,bitmap->DataAddress(),bitmap->DataStride()), KErrNone);
	CleanupStack::PopAndDestroy(bitmap);

	// launch 2 threads
	Test_MultiThreadL(2, ETrue);
	
	// clean everything
	iSgImageShared.Close();
	CleanupStack::PopAndDestroy(eglSess);
	TerminateDisplayL();
	INFO_PRINTF1(_L("Exit: CEglTest_EGL_Image_Multi_Thread_Sibling_Basic::doTestStepL"));
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

void CEglTest_EGL_Image_Multi_Thread_Sibling_Basic::doThreadFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Thread_Sibling_Basic::doThreadFunctionL, Thread %d"),aIdx);
	CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, aIdx);
	eglSess->InitializeL();
	eglSess->OpenSgDriverL();
			
	EGLImageKHR eglImageLocal = EGL_NO_IMAGE_KHR;
	if(aIdx == 0)
		{
		INFO_PRINTF2(_L("Thread %d, Creating an EGLImage from the shared RSgImage"),aIdx);
		eglImageLocal = eglSess->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&iSgImageShared,KEglImageAttribsPreservedTrue);
		ASSERT_EGL_TRUE(eglImageLocal != EGL_NO_IMAGE_KHR);
		}
	
	Rendezvous(aIdx);
	
	if(aIdx == 1)
		{
		INFO_PRINTF2(_L("Thread %d, Creating an EGLImage from the shared RSgImage"),aIdx);
		eglImageLocal = eglSess->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&iSgImageShared,KEglImageAttribsPreservedTrue);
		ASSERT_EGL_TRUE(eglImageLocal == EGL_NO_IMAGE_KHR);
		ASSERT_EGL_ERROR(EGL_BAD_ACCESS);
		}
	
	Rendezvous(aIdx);
	
	// cleanup
	if(aIdx == 0)
		{
		ASSERT_EGL_TRUE(eglSess->DestroyEGLImage(iDisplay, eglImageLocal));
		}
	eglSess->CloseSgDriver();
	CleanupStack::PopAndDestroy(eglSess);
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0157

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Check if EGL Implementation allows two threads to work in parallel.
Each thread is allowed to create a VGImage from the same EGLImage

@SYMTestActions
Main Thread: creates an RSgImage with the same content as the reference bitmap and creates an EGLImage from it; starts thread1 and thread2.
Thread1: Creates an egl context and a pixmap surface linked to it. Creates an VGImage from the EGLImage previous mentioned
Thread2: Creates an egl context and a pixmap surface linked to it. Creates an VGImage from the EGLImage previous mentioned
--------
Thread 1: Changes the content of the RSgImage by using the VGImage that has a reference to it.
--------
Thread1: Passes the VGImage into vgDestroyImage()
Thread2: Copies the VGImage to the pixmap surface and checks the contents
Thread2: Passes the VGImage into vgDestroyImage()
Main Thread: Closes the EGLImage
Main Thread: Closes the RSgImage

@SYMTestExpectedResults
No error is generated within both threads. The changes apported by the first thread affects the second thread. The content the pixmap surface will matches the one of the reference bitmap changed by the first thread.
No memory or handle leaks
*/
TVerdict CEglTest_EGL_Image_Multi_Thread_Sibling_VGImage::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0157"));
	INFO_PRINTF1(_L("Enter: CEglTest_EGL_Image_Multi_Thread_Sibling_VGImage::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	// Create display object
	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
    GetDisplayL();
    CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, 0);
	eglSess->InitializeL();

	// Make sure the driver is ready
	eglSess->OpenSgDriverL();

	// create a reference bitmap (we give index 3, as there's only 1 image in this test case)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
	CFbsBitmap* bitmap = eglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 3);
	CleanupStack::PushL(bitmap);
	
	// Create an RSgImage (member variable as it is 'shared' in the thread funtion...) 
	INFO_PRINTF1(_L("Parent Thread, Creating the shared RSgImage"));
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(KDefaultSourceFormat, KPixmapSize);
	ASSERT_EQUALS(iSgImageShared.Create(imageInfo,bitmap->DataAddress(),bitmap->DataStride()), KErrNone);
	CleanupStack::PopAndDestroy(bitmap);
	
	// Create an EGLImage from the RSgImage (member variable as it is 'shared' in the thread funtion...) 
	INFO_PRINTF1(_L("Parent Thread, Creating the shared EGLImage"));
	iEGLImageShared = eglSess->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&iSgImageShared,KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(iEGLImageShared != EGL_NO_IMAGE_KHR);
	iSgImageShared.Close();

	// launch 2 threads
	Test_MultiThreadL(2, ETrue);
	
	// cleanup
	ASSERT_EGL_TRUE(eglSess->DestroyEGLImage(iDisplay, iEGLImageShared));
	CleanupStack::PopAndDestroy(eglSess);
	TerminateDisplayL();	
	INFO_PRINTF1(_L("Exit: CEglTest_EGL_Image_Multi_Thread_Sibling_VGImage::doTestStepL"));	
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

void CEglTest_EGL_Image_Multi_Thread_Sibling_VGImage::doThreadFunctionL(TInt aIdx)
	{	
	INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Thread_Sibling_VGImage::doThreadFunctionL, Thread %d"),aIdx);
	CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, aIdx);
	eglSess->InitializeL();
	eglSess->OpenSgDriverL();

	INFO_PRINTF2(_L("Thread %d, Creating a Surface and a Context bound to OpenVG"),aIdx);
	TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(KDefaultSurfaceFormat);
	TSgImageInfoOpenVgTarget imageInfo = TSgImageInfoOpenVgTarget(pixelFormat, KPixmapSize);
	// Create a pixmap surface matching the native image pixel format
	eglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo,CTestEglSession::EResourceCloseSgImageEarly);
	
	// Create a VGImage from the EGLImage
	INFO_PRINTF2(_L("Thread %d, Creating one VGImage from the shared EGLImage"),aIdx);
	VGImage vgImageLocal = eglSess->vgCreateImageTargetKHR((VGeglImageKHR)iEGLImageShared);	
	ASSERT_VG_TRUE(vgImageLocal != VG_INVALID_HANDLE);
	
	Rendezvous(aIdx);
		
	if(aIdx == 0)
		{
		INFO_PRINTF2(_L("Thread %d, Updating contents of the VGImage from the shared EGLImage"),aIdx);
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
		CFbsBitmap* bitmap = eglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 8);
    	// Add pixel data to the VGImage reference from the bitmap reference. 
        // Mind the fact that CFbsBitmap and VGImages use different coordinates origin!
		TSize bitmapSize = bitmap->SizeInPixels();
    	TUint8* address = reinterpret_cast<TUint8*>(bitmap->DataAddress());
    	TInt stride = bitmap->DataStride();
    	address += (bitmapSize.iHeight - 1) * stride;
        vgImageSubData(vgImageLocal, address, -stride, KDefaultSurfaceFormat, 0,0, bitmapSize.iWidth, bitmapSize.iHeight);
		delete bitmap;
        bitmap = NULL;
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
		eglWaitClient();
		}

	Rendezvous(aIdx);
	
	if(aIdx == 1)
		{
		INFO_PRINTF2(_L("Thread %d, Drawing the VGImage to the current surface"),aIdx);
		// Copy the source VGImage to the surface
    	vgSetPixels(0, 0, vgImageLocal, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
		eglWaitClient();

		// we can now compare the VgImage to the one we expect after changing it in the other thread
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
		CFbsBitmap* refBitmap = eglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 8);
		CleanupStack::PushL(refBitmap);
		eglSess->CheckVgDrawingL(KDefaultSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(refBitmap);
		INFO_PRINTF2(_L("Thread %d, Drawing successful"),aIdx);
		}		
	
	// cleanup
	vgDestroyImage(vgImageLocal);
	ASSERT_TRUE(vgGetError() == VG_NO_ERROR);
	eglSess->CloseSgDriver();
	CleanupStack::PopAndDestroy(eglSess);
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0158

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Check if EGL Implementation allows two threads to work in parallel.
Each thread is allowed to create a VGImage from an EGLImage created by another thread.

@SYMTestActions
Main Thread: creates an RSgImage ; starts thread1 and thread2.
Thread1: Creates an EGLImage from the previous mentioned RSgImage
--------
Thread2: Creates an VGImage from the EGLImage created by the other thread
--------
Thread1: Passes the EGLImage into eglDestroyImageKHR ()
Thread2: Passes the VGImage into vgDestroyImage()
Main Thread: Closes the RSgImage

@SYMTestExpectedResults
No error is generated within both threads
No memory or handle leaks
*/
TVerdict CEglTest_EGL_Image_Multi_Thread_Sibling_VGImage_PassingEGLImage::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0158"));
	INFO_PRINTF1(_L("Enter: CEglTest_EGL_Image_Multi_Thread_Sibling_VGImage_PassingEGLImage::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	// Create display object
	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
    GetDisplayL();
    CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, 0);
	eglSess->InitializeL();

	// Make sure the driver is ready
	eglSess->OpenSgDriverL();

	// create a reference bitmap (we give index 5, as there's only 1 image in this test case)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
	CFbsBitmap* bitmap = eglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 5);
	CleanupStack::PushL(bitmap);
	
	// Create an RSgImage (member variable as it is 'shared' in the thread funtion...) 
	INFO_PRINTF1(_L("Parent Thread, Creating the shared RSgImage"));
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(KDefaultSourceFormat, KPixmapSize);
	ASSERT_EQUALS(iSgImageShared.Create(imageInfo,bitmap->DataAddress(),bitmap->DataStride()), KErrNone);
	CleanupStack::PopAndDestroy(bitmap);
	
	// launch 2 threads
	Test_MultiThreadL(2, ETrue);
	
	// cleanup
	iSgImageShared.Close();
	CleanupStack::PopAndDestroy(eglSess);
	TerminateDisplayL();	
	INFO_PRINTF1(_L("Exit: CEglTest_EGL_Image_Multi_Thread_Sibling_VGImage_PassingEGLImage::doTestStepL"));	
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

void CEglTest_EGL_Image_Multi_Thread_Sibling_VGImage_PassingEGLImage::doThreadFunctionL(TInt aIdx)
	{	
	INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Thread_Sibling_VGImage_PassingEGLImage::doThreadFunctionL, Thread %d"),aIdx);
	CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, aIdx);
	eglSess->InitializeL();
	eglSess->OpenSgDriverL();	

	// check there is no eglImage at the start of either thread
	ASSERT_EGL_TRUE(iEGLImageShared == EGL_NO_IMAGE_KHR);
	
	// Wait for both thread to reach this point
	//	This is important for multicore platforms (i.e., naviengine) as the execution order is not known and it could
	//	happen that the first thread has already created the image before the second checks there is no eglImage 
	Rendezvous(aIdx);

	if(aIdx == 0)
		{
		// Create an EGLImage from the RSgImage (member variable as it is 'shared' in the thread function...) 
		INFO_PRINTF2(_L("Thread %d, Creating the shared EGLImage"),aIdx);
		iEGLImageShared = eglSess->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&iSgImageShared,KEglImageAttribsPreservedTrue);
		ASSERT_EGL_TRUE(iEGLImageShared != EGL_NO_IMAGE_KHR);
		iSgImageShared.Close();
		}
	
	Rendezvous(aIdx);
	
	if(aIdx == 1)
		{
		INFO_PRINTF2(_L("Thread %d, Creating a Surface and a Context bound to OpenVG"),aIdx);
		TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(KDefaultSurfaceFormat);
		TSgImageInfoOpenVgTarget imageInfo = TSgImageInfoOpenVgTarget(pixelFormat, KPixmapSize);
		// Create a pixmap surface matching the native image pixel format
		eglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo,CTestEglSession::EResourceCloseSgImageEarly);

		// Create a VGImage from the EGLImage
		INFO_PRINTF2(_L("Thread %d, Creating one VGImage from the shared EGLImage"),aIdx);
		VGImage vgImageLocal = eglSess->vgCreateImageTargetKHR((VGeglImageKHR)iEGLImageShared);	
		ASSERT_VG_TRUE(vgImageLocal != VG_INVALID_HANDLE);

		INFO_PRINTF2(_L("Thread %d, Drawing the VGImage to the current surface"),aIdx);
		// Copy the source VGImage to the surface
    	vgSetPixels(0, 0, vgImageLocal, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
		eglWaitClient();

		// we can now compare the VgImage to the one we expect from the main thread
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
		CFbsBitmap* refBitmap = eglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 5);
		CleanupStack::PushL(refBitmap);
		eglSess->CheckVgDrawingL(KDefaultSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(refBitmap);
		INFO_PRINTF2(_L("Thread %d, Drawing successful"),aIdx);

		vgDestroyImage(vgImageLocal);
		ASSERT_TRUE(vgGetError() == VG_NO_ERROR);
		}
	
	Rendezvous(aIdx);
	
	if(aIdx == 0)
		{
		ASSERT_EGL_TRUE(eglSess->DestroyEGLImage(iDisplay, iEGLImageShared));
		iEGLImageShared = EGL_NO_IMAGE_KHR;
		}
	
	// cleanup
	eglSess->CloseSgDriver();
	CleanupStack::PopAndDestroy(eglSess);
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0159

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Check if EGL Implementation allows two threads to work in parallel.
If the thread that has created the EGLImage terminates without destroying it the other thread can still use it.

@SYMTestActions
Main Thread: Creates an RSgImage ; starts thread1 and thread2.
Thread1: An EGLImage from the previous mentioned RSgImage
--------
Thread1: Calls eglReleaseThread()
Thread1: Finishes gracefully
Thread2: Creates an VGImage the EGLImage created by the other thread
Thread2: Passes the VGImage into vgDestroyImage()
Thread2: Passes the EGLImage into eglDestroyImageKHR ()
Main Thread: Closes the RSgImage.

@SYMTestExpectedResults
No error is generated within both threads
No memory or handle leaks
*/
TVerdict CEglTest_EGL_Image_Multi_Thread_Exit_Thread::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0159"));
	INFO_PRINTF1(_L("Enter: CEglTest_EGL_Image_Multi_Thread_Exit_Thread::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	// Create display object
	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
    GetDisplayL();
    CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, 0);
	eglSess->InitializeL();

	// Make sure the driver is ready
	eglSess->OpenSgDriverL();

	// create a reference bitmap (we give index 2, as there's only 1 image in this test case)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
	CFbsBitmap* bitmap = eglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 2);
	CleanupStack::PushL(bitmap);
	
	// Create an RSgImage (member variable as it is 'shared' in the thread funtion...) 
	INFO_PRINTF1(_L("Parent Thread, Creating the shared RSgImage"));
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(KDefaultSourceFormat, KPixmapSize);
	ASSERT_EQUALS(iSgImageShared.Create(imageInfo,bitmap->DataAddress(),bitmap->DataStride()), KErrNone);
	CleanupStack::PopAndDestroy(bitmap);
	
	// launch 2 threads
	Test_MultiThreadL(2, ETrue);
	
	// cleanup
	iSgImageShared.Close();
	CleanupStack::PopAndDestroy(eglSess);	
	TerminateDisplayL();
	INFO_PRINTF1(_L("Exit: CEglTest_EGL_Image_Multi_Thread_Exit_Thread::doTestStepL"));	
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

void CEglTest_EGL_Image_Multi_Thread_Exit_Thread::doThreadFunctionL(TInt aIdx)
	{	
	INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Thread_Exit_Thread::doThreadFunctionL, Thread %d"),aIdx);
	CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, aIdx);
	eglSess->InitializeL();
	eglSess->OpenSgDriverL();

	// check there is no eglImage at the start of either thread
	ASSERT_EGL_TRUE(iEGLImageShared == EGL_NO_IMAGE_KHR);

	TRequestStatus statusThread0;
	if(aIdx == 1)
		{
		INFO_PRINTF2(_L("Thread %d, Asking to be notified when thread 0 exits"),aIdx);
		iThreadStatus[0].iThread.Logon(statusThread0);
		}
	
	Rendezvous(aIdx);
	
	//Thread 0 creates the EGLImage and exit
	if(aIdx == 0)	
		{
		// Create an EGLImage from the RSgImage (member variable as it is 'shared' in the thread function...) 
		INFO_PRINTF2(_L("Thread %d, Creating the shared EGLImage"),aIdx);
		iEGLImageShared = eglSess->eglCreateImageKhrL(iDisplay,EGL_NO_CONTEXT,EGL_NATIVE_PIXMAP_KHR,&iSgImageShared,KEglImageAttribsPreservedTrue);
		ASSERT_EGL_TRUE(iEGLImageShared != EGL_NO_IMAGE_KHR);
		iSgImageShared.Close();
		INFO_PRINTF2(_L("Thread %d -----  Exiting ---- EGLImage should be kept alive"),aIdx);
		}
	
	//Thread1 waits for Thread0 to exit and then uses the EGLImage and performs some drawing
	if(aIdx == 1)
		{
		INFO_PRINTF2(_L("Thread %d, Waiting for thread 0 to exit"),aIdx);
		User::WaitForRequest(statusThread0);

		INFO_PRINTF2(_L("Thread %d, Creating a Surface and a Context bound to OpenVG"),aIdx);
		TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(KDefaultSurfaceFormat);
		TSgImageInfoOpenVgTarget imageInfo = TSgImageInfoOpenVgTarget(pixelFormat, KPixmapSize);
		// Create a pixmap surface matching the native image pixel format
		eglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo,CTestEglSession::EResourceCloseSgImageEarly);

		// Create a VGImage from the EGLImage
		INFO_PRINTF2(_L("Thread %d, Creating one VGImage from the shared EGLImage"),aIdx);
		VGImage vgImageLocal = eglSess->vgCreateImageTargetKHR((VGeglImageKHR)iEGLImageShared);	
		ASSERT_VG_TRUE(vgImageLocal != VG_INVALID_HANDLE);

		INFO_PRINTF2(_L("Thread %d, Drawing the VGImage to the current surface"),aIdx);
		// Copy the source VGImage to the surface
    	vgSetPixels(0, 0, vgImageLocal, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
		eglWaitClient();

		// we can now compare the VgImage to the one we expect after changing it in the other thread
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
		CFbsBitmap* refBitmap = eglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 2);
		CleanupStack::PushL(refBitmap);
		eglSess->CheckVgDrawingL(KDefaultSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(refBitmap);
		INFO_PRINTF2(_L("Thread %d, Drawing successful"),aIdx);

		vgDestroyImage(vgImageLocal);
		ASSERT_TRUE(vgGetError() == VG_NO_ERROR);
		ASSERT_EGL_TRUE(eglSess->DestroyEGLImage(iDisplay, iEGLImageShared));
		iEGLImageShared = EGL_NO_IMAGE_KHR;
		}
	
	// cleanup
	eglSess->CloseSgDriver();
	CleanupStack::PopAndDestroy(eglSess);
	}


/**
@SYMTestCaseID GRAPHICS-EGL-0163

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
All the EGL resources may be used implicitly by a thread even after another thread has terminated the display.

@SYMTestActions
Thread0: Asks to be notified when Thread1 exits
Thread0: Creates a RSgImage having the reference bitmap as content
Thread0: Creates a EGLImage from the RSgImage
Thread0: Creates a surface
Thread0: Creates a VGImage from the EGLImage
--------
Thread1: Calls eglTerminate
--------
Thread1: Exits
Thread0: Resumes when Thread1 exits
Thread0: Uses the VGImage and draw it to the surface
Thread0: Checks the contents

@SYMTestExpectedResults
Thread0 is able to use successfully the VGmage and the surface's content are the ones expected.
*/
TVerdict CEglTest_EGL_Image_Multi_Thread_DrawAfterTerminate::doTestStepL()
	{
    SetTestStepID(_L("GRAPHICS-EGL-0163"));
	INFO_PRINTF1(_L("Enter: CEglTest_EGL_Image_Multi_Thread_DrawAfterTerminate::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	Test_MultiThreadL(2, ETrue);
			
	INFO_PRINTF1(_L("Exit: CEglTest_EGL_Image_Multi_Thread_DrawAfterTerminate::doTestStepL"));	
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

void CEglTest_EGL_Image_Multi_Thread_DrawAfterTerminate::doThreadFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Thread_DrawAfterTerminate::doThreadFunctionL, Thread %d"),aIdx);
	GetDisplayL();
	CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, aIdx);
	eglSess->InitializeL();
	eglSess->OpenSgDriverL();

	TRequestStatus statusThread1;
	VGImage vgImageLocal = VG_INVALID_HANDLE;
	EGLint surfaceWidth;
	EGLint surfaceHeigth;
	
	//Thread0 asks to be notified when Thread1 exits
	//Creates a RSgImage having the reference bitmap as content
	//Creates a EGLImage from the RSgImage
	//Creates a surface
	//Creates a VGImage from the EGLImage
	if(aIdx == 0)
		{
		// Thread0 asks to be notified when Thread1 exits
		iThreadStatus[1].iThread.Logon(statusThread1);

	    RSgImage rSgImageLocal;
		CleanupClosePushL(rSgImageLocal);

		// create a reference bitmap (we give index 9, as there's only 1 image in this test case)
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
		CFbsBitmap* bitmap = eglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 9);
		CleanupStack::PushL(bitmap);

		// Create an RSgImage
		INFO_PRINTF2(_L("Thread %d, Creating a RSgImage having the reference bitmap's content"),aIdx);
		TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(KDefaultSourceFormat, KPixmapSize);
		ASSERT_EQUALS(rSgImageLocal.Create(imageInfo,bitmap->DataAddress(),bitmap->DataStride()), KErrNone);
		CleanupStack::PopAndDestroy(bitmap);

		INFO_PRINTF2(_L("Thread %d, Creating an EGLImage from the shared RSgImage"),aIdx);
		EGLImageKHR eglImageLocal = eglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &rSgImageLocal, KEglImageAttribsPreservedTrue);
		ASSERT_EGL_TRUE(eglImageLocal != EGL_NO_IMAGE_KHR);
		CleanupStack::PopAndDestroy(&rSgImageLocal); 	//transferring ownership of the buffer to the EGLImage

		INFO_PRINTF2(_L("Thread %d, Creating a Surface and a Context bound to OpenVG"),aIdx);
		TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(KDefaultSurfaceFormat);
		TSgImageInfoOpenVgTarget imageInfo2 = TSgImageInfoOpenVgTarget(pixelFormat, KPixmapSize);
		// Create a pixmap surface matching the native image pixel format
		eglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo2,CTestEglSession::EResourceCloseSgImageEarly);

		//We now store the size of the surface because after eglTerminate is called on the display
		//it will not be possible to use explicitly all the resources linked to it
		//i.e. every call to an EGL API who takes a display as an argument will raise a EGL_BAD_DISPLAY error
		eglQuerySurface(iDisplay, eglSess->Surface(), EGL_WIDTH, &surfaceWidth);
		eglQuerySurface(iDisplay, eglSess->Surface(), EGL_HEIGHT, &surfaceHeigth);		
		
		INFO_PRINTF2(_L("Thread %d, Creating one VGImage from the EGLImage"),aIdx);
		vgImageLocal = eglSess->vgCreateImageTargetKHR((VGeglImageKHR)eglImageLocal);	
		ASSERT_VG_TRUE(vgImageLocal != VG_INVALID_HANDLE);
		ASSERT_EGL_TRUE(eglSess->DestroyEGLImage(iDisplay, eglImageLocal));
		}
	
	Rendezvous(aIdx);
	
	//Thread1 calls eglTerminate and exit
	if(aIdx == 1)
		{
		INFO_PRINTF2(_L(", Thread %d, Calling eglTerminate and exiting"),aIdx);
		TerminateDisplayL();
		}
	
	Rendezvous(aIdx);
	
	//Thread0 resumes when Thread1 exits
	//and it uses the resources that are linked to the just destroyed display
	if(aIdx == 0)
		{
		User::WaitForRequest(statusThread1);

		// It's still possible to use the current context, surface and VGImage even though the display has been terminated
		//Copy the source VGImage to the surface
    	vgSetPixels(0, 0, vgImageLocal, 0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
		eglWaitClient();

		// we can now compare the VgImage to the one we would expect for this particular thread
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
		CFbsBitmap* refBitmap = eglSess->CreateReferenceBitmapL(bitmapMode, KPixmapSize, 9);
		CleanupStack::PushL(refBitmap);
		eglSess->CheckVgDrawingL(KDefaultSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(refBitmap);
		INFO_PRINTF2(_L("Drawing successful, Thread %d"),aIdx);

		vgDestroyImage(vgImageLocal);
		ASSERT_TRUE(vgGetError() == VG_NO_ERROR);
		eglReleaseThread();
		}
	
	// cleanup
	eglSess->CloseSgDriver();
	CleanupStack::PopAndDestroy(eglSess);
	}
