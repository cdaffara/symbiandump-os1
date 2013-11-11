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
// This contains ESock Sequenced Start-up Test cases
// 
//

// EPOC includes
#include <e32base.h>
#include <ss_std.h>

// Test system includes
#include "ES_DUMMY.H"
#include "ESockSSA_Step1.h"


// Test step 1
const TDesC& CESockSSATest1::GetTestName()
	{
	_LIT(ret,"Test1");
	return ret;
	}

enum TVerdict CESockSSATest1::doTestStepL()
	{
	TVerdict verdict = EFail;

	Logger().WriteFormat(_L("Single parked RSocket::Open()"));

	const TInt KNumDataThreads = 1;
	RMultipleWait blockSemaphore(- KNumDataThreads);
	User::LeaveIfError(blockSemaphore.CreateLocal());
	CleanupClosePushL(blockSemaphore);
	TDataThreadControl dtCtrl(blockSemaphore, KErrNone);
	User::LeaveIfError(CreateDataThread(TDataThreadControl::ESocketOpen, dtCtrl));
	CleanupClosePushL(dtCtrl.iThread);

	verdict = WaitForDataThreadsToBlock(&dtCtrl, KNumDataThreads);
	if(verdict == EPass)
		{
		// Allow the ESOCK bootstrapping to proceed & wait for the data threads to be release & exit;
		verdict = WaitForDataThreadsToComplete(&dtCtrl, KNumDataThreads);
		}

	CleanupStack::PopAndDestroy(&dtCtrl.iThread);
	CleanupStack::PopAndDestroy(&blockSemaphore);
 
 	SetTestStepResult(verdict);   
	return verdict;
	}
//----------------------------------------------------------------------------------------
// Test step 2
const TDesC& CESockSSATest2::GetTestName()
	{
	_LIT(ret,"Test2");
	return ret;
	}

enum TVerdict CESockSSATest2::doTestStepL()
	{
	TVerdict verdict = EFail;

	Logger().WriteFormat(_L("Single parked RHostResolver::Open()"));

	const TInt KNumDataThreads = 1;
	RMultipleWait blockSemaphore(- KNumDataThreads);
	User::LeaveIfError(blockSemaphore.CreateLocal());
	CleanupClosePushL(blockSemaphore);
	TDataThreadControl dtCtrl(blockSemaphore, KErrNone);
	User::LeaveIfError(CreateDataThread(TDataThreadControl::EHostResolverOpen, dtCtrl));
	CleanupClosePushL(dtCtrl.iThread);

	verdict = WaitForDataThreadsToBlock(&dtCtrl, KNumDataThreads);
	if(verdict == EPass)
		{
		// Allow the ESOCK bootstrapping to proceed & wait for the data threads to be release & exit;
		verdict = WaitForDataThreadsToComplete(&dtCtrl, KNumDataThreads);
		}

	CleanupStack::PopAndDestroy(&dtCtrl.iThread);
	CleanupStack::PopAndDestroy(&blockSemaphore);
 
 	SetTestStepResult(verdict);   
	return verdict;
	}
//----------------------------------------------------------------------------------------
// Test step 3
const TDesC& CESockSSATest3::GetTestName()
	{
	_LIT(ret,"Test3");
	return ret;
	}

enum TVerdict CESockSSATest3::doTestStepL()
	{
	TVerdict verdict = EFail;

	Logger().WriteFormat(_L("Single parked RServiceResolver::Open()"));

	const TInt KNumDataThreads = 1;
	RMultipleWait blockSemaphore(- KNumDataThreads);
	User::LeaveIfError(blockSemaphore.CreateLocal());
	CleanupClosePushL(blockSemaphore);
	TDataThreadControl dtCtrl(blockSemaphore, KErrNone);
	User::LeaveIfError(CreateDataThread(TDataThreadControl::EServiceResolverOpen, dtCtrl));
	CleanupClosePushL(dtCtrl.iThread);

	verdict = WaitForDataThreadsToBlock(&dtCtrl, KNumDataThreads);
	if(verdict == EPass)
		{
		// Allow the ESOCK bootstrapping to proceed & wait for the data threads to be release & exit;
		verdict = WaitForDataThreadsToComplete(&dtCtrl, KNumDataThreads);
		}

	CleanupStack::PopAndDestroy(&dtCtrl.iThread);
	CleanupStack::PopAndDestroy(&blockSemaphore);
 
 	SetTestStepResult(verdict);   
	return verdict;
	}

//----------------------------------------------------------------------------------------
// Test step 4
const TDesC& CESockSSATest4::GetTestName()
	{
	_LIT(ret,"Test4");
	return ret;
	}

