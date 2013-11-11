// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32msgqueue.h>

#include "egltest_image_multiprocess.h"

#include <test/egltestcommonconversion.h>
#include <test/egltestcommoninisettings.h>
#include <test/egltestcommonsgimageinfo.h>
#include <test/egltestcommonprocess.h>

//
// Constant definitions
//
const TInt KNumProcesses = 8;
const TInt KNumImages = 4;
#define KImageSize TSize(100,100)	// use #define to avoid global temporary constructors


/**
@SYMTestCaseID GRAPHICS-EGL-0160

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Check if EGL Implementation allows two processes to work in parallel.
A process can create an EGLImage from the same RSgImage that already has been linked to an EGLImage by another process.

@SYMTestActions
Main Process: creates an RsgImage and starts Process and Process.
Process1: Creates an EGLImage from the RsgImage previous mentioned
--------
Process2: Creates an EGLImage from the RsgImage previous mentioned and check that eglCreateImageKHR() does NOT return EGL_NO_IMAGE_KHR
--------
Process1: Closes the EGLImage
Process2: Closes the EGLImage
Main Process: Closes the RsgImage

@SYMTestExpectedResults
No errors within both processes.
No memory or handle leaks
*/
TVerdict CEglTest_EGL_Image_Multi_Process_Sibling_Basic::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0160"));
	SetTestStepName(KEGL_Image_Multi_Process_Sibling_Basic);
	INFO_PRINTF1(_L("Enter: CEglTest_EGL_Image_Multi_Process_Sibling_Basic::doTestStepL"));

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
    CreateEglSessionL();
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	// create a reference bitmap (we give index 0, as there's only 1 image in this test case)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, 0);
	CleanupStack::PushL(bitmap);

    // Create an RSgImage
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(KDefaultSourceFormat, KImageSize);
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	imageInfo.iShareable = ETrue;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    RSgImage sgImage;
	CleanupClosePushL(sgImage);
	ASSERT_EQUALS(sgImage.Create(imageInfo,bitmap->DataAddress(),bitmap->DataStride()), KErrNone);

	// launch 2 processes
	Test_MultiProcessL(KEglTestStepDllName, 2, TestStepName(), sgImage.Id());

	// destroy sgImage
	CleanupStack::PopAndDestroy(2, bitmap);

	// clean everything
	CleanAll();
	INFO_PRINTF1(_L("Exit: CEglTest_EGL_Image_Multi_Process_Sibling_Basic::doTestStepL"));
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

void CEglTest_EGL_Image_Multi_Process_Sibling_Basic::doProcessFunctionL(TInt aIdx,const TSgDrawableId& aSgId)
	{
	INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Process_Sibling_Basic::doProcessFunctionL, Process %d"),aIdx);
	GetDisplayL();
	CreateEglSessionL(aIdx);
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	RSgImage sgImageFromId;
	CleanupClosePushL(sgImageFromId);
	ASSERT_EQUALS(sgImageFromId.Open(aSgId),KErrNone);

	INFO_PRINTF2(_L("Process %d, Creating an EGLImage from the shared RSgImage"),aIdx);
	EGLImageKHR eglImage = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImageFromId, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(eglImage != EGL_NO_IMAGE_KHR);
	CleanupStack::PopAndDestroy(&sgImageFromId);

	INFO_PRINTF2(_L("Process %d, EGLImage successfully created, now destroying it"),aIdx);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, eglImage));

	// cleanup
	CleanAll();
	}


/**
@SYMTestCaseID GRAPHICS-EGL-0161

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Check if EGL Implementation allows two processes to work in parallel.

@SYMTestActions
Run two processes that independently perform the same actions detailed below.
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
Pixmap surface has the expected contents in both processes (within tolerance)
No memory or handle leaks
*/
TVerdict CEglTest_EGL_Image_Multi_Process_Parallel::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0161"));
	SetTestStepName(KEGL_Image_Multi_Process_Parallel);
	INFO_PRINTF1(_L("Enter: CEglTest_EGL_Image_Multi_Process_Parallel::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	// launch 2 processes
	Test_MultiProcessL(KEglTestStepDllName, 2, TestStepName());

	INFO_PRINTF1(_L("Exit: CEglTest_EGL_Image_Multi_Process_Parallel::doTestStepL"));
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

void CEglTest_EGL_Image_Multi_Process_Parallel::doProcessFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Process_Parallel::doProcessFunctionL, Process %d"),aIdx);
	GetDisplayL();
	CreateEglSessionL(aIdx);
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	// create a reference bitmap (we give index 3 for example, as there's only 1 image in this test case)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, 3);
	CleanupStack::PushL(bitmap);

	// Create an RSgImage
	INFO_PRINTF2(_L("Process %d, Creating a RSgImage having the reference bitmap's content"),aIdx);
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(KDefaultSourceFormat, KImageSize);
    RSgImage rSgImageLocal;
	CleanupClosePushL(rSgImageLocal);
	ASSERT_EQUALS(rSgImageLocal.Create(imageInfo,bitmap->DataAddress(),bitmap->DataStride()), KErrNone);

	INFO_PRINTF2(_L("Process %d, Creating an EGLImage from the shared RSgImage"),aIdx);
	EGLImageKHR eglImageLocal = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &rSgImageLocal, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(eglImageLocal != EGL_NO_IMAGE_KHR);
	CleanupStack::PopAndDestroy(&rSgImageLocal); 	//transferring ownership of the buffer to the EGLImage
	CleanupStack::PopAndDestroy(bitmap);

	INFO_PRINTF2(_L("Process %d, Creating a Surface and a Context bound to OpenVG"),aIdx);
	TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(KDefaultSurfaceFormat);
	TSgImageInfoOpenVgTarget imageInfo2 = TSgImageInfoOpenVgTarget(pixelFormat, KImageSize);
	// Create a pixmap surface matching the native image pixel format
	iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo2,CTestEglSession::EResourceCloseSgImageEarly);

	INFO_PRINTF2(_L("Process %d, Creating one VGImage from the EGLImage"),aIdx);
	VGImage vgImageLocal = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)eglImageLocal);
	ASSERT_VG_TRUE(vgImageLocal != VG_INVALID_HANDLE);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, eglImageLocal));

	// Copy the source VGImage to the surface
	vgSetPixels(0, 0, vgImageLocal, 0, 0, KImageSize.iWidth, KImageSize.iHeight);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
	eglWaitClient();

	// destroy VGImage
	vgDestroyImage(vgImageLocal);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);

	// we can now compare the VgImage to the one we would expect for this particular process
	CFbsBitmap* refBitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, 3);
	CleanupStack::PushL(refBitmap);
	iEglSess->CheckVgDrawingL(KDefaultSurfaceFormat, refBitmap);
	CleanupStack::PopAndDestroy(refBitmap);
	INFO_PRINTF2(_L("Drawing successful, Process %d"),aIdx);

	// cleanup
	CleanAll();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0162

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Check if EGL Implementation allows two processes to work in parallel.
A process can create an EGLImage from the same RSgImage that already has been linked to an EGLImage by another process.
Both process can create a VGImage from it.
Another process uses the VGImage as a target
One process uses the VGImage as a source being able to see the drawing done by the other process

@SYMTestActions
Main Process: creates an RsgImage and starts Process and Process.
Process1: Creates an egl context and a pizmap surface linked to it. Creates an EGLImage from the RsgImage previous mentioned. Creates a VGImage from the EGLImage.
Process2: Creates an egl context and a pizmap surface linked to it. Creates an EGLImage from the RsgImage previous mentioned. Creates a VGImage from the EGLImage.
Process1: Changes the contents of the VGImage
--------
Process1: Closes the VGImage and the EGLImage
Process2: Draws the VGImage to the surface and checks if the contets match the reference bitmap plus the chages made by the first process.
Process2: Closes the VGImage and the EGLImage
Main Process: Closes the RsgImage

@SYMTestExpectedResults
No errors within both processes.
The content of the pixmap surface will match the one of the reference bitmap changed by the first process.
No memory or handle leaks.
*/
TVerdict CEglTest_EGL_Image_Multi_Process_Sibling_CheckContents::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0162"));
	SetTestStepName(KEGL_Image_Multi_Process_Sibling_CheckContents);
	INFO_PRINTF1(_L("Enter: CEglTest_EGL_Image_Multi_Process_Sibling_CheckContents::doTestStepL"));

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
    CreateEglSessionL();
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	// create a reference bitmap (we give index 0, as there's only 1 image in this test case)
	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, 0);
	CleanupStack::PushL(bitmap);

    // Create an RSgImage
	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(KDefaultSourceFormat, KImageSize);
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	imageInfo.iShareable = ETrue;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    RSgImage sgImage;
	CleanupClosePushL(sgImage);
	ASSERT_EQUALS(sgImage.Create(imageInfo,bitmap->DataAddress(),bitmap->DataStride()), KErrNone);

	// launch 2 processes
	Test_MultiProcessL(KEglTestStepDllName, 2, TestStepName(), sgImage.Id());

	// destroy sgImage
	CleanupStack::PopAndDestroy(&sgImage);
	CleanupStack::PopAndDestroy(bitmap);

	// clean everything
	CleanAll();
	INFO_PRINTF1(_L("Exit: CEglTest_EGL_Image_Multi_Process_Sibling_CheckContents::doTestStepL"));
    RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

