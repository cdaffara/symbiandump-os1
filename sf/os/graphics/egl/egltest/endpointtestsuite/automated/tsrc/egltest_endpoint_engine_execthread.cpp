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

#include "egltest_endpoint_engine_execthread.h"
#include <hal.h>

/* 
 * This class forms the execution thread for running EGL commands. 
 * This is primarily so that we can perform EglSwapBuffer (EContentUdpateCase)
 * BEFORE calling EglReleaseImageNOK on the remote end. Since EglSwapBuffer
 * does not complete until the EglReleaseImageNOK is called, we must have a
 * separate thread from the thread performing the table reading/controlling.
 * 
 * Here's a summary of the classes/threads, and what role they play:
 * class CEgltest_Local_Engine
 *    Controller for the execution of "engine" tables. 
 *    
 *    This runs in the thread created by the TestExecute Framework itself
 *     
 *    It coordinates the Local and Remote execution of commands from
 *    the table. 
 *    
 *    When the command sequence switches from local to remote a "command" of 
 *    ESyncLocalCase is automatically inserted into the local command stream,
 *    and the controller waits for the reply from this Sync before continuing
 *    with the remote command. There is one exception: when the 
 *    EContentUpdateCase command is issued with ENoSync flag set, a sync is 
 *    sent BEFORE the EContentUpdateCase, but no further Sync is performed
 *    until the remote thread is complete.
 *    
 *    The controller also creates a thread to monitor the controller itself
 *    and the CEgltest_Local_Engine_Exec thread. See further comemnts in the
 *    source of that class.
 *    
 * class CEgltest_Local_Engine_Exec
 *    Runs in a thread created by the CEgltest_Local_Engine thread.
 *    
 *    Performs the Actual "client" commands for the test-sequence. The reason
 *    for a separate execution thread is outlined above. The reason ALL commands
 *    must be executed in this thread is that certain EGL commands (e.g. 
 *    SwapBuffer) must be executed by a single thread.
 *     
 * class CEgltest_Remote_Engine
 *    Runs in a thread created by the Test Renderstage code. 
 *    
 *    This thread performs the EglEndpointNOK specific calls. It also does 
 *    various image comparisons. All commands sent to this thread will be 
 *    synchronous with the controller execution - meaning that a remote 
 *    command will be finished by the time the next command in the controller
 *    is issued.
 */

const TInt KTerminated = 0xAABBCCDD;

CEgltest_Local_Engine_Exec::CEgltest_Local_Engine_Exec() 
    : iLogging(EFalse), iVerdict(EPass), iSurfaceTypeDisplayed(EFalse)
    {
    HAL::Get(HALData::EFastCounterFrequency, iFastFreq);
    }


CEgltest_Local_Engine_Exec::~CEgltest_Local_Engine_Exec()
    {
    iParamsInQueue.Close();
    iResultOutQueue.Close();
    }


