// btserialserver.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "btserialserver.h"
#include "btserialsession.h"
#include <fshell/btincomingserial.h>

static void RunServerL(RServer2* aServerPtr)
//
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
	{
	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(RThread::RenameMe(KBtSerialServerThreadName));
	//
	// create and install the active scheduler we need
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	//
	// create the server (leave it on the cleanup stack)
	CServer2* server = CBtSerialServer::NewLC();
	*aServerPtr = server->Server();
	//
	// Initialisation complete, now signal the client
	RThread::Rendezvous(KErrNone);
	//
	// Ready to run
	CActiveScheduler::Start();
	//
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

TInt ServerThread(TAny* aRServerPtr)
//
// Server process entry-point
//
	{
	__UHEAP_MARK;
	//
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		RServer2* serverPtr = static_cast<RServer2*>(aRServerPtr);
		
		TRAP(r,RunServerL(serverPtr));
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	return r;
	}

//______________________________________________________________________________
//						CBtSerialServer
CServer2* CBtSerialServer::NewLC()
	{
	CBtSerialServer* self=new(ELeave) CBtSerialServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
inline CBtSerialServer::CBtSerialServer()
	:CServer2(0,EUnsharableSessions)
	{
	}



void CBtSerialServer::ConstructL()
	{
	StartL(KNullDesC);
	}


CSession2* CBtSerialServer::NewSessionL(const TVersion&,const RMessage2&) const
	{
	if (iSession)
		{
		User::Leave(KErrAlreadyExists);
		}
	const_cast<CBtSerialServer*>(this)->iSession = new(ELeave) CBtSerialSession();
	return iSession;
	}

void CBtSerialServer::DropSession()
	{
	iSession = NULL;
	CActiveScheduler::Stop();
	}

