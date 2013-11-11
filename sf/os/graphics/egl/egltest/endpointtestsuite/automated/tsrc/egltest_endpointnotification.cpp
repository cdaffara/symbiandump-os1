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

#include "egltest_endpointnotification.h"

static const TTestCase KNotifcationPositiveCases[] =
{
    {
        TESTIDS("559,561"),
        CASETITLE("Functional: Positive: eglRequestNotification basic test."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { ERequestNotificationCase, 0,          EGL_SUCCESS },
        { ECancelNotificationCase, 0,           EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("604"),
        CASETITLE("Functional: Positive: eglRequestNotification with submitupdate."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,          0,              EGL_SUCCESS },
        { ECreateSurfaceCase,       0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,      0,              EGL_SUCCESS },
        { ERequestNotificationCase, 0,              EGL_SUCCESS },
        { EDrawContentCase,         0,              EGL_SUCCESS },
        { EContentUpdateCase,       0,              EGL_SUCCESS },
        { EWaitForNotificationCase, 0,              KErrNone,    0, 0, 100000 },
        { ECancelNotificationCase,  0,              EGL_SUCCESS },
        { EDestroyEndpointCase,     0,              EGL_SUCCESS },
        { ETerminateCase,           0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("605"),
        CASETITLE("Functional: Positive: eglRequestNotification with no submitupdate."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,          0,              EGL_SUCCESS },
        { ECreateSurfaceCase,       0,              EGL_SUCCESS,  0, EStandardSurface },
        { ECreateEndpointCase,      0,              EGL_SUCCESS },
        { ERequestNotificationCase, 0,              EGL_SUCCESS },
        { EWaitForNotificationCase, EExpectError,   KErrTimedOut, 0, 0, 100000 },
        { ECancelNotificationCase,  0,              EGL_SUCCESS },
        { EDestroyEndpointCase,     0,              EGL_SUCCESS },
        { ETerminateCase,           0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("606"),
        CASETITLE("Functional: Positive: eglRequestNotification with submitupdate first."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,          0,              EGL_SUCCESS },
        { ECreateSurfaceCase,       0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,      0,              EGL_SUCCESS },
        { EDrawContentCase,         0,              EGL_SUCCESS },
        { EContentUpdateCase,       0,              EGL_SUCCESS },
        { ERequestNotificationCase, 0,              EGL_SUCCESS },
        { EWaitForNotificationCase, 0,              KErrNone,    0, 0, 100000 },
        { ECancelNotificationCase,  0,              EGL_SUCCESS },
        { EDestroyEndpointCase,     0,              EGL_SUCCESS },
        { ETerminateCase,           0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("607A"),
        CASETITLE("Functional: Positive: eglRequestNotification with multiple endpoints."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,          0,              EGL_SUCCESS },
        { ECreateSurfaceCase,       0,              EGL_SUCCESS,  0, EStandardSurface },
        { ECreateSurfaceCase,       0,              EGL_SUCCESS,  1, EStandardSurface },
        { ECreateEndpointCase,      0,              EGL_SUCCESS,  0, 0 },
        { ECreateEndpointCase,      0,              EGL_SUCCESS,  1, 1},
        { EDrawContentCase,         0,              EGL_SUCCESS,  0, 0 },
        { EContentUpdateCase,       0,              EGL_SUCCESS,  0, 0 },
        { ERequestNotificationCase, 0,              EGL_SUCCESS,  0, 0 },
        { ERequestNotificationCase, 0,              EGL_SUCCESS,  1, 1 },
        { EWaitForNotificationCase, 0,              KErrNone,     0, 0, 100000 },
        { EWaitForNotificationCase, EExpectError,   KErrTimedOut, 1, 1, 100000 },
        { EDestroyEndpointCase,     0,              EGL_SUCCESS,  0, 0 },
        { EDestroyEndpointCase,     0,              EGL_SUCCESS,  1, 1 },
        { ETerminateCase,           0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("607B"),
        CASETITLE("Functional: Positive: Three endpoints, varying async notifications."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,          0,              EGL_SUCCESS },
        { ECreateSurfaceCase,       0,              EGL_SUCCESS,  0, EStandardSurface },
        { ECreateSurfaceCase,       0,              EGL_SUCCESS,  1, EStandardSurface },
        { ECreateSurfaceCase,       0,              EGL_SUCCESS,  2, EStandardSurface },
        { ECreateEndpointCase,      0,              EGL_SUCCESS,  0, 0 },
        { ECreateEndpointCase,      0,              EGL_SUCCESS,  1, 1},
        { ECreateEndpointCase,      0,              EGL_SUCCESS,  2, 2},
        { ERequestNotificationCase, 0,              EGL_SUCCESS,  0, 0 },
        { ERequestNotificationCase, 0,              EGL_SUCCESS,  1, 1 },
        { ERequestNotificationCase, 0,              EGL_SUCCESS,  2, 2 },
        { EDrawContentCase,         0,              EGL_SUCCESS,  1, 1 },
        { EContentUpdateCase,       0,              EGL_SUCCESS,  1, 1 },
        { EWaitForNotificationCase, 0,              KErrNone,     1, 1, 100000 },
        { EWaitForNotificationCase, EExpectError,   KErrTimedOut, 0, 0, 100000 },
        { EWaitForNotificationCase, EExpectError,   KErrTimedOut, 2, 2, 100000 },
        { EDrawContentCase,         0,              EGL_SUCCESS,  2, 2 },
        { EContentUpdateCase,       0,              EGL_SUCCESS,  2, 2 },
        { EWaitForNotificationCase, 0,              KErrNone,     2, 2, 100000 },
        { EWaitForNotificationCase, EExpectError,   KErrTimedOut, 0, 0, 100000 },
        { EWaitForNotificationCase, EExpectError,   KErrTimedOut, 1, 1, 100000 },
        { ECancelNotificationCase,  0,              EGL_SUCCESS,  0, 0 },
        { EWaitForNotificationCase, EExpectError,   KErrCancel,   0, 0, 10000 },
        { EDestroyEndpointCase,     0,              EGL_SUCCESS,  0, 0 },
        { EDestroyEndpointCase,     0,              EGL_SUCCESS,  1, 1 },
        { EDestroyEndpointCase,     0,              EGL_SUCCESS,  2, 2 },
        { ETerminateCase,           0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("608"),
        CASETITLE("Functional: Positive: Multiple eglRequestNotification with no acquire."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,          0,              EGL_SUCCESS },
        { ECreateSurfaceCase,       0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,      0,              EGL_SUCCESS },
        { EDrawContentCase,         0,              EGL_SUCCESS },
        { EContentUpdateCase,       0,              EGL_SUCCESS },
        { ERequestNotificationCase, 0,              EGL_SUCCESS },
        { EWaitForNotificationCase, 0,              KErrNone,    0, 0, 100000 },
        { ERequestNotificationCase, 0,              EGL_SUCCESS },
        { EWaitForNotificationCase, 0,              KErrNone,    0, 0, 100000 },
        { EDestroyEndpointCase,     0,              EGL_SUCCESS },
        { ETerminateCase,           0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("609"),
        CASETITLE("Functional: Positive: Notifications need submitupdate after acquire."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,          0,              EGL_SUCCESS },
        { ECreateSurfaceCase,       0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,      0,              EGL_SUCCESS },
        { EDrawContentCase,         0,              EGL_SUCCESS },
        { EContentUpdateCase,       0,              EGL_SUCCESS },
        { ERequestNotificationCase, 0,              EGL_SUCCESS },
        { EWaitForNotificationCase, 0,              KErrNone,   0, 0, 100000 },
        { EBeginStreamingCase,      0,              EGL_SUCCESS },
        { EAcquireImageCase,        0,              EGL_SUCCESS },
        { ERequestNotificationCase, 0,              EGL_SUCCESS },
        { EWaitForNotificationCase, EExpectError,   KErrTimedOut,0, 0, 100000 },
        { EContentUpdateCase,       ENoSync,        EGL_SUCCESS },
        { EWaitForNotificationCase, 0,              KErrNone,    0, 0, 100000 },
        { EDestroyEndpointCase,     0,              EGL_SUCCESS },
        { ETerminateCase,           0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("610"),
        CASETITLE("Functional: Positive: Destroy Endpoint acts as eglCancelNotificationNOK."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,          0,              EGL_SUCCESS },
        { ECreateSurfaceCase,       0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,      0,              EGL_SUCCESS },
        { ERequestNotificationCase, 0,              EGL_SUCCESS },
        { EDestroyEndpointCase,     0,              EGL_SUCCESS },
        { EWaitForNotificationCase, EExpectError,   KErrCancel, 0, 0, 100000 },
        { ETerminateCase,           0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("612"),
        CASETITLE("Functional: Positive: eglCancelNotificationNOK completes with KErrCancel."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,          0,              EGL_SUCCESS },
        { ECreateSurfaceCase,       0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,      0,              EGL_SUCCESS },
        { ERequestNotificationCase, 0,              EGL_SUCCESS },
        { ECancelNotificationCase,  0,              EGL_SUCCESS },
        { EWaitForNotificationCase, EExpectError,   KErrCancel, 0, 0, 100000 },
        { EDestroyEndpointCase,     0,              EGL_SUCCESS },
        { ETerminateCase,           0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("613"),
        CASETITLE("Functional: Positive: eglCancelNotificationNOK works after notification completed."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,          0,              EGL_SUCCESS },
        { ECreateSurfaceCase,       0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,      0,              EGL_SUCCESS },
        { ERequestNotificationCase, 0,              EGL_SUCCESS },
        { EContentUpdateCase,       0,              EGL_SUCCESS },
        { EWaitForNotificationCase, 0,              KErrNone,    0, 0, 100000 },
        { ECancelNotificationCase,  0,              EGL_SUCCESS },
        { EWaitForNotificationCase, EExpectError,   KErrTimedOut,0, 0, 100000 },
        { EDestroyEndpointCase,     0,              EGL_SUCCESS },
        { ETerminateCase,           0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};

static const TTestCase KNotifcationNegativeCases[] =
{
    {
        TESTIDS("560"),
        CASETITLE("Functional: Negative: eglRequestNotification bad parameter test."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        // Bad display
        { ERequestNotificationCase, EUseBadDisplay, EGL_BAD_DISPLAY },
        { ERequestNotificationCase, EUseBadDisplay | EUseBadEndpoint, EGL_BAD_DISPLAY },
        { ERequestNotificationCase, EUseBadDisplay | EUseBadSync, EGL_BAD_DISPLAY },
        { ERequestNotificationCase, EUseBadDisplay | EUseBadEndpoint | EUseBadSync, EGL_BAD_DISPLAY },
        { ERequestNotificationCase, EUseBadEndpoint, EGL_BAD_ENDPOINT_NOK },
        { ERequestNotificationCase, EUseBadSync, EGL_BAD_PARAMETER },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("562"),
        CASETITLE("Functional: Negative: eglCancelNotification bad parameter test."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        // Bad display
        { ECancelNotificationCase, EUseBadDisplay, EGL_BAD_DISPLAY },
        { ECancelNotificationCase, EUseBadDisplay | EUseBadEndpoint, EGL_BAD_DISPLAY },
        { ECancelNotificationCase, EUseBadEndpoint, EGL_BAD_ENDPOINT_NOK },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};



static const TTestCases KTestCases[] =
{
        TestCase(KNotifcationPositiveCases),
        TestCase(KNotifcationNegativeCases),
};

const TInt KNumberTestCases = sizeof(KTestCases) / sizeof(KTestCases[0]);

CEglTest_LocalTestStep_EndpointNotification::CEglTest_LocalTestStep_EndpointNotification():
    CEgltest_Local_Engine(KTestCases, KNumberTestCases)
    {
    }