CEgltest_Local_Engine_Exec* CEgltest_Local_Engine_Exec::NewL()
    {
    CEgltest_Local_Engine_Exec *self = new (ELeave) CEgltest_Local_Engine_Exec();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CEgltest_Local_Engine_Exec::ConstructL()
    {
    TInt err = iResultOutQueue.OpenGlobal(KExecResultQueueName, EOwnerThread);
    ENGINE_ASSERT(err == KErrNone);
    User::LeaveIfError(err);
    err = iParamsInQueue.OpenGlobal(KExecParamsQueueName, EOwnerThread);
    ENGINE_ASSERT(err == KErrNone);
    User::LeaveIfError(err);
    }

TInt CEgltest_Local_Engine_Exec::ThreadEntry(TAny */* aDummy */)
    {
    CTrapCleanup *cleanUpStack = CTrapCleanup::New();
    if (!cleanUpStack)
       {
       // Can't use INFO_PRINTF here, as we have not yet
       // created the logger object - nor can we until we have
       // a working cleanupstack, so we just do our best at a 
       // reason able error message.
       RDebug::Printf("Could not allocate memory for cleanupStack!");
       User::Panic(_L("ExecThread"), __LINE__);
       return KErrNoMemory;
       }

    TRAPD(err, ThreadEntryL());
    delete cleanUpStack;
    if (err != KErrNone)
        {
        RDebug::Printf("Thread left with err=%d", err);
        User::Panic(_L("ExecThread"), __LINE__);
        }
    return err;
    }


void CEgltest_Local_Engine_Exec::ThreadEntryL()
    {
    CEgltest_Local_Engine_Exec *self = CEgltest_Local_Engine_Exec::NewL(); 
    CleanupStack::PushL(self);
    TInt err = KErrNone;
    TInt ret = 0;
    do {
        TRAP(err, ret = self->ThreadLoopL());
        if (err != KErrNone)
            {
            self->SetTestStepResult(EFail);
            RDebug::Printf("%s:%d: Leaving with %d", __FILE__, __LINE__, err);
            User::Leave(err);
            }
        self->Logger().Close();
    } while(ret != KTerminated);
    self->TidyUp();
    CleanupStack::PopAndDestroy(self);
    }


// Initialize EGL, etc. 
void CEgltest_Local_Engine_Exec::SetUpL()
    {
    iDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLint err;
    if (iDisplay == EGL_NO_DISPLAY)
        {
        err = eglGetError();
        INFO_PRINTF2(_L("eglGetDisplay failed: err = %x"), err);
        User::Leave(KErrNotSupported);
        }

    if (!eglInitialize(iDisplay, NULL, NULL))
        {
        err = eglGetError();
        INFO_PRINTF2(_L("EglInitialize failed: err = %x"), err);
        User::Leave(KErrNotSupported);
        }
    }

void CEgltest_Local_Engine_Exec::TidyUp()
    {
    // Clean up. 
    iSurfaceTypeDisplayed = EFalse;
    for(TInt i = 0; i < KMaxEndpoints; i++)
        {
        if(iSurfaces[i])
            {
            delete iSurfaces[i];
            iSurfaces[i] = NULL;
            }
        }
    eglTerminate(iDisplay);
    eglReleaseThread();
    }

void CEgltest_Local_Engine_Exec::SetTestStepResult(TVerdict aVerdict)
    {
    iVerdict = aVerdict;
    }

TVerdict CEgltest_Local_Engine_Exec::TestStepResult() const
    {
    return iVerdict;
    }
    
void CEgltest_Local_Engine_Exec::SendResult(const TExecResult &aResult)
    {
    iResultOutQueue.SendBlocking(aResult);
    }

TInt CEgltest_Local_Engine_Exec::ThreadLoopL()
    {
    TBool testFinished = EFalse;
    TRemoteTestResult result;

    TEndpointUtil::SetLoggerForProcessWrapperL(iLogger);
    do
        {
        TRemoteTestParamsPacket params;
        iParamsInQueue.ReceiveBlocking(params);
       
        const TEngineTestCase& ec = params.iParams.iEndpointEngine.iEngineTestCase;

        TInt index = ec.iImageIndex;
        TInt endpointIndex = ec.iEndpointIndex;
        
        if (iLogging)
            {
            LogDump(Logger(), ec);
            }

        switch(ec.iCase)
            {
            case EInitializeCase:
                TidyUp();
                SetUpL();
                break;
                
            case ECreateSurfaceCase:
                {
                ENGINE_ASSERT(endpointIndex < KMaxEndpoints);
                ENGINE_ASSERT(!iSurfaces[endpointIndex]);
                TSurfaceType surfType = params.iParams.iEndpointEngine.iSurfaceParams.iSurfaceType;
                iSurfaces[endpointIndex] = CSurface::SurfaceFactoryL(surfType);
                TRAPD(err, iSurfaces[endpointIndex]->CreateL(index));
                if (err == KErrNone)
                    {
                    if (!iSurfaceTypeDisplayed)
                        {
                        INFO_PRINTF4(_L("Using surfaces of type %s (%dx%d pixels)"), 
                                iSurfaces[endpointIndex]->GetSurfaceTypeStr(),
                                        iSurfaces[endpointIndex]->Size().iWidth,
                                        iSurfaces[endpointIndex]->Size().iHeight);
                        iSurfaceTypeDisplayed = ETrue;
                        }
                    }
                else
                    {
                    INFO_PRINTF2(_L("Could not create surface, err=%d"), err);
                    delete iSurfaces[endpointIndex];
                    iSurfaces[endpointIndex] = NULL;
                    SetTestStepResult(EFail);
                    }
                }
                break;
                
            case EDestroySurfaceCase:
                {
                delete iSurfaces[endpointIndex];
                iSurfaces[endpointIndex] = NULL;
                }
                break;

            case EContentUpdateCase:
                {
                TInt err;
                if (!iSurfaces[endpointIndex])
                    {
                    err = KErrNotSupported;
                    }
                else
                    {
                    err = iSurfaces[endpointIndex]->SubmitContent(!(ec.iFlags & ENoWait), index);
                    }
                
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("ContentUpdate failed, err=%d"), err);
                    SetTestStepResult(EFail);
                    }
                }
                break;

            case EDrawContentCase:
                if (iSurfaces[endpointIndex])
                    {
                    TRAPD(err, iSurfaces[endpointIndex]->DrawContentL(index % CTestImage::KImageCount));
                    if (err != KErrNone)
                        {
                        SetTestStepResult(EFail);
                        }
                    }
                else
                    {
                    SetTestStepResult(EFail);
                    }
                break;

            case EBufferCountCase:
                {
                TBool pass = ETrue;
                TSurfaceParamsRemote surfParams;
                TInt buffers = 0;
                TInt min = ec.iArg1;
                TInt max = ec.iArg2;

                if (!iSurfaces[endpointIndex])
                    {
                    pass = EFalse;
                    }
                else
                    {
                    iSurfaces[endpointIndex]->GetSurfaceParamsL(surfParams);
                    buffers = surfParams.iCommonParams.iBuffers;
                    if (min && buffers < min)
                        {
                        pass = EFalse;
                        }
                    if (max && buffers > max)
                        {
                        pass = EFalse;
                        }
                    }
                if (!pass)
                    {
                    INFO_PRINTF4(_L("Surface has %d buffers, test expect [%d..%d] buffers (0 = 'any number') - Test ignored (pass)"),
                            buffers, min, max);
                    SendResult(TExecResult(EFail, EPass, ec.iCase));
                    testFinished = ETrue;
                    }
                else
                    {
                    SendResult(TExecResult(EPass, EPass, ec.iCase));
                    }
                }
                break;

            case ENotifyWhenCase:
                {
                if (!iSurfaces[endpointIndex])
                    {
                    SetTestStepResult(EFail);
                    }
                else
                    {
                    TInt err = iSurfaces[endpointIndex]->Notify((TNotification)index, iStatus[endpointIndex], ec.iArg1);
                    if (err != ec.iErrorExpected)
                        {
                        ERR_PRINTF4(_L("Wrong error code from 'NotifyWhen' for notifiction %d - error %d, expected %d"),
                                    index, err, ec.iErrorExpected);
                        SetTestStepResult(EFail);
                        }
                    }
                }
                break;

            case EWaitForCase:
                {
#if defined (__WINS__)
                const TInt KDiffAllowed = 2500000;   // Max 2500ms difference. Not realy testing anything.
#else
                const TInt KDiffAllowed = 25000;   // Max 25ms difference.
#endif
                TUint32 beginTimeStamp = iTimeStamp[endpointIndex];
                TUint32 endTimeStamp = User::FastCounter();
                ENGINE_ASSERT(endpointIndex < KMaxEndpoints);
                if (!iSurfaces[endpointIndex])
                    {
                    SetTestStepResult(EFail);
                    }
                else
                    {
                    TInt err = iSurfaces[endpointIndex]->WaitFor((TNotification)index,
                            iStatus[endpointIndex], ec.iArg1, endTimeStamp);
    
                    //Now, figure out the delta in microseconds.
                    TUint32 deltaTime = endTimeStamp - beginTimeStamp;
                    deltaTime *= 1000;
                    deltaTime /= (iFastFreq / 1000);
    
                    if (err != ec.iErrorExpected)
                        {
                        ERR_PRINTF4(_L("Wrong error code from 'WaitFor' for notifiction %d - error %d, expected %d"),
                                    index, err, ec.iErrorExpected);
                        INFO_PRINTF5(_L("Timeout: %d, beginTimeStamp = %u, endTimeStamp = %u, deltaTime = %u"),
                                     ec.iArg1, beginTimeStamp, endTimeStamp, deltaTime);
                        SetTestStepResult(EFail);
                        }
                    // If iArg2 is non-zero, and we waited for "displayed" and no error, check the timestamp.
                    if (index == ENotifyWhenDisplayed && err == KErrNone && ec.iArg2)
                        {
                        if (Abs((TInt)deltaTime - ec.iArg2) > KDiffAllowed)
                            {
                            ERR_PRINTF3(_L("TimeStamp is incorrect - expected %d microseconds, got %d microseconds"),
                                    ec.iArg2, deltaTime);
                            INFO_PRINTF4(_L("original timestamp: %u, endpoint ts=%u, iFastFreq=%u"),
                                    endTimeStamp, iTimeStamp[endpointIndex], iFastFreq);
                            SetTestStepResult(EFail);
                            }
                        }
                    }
                }
                break;

            case ETimeStampCase:
                iTimeStamp[endpointIndex] = User::FastCounter();
                break;
                
            case EFinishedCase:
                SendResult(TExecResult(TestStepResult(), ec.iCase));
                testFinished = ETrue;
                break;
                
            case EBreakPointCase:
                __BREAKPOINT();
                break;
                
            case ELogEnableCase:
                iLogging = ETrue;
                break;
                
            case ESyncLocalCase:
                {
                SendResult(TExecResult(TestStepResult(), ec.iCase));
                }
                break;
                
            case EGetSurfaceParamsCase:
                {
                TExecResult result(TestStepResult(), ec.iCase);
                if (iSurfaces[endpointIndex])
                    {
                    iSurfaces[endpointIndex]->GetSurfaceParamsL(result.iSurfaceParams);
                    }
                SendResult(result);
                }
                break;
                
            case ETerminateCase:
                SendResult(TExecResult(TestStepResult(), ec.iCase));
                return KTerminated;
                
            case ESetVerdictCase:
                {
                SetTestStepResult(static_cast<TVerdict>(endpointIndex));
                TExecResult result(TestStepResult(), ec.iCase);
                SendResult(result);
                }
                break;
                
            case EPanicCase:
                // This part is intended to be used to test the implementation.
                // In normal tests, this functionality should not be used.
                // If anyone decides to use this for some purpose in normal
                // code, then please change the above comment to reflect
                // that it IS used, and explain why it makes sense.
                INFO_PRINTF1(_L("Performing intentional panic!"));
                User::Panic(_L("EPanicCase"), -1);
                break;
                
            default:
                ERR_PRINTF2(_L("Unknown case: %d"), ec.iCase);
                ENGINE_ASSERT(0);
                break;
            }
        }
    while(!testFinished);
    return KErrNone;
    }