void CEglTest_EGL_Image_Multi_Process_Sibling_CheckContents::doProcessFunctionL(TInt aIdx,const TSgDrawableId& aSgId)
	{
	INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Process_Sibling_CheckContents::doProcessFunctionL, Process %d"),aIdx);
	GetDisplayL();
	CreateEglSessionL(aIdx);
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	RSgImage sgImageFromId;
	CleanupClosePushL(sgImageFromId);
	ASSERT_EQUALS(sgImageFromId.Open(aSgId),KErrNone);

	INFO_PRINTF2(_L("Process %d, Creating an EGLImage from the shared RSgImage"),aIdx);
	EGLImageKHR eglImageLocal = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImageFromId, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(eglImageLocal != EGL_NO_IMAGE_KHR);
	CleanupStack::PopAndDestroy(&sgImageFromId);

	INFO_PRINTF2(_L("Process %d, Creating a Surface and a Context bound to OpenVG"),aIdx);
	TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(KDefaultSurfaceFormat);
	TSgImageInfoOpenVgTarget imageInfo = TSgImageInfoOpenVgTarget(pixelFormat, KImageSize);
	// Create a pixmap surface matching the native image pixel format
	iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo,CTestEglSession::EResourceCloseSgImageEarly);

	INFO_PRINTF2(_L("Process %d, Creating one VGImage from the EGLImage"),aIdx);
	VGImage vgImageLocal = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)eglImageLocal);
	ASSERT_VG_TRUE(vgImageLocal != VG_INVALID_HANDLE);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, eglImageLocal));

	if(aIdx == 0)
		{
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
		CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, 4);
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

	// Wait for both processes to reach this point (process 0 will have updated the VGImage)
	Rendezvous(aIdx);

	if(aIdx == 1)
		{
		INFO_PRINTF2(_L("Process %d, Drawing the VGImage to the current surface"),aIdx);
		// Copy the source VGImage to the surface
    	vgSetPixels(0, 0, vgImageLocal, 0, 0, KImageSize.iWidth, KImageSize.iHeight);
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
		eglWaitClient();

		// we can now compare the VgImage to the one we expect after changing it in the other process
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
		CFbsBitmap* refBitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, 4);
		CleanupStack::PushL(refBitmap);
		iEglSess->CheckVgDrawingL(KDefaultSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(refBitmap);
		INFO_PRINTF2(_L("Process %d, Drawing successful"),aIdx);
		}

	// destroy VGImage
	vgDestroyImage(vgImageLocal);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);

	// cleanup
	CleanAll();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0350

@SYMTestPriority 1

@SYMPREQ 2637

@SYMTestCaseDesc
Check sharing SgImage between 2 processes where a writer process populates SgImage through VgImage and a reader process draws SgImage to window surface.

@SYMTestActions
Main Process: starts Process1 and Process2.
Process1: Creates an RsgImage and passes RsgImage ID to process2. Creates an egl context and a pixmap surface linked to it. Creates an EGLImage from the RsgImage previous mentioned. Creates a VGImage from the EGLImage.
Process2: Creates an RsgImage using RsgImage ID passed into it. Creates an egl context and a pixmap surface linked to it. Creates an EGLImage from the RsgImage previous mentioned. Creates a VGImage from the EGLImage.
--------
Process2: Draws the VGImage to the surface before Process1 changes the contents of the VGImage.
--------
Process1: Changes the contents of the VGImage
--------
Process2: Draws the VGImage to the surface after Process1 changes the contents of the VGImage and checks if the contets match the reference bitmap plus the chages made by the process1.
Process1: Closes the VGImage, the EGLImage, the RsgImage.
Process2: Closes the VGImage, the EGLImage, the RsgImage.

@SYMTestExpectedResults
No errors within both processes.
The content of the pixmap surface will match the one of the reference bitmap changed by the first process.
No memory or handle leaks.
*/
TVerdict CEglTest_EGL_Image_Multi_Process_VgImage_Source::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0350"));
	SetTestStepName(KEGL_Image_Multi_Process_VgImage_Source);
	INFO_PRINTF1(_L("Enter: CEglTest_EGL_Image_Multi_Process_VgImage_Source::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	// launch 2 processes
	Test_MultiProcessL(KEglTestStepDllName, 2, TestStepName());

    INFO_PRINTF1(_L("Exit: CEglTest_EGL_Image_Multi_Process_VgImage_Source::doTestStepL"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

void CEglTest_EGL_Image_Multi_Process_VgImage_Source::doProcessFunctionL(TInt aIdx)
    {
    INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Process_VgImage_Source::doProcessFunctionL, Process %d"),aIdx);
	GetDisplayL();
	CreateEglSessionL(aIdx);
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

    RMsgQueue<TSgDrawableId> messageQueue;
    User::LeaveIfError(messageQueue.Open(EProcSlotMsgQueueSgId, EOwnerProcess));
    CleanupClosePushL(messageQueue);

    RSgImage rSgImageLocal;
	if(aIdx == 0)
        {
    	// create a reference bitmap (we give index 0, as there's only 1 image in this test case)
    	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
    	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, 0);
    	CleanupStack::PushL(bitmap);

    	// Create an RSgImage
    	INFO_PRINTF2(_L("Process %d, Creating a RSgImage having the reference bitmap's content"),aIdx);
    	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(KDefaultSourceFormat, KImageSize);
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    	imageInfo.iShareable = ETrue;
#endif
    	ASSERT_EQUALS(rSgImageLocal.Create(imageInfo,bitmap->DataAddress(),bitmap->DataStride()), KErrNone);
        CleanupStack::PopAndDestroy(bitmap);

        INFO_PRINTF2(_L("Process %d, Sending SgImage ID to other process..."), aIdx);
        messageQueue.SendBlocking(rSgImageLocal.Id());
        }
    else if(aIdx == 1)
        {
        INFO_PRINTF2(_L("Process %d, Receiving SgImage ID from other process..."), aIdx);
        TSgDrawableId sgImageId;
        messageQueue.ReceiveBlocking(sgImageId);
        ASSERT_EQUALS(rSgImageLocal.Open(sgImageId), KErrNone);
        }

	// Wait for both processes to reach this point
    Rendezvous(aIdx);
    
    INFO_PRINTF2(_L("Process %d, Creating an EGLImage from the shared RSgImage"),aIdx);
	CleanupClosePushL(rSgImageLocal);
	EGLImageKHR eglImageLocal = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &rSgImageLocal, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(eglImageLocal != EGL_NO_IMAGE_KHR);
	CleanupStack::PopAndDestroy(&rSgImageLocal); 	//transferring ownership of the buffer to the EGLImage

	INFO_PRINTF2(_L("Creating a Surface and a Context bound to OpenVG, Process %d"),aIdx);
	TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(KDefaultSurfaceFormat);
	TSgImageInfoOpenVgTarget imageInfo = TSgImageInfoOpenVgTarget(pixelFormat, KImageSize);
	// Create a pixmap surface matching the native image pixel format
	iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo,CTestEglSession::EResourceCloseSgImageEarly);

	INFO_PRINTF2(_L("Process %d, Creating one VGImage from the EGLImage"),aIdx);
	VGImage vgImageLocal = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)eglImageLocal);
	ASSERT_VG_TRUE(vgImageLocal != VG_INVALID_HANDLE);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, eglImageLocal));

    if(aIdx == 1)
        {
        INFO_PRINTF2(_L("Process %d, Drawing the VGImage to the current surface before changing contents of the VGImage"),aIdx);
    	// Copy the source VGImage to the surface
    	vgSetPixels(0, 0, vgImageLocal, 0, 0, KImageSize.iWidth, KImageSize.iHeight);
    	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
    	eglWaitClient();

    	// we can now compare the VgImage to the one we expect before we apply any change to it
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
		CFbsBitmap* refBitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, 0);
		CleanupStack::PushL(refBitmap);
		iEglSess->CheckVgDrawingL(KDefaultSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(refBitmap);
		INFO_PRINTF2(_L("Process %d, Drawing successful"),aIdx);
        }

	// Wait for both processes to reach this point
    Rendezvous(aIdx);

    if(aIdx == 0)
        {
        INFO_PRINTF2(_L("Process %d, Changing contents of the VGImage"),aIdx);
        TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
        CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, 4);
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

	// Wait for both processes to reach this point (process 0 will have updated the VGImage)
	Rendezvous(aIdx);

	if(aIdx == 1)
		{
        INFO_PRINTF2(_L("Drawing the VGImage to the current surface after changing contents of the VGImage, Process %d"),aIdx);
        // Copy the source VGImage to the surface
    	vgSetPixels(0, 0, vgImageLocal, 0, 0, KImageSize.iWidth, KImageSize.iHeight);
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
		eglWaitClient();

		// we can now compare the VgImage to the one we expect after changing it in the other process
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
		CFbsBitmap* refBitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, 4);
		CleanupStack::PushL(refBitmap);
		iEglSess->CheckVgDrawingL(KDefaultSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(refBitmap);
		INFO_PRINTF2(_L("Process %d, Drawing successful"),aIdx);
        }

	vgDestroyImage(vgImageLocal);
	ASSERT_TRUE(vgGetError() == VG_NO_ERROR);

	CleanupStack::PopAndDestroy(&messageQueue);
    CleanAll();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0351

@SYMTestPriority 1

@SYMPREQ 2637

@SYMTestCaseDesc
Check when sharing SgImage between 2 processes where a writer process populates SgImage through VgImage and a reader process draws SgImage to window surface,
process2 has reference to SgImage even after process1 loses all references to SgImage

@SYMTestActions
Main Process: starts Process1 and Process2.
Process1: Creates an RsgImage and passes RsgImage ID to process2. Creates an egl context and a pixmap surface linked to it. Creates an EGLImage from the RsgImage previous mentioned. Creates a VGImage from the EGLImage.
Process2: Creates an RsgImage using RsgImage ID passed into it. Creates an egl context and a pixmap surface linked to it. Creates an EGLImage from the RsgImage previous mentioned. Creates a VGImage from the EGLImage.
Process2: Drawss the VGImage to the surface before Process1 changes the contents of the VGImage.
--------
Process1: Changes the contents of the VGImage
Process1: Closes the VGImage, the EGLImage, the RsgImage.
--------
Process2: Draws the VGImage to the surface after Process1 changes the contents of the VGImage and checks if the contents match the reference bitmap plus the changes made by the process1.
Process2: Closes the VGImage, the EGLImage, the RsgImage.

