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


/** @file
    @internalComponent - Internal Symbian test code */


#include <e32base.h>
#include <e32debug.h>
#include <e32msgqueue.h>
#include <ecom/ecom.h>
#include <test/testexecuteserverbase.h>
#include <e32math.h>
#include "localtestbase.h"
#include "egltest_commscommon.h"
#include "egltest_endpoint_util.h"


_LIT(KEglEndpointTestServerName,"eglendpointtestserver");


CLocalTestStepBase::CLocalTestStepBase(enum TTestUid aUid) :
    iTestId(aUid),
    iIsInTestStep(EFalse)
    {
    }

void CLocalTestStepBase::DoPreambleL()
    {
    //null implmentation for base class
    }

void CLocalTestStepBase::DoPostambleL()
    {
    //null implmentation for base class
    }

//function used for creating the queues.
TVerdict CLocalTestStepBase::doTestStepPreambleL()
    {
    //Open the queues.
    User::LeaveIfError(iResultOutQueue.OpenGlobal(KResultQueueName));
    User::LeaveIfError(iParamsInQueue.OpenGlobal(KParamsQueueName));
    SetTestStepResult(EPass);
    iHasCurrentTestIds = EFalse;
    iResultLog = CTestIdResultLogger::NewL(Logger());
    DoPreambleL();
    return EPass;
    }

TVerdict CLocalTestStepBase::doTestStepPostambleL()
    {
    //Log the result of the current tests.
    if(iHasCurrentTestIds)
        {
        iResultLog->LogResult(iTestIdVerdict);
        }
    
    DoPostambleL();
    delete iResultLog;
    iResultLog = NULL;
    iResultOutQueue.Close();
    iParamsInQueue.Close();
    return EPass;
    }

CLocalTestStepBase::~CLocalTestStepBase()
    {
    //closing an already closed handle is harmless
    iResultOutQueue.Close();
    iParamsInQueue.Close();
    }

TVerdict CLocalTestStepBase::StartRemoteTestStep(const TRemoteTestParams& aMessageIn)
    {
    //Starting the remote test step is implemented as a special case
    //of running a test case, with TestCase = KStartTestStepCaseNumber.
    iIsInTestStep = ETrue;
    TVerdict retVal = RunRemoteTestCase(KStartTestStepCaseNumber, aMessageIn);
    if(retVal != EPass)
        {
        iIsInTestStep = EFalse;
        }
    return retVal;
    }

TVerdict CLocalTestStepBase::EndRemoteTestStep(const TRemoteTestParams& aMessageIn)
    {
    //Ending the remote test step is implemented as a special case
    //of running a test case, with TestCase = KEndTestStepCaseNumber.
    TVerdict retVal = RunRemoteTestCase(KEndTestStepCaseNumber, aMessageIn);
    iIsInTestStep = EFalse;
    return retVal;
    }

