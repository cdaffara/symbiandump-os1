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


/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


/*
 * This file contains the parts of CRemoteTestEnv and CRemoteTestStepBase
 * that do not need editting when adding new tests.
 *
 * CRemoteTestEnv acts as the controller. It provides the means of communicating
 * with the local side and it instantiates remote test steps and executes them in
 * their own thread. It monitors this thread for panic / timeout. If this
 * happens, it informs the local side.
 */


#include "remotetestbase.h"
#include <e32debug.h>
#include <e32math.h>


// Default timout for remote test steps. This MUST be smaller
// than any timeout passed to TEF for the local test step
// in the script file. Remote test steps can override this 
// value by implementing the Timeout() function in their 
// derrived class.
const TInt KRemoteTestStepTimeout = 10 * 1000000;


//Active object used to generate a timeout if worker thread takes too long. ------

CTimeoutTimer* CTimeoutTimer::NewL(CRemoteTestEnv& aEnv, TInt aPriority)
    {
    CTimeoutTimer* obj = new (ELeave) CTimeoutTimer(aEnv, aPriority);
    CleanupStack::PushL(obj);
    obj->ConstructL();
    CleanupStack::Pop(obj);
    return obj;
    }


CTimeoutTimer::CTimeoutTimer(CRemoteTestEnv& aEnv, TInt aPriority) :
    CTimer(aPriority),
    iEnv(aEnv)
    {
    }


void CTimeoutTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    }


CTimeoutTimer::~CTimeoutTimer()
    {
    Cancel();
    }


void CTimeoutTimer::RunL()
    {
    ENDPOINT_ASSERT_DEBUG(iStatus.Int() == KErrNone, User::Invariant());
    iEnv.TestCaseTimedOut();
    }

//--------------------------------------------------------------------------------


//Active object used to listen to the worker thread to see if it panics. ---------

CWorkerListener* CWorkerListener::NewL(CRemoteTestEnv& aEnv, TInt aPriority)
    {
    CWorkerListener* obj = new (ELeave) CWorkerListener(aEnv, aPriority);
    CleanupStack::PushL(obj);
    obj->ConstructL();
    CleanupStack::Pop(obj);
    return obj;
    }


CWorkerListener::CWorkerListener(CRemoteTestEnv& aEnv, TInt aPriority) :
    CActive(aPriority),
    iEnv(aEnv)
    {
    }


CWorkerListener::~CWorkerListener()
    {
    Cancel();
    }


void CWorkerListener::ConstructL()
    {
    CActiveScheduler::Add(this);
    }


void CWorkerListener::Listen(RThread& aThread)
    {
    ENDPOINT_ASSERT_DEBUG(!IsActive(), User::Invariant());
    iThread = &aThread;
    iThread->Logon(iStatus);
    SetActive();
    }


void CWorkerListener::RunL()
    {
    iEnv.WorkerExitted();
    }


void CWorkerListener::DoCancel()
    {
    iThread->LogonCancel(iStatus);
    }

//--------------------------------------------------------------------------------


//Active object used to listen for test case completion from the worker thread. --

CTestCaseListener* CTestCaseListener::NewL(CRemoteTestEnv& aEnv, TInt aPriority)
    {
    CTestCaseListener* obj = new (ELeave) CTestCaseListener(aEnv, aPriority);
    CleanupStack::PushL(obj);
    obj->ConstructL();
    CleanupStack::Pop(obj);
    return obj;
    }


CTestCaseListener::CTestCaseListener(CRemoteTestEnv& aEnv, TInt aPriority) :
    CActive(aPriority),
    iEnv(aEnv)
    {
    }


CTestCaseListener::~CTestCaseListener()
    {
    Cancel();
    }


void CTestCaseListener::ConstructL()
    {
    CActiveScheduler::Add(this);
    }


void CTestCaseListener::Listen()
    {
    ENDPOINT_ASSERT_DEBUG(!IsActive(), User::Invariant());
    iStatus = KRequestPending;
    SetActive();
    }


void CTestCaseListener::RunL()
    {
    ENDPOINT_ASSERT_DEBUG(iStatus.Int() == KErrNone, User::Invariant());
    iEnv.TestCaseCompleted();
    }


void CTestCaseListener::DoCancel()
    {
    //There is no way to actually cancel a test case,
    //But this AO will only be cancelled when the thread
    //has panicked or timed out - which is as good as a
    //cancel. We still need to call canel on the AO though
    //to set it as inactive.

    //Also need to do a request complete if the worker has not already
    //done it. There is no danger of the worker completing between our
    //check (the if) and actually completing because the worker is dead.
    if(iStatus.Int() == KRequestPending)
        {
        TRequestStatus* myStatus = &iStatus;
        User::RequestComplete(myStatus, KErrCancel);
        }
    }

