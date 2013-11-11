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
// Testing of eglEndpointDirtyAreaNOK functionality.
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "egltest_endpointdirtyarea.h"

static const TTestCase KDirtyAreaPositiveCases[] =
{
    {
        TESTIDS("614"),
        CASETITLE("Functional: Positive: eglGetEndpointDirtyArea basic test."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,           0,              EGL_SUCCESS },
        { ECreateSurfaceCase,        0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,       0,              EGL_SUCCESS },
        { EDrawContentCase,          0,              EGL_SUCCESS },
        { EContentUpdateCase,        0,              EGL_SUCCESS },
        { EBeginStreamingCase,       0,              EGL_SUCCESS },
        { EAcquireImageCase,         0,              EGL_SUCCESS },
        { EGetEndpointDirtyAreaCase, 0,              EGL_SUCCESS, 0, 0, 0, 1 },
        { EReleaseImageCase,         0,              EGL_SUCCESS },
        { EDestroyEndpointCase,      0,              EGL_SUCCESS },
        { ETerminateCase,            0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("615"),
        CASETITLE("Functional: Positive: eglGetEndpointDirtyArea simple working test - collapse area."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,           0,                EGL_SUCCESS },
        { ECreateSurfaceCase,        0,                EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,       0,                EGL_SUCCESS },
        { EDrawContentCase,          0,                EGL_SUCCESS },
        { EContentUpdateCase,        0,                EGL_SUCCESS },
        { EBeginStreamingCase,       0,                EGL_SUCCESS },
        { EAcquireImageCase,         0,                EGL_SUCCESS },
        { EGetEndpointDirtyAreaCase, EUseCollapseArea, EGL_SUCCESS, 0, 0, 0, 2 },
        { EReleaseImageCase,         0,                EGL_SUCCESS },
        { EDestroyEndpointCase,      0,                EGL_SUCCESS },
        { ETerminateCase,            0,                EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("617"),
        CASETITLE("Functional: Positive: eglGetEndpointDirtyArea - check number of rects avialable"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,           0,              EGL_SUCCESS },
        { ECreateSurfaceCase,        0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,       0,              EGL_SUCCESS },
        { EDrawContentCase,          0,              EGL_SUCCESS },
        { EContentUpdateCase,        0,              EGL_SUCCESS },
        { EBeginStreamingCase,       0,              EGL_SUCCESS },
        { EAcquireImageCase,         0,              EGL_SUCCESS },
        { EGetEndpointDirtyAreaCase, EUseNullRects,  EGL_SUCCESS, 0, 0, 0, 0 },
        { EReleaseImageCase,         0,              EGL_SUCCESS },
        { EDestroyEndpointCase,      0,              EGL_SUCCESS },
        { ETerminateCase,            0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};
static const TTestCase KDirtyAreaNegativeCases[] =
{
    {
        TESTIDS("616"),
        CASETITLE("Functional: Negative: eglRequestNotification bad parameter test."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,           0,                    EGL_SUCCESS },
        { ECreateSurfaceCase,        0,                    EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,       0,                    EGL_SUCCESS },
        { EDrawContentCase,          0,                    EGL_SUCCESS },
        { EContentUpdateCase,        0,                    EGL_SUCCESS },
        { EBeginStreamingCase,       0,                    EGL_SUCCESS },
        { EAcquireImageCase,         0,                    EGL_SUCCESS },
        // Bad display
        { EGetEndpointDirtyAreaCase, EUseBadDisplay,       EGL_BAD_DISPLAY },
        { EGetEndpointDirtyAreaCase, EUseBadDisplay | EUseBadEndpoint, EGL_BAD_DISPLAY },
        { EGetEndpointDirtyAreaCase, EUseBadDisplay | EUseBadRects, EGL_BAD_DISPLAY },
        { EGetEndpointDirtyAreaCase, EUseBadDisplay | EUseBadEndpoint | EUseBadRects, EGL_BAD_DISPLAY },
        { EGetEndpointDirtyAreaCase, EUseBadEndpoint,      EGL_BAD_ENDPOINT_NOK },
        { EGetEndpointDirtyAreaCase, EUseBadRects,         EGL_BAD_PARAMETER, 0, 0, 0, 1 },
        // All zeros, but not NULL rects -> bad parameter error.
        { EGetEndpointDirtyAreaCase, EExpectError,         EGL_BAD_PARAMETER, 0, 0, 0, 0 },
        { EDestroyEndpointCase, 0,                         EGL_SUCCESS },
        { ETerminateCase,       0,                         EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("618"),
        CASETITLE("Functional: Negative: eglRequestNotification no acquired image."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,           0,                    EGL_SUCCESS },
        { ECreateSurfaceCase,        0,                    EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,       0,                    EGL_SUCCESS },
        { EDrawContentCase,          0,                    EGL_SUCCESS },
        { EContentUpdateCase,        0,                    EGL_SUCCESS },
        { EBeginStreamingCase,       0,                    EGL_SUCCESS },
        { EGetEndpointDirtyAreaCase, EExpectError,         EGL_BAD_ACCESS, 0, 0, 0, 1 },
        { EGetEndpointDirtyAreaCase, EUseNullRects|EExpectError, EGL_BAD_ACCESS, 0, 0, 0, 0 },
        { EDestroyEndpointCase, 0,                         EGL_SUCCESS },
        { ETerminateCase,       0,                         EGL_SUCCESS },
        { EFinishedCase }
        },
    },

};


static const TTestCases KTestCases[] =
{
        TestCase(KDirtyAreaPositiveCases),
        TestCase(KDirtyAreaNegativeCases),
};

const TInt KNumberTestCases = sizeof(KTestCases) / sizeof(KTestCases[0]);

CEglTest_LocalTestStep_EndpointDirtyArea::CEglTest_LocalTestStep_EndpointDirtyArea():
    CEgltest_Local_Engine(KTestCases, KNumberTestCases)
    {
    }
