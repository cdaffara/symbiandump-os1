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

#include <e32debug.h>
#include "lbsmemlogger.h"
#include "lbsdevloggermacros.h"
#include "csecureasbase.h"
#include "tserverstartparams.h"

/** 
Default constructor

@internalTechnology
@released
 */	
CSecureASBase::CSecureASBase()
	{
	}
	
/** 
The main sever entry point with sever start parameters, create the cleanup stack

@param aParams A reference to TServerStartParams object
@see TServerStartParams
@return Symbian Standard Error code
@internalTechnology
@released
 */	
TInt CSecureASBase::ServerMain(TServerStartParams& aParams)
	{
	// called from the server process from E32Main
	__UHEAP_MARK;
	//
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL(aParams));
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	return r;

	} 	

/** 
start sever with parameters, create ActiveScheduler and install it
create a server implementation and signal the client when initialisation completes
start the AS object and wait to destroy.

@param aParams A reference to TServerStartParams object
@see TServerStartParams
@return Symbian Standard Error code
@internalTechnology
@released
 */	
TInt CSecureASBase::RunServerL(TServerStartParams& aParams)
	{
	LBSLOG(ELogP3, "->CSecureASBase::RunServerL");
		
	// first check that we are about to create the right type of server
	if(aParams.GetType() != KServerStartParamsTypeUid)
		{
		// this is not a process type that we understand. Has the caller
		// pass TProcessStartParams by mistake?
		User::Leave(KErrArgument); // this aborts the server startup
		}
	User::LeaveIfError(User::RenameThread(aParams.GetServerName()));
	//
	// create and install the active scheduler we need
	CSecureASBase* s = new (ELeave) CSecureASBase();
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	
	LBSMEMLOG_BEGIN();
	
	// test with the impl  
	MCreateServerImpl* impl = s->GetImplLC();	
	//
	// create the server (leave it on the cleanup stack)
	impl->CreateServerLC(aParams);
	//
	// Initialisation complete, now signal the client
	RProcess().Rendezvous(KErrNone);
	//
	// Ready to run
	LBSLOG(ELogP3, "Calling CActiveScheduler::Start");
	CActiveScheduler::Start();
	//
	// Cleanup the impl, server and scheduler. Impl is destroyed via ~CBase not the M class 
	LBSLOG(ELogP3, "Calling CleanupStack::PopAndDestroy(3)");

	CleanupStack::PopAndDestroy(3);
	
	LBSLOG(ELogP3, "<-CSecureASBase::RunServerL");

	LBSMEMLOG_END();

	return KErrNone;
	}


/** 
Panic the server with CSecureASBase category

@param aPanic Reason to painc
@see TSecureASBasePanic 
@internalTechnology
@released
 */	
void CSecureASBase::Panic(const TSecureASBasePanic aPanic)
	{
	_LIT(KSecureAsBase, "CSecureASBase");
	User::Panic(KSecureAsBase, aPanic);
	}
	

	