//--------------------------------------------------------------------------------


//CRemoteTestEnv -----------------------------------------------------------------

CRemoteTestEnv* CRemoteTestEnv::NewL()
    {
    CRemoteTestEnv* obj = new (ELeave) CRemoteTestEnv();
    CleanupStack::PushL(obj);
    obj->ConstructL();
    CleanupStack::Pop(obj);
    return obj;
    }


CRemoteTestEnv::CRemoteTestEnv() :
    CActive(CActive::EPriorityStandard)
    {
    }


void CRemoteTestEnv::ConstructL()
    {
    //Create the message queues.
    User::LeaveIfError(iResultOutQueue.CreateGlobal(KResultQueueName, 5));
    User::LeaveIfError(iParamsInQueue.CreateGlobal(KParamsQueueName, 1));

    iSupervisorId = RThread().Id();

    //Create AOs that monitor for events.
    //These priorities are important, since if, for example, the worker
    //thread exits by returning from the thread entrypoint after
    //successfully completing the EndTestStep() case, but before the
    //supervisor runs, the supervisor's AS will find that both iWorkerListener
    //and iTestCaseListener have completed. We use the priorities to determine
    //which signal to run first. (The one we run will cancel the others).
    iTimeoutTimer = CTimeoutTimer::NewL(*this, CActive::EPriorityLow);
    iWorkerListener = CWorkerListener::NewL(*this, CActive::EPriorityStandard);
    iTestCaseListener = CTestCaseListener::NewL(*this, CActive::EPriorityHigh);

    //Add self to active scheduler.
    CActiveScheduler::Add(this);
    }


CRemoteTestEnv::~CRemoteTestEnv()
    {
    Cancel();
    delete iTimeoutTimer;
    delete iWorkerListener;
    delete iTestCaseListener;
    iParamsInQueue.Close();
    iResultOutQueue.Close();
    }


void CRemoteTestEnv::StartReceivingCmds()
    {
    ReceiveCmd();
    CActiveScheduler::Start();
    }


void CRemoteTestEnv::ReceiveCmd()
    {
    ENDPOINT_ASSERT_DEBUG(!IsActive(), User::Invariant());
    iParamsInQueue.NotifyDataAvailable(iStatus);
    SetActive();
    }


//This is run when an packet arrives in the queue from the local side.
//It is not rearmed until the test step has run to completion.
void CRemoteTestEnv::RunL()
    {
    //Retrieve the packet from the queue.
    TInt err = iParamsInQueue.Receive(iCurTestCaseParamsPacket);
    ENDPOINT_ASSERT_DEBUG(err == KErrNone, User::Invariant());
    
    //Create the appropriate TestStep and launch thread if this is a "StartTestStep".
    if(iCurTestCaseParamsPacket.iTestCase == KStartTestStepCaseNumber)
        {
        //At this point in a well behaved system, iCurTestStep must be NULL.
        //If it is not, comms has gone wrong (most likely the local side has 
        //panicked midway through the test step). If iCurTestStep is not NULL
        //we can also guarantee that the thread is still running. Recover 
        //from this by getting the thread to do some special teardown, 
        //followed by tidying up in this thread.
        if(iCurTestStep)
            {
            //Logon to worker then signal to abort the test case
            //and wait for completion.
            TRequestStatus threadStat;
            iCurWorker.Logon(threadStat);
            TRequestStatus* notifyRunTestCase = &iNotifyRunTestCase;
            iCurWorker.RequestComplete(notifyRunTestCase, KErrAbort);
            User::WaitForRequest(threadStat);
            
            //Tidy up.
            iCurWorker.Close();
            delete iCurTestStep;
            iCurTestStep = NULL;
            }
        
        TBool result = SetupTestStep();

        //If we failed to setup the test step (invalid uid),
        //just register for another command and return.
        if(!result)
            {
            //Register to receive another packet from the message queue.
            ReceiveCmd();
            return;
            }
        }

    //Activate the TimoutTimer, TestCaseListener and WorkerListener.
    iTimeoutTimer->After(iCurTestStep->Timeout());
    iTestCaseListener->Listen();
    iWorkerListener->Listen(iCurWorker);

    //Signal the worker thread to start a test case.
    TRequestStatus* notifyRunTestCase = &iNotifyRunTestCase;
    iCurWorker.RequestComplete(notifyRunTestCase, KErrNone);
    }


