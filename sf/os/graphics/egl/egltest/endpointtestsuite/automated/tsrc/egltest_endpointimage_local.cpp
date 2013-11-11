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

#include "egltest_endpointimage.h"


static const TTestCase KImagePositiveCases[] =
{
    {
        TESTIDS("565A"),
        CASETITLE("Functional: Positive: Image content preservation with single update"),
        // TODO: Add video- and viewfinder surfaces.
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
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

        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },

        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS },
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
        TESTIDS("566A"),
        CASETITLE("Functional: Positive: Image content preservation with multiple updates"),
        // TODO: Add video- and viewfinder surfaces.
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeRawSingleBuffered, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        // Draw and compare four times.
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },

        { EDrawContentCase,     0, EGL_SUCCESS, 0, 1 },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS, 0, 1 },
        { EReleaseImageCase,    0, EGL_SUCCESS },

        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },

        { EDrawContentCase,     0, EGL_SUCCESS, 0, 1 },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS, 0, 1 },
        { EReleaseImageCase,    0, EGL_SUCCESS },

        { EEndStreamingCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("567A"),
        CASETITLE("Functional: Positive: Image content preservation in multiple endpoints with single update"),
        // TODO: Add video- and viewfinder surfaces.
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeRawSingleBuffered, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 1, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS, 0, 0 },
        { ECreateEndpointCase,  0, EGL_SUCCESS, 1, 1 },
        // Draw and compare four times.
        { EDrawContentCase,     0, EGL_SUCCESS, 0, 0 },
        { EDrawContentCase,     0, EGL_SUCCESS, 1, 1 },
        { EContentUpdateCase,   0, EGL_SUCCESS, 0 },
        { EContentUpdateCase,   0, EGL_SUCCESS, 1 },
        { EBeginStreamingCase,  0, EGL_SUCCESS, 0 },
        { EBeginStreamingCase,  0, EGL_SUCCESS, 1 },

        { EAcquireImageCase,    0, EGL_SUCCESS, 0, 0 },
        { EAcquireImageCase,    0, EGL_SUCCESS, 1, 1 },
        { ECompareImageCase,    0, EGL_SUCCESS, 0, 0},
        { ECompareImageCase,    0, EGL_SUCCESS, 1, 1 },
        { EReleaseImageCase,    0, EGL_SUCCESS, 0, 0 },
        { EReleaseImageCase,    0, EGL_SUCCESS, 1, 1 },

        { EAcquireImageCase,    0, EGL_SUCCESS, 0, 0 },
        { EAcquireImageCase,    0, EGL_SUCCESS, 1, 1 },
        { ECompareImageCase,    0, EGL_SUCCESS, 0, 0},
        { ECompareImageCase,    0, EGL_SUCCESS, 1, 1 },
        { EReleaseImageCase,    0, EGL_SUCCESS, 0, 0 },
        { EReleaseImageCase,    0, EGL_SUCCESS, 1, 1 },

        { EAcquireImageCase,    0, EGL_SUCCESS, 1, 1 },
        { EAcquireImageCase,    0, EGL_SUCCESS, 0, 0 },
        { ECompareImageCase,    0, EGL_SUCCESS, 1, 1 },
        { ECompareImageCase,    0, EGL_SUCCESS, 0, 0 },
        { EReleaseImageCase,    0, EGL_SUCCESS, 1, 1 },
        { EReleaseImageCase,    0, EGL_SUCCESS, 0, 0 },

        { EAcquireImageCase,    0, EGL_SUCCESS, 0, 0 },
        { EAcquireImageCase,    0, EGL_SUCCESS, 1, 1 },
        { ECompareImageCase,    0, EGL_SUCCESS, 1, 1 },
        { ECompareImageCase,    0, EGL_SUCCESS, 0, 0},
        { EReleaseImageCase,    0, EGL_SUCCESS, 0, 0 },
        { EReleaseImageCase,    0, EGL_SUCCESS, 1, 1 },

        { EEndStreamingCase,    0, EGL_SUCCESS, 0 },
        { EEndStreamingCase,    0, EGL_SUCCESS, 1 },
        { EDestroyEndpointCase, 0, EGL_SUCCESS, 0 },
        { EDestroyEndpointCase, 0, EGL_SUCCESS, 1 },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
    {
        TESTIDS("567B"),
        CASETITLE("Functional: Positive: Image content preservation in multiple endpoints with multiple updates"),
        // TODO: Add video- and viewfinder surfaces.
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeRawSingleBuffered, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 1, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS, 0, 0 },
        { ECreateEndpointCase,  0, EGL_SUCCESS, 1, 1 },
        // Draw and compare four times.
        { EDrawContentCase,     0, EGL_SUCCESS, 0, 0 },
        { EDrawContentCase,     0, EGL_SUCCESS, 1, 1 },
        { EContentUpdateCase,   0, EGL_SUCCESS, 0 },
        { EContentUpdateCase,   0, EGL_SUCCESS, 1 },
        { EBeginStreamingCase,  0, EGL_SUCCESS, 0 },
        { EBeginStreamingCase,  0, EGL_SUCCESS, 1 },
        { EAcquireImageCase,    0, EGL_SUCCESS, 0, 0 },
        { EAcquireImageCase,    0, EGL_SUCCESS, 1, 1 },
        { ECompareImageCase,    0, EGL_SUCCESS, 0, 0},
        { ECompareImageCase,    0, EGL_SUCCESS, 1, 1 },
        { EReleaseImageCase,    0, EGL_SUCCESS, 0, 0 },
        { EReleaseImageCase,    0, EGL_SUCCESS, 1, 1 },

        { EDrawContentCase,     0, EGL_SUCCESS, 0, 2 },
        { EDrawContentCase,     0, EGL_SUCCESS, 1, 3 },
        { EContentUpdateCase,   0, EGL_SUCCESS, 0 },
        { EContentUpdateCase,   0, EGL_SUCCESS, 1 },
        { EBeginStreamingCase,  0, EGL_SUCCESS, 0 },
        { EBeginStreamingCase,  0, EGL_SUCCESS, 1 },
        { EAcquireImageCase,    0, EGL_SUCCESS, 0, 0 },
        { EAcquireImageCase,    0, EGL_SUCCESS, 1, 1 },
        { ECompareImageCase,    0, EGL_SUCCESS, 0, 2},
        { ECompareImageCase,    0, EGL_SUCCESS, 1, 3 },
        { EReleaseImageCase,    0, EGL_SUCCESS, 0, 0 },
        { EReleaseImageCase,    0, EGL_SUCCESS, 1, 1 },

        { EDrawContentCase,     0, EGL_SUCCESS, 0, 0 },
        { EDrawContentCase,     0, EGL_SUCCESS, 1, 1 },
        { EContentUpdateCase,   0, EGL_SUCCESS, 0 },
        { EContentUpdateCase,   0, EGL_SUCCESS, 1 },
        { EBeginStreamingCase,  0, EGL_SUCCESS, 0 },
        { EBeginStreamingCase,  0, EGL_SUCCESS, 1 },
        { EAcquireImageCase,    0, EGL_SUCCESS, 0, 0 },
        { EAcquireImageCase,    0, EGL_SUCCESS, 1, 1 },
        { ECompareImageCase,    0, EGL_SUCCESS, 0, 0},
        { ECompareImageCase,    0, EGL_SUCCESS, 1, 1 },
        { EReleaseImageCase,    0, EGL_SUCCESS, 0, 0 },
        { EReleaseImageCase,    0, EGL_SUCCESS, 1, 1 },

        { EEndStreamingCase,    0, EGL_SUCCESS, 0 },
        { EEndStreamingCase,    0, EGL_SUCCESS, 1 },
        { EDestroyEndpointCase, 0, EGL_SUCCESS, 0 },
        { EDestroyEndpointCase, 0, EGL_SUCCESS, 1 },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
	//569 tests cause a panic
    {
        TESTIDS("569A"),
        CASETITLE("Functional: Positive: VGImage handle remains valid after EGLImage goes out of scope"),
        // TODO: Add video- and viewfinder surfaces.
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeRawSingleBuffered, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECreateVgImageCase,   0, EGL_SUCCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },
        { EEndStreamingCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ETestVgImageValidCase, 0, EGL_SUCCESS },
        { EDestroyVgImageCase,  0, EGL_SUCCESS },
        { ECreateVgImageCase,   EExpectError, VG_BAD_HANDLE_ERROR },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        }
    },
    {
        TESTIDS("569B"),
        CASETITLE("Functional: Positive: VGImage handle remains valid after EGLImage goes out of scope"),
        // TODO: Add video- and viewfinder surfaces.
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeRawSingleBuffered, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECreateVgImageCase,   0, EGL_SUCCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },
        { ETestVgImageValidCase,     0, EGL_SUCCESS },
        { EDestroyVgImageCase,  0, EGL_SUCCESS },
        { EEndStreamingCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ECreateVgImageCase,   EExpectError, VG_BAD_HANDLE_ERROR },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        }
    },
    {
        TESTIDS("569C"),
        CASETITLE("Functional: Positive: VGImage handle remains valid after EGLImage goes out of scope"),
        // TODO: Add video- and viewfinder surfaces.
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeRawSingleBuffered, ESurfTypeEglWindow),
        {
        { EInitializeCase,      0, EGL_SUCCESS },
        { ECreateSurfaceCase,   0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,  0, EGL_SUCCESS },
        { EDrawContentCase,     0, EGL_SUCCESS },
        { EContentUpdateCase,   0, EGL_SUCCESS },
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECreateVgImageCase,   0, EGL_SUCCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },
        { EEndStreamingCase,    0, EGL_SUCCESS },
        { ETestVgImageValidCase,     0, EGL_SUCCESS },
        { EDestroyVgImageCase,  0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { ECreateVgImageCase,   EExpectError, VG_BAD_HANDLE_ERROR },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        }
    },
    {
        TESTIDS("569D"),
        CASETITLE("Functional: Positive: VGImage handle remains valid after EGLImage goes out of scope with two VG images"),
        // TODO: Add video- and viewfinder surfaces.
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeRawSingleBuffered, ESurfTypeEglWindow),
        {
        { EInitializeCase,       0, EGL_SUCCESS },
        { ECreateSurfaceCase,    0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,   0, EGL_SUCCESS },
        { EDrawContentCase,      0, EGL_SUCCESS },
        { EContentUpdateCase,    0, EGL_SUCCESS },
        { EBeginStreamingCase,   0, EGL_SUCCESS },
        { EAcquireImageCase,     0, EGL_SUCCESS },
        { ECreateVgImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,     0, EGL_SUCCESS },
        { EDrawContentCase,      0, EGL_SUCCESS, 0, 1 },
        { EContentUpdateCase,    0, EGL_SUCCESS },
        { EAcquireImageCase,     0, EGL_SUCCESS },
        { ECreateVgImageCase,    0, EGL_SUCCESS, 0, 1 },
        { EEndStreamingCase,     0, EGL_SUCCESS },
        { ETestVgImageValidCase, 0, EGL_SUCCESS },
        { ETestVgImageValidCase, 0, EGL_SUCCESS, 0, 1 },
        { EDestroyVgImageCase,   0, EGL_SUCCESS },
        { EDestroyVgImageCase,   0, EGL_SUCCESS, 0, 1 },
        { EDestroyEndpointCase,  0, EGL_SUCCESS },
        { ECreateVgImageCase,    EExpectError, VG_BAD_HANDLE_ERROR },
        { ETerminateCase,        0, EGL_SUCCESS },
        { EFinishedCase }
        }
    },
    {
        TESTIDS("569E"),
        CASETITLE("Functional: Positive: VGImage handle remains valid after EGLImage goes out of scope with two VG images"),
        // TODO: Add video- and viewfinder surfaces.
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeRawSingleBuffered, ESurfTypeEglWindow),
        {
        { EInitializeCase,       0, EGL_SUCCESS },
        { ECreateSurfaceCase,    0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,   0, EGL_SUCCESS },
        { EDrawContentCase,      0, EGL_SUCCESS },
        { EContentUpdateCase,    0, EGL_SUCCESS },
        { EBeginStreamingCase,   0, EGL_SUCCESS },
        { EAcquireImageCase,     0, EGL_SUCCESS },
        { ECreateVgImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,     0, EGL_SUCCESS },
        { EDrawContentCase,      0, EGL_SUCCESS, 0, 1 },
        { EContentUpdateCase,    0, EGL_SUCCESS },
        { EAcquireImageCase,     0, EGL_SUCCESS },
        { ECreateVgImageCase,    0, EGL_SUCCESS, 0, 1 },
        { EEndStreamingCase,     0, EGL_SUCCESS },
        { EDestroyEndpointCase,  0, EGL_SUCCESS },
        { ETestVgImageValidCase, 0, EGL_SUCCESS },
        { ETestVgImageValidCase, 0, EGL_SUCCESS, 0, 1 },
        { EDestroyVgImageCase,   0, EGL_SUCCESS },
        { EDestroyVgImageCase,   0, EGL_SUCCESS, 0, 1 },
        { ECreateVgImageCase,    EExpectError, VG_BAD_HANDLE_ERROR },
        { ETerminateCase,        0, EGL_SUCCESS },
        { EFinishedCase }
        }
    },
    {
        TESTIDS("569F"),
        CASETITLE("Functional: Positive: VGImage handle remains valid after EGLImage goes out of scope with two VG images"),
        // TODO: Add video- and viewfinder surfaces.
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeRawSingleBuffered, ESurfTypeEglWindow),
        {
        { EInitializeCase,       0, EGL_SUCCESS },
        { ECreateSurfaceCase,    0, EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,   0, EGL_SUCCESS },
        { EDrawContentCase,      0, EGL_SUCCESS },
        { EContentUpdateCase,    0, EGL_SUCCESS },
        { EBeginStreamingCase,   0, EGL_SUCCESS },
        { EAcquireImageCase,     0, EGL_SUCCESS },
        { ECreateVgImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,     0, EGL_SUCCESS },
        { EDrawContentCase,      0, EGL_SUCCESS, 0, 1 },
        { EContentUpdateCase,    0, EGL_SUCCESS },
        { EAcquireImageCase,     0, EGL_SUCCESS },
        { ECreateVgImageCase,    0, EGL_SUCCESS, 0, 1 },
        { EEndStreamingCase,     0, EGL_SUCCESS },
        { ETestVgImageValidCase, 0, EGL_SUCCESS },
        { EDestroyVgImageCase,   0, EGL_SUCCESS },
        { EDestroyEndpointCase,  0, EGL_SUCCESS },
        { ETestVgImageValidCase, 0, EGL_SUCCESS, 0, 1 },
        { EDestroyVgImageCase,   0, EGL_SUCCESS, 0, 1 },
        { ECreateVgImageCase,    EExpectError, VG_BAD_HANDLE_ERROR },
        { ETerminateCase,        0, EGL_SUCCESS },
        { EFinishedCase }
        }
    },
    {
        TESTIDS("570"),
        CASETITLE("Functional: Positive: Acquired EGLImage is not corrupted by submitting new content to surface"),
        // TODO: Add triple buffered, video- and viewfinder surfaces.
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
        { EDrawContentCase,     0, EGL_SUCCESS, 0, 1 },
        { EContentUpdateCase,   ENoSync, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS, 0, 1 },
        { EReleaseImageCase,    0, EGL_SUCCESS },

        { EEndStreamingCase,    0, EGL_SUCCESS, 0 },
        { EDestroyEndpointCase, 0, EGL_SUCCESS, 0 },
        { ETerminateCase,       0, EGL_SUCCESS },
        { EFinishedCase }
        },
    },
};


