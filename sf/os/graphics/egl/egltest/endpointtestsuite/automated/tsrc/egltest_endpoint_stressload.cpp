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

#include <e32math.h>
#include <e32atomics.h>
#include "egltest_endpoint_stressload.h"
#include "egltest_endpoint_engine.h"


// This would run in conjunction with 
static TStressTestSection test603[] =
{
    1, EOnErrorPolicyFail, 
    { NO_PARAMS },
    {
        { EInitializeCase,           0,                 EGL_SUCCESS },
        { EStartLoadThreadCase,      EThreadController, EGL_SUCCESS, EThreadLoadGpuProcessor },
        { EFinishedCase }
    },
    1000, EOnErrorPolicyContinue, 
    { PARAM_DECL_ITER(0, 4, 1) },
    {
        { ECreateSurfaceCase,        0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,       0,              EGL_SUCCESS, 0 },
        { EDrawContentCase,          0,              EGL_SUCCESS, 0, PARAM0(0) },
        { EContentUpdateCase,        0,              EGL_SUCCESS, 0 },
        { EBeginStreamingCase,       0,              EGL_SUCCESS, 0 },
        { EAcquireImageCase,         0,              EGL_SUCCESS, 0 },
        { ECompareImageCase,         0,              EGL_SUCCESS, 0, PARAM0(0) },
        { EReleaseImageCase,         0,              EGL_SUCCESS, 0, 0 },
        { EDestroyEndpointCase,      0,              EGL_SUCCESS, 0 },
        { EDestroySurfaceCase,       0,              EGL_SUCCESS, 0 },
        { EFinishedCase }
    },
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { EEndLoadThreadCase,        EThreadController, EGL_SUCCESS, EThreadLoadGpuProcessor },
        { ETerminateCase,            0,                 EGL_SUCCESS },
        { EFinishedCase }
    },
};


