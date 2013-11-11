// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains MBufMgr Test Step 16 - Memory Full
// 
//

// EPOC includes
#include <e32base.h>
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

#include "test16memoryfull.h"

#include <comms-infras/mbufallocator.h>
#include <comms-infras/commsbufpond.h>
// constructor
CTest16MemoryFull::CTest16MemoryFull()
	{
	SetTestStepName( _L("MBufMgrTest16") );	// Store the name of this test case
	}

// destructor
CTest16MemoryFull::~CTest16MemoryFull()
	{
	}

//
void CTest16MemoryFull::FillDes(TDes8 &aBuf, TInt aStartPos, TInt anEndPos, TUint aStartChar, TUint anEndChar)
	{
	StripeDes(aBuf, aStartPos, anEndPos, aStartChar, anEndChar);
	}

RCommsBufPond& CTest16MemoryFull::BufPond()
	{
	return iBufPond;
	}

enum TVerdict CTest16MemoryFull::doTestStepL(void)
	{
	//-------------- substep 1 --------------------
	INFO_PRINTF1(_L("  01 Create CMBufManager and install active scheduler:"));
	CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);

	// Create some initial pools
	// AddL(KMBuf_MBufSize, KMBuf_InitialAllocation, KMBuf_MinGrowth, KMBuf_GrowthThreshold)
	// Background memory allocation (CMBufPoolManager::RunL) is used more frequently if
	// the KMBuf_MinGrowth and the KMBuf_GrowthThreshold are similar in size
	// Synchronous allocation (AllocPool(*poolChain, growth, ETrue)) occurs when the
	// requested size (iChain.CopyIn(size)) is larger than the KMBuf_MinGrowth
	// Multiple threads attempt to grow different poolchains at the same time when there
	// are a greater variety of pool sizes to grow.
	// 16384

	RArray<TCommsBufPoolCreateInfo> createInfoArray;
	
	TCommsBufPoolCreateInfo createInfo;
	createInfo.iBufSize = 128;
	createInfo.iInitialBufs = 128;
	createInfo.iGrowByBufs = 64;
	createInfo.iMinFreeBufs = 40;
	createInfo.iCeiling = 410; 

	TCommsBufPoolCreateInfo createInfo2;
	createInfo2.iBufSize = 256;
	createInfo2.iInitialBufs = 64;
	createInfo2.iGrowByBufs = 7;
	createInfo2.iMinFreeBufs = 6;
	createInfo2.iCeiling = 204;

	TCommsBufPoolCreateInfo createInfo3;
	createInfo3.iBufSize = 512;
	createInfo3.iInitialBufs = 32;
	createInfo3.iGrowByBufs = 6;
	createInfo3.iMinFreeBufs = 5;
	createInfo3.iCeiling = 125;

	TCommsBufPoolCreateInfo createInfo4;
	createInfo4.iBufSize = 1024;
	createInfo4.iInitialBufs = 16;
	createInfo4.iGrowByBufs = 5;
	createInfo4.iMinFreeBufs = 4;
	createInfo4.iCeiling = 51; 

	TCommsBufPoolCreateInfo createInfo5;
	createInfo5.iBufSize = 2048;
	createInfo5.iInitialBufs = 8;
	createInfo5.iGrowByBufs = 4;
	createInfo5.iMinFreeBufs = 3;
	createInfo5.iCeiling = 15; 

	createInfoArray.Append(createInfo);
	createInfoArray.Append(createInfo2);
	createInfoArray.Append(createInfo3);
	createInfoArray.Append(createInfo4);
	createInfoArray.Append(createInfo5);
	// Create 262144
	CreateInstanceMBufMgrL(createInfoArray);
	CleanupClosePushL(iBufPond);
	createInfoArray.Close ();

	TBuf8<KFactor*KFactor*KFactor*KFactor> *aDes1, *aDes2;
	CleanupStack::PushL( aDes1 = new(ELeave) TBuf8<KFactor*KFactor*KFactor*KFactor> );
	CleanupStack::PushL( aDes2 = new(ELeave) TBuf8<KFactor*KFactor*KFactor*KFactor> );

	//-------------------substep 02-----------------------------
	INFO_PRINTF1(_L("  02 Create and install active object that will do the test:"));
	RMBufAsyncRequest async;
	CMBufAsyncMemFull* memoryfull;
	CleanupStack::PushL(memoryfull = new(ELeave) CMBufAsyncMemFull(this, async, aDes1, aDes2));
	CActiveScheduler::Add(memoryfull);

	//-------------------substep 03-----------------------------
	INFO_PRINTF1(_L("  03 Start the test:"));

	TInt ret = memoryfull->DoStartTest();
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Error: Async Alloc's failing in test run:"));
		User::Leave(EFail);
		}

	// If allocation less than expected then there may be a problem.
	// So get the allocated memory for each pool chain
	RMBufAllocator allocator;

	TInt size = allocator.NextMBufSize(0);
	_LIT(aLogD,"     Info: Available MBufs in %d Pool Chain: %d"); 
	while (size != KErrNotFound)
		{
		INFO_PRINTF3(aLogD, size, allocator.BytesAvailable(size) / size);
		size = allocator.NextMBufSize(size);
		}
	
