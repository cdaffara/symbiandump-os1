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
 @internalComponent - Internal Symbian test code
*/

#include "egltest_endpointacquire.h"


static const TTestCase KAcquirePositiveCases[] =
{
    // Test "working case"
    {
        TESTIDS("547A"),
        CASETITLE("Functional: Positive: eglAcquireImageNOK with Valid Parameters: Basic Working case"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },
        { EEndStreamingCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    // Positive: Acquire twice with release
    {
        TESTIDS("547B"),
        CASETITLE("Functional: Positive: eglAcquireImageNOK with Valid Parameters: Acquire twice (with release)"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   ENoSync, EGL_SUCCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },
        { EEndStreamingCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("547C"),
        CASETITLE("Functional: Positive: eglAcquireImageNOK with Valid Parameters: Two endpoints, acquire from each of them"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface},
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 1, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS, 0, 0},      // Endpoint 0, image(surface) 0
        { ECreateEndpointCase,  0, EGL_SUCCESS, 1, 1},      // Endpoint 1, image(surface) 1
        { EDrawContentCase,     0, EGL_SUCCESS, 0, 0 },     // Surface 0, Image 0
        { EContentUpdateCase,   0, EGL_SUCCESS, 0 },        // Surface 0
        { EDrawContentCase,     0, EGL_SUCCESS, 1, 1 },     // Surface 1, Image 1
        { EContentUpdateCase,   0, EGL_SUCCESS, 1 },        // Surface 1
        { EBeginStreamingCase,  0, EGL_SUCCESS, 0 },        // Endpoint 0.
        { EBeginStreamingCase,  0, EGL_SUCCESS, 1 },        // Endpoint 1.
        { EAcquireImageCase,    0, EGL_SUCCESS, 1 },        // Endpoint 1
        { EAcquireImageCase,    0, EGL_SUCCESS, 0 },        // Endpoint 0
        { ECompareImageCase,    0, EGL_SUCCESS, 0, 0 },     // eglImage 0, image 0
        { ECompareImageCase,    0, EGL_SUCCESS, 1, 1 },     // eglImage 1, image 1
        { EReleaseImageCase,    0, EGL_SUCCESS, 0, 0 },     // Endpoint 0, image 0
        { EReleaseImageCase,    0, EGL_SUCCESS, 1, 1 },     // Endpoint 1, image 1
        { EEndStreamingCase,    0, EGL_SUCCESS, 0 },        // Endpoint 0
        { EEndStreamingCase,    0, EGL_SUCCESS, 1 },        // Endpoint 1
        { EDestroyEndpointCase, 0, EGL_SUCCESS, 0 },        // Endpoint 0
        { EDestroyEndpointCase, 0, EGL_SUCCESS, 1 },        // Endpoint 1
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    // Positive: Acquire, early destroy...
    {
        TESTIDS("547D"),
        CASETITLE("Functional: Positive: eglAcquireImageNOK with Valid Parameters: Acquire and destroy without release"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    // Positive: Acquire, early endstreaming...
    {
        TESTIDS("547E"),
        CASETITLE("Functional: Positive: eglAcquireImageNOK with Valid Parameters: Acquire and endstreaming without release"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS },
        { EEndStreamingCase,    0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS }, // We need to acquire again to see that it actually works correctly.
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};


static const TTestCase KAcquireNegativeCases[] =
{
    {
        TESTIDS("555A"),
        CASETITLE("Functional: Negative: eglAcquireImageNOK with Bad Parameters: Not ready"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    EExpectError, EGL_BAD_ACCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("555B"),
        CASETITLE("Functional: Negative: eglAcquireImageNOK with Bad Parameters: Not streaming"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EAcquireImageCase,    EExpectError, EGL_BAD_ACCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    // Negative: Acquire twice with no release
    {
        TESTIDS("555C"),
        CASETITLE("Functional: Negative: eglAcquireImageNOK with Bad Parameters: Acquire twice (no release)"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS },
        // Note that when a bad acquire is done, the eglimage value is not stored.
        // so we can still use the same image slot for the release later on.
        { EAcquireImageCase,    EExpectError, EGL_BAD_ACCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },
        { EEndStreamingCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    // Negative: Release bad eglImage then release the proper one, and acquire another (to ensure all worked).
    {
        TESTIDS("555D"),
        CASETITLE("Functional: Negative: eglAcquireImageNOK with Bad Parameters: release bad image then release correct one"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,    EUseBadEglImage, EGL_BAD_PARAMETER },
        { EReleaseImageCase,    0, EGL_SUCCESS },
        { EEndStreamingCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    }
};

static const TTestCase KAcquireBadParamCases[] =
{
    {
        TESTIDS("555E"),
        CASETITLE("Functional: Negative: eglAcquireImageNOK with Bad Parameters: Various bad Parameters(Not Ready state)"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    EUseBadDisplay,  EGL_BAD_DISPLAY },
        { EAcquireImageCase,    EUseBadDisplay | EUseBadEndpoint,    EGL_BAD_DISPLAY },
        { EAcquireImageCase,    EUseBadEndpoint,  EGL_BAD_ENDPOINT_NOK },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase },
        },
    },
    {
        TESTIDS("555F"),
        CASETITLE("Functional: Negative: eglAcquireImageNOK with Bad Parameters: Various bad Parameters(Ready state)"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EAcquireImageCase,    EUseBadDisplay,  EGL_BAD_DISPLAY },
        { EAcquireImageCase,    EUseBadDisplay | EUseBadEndpoint,    EGL_BAD_DISPLAY },
        { EAcquireImageCase,    EUseBadEndpoint,  EGL_BAD_ENDPOINT_NOK },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase },
        },
    },
    {
        TESTIDS("555G"),
        CASETITLE("Functional: Negative: eglAcquireImageNOK with Bad Parameters: Various bad Parameters(Streaming state)"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    EUseBadDisplay,  EGL_BAD_DISPLAY },
        { EAcquireImageCase,    EUseBadDisplay | EUseBadEndpoint,    EGL_BAD_DISPLAY },
        { EAcquireImageCase,    EUseBadEndpoint,  EGL_BAD_ENDPOINT_NOK },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase },
        },
    },
    {
        TESTIDS("555H"),
        CASETITLE("Functional: Negative: eglAcquireImageNOK with Bad Parameters: Various bad Parameters(Image Acquired state)"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { EAcquireImageCase,    EUseBadDisplay,  EGL_BAD_DISPLAY },
        { EAcquireImageCase,    EUseBadDisplay | EUseBadEndpoint,    EGL_BAD_DISPLAY },
        { EAcquireImageCase,    EUseBadEndpoint,  EGL_BAD_ENDPOINT_NOK },
        { ECompareImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase },
        },
    },
};


static const TTestCase KReleaseBadParamCases[] =
{
    {
        TESTIDS("556A"),
        CASETITLE("Functional: Negative: eglReleaseImageNOK with Bad Parameters: Various bad Parameters(Not Ready state)"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EReleaseImageCase,    EUseBadEglImage, EGL_BAD_PARAMETER },
        { EReleaseImageCase,    EUseBadEglImage | EUseBadDisplay, EGL_BAD_DISPLAY },
        { EReleaseImageCase,    EUseBadEglImage | EUseBadDisplay | EUseBadEndpoint, EGL_BAD_DISPLAY },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase },
        },
    },
    {
        TESTIDS("556B"),
        CASETITLE("Functional: Negative: eglReleaseImageNOK with Bad Parameters: Various bad Parameters(Ready state)"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EReleaseImageCase,    EUseBadEglImage, EGL_BAD_PARAMETER },
        { EReleaseImageCase,    EUseBadEglImage | EUseBadDisplay, EGL_BAD_DISPLAY },
        { EReleaseImageCase,    EUseBadEglImage | EUseBadDisplay | EUseBadEndpoint, EGL_BAD_DISPLAY },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase },
        },
    },
    {
        TESTIDS("556C"),
        CASETITLE("Functional: Negative: eglReleaseImageNOK with Bad Parameters: Bad Parameters(Streaming state)"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EReleaseImageCase,    EUseBadEglImage, EGL_BAD_PARAMETER },
        { EReleaseImageCase,    EUseBadEglImage | EUseBadDisplay, EGL_BAD_DISPLAY },
        { EReleaseImageCase,    EUseBadEglImage | EUseBadDisplay | EUseBadEndpoint, EGL_BAD_DISPLAY },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase },
        },
    },
    {
        TESTIDS("556D"),
        CASETITLE("Functional: Negative: eglReleaseImageNOK with Bad Parameters: Various bad Parameters(Image Acquired state)"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,    EUseBadDisplay, EGL_BAD_DISPLAY },
        { EReleaseImageCase,    EUseBadEndpoint, EGL_BAD_ENDPOINT_NOK },
        { EReleaseImageCase,    EUseBadEglImage, EGL_BAD_PARAMETER },
        { EReleaseImageCase,    EUseBadApi, EGL_BAD_PARAMETER },
        { EReleaseImageCase,    EUseBadDisplay | EUseBadEndpoint, EGL_BAD_DISPLAY },
        { EReleaseImageCase,    EUseBadDisplay | EUseBadEglImage, EGL_BAD_DISPLAY },
        { EReleaseImageCase,    EUseBadDisplay | EUseBadApi, EGL_BAD_DISPLAY },
        { EReleaseImageCase,    EUseBadEglImage | EUseBadApi, EGL_BAD_PARAMETER },
        { EReleaseImageCase,    EUseBadDisplay | EUseBadEndpoint | EUseBadEglImage, EGL_BAD_DISPLAY },
        { EReleaseImageCase,    EUseBadDisplay | EUseBadEndpoint | EUseBadApi, EGL_BAD_DISPLAY },
        { EReleaseImageCase,    EUseBadDisplay | EUseBadEglImage | EUseBadApi, EGL_BAD_DISPLAY },
        { EReleaseImageCase,    EUseBadDisplay | EUseBadEndpoint | EUseBadEglImage | EUseBadApi, EGL_BAD_DISPLAY },
        { ECompareImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase },
        },
    },
};


