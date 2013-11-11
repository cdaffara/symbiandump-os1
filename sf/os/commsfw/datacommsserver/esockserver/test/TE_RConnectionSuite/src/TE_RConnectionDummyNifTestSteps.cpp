// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains the implementation of the test cases that predominately use the dummy nif
// as the default interface.
//
//

/**
 @file
*/

#include "TE_RConnectionDummyNifTestSteps.h"
// for the null agt progress values
#include <nullagtprog.h>
#include <connectprog.h>
#include "TE_RConnPanicStep.cpp"
#include <networking/pppprog.h>
#include <nifman.h>
#include <inet6err.h>
#include <dummynifvar.h>
#include <comms-infras/esocktestutils.h>
#include <es_panic.h>

#include <e32cons.h>
#include <comms-infras/es_commsdataobject.h>
#include <commdbconnpref.h>
#include <commsdattypeinfov1_1.h>
#include <comms-infras/connectionqueryset.h>
#include <csdprog.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestTERCnct, "ESockTestTERCnct");
#endif


using namespace ESock;

TE_RConnectionTest1::~TE_RConnectionTest1()
{
}


// creates a connection and destroys it again
enum TVerdict TE_RConnectionTest1::doTestStepL()
{
	TInt err;

	RConnection conn;
	RSocketServ ss;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();
	INFO_PRINTF1(_L("TEST ABOUT TOO COMPLETE..."));
	return TestStepResult();
} // TE_RConnectionTest1



/**
Testing closing a connection during its startup.
C++ constructor
*/

TE_RConnectionTest1b::~TE_RConnectionTest1b()
	{
	}

class MProgressObserverTest1b
	{
public:
	virtual void MPOStageReached(TInt aStage, TInt aError) = 0;
	};

class CProgressWatcherTest1b : public CActive
	{
public:
	static CProgressWatcherTest1b* NewL(RConnection& aConnection, MProgressObserverTest1b& aObserver);
	virtual ~CProgressWatcherTest1b();
	
	const TNifProgressBuf& Progress() const { return iNifProgressBuf; }

protected:
	virtual void DoCancel();
	virtual void RunL();

private:
	CProgressWatcherTest1b(RConnection& aConnection, MProgressObserverTest1b& aObserver);
	void ConstructL();
	void RequestProgressNotification();

private:
	MProgressObserverTest1b& iObserver;
	RConnection& iConnection;
	TNifProgressBuf iNifProgressBuf;
	};

CProgressWatcherTest1b* CProgressWatcherTest1b::NewL(RConnection& aConnection, MProgressObserverTest1b& aObserver)
	{
	CProgressWatcherTest1b* self = new(ELeave) CProgressWatcherTest1b(aConnection, aObserver);
	return self;
	}

CProgressWatcherTest1b::CProgressWatcherTest1b(RConnection& aConnection, MProgressObserverTest1b& aObserver)
: CActive(CActive::EPriorityStandard), iObserver(aObserver), iConnection(aConnection)
	{
	CActiveScheduler::Add(this);
	RequestProgressNotification();
	}

CProgressWatcherTest1b::~CProgressWatcherTest1b()
	{
	Cancel();
	}

void CProgressWatcherTest1b::DoCancel()
	{
    if (iConnection.SubSessionHandle() != 0)
        {
        iConnection.CancelProgressNotification();
        }
	}

void CProgressWatcherTest1b::RunL()
	{
	if(iStatus == KErrNone)
		{
		iObserver.MPOStageReached(iNifProgressBuf().iStage, iNifProgressBuf().iError);
		}
	RequestProgressNotification();
	}

void CProgressWatcherTest1b::RequestProgressNotification()
	{
	if (!IsActive())
		{
		iConnection.ProgressNotification(iNifProgressBuf,iStatus);
		SetActive();
		}
	}

class CConnectionTest1b : public CActive
	{
public:
	static CConnectionTest1b* NewL(MProgressObserverTest1b& aObserver, CTestExecuteLogger& aLogger);
	virtual ~CConnectionTest1b();

	void Start();
	void StopProgressObservation();
	void GetConnectionInfoL(TConnectionInfo& aInfo);

protected:
	virtual void DoCancel();
	virtual void RunL();

private:
	CConnectionTest1b(CTestExecuteLogger& aLogger);
	void ConstructL(MProgressObserverTest1b& aObserver);

	inline CTestExecuteLogger& Logger() {return iLogger;};

private:
	CTestExecuteLogger& iLogger;
	RSocketServ iSocketServ;
	RConnection iConnection;
	CProgressWatcherTest1b* iProgressWatcher;
	};

CConnectionTest1b* CConnectionTest1b::NewL(MProgressObserverTest1b& aObserver, CTestExecuteLogger& aLogger)
	{
	CConnectionTest1b* self = new(ELeave) CConnectionTest1b(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver);
	CleanupStack::Pop(self);
	return self;
	}

CConnectionTest1b::CConnectionTest1b(CTestExecuteLogger& aLogger)
: CActive(CActive::EPriorityStandard), iLogger(aLogger)
	{
	CActiveScheduler::Add(this);
	}

void CConnectionTest1b::ConstructL(MProgressObserverTest1b& aObserver)
	{
	INFO_PRINTF1(_L("Opening Connection"));

	User::LeaveIfError(iSocketServ.Connect());
	User::LeaveIfError(iConnection.Open(iSocketServ));
	iProgressWatcher = CProgressWatcherTest1b::NewL(iConnection, aObserver);
	}

CConnectionTest1b::~CConnectionTest1b()
	{
	if(iConnection.SubSessionHandle() && iProgressWatcher)
		{
		INFO_PRINTF2(_L("Closing Connection at Stage %d"), iProgressWatcher->Progress()().iStage);
		}
	
    Cancel();
	delete iProgressWatcher;
	iProgressWatcher = NULL;

	iSocketServ.Close();
	}

void CConnectionTest1b::Start()
	{
	INFO_PRINTF1(_L("Starting Connection"));
	iConnection.Start(iStatus);
	SetActive();
	}

void CConnectionTest1b::StopProgressObservation()
	{
	delete iProgressWatcher;
	iProgressWatcher = NULL;
	}

void CConnectionTest1b::DoCancel()
	{
    iConnection.Close();
	StopProgressObservation();
	}

void CConnectionTest1b::RunL()
	{
	}

class CTest1b : public CActive, public MProgressObserverTest1b
	{
public:
	static CTest1b* NewLC(CTestExecuteLogger& aLogger);
	virtual ~CTest1b();

	virtual void MPOStageReached(TInt aStage, TInt aError);

protected:
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);

private:
	CTest1b(CTestExecuteLogger& aLogger);
	void ConstructL();

	inline CTestExecuteLogger& Logger() {return iLogger;};

private:
	CTestExecuteLogger& iLogger;
	CConnectionTest1b* iConnection;
	TInt iCancelAtStage;
	TInt iCurrentStage;
	TBool iLaststage;
	};

