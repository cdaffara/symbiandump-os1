/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#include <e32base.h>
#include <e32property.h>
#include <es_sock.h>
#include <test/es_dummy.h>
#include <in_sock.h>
#include <test/testexecutestepbase.h>
#include <test/testexecuteserverbase.h>


class  CLoopbackDrvTestServer : public CTestServer
	{
public:
	CLoopbackDrvTestServer();
	~CLoopbackDrvTestServer();
	static CTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	
private:
	};

class CTestStepLoopback : public CTestStep
	{
public:
	CTestStepLoopback();
	~CTestStepLoopback();
	enum TVerdict doTestStepL();
	static const TDesC& GetTestName()
		{
		_LIT(KTestName, "tcploop");
		return KTestName();
		}
	//	CTestSuiteIPC * iEsockSuite;// Pointer to the suite which owns this test
	};




CTestServer* CLoopbackDrvTestServer::NewL()
/**
* @return - Instance of the test server
* Called inside the MainL() function to create and start the
* CTestServer derived server.
*/
	{
	CTestServer * server = new (ELeave) CLoopbackDrvTestServer();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// Server Logging.
	
	//server->StartL(KServerName); 
	_LIT(KServerName, "te_loopback");
	server-> ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
* Much simpler, uses the new Rendezvous() call to sync with the client
*/
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	
	CTestServer* server = NULL;
	// Create the CTestServer derived server
	
	TRAPD(err,server = CLoopbackDrvTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

// Only a DLL on emulator for typhoon and earlier

GLDEF_C TInt E32Main()
/**
* @return - Standard Epoc error code on exit
*/
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }

// Create a thread in the calling process
// Emulator typhoon and earlier

CLoopbackDrvTestServer::CLoopbackDrvTestServer()
	{
	}

CLoopbackDrvTestServer::~CLoopbackDrvTestServer()
	{
	}


CTestStep* CLoopbackDrvTestServer::CreateTestStep(const TDesC& aStepName)
/**
* @return - A CTestStep derived instance
* Implementation of CTestServer pure virtual
*/
	{
    if(aStepName.CompareF(CTestStepLoopback::GetTestName()) == 0)
		{
		CTestStepLoopback* ret = new(ELeave) CTestStepLoopback;
		ret->SetTestStepName(CTestStepLoopback::GetTestName());
		return ret;
		}
    return NULL;
	}




// constructor
CTestStepLoopback::CTestStepLoopback() 
	{
	}

// destructor
CTestStepLoopback::~CTestStepLoopback()
	{
	}

enum TVerdict CTestStepLoopback::doTestStepL()
	{
	SetTestStepResult(EFail);
	
//	const TUid KPktTxKey = {0x104045dd};
//	const TUid KPktRxKey = {0x104045de};
//	const TUid KMeUid = {0x101F529F};
//	TSecurityPolicy readPolicy(ECapability_None);
//	TSecurityPolicy writePolicy(ECapability_None);
//	TInt result = RProperty::Define(KMeUid, KPktTxKey .iUid, RProperty::EInt, readPolicy, writePolicy);
//	result = RProperty::Define(KMeUid, KPktRxKey .iUid, RProperty::EInt, readPolicy, writePolicy);
	
	
	RSocketServ ss;
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Start: creating + starting connection"));
	User::LeaveIfError(ss.Connect());
	RConnection conn;
	CleanupClosePushL(conn);
	User::LeaveIfError(conn.Open(ss));
	User::LeaveIfError(conn.Start());

	TInt srvPort;
	if(!GetIntFromConfig(_L("TcpLoop"), _L("Port"), srvPort))
		{
		srvPort = 5002;
		}
	Logger().WriteFormat(_L("Start: creating server socket listening on %d"), srvPort);
	RSocket srv;
	CleanupClosePushL(srv);
	User::LeaveIfError(srv.Open(ss, KAfInet, KSockStream, KProtocolInetTcp, conn));
	TInetAddr srvAddr(KAfInet);
	srvAddr.SetPort(srvPort);
	User::LeaveIfError(srv.Bind(srvAddr));
	User::LeaveIfError(srv.Listen(5));
	RSocket acc;
	CleanupClosePushL(acc);
	User::LeaveIfError(acc.Open(ss));
	TRequestStatus accStat;
	srv.Accept(acc, accStat);
	
	Logger().WriteFormat(_L("Start: connecting client socket"));
	RSocket cli;
	CleanupClosePushL(cli);
	User::LeaveIfError(cli.Open(ss, KAfInet, KSockStream, KProtocolInetTcp, conn));
	srvAddr.SetAddress(0xC0A80707);
	TRequestStatus cliStat;
	cli.Connect(srvAddr, cliStat);
	User::WaitForRequest(cliStat, accStat);
	User::WaitForRequest(cliStat, accStat);
	User::LeaveIfError(cliStat.Int());
	User::LeaveIfError(accStat.Int());

	//_LIT8(KTest, "jackdaws love my big sphinx of quartz");
	
	TInt xferSize = 0;
	if(!GetIntFromConfig(_L("TcpLoop"), _L("xferSize"), xferSize))
		{
		xferSize = 1 * 1000 * 1000;
		}
	TInt fc = User::FastCounter();
	TInt txSize = 0;
	TInt txCnt = 0;
	TRequestStatus txStat(KErrNone);
	TBuf8<4096> txBuf;
	txBuf.SetMax();
	TInt rxSize = 0;
	TInt rxCnt = -1;
	TRequestStatus rxStat(KErrNone);
	TBuf8<4096> rxBuf;
	
	Logger().WriteFormat(_L("Transferring %d bytes"), xferSize);
	while(rxSize < xferSize)
		{
		fc = User::FastCounter();
		if(txStat.Int() != KRequestPending)
			{
			RDebug::Printf("tx status:%d, ", txStat.Int());
			cli.Send(txBuf, 0, txStat);
			++txCnt;
			txSize += txBuf.Length();
			RDebug::Printf("tx #%d, %d, +%d\n", txCnt, fc, txBuf.Length());
			}
		if(rxStat.Int() != KRequestPending)
			{
			RDebug::Printf("rx status:%d, ", rxStat.Int());
			++rxCnt;
			rxSize += rxBuf.Length();
			RDebug::Printf("rx #%d, %d, +%d\n", rxCnt, fc, rxBuf.Length());
			TSockXfrLength dummy;
			acc.RecvOneOrMore(rxBuf, 0, rxStat, dummy);
			}
		User::WaitForRequest(rxStat, txStat);
		}
	
	Logger().WriteFormat(_L("Transferred; %d writes, %d reads"), txCnt, rxCnt);
	
//	RProperty::Get(KUidSystemCategory, KPktTxKey .iUid, txCnt);
//	RProperty::Define(KUidSystemCategory, KPktRxKey .iUid, rxCnt);
//	Logger().WriteFormat(_L("Packet counts; %d sends, %d processes"), txCnt, rxCnt);
	
	
	CleanupStack::PopAndDestroy(5, &ss);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	};

