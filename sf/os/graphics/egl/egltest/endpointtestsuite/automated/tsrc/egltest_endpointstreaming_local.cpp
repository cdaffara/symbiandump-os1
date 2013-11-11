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


/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "egltest_endpointstreaming.h"


/**
@SYMTestCaseID GRAPHICS-EGL-04xx

@SYMTestPriority 1

@SYMPREQ 2670

@SYMREQ

@SYMTestCaseDesc
Test the behaviour of eglEndpointBeginStreamingNOK and eglEndpointEndStreamingNOK with and
without bad parameters.

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
If dpy is not the handle of a valid EGLDisplay object, the error EGL_BAD_DISPLAY is
generated for both BeginSreaming and EndStraming
If endpoint is not a valid EGLEndpointNOK object, the error EGL_BAD_ENDPOINT_NOK is
generated for both BeginStreaming and EndStraming.
If the endpoint is not ready,EGL_BAD_ACCESS is generated for calling BeginStreaming.
*/


static const TTestCase KStreamingPositiveCases[] =
{
    {
        TESTIDS("545A, 546A"),
        CASETITLE("Functional: Positive: eglEndpointBeginStreamingNOK & eglEndpointEndStreamingNOK with Valid Parameters: Begin and Endstreaming only"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EBeginStreamingCase,  0,              EGL_SUCCESS },
        { EEndStreamingCase,    0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("545B, 546B"),
        CASETITLE("Functional: Positive: eglEndpointBeginStreamingNOK & eglEndpointEndStreamingNOK with Valid Parameters: Begin, twice, then Endstreaming"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EBeginStreamingCase,  0,              EGL_SUCCESS },
        { EBeginStreamingCase,  0,              EGL_SUCCESS },
        { EEndStreamingCase,    0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("545C, 546C"),
        CASETITLE("Functional: Positive: eglEndpointBeginStreamingNOK & eglEndpointEndStreamingNOK with Valid Parameters: Begin then Endstreaming twice"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EBeginStreamingCase,  0,              EGL_SUCCESS },
        { EEndStreamingCase,    0,              EGL_SUCCESS },
        { EEndStreamingCase,    0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    // Now check that we can and can't acquire images when in or out of a streaming block.
    {
        TESTIDS("545D, 546D"),
        CASETITLE("Functional: Positive: eglEndpointBeginStreamingNOK & eglEndpointEndStreamingNOK with Valid Parameters: Beginstreaming, acquire, endstreaming"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EBeginStreamingCase,  0,              EGL_SUCCESS },
        { EAcquireImageCase,    0,              EGL_SUCCESS },
        { ECompareImageCase,    0,              EGL_SUCCESS },
        { EEndStreamingCase,    0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("545E, 546E"),
        CASETITLE("Functional: Positive: eglEndpointBeginStreamingNOK & eglEndpointEndStreamingNOK with Valid Parameters: Beginstreaming, endstreaming, acquire"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EBeginStreamingCase,  0,              EGL_SUCCESS },
        { EEndStreamingCase,    0,              EGL_SUCCESS },
        { EAcquireImageCase,    EExpectError,   EGL_BAD_ACCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("545F, 546F"),
        CASETITLE("Functional: Positive: eglEndpointBeginStreamingNOK & eglEndpointEndStreamingNOK with Valid Parameters: Beginstreaming twice, endstreaming, acquire"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EBeginStreamingCase,  0,              EGL_SUCCESS },
        { EBeginStreamingCase,  0,              EGL_SUCCESS },
        { EEndStreamingCase,    0,              EGL_SUCCESS },
        { EAcquireImageCase,    EExpectError,   EGL_BAD_ACCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("545G, 546G"),
        CASETITLE("Functional: Positive: eglEndpointBeginStreamingNOK & eglEndpointEndStreamingNOK with Valid Parameters: Beginstreaming, endstreaming twice, beginstreaming, acquire"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EBeginStreamingCase,  0,              EGL_SUCCESS },
        { EEndStreamingCase,    0,              EGL_SUCCESS },
        { EEndStreamingCase,    0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EBeginStreamingCase,  0,              EGL_SUCCESS },
        { EAcquireImageCase,    0,              EGL_SUCCESS },
        { ECompareImageCase,    0,              EGL_SUCCESS },
        { EEndStreamingCase,    0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("545H, 546H"),
        CASETITLE("Functional: Positive: eglEndpointBeginStreamingNOK & eglEndpointEndStreamingNOK with Valid Parameters: Beginstreaming, destroy, create, acquire (not ready)"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EBeginStreamingCase,  0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        // We should get bad access, as the endpoint is not ready!
        { EAcquireImageCase,    EExpectError,   EGL_BAD_ACCESS },
        { EEndStreamingCase,    0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("545I, 546I"),
        CASETITLE("Functional: Positive: eglEndpointBeginStreamingNOK & eglEndpointEndStreamingNOK with Valid Parameters: Beginstreaming, destroy, create, acquire (ready)"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EBeginStreamingCase,  0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        // We should get bad access, as the endpoint is not in a streaming block!
        { EAcquireImageCase,    EExpectError,   EGL_BAD_ACCESS },
        { EEndStreamingCase,    0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};

static const TTestCase KStreamingBadParameterCases[] =
{
    {
        TESTIDS("553A"),
        CASETITLE("Functional: Negative: eglEndpointBeginStreamingNOK with Bad Parameters: Test eglEndpointBeginStreaming bad parameter variations"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        // Bad display on it's own.
        { EBeginStreamingCase,  EUseBadDisplay, EGL_BAD_DISPLAY },
        // Bad display should be priority over the bad endpoint
        { EBeginStreamingCase,  EUseBadDisplay | EUseBadEndpoint, EGL_BAD_DISPLAY },
        // Bad endpoint should result in "BAD_ENDPOINT" error.
        { EBeginStreamingCase,  EUseBadEndpoint, EGL_BAD_ENDPOINT_NOK },
        { EEndStreamingCase,    0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("554A"),
        CASETITLE("Functional: Negative: eglEndpointEndStreamingNOK with Bad Parameters: Test eglEndpointEndStreaming bad parameter variations"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        // Bad display on it's own.
        { EEndStreamingCase,    EUseBadDisplay, EGL_BAD_DISPLAY },
        // Bad display should be priority over the bad endpoint
        { EEndStreamingCase,    EUseBadDisplay | EUseBadEndpoint, EGL_BAD_DISPLAY },
        { EEndStreamingCase,    EUseBadEndpoint, EGL_BAD_ENDPOINT_NOK },
        { EEndStreamingCase,    0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};

static const TTestCase KStreamingNegativeCases[] =
{
    {
        TESTIDS("553B"),
        CASETITLE("Functional: Negative: eglEndpointBeginStreamingNOK with Bad Parameters: eglEndpointBeginStreaming with no contentupdate"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EBeginStreamingCase,  EExpectError,   EGL_BAD_ACCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EBeginStreamingCase,  0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};

static const TTestCases KTestCases[] =
{
    TestCase(KStreamingPositiveCases),
    TestCase(KStreamingBadParameterCases),
    TestCase(KStreamingNegativeCases),
};

const TInt KNumberTestCases = sizeof(KTestCases) / sizeof(KTestCases[0]);

CEglTest_LocalTestStep_EndpointStreaming::CEglTest_LocalTestStep_EndpointStreaming():
    CEgltest_Local_Engine(KTestCases, KNumberTestCases)
    {
    }
