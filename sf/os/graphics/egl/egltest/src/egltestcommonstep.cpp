// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include <test/tefunit.h> // for ASSERT macros
#include <iniparser.h>
#include <apgtask.h>
#include <e32math.h>
#include <e32msgqueue.h> 

#include "egltestcommonstep.h"
#include "egltestcommonsession.h"
#include "egltestcommonutils.h"
#include "egltestcommonprocess.h"

static const TUint KDefaultHeapSize = 0x100000;
CEglTestStep::TThreadStatus::TThreadStatus()
    {
    for(TInt i=0; i<ESize; i++)
        {
        iStatus[i] = 0;
        }
    }

EXPORT_C CEglTestStep::CEglTestStep() : 
    iWaitForCompletionOnPostamble(EFalse),
	iSourceFormat(KDefaultSourceFormat),
	iSurfaceFormat(KDefaultSurfaceFormat)
	{
	}

EXPORT_C CEglTestStep::~CEglTestStep()
	{
	for (TInt i=0; i<KMaxProcessNumber; i++)
		{
		iProcessStatus[i].iProcess.Close();
		}
	TInt countThread = iThreadStatus.Count();
	for (TInt i=0; i<countThread; i++)
		{
		iThreadStatus[i].iThread.Close();
		}
	iThreadStatus.Close();

    iSemaphore[0].Close();
    iSemaphore[1].Close();
  
    CleanAll();
    CloseWsSession();
	}

EXPORT_C TVerdict CEglTestStep::doTestStepPreambleL()
	{
	User::LeaveIfError(Logger().ShareAuto());
	//When EGL Logging is enabled this causes a file server session to be allocated
	//Which needs to be done before any handle checks otherwise the test will fail
	ASSERT_EGL_TRUE(eglReleaseThread());	
	__UHEAP_MARK;
	HandleMark();
	return TestStepResult();
	}

EXPORT_C TVerdict CEglTestStep::doTestStepPostambleL()
	{
	if(iWaitForCompletionOnPostamble && (iThreadStatus.Count() > 0))
        {
        INFO_PRINTF1(_L("Main thread waits for other threads to be terminated!!"));
        Test_MultiThread_WaitL(ETrue, TThreadStatus::ELogin);
        }	
	
	if (iDisplay != EGL_NO_DISPLAY)
		{
		// Output a warning because this should be done by the test
		WARN_PRINTF1(_L("Terminating Display during doTestStepPostambleL"));
		ASSERT_EGL_TRUE(eglTerminate(iDisplay));
		iDisplay = EGL_NO_DISPLAY;
		}

	ASSERT_EGL_TRUE(eglReleaseThread());

	HandleMarkEnd();
 	__UHEAP_MARKEND;
	return TestStepResult();
	}

EXPORT_C void CEglTestStep::CleanAll()
	{
	delete iEglSess;
	iEglSess = NULL;
	
	if (iDisplay != EGL_NO_DISPLAY)
		{
		ASSERT_EGL_TRUE(eglTerminate(iDisplay));
		iDisplay = EGL_NO_DISPLAY;
		}
	ASSERT_EGL_TRUE(eglReleaseThread());
	}

/*****************************************************************************
 ** Utility methods
 *****************************************************************************/

void CEglTestStep::HandleMark()
	{
	RThread().HandleCount(iProcHandleMark, iThreadHandleMark);
	INFO_PRINTF3(_L("MARK: (%d) process-owned handle(s) / (%d) thread-owned handle(s)"), iProcHandleMark, iThreadHandleMark);
	}

void CEglTestStep::HandleMarkEnd()
	{
	RThread().HandleCount(iProcHandleMarkEnd, iThreadHandleMarkEnd);
	INFO_PRINTF3(_L("MARK-END: (%d) process-owned handle(s) / (%d) thread-owned handle(s)"), iProcHandleMarkEnd, iThreadHandleMarkEnd);
#ifdef __WINS__
	WARN_PRINTF1(_L("Process-owned handle test is ignored on WINS build due to Pls() behaviour."));
#endif
	// When using Pls() on WINS build, it inteferes with handle count assert here due to Pls() behaviour which initialises PLS object
	// on first call of Pls() rather than during DLL loading, which cause extra count into iProcHandleMark.
	// ARMV5 build does not suffer this problem as proper WSD support is used.
#ifndef __WINS__
	ASSERT_EQUALS(iProcHandleMarkEnd, iProcHandleMark);
#endif
	ASSERT_EQUALS(iThreadHandleMarkEnd, iThreadHandleMark);
	}