@SYMTestExpectedResults
No errors within both processes.
The content of the pixmap surface will match the one of the reference bitmap changed by the process1.
No memory or handle leaks.
*/
TVerdict CEglTest_EGL_Image_Multi_Process_VgImage_DrawAfterTerminate::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0351"));
	SetTestStepName(KEGL_Image_Multi_Process_VgImage_DrawAfterTerminate);
    INFO_PRINTF1(_L("Enter: CEglTest_EGL_Image_Multi_Process_VgImage_DrawAfterTerminate::doTestStepL"));

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

	// launch 2 processes
 	Test_MultiProcessL(KEglTestStepDllName, 2, TestStepName());

	INFO_PRINTF1(_L("Exit: CEglTest_EGL_Image_Multi_Process_VgImage_DrawAfterTerminate::doTestStepL"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

void CEglTest_EGL_Image_Multi_Process_VgImage_DrawAfterTerminate::doProcessFunctionL(TInt aIdx)
    {
    INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Process_VgImage_DrawAfterTerminate::doProcessFunctionL, Process %d"),aIdx);
    GetDisplayL();
    CreateEglSessionL(aIdx);
    iEglSess->InitializeL();
    iEglSess->OpenSgDriverL();

    RMsgQueue<TSgDrawableId> messageQueue;
    User::LeaveIfError(messageQueue.Open(EProcSlotMsgQueueSgId, EOwnerProcess));
    CleanupClosePushL(messageQueue);

    RSgImage rSgImageLocal;
    if(aIdx == 0)
        {
    	// create a reference bitmap (we give index 0, as there's only 1 image in this test case)
    	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
    	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, 0);
    	CleanupStack::PushL(bitmap);

    	// Create an RSgImage
    	INFO_PRINTF2(_L("Process %d, Creating a RSgImage having the reference bitmap's content"),aIdx);
    	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(KDefaultSourceFormat, KImageSize);
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    	imageInfo.iShareable = ETrue;
#endif
    	ASSERT_EQUALS(rSgImageLocal.Create(imageInfo,bitmap->DataAddress(),bitmap->DataStride()), KErrNone);
        CleanupStack::PopAndDestroy(bitmap);

        INFO_PRINTF2(_L("Process %d, Sending SgImage ID to other process..."), aIdx);
        messageQueue.SendBlocking(rSgImageLocal.Id());
        }
    else if(aIdx == 1)
        {
        INFO_PRINTF2(_L("Process %d: Receiving SgImage ID from other process..."), aIdx);
        TSgDrawableId sgImageId;
        messageQueue.ReceiveBlocking(sgImageId);
        ASSERT_EQUALS(rSgImageLocal.Open(sgImageId),KErrNone);
        }

	// Wait for both processes to reach this point
    Rendezvous(aIdx);

	INFO_PRINTF2(_L("Process %d, Creating an EGLImage from the shared RSgImage"),aIdx);
	CleanupClosePushL(rSgImageLocal);
	EGLImageKHR eglImageLocal = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &rSgImageLocal, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(eglImageLocal != EGL_NO_IMAGE_KHR);
	CleanupStack::PopAndDestroy(&rSgImageLocal); 	//transferring ownership of the buffer to the EGLImage

	INFO_PRINTF2(_L("Creating a Surface and a Context bound to OpenVG, Process %d"),aIdx);
	TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(KDefaultSurfaceFormat);
	TSgImageInfoOpenVgTarget imageInfo = TSgImageInfoOpenVgTarget(pixelFormat, KImageSize);
	// Create a pixmap surface matching the native image pixel format
	iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo,CTestEglSession::EResourceCloseSgImageEarly);

	INFO_PRINTF2(_L("Process %d, Creating one VGImage from the EGLImage"),aIdx);
	VGImage vgImageLocal = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)eglImageLocal);
	ASSERT_VG_TRUE(vgImageLocal != VG_INVALID_HANDLE);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, eglImageLocal));

    if(aIdx == 1)
        {
        INFO_PRINTF2(_L("Process %d, Drawing the VGImage to the current surface before changing contents of the VGImage"),aIdx);
        // Copy the source VGImage to the surface
    	vgSetPixels(0, 0, vgImageLocal, 0, 0, KImageSize.iWidth, KImageSize.iHeight);
    	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
    	eglWaitClient();


    	// we can now compare the VgImage to the one we expect before we apply any change to it
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
		CFbsBitmap* refBitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, 0);
		CleanupStack::PushL(refBitmap);
		iEglSess->CheckVgDrawingL(KDefaultSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(refBitmap);
		INFO_PRINTF2(_L("Process %d, Drawing successful"),aIdx);
        }

	// Wait for both processes to reach this point
    Rendezvous(aIdx);

    if(aIdx == 0)
        {
        INFO_PRINTF2(_L("Process %d, Changing contents of the VGImage"),aIdx);
        TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
        CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, 2);
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

    	vgDestroyImage(vgImageLocal);
    	ASSERT_TRUE(vgGetError() == VG_NO_ERROR);
    	CleanupStack::PopAndDestroy(&messageQueue);
        CleanAll();
        }

	// Wait for both processes to reach this point
    Rendezvous(aIdx);

    if(aIdx == 1)
        {
        INFO_PRINTF2(_L("Drawing the VGImage to the current surface after changing contents of the VGImage, Process %d"),aIdx);
 		// Copy the source VGImage to the surface
    	vgSetPixels(0, 0, vgImageLocal, 0, 0, KImageSize.iWidth, KImageSize.iHeight);
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
		eglWaitClient();

		// we can now compare the VgImage to the one we expect after changing it in the other process
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(KDefaultSourceFormat);
		CFbsBitmap* refBitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, 2);
		CleanupStack::PushL(refBitmap);
		iEglSess->CheckVgDrawingL(KDefaultSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(refBitmap);
		INFO_PRINTF2(_L("Process %d, Drawing successful"),aIdx);

		vgDestroyImage(vgImageLocal);
		ASSERT_TRUE(vgGetError() == VG_NO_ERROR);

		CleanupStack::PopAndDestroy(&messageQueue);
    	CleanAll();
        }
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0406

@SYMTestPriority 1

@SYMPREQ PREQ2637

@SYMTestCaseDesc
To ensure that RSgImage with uploaded data can be shared across processes.
To ensure that reference counting of RSgImage works correctly.

@SYMTestActions
From the main process:
- Create M SgImage(s) with the flag ESgUsageBitOpenVgImage. The size are all the same.
  We are running through all the possible configurations, with the values assumed being:
•	EUidPixelFormatRGB_565
•	EUidPixelFormatXRGB_8888
•	EUidPixelFormatARGB_8888 (source only)
•	EUidPixelFormatARGB_8888_PRE
•	EUidPixelFormatA_8  (source only)
- Note that when using EUidPixelFormatA_8 as a source, the reference bitmap display mode used is EGray256,
  This is to enable using the reference bitmap as an alpha mask.
- Spawn N client processes. During the process launching, pass to each process single drawable ID from the SgImages.
  In order to define which SgImage needs to be passed to the particular process, there will be following
  formula applied: J = P Mod (M), where J is the sequence number of the image, P is the particular process number.
From processes 1 to N:
- Open SgImage by using TSgDrawableId, obtained from the SgImage which was passed from the process A.
- Using EGL extension, create EGLImage specifying as EGLClientBuffer SgImage which was created on
  previous step,  EGL_NATIVE_PIXMAP_KHR as a target and EGL_IMAGE_PRESERVED_KHR as an attribute
- Using VG extension, create VG image based on EGLImage from the previous step.
- Close Sg and EGL images
- Create second SgImage with the same size and pixel format as first SgImage and usage flag is set to ESgUsageBitOpenVgSurface.
- Create off-screen pixmap surface with underlining second SgImage and make it current for the drawing context.
- Draw VGImage to the off-screen surface (note that when using EUidPixelFormatA_8 as a source, the
  reference bitmap is used as an alpha mask).
- Draw VGImage to the off-screen surface.
- Retrieve surface data (see vgReadPixels() API)

@SYMTestExpectedResults
Creation of all drawable  resources have been completed without errors.
Image data obtained in client processes 1-N matches to the data which have been uploaded to the SgImages buffer from
process A. Reference counting works correctly and keeps VG image alive although bound Sg and EGL images have been destroyed.
When all resources are closed, resource count maintained by RSgDriver extension is zero in all processes.
*/
TVerdict CEglTest_EGL_Image_Multi_Process_FontServer_Upfront::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0406"));
	SetTestStepName(KEGL_Image_Multi_Process_FontServer_Upfront);
	INFO_PRINTF1(_L("CEglTest_EGL_Image_Multi_Process_FontServer_Upfront::doTestStepL"));

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

TVerdict CEglTest_EGL_Image_Multi_Process_FontServer_Upfront::doTestPartialStepL()
	{
	INFO_PRINTF1(_L("CEglTest_EGL_Image_Multi_Process_FontServer_Upfront::doTestPartialStepL"));
	PrintUsedPixelConfiguration();

	// Create display object
	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	// list to maintain TSgDrawableId
	RArray<TSgDrawableId> sgIdList;
	CleanupClosePushL(sgIdList);
    RSgImage sgImages[KNumImages];

	INFO_PRINTF2(_L("MAIN PROCESS: Creating %d RSgImage(s)..."), KNumImages);
	for (TInt i=0; i<KNumImages; i++)
		{
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
		CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, i);
		CleanupStack::PushL(bitmap);

		TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(iSourceFormat, KImageSize);
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    	imageInfo.iShareable = ETrue;
#endif
		ASSERT_EQUALS(sgImages[i].Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride()), KErrNone);
        CleanupClosePushL(sgImages[i]);
		ASSERT_EQUALS(sgIdList.Insert(sgImages[i].Id(),i), KErrNone);
		}

	INFO_PRINTF2(_L("MAIN PROCESS: About to launch %d processes..."), KNumProcesses);
	Test_MultiProcessL(KEglTestStepDllName, KNumProcesses, TestStepName(), sgIdList); //the function will guarantee that all images will be opened before it returns
    CleanupStack::PopAndDestroy(2 * KNumImages + 1, &sgIdList); // KNumImages SgImages, KNumImages bitmaps, sgIdList
	INFO_PRINTF2(_L("MAIN PROCESS: All %d launched processes have completed!"), KNumProcesses);

	CleanAll();
	INFO_PRINTF1(_L("End of CEglTest_EGL_Image_Multi_Process_FontServer_Upfront::doTestPartialStepL"));
	return TestStepResult();
	}