TVerdict CLocalTestStepBase::RunRemoteTestCase(TInt aTestCase, const TRemoteTestParams& aMessageIn)
    {
    //Don't attempt to run any test cases if we are not in a test step.
    if(!iIsInTestStep)
        {
        SetTestStepResult(EFail);
        return TestStepResult();
        }

    //send the message
    TRemoteTestParamsPacket message(iTestId, aTestCase, aMessageIn);
    iParamsInQueue.SendBlocking(message);

    TRemoteTestResult result;
    do
        {
        //relying on TEF timeout if there are problems such as the render stage not loaded.
        iResultOutQueue.ReceiveBlocking(result);

        //if uid and test case doesn't match something has gone badly wrong
        if (result.iUid != iTestId || result.iTestCase != aTestCase)
            {
            //test is out of Sync
            User::Panic(_L("Test out of sync with render stage"), KErrGeneral);
            }

        //log the message if there is one
        if (!result.iFinished)
            {
            //Convert the filename to a C string. The remote test env guarantees
            //that there is a free space at the end of the descriptor to add NULL.
            const TText8* file = result.iFile.PtrZ();

            //Convert the message to unicode and log the message.
            TBuf<KRSLogMessageLength> message;
            message.Copy(result.iMessage);
            Logger().LogExtra(file, result.iLine, result.iSeverity, _L("%S"), &message);
            }
        }while (!result.iFinished);

    //Translate the RemoteTestStep verdict to a TVerdict.
    TVerdict retVal = EPass;
    switch (result.iVerdict)
        {
        case ERtvPass:
            retVal = EPass;
            break;

        case ERtvFail:
            retVal = EFail;
            break;

        case ERtvInconclusive:
            retVal = EInconclusive;
            break;

        case ERtvAbort:
            retVal = EAbort;
            break;

        case ERtvPanic:
            //The remote test paniced, so we panic too.
            //This means the output log relects what actually happened.
            User::Panic(_L("Remote Test Step Paniced!"), KErrGeneral);
            break;

        case ERtvTimeout:
            //The remote test timedout, so we sleep so that tef times us out too.
            //This means the output log relects what actually happened.
            User::After(KMaxTInt);
            break;

        case ERtvUnknownTestUid:
            retVal = EIgnore;
            break;

        default:
            User::Panic(_L("Invalid verdict returned from the remote test step!"), KErrGeneral);
            break;
        }

    if(retVal != EPass)
        {
        SetTestStepResult(retVal);
        }

    return retVal;
    }


void CLocalTestStepBase::RegisterTestIdsL(const TDesC& aTestString)
    {
    iResultLog->RegisterTestIdsL(aTestString);
    }


void CLocalTestStepBase::SetCurrentTestIds(const TDesC& aTestString)
    {
    if(iHasCurrentTestIds)
        {
        iResultLog->LogResult(iTestIdVerdict);
        }
    
    iResultLog->SetCurrentTestIds(aTestString);
    iHasCurrentTestIds = ETrue;
    iTestIdVerdict = EPass;
    }


void CLocalTestStepBase::SetTestStepResult(TVerdict aVerdict)
    {
    iTestIdVerdict = aVerdict;
    CTestStep::SetTestStepResult(aVerdict);
    }


//Used by the result logger to delimit the csv test id strings.
class TCommaDelimiter
    {
private:
    mutable TPtrC iRemaining;
    
public:
    TCommaDelimiter(const TDesC& aString)
        {
        //Set our remaining string to the whole string, or NULL if empty.
        if(aString.Length() == 0)
            {
            iRemaining.Set(NULL, 0);
            }
        else
            {
            iRemaining.Set(aString);
            }
        }
    
    
    TInt GetNext(TPtrC& aSegment) const
        {
        //Trim off any leading commas.
        while(iRemaining.Length() >= 2 && iRemaining[0] == ',')
            {
            iRemaining.Set(&iRemaining[1], iRemaining.Length() - 1);
            }
        
        //If remaining string is empty or has one remaining comma, return.
        if(iRemaining.Length() == 0 || iRemaining[0] == ',')
            {
            iRemaining.Set(NULL, 0);
            return KErrNotFound;
            }
        
        //Find the first comma.
        TInt pos = iRemaining.Locate(',');
        
        //If comma not found, return all remaining string.
        if(pos == KErrNotFound)
            {
            aSegment.Set(iRemaining);
            iRemaining.Set(NULL, 0);
            return KErrNone;
            }
        
        //Comma found. There must be non-comma chars between 0 
        //and pos since we trimmed leading commas previously.
        aSegment.Set(&iRemaining[0], pos);
        iRemaining.Set(&iRemaining[pos], iRemaining.Length() - pos);
        return KErrNone;
        }
    
    
    TInt GetNextTrimmed(TPtrC& aSegment) const
        {
        //Keep calling GetNext() until we get a segment that has 
        //chars other than spaces or there are no more segments.
        while(1)
            {
            TPtrC segment;
            TInt err = GetNext(segment);
            if(err != KErrNone)
                {
                return err;
                }
            
            TInt front;
            TInt back;
            for(front = 0; front < segment.Length() && segment[front] == ' '; front++) {}
            for(back = segment.Length() - 1; back >= 0 && segment[back] == ' '; back--) {}
            
            TInt length = (back + 1) - front;
            if(length > 0 && segment[front] != ' ' && segment[back] != ' ')
                {
                aSegment.Set(&segment[front], length);
                return KErrNone;
                }
            }
        }
    };