static TStressTestSection test625[] =
{
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { EInitializeCase,           0,                 EGL_SUCCESS },
        { EStartLoadThreadCase,      EThreadController, EGL_SUCCESS, EThreadLoadGpuMemory },
        { EFinishedCase }
    },
    200, EOnErrorPolicyContinue, 
    { PARAM_DECL_ITER(0, 4, 1) },
    {
        { ECreateSurfaceCase,        0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,       0,              EGL_SUCCESS, 0 },
        { EDrawContentCase,          0,              EGL_SUCCESS, 0, PARAM0(0) },
        { EContentUpdateCase,        0,              EGL_SUCCESS, 0 },
        { EBeginStreamingCase,       0,              EGL_SUCCESS, 0 },
        { EAcquireImageCase,         0,              EGL_SUCCESS, 0 },
        { ECompareImageCase,         0,              EGL_SUCCESS, 0, PARAM0(0) },
        { EReleaseImageCase,         0,              EGL_SUCCESS, 0, 0 },
        { EDestroyEndpointCase,      0,              EGL_SUCCESS, 0 },
        { EDestroySurfaceCase,       0,              EGL_SUCCESS, 0 },
        { EFinishedCase }
    },
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { EEndLoadThreadCase,        EThreadController, EGL_SUCCESS, EThreadLoadGpuMemory },
        { ECheckForMemoryLeaks,      0,              EGL_SUCCESS }, 
        { EStartLoadThreadCase,      EThreadController, EGL_SUCCESS, EThreadLoadGpuMemory },
        { EFinishedCase }
    },
    200, EOnErrorPolicyContinue, 
    { PARAM_DECL_ITER(0, 4, 1) },
    {
        { ECreateSurfaceCase,        0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,       0,              EGL_SUCCESS, 0 },
        { EDrawContentCase,          0,              EGL_SUCCESS, 0, PARAM0(0) },
        { EContentUpdateCase,        0,              EGL_SUCCESS, 0 },
        { EBeginStreamingCase,       0,              EGL_SUCCESS, 0 },
        { EAcquireImageCase,         0,              EGL_SUCCESS, 0 },
        { ECompareImageCase,         0,              EGL_SUCCESS, 0, PARAM0(0) },
        { EReleaseImageCase,         0,              EGL_SUCCESS, 0, 0 },
        { EDestroyEndpointCase,      0,              EGL_SUCCESS, 0 },
        { EDestroySurfaceCase,       0,              EGL_SUCCESS, 0 },
        { EFinishedCase }
    },
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { EEndLoadThreadCase,        EThreadController, EGL_SUCCESS, EThreadLoadGpuMemory },
        { ECheckForMemoryLeaks,      0,              EGL_SUCCESS }, 
        { EStartLoadThreadCase,      EThreadController, EGL_SUCCESS, EThreadLoadGpuMemory },
        { EFinishedCase }
    },
    200, EOnErrorPolicyContinue,
    { PARAM_DECL_ITER(0, 4, 1) },
    {
        { ECreateSurfaceCase,        0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,       0,              EGL_SUCCESS, 0 },
        { EDrawContentCase,          0,              EGL_SUCCESS, 0, PARAM0(0) },
        { EContentUpdateCase,        0,              EGL_SUCCESS, 0 },
        { EBeginStreamingCase,       0,              EGL_SUCCESS, 0 },
        { EAcquireImageCase,         0,              EGL_SUCCESS, 0 },
        { ECompareImageCase,         0,              EGL_SUCCESS, 0, PARAM0(0) },
        { EReleaseImageCase,         0,              EGL_SUCCESS, 0, 0 },
        { EDestroyEndpointCase,      0,              EGL_SUCCESS, 0 },
        { EDestroySurfaceCase,       0,              EGL_SUCCESS, 0 },
        { EFinishedCase }
    },
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { EEndLoadThreadCase,        EThreadController, EGL_SUCCESS, EThreadLoadGpuMemory },
        { ECheckForMemoryLeaks,      0,              EGL_SUCCESS }, 
        { EStartLoadThreadCase,      EThreadController, EGL_SUCCESS, EThreadLoadGpuMemory },
        { EFinishedCase }
    },
    200, EOnErrorPolicyContinue,
    { PARAM_DECL_ITER(0, 4, 1) },
    {
        { ECreateSurfaceCase,        0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,       0,              EGL_SUCCESS, 0 },
        { EDrawContentCase,          0,              EGL_SUCCESS, 0, PARAM0(0) },
        { EContentUpdateCase,        0,              EGL_SUCCESS, 0 },
        { EBeginStreamingCase,       0,              EGL_SUCCESS, 0 },
        { EAcquireImageCase,         0,              EGL_SUCCESS, 0 },
        { ECompareImageCase,         0,              EGL_SUCCESS, 0, PARAM0(0) },
        { EReleaseImageCase,         0,              EGL_SUCCESS, 0, 0 },
        { EDestroyEndpointCase,      0,              EGL_SUCCESS, 0 },
        { EDestroySurfaceCase,       0,              EGL_SUCCESS, 0 },
        { EFinishedCase }
    },
    1, EOnErrorPolicyFail,
    { NO_PARAMS },
    {
        { EEndLoadThreadCase,        EThreadController, EGL_SUCCESS, EThreadLoadGpuMemory },
        { ECheckForMemoryLeaks,      0,              EGL_SUCCESS }, 
        { EStartLoadThreadCase,      EThreadController, EGL_SUCCESS, EThreadLoadGpuMemory },
        { EFinishedCase }
    },
    200, EOnErrorPolicyContinue,
    { PARAM_DECL_ITER(0, 4, 1) },
    {
        { ECreateSurfaceCase,        0,              EGL_SUCCESS, 0, EStandardSurface },
        { ECreateEndpointCase,       0,              EGL_SUCCESS, 0 },
        { EDrawContentCase,          0,              EGL_SUCCESS, 0, PARAM0(0) },
        { EContentUpdateCase,        0,              EGL_SUCCESS, 0 },
        { EBeginStreamingCase,       0,              EGL_SUCCESS, 0 },
        { EAcquireImageCase,         0,              EGL_SUCCESS, 0 },
        { ECompareImageCase,         0,              EGL_SUCCESS, 0, PARAM0(0) },
        { EReleaseImageCase,         0,              EGL_SUCCESS, 0, 0 },
        { EDestroyEndpointCase,      0,              EGL_SUCCESS, 0 },
        { EDestroySurfaceCase,       0,              EGL_SUCCESS, 0 },
        { EFinishedCase }
    },
    1, EOnErrorPolicyFail, 
    { NO_PARAMS },
    {
        { EEndLoadThreadCase,        EThreadController, EGL_SUCCESS, EThreadLoadGpuMemory },
        { ECheckForMemoryLeaks,      0,                 EGL_SUCCESS }, 
        { ECheckForMemoryLeaksFinish,0,                 EGL_SUCCESS },
        { ETerminateCase,            0,                 EGL_SUCCESS },
        { EFinishedCase }
    },
};


static const TStressTestTable testTable[] =
{
    {
        {
        TESTIDS("625"),
        CASETITLE("Stress: Test with GPU memory low."),
        SurfaceTypes3(ESurfTypeEglWindow, ESurfTypeRawSingleBuffered, ESurfTypeRaw),
        },
        TESTTABLE(test625)
    },
    {
        {
        TESTIDS("603"),
        CASETITLE("Stress: Test with high GPU load."),
        SurfaceTypes3(ESurfTypeRaw, ESurfTypeEglWindow, ESurfTypeRawSingleBuffered),
        },
        TESTTABLE(test603)
    },
};


CEglTest_TestStep_StressLoad::CEglTest_TestStep_StressLoad()
    {
    }

CEglTest_TestStep_StressLoad* CEglTest_TestStep_StressLoad::New()
    {
    CEglTest_TestStep_StressLoad *self = new CEglTest_TestStep_StressLoad;
    
    if (self)
        {
        self->ConstructL(testTable, sizeof(testTable)/sizeof(testTable[0]));
        }
    return self;
    }


