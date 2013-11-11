// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Internal Symbian test code
*/

#include "egltest_endpointimage.h"
#include "egltest_surface.h"
#include "egltest_endpoint_util.h"

CEglTest_LocalTestStep_EndpointImageLifetimeProcess::CEglTest_LocalTestStep_EndpointImageLifetimeProcess()
    {
    TRAPD(err, TEndpointUtil::SetLoggerForProcessWrapperL(Logger()));
    if (err != KErrNone)
        {
        RDebug::Printf("Logging is not set up... Err = %d. Panicking...", err);
        ENGINE_ASSERT(0);
        }
    }


//function used for creating the queues.
TVerdict CEglTest_LocalTestStep_EndpointImageLifetimeProcess::doTestStepPreambleL()
    {
    //Open the queues.
    User::LeaveIfError(iResultOutQueue.OpenGlobal(KResultProcessQueueName));
    User::LeaveIfError(iParamsInQueue.OpenGlobal(KParamsProcessQueueName));
    SetTestStepResult(EPass);
    return EPass;
    }

TVerdict CEglTest_LocalTestStep_EndpointImageLifetimeProcess::doTestStepPostambleL()
    {
    iResultOutQueue.Close();
    iParamsInQueue.Close();
    return EPass;
    }

CEglTest_LocalTestStep_EndpointImageLifetimeProcess::~CEglTest_LocalTestStep_EndpointImageLifetimeProcess()
    {
    //closing an already closed handle is harmless
    iResultOutQueue.Close();
    iParamsInQueue.Close();
    }


TVerdict CEglTest_LocalTestStep_EndpointImageLifetimeProcess::doTestStepL()
    {
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (!eglInitialize(dpy, NULL, NULL))
        {
        INFO_PRINTF2(_L("EglInitialize failed, error %x"), eglGetError());
        User::Leave(KErrNotFound);
        }

    TInt surfType;
    iResultOutQueue.ReceiveBlocking(surfType);

    CSurface *surface = CSurface::SurfaceFactoryL(static_cast<TSurfaceType>(surfType));
    CleanupStack::PushL(surface);

    surface->CreateL(EStandardSurface);
    INFO_PRINTF2(_L("Using surface type %s"), surface->GetSurfaceTypeStr());
    TSurfaceId id = surface->SurfaceId();
    iParamsInQueue.SendBlocking(id);

    TInt result;
    iResultOutQueue.ReceiveBlocking(result);

    // Draw something.
    surface->DrawContentL(0);
    User::LeaveIfError(surface->SubmitContent(ETrue));

    if (result)
        {
        User::Panic(_L("Expected Panic"), 1);
        }

    eglTerminate(dpy);
    eglReleaseThread();
    CleanupStack::PopAndDestroy(surface);
    return EPass;
    }


// static
void CEglTest_LocalTestStep_EndpointImageLifetimeProcess::MainL()
    {
#ifdef __WINS__
    // Construct and destroy a process-wide state object in emulator builds.
    // This will cause initialisation of PLS for EGL and SgDriver
    // and allow checking for leaks in tests
    eglReleaseThread();
#endif //__WINS__

    // Create test step and perform CTestStep style initialisation (e.g. logging)
    CEglTest_LocalTestStep_EndpointImageLifetimeProcess* testStep = new CEglTest_LocalTestStep_EndpointImageLifetimeProcess();
    if (testStep == NULL)
        {
        User::Leave(KErrNotFound);
        }
    CleanupStack::PushL(testStep);

    // perform CTestStep pre-amble
    User::LeaveIfError(testStep->doTestStepPreambleL());

    testStep->doTestStepL();
    // perform CTestStep post-amble
    User::LeaveIfError(testStep->doTestStepPostambleL());

    //clean-up
    CleanupStack::PopAndDestroy(testStep);
    }


GLDEF_C TInt E32Main()
    {
    //When EGL Logging is enabled this causes a file server session to be allocated
    //Which needs to be done before any allocation checks otherwise the test will fail
    eglReleaseThread();

    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if(cleanup == NULL)
        {
        return KErrNoMemory;
        }

    TRAPD(err, CEglTest_LocalTestStep_EndpointImageLifetimeProcess::MainL());

    delete cleanup;
    __UHEAP_MARKEND;

    return err;
    }