/** Initialises the window server session and window group objects. */
EXPORT_C void CEglTestStep::OpenWsSessionL(TInt aGroupId)
	{
	User::LeaveIfError(iWsSession.Connect());
	iWindowGroup = RWindowGroup(iWsSession);
	User::LeaveIfError(iWindowGroup.Construct(aGroupId));
	}

/** Uninitialises the window group object and the window server session. */
EXPORT_C void CEglTestStep::CloseWsSession()
	{
	iWindowGroup.Close();
	iWsSession.Close();
	}

/**
Uses the Eikon Environment to construct a window and put it on top.
@param aWindow A non-constructed window object
@param aRect The intial position and size of the window
@leave Standard system errors
*/
EXPORT_C void CEglTestStep::ConstructWindowL(RWindow& aWindow, const TRect& aRect)
	{
	INFO_PRINTF1(_L("CEglTestStep::CreateWindowL()"));

	const TUint32 ENullWsHandle = 0xFFFFFFFF;	// Events delivered to this handle are thrown away
	aWindow = RWindow(iWsSession);
	CleanupClosePushL(aWindow);
	User::LeaveIfError(aWindow.Construct(iWindowGroup, ENullWsHandle));
	aWindow.SetExtent(aRect.iTl, aRect.Size());
	aWindow.Activate();
	CleanupStack::Pop(&aWindow);
	}

/**
Prints both the Source pixel format and the target pixel format
*/
EXPORT_C void CEglTestStep::PrintUsedPixelConfiguration()
	{
	INFO_PRINTF1(_L("******UsedPixelConfiguration******"));
	INFO_PRINTF1(_L("Source Pixel Format"));
	PrintPixelFormat(iSourceFormat);	
	
	INFO_PRINTF1(_L("Target Format"));
	PrintVGImageFormat(iSurfaceFormat);
	INFO_PRINTF1(_L("**********************************"));
	}

EXPORT_C void CEglTestStep::PrintPixelFormat(TUidPixelFormat aFormat)
	{
	switch(aFormat)
		{
        case EUidPixelFormatA_8:
            INFO_PRINTF1(_L("EUidPixelFormatA_8"));
            break;
		case EUidPixelFormatRGB_565:
			INFO_PRINTF1(_L("EUidPixelFormatRGB_565"));
			break;
		case EUidPixelFormatXRGB_8888:
			INFO_PRINTF1(_L("EUidPixelFormatXRGB_8888"));
			break;
		case EUidPixelFormatARGB_8888:
			INFO_PRINTF1(_L("EUidPixelFormatARGB_8888"));
			break;
		case EUidPixelFormatARGB_8888_PRE:
			INFO_PRINTF1(_L("EUidPixelFormatARGB_8888_PRE"));
			break;
		default:
			ERR_PRINTF2(_L("Unsupported pixel format (%d)"), aFormat);
			ASSERT_TRUE(EFalse);
		}
	}

EXPORT_C void CEglTestStep::PrintVGImageFormat(VGImageFormat aAttr)
	{
	switch(aAttr)
		{
		case VG_sRGB_565:
			INFO_PRINTF1(_L("VG_sRGB_565"));
			break;
		case VG_sXRGB_8888:
			INFO_PRINTF1(_L("VG_sXRGB_8888"));
			break;
		case VG_sARGB_8888:
			INFO_PRINTF1(_L("VG_sARGB_8888"));
			break;
		case VG_sARGB_8888_PRE:
			INFO_PRINTF1(_L("VG_sARGB_8888_PRE"));
			break;
		default:
			ERR_PRINTF2(_L("Unsupported VGImage format (%d)"), aAttr);
			ASSERT_TRUE(EFalse);
		}
	}


/*****************************************************************************
 ** Multiprocess test utils
 *****************************************************************************/