static const TTestCases KTestCases[] =
{
        TestCase(KImagePositiveCases),
};

const TInt KNumberTestCases = sizeof(KTestCases) / sizeof(KTestCases[0]);


CEglTest_LocalTestStep_EndpointImage::CEglTest_LocalTestStep_EndpointImage():
    CEgltest_Local_Engine(KTestCases, KNumberTestCases)
    {
    }


// Class for lifetime testing - we need to create another process and use endpoints from that process...
_LIT(KEglEndpointLifetimeExe, "eglendpointlifetimetesthelper.exe");


CEglTest_LocalTestStep_EndpointImageLifetime::CEglTest_LocalTestStep_EndpointImageLifetime()
: CLocalTestStepBase(ETestUidEndpointEngine)  // We use the engine to do the remote side functionality.
    {
    }

void CEglTest_LocalTestStep_EndpointImageLifetime::DoPreambleL()
    {
    //Register the test id.
    _LIT(KTestId, "568");
    RegisterTestIdsL(KTestId);
    SetCurrentTestIds(KTestId);
    
    TInt err= iProcessResultInQueue.CreateGlobal(KResultProcessQueueName, 5);
    User::LeaveIfError(err);
    err = iProcessParamsOutQueue.CreateGlobal(KParamsProcessQueueName, 1);
    User::LeaveIfError(err);
    }


