// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <e32def.h>
#include "testserver.h"

//---------------------------------------------------------------------------------------------
TInt LoadDrivers()
	{
#ifdef __WINS__
	#define KPDDName _L("ECDRV")
	#define KLDDName _L("ECOMM")
#else
	#define KPDDName _L("EUART1")
	#define KLDDName _L("ECOMM")
#endif

	TInt rerr = User::LoadPhysicalDevice(KPDDName);
	if(rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		return rerr;
		}
	rerr = User::LoadLogicalDevice(KLDDName);	
	if(rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		return rerr;
		}
	return KErrNone;	
	}
//---------------------------------------------------------------------------------------------
LOCAL_C void MainL()
	{
	CActiveScheduler* sched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
		
	CBluetoothUtilsTestServer* server = CBluetoothUtilsTestServer::NewL();
	RProcess::Rendezvous(KErrNone);
	sched->Start();

	delete server;
	CleanupStack::PopAndDestroy(sched);
	}
//---------------------------------------------------------------------------------------------
GLDEF_C TInt E32Main()
	{
	TInt rerr(KErrNone);
	
	rerr = LoadDrivers();
	if( rerr )
		{
		return rerr;
		}
		
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();

	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());

	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}

