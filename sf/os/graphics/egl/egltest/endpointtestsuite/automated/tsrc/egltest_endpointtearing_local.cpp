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
#include "egltest_surface.h"
#include <e32atomics.h>


/**
@SYMTestCaseID GRAPHICS-EGL-573

@SYMTestPriority medium

@SYMPREQ 2670

@SYMTestCaseDesc
Image tearing test with raw surface manager surface.
Check that there is no buffer tearing and that locks are implemented correctly.

@SYMTestActions
Create a Raw Surface Manager surface.
1)  Thread B (test application): Fill the buffers with colours mostly red, and 
    mostly blue, and mostly green if triple buffered.  This is done so that 
    there is a value greater than zero in all colour channels.
2)  Thread A (wserve): Create an endpoint from the surface belonging to the window
3)  Thread B: Submit update.
4)  Thread A: Begin streaming block
5)  Thread A in the windows server for ten seconds, as fast as possible:
6)  Thread A: Acquire an EGLImage from this endpoint
7)  Thread A: Check that the resulting image all a solid colour.  Five samples, 
    one at the centre and at the four corners should be sufficient.
8)  Thread A: Release the EGLImage
9)  In thread B, whilst thread A is running: fill surface with different solid 
    colours, and call Submit Update. Run this thread as fast a possible, with the 
    same priority as thread A. The solid colour used should be varied for each 
    iteration.  Do not synchronize threads A and B, but random waits (using delay 
    loops) may be used to give each thread a variation and reduce any 
    synchronization in the EGL code.
10) Thread A: End the streaming block
11) Thread A & B: Close and delete resources.

@SYMTestExpectedResults
Each acquired image from the endpoint is a solid colour, 
calculated by comparing all the corners and the center pixel.
*/


CEglTest_LocalTestStep_EndpointTearing::CEglTest_LocalTestStep_EndpointTearing() :
    CLocalTestStepBase(ETestUidEndpointTearing)
    {
    }


CEglTest_LocalTestStep_EndpointTearing::~CEglTest_LocalTestStep_EndpointTearing()
    {
    }

void CEglTest_LocalTestStep_EndpointTearing::DoPreambleL()
    {
    //Register the test id.
    _LIT(KTestId, "573");
    RegisterTestIdsL(KTestId);
    SetCurrentTestIds(KTestId);
    
    //If this fails, the test will fail in a round about way with EGL_NOT_INITIALIZED.
    eglInitialize(eglGetDisplay(EGL_DEFAULT_DISPLAY), NULL, NULL);
    
    //Create the fast locaks used for comms between the test thread and the draw thread.
    User::LeaveIfError(iStartDraw.CreateLocal());
    User::LeaveIfError(iFirstDrawDone.CreateLocal());
    }


void CEglTest_LocalTestStep_EndpointTearing::DoPostambleL()
    {
    iStartDraw.Close();
    iFirstDrawDone.Close();
    eglTerminate(eglGetDisplay(EGL_DEFAULT_DISPLAY));
    }


TVerdict CEglTest_LocalTestStep_EndpointTearing::doTestStepL()
    {
    //Acquire the start drawing lock to force the draw thread to wait until we signal.
    iStartDraw.Wait();
    
    //Start drawing thread. When this returns, iSurfaceId
    //contains the surface handle for the surface the thread uses
    //and the draw thread is waiting on iStartDraw.
    StartDrawThreadL();
    
    //Params for the remote test step.
    TRemoteTestParams params;
    params.iEndpointTearing.iSurfaceId = iSurfaceId;
    
    //Setup the remote test step. (creates an endpoint).
    StartRemoteTestStep(params);
    
    //Tell the draw thread to continously fill the surface 
    //with a block colour and submit the buffer, honoring the
    //notifyWhenAvailable semantics. 
    iStartDraw.Signal();
    
    //Wait for the first buffer to be drawn, so that the remote side
    //is guaranteed to be able to begin streaming on the endpoint
    //when we run the remote test case. The remote side continuously
    //checks that each acquired image is a solid colour for a set time.
    iFirstDrawDone.Wait();
    RunRemoteTestCase(0, TRemoteTestParams());
    
    //Stop the drawing thread, so that it does not report a 'not 
    //registered' error when we end the remote test step (destroy ep).
    StopDrawThread();
    EndRemoteTestStep(TRemoteTestParams());
    
    return TestStepResult();
    }


