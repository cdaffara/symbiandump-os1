/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "egltest_endpointtearing.h"
#include "egltest_endpoint_images.h"


CEglTest_RemoteTestStep_EndpointTearing::CEglTest_RemoteTestStep_EndpointTearing() :
    CRemoteTestStepBase(ETestUidEndpointTearing)
    {
    }


CEglTest_RemoteTestStep_EndpointTearing::~CEglTest_RemoteTestStep_EndpointTearing()
    {
    }


TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointTearing::DoStartRemoteTestStepL(const TRemoteTestParams& aMessageIn)
    {
    REMOTE_INFO_PRINTF1(_L("Starting Remote Test Step."));
    
    EglStartL();
    iDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    
    TSurfaceId surface = aMessageIn.iEndpointTearing.iSurfaceId; 
    iEndpoint = EglEndpoint().CreateEndpoint(iDisplay, EGL_ENDPOINT_TYPE_CONSUMER_NOK, EGL_TSURFACEID_NOK, &surface, NULL);
    if(iEndpoint == EGL_NO_ENDPOINT_NOK)
        {
        REMOTE_ERR_PRINTF2(_L("Failed to create endpoint. err = %d"), eglGetError());
        User::Leave(KErrBadHandle);
        }
    REMOTE_INFO_PRINTF1(_L("Endpoint created."));
    
    return ERtvPass;
    }


TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointTearing::DoEndRemoteTestStepL(const TRemoteTestParams& /*aMessageIn*/)
    {
    REMOTE_INFO_PRINTF1(_L("Ending Remote Test Step."));
    EglEndpoint().DestroyEndpoint(iDisplay, iEndpoint);
    EglEndL();
    return ERtvPass;
    }


TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointTearing::DoRunRemoteTestCaseL(TInt aTestCase, const TRemoteTestParams& aParams)
    {
    switch(aTestCase)
        {
        case 0:     return TearingTestCaseL(aParams);
        default:    return ERtvAbort;
        }
    }


TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointTearing::TearingTestCaseL(const TRemoteTestParams& /*aParams*/)
    {
    //Begin streaming.
    EGLBoolean result = EglEndpoint().EndpointBeginStreaming(iDisplay, iEndpoint);
    if(result == EGL_FALSE)
        {
        REMOTE_ERR_PRINTF2(_L("Failed to enter streaming block. err = %d"), eglGetError());
        User::Leave(KErrBadHandle);
        }
    
    //Create an EglWindowSurface so we have a current context for vg operations.
    CEglWindowSurface* surface = CEglWindowSurface::NewL();
    CleanupStack::PushL(surface);
    surface->CreateL(EStandardSurface, TPoint(0, 0));
    surface->ActivateL();
    
    REMOTE_INFO_PRINTF1(_L("Repeatedly acquiring images and checking for evidence of tearing."));
    
    //The test should run for 7 seconds so we use a timer 
    //and loop while the request status is KRequestPending.
    TRequestStatus notifyTimeDone;
    RTimer timer;
    User::LeaveIfError(timer.CreateLocal());
    CleanupClosePushL(timer);
    timer.After(notifyTimeDone, 7 * 1000000);
    
    //Acquire and check that image is solid colour in a loop while the timer has not fired.
    TRemoteTestVerdict retVal = ERtvPass;
    TInt numIters = 0;
    while(notifyTimeDone.Int() == KRequestPending)
        {
        TRAPD(err, retVal = AcquireAndCheckImageL());
        if(err != KErrNone)
            {
            //Cancel the timer, wait for the notification and re-leave.
            timer.Cancel();
            User::WaitForRequest(notifyTimeDone);
            User::Leave(err);
            }
        if(retVal != ERtvPass)
            {
            REMOTE_ERR_PRINTF2(_L("Detected tearing in aquired image number %d"), numIters);
            timer.Cancel();
            break;
            }
        ++numIters;
        }
    
    //Tidy up the thread semaphore.
    User::WaitForRequest(notifyTimeDone);
    
    REMOTE_ERR_PRINTF2(_L("Number of successful iterations: %d"), numIters);
    
    CleanupStack::PopAndDestroy(2, surface);
    EglEndpoint().EndpointEndStreaming(iDisplay, iEndpoint);
    return retVal;
    }


TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointTearing::AcquireAndCheckImageL()
    {
    //Acquire an image from the endpoint.
    EGLImageKHR eglImage = EglEndpoint().AcquireImage(iDisplay, iEndpoint);
    if(eglImage == EGL_NO_IMAGE_KHR)
        {
        REMOTE_ERR_PRINTF2(_L("Failed to acquire image from endpoint. err = %d"), eglGetError());
        User::Leave(KErrBadHandle);
        }
    
    //Check the image. This bit can leave, so we trap it so we 
    //have an opportunity to release the image before re-leaving.
    TRemoteTestVerdict retVal = ERtvPass;
    TRAPD(err, retVal = CheckImageL(eglImage));
    
    //Release the image back to the endpoint.
    EGLBoolean err2 = EglEndpoint().ReleaseImage(iDisplay, iEndpoint, eglImage, EGL_OPENVG_API);
    
    //Report the 1st error.
    User::LeaveIfError(err);
    if(err2 == EGL_FALSE)
        {
        REMOTE_ERR_PRINTF2(_L("Failed to release image back to endpoint. err = %d"), eglGetError());
        User::Leave(KErrBadHandle);
        }
    
    return retVal;
    }


TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointTearing::CheckImageL(EGLImageKHR aEglImage)
    {
    //Convert the image to a CTestVgEglImage
    CTestVgEglImage* vgEglImage = CTestVgEglImage::NewL(aEglImage);
    CleanupStack::PushL(vgEglImage);
    
    //Check the corners and center pixel are the same colour. 
    TBool retVal = vgEglImage->IsSolidColourL();
    
    CleanupStack::PopAndDestroy(vgEglImage);
    return retVal ? ERtvPass : ERtvFail;
    }

