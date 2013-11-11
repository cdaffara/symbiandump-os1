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

#include <e32std.h>
#include <e32math.h>
#include "egltest_endpoint_engine.h"
#include "egltest_endpoint_engine_execthread.h"


TExecResult::TExecResult(void)
    {
    iVerdict = EInconclusive;
    Mem::FillZ(&iSurfaceParams, sizeof(iSurfaceParams));
    }

TExecResult::TExecResult(TVerdict aVerdict, TEngineCases aCase)
    {
    iCase = aCase;
    iVerdict = aVerdict;
    }

TExecResult::TExecResult(TVerdict aBufferCountVerdict, TVerdict aVerdict, TEngineCases aCase)
    {
    iCase = aCase;
    iBufferCountVerdict = aBufferCountVerdict;
    iVerdict = aVerdict;
    }

CEgltest_Local_Engine::CEgltest_Local_Engine(const TTestCases *aTestCases, TInt aNumCases) :
    CLocalTestStepBase(ETestUidEndpointEngine),
    iTestCases(aTestCases),
    iNumCases(aNumCases)
    {
    }

CEgltest_Local_Engine::CEgltest_Local_Engine() :
    CLocalTestStepBase(ETestUidEndpointEngine),
    iTestCases(NULL),
    iNumCases(0)
    {
    }


CEgltest_Local_Engine::~CEgltest_Local_Engine()
    {
    }

void CEgltest_Local_Engine::DoPreambleL()
    {
    //Register all of the test ids. Do this first so 
    //that they get logged even if a panic occurs.
    const TTestCases* outerCases = iTestCases;
    TInt numOuterCases = iNumCases;
    for(TInt curOuterCase = 0; curOuterCase < numOuterCases; curOuterCase++)
        {
        const TTestCase* innerCases = outerCases[curOuterCase].iCase;
        TInt numInnerCases = outerCases[curOuterCase].iCount;
        for(TInt curInnerCase = 0; curInnerCase < numInnerCases; curInnerCase++)
            {
            TPtrC testIds(innerCases[curInnerCase].iBase.iRelatedTestIds);
            RegisterTestIdsL(testIds);
            }
        }
    CommonPreambleL();
    }

void CEgltest_Local_Engine::CommonPreambleL()
    {
    // For details about the threads, see comment in the egltest_local_engine_exec.cpp
    
    // Amount of stack-space we allow for the execution thread. 
    const TInt KStackSize = 12000;
    // Min/Max Heap Sizes
    const TInt KHeapMinSize = 16000;
    const TInt KHeapMaxSize = 1000000;
    
    // We don't need a very long queue length here. But it needs to be long enough
    // to handle a few outstanding requests. 10 seems like a good number.
    const TInt KMaxQueueLen = 10;

    TInt err = iExecParamsInQueue.CreateGlobal(KExecParamsQueueName, KMaxQueueLen, EOwnerThread);
    if (err != KErrNone)
        {
        INFO_PRINTF1(_L("Could not create message queue for Exec Thread parameters"));
        User::Leave(err);
        }
    
    err = iExecResultOutQueue.CreateGlobal(KExecResultQueueName, KMaxQueueLen, EOwnerThread);
    if (err != KErrNone)
        {
        INFO_PRINTF1(_L("Could not create message queue for Exec Thread results"));
        User::Leave(err);
        }
   
    // Copy the current thread so that we can use it in another thread.
    iControllerThread.Duplicate(RThread(), EOwnerProcess);
    
    //Set up for creating unique thread names.
    TUint32 random = Math::Random();
    TName threadName;
    _LIT(KThreadNameFormat, "%S-%u");
    
    // Creater a monitor thread. The monitor thread is used to keep track
    // of the controller (this) thread and the exec thread. If one panics
    // the monitor thread will ensure the other thread also panics.
    // The monitor thread shouldn't need much in the way of a heap, so
    // use min for both the min and max size of the heap.
    _LIT(KMonitorName, "EpTestLocalMonitor");
    threadName.Format(KThreadNameFormat, &KMonitorName, random);
    err = iMonitorThread.Create(threadName, CEgltest_Local_Engine::MonitorThreadEntry, 
            KStackSize, KHeapMinSize, KHeapMinSize, this, EOwnerThread);  
    if (err != KErrNone)
        {
        INFO_PRINTF1(_L("Could not create monitor thread"));
        User::Leave(err);
        }
    _LIT(KExecName, "EpTestLocalExec");
    threadName.Format(KThreadNameFormat, &KExecName, random);
    err = iExecThread.Create(threadName, CEgltest_Local_Engine_Exec::ThreadEntry, 
            KStackSize, KHeapMinSize, KHeapMaxSize, NULL, EOwnerThread);
    if (err != KErrNone)
        {
        INFO_PRINTF1(_L("Could not create execution thread"));
        User::Leave(err);
        }
    TRequestStatus rendezvous;
    iMonitorThread.Rendezvous(rendezvous);
    iMonitorThread.Resume();
    // Make sure the monitor thread has logged onto the exec thread BEFORE
    // the exec thread gets to start. Otherwise, there is a small chance
    // the monitor thread will not have run by the time the exec (or this)
    // thread panics.
    User::WaitForRequest(rendezvous);
    iControllerThread.Close();
    iExecThread.Resume();
    }

