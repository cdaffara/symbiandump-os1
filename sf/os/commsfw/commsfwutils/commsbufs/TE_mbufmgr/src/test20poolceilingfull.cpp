/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

#include <ss_std.h>

// Test system includes
//#ifdef SYMBIAN_OLD_EXPORT_LOCATION
//#include "networking/log.h"
//#include "networking/teststep.h"
//#else
//#include <networking/log.h>
//#include <networking/teststep.h>
//#endif

#include "TestStepCTMbufmgr.h"

#include "test20poolceilingfull.h"


CTest20PoolCeilingFull::CTest20PoolCeilingFull()
	{
	SetTestStepName( _L("MBufMgrTest20") );	// Store the name of this test case
	}

CTest20PoolCeilingFull::~CTest20PoolCeilingFull()
	{
	iChain.Free();
	}

TVerdict CTest20PoolCeilingFull::doTestStepL()
	{
	__UHEAP_MARK;
	//-------------- substep 1 --------------------
	INFO_PRINTF1(_L("  01 Create CMBufManager and install active scheduler:"));
	
	CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);

	RArray<TCommsBufPoolCreateInfo> createInfoArray;
	
	TCommsBufPoolCreateInfo createInfo;
	createInfo.iBufSize = 256;
	createInfo.iInitialBufs = 50;
	createInfo.iGrowByBufs = 20;
	createInfo.iMinFreeBufs = 4;
	createInfo.iCeiling = 100; 
	createInfoArray.Append(createInfo);

	CreateInstanceMBufMgrL(createInfoArray);
	CleanupClosePushL(iBufPond);
	createInfoArray.Close ();

	INFO_PRINTF1(_L("  02 Create and install active object that will do the test:"));

	CMBufAsyncPoolCeilingFull* poolCeilingFull;
	CleanupStack::PushL(poolCeilingFull = new(ELeave) CMBufAsyncPoolCeilingFull(*this));

	INFO_PRINTF1(_L("  03 Start the test:"));
	if(poolCeilingFull->DoStartTest() != KErrNone)
		{
		INFO_PRINTF1(_L("Error: Async Alloc's failing in test run:"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	//Clean up stack
	CleanupStack::PopAndDestroy(); // poolCeilingFull
	CleanupStack::PopAndDestroy(); // bufpond
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);
	
	__UHEAP_MARKEND;
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

RCommsBufPond& CTest20PoolCeilingFull::BufPond()
	{
	return iBufPond;
	}

CMBufAsyncPoolCeilingFull::CMBufAsyncPoolCeilingFull(CTest20PoolCeilingFull& aTestObject)
: CActive(EPriorityStandard),
	iTestObject(aTestObject)
	{
	CActiveScheduler::Add(this);
	}


CMBufAsyncPoolCeilingFull::~CMBufAsyncPoolCeilingFull()
	{
	Cancel();
	iAsyncChain.Free();	
	}

void CMBufAsyncPoolCeilingFull::CompleteSelf()
	{
	TRequestStatus* pStat = &iStatus;
	User::RequestComplete(pStat, KErrNone);
	SetActive();
	}

void CMBufAsyncPoolCeilingFull::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	if((iThread1.ExitType() != EExitPending))
		{
		CActiveScheduler::Stop();
		return;		
		}
	if(iAsyncChain.IsEmpty())
		{
		iAsyncRequest.Alloc(iAsyncChain, 256 * 8, iStatus);	
		SetActive();							
		}
	else
		{
		iAsyncChain.Free();			
		CompleteSelf();		
		}	
	}

void CMBufAsyncPoolCeilingFull::DoCancel()
	{
	iAsyncRequest.Cancel();
	}


TInt CMBufAsyncPoolCeilingFull::DoStartTest()
	{
	
	TInt err=iThread1.Create(_L("testThread1Rec"),
		                       fThread1,
 							   KDefaultStackSize,
							   KDefaultHeapSize,
							   KMaxHeapSize,
							   (TAny*)&iTestObject,
							   EOwnerProcess);
	if (err!=KErrNone)
		{
		User::Leave(EFail);
		}
	//iThread1.SetPriority(EPriorityAbsoluteHigh);
	iThread1.Resume();
	iAsyncRequest.Alloc(iAsyncChain, 256, iStatus);	
	SetActive();
	CActiveScheduler::Start();

	iThread1.Close();
	iThread2.Close();
	iThread3.Close();

	return iError;
	}


TInt CMBufAsyncPoolCeilingFull::fThread1(TAny* aInput)
	{
	TInt totalAllocation = 90;
	CTest20PoolCeilingFull* pTestObject = (CTest20PoolCeilingFull*)aInput;
	// We need to introduce this new client thread to the MBufMgr
	TCommsBufPondTLSOp tls(pTestObject->BufPond());
	tls.Set();
	
	CTrapCleanup* aCleanup = CTrapCleanup::New();
	
	TBool freeMem = EFalse;
	for(TInt i=0; i < 1000; ++i)
		{
        //INFO_PRINTF2(_L("Inside the for loop: %d"),i);
        RDebug::Printf("inside the for loop: %d", i);

		RMBufChain chain;
		chain.Alloc(512, 256, 256);
		if(freeMem)
			{
			chain.Free();
			freeMem = EFalse;	
			}
		else
			{
			pTestObject->iChain.Append(chain);
			freeMem = ETrue;
			}
		if(pTestObject->iChain.NumBufs() == totalAllocation)
			{
			// Free all. Will trigger the AO.
			pTestObject->iChain.Free();
			break;
			}	
		//Sleep for 5ms
		User::After(5000);
		}
	
	delete aCleanup;
	return KErrNone;
	}