void CEglTest_EGL_Image_Multi_Process_FontServer_Upfront::doProcessFunctionL(TInt aIdx,const TSgDrawableId& aSgId)
	{
	INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Process_FontServer_Upfront::doProcessFunctionL, Process %d"),aIdx);
	GetDisplayL();
	CreateEglSessionL(aIdx);
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	//Retrieve source formats for the launched process from the process parameters.
	User::LeaveIfError(User::GetTIntParameter(EProcSlotSourceFormat, reinterpret_cast<TInt&>(iSourceFormat)));
	User::LeaveIfError(User::GetTIntParameter(EProcSlotSurfaceFormat, reinterpret_cast<TInt&>(iSurfaceFormat)));

	RSgImage sgImageFromId;
	CleanupClosePushL(sgImageFromId);
	ASSERT_EQUALS(sgImageFromId.Open(aSgId), KErrNone);

	INFO_PRINTF2(_L("Process %d, Creating an EGLImage from the shared RSgImage"),aIdx);
	EGLImageKHR eglImage = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImageFromId, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(eglImage != EGL_NO_IMAGE_KHR);
	CleanupStack::PopAndDestroy(&sgImageFromId);

	INFO_PRINTF2(_L("Process %d, Creating a Surface and a Context bound to OpenVG"),aIdx);
    TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
    TSgImageInfoOpenVgTarget imageInfo = TSgImageInfoOpenVgTarget(pixelFormat, KImageSize);
	// Create a pixmap surface matching the native image pixel format
    iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo,CTestEglSession::EResourceCloseSgImageEarly);

    INFO_PRINTF2(_L("Process %d, Creating one VGImage from the EGLImage"),aIdx);
    VGImage vgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)eglImage);
	ASSERT_VG_TRUE(vgImage != VG_INVALID_HANDLE);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, eglImage));

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
		vgClear(0, 0, KImageSize.iWidth, KImageSize.iHeight);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);

		// fill paint
		VGPaint fillPaint = vgCreatePaint();
		vgSetPaint(fillPaint, VG_FILL_PATH);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		vgSetParameteri(fillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
		VGuint fillColor = 0x008000ff; // opaque dark green
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
		vgSetPixels(0, 0, vgImage, 0, 0, KImageSize.iWidth, KImageSize.iHeight);
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
		eglWaitClient();
		}

	// Check that the surface contains the expected pixels
	//  # if the source is a A_8, to compare the surface with a reference bitmap, the following is needed:
	//    a) a reference bitmap needs to be cleared to black (same colour as the surface was cleared to)
	//    b) a Pen bitmap, that we clear to dark green (same colour as the fillPaint used to draw to the surface)
	//    c) a mask bitmap, which is the reference bitmap used to create the SgImage
	//  # otherwise, the surface must contain the same pixels as the bitmap used to create the SgImage
	if (iSourceFormat == EUidPixelFormatA_8)
		{
		TDisplayMode maskMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
		CFbsBitmap* mask = iEglSess->CreateReferenceBitmapL(maskMode, KImageSize, ImageIndexFromProcessId(aIdx, KNumImages));
		CleanupStack::PushL(mask);

		// we need a reference bitmap with the same pixel format as the target surface
		TUidPixelFormat format = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
		TDisplayMode refbitmapMode = EglTestConversion::PixelFormatToDisplayMode(format);

		CFbsBitmap* refBitmap = iEglSess->CreateReferenceMaskedBitmapL(refbitmapMode, KRgbDarkGreen, mask);
		CleanupStack::PushL(refBitmap);

		// compare the obtained reference bitmap with the surface drawn
		iEglSess->CheckVgDrawingL(iSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(2, mask); //mask, refBitmap
		}
	else
		{
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
		CFbsBitmap* refBitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, ImageIndexFromProcessId(aIdx, KNumImages));
		CleanupStack::PushL(refBitmap);
		iEglSess->CheckVgDrawingL(iSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(refBitmap);
		}
	INFO_PRINTF2(_L("Process %d, VG drawing successfully completed and checked"),aIdx);

	// destroy VGImage
	vgDestroyImage(vgImage);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);

	// cleanup
	CleanAll();
	}


/**
@SYMTestCaseID GRAPHICS-EGL-0410

@SYMTestPriority 1

@SYMPREQ PREQ2637

@SYMTestCaseDesc
To ensure that RSgImage with uploaded data can be shared across processes.
To ensure that reference counting of RSgImage works correctly

@SYMTestActions
From the main process:
- Create M SgImage(s) with the flag ESgUsageBitOpenVgImage. The size are all the same.
  We are running through all the possible target configurations, with the values assumed being:
•	EUidPixelFormatRGB_565
•	EUidPixelFormatXRGB_8888
•	EUidPixelFormatARGB_8888_PRE
- Using EGL extension, create M EGLImage(s), specifying as EGLClientBuffer SgImage(s) which were created on
  first step and EGL_NATIVE_PIXMAP_KHR as a target
- Using VG extension, create VG images based on EGLImage(s) from the previous step
- Close Sg and EGL Images
- Populate data in VGImages (see vgImageSubData(..) API), there will be different data uploaded for each VGImage
- Spawn N client processes. During the process launching, pass to each process single drawable ID from the SgImages.
  In order to define which SgImage(s) needs to be passed to the particular processes, there will be following
  formula applied: J = P Mod (M), where J is the sequence number of the image, P is the particular process number.
From processes 1 to N:
- Open SgImage by using TSgDrawableId, obtained from the SgImage which was passed from the process A.
- Using EGL extension, create EGLImage specifying as EGLClientBuffer SgImage which was created on previous step,
  EGL_NATIVE_PIXMAP_KHR as a target and EGL_IMAGE_PRESERVED_KHR as an attribute
- Using VG extension, create VG image based on EGLImage from the previous step.
- Close Sg and EGL images
- Create second SgImage with the same size and pixel format as first SgImage and usage flag is set to ESgUsageBitOpenVgSurface.
- Create off-screen pixmap surface with underlining second SgImage and make it current for the drawing context.
- Draw VGImage to the off-screen surface.
- Retrieve surface data (see vgReadPixels() API)

@SYMTestExpectedResults
Creation of all drawable  resources have been completed without errors.
Image data obtained in client processes 1-N matches to the data which have been uploaded to the SgImages buffer from
process A. Reference counting works correctly and keep VG image alive although bound Sg and EGL images have been closed.
When all resources are closed, resource count maintained by RSgDriver extension is zero in all processes.
*/
TVerdict CEglTest_EGL_Image_Multi_Process_FontServer_Deferred::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0410"));
	SetTestStepName(KEGL_Image_Multi_Process_FontServer_Deferred);
	INFO_PRINTF1(_L("CEglTest_EGL_Image_Multi_Process_FontServer_Deferred::doTestStepL"));

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
	if(!numPixmapSgSurfaceFormats)
		{
		ERR_PRINTF1(_L("No formats to iterate through"));
		User::Leave(KErrArgument);
		}
	for(TUint j=0; j < numPixmapSgSurfaceFormats; j++)
		{
		iSurfaceFormat = iniParser->GetVgFormat(KSectionPixmapSgSurfaceFormats,j);
		iSourceFormat = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
		// A_8 related tests are only performed for SgImage-Lite
		if (iSourceFormat == EUidPixelFormatA_8)
			continue;
#endif
		doTestPartialStepL();
		}

	CleanupStack::PopAndDestroy(iniParser);
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

TVerdict CEglTest_EGL_Image_Multi_Process_FontServer_Deferred::doTestPartialStepL()
	{
	INFO_PRINTF1(_L("CEglTest_EGL_Image_Multi_Process_FontServer_Deferred::doTestPartialStepL"));
	PrintUsedPixelConfiguration();

	// Create display object
	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	// Create RSgImage's attributes
	TSgImageInfoTest imageInfo = TSgImageInfoTest(iSourceFormat, KImageSize);
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	imageInfo.iUsage = ESgUsageBitOpenVgImage | ESgUsageBitOpenVgSurface;
#else
    imageInfo.iUsage = ESgUsageOpenVgImage | ESgUsageOpenVgTarget;
   	imageInfo.iShareable = ETrue;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	// Create a pixmap surface matching the given pixel format
	iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo,CTestEglSession::EResourceCloseSgImageEarly);

	// list to maintain TSgDrawableId
	RArray<TSgDrawableId> sgIdList;
	CleanupClosePushL(sgIdList);
    RSgImage sgImages[KNumImages];

	INFO_PRINTF2(_L("MAIN PROCESS: Creating %d RSgImage(s)..."), KNumImages);
	for (TInt i=0; i<KNumImages; i++)
		{
		ASSERT_EQUALS(sgImages[i].Create(imageInfo, NULL, NULL), KErrNone);
        CleanupClosePushL(sgImages[i]);
		ASSERT_EQUALS(sgIdList.Insert(sgImages[i].Id(),i), KErrNone);

		EGLImageKHR eglImage = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImages[i], KEglImageAttribsPreservedTrue);
		ASSERT_EGL_TRUE(eglImage != EGL_NO_IMAGE_KHR);

		VGImage vgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)eglImage);
		ASSERT_VG_TRUE(vgImage != VG_INVALID_HANDLE);
		ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, eglImage));

		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
        CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, i);
    	// Add pixel data to the VGImage reference from the bitmap reference.
        // Mind the fact that CFbsBitmap and VGImages use different coordinates origin!
		TSize bitmapSize = bitmap->SizeInPixels();
    	TUint8* address = reinterpret_cast<TUint8*>(bitmap->DataAddress());
    	TInt stride = bitmap->DataStride();
    	address += (bitmapSize.iHeight - 1) * stride;
        vgImageSubData(vgImage, address, -stride, iSurfaceFormat, 0,0, bitmapSize.iWidth, bitmapSize.iHeight);
        delete bitmap;
        bitmap = NULL;
    	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
        eglWaitClient();
   		}

	INFO_PRINTF2(_L("MAIN PROCESS: About to launch %d processes..."), KNumProcesses);
	Test_MultiProcessL(KEglTestStepDllName, KNumProcesses, TestStepName(), sgIdList);
	CleanupStack::PopAndDestroy(KNumImages + 1, &sgIdList); //KNumImages SgImages, sgIdList
	INFO_PRINTF2(_L("MAIN PROCESS: All %d launched processes have completed!"), KNumProcesses);

	CleanAll();
	INFO_PRINTF1(_L("End of CEglTest_EGL_Image_Multi_Process_FontServer_Deferred::doTestPartialStepL"));
	return TestStepResult();
	}

