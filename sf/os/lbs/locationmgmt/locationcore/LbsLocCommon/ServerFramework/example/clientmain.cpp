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
	
	
#include "e32base.h"
#include "clientside.h"
#include "csecureprocessasbase.h"
#include "claunchedprocess.h"
#include "e32debug.h"
// Lbs
#include "lbsdevloggermacros.h"
   
void DoExampleL(CExClient* aCli)
	{
	LBSLOG(ELogP1,"->DoExampleL");
	aCli->ServerConnectCloseL();
	//User::After(2*2000000);
	aCli->SessionOpenCloseL();
	aCli->SessionMultiConnectCloseL();
	aCli->SessionMultiOpenCloseL();
	aCli->SessionMutiConnectOpenCloseL();
//	User::After(2*2000000);
	aCli->GetTimeSync();
//	aCli->GetTimeAsync();
	aCli->InvalidMessage();
	
	aCli->GetNames();
	
	aCli->ConstructQuietusL();
	aCli->CoupDeGrace();
	
	
	LBSLOG(ELogP1,"<-DoExampleL");
	} 


TInt E32Main()
	{
	/*
	God & Lorna need to create process like this:
	
	RProcess p;
	_LIT(KProcessName, "process.exe");
	CreateCommandLine
	TPtr commandline(CreateCommandLine(ERendezvousRequired));
	p.Create(KProcessName, commandLine);
	
	in the process launch code we need to figure out what the command buffer was
	and complete the rendezvous if required
	*/
	// this process has been kicked off by another process!
	// match what teh server side code does and process the command line here!
	// We haev to do this cos the TProcessStartParams tyep can be redfiend/etended by the
	TInt r = KErrNone;
	// do we need to process any startline params?
	if(User::CommandLineLength()>0)
		{
		// yes, so assume these are TProcessStartParams that we understand
		TProcessStartParams params;
		TPtr ptr(reinterpret_cast<TText*>(&params), 0, sizeof(TProcessStartParams)/sizeof(TText16));
		User::CommandLine(ptr);
		r = CLaunchedProcess::CompleteProcessLaunch(params);
		}
	else
		{
		// no - so somebody and this should ONLY be the test team!! - has passed
		// us a null command line, presumably by directly calling RProcess::Create()
		// NB in this case you have no way of determining directly that the process
		// creation has failed, or if the process is ready to start processing events!
		r = CLaunchedProcess::CompleteProcessLaunch();
		}
	return r;
	}


CBase* CSecureProcessASBase::CreateRootObjectL()
	{
	CExClient* cli = new (ELeave) CExClient();
	CleanupStack::PushL(cli);
	cli->ConstructL();
	CleanupStack::Pop(cli);
	return cli;
	}

TInt CSecureProcessASBase::EntryPointL(CBase* aBaseObj)
	{
	CExClient* cli = reinterpret_cast<CExClient*>(aBaseObj);
	DoExampleL(cli);
	return KErrNone;
	}
	
// must be static	
void CSecureProcessASBase::Final()
	{
	LBSLOG(ELogP1,"waiting...");
	User::After(10*2000000);
	LBSLOG(ELogP1,"Client Exit");
	}