void CRemoteTestEnv::DoCancel()
    {
    iParamsInQueue.CancelDataAvailable();
    }


//The test case can end in three ways:
//    1. Test Case compeletes normally (this includes failing).
//    2. The Test Case times out (in which case the thread is panicked).
//    3. The Test Case panics the worker thread.
//Three AOs listen for each of these possibilities and one of the functions below.


//This is called for case 1.
void CRemoteTestEnv::TestCaseCompleted()
    {
    //Cancel the TimeoutTimer and WorkerListener.
    iTimeoutTimer->Cancel();
    iWorkerListener->Cancel();

    //Test case completed correctly, so send test result.
    SendResult(iCurTestCaseVerdict);

    //Tidy up if this is the end of the test step.
    if(iCurTestCaseParamsPacket.iTestCase == KEndTestStepCaseNumber)
        {
        iCurWorker.Close();
        delete iCurTestStep;
        iCurTestStep = NULL;
        }

    //Register to receive another packet from the message queue.
    ReceiveCmd();
    }


//This is called for case 2.
void CRemoteTestEnv::TestCaseTimedOut()
    {
    //Cancel the TestCaseListener and WorkerListener.
    iTestCaseListener->Cancel();
    iWorkerListener->Cancel();

    //Thread timed out so log that it timed out and send the ERtvTimeout result.
    iCurTestStep->REMOTE_ERR_PRINTF1(_L("Remote test step timed out."));
    SendResult(ERtvTimeout);

    //Tidy up. Because we timed out, we abandon the test step, so
    //kill the thread and release even if this was not an EndTestStep
    iCurWorker.Kill(KErrTimedOut);
    iCurWorker.Close();
    delete iCurTestStep;
    iCurTestStep = NULL;

    //Register to receive another packet from the message queue.
    ReceiveCmd();
    }


//This is called for case 3.
void CRemoteTestEnv::WorkerExitted()
    {
    //Cancel the TimeoutTimer and TestCaseListener.
    iTimeoutTimer->Cancel();
    iTestCaseListener->Cancel();

    //Even if we were running a EndTestStep (ie the thread will exit normally), TestCaseListener should still
    //fire first, and it will cancel the WorkerListener - so we know that if we get here, it is because the
    //thread exitted abnormally.

    //Thread was panicked, so log the panic category before sending the ERtvPanic result.
    TExitCategoryName exitCategory = iCurWorker.ExitCategory();
    iCurTestStep->REMOTE_ERR_PRINTF3(_L("Remote test step panicked with: %S, code = %d."), &exitCategory, iCurWorker.ExitReason());
    SendResult(ERtvPanic);

    //Tidy up. Because we panicked, we abandon the test step, so
    //release resources even if this was not an EndTestStep
    iCurWorker.Close();
    delete iCurTestStep;
    iCurTestStep = NULL;

    //Register to receive another packet from the message queue.
    ReceiveCmd();
    }


TBool CRemoteTestEnv::SetupTestStep()
    {
    //Set the TRequestStatus that the worker thread triggers off for the first time.
    //After this, the worker thread will set it back to KRequestPending itself.
    iNotifyRunTestCase = KRequestPending;

    //Create TestStep
    TRAPD(err, iCurTestStep = CreateRemoteTestStepL(iCurTestCaseParamsPacket.iUid));
    if(err == KErrUnknown)
        {
        //Unknown test step. Tell the driver app.
        SendResult(ERtvUnknownTestUid);
        return EFalse;
        }
    else if(err != KErrNone || !iCurTestStep)
        {
        User::Invariant();
        }

    //Construct the test step base class.
    TRAP(err, iCurTestStep->ConstructL(*this));
    __ASSERT_ALWAYS(err == KErrNone, User::Invariant());

    //Create Test Thread.
    static const TInt KStackSize =   0x2000;      //  8KB
    static const TInt KHeapMinSize = 0x1000;      //  4KB
    static const TInt KHeapMaxSize = 0x1000000;   // 16MB
    TUint32 random = Math::Random();
    TName threadName;
    _LIT(KThreadNameFormat, "%S-%u");
    _LIT(KExecName, "EpTestRemoteExec");
    threadName.Format(KThreadNameFormat, &KExecName, random);
    err = iCurWorker.Create(threadName, TestThreadEntryPoint, KStackSize, KHeapMinSize, KHeapMaxSize, this);
    __ASSERT_ALWAYS(err == KErrNone, User::Invariant());

    //Start the test thread.
    iCurWorker.Resume();

    return ETrue;
    }