/**
Launches the specified number of processes, where each process will perform the actions specified in 
the doProcessFunctionL of the calling test. As no images TSgDrawableId has been passed, an 
an array of one (NULL) TSgDrawableId will be created.
@param aProcessCount Number of processes
@param aTestName Name of the calling test case (so that it can call it's doProcessFunctionL method 
@leave Standard system errors
*/  
EXPORT_C void CEglTestStep::Test_MultiProcessL(const TDesC& aTestDllName, TInt aProcessCount, const TDesC& aTestStepName)
	{
	TSgDrawableId sgId;
	Mem::FillZ(&sgId, sizeof(TSgDrawableId));
	Test_MultiProcessL(aTestDllName, aProcessCount, aTestStepName, sgId);
	}

/**
Launches the specified number of processes, where each process will perform the actions specified in 
the doProcessFunctionL of the calling test.
@param aProcessCount Number of processes
@param aTestName Name of the calling test case (so that it can call it's doProcessFunctionL method 
@param aSgId Images TSgDrawableId which will be used to create an array of one TSgDrawableId
@leave Standard system errors
*/  
EXPORT_C void CEglTestStep::Test_MultiProcessL(const TDesC& aTestDllName, TInt aProcessCount, const TDesC& aTestStepName, const TSgDrawableId& aSgId)
	{
	// we assume we pass the same Id to all the processes (array of one)
	RArray<TSgDrawableId> sgIdList;
	ASSERT_EQUALS(sgIdList.Insert(aSgId,0), KErrNone);
	Test_MultiProcessL(aTestDllName, aProcessCount, aTestStepName, sgIdList);
	sgIdList.Close();
	}

