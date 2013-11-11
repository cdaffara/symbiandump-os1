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

#include "egltest_endpointcreate.h"

/**
@SYMTestCaseID GRAPHICS-EGL-0406

@SYMTestPriority 1

@SYMPREQ 2670

@SYMREQ 13245

@SYMTestCaseDesc
Test the beahviour of eglCreateEndpointNOK and eglDestroyEndpoint with good and bad parameters.

@SYMTestActions
Create a number of surfaces
For each test case in a loop
    *   Send a commands with parameters to the windows server process test render stage to
        perform operations. These parameters include any error codes which should be
        generated.  The remote side checks the error code if any, and returns the result to the
        local side engine (test application), which sets the verdict of the test .
    *   Log any failures
If the procedure was successful release all the resources and check the heap for memory leaks.
No memory or handle leaks.

@SYMTestExpectedResults
If an error is generated, the value EGL_NO_ENDPOINT_NOK is returned.
If dpy is not the handle of a valid EGLDisplay object, the error EGL_BAD_DISPLAY is generated.
If type is not EGL_ENDPOINT_TYPE_CONSUMER_NOK, error EGL_BAD_PARAMETER is generated.
If source is not a pointer to a valid TSurfaceId, the error EGL_BAD_PARAMETER is generated.
If source_type is not EGL_TSURFACEID_NOK, the error EGL_BAD_PARAMETER is generated.
If attrib_list is not NULL or empty, the error EGL_BAD_PARAMETER is generated.
*/

static const TTestCase KCreatePositiveCases[] =
{
    {
        TESTIDS("541A, 542A"),
        CASETITLE("Functional: Positive: eglCreateEndpointNOK & eglDestroyEndpointNOK with Valid Parameters: Create and destroy one endpoint"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("541B, 542B"),
        CASETITLE("Functional: Positive: eglCreateEndpointNOK & eglDestroyEndpointNOK with Valid Parameters: Create and destroy two endpoints, sequential"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 0, EStandardSurface },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("541C, 542C"),
        CASETITLE("Functional: Positive: eglCreateEndpointNOK & eglDestroyEndpointNOK with Valid Parameters: Create and destroy three endpoints, parallel, destroy in order"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 1, EStandardSurface },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 2, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 0, 0 },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 1, 1 },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 2, 2 },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS, 0, 0 },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS, 1, 1 },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS, 2, 2 },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("541D, 542D"),
        CASETITLE("Functional: Positive: eglCreateEndpointNOK & eglDestroyEndpointNOK with Valid Parameters: Create and destroy three endpoints, parallel, destroy reverse order"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 1, EStandardSurface },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 2, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 0, 0 },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 1, 1 },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 2, 2 },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS, 2, 2 },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS, 1, 1 },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS, 0, 0 },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("541E, 542E"),
        CASETITLE("Functional: Positive: eglCreateEndpointNOK & eglDestroyEndpointNOK with Valid Parameters: Create and destroy three endpoints, parallel, destroy out of order"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 1, EStandardSurface },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 2, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 0, 0 },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 1, 1 },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 2, 2 },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS, 2, 2 },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS, 0, 0 },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS, 1, 1 },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("541F"),
        CASETITLE("Functional: Positive: eglCreateEndpointNOK & eglTerminate with Valid Parameters: Create one endpoint then terminate EGL"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("541G"),
        CASETITLE("Functional: Positive: eglCreateEndpointNOK & eglTerminate with Valid Parameters: Create three endpoints then terminate EGL"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 1, EStandardSurface },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 2, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 0, 0 },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 1, 1 },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 2, 2 },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};

