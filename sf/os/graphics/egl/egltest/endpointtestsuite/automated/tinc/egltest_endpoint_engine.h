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


#ifndef __EGLTEST_ENDPOINT_ENGINE_H__
#define __EGLTEST_ENDPOINT_ENGINE_H__

#include "localtestbase.h"
#include "remotetestbase.h"
#include "egltest_surface.h"
#include "egltest_endpoint_engine_types.h"
#include "egltest_endpoint_images.h"

#define ENGINE_ASSERT(x) DoEngineAssert((x) != 0, #x, __FILE__, __LINE__)

static inline void DoEngineAssert(TInt aX, const char *aXStr, 
        const char *aFile, TInt aLine)
    {
    if (!aX)
        {
        RDebug::Printf("EGL_ENDPOINT_TEST_ENGINE ASSERT(%s) failed (value %d) at %s:%d\n",
                aXStr, aX, aFile, aLine);
        User::Panic(_L("ENGINE_ASSERT"), aLine);
        }
    }


// This could be wrapped in #if/#else to allow us to generate non-unicode text content.
#define TESTIDS(x)  _S(x)
#define CASETITLE(x)  _S(x)


// SurfaceType lists.
// We use macros so that we get an automatic comment as to what the fields are,
// rather than just listing enums in an array.
#define SurfaceTypes1(type1) 1, { type1 }
#define SurfaceTypes2(type1, type2) 2, { type1, type2 }
#define SurfaceTypes3(type1, type2, type3) 3, { type1, type2, type3 }
#define SurfaceTypes4(type1, type2, type3, type4) 4, { type1, type2, type3, type4 }

// Macro used for building a TTestCases entry.
#define TestCase(x) { x, sizeof(x) / sizeof(x[0]) }

// Forward declare this class.
class CEgltest_Local_Engine_Exec;

// The object passed back from the exec thread to the
// controller thread.
class TExecResult
    {
public:
    TExecResult();

    //Constructor for sending result info.
    TExecResult(TVerdict aVerdict, TEngineCases aCase);
    
    // Constructor for sending result of buffer count paramters.
    TExecResult(TVerdict aBufferCountVerdict, TVerdict aVerdict, TEngineCases aCase);
    
public:
    //Result message.
    TVerdict             iVerdict;
    // Checking buffer count. 
    TVerdict             iBufferCountVerdict;
    // Get Surface parameter reply.
    TSurfaceParamsRemote iSurfaceParams;
    // To check that we are in sync - it should match the request.
    TEngineCases         iCase;
    };

const TInt KMaxLoadThreads = 4;

NONSHARABLE_CLASS(CEgltest_Local_Engine): public CLocalTestStepBase
    {
private:
    enum TExecState
        {
        EExecStateNone  = 0,
        EExecStateRemote, 
        EExecStateLocal
        };
public:
    CEgltest_Local_Engine(const TTestCases *aTestCases, TInt aNumCases);
    virtual ~CEgltest_Local_Engine();
    virtual TVerdict doTestStepL();
    virtual void DoPreambleL();
    virtual void DoPostambleL();
    void SetTestCasesL(const TTestCases * const aTestCases, TInt aNumCases);
    
private:
    void RunTestCase(const TTestCases &aTestCase);
    void RunOneTestCase(const TTestCase& aTestCase, TSurfaceType aSurfType);
    void SendLocalTestCase(const TRemoteTestParams &aParams);
    void RunLocalTestCase(TEngineCases aCase);
    void DoSyncToLocal();
    void GetLocalResult(TExecResult& aResult, TEngineCases aCase);
    void DoMonitorThreadEntry();
    void ForwardPanic(RThread& aThread, RThread& aOtherThread, TRequestStatus &aStatus);
    static TInt MonitorThreadEntry(TAny *aParam);
    void RunControllerLocalAndRemoteL(const TEngineTestCase&, const TRemoteTestParams& params);
    
protected:
    void RunLocalTestCase(const TRemoteTestParams& aMessageIn, TExecResult& aResult);
    TVerdict RunRemoteTestCase(TInt aTestCase, const TRemoteTestParams& aMessageIn);
    void RunSingleCaseL(const TTestCase& aTestCase, TSurfaceType aSurfType);
    void CommonPreambleL();
    CEgltest_Local_Engine();
    // The StartThread and EndThread needs to be overridden in the 
    // implementation to provide a good functionality. 
    virtual void StartThreadL(TInt aThreadNumber);
    virtual void EndThread(TInt aThreadNumber);
    
protected:
    const TTestCases*                  iTestCases;
    TInt                               iNumCases;
    RThread                            iExecThread;
    RThread                            iMonitorThread;
    RThread                            iControllerThread;  // Controller thread
    RMsgQueue<TExecResult>             iExecResultOutQueue;
    RMsgQueue<TRemoteTestParamsPacket> iExecParamsInQueue;
    TExecState                         iExecState;
    TBool                              iLogging;
    };


