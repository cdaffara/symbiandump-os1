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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code   
*/

#include <e32std.h>
#include <e32err.h>
#include <e32base.h>

TInt MainL()
	{
	RSemaphore sem;
	sem.OpenGlobal(_L("TSysMon_Critical"));
	CleanupClosePushL(sem);
	
	User::LeaveIfError(User::SetProcessCritical(User::ESystemCritical));

	RProcess::Rendezvous(KErrNone);

	sem.Wait();
	
	User::LeaveIfError(User::SetProcessCritical(User::ENotCritical));
	
	sem.Signal();
	
	CleanupStack::PopAndDestroy();

	return KErrNone;
	}


//  Global Functions

TInt E32Main()
	{
	// Create cleanup stack
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();

	// Run application code inside TRAP harness
	TRAPD(testError, MainL());
	if (testError)
		{
		User::Panic(_L("Test failure"), testError);
		}

	delete cleanup;
	__UHEAP_MARKEND;
	return testError;
	}
