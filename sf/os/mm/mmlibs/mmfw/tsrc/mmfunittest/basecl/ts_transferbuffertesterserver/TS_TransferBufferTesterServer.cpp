// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32svr.h>
#include <testframework.h>
#include "TS_TransferBufferTesterServer.h"


inline TTransferBufferTesterServerStart::TTransferBufferTesterServerStart()
	{}

inline CTBTServerShutdown::CTBTServerShutdown()
	:CTimer(-1)
	{CActiveScheduler::Add(this);}
inline void CTBTServerShutdown::ConstructL()
	{CTimer::ConstructL();}
inline void CTBTServerShutdown::Start()
	{After(KTBTServerShutdownDelay);}

/** 
 *Signal the owning thread that the server has started successfully
 *This may itself fail
 */
void TTransferBufferTesterServerStart::SignalL()
	{
	RThread starter;
	User::LeaveIfError(starter.Open(iId));
	starter.RequestComplete(iStatus,KErrNone);
	starter.Close();
	}


/**
 * Initiate server exit when the timer expires
 */
void CTBTServerShutdown::RunL()
	{
	CActiveScheduler::Stop();
	}



CMmfIpcServer* CTBTServer::NewL()
	{
	CTBTServer* s = new(ELeave) CTBTServer;
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

CTBTServer::CTBTServer()
	:CMmfIpcServer(0,ESharableSessions)
	{
	}

void CTBTServer::ConstructL()
	{
	StartL(KTransferBufferTesterServerName);
	RThread serverThread;
	serverThread.Rename(KTransferBufferTesterServerName);
	serverThread.Close();
	//Ensure that the server will exit even if the 1st client fails to connect
	iShutdown.ConstructL();
	iShutdown.Start();
	}

CTBTServer::~CTBTServer()
	{
	}

CMmfIpcSession* CTBTServer::NewSessionL(const TVersion &aVersion) const
	{
	TVersion v(KTransferBufferTesterServerMajorVersionNumber,
			   KTransferBufferTesterServerMinorVersionNumber,
			   KTransferBufferTesterServerBuildVersionNumber);
	if (!User::QueryVersionSupported(v,aVersion))
		User::Leave(KErrNotSupported);
	// make new session
	return new(ELeave) CTBTSession();
	}

void CTBTServer::AddSession()
	{
	++iSessionCount;
	iShutdown.Cancel();
	}

void CTBTServer::DropSession()
	{
	if (--iSessionCount <= 0)
		iShutdown.Start();
	}

TInt CTBTServer::RunError(TInt aError)
	{
	Message().Complete(aError);
	ReStart();
	return KErrNone;
	}



//
//CTBTSession
//

CTBTSession::CTBTSession()
	{
	}

void CTBTSession::CreateL(const CMmfIpcServer& aServer)
	{
	CMmfIpcSession::CreateL(aServer);	// does not leave
	//Add session to server first. If anything leaves, it will be removed by the destructor
	iServer = STATIC_CAST(CTBTServer*, (CONST_CAST(CMmfIpcServer*, &aServer)));
	iServer->AddSession();
	ConstructL();
	}

void CTBTSession::ConstructL()
	{
	}

CTBTSession::~CTBTSession()
	{
	iTransferWindow.Close();
	iTransferBuffer.Close();
#ifdef __MMF_USE_IPC_V2__
	iClient.Close();
#endif
	iServer->DropSession();
	}

void CTBTSession::ServiceL(const RMmfIpcMessage& aMessage)
	{
	switch (aMessage.Function())
		{
	case ECreateTransferBuffer:
		SetOwnCopyOfMessageL(aMessage);		
		CreateTransferBufferL(aMessage);
		break;
	case EMapBufferIntoClientWindowHandle:
		MapBufferIntoClientWindowHandleL(aMessage);
		break;
	case EMapOutBuffer:
		//removed - no longer needed
		break;
	case EGetThreadId:
		GetThreadIdL(aMessage);
		break;
	case EWriteTestData:
		 WriteTestDataL(aMessage);
		 break;
	default:
		break;
		}
	aMessage.Complete(KErrNone);
	}



void CTBTSession::SetOwnCopyOfMessageL(const RMmfIpcMessage& aMessage)
	{
	if (iCanCompleteMessage)
		User::Leave(KErrAlreadyExists);
	//else
	iMessage = aMessage;
	iCanCompleteMessage = ETrue;
	}

void CTBTSession::CompleteOwnCopyOfMessage(TInt aReason)
	{
	if (iCanCompleteMessage)
		iMessage.Complete(aReason);
	iCanCompleteMessage = EFalse;
	}

/**
 *
 * Server side procedure to create a RTransferWindow
 * and RTransferBuffer
 *
 */
void CTBTSession::CreateTransferBufferL(const RMmfIpcMessage &aMessage)
	{
	if (iTransferWindow.Handle()) User::Leave(KErrAlreadyExists);

	RThread serverThread;
	serverThread.Rename(KTransferBufferTesterServerName);
	serverThread.Close();

	User::LeaveIfError(iTransferWindow.Create(0x10000));
	User::LeaveIfError(iTransferBuffer.Create(aMessage.Int0()));
	}

/**
 *
 * Server side procedure to map the server side buffer into the client RTransferWindow 
 * client RTransferWindow specified by aClientWindowHandle
 *
 */
void CTBTSession::MapBufferIntoClientWindowHandleL(const RMmfIpcMessage& aMessage)
	{
	iClientTransferWindowCopy.SetHandle(aMessage.Int0());  //set to client window handle
#ifdef __MMF_USE_IPC_V2__
	User::LeaveIfError(aMessage.Client(iClient));
#else
	iClient = aMessage.Client();
#endif // __MMF_USE_IPC_V2__
	iClientTransferWindowCopy.Duplicate(iClient); //duplicate handle using client thread id
	iClientTransferWindowCopy.MapInBuffer(iTransferBuffer);
	iClientTransferWindowCopy.Close();
	}

void CTBTSession::GetThreadIdL(const RMmfIpcMessage& aMessage)
	{
	RThread thisThread;
	TPckgBuf<TInt> pckg;
	pckg() = thisThread.Id();
	MmfMessageUtil::WriteL(aMessage, 0, pckg);
	}

/**
 *
 * Server side procedure to populate
 * the server RTransferBuffer with KTestData
 *
 */
void CTBTSession::WriteTestDataL(const RMmfIpcMessage& aMessage)
	{
	if (iTransferWindow.Handle() == 0) User::Leave(KErrNotReady);
	
	iTransferWindow.MapInBuffer(iTransferBuffer);
	TPtrC8 clientDescriptor(KTestData);
	TPtr8 transferbuf((TUint8*)iTransferWindow.GetBufferAddress(), 0, 4096);
	transferbuf.Copy(clientDescriptor);
	iTransferWindow.MapOutBuffer();

	MmfMessageUtil::WriteL(aMessage, 0, clientDescriptor);
	}


/**
 * Perform all server initialisation, in particular creation of the
 * scheduler and server and then run the scheduler
 */
static void RunServerL(TTransferBufferTesterServerStart& aStart)

	{
	// create and install the active scheduler we need
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	//
	// create the server (leave it on the cleanup stack)
	CleanupStack::PushL(CTBTServer::NewL());
	//
	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(RThread().Rename(KTransferBufferTesterServerName));
	//
	// Initialisation complete, now signal the client
	aStart.SignalL();
	//
	// Ready to run
	CActiveScheduler::Start();
	//
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}


/**
 * Main entry-point for the server thread
 */
static TInt RunServer(TTransferBufferTesterServerStart& aStart)

	{
	__MM_HEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL(aStart));
		delete cleanup;
		}
	//
	__MM_HEAP_MARKEND;
	return r;
	}