void CEglTest_EGL_Image_Multi_Process_FontServer_Deferred::doProcessFunctionL(TInt aIdx,const TSgDrawableId& aSgId)
	{
	INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Process_FontServer_Deferred::doProcessFunctionL, Process %d"),aIdx);
	GetDisplayL();
	CreateEglSessionL(aIdx);
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	//Retrieve source formats for the launched process from the process parameters.
	User::LeaveIfError(User::GetTIntParameter(EProcSlotSourceFormat, reinterpret_cast<TInt&>(iSourceFormat)));
	User::LeaveIfError(User::GetTIntParameter(EProcSlotSurfaceFormat, reinterpret_cast<TInt&>(iSurfaceFormat)));

	RSgImage sgImageFromId;
	CleanupClosePushL(sgImageFromId);
	ASSERT_EQUALS(sgImageFromId.Open(aSgId), KErrNone);

	INFO_PRINTF2(_L("Process %d, Creating an EGLImage from the shared RSgImage"),aIdx);
	EGLImageKHR eglImage = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImageFromId, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(eglImage != EGL_NO_IMAGE_KHR);
	CleanupStack::PopAndDestroy(&sgImageFromId);

	INFO_PRINTF2(_L("Process %d, Creating a Surface and a Context bound to OpenVG"),aIdx);
    TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
    TSgImageInfoOpenVgTarget imageInfo = TSgImageInfoOpenVgTarget(pixelFormat, KImageSize);
	// Create a pixmap surface matching the native image pixel format
    iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo,CTestEglSession::EResourceCloseSgImageEarly);

    INFO_PRINTF2(_L("Process %d, Creating one VGImage from the EGLImage"),aIdx);
    VGImage vgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)eglImage);
	ASSERT_VG_TRUE(vgImage != VG_INVALID_HANDLE);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, eglImage));

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
		vgClear(0, 0, KImageSize.iWidth, KImageSize.iHeight);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);

		// fill paint
		VGPaint fillPaint = vgCreatePaint();
		vgSetPaint(fillPaint, VG_FILL_PATH);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		vgSetParameteri(fillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
		VGuint fillColor = 0x008000ff; // opaque dark green
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
		vgSetPixels(0, 0, vgImage, 0, 0, KImageSize.iWidth, KImageSize.iHeight);
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
		eglWaitClient();
		}

	// Check that the surface contains the expected pixels
	//  # if the source is a A_8, to compare the surface with a reference bitmap, the following is needed:
	//    a) a reference bitmap needs to be cleared to black (same colour as the surface was cleared to)
	//    b) a Pen bitmap, that we clear to dark green (same colour as the fillPaint used to draw to the surface)
	//    c) a mask bitmap, which is the reference bitmap used to create the SgImage
	//  # otherwise, the surface must contain the same pixels as the bitmap used to create the SgImage
	if (iSourceFormat == EUidPixelFormatA_8)
		{
		TDisplayMode maskMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
		CFbsBitmap* mask = iEglSess->CreateReferenceBitmapL(maskMode, KImageSize, ImageIndexFromProcessId(aIdx, KNumImages));
		CleanupStack::PushL(mask);

		// we need a reference bitmap with the same pixel format as the target surface
		TUidPixelFormat format = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
		TDisplayMode refbitmapMode = EglTestConversion::PixelFormatToDisplayMode(format);

		CFbsBitmap* refBitmap = iEglSess->CreateReferenceMaskedBitmapL(refbitmapMode, KRgbDarkGreen, mask);
		CleanupStack::PushL(refBitmap);

		// compare the obtained reference bitmap with the surface drawn
		iEglSess->CheckVgDrawingL(iSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(2, mask); //mask, refBitmap
		}
	else
		{
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
		CFbsBitmap* refBitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, ImageIndexFromProcessId(aIdx, KNumImages));
		CleanupStack::PushL(refBitmap);
		iEglSess->CheckVgDrawingL(iSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(refBitmap);
		}
	INFO_PRINTF2(_L("Process %d, VG drawing successfully completed and checked"),aIdx);

	// destroy VGImage
	vgDestroyImage(vgImage);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);

	// cleanup
	CleanAll();
	}


/**
@SYMTestCaseID GRAPHICS-EGL-0415

@SYMTestPriority 1

@SYMPREQ PREQ2637

@SYMTestCaseDesc
To ensure that SgImage with the data rendered as Pixmap surface can be shared across processes.
To ensure that reference counting of SgImage works correctly

@SYMTestActions
From the main process:
- Create M SgImages with the flags ESgUsageBitOpenVgImage & ESgUsageBitOpenVgSurface. The size are all the same.
  We are running through all the possible target configurations, with the values assumed being:
•	EUidPixelFormatRGB_565
•	EUidPixelFormatXRGB_8888
•	EUidPixelFormatARGB_8888_PRE
- Choose egl config, supplying as a native pixmap type in attribute (flag EGL_MATCH_NATIVE_PIXMAP) the SgImages which
  were created on the previous step. The EGL_RENDERABLE_TYPE of the config attributes must include EGL_OPENVG_BIT.
- Create M pixmap surfaces based on SgImages from the first step. The surface is created with EGL_ALPHA_FORMAT_PRE
  flag supplied in attribute list if the underlining SgImage was of type ESgPixelFormatARGB_8888_PRE.
- In iteration from 1 to M perform three following steps:
	1. Make the pixmap surface current (see eglMakeCurrent(.) API)
	2. Draw something to the current surface, for instance, clear the whole area with color and then draw a
	   few graphics primitives. The drawing needs to be unique for each surface and complicated enough to
	   ensure that bit comparison will reveal any mismatch
	3. Make no surface current
- Close all pixmap surfaces
- Spawn N client processes. During the process launching, pass to each process single drawable ID from the SgImages.
  In order to define which SgImage(s) needs to be passed to the particular processes, there will be following
  formula applied: J = P Mod (M), where J is the sequence number of the image, P is the particular process number.
From processes 1 to N:
- Open SgImage by TSgDrawableId obtained from the SgImage which was passed from the process A.
- Using EGL extension, create EGLImage specifying as EGLClientBuffer SgImage which was opened on the previous
  step,  EGL_NATIVE_PIXMAP_KHR as a target and EGL_IMAGE_PRESERVED_KHR as an attribute
- Using VG extension, create VG image based on EGLImage from the previous step.
- Close both Sg and EGL images
- Create second SgImage with the same size and pixel format as first SgImage and usage flag is set to ESgUsageBitOpenVgSurface.
- Create off-screen pixmap surface with underlining second SgImage and make it current for the drawing context.
- Draw VGImage to the off-screen surface.
- Retrieve surface data (see vgReadPixels() API)

@SYMTestExpectedResults
Creation of all drawable resources has been completed without errors.
On return eglChooseConfig() must return EGL_OPENVG_BIT in config attribute list (actual attributes should
be retrieved via call to eglGetConfigAttrib()).
Image data obtained in client processes 1 - N matches to the pixmap surface which was drawn in the process A.
Reference counting works correctly and keep VG image alive although bound Sg and EGL images have been closed.
When all resources are closed, resource count maintained by RSgDriver extension is zero in all processes.
*/
TVerdict CEglTest_EGL_Image_Multi_Process_ThemeServer::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0415"));
	SetTestStepName(KEGL_Image_Multi_Process_ThemeServer);
	INFO_PRINTF1(_L("CEglTest_EGL_Image_Multi_Process_ThemeServer::doTestStepL"));

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
	if(!numPixmapSgSurfaceFormats)
		{
		ERR_PRINTF1(_L("No formats to iterate through"));
		User::Leave(KErrArgument);
		}
	for(TUint j=0; j < numPixmapSgSurfaceFormats; j++)
		{
		iSurfaceFormat = iniParser->GetVgFormat(KSectionPixmapSgSurfaceFormats,j);
		iSourceFormat = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
		// A_8 related tests are only performed for SgImage-Lite
		if (iSourceFormat == EUidPixelFormatA_8)
			continue;
#endif
		doTestPartialStepL();
		}

	CleanupStack::PopAndDestroy(iniParser);
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

TVerdict CEglTest_EGL_Image_Multi_Process_ThemeServer::doTestPartialStepL()
	{
	INFO_PRINTF1(_L("CEglTest_EGL_Image_Multi_Process_ThemeServer::doTestPartialStepL"));
	PrintUsedPixelConfiguration();

	// Create display object
	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	// list to maintain TSgDrawableId
	RArray<TSgDrawableId> sgIdList;
	CleanupClosePushL(sgIdList);
    RSgImage sgImages[KNumImages];

	INFO_PRINTF2(_L("MAIN PROCESS: Creating %d RSgImage(s)..."), KNumImages);
	for (TInt i=0; i<KNumImages; i++)
		{
		// Create RSgImage's attributes
		TSgImageInfoTest imageInfo = TSgImageInfoTest(iSourceFormat, KImageSize);
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
		imageInfo.iUsage = ESgUsageBitOpenVgImage | ESgUsageBitOpenVgSurface;
#else
		imageInfo.iUsage = ESgUsageOpenVgImage | ESgUsageOpenVgTarget;
    	imageInfo.iShareable = ETrue;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE

		ASSERT_EQUALS(sgImages[i].Create(imageInfo, NULL, NULL), KErrNone);
	    CleanupClosePushL(sgImages[i]);
		ASSERT_EQUALS(sgIdList.Insert(sgImages[i].Id(),i), KErrNone);

		INFO_PRINTF1(_L("Calling sequence - eglBindAPI(EGL_OPENVG_API) - eglCreatePixmapSurface - eglCreateContext - eglMakeCurrent"));
	    ASSERT_EGL_TRUE(eglBindAPI(EGL_OPENVG_API));

    	const EGLint KAttrib_list_image_pre[] = {   EGL_MATCH_NATIVE_PIXMAP,	reinterpret_cast<EGLint>(&sgImages[i]),
													EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
													EGL_SURFACE_TYPE, EGL_PIXMAP_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT,
													EGL_NONE };
    	const EGLint KAttrib_list_image_nonpre[] = {EGL_MATCH_NATIVE_PIXMAP,	reinterpret_cast<EGLint>(&sgImages[i]),
													EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
													EGL_SURFACE_TYPE, EGL_PIXMAP_BIT,
													EGL_NONE };
		EGLConfig currentConfig;
		EGLint numconfigs =0;
		EGLSurface surface = EGL_NO_SURFACE;
	    if (iSourceFormat == EUidPixelFormatARGB_8888_PRE)
	    	{
			ASSERT_EGL_TRUE(eglChooseConfig(iDisplay,KAttrib_list_image_pre,&currentConfig,1,&numconfigs))
			ASSERT_EGL_TRUE(numconfigs==1);
	    	surface = eglCreatePixmapSurface(iDisplay, currentConfig,&sgImages[i], KPixmapAttribsVgAlphaFormatPre);
	    	}
	    else
	    	{
			ASSERT_EGL_TRUE(eglChooseConfig(iDisplay,KAttrib_list_image_nonpre,&currentConfig,1,&numconfigs))
			ASSERT_EGL_TRUE(numconfigs==1);
	    	surface = eglCreatePixmapSurface(iDisplay, currentConfig,&sgImages[i], KPixmapAttribsVgAlphaFormatNonPre);
	    	}
    	ASSERT_EGL_TRUE(surface != EGL_NO_SURFACE);
	    EGLContext context = eglCreateContext(iDisplay, currentConfig, EGL_NO_CONTEXT, NULL);
	    ASSERT_EGL_TRUE(context != EGL_NO_CONTEXT);
	    ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, surface, surface, context));

    	//Drawing to the current surface (and hence to the RSgImage) to test that the contents are preserved
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
		CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, i);
        // Mind the fact that CFbsBitmap and VGImages use different coordinates origin!
		TSize bitmapSize = bitmap->SizeInPixels();
    	TUint8* address = reinterpret_cast<TUint8*>(bitmap->DataAddress());
    	TInt stride = bitmap->DataStride();
    	address += (bitmapSize.iHeight - 1) * stride;
    	vgWritePixels(address, -stride, iSurfaceFormat, 0,0, bitmapSize.iWidth, bitmapSize.iHeight);
   	    eglWaitClient();   // wait for writing to finish
		delete bitmap;
		bitmap = NULL;
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);

    	// Make no surface current and destroy surface
       	ASSERT_EGL_TRUE(eglDestroySurface(iDisplay, surface));
    	ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
		}

	INFO_PRINTF2(_L("MAIN PROCESS: About to launch %d processes..."), KNumProcesses);
	Test_MultiProcessL(KEglTestStepDllName, KNumProcesses, TestStepName(), sgIdList);
	CleanupStack::PopAndDestroy(KNumImages + 1, &sgIdList); //KNumImages SgImages, sgIdList
	INFO_PRINTF2(_L("MAIN PROCESS: All %d launched processes have completed!"), KNumProcesses);

	CleanAll();
	INFO_PRINTF1(_L("End of CEglTest_EGL_Image_Multi_Process_ThemeServer::doTestPartialStepL"));
	return TestStepResult();
	}