TInt CEgltest_Local_Engine::MonitorThreadEntry(TAny *aParam)
    {
    CEgltest_Local_Engine *self = reinterpret_cast<CEgltest_Local_Engine *>(aParam);
    self->DoMonitorThreadEntry();
    return 0;
    }

// Forward a the panic from the controller to the exec thread or vice versa.
void CEgltest_Local_Engine::DoMonitorThreadEntry()
    {
    //Duplicate the thread handles into this thread,
    //and keep them locally so they can't go out of scope.
    RThread execThread = iExecThread;
    execThread.Duplicate(iControllerThread, EOwnerThread);
    RThread controllerThread = iControllerThread;
    controllerThread.Duplicate(iControllerThread, EOwnerThread);
    
    //Logon to the two threads.
    TRequestStatus execStatus;
    TRequestStatus controllerStatus;    
    execThread.Logon(execStatus);
    controllerThread.Logon(controllerStatus);
    
    //Rendevous with our parent then wait for either thread to exit.
    RThread().Rendezvous(KErrNone);
    User::WaitForRequest(execStatus, controllerStatus);
    
    //Forward the panic from the one thread to the other.
    //If a panic did occur, terminate EGL.
    if (execStatus != KRequestPending)
        {
        if(execThread.ExitType() == EExitPanic)
            {
            controllerThread.Panic(execThread.ExitCategory(), execThread.ExitReason());
            eglTerminate(eglGetDisplay(EGL_DEFAULT_DISPLAY));
            eglReleaseThread();
            }
        controllerThread.LogonCancel(controllerStatus);
        User::WaitForRequest(controllerStatus);
        }
    else if (controllerStatus != KRequestPending)
        {
        if(controllerThread.ExitType() == EExitPanic)
            {
            execThread.Panic(controllerThread.ExitCategory(), controllerThread.ExitReason());
            eglTerminate(eglGetDisplay(EGL_DEFAULT_DISPLAY));
            eglReleaseThread();
            }
        execThread.LogonCancel(execStatus);
        User::WaitForRequest(execStatus);
        }
    
    controllerThread.Close();
    execThread.Close();
    }

// Tear down threads.
void CEgltest_Local_Engine::DoPostambleL()
    {
    TRequestStatus status;
    iExecThread.Logon(status);
    RunLocalTestCase(ETerminateCase);
    User::WaitForRequest(status);
    iExecThread.Close();
    iMonitorThread.Close();
    iControllerThread.Close();
    iExecParamsInQueue.Close();
    iExecResultOutQueue.Close();
    }

TVerdict CEgltest_Local_Engine::doTestStepL()
    {
    SetTestStepResult(EPass);
    
    INFO_PRINTF2(_L("Executing test with %d cases..."), iNumCases);
    for(TInt runCase = 0; runCase < iNumCases; runCase++)
        {
        RunTestCase(iTestCases[runCase]);
        }
    return TestStepResult();
    }

void CEgltest_Local_Engine::RunTestCase(const TTestCases &aTestCases)
    {
    for(TInt testCase = 0; testCase < aTestCases.iCount; testCase++)
        {
        const TTestCase &thisCase = aTestCases.iCase[testCase];
        
        //Set the Ids of the test case as the current ones.
        TPtrC testIds(thisCase.iBase.iRelatedTestIds);
        SetCurrentTestIds(testIds);
        
        for(TInt surfIter= 0; surfIter < thisCase.iBase.iSurfaceTypeCount; surfIter++)
            {
            TSurfaceType surfType = thisCase.iBase.iSurfaceTypeList[surfIter];
            
            INFO_PRINTF3(_L("Running testcase GRAPHICS-EGL-%s: %s"),
                    thisCase.iBase.iRelatedTestIds, thisCase.iBase.iName);

            RunOneTestCase(thisCase, surfType);
            }
        }
    }