/**
Launches the specified number of processes, where each process will perform the actions specified in 
the doProcessFunctionL of the calling test. The association of images and processes is done via the
predefined ImageIndexFromProcessId() method.
@param aProcessCount Number of processes
@param aTestName Name of the calling test case (so that it can call it's doProcessFunctionL method 
@param aSgIdList Array containing the list of images' TSgDrawableId 
@leave Standard system errors
*/  
EXPORT_C void CEglTestStep::Test_MultiProcessL(const TDesC& aTestDllName, TInt aProcessCount, const TDesC& aTestStepName, const RArray<TSgDrawableId>& aSgIdList)
	{
	TInt imageCount = aSgIdList.Count();
	if(aProcessCount <= 0 || imageCount <=0 || aProcessCount > KMaxProcessNumber || imageCount > aProcessCount)
		{
		ERR_PRINTF1(_L("Invalid process request!"));
		User::Leave(KErrArgument);
		}

    // create MsgQueue (only used in some test to pass data between 2 processes)
	RMsgQueue<TSgDrawableId> messageQueueSgId;
	TInt ret = messageQueueSgId.CreateGlobal(KNullDesC, 1, EOwnerProcess);
	ASSERT_EQUALS(ret, KErrNone);
	CleanupClosePushL(messageQueueSgId);

	RMsgQueue<TProcessId> messageQueueProcId;
	ret = messageQueueProcId.CreateGlobal(KNullDesC, 1, EOwnerProcess);
	ASSERT_EQUALS(ret, KErrNone);
	CleanupClosePushL(messageQueueProcId);

    // Create semphores that can be shared (only used in some test to synch between 2 process)
    ret = iSemaphore[0].CreateGlobal(KNullDesC(), 0, EOwnerProcess);
    ASSERT_EQUALS(ret, KErrNone);
    ret = iSemaphore[1].CreateGlobal(KNullDesC(), 0, EOwnerProcess);
    ASSERT_EQUALS(ret, KErrNone);

    // create MsgQueue (only used in some tests to pass data from client processes to the main process)
    RMsgQueue<TEglStepMessageBuffer> messageQueueClientProcParam;
    ret = messageQueueClientProcParam.CreateGlobal(KNullDesC, 1, EOwnerProcess);
    ASSERT_EQUALS(ret, KErrNone);
    CleanupClosePushL(messageQueueClientProcParam);
    
	for (TInt i=0; i<aProcessCount; i++)
		{
		TProcessInfo info;
		info.iIdx=i;
		info.iSgId=	aSgIdList[ImageIndexFromProcessId(i, imageCount)];

		ret = iProcessStatus[i].iProcess.Create(KEglTestServerWrapperProcess, KNullDesC);
		User::LeaveIfError(ret);
      
		// Specify the test for the process
		ret = iProcessStatus[i].iProcess.SetParameter(EProcSlotTestDllName, aTestDllName);
		User::LeaveIfError(ret);
		ret = iProcessStatus[i].iProcess.SetParameter(EProcSlotTestStepName, aTestStepName);
		User::LeaveIfError(ret);	
        
		// Specify the non-handle params passed to the process
		TPckg<TProcessInfo> pckgInfo(info);
		ret = iProcessStatus[i].iProcess.SetParameter(EProcSlotParams, pckgInfo);
		User::LeaveIfError(ret);

		// Pass in the semaphores
		ret = iProcessStatus[i].iProcess.SetParameter(EProcSlotSemaphore0, iSemaphore[0]);
		User::LeaveIfError(ret);
		ret = iProcessStatus[i].iProcess.SetParameter(EProcSlotSemaphore1, iSemaphore[1]);
		User::LeaveIfError(ret);
		ret = iProcessStatus[i].iProcess.SetParameter(EProcSlotMsgQueueSgId, messageQueueSgId);
		User::LeaveIfError(ret);
		ret = iProcessStatus[i].iProcess.SetParameter(EProcSlotMsgQueueProcId, messageQueueProcId);
		User::LeaveIfError(ret);
		ret = iProcessStatus[i].iProcess.SetParameter(EProcSlotSourceFormat, static_cast<TInt>(iSourceFormat));
		User::LeaveIfError(ret);
		ret = iProcessStatus[i].iProcess.SetParameter(EProcSlotSurfaceFormat, static_cast<TInt>(iSurfaceFormat));
		User::LeaveIfError(ret);
        ret = iProcessStatus[i].iProcess.SetParameter(EProcSlotCustomClientParam, messageQueueClientProcParam);
        User::LeaveIfError(ret);
		
		iProcessStatus[i].iProcess.Logon(iProcessStatus[i].iStatus); 
		iProcessStatus[i].iProcess.Resume();
		}
    
	//by default an empty implementation
    ReceiveMessageFromClient(messageQueueClientProcParam);

	// wait for all processes to complete (not worried about the order)
	// This is needed, as the only way to determine whether the process step has failed is to check
	//  the return value (using TEST(EFalse) has no effect on the spawned process)
	for (TInt i=0; i<aProcessCount; i++)
		{
		User::WaitForRequest(iProcessStatus[i].iStatus);
		CheckProcessStatusL(i, iProcessStatus[i].iStatus, iProcessStatus[i].iProcess);
		RDebug::Print(_L(">>>>>(%d)>> status :%d"), i, iProcessStatus[i].iStatus.Int());
		iProcessStatus[i].iProcess.Close();
		}

	// close MsgQueue and semaphores (as used in some test with 2 spawned processes)
	CleanupStack::PopAndDestroy(3, &messageQueueSgId); //messageQueueClientProcParam, messageQueueProcId
	iSemaphore[1].Close();
	iSemaphore[0].Close();
	}

/**
Check the status of a process running as part of the current teststep. 
@param aIndex Index of the process
@param aStatus The request status of the process in question. 
@param aProcess The process object itself.  
@leave Standard system errors
*/  
void CEglTestStep::CheckProcessStatusL(TInt aIndex, const TRequestStatus& aStatus, const RProcess& aProcess)
	{
	TInt status = aStatus.Int();
	if (status == KErrNone)
		{
		return;
		}
	if (status == KRequestPending)
		{
		// If the process is still running, that's an error, as we waited for the status  
		ERR_PRINTF2(_L("Error in process %d - status should not be KRequestPending"), aIndex);
		User::Leave(KErrTEFUnitFail);
		}
	// Something went wrong
	switch (aProcess.ExitType())
		{
		case EExitPanic:		// The thread or process has been panicked.
			{
			TPtrC ptrExitCategory = aProcess.ExitCategory();
			ERR_PRINTF4(_L("Panic in process %d - category:[%S] reason: %d"), aIndex, &ptrExitCategory, aProcess.ExitReason());
			// Propagate the panic
			User::Panic(aProcess.ExitCategory(), aProcess.ExitReason());
			}
			// follow through
		case EExitKill: 		// The thread or process has ended as a result of a kill, i.e. Kill() has been called on the RThread or RProcess handle. Or a thread was ended as a result of calling User::Exit(). 
		case EExitTerminate: 	// The thread or process has ended as a result of a terminate, i.e. Terminate() has been called on the RThread or RProcess handle. 
		case EExitPending:		// The thread or process is alive. 
		default:
			// Propagate the error
			ERR_PRINTF3(_L("Error in process %d - code %d"), aIndex, aStatus.Int());
			User::Leave(aStatus.Int());
		}
	ASSERT(0);
	}