// The DoEglHeapMark and DoEglHeapCheck are intended to make sure memory
// allocations are freed when the testing is complete. The current
// implementation only supports the Symbian/Nokia reference implementation.
// An implementor of another EGL implementation is free to add their own
// variant of heapchecking here, with suitable #if around it.
// The function in egl should call __DbgMarkStart() and __DbgMarkEnd()
// on the heap for the egl implementation - or the equivalent if the
// heap is not a typical Symbian heap.
void CRemoteTestEnv::DoEglHeapMark()
    {
#if USE_EGLHEAP_CHECKING
    typedef void (*TEglDebugHeapMarkStartPtr)();

    TEglDebugHeapMarkStartPtr  heapMarkStart = reinterpret_cast<TEglDebugHeapMarkStartPtr>(eglGetProcAddress("egliDebugHeapMarkStart"));
    if (heapMarkStart)
        {
        heapMarkStart();
        }
#endif
    }

void CRemoteTestEnv::DoEglHeapCheck()
    {
#if USE_EGLHEAP_CHECKING
    typedef EGLint (*TEglDebugHeapMarkEndPtr)(EGLint count);

    TEglDebugHeapMarkEndPtr heapMarkEnd = reinterpret_cast<TEglDebugHeapMarkEndPtr>(eglGetProcAddress("egliDebugHeapMarkEnd"));
    if (heapMarkEnd)
        {
        (void)heapMarkEnd(0);
        }
#endif
    }

#define __EGLHEAP_MARK   DoEglHeapMark()
#define __EGLHEAP_MARKEND  DoEglHeapCheck()


void CRemoteTestEnv::RunCurrentTestStepL()
    {
    TInt processHandleMarkDummy;
    TInt threadHandleMarkStart;
    TInt threadHandleMarkEnd;
    TBool finished = EFalse;

    while(!finished)
        {
        //Wait to be signalled to run a test case.
        User::WaitForRequest(iNotifyRunTestCase);
        
        //We are aborting the test step. Tidy up EGL and exit.
        if(iNotifyRunTestCase.Int() == KErrAbort)
            {
            iCurTestStep->EglEndL();
            iNotifyRunTestCase = KRequestPending;
            return;
            }
        
        //Rearm the TRequestStatus (The first arming is done in the supervisor thread).
        iNotifyRunTestCase = KRequestPending;

        //Run the test case and panic if it leaves. Start/End are just special test cases.
        if(iCurTestCaseParamsPacket.iTestCase == KStartTestStepCaseNumber)
            {
            //Mark the user heap & thread handle count (we don't care about the process handle count).
            RThread().HandleCount(processHandleMarkDummy, threadHandleMarkStart);
            __UHEAP_MARK;
            __EGLHEAP_MARK;

            //StartRemoteTest.
            TRAPD(err, iCurTestCaseVerdict = iCurTestStep->DoStartRemoteTestStepL(iCurTestCaseParamsPacket.iParams));
            __ASSERT_ALWAYS(err == KErrNone, User::Panic(_L("tried to leave."), __LINE__));
            }
        else if(iCurTestCaseParamsPacket.iTestCase == KEndTestStepCaseNumber)
            {
            //EndRemoteTest.
            TRAPD(err, iCurTestCaseVerdict = iCurTestStep->DoEndRemoteTestStepL(iCurTestCaseParamsPacket.iParams));
            __ASSERT_ALWAYS(err == KErrNone, User::Panic(_L("tried to leave."), __LINE__));

            //This will cause a panic if the test step leaked memory or thread handles.
            __UHEAP_MARKEND;
            __EGLHEAP_MARKEND;
            RThread().HandleCount(processHandleMarkDummy, threadHandleMarkEnd);
            __ASSERT_ALWAYS(threadHandleMarkStart == threadHandleMarkEnd, User::Panic(_L("leaked handles."), KErrBadHandle));

            //Exit the loop (and eventually the thread).
            finished = ETrue;
            }
        else
            {
            //Run a regular Test Case.
            TRAPD(err, iCurTestCaseVerdict = iCurTestStep->DoRunRemoteTestCaseL(iCurTestCaseParamsPacket.iTestCase, iCurTestCaseParamsPacket.iParams));
            __ASSERT_ALWAYS(err == KErrNone, User::Panic(_L("tried to leave."), __LINE__));
            }

        //Notify the supervisor that we have completed the test case.
        RThread supervisor;
        TInt err = supervisor.Open(iSupervisorId);
        __ASSERT_ALWAYS(err == KErrNone, User::Panic(_L("framework error."), __LINE__));
        TRequestStatus* notifyFinishTestCase = &iTestCaseListener->iStatus;
        supervisor.RequestComplete(notifyFinishTestCase, KErrNone);
        supervisor.Close();
        }
    }


