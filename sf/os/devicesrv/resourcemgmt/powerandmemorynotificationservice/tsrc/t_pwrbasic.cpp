// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This test will be able to do what it is planned to do only if the shutdown client and server
// components are built with SYSLIBS_TEST macro defined.
// 
//

#include <e32test.h>        //RTest
#include <e32svr.h>         //RDebug
#include <savenotf.h>       //RSaveSession, MSaveObserver, CSaveNotifier
#include <e32consf.h>		//for logging to file

static RTest TheTest(_L("T_PwrBasic"));
const TInt KOneSec = 1000000;
_LIT(KTestFailed, "Invalid Test Step %d\n");

// The timeout of 2 seconds which is longer than the one defined at server side(1.5 seconds).
// It triggers events after CServShutdownServer::SwitchOff() has been called.
const TUint32 KShtdwnTimeoutLonger =2000000;

// The timeout of 1 second which is shorter than the one defined at server side(1.5 seconds).
// It triggers events before CServShutdownServer::SwitchOff() has been called.
const TUint32 KShtdwnTimeoutShorter =1000000;

//
//
//Test macroses and functions
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

//

static void Leave(TInt aLine, TInt aError)
	{
	RDebug::Print(_L("*** Leave. Error: %d, line: %d\r\n"), aError, aLine);
	User::Leave(aError);
	}
static void LeaveIfError(TInt aLine, TInt aError)
	{
	if(aError < KErrNone)
		{
		::Leave(aLine, aError);
		}
	}
static void LeaveIfNull(TInt aLine, TAny* aPtr)
	{
	if(!aPtr)
		{
		::Leave(aLine, KErrNoMemory);
		}
	}
#define __LEAVE(err) ::Leave(__LINE__, err)
#define __LEAVE_IF_ERROR(err) ::LeaveIfError(__LINE__, err)
#define __LEAVE_IF_NULL(ptr) ::LeaveIfNull(__LINE__, ptr)

//
//
//Test classes & objects

//This test class is used to receive a powerdown notification from the shutdown server.
class CPowerdownClient : public CActive, public MSaveObserver
    {
public:
    static CPowerdownClient* NewLC();
    virtual ~CPowerdownClient();
    virtual void SaveL(MSaveObserver::TSaveType aSaveType);

protected:
	virtual void DoCancel();
	virtual void RunL();

private:
    CPowerdownClient();
    void ConstructL();

private:
    RTimer iTimer;
    CSaveNotifier* iSaveNotifier;

    };