/*****************************************************************************
 ** Multithread test utils
 *****************************************************************************/

/**
Launches the specified number of threads, where each thread will perform the actions specified in 
the doThreadFunctionL of the calling test. 
@param aThreadCount Number of threads
@param aWaitForCompletion To wait until the launched thread has completed 
@leave Standard system errors
*/  
EXPORT_C void CEglTestStep::Test_MultiThreadL(TInt aThreadCount, TBool aWaitForCompletion)
	{
	if(aThreadCount <= 0 || aThreadCount > KMaxThreadNumber)
		{
		ERR_PRINTF1(_L("Invalid thread request!"));
		User::Leave(KErrArgument);
		}

	iWaitForCompletionOnPostamble = !aWaitForCompletion;
	
	//we just care for these 2 semaphores
	ASSERT_EQUALS(iSemaphore[0].CreateLocal(0, EOwnerProcess), KErrNone);
	ASSERT_EQUALS(iSemaphore[1].CreateLocal(0, EOwnerProcess), KErrNone);
	
	_LIT(KThread, "CEglTestStep_Thread");
	_LIT(KUnderScore, "_");
 	
	TInt ret = KErrNone;
	ASSERT_EQUALS(iThreadStatus.Count(),0);
	// Reserve space to avoid reallocation of iThreadStatus.iStatus
	iThreadStatus.ReserveL(aThreadCount);
	for (TInt i=0; i<aThreadCount; i++)	
		{
		iThreadInfos[i].iSelf=this;
		iThreadInfos[i].iIdx=i;
 
		TTime tm;
		TBuf<32> bufTime;
		tm.UniversalTime();
	    tm.FormatL(bufTime, _L("_%H%T%S%C_"));

		// guaranteed unique thread name (useful if several threads are created with aWaitForCompletion = false)
		TName threadName(KThread);
		threadName.Append(KUnderScore);
		threadName.AppendNum(i, EDecimal);
		threadName.Append(KUnderScore);
		threadName.Append(bufTime); 
	    threadName.AppendNum(Math::Random(), EHex);

	    iThreadStatus.AppendL(TThreadStatus());
		ret = iThreadStatus[i].iThread.Create(threadName, ThreadFunction, KDefaultStackSize, KMinHeapSize, KDefaultHeapSize, &iThreadInfos[i], EOwnerProcess);
		User::LeaveIfError(ret);

		if(!aWaitForCompletion)
			{
			// We want to wait for the notification that the extra thread is about to be launched
			// Improves timing issues within a hardware WDP environment
			iThreadStatus[i].iThread.Rendezvous(iThreadStatus[i].iStatus[TThreadStatus::ERendezvous]);
			}
		iThreadStatus[i].iThread.Logon(iThreadStatus[i].iStatus[TThreadStatus::ELogin]);
		iThreadStatus[i].iThread.Resume();
		}
    Test_MultiThread_WaitL(aWaitForCompletion, aWaitForCompletion ? TThreadStatus::ELogin : TThreadStatus::ERendezvous);
   	}

EXPORT_C void CEglTestStep::Test_MultiThread_WaitL(TBool aCloseThreads, TThreadStatus::TStatusId aStatusId)
    {
    // Close handles and wait for all threads to complete (not worried about the order). Note that some 
    //   tests do not require to wait for completion. Nevertheless, care should be taken to ensure that the 
    //   spawned thread is capable of modifying the main TEF process TestStepResult.
       
    TInt countThread = iThreadStatus.Count();
    for (TInt i=0; i<countThread; i++)
        {
        User::WaitForRequest(iThreadStatus[i].iStatus[aStatusId]);
        CheckThreadStatusL(i, iThreadStatus[i].iStatus[aStatusId], iThreadStatus[i].iThread);
        INFO_PRINTF3(_L(">>>>>(%d)>> status :%d"), i, iThreadStatus[i].iStatus[aStatusId].Int());

        if(aCloseThreads)
            {
            iThreadStatus[i].iThread.Close();
            }
        }
    if(aCloseThreads)
        {
        iThreadStatus.Reset();

        iSemaphore[0].Close();
        iSemaphore[1].Close();
        }
    }