CTest1b* CTest1b::NewLC(CTestExecuteLogger& aLogger)
	{
	CTest1b* self = new(ELeave) CTest1b(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTest1b::CTest1b(CTestExecuteLogger& aLogger)
: CActive(CActive::EPriorityStandard), iLogger(aLogger)
	{
	CActiveScheduler::Add(this);
	}

void CTest1b::ConstructL()
	{
	INFO_PRINTF2(_L("Waiting for progress #%d"), iCancelAtStage);

	iConnection = CConnectionTest1b::NewL(*this, Logger());
	iConnection->Start();
	}

CTest1b::~CTest1b()
	{
	Cancel();
	delete iConnection;
	}

void CTest1b::MPOStageReached(TInt aStage, TInt aError)
	{
	INFO_PRINTF3(_L("Progress received - stage: %d, error: %d"), aStage, aError);

	iCurrentStage++;
	iLaststage = (aStage==0) & (aError==0);
	if(iCancelAtStage <= iCurrentStage || iLaststage)
		{
		SetActive();
		TRequestStatus* pS = &iStatus;
		User::RequestComplete(pS, aError);
		}
	}

void CTest1b::DoCancel()
	{
	RDebug::Print(_L("\nCTest::DoCancel"));
	delete iConnection;
	iConnection = NULL;
	}

void CTest1b::RunL()
	{
	delete iConnection;
	iConnection = NULL;

	if (iLaststage)
		{
		INFO_PRINTF1(_L("Completed test successfully"));

		CActiveScheduler::Stop();
		}
	else
		{
		iCancelAtStage++;

		INFO_PRINTF2(_L("Waiting for progress #%d"), iCancelAtStage);

		iConnection = CConnectionTest1b::NewL(*this, Logger());
		iCurrentStage = 1;
		iConnection->Start();
		}
	}

TInt CTest1b::RunError(TInt aError)
	{
	INFO_PRINTF3(_L("Error %d while waiting for progress #%d"), aError, iCancelAtStage);

	CActiveScheduler::Stop();
	return KErrNone;
	}

LOCAL_C void DoTestL(CTestExecuteLogger& aLogger)
	{
	CTest1b* test = CTest1b::NewLC(aLogger);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(test);
	}

enum TVerdict TE_RConnectionTest1b::doTestStepL(void)
	{
	__UHEAP_MARK;
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	TRAPD(r, DoTestL(Logger()));
	RDebug::Print(_L("TE_RConnectionTest1b::DoTestL returned error %d"), r);
	delete trapCleanup;
	__UHEAP_MARKEND;

	return TestStepResult();
	}



TE_RConnectionTest2::~TE_RConnectionTest2()
{
}

// creates a connection and destroy it again - use the synchronous start
enum TVerdict TE_RConnectionTest2::doTestStepL(void)
{
	TInt err;

	RConnection conn;
	RSocketServ ss;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionSynchronous(conn);
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest2



TE_RConnectionTest3::~TE_RConnectionTest3()
{
}

// Create a connection using database overrides and then destroy it
enum TVerdict TE_RConnectionTest3::doTestStepL(void)
{
	TInt err;

	RConnection conn;
	RSocketServ ss;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest3


TE_RConnectionTest4::~TE_RConnectionTest4()
{
}

// Create a connection using database overrides and then destroy it - use the synchronous start
enum TVerdict TE_RConnectionTest4::doTestStepL(void)
{
	TInt err;

	RConnection conn;
	RSocketServ ss;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverridesSynchronous(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest4


TE_RConnectionTest5::~TE_RConnectionTest5()
{
}

// Implicitly create a single connection using SendTo()
enum TVerdict TE_RConnectionTest5::doTestStepL(void)
{
	TInt err;

	RSocketServ ss;
	RSocket sock;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr); // this will use SendTo()
	TESTEL(KErrNone == err, err);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest5


TE_RConnectionTest6::~TE_RConnectionTest6()
{
}

// Explicitly associate a UDP socket to an existing connection
enum TVerdict TE_RConnectionTest6::doTestStepL(void)
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest6


TE_RConnectionTest7::~TE_RConnectionTest7()
{
}

// Create two connections (separate interfaces) and explcitly associate a separate socket
// with each, sending data over each checking it goes to the correct interface each time.
enum TVerdict TE_RConnectionTest7::doTestStepL(void)
{
	TInt err;

	RSocketServ ss;
	RConnection conn1, conn2;
	RSocket sock1, sock2;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	// Set up the connections, one default and one overridden

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);
	err = StartConnection(conn1);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);
	err = StartConnectionWithOverrides(conn2, iHungryNifIap);
	TESTEL(KErrNone == err, err);
	
	// Set up the sockets

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = OpenUdpSocketExplicitL(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	// Test the interfaces. Conn1 should be fine (default settings, decent
	// dummy NIF) Conn2 should be a black hole for all traffic (should time out)
	
	//If DummyNif is used to send the data (over UDP sockets) first,
	//then data received back would be considered for both the sockets, in which
	//case , when we try to receive the data using second connection (using HungryNif),
	//stack would not ask the provider for data, as previously received data from
	//first connection (using DummyNif) would be available for second connection 
	//(using HungryNif) as well.(Stack code check if it has multiple receivers for 
	//data received over UDP, and make data available for all receivers.)
	err = TestUdpDataPathL(sock2, iHungryNifSendAddr);
	TESTEL(KErrTimedOut == err, err);
	
	err = TestUdpDataPathL(sock1, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);
	
	// Clean up the mess, get rid of the sockets and the connections

	DestroyUdpSocket(sock2);
	CleanupStack::Pop();

	DestroyUdpSocket(sock1);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest7


TE_RConnectionTest8::~TE_RConnectionTest8()
{
}

enum TVerdict TE_RConnectionTest8::doTestStepL(void)
/*
 * Ensure that initially chosen interface with implicit connection creation
 * matches that chosen for later reuses (using UDP sockets)
 */
{
	TInt err;

	RSocketServ ss;
	RSocket sock;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	// this should implicitly create a connection
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest8


TE_RConnectionTest9::~TE_RConnectionTest9()
{
}

enum TVerdict TE_RConnectionTest9::doTestStepL(void)
/*
 * Ensure that initially chosen interface with implicit connection creation
 * matches that chosen for later reuses (using UDP sockets) after half of the short time out
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;
	TTimeoutValues timeouts;
	TUint numOfConnections;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	/*
	 * Temporarily create a connection to read the timeout vals from comm db
	 */

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	err = GetTimeoutValues(conn, timeouts);
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop();

	err = WaitForAllInterfacesToCloseL(ss);
	TESTEL(KErrNone == err, err);

	/*
	 * Now do the test itself
	 */

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	// this should implicitly create a connection
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	// wait for half of the short timeout
	User::After((timeouts.iShortTimeout*1000000)/2);

	// should reuse the existing interface
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	CloseConnection(conn);
	CleanupStack::Pop();

	/*
	 * Tidy up
	 */

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest9


TE_RConnectionTest10::~TE_RConnectionTest10()
{
}

enum TVerdict TE_RConnectionTest10::doTestStepL(void)
/*
 * Send data over two separate sockets using the same connection and interface
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock1, sock2;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketExplicitL(sock1, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = OpenUdpSocketExplicitL(sock2, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = TestUdpDataPathL(sock1, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	err = TestUdpDataPathL(sock2, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	DestroyUdpSocket(sock2);
	CleanupStack::Pop();

	DestroyUdpSocket(sock1);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest10


TE_RConnectionTest11::~TE_RConnectionTest11()
{
}

enum TVerdict TE_RConnectionTest11::doTestStepL(void)
/*
 * Two sockets (UDP) sending over different sessions (RConnection) to the same interface
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn1, conn2;
	RSocket sock1, sock2;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);
	err = StartConnection(conn1);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);
	err = StartConnection(conn2);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);
	err = OpenUdpSocketExplicitL(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = TestUdpDataPathL(sock1, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);
	err = TestUdpDataPathL(sock2, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	DestroyUdpSocket(sock2);
	CleanupStack::Pop();
	DestroyUdpSocket(sock1);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();
	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest11


TE_RConnectionTest12::~TE_RConnectionTest12()
{
}

enum TVerdict TE_RConnectionTest12::doTestStepL(void)
/*
 * Two sockets sending over the same interface accessing it from different socket servers
 */
{
	TInt err;

	RSocketServ ss1, ss2;
	RConnection conn1, conn2;
	RSocket sock1, sock2;
	TUint numOfConnections;

	err = OpenSocketServer(ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss1);
	err = OpenSocketServer(ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss2);

	err = OpenConnection(conn1, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);
	err = OpenConnection(conn2, ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	err = StartConnection(conn1);
	TESTEL(KErrNone == err, err);
	err = StartConnection(conn2);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	err = OpenUdpSocketExplicitL(sock1, ss1, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);
	err = OpenUdpSocketExplicitL(sock2, ss2, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = TestUdpDataPathL(sock1, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);
	err = TestUdpDataPathL(sock2, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	DestroyUdpSocket(sock2);
	CleanupStack::Pop();
	DestroyUdpSocket(sock1);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();
	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss2);
	CleanupStack::Pop();

	CloseSocketServer(ss1);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnection12



TE_RConnectionTest13::~TE_RConnectionTest13()
{
}

enum TVerdict TE_RConnectionTest13::doTestStepL(void)
/*
 * Two sockets sending over different interfaces from within different socket servers,
 * explicitly created connections.
 */
{
	TInt err;

	RSocketServ ss1, ss2;
	RConnection conn1, conn2;
	RSocket sock1, sock2;
	TUint numOfConnections;

	/*
	 * First set up the first interface/connection and its socket
	 */

	err = OpenSocketServer(ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss1);

	err = OpenConnection(conn1, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = StartConnectionWithOverrides(conn1, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketExplicitL(sock1, ss1, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	/*
	 * Now set up the second interface/connection and its socket
	 */

	err = OpenSocketServer(ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss2);

	err = OpenConnection(conn2, ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	err = StartConnectionWithOverrides(conn2, iHungryNifIap);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketExplicitL(sock2, ss2, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	/*
	 * Test that we have two interfaces created
	 */

	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == numOfConnections, numOfConnections);

	/*
	 * Now test the traffic paths
	 */
	
	//If DummyNif is used to send the data (over UDP sockets) first,
	//then data received back would be considered for both the sockets, in which
	//case , when we try to receive the data using second connection (using HungryNif),
	//stack would not ask the provider for data, as previously received data from
	//first connection (using DummyNif) would be available for second connection 
	//(using HungryNif) as well.(Stack code check if it has multiple receivers for 
	//data received over UDP, and make data available for all receivers.)
	
	err = TestUdpDataPathL(sock2, iHungryNifSendAddr);
	TESTEL(KErrTimedOut == err, err);
	
	err = TestUdpDataPathL(sock1, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	// Cleanup what we created
	DestroyUdpSocket(sock2);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseSocketServer(ss2);
	CleanupStack::Pop();

	DestroyUdpSocket(sock1);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss1);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest13


TE_RConnectionTest14::~TE_RConnectionTest14()
{
}

enum TVerdict TE_RConnectionTest14::doTestStepL(void)
/*
 * Two sockets sending over the same interface from within different socket servers,
 * implicitly created connections.
 */
{
	TInt err;

	RSocketServ ss1, ss2;
	RConnection conn;
	RSocket sock1, sock2;
	TUint numOfConnections = 0;

	/*
	 * Sort out the first socket
	 */

	err = OpenSocketServer(ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss1);

	err = OpenUdpSocketL(sock1, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	/*
	 * Sort out the second socket
	 */

	err = OpenSocketServer(ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss2);

	err = OpenUdpSocketL(sock2, ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	/*
	 * Try out the traffic paths, both should succeed
	 */

	err = TestUdpDataPathL(sock1, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	err = TestUdpDataPathL(sock2, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	/*
	 * Try and find out how many interfaces there are
	 */

	err = OpenConnection(conn, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	CloseConnection(conn);
	CleanupStack::Pop();

	/*
	 * Now tidy up in reverse order to creation so the order is right for the cleanup stack
	 */

	DestroyUdpSocket(sock2);
	CleanupStack::Pop();

	CloseSocketServer(ss2);
	CleanupStack::Pop();

	DestroyUdpSocket(sock1);
	CleanupStack::Pop();

	CloseSocketServer(ss1);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest14


TE_RConnectionTest15::~TE_RConnectionTest15()
{
}

enum TVerdict TE_RConnectionTest15::doTestStepL(void)
/*
 * Ensure that Close() effectively pulls down the interface when no other subsessions are
 * associated with it.
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;
	TUint numOfConnections = 0;
	TUint numOfInterfaces;

	TTimeoutValues timeouts;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	INFO_PRINTF2(_L("Number of active Connection = %d (1 expected)"), numOfConnections);
	TESTEL(1 == numOfConnections, numOfConnections);
	// With PREQ399, the implicit socket behavior has been deprecated. 
	// Sockets are not automatically associated with an already open connection, hence
	// we need to explicitly associate the socket with the connection we want to test.
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	
	// Reading Timeout values from configuration file
	err = GetTimeoutValues(conn, timeouts);
	INFO_PRINTF2(_L("Getting Timeout Values = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);
	INFO_PRINTF2(_L("Opening an UDP Socket on a Connection = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	INFO_PRINTF2(_L("Testing UDP DataPath = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	INFO_PRINTF2(_L("Short Timeout (from the Configuration file \"rconnectionudp.xml\"): %d"), timeouts.iShortTimeout);
	
	INFO_PRINTF2(_L("Waiting for %d microseconds that all the Interfaces are down"), iShortTimeout);
	User::After(iShortTimeout);
	
	numOfInterfaces = NumberOfInterfacesL(ss);
	INFO_PRINTF2(_L("Number of active Interfaces = %d (0 expected)"), numOfInterfaces);
	TESTEL(0 == numOfInterfaces, numOfInterfaces);

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest15



TE_RConnectionTest16::~TE_RConnectionTest16()
{
}

enum TVerdict TE_RConnectionTest16::doTestStepL(void)
/*
 * Ensure that Close() does not pull down the interface when there are other connections
 * associated with it.
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn1, conn2;
	TUint numOfConnections;
	TUint numOfInterfaces;

	TTimeoutValues timeouts;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	/*
	 * Establish the two connections on the same (default) interface
	 */

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);
	err = StartConnection(conn1);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);
	err = StartConnection(conn2);
	TESTEL(KErrNone == err, err);

	/*
	 * See how many interfaces we have created
	 */

	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	INFO_PRINTF2(_L("Number of active Connection = %d (1 expected)"), numOfConnections);
	TESTEL(1 == numOfConnections, numOfConnections);

	/*
	 * Close down the first connection
	 */

	CloseConnection(conn1);
	/*
	 * conn1 isn't at top of cleanup stack, hence pop off two then put conn2 back on
	 */
	CleanupStack::Pop(2);
	CleanupClosePushL(conn2);

	/*
	 * Wait for this connection to be destroyed, don't wait too long or the other one will
	 * be destroyed also! (don't wait mediumTimeOut)
	 */
//	User::After(iShortTimeout);

	/*
	 * See how many connections we have now
	 */

	err = EnumerateConnections(conn2, numOfConnections);
	TESTEL(KErrNone == err, err);
	INFO_PRINTF2(_L("Number of active Connection = %d (1 expected)"), numOfConnections);
	TESTEL(1 == numOfConnections, numOfConnections);

	// Reading Timeout values from configuration file
	err = GetTimeoutValues(conn2, timeouts);
	INFO_PRINTF2(_L("Getting Timeout Values = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);
	
	/*
	 * Close down everything else
	 */

	CloseConnection(conn2);
	CleanupStack::Pop();

	INFO_PRINTF2(_L("Short Timeout (from the Configuration file \"rconnectionudp.xml\"): %d seconds"), timeouts.iShortTimeout);
	
	INFO_PRINTF2(_L("Waiting for %d microseconds that all the Interfaces are down"), iShortTimeout);
	User::After(iShortTimeout);

	numOfInterfaces = NumberOfInterfacesL(ss);
	INFO_PRINTF2(_L("Number of active Interfaces = %d (0 expected)"), numOfInterfaces);
	TESTEL(0 == numOfInterfaces, numOfInterfaces);

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest16


TE_RConnectionTest17::~TE_RConnectionTest17()
{
}

enum TVerdict TE_RConnectionTest17::doTestStepL(void)
/*
 * Ensure that Close() does not pull down the interface when there are sockets associated with it
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn, enumerationConn;
	RSocket sock;
	TUint numOfConnections;
	TTimeoutValues timeouts;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	err = GetTimeoutValues(conn, timeouts);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	CloseConnection(conn);
	/*
	 * conn isn't at top of cleanup stack hence pop 3 then put connMonitor  sock back on
	 */
	CleanupStack::Pop(2);
	CleanupClosePushL(sock);

	// Need to wait the time for the interface to be destroyed normally when nothing associated, but don't
	// wait too long or it will actually be destroyed
	User::After(iShortTimeout);

	err = OpenConnection(enumerationConn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(enumerationConn);

	err = EnumerateConnections(enumerationConn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	CloseConnection(enumerationConn);
	CleanupStack::Pop();

	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	DestroyUdpSocket(sock);
	/*
	 * sock isn't at top of cleanup stack hence pop 2 then put connMonitor back on
	 */
	CleanupStack::Pop();

	User::After(timeouts.iShortTimeout+15000000);

	err = OpenConnection(enumerationConn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(enumerationConn);

	err = EnumerateConnections(enumerationConn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(0 == numOfConnections, numOfConnections);

	CloseConnection(enumerationConn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest17


TE_RConnectionTest18::~TE_RConnectionTest18()
{
}

enum TVerdict TE_RConnectionTest18::doTestStepL(void)
/*
 * Ensure that Stop() pulls down the interface when there are no subsessions other
 * than the connection associated with it
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RConnection countingConn;
	TUint numOfConnections = 0;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(countingConn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(countingConn);

	err = EnumerateConnections(countingConn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	err = StopConnection(conn);
	TESTEL(KErrNone == err, err);
	CloseConnection(conn);
	/*
	 * conn isn't at top of cleanup stack hence pop 2 then put countingConn back on
	 */
	CleanupStack::Pop(2);
	CleanupClosePushL(countingConn);

	User::After(iShortTimeout);

	err = EnumerateConnections(countingConn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(0 == numOfConnections, numOfConnections);

	CloseConnection(countingConn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest18


TE_RConnectionTest19::~TE_RConnectionTest19()
{
}

enum TVerdict TE_RConnectionTest19::doTestStepL(void)
/*
 * Ensure that Stop() pulls down the interface when there are sockets associated with it
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;
	TUint numOfConnections;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	err = StopConnection(conn);
	TESTEL(KErrNone == err, err);
	CloseConnection(conn);
	/*
	 * conn isn't at the top of the cleanup stack hence pop 2 then put sock back on
	 */
	CleanupStack::Pop(2);
	CleanupClosePushL(sock);

	User::After(iShortTimeout);

	numOfConnections = NumberOfInterfacesL(ss);
	TESTEL(0 == numOfConnections, numOfConnections);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest19


TE_RConnectionTest20::~TE_RConnectionTest20()
{
}

enum TVerdict TE_RConnectionTest20::doTestStepL(void)
/*
 * Stop() a connection over an interface leaving other remaining subsessions implicitly disconnected,
 * then create a new connection over that interface
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	/*
	 * ok, let's start any old connection
	 */

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDefaultIap);
	TESTEL(KErrNone == err, err);

	/*
	 * let's create and connect a tcp socket - connect is the call that
	 * would implicitly create an interface if one didn't exist (not send)
	 */

	err = OpenTcpSocketExplicit(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrNone == err, err);

	err = TestTcpDataPathL(sock);
	TESTEL(KErrNone == err, err);

	/*
	 * Stop the connection then try the socket again
	 */

	err = StopConnection(conn);
	TESTEL(KErrNone == err, err);
	CloseConnection(conn);
	// conn isn't at top of cleanup stack so pop 2 and then push sock
	CleanupStack::Pop(2);
	CleanupClosePushL(sock);

	err = TestTcpDataPathL(sock);
	TESTEL(KErrCancel == err, err);

	/*
	 * let's create another connection now
	 */

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	User::After(iShortTimeout+1000000); // wait for the interface to be destroyed

	err = StartConnectionWithOverrides(conn, iDefaultIap);
	TESTEL(KErrNone == err, err);

	/*
	 * the same interface has come back, but this socket was never attached to this
	 * instance of it, so still expect this to fail
	 */

	err = TestTcpDataPathL(sock);
	TESTEL(KErrCancel == err, err);

	/*
	 * connect it again
	 */

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTEL(KErrCancel == err, err);

	err= TestTcpDataPathL(sock);
	TESTEL(KErrCancel == err, err);

	/*
	 * let's tidy up
	 */

	CloseConnection(conn);
	CleanupStack::Pop();

	err = DestroyTcpSocket(sock);
	TESTEL(KErrCancel == err, err);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest20


TE_RConnectionTest21::~TE_RConnectionTest21()
{
}

enum TVerdict TE_RConnectionTest21::doTestStepL(void)
/*
 * Stop() a connection and ensure that other connections over interfaces are unaffected
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn1, conn2;
	RSocket sock;
	TUint numOfConnections;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = StartConnectionWithOverrides(conn1, iHungryNifIap);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	err = StartConnectionWithOverrides(conn2, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn2, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == numOfConnections, numOfConnections);

	err = StopConnection(conn1);
	TESTEL(KErrNone == err, err);
	CloseConnection(conn1);

	/*
	 * conn1 is not at the top of the cleanup stack hence pop 2 then add conn2 again
	 */
	CleanupStack::Pop(2);
	CleanupClosePushL(conn2);

	User::After(iShortTimeout);

	err = EnumerateConnections(conn2, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest21



TE_RConnectionTest22::~TE_RConnectionTest22()
{
}

enum TVerdict TE_RConnectionTest22::doTestStepL(void)
/*
 * Test sequence of progress notifications for a single explicitly created connection
 */
{
	TInt err;
	TRequestStatus status;

	RSocketServ ss;
	RConnection conn;
	TNifProgressBuf progress;
	TNifProgress currentProgress;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDefaultIap);
	TESTEL(KErrNone == err, err);

	/*
	 * Using the default interface which should be null agt and dummy nif, hence
	 * use the null agt progress states
	 */


	// Fix of test break due to additional progress notifications
	// resulting from changes to Nifman in GT0161 project.
	// Because there are additional progress notifications, we ingore the ones we don't
	// explicitly expect.
	WaitForRelevantProgress(conn, progress, ECsdFinishedConnect);

	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdFinishedConnect == progress().iStage, progress().iStage);

	// wait for ECsdConnectionOpen
	ProgressNotification(conn, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdConnectionOpen == progress().iStage, progress().iStage);

	// wait for EIfProgressAuthenticationComplete
	ProgressNotification(conn, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(EPppProgressAuthenticationComplete == progress().iStage, progress().iStage);

	// wait for EIfProgressLinkUp
	ProgressNotification(conn, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(EPppProgressLinkUp == progress().iStage, progress().iStage);

	// test the current progress state matched up ok
	err = Progress(conn, currentProgress);
	TESTEL(KErrNone == err, err);
	TESTEL(KErrNone == currentProgress.iError, currentProgress.iError);
	TESTEL(KLinkLayerOpen == currentProgress.iStage, currentProgress.iStage);

	/*
	 * Close down
	 */

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest22



TE_RConnectionTest23::~TE_RConnectionTest23()
{
}

enum TVerdict TE_RConnectionTest23::doTestStepL(void)
/*
 * Create two connections on the same interface and check that the progress notification
 * for each is the same
 */
{
	TInt err;
	TRequestStatus startStatus, status;

	RSocketServ ss;
	RConnection conn1, conn2;
	TNifProgressBuf progress;
	TCommDbConnPref prefs;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	/*
	 * Get the first connection going, monitoring the progress
	 */

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	StartConnectionWithOverridesAsynchronous(conn1, prefs, iDefaultIap, startStatus);

	// wait for KStartingSelection (1000)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KStartingSelection == progress().iStage, progress().iStage);

	// wait for KFinishedSelection (2000)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KFinishedSelection == progress().iStage, progress().iStage);

	// wait for ECsdScanningScript (2502)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdScanningScript == progress().iStage, progress().iStage);

	// wait for ECsdScannedScript (2503)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdScannedScript == progress().iStage, progress().iStage);

	// wait for ECsdGettingLoginInfo (2504)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdGettingLoginInfo == progress().iStage, progress().iStage);

	// wait for ECsdGotLoginInfo (2505)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdGotLoginInfo == progress().iStage, progress().iStage);

	// wait for ECsdStartingConnect (2506)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdStartingConnect == progress().iStage, progress().iStage);

	// wait for ECsdFinishedConnect (2507)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdFinishedConnect == progress().iStage, progress().iStage);

	// wait for ECsdConnectionOpen (3500)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdConnectionOpen == progress().iStage, progress().iStage);

	// wait for KMinNifProgress (6000)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KMinNifProgress == progress().iStage, progress().iStage);

	// wait for EIfProgressLinkUp
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KLinkLayerOpen == progress().iStage, progress().iStage);

	User::WaitForRequest(startStatus);
	TESTEL(startStatus.Int() == KErrNone, startStatus.Int());

	/*
	 * Now start the second connection and see that it is immediately up
	 */

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	StartConnectionWithOverridesAsynchronous(conn2, prefs, iDefaultIap, startStatus);

	// wait for KStartingSelection (1000)
	ProgressNotification(conn2, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KStartingSelection == progress().iStage, progress().iStage);

	// wait for KFinishedSelection (2000)
	ProgressNotification(conn2, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KFinishedSelection == progress().iStage, progress().iStage);

	// wait for EIfProgressLinkUp (7000)
	ProgressNotification(conn2, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KLinkLayerOpen == progress().iStage, progress().iStage);

	User::WaitForRequest(startStatus);
	TESTEL(startStatus.Int() == KErrNone, startStatus.Int());

	/*
	 * Tidy everything up
	 */

	err = StopConnection(conn2);
	TESTEL(KErrNone == err, err);
	CloseConnection(conn2);
	CleanupStack::Pop();


	// Fix for the test break due to additional progress notifications
	// resulting from changes to Nifman in GT0161 project.
	// Because there are additional progress notifications, we ingore the ones we don't
	// explicitly expect.
	WaitForRelevantProgress(conn1, progress, ECsdStartingHangUp);

	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdStartingHangUp == progress().iStage, progress().iStage);
	
	INFO_PRINTF1(_L("Waiting for KConnectionDown and KConnectionUninitialised. If the test times-out they didn't come (in the right seq)."));
    WaitForRelevantProgress(conn1, progress, KConnectionDown);
    WaitForRelevantProgress(conn1, progress, KConnectionUninitialised); 

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest23


TE_RConnectionTest24::~TE_RConnectionTest24()
{
}

enum TVerdict TE_RConnectionTest24::doTestStepL(void)
/*
 * Start a connection using the commdb default settings when a connection using overriden values
 * already exists.
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn1, conn2;
	RSocket sock1, sock2;
	TUint numOfConnections = 0;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);
	err = StartConnectionWithOverrides(conn1, iHungryNifIap);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);
	err = StartConnection(conn2);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketExplicitL(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = TestUdpDataPathL(sock2, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == numOfConnections, numOfConnections);

	/*
	 * Tidy up in reverse order
	 */

	DestroyUdpSocket(sock2);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();

	DestroyUdpSocket(sock1);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest24


TE_RConnectionTest25::~TE_RConnectionTest25()
{
}

void CleanupDisableTimers(TAny* aConnection)
	{
	reinterpret_cast<RConnection*>(aConnection)->SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	}
	
enum TVerdict TE_RConnectionTest25::doTestStepL(void)
/*
 * Send data over a new (connectionless) socket when an existing connection using non-default
 * parameter values already exists within the same socket server
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;
	TUint numOfConnections = 0;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iHungryNifIap);
	TESTEL(KErrNone == err, err);

	//********** turn off idle timers so we don't need to worry about them ***********
	err = conn.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	CleanupStack::PushL(TCleanupItem(CleanupDisableTimers, &conn));
	TESTEL(KErrNone == err, err);
	// With PREQ399, the implicit socket behavior has been deprecated. 
	// Sockets are not automatically associated with an already open connection, hence
	// we need to explicitly associate the socket with the connection we want to test.
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	// This should time out as it should be associated with the existing connection (hungry nif)
	// therefore nothing will be received, and the timer will pop.  Idle timers off, so
	// connection will not time out.
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrTimedOut == err, err);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	//********** turn idle timers back on **********
	err = conn.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	CleanupStack::Pop();
	TESTEL(KErrNone == err, err);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest25


TE_RConnectionTest26::~TE_RConnectionTest26()
{
}

enum TVerdict TE_RConnectionTest26::doTestStepL(void)
/*
 * Send data over a new socket in a new socket server when a connection exists in another socket server
 */
{
	TInt err;

	RSocketServ ss1, ss2;
	RConnection conn1;
	RSocket sock2;
	TUint numOfConnections;

	err = OpenSocketServer(ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss1);

	err = OpenConnection(conn1, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = StartConnectionWithOverrides(conn1, iHungryNifIap);
	TESTEL(KErrNone == err, err);

	err = OpenSocketServer(ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss2);

	err = OpenUdpSocketL(sock2, ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	// On calling SendTo() should create an new interface (the dummy which is default)
	err = TestUdpDataPathL(sock2, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == numOfConnections, numOfConnections);

	DestroyUdpSocket(sock2);
	CleanupStack::Pop();

	CloseSocketServer(ss2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss1);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest26


TE_RConnectionTest27::~TE_RConnectionTest27()
{
}

enum TVerdict TE_RConnectionTest27::doTestStepL(void)
{
	TInt err;

	RSocketServ ss1, ss2;
	RConnection conn1, conn2;
	RSocket sock2;
	TUint numOfConnections;

	err = OpenSocketServer(ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss1);

	err = OpenSocketServer(ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss2);

	err = OpenConnection(conn1, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = StartConnection(conn1);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn2, ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	err = StartConnectionWithOverrides(conn2, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn2, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	err = OpenUdpSocketExplicitL(sock2, ss2, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = TestUdpDataPathL(sock2, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	DestroyUdpSocket(sock2);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss2);
	CleanupStack::Pop();

	CloseSocketServer(ss1);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest27



TE_RConnectionTest28::~TE_RConnectionTest28()
{
}

enum TVerdict TE_RConnectionTest28::doTestStepL(void)
/*
 * Create two connections (non-default first) then use SendTo() implicitly associating to or
 * creating a new connection
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn1, conn2;
	RSocket sock;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);
	err = StartConnectionWithOverrides(conn1, iHungryNifIap);
	TESTEL(KErrNone == err, err);

	// this should implicitly create the dummy nif interface
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);
	err = StartConnection(conn2);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	// the data will go over the defaul interface (dummy nif)
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest28


TE_RConnectionTest29::~TE_RConnectionTest29()
{
}

enum TVerdict TE_RConnectionTest29::doTestStepL(void)
/*
 * Within a single socket server first implicitly create a connection by using the RSocket method
 * SendTo() then explicitly create a connection with the default commdb settings.
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;
	TUint numOfConnections = 0;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	CloseConnection(conn);
	CleanupStack::Pop();

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest29


TE_RConnectionTest30::~TE_RConnectionTest30()
{
}

enum TVerdict TE_RConnectionTest30::doTestStepL(void)
/*
 * Test interface to CommDb through RConnection
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	// read an integer value back from commdb (IAP\Id chosen)
	_LIT(KName1, "IAP\\id");
	TBufC<7> name1(KName1);
	TUint32 intval;
	err = conn.GetIntSetting(name1, intval);
	TESTEL(KErrNone == err, err);

	// read a bool value back from commdb (DialOutISP\IfPromptForAuth chosen)
	_LIT(KName2, "DialOutISP\\IfPromptForAuth");
	TBufC<27> name2(KName2);
	TBool boolval;
	err = conn.GetBoolSetting(name2, boolval);
	TESTEL(KErrNone == err, err);

	// read a descriptor back from commdb (DialOutISP\IfName chosen)
	// this should fail as this field does not exist anymore
	_LIT(KName3, "DialOutISP\\IfName");
	TBufC<18> name3(KName3);
	TBuf<128> desval;
	err = conn.GetDesSetting(name3, desval);
	TESTEL(KErrNotFound == err, err);

	// read a long descriptor back from commdb (DialOutISP\LoginScript chosen)
	_LIT(KName4, "DialOutISP\\LoginScript");
	TBufC<23> name4(KName4);
	TBuf<128> longdesval;
	err = conn.GetLongDesSetting(name4, longdesval);
	TESTEL(KErrNone == err, err);

	// read a descriptor that actually will be successfully read back from the database
	_LIT(KName5, "ModemBearer\\IfName");
	TBufC<21> name5(KName5);
	TBuf16<128> desval16;
	err = conn.GetDesSetting(name5, desval16);
	TESTEL(KErrNone == err, err);
	TBuf8<128> desval8;
	err = conn.GetDesSetting(name5, desval8);
	TESTEL(KErrNone == err, err);
	
	//-----------
	//error cases
	//-----------
	//1. Using GetIntSetting to retrive a boolean field
	_LIT(KBoolFieldForGetInt, "DialOutISP\\IfPromptForAuth");
	TBufC<27> boolFieldForGetInt(KBoolFieldForGetInt);
	TUint32 boolValueForGetInt;
	err = conn.GetIntSetting(boolFieldForGetInt, boolValueForGetInt);
	TESTEL(KErrBadName == err, err);
	
	//2. Using GetIntSetting to retrive a text field
	_LIT(KTextFieldForGetInt, "DialOutISP\\LoginScript");
	TBufC<23> textFieldForGetInt(KTextFieldForGetInt);
	TUint32 desValueForGetInt;
	err = conn.GetIntSetting(textFieldForGetInt, desValueForGetInt);
	TESTEL(KErrBadName == err, err);
	
	//3. GetBoolSetting to retrive an int field
	_LIT(KIntFieldForGetBool, "DialOutISP\\BearerSpeed");
	TBufC<27> intFieldForGetBool(KIntFieldForGetBool);
	TBool boolValueForGetBool;
	err = conn.GetBoolSetting(intFieldForGetBool, boolValueForGetBool);
	TESTEL(KErrBadName == err, err);
	
	//4. GetBoolSetting to retrieve a text field
	_LIT(KTextFieldForGetBool, "DialOutISP\\LoginScript");
	TBufC<27> textFieldForGetBool(KTextFieldForGetBool);
	TBool desValueForGetBool;
	err = conn.GetBoolSetting(textFieldForGetBool, desValueForGetBool);
	TESTEL(KErrBadName == err, err);
	
	//5. GetDesSetting to retrive an int field
	_LIT(KIntFieldForGetDes, "DialOutISP\\BearerSpeed");
	TBufC<24> intFieldForGetDes(KIntFieldForGetDes);
	TBuf16<128> intValueForGetDes;
	err = conn.GetDesSetting(intFieldForGetDes, intValueForGetDes);
	TESTEL(KErrBadName == err, err);
	
	//6. GetDesSetting to retrieve a bool field
	_LIT(KBoolFieldForGetDes, "DialOutISP\\IfPromptForAuth");
	TBufC<27> boolFieldForGetDes(KBoolFieldForGetDes);
	TBuf16<128> boolValueForGetDes;
	err = conn.GetDesSetting(boolFieldForGetDes, boolValueForGetDes);
	TESTEL(KErrBadName == err, err);
	
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest30


TE_RConnectionTest31::~TE_RConnectionTest31()
{
}

enum TVerdict TE_RConnectionTest31::doTestStepL(void)
/*
 * Test reporting of the interface coming down
 */
{
	TInt err;
	TRequestStatus status;

	RSocketServ ss;
	RConnection conn;
	TNifProgressBuf progress;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	err = RequestInterfaceDownL(conn, ss);
	TESTEL(KErrNone == err, err);

	ProgressNotification(conn, status, progress, KLinkLayerClosed);
	User::WaitForRequest(status);
	TESTEL(status.Int() == KErrNone, status.Int());
	TESTEL(progress().iStage == KLinkLayerClosed, progress().iStage);

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest31


TE_RConnectionTest32::~TE_RConnectionTest32()
{
}

enum TVerdict TE_RConnectionTest32::doTestStepL(void)
/*
 * Test cancelling of progress notification
 */
{
	TInt err;
	TRequestStatus status;

	RSocketServ ss;
	RConnection conn;
	TNifProgressBuf progress;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	// use the progress notification to make sure it works
	ProgressNotification(conn, status, progress, KLinkLayerOpen);
	User::WaitForRequest(status);

	// this is the one we are going to cancel
	ProgressNotification(conn, status, progress, KLinkLayerClosed);

	CancelProgressNotification(conn);

	User::WaitForRequest(status);
	TESTEL(KErrCancel == status.Int(), status.Int());

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest32


TE_RConnectionTest33::~TE_RConnectionTest33()
{
}

enum TVerdict TE_RConnectionTest33::doTestStepL(void)
/*
 * Duplicate an already existing connection within a single socket server.
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn1, conn2;
	RSocket sock1, sock2;
	TUint numOfConnections;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	/*
	 * Open the first connection and check it works
	 */

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = StartConnection(conn1);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketExplicitL(sock1, ss, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = TestUdpDataPathL(sock1, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	/*
	 * Create the second connection, passing it the name of the first
	 */

	TName name;

	err = conn1.Name(name);
	TESTEL(KErrNone == err, err);

	// With Platform Security, we need to explicitly enable clone opening

	TSecurityPolicyBuf passPolicy(TSecurityPolicy::EAlwaysPass);
	err = conn1.Control(KCOLConnection, KCoEnableCloneOpen, passPolicy);
	INFO_PRINTF2(_L("Enabling clone open returned %d"), err);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn2, ss, name);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	err = OpenUdpSocketExplicitL(sock2, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = TestUdpDataPathL(sock2, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	/*
	 * Tidy up
	 */

	DestroyUdpSocket(sock2);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();

	DestroyUdpSocket(sock1);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest33


TE_RConnectionTest34::~TE_RConnectionTest34()
{
}

enum TVerdict TE_RConnectionTest34::doTestStepL(void)
/*
 * Duplicate an already existing connection within a different socket server.
 */
{
	TInt err;

	RSocketServ ss1, ss2;
	RConnection conn1, conn2;
	RSocket sock1, sock2;
	TUint numOfConnections;

	err = OpenSocketServer(ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss1);

	err = OpenSocketServer(ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss2);

	/*
	 * Open the first connection and check it works
	 */

	err = OpenConnection(conn1, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = StartConnection(conn1);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketExplicitL(sock1, ss1, conn1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = TestUdpDataPathL(sock1, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	/*
	 * Create the second connection, passing it the name of the first
	 */

	TName name;

	err = conn1.Name(name);
	TESTEL(KErrNone == err, err);

	// With Platform Security, we need to explicitly enable clone opening

	TSecurityPolicyBuf passPolicy(TSecurityPolicy::EAlwaysPass);
	err = conn1.Control(KCOLConnection, KCoEnableCloneOpen, passPolicy);
	INFO_PRINTF2(_L("Enabling clone open returned %d"), err);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn2, ss2, name);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	err = OpenUdpSocketExplicitL(sock2, ss2, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	err = TestUdpDataPathL(sock2, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	/*
	 * Tidy up
	 */

	DestroyUdpSocket(sock2);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();

	DestroyUdpSocket(sock1);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss2);
	CleanupStack::Pop();

	CloseSocketServer(ss1);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest34

/*
 * Test 37
 * TestConnectionGoneStateL()
 * Create a connection and a socket, destroy the subsessions, wait a bit, then use the socket again
 * before the interface has been destroyed by nifman (esock will have no record of the IF but
 * nifman will still be aware of it).
 */


TE_RConnectionTest37::~TE_RConnectionTest37()
{
}

enum TVerdict TE_RConnectionTest37::doTestStepL(void)
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	// Use the socket, just for good measure
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	/*
	 * everything is set up, now do the test
	 * must close both the socket and the connection
	 */

	TInt interfaceIndex;
	TPckg<TInt> indexBuf(interfaceIndex);
	sock.GetOpt(KSoInterfaceIndex, KSolInetIp, indexBuf);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	User::After(100000 /*iShortTimeout/3*/);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	sock.SetOpt(KSoInterfaceIndex, KSolInetIp, indexBuf);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	/*
	 * Tidy up
	 */

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest37

/*
 * Test 38
 * TestDefaultConnectionL()
 * Create an implicit connection in the first socket server, an explicit connection in the
 * second socket server and then an implicit connection in the third socket server should
 * reuse the interface from the first socket server.
 */


TE_RConnectionTest38::~TE_RConnectionTest38()
{
}

enum TVerdict TE_RConnectionTest38::doTestStepL(void)
{
	TInt err;

	RSocketServ ss1, ss2, ss3;
	RConnection conn2;
	RSocket sock1, sock3;
	TUint numOfConnections;

	/*
	 * Open all three socket servers
	 */

	err = OpenSocketServer(ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss1);

	err = OpenSocketServer(ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss2);

	err = OpenSocketServer(ss3);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss3);

	/*
	 * Implicit connection in ss1 - let's use a UDP socket
	 */

	err = OpenUdpSocketL(sock1, ss1);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	err = TestUdpDataPathL(sock1, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	/*
	 * Explicitly create connection in ss2
	 */

	err = OpenConnection(conn2, ss2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	err = StartConnectionWithOverrides(conn2, iHungryNifIap);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn2, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == numOfConnections, numOfConnections);

	/*
	 * Implicitly create connection in ss3, UDP socket again
	 */

	err = OpenUdpSocketL(sock3, ss3);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock3);

	err = TestUdpDataPathL(sock3, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	/*
	 * Check that another interface was not created
	 */

	err = EnumerateConnections(conn2, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == numOfConnections, numOfConnections);

	/*
	 * Tidy up
	 */

	DestroyUdpSocket(sock3);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();

	DestroyUdpSocket(sock1);
	CleanupStack::Pop();

	CloseSocketServer(ss3);
	CleanupStack::Pop();

	CloseSocketServer(ss2);
	CleanupStack::Pop();

	CloseSocketServer(ss1);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest38

/*
 * Test 39
 * TestControlFunctionsL()
 * Use SetOpt() and Control() testing error codes, including testing bad parameters.
 */



TE_RConnectionTest39::~TE_RConnectionTest39()
{
}

enum TVerdict TE_RConnectionTest39::doTestStepL(void)
{
	TInt err;
	TInt i;

	RSocketServ ss;
	RConnection conn;

	TInt value;
	TBool disable = ETrue;
	TPckg<TBool> disablePkg(disable);
	TInt64 dummyLarge;
	TPckg<TInt64> dummyLargePkg(dummyLarge);
	TConnInterfaceName name;
	TPckg<TConnInterfaceName> namePkg(name);
	TName dummyName;
	TPckg<TName> dummyNamePkg(dummyName);

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDummyNifTwoBinderIap);
	TESTEL(KErrNone == err, err);

	/*
	 * Let's do some SetOpt() calls :-)
	 */

	// SetOpt() with invalid level
	err = conn.SetOpt(KCOLProvider + 100, KConnDisableTimers, 1);
	TESTEL(KErrNotSupported == err, err); // should this really be KErrArgument?

	// SetOpt() with invalid option
	err = conn.SetOpt(KCOLProvider, KConnDisableTimers + 100, 1);
	TESTEL(KErrNotSupported == err, err); // should this really be KErrArgument?

	err = conn.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	err = conn.SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
	TESTEL(KErrNone == err, err);

	/*
	 * Now for some GetOpt() calls :-D
	 */

	err = conn.GetOpt(KCOLProvider, KConnDisableTimers, value);
	TESTEL(KErrNotSupported == err, err);

	/*
	 * Now for some Control() calls 8-D
	 */

	// Control() with invalid level
	err = conn.Control(KCOLProvider + 100, KConnDisableTimers, disablePkg);
	TESTEL(KErrNotSupported == err, err); // should this be KErrArgument?

	// Control() with invalid option
	err = conn.Control(KCOLProvider, KConnDisableTimers + 100, disablePkg);
	TESTEL(KErrNotSupported == err, err); // should this be KErrArgument?

	// Control() with option greater than 4
	err = conn.Control(KCOLProvider, KConnDisableTimers, dummyLargePkg);
	TESTEL(KErrArgument == err, err);

	// Test that the names can be read back - one per binder
	for (i=0; i<4; i++)
	{
		name.iIndex = i;
		err = conn.Control(KCOLProvider, KConnGetInterfaceName, namePkg);
		if (err == KErrNone)
		{
			TESTEL((i==1) || (i==2), i);
			TPtrC foo(name.iName.Ptr(), 8);
			TInt res = foo.CompareF(_L("dummynif"));
			TESTEL(0 == res, res); // zero means they are the same
		}
		else
			TESTEL((i==0)||(i==3), i);
	}

	// GetInterfaceName with bad option
	err = conn.Control(KCOLProvider, KConnGetInterfaceName, dummyNamePkg);
	TESTEL(KErrArgument == err, err);

	/*
	 * Tidy up
	 */

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest39

/*
 * Test 40
 * TestLocalLoopbackL()
 * Use the local loopback with UDP sockets (the TCP/IP stack loops back any packets send
 * to 127.x.x.x without sending them to any interface)
 */


TE_RConnectionTest40::~TE_RConnectionTest40()
{
}

enum TVerdict TE_RConnectionTest40::doTestStepL(void)
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;
	TInetAddr dest;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start up the hungry nif so that any packets reaching the interface will be lost...
	// should be looped back by the stack
	err = StartConnectionWithOverrides(conn, iHungryNifIap);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	dest.Input(_L("127.0.0.1"));
	dest.SetPort(iEchoPortNum);

	err = TestUdpDataPathL(sock, dest);
	TESTEL(KErrNone == err, err);

	/*
	 * Tidy up
	 */

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest40

/*
 * Test 41
 * TestAttachL()
 * Attach a new RConnection to an existing connection. Also try to attach to a non-existant interface
 */



TE_RConnectionTest41::~TE_RConnectionTest41()
{
}

enum TVerdict TE_RConnectionTest41::doTestStepL(void)
{
	TInt err;
	TRequestStatus status;

	RSocketServ ss;
	RConnection conn1, conn2;
	TPckgBuf<TConnectionInfo> info;
	TUint numOfConnections = 0;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	StartConnectionAsynchronous(conn1, status);

	/*
	 * Attempt to attach before the start has completed - should fail
	 */


	err = AttachNormal(conn2, TPtrC8());
	TESTEL(KErrArgument == err, err);

	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());

	/*
	 * Attempt to attach to non-existant connection
	 */

	info().iIapId = 9999;
	info().iNetId = 9999;

	err = AttachNormal(conn2, info); // should fail
	TESTEL(KErrCouldNotConnect == err, err);

	/*
	 * Check how many interfaces we have
	 */

	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);

	/*
	 * Now for an attach that should succeed
	 */

	err = conn1.GetConnectionInfo(1, info);
	TESTEL(KErrNone == err, err);

	err = AttachNormal(conn2, info);
	TESTEL(KErrNone == err, err);

	TNifProgress progress;
	err = Progress(conn2, progress);
	TESTEL(KErrNone == err, err);
	TESTEL(progress.iStage == KLinkLayerOpen, progress.iStage);
	TESTEL(progress.iError == 0, progress.iError);

	/*
	 * Tidy up
	 */

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest41

/*
 * Test 42
 * TestStartFailL()
 * Fail to start NT RAS - it will return some sort of error, perhaps due to a bad passwd :-) then
 * use a different IAP which should work. This is also testing selected progress with error.
 */


TE_RConnectionTest42::~TE_RConnectionTest42()
{
}

enum TVerdict TE_RConnectionTest42::doTestStepL(void)
{
	TInt err;
	TRequestStatus status, startStatus;

	RSocketServ ss;
	RConnection conn;
	TNifProgressBuf progress;
	TCommDbConnPref prefs;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	/*
	 * Try and start the connection on a bad NT RAS IAP
	 */


	StartConnectionWithOverridesAsynchronous(conn, prefs, iBadNtRasIap, startStatus);
	ProgressNotification(conn, status, progress, KLinkLayerOpen);

	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());

	User::WaitForRequest(startStatus);
	TESTEL(KErrIfAuthenticationFailure == startStatus.Int(), startStatus.Int()); // should be an error here

	// this is also testing selected progress with error
	TESTEL(progress().iError == KErrIfAuthenticationFailure, progress().iError);
	TESTEL(progress().iStage != KLinkLayerOpen, progress().iStage);

	/*
	 * Start the connection on an NT RAS IAP that will work, once the previous connection has
         * release the serial device
	 */

	err = WaitForAllInterfacesToCloseL(ss);

	TESTEL(KErrNone == err, err);

	err = StartConnectionWithOverrides(conn, iDefaultIap);
	TESTEL(KErrNone == err, err);

	/*
	 * Tidy up
	 */

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest42

/*
 * Test 43
 * TestAttachMonitorL()
 * Attach an RConnection to another as a monitor
 */



TE_RConnectionTest43::~TE_RConnectionTest43()
{
}

enum TVerdict TE_RConnectionTest43::doTestStepL(void)
{
	TInt err;

	RSocketServ ss;
	RConnection conn1, conn2;
	TPckgBuf<TConnectionInfo> info1, info2;
	TUint numOfConnections;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = StartConnection(conn1);
	TESTEL(KErrNone == err, err);

	/*
	 * So let's attach as monitor
	 */

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	err = GetInfo(conn1, info1);
	TESTEL(KErrNone == err, err);

	err = AttachMonitor(conn2, info1);
	TESTEL(KErrNone == err, err);

	err = GetInfo(conn2, info2);
	TESTEL(KErrNone == err, err);

	/*
	 * Check they are the same interface
	 */

	TESTEL(info1().iIapId == info2().iIapId, info2().iIapId);
	TESTEL(info1().iNetId == info2().iNetId, info2().iNetId);

	/*
	 * Close down
	 */

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	err = WaitForAllInterfacesToCloseL(ss);
	TESTEL(KErrNone == err, err);

	/*
	 * Now do the whole thing again, but close them in the opposite order, having fun and
	 * japes with the cleanup stack along the way
	 */

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = StartConnection(conn1);
	TESTEL(KErrNone == err, err);

	/*
	 * Right then, let's attach as monitor again
	 */

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// need to call Enumerate() before GetConnectionInfo() to set up array used there
	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);

	err = conn1.GetConnectionInfo(1, info1);
	TESTEL(KErrNone == err, err);

	err = AttachMonitor(conn2, info1);
	TESTEL(KErrNone == err, err);

	// need to call Enumerate() before GetConnectionInfo() to set up array used there
	err = EnumerateConnections(conn2, numOfConnections);
	TESTEL(KErrNone == err, err);

	err = conn2.GetConnectionInfo(1, info2);
	TESTEL(KErrNone == err, err);

	/*
	 * Check that they are the same interface
	 */

	TESTEL(info1().iIapId == info2().iIapId, info2().iIapId);
	TESTEL(info1().iNetId == info2().iNetId, info2().iIapId);

	/*
	 * Close down, here we go....
	 */

	CloseConnection(conn1);
	// conn1 not at top of cleanup stack, hence pop 2 and push conn2 again
	CleanupStack::Pop(2);
	CleanupClosePushL(conn2);

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest43

TE_RConnectionTest44::~TE_RConnectionTest44()
{
}

enum TVerdict TE_RConnectionTest44::doTestStepL(void)
/*
 * Test 44
 * TestIdleTimeoutsL()
 * Set up varioous situations and wait for the timeouts, checking timestamps
 */
{
	TInt err;
	TRequestStatus status;

	RSocketServ ss;
	RConnection conn1, conn2, connMonitor;
	RSocket sock;

	TPckgBuf<TConnectionInfo> info;
	TUint numOfConnections;
	TNifProgressBuf progress;

	TTimeoutValues timeouts;
	TTimeIntervalSeconds timeElapsed;
	TInt delta;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	/*
	 * Test the short timeout
	 */
	INFO_PRINTF1(_L("--- Start testing the Short Timeout ---"));
	
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = OpenConnection(connMonitor, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connMonitor);

	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);

	err = GetTimeoutValues(conn1, timeouts);
	INFO_PRINTF2(_L("Getting Timeout Values = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);

	// need to call Enumerate() before GetConnectionInfo() to set up array used there
	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(numOfConnections == 1, numOfConnections);

	err = conn1.GetConnectionInfo(1, info);
	INFO_PRINTF2(_L("Getting Connection Info = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);

	// perform dummy attach and start operations to test accounting of internal counters
	PerformDummyStartAttachOperationsL(ss, info);

	// set up the monitor
	err = AttachMonitor(connMonitor, info);
	INFO_PRINTF2(_L("Attaching Monitor = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);

	// we want to use it to watch for the status of the interface changing
	ProgressNotification(connMonitor, status, progress, KLinkLayerClosed);

	// close the original connection
	CloseConnection(conn1);
	// conn isn't at the top of the cleanup stack, hence pop 2 then push connMonitor
	CleanupStack::Pop(2);
	CleanupClosePushL(connMonitor);
	
	err = TimeUntilRequestComplete(status, timeElapsed);
	INFO_PRINTF2(_L("Requesting the Time Until Request Complete = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);

	// check that it took the right length of time	
	delta = timeElapsed.Int() - timeouts.iShortTimeout;
	INFO_PRINTF2(_L("Evaluating Elapsed Time = %d (-1 <= X <= 1 expected)"), delta);
	TESTEL(delta >= -1 && delta <= 1, delta);

	CloseConnection(connMonitor);
	CleanupStack::Pop();

	// Wait that all the Connections go down...
	User::After(iShortTimeout);
	
	/*
	 * Now test the medium timeout
	 */
	INFO_PRINTF1(_L("--- Start testing the Medium Timeout ---"));
	
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = OpenConnection(connMonitor, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connMonitor);

	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);
	
	// need to call Enumerate() before GetConnectionInfo() to set up array used there
	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	INFO_PRINTF2(_L("Number of active Connection = %d (1 expected)"), numOfConnections);
	TESTEL(numOfConnections == 1, numOfConnections);

	err = conn1.GetConnectionInfo(1, info);
	INFO_PRINTF2(_L("Getting Connection Info = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);

	// perform dummy attach and start operations to test accounting of internal counters
	PerformDummyStartAttachOperationsL(ss, info);

	err = AttachMonitor(connMonitor, info);
	INFO_PRINTF2(_L("Attaching Monitor = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);

	// we want to use the monitor to watch for the status of the interface changing
	ProgressNotification(connMonitor, status, progress, KLinkLayerClosed);

	err = TimeUntilRequestComplete(status, timeElapsed);
	INFO_PRINTF2(_L("Requesting the Time Until Request Complete = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);

	delta = timeElapsed.Int() - timeouts.iMediumTimeout;
	INFO_PRINTF2(_L("Evaluating Elapsed Time = %d (-1 <= X <= 1 expected)"), delta);
	TESTEL(delta >= -1 && delta <= 1, delta);

	CloseConnection(connMonitor);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();
	
	// Wait that all the Connections go down...
	User::After(iShortTimeout);

	/*
	 * And finally test the long timeout
	 */
	INFO_PRINTF1(_L("--- Start testing the Long Timeout ---"));
	
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = OpenConnection(connMonitor, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connMonitor);

	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);

	// need to call Enumerate() before GetConnectionInfo() to set up array used there
	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	INFO_PRINTF2(_L("Number of active Connection = %d (1 expected)"), numOfConnections);
	TESTEL(numOfConnections == 1, numOfConnections);

	err = conn1.GetConnectionInfo(1, info);
	INFO_PRINTF2(_L("Getting Connection Info = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);

	// perform dummy attach and start operations to test accounting of internal counters
	PerformDummyStartAttachOperationsL(ss, info);

	err = AttachMonitor(connMonitor, info);
	INFO_PRINTF2(_L("Attaching Monitor = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketExplicitL(sock, ss, conn1);
	INFO_PRINTF2(_L("Opening UDP Socket Explicit = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	ProgressNotification(connMonitor, status, progress, KLinkLayerClosed);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	INFO_PRINTF2(_L("Testing UDP Data Path = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);

	// close the original connection
	CloseConnection(conn1);
	// conn1 isn't at the top of the cleanup stack, hence pop 3 then push connMonitor and sock
	CleanupStack::Pop(3);
	CleanupClosePushL(connMonitor);
	CleanupClosePushL(sock);

	err = TimeUntilRequestComplete(status, timeElapsed);
	INFO_PRINTF2(_L("Requesting the Time Until Request Complete = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);

	delta = timeElapsed.Int() - timeouts.iLongTimeout;
	INFO_PRINTF2(_L("Evaluating Elapsed Time = %d (-1 <= X <= 1 expected)"), delta);
	TESTEL(delta >= -1 && delta <= 1, delta);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseConnection(connMonitor);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest44

/*
 * Test 45
 * TestOOMStartLoopL()
 * Try to perform Start() repeatedly while increasing failnext on esock
 */


TE_RConnectionTest45::~TE_RConnectionTest45()
{
}

enum TVerdict TE_RConnectionTest45::doTestStepL(void)
{
	TInt err = KErrGeneral;
	TInt i = 0;
	TUint numOfConnections = 0;
	RSocketServ ss;
	RConnection conn, conn2, connMonitor;
	RSocket sock;
	TPckgBuf<TConnectionInfo> info;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

// NOTE: The following OOM tests do leave a number of Client Handles open which can
// cause problems later in the test run. ESock cleanup could either be done between
// each test itteration, or the tests may need to be redesigned.

// Test1: OOM conditions when opening connection.

	INFO_PRINTF1 (_L ("Test OOM opening connection"));
/* With ESockV3 this doesnt work anymore
	ss.__DbgMarkHeap();
*/
	do
		{
		ss.__DbgFailNext(i);
		err = OpenConnection(conn, ss);
		INFO_PRINTF3(_L("Returned %d for __DbgFailNext(%d)"),err,i);
		TESTEL(KErrNoMemory == err || KErrNone == err, err);
/* With ESockV3 this doesnt work anymore
		if (err != KErrNone)
			{
			ss.__DbgCheckHeap(0);
			} */
		i++;
		if (i == 100) //This is to ensure that the test finishes even in the case of serious failure.
			TESTEL(EFail, err);
		}
	while (err != KErrNone);

	ss.__DbgFailNext(-1);

	CloseConnection(conn);

	err = WaitForAllInterfacesToCloseL(ss);
	TESTEL(KErrNone == err, err);

	//ss.__DbgMarkEnd(0);

// Test2: OOM conditions when starting connection
// It checks Socket Server and other components running in the same thread against panic.
// Memory leak test cannot be performed as nifman and netcon do not release their resources immediately.
	INFO_PRINTF1 (_L ("Test OOM2: Starting connection"));
	i = 0;

/* With ESockV3 this doesnt work anymore
	ss.__DbgMarkHeap();
*/
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	do
		{
		ss.__DbgFailNext(i);
		err = StartConnectionWithOverrides(conn, iDummyNifIap);
		INFO_PRINTF3(_L("Returned %d for __DbgFailNext(%d)"),err,i);
		TESTEL(KErrNoMemory == err || KErrNone == err, err);
		i++;
		if (i == 100) //This is to ensure that the test finishes even in the case of serious failure.
			TESTEL(EFail, err);
		}
	while (err != KErrNone);

	// Cancel outstanding "Fail next" debug command.
	ss.__DbgFailNext(-1);

	err = StopConnection(conn);
	TESTEL(KErrNone == err, err);
	CloseConnection(conn);

	CleanupStack::Pop(&conn);

	err = WaitForAllInterfacesToCloseL(ss);
	TESTEL(KErrNone == err, err);

//	ss.__DbgMarkEnd(0); // DEFECT

// Test3: OOM conditions with Attach() normal
// It checks Socket Server and other components running in the same thread against panic.
// Memory leak test cannot be performed as nifman and netcon do not release their resources immediately.

	INFO_PRINTF1 (_L ("Test OOM normal Attach"));
	i = 0;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	// need to call Enumerate() before GetConnectionInfo() to set up array used there
	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);

	err = conn.GetConnectionInfo(1, info);
	TESTEL(KErrNone == err, err);
	TESTEL(numOfConnections == 1, numOfConnections);

	do
		{
		ss.__DbgFailNext(i);
		err = AttachNormal(conn2, info);
		INFO_PRINTF3(_L("Returned %d for __DbgFailNext(%d)"),err,i);
		TESTEL(KErrNoMemory == err || KErrNone == err, err);
		i++;
		if (i == 100) //This is to ensure that the test finishes even in the case of serious failure.
			TESTEL(EFail, err);
		}
	while (err != KErrNone);

	// Cancel outstanding "Fail next" debug command.
	ss.__DbgFailNext(-1);

	CloseConnection(conn2);
	CleanupStack::Pop(&conn2);
	CloseConnection(conn);

	CleanupStack::Pop(&conn);

	err = WaitForAllInterfacesToCloseL(ss);
	TESTEL(KErrNone == err, err);

// Test4: OOM conditions with Attach() monitor
// It checks Socket Server and other components running in the same thread against panic.
// Memory leak test cannot be performed as nifman and netcon do not release their resources immediately.

	INFO_PRINTF1 (_L ("Test OOM monitor Attach"));
	i = 0;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	err = OpenConnection(connMonitor, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connMonitor);

	// need to call Enumerate() before GetConnectionInfo() to set up array used there
	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);

	err = conn.GetConnectionInfo(1, info);
	TESTEL(KErrNone == err, err);
	TESTEL(numOfConnections == 1, numOfConnections);

	do
		{
		ss.__DbgFailNext(i);
		err = AttachMonitor(connMonitor, info);
		INFO_PRINTF3(_L("Returned %d for __DbgFailNext(%d)"),err,i);
		TESTEL(KErrNoMemory == err || KErrNone == err, err);
		i++;
		if (i == 100) //This is to ensure that the test finishes even in the case of serious failure.
			TESTEL(EFail, err);
		}
	while (err != KErrNone);

	// Cancel outstanding "Fail next" debug command.
	ss.__DbgFailNext(-1);

	CloseConnection(connMonitor);
	CleanupStack::Pop(&connMonitor);

	CloseConnection(conn);

	CleanupStack::Pop(&conn);

	err = WaitForAllInterfacesToCloseL(ss);
	TESTEL(KErrNone == err, err);

// Test5: OOM conditions when sending over a UDP socket
// It checks Socket Server and other components running in the same thread against panic.
// Memory leak test cannot be performed as nifman and netcon do not release their resources immediately.

	INFO_PRINTF1 (_L ("Test OOM udp send over explicit connection"));
	i = 0;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	do
		{
		ss.__DbgFailNext(i);
		err = TestUdpDataPathL(sock, iDummyNifSendAddr);
		INFO_PRINTF3(_L("Returned %d for __DbgFailNext(%d)"),err,i);
		TESTEL(KErrNoMemory == err || KErrNone == err, err);
		i++;
		if (i == 100) //This is to ensure that the test finishes even in the case of serious failure.
			TESTEL(EFail, err);
		}
	while (err != KErrNone);

	ss.__DbgFailNext(-1);

	DestroyUdpSocket(sock);
	CleanupStack::Pop(&sock);

	CloseConnection(conn);

	// This is to remove pending DbgNextFail request. This will fail!
	err = OpenConnection(conn, ss);
	CloseConnection(conn);

	CleanupStack::Pop(&conn);

	err = WaitForAllInterfacesToCloseL(ss);
	TESTEL(KErrNone == err, err);

// Test6: OOM conditions when sending over a UDP socket (implicitly creating connection)
// It checks Socket Server and other components running in the same thread against panic.
// Memory leak test cannot be performed as nifman and netcon do not release their resources immediately.

	 INFO_PRINTF1 (_L ("Test OOM udp send over implicit connection"));
	i = 0;

	do
		{
		err = OpenUdpSocketL(sock, ss);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(sock);

		ss.__DbgFailNext(i);
		err = TestUdpDataPathL(sock, iDummyNifSendAddr);
		ss.__DbgFailNext(-1);
		INFO_PRINTF3(_L("Returned %d for __DbgFailNext(%d)"),err,i);

		DestroyUdpSocket(sock);
		CleanupStack::Pop(&sock);

		i++;
		if (i == 100) //This is to ensure that the test finishes even in the case of serious failure.
			TESTEL(EFail, err);
		}
	while (err != KErrNone);

	ss.__DbgFailNext(-1);

	err = WaitForAllInterfacesToCloseL(ss);
	TESTEL(KErrNone == err, err);

	/*
	 * All the excitement is over, so close the socket server now
	 */

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();
} // TE_RConnectionTest45

/*
 * Test 46
 * TestMediumTimeoutExtensionL()
 * Create a connection, then wait a bit before starting another connection thus
 * extending the medium timer
 */


TE_RConnectionTest46::~TE_RConnectionTest46()
{
}

enum TVerdict TE_RConnectionTest46::doTestStepL(void)
{
	TInt err;
	TRequestStatus status;

	RSocketServ ss;
	RConnection conn1, conn2;
	TTimeIntervalSeconds timeElapsed;
	TInt delta;
	TTimeoutValues timeouts;
	TNifProgressBuf progress;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);

	err = GetTimeoutValues(conn1, timeouts);
	TESTEL(KErrNone == err, err);

	ProgressNotification(conn1, status, progress, KLinkLayerClosed);

	User::After((timeouts.iMediumTimeout * 1000000) /2); // wait part of the time out

	/*
	 * Hopefully we will extend the timeout by opening another connection
	 */

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	err = StartConnectionWithOverrides(conn2, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);

	err = TimeUntilRequestComplete(status, timeElapsed);
	TESTEL(KErrNone == err, err);

	delta = timeElapsed.Int() - timeouts.iMediumTimeout;
	TESTEL(delta >= -1 && delta <= 1, delta);

	/*
	 * Tidy up
	 */

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest46

/*
 * Test 47
 * TestMediumTimeoutExtensionL()
 * Create a connection, then wait a bit before attaching (normally) another connection thus
 * extending the medium timer
 */


TE_RConnectionTest47::~TE_RConnectionTest47()
{
}

enum TVerdict TE_RConnectionTest47::doTestStepL(void)
{
	TInt err;
	TRequestStatus status;

	RSocketServ ss;
	RConnection conn1, conn2;
	TPckgBuf<TConnectionInfo> info;
	TTimeIntervalSeconds timeElapsed;
	TInt delta;
	TTimeoutValues timeouts;
	TUint numOfConnections;
	TNifProgressBuf progress;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);

	err = GetTimeoutValues(conn1, timeouts);
	TESTEL(KErrNone == err, err);

	// need to call Enumerate() before GetConnectionInfo() to set up array used there
	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(numOfConnections == 1, numOfConnections);

	err = conn1.GetConnectionInfo(1, info);
	TESTEL(KErrNone == err, err);

	ProgressNotification(conn1, status, progress, KLinkLayerClosed);

	User::After((timeouts.iMediumTimeout * 1000000) /2); // wait part of the time out

	/*
	 * Hopefully we will extend the timeout by opening another connection
	 */

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	err = AttachNormal(conn2, info);
	TESTEL(KErrNone == err, err);

	err = TimeUntilRequestComplete(status, timeElapsed);
	TESTEL(KErrNone == err, err);

	delta = timeElapsed.Int() - timeouts.iMediumTimeout;
	TESTEL(delta >= -1 && delta <= 1, delta);

	/*
	 * Tidy up
	 */

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest47

/*
 * Test 48
 * TestMediumTimeoutExtensionL()
 * Create a connection, then wait a bit before monitor-attaching another connection and checking
 * that this does not extend the medium timer.
 */


TE_RConnectionTest48::~TE_RConnectionTest48()
{
}

enum TVerdict TE_RConnectionTest48::doTestStepL(void)
{
	TInt err;
	TRequestStatus status;

	RSocketServ ss;
	RConnection conn1, conn2;
	TPckgBuf<TConnectionInfo> info;
	TTimeIntervalSeconds timeElapsed;
	TInt delta;
	TTimeoutValues timeouts;
	TUint numOfConnections;
	TNifProgressBuf progress;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);

	err = GetTimeoutValues(conn1, timeouts);
	TESTEL(KErrNone == err, err);

	// need to call Enumerate() before GetConnectionInfo() to set up array used there
	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(numOfConnections == 1, numOfConnections);

	err = conn1.GetConnectionInfo(1, info);
	TESTEL(KErrNone == err, err);

	ProgressNotification(conn1, status, progress, KLinkLayerClosed);

	User::After((timeouts.iMediumTimeout * 1000000) /2); // wait part of the time out

	/*
	 * Hopefully the timer will not be extended by attaching a connection as a monitor
	 */

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);

	err = AttachMonitor(conn2, info);
	TESTEL(KErrNone == err, err);

	err = TimeUntilRequestComplete(status, timeElapsed);
	TESTEL(KErrNone == err, err);

	delta = timeElapsed.Int() - (timeouts.iMediumTimeout)/2;
	TESTEL(delta >= -1 && delta <= 1, delta);

	/*
	 * Tidy up
	 */

	CloseConnection(conn2);
	CleanupStack::Pop();

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest48

/*
 * Test 49
 * TestLongTimeoutExtensionL()
 * Open a connection and a socket, wait a bit then send some data over the socket. This should
 * extend the long timer, check it does so.
 */


TE_RConnectionTest49::~TE_RConnectionTest49()
{
}

enum TVerdict TE_RConnectionTest49::doTestStepL(void)
{
	TInt err;
	TRequestStatus status;

	RSocketServ ss;
	RConnection conn;
	RSocket sock1;
	TTimeIntervalSeconds timeElapsed;
	TTimeoutValues timeouts;
	TNifProgressBuf progress;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);

	err = GetTimeoutValues(conn, timeouts);
	TESTEL(KErrNone == err, err);

	err = OpenUdpSocketExplicitL(sock1, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);

	// Generate some activity 
    err = TestUdpDataPathL(sock1, iDummyNifSendAddr); 
    TESTEL(KErrNone == err, err);

	ProgressNotification(conn, status, progress, KLinkLayerClosed);

	User::After((timeouts.iLongTimeout * 1000000) /2); // wait part of the time out

	/*
	 * Hopefully we will extend the timeout by sending data
	 */

	err = TestUdpDataPathL(sock1, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	err = TimeUntilRequestComplete(status, timeElapsed);
	TESTEL(KErrNone == err, err);

	TInt delta = timeElapsed.Int() - timeouts.iLongTimeout;
	TESTEL(delta >= -1 && delta <= 1, delta);

	/*
	 * Tidy up
	 */

	DestroyUdpSocket(sock1);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest49

/*
 * Test 50
 * TestBadRConnectionSubSessionOpenL()
 * Open a socket associated with a non-started connection. This should fail, then try again with
 * socket server and opened connection from a different socket server - this should panic. Also
 * test the same thing with host resolvers.
 */


TE_RConnectionTest50::~TE_RConnectionTest50()
{
}

enum TVerdict TE_RConnectionTest50::doTestStepL(void)
{

	TRConnParams params(iShortTimeout, iEchoPortNum);
	const TInt KMaxHeapSize = 32768;
	RThread testThread;

	testThread.Create(_L("test"), (TThreadFunction)PanicTestThread::EntryPoint, KDefaultStackSize, KMinHeapSize, KMaxHeapSize, &params);
	TRequestStatus logon;
	testThread.Logon(logon);
	testThread.Resume();
	User::WaitForRequest(logon);
	if(testThread.ExitReason() == 23) //23 = EBadConnection panic in esock
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
} // TE_RConnectionTest50


TE_RConnectionTest51::~TE_RConnectionTest51()
{
}

enum TVerdict TE_RConnectionTest51::doTestStepL()
/*
 * Use the service notification and cancel service notification methods
 */
	{
	TInt err;
	TRequestStatus status;

	RSocketServ ss;
	RConnection conn;

	TUint32 isp;
	TBuf<20> newServiceType;

	// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
	// START
	// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
	// Open a SocketServer and Push on the CleanupStack
	err = OpenSocketServer (ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL (ss);

	// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
	// FIRST: Trying to use ServiceChangeNotification BEFORE 
	// Starting the Connection. It is supposed to pass but
	// to set the Status to KErrNotReady.
	// It will mean that the API can't be used yet.
	// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
	INFO_PRINTF1(_L("PART 1: Trying to use ServiceChangeNotification BEFORE Starting the Connection."));
	
	// Open a Connection and Put on the CleanupStack
	err = OpenConnection (conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL (conn);

	// Register for ServiceChangeNotification
	conn.ServiceChangeNotification (isp, newServiceType, status);

	// Start the Connection with another IAP
	err = StartConnection (conn);
	TESTEL(KErrNone == err, err);

	// Wait for the Start event to be propagated as "ServiceChangeNotification"
	User::WaitForRequest (status);
	// Check the Result (it MUST be "KErrNone")
	TESTEL(status.Int() == KErrNotReady, status.Int());

	// Cancelling Registration for ServiceChangeNotification
	conn.CancelServiceChangeNotification ();
	// Stop the Connection: IT WILL GENERATE A "SERVICE CHANGED NOTIFICATION"
	conn.Stop ();

	// Close, Pop and Destroy the Connection
	CloseConnection (conn);
	CleanupStack::PopAndDestroy ();
	
	// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
	// SECOND: Trying to use ServiceChangeNotification AFTER 
	// Starting the Connection. We will use "Connection.Stop()" as event
	// to Monitor. It is supposed to set the Status to KErrCancel.
	// It will mean that the "Stop" cancelled the ServiceChangeNotification
	// request.
	// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
	INFO_PRINTF1(_L("PART 2: Trying to use ServiceChangeNotification AFTER Starting the Connection."));
	
	// Open a Connection and Put on the CleanupStack
	err = OpenConnection (conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL (conn);

	// Start for the First Time (needed to have an underlying layer)
	err = StartConnection (conn);
	TESTEL(KErrNone == err, err);

	// Register for ServiceChangeNotification
	conn.ServiceChangeNotification (isp, newServiceType, status);
	// Stop the Connection: IT WILL GENERATE A "SERVICE CHANGED NOTIFICATION"
	conn.Stop ();
	// Wait for the Stop event to be propagated as "ServiceChangeNotification"
	User::WaitForRequest (status);
	// Check the Result (it MUST be "KErrCancel")
	TESTEL(status.Int() == KErrCancel, status.Int());

	// Cancelling Registration for ServiceChangeNotification
	conn.CancelServiceChangeNotification ();

	// Close, Pop and Destroy the Connection
	CloseConnection (conn);
	CleanupStack::PopAndDestroy ();
	
	// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
	// THIRD: Trying to use ServiceChangeNotification and then cancel the 
	// ServiceChangeNotification Request.
	// It is supposed to set the Status to KErrCancel.
	// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
	INFO_PRINTF1(_L("PART 3: Trying to use ServiceChangeNotification and then cancel the ServiceChangeNotification Request."));
	
	// Open a Connection and Put on the CleanupStack
	err = OpenConnection (conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL (conn);

	// Start for the First Time (needed to have an underlying layer)
	err = StartConnection (conn);
	TESTEL(KErrNone == err, err);

	// Register for ServiceChangeNotification
	conn.ServiceChangeNotification (isp, newServiceType, status);
	// Cancelling Registration for ServiceChangeNotification
	conn.CancelServiceChangeNotification ();

	// Wait for the Cancel to be propagated as "ServiceChangeNotification"
	User::WaitForRequest (status);
	// Check the Result (it MUST be "KErrCancel")
	TESTEL(status.Int() == KErrCancel, status.Int());

	// Stop the Connection
	conn.Stop ();

	// Close, Pop and Destroy the Connection
	CloseConnection (conn);
	CleanupStack::PopAndDestroy ();
	
	// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
	// FOURTH: Verify that CancelServiceChangeNotification has no effect
	// if called without starting the Connection.
	// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
	INFO_PRINTF1(_L("PART 4: Verify that CancelServiceChangeNotification has no effect if called without starting the Connection."));
	
	// Open a Connection and Put on the CleanupStack
	err = OpenConnection (conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL (conn);

	// Cancelling Registration for ServiceChangeNotification
	conn.CancelServiceChangeNotification ();

	// Close, Pop and Destroy the Connection
	CloseConnection (conn);
	CleanupStack::PopAndDestroy ();
	
	// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
	// FIFTH: Verify CancelServiceChangeNotification has no effect if called
	// when there is no outstanding ServiceChangeNotification request.
	// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
	INFO_PRINTF1(_L("PART 5: Verify CancelServiceChangeNotification has no effect if called when there is no outstanding ServiceChangeNotification request."));
	
	// Open a Connection and Put on the CleanupStack
	err = OpenConnection (conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL (conn);

	// Start for the First Time (needed to have an underlying layer)
	err = StartConnection (conn);
	TESTEL(KErrNone == err, err);

	// Cancelling Registration for ServiceChangeNotification (also if there
	// is no one, it will go fine)
	conn.CancelServiceChangeNotification ();

	// Stop the Connection
	conn.Stop ();

	// Close, Pop and Destroy the Connection
	CloseConnection (conn);
	CleanupStack::PopAndDestroy ();
	
	// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
	// SIXTH: Verify ServiceChangeNotification fails if the connection
	// is stopped before the request complete.
	// It is supposed to set the Status to KErrCancel.
	// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
	INFO_PRINTF1(_L("PART 6: Verify ServiceChangeNotification fails if the connection is stopped before the request complete."));
	
	// Open a Connection and Put on the CleanupStack
	err = OpenConnection (conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL (conn);

	// Start for the First Time (needed to have an underlying layer)
	err = StartConnection (conn);
	TESTEL(KErrNone == err, err);

	// Register for ServiceChangeNotification
	conn.ServiceChangeNotification (isp, newServiceType, status);

	// Stop the Connection
	conn.Stop ();

	// Wait for the Cancel to be propagated as "ServiceChangeNotification"
	User::WaitForRequest (status);
	// Check the Result (it MUST be "KErrCancel")
	TESTEL(status.Int() == KErrCancel, status.Int());

	// Close, Pop and Destroy the Connection
	CloseConnection (conn);
	CleanupStack::PopAndDestroy ();
	
	// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
	// END
	// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
	// Close, Pop and Destroy the SocketServer
	CloseSocketServer (ss);
	CleanupStack::PopAndDestroy ();

	return TestStepResult ();
}


TE_RConnectionTest52::~TE_RConnectionTest52()
	{ }

enum TVerdict TE_RConnectionTest52::doTestStepL()
/*
 * Test the connection enumeration methods
 */
	{

	TInt err(KErrNone);

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	//
	// Find out how many connections are alive
	//

	TUint count(0);
	err = EnumerateConnections(conn, count);
	TESTEL(KErrNone == err, err);
	TESTL(count == 1);

	//
	// Test retrieval of connection information
	//

	TPckgBuf<TConnectionInfo> info;
	err = GetConnectionInfo(conn, 0, info);
	TESTL(KErrArgument == err);

	err = GetConnectionInfo(conn, 1, info);
	TESTEL(KErrNone == err, err);

	TPckgBuf<TConnectionInfo> info2;
	err = GetInfo(conn, info2);
	TESTEL(err == KErrNone, err);
	TESTL(info == info2);

	err = GetConnectionInfo(conn, 2, info);
	TESTL(KErrNotFound == err);

	//
	// Stop the connection
	//

	TNifProgressBuf prog;
	Progress(conn, prog());

	err = StopConnection(conn);
	TESTEL(KErrNone == err, err);

	//
	// Wait until the connection has stopped
	//

	TRequestStatus stat;
	while(prog().iStage!=KConnectionUninitialised)
		{
		ProgressNotification(conn, stat, prog, KConnectionUninitialised);
		User::WaitForRequest(stat);
		}

	User::After(1000000);	// urgh!

	//
	// Find out how many connections are alive
	//

	err = EnumerateConnections(conn, count);
	TESTEL(KErrNone == err, err);
	TESTEL(count == 0, count);

	CloseConnection(conn);
	CleanupStack::Pop();  // conn

	CloseSocketServer(ss);
	CleanupStack::Pop();  // ss

	return TestStepResult();
	}


TE_RConnectionTest53::~TE_RConnectionTest53()
	{ }

enum TVerdict TE_RConnectionTest53::doTestStepL()
/*
 * Test the connection client enumeration methods
 */
	{

	TInt err(KErrNone);

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	RSocket sock1;
	err = OpenTcpSocketExplicit(sock1, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock1);


	RHostResolver hr;
	err = OpenHostResolverExplicit(hr, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	//
	// Find out how many connections are alive
	//

	TUint count(0);
	err = EnumerateConnections(conn, count);
	TESTEL(KErrNone == err, err);
	TESTL(count == 1);

	//
	// Retrieve connection information
	//

	TPckgBuf<TConnectionInfo> info;
	err = GetConnectionInfo(conn, 1, info);
	TESTEL(KErrNone == err, err);

	TPckgBuf<TConnectionInfo> info2;
	err = GetInfo(conn, info2);
	TESTEL(err == KErrNone, err);
	TESTL(info == info2);

	//
	// Find out how many clients of the connection there are
	//

	err = EnumerateClients(conn, 1, count);
	TESTEL(KErrNone == err, err);
	TESTL(count == 1);

	//
	// Test retrieval of connection client information
	//

	TConnectionClientInfo clientInfo(0, 0, 0);
	err = GetConnectionClientInfo(conn, 1, clientInfo);
	TESTEL(KErrNone == err, err);
	TESTEL(clientInfo.iThreadId == RThread().Id(), KErrGeneral);

	RProcess proc;
	err = RThread().Process(proc);
	TESTEL(KErrNone == err, err);
	TESTL(clientInfo.iProcessId == proc.Id());
	TESTL(clientInfo.iUid == proc.Type().MostDerived());

	//
	// Stop the connection
	//

	err = StopConnection(conn);
	TESTEL(err == KErrNone, err);

	CleanupStack::PopAndDestroy(&hr); //hr
	CleanupStack::PopAndDestroy(&sock1); //sock1
	CleanupStack::PopAndDestroy(&sock); //sock
	CloseConnection(conn);
	CleanupStack::Pop();  // conn

	CloseSocketServer(ss);
	CleanupStack::Pop();  // ss

	return TestStepResult();
	}


TE_RConnectionTest54::~TE_RConnectionTest54()
	{ }

enum TVerdict TE_RConnectionTest54::doTestStepL()
/*
 * Test the connection socket enumeration methods
 */
	{

	TInt err(KErrNone);

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = ConnectTcpSocket(sock, iDummyNifSendAddr);  // used here to connect the UDP socket
	TESTEL(KErrNone == err, err);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	//
	// Find out how many connections are alive
	//

	TUint count(0);
	err = EnumerateConnections(conn, count);
	TESTEL(KErrNone == err, err);
	TESTL(count == 1);

	//
	// Retrieve connection information
	//

	TPckgBuf<TConnectionInfo> info;
	err = GetConnectionInfo(conn, 1, info);
	TESTEL(KErrNone == err, err);

	TPckgBuf<TConnectionInfo> info2;
	err = GetInfo(conn, info2);
	TESTEL(err == KErrNone, err);
	TESTL(info == info2);

	//
	// Find out how many sockets of the connection there are
	//

	err = EnumerateSockets(conn, 1, count);
	TESTEL(KErrNone == err, err);
	TESTL(count == 1);

	//
	// Test retrieval of connection socket information
	//

	TConnectionSocketInfo socketInfo;
	err = GetConnectionSocketInfo(conn, 1, socketInfo);
	TESTEL(KErrNone == err, err);
	TESTL(socketInfo.iAddressFamily == KAfInet);
	TInetAddr source;
	sock.LocalName(source);
	TESTL(socketInfo.iSourceAddress == source);
	TESTL(socketInfo.iDestinationAddress == iDummyNifSendAddr);
	TESTL(socketInfo.iProtocol == KProtocolInetUdp);

	//
	// Close the socket
	//

	DestroyUdpSocket(sock);
	CleanupStack::Pop();  // sock

	//
	// Find out how many sockets of the connection there are
	//

	err = EnumerateSockets(conn, 1, count);
	TESTEL(KErrNone == err, err);
	TESTL(count == 0);

	//
	// Stop the connection
	//

	err = StopConnection(conn);
	TESTEL(err == KErrNone, err);

	CloseConnection(conn);
	CleanupStack::Pop();  // conn

	CloseSocketServer(ss);
	CleanupStack::Pop();  // ss

	return TestStepResult();
	}


TE_RConnectionTest55::~TE_RConnectionTest55()
{
}

// NOTE This test is intended to be run MANUALLY
// Creates a connection, stops  and destroys it again
// Related to test 1

enum TVerdict TE_RConnectionTest55::doTestStepL(void)
{
	TInt err;

	RConnection conn;
	RSocketServ ss;
	TRequestStatus status;
	TCommDbConnPref prefs;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	//StartConnectionAsynchronous(conn, status);
	StartConnection(conn);
	//	User::After(5000000);
	//User::WaitForRequest(status);

	err = StopConnection(conn);
	TESTEL(KErrNone == err, err);

	// IAP == 2 => Pipex via IR GSM
	StartConnectionWithOverridesAsynchronous(conn, prefs, 2, status);

	User::WaitForRequest(status);
	TESTEL(status.Int() == KErrNone, status.Int());

	// AW...the connection should close...
	err = StopConnection(conn);
	TESTEL(KErrNone == err, err);
	// end

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest55


TE_RConnectionTest56::~TE_RConnectionTest56()
{
}

// Creates a connection, stops  and destroys it again
// Related to test 1
enum TVerdict TE_RConnectionTest56::doTestStepL(void)
{
	TInt err;

	RConnection conn;
	RSocketServ ss;
	TRequestStatus status;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	StartConnectionAsynchronous(conn, status);

	User::WaitForRequest(status);

	err = StopConnection(conn);
	TESTEL(KErrNone == err, err);

	// status completion code should be either:
	//	KErrNone - if asynchronous start has completed
	//	KErrCancel - stop terminates start
	// anything else is an error.
	if((status.Int() == KErrCancel || status.Int() == KErrNone))
	{
		SetTestStepResult(EPass);
	}

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionTest56

// Related to test 23

TE_RConnectionTest57::~TE_RConnectionTest57()
{
}

enum TVerdict TE_RConnectionTest57::doTestStepL(void)
/*
 * Create two connections on the same interface and check that the progress notification
 * for each is the same
 */
{
	TInt err;
	TRequestStatus startStatus, status;

	RSocketServ ss;
	RConnection conn1;
	TNifProgressBuf progress;
	TCommDbConnPref prefs;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	/*
	 * Get the first connection going, monitoring the progress
	 */

	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	StartConnectionWithOverridesAsynchronous(conn1, prefs, iDefaultIap, startStatus);

	// wait for KStartingSelection (1000)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KStartingSelection == progress().iStage, progress().iStage);

	// wait for KFinishedSelection (2000)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KFinishedSelection == progress().iStage, progress().iStage);

	// wait for ECsdScanningScript (2502)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdScanningScript == progress().iStage, progress().iStage);

	// wait for ECsdScannedScript (2503)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdScannedScript == progress().iStage, progress().iStage);

	// wait for ECsdGettingLoginInfo (2504)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdGettingLoginInfo == progress().iStage, progress().iStage);

	// wait for ECsdGotLoginInfo (2505)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdGotLoginInfo == progress().iStage, progress().iStage);

	// wait for ECsdStartingConnect (2506)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdStartingConnect == progress().iStage, progress().iStage);

	// wait for ECsdFinishedConnect (2507)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdFinishedConnect == progress().iStage, progress().iStage);

	// wait for ECsdConnectionOpen (3500)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdConnectionOpen == progress().iStage, progress().iStage);

	// wait for KMinNifProgress (6000)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KMinNifProgress == progress().iStage, progress().iStage);

	// wait for EIfProgressLinkUp
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KLinkLayerOpen == progress().iStage, progress().iStage);

	User::WaitForRequest(startStatus);
	TESTEL(startStatus.Int() == KErrNone, startStatus.Int());

	err = StopConnection(conn1);
	TESTEL(KErrNone == err, err);

	/****************************** restart connection ******************************/

	StartConnectionWithOverridesAsynchronous(conn1, prefs, iDefaultIap, startStatus);

	// wait for KStartingSelection (1000)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);	
	progress();
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KStartingSelection == progress().iStage, progress().iStage);

	// wait for KFinishedSelection (2000)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KFinishedSelection == progress().iStage, progress().iStage);

	// wait for ECsdScanningScript (2502)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdScanningScript == progress().iStage, progress().iStage);

	// wait for ECsdScannedScript (2503)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdScannedScript == progress().iStage, progress().iStage);

	// wait for ECsdGettingLoginInfo (2504)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdGettingLoginInfo == progress().iStage, progress().iStage);

	// wait for ECsdGotLoginInfo (2505)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdGotLoginInfo == progress().iStage, progress().iStage);

	// wait for ECsdStartingConnect (2506)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdStartingConnect == progress().iStage, progress().iStage);

	// wait for ECsdFinishedConnect (2507)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdFinishedConnect == progress().iStage, progress().iStage);

	// wait for ECsdConnectionOpen (3500)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdConnectionOpen == progress().iStage, progress().iStage);

	// wait for KMinNifProgress (6000)
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KMinNifProgress == progress().iStage, progress().iStage);

	// wait for EIfProgressLinkUp
	ProgressNotification(conn1, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KLinkLayerOpen == progress().iStage, progress().iStage);

	User::WaitForRequest(startStatus);
	TESTEL(startStatus.Int() == KErrNone, startStatus.Int());

	err = StopConnection(conn1);
	TESTEL(KErrNone == err, err);


	// Fix for the test break due to additional progress notifications
	// resulting from changes to Nifman in GT0161 project.
	// Because there are additional progress notifications, we ingore the ones we don't
	// explicitly expect.
	WaitForRelevantProgress(conn1, progress, ECsdStartingHangUp);

	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(ECsdStartingHangUp == progress().iStage, progress().iStage);
	
    INFO_PRINTF1(_L("Waiting for KConnectionDown and KConnectionUninitialised. If the test times-out they didn't come (in the right seq)."));	
    WaitForRelevantProgress(conn1, progress, KConnectionDown);
    WaitForRelevantProgress(conn1, progress, KConnectionUninitialised);	

	CloseConnection(conn1);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest57


TE_RConnectionTest58::~TE_RConnectionTest58()
{
}

// Check that connection status is saved and returned even
// after an interface is dropped

// RConnection::Start()
// wait for idle timer to close connection
// RConnection::LastProgressError()
// check that return code from LastProgressError() is KErrNone,
// 	and that iStage is KLinkLayerDown and iError is KErrTimedOut

enum TVerdict TE_RConnectionTest58::doTestStepL(void)
{
	TInt err;

	RConnection conn;
	RSocketServ ss;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	// wait for idle timer to close connection
	err = WaitForAllInterfacesToCloseL(ss);

	// RConnection::LastProgressError()
	TNifProgress lastError;
	err = LastProgressError(conn, lastError);

	// check that return code from LastProgressError() is KErrNone,
	// 	and that iStage is KLinkLayerDown and iError is KErrTimedOut
	TESTEL(KErrNone == err, err);
	TESTEL(KLinkLayerClosed == lastError.iStage, lastError.iStage);
	TESTEL(KErrTimedOut == lastError.iError, lastError.iError);

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest58



TE_RConnectionTest59::~TE_RConnectionTest59()
{
}

enum TVerdict TE_RConnectionTest59::doTestStepL(void)
{

	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	/*
	 * Get the first connection going, monitoring the progress
	*/

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDefaultIap);
	TESTEL(KErrNone == err, err);

	/*
	 * let's create and connect a tcp socket - connect is the call that
	 * would implicitly create an interface if one didn't exist (not send)
	 */

	err = OpenTcpSocketExplicit(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = WaitForAllInterfacesToCloseL(ss);
	TESTEL(KErrNone == err, err);

	err = ConnectTcpSocket(sock, iEchoServerAddr);
	// TimeOuts - unable to connect to ssock whis is down
	TESTEL(KErrTimedOut == err, err);
	
	err = TestTcpDataPathL(sock);
	TESTEL(KErrTimedOut == err, err);
	

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	err = WaitForAllInterfacesToCloseL(ss);
	TESTEL(KErrNone == err, err);

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return EPass;
}


TE_RConnectionTest60::~TE_RConnectionTest60()
{
}

enum TVerdict TE_RConnectionTest60::doTestStepL(void)
{
/**/
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	// Get the first connection going, monitoring the progress

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDefaultIap);
	TESTEL(KErrNone == err, err);

	// Now ascociate the host resolver with the connection
	err = OpenHostResolverExplicit(hr, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	err = MakeNameRequest(hr, iTestName); // this will use GetByName()
	TESTEL(KErrNone == err, err);

	err = WaitForAllInterfacesToCloseL(ss);
	TESTEL(KErrNone == err, err);

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return EPass;
}

TE_RConnectionTest61::~TE_RConnectionTest61()
{
}

enum TVerdict TE_RConnectionTest61::doTestStepL(void)
{
	TInt err;
	TRequestStatus statusC, statusP;
	RConnection conn;
	RSocketServ ss;
	TNifProgressBuf progress;

	// ...
	INFO_PRINTF1(_L("Connect Sockect Server and Open Connection"));
	err = ss.Connect();
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	err =  conn.Open(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start + stop
	INFO_PRINTF1(_L("Start Connection"));
	conn.Start(statusC);
	conn.ProgressNotification(progress, statusP);
	User::WaitForRequest(statusP);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KStartingSelection == progress().iStage, progress().iStage);
	INFO_PRINTF1(_L("Stop Connection"));
	err = conn.Stop();
	TESTEL(KErrNone == err, err);
	User::WaitForRequest(statusC);


	// and again start + stop
	INFO_PRINTF1(_L("Start Connection 2nd Time"));
	conn.Start(statusC);
	conn.ProgressNotification(progress, statusP);
	User::WaitForRequest(statusP);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KStartingSelection == progress().iStage, progress().iStage);
	INFO_PRINTF1(_L("Stop Connection 2nd Time"));
	err = conn.Stop();
	TESTEL(KErrNone == err, err);
	User::WaitForRequest(statusC);


	// last time start + stop
	INFO_PRINTF1(_L("Start Connection 3rd Time"));
	conn.Start(statusC);
	conn.ProgressNotification(progress, statusP);
	User::WaitForRequest(statusP);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KStartingSelection == progress().iStage, progress().iStage);
	INFO_PRINTF1(_L("Stop Connection 3rd Time"));
	err = conn.Stop();
	TESTEL(KErrNone == err, err);
	User::WaitForRequest(statusC);


	// Finally start connection fully and stop.
	INFO_PRINTF1(_L("Start Connection 4th Time"));
	err = conn.Start();
	TESTEL(KErrNone == err, err);
	INFO_PRINTF1(_L("Stop Connection 4th Time"));
	err = conn.Stop();
	TESTEL(KErrNone == err, err);

	// ...done.
	INFO_PRINTF1(_L("Close Connection and Socket Server"));
	conn.Close();
	CleanupStack::Pop();
	ss.Close();
	CleanupStack::Pop();
	return EPass;
} // TE_RConnectionTest61::doTestStepL()


TE_RConnectionTest63::~TE_RConnectionTest63()
{
}

enum TVerdict TE_RConnectionTest63::doTestStepL(void)
/**
Check that the following works okay on a single unconnected UDP socket:
(1) SendTo() towards a scoped address routeable to an active interface
(2) SendTo() towards an un-scoped address
The second write generates a CSocket::NoBearer() upcall in ESOCK that has
to be handled correctly to avoid the second SendTo() hanging.
*/
{
	TInt err;
	RConnection conn;
	RSocketServ ss;

	// instantiate ESock session and RConnection
	err = ss.Connect();
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	err =  conn.Open(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start Dummy NIF
	err = StartConnectionWithOverridesSynchronous(conn, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);

	// Open UDP socket
	RSocket sock;

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	TBuf8<10> outBuf;				// setup a simple output buffer for SendTo()'s
	outBuf.Append(_L("xxxxx"));

	TPckgBuf<TConnectionInfo> info;
	GetInfo(conn, info);			// retrieve IAP and (especially) Network Id

	TInetAddr addr;
	TRequestStatus status;

	// Send 1: to a scoped address that is routeable to the Dummy NIF
	// (note: internal assumption that scope id is same as Network Id)
	addr.SetV4MappedAddress(KDummyNifLocalAddressBase + 1);
	addr.SetScope(info().iNetId);

	// DEF51179 - Set the port# top some legitimate value, otherwise the address parsing within
	// CProviderUDP6::Send() will fail.
	//
	const TUint KRandomValidPort = 10;
	addr.SetPort(KRandomValidPort);

	sock.SendTo(outBuf, addr, 0, status);
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());

	// Send 2: to an un-scoped address
	// (note: using same address as above but without the scope doesn't
	// seem to work - it gets routed okay without generating a NoBearer().
	// Invent an un-scoped address that isn't the same as the Dummy NIF address).
	_LIT(KUnscopedAddress, "::ffff:100.1.1.1");
	addr.Input(KUnscopedAddress());
	addr.SetScope(0);				// just for sanity
	sock.SendTo(outBuf, addr, 0, status);
	User::WaitForRequest(status);
	TESTEL(KErrNone == status.Int(), status.Int());

	// clean up.
	CleanupStack::PopAndDestroy();	// sock
	conn.Stop();
	CleanupStack::PopAndDestroy();	// conn
	CleanupStack::PopAndDestroy();	// ss
	return EPass;
}

/*
 * Test TE_RConnectionStep::StopAllInterfaces
 * When a test fails (and leaves) it can leave intefaces up - and if it has the idle timers disabled then
 * the interfaces will stay around forever. This method attempts to kill them again so the next tests
 * can again run in isolation (and pass?).
 * Simple test: create some interfaces, disable idle timers, then kill the interfaces with
 * the method StopAllInterfaces().
 * The method StopAllInterfaces has been moved into TS_EsockUtil and so that version will now be tested here.
 */


TE_RConnectionTest64::~TE_RConnectionTest64()
{
}

enum TVerdict TE_RConnectionTest64::doTestStepL(void)
{
	TInt err;
	RSocketServ ss;
	RConnection conn1, conn2;
	TUint numOfConnections;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//********** open the first connection ***********

	// open it
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);
	// start it
	err = StartConnectionWithOverrides(conn1, iHungryNifIap);
	TESTEL(KErrNone == err, err);
	// turn off the idle timers for this connection
	err = conn1.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	//********** open the second connection ***********

	// open it
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);
	// start it
	err = StartConnectionWithOverrides(conn2, iDummyNifIap);
	TESTEL(KErrNone == err, err);
	// turn off the idle timers for this connection
	err = conn2.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
	TESTEL(KErrNone == err, err);

	//********** just make sure we have the right number of interfaces ***********

	err = EnumerateConnections(conn2, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(2 == numOfConnections, numOfConnections);

	//********** the meat of the test ***********

	TBuf<200> errDesc;
	err = ESockTestUtils::StopAllInterfaces(errDesc);
	INFO_PRINTF3(_L("StopAllInterfaces() err=%d\n%S"), err, &errDesc);
	TESTEL(KErrNone == err, err);

	//********** pop everything off the cleanup stack? ***********

	// hmmmm this is a bit odd, but we really do need to get it off the cleanup stack before finishing :-S
	// ...have already been closed by StopAllInterfaces hopefully.
	CleanupStack::Pop(2);

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest64::doTestStepL()


TE_RConnectionTest65::~TE_RConnectionTest65()
{
}

enum TVerdict TE_RConnectionTest65::doTestStepL(void)
{
	TInt err;
	TRequestStatus status;
	RConnection conn;
	RSocketServ ss;

	err = ss.Connect();
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	err =  conn.Open(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// start + stop
	conn.Start(status);
	err = conn.Stop();

	User::WaitForRequest(status);
	TESTEL(KErrNone == err, err);
	TESTEL(KErrCancel == status.Int(), status.Int());

	// ...done.
	CleanupStack::PopAndDestroy(2);	// conn, ss
	return EPass;
}

TE_RConnectionTest66::~TE_RConnectionTest66()
{
}
enum TVerdict TE_RConnectionTest66::doTestStepL(void)
/*
 * When the LONG and MEDIUM Idle timers are disabled, ensure that Close()
 * effectively pulls down the interface when no other subsessions are
 * associated with it.
 */
{
	TInt err;

	RSocketServ ss;
	RConnection conn;
	RSocket sock;
	TUint numOfConnections = 0;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iNtRasIapOnlyShortTimer);
	TESTEL(KErrNone == err, err);

	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTEL(KErrNone == err, err);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();

	User::After(iShortTimeout+10000000); // wait for the interface to be destroyed

	numOfConnections = NumberOfInterfacesL(ss);
	TESTEL(0 == numOfConnections, numOfConnections);

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest66


TE_RConnectionTest67::~TE_RConnectionTest67()
{
}
enum TVerdict TE_RConnectionTest67::doTestStepL(void)
/*
 * Ensure Close() cancels a Start()
 */
{
	TInt err;

	TRequestStatus startStatus;
	RSocketServ ss;
	RConnection conn;
	// amount of time to wait for underlying interface to disappear after closing RConnection during a start
	const TInt KShortWaitTime = 3;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	// Start the connection asynchronously and immediately close it

	StartConnectionAsynchronous(conn, startStatus);

	CloseConnection(conn);
	CleanupStack::Pop();

	// Wait for the RConnection closure to generate a KErrCancel completion (note that this
	// does not necessarily mean that the underlying interface has gone away).

	User::WaitForRequest(startStatus);
	TESTEL(KErrCancel == startStatus.Int(), startStatus.Int());

	// Now check to see that the interface disappears (fairly soon).

	err = WaitForAllInterfacesToCloseL(ss, KShortWaitTime);

	TESTEL(KErrNone == err, err);

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest67



//
// Tests to support Nifman idle timeouts runtime update mechanism.
// Design decision: the testing is split into test cases by the type of timeout, rather than
// by the test scenario, to make it is easier to track possible defects to a specific timeout type.
//

/*
 Test 68: LastSessionClosed timeout.
 C++ constructor
 */

/**
C++ destructor
*/
TE_RConnectionTest68::~TE_RConnectionTest68()
	{
	}

/**
Configures and executes the test sequence for the LastSessionClosed (Short) timeout
@return test result
*/
enum TVerdict TE_RConnectionTest68::doTestStepL(void)
	{
	// Call the base test logic for the LastSessionClosed timeout with parameters:

	// Increase the timeout by a given factor. Tests the effects on Nifman
	// if the timeout is lengthened.
	// Uses "increase by a factor" method to guarantee that the new value is longer than
	// the exising one. This makes this test independent of the values in CommDB.
	// Covers the general case where the timeout is lengthened.
	static const TInt KTimeoutIncreaseFactorOfTwo(2);
	DoTestSequenceL(ELastSessionClosedTimeout,  KTimeoutIncreaseFactorOfTwo, KErrNone);

	// Test the effects on Nifman when the timeout is set to zero.
	// This covers the general case when the timeout is shortened.
	static const TInt KNewTimeoutZero(0);
	DoTestSequenceL(ELastSessionClosedTimeout, KNewTimeoutZero, KErrNone);

	// Test the effects on Nifman when the timeout is made negative
	// This is an obvious error case. Nifman expires immediately on a negative timeout.
	// It is possible that in the future negative timeouts will be disallowed, and this test
	// will have to be deprecated.
	static const TInt KNewTimeoutNegative(-1);
	DoTestSequenceL(ELastSessionClosedTimeout, KNewTimeoutNegative, KErrNone);

	return TestStepResult();
	}

TE_RConnectionTest69::~TE_RConnectionTest69()
	{
	}

/**
Configures and executes the test sequence for the LastSocketClosed (Medium) timeout
@return test result
*/
enum TVerdict TE_RConnectionTest69::doTestStepL(void)
	{
	// Call the base test logic for the LastSocketClosed timeout with parameters:

	// Increase the timeout by a given factor.  Tests the effects on Nifman
	// if the timeout is lengthened.
	// Uses "increase by a factor" method to guarantee that the new value is longer than
	// the exising one. This makes this test independent of the values in CommDB.
	// Covers the general case where the timeout is lengthened.
	static const TInt KTimeoutIncreaseFactorOfTwo(2);
	DoTestSequenceL(ELastSocketClosedTimeout,   KTimeoutIncreaseFactorOfTwo, KErrNone);

	// Test the effects on Nifman when the timeout is set to zero.
	// This covers the general case when the timeout is shortened.
	static const TInt KNewTimeoutZero(0);
	DoTestSequenceL(ELastSocketClosedTimeout, KNewTimeoutZero, KErrNone);

	// Test the effects on Nifman when the timeout is made negative.
	// It is possible that in the future negative timeouts will be disallowed, and this test
	// will have to be deprecated.
	static const TInt KNewTimeoutNegative(-1);
	DoTestSequenceL(ELastSocketClosedTimeout, KNewTimeoutNegative, KErrNone);

	return TestStepResult();
	}



TE_RConnectionTest70::~TE_RConnectionTest70()
	{
	}

/**
Configures and executes the test sequence for the LastSocketActivity (Long) timeout
@return test result
*/
enum TVerdict TE_RConnectionTest70::doTestStepL(void)
	{
	// Call the base test logic for the LastSocketActivity timeout with parameters:

	// Increase the timeout by a given factor.  Tests the effects on Nifman
	// if the timeout is lengthened.
	// Uses "increase by a factor" method to guarantee that the new value is longer than
	// the exising one. This makes this test independent of the values in CommDB.
	// Covers the general case where the timeout is lengthened.
	static const TInt KTimeoutIncreaseFactorOfTwo(2);
	DoTestSequenceL(ELastSocketActivityTimeout, KTimeoutIncreaseFactorOfTwo, KErrNone);

	// Test the effects on Nifman when the timeout is set to zero.
	// This covers the general case when the timeout is shortened.
	static const TInt KNewTimeoutZero(0);
	DoTestSequenceL(ELastSocketActivityTimeout, KNewTimeoutZero, KErrNone);

	// It is possible that in the future negative timeouts will be disallowed, and this test
	// will have to be deprecated.
	static const TInt KNewTimeoutNegative(-1);
	DoTestSequenceL(ELastSocketActivityTimeout, KNewTimeoutNegative, KErrNone);

	return TestStepResult();
	}

/**
Implements the test sequence for the Nifman Idle timeout runtime update mechanism.

@param aTimeoutType timeout to test: one of:
	ELastSocketActivityTimeout to test LastSocketActivity (Long) timeout
	ELastSocketClosedTimeout   to test LastSocketClosed (Medium) timeout
	ELastSessionClosedTimeout  to test LastSessionClosed (Short) timeout
@param aTimeoutChangeFactor factor by which to change the existing (specified in the Bearer table) timeout.
@param aExpectedUpdateErr error expected to be returned by the update routines.
@post the test result is set to Pass if the test passes, Fail otherwise.
*/
void TE_RConnectionIdleTimeoutUpdateTest::DoTestSequenceL(enum TTimeoutType aTimeoutType, TInt aTimeoutChangeFactor, TInt aExpectedUpdateErr)
	{
	TRequestStatus status;

	RSocketServ ss;
	RConnection conn1, conn2, connMonitor;
	RSocket sock;

	TPckgBuf<TConnectionInfo> info;
	TUint numOfConnections;
	TNifProgressBuf progress;

	TTimeoutValues timeouts;
	TTimeIntervalSeconds timeElapsed;

	TInt err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//
	// Create a connection and make Nifman read Timeout values from CommDB
	//
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn1);

	err = OpenConnection(connMonitor, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connMonitor);

	err = StartConnectionWithOverrides(conn1, iDummyNifLongTimeoutIap);
	TESTEL(KErrNone == err, err);

	err = GetTimeoutValues(conn1, timeouts);
	TESTEL(KErrNone == err, err);

	// need to call Enumerate() before GetConnectionInfo() to set up array used there
	err = EnumerateConnections(conn1, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(numOfConnections == 1, numOfConnections);

	err = conn1.GetConnectionInfo(1, info);
	TESTEL(KErrNone == err, err);

	err = AttachMonitor(connMonitor, info);
	TESTEL(KErrNone == err, err);



	//
	// Engage the timeout specified by the test scenario
	//
	TInt curTimeoutVal(0); // Value read from CommDB
	TInt ifaceColTimeoutOption(-1); // Control option to set the specific timeout

	if(ELastSessionClosedTimeout == aTimeoutType) //Short: No Socket and no RConnections
		{
		CloseConnection(conn1); // close the original connection
		CleanupStack::Pop(&connMonitor);
		CleanupStack::Pop(&conn1);
		CleanupClosePushL(connMonitor);

		curTimeoutVal = timeouts.iShortTimeout;
		ifaceColTimeoutOption = KTestSoDummyNifSetLastSessionClosedTimeout;
		}
	else if(ELastSocketClosedTimeout == aTimeoutType) // Medium: RConnections, no Socket
		{
		// We already have an RConnection, so don't do anything.

		curTimeoutVal = timeouts.iMediumTimeout;
		ifaceColTimeoutOption = KTestSoDummyNifSetLastSocketClosedTimeout;
		}
	else if(ELastSocketActivityTimeout == aTimeoutType) // Long: RConnection and Socket
		{
		err = OpenUdpSocketExplicitL(sock, ss, conn1);
		TESTEL(KErrNone == err, err);
		CleanupClosePushL(sock);

		err = TestUdpDataPathL(sock, iDummyNifSendAddr);
		TESTEL(KErrNone == err, err);

		curTimeoutVal = timeouts.iLongTimeout;
		ifaceColTimeoutOption = KTestSoDummyNifSetLastSocketActivityTimeout;
		}
	else // Invalid Timeout type.
		{
		User::Leave(KErrArgument);
		}


	//
	// Change the original timeout after it was loaded by Nifman from CommDB.
	//
	TInt newTimeoutVal(aTimeoutChangeFactor * curTimeoutVal);
	INFO_PRINTF2(_L("aTimeoutChangeFactor = %d"), aTimeoutChangeFactor);
	INFO_PRINTF2(_L("curTimeoutVal = %d"), curTimeoutVal);
	INFO_PRINTF2(_L("newTimeoutVal = %d"), newTimeoutVal);
	
	// Nifman updates the timeouts when the current timeout tick (1 second) expires.
	// This means that it may it 2 ticks to process the new timeout and
	// expire it: 1 tick to update the timeout + 1 tick to expire on the new timeout.
	// This matters only if the new timeout is short, i.e. < 3 ticks.
	// In these cases we should factor these 2 "overhead" ticks in when comparing the time it
	// took the system to process the timeout with the timeout given to Nifman.
	// In normal cases, the "extra" 2 don't matter.
	// It takes at least 2 ticks to update and expire on 10 tick timeout, but we don't notice that.

    static const TInt KNifmanTimerOverhead(2);
	static const TInt KBaseDelta(1); // System overhead
	TInt acceptableDelta(KBaseDelta); // Used to determine if the test passed.
	if(KNifmanTimerOverhead + KBaseDelta >= newTimeoutVal) // total possible overhead >= new timeout
		{
		acceptableDelta = KBaseDelta + KNifmanTimerOverhead; // Increase the acceptable delta
		}
	INFO_PRINTF2(_L("acceptableDelta = %d"), acceptableDelta);
	
	// Make sure the new timeout is sufficiently different - the difference is larger than the delta
	// Else we can't be sure that the new timeout was actually used by Nifman.
	// A failure here is test configuration failure: the timeouts in CommDB are too short.
	const TInt KNewTimeoutDiff = Abs(newTimeoutVal - curTimeoutVal);
	TESTEL(KNewTimeoutDiff > acceptableDelta, KNewTimeoutDiff);
	
	// Update the Timeout:
	TPckgBuf<TInt> timeoutValPckg(newTimeoutVal);
	TInt colErr = connMonitor.Control(KCOLInterface, ifaceColTimeoutOption, timeoutValPckg);
	TESTEL(aExpectedUpdateErr == colErr, colErr); // Could not update timeout.

	// Get notification when Nifman times out and closes the connection.
	ProgressNotification(connMonitor, status, progress, KLinkLayerClosed);
	err = TimeUntilRequestComplete(status, timeElapsed); // Wait for the timeout.
	INFO_PRINTF2(_L("ProgressNotification = %d (KErrNone(0) expected)"), err);
	TESTEL(KErrNone == err, err);

	// Determine if the test has passed: Did Nifman actually use the new timeout ?
	if(newTimeoutVal < 0) // A pathological case where a negative timeout is specified.
		{
		newTimeoutVal = 0; // Should expire immediately on a negative timeout.
		}

	TInt delta = Abs(timeElapsed.Int() - newTimeoutVal); // difference between the specified timeout and the
													// time it took the system to process it.
	INFO_PRINTF2(_L("timeElapsed.Int() = %d"), timeElapsed.Int());
	INFO_PRINTF2(_L("newTimeoutVal = %d"), newTimeoutVal);
	INFO_PRINTF2(_L("delta = %d = Abs(timeElapsed.Int() - newTimeoutVal). (delta must be <= acceptableDelta)"), delta);
	TESTEL(delta <= acceptableDelta, delta);

	//
	// The testing is over. Cleanup.
	//
	if(ELastSessionClosedTimeout == aTimeoutType) // No Socket and no RConnections
		{
		CloseConnection(connMonitor);
		CleanupStack::Pop(&connMonitor);
		}
	else if(ELastSocketClosedTimeout == aTimeoutType) // RConnections, no Socket
		{
		// close the original connection
		CloseConnection(connMonitor);
		CloseConnection(conn1);
		CleanupStack::Pop(2);
		}
	else if(ELastSocketActivityTimeout == aTimeoutType) // RConnection and Socket
		{
		DestroyUdpSocket(sock);
		CloseConnection(connMonitor);
		CloseConnection(conn1);
		CleanupStack::Pop(3);
		}
	else
		{
		User::Leave(KErrArgument);
		}

	CloseSocketServer(ss);
	CleanupStack::Pop();
	}

// Start of TE_RConnectionTest71

// Starts a connection in asynchronous mode.
// Checks for Progress Notification for every stage.
// After the completion of first stage, stops with EStopAuthoritative type
// Gets the progress to see if it stopped successfully

TE_RConnectionTest71::~TE_RConnectionTest71()
	{
	}

enum TVerdict TE_RConnectionTest71::doTestStepL(void)
/*
 * Tests if CNifSession::DecideWhetherToRetry() sets the retry flag to false
 * when the error code is KErrConnectionTerminated due to RConnection::Stop(EStopAuthoritative)
 * The client (user of RConnection will be notified with KErrNone though).
 */
	{
	TInt err;
	TRequestStatus status;
	TRequestStatus startStatus;
	TCommDbConnPref prefs;

	RSocketServ ss;
	RConnection conn;
	TNifProgressBuf progress;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

    //Starts the async connection attempt
	StartConnectionWithOverridesAsynchronous(conn,prefs, iDefaultIap,startStatus);

	// wait for KStartingSelection (1000)
	//Checking progress notification for each stage
	ProgressNotification(conn, status, progress);

	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KStartingSelection == progress().iStage, progress().iStage);

	// Stop with EStopAuthoritative type
	conn.Stop(RConnection::EStopAuthoritative);

	User::WaitForRequest(startStatus);
	// start was cancelled
	TESTEL(KErrCancel == startStatus.Int(), startStatus.Int());

	//Checking the status of Progress Notification
	ProgressNotification(conn,status, progress);
	User::WaitForRequest(status);

	TESTEL(KErrNone == progress().iError, progress().iError);

	/*
	 * Close down
	 */
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
	}

// End of Test TE_RConnectionTest71


//
// Start of TE_RConnectionTest72
//
//
// Starts a connection in asynchronous mode.
// Checks for Progress Notification upto selection complete (KFinishedSelection)
// and closes the connection, which should
//

TE_RConnectionTest72::~TE_RConnectionTest72()
	{
	}

enum TVerdict TE_RConnectionTest72::doTestStepL()
	{
	RSocketServ ss;
	TInt err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//
	// Get the connection going, monitoring the progress
	//
	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	TCommDbConnPref prefs;
	TRequestStatus startStatus;
	StartConnectionWithOverridesAsynchronous(conn, prefs, iDefaultIap, startStatus);

	TNifProgressBuf progress;

	//
	// wait for KStartingSelection (1000)
	//
	TRequestStatus status;
	ProgressNotification(conn, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KStartingSelection == progress().iStage, progress().iStage);

	//
	// wait for KFinishedSelection (2000)
	//
	ProgressNotification(conn, status, progress);
	User::WaitForRequest(status);
	TESTEL(KErrNone == progress().iError, progress().iError);
	TESTEL(KFinishedSelection == progress().iStage, progress().iStage);

	//
	// Close the connection and check the expected return, which should be KErrCancel
	//
	CloseConnection(conn);
	User::WaitForRequest(startStatus);
	TESTEL(KErrCancel == startStatus.Int(), startStatus.Int());

	CleanupStack::Pop(&conn);

	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);

	return TestStepResult();
	};

// End of Test TE_RConnectionTest72

TE_RConnectionTest73::~TE_RConnectionTest73()
{
}

enum TVerdict TE_RConnectionTest73::doTestStepL(void)
{
/*Simplistic test to verify cancel of progress notification does not panic client*/
	SetTestStepResult(EFail);
	TRequestStatus notificationStatus;
	TNifProgressBuf progBuffer;

	TInt err;
	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	// Get the first connection going, monitoring the progress
	RConnection conn;
	err = OpenConnection(conn,ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDummyNifIap);
	TESTEL(KErrNone == err, err);

	TPckgBuf<TSubConnectionInfo> subconnectionInfo;
	// for entire connection (index 0)
	err = conn.GetSubConnectionInfo(0, subconnectionInfo);
	TESTEL(KErrNone == err, err);

	TSubConnectionUniqueId SubConnId = subconnectionInfo().iSubConnectionUniqueId;
	conn.ProgressNotification(progBuffer, notificationStatus, SubConnId);
	conn.CancelProgressNotification(SubConnId);
	User::WaitForRequest(notificationStatus);
	if( notificationStatus.Int() == KErrCancel ||  notificationStatus.Int() == KErrNone )
		{
		SetTestStepResult(EPass);
		}
	err = WaitForAllInterfacesToCloseL(ss);
	TESTEL(KErrNone == err, err);

	CleanupStack::Pop();
	CleanupStack::Pop();

	CloseConnection(conn);
	CloseSocketServer(ss);
	return TestStepResult();
}

// End of Test TE_RConnectionTest73

void TestStepInvalidAddress()
	{
	TInt err;

	RSocketServ ss1;
	RConnection conn2;

	err = ss1.Connect();
	__ASSERT_DEBUG(err == KErrNone, User::Panic(KSpecAssert_ESockTestTERCnct, 1));
	CleanupClosePushL(ss1);

	TName name;
	name.Copy(_L("0,3515125"));
	err = conn2.Open(ss1, name);
	//Should panic here
	__ASSERT_DEBUG(err == KErrNone, User::Panic(KSpecAssert_ESockTestTERCnct, 2));
	//Well things should be dead here anyway
	}

TInt TestPanics(TAny* /*aData*/)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();

	TRAPD(err, TestStepInvalidAddress();
	);
	__ASSERT_DEBUG(err == KErrNone, User::Panic(KSpecAssert_ESockTestTERCnct, 3));
	delete cleanup;

	return(KErrNone);
	}

TE_RConnectionTest74::~TE_RConnectionTest74()
{
}

enum TVerdict TE_RConnectionTest74::doTestStepL(void)
{
	TInt err;
	const TInt KMaxHeapSize = 32768;

	RSocketServ ss;
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	// Get the first connection going, monitoring the progress
	RConnection conn;
	err = OpenConnection(conn,ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);

	// With Platform Security, we need to explicitly enable clone opening

	TSecurityPolicyBuf passPolicy(TSecurityPolicy::EAlwaysPass);
	err = conn.Control(KCOLConnection, KCoEnableCloneOpen, passPolicy);
	TESTEL(KErrNone == err, err);

	INFO_PRINTF1(_L("Create thread"));
	RThread testThread;

	TRequestStatus status;
	_LIT(KThreadName,"PanicThread");

	err = testThread.Create(KThreadName, TestPanics, KDefaultStackSize, KMinHeapSize, KMaxHeapSize, NULL);

	TESTL(err == KErrNone);
	CleanupClosePushL(testThread);

	INFO_PRINTF1(_L("Run thread"));
	testThread.Logon(status);
	testThread.Resume();
	User::WaitForRequest(status);

	INFO_PRINTF1(_L("Check for Panic"));
	TESTL(testThread.ExitType() == EExitPanic);

	INFO_PRINTF1(_L("Close thread"));
	CleanupStack::Pop(&testThread);
	testThread.Close();

	conn.Stop();

	CleanupStack::Pop(&conn);
	conn.Close();

	CleanupStack::Pop(&ss);
	ss.Close();


	return EPass;
}

// End of Test TE_RConnectionTest74

//
// Start of TE_RConnectionTest75
//
// Check that an Agent can bring an interface down by issuing 
// Requires use of ADummy agent.

TE_RConnectionTest75::~TE_RConnectionTest75()
	{
	}

enum TVerdict TE_RConnectionTest75::doTestStepL()
	{					
	RSocketServ ss;
	TInt err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	//
	// Get the connection going
	//
	RConnection conn;
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnectionWithOverrides(conn, iDummyNifAgentIap);
	TESTEL(KErrNone == err, err);

	RSocket sock;
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr); // this will use SendTo()
	TESTEL(KErrNone == err, err);

	// Ask the ADummy Agent to stop the connection.
	const TUint KADummyIssueStop = (2 | KConnReadUserDataBit);
	TPckg<TInt> argPckg(1);
	err = conn.Control(KCOLAgent, KADummyIssueStop, argPckg);
	TESTEL(KErrNone == err, err);

	// Note that the connection may take some time to come down.  If we try to 
	// perform TestUdpDataPathL() at this point, it may well complete successfully
	// before the interface has come down.  Consequently, we issue a receive on the
	// socket which should:
	// - block until the connection comes down, 
	// - be error'd with KErrEof (the error code that the ADummy Agent uses)

	TInetAddr recvAddr;
	err = RecvUdpData(sock, recvAddr);
	TESTEL(KErrEof == err, err);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop(&conn);
		
	CloseSocketServer(ss);
	CleanupStack::Pop(&ss);
	return TestStepResult();	
	};

TE_RConnectionTest76::~TE_RConnectionTest76()
	{
	}

enum TVerdict TE_RConnectionTest76::doTestStepL(void)
/**
Check that the following works okay:
	•	Requesting values from GetParameters on an un-started or unconnected
		RConnection object should result in errors being returned and the query
		not being serviced.
*/
	{
	SetTestStepResult(EFail);
	const CommsDat::TMDBElementId KIapTableIdFieldId = KCDTIdIAPRecord | KCDTIdRecordTag;

	TInt err;

	//1.	Instantiate an RSocketServ objects.
	INFO_PRINTF1(_L("1. Instantiate an RSocketServ objects."));
	RConnection connection;
	RSocketServ socketServ;

	//2.	Open the socket servers.
	INFO_PRINTF1(_L("2. Open the socket servers."));
	err = socketServ.Connect();
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServ);

	//3.	Open a connection  but do not start it (use RConnection::Open() but
	//		do not call RConnection::Start()).
	INFO_PRINTF1(_L("3. Open a connection  but do not start it (use RConnection::Open() but do not call RConnection::Start())."));
	err =  connection.Open(socketServ);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connection);


	//4.	Create the neccessry classes and populate with a valid GetParameters
	//		request.
	INFO_PRINTF1(_L("4. Create the neccessry classes and populate with a valid GetParameters request."));
	ESock::XConnectionQuerySet* querySet = NULL;
	querySet = ESock::XConnectionQuerySet::NewL();
	CleanupStack::PushL( querySet );
	ESock::XUintQuery::NewL(KIapTableIdFieldId, *querySet); // ignore returned value, it's safe as member of queryset
	ESock::CConnectionQuerySet* connectionQuerySet = NULL;
	connectionQuerySet = ESock::CConnectionQuerySet::NewL(querySet);
	CleanupStack::Pop( querySet );
	querySet = NULL;
	CleanupStack::PushL( connectionQuerySet );

	//5.	Attempt the request to RConnection::GetParameters().  This should
	//		fail (return an error).
	INFO_PRINTF1(_L("5. Attempt the request to RConnection::GetParameters().  This should fail (return an error)."));
	err = connection.GetParameters( *connectionQuerySet );
	INFO_PRINTF2(_L("5. GetParameters returns <B>%d</b>"), err);
	TESTEL(KErrNotReady == err, err);


	//6.	Close the connections and the socket servers.
	INFO_PRINTF1(_L("6. Close the connections and the socket servers."));
	CleanupStack::PopAndDestroy( connectionQuerySet );
	connectionQuerySet = NULL;

	CleanupStack::PopAndDestroy();	// connection
	CleanupStack::PopAndDestroy();	// socketServ

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TE_RConnectionTest77::~TE_RConnectionTest77()
	{
	}

enum TVerdict TE_RConnectionTest77::doTestStepL(void)
/**
Check that the following works okay:
	•	Requesting ID’s which don’t exist should result in an error response
*/
	{
	SetTestStepResult(EFail);
	const CommsDat::TMDBElementId KUnknownBadId = 0xFFFFFFFF;

	TInt err = KErrNone;

	//1.	Instantiate an RSocketServ objects.
	INFO_PRINTF1(_L("1. Instantiate an RSocketServ objects."));
	RConnection connection;
	RSocketServ socketServ;

	//2.	Open the socket servers.
	INFO_PRINTF1(_L("2. Open the socket servers."));
	err = OpenSocketServer(socketServ); //err = socketServ.Connect();
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServ);

	//3.	Open a connection.
	INFO_PRINTF1(_L("3. Open a connection"));
	err = OpenConnection(connection, socketServ); //err =  connection.Open(socketServ);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connection);

	//4.	Start the RConnection
	INFO_PRINTF1(_L("4. Start the RConnection"));
	err = StartConnection(connection);
	INFO_PRINTF2(_L("4. StartConnection returns <B>%d</b>"), err);
	TESTEL(KErrNone == err, err);
	ESock::XConnectionQuerySet* outputQuerySet = NULL;

	//5.	Create an invalid query, requesting an ID value that doesn’t exist.
	INFO_PRINTF1(_L("5. Create an invalid query, requesting an ID value that doesn't exist."));
	ESock::XConnectionQuerySet* querySet = NULL;
	querySet = ESock::XConnectionQuerySet::NewL();
	CleanupStack::PushL( querySet );
	ESock::XUintQuery::NewL(KUnknownBadId, *querySet); // ignore returned value, it's safe as member of queryset
	ESock::CConnectionQuerySet* connectionQuerySet = NULL;
	connectionQuerySet = ESock::CConnectionQuerySet::NewL(querySet);
	CleanupStack::Pop( querySet );
	querySet = NULL;
	CleanupStack::PushL( connectionQuerySet );

	//6.	Attempt to request GetParameters(…) this should succeed, however
	//		the individual requests themselves should fail.
	INFO_PRINTF1(_L("6. Attempt to request GetParameters(...) this should succeed, however the individual requests themselves should fail."));
	err = connection.GetParameters( *connectionQuerySet );
	INFO_PRINTF2(_L("6. GetParameters returns %d"), err);
	TESTEL(KErrNone == err, err);
	ESock::XUintQuery* unitQuery = NULL;
	outputQuerySet = &connectionQuerySet->DataObject();
	unitQuery = static_cast<ESock::XUintQuery*>(outputQuerySet->FindQuery( KUnknownBadId ));
	INFO_PRINTF2(_L("6. finds bad entry @ %d"), reinterpret_cast<TUint32>(unitQuery));
	TBool foundEntry = (unitQuery!= NULL);
	INFO_PRINTF2(_L("6. found entry %d"), foundEntry);
	TESTEL(foundEntry, foundEntry);
	INFO_PRINTF2(_L("6. Error should be KErrNotFound(-1) and is %d"), unitQuery->Error());
	TESTEL(unitQuery->Error() == KErrNotFound, unitQuery->Error());

	INFO_PRINTF2(_L("6. query is answered %d"), unitQuery->IsQueryAnswered());
	TESTEL(!unitQuery->IsQueryAnswered(), unitQuery->IsQueryAnswered());

	//7.	Close the connections and the socket servers.
	INFO_PRINTF1(_L("7. Close the connections and the socket servers."));
	CleanupStack::PopAndDestroy( connectionQuerySet );
	connectionQuerySet = NULL;

	CleanupStack::PopAndDestroy();	// connection
	CleanupStack::PopAndDestroy();	// socketServ

	SetTestStepResult(EPass);
	return TestStepResult();
	}


// End of Test TE_RConnectionTest77

TE_RConnectionTest78::~TE_RConnectionTest78()
	{
	}

enum TVerdict TE_RConnectionTest78::doTestStepL(void)
/**
Check that the following works okay:
	•	Requesting good ID’s but with wrong data type should also result in an
		error.
*/
	{
	SetTestStepResult(EFail);
	const CommsDat::TMDBElementId KIapTableIdFieldId = KCDTIdIAPRecord | KCDTIdRecordTag;

	TInt err;

	//1.	Instantiate an RSocketServ objects.
	INFO_PRINTF1(_L("1. Instantiate an RSocketServ objects."));
	RConnection connection;
	RSocketServ socketServ;

	//2.	Open the socket servers.
	INFO_PRINTF1(_L("2. Open the socket servers."));
	err = socketServ.Connect();
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServ);

	//3.	Open a connection
	INFO_PRINTF1(_L("3. Open a connection."));
	err =  connection.Open(socketServ);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(connection);

	//4.	Start the RConnection
	INFO_PRINTF1(_L("4. Start the RConnection"));
	err = StartConnection(connection);
	TESTEL(KErrNone == err, err);
	ESock::XConnectionQuerySet* outputQuerySet = NULL;

	//5.	Create an invalid query, requesting a valid ID value, with an
	//		incorrect data type
	INFO_PRINTF1(_L("5. Create an invalid query, requesting a valid ID value, with an incorrect data type"));
	ESock::XConnectionQuerySet* querySet = NULL;
	querySet = ESock::XConnectionQuerySet::NewL();
	CleanupStack::PushL( querySet );
	ESock::XBoolQuery::NewL(KIapTableIdFieldId, *querySet); // ignore returned value, it's safe as member of queryset
	ESock::CConnectionQuerySet* connectionQuerySet = NULL;
	connectionQuerySet = ESock::CConnectionQuerySet::NewL(querySet);
	CleanupStack::Pop( querySet );
	querySet = NULL;
	CleanupStack::PushL( connectionQuerySet );

	//6.	the individual requests themselves should fail.
	INFO_PRINTF1(_L("6. Attempt to request GetParameters(..) this should succeed, however the individual requests themselves should fail."));
	err = connection.GetParameters( *connectionQuerySet );
	INFO_PRINTF2(_L("6.1 GetParameters returns %d"), err);
	TESTEL(KErrNone == err, err);
	ESock::XBoolQuery* booleanQuery = NULL;
	outputQuerySet = &connectionQuerySet->DataObject();
	booleanQuery = static_cast<ESock::XBoolQuery*>(outputQuerySet->FindQuery( KIapTableIdFieldId ));
	TBool foundEntry = (booleanQuery!= NULL);
	TESTEL(foundEntry, foundEntry);
	INFO_PRINTF3(_L("6.2 ESock::XBoolQuery::Error() returns %d (KErrArgument = %d was expected)"), booleanQuery->Error(), KErrArgument);
	TESTEL(booleanQuery->Error() == KErrArgument, booleanQuery->Error());
	TESTEL(!booleanQuery->IsQueryAnswered(), booleanQuery->IsQueryAnswered());

	//7.	Close the connections and the socket servers.
	INFO_PRINTF1(_L("7. Close the connections and the socket servers."));
	CleanupStack::PopAndDestroy( connectionQuerySet );
	connectionQuerySet = NULL;

	CleanupStack::PopAndDestroy();	// connection
	CleanupStack::PopAndDestroy();	// socketServ

	SetTestStepResult(EPass);
	return TestStepResult();
	}

// End of Test TE_RConnectionTest78

TE_RConnectionTest79::~TE_RConnectionTest79()
{
}

enum TVerdict TE_RConnectionTest79::doTestStepL(void)
	{
	RSocketServ    	socketServer;
	TRequestStatus	status;
	RConnection   	conn;
	RConnection   	mon;
	TInt          	ret;

	ret = OpenSocketServer(socketServer);
	TESTEL(ret == KErrNone, ret);
	CleanupClosePushL(socketServer);

	//Open Monitor
	ret = OpenConnection(mon, socketServer);
	TESTEL(ret == KErrNone, ret);
	CleanupClosePushL(mon);

	//Open Normal Connection
	ret = OpenConnection(conn, socketServer);
	TESTEL(ret == KErrNone, ret);
	CleanupClosePushL(conn);

	TRequestStatus progStatus;
	TNifProgressBuf progBuf;
	ProgressNotification(conn, progStatus, progBuf,  8300);
    	TCommDbConnPref pref;
	StartConnectionWithOverridesAsynchronous(conn,pref, iDefaultIap, status);
	User::WaitForRequest (progStatus);
	TESTEL(progBuf().iStage == 8300, progBuf().iStage);
	TESTEL(progBuf().iError == KErrNone, progBuf().iError);

	TUint count;
	ret= EnumerateConnections(conn, count);
	TESTEL(ret == KErrNone, ret);
	TESTEL(count == 1, 1);
	TConnectionInfoBuf info;
	ret= GetConnectionInfo(conn,1,info);
	TESTEL(ret == KErrNone, ret);
	ret = AttachMonitor(mon, info);
	TESTEL(ret == KErrNone, ret);
	ProgressNotification(mon, progStatus, progBuf,  8000);
	CleanupStack::Pop(&conn);
	conn.Close();
	//The Start should complete immediately
	User::WaitForRequest(status);
	TESTEL(status.Int() != KErrNone, status.Int());

	//The connection should now report that the link is going down
	User::WaitForRequest (progStatus);
	TESTEL(progStatus.Int() == KErrNone, progStatus.Int());
	CleanupStack::Pop(&mon);
	mon.Close();
	CleanupStack::Pop(&socketServer);
	socketServer.Close();

	return EPass;
	}

// End of Test TE_RConnectionTest79


TE_RConnectionTest80::~TE_RConnectionTest80()
{
}


enum TVerdict TE_RConnectionTest80::doTestStepL(void)
	{

	const CommsDat::TMDBElementId KIapTableIdFieldId = CommsDat::KCDTIdIAPRecord | CommsDat::KCDTIdRecordTag;
	const TInt realIapId = 15;
	const TInt tunnelIapId = 16;
	TInt ret;
	
	RConnection connection_agent, connection_tunnel;
	RSocketServ socketServ;
	RSubConnection subconn_agent, subconn_tunnel;
	
	ret = socketServ.Connect();
	TESTEL(KErrNone == ret, ret);
	CleanupClosePushL(socketServ);

	ret =  connection_agent.Open(socketServ);
	TESTEL(KErrNone == ret, ret);
	CleanupClosePushL(connection_agent);
	ret =  connection_tunnel.Open(socketServ);
	TESTEL(KErrNone == ret, ret);
	CleanupClosePushL(connection_tunnel);

	ret = StartConnectionWithOverrides(connection_agent, realIapId);
	TESTEL(KErrNone == ret, ret);
	ret = subconn_agent.Open(socketServ, RSubConnection::ECreateNew, connection_agent);
	TESTEL(KErrNone == ret, ret);
	CleanupClosePushL(subconn_agent);
	
	ret = StartConnectionWithOverrides(connection_tunnel, tunnelIapId);
	TESTEL(KErrNone == ret, ret);
	ret = subconn_tunnel.Open(socketServ, RSubConnection::ECreateNew, connection_tunnel);
	TESTEL(KErrNone == ret, ret);
	CleanupClosePushL(subconn_tunnel);


	//===============================================================================================
	// case 1 : Get Bottom IAP, expected to success
	ESock::XTunnelConnQuerySet* querySet_GetBottomIAP = NULL;
	querySet_GetBottomIAP = ESock::XTunnelConnQuerySet::NewL();
	CleanupStack::PushL( querySet_GetBottomIAP );

	ESock::XUintQuery::NewL(KIapTableIdFieldId, *querySet_GetBottomIAP); // ignore returned value, it's safe as member of queryset
	
	ESock::CTunnelConnQuerySet* tunnelConnQuerySet_GetBottomIAP = NULL;
	tunnelConnQuerySet_GetBottomIAP = ESock::CTunnelConnQuerySet::NewL(querySet_GetBottomIAP);
	CleanupStack::PushL(tunnelConnQuerySet_GetBottomIAP);
	ret = connection_tunnel.GetParameters( *tunnelConnQuerySet_GetBottomIAP );
	TESTEL(KErrNone == ret, ret);

	{
	ESock::XUintQuery* unitQuery = NULL;
	XTunnelConnQuerySet* outputQuerySet = &tunnelConnQuerySet_GetBottomIAP->DataObject();
	unitQuery = static_cast<ESock::XUintQuery*>(outputQuerySet->FindQuery( KIapTableIdFieldId ));
	TBool foundEntry = (unitQuery!= NULL);
	TESTEL(foundEntry, foundEntry);
	TESTEL(unitQuery->Error() == KErrNone, unitQuery->Error());
	TESTEL(unitQuery->IsQueryAnswered(), unitQuery->IsQueryAnswered());
	TESTEL(unitQuery->Data() == realIapId, unitQuery->Data());
	}

	CleanupStack::PopAndDestroy(tunnelConnQuerySet_GetBottomIAP);
	tunnelConnQuerySet_GetBottomIAP = NULL;
	CleanupStack::Pop(querySet_GetBottomIAP);
	querySet_GetBottomIAP = NULL;



	//===============================================================================================
	// case 2 : Get 1st layer IAP, expected to success
	ESock::XTunnelConnQuerySet* querySet_Get1stLayerIAP = NULL;
	querySet_Get1stLayerIAP = ESock::XTunnelConnQuerySet::NewL(1);
	CleanupStack::PushL( querySet_Get1stLayerIAP );

	ESock::XUintQuery::NewL(KIapTableIdFieldId, *querySet_Get1stLayerIAP); // ignore returned value, it's safe as member of queryset
	ESock::CTunnelConnQuerySet* tunnelConnQuerySet_Get1stLayerIAP = NULL;
	tunnelConnQuerySet_Get1stLayerIAP = ESock::CTunnelConnQuerySet::NewL(querySet_Get1stLayerIAP);
	CleanupStack::PushL(tunnelConnQuerySet_Get1stLayerIAP);
	ret = connection_tunnel.GetParameters( *tunnelConnQuerySet_Get1stLayerIAP );
	TESTEL(KErrNone == ret, ret);
	
	{
	ESock::XUintQuery* unitQuery = NULL;
	XTunnelConnQuerySet* outputQuerySet = &tunnelConnQuerySet_Get1stLayerIAP->DataObject();
	unitQuery = static_cast<ESock::XUintQuery*>(outputQuerySet->FindQuery( KIapTableIdFieldId ));
	TBool foundEntry = (unitQuery!= NULL);
	TESTEL(foundEntry, foundEntry);
	TESTEL(unitQuery->Error() == KErrNone, unitQuery->Error());
	TESTEL(unitQuery->IsQueryAnswered(), unitQuery->IsQueryAnswered());
	TESTEL(unitQuery->Data() == tunnelIapId, unitQuery->Data());
	}

	CleanupStack::PopAndDestroy(tunnelConnQuerySet_Get1stLayerIAP);
	tunnelConnQuerySet_Get1stLayerIAP = NULL;
	CleanupStack::Pop(querySet_Get1stLayerIAP);
	querySet_Get1stLayerIAP = NULL;


	//===============================================================================================	
	// case 3 : Get 2nd layer IAP, expected to success
	ESock::XTunnelConnQuerySet* querySet_Get2ndLayerIAP = NULL;
	querySet_Get2ndLayerIAP = ESock::XTunnelConnQuerySet::NewL(2);
	CleanupStack::PushL(querySet_Get2ndLayerIAP);

	ESock::XUintQuery::NewL(KIapTableIdFieldId, *querySet_Get2ndLayerIAP); // ignore returned value, it's safe as member of queryset
	ESock::CTunnelConnQuerySet* tunnelConnQuerySet_Get2ndLayerIAP = NULL;
	tunnelConnQuerySet_Get2ndLayerIAP = ESock::CTunnelConnQuerySet::NewL(querySet_Get2ndLayerIAP);
	CleanupStack::PushL(tunnelConnQuerySet_Get2ndLayerIAP);
	ret = connection_tunnel.GetParameters( *tunnelConnQuerySet_Get2ndLayerIAP );
	TESTEL(KErrNone == ret, ret);

	{
	ESock::XUintQuery* unitQuery = NULL;
	XTunnelConnQuerySet* outputQuerySet = &tunnelConnQuerySet_Get2ndLayerIAP->DataObject();
	unitQuery = static_cast<ESock::XUintQuery*>(outputQuerySet->FindQuery( KIapTableIdFieldId ));
	TBool foundEntry = (unitQuery!= NULL);
	TESTEL(foundEntry, foundEntry);
	TESTEL(unitQuery->Error() == KErrNone, unitQuery->Error());
	TESTEL(unitQuery->IsQueryAnswered(), unitQuery->IsQueryAnswered());
	TESTEL(unitQuery->Data() == realIapId, unitQuery->Data());
	}

	CleanupStack::PopAndDestroy(tunnelConnQuerySet_Get2ndLayerIAP);
	tunnelConnQuerySet_Get2ndLayerIAP = NULL;
	CleanupStack::Pop(querySet_Get2ndLayerIAP);
	querySet_Get2ndLayerIAP = NULL;



	//===============================================================================================
	// case 4 : Get 10th layer IAP, expected to fail because we have only 2 layers here
	ESock::XTunnelConnQuerySet* querySet_Get10thLayerIAP = NULL;
	querySet_Get10thLayerIAP = ESock::XTunnelConnQuerySet::NewL(10);
	CleanupStack::PushL(querySet_Get10thLayerIAP);

	ESock::XUintQuery::NewL(KIapTableIdFieldId, *querySet_Get10thLayerIAP); // ignore returned value, it's safe as member of queryset
	ESock::CTunnelConnQuerySet* tunnelConnQuerySet_Get10thLayerIAP = NULL;
	tunnelConnQuerySet_Get10thLayerIAP = ESock::CTunnelConnQuerySet::NewL(querySet_Get10thLayerIAP);
	CleanupStack::PushL(tunnelConnQuerySet_Get10thLayerIAP);
	ret = connection_tunnel.GetParameters( *tunnelConnQuerySet_Get10thLayerIAP );
	TESTEL(KErrNone == ret, ret);

	{
	ESock::XUintQuery* unitQuery = NULL;
	XTunnelConnQuerySet* outputQuerySet = &tunnelConnQuerySet_Get10thLayerIAP->DataObject();
	unitQuery = static_cast<ESock::XUintQuery*>(outputQuerySet->FindQuery( KIapTableIdFieldId ));
	TESTEL(unitQuery->Error() == KErrNone, unitQuery->Error());
	TESTEL(unitQuery->IsQueryAnswered() == EFalse, unitQuery->IsQueryAnswered());
	}

	CleanupStack::PopAndDestroy(tunnelConnQuerySet_Get10thLayerIAP);
	tunnelConnQuerySet_Get10thLayerIAP = NULL;
	CleanupStack::Pop(querySet_Get10thLayerIAP);
	querySet_Get10thLayerIAP = NULL;


	REComSession::FinalClose();


	subconn_tunnel.Close();
	CleanupStack::Pop();

	subconn_agent.Close();
	CleanupStack::Pop();

	connection_tunnel.Stop();
	connection_tunnel.Close();
	CleanupStack::Pop();

	connection_agent.Stop();
	connection_agent.Close();
	CleanupStack::Pop();

	socketServ.Close();
	CleanupStack::Pop();

	SetTestStepResult(EPass);
	return TestStepResult();
	}
// End of Test TE_RConnectionTest80


TE_RConnectionTest81::~TE_RConnectionTest81()
{
}

enum TVerdict TE_RConnectionTest81::doTestStepL(void)
/*
 * Test reporting of the interface coming down
 */
{
	TInt err;
	TRequestStatus status;

	RSocketServ ss;
	RConnection conn;
	RConnection conn_clone;
	TNifProgressBuf progress;
	TNifProgress progMain, progClone;
	TName connname;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	err = StartConnection(conn);
	TESTEL(KErrNone == err, err);
	
	_LIT_SECURITY_POLICY_C2(KProcPolicy1, ECapabilityNetworkServices, ECapabilityNetworkControl);
	TSecurityPolicyBuf secPolBuf;
	secPolBuf().Set(KProcPolicy1().Package());
	conn.Control(KCOLConnection, KCoEnableCloneOpen, secPolBuf);

	User::LeaveIfError(conn.Name(connname));
	User::LeaveIfError(conn_clone.Open(ss, connname));
	CleanupClosePushL(conn_clone);

	err = Progress(conn, progMain);
	TESTEL(KErrNone == err, err);
	err = Progress(conn_clone, progClone);
	TESTEL(KErrNone == err, err);

	TESTEL(progMain == progClone, progMain.iStage);

	ProgressNotification(conn_clone, status, progress, KLinkLayerClosed);

	err = RequestInterfaceDownL(conn, ss);
	TESTEL(KErrNone == err, err);

	User::WaitForRequest(status);
	TESTEL(status.Int() == KErrNone, status.Int());
	TESTEL(progress().iStage == KLinkLayerClosed, progress().iStage);

	CloseConnection(conn_clone);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionTest31