TInt CRemoteTestEnv::TestThreadEntryPoint(TAny* aSelf)
    {
    CRemoteTestEnv* self = static_cast<CRemoteTestEnv*>(aSelf);
    
    //Create cleanup stack.
    CTrapCleanup* cleanup = CTrapCleanup::New();
    ASSERT(cleanup);
    
    //Create active scheduler.
    CActiveScheduler* scheduler = new CActiveScheduler();
    ASSERT(scheduler);
    CActiveScheduler::Install(scheduler);
    
    TRAPD(err, self->RunCurrentTestStepL());
    __ASSERT_ALWAYS(err == KErrNone, User::Invariant());

    //Clean up.
    delete scheduler;
    delete cleanup;
    return KErrNone;
    }


void CRemoteTestEnv::SendResult(TRemoteTestVerdict aVerdict)
    {
    iResultOutQueue.SendBlocking(TRemoteTestResult(iCurTestCaseParamsPacket.iUid, iCurTestCaseParamsPacket.iTestCase, aVerdict));
    }


void CRemoteTestEnv::SendLog(const TDesC8& aFile, TInt aLine, TInt aSeverity, const TDesC& aMessage)
    {
    iResultOutQueue.SendBlocking(TRemoteTestResult(iCurTestCaseParamsPacket.iUid, iCurTestCaseParamsPacket.iTestCase, aFile, aLine, aSeverity, aMessage));
    }

//--------------------------------------------------------------------------------


//CRemoteTestStepBase ------------------------------------------------------------

CRemoteTestStepBase::CRemoteTestStepBase(TTestUid aUid) :
    iUid(aUid)
    {
    }


void CRemoteTestStepBase::ConstructL(CRemoteTestEnv& aTestEnv)
    {
    iTestEnv = &aTestEnv;
    if (iEndpoint.Error() != KErrNone)
        {
        RDebug::Printf("Could not construct CRemoteTestStepBase"
                " - is EglEndpointNOK enabled?? -- Error: %d", iEndpoint.Error());
        User::Leave(iEndpoint.Error());
        }
    }


CRemoteTestStepBase::~CRemoteTestStepBase()
    {
    }


TRemoteTestVerdict CRemoteTestStepBase::DoStartRemoteTestStepL(const TRemoteTestParams& /*aMessageIn*/)
    {
    //Default implementation does nothing.
    return ERtvPass;
    }


TRemoteTestVerdict CRemoteTestStepBase::DoEndRemoteTestStepL(const TRemoteTestParams& /*aMessageIn*/)
    {
    //Default implementation does nothing.
    return ERtvPass;
    }


TInt CRemoteTestStepBase::Timeout() const
    {
    return KRemoteTestStepTimeout;
    }


class TOverflowTruncate : public TDesOverflow
    {
public:
    virtual void Overflow(TDes& /*aDes*/)
        {
        //Do nothing - just let it truncate.
        }
    };


void CRemoteTestStepBase::Log(const TText8* aFile, TInt aLine, TInt aSeverity, TRefByValue<const TDesC> aFmt, ...)
    {
    if(iTestEnv)
        {
        TOverflowTruncate overflow;
        VA_LIST list;
        VA_START(list, aFmt);
        TBuf<0x100> buf;
        buf.AppendFormatList(aFmt, list, &overflow);
        TPtrC8 file(aFile);
        iTestEnv->SendLog(file, aLine, aSeverity, buf);
        }
    }


void CRemoteTestStepBase::EglStartL()
    {
    eglInitialize(eglGetDisplay(EGL_DEFAULT_DISPLAY), NULL, NULL);
    if (eglGetError()!=EGL_SUCCESS)
        {
        REMOTE_INFO_PRINTF1(_L("could not initialise egl"));
        User::Leave(KErrGeneral);
        }
    }


void CRemoteTestStepBase::EglEndL()
    {
    eglTerminate(eglGetDisplay(EGL_DEFAULT_DISPLAY));
    if (eglGetError()!=EGL_SUCCESS)
        {
        REMOTE_INFO_PRINTF1(_L("could not terminate egl"));
        User::Leave(KErrGeneral);
        }
    eglReleaseThread();
    }


const TEglEndpointWrap& CRemoteTestStepBase::EglEndpoint() const
    {
    return iEndpoint;
    }

//--------------------------------------------------------------------------------