static const TTestCase KReleasePositiveCases[] =
{
    {
        TESTIDS("548A"),
        CASETITLE("Functional: Positive: eglReleaseImageNOK with Valid Parameters: Simple acquire then release"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase },
        },
    },
    {
        TESTIDS("548B"),
        CASETITLE("Functional: Positive: eglReleaseImageNOK with Valid Parameters: Acquire, release twice"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },
        // Should fail.
        { EReleaseImageCase,    EExpectError, EGL_BAD_PARAMETER },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase },
        },
    },
};

static const TTestCase KReleaseNegativeCases[] =
{
    {
        TESTIDS("548C"),
        CASETITLE("Functional: Negative: eglReleaseImageNOK with Valid Parameters: Two EPs, acquire, then release on wrong EP."),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 1, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS, 0, 0 },
        { ECreateEndpointCase,  0, EGL_SUCCESS, 1, 1 },
        { EDrawContentCase,     0, EGL_SUCCESS, 0, 0 },
        { EContentUpdateCase,   0, EGL_SUCCESS, 0 },
        { EDrawContentCase,     0, EGL_SUCCESS, 1, 1 },
        { EContentUpdateCase,   0, EGL_SUCCESS, 1 },
        { EBeginStreamingCase,  0, EGL_SUCCESS, 0 },
        { EBeginStreamingCase,  0, EGL_SUCCESS, 1 },
        { EAcquireImageCase,    0, EGL_SUCCESS, 0, 0 },
        { EAcquireImageCase,    0, EGL_SUCCESS, 1, 1 },
        // Swapping the release of the image - should fail.
        { EReleaseImageCase,    EExpectError, EGL_BAD_PARAMETER, 0, 1 },
        { EReleaseImageCase,    EExpectError, EGL_BAD_PARAMETER, 1, 0 },
        // Check that the originally acquired images are still there...
        { ECompareImageCase,    0, EGL_SUCCESS, 0, 0 },
        { ECompareImageCase,    0, EGL_SUCCESS, 1, 1 },
        // Now do it the right way around
        { EReleaseImageCase,    0, EGL_SUCCESS, 1, 1 },
        { EReleaseImageCase,    0, EGL_SUCCESS, 0, 0 },
        { EDestroyEndpointCase, 0, EGL_SUCCESS, 0 },
        { EDestroyEndpointCase, 0, EGL_SUCCESS, 1 },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase },
        },
    },
    {
        TESTIDS("619"),
        CASETITLE("Functional: Negative: eglDestroyImageKHR can not be used for images acquired from endpoint."),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS, },
        { EDrawContentCase,     0, EGL_SUCCESS, },
        { EContentUpdateCase,   0, EGL_SUCCESS, },
        { EBeginStreamingCase,  0, EGL_SUCCESS, },
        { EAcquireImageCase,    0, EGL_SUCCESS, },
        { EDestroyEglImageCase, EExpectError, EGL_BAD_ACCESS },
        // Compare Image to make sure it's still "there".
        { ECompareImageCase,    0, EGL_SUCCESS, },
        { EReleaseImageCase,    0, EGL_SUCCESS, },
        { EDestroyEndpointCase, 0, EGL_SUCCESS, },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase },
        },
    },

};


static const TTestCases KTestCases[] =
{
        TestCase(KAcquirePositiveCases),
        TestCase(KAcquireNegativeCases),
        TestCase(KAcquireBadParamCases),
        TestCase(KReleasePositiveCases),
        TestCase(KReleaseBadParamCases),
        TestCase(KReleaseNegativeCases),
};

const TInt KNumberTestCases = sizeof(KTestCases) / sizeof(KTestCases[0]);


CEglTest_LocalTestStep_EndpointAcquire::CEglTest_LocalTestStep_EndpointAcquire():
    CEgltest_Local_Engine(KTestCases, KNumberTestCases)
    {
    }