// Wait for a reply from the local execution thread. 
// All results have a verdict, and we check her to set it
// if a "non-pass" result is returned.
void CEgltest_Local_Engine::GetLocalResult(TExecResult &result, TEngineCases aCase)
    {
    iExecResultOutQueue.ReceiveBlocking(result);
    // Check verdict; but only change if it's the FIRST failure.
    if (result.iVerdict != EPass && TestStepResult() == EPass)
        {
        SetTestStepResult(result.iVerdict);
        }
    ENGINE_ASSERT(aCase == result.iCase);
    }

TVerdict CEgltest_Local_Engine::RunRemoteTestCase(TInt aTestCase, const TRemoteTestParams& aMessageIn)
    {
    if (iExecState != EExecStateRemote)
        {
        DoSyncToLocal();
        iExecState = EExecStateRemote;
        }
    return CLocalTestStepBase::RunRemoteTestCase(aTestCase, aMessageIn);
    }

// Send Sync command to Local execution thread, then 
// wait for the reply from that sync. 
void CEgltest_Local_Engine::DoSyncToLocal()
    {
    RunLocalTestCase(ESyncLocalCase);
    }


// Send command to local exec thread.
void CEgltest_Local_Engine::SendLocalTestCase(const TRemoteTestParams &aParams)
    {
    TRemoteTestParamsPacket message(iTestId, aParams.iEndpointEngine.iEngineTestCase.iCase, aParams);

    iExecParamsInQueue.SendBlocking(message);
    iExecState = EExecStateLocal;
    }

// Send command to Local execution thread, then 
// wait for the reply from that command.  
void CEgltest_Local_Engine::RunLocalTestCase(const TRemoteTestParams& aMessageIn, TExecResult& aResult)
    {
    SendLocalTestCase(aMessageIn);
    GetLocalResult(aResult, aMessageIn.iEndpointEngine.iEngineTestCase.iCase);
    }

// Wrapper for simple cases where we just want to send 
// the command with no arguments, and just get the
// result for the purpose of syncing - so result
// content is ignored.
void CEgltest_Local_Engine::RunLocalTestCase(TEngineCases aCase)
    {
    TRemoteTestParams params = {};
    TExecResult result;
    params.iEndpointEngine.iEngineTestCase.iCase = aCase;
    RunLocalTestCase(params, result);
    }


void CEgltest_Local_Engine::StartThreadL(TInt aThreadNumber)
    {
    INFO_PRINTF2(_L("Attempt to start thread %d using base-class StartThreadL"), aThreadNumber);
    User::Panic(_L("StartThreadL"), __LINE__);
    }


void CEgltest_Local_Engine::EndThread(TInt aThreadNumber)
    {
    INFO_PRINTF2(_L("Attempt to end thread %d using base-class EndThread"), aThreadNumber);
    User::Panic(_L("EndThread"), __LINE__);
    }

void CEgltest_Local_Engine::RunControllerLocalAndRemoteL(const TEngineTestCase& aCase, const TRemoteTestParams& aParams)
    {
    if (aCase.iFlags & EDebugController)
        {
        switch(aCase.iCase)
            {
            case EBreakPointCase:
                __BREAKPOINT();
                break;
            case ELogEnableCase:
                iLogging = ETrue;
                break;
            case EEndLoadThreadCase:
                EndThread(aCase.iEndpointIndex);
                break;
            case EStartLoadThreadCase:
                StartThreadL(aCase.iEndpointIndex);
                break;
            case EPanicCase:
                User::Panic(_L("EPanicCase"), -1);
                break;
            case ESetVerdictCase:
                SetTestStepResult(static_cast<TVerdict>(aCase.iEndpointIndex));
                break;
            default:
                ERR_PRINTF2(_L("Unrecognised case %d"), aCase.iCase);
                User::Panic(_L("BADCASE"), -2);
                break;
            }
        }
    if (aCase.iFlags & EDebugExecThread)
        {
        SendLocalTestCase(aParams);
        }
    if (aCase.iFlags & EDebugRemote)
        {
        RunRemoteTestCase(aCase.iCase, aParams);
        }
    }