//	TInt heapSize = iMBMngr->__DbgGetHeapSize();
	
	//Clean up stack
	CleanupStack::PopAndDestroy(memoryfull);
	CleanupStack::PopAndDestroy(aDes2);
	CleanupStack::PopAndDestroy(aDes1);
	CleanupStack::PopAndDestroy();
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);
	
	//-------------------substep 04-----------------------------
	_LIT(aLog1,"     Info: Number of main thread checkings: %d with %d fails last failure on %d");
	INFO_PRINTF4(aLog1,iMainThreadTries,iMainThreadFails,iMainThreadLastFailure);
	_LIT(aLog2,"     Info: Number of main thread allocate size failures: %d");
	INFO_PRINTF2(aLog2,iMainThreadSizeFails);
	_LIT(aLog3,"     Info: Number of high priority thread checkings: %d with %d fails");
	INFO_PRINTF3(aLog3,iThread3Tries,iThread3Fails);
	_LIT(aLog4,"     Info: Number of high priority thread allocation failures: %d");
	INFO_PRINTF2(aLog4,iThread3AllocFails);
	_LIT(aLog5,"     Info: Number of memory gobbler allocate tries and failures: %d with %d memory full last memory full on %d");
	INFO_PRINTF4(aLog5,iThread1AllocTries,iThread1MemoryFull,iThread1LastMemoryFull);
	_LIT(aLog6,"     Info: Number of memory gobbler allocate size failures: %d");
	INFO_PRINTF2(aLog6,iThread1SizeFails);
	_LIT(aLog7,"     Info: Number of memory gobbler allocate tries and failures: %d with %d memory full last memory full on %d");
	INFO_PRINTF4(aLog7,iThread2AllocTries,iThread2MemoryFull,iThread2LastMemoryFull);
	_LIT(aLog8,"     Info: Number of memory gobbler allocate size failures: %d");
	INFO_PRINTF2(aLog8,iThread2SizeFails);
	_LIT(aLog9,"     Info: Max chain length for memory gobbler allocation: %d ; %d");
	INFO_PRINTF3(aLog9,iThread1MaxLen,iThread2MaxLen);