TInt CEglTest_TestStep_StressLoad::LoadGpuProcessor(TAny* aSelf )
    {
    CEglTest_TestStep_StressLoad* self = reinterpret_cast<CEglTest_TestStep_StressLoad*>(aSelf);
    CTrapCleanup *cleanUpStack = CTrapCleanup::New();
    if (!cleanUpStack)
       {
       // Can't use INFO_PRINTF here, as we have not yet
       // created the logger object - nor can we until we have
       // a working cleanupstack, so we just do our best at a 
       // reason able error message.
       RDebug::Printf("Could not allocate memory for cleanupStack!");
       User::Panic(_L("LoadThread"), __LINE__);
       return KErrNoMemory;
       }

    TRAPD(err, self->LoadGpuProcessorL());
    delete cleanUpStack;
    if (err != KErrNone)
        {
        RDebug::Printf("LoadGpuProcessorL left with %d", err);
        User::Panic(_L("LoadGpuProc"), __LINE__);
        }
    return err;
    }


void CEglTest_TestStep_StressLoad::LoadGpuProcessorL()
    {
    CSurface  *surface = CSurface::SurfaceFactoryL(ESurfTypeEglWindow);
    CleanupStack::PushL(surface);
    surface->CreateL(EStandard128sqSurface, TPoint(128, 128));
    TRgb bg = TRgb(0x55, 0x88, 0xff);   // Cyan/turqoise-ish.  
    TRgb fg = TRgb(0xff, 0x11, 0x22);   // Red (sort of)
    while(!__e32_atomic_load_acq32(&iStopThreadFlag[EThreadLoadGpuProcessor])) 
        {
            surface->DrawContentL(bg);
            surface->DrawComplexL(fg);
            surface->SubmitContent(ETrue);
        }
    CleanupStack::PopAndDestroy(surface);
    eglReleaseThread();
    }


TInt CEglTest_TestStep_StressLoad::LoadGpuMemory(TAny* aSelf)
    { 
    CEglTest_TestStep_StressLoad* self = reinterpret_cast<CEglTest_TestStep_StressLoad*>(aSelf);
    CTrapCleanup *cleanUpStack = CTrapCleanup::New();
    if (!cleanUpStack)
       {
       // Can't use INFO_PRINTF here, as we have not yet
       // created the logger object - nor can we until we have
       // a working cleanupstack, so we just do our best at a 
       // reasonable error message.
       RDebug::Printf("Could not allocate memory for cleanupStack!");
       User::Panic(_L("LoadThread"), __LINE__);
       return KErrNoMemory;
       }

    TRAPD(err, self->LoadGpuMemoryL());
    delete cleanUpStack;
    if (err != KErrNone)
        {
        RDebug::Printf("LoadThreadL left with %d", err);
        User::Panic(_L("LoadThread"), __LINE__);
        }
    return err;
    }


void CEglTest_TestStep_StressLoad::LoadGpuMemoryL()
    {
    const TInt KMaxSurfaceAllocs = 1000;
    CSurface **surfaces = new CSurface*[KMaxSurfaceAllocs];
    ENGINE_ASSERT(surfaces);
    TInt nSurfaces = 0;
    TInt err;
    while(!__e32_atomic_load_acq32(&iStopThreadFlag[EThreadLoadGpuMemory]))     
        {
        ENGINE_ASSERT(nSurfaces < KMaxSurfaceAllocs);
        CSurface* s = CSurface::SurfaceFactoryL(ESurfTypePBuffer);
        if (s)
            {
            TRAP(err, s->CreateL(ELargeSurface));
            if (err == KErrNone)
                {
                surfaces[nSurfaces++] = s;
                TRAP(err, s->DrawContentL(TRgb(0x10, 0x20, 0xB0)));
                }
            else
                {
                delete s;
                s = NULL;
                }
            }
        if (!s)
            {
            User::After(100 * 1000);
            TInt nRelease = nSurfaces / 4;
            for(TInt i = 0; i < nRelease; i++)
                {
                delete surfaces[--nSurfaces];
                surfaces[nSurfaces] = NULL;
                }
            User::After(100 * 1000); // 100 ms. 
            }
        }
    while(nSurfaces)
        {
        delete surfaces[--nSurfaces];
        }
    delete [] surfaces;
    eglReleaseThread();
    }



TThreadFunction CEglTest_TestStep_StressLoad::GetThreadFunction(TInt aThreadNumber)
    {
    switch(aThreadNumber)
        {
        case EThreadLoadGpuProcessor:
            return LoadGpuProcessor; 

        case EThreadLoadGpuMemory:
            return LoadGpuMemory;
        }
    RDebug::Printf("%s:%d: Unknown thread number %d", __FILE__, __LINE__, aThreadNumber);
    return NULL;
    }