// This function runs one set of table entries for one surface type. 
void CEgltest_Local_Engine::RunSingleCaseL(const TTestCase& aTestCase, TSurfaceType aSurfType)
    {
    const TTestCase& cs = aTestCase;

    TRemoteTestParams params;
    TExecResult result;

    for(TInt j = 0; cs.iEngineTestCase[j].iCase != EFinishedCase; j++)
        {
        const TEngineTestCase& ec = cs.iEngineTestCase[j];
        params.iEndpointEngine.iEngineTestCase = ec;
        params.iEndpointEngine.iSurfaceParams.iSurfaceType = aSurfType;
        
        if (iLogging)
            {
            LogDump(Logger(), ec);
            }

        switch(ec.iCase)
            {
            // Work done locally. 
            case ECreateSurfaceCase:
            case EDestroySurfaceCase:
            case EDrawContentCase:
            case ENotifyWhenCase:
            case EWaitForCase:
            case ETimeStampCase:
                SendLocalTestCase(params);
                break;
                
            // Special case: 
            case EContentUpdateCase:
                if (ec.iFlags & ENoSync)
                    {
                    // We sync here to make sure local commands sent
                    // BEFORE the ContentUpdate have completed!
                    DoSyncToLocal();
                    }
                SendLocalTestCase(params);
                if (ec.iFlags & ENoSync)
                    {
                    // Delay an arbitrary amount to allow SUS
                    // to receive the update!
                    User::After(300 * 1000);  // 300 ms.
                    iExecState = EExecStateRemote;
                    }
                break;
                
            case EBufferCountCase:
                RunLocalTestCase(params, result);
                if (result.iBufferCountVerdict == EFail)
                    {
                    RunRemoteTestCase(ETerminateCase, params);
                    }
                break;
                
            // These commands are all sent to the remote side.
            // By definition, they are synchronous, as the RunRemoteTestCase
            // waits for a reply from the remote side before returning.
            case EAcquireImageCase:
            case EEndStreamingCase:
            case EReleaseImageCase:
            case EDestroyEndpointCase:
            case EBeginStreamingCase:
            case ETerminateCase:
            case EGetAttribCase:
            case ESetAttribCase:
            case ECompareImageCase:
            case ECreateVgImageCase:
            case EDestroyVgImageCase:
            case EDestroyEglImageCase:
            case ETestVgImageValidCase:
            case ERequestNotificationCase:
            case ECancelNotificationCase:
            case EWaitForNotificationCase:
            case EGetEndpointDirtyAreaCase:
            case ECheckForMemoryLeaks:
            case ECheckForMemoryLeaksFinish:
            case ESpecialEglHeapCase:
                RunRemoteTestCase(ec.iCase, params);
                break;

            // Special case: Need surface parameters filled in first. 
            case ECreateEndpointCase:
                {
                // Copy parameters.
                TRemoteTestParams tempParams = params;
                tempParams.iEndpointEngine.iEngineTestCase.iCase = EGetSurfaceParamsCase;
                RunLocalTestCase(tempParams, result);
                params.iEndpointEngine.iSurfaceParams = result.iSurfaceParams;
               
                // The above code syncs the execution so no need to sync again!
                iExecState = EExecStateRemote;
                RunRemoteTestCase(ec.iCase, params);
                }
                break;
                
            case EInitializeCase:
                SendLocalTestCase(params);
                RunRemoteTestCase(ec.iCase, params);
                break;
                

            // These commands are controlled by the EDebugXXX flags
            // They may go to this thread, the exectrhead or the remote RS.
            case EStartLoadThreadCase:
            case EEndLoadThreadCase:
            case ELogEnableCase:
            case EBreakPointCase:
            case EPanicCase:
            case ESetVerdictCase:
                RunControllerLocalAndRemoteL(ec, params);
                break;
                
           default:
                ERR_PRINTF2(_L("Unknown case: %d"), ec.iCase);
                SetTestStepResult(EFail);
                break;
            }
        }
    // Tell local execution thread we are finished.
    RunLocalTestCase(EFinishedCase);
    }

void CEgltest_Local_Engine::RunOneTestCase(const TTestCase& aTestCase, TSurfaceType aSurfType)
    {
    TRemoteTestParams nullParams = {};
    nullParams.iEndpointEngineConfig.iLogErrors = ETrue;
    
    StartRemoteTestStep(nullParams);

    TRAPD(err, RunSingleCaseL(aTestCase, aSurfType));
    if (err != KErrNone)
        {
        INFO_PRINTF2(_L("RunSingleCaseL left with an error: %d"), err);
        }

    EndRemoteTestStep(nullParams);
    }
