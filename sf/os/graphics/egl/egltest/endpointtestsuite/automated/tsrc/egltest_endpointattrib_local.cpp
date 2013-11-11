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

#include "egltest_endpointattrib.h"

const TInt KBadAttribValue = EGL_BAD_ATTRIBUTE;  // Use error code as a "invalid attribute value".

// For testing flexibility of the concept. Not actually used in the testcode at this point in time.
static const TTestCase KAttribBadParameterCases[] =
{
    {
        TESTIDS("551A"),
        CASETITLE("Functional: Negative: eglGetEndpointAttribNOK with Bad Parameters: bad display is error code when getting attributes"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EGetAttribCase,       EUseBadDisplay, EGL_BAD_DISPLAY, 0, 0, EGL_ENDPOINT_READY_NOK, 0},
        // Use bad endpoint with bad display - should give bad display.
        { EGetAttribCase,       EUseBadEndpoint | EUseBadDisplay,
                EGL_BAD_DISPLAY, 0, 0, EGL_ENDPOINT_READY_NOK, 0},
        { EGetAttribCase,       EUseBadEndpoint,
                EGL_BAD_ENDPOINT_NOK, 0, 0, EGL_ENDPOINT_READY_NOK, 0},
        { EGetAttribCase,       EUseBadDisplay, EGL_BAD_DISPLAY, 0, 0, EGL_DELAY_NOK, 0 },
        // Use bad endpoint with bad display - should give bad display.
        { EGetAttribCase,       EUseBadEndpoint | EUseBadDisplay,
                EGL_BAD_DISPLAY, 0, 0, EGL_DELAY_NOK, 0},
        { EGetAttribCase,       EUseBadEndpoint,EGL_BAD_ENDPOINT_NOK, 0, 0, EGL_DELAY_NOK, 0},
        { EGetAttribCase,       EUseBadDisplay, EGL_BAD_DISPLAY, 0, 0, KBadAttribValue, 0},
        { EGetAttribCase,       EExpectError,   EGL_BAD_PARAMETER, 0, 0, KBadAttribValue, 0},
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("551B"),
        CASETITLE("Functional: Negative: eglGetEndpointAttribNOK with Bad Parameters: bad parameter is error code when requesting invalid attribute"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        // First check that bad display is issued even when the attribute is bad.
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    // SetEndpointAttrib tests.
    {
        TESTIDS("552A"),
        CASETITLE("Functional: Negative: eglSetEndpointAttribNOK with Bad Parameters: bad display is error code when setting endpoint ready"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { ESetAttribCase,       EUseBadDisplay, EGL_BAD_DISPLAY, 0, 0, EGL_ENDPOINT_READY_NOK, 0},
        { ESetAttribCase,       EUseBadDisplay, EGL_BAD_DISPLAY, 0, 0, EGL_ENDPOINT_READY_NOK, 0},
        // Use bad endpoint with bad display - should give bad display.
        { ESetAttribCase,       EUseBadEndpoint | EUseBadDisplay,
                EGL_BAD_DISPLAY, 0, 0, EGL_ENDPOINT_READY_NOK, 0},
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("552B"),
        CASETITLE("Functional: Negative: eglSetEndpointAttribNOK with Bad Parameters: bad display is error code when setting endpoint delay"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { ESetAttribCase,       EUseBadDisplay, EGL_BAD_DISPLAY, 0, 0, EGL_DELAY_NOK, 0 },
        // Use bad endpoint with bad display - should give bad display.
        { ESetAttribCase,       EUseBadEndpoint | EUseBadDisplay,
                EGL_BAD_DISPLAY, 0, 0, EGL_DELAY_NOK, 0},
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("552C"),
        CASETITLE("Functional: Negative: eglSetEndpointAttribNOK with Bad Parameters: bad endpoint is error code when setting endpoint delay"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { ESetAttribCase,       EUseBadEndpoint,EGL_BAD_ENDPOINT_NOK, 0, 0, EGL_DELAY_NOK, 0},
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("552D"),
        CASETITLE("Functional: Negative: eglSetEndpointAttribNOK with Bad Parameters: bad parameter is error code when setting invalid attribute"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        // First check that bad display is issued even when the attribute is bad.
        { ESetAttribCase,       EUseBadDisplay, EGL_BAD_DISPLAY, 0, 0, KBadAttribValue, 0},
        { ESetAttribCase,       EExpectError,   EGL_BAD_PARAMETER, 0, 0, KBadAttribValue, 0},
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("552E"),
        CASETITLE("Functional: Negative: eglSetEndpointAttribNOK with Bad Parameters: bad parameter is error code when setting EGL_ENDPOINT_READY_NOK"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { ESetAttribCase,       EExpectError,   EGL_BAD_PARAMETER, 0, 0, EGL_ENDPOINT_READY_NOK, 0},
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};

static const TTestCase KAttribPositiveCases[] =
{
    {
        TESTIDS("543A"),
        CASETITLE("Functional: Positive: eglGetEndpointAttribNOK with Valid Parameters: check that EGL_ENDPOINT_READY_NOK returns EGL_FALSE when not ready"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EGetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_ENDPOINT_READY_NOK, EGL_FALSE },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("543B"),
        CASETITLE("Functional: Positive: eglGetEndpointAttribNOK with Valid Parameters: check that EGL_ENDPOINT_READY_NOK returns EGL_TRUE when ready"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EGetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_ENDPOINT_READY_NOK, EGL_TRUE },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("543C"),
        CASETITLE("Functional: Positive: eglGetEndpointAttribNOK with Valid Parameters: Two endpoints, check EGL_ENDPOINT_READY_NOK returns EGL_TRUE for one but not other endpoint"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 1, EUnalignedPixelSizeSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 1 },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EGetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_ENDPOINT_READY_NOK, EGL_TRUE },
        { EGetAttribCase,       0,              EGL_SUCCESS, 1, 0, EGL_ENDPOINT_READY_NOK, EGL_FALSE },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS, 1 },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("543D"),
        CASETITLE("Functional: Positive: eglGetEndpointAttribNOK with Valid Parameters: check that EGL_DELAY_NOK returns 0 for a new endpoint"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EGetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 0 },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    // SetEndpointAttribute tests.
    {
        TESTIDS("544A"),
        CASETITLE("Functional: Positive: eglSetEndpointAttribNOK with Valid Parameters: set EGL_DELAY_NOK"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { ESetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 100 },
        { EGetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 100 },
        // Check that we can set a relatively large value, 250 ms, and that it remains.
        { ESetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 250000 },
        { EGetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 250000 },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("544B"),
        CASETITLE("Functional: Positive: eglSetEndpointAttribNOK with Valid Parameters: set EGL_DELAY_NOK, then set again to a different value"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { ESetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 100 },
        { EGetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 100 },
        { ESetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 208 },
        { EGetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 208 },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("544C"),
        CASETITLE("Functional: Positive: eglSetEndpointAttribNOK with Valid Parameters: set EGL_DELAY_NOK, then with draw/update between set and get, then set again"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS },
        { ESetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 100 },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EGetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 100 },
        { ESetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 208 },
        { EGetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 208 },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("544D"),
        CASETITLE("Functional: Positive: eglSetEndpointAttribNOK with Valid Parameters: set EGL_DELAY_NOK on two endpoints, with different values"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 1, EUnalignedPixelSizeSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 0, 0 },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 1, 1 },
        { ESetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 100 },
        { ESetAttribCase,       0,              EGL_SUCCESS, 1, 0, EGL_DELAY_NOK, 208 },
        { EGetAttribCase,       0,              EGL_SUCCESS, 1, 0, EGL_DELAY_NOK, 208 },
        { EGetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 100 },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS, 1, 1 },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS, 0, 0 },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("544E"),
        CASETITLE("Functional: Positive: eglSetEndpointAttribNOK with Valid Parameters: set EGL_DELAY_NOK on two endpoints, with different values, and swapping buffers between"),
        SurfaceTypes2(ESurfTypeRaw, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0,              EGL_SUCCESS },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateSurfaceCase,   0,              EGL_SUCCESS, 1, EUnalignedPixelSizeSurface },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 0, 0 },
        { ECreateEndpointCase,  0,              EGL_SUCCESS, 1, 1 },
        { ESetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 100 },
        { ESetAttribCase,       0,              EGL_SUCCESS, 1, 0, EGL_DELAY_NOK, 208 },
        { EDrawContentCase,     0,              EGL_SUCCESS },
        { EContentUpdateCase,   0,              EGL_SUCCESS },
        { EGetAttribCase,       0,              EGL_SUCCESS, 1, 0, EGL_DELAY_NOK, 208 },
        { EGetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_DELAY_NOK, 100 },
        { EGetAttribCase,       0,              EGL_SUCCESS, 1, 0, EGL_ENDPOINT_READY_NOK, EGL_FALSE },
        { EGetAttribCase,       0,              EGL_SUCCESS, 0, 0, EGL_ENDPOINT_READY_NOK, EGL_TRUE },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS, 1, 1 },
        { EDestroyEndpointCase, 0,              EGL_SUCCESS, 0, 0 },
        { ETerminateCase,       0,              EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};

static const TTestCases KTestCases[] =
{
        TestCase(KAttribPositiveCases),
        TestCase(KAttribBadParameterCases),
};

const TInt KNumberTestCases = sizeof(KTestCases) / sizeof(KTestCases[0]);

CEglTest_LocalTestStep_EndpointAttrib::CEglTest_LocalTestStep_EndpointAttrib():
    CEgltest_Local_Engine(KTestCases, KNumberTestCases)
    {
    }
