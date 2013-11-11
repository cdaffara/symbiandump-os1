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

#include "egltest_stress_process_sgimage.h"

GLDEF_C TInt E32Main()
	{
	RDebug::Print(_L("Stress test client process::E32Main - entry"));
	CTrapCleanup* TheTrapCleanup = CTrapCleanup::New();

	TRAPD(err, LaunchClientProcessL());
	if (err)
		{
		User::Panic(KStressTestChildAppPanic, err);
		}

    delete TheTrapCleanup;

	RDebug::Print(_L("Stress test client process::E32Main - exit"));
	return KErrNone;
	}

LOCAL_C void LaunchClientProcessL()
	{
	__UHEAP_MARK;
	RProcess::Rendezvous(KErrNone);

    RSemaphore sem;
    User::LeaveIfError(sem.OpenGlobal(KEglStressTest));
    CleanupClosePushL(sem);

    //Access data passed from the main process
    TStressProcessInfo info;
    TPckg<TStressProcessInfo> pckgInfo(info);
    User::LeaveIfError(User::GetDesParameter(KMultiProcessSlot, pckgInfo));

    //Create RSgDriver and open the image
    RSgDriver driver;
    User::LeaveIfError(driver.Open());
    CleanupClosePushL(driver);

    RSgImage image;
    User::LeaveIfError(image.Open(info.iSgId));
    CleanupClosePushL(image);

    EGLDisplay display;
    EGL_LEAVE_NULL(display, eglGetDisplay(EGL_DEFAULT_DISPLAY));
    EGL_LEAVE_ERROR(eglInitialize(display, NULL, NULL));
    EGL_LEAVE_ERROR(eglBindAPI(EGL_OPENVG_API));

    //Initialise to remove arm compiler warnings
    EGLConfig config = 0;
    EGLContext context = EGL_NO_CONTEXT;
    EGLSurface surface = EGL_NO_SURFACE;

    if(info.iTestType == EStressRead)
        {
        TSgImageInfo sginfo;
        User::LeaveIfError(image.GetInfo(sginfo));

        //Create an independant pixmap surface on which to copy the vgimage 
        RSgImage image2;
        User::LeaveIfError(image2.Create(sginfo, NULL, NULL));
        CleanupClosePushL(image2);
        ChooseConfigAndCreateContextL(display, context, config, image2, KStressTestChildAppPanic, info.iAlphaPre);
        EGL_LEAVE_NULL(surface, CreatePixmapSurfaceL(display, config, image2, info.iAlphaPre)); 
        CleanupStack::PopAndDestroy(&image2);
        }
    else
        {
        ChooseConfigAndCreateContextL(display, context, config, image, KStressTestChildAppPanic, info.iAlphaPre);
        EGL_LEAVE_NULL(surface, CreatePixmapSurfaceL(display, config, image, info.iAlphaPre));       
        }

    EGL_LEAVE_ERROR(eglMakeCurrent(display, surface, surface, context));

    VGImage vgImage;
    GenerateVgImageL(display, &image, vgImage);

    /* Create and install the active scheduler */
    CActiveScheduler* sched = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(sched);
    CleanupStack::PushL(sched);

    TInt width = vgGetParameteri(vgImage, VG_IMAGE_WIDTH);
    VgLeaveIfErrorL();

    TInt height = vgGetParameteri(vgImage, VG_IMAGE_HEIGHT);
    VgLeaveIfErrorL();

    VGImageFormat format = static_cast<VGImageFormat>(vgGetParameteri(vgImage, VG_IMAGE_FORMAT));
    VgLeaveIfErrorL();

    TBool testPass = ETrue;

    CTReadWriteChild* painter = CTReadWriteChild::NewL(vgImage, width, height, info.iByteSize, format, info.iTestType, testPass);
    CleanupStack::PushL(painter);
    painter->After(TTimeIntervalMicroSeconds32(0));

    //Data access is synchronised from the main process
    sem.Wait();
    sched->Start();

    if(testPass == EFalse)
        {
		// Leave with a 'known' test error so that we can catch this particular failure 
        User::Leave(KTestStressUnexpectedPixelError);
        }
    
    CleanupStack::PopAndDestroy(5, &sem); //painter, sched, image, driver, sem
      
    __UHEAP_MARKEND;
    }

/**
 *    class CTReadWriteChild
 *    A Child of CTReadWrite which contains member data not included in the base class and 
 *    implementations of pure virtual functions.
 *    a) One for each particular test case
 *    b) Support functions MakeCurrentL() and IsFinished()
 *    
 *    The base class is an active object and the implemented virtual functions are invoked
 *    indirectly from the RunL() function
 */
CTReadWriteChild::CTReadWriteChild(VGImage aImage, TInt aWidth, TInt aHeight, TInt aByteSize, VGImageFormat aFormat, const TTestType& aTestType, TBool& aTestPass)
: CTReadWrite(aWidth, aHeight, aByteSize, aFormat, aTestType, aTestPass),
    iImage(aImage)
    {
    }

CTReadWriteChild* CTReadWriteChild::NewL(VGImage aImage, TInt aWidth, TInt aHeight, TInt aByteSize, VGImageFormat aFormat, const TTestType& aTestType, TBool& aTestPass)
    {
    CTReadWriteChild* self = new (ELeave) CTReadWriteChild(aImage, aWidth, aHeight, aByteSize, aFormat, aTestType, aTestPass);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CTReadWriteChild::MakeCurrentL() const
    {
    }

void CTReadWriteChild::ReadImageFuncL()
    {
    vgGetImageSubData(iImage, iData, iWidth*iByteSize, iFormat, 0, 0, iWidth, iHeight);
    VgLeaveIfErrorL();
    }

void CTReadWriteChild::ReadFuncL()
    {
    vgDrawImage(iImage);
    vgReadPixels(iData, iWidth*iByteSize, iFormat, 0, 0, iWidth, iHeight);
    VgLeaveIfErrorL();

    //Small delay to aid pixel access sharing between processes
    User::After(TTimeIntervalMicroSeconds32(1000));
    }

void CTReadWriteChild::WriteImageFuncL()
    {
    vgImageSubData(iImage, iData, iWidth*iByteSize, iFormat, 0, 0, iWidth, iHeight);
    VgLeaveIfErrorL();
    }

TBool CTReadWriteChild::IsFinished()
    {
    return ETrue;
    }

void CTReadWriteChild::VgImageFuncL()
    {
    vgDrawImage(iImage);
    VgLeaveIfErrorL();
    }

void CTReadWriteChild::PixmapSurfaceFuncL()
    {
    vgDrawImage(iImage);
    VgLeaveIfErrorL();
    }
