/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "egltest_endpointlocking.h"

/**
@SYMTestCaseID GRAPHICS-EGL-057x

@SYMTestPriority 2

@SYMPREQ 2670

@SYMREQ

@SYMTestCaseDesc
Testing if the implementation exhibits correct locking behaviour in multiple buffer surface cases.

@SYMTestActions
Create a number of surfaces
For each test case in a loop
    *   start the test in the remote windows server side to create endpoints
    *   in the client side call a surface update on any surface which should have the ready
        attribute set
    *   Send commands with parameters to the windows server process test render stage to
        begin/end streaming on an end point. These parameters include any error codes which
        is expected.
    *   Log any failures
If the procedure was successful release all the resources and check the heap for memory leaks.
No memory or handle leaks.

@SYMTestExpectedResults
If the function fails, the return value is undefined and an error is generated.
If endpoint is not a valid EGLEndpointNOK object, the error EGL_BAD_ENDPOINT_NOK is
generated for both BeginStreaming and EndStreaming.
If there is a timeout expected from a wait function then KErrTimedOut is received instead
of a valid notification.
*/

static const TTestCase KLockingMultibufferCases[] =
{
    {
        TESTIDS("611"),
        CASETITLE("Functional Locking: Update Server Notifications with acquire and release"),
        SurfaceTypes1(ESurfTypeRaw),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EBeginStreamingCase,  0,              EGL_SUCCESS },
        { ESetAttribCase,       0,              EGL_SUCCESS,  0, 0, EGL_DELAY_NOK, 50000 }, //50ms delay
        { ENotifyWhenCase,      0,              KErrNone,     0, ENotifyWhenAvailable },
        { EContentUpdateCase,   ENoWait,        EGL_SUCCESS },
        { EAcquireImageCase,    0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { EWaitForCase,         0,              KErrNone,     0, ENotifyWhenAvailable, 100000, 50000 }, //100 ms timeout, timestamp not used
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("577"),
        CASETITLE("Functional Locking: Multiple Buffer: DestroyEndpoint results in EndStreaming if in streaming mode."),
        SurfaceTypes1(ESurfTypeRaw),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EBeginStreamingCase,  0,              EGL_SUCCESS },
        { EContentUpdateCase,   ENoWait,        EGL_SUCCESS },
        { EAcquireImageCase,    0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { EGetAttribCase,       EExpectError,   EGL_BAD_ENDPOINT_NOK, 0, 0, EGL_ENDPOINT_READY_NOK, EGL_FALSE},
        { EEndStreamingCase,    EExpectError,   EGL_BAD_ENDPOINT_NOK },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("578"),
        CASETITLE("Functional Locking: Multiple Buffers: Destroy Endpoint results in ReleaseImage"),
        SurfaceTypes1(ESurfTypeRaw),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EBeginStreamingCase,  0,              EGL_SUCCESS },
        { EContentUpdateCase,   ENoWait,        EGL_SUCCESS },
        { EAcquireImageCase,    0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ECreateVgImageCase,   EExpectError,   EFalse},
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};


static const TTestCases KTestCases[] =
{
    TestCase(KLockingMultibufferCases),
};

const TInt KNumberTestCases = sizeof(KTestCases) / sizeof(KTestCases[0]);

CEglTest_LocalTestStep_EndpointLocking::CEglTest_LocalTestStep_EndpointLocking():
    CEgltest_Local_Engine(KTestCases, KNumberTestCases)
    {
    }
