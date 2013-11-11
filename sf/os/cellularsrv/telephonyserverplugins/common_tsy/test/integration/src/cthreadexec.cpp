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
// Contains the CThreadExec class definition.
// 
//

/**
 @file 
 @internalTechnology
*/
 
#include "cthreadexec.h"
#include "cctsytestlogging.h"

CThreadExec* CThreadExec::NewL()
	{	
	TEST_FRAMEWORK_LOG1(_L("CThreadExec::NewL"));
	CThreadExec* self = new(ELeave) CThreadExec();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CThreadExec::CThreadExec()
	{
	}
	
void CThreadExec::ConstructL()
	{
	TEST_FRAMEWORK_LOG1(_L("CThreadExec::ConstructL"));
	iActiveFnExec = CActiveFnExec::NewL(*this);
	CActiveScheduler::Add(iActiveFnExec);
	
	iExecSemaphore.CreateLocal(0);
	iActiveFnExec->Start();
	}
	
CThreadExec::~CThreadExec()
	{
	TEST_FRAMEWORK_LOG1(_L("CThreadExec::~CThreadExec"));
	iActiveFnExec->Cancel();
	delete iActiveFnExec;
	}

TInt CThreadExec::Exec(MFunctor* aFunction)
	{
	TEST_FRAMEWORK_LOG1(_L("CThreadExec::Exec"));
	iFunction = aFunction;
    iNeedSignal = ETrue;
	iActiveFnExec->Exec();
	// wait for the function to terminate
	iExecSemaphore.Wait();
	return iLatestError;
	}

void CThreadExec::DoExec()
	{
	TEST_FRAMEWORK_LOG1(_L("CThreadExec::DoExec"));
	// run the exec function
	iLatestError = KErrNone;
	TRAP(iLatestError, iFunction->ExecL());
	// signal called it's done
	if(iNeedSignal)
		{
		iExecSemaphore.Signal();
		}
	}

void CThreadExec::AsyncExec(MFunctor* aFunction)
/**
 *   Execute a function asynchronously. 
 *   This will execute the function in another thread, without waiting for completion (while the Exec will do the same, but the it will wait for the function completion).
 *   @param aFunction the function to execute
 */
	{
	iFunction = aFunction;
	iNeedSignal = EFalse;
	iActiveFnExec->Exec();
	}

CThreadExec::CActiveFnExec* CThreadExec::CActiveFnExec::NewL(CThreadExec& aOwner)
	{
	TEST_FRAMEWORK_LOG1(_L("CThreadExec::CActiveFnExec::NewL"));
	CActiveFnExec* self = new(ELeave) CActiveFnExec(aOwner);
	return self;
	}

CThreadExec::CActiveFnExec::CActiveFnExec(CThreadExec& aOwner)
	: CActive(CActive::EPriorityHigh),iOwner(aOwner)
	{
	TEST_FRAMEWORK_LOG1(_L("CThreadExec::CActiveFnExec::CActiveFnExec"));
	// open a reference to the current thread
	iThread.Open(RThread().Id());
	}

void CThreadExec::CActiveFnExec::Start()
	{
	TEST_FRAMEWORK_LOG1(_L("CThreadExec::CActiveFnExec::Start"));
	iStatus = KRequestPending;
	SetActive();
	}
	
void CThreadExec::CActiveFnExec::Exec()
	{
	TEST_FRAMEWORK_LOG1(_L("CThreadExec::CActiveFnExec::Exec"));
	// signal the main thread 
	TRequestStatus* reqStatus = &iStatus;
	iThread.RequestComplete(reqStatus, KErrNone);
	}

void CThreadExec::CActiveFnExec::RunL()
	{
	TEST_FRAMEWORK_LOG1(_L("CThreadExec::CActiveFnExec::RunL"));
	iOwner.DoExec();
	iStatus = KRequestPending;
	SetActive();
	}

void CThreadExec::CActiveFnExec::DoCancel()
	{
	TEST_FRAMEWORK_LOG1(_L("CThreadExec::CActiveFnExec::DoCancel"));
	TRequestStatus* reqStatus = &iStatus;
	User::RequestComplete(reqStatus, KErrCancel);
	}
	