/* 
The server binary is an "EPOCEXE" target type
Thus the server parameter passing and startup code for WINS and EPOC are
significantly different.
*/
#ifdef __WINS__
/*
In WINS, the EPOCEXE target is a DLL with an entry point called WinsMain,
taking no parameters and returning TInt. This is not really valid as a thread
function which takes a TAny* parameter which we need.

So the DLL entry-point WinsMain() is used to return a TInt representing the
real thread function within the DLL. This is good as long as
sizeof(TInt)>=sizeof(TThreadFunction).
*/

static TInt ThreadFunction(TAny* aParms)
/**
WINS thread entry-point function.
The TServerStart objects is passed as the thread parameter
**/
	{
	return RunServer(*static_cast<TTransferBufferTesterServerStart*>(aParms));
	}

IMPORT_C TInt WinsMain();
EXPORT_C TInt WinsMain()
/**
WINS DLL entry-point. Just return the real thread function 
cast to TInt
**/
	{
	return reinterpret_cast<TInt>(&ThreadFunction);
	}

TInt E32Dll(TDllReason)
	{
	return KErrNone;
	}

#else

TInt TTransferBufferTesterServerStart::GetCommand()
/**
In EPOC, the EPOCEXE target is a process, and the server startup
parameters are encoded in the command line
**/
	{
	RProcess p;
	if (p.CommandLineLength()!=sizeof(TTransferBufferTesterServerStart)/sizeof(TText))
		return KErrGeneral;
	TPtr ptr(reinterpret_cast<TText*>(this),0,sizeof(TTransferBufferTesterServerStart)/sizeof(TText));
	p.CommandLine(ptr);
	return KErrNone;
	}

TInt E32Main()
/**
Server process entry-point
Recover the startup parameters and run the server
**/
	{
	TTransferBufferTesterServerStart start;
	TInt r=start.GetCommand();
	if (r==KErrNone)
		r=RunServer(start);
	return r;
	}
#endif