CTestIdResultLogger* CTestIdResultLogger::NewL(CTestExecuteLogger& aLogger)
    {
    CTestIdResultLogger* self = new (ELeave) CTestIdResultLogger(aLogger);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


CTestIdResultLogger::CTestIdResultLogger(CTestExecuteLogger& aLogger) :
    iOriginalThread(RThread().Id()),
    iLogger(aLogger)
    {
    }


void CTestIdResultLogger::ConstructL()
    {
    //Create panic monitor thread. Note that we share the heap with this 
    //thread so that the arrays remain in scope if this thread panics.
    //Note also no need for explicit locking of arrays since the panic 
    //monitor will only access them if a panic occurs here.
    static const TInt KStackSize =   0x2000;      //  8KB
    TUint32 random = Math::Random();
    TName threadName;
    _LIT(KThreadNameFormat, "%S-%u");
    _LIT(KEnvName, "EpTestIdLogger");
    threadName.Format(KThreadNameFormat, &KEnvName, random);
    User::LeaveIfError(iPanicMonitor.Create(threadName, PanicMonitorMain, KStackSize, &User::Heap(), this, EOwnerThread));
    
    //Rendezvous with panic thread.
    TRequestStatus rendStat;
    iPanicMonitor.Rendezvous(rendStat);
    iPanicMonitor.Resume();
    User::WaitForRequest(rendStat);
    }


CTestIdResultLogger::~CTestIdResultLogger()
    {
    TRequestStatus logonStat;
    iPanicMonitor.Logon(logonStat);
    iPanicMonitor.RequestComplete(iCloseMonitor, KErrNone);
    User::WaitForRequest(logonStat);
    iPanicMonitor.Close();
    iRegisteredTestIds.Close();
    iCurrentTestIds.Close();
    }


void CTestIdResultLogger::RegisterTestIdsL(const TDesC& aTestString)
    {
    //Set up delimitter.
    TCommaDelimiter delimit(aTestString);
    
    //Get every test id from the string and add it to the registered test ids array.
    TPtrC testIdPtr;
    while(delimit.GetNextTrimmed(testIdPtr) == KErrNone)
        {
        TTestId testId(testIdPtr);
        iRegisteredTestIds.AppendL(testId);
        }
    
    //Reserve enough space in the current test ids array so that SCurrentTestIds() can not fail.
    iCurrentTestIds.ReserveL(iRegisteredTestIds.Count());
    }


void CTestIdResultLogger::SetCurrentTestIds(const TDesC& aTestString)
    {
    ASSERT(iCurrentTestIds.Count() == 0);
    
    //Set up delimitter.
    TCommaDelimiter delimit(aTestString);
    
    //Get every test id from the string and add it to the registered test ids array.
    TPtrC testIdPtr;
    while(delimit.GetNextTrimmed(testIdPtr) == KErrNone)
        {
        TTestId testId(testIdPtr);
		
		//This cannot fail under legitimate circumstances, since enough 
		//space is reserved in this array when registering TestIds.
        TInt err = iCurrentTestIds.Append(testId);
		ASSERT(err == KErrNone);
        }
    
    //Make sure these tests were registered and remove from the registered array.
    for(TInt i=0; i < iCurrentTestIds.Count(); i++)
        {
        TInt idx = iRegisteredTestIds.Find(iCurrentTestIds[i]);
        ASSERT(idx != KErrNotFound);
        iRegisteredTestIds.Remove(idx);
        }
    }


void CTestIdResultLogger::LogResult(TVerdict aVerdict)
    {
    const TInt KMaxVerdictLength = 20;
    TBuf<KMaxVerdictLength> verdict;
    switch(aVerdict)
        {
        case EPass:           verdict.Append(_L("PASS"));            break;
        case EFail:           verdict.Append(_L("FAIL"));            break;
        case EInconclusive:   verdict.Append(_L("INCONCLUSIVE"));    break;
        case ETestSuiteError: verdict.Append(_L("TEST SUTE ERROR")); break;
        case EAbort:          verdict.Append(_L("ABORT"));           break;
        case EIgnore:         verdict.Append(_L("IGNORE"));          break;
        }
    
    while(iCurrentTestIds.Count())
        {
        LogResult(iLogger, iCurrentTestIds[0], verdict);
        iCurrentTestIds.Remove(0);
        }
    }


void CTestIdResultLogger::LogResult(CTestExecuteLogger& aLogger, const TTestId& aTestId, const TDesC& aVerdict)
    {
    aLogger.LogExtra(((TText8*)"EGL ENDPOINT TEST RESULT >>>"), 0, ESevrInfo, _L("GRAPHICS-EGL-%S: %S"), &aTestId, &aVerdict);
    }


TInt CTestIdResultLogger::PanicMonitorMain(TAny* aSelf)
    {
    CTestIdResultLogger* self = static_cast<CTestIdResultLogger*>(aSelf);
    
    //Create cleanup stack.
    CTrapCleanup* cleanup = CTrapCleanup::New();
    ASSERT(cleanup);
    
    //Create active scheduler.
    CActiveScheduler* scheduler = new CActiveScheduler();
    ASSERT(scheduler);
    CActiveScheduler::Install(scheduler);
    
    TRAPD(err,  self->PanicMonitorMainL());
    __ASSERT_ALWAYS(err == KErrNone, User::Invariant());
    
    delete scheduler;
    delete cleanup;
    return KErrNone;
    }
    

void CTestIdResultLogger::PanicMonitorMainL()
    {
    //Setup logging.
    CTestExecuteLogger logger;
    TEndpointUtil::SetLoggerForProcessWrapperL(logger);
    
    //Tell parent how to close us.
    TRequestStatus closeStatus = KRequestPending;
    iCloseMonitor = &closeStatus;
    
    //Open parent thread and logon.
    RThread origThread;
    User::LeaveIfError(origThread.Open(iOriginalThread, EOwnerThread));
    TRequestStatus origStatus;
    origThread.Logon(origStatus);
    
    //Rendevous with our parent then wait for thread to exit or close command.
    RThread().Rendezvous(KErrNone);
    User::WaitForRequest(closeStatus, origStatus);
    
    if (closeStatus != KRequestPending)
        {
        //Parent is shutting us down. Just cancel our outstanding request and exit.
        origThread.LogonCancel(origStatus);
        User::WaitForRequest(origStatus);
        }
    else if (origStatus != KRequestPending)
        {
        //We can only get here if parent panicked.
        //Log that all current tests were panicked and all registered tests were not run.
        _LIT(KPanicked, "PANIC");
        _LIT(KNotRun, "NOT RUN DUE TO PREVIOUS PANIC");
        while(iCurrentTestIds.Count())
            {
            LogResult(logger, iCurrentTestIds[0], KPanicked);
            iCurrentTestIds.Remove(0);
            }
        while(iRegisteredTestIds.Count())
            {
            LogResult(logger, iRegisteredTestIds[0], KNotRun);
            iRegisteredTestIds.Remove(0);
            }
        }
    
    origThread.Close();
    }


CEglEndpointTestServer* CEglEndpointTestServer::NewL()
	{
	CEglEndpointTestServer* server = new(ELeave) CEglEndpointTestServer();
	CleanupStack::PushL(server);
	// CServer base class call
	TParsePtrC serverName(RProcess().FileName());
	server->StartL(serverName.Name());
	CleanupStack::Pop(server);
	return server;
	}

static void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	CEglEndpointTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CEglEndpointTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

/**
  @return Standard Epoc error code on process exit
  Process entry point. Called by client using RProcess API
  */
TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());

	if (err)
		{
		RDebug::Print(_L("CEglEndpointTestServer::MainL - Error: %d"), err);
	   	User::Panic(KEglEndpointTestServerName, err);
		}

	delete cleanup;
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	return KErrNone;
    }
