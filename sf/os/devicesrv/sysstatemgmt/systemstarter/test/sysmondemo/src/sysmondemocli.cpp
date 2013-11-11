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
#include "e32cons.h"
#include "e32cmn.h"
#include "bacline.h"
#include "restartsys.h"

_LIT(KPOKE, "poke") ;
_LIT(KKILL, "kill") ;
_LIT(KDONE, "done") ;
_LIT(KRESTART, "restart") ;


static TInt StartServer()
//
// Start the server process. Simultaneous launching
// of two such processes should be detected when the second one attempts to
// create the server object, failing with KErrAlreadyExists.
//
	{
	/* we shall not load the server if it is not there*/
	return -1 ;	
	}


TInt RDemoCli::Open()
{
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KDemoServerName,TVersion(0,0,0),1);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
}

void RDemoCli::Close()
{
	RSessionBase::Close() ;	
}

void RDemoCli::Poke(TTime &time)
{	
//	TPckgBuf<TTime*> buf(&time) ;	
//	TIpcArgs ipcArgs(&buf) ;	

	TPckg<TTime> buf(time) ;
	TIpcArgs ipcArgs(&buf) ;	
	
	SendReceive(EServerPoke,ipcArgs) ;			
}

void RDemoCli::Kill(TTime &time)
{
//	TPckgBuf<TTime*> buf(&time) ;	
//	TIpcArgs ipcArgs(&buf) ;	

	TPckg<TTime> buf(time) ;
	TIpcArgs ipcArgs(&buf) ;	
	
	SendReceive(EServerKill,ipcArgs) ;	
}

void RDemoCli::Done(TTime &time)
{
//	TPckgBuf<TTime*> buf(&time) ;	
//	TIpcArgs ipcArgs(&buf) ;	

	TPckg<TTime> buf(time) ;
	TIpcArgs ipcArgs(&buf) ;	
	
	SendReceive(EServerDone,ipcArgs) ;	
}

CConsoleBase *gConsole ;

void TestDemoL()
{
	RDemoCli demoCli ;
	
	TTime time ;	
	gConsole = Console::NewL(_L("Demo"), TSize(KConsFullScreen, KConsFullScreen)) ;	
	
	CleanupStack::PushL(gConsole) ;
	
	CCommandLineArguments* args = CCommandLineArguments::NewL() ;	
	CleanupStack::PushL(args) ;
	
	TPtrC command ;
	command.Set(KNullDesC) ;
	
	if (args->Count() > 1)
	{
		command.Set(args->Arg(1))  ;	
	}
	else
	{
		gConsole->Printf(_L("Usage: sysmondemocli [poke|kill|done|restart <mode>]\n")) ;
	}

	if (command.Compare(KRESTART) == 0)
	{
	
		TBuf<10> modeBuf = args->Arg(2) ;
		
		TPtr8 data = modeBuf.Collapse() ;
		
		TLex8 txt8 = TLex8(const_cast<TUint8*> (data.PtrZ())) ;
		
		TInt mode ;
		txt8.Val(mode) ;
		
		TInt err = RestartSys::RestartSystem(mode) ; /* restart in a different mode */		
		
		User::Leave(err) ;
	}	
	
		
	if (demoCli.Open() != KErrNone) 
	{
		gConsole->Printf(_L("The SysMonDemo server is not there - bye\n")) ;

		gConsole->Printf(_L("Press any key to continue...\n")) ;	
		gConsole->Getch() ;
				
		User::Leave(KErrNone) ;
	}	
	
	CleanupClosePushL(demoCli) ;
	
	if (command.Compare(KPOKE) == 0)
	{		
		demoCli.Poke(time) ;
		gConsole->Printf(_L("The SysMonDemo server was last loaded at (local time):\n")) ;
		TDateTime dt = time.DateTime() ;
		gConsole->Printf(_L("Year %d Month %d Day %d Hour %d Min %d Sec %d\n"), dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second()) ;		
	}
	else if (command.Compare(KKILL) == 0)
	{
		demoCli.Kill(time) ;
		gConsole->Printf(_L("The SysMonDemo server has been killed.\n")) ;
		gConsole->Printf(_L("The time of death:\n")) ;
		TDateTime dt = time.DateTime() ;
		gConsole->Printf(_L("Year %d Month %d Day %d Hour %d Min %d Sec %d\n"), dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second()) ;		

		gConsole->Printf(_L("SysMon is going to reload SysMonDemo server soon.\n")) ;
	}
	else if (command.Compare(KDONE) == 0)
	{
		demoCli.Done(time) ;
		gConsole->Printf(_L("Monitoring has been cancelled for SysMonDemo\n")) ;
		gConsole->Printf(_L("The SysMonDemo server has been killed. But it won't be reloaded\n")) ;
		gConsole->Printf(_L("The time of death:\n")) ;
		TDateTime dt = time.DateTime() ;
		gConsole->Printf(_L("Year %d Month %d Day %d Hour %d Min %d Sec %d\n"), dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second()) ;				
	}
	
	
	// User::After(10000000) ; // let the console stay for a while
	
	gConsole->Printf(_L("Press any key to continue...\n")) ;
	gConsole->Getch() ;
	
	
	CleanupStack::PopAndDestroy(&demoCli) ;
	CleanupStack::PopAndDestroy(args) ;
	CleanupStack::PopAndDestroy(gConsole) ;	
	
	
}



TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	
	TInt err = KErrNone ;
	
//	for(;;)
	{
		TRAP(err, TestDemoL());		
				
	}
	
	
	delete cleanup;
	
	__UHEAP_MARKEND;

	return (err);
	}


