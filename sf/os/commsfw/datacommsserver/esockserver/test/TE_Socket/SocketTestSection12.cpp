// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains ESock Test cases from section 12
// 
//

// EPOC includes
#include <e32base.h>
#include <ss_std.h>
#include <e32std.h>
#include <e32kpan.h>

// Test system includes

#include "SocketTestSection12.h"

// vc doesn't like taking address of t.ExitCategory() for logging
#pragma warning (disable:4238) 

// Test step 12.1
const TDesC& CSocketTest12_1::GetTestName()
	{
	_LIT(ret,"Test12.1");
	return ret;
	}

enum TVerdict CSocketTest12_1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Panic Client using a Bad Handle"));
	
	// stop debugger crashing
	TBool oldJit = User::JustInTime();
	User::SetJustInTime(EFalse);
	
	Logger().WriteFormat(_L("Creating thread which uses a bad handle"));
	RThread t;
	
	TSocketThreadArg tArg;
	tArg.iHandle = this;
	tArg.iSem  = NULL;
	tArg.iNumSockets = 0;
	
	//Need to share logger across different threads
	Logger().ShareAuto();
	TInt ret = t.Create(_L("BadHandle"), BadHandleThread, KDefaultStackSize, 
		KDefaultHeapSize, KDefaultHeapSize, &tArg);
	CleanupClosePushL(t);
	Logger().WriteFormat(_L("Create returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Logging on and resuming thread"));
	TRequestStatus stat;
	t.Logon(stat);
	t.Resume();
	User::WaitForRequest(stat);
	TPtrC catName = t.ExitCategory();
	Logger().WriteFormat(_L("Thread Exit Category '%S', Reason %d, Type %d"), 
		&catName, t.ExitReason(), t.ExitType());
	TESTL(t.ExitCategory() == KESockClientPanic);
	TESTL(EBadHandle == t.ExitReason());
	TESTL(EExitPanic == t.ExitType());

	CleanupStack::PopAndDestroy(&t);
	
	// reset JIT state
	User::SetJustInTime(oldJit);
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 12.2
const TDesC& CSocketTest12_2::GetTestName()
	{
	_LIT(ret,"Test12.2");
	return ret;
	}

enum TVerdict CSocketTest12_2::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Panic for Reading Twice"));
	
	// stop debugger crashing
	TBool oldJit = User::JustInTime();
	User::SetJustInTime(EFalse);
	
	Logger().WriteFormat(_L("Creating thread which reads twice"));
	RThread t;
	TSocketThreadArg tArg;
	tArg.iHandle = this;
	tArg.iSem  = NULL;
	tArg.iNumSockets = 0;
	//Need to share logger across different threads
	Logger().ShareAuto();
	TInt ret = t.Create(_L("ReadTwice"), ReadTwiceThread, KDefaultStackSize, 
		KDefaultHeapSize, KDefaultHeapSize, &tArg);
	CleanupClosePushL(t);
	Logger().WriteFormat(_L("Create returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Logging on and resuming thread"));
	TRequestStatus stat;
	t.Logon(stat);
	t.Resume();
	User::WaitForRequest(stat);
	TPtrC catName = t.ExitCategory();
	Logger().WriteFormat(_L("Thread Exit Category '%S', Reason %d, Type %d"), 
		&catName, t.ExitReason(), t.ExitType());
	TESTL(t.ExitCategory() == KESockClientPanic );
	TESTL(EReadingAlready == t.ExitReason());
	TESTL(EExitPanic == t.ExitType());
	
	CleanupStack::PopAndDestroy(&t);
	
	// reset JIT state
	User::SetJustInTime(oldJit);
	
	SetTestStepResult(verdict);
	return verdict;
	}