void CEglTest_EGL_Image_Multi_Process_ThemeServer::doProcessFunctionL(TInt aIdx,const TSgDrawableId& aSgId)
	{
	INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Process_ThemeServer::doProcessFunctionL, Process %d"),aIdx);
	GetDisplayL();
	CreateEglSessionL(aIdx);
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();

	//Retrieve source formats for the launched process from the process parameters.
	User::LeaveIfError(User::GetTIntParameter(EProcSlotSourceFormat, reinterpret_cast<TInt&>(iSourceFormat)));
	User::LeaveIfError(User::GetTIntParameter(EProcSlotSurfaceFormat, reinterpret_cast<TInt&>(iSurfaceFormat)));

	RSgImage sgImageFromId;
	CleanupClosePushL(sgImageFromId);
	ASSERT_EQUALS(sgImageFromId.Open(aSgId), KErrNone);

	INFO_PRINTF2(_L("Process %d, Creating an EGLImage from the shared RSgImage"),aIdx);
	EGLImageKHR eglImage = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &sgImageFromId, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(eglImage != EGL_NO_IMAGE_KHR);
	CleanupStack::PopAndDestroy(&sgImageFromId);

	INFO_PRINTF2(_L("Process %d, Creating a Surface and a Context bound to OpenVG"),aIdx);
    TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(KDefaultSurfaceFormat);
    TSgImageInfoOpenVgTarget imageInfo = TSgImageInfoOpenVgTarget(pixelFormat, KImageSize);
	// Create a pixmap surface matching the native image pixel format
    iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo,CTestEglSession::EResourceCloseSgImageEarly);

    INFO_PRINTF2(_L("Process %d, Creating one VGImage from the EGLImage"),aIdx);
    VGImage vgImage = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)eglImage);
	ASSERT_VG_TRUE(vgImage != VG_INVALID_HANDLE);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, eglImage));

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
		vgClear(0, 0, KImageSize.iWidth, KImageSize.iHeight);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);

		// fill paint
		VGPaint fillPaint = vgCreatePaint();
		vgSetPaint(fillPaint, VG_FILL_PATH);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		vgSetParameteri(fillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
		VGuint fillColor = 0x008000ff; // opaque dark green
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
		vgSetPixels(0, 0, vgImage, 0, 0, KImageSize.iWidth, KImageSize.iHeight);
		ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
		eglWaitClient();
		}

	// Check that the surface contains the expected pixels
	//  # if the source is a A_8, to compare the surface with a reference bitmap, the following is needed:
	//    a) a reference bitmap needs to be cleared to black (same colour as the surface was cleared to)
	//    b) a Pen bitmap, that we clear to dark green (same colour as the fillPaint used to draw to the surface)
	//    c) a mask bitmap, which is the reference bitmap used to create the SgImage
	//  # otherwise, the surface must contain the same pixels as the bitmap used to create the SgImage
	if (iSourceFormat == EUidPixelFormatA_8)
		{
		TDisplayMode maskMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
		CFbsBitmap* mask = iEglSess->CreateReferenceBitmapL(maskMode, KImageSize, ImageIndexFromProcessId(aIdx, KNumImages));
		CleanupStack::PushL(mask);

		// we need a reference bitmap with the same pixel format as the target surface
		TUidPixelFormat format = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
		TDisplayMode refbitmapMode = EglTestConversion::PixelFormatToDisplayMode(format);

		CFbsBitmap* refBitmap = iEglSess->CreateReferenceMaskedBitmapL(refbitmapMode, KRgbDarkGreen, mask);
		CleanupStack::PushL(refBitmap);

		// compare the obtained reference bitmap with the surface drawn
		iEglSess->CheckVgDrawingL(iSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(2, mask); //mask, refBitmap
		}
	else
		{
		TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
		CFbsBitmap* refBitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, ImageIndexFromProcessId(aIdx, KNumImages));
		CleanupStack::PushL(refBitmap);
		iEglSess->CheckVgDrawingL(iSurfaceFormat, refBitmap);
		CleanupStack::PopAndDestroy(refBitmap);
		}
	INFO_PRINTF2(_L("Process %d, VG drawing successfully completed and checked"),aIdx);

	// destroy VGImage
	vgDestroyImage(vgImage);
	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);

	// cleanup
	CleanAll();
	}


/**
@SYMTestCaseID GRAPHICS-EGL-0430

@SYMTestPriority 1

@SYMPREQ 2637

@SYMTestCaseDesc
Functional test - Killing the RSgImage creating process

@SYMTestPurpose
To verify correct operation of RSgImage sharing across processes when the creating process is killed

@SYMTestActions
Run two processes that independently perform the actions detailed below.
* From Process A
	Open the RSgDriver
	Create an RSgImage passing an initialised bitmap
	Signal (by semaphore or otherwise) to process B, passing the drawable ID to it

* From Process B:
	Open the RSgDriver
	Using the drawable ID, open the RSgImage
	Close the RSgImage
	Re-open the RSgImage

* From Process A:
	Unexpectedly terminate process A without performing any explicit clean-up

* From Process B:
	Wait for Process A to be killed:
	Create an EGLImage from the RSgImage
	Create a VGImage from the EGLImage
	Close the RSgImage
	Close the EGLImage
	Create an off-screen surface
	Draw VGImage to the off-screen surface
	Read the pixel data and verify that it matches the data populated by process A
	Destroy the off-screen surface
	Close the VGImage
	Close the RSgDriver
	Exit

@SYMTestExpectedResults
Process B should be able to populate the VGImage with data and copy it to the off-screen surface
All allocated image memory should be freed
*/
TVerdict CEglTest_EGL_Image_Multi_Process_VgImage_ProcessTerminate::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0430"));
	SetTestStepName(KEGL_Image_Multi_Process_VgImage_ProcessTerminate);
    INFO_PRINTF1(_L("Enter: CEglTest_EGL_Image_Multi_Process_VgImage_ProcessTerminate::doTestStepL"));

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

	// launch 2 processes
 	Test_MultiProcessL(KEglTestStepDllName, 2, TestStepName());

	INFO_PRINTF1(_L("Exit: CEglTest_EGL_Image_Multi_Process_VgImage_ProcessTerminate::doTestStepL"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

void CEglTest_EGL_Image_Multi_Process_VgImage_ProcessTerminate::doProcessFunctionL(TInt aIdx)
    {
    INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Process_VgImage_ProcessTerminate::doProcessFunctionL, Process %d"),aIdx);
    GetDisplayL();
    CreateEglSessionL(aIdx);
    iEglSess->InitializeL();
    iEglSess->OpenSgDriverL();

	//Retrieve source formats for the launched process from the process parameters.
	User::LeaveIfError(User::GetTIntParameter(EProcSlotSourceFormat, reinterpret_cast<TInt&>(iSourceFormat)));
	User::LeaveIfError(User::GetTIntParameter(EProcSlotSurfaceFormat, reinterpret_cast<TInt&>(iSurfaceFormat)));

	//create the queue to send/receive SgImage ID between processes
	RMsgQueue<TSgDrawableId> messageQueueSgId;
    User::LeaveIfError(messageQueueSgId.Open(EProcSlotMsgQueueSgId, EOwnerProcess));
    CleanupClosePushL(messageQueueSgId);

	//create the queue to send/receive Process ID between processes
    RMsgQueue<TProcessId> messageQueueProcId;
    User::LeaveIfError(messageQueueProcId.Open(EProcSlotMsgQueueProcId, EOwnerProcess));
    CleanupClosePushL(messageQueueProcId);

    RProcess process;
    CleanupClosePushL(process);
	TRequestStatus status;

	TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, KImageSize, 6);
	CleanupStack::PushL(bitmap);

	RSgImage rSgImageLocal;
    if(aIdx == 0)
        {
    	// Create an RSgImage
    	INFO_PRINTF2(_L("Process %d, Creating a RSgImage"),aIdx);
    	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(iSourceFormat, KImageSize);
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    	imageInfo.iShareable = ETrue;
    	imageInfo.iCpuAccess = ESgCpuAccessReadWrite;
#endif
    	ASSERT_EQUALS(rSgImageLocal.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride()), KErrNone);

        INFO_PRINTF2(_L("Process %d, Sending SgImage ID to other process..."), aIdx);
        messageQueueSgId.SendBlocking(rSgImageLocal.Id());

        // Sending Process ID to other process... so that the other process can identify when this one dies.
        messageQueueProcId.SendBlocking(RProcess().Id());
        }
    else if(aIdx == 1)
        {
        INFO_PRINTF2(_L("Process %d: Receiving SgImage ID from other process..."), aIdx);
        TSgDrawableId sgImageId;
        messageQueueSgId.ReceiveBlocking(sgImageId);
    	ASSERT_EQUALS(rSgImageLocal.Open(sgImageId),KErrNone);

        // Also receiving RProcess ID from other process to be able to identify when it dies
        TProcessId procId;
        messageQueueProcId.ReceiveBlocking(procId);
        process.Open(procId);
        process.Logon(status);

        INFO_PRINTF2(_L("Process %d: Closing and Opening SgImage again..."), aIdx);
    	rSgImageLocal.Close();
    	ASSERT_EQUALS(rSgImageLocal.Open(sgImageId),KErrNone);
        }

	// Wait for both processes to reach this point
    Rendezvous(aIdx);

    if(aIdx == 0)
    	{
    	// simulate this process being killed
    	// note that we terminate with reason=0 (otherwise the egl test framework would think it's an error)
       	INFO_PRINTF2(_L("Process %d, Simulate the process is being killed!"),aIdx);
    	RProcess().Terminate(KErrNone);

    	// this line is unreachable
    	ASSERT(0);
    	}
    else if(aIdx == 1)
        {
        // first wait for the other process to finish
        User::WaitForRequest(status);
        ASSERT_EQUALS(status.Int(), KErrNone);

        INFO_PRINTF2(_L("Process %d, Creating an EGLImage from the shared RSgImage"),aIdx);
    	CleanupClosePushL(rSgImageLocal);
        EGLImageKHR eglImageLocal = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &rSgImageLocal, KEglImageAttribsPreservedTrue);
    	ASSERT_EGL_TRUE(eglImageLocal != EGL_NO_IMAGE_KHR);
    	CleanupStack::PopAndDestroy(&rSgImageLocal); 	//transferring ownership of the buffer to the EGLImage

    	INFO_PRINTF2(_L("Creating a Surface and a Context bound to OpenVG, Process %d"),aIdx);
    	TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
    	TSgImageInfoOpenVgTarget imageInfo = TSgImageInfoOpenVgTarget(pixelFormat, KImageSize);
    	// Create a pixmap surface matching the native image pixel format
    	iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo,CTestEglSession::EResourceCloseSgImageEarly);

    	INFO_PRINTF2(_L("Process %d, Creating one VGImage from the EGLImage"),aIdx);
    	VGImage vgImageLocal = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)eglImageLocal);
    	ASSERT_VG_TRUE(vgImageLocal != VG_INVALID_HANDLE);
    	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, eglImageLocal));

        INFO_PRINTF2(_L("Process %d, Drawing the VGImage to the current surface before changing contents of the VGImage"),aIdx);
        // Copy the source VGImage to the surface
    	vgSetPixels(0, 0, vgImageLocal, 0, 0, KImageSize.iWidth, KImageSize.iHeight);
    	ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
    	eglWaitClient();

    	// we can now compare the VgImage to the one we expect
		iEglSess->CheckVgDrawingL(iSurfaceFormat, bitmap);
		INFO_PRINTF2(_L("Process %d, Drawing successful"),aIdx);

		// cleanup
		vgDestroyImage(vgImageLocal);
    	ASSERT_TRUE(vgGetError() == VG_NO_ERROR);
        }

    //cleanup and finish
	CleanupStack::PopAndDestroy(4, &messageQueueSgId); //messageQueueSgId, messageQueueProcId, process, bitmap
	CleanAll();
    }


