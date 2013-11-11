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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "egltest_endpoint_stresstests.h"
#include "egltest_endpoint_engine.h"


// an example of table with "random order numbers 0..3". 
static const TInt KImageTable[] = { 1, 3, 0, 2, 3, 1, 2, 0 };

static TStressTestSection test588A[] =
{
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { EInitializeCase,           0,                EGL_SUCCESS },
        { EFinishedCase }
    },
    100, EOnErrorPolicyBreak,
    { PARAM_DECL_ITER(0, 100, 1) },
    {
        { ECreateSurfaceCase,        0,              EGL_SUCCESS, PARAM0(0), EStandardSurface },
        { EFinishedCase }
    },
    100, EOnErrorPolicyBreak,
    { PARAM_DECL_ITER(0, 100, 1), PARAM_DECL_TABLE(KImageTable) },
    {
        { ECreateEndpointCase,       0,              EGL_SUCCESS, PARAM0(0) },
        { EDrawContentCase,          0,              EGL_SUCCESS, PARAM0(0), PARAM1(0) },
        { EContentUpdateCase,        0,              EGL_SUCCESS, PARAM0(0) },
        { EBeginStreamingCase,       0,              EGL_SUCCESS, PARAM0(0) },
        { EAcquireImageCase,         0,              EGL_SUCCESS, PARAM0(0) },
        { ECompareImageCase,         0,              EGL_SUCCESS, PARAM0(0), PARAM1(0) },
        { EReleaseImageCase,         0,              EGL_SUCCESS, PARAM0(0), PARAM0(0) },
        { EDestroyEndpointCase,      0,              EGL_SUCCESS, PARAM0(0) },
        { EFinishedCase }
    },
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { ETerminateCase,            0,              EGL_SUCCESS },
        { EFinishedCase }
    },
};

static TStressTestSection test588B[] =
{
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { EInitializeCase,           0,                EGL_SUCCESS },
        { EFinishedCase }
    },
    1000, EOnErrorPolicyBreak,
    { PARAM_DECL_ITER(0, 1000, 1) },
    {
        { ECreateSurfaceCase,        0,              EGL_SUCCESS, PARAM0(0), EStandardSurface },
        { ECreateEndpointCase,       0,              EGL_SUCCESS, PARAM0(0) },
        { EDrawContentCase,          0,              EGL_SUCCESS, PARAM0(0), PARAM0(0) },
        { EContentUpdateCase,        0,              EGL_SUCCESS, PARAM0(0) },
        { EBeginStreamingCase,       0,              EGL_SUCCESS, PARAM0(0) },
        { EAcquireImageCase,         0,              EGL_SUCCESS, PARAM0(0) },
        { EFinishedCase }
    },
    1000, EOnErrorPolicyBreak,
    { PARAM_DECL_ITER(0, 1000, 1) },
    {
        { EReleaseImageCase,         0,              EGL_SUCCESS, PARAM0(0), PARAM0(0) },
        { EDestroyEndpointCase,      0,              EGL_SUCCESS, PARAM0(0) },
        { EFinishedCase }
    },
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { ETerminateCase,            0,              EGL_SUCCESS },
        { EFinishedCase }
    },
};

static TStressTestSection test589[] =
{
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { EInitializeCase,           0,              EGL_SUCCESS },
        { ECreateSurfaceCase,        0,              EGL_SUCCESS, 0, ELargestPossibleSurface },
        { ECreateEndpointCase,       0,              EGL_SUCCESS, 0 },
        { EFinishedCase }
    },
    5, EOnErrorPolicyBreak,
    { PARAM_DECL_ITER(0, 4, 1) },
    {
        { EDrawContentCase,          0,              EGL_SUCCESS, 0, PARAM0(0) },
        { EContentUpdateCase,        0,              EGL_SUCCESS, 0 },
        { EBeginStreamingCase,       0,              EGL_SUCCESS, 0 },
        { EAcquireImageCase,         0,              EGL_SUCCESS, 0 },
        { ECompareImageCase,         0,              EGL_SUCCESS, 0, PARAM0(0) },
        { EReleaseImageCase,         0,              EGL_SUCCESS, 0 },
        { EFinishedCase }
    },
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { EDestroyEndpointCase,      0,              EGL_SUCCESS, 0 },
        { EDestroySurfaceCase,       0,              EGL_SUCCESS, 0 },
        { ETerminateCase,            0,              EGL_SUCCESS },
        { EFinishedCase }
    },
};

static TStressTestSection test590A[] =
{
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { EInitializeCase,          0,          EGL_SUCCESS },
        { ECreateSurfaceCase,       0,          EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,      0,          EGL_SUCCESS, 0 },
        { EFinishedCase }
    },
    1000, EOnErrorPolicyBreak,
    { PARAM_DECL_TABLE(KImageTable) },
    {
        { EDrawContentCase,         0,         EGL_SUCCESS, 0, PARAM0(0)},
        { EContentUpdateCase,       0,         EGL_SUCCESS, 0 },
        { EBeginStreamingCase,      0,         EGL_SUCCESS, 0 },
        { EAcquireImageCase,        0,         EGL_SUCCESS, 0 },
        { ECompareImageCase,        0,         EGL_SUCCESS, 0, PARAM0(0) },
        { EReleaseImageCase,        0,         EGL_SUCCESS, 0 },
        { EFinishedCase }
    },
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { EDestroyEndpointCase,     0,          EGL_SUCCESS, 0 },
        { ETerminateCase,           0,          EGL_SUCCESS },
        { EFinishedCase }
    },
};