void CEglTest_LocalTestStep_EndpointImageLifetime::DoPostambleL()
    {
    iProcessResultInQueue.Close();
    iProcessParamsOutQueue.Close();
    }

CEglTest_LocalTestStep_EndpointImageLifetime::~CEglTest_LocalTestStep_EndpointImageLifetime()
    {
    iProcessResultInQueue.Close();
    iProcessParamsOutQueue.Close();
    }

void CEglTest_LocalTestStep_EndpointImageLifetime::RunScript(
        const TEngineTestCase *aTestCases,
        TRemoteTestParams &params)
    {
    for(TInt i = 0; aTestCases[i].iCase != EFinishedCase; i++)
        {
        params.iEndpointEngine.iEngineTestCase = aTestCases[i];
        RunRemoteTestCase(aTestCases[i].iCase, params);
        }
    }

void CEglTest_LocalTestStep_EndpointImageLifetime::EndpointLifetimeTestL(TSurfaceType aSurfaceType, TInt aExitType)
    {
    // Create an external process that creates a surface.
    TInt err = iProcessStatus.iProcess.Create(KEglEndpointLifetimeExe, KNullDesC);
    User::LeaveIfError(err);
    iProcessStatus.iProcess.Logon(iProcessStatus.iStatus);
    iProcessStatus.iProcess.Resume();


    iProcessResultInQueue.SendBlocking(aSurfaceType);

    TSurfaceId id;
    iProcessParamsOutQueue.ReceiveBlocking(id);
    // Now perform a bit of engine stuff...

    TRemoteTestParams params = {};

    params.iEndpointEngine.iSurfaceParams.iSurfaceId = id;

    static const TEngineTestCase createEndpointSteps[] =
    {
        { EInitializeCase,     0, EGL_SUCCESS },
        { ECreateEndpointCase, 0, EGL_SUCCESS },
        { EFinishedCase,       0, EGL_SUCCESS },
    };

    RunScript(createEndpointSteps, params);

    // Now that we have an endpoint, we can continue the other process.
    iProcessResultInQueue.SendBlocking(aExitType);

    User::WaitForRequest(iProcessStatus.iStatus);

    INFO_PRINTF2(_L("External Process finished with status = %d"), iProcessStatus.iStatus.Int());

    // Check the endpoint content.
    static const TEngineTestCase checkEndpointSteps[] =
    {
        { EBeginStreamingCase,  0, EGL_SUCCESS },
        { EAcquireImageCase,    0, EGL_SUCCESS },
        { ECompareImageCase,    0, EGL_SUCCESS },
        { EReleaseImageCase,    0, EGL_SUCCESS },
        { EDestroyEndpointCase, 0, EGL_SUCCESS },
        { EFinishedCase,        0, EGL_SUCCESS },
    };

    RunScript(checkEndpointSteps, params);
    }

TVerdict CEglTest_LocalTestStep_EndpointImageLifetime::doTestStepL()
    {
    static const TRemoteTestParams nullParams = {};
    StartRemoteTestStep(nullParams);

    TSurfaceType surfaceTypes[] = { ESurfTypeEglWindow, ESurfTypeRaw, ESurfTypeRawSingleBuffered };
    for(TInt i = 0; i < sizeof(surfaceTypes) / sizeof(surfaceTypes[0]); i++)
        {
        TSurfaceType sType = surfaceTypes[i];
        EndpointLifetimeTestL(sType, EEndpointLifetimeNormalExit);
        EndpointLifetimeTestL(sType, EEnfpointLifetimePanicExit);
        }

    EndRemoteTestStep(nullParams);
    INFO_PRINTF1(_L("Lifetime test finished"));
    return TestStepResult();
    }
