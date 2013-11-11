// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "sysmondemo.h"
#include "sysmonclisess.h"
#include "dscstore.h"
#include "dscitem.h"



_LIT(KHelloWorld, "Z:\\sys\\bin\\helloworld.exe") ;
/************************************************************************************/

CDemoSSess* CDemoSSess::NewL()
{
	CDemoSSess* sess = new(ELeave) CDemoSSess ;
	return sess ;
}

CDemoSSess::~CDemoSSess()
{
	
}

void CDemoSSess::ServiceL(const RMessage2 &aMessage)
{
	iMessage = const_cast<RMessage2&> (aMessage) ;
	
	switch (iMessage.Function())
	{
		case EServerPoke:
			CmdPokeL() ;
			break ;
		
		case EServerKill:
			CmdKillL() ;
			break ;
		
		case EServerDone:
			CmdDoneL() ;
			break ;			
		default:
			break ;
				
	}	
	
	iMessage.Complete(KErrNone) ;	
}


void CDemoSSess::CmdPokeL()
{

	TPckg<TTime> package(DemoServer()->GetLoadTime()) ;		
	iMessage.WriteL(0, package) ;
}
		
void CDemoSSess::CmdKillL()
{
	TTime time1 ;
	time1.HomeTime() ; /* returning the time of death */
	

	TPckg<TTime> package(time1) ;		
	iMessage.WriteL(0, package) ;
	
	CActiveScheduler::Stop() ;	
}
		
void CDemoSSess::CmdDoneL()
{
	TTime time1 ;
	time1.HomeTime() ; /* returning the time of death */
	
	TPckg<TTime> package(time1) ;		
	iMessage.WriteL(0, package) ;

	/* connect to monitor and cancel monitoring */
	
	RSysMonSession sysMon ;
	
	sysMon.OpenL() ;
	
	sysMon.CancelMonitorSelfL() ;
	
	sysMon.Close() ;
	
	CActiveScheduler::Stop() ;	// going to kill this server also
	
}



CDemoServ* CDemoSSess::DemoServer()
{
	return static_cast<CDemoServ*> (const_cast<CServer2*> (Server())) ;
}



/************************************************************************************/
/**************************************************************************************/

CDemoServ* CDemoServ::NewLC()
{
	CDemoServ* serv = new(ELeave) CDemoServ() ;
	CleanupStack::PushL(serv) ;
	
	
	serv->StartL(KDemoServerName) ;	
	
	return serv ;	
}

CDemoServ::CDemoServ():CServer2(EPriorityHigh)
{
	iLoadTime.HomeTime() ;
}

CDemoServ::~CDemoServ()
{
	
}
		

TTime CDemoServ::GetLoadTime()
{
	return iLoadTime ;
}


CSession2* CDemoServ::NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
{
	return CDemoSSess::NewL() ;	
}


/**************************************************************************************/

static void AddToDscL()
{
	RDscStore store ;
	
	store.OpenL() ;
	CleanupClosePushL(store) ;
	
	TInt err ;
	
	TRAP(err, store.DeleteDscL(TUid::Uid(KDefaultSymbianDsc))) ;
		
	TRAP(err, store.CreateDscL()) ;
	
	
	CDscItem *item = CDscItem::NewL() ;
	CleanupStack::PushL(item) ;
	
	item->SetFileParamsL(KHelloWorld, KNullDesC) ;
	item->SetMonitored(ETrue) ;
	
		
	TRAP(err, store.AddItemL(*item)) ;
	
	
	if (err != KErrNone)
		{
		store.DeleteItemL(*item) ;
		store.AddItemL(*item) ;
		}
	
	CleanupStack::PopAndDestroy(item) ;
	CleanupStack::PopAndDestroy(&store) ;
	
}


static void RunServerL()
//
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
	{
	// naming the server thread after the server helps to debug panics

	User::LeaveIfError(RThread::RenameMe(KDemoServerName))  ;
	
	AddToDscL() ;
		
	//
	// create and install the active scheduler we need
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	//
	// create the server (leave it on the cleanup stack)
	CDemoServ::NewLC();
	//
	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);
	//
	// Ready to run
	CActiveScheduler::Start();
	//
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

TInt E32Main()
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
		TRAP(r,RunServerL());
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	return r;
	}