static TStressTestSection test590B[] =
{
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { EInitializeCase,          0,          EGL_SUCCESS },
        { EFinishedCase }
    },
    260, EOnErrorPolicyBreak,
    {PARAM_DECL_ITER(0, 260, 1), PARAM_DECL_TABLE(KImageTable)},
    {
        { ECreateSurfaceCase,       0,          EGL_SUCCESS, PARAM0(0), EStandardSurface },
        { ECreateEndpointCase,      0,          EGL_SUCCESS, PARAM0(0) },
        { EDrawContentCase,         0,          EGL_SUCCESS, PARAM0(0), PARAM1(0)},
        { EContentUpdateCase,       0,          EGL_SUCCESS, PARAM0(0) },
        { EBeginStreamingCase,      0,          EGL_SUCCESS, PARAM0(0) },
        { EAcquireImageCase,        0,          EGL_SUCCESS, PARAM0(0) },
        { ECompareImageCase,        0,          EGL_SUCCESS, PARAM0(0), PARAM1(0)},
        { EFinishedCase }
    },
    260, EOnErrorPolicyBreak,
    {PARAM_DECL_ITER(0, 260, 1)},
    {
        { EReleaseImageCase,        0,          EGL_SUCCESS, PARAM0(0), PARAM0(0) },
        { EDestroyEndpointCase,     0,          EGL_SUCCESS, PARAM0(0) },
        { EFinishedCase }
    },
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { ETerminateCase,           0,          EGL_SUCCESS },
        { EFinishedCase }
    },
};

static TStressTestSection test592A[] =
{
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { EInitializeCase,          0,          EGL_SUCCESS },
        { ECreateSurfaceCase,       0,          EGL_SUCCESS, 0, EStandardSurface },
        { EFinishedCase }
    },
    1000, EOnErrorPolicyBreak,
    { PARAM_DECL_TABLE(KImageTable) },
    {
        { ECreateEndpointCase,      0,          EGL_SUCCESS, 0 },
        { EDrawContentCase,         0,          EGL_SUCCESS, 0, PARAM0(0)},
        { EContentUpdateCase,       0,          EGL_SUCCESS, 0 },
        { EBeginStreamingCase,      0,          EGL_SUCCESS, 0 },
        { EAcquireImageCase,        0,          EGL_SUCCESS, 0 },
        { ECompareImageCase,        0,          EGL_SUCCESS, 0, PARAM0(0)},
        { EReleaseImageCase,        0,          EGL_SUCCESS, 0 },
        { EDestroyEndpointCase,     0,          EGL_SUCCESS, 0 },
        { EFinishedCase }
    },
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { ETerminateCase,           0,          EGL_SUCCESS },
        { EFinishedCase }
    },
};

static TStressTestSection test592B[] =
{
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { EInitializeCase,          0,          EGL_SUCCESS },
        { EFinishedCase }
    },
    1000, EOnErrorPolicyBreak,
    { PARAM_DECL_TABLE(KImageTable) },
    {
        { ECreateSurfaceCase,       0,          EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,      0,          EGL_SUCCESS, 0 },
        { EDrawContentCase,         0,          EGL_SUCCESS, 0, PARAM0(0)},
        { EContentUpdateCase,       0,          EGL_SUCCESS, 0 },
        { EBeginStreamingCase,      0,          EGL_SUCCESS, 0 },
        { EAcquireImageCase,        0,          EGL_SUCCESS, 0 },
        { ECompareImageCase,        0,          EGL_SUCCESS, 0, PARAM0(0) },
        { EReleaseImageCase,        0,          EGL_SUCCESS, 0 },
        { EDestroyEndpointCase,     0,          EGL_SUCCESS, 0 },
        { EDestroySurfaceCase,      0,          EGL_SUCCESS, 0 },
        { EFinishedCase }
    },
    1,  EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { ETerminateCase,           0,          EGL_SUCCESS },
        { EFinishedCase }
    },
};
static const TStressTestTable testTable[] =
{
#if 0
    {
        {
        TESTIDS("588A"),
        CASETITLE("Functional: Stress: Test 100 surfaces."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        },
        TESTTABLE(test588A)
    },
    {
        {
        TESTIDS("588B"),
        CASETITLE("Functional: Stress: Test 1000 surfaces, destroy after."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        },
        TESTTABLE(test588B)
    },
    {
        {
        TESTIDS("589"),
        CASETITLE("Functional: Stress: Test with largest possible surface"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        },
        TESTTABLE(test589)
    }, 
#endif
    {
        {
        TESTIDS("590A"),
        CASETITLE("Functional: Stress: Test with 1000 surface updates"),
        SurfaceTypes3(ESurfTypeEglWindow, ESurfTypeRaw, ESurfTypeRawSingleBuffered),
        },
        TESTTABLE(test590A) 
    },
#if 0
    {
        {
        TESTIDS("590B"),
        CASETITLE("Functional: Stress: Test with 260 surfaces"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        },
        TESTTABLE(test590B)
    },
    {
        {
        TESTIDS("592A"),
        CASETITLE("Functional: Stress: Large number of endpoint creation/destruction"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        },
        TESTTABLE(test592A) 
    },   
    {
        {
        TESTIDS("592B"),
        CASETITLE("Functional: Stress: Large number of endpoint creation/destruction"),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        },
        TESTTABLE(test592B)       
    },
#endif
};


CEglTest_TestStep_StressTests::CEglTest_TestStep_StressTests()
    {
    }

CEglTest_TestStep_StressTests* CEglTest_TestStep_StressTests::New()
    {
    CEglTest_TestStep_StressTests *self = new CEglTest_TestStep_StressTests;
    
    if (self)
        {
        self->ConstructL(testTable, sizeof(testTable)/sizeof(testTable[0]));
        }
    return self;
    }