/**
Check the status of a thread running as part of the current teststep. 
@param aIndex Index of the thread
@param aStatus The request status of the thread in question. 
@param aThread The thread object itself.  
@leave Standard system errors
*/ 
void CEglTestStep::CheckThreadStatusL(TInt aIndex, const TRequestStatus& aStatus, const RThread& aThread)
	{
	TInt status = aStatus.Int();
	if (status == KErrNone)
		{
		// All went well
		return;
		}
	if (status == KRequestPending)
		{
		// If the thread is still running, that's an error, as we waited for the status  
		ERR_PRINTF2(_L("Error in thread %d - status should not be KRequestPending"), aIndex);
		User::Leave(KErrTEFUnitFail);
		}
	// Something went wrong
	switch (aThread.ExitType())
		{
		case EExitPanic:		// The thread or process has been panicked.
			{
			TPtrC ptrExitCategory = aThread.ExitCategory();
			ERR_PRINTF4(_L("Panic in thread %d - category:[%S] reason: %d"), aIndex, &ptrExitCategory, aThread.ExitReason());
			User::Panic(aThread.ExitCategory(), aThread.ExitReason());
			}
			// follow through
		case EExitKill: 		// The thread or process has ended as a result of a kill, i.e. Kill() has been called on the RThread or RProcess handle. Or a thread was ended as a result of calling User::Exit(). 
		case EExitTerminate: 	// The thread or process has ended as a result of a terminate, i.e. Terminate() has been called on the RThread or RProcess handle. 
		case EExitPending:		// The thread or process is alive. 
		default:
			// Propagate the error
			ERR_PRINTF3(_L("Error in thread %d - code %d"), aIndex, status);
			User::Leave(aStatus.Int());
		}
	// We should not get here!
	ASSERT(0);
	}

TInt CEglTestStep::ThreadFunction(TAny* aInfo)
// static
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}

	CEglTestStep::TThreadInfo* info = reinterpret_cast<CEglTestStep::TThreadInfo*>(aInfo);
	TRAPD(err, info->iSelf->ThreadFunctionL(*info));

	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}

void CEglTestStep::ThreadFunctionL(TThreadInfo& aInfo)
	{
	// Mark the handle count for this thread
	TInt processHandleMark=0;
	TInt threadHandleMark=0;
	RThread().HandleCount(processHandleMark, threadHandleMark);
	INFO_PRINTF4(_L("MARK: THREAD %d: (%d) process-owned handle(s) / (%d) thread-owned handle(s)"), aInfo.iIdx, processHandleMark, threadHandleMark);	
	
	// Notify the main thread that we are about to launch the extra thread
	RThread::Rendezvous(KErrNone);
	
	// Run the real thread funciton
	aInfo.iSelf->doThreadFunctionL(aInfo.iIdx);

	// Release EGL thread state
	INFO_PRINTF2(_L("thread %d: Calling eglReleaseThread()"), aInfo.iIdx);
	ASSERT_EGL_TRUE(eglReleaseThread());
	
	// Check the handle count for this thread has not changed
	TInt processHandleMarkEnd=0;
	TInt threadHandleMarkEnd=0;
	RThread().HandleCount(processHandleMarkEnd, threadHandleMarkEnd);
	INFO_PRINTF4(_L("MARK-END: THREAD %d: (%d) process-owned handle(s) / (%d) thread-owned handle(s)"), aInfo.iIdx, processHandleMarkEnd, threadHandleMarkEnd);
	
	//Not testing equality of process-owned handles as these should only be tested from the main thread.
	//Process handlecount is dependent on all threads, therefore process handle imbalances could be the responsibility of other threads.
	ASSERT_EQUALS(threadHandleMark, threadHandleMarkEnd);
	}

/**
Tests should override this method for multithreaded testing
*/
EXPORT_C void CEglTestStep::doThreadFunctionL(TInt aIdx)
	{
	// Not supported for this test step
	ERR_PRINTF2(_L("thread %d: Calling CEglTestStep::doThreadFunctionL() - should be overriden"), aIdx);
	User::Leave(KErrNotSupported);
	}