CPowerdownClient* CPowerdownClient::NewLC()
    {
    CPowerdownClient* self = new CPowerdownClient;
    __LEAVE_IF_NULL(self);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CPowerdownClient::~CPowerdownClient()
    {
    Cancel();
    iTimer.Close();
    delete iSaveNotifier;
    }

//MSaveNotifier::SaveL() implementation. Called when powerdown event occurs.
void CPowerdownClient::SaveL(MSaveObserver::TSaveType)
    {
    iSaveNotifier->DelayRequeue();

    iTimer.After(iStatus, KOneSec);
    TEST2(iStatus.Int(), KRequestPending);
    SetActive();
    }

void CPowerdownClient::DoCancel()
    {
    iTimer.Cancel();
    }

//Processes timer events.
void CPowerdownClient::RunL()
    {
    static TBool powerOffCalled = EFalse;
    if(!powerOffCalled)
        {
        TEST2(iSaveNotifier->SwitchOff(MSaveObserver::ESaveAll, ETrue), KErrNone);
        powerOffCalled = ETrue;
        }
    else
        {
        iSaveNotifier->HandleError(KErrGeneral);
        User::After(KOneSec);
        CActiveScheduler::Stop();
        }
    }

//Adds the CPowerdownClient object to the active scheduler.
CPowerdownClient::CPowerdownClient() :
    CActive(CActive::EPriorityStandard)
    {
	CActiveScheduler::Add(this);
    }

//Constructs CPowerdownClient object and generates a timer event, processed 1 second later.
void CPowerdownClient::ConstructL()
    {
    iSaveNotifier = CSaveNotifier::NewL(*this);
    __LEAVE_IF_ERROR(iTimer.CreateLocal());
    iTimer.After(iStatus, KOneSec);
    TEST2(iStatus.Int(), KRequestPending);
    SetActive();
    }
/**
  DEF048920 - PwrCli server may panic if CSaveNotifier::HandleError() is called from...
  What the test does is:
  1) The test creates a CPowerdownClient object.
  2) CPowerdownClient object will raise a timer event, which will be processed after 1 second.
  3) The test starts the scheduler.
  4) The timer event gets processed in CPowerdownClient::RunL() - it initiates a powerdown
     sequence.
  5) The shutdown server will notify the client (CPowerdownClient), calling 
     CPowerdownClient::SaveL() method.
  6) CPowerdownClient::SaveL() will call CSaveNotifier::DelayRequeue() not to re-register itself. 
     Then it will raise a timer event, which will be processed after 1 second.
  7) The second timer event gets processed in CPowerdownClient::RunL() - it calls 
     CSaveNotifier::HandleError(). 
     Without the fix CSaveNotifier::HandleError() causes "BadHandle" panic on the server side.

@SYMTestCaseID          SYSLIB-PWRLCLI-CT-3686
@SYMTestCaseDesc	    Tests for defect number DEF048920
@SYMTestPriority 	    High
@SYMTestActions  	    Initiate a powerdown sequence.The shutdown server will notify the client,which delays the 
                        power off and then raises a timer event which stops the scheduler   
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void DEF048920L()
    {	
	CActiveScheduler* scheduler = new CActiveScheduler;
    __LEAVE_IF_NULL(scheduler);
    CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

    CPowerdownClient* powerdownClient = CPowerdownClient::NewLC();

	CActiveScheduler::Start();

    CleanupStack::PopAndDestroy(powerdownClient);
    CleanupStack::PopAndDestroy(scheduler);
    }



// This test class is used to receive a powerdown notification from the shutdown server.
// Its SaveL() calls MSaveObserver::DelayRequeue() not to re-register itself. Its RunL() may call 
// CSaveNotifier::HandleError() to send error to the server. But both behaviors will not stop the server's 
// shutdown execution.
class CPowerdownClient2 : public CActive, public MSaveObserver
    {
public:
    static CPowerdownClient2* NewLC();
    virtual ~CPowerdownClient2();
    virtual void SaveL(MSaveObserver::TSaveType aSaveType);

protected:
	virtual void DoCancel();
	virtual void RunL();

private:
    CPowerdownClient2();
    void ConstructL();

private:
	enum TTestSteps
		{
		TRequestSwitchOff1,
		TClientSave1,
		TClientCheckServerPowerOff1,		
		TRequestSwitchOff2,
		TClientSave2,
		TClientSendError,
		TClientCheckServerPowerOff2,
		TTestStepEnd,
		};

private:
    RTimer iTimer;
    CSaveNotifier* iSaveNotifier;
    TTestSteps iTestStep;
    };

CPowerdownClient2* CPowerdownClient2::NewLC()
    {
    CPowerdownClient2* self = new CPowerdownClient2();
    __LEAVE_IF_NULL(self);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CPowerdownClient2::~CPowerdownClient2()
    {
    Cancel();
    iTimer.Close();
    delete iSaveNotifier;
    }

//MSaveNotifier::SaveL() implementation. Called when powerdown event occurs.
void CPowerdownClient2::SaveL(MSaveObserver::TSaveType)
    {
	__ASSERT_ALWAYS(iTestStep == TClientSave1 || iTestStep == TClientSave2, \
		TheTest.Panic(KErrNotSupported, KTestFailed, iTestStep));
	
	TBool powerOff = EFalse;
	
	// Checks the power state of the server. It should be off because the server has received
	// the power off request.
	// Calls DelayRequeue() not to re-register itself.
	// Starts a timer which is longer than the server's shutdown timer.
	// Move to next test step
	if(iTestStep == TClientSave1)
		{
		iSaveNotifier->ServerPowerState(powerOff);
		TEST(powerOff);		
		iSaveNotifier->DelayRequeue();
   		iTestStep = TClientCheckServerPowerOff1;
   		iTimer.After(iStatus, KShtdwnTimeoutLonger);
   		TEST2(iStatus.Int(), KRequestPending);
   		SetActive();
		}

	// Checks the power state of the server. It should be off because the server has received
	// the power off request.
	// Calls DelayRequeue() not to re-register itself.
	// Starts a timer which is shorter than the server's shutdown timer.
	// Move to next test step
   	if(iTestStep == TClientSave2)
   		{
		iSaveNotifier->ServerPowerState(powerOff);
		TEST(powerOff);		
		iSaveNotifier->DelayRequeue();
		iTestStep = TClientSendError;
   		iTimer.After(iStatus, KShtdwnTimeoutShorter);
   		TEST2(iStatus.Int(), KRequestPending);
   		SetActive();
   		}
    }

void CPowerdownClient2::DoCancel()
    {
    iTimer.Cancel();
    }

//Processes timer events.
void CPowerdownClient2::RunL()
    {
	__ASSERT_ALWAYS(iTestStep == TRequestSwitchOff1 || iTestStep == TClientCheckServerPowerOff1 ||\
				 iTestStep == TClientSendError, TheTest.Panic(KErrNotSupported, KTestFailed, iTestStep));    
	
	TBool powerOff = ETrue;
	
	// Requests server for switchoff for the first time
    if(iTestStep == TRequestSwitchOff1)
    	{
    	TEST2(iSaveNotifier->SwitchOff(MSaveObserver::ESaveAll, ETrue), KErrNone);
    	iTestStep = TClientSave1;
    	}
    
	// Checks the power state of the server. It should be on because CServShutdownServer::SwitchOff()
	// has been called.
    // Requests server for switchoff for the second time.
    if(iTestStep == TClientCheckServerPowerOff1)
    	{
		iSaveNotifier->ServerPowerState(powerOff);
		TEST(!powerOff);
    	iSaveNotifier->Queue(); // client re-register after the system resumes.
    	iTestStep = TRequestSwitchOff2;
    	User::After(KOneSec);
    	iTestStep = TClientSave2;
    	TEST2(iSaveNotifier->SwitchOff(MSaveObserver::ESaveAll, ETrue), KErrNone);
    	}
    	
	// Checks the power state of the server. It should be off because CServShutdownServer::SwitchOff()
	// has not been called.
	// Sends error to server. The server will ignore the error message. 
	// HandleError re-registers the client thus triggers the CServShutdownServer::SwitchOff().
	// Checks the power state of the server again. It should be on because CServShutdownServer::SwitchOff()
	// has been called.
	// Stops the active scheduler.
    if(iTestStep == TClientSendError)
    	{
		iSaveNotifier->ServerPowerState(powerOff);
		TEST(powerOff);	
    	iSaveNotifier->HandleError(KErrGeneral);
    	iTestStep = TClientCheckServerPowerOff2;
    	User::After(KOneSec);
		iSaveNotifier->ServerPowerState(powerOff);
		TEST(!powerOff);		
		iTestStep = TTestStepEnd;
		CActiveScheduler::Stop();
    	}
    }

//Adds the CPowerdownClient2 object to the active scheduler.
CPowerdownClient2::CPowerdownClient2() :
    CActive(CActive::EPriorityStandard), iTestStep(TRequestSwitchOff1)
    {
	CActiveScheduler::Add(this);
    }

//Constructs CPowerdownClient2 object and generates a timer event, processed 1 second later.
void CPowerdownClient2::ConstructL()
    {
    iSaveNotifier = CSaveNotifier::NewL(*this);
    __LEAVE_IF_ERROR(iTimer.CreateLocal());
    iTimer.After(iStatus, KOneSec);
    TEST2(iStatus.Int(), KRequestPending);
    SetActive();
    }


/**
  DEF111025 - Device restarts from PREQ1089 UIF functionality can be delayed indefinitely.
  What this test does is:
  1) The test creates a CPowerdownClient2 object, which raises a 1-second timer event. The test starts 
     the scheduler.
  2) The timer event gets processed in CPowerdownClient2::RunL() - it initiates a powerdown
     sequence.
  3) The shutdown server notifies the client (CPowerdownClient2), calling CPowerdownClient2::SaveL() 
     method.
  4) CPowerdownClient2::SaveL() gets the power state of the server and checks whether it is off to ensure 
     the server has received the shutdown request but not executed CServShutdownServer::SwitchOff() yet. 
     It calls CSaveNotifier::DelayRequeue() not to re-register itself. 
     Then it raises a timer, which is longer than the shutdown timer defined at the server side.
  5) The timer event gets processed in CPowerdownClient::RunL(). It gets the power state of the server 
     and checks whether it is on to ensure the server has had CServShutdownServer::SwitchOff() executed by 
     the shutdown timer. 
     Then it initiates a powerdown sequence for the second time.
  6) The shutdown server notifies the client (CPowerdownClient2), calling  CPowerdownClient2::SaveL() 
     method.
  7) CPowerdownClient2::SaveL() gets the power state of the server and checks whether it is off to ensure 
     the server has received the shutdown request but not executed CServShutdownServer::SwitchOff() yet. 
     It calls CSaveNotifier::DelayRequeue() not to re-register itself. 
     Then it raises a timer, which is shorter than the shutdown timer defined at the server side.
  8) The timer event gets processed in CPowerdownClient::RunL(). It gets the power state of the server 
     and checks whether it is off to ensure the server still has not executed CServShutdownServer::SwitchOff(). 
     It calls CSaveNotifier::HandleError() to send an error to the server. After sometime, It gets the power 
     state of the server again and checks whether it is on to ensure the server has had 
     CServShutdownServer::SwitchOff() executed by the shutdown timer.
     Then it stops the active scheduler.
     
  Without the fix the checking of power state of the server would fail in a certain step.

@SYMTestCaseID          SYSLIB-PWRCLI-CT-4001
@SYMTestCaseDesc	    Tests for defect number DEF111025
@SYMTestPriority 	    High
@SYMTestActions  	    Initiates a powerdown sequence which sends 2 poweroff requests to server. The shutdown 
					    server will notify the client twice. The first time when being notified, the client will 
					    not re-register itself. For the second time, it will send error to the server. During 
					    this procedure, some checkpoints will be set to check the power states of the server.
@SYMTestExpectedResults The test program should not panic or leave. At power state checkpoints, the power states 
						queried from the server should be the expected values.
@SYMDEF                 DEF111025
*/		
void DEF111025L()
	{	
	CActiveScheduler* scheduler = new CActiveScheduler;
    __LEAVE_IF_NULL(scheduler);
    CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	
    CPowerdownClient2* powerdownClientTimer = CPowerdownClient2::NewLC();

	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(powerdownClientTimer);
    CleanupStack::PopAndDestroy(scheduler);
	}
//
//
//Tests

static void DoRunL()
    {
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-PWRLCLI-CT-3686  DEF048920 "));
    ::DEF048920L(); 
    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-PWRCLI-CT-4001 DEF111025 "));
    ::DEF111025L();
    }

TInt E32Main()
	{
	LogRTestToFile(TheTest);
    TheTest.Title();
	__UHEAP_MARK;
	CTrapCleanup* tc = CTrapCleanup::New();
    TEST(tc != NULL);

	TheTest.Start(_L("Power shutdown tests"));

    TRAPD(err, ::DoRunL());
	TEST2(err, KErrNone);

	TheTest.End();
	TheTest.Close();
	
	delete tc;

	__UHEAP_MARKEND;

	User::Heap().Check();
	return KErrNone;
	}