struct TRemoteTestArgs
    {
    EGLDisplay     iDisplay;
    EGLEndpointNOK iEndpoint;
    EGLImageKHR    iImage;
    };


struct TAvailableMemory
    {
    TInt iHeapMemAvailable;
    TInt iGpuMemAvailable;
    };

// Base class that allows "script" style exectution of testcases.
NONSHARABLE_CLASS(CEgltest_Remote_Engine): public CRemoteTestStepBase
    {
public:
    CEgltest_Remote_Engine();
    virtual ~CEgltest_Remote_Engine();
    virtual TRemoteTestVerdict DoStartRemoteTestStepL(const TRemoteTestParams& aParams);
    virtual TRemoteTestVerdict DoEndRemoteTestStepL(const TRemoteTestParams& aParams);
    virtual TRemoteTestVerdict DoRunRemoteTestCaseL(TInt aTestCase, const TRemoteTestParams& aParams);
private:
    void LogDump(const TEngineTestCase& aCase);
    EGLDisplay GetDisplay(TInt aFlags);
    EGLEndpointNOK GetEndpoint(TInt aIndex, TInt aFlags);
    EGLImageKHR GetImage(TInt aIndex, TInt aFlags);
    EGLenum GetEndpointType(TInt aFlags);
    EGLenum GetSourceType(TInt aFlags);
    const TSurfaceId GetSurfaceId(TInt aFlags, const TSurfaceId& aSurfaceId);
    void CheckReturn(TInt aRetval, const TEngineTestCase& aStreamItem, TInt aFailValue,
                      const TText* aFailSymbol, const TText* aFunction);
    void RunCaseL(TInt aTestCase, const TRemoteTestParams &aParams, const TRemoteTestArgs& aArgs);
    void CreateEndpointCaseL(const TRemoteTestParams& aParams, const TRemoteTestArgs& aArgs);
    void ReleaseImageCaseL(const TRemoteTestParams& aParams, const TRemoteTestArgs& aArgs);
    void ActivateVgContextL();
    void RequestNotificationL(const TRemoteTestParams& aParams, const TRemoteTestArgs& aArgs);
    void GetEndpointDirtyAreaL(const TRemoteTestParams& aParams, const TRemoteTestArgs& aArgs);
    void DoCheckRectsL(EGLint *aActualRects, EGLint aRectCount, EGLint aMaxRects,
                       TInt aRectsIndex, const TRect aSurfacRect);
    void CheckForMemoryLeaks();
    void CheckForMemoryLeaksFinish();
    
    TInt FillGpuMemory();
    TInt CalculateAvailableGPUMemory();
    TInt CalculateAvailableHeapMemory();
    
    // Thread entry points for "load" threads.
    static TInt LoadHeapMemory(TAny *);
    void LoadHeapMemoryL();
    static TInt LoadGpuMemory(TAny *);
    void LoadGpuMemoryL();

    void StartThreadL(TInt aThreadNumber);
    void EndThread(TInt aThreadNumber);
    TThreadFunction GetThreadFunction(TInt aThreadNumber);

private:
    EGLEndpointNOK iEndpoints[KMaxEndpoints];
    EGLImageKHR iEglImage[KMaxEndpoints];
    CTestVgEglImage* iVgImage[KMaxEndpoints];
    TRequestStatus iRequestStatus[KMaxEndpoints];
    TRemoteTestVerdict iTestVerdict;
    TBool iLogging;
    CEglWindowSurface *iSurface;
    // when this flag is set, errors discovered during a test-step is logged.
    // This is normally true, but for stress tests [etc], it can be turned
    // off so that when a test is set up to run 1000 times, but only succeeds
    // X times, we don't get "error, something failed", when we really didn't
    // get an error.
    TBool iLogErrors;
    
    // Tracking memory available. 
    RArray<TAvailableMemory> iMemoryStats;
    
    // Variables to keep track of load-threads.
    RThread iLoadThread[KMaxLoadThreads];
    TBool   iStopThreadFlag[KMaxLoadThreads]; 
    
    RHeap* iMainThreadHeap;
    PFNEGLQUERYPROFILINGDATANOKPROC ipfnEglQueryProfilingDataNOK;
    };


