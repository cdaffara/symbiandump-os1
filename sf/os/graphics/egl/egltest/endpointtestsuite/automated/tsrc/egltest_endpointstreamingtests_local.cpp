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


#include "egltest_endpointstreamingtests.h"
#include "egltest_surface.h"


static const TTestCase KStreamingTestCases[] =
{
    {
        TESTIDS("582"),
        CASETITLE("Functional: Streaming: aStatusDisplayed behaves properly within streaming block"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { ESetAttribCase,       0, EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 50000 },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { ETimeStampCase,       0, KErrNone },
        { ENotifyWhenCase,      0, KErrNone, 0, ENotifyWhenDisplayed },
        { EContentUpdateCase,   ENoWait, EGL_SUCCESS },
        { EWaitForCase,         0, KErrNone, 0, ENotifyWhenDisplayed, 55000, 50000 },
        { ESetAttribCase,       0, EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 100000 },
        { ETimeStampCase,       0, KErrNone },
        { ENotifyWhenCase,      0, KErrNone, 0, ENotifyWhenDisplayed },
        { EContentUpdateCase,   ENoWait, EGL_SUCCESS },
        { EWaitForCase,         0, KErrNone, 0, ENotifyWhenDisplayed, 110000, 100000 },
        { EEndStreamingCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("584"),
        CASETITLE("Functional: Streaming: aStatusDisplayed behaves properly outside of streaming block"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { ESetAttribCase,       0, EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 50000 },
        { ENotifyWhenCase,      0, KErrNone, 0, ENotifyWhenDisplayed },
        { EContentUpdateCase,   ENoWait, EGL_SUCCESS },
        { EWaitForCase,         0, KErrNotVisible, 0, ENotifyWhenDisplayed, 55000 },
        { ENotifyWhenCase,      0, KErrNone, 0, ENotifyWhenDisplayed },
        { EContentUpdateCase,   ENoWait, EGL_SUCCESS },
        { EWaitForCase,         0, KErrNotVisible, 0, ENotifyWhenDisplayed, 5000 },
        { EEndStreamingCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("585"),
        CASETITLE("Functional: Streaming: aStatusDisplayedXTimes behaves properly outside streaming block"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { ESetAttribCase,       0, EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 50000 },
        { ENotifyWhenCase,      0, KErrNone, 0, ENotifyWhenDispXTimes, 2 },
        { EContentUpdateCase,   ENoWait, EGL_SUCCESS },
        { EWaitForCase,         0, KErrNotVisible, 0, ENotifyWhenDispXTimes, 5000 },
        { ENotifyWhenCase,      0, KErrNone, 0, ENotifyWhenDispXTimes, 10 },
        { EContentUpdateCase,   ENoWait, EGL_SUCCESS },
        { EWaitForCase,         0, KErrNotVisible, 0, ENotifyWhenDispXTimes, 5000 },
        { EEndStreamingCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("586"),
        CASETITLE("Functional: Streaming: Additional SubmitUpdate issued before aStatusDisplayed is notified results in error KErrOverflow"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { ESetAttribCase,       0, EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 20000 },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { ENotifyWhenCase,      0, KErrNone, 0, ENotifyWhenDisplayed },
        { EContentUpdateCase,   ENoWait, EGL_SUCCESS },
        { EContentUpdateCase,   ENoWait, EGL_SUCCESS },
        { EWaitForCase,         0, KErrOverflow, 0, ENotifyWhenDisplayed, 10000 },
        { EEndStreamingCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("587A"),
        CASETITLE("Functional: Streaming: aStatusDisplayed behaves properly on transition out of streaming block (variant 1)"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { ESetAttribCase,       0, EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 20000 },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { ENotifyWhenCase,      0, KErrNone, 0, ENotifyWhenDisplayed },
        { EContentUpdateCase,   ENoWait, EGL_SUCCESS },
        { EWaitForCase,         0, KErrNone, 0, ENotifyWhenDisplayed, 25000 },
        { EEndStreamingCase,    0, EGL_SUCCESS },
        { ENotifyWhenCase,      0, KErrNone, 0, ENotifyWhenDisplayed },
        { EContentUpdateCase,   ENoWait, EGL_SUCCESS },
        { EWaitForCase,         0, KErrNotVisible, 0, ENotifyWhenDisplayed, 25000 },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("587B"),
        CASETITLE("Functional: Streaming: aStatusDisplayed behaves properly on transition out of streaming block (variant 2)"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { ESetAttribCase,       0, EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 20000 },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { ENotifyWhenCase,      0, KErrNone, 0, ENotifyWhenDisplayed },
        { EContentUpdateCase,   ENoWait, EGL_SUCCESS },
        { EWaitForCase,         0, KErrNone, 0, ENotifyWhenDisplayed, 25000 },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ENotifyWhenCase,      0, KErrNone, 0, ENotifyWhenDisplayed },
        { EContentUpdateCase,   ENoWait, EGL_SUCCESS },
        { EWaitForCase,         0, KErrSurfaceNotRegistered, 0, ENotifyWhenDisplayed, 25000 },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};


static const TTestCases KTestCases[] =
{
    TestCase(KStreamingTestCases),
};

const TInt KNumberTestCases = sizeof(KTestCases) / sizeof(KTestCases[0]);


CEglTest_LocalTestStep_EndpointStreamingTests::CEglTest_LocalTestStep_EndpointStreamingTests():
    CEgltest_Local_Engine(KTestCases, KNumberTestCases)
    {
    }


//------------------------------------------------------
// GRAPHICS-EGL-583 can only be implemented with code...
//------------------------------------------------------


CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream::CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream()
: CLocalTestStepBase(ETestUidEndpointEngine)  // We use the engine to do the remote side functionality.
    {
    }

void CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream::DoPreambleL()
    {
    }

void CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream::DoPostambleL()
    {
    }

CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream::~CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream()
    {
    }

void CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream::RunScript(const TEngineTestCase *aTestCases, TRemoteTestParams &params)
    {
    for(TInt i = 0; aTestCases[i].iCase != EFinishedCase; i++)
        {
        params.iEndpointEngine.iEngineTestCase = aTestCases[i];
        RunRemoteTestCase(aTestCases[i].iCase, params);
        }
    }

TUint CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream::TicksForDisplayXTimesL(CSurface* aSurface, TInt aNumTimes)
    {
    //Calculate timeout by assuming low framerate of 10 fps, 
    //multiplying period by number of times and an extra factor of 3.
    //This should be plenty. Measured in microseconds.
    TInt timeout = 3 * aNumTimes * TInt(1000000.0f / 10.0f);
    
    TRequestStatus dispXTimesRs;
    User::LeaveIfError(aSurface->Notify(ENotifyWhenDispXTimes, dispXTimesRs, aNumTimes));
    
    TUint startTickCount = User::TickCount();
    User::LeaveIfError(aSurface->SubmitContent(EFalse));
    
    TUint32 dummyTimeStamp;
    TInt err = aSurface->WaitFor(ENotifyWhenDispXTimes, dispXTimesRs, timeout, dummyTimeStamp);
    if(err != KErrNone)
        {
        ERR_PRINTF3(_L("Waiting for ENotifyWhenDispXTimes returned %d when we expected %d."), err, KErrNone);
        User::Leave(err);
        }
    
    return User::TickCount() - startTickCount;
    }
    
void CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream::DispXTimesInStreamTestL(TSurfaceType aSurfaceType)
    {
    //Local: Create surface and save the id for the remote side.
    CSurface* surface = CSurface::SurfaceFactoryL(aSurfaceType);
    CleanupStack::PushL(surface);
    surface->CreateL(EStandardSurface);
    surface->GetSurfaceParamsL(iParams.iEndpointEngine.iSurfaceParams);
    
    //Remote: Init remote side and create an endpoint from the surface. (tidy up ops are on cleanup stack)
    static const TEngineTestCase createEndpointSteps[] = 
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EFinishedCase,        0, EGL_SUCCESS },
        };
    RunScript(createEndpointSteps, iParams);
    CleanupStack::PushL(TCleanupItem(CleanupRemoteTestStep, this));
    
    //Local: Submit update for the surface so that the endpoint enters the ready state.
    User::LeaveIfError(surface->SubmitContent(ETrue));
    
    //Remote: Set the EGL_DELAY_NOK value and begin streaming on the endpoint.
    static const TEngineTestCase setDelayAndBeginStreamingSteps[] = 
        {
        { ESetAttribCase,       0, EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 5000 },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EFinishedCase,        0, EGL_SUCCESS },
        };
    RunScript(setDelayAndBeginStreamingSteps, iParams);
    
    //Local: Get number of ticks between submitting content and X frames being displayed.
    TUint ticksForDisp20Times = TicksForDisplayXTimesL(surface, 20);
    INFO_PRINTF2(_L("ticksForDisp20Times = %d"), ticksForDisp20Times);
    TUint ticksForDisp200Times = TicksForDisplayXTimesL(surface, 200);
    INFO_PRINTF2(_L("ticksForDisp200Times = %d"), ticksForDisp200Times);
    
    //check to see if ticksForDisp200Times = 10 * ticksForDisp20Times (with some fuzz).
    if( (ticksForDisp200Times < 5*ticksForDisp20Times) || (ticksForDisp200Times > 15*ticksForDisp20Times) )
        {
        ERR_PRINTF1(_L("Ratio between time for display 20 times and display 200 times not within limits."));
        User::Leave(EFail);
        }
    else
        {
        INFO_PRINTF1(_L("Ratio between time for display 20 times and display 200 times is within limits."));
        }
    
    CleanupStack::PopAndDestroy(2, surface);
    }

TVerdict CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream::doTestStepL()
    {
    static const TRemoteTestParams nullParams = {};
    StartRemoteTestStep(nullParams);
    
    DispXTimesInStreamTestL(ESurfTypeRaw);
    DispXTimesInStreamTestL(ESurfTypeRawSingleBuffered);
    
    EndRemoteTestStep(nullParams);
    INFO_PRINTF1(_L("EndpointStreamingDispXTimesInStream test finished"));
    return TestStepResult();
    }

void CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream::CleanupRemoteTestStep(TAny* aStreamingDispXTimesInStream)
    {
    CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream* self = 
                static_cast<CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream*>(aStreamingDispXTimesInStream);
    
    //Remote: Destroy the endpoint and terminate EGL.
    static const TEngineTestCase destroyEndpointSteps[] = 
        {
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase },
        };
    self->RunScript(destroyEndpointSteps, self->iParams);
    }