static const TTestCase KCreateBadParameterCases[] =
{
    {
        TESTIDS("549A"),
        CASETITLE("Functional: Negative: eglCreateEndpointNOK with Bad parameters: Create with bad display and various other bad parameters"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  EUseBadDisplay, EGL_BAD_DISPLAY },
        { ECreateEndpointCase,  EUseBadDisplay | EUseBadEndpointType, EGL_BAD_DISPLAY },
        { ECreateEndpointCase,  EUseBadDisplay | EUseBadSourceType, EGL_BAD_DISPLAY },
        { ECreateEndpointCase,  EUseBadDisplay | EUseBadSourceType | EUseBadEndpointType, EGL_BAD_DISPLAY },
        { ECreateEndpointCase,  EUseBadEndpointType, EGL_BAD_PARAMETER },
        { ECreateEndpointCase,  EUseBadSourceType, EGL_BAD_PARAMETER },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("549B"),
        CASETITLE("Functional: Negative: eglCreateEndpointNOK with Bad parameters: Create with bad attributes"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,                EGL_SUCCESS },
        { ECreateSurfaceCase,   0,                EGL_SUCCESS, 0, EBadAttribSurface },
        { ECreateEndpointCase,  EUseBadDisplay,   EGL_BAD_DISPLAY },
        { ECreateEndpointCase,  EUseBadDisplay | EUseBadEndpointType, EGL_BAD_DISPLAY },
        { ECreateEndpointCase,  EUseBadDisplay | EUseBadSourceType, EGL_BAD_DISPLAY },
        { ECreateEndpointCase,  EUseBadDisplay | EUseBadSourceType | EUseBadEndpointType, EGL_BAD_DISPLAY },
        // Now, try with correct parameters aside from the attribute set - this should give us bad parameter.
        { ECreateEndpointCase,  EExpectError, EGL_BAD_PARAMETER },
        { ETerminateCase,       0,                EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("549C"),
        CASETITLE("Functional: Negative: eglCreateEndpointNOK with Bad parameters: Create with bad surfaceid"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
            { EInitializeCase,      0,                EGL_SUCCESS },
            { ECreateSurfaceCase,   0,                EGL_SUCCESS },
            // Check that bad display takes precedence over bad surface!
            { ECreateEndpointCase,  EUseBadDisplay | EUseBadSurfaceId, EGL_BAD_DISPLAY },
            { ECreateEndpointCase,  EUseBadSurfaceId, EGL_BAD_PARAMETER },
            // Now create an endpoint to see that nothing got messed up.
            { ECreateEndpointCase,  0,                EGL_SUCCESS },
            { EDestroyEndpointCase, 0,                EGL_SUCCESS },
            { ETerminateCase,       0,                EGL_SUCCESS },
            { EFinishedCase }
        }
    },
};

// Note that some cases of "destroy" are tested as part of AcquireImage tests - they are
// borderline between the two functions, e.g. destroy when an image is acquired.
// Also, destroyendpoint is tested in the streaming tests, for the case where endstreaming
// is not performed before the destroyendpoint.
static const TTestCase KCreateNegativeCases[] =
{
    {
        TESTIDS("549D"),
        CASETITLE("Functional: Negative: eglCreateEndpointNOK with Bad parameters: check that creating an endpoint twice gives correct error"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,                EGL_SUCCESS },
        { ECreateSurfaceCase,   0,                EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,                EGL_SUCCESS },
        { ECreateEndpointCase,  EExpectError,     EGL_BAD_PARAMETER },
        { EDestroyEndpointCase, 0,                EGL_SUCCESS },
        { ETerminateCase,       0,                EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};

static const TTestCase KDestroyBadParameterCases[] =
{
    {
        TESTIDS("550A"),
        CASETITLE("Functional: Negative: eglDestroyEndpointNOK with Bad parameters: check various trivial bad parameters"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { EDestroyEndpointCase, EUseBadDisplay, EGL_BAD_DISPLAY },
        { EDestroyEndpointCase, EUseBadDisplay, EGL_BAD_DISPLAY },
        // Now use bad endpoint WITH bad display - should give bad display.
        { EDestroyEndpointCase, EUseBadDisplay | EUseBadEndpoint, EGL_BAD_DISPLAY },
        // And bad endpoint on it's own, should give bad-endpoint.
        { EDestroyEndpointCase, EUseBadEndpoint, EGL_BAD_ENDPOINT_NOK },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};

static const TTestCase KDestroyNegativeCases[] =
{
    {
        TESTIDS("550B"),
        CASETITLE("Functional: Negative: eglDestroyEndpointNOK with Bad parameters: check that destroying the endpoint twice gives correct error"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,                EGL_SUCCESS },
        { ECreateSurfaceCase,   0,                EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,                EGL_SUCCESS },
        { EDestroyEndpointCase, 0,                EGL_SUCCESS },
        { EDestroyEndpointCase, EExpectError,     EGL_BAD_ENDPOINT_NOK },
        { ETerminateCase,       0,                EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};


static const TTestCase KDestroyEndpointClosesAllReferences[] =
{
    {
        TESTIDS("564A"),
        CASETITLE("Functional: General: eglDestroyEndpoint closes all references: try to begin streaming after destroy endpoint"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { EBeginStreamingCase,  EExpectError,   EGL_BAD_ENDPOINT_NOK },
        { EAcquireImageCase,    EExpectError,   EGL_BAD_ENDPOINT_NOK },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("564B"),
        CASETITLE("Functional: General: eglDestroyEndpoint closes all references: try to create vg image from egl image after destroy endpoint"),
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
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ECreateVgImageCase,   EExpectError,   EFalse },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};

static const TTestCases KTestCases[] =
{
        TestCase(KCreatePositiveCases),
        TestCase(KCreateBadParameterCases),
        TestCase(KCreateNegativeCases),
        // Note: No positive tests for destroy - that is already done as part of
        // the positive create tests.
        TestCase(KDestroyBadParameterCases),
        TestCase(KDestroyNegativeCases),
        TestCase(KDestroyEndpointClosesAllReferences),
};

const TInt KNumberTestCases = sizeof(KTestCases) / sizeof(KTestCases[0]);

CEglTest_LocalTestStep_EndpointCreate::CEglTest_LocalTestStep_EndpointCreate():
    CEgltest_Local_Engine(KTestCases, KNumberTestCases)
    {
    }