#define CASE(x)   case x: caseName=_S(#x); break

static inline const TText* EngineCaseName(TEngineCases aCase)
    {
    const TText* caseName = _S("Unknown Engine Case");
    switch (aCase)
        {
	  CASE(ECreateEndpointCase);
	  CASE(EBeginStreamingCase);
	  CASE(EAcquireImageCase);
	  CASE(EReleaseImageCase);
	  CASE(EEndStreamingCase);
	  CASE(EContentUpdateCase);
	  CASE(EDestroyEndpointCase);
	  CASE(EInitializeCase);
	  CASE(ETerminateCase);
	  CASE(EGetAttribCase);
	  CASE(ESetAttribCase);
	  CASE(ECreateSurfaceCase);
	  CASE(EDrawContentCase);
	  CASE(ECompareImageCase);
	  CASE(EWaitForCase);
	  CASE(ENotifyWhenCase);
	  CASE(ETestVgImageValidCase);
	  CASE(EDestroyVgImageCase);
	  CASE(EDestroyEglImageCase);
	  CASE(ETimeStampCase);
	  CASE(ERequestNotificationCase);
	  CASE(EWaitForNotificationCase);
	  CASE(ECancelNotificationCase);
	  CASE(EBreakPointCase);
	  CASE(ELogEnableCase);
	  CASE(EIllegalCase);
	  CASE(EPanicCase);
	  CASE(EGetEndpointDirtyAreaCase);
	  CASE(ESyncLocalCase);
	  CASE(EGetSurfaceParamsCase);
	  CASE(EFinishedCase);
	  CASE(ECreateVgImageCase);
      CASE(EDestroySurfaceCase);
      CASE(EStartLoadThreadCase);
      CASE(EEndLoadThreadCase);
      CASE(ECheckForMemoryLeaks);
      CASE(ECheckForMemoryLeaksFinish);
        }
    return caseName;
    }

#undef CASE


inline void LogDump(CTestExecuteLogger &aLogger, const TEngineTestCase& aCase)
{
    const TText *caseName = EngineCaseName(aCase.iCase);
    aLogger.LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo,
                    _L("Performing subcase %d (%s), with flags=%d, err=%04x endpointidx=%d, image=%d, args=(%d, %d)"),
                    aCase.iCase,
                    caseName,
                    aCase.iFlags,
                    aCase.iErrorExpected,
                    aCase.iEndpointIndex,
                    aCase.iImageIndex,
                    aCase.iArg1, aCase.iArg2);
}


#endif // __EGLTEST_ENDPOINT_ENGINE_H__