//	_LIT(aLogA,"     Info: Max allocated memory: %d"); Log(aLogA,heapSize);
	_LIT(aLogB,"     Info: Basic API (uses TLS) - Average time to allocate mbufs: %d ; Average time to free mbufs %d");
	INFO_PRINTF3(aLogB,iThread1AllocTime/iThread1AllocTries,iThread1FreeTime/(1000 - iThread1AllocTries));
	_LIT(aLogC,"     Info: Higher Speed API (no TLS) - Average time to allocate mbufs: %d ; Average time to free mbufs %d");
	INFO_PRINTF3(aLogC,iThread2AllocTime/iThread2AllocTries,iThread2FreeTime/(1000 - iThread2AllocTries));
	
	if (iThread3Tries <1000)
		{
		INFO_PRINTF1(_L("   Error: High priority thread ended before 1000 loops are finished"));

		return EFail;
		}

	// test to see if the byte by byte comparisons failed
	if ( iThread3Fails || iMainThreadFails || iThread1SizeFails || iThread2SizeFails)
		{
		INFO_PRINTF1(_L("   Error: MBuf allocation failure"));

        SetTestStepResult(EFail);
		return TestStepResult();
		}

	// Test the maximum allocated memory
	// Some of the possible problems include failing to background allocate new memory
/*	if ( KMBufDefaultHeapSize > (heapSize + KMBufDefaultHeapSize/100) )
		{
		Log(_L("   Error: Background allocation may have failed"));

		return EFail;
		}
*/   async.Close();

    SetTestStepResult(EPass);
	return TestStepResult();
  }

  // Async request
CMBufAsyncMemFull::CMBufAsyncMemFull(CTest16MemoryFull* aTestObject,
		RMBufAsyncRequest& aMBufAsyncReq,
		TDes8 *aDes1,
		TDes8 *aDes2)
		:CActive(EPriorityStandard)
		,iTestObject(aTestObject)
		,iDes1(aDes1)
		,iDes2(aDes2)
		,iMBufAsyncReq(aMBufAsyncReq)
	{
	}

TInt CMBufAsyncMemFull::DoStartTest()
	{

	// Create some random sizes for allocation later
	for (TInt i=1; i<(KFactor+1); i++)
		{
		iRequested_size[i-1] = i * i * i * KFactor;
		}

	// Allocate two 5000-bytes long descriptors (Des1 & Des2):"));
	iDes1->SetLength(iRequested_size[KFactor-1]);
	iDes2->SetLength(iRequested_size[KFactor-1]);
	//Fill in the Des1 with a pattern:"));
	iTestObject->FillDes(*iDes1, 0, iRequested_size[KFactor-1], '@', 'Z');

	//-------------------substep 01-----------------------------

	TInt err=iThread1.Create(_L("testThread1Rec"),
		                       fThread1,
 							   KDefaultStackSize,
							   KDefaultHeapSize,
							   KMaxHeapSize,
							   (TAny*)iTestObject,
							   EOwnerProcess);
	if (err!=KErrNone)
		{
		User::Leave(EFail);
		}
	iThread1.SetPriority(EPriorityAbsoluteHigh);
	iThread1.Resume();

	//-------------------substep 02-----------------------------
	RThread aThread2;

	err=iThread2.Create(_L("testThread2Rec"),
		                       fThread2,
 							   KDefaultStackSize,
							   KDefaultHeapSize,
 							   KMaxHeapSize,
							   (TAny*)iTestObject,
							   EOwnerProcess);
	if (err!=KErrNone)
		{
		User::Leave(EFail);
		}
	iThread2.SetPriority(EPriorityAbsoluteHigh);
	iThread2.Resume();

	//-------------------substep 03-----------------------------
	RThread aThread3;

	err=iThread3.Create(_L("testThread3Rec"),
		                       fThread3,
 							   KDefaultStackSize,
							   KDefaultHeapSize,
							   KMaxHeapSize,
							   (TAny*)iTestObject,
							   EOwnerProcess);
	if (err!=KErrNone)
		{
		User::Leave(EFail);
		}
	iThread3.SetPriority(EPriorityAbsoluteHigh);
	iThread3.Resume();

	//-------------------substep 04-----------------------------

	iMBufAsyncReq.Alloc(iChain, iRequested_size[iRequestloop], iStatus);
	SetActive();
	CActiveScheduler::Start();

	iThread1.Close();
	iThread2.Close();
	iThread3.Close();

	return iStatus.Int();
	}