void CEglTest_LocalTestStep_EndpointTearing::StartDrawThreadL()
    {
    //Stack and heap sizes.
    static const TInt KStackSize =   0x2000;      //  8KB
    static const TInt KHeapMinSize = 0x1000;      //  4KB
    static const TInt KHeapMaxSize = 0x1000000;   // 16MB

    //Create draw thread and wait for rendezvous.
    User::LeaveIfError(iDrawThread.Create(KNullDesC, DrawThreadEntryPoint, KStackSize, KHeapMinSize, KHeapMaxSize, this));
    CleanupClosePushL(iDrawThread);
    iDrawThread.Resume();
    TRequestStatus rendezvous;
    iDrawThread.Rendezvous(rendezvous);
    User::WaitForRequest(rendezvous);
    User::LeaveIfError(rendezvous.Int());
    CleanupStack::Pop(&iDrawThread);
    }


void CEglTest_LocalTestStep_EndpointTearing::StopDrawThread()
    {
    TRequestStatus logon;
    iDrawThread.Logon(logon);
    __e32_atomic_store_rel32(&iStopDrawing, ETrue);
    User::WaitForRequest(logon);
    iDrawThread.Close();
    }


TInt CEglTest_LocalTestStep_EndpointTearing::DrawThreadEntryPoint(TAny* aSelf)
    {
    CEglTest_LocalTestStep_EndpointTearing* self = static_cast<CEglTest_LocalTestStep_EndpointTearing*>(aSelf);
    CTrapCleanup* cleanup = CTrapCleanup::New();
    
    TRAPD(err,
        //Create active scheduler.
        CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
        CleanupStack::PushL(scheduler);
        CActiveScheduler::Install(scheduler);

        //Setup the draw loop.
        self->EnterDrawLoopL();

        //Clean up.
        CleanupStack::PopAndDestroy(scheduler);
        );
    
    __ASSERT_ALWAYS(err == KErrNone, User::PanicUnexpectedLeave());
    delete cleanup;
    
    return KErrNone;
    }


void CEglTest_LocalTestStep_EndpointTearing::EnterDrawLoopL()
    {
    //Create a standard surface. 
    CSurface* surface = CSurface::SurfaceFactoryL(ESurfTypeRaw);
    CleanupStack::PushL(surface);
    surface->CreateL(ELargeSurface);
    
    TRequestStatus availableStat[2];
    TInt currentStat = 0;
    TInt currentColour = 0x88CC44;
    
    //Take the FirstDrawDone fast lock, save the surfaceId then rendezvous.
    iFirstDrawDone.Wait();
    iSurfaceId = surface->SurfaceId();
    RThread().Rendezvous(KErrNone);
    
    //Wait for the signal to draw the first buffer.
    iStartDraw.Wait();
    surface->DrawContentL(TRgb(currentColour, 255));
    surface->Notify(ENotifyWhenAvailable, availableStat[currentStat++ & 1], 0);
    // We must wait until the image is copied; the endpoint isn't ready until the ContentUpdated has been processed by GCE BE.
    // It isn't enough to simply call SubmitContentL; there's multiple process/thread boundaries to cross before the endpoint is 'ready'
    User::LeaveIfError(surface->SubmitContent(ETrue));
    iFirstDrawDone.Signal();
    
    //Loop until we are told to stop drawing.
    while(!__e32_atomic_load_acq32(&iStopDrawing))
        {
        //Draw content, ask for notification when the buffer we have drawn to is available and submit.
        surface->DrawContentL(TRgb(currentColour, 255));
        surface->Notify(ENotifyWhenAvailable, availableStat[currentStat++ & 1], 0);
        User::LeaveIfError(surface->SubmitContent(EFalse));
        
        //Wait until the other buffer is free, so we don't tear. There are 2 buffers 
        //in the surface: We asked for ELargeSurface, which is double buffered.
        TUint32 dummyTimeStamp;
        TInt err = surface->WaitFor(ENotifyWhenAvailable, availableStat[currentStat & 1], 1000, dummyTimeStamp);
        User::LeaveIfError(err);
        
        //Modify the colour that we draw.
        currentColour += 16;
        }
    
    CleanupStack::PopAndDestroy(surface);
    }
