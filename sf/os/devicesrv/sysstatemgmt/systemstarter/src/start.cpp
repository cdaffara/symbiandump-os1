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
//

#include "SysStart.h"

LOCAL_C void StartSystemL();

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK; // mark heap state

	CTrapCleanup* theTrapCleanup=CTrapCleanup::New();
	
	if (!theTrapCleanup)
		{
		return KErrNoMemory;
		}
	else
		{		
		TRAPD(error, StartSystemL());
    
    	delete theTrapCleanup;
		__UHEAP_MARKEND; // check no memory leak

		return(error);
		}
	}

LOCAL_C void StartSystemL()
	{
	if(!User::CreatorHasCapability(ECapabilityPowerMgmt))
		{
		User::Leave(KErrPermissionDenied); // ensure creator is trusted
		}

	__UHEAP_MARK;

	CActiveScheduler* theScheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(theScheduler);
	CActiveScheduler::Install(theScheduler);

	CSystemStarter* starter = CSystemStarter::NewLC();

	starter->Start();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(starter);
	CleanupStack::PopAndDestroy(theScheduler);
	
	__UHEAP_MARKEND;
	}
