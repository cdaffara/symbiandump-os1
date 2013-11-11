// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// THostRes.CPP
// BT Host Resolver test code
// 
//

#include "tcsy.h"
#include <in_sock.h>
#include <e32std.h>
#include <bluetooth/logger.h>

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else  // __GCC32__
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_COMM_TCSY);
#endif

#ifdef _DEBUG
PANICCATEGORY("btcommtcsy");
#endif

//GLDEF_D RTest test(_L("CSY open/close tests"));

GLDEF_D TSize gSmallConsole(30,10);
GLDEF_D TInt gPanics=0;
GLDEF_D TInt gServerReboots=0;

// define/declare statics
TInt CCSY::iCommInstance = 0;

void LoadLDD_PDD()
	{
	RTest test(_L("LoadLDD_PDD"));
	TInt r;

	r=StartC32();
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else
		test.Printf(_L("Started C32\n"));

	test.Printf(_L("Loading PDD\n"));
	r=User::LoadPhysicalDevice(PDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else 
		test.Printf(_L("Loaded LDD\n"));
	test.Printf(_L("Loading LDD\n"));
	r=User::LoadLogicalDevice(LDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else
		test.Printf(_L("Loaded PDD\n"));
	test.Close();
	}

// ----------------------------------------------------------------------------
//							CSY App
// ----------------------------------------------------------------------------

CCSYApp::CCSYApp(CConsoleBase& aConsole)
: iConsole(aConsole)
	{
	}

CCSYApp::~CCSYApp()
	{
	DeleteAllConnections();
	iConnections.Close();
	iCommServer.UnloadCommModule(_L("BTCOMM"));
	iCommServer.Close();
	delete iJimmier;
	delete iActiveRootConsole;
	}

CCSYApp* CCSYApp::NewL(CConsoleBase& aConsole, TBool aAutoAuto)
	{
	CCSYApp* thisapp = new (ELeave) CCSYApp(aConsole);
	CleanupStack::PushL(thisapp);
	thisapp->ConstructL(aConsole, aAutoAuto);
	CleanupStack::Pop();
	return thisapp;
	}

void CCSYApp::ConstructL(CConsoleBase& aConsole, TBool aAutoAuto)
	{
	iActiveRootConsole = CActiveRootConsole::NewL(aConsole, *this);
	iJimmier = CAutoJimmier::NewL(aConsole, *this);
	User::LeaveIfError(iCommServer.Connect());

	if (!aAutoAuto)
		AutoMode();
	}

void CCSYApp::StartL()
	{
	// go ....
	CActiveScheduler::Start();
	}

void CCSYApp::Stop()
	{
	CActiveScheduler::Stop();
	}

void CCSYApp::NewCSYL(TBool aStartReading)
	{
	CCSY* csy=NULL;

	do
		{
		TRAPD(err, csy = CCSY::NewL(iConsole, *this, iCommServer));

		if (err == KErrServerTerminated)
			{
			//restart c32...not sure why it terminated tho...
			iConsole.Printf(_L("Restarting c32...\n"));
			++gServerReboots;
			iCommServer.Close();
			iCommServer.Connect();
			}
		else if (err != KErrNone)
			{
			User::Leave(err);
			}
		} while (csy == NULL);

	iConnections.Append(csy);
	if (aStartReading)
		csy->Read();
	}

void CCSYApp::SingleCSYL()
	{
	if (iConnections.Count() >= 1)
		{
		iConsole.Printf(_L("RComm instance already open (on session %d)\n"), gServerReboots);
		return;
		}
	NewCSYL(ETrue);
	}

void CCSYApp::DeleteAllConnections()
	{
	iConnections.ResetAndDestroy();
	}

void CCSYApp::DeleteConnection(const CCSY* aComm)
	{
	TInt index = iConnections.Find(aComm);
	ASSERT_DEBUG(index != KErrNotFound);

	delete iConnections[index];
	iConnections.Remove(index);
	}

void CCSYApp::AutoMode()
	{
	iJimmier->StartTimer();
	}

void CCSYApp::ReStartAutoMode()
	{
	DeleteAllConnections();
	AutoMode();
	}

// Playpen...

void CCSYApp::FetchWebPageL()
	{
	typedef TBuf8<4096> TBuffer;

	RSocketServ socketServer;
	User::LeaveIfError(socketServer.Connect());

	RSocket tcpSocket;
	User::LeaveIfError(tcpSocket.Open(socketServer, _L("TCPIP6")));

	// address
	TInetAddr remoteServer;

	const TUint32 KRemoteServer = INET_ADDR(194,129,2,54);
	remoteServer.SetAddress(KRemoteServer);

//	const TUint KHttpPort = 80;

	TRequestStatus status;

	iConsole.Printf(_L("Playpen = making TCP connection..."));
	tcpSocket.Connect(remoteServer, status);

	
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());	
//	
	
	iConsole.Printf(_L("Connect OK, Send HTML request\r\n"));

	TBuffer* wtemp=new TBuffer;
	TBuffer& wbuf=*wtemp;

	TBuffer* rtemp=new TBuffer;
	TBuffer& rbuf=*rtemp;

	wtemp->Zero();
	
	// Build an HTML request 
	wtemp->Copy(_L("GET "));
	const TBufC8<40> url(_L8("index.html"));
	wtemp->Append(url);
	wtemp->Append(_L(" \r\n\r\n"));
	//Send the request
	tcpSocket.Write(wbuf, status);
	User::WaitForRequest(status);
	if(status!=KErrNone)
		{
		iConsole.Printf(_L("Send Failed %d\n"), status.Int());
		tcpSocket.Close();
		delete wtemp;
		delete rtemp;
		return;
		}

	iConsole.Printf(_L("Send OK, Recv HTML page \r\n"));

	iConsole.Printf(_L("\r\nUrl: "));
	iConsole.Printf(_L("%S"), &url);
	iConsole.Printf(_L("\r\n"));

	// Send Okay, Recv HTML page
	// Get IP Address 
	
	// Wait for page
	TSockXfrLength HTTPHeaderLen;
	TInt rcount = 0;
	FOREVER
		{
		rtemp->Zero();
		tcpSocket.RecvOneOrMore(rbuf,0,status, HTTPHeaderLen);
		User::WaitForRequest(status);
		if(status!=KErrNone)
			break;
		rcount += rbuf.Length();
		iConsole.SetPos(0);
		iConsole.Printf(_L("Recved:%5dBytes"), rcount);
		}
	tcpSocket.Close();
	if(status!=KErrEof)
		{
		// Read Failed
		// Read Error Code
		// Error Message Convert TInt to TBuf
		iConsole.Printf(_L("Read Failed %d\n"), status.Int());
		}
	else
		{
		// Read Okay
		iConsole.Printf(_L(" - Recv HTML page, OK - Test Finished\r\n"));
		}
	
	delete wtemp;
	delete rtemp;
	}
	
void CCSYApp::C32UHeapMark()
	{
	iCommServer.__DbgMarkHeap();
	}

void CCSYApp::C32UHeapMarkEnd()
	{
	iCommServer.__DbgMarkEnd(0);
	}

void CCSYApp::StartExclusiveSessionL()
	{
	User::LeaveIfError(iExclusiveSession.Connect());
	TRequestStatus status;
	iExclusiveSession.SetExclusiveMode(status);
	User::WaitForRequest(status);
	}

void CCSYApp::StopExclusiveSession()
	{
	iExclusiveSession.ClearExclusiveMode();
	iExclusiveSession.Close();
	}

// ----------------------------------------------------------------------------
//							Auto Jimmier
// ----------------------------------------------------------------------------

CAutoJimmier* CAutoJimmier::NewL(CConsoleBase& aConsole, CCSYApp& aApp)
	{
	CAutoJimmier* c = new (ELeave) CAutoJimmier(aConsole, aApp);
	CleanupStack::PushL(c);
	c->ConstructL();
	CleanupStack::Pop(c);
	return c;
	}

void CAutoJimmier::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

CAutoJimmier::CAutoJimmier(CConsoleBase& aConsole, CCSYApp& aApp)
: CTimer(EPriorityStandard), iConsole(aConsole), iApp(aApp)
	{
	}

void CAutoJimmier::StartTimer()
	{
	if (IsActive())
		return;

	TInt time;
	if (iRemove)
		time = KAutoRemoveTime;
	else
		time = KAutoStartTime;

	iConsole.Printf(_L("Queing timer of %d musec\n"), time);
	After(time);
	}

void CAutoJimmier::RunL()
	{
//	if (Math::Rand(iSeed) < KMaxTInt/10) //random version
	if (iRemove)
		{
		iConsole.Printf(_L("Auto removing all RComm...\n"));
		iApp.C32UHeapMarkEnd();
		iApp.DeleteAllConnections();
		}
	else
		{
		iConsole.Printf(_L("Auto starting RComm...\n"));
		iApp.C32UHeapMark();
//		iApp.NewCSYL(ETrue);	// for multiple service/device testing
		iApp.SingleCSYL();
		}

	iRemove=!iRemove;
	StartTimer();
	}

TInt CAutoJimmier::RunError(TInt aError)
	{
	if (aError == KErrNoMemory)
		{
		iConsole.Printf(_L("****Hey! Out of memory!***\nAuto-restarting...\n"));
		iApp.ReStartAutoMode();
//		CActiveScheduler::Stop();
		}

	return KErrNone;
	}

// ----------------------------------------------------------------------------
//							CSY Connection
// ----------------------------------------------------------------------------

CCSY::CCSY(CConsoleBase& aConsole, CCSYApp& aApp, RCommServ& aCommServer)
: CActive(EPriorityStandard), iRootConsole(aConsole), iApp(aApp), iCommServer(aCommServer)
	{
	CActiveScheduler::Add(this);
	++iCommInstance;
	}

CCSY::~CCSY()
	{
	Cancel();

	if (iComm.SubSessionHandle())
		iComm.Close();

	delete iActiveCommConsole;
	delete iCommConsole;

	--iCommInstance; //funny, but static!
	}

CCSY* CCSY::NewL(CConsoleBase& aConsole, CCSYApp& aApp, RCommServ& aCommServer)
	{
	CCSY* thisconnection = new (ELeave) CCSY(aConsole, aApp, aCommServer);
	CleanupStack::PushL(thisconnection);
	thisconnection->ConstructL();
	CleanupStack::Pop();
	return thisconnection;
	}

void CCSY::ConstructL()
	{
	TBuf<8> title(_L("RComm"));
	TInt err;
	title.AppendNum(iCommInstance);
	iCommConsole = AutoSizeNewL(title, gSmallConsole);
	iActiveCommConsole = CActiveCommConsole::NewL(*iCommConsole, *this);
	iRootConsole.Printf(_L("Loading BTCOMM module\n"));
	err = iCommServer.LoadCommModule(_L("BTCOMM"));
	if (err != KErrNone)
		{
		iRootConsole.Printf(_L("ERROR %d opening BTCOMM...restarting c32\n"), err);
		//not sure why this happens yet!
//		User::After(10000000);
		User::Leave(err);
		}

	iRootConsole.Printf(_L("Open BTCOMM serial port in exclusive mode\r\n"));

	TBuf<10> commname(_L("BTCOMM::"));
//	commname.AppendNum(iCommInstance-1);
	commname.AppendNum(0);

	User::LeaveIfError(iComm.Open(iCommServer,commname,ECommExclusive));
	}

CConsoleBase* CCSY::AutoSizeNewL(const TDesC& aTitle, TSize aSize)
	{
	// Try to create a console of our preferred size, otherwise we
	// guess the screen is too small and use its entirety.
	CConsoleBase* console = NULL;
	TRAPD(err, console = Console::NewL(aTitle, aSize));
	if (err != KErrNone)
		{
		// If we leave now it is not because of offscreen drawing.
		console = Console::NewL(aTitle, TSize(KConsFullScreen, KConsFullScreen));
		}
	return console;
	}

void CCSY::Read()
	{
	TInt err = KErrNone;
	if(!iComm.SubSessionHandle())
		{
		TBuf<10> commname(_L("BTCOMM::"));
		commname.AppendNum(0);

		err = iComm.Open(iCommServer,commname,ECommExclusive);
		}
	if(!err)
		{	
		iCommConsole->Printf(_L("Read request issued\n"));
		iComm.ReadOneOrMore(iStatus, iBuffer);
		SetActive();
		}
	}
	
void CCSY::Close()
	{
	iCommConsole->Printf(_L("Close\n"));
	iComm.Close();
	}

void CCSY::RunL()
	{
	switch (iCommState)
		{
		case EReading:
			{
			iCommConsole->Printf(_L("Read complete with %d\n"), iStatus.Int());
			TBuf<40> readable;
			readable.Copy(iBuffer);
			iCommConsole->Printf(_L("Read = %S\n"), &readable);
			break;
			}
		case EWriting:
			iCommConsole->Printf(_L("Write complete with %d\n"), iStatus.Int());
			break;
		default:
			__DEBUGGER();
		}
	}

TInt CCSY::RunError(TInt /*aError*/)
	{
	__DEBUGGER();
	return KErrNone;
	}

void CCSY::DoCancel()
	{
	iCommConsole->Printf(_L("Cancelled.\n"));
	iComm.ReadCancel();
	iComm.WriteCancel();
	}

void CCSY::Destroy()
	{
	iApp.DeleteConnection(this);
	}



TInt StartCCSYApp()	
	{
	RTest test(_L("CSY open/close tests"));
	test.SetLogged(EFalse);
	test.Title();

	CCSYApp* app = CCSYApp::NewL(*test.Console());
	app->StartL();

	delete app;
	return KErrNone;
	}

TInt StartTestThread(TAny*)
	{
	CTrapCleanup* cleanupStack=CTrapCleanup::New();	// Get CleanupStack
	CActiveScheduler* activescheduler=new CActiveScheduler;
	CActiveScheduler::Install(activescheduler);

	TRAPD(err, StartCCSYApp());
	delete cleanupStack;
	return err;
	}
	
void StartL()
	{
	TBool again = ETrue;
	RTest test(_L("Test thread"));
	do
		{
		RThread testThread;
		TThreadFunction testThreadFunction(StartTestThread);
		TInt err=KErrNone;
		TRequestStatus stat;
		_LIT(KTestThread, "TCSY Test Thread");
		err=testThread.Create(KTestThread,testThreadFunction,KDefaultStackSize,KMinHeapSize,1024*1000,NULL, EOwnerThread);
		test.Printf(_L("Test thread create %d\n"), err);

		(void)(err != KErrNone); // keep the compiler happy by taking err as an r-value in urel

		testThread.Logon(stat);

		//User::SetJustInTime(EFalse);
		testThread.Resume();
		User::WaitForRequest(stat);
		User::SetJustInTime(ETrue);
		
// the test thread gets Killed on exit, but might panic due to BTCOMM ;-)
		if (testThread.ExitType()==EExitKill)
			{
			again = EFalse;
			}
		else
			{
			// we don't mind about the panics at present, but we do want to get an Access Violation
			TInt reason = testThread.ExitReason();
			TExitCategoryName category = testThread.ExitCategory();

			test.Printf(_L("Test thread Panic %d: StopCategory %S, StopReason 0x%04x\n"), ++gPanics, &category, reason); 
//			test.Getch();
			User::LeaveIfError(reason); // if EPOC error, lets give up; otherwise restart
			}

		testThread.Close();
		} while (again);
	test.Close();
	}


TInt E32Main()
	{
//	TInt ret;

// For some reason, you have to do the following to
// ensure that the file server behaves properly.
	RFs fs;
	(void)fs.Connect();
	if(fs.Handle())
		{
		fs.Close();
		}
	
	LoadLDD_PDD();

	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();	// Get CleanupStack

	CActiveScheduler* activescheduler=new CActiveScheduler;
	CActiveScheduler::Install(activescheduler);

	TRAPD(err, StartL());

	delete activescheduler;
	delete cleanupStack;
	__UHEAP_MARKEND;

	return err;
	}