void CMBufAsyncMemFull::RunL()
	{
	if (iStatus.Int() == KErrNone)
		{

		//Copy in Des1 into Chain
		iChain.CopyIn(iDes1->LeftTPtr(iRequested_size[iRequestloop]));
		//Fill left most part of Des2 with zeros:"));
		iDes2->SetLength(iRequested_size[KFactor-1]);
		iTestObject->FillDes(*iDes2, 0, iRequested_size[iRequestloop], 0, 0);
		//Copy out Chain into Des2:"));
		iChain.CopyOut(*iDes2);
		//Compare the contents of Des1 & Des2:
		if((iDes1->LeftTPtr(iRequested_size[iRequestloop])).
				Compare(iDes2->Left(iRequested_size[iRequestloop])))
			{
			iTestObject->iMainThreadFails++;
			iTestObject->iMainThreadLastFailure = iTestObject->iMainThreadTries;
			if (iChain.Length() != iRequested_size[iRequestloop])
				{
				iTestObject->iMainThreadSizeFails++;
				}
			}
		iTestObject->iMainThreadTries++;
		//Free chain
		iChain.Free();

		//Check whether the other task has finished
		TBool isRunning1 = ETrue;
		TBool isRunning2 = ETrue;
		TBool isRunning3 = ETrue;
		volatile TExitType aExit;
		aExit = iThread1.ExitType();
		if (aExit != EExitPending)
			{
			isRunning1 = EFalse;
			}
		aExit = iThread2.ExitType();
		if (aExit != EExitPending)
			{
			isRunning2 = EFalse;
			}
		aExit = iThread3.ExitType();
		if (aExit != EExitPending)
			{
			isRunning3 = EFalse;
			}

		iRequestloop++;
		if (iRequestloop>9)
			{
			iRequestloop = 0;
			}

		if (isRunning1 || isRunning2 || isRunning3)
			{

			iMBufAsyncReq.Alloc(iChain, iRequested_size[iRequestloop], iStatus);

			SetActive();

			return;
			}
		}
	
	CActiveScheduler::Stop();

	}

// The memory gobbler thread
TInt CMBufAsyncMemFull::fThread1(TAny* aInput)
	{
	CTest16MemoryFull* pTestObject = (CTest16MemoryFull*)aInput;
	// We need to introduce this new client thread to the MBufMgr
	TCommsBufPondTLSOp tls(pTestObject->BufPond());
	tls.Set();
	
	CTrapCleanup* aCleanup = CTrapCleanup::New();

	//Install active scheduler
    CActiveScheduler* aActSch = new CActiveScheduler;
	if(aActSch==NULL)
		{
		return KErrNoMemory;
		}
	CActiveScheduler::Install(aActSch);

	RTimer aTimer;
	TRequestStatus aTimerStatus;	// Request status associated with timer
	aTimer.CreateLocal();			// Create timer for this thread
	//-------------- substep 1 --------------------

	// Create some random sizes for allocation later
	TInt requested_size[KFactor1];
	for (TInt i=1; i<(KFactor1+1); i++)
		{
		requested_size[i-1] = i * i * KFactor1;
		}

	// Allocate two 5000-bytes long descriptors (Des1 & Des2):"));
	TBuf8<KFactor1*KFactor1*KFactor1> *aDes1=NULL;
	TRAPD(ret, aDes1 = new(ELeave) TBuf8<KFactor1*KFactor1*KFactor1> );
	if(ret!=KErrNone)
		{
		return ret;
		}

	aDes1->SetLength(requested_size[KFactor1-1]);

	//Fill in the Des1 with a pattern:"));
	pTestObject->FillDes(*aDes1, 0, requested_size[KFactor1-1], 1, 1);
	
	TUint time;
	RMBufChain aChain;
	RMBufChain bChain;
	TInt requestloop = 0;
	TBool isFillMe = ETrue;
	for (TInt i = 0 ; i<1000;i++)
		{

		if (isFillMe)
			{
			pTestObject->iThread1AllocTries++;
			//Use up mbuf memory by appending to a chain
			time = User::FastCounter();
			TRAP(ret,aChain.AllocL(requested_size[requestloop]));
			time = User::FastCounter() - time;
			if (ret != KErrNone)
				{
				isFillMe = EFalse;
				pTestObject->iThread1MemoryFull++;
				pTestObject->iThread1LastMemoryFull = pTestObject->iThread1AllocTries - 1;
				}
			else
				{
				pTestObject->iThread1AllocTime += time;
				//Copy in Des1 into Chain
				aChain.CopyIn(aDes1->LeftTPtr(requested_size[requestloop]));
				if (aChain.Length() != requested_size[requestloop])
					{
					pTestObject->iThread1SizeFails++;
					}
				// Now grow the chain
				bChain.Append(aChain);
				}
			}
		else
			{
			//Free some memory
			TInt length = bChain.Length();
			if (length > pTestObject->iThread1MaxLen)
				{
				pTestObject->iThread1MaxLen = length;
				}
			TInt trimOffset = length - requested_size[requestloop];
			if (trimOffset > 0)
				{
				time = User::FastCounter();
				bChain.TrimEnd(trimOffset);
				time = User::FastCounter() - time;
				pTestObject->iThread1FreeTime += time;
				}
			else
				{
				isFillMe = ETrue;
				}
			}

		//Sleep for 5ms
		aTimer.After(aTimerStatus,5000);
		User::WaitForRequest(aTimerStatus);

		requestloop++;
		if (requestloop>(KFactor1-1))
			{
			requestloop = 0;
			}
		}
	// Free the memory
	bChain.Free();

	delete aDes1;
	CActiveScheduler::Install(NULL);
	delete aActSch;
	delete aCleanup;
	return ret;
}