enum TVerdict CESockSSATest4::doTestStepL()
	{
	TVerdict verdict = EFail;

	Logger().WriteFormat(_L("Single parked RNetDatabase::Open()"));

	const TInt KNumDataThreads = 1;
	RMultipleWait blockSemaphore(- KNumDataThreads);
	User::LeaveIfError(blockSemaphore.CreateLocal());
	CleanupClosePushL(blockSemaphore);
	TDataThreadControl dtCtrl(blockSemaphore, KErrNone);
	User::LeaveIfError(CreateDataThread(TDataThreadControl::ENetDBOpen, dtCtrl));
	CleanupClosePushL(dtCtrl.iThread);

	verdict = WaitForDataThreadsToBlock(&dtCtrl, KNumDataThreads);
	if(verdict == EPass)
		{
		// Allow the ESOCK bootstrapping to proceed & wait for the data threads to be release & exit;
		verdict = WaitForDataThreadsToComplete(&dtCtrl, KNumDataThreads);
		}

	CleanupStack::PopAndDestroy(&dtCtrl.iThread);
	CleanupStack::PopAndDestroy(&blockSemaphore);
 
 	SetTestStepResult(verdict);   
	return verdict;
	}
//----------------------------------------------------------------------------------------
// Test step 5
const TDesC& CESockSSATest5::GetTestName()
	{
	_LIT(ret,"Test5");
	return ret;
	}

enum TVerdict CESockSSATest5::doTestStepL()
	{
	TVerdict verdict = EFail;

	Logger().WriteFormat(_L("Multiple parked Open() of various types"));

	const TInt KNumDataThreads = 4;
	
	RMultipleWait blockSemaphore(- KNumDataThreads);
	User::LeaveIfError(blockSemaphore.CreateLocal());
	CleanupClosePushL(blockSemaphore);
	
	CArrayFixFlat<TDataThreadControl> *dataThreads = new (ELeave)CArrayFixFlat<TDataThreadControl>(KNumDataThreads);
	CleanupStack::PushL(dataThreads);
	
	TDataThreadControl dummyThreadControl0(blockSemaphore, KErrNone);
	dataThreads->AppendL(dummyThreadControl0);
	User::LeaveIfError(CreateDataThread(TDataThreadControl::EHostResolverOpen, (*dataThreads)[0]));
	CleanupClosePushL((*dataThreads)[0].iThread);
	
	TDataThreadControl dummyThreadControl1(blockSemaphore, KErrNone);
	dataThreads->AppendL(dummyThreadControl1);
	User::LeaveIfError(CreateDataThread(TDataThreadControl::ENumProtocols, (*dataThreads)[1]));
	CleanupClosePushL((*dataThreads)[1].iThread);
	
	TDataThreadControl dummyThreadControl2(blockSemaphore, KErrNotFound);
	dataThreads->AppendL(dummyThreadControl2);
	User::LeaveIfError(CreateDataThread(TDataThreadControl::EGetProtocolInfo, (*dataThreads)[2]));
	CleanupClosePushL((*dataThreads)[2].iThread);
	
	TDataThreadControl dummyThreadControl3(blockSemaphore, KErrNotFound);
	dataThreads->AppendL(dummyThreadControl3);
	User::LeaveIfError(CreateDataThread(TDataThreadControl::EFindProtocol, (*dataThreads)[3]));
	CleanupClosePushL((*dataThreads)[3].iThread);
	
	verdict = WaitForDataThreadsToBlock((*dataThreads).Back(0), KNumDataThreads);
	if(verdict == EPass)
		{
		RSocketServ session;
		
		session.Connect();
		
		TUint absentIndex = 99;
		TRequestStatus status1 = KRequestPending;
		TRequestStatus status2 = KRequestPending;
		session.StartProtocol(KDummyAddrFamily, KSockDatagram, absentIndex, status1);
		session.StopProtocol(KDummyAddrFamily, KSockDatagram, absentIndex, status2);
		session.Close();
		
		// Allow the ESOCK bootstrapping to proceed & wait for the data threads to be release & exit;
		verdict = WaitForDataThreadsToComplete((*dataThreads).Back(0), KNumDataThreads);
		if(verdict == EPass && status1 != KErrNone && status2 != KErrNone)
			{
			verdict = EPass;
			}
		else
			{
			verdict = EFail;
			}
		}

	for(TInt i=KNumDataThreads - 1; i>=0; i--)
		{
		CleanupStack::PopAndDestroy(&((*dataThreads)[i].iThread));
		}

	CleanupStack::PopAndDestroy(dataThreads);
	CleanupStack::PopAndDestroy(&blockSemaphore);
 
 	SetTestStepResult(verdict);   
	return verdict;
	}