/**
@SYMTestCaseID GRAPHICS-EGL-0429

@SYMTestPriority 1

@SYMPREQ 2637

@SYMTestCaseDesc
Functional test - Killing the RSgImage creating process

@SYMTestPurpose
To verify correct operation of RSgImage sharing across processes when the creating process is killed

@SYMTestActions
Run two processes that independently perform the actions detailed below.
* From Process A:
	Open the RSgDriver
	Create an RSgImage
	Signal (by semaphore or otherwise) to process B, passing the drawable ID to it

* From Process B:
	Open the RSgDriver

* From Process A:
	Unexpectedly terminate process A without performing any explicit clean-up

* From Process B:
	Wait for Process A to be killed:
	Using the drawable ID, attempt to open the RSgImage
	Close the RSgDriver
	Exit

@SYMTestExpectedResults
Process B should be unable to open the RSgImage and the call to Open() should return error code KErrNotFound.
All allocated image memory should be freed
*/
TVerdict CEglTest_EGL_Image_Multi_Process_VgImage_ProcessTerminateNegative::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0429"));
	SetTestStepName(KEGL_Image_Multi_Process_VgImage_ProcessTerminateNegative);
    INFO_PRINTF1(_L("Enter: CEglTest_EGL_Image_Multi_Process_VgImage_ProcessTerminateNegative::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	// This test is performed for default pixel format
	PrintUsedPixelConfiguration();

	// launch 2 processes
	Test_MultiProcessL(KEglTestStepDllName, 2, TestStepName());

	INFO_PRINTF1(_L("Exit: CEglTest_EGL_Image_Multi_Process_VgImage_ProcessTerminateNegative::doTestStepL"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

void CEglTest_EGL_Image_Multi_Process_VgImage_ProcessTerminateNegative::doProcessFunctionL(TInt aIdx)
    {
    INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Process_VgImage_ProcessTerminateNegative::doProcessFunctionL, Process %d"),aIdx);
    GetDisplayL();
    CreateEglSessionL(aIdx);
    iEglSess->InitializeL();
    iEglSess->OpenSgDriverL();

	//Retrieve source formats for the launched process from the process parameters.
	User::LeaveIfError(User::GetTIntParameter(EProcSlotSourceFormat, reinterpret_cast<TInt&>(iSourceFormat)));
	User::LeaveIfError(User::GetTIntParameter(EProcSlotSurfaceFormat, reinterpret_cast<TInt&>(iSurfaceFormat)));

	//create the queue to send/receive SgImage ID between processes
	RMsgQueue<TSgDrawableId> messageQueueSgId;
    User::LeaveIfError(messageQueueSgId.Open(EProcSlotMsgQueueSgId, EOwnerProcess));
    CleanupClosePushL(messageQueueSgId);

	//create the queue to send/receive Process ID between processes
    RMsgQueue<TProcessId> messageQueueProcId;
    User::LeaveIfError(messageQueueProcId.Open(EProcSlotMsgQueueProcId, EOwnerProcess));
    CleanupClosePushL(messageQueueProcId);

    RProcess process;
    CleanupClosePushL(process);
	TRequestStatus status;

    RSgImage rSgImageLocal;
    TSgDrawableId sgImageId;
	if(aIdx == 0)
        {
    	// Create an RSgImage
    	INFO_PRINTF2(_L("Process %d, Creating a RSgImage"),aIdx);
    	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(iSourceFormat, KImageSize);
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    	imageInfo.iShareable = ETrue;
    	imageInfo.iCpuAccess = ESgCpuAccessReadWrite;
#endif
    	ASSERT_EQUALS(rSgImageLocal.Create(imageInfo,NULL, NULL), KErrNone);

        INFO_PRINTF2(_L("Process %d, Sending SgImage ID to other process..."), aIdx);
        messageQueueSgId.SendBlocking(rSgImageLocal.Id());

        // Sending Process ID to other process... so that the other process can identify when this one dies.
        messageQueueProcId.SendBlocking(RProcess().Id());
        }
    else if(aIdx == 1)
        {
        INFO_PRINTF2(_L("Process %d: Receiving SgImage ID from other process..."), aIdx);
        //unlike the other cases, do not open it (yet)
        messageQueueSgId.ReceiveBlocking(sgImageId);

        // Also receiving RProcess ID from other process to be able to identify when it dies
        TProcessId procId;
        messageQueueProcId.ReceiveBlocking(procId);
        process.Open(procId);
        process.Logon(status);
        }

	// Wait for both processes to reach this point
    Rendezvous(aIdx);

    if(aIdx == 0)
    	{
    	// simulate this process being killed
    	// note that we terminate with reason=0 (otherwise the egl test framework would think it's an error)
       	INFO_PRINTF2(_L("Process %d, Simulate the process is being killed!"),aIdx);
    	RProcess().Terminate(KErrNone);

    	// this line is unreachable
    	ASSERT(0);
    	}
    else if(aIdx == 1)
        {
        // first wait for the other process to finish
        User::WaitForRequest(status);
        ASSERT_EQUALS(status.Int(), KErrNone);

        // NOTE: We can't guarante when the kernel will have completed the cleanup. This process
        //	could have been notified that the other process has terminated but this does not guarantee
        //	that all handles to the process have been released.
        //	This is not generally a problem in single processor hardware, but can be a problem in dual
        //	processor hardware (ie, NaviEngine) where one processor could be cleaning up the terminated
        //	process, the other processor could already be issuing the notification to the waiting process
        //	Not much we can do other than adding a small delay to ensure this...
        User::After(1*1000*1000); // 1 second

        // we're expecting it to fail with the appropriate error
        TInt ret = rSgImageLocal.Open(sgImageId);
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
        INFO_PRINTF4(_L("Process %d: Opening SgImage resulted in %d (expected was %d)."), aIdx, ret, KErrNotFound);
        ASSERT_EQUALS(ret, KErrNotFound);
#else
        INFO_PRINTF4(_L("Process %d: Opening SgImage resulted in %d (expected was %d)."), aIdx, ret, KErrArgument);
        ASSERT_EQUALS(ret, KErrArgument);
#endif
        }

    //cleanup and finish
	CleanupStack::PopAndDestroy(3, &messageQueueSgId); //messageQueueSgId, messageQueueProcId, process
	CleanAll();
    }


/**
@SYMTestCaseID GRAPHICS-EGL-0431

@SYMTestPriority 1

@SYMPREQ 2637

@SYMTestCaseDesc
Functional test - Simultaneous reading and writing of simulated glyphs.
The rectangular area of RSgImage will be divided into the following section:
	 -----------
    ¦ 0 ¦ 1 ¦ 2 ¦
    ¦-----------
    ¦ 3 ¦ 4 ¦ 5 ¦
    ¦-----------
    ¦ 6 ¦ 7 ¦ 8 ¦
	 -----------
The image size is taken to be 90x90 so that it is easily split between 9 sub-sections
It is obvoious that each sub-section will therefore be of 30x30:

@SYMTestPurpose
To determine that the system can cope with simultaneous
reading and writing from/to area within RSgImage without corrupting each other.

@SYMTestActions
Run two processes that independently perform the actions detailed below.
* From Process A:
	Open the RSgDriver
	Create an RSgImages with no content
	For each RSgImage, create an EGLImage and from that create a VGImage
	Close the RSgImage and the EGLImage
* From Process B:
	Open the RSgDriver
	Open the RSgImage using the drawable ID passed from process A
	Create an EGLImage and then a VGImage
	Close the RSgImage and the EGLImage
	Wait for signal from process A

* Concurrently from Process A and the client process:
	Process A:
		For i = 1 to 9
			Shade section[i] to colour[i]
			Signal client process that section[i] can be read
			Repeat until client process signal read complete
				Shade sections surrounding section[i] to other colors e.g. when i=1, 
				surrounding sections are section 0, 2 and 4 
				End loop
			End loop

	Process B:
		For i = 1 to 9
			Wait for signal that section[i] is ready
			Create child VGImage for section[i]
			Read the value of child VGImage and compare it with colour[i]
			Signal process A to indicate read is complete
			Destroy child VGImage
			End loop

* Processes A and B:
Close the VGImage and RSgImage driver

@SYMTestExpectedResults
The content of each section read by client process should match the content written by Process A.
All image memory should be freed
*/
TVerdict CEglTest_EGL_Image_Multi_Process_VgImage_ReadWrite::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0431"));
	SetTestStepName(KEGL_Image_Multi_Process_VgImage_ReadWrite);
    INFO_PRINTF1(_L("Enter: CEglTest_EGL_Image_Multi_Process_VgImage_ReadWrite::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap | KVG_KHR_EGL_image);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	// This test is performed for ARGB_8888 non pre-multiplied
	// as we compare pixels manually, we avoid having to do the pre-multiply in the test itself
    iSourceFormat = EUidPixelFormatARGB_8888;
    iSurfaceFormat = VG_sARGB_8888;
	PrintUsedPixelConfiguration();

	// launch 2 processes
	Test_MultiProcessL(KEglTestStepDllName, 2, TestStepName());

	INFO_PRINTF1(_L("Exit: CEglTest_EGL_Image_Multi_Process_VgImage_ReadWrite::doTestStepL"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

void CEglTest_EGL_Image_Multi_Process_VgImage_ReadWrite::doProcessFunctionL(TInt aIdx)
    {
    INFO_PRINTF2(_L("CEglTest_EGL_Image_Multi_Process_VgImage_ReadWrite::doProcessFunctionL, Process %d"),aIdx);
    GetDisplayL();
    CreateEglSessionL(aIdx);
    iEglSess->InitializeL();
    iEglSess->OpenSgDriverL();

	const TSize KTestReadWriteImageSize(90,90);
	const TInt KTestReadWriteSubImageLength = KTestReadWriteImageSize.iHeight / 3;
	const TInt KTestNumColors = 9;
	const VGfloat KTestClearColors[KTestNumColors][4] =
		{
		{0.11f, 0.13f, 0.15f, 0.17f},	// arbitrary colour 1
		{0.21f, 0.23f, 0.25f, 0.27f},	// arbitrary colour 2
		{0.31f, 0.33f, 0.35f, 0.37f},	// arbitrary colour 3
		{0.41f, 0.43f, 0.45f, 0.47f},	// arbitrary colour 4
		{0.51f, 0.53f, 0.55f, 0.57f},	// arbitrary colour 5
		{0.61f, 0.63f, 0.65f, 0.67f},	// arbitrary colour 6
		{0.71f, 0.73f, 0.75f, 0.77f},	// arbitrary colour 7
		{0.81f, 0.83f, 0.85f, 0.87f},	// arbitrary colour 8
		{0.91f, 0.93f, 0.95f, 0.97f}	// arbitrary colour 9
		};

	//Retrieve source formats for the launched process from the process parameters.
	User::LeaveIfError(User::GetTIntParameter(EProcSlotSourceFormat, reinterpret_cast<TInt&>(iSourceFormat)));
	User::LeaveIfError(User::GetTIntParameter(EProcSlotSurfaceFormat, reinterpret_cast<TInt&>(iSurfaceFormat)));

	//create the queue to send/receive SgImage ID between processes
	RMsgQueue<TSgDrawableId> messageQueueSgId;
    User::LeaveIfError(messageQueueSgId.Open(EProcSlotMsgQueueSgId, EOwnerProcess));
    CleanupClosePushL(messageQueueSgId);

    RSgImage rSgImageLocal;
    if(aIdx == 0)
        {
    	// Create an RSgImage
    	INFO_PRINTF2(_L("Process %d, Creating a RSgImage"),aIdx);
    	TSgImageInfoOpenVgImage imageInfo = TSgImageInfoOpenVgImage(iSourceFormat, KImageSize);
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    	imageInfo.iShareable = ETrue;
    	imageInfo.iCpuAccess = ESgCpuAccessReadWrite;
#endif
    	ASSERT_EQUALS(rSgImageLocal.Create(imageInfo,NULL, NULL), KErrNone);

        INFO_PRINTF2(_L("Process %d, Sending SgImage ID to other process..."), aIdx);
        messageQueueSgId.SendBlocking(rSgImageLocal.Id());
        }
    else if(aIdx == 1)
        {
        INFO_PRINTF2(_L("Process %d: Receiving SgImage ID from other process..."), aIdx);
        TSgDrawableId sgImageId;
        messageQueueSgId.ReceiveBlocking(sgImageId);
    	ASSERT_EQUALS(rSgImageLocal.Open(sgImageId),KErrNone);
        }

	// Wait for both processes to reach this point
    Rendezvous(aIdx);
    
    INFO_PRINTF2(_L("Process %d, Creating an EGLImage from the shared RSgImage"),aIdx);
	CleanupClosePushL(rSgImageLocal);
	EGLImageKHR eglImageLocal = iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &rSgImageLocal, KEglImageAttribsPreservedTrue);
	ASSERT_EGL_TRUE(eglImageLocal != EGL_NO_IMAGE_KHR);
	CleanupStack::PopAndDestroy(&rSgImageLocal); 	//transferring ownership of the buffer to the EGLImage

	// OpenVG needs a current VG context before it will allow the call vgCreateEGLImageTargetKHR
	// The created surface will remain un-used, hence we create it with the default pixel format, as we don't care
	TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(KDefaultSurfaceFormat);
	TSgImageInfoOpenVgTarget imageInfo = TSgImageInfoOpenVgTarget(pixelFormat, KTestReadWriteImageSize);
	iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo,CTestEglSession::EResourceCloseSgImageEarly);

	INFO_PRINTF2(_L("Process %d, Creating one VGImage from the EGLImage"),aIdx);
	VGImage vgImageLocal = iEglSess->vgCreateImageTargetKHR((VGeglImageKHR)eglImageLocal);
	ASSERT_VG_TRUE(vgImageLocal != VG_INVALID_HANDLE);
	ASSERT_EGL_TRUE(iEglSess->DestroyEGLImage(iDisplay, eglImageLocal));

	// Wait for both processes to reach this point
    Rendezvous(aIdx);

    for (TInt section=0; section<9; section++)
    	{
    	INFO_PRINTF3(_L("Process %d, Starting loop for section[%d]"),aIdx, section);
    	if (aIdx==0)
    		{
        	INFO_PRINTF3(_L("Process %d, Shading section[%d]"),aIdx, section);
    		//Shade section[i] to color[i]
    	    vgSetfv(VG_CLEAR_COLOR, 4, KTestClearColors[section]);
            vgClearImage(vgImageLocal, (section%3)*KTestReadWriteSubImageLength, (section/3)*KTestReadWriteSubImageLength, KTestReadWriteSubImageLength, KTestReadWriteSubImageLength);        
    		vgFinish();
    		}

    	// Wait for both processes to reach this point
        Rendezvous(aIdx);

        VGImage childImage = VG_INVALID_HANDLE;
        if (aIdx==1)
    		{
        	INFO_PRINTF3(_L("Process %d, Creating child vgimage for section[%d]"),aIdx, section);
			//Create child VGImage for section[i]
    		childImage = vgChildImage(vgImageLocal, (section%3)*KTestReadWriteSubImageLength, (section/3)*KTestReadWriteSubImageLength, KTestReadWriteSubImageLength, KTestReadWriteSubImageLength);
    		//Read the value of child VGImage and compare it with colour[i]
			TUint32* vgPixel = new(ELeave) TUint32[KTestReadWriteSubImageLength];
			CleanupArrayDeletePushL(vgPixel);
    		for (TInt i=0; i<KTestReadWriteSubImageLength; i++)
    			{
    			// Read childImage, a line at a time
    			vgGetImageSubData(childImage, vgPixel, 1, iSurfaceFormat, 0, i, KTestReadWriteSubImageLength, 1);
    			for (TInt j=0; j<KTestReadWriteSubImageLength; j++)
    				{
					// Should be exact, but give a tolerance of 1 because VG rounds to nearer integer, whereas TInt rounds down 
    				ASSERT_TRUE(Abs(((vgPixel[j] & 0xff000000) >> 24) - (255 * KTestClearColors[section][3])) <= 1);	//alpha
    				ASSERT_TRUE(Abs(((vgPixel[j] & 0x00ff0000) >> 16) - (255 * KTestClearColors[section][0])) <= 1);	//red
    				ASSERT_TRUE(Abs(((vgPixel[j] & 0x0000ff00) >> 8) - (255 * KTestClearColors[section][1])) <= 1); 	//green
    				ASSERT_TRUE(Abs(((vgPixel[j] & 0x000000ff) >> 0) - (255 * KTestClearColors[section][2])) <= 1); 	//blue
					}
    			}
    		CleanupStack::PopAndDestroy(vgPixel);
    		}
    	if (aIdx==0)
    		{
        	INFO_PRINTF3(_L("Process %d, Shading surrounding sections to section[%d]"),aIdx, section);
        	for (TInt k=-3; k<=3; k=k+2)
        		{
            	TInt surroundingSection = (KTestNumColors + section + k) % KTestNumColors;
        	    vgSetfv(VG_CLEAR_COLOR, 4, KTestClearColors[surroundingSection]);
        	    vgClearImage(vgImageLocal, (surroundingSection%3)*KTestReadWriteSubImageLength, (surroundingSection/3)*KTestReadWriteSubImageLength, KTestReadWriteSubImageLength, KTestReadWriteSubImageLength);        
        		}
    		vgFinish(); 
    		}

    	// Wait for both processes to reach this point
        Rendezvous(aIdx);

        if (aIdx==1)
        	{
    		INFO_PRINTF3(_L("Process %d, destroying child vgimage for section[%d]"),aIdx, section);
        	//Destroy child VGImage
    		vgDestroyImage(childImage);
    		ASSERT_TRUE(vgGetError() == VG_NO_ERROR);
        	}
    	}

	// cleanup
	vgDestroyImage(vgImageLocal);
	ASSERT_TRUE(vgGetError() == VG_NO_ERROR);
	CleanupStack::PopAndDestroy(&messageQueueSgId);
	CleanAll();
    }