// The memory gobbler thread
TInt CMBufAsyncMemFull::fThread2(TAny* aInput)
	{
	CTest16MemoryFull* pTestObject = (CTest16MemoryFull*)aInput;
	// We need to introduce this new client thread to the MBufMgr
	TCommsBufPondTLSOp tls(pTestObject->BufPond());
	tls.Set();

	CTrapCleanup* aCleanup = CTrapCleanup::New();

	//Install active scheduler
	CActiveScheduler* aActSch = new CActiveScheduler;
	if(aActSch==NULL)
		{
		return KErrNoMemory;
		}
	CActiveScheduler::Install(aActSch);

	RTimer aTimer;
	TRequestStatus aTimerStatus;	// Request status associated with timer
	aTimer.CreateLocal();			// Create timer for this thread
	//-------------- substep 1 --------------------

	// Create some random sizes for allocation later
	TInt requested_size[KFactor2];
	for (TInt i=1; i<(KFactor2+1); i++)
		{
		requested_size[i-1] = i * i * KFactor2;
		}

	// Allocate two 5000-bytes long descriptors (Des1 & Des2):"));
	TBuf8<KFactor2*KFactor2*KFactor2> *aDes1=NULL;
	TRAPD(ret, aDes1 = new(ELeave) TBuf8<KFactor2*KFactor2*KFactor2> );
	if(ret!=KErrNone)
		{
		return ret;
		}

	aDes1->SetLength(requested_size[KFactor2-1]);

	//Fill in the Des1 with a pattern:"));
	pTestObject->FillDes(*aDes1, 0, requested_size[KFactor2-1], 2, 2);
	
	RMBufAllocator allocator;
	TUint time;
	
	RMBufChain aChain;
	RMBufChain bChain;
	TInt requestloop = 0;
	TBool isFillMe = ETrue;
	for (TInt i = 0 ; i<1000;i++)
		{

		if (isFillMe)
			{
			pTestObject->iThread2AllocTries++;
			//Use up mbuf memory by appending to a chain
			time = User::FastCounter();
			TRAP(ret,aChain.AllocL(requested_size[requestloop], allocator));
			time = User::FastCounter() - time;
			if (ret != KErrNone)
				{
				isFillMe = EFalse;
				pTestObject->iThread2MemoryFull++;
				pTestObject->iThread2LastMemoryFull = pTestObject->iThread2AllocTries - 1;
				}
			else
				{
				pTestObject->iThread2AllocTime += time;
				//Copy in Des1 into Chain
				aChain.CopyIn(aDes1->LeftTPtr(requested_size[requestloop]));
				if (aChain.Length() != requested_size[requestloop])
					{
					pTestObject->iThread2SizeFails++;
					}
				// Now grow the chain
				bChain.Append(aChain);
				}
			}
		else
			{
			//Free some memory
			TInt length = bChain.Length();
			if (length > pTestObject->iThread2MaxLen)
				{
				pTestObject->iThread2MaxLen = length;
				}
			TInt trimOffset = length - requested_size[requestloop];
			if (trimOffset > 0)
				{
				time = User::FastCounter();
				bChain.TrimEnd(trimOffset);
				time = User::FastCounter() - time;
				pTestObject->iThread2FreeTime += time;
				}
			else
				{
				isFillMe = ETrue;
				}
			}

		//Sleep for 5ms
		aTimer.After(aTimerStatus,5000);
		User::WaitForRequest(aTimerStatus);

		requestloop++;
		if (requestloop>(KFactor2-1))
			{
			requestloop = 0;
			}
		}
	// Free the memory
	bChain.Free();

	delete aDes1;

	CActiveScheduler::Install(NULL);
	delete aActSch;
	delete aCleanup;
	return ret;
}