EXPORT_C void CEglTestStep::doThreadFunctionL(TInt aIdx,const TSgDrawableId& aSgId)
	{
	// Not supported for this test step
	ERR_PRINTF3(_L("thread %d: Calling CEglTestStep::doThreadFunctionL() - should be overriden, TSgDrawableId %lu."), aIdx, aSgId.iId);
	User::Leave(KErrNotSupported);
	}

/**
Tests should override this method for multiprocess testing
*/
EXPORT_C void CEglTestStep::doProcessFunctionL(TInt aIdx)
	{
	// Not supported for this test step
	ERR_PRINTF2(_L("Process %d: Calling CEglTestStep::doProcessFunctionL() - should be overriden"), aIdx);
	User::Leave(KErrNotSupported);
	}

EXPORT_C void CEglTestStep::doProcessFunctionL(TInt aIdx,const TSgDrawableId& aSgId)
	{
	// Not supported for this test step
	ERR_PRINTF3(_L("Process %d: Calling CEglTestStep::doProcessFunctionL() - should be overriden, TSgDrawableId %lu."), aIdx, aSgId.iId);
	User::Leave(KErrNotSupported);
	}


/**
Rendezvous: Ensures that both threads get to this point before continuing
@param aIdx The thread index value that was passed into
			the override of CEglTestStep::doThreadFunctionL()
*/
EXPORT_C void CEglTestStep::Rendezvous(TInt aIdx)
	{
	if(aIdx >= 2)
	    {
	    // Currently Rendezvous is only supported between threads with index 0 and index 1
	    INFO_PRINTF2(_L("CEglTestStep::Rendezvous() - aIdx (%d) is too big!!"), aIdx);
	    ASSERT(0);
	    }
	INFO_PRINTF2(_L("thread %d: ...At Rendezvous..."), aIdx);
	iSemaphore[aIdx].Signal();
	iSemaphore[1-aIdx].Wait();
	}


/*****************************************************************************
 ** Egl Helpers
 *****************************************************************************/
 
/**
Temporarily initializes the EGL thread and display in order to check for the 
supplied extension string.
The display is then released and terminated.
Use this method to pre-check for the existence of an extension string prior 
to starting a test.
There are 2 ways to ask for an extension, via the ID (the default way to do it) 
or passing a string containing the full name of the extension (used in some tests only)
@param aExtensions The extension ID to look for
@param aExtensionName The extension name to look for
@return Whether the extension string can be found
*/
EXPORT_C TBool CEglTestStep::CheckForExtensionL(TInt aExtensions, const TDesC& aExtensionName)
	{
	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, -1);
	eglSess->InitializeL();
	
	TBool bFoundExtensions = eglSess->CheckNeededExtensionL(aExtensions, aExtensionName);
	
	// Cleanup EGL Completely
	CleanupStack::PopAndDestroy(eglSess);
	eglSess = NULL;
	TerminateDisplayL();
	ASSERT_EGL_TRUE(eglReleaseThread());

	// return whether the extension string was found
	return bFoundExtensions;
	}

/**
Uses eglGetDisplay() to initialise iDisplay, and to check the result of the call
*/
EXPORT_C void CEglTestStep::GetDisplayL()
	{
	INFO_PRINTF1(_L("Calling eglGetDisplay..."));
	
	iDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	ASSERT_EGL_TRUE(iDisplay != EGL_NO_DISPLAY);
	}

/**
If the iDisplay has been initialised then this method uses eglTerminate() 
to terminate iDisplay, and to check the result of the call.
This method also resets the value of iDisplay to EGL_NO_DISPLAY to indicate
that the display is no longer initialised.
*/
EXPORT_C void CEglTestStep::TerminateDisplayL()
	{
	if (iDisplay != EGL_NO_DISPLAY)
		{
		INFO_PRINTF1(_L("Calling eglTerminate..."));
		ASSERT_EGL_TRUE(eglTerminate(iDisplay));
		iDisplay = EGL_NO_DISPLAY;
		}
	}

