// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>        //RTest
#include <e32svr.h>         //RDebug
#include <savenotf.h>       //RSaveSession, MSaveObserver, CSaveNotifier
#include <e32consf.h>		//for logging to file
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <shutdownsrv.h>    //CServShutdownServer
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
#include "shutdownsess.h" //CServShutdownServer
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include <savepriv.h>

static RTest TheTest(_L("T_ShutdownServer"));
const TInt KOneSec = 1000000;


// The timeout of 1 second which is shorter than the one defined at server side(1.5 seconds).
// It will give a pause to close all requests in thread
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
        TBool powerOff = ETrue;
        iSaveNotifier->ServerPowerState(powerOff);
		TEST(powerOff);
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
 This function is creating the test shutdown server and starting its scheduler.
 */
static void RunAdaptionSrvL(CServShutdownServer* aServer)
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler();
    CleanupStack::PushL( sched );
    CActiveScheduler::Install( sched );
   
    aServer->ConstructL();
    RThread::Rendezvous( KErrNone );
    
    CActiveScheduler::Start();
    delete aServer;
    CleanupStack::PopAndDestroy( sched );
	}


/**
 This function is called when the StartShutDownSrv's thread is resumed.
 */
static TInt AdaptionSrvInitFunction( TAny* aServer )
	{
    CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt err = KErrNoMemory;
	if (cleanup)
		{
	    TRAP( err, RunAdaptionSrvL(static_cast<CServShutdownServer*>(aServer)));
	    delete cleanup;
		}
    return err;
	}
//
// Functions to launch the servers (these will run in the main thread)
//

/**
Function to launch the shutdown server in a new thread.
 */
static TInt StartShutDownSrv(CServShutdownServer* aServer,RThread* aServerThread )
	{
	//RThread server;
	TInt err = aServerThread->Create( __TEST_SHUTDOWN_SERVER_NAME, &AdaptionSrvInitFunction, 
								KDefaultStackSize, NULL, &(*aServer ));

	if( KErrNone == err )
		{
		TRequestStatus trs;
		aServerThread->Rendezvous( trs );
		aServerThread->Resume();
		
		User::WaitForRequest( trs );	
		
		//We can't use the 'exit reason' if the server panicked as this is the 
		//panic 'reason' and may be '0' which cannot be distinguished from KErrNone
		err = (aServerThread->ExitType()==EExitPanic) ? KErrGeneral : trs.Int();
		}
	return err;
	}

/**
@SYMTestCaseID 			SYSLIB-PWRLCLI-UT-3693
@SYMTestCaseDesc 		Test the ClientArrayLC functionality of shutdownserver.
@SYMPREQ				PREQ1089
@SYMREQ					REQ6883
@SYMTestStatus			Implemented
@SYMTestPriority		Critical
@SYMTestPurpose			To test the existing default behaviour of shutdownserver 
@SYMTestActions			1.Create a test shutdown server in a thread.
						2.Test the power down state in shutdown server
						3.Register with shutdown server for powerdown notification.
						4.Test that registered clients with the shutdown server.
						5.Test that registered clients does not have any pending requests.
						
@SYMTestExpectedResults	1.Registered clients with shutdown server is 1
						2.Registered clients will not have any pending requests.
*/	
void TestShutDownSrvObserverFunctions()
    {
 	CActiveScheduler* scheduler = new CActiveScheduler;
    __LEAVE_IF_NULL(scheduler);
    CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	//Create a test server in a different thread.
	RThread serverThread;
    CServShutdownServer* server = CServShutdownServer::NewL();
    StartShutDownSrv(server,&serverThread);
	//Power down state will be 0. By default power state is set to 0.
	TBool allSessionsHavePendingRequest;
	TBool powerOff;
	server->GetShutdownState(powerOff,allSessionsHavePendingRequest);
	TEST(!powerOff);
	//Register to shutdown server for power down notification
    CPowerdownClient* powerdownClient = CPowerdownClient::NewLC();
	//CPowerdownClient will test the power down states of shutdown server and completes the request.
	CActiveScheduler::Start();
	
	//Test that shutdown server has only one registered client.
	CArrayFix<TThreadId>* array = server->ClientArrayLC();
	TEST(array->Count() == 1);
	//Test that registered client has no pending requests.
	TBool res = server->IsClientHung(array->At(0));	
	TEST(!res);
		
	serverThread.Kill(KErrNone);
	serverThread.Close();
	User::After(KShtdwnTimeoutShorter);				
	CleanupStack::PopAndDestroy(array);	
    CleanupStack::PopAndDestroy(powerdownClient);
    CleanupStack::PopAndDestroy(scheduler);
    }


//
//
//Tests
static void DoRunL()
    {
    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-PWRLCLI-UT-3693 Test the ClientArrayLC functionality of shutdownserver."));
    ::TestShutDownSrvObserverFunctions();
    }

TInt E32Main()
	{
	LogRTestToFile(TheTest);
    TheTest.Title();
	CTrapCleanup* tc = CTrapCleanup::New();
    TEST(tc != NULL);

	TheTest.Start(_L("Power shutdown tests"));

    TRAPD(err, ::DoRunL());
	TEST2(err, KErrNone);

	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