//
TInt CMBufAsyncMemFull::fThread3(TAny* aInput)
	{
	CTest16MemoryFull* pTestObject = (CTest16MemoryFull*)aInput;
	// We need to introduce this new client thread to the MBufMgr
	TCommsBufPondTLSOp tls(pTestObject->BufPond());
	tls.Set();

	CTrapCleanup* aCleanup = CTrapCleanup::New();

	//Install active scheduler
	CActiveScheduler* aActSch = new CActiveScheduler;
	if(aActSch==NULL)
		{
		return KErrNoMemory;
		}
	CActiveScheduler::Install(aActSch);

	RTimer aTimer;
	TRequestStatus aTimerStatus;	// Request status associated with timer
	aTimer.CreateLocal();			// Create timer for this thread
	//-------------- substep 1 --------------------

	// Allocate two 500-bytes long descriptors (Des1 & Des2):"));
	TBuf8<500> *aDes1 = NULL;
	TBuf8<500> *aDes2 = NULL;

	TRAPD(ret, aDes1 = new(ELeave) TBuf8<500>);
	if(ret!=KErrNone)
		{
		return ret;
		}

	TRAP(ret, aDes2 = new(ELeave) TBuf8<500>);
	if(ret!=KErrNone)
		{
		delete aDes1;
		return ret;
		}

	aDes1->SetLength(500);
	aDes2->SetLength(500);

	//Fill in the Des1 with a pattern
	pTestObject->FillDes(*aDes1, 0, 500, 'a', 'z');

	for (TInt i = 0 ; i<1000;i++)
		{
		//Allocate 500-bytes long RMBufChain
		RMBufChain aChain;
		TRAP(ret,aChain.AllocL(500));
		if (ret == KErrNone)
			{
			//Copy in Des1 into Chain
			aChain.CopyIn(*aDes1);
			//Fill in Des2 with zeros
			pTestObject->FillDes(*aDes2, 0, 500, 0, 0);
			//Copy out Chain into Des2;
			aChain.CopyOut(*aDes2);
			//Compare the contents of Des1 & Des2
			if(aDes1->Compare(*aDes2))
				pTestObject->iThread3Fails++;
			//Free chain
			aChain.Free();
			}
		else
			{
			pTestObject->iThread3AllocFails++;
			}
		pTestObject->iThread3Tries++;
		//Sleep for 5ms
		aTimer.After(aTimerStatus,5000);
		User::WaitForRequest(aTimerStatus);
		}
	delete aDes1;
	delete aDes2;
	CActiveScheduler::Install(NULL);
	delete aActSch;
	delete aCleanup;
	return ret;
}