/**
Cut and paste from CTestStep::SetLogger() - which is not exported
As the name suggests, this is for use by the egl test process wrapper
*/
void CEglTestStep::SetLoggerForProcessWrapperL()
	{
	// Create a cinidata object for parsing the testexecute.ini
	CTestExecuteIniData* parseTestExecuteIni = NULL;
	TBuf<KMaxTestExecuteNameLength> resultFilePath;
	TBuf<KMaxTestExecuteNameLength> xmlFilePath;
	TInt logMode;
	TInt logLevel;
	
	TRAPD(err,parseTestExecuteIni = CTestExecuteIniData::NewL());
	if (err == KErrNone)
		{
		CleanupStack::PushL(parseTestExecuteIni);
		parseTestExecuteIni->ExtractValuesFromIni();
		parseTestExecuteIni->GetKeyValueFromIni(KTEFHtmlKey, resultFilePath);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFXmlKey, xmlFilePath);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFLogMode, logMode);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFLogSeverityKey, logLevel);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFEnableIniAccessLog, IniAccessLog());
		}
	else
		{
		resultFilePath.Copy(KTestExecuteLogPath);
		xmlFilePath.Copy(KTestExecuteLogPath);
		logMode = TLoggerOptions(ELogHTMLOnly);
		logLevel = RFileFlogger::TLogSeverity(ESevrAll);
		IniAccessLog() = ETrue;
		}
	Logger().SetLoggerOptions(logMode);
		
	// Initialise a handle to the file logger
	User::LeaveIfError(Logger().Connect());
	RFs fS;
	User::LeaveIfError(fS.Connect());
	CleanupClosePushL(fS);
	RFile file;
	TBuf<KMaxTestExecuteNameLength> xmlLogFile(xmlFilePath);
	TBuf<KMaxTestExecuteNameLength> logFile;
	TBuf<KMaxTestExecuteNameLength> logFileNameFile(resultFilePath);
	logFileNameFile.Append(KTestExecuteScheduleTestLogCompatibilityNameFile);
	if(file.Open(fS,logFileNameFile,EFileRead | EFileShareAny) != KErrNone)
		{
		// For the old flogger we have to create an individual file
		logFile.Copy(TestStepName());
		_LIT(KTxtExtension,".txt");
		logFile.Append(KTxtExtension);
		logMode = TLoggerOptions(0);
		Logger().SetLoggerOptions(logMode);
		}
	else
		{
		CleanupClosePushL(file);
		TBuf8<KMaxTestExecuteNameLength> logFile8;
		TInt fileSize;
		User::LeaveIfError(file.Size(fileSize));
		User::LeaveIfError(file.Read(logFile8,fileSize));
		logFile.Copy(logFile8);
		xmlLogFile.Append(logFile);
		_LIT(KXmlExtension,".xml");
		xmlLogFile.Append(KXmlExtension);
		_LIT(KHtmExtension,".htm");
		logFile.Append(KHtmExtension);
		CleanupStack::Pop(&file);
		file.Close();
		}
	TBuf<KMaxTestExecuteLogFilePath> logFilePath(resultFilePath);
	logFilePath.Append(logFile);
	CleanupStack::Pop(&fS);
	fS.Close();
	
	if (logMode == 0 || logMode == 2)
		{
		User::LeaveIfError(Logger().HtmlLogger().CreateLog(logFilePath,RTestExecuteLogServ::ELogModeAppend));
		Logger().HtmlLogger().SetLogLevel(TLogSeverity(logLevel));
		}
	if (logMode == 1 || logMode == 2)
		{
		User::LeaveIfError(Logger().XmlLogger().CreateLog(xmlLogFile,RFileFlogger::ELogModeAppend));
		Logger().XmlLogger().SetLogLevel(RFileFlogger::TLogSeverity(logLevel));
		}
	if (parseTestExecuteIni != NULL)
		{
		CleanupStack::PopAndDestroy(parseTestExecuteIni);
		}
	}

EXPORT_C void CEglTestStep::PartialInitialiseL(const TDesC& aStepName)
	{
	SetTestStepName(aStepName);	
	SetLoggerForProcessWrapperL();
	// Assume pass
	SetTestStepResult(EPass);
	}

EXPORT_C void CEglTestStep::CreateEglSessionL(TInt aIdx)
	{
	delete iEglSess; //just in case it was called twice
	iEglSess = CTestEglSession::NewL(Logger(), iDisplay, aIdx);
	}

