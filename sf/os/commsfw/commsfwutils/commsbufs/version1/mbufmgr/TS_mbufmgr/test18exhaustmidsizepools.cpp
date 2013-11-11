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
// Contains MBufMgr Test Step 18 for exhausting mid size pools
// 
//


#include <e32base.h>

// Test system includes
#include <networking/log.h>
#include <networking/teststep.h>
#include "TestStepCTMbufmgr.h"
#include "TestSuiteCTMbufmgr.h"

#include "test18exhaustmidsizepools.h"

// RHeap information
#include <comms-infras/mbufallocator.h>

CTest18ExhaustMidSizePools::CTest18ExhaustMidSizePools()
	{
	iTestStepName = _L("MBufMgrTest18");// Store the name of this test case
	}

CTest18ExhaustMidSizePools::~CTest18ExhaustMidSizePools()
	{
	}

void CTest18ExhaustMidSizePools::FillDes(TDes8 &aBuf, TInt aStartPos, TInt anEndPos, TUint aStartChar, TUint anEndChar)
	{
	StripeDes(aBuf, aStartPos, anEndPos, aStartChar, anEndChar);
	}

enum TVerdict CTest18ExhaustMidSizePools::doTestStepL(void)
	{
	__UHEAP_MARK;
	
#ifdef __CFLOG_ACTIVE
	__CFLOG_CREATEL;
	__CFLOG_OPEN;
#endif
	
	//-------------- substep 1 --------------------
	Log(_L("  01 Create CMBufManager and install active scheduler:"));
    CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);

	MMBufSizeAllocator* MBufSizeAllocator;

	// Create 131072
	CleanupStack::PushL(CreateInstanceMBufMgrL(KMBufDefaultHeapSize / 2, MBufSizeAllocator));

	// Create some initial pools
	// AddL(KMBuf_MBufSize, KMBuf_InitialAllocation, KMBuf_MinGrowth, KMBuf_GrowthThreshold)
	// Background memory allocation (CMBufPoolManager::RunL) is used more frequently if
	// the KMBuf_MinGrowth and the KMBuf_GrowthThreshold are similar in size
	// Synchronous allocation (AllocPool(*poolChain, growth, ETrue)) occurs when the
	// requested size (iChain.CopyIn(size)) is larger than the KMBuf_MinGrowth
	// Multiple threads attempt to grow different poolchains at the same time when there
	// are a greater variety of pool sizes to grow.
	// 16384
	MBufSizeAllocator->AddL(128,128,64,40);
	// 16384
	MBufSizeAllocator->AddL(256, 64, 7, 6);
	// 16384
	MBufSizeAllocator->AddL(512, 32, 6, 5);
	// 16384
	MBufSizeAllocator->AddL(1024, 16, 5, 4);
	// 16384
	MBufSizeAllocator->AddL(2048, 8, 4, 3);
	
	RMBufAllocator allocator;
	
	TInt allocTime128 = 0;
	TInt allocTime256 = 0;
	TInt allocTime512 = 0;
	TInt allocTime1024 = 0;
	TInt allocTime2048 = 0;
	
	TInt allocCount128 = 0;
	TInt allocCount256 = 0;
	TInt allocCount512 = 0;
	TInt allocCount1024 = 0;
	TInt allocCount2048 = 0;
	
	TInt allocIndex128 = 0;
	TInt allocIndex256 = 0;
	TInt allocIndex512 = 0;
	TInt allocIndex1024 = 0;
	TInt allocIndex2048 = 0;
	
	TInt failedSizeCount = 0;
	
	TInt counter = 0;
	
	TUint time;
	RMBufChain aChain, bChain;
	RMBufChain mbuf1, mbuf2, mbuf3, mbuf4, mbuf5, mbuf6;
	
	//-------------- substep 2 --------------------
	Log(_L("  02 Fill in the Des1 with a pattern:"));
	TBuf8<2500> *aDes1 = NULL;
	TRAPD(ret, aDes1 = new(ELeave) TBuf8<2500>);
	if(ret != KErrNone)
		return EFail;

	aDes1->SetLength(2500);
	FillDes(*aDes1, 0, 2500, 1, 1);
	
	//-------------- substep 3 --------------------
	Log(_L("  03 Start to allocate 500-bytes long RMBufs:"));

	time = User::FastCounter();
	ret = aChain.Alloc(500, allocator);
	time = User::FastCounter() - time;
	while (ret == KErrNone)
		{
		//Copy in Des1 into Chain
		aChain.CopyIn(aDes1->LeftTPtr(500));
		if (aChain.Length() != 500)
			{
			Log(_L("Error: Allocated RMBufChain size does not match requested:"));
			failedSizeCount++;
			}
		
		switch (aChain.First()->Size())
			{
			case 128:
				if (allocator.BytesAvailable(256) / 256 > 0)
					Log(_L("Error: 256-bytes long RMBuf available, but allocated 128-bytes long RMBuf:"));
				else if (allocIndex128 == 0)
					{
					counter++;
					allocIndex128 = counter;
					}
				allocCount128++;
				allocTime128 += time;
				break;
			case 256:
				if (allocator.BytesAvailable(2048) / 2048 > 0)
					Log(_L("Error: 2048-bytes long RMBuf available, but allocated 256-bytes long RMBuf:"));
				else if (allocIndex256 == 0)
					{
					counter++;
					allocIndex256 = counter;
					}
				allocCount256++;
				allocTime256 += time;
				break;
			case 512:
				if (allocIndex512 == 0)
					{
					counter++;
					allocIndex512 = counter;
					}
				allocCount512++;
				allocTime512 += time;
				break;
			case 1024:
				if (allocator.BytesAvailable(512) / 512 > 0)
					Log(_L("Error: 512-bytes long RMBuf available, but allocated 1024-bytes long RMBuf:"));
				else if (allocIndex1024 == 0)
					{
					counter++;
					allocIndex1024 = counter;
					}
				allocCount1024++;
				allocTime1024 += time;
				break;
			case 2048:
				if (allocator.BytesAvailable(1024) / 1024 > 0)
					Log(_L("Error: 1024-bytes long RMBuf available, but allocated 2048-bytes long RMBuf:"));
				else if (allocIndex2048 == 0)
					{
					counter++;
					allocIndex2048 = counter;
					}
				allocCount2048++;
				allocTime2048 += time;				
				break;
			}
		
		// Now grow the chain
		if (allocCount256 == 1)
			{
			mbuf1.Append(aChain);
			}
		else if (allocCount512 == 1)
			{
			mbuf2.Append(aChain);
			}
		else if (allocCount512 == 2)
			{
			mbuf3.Append(aChain);
			}
		else if (allocCount512 == 3)
			{
			mbuf4.Append(aChain);
			}
		else if (allocCount2048 == 1)
			{
			mbuf5.Append(aChain);
			}
		else if (allocCount2048 == 2)
			{
			mbuf6.Append(aChain);
			}
		else
			{
			bChain.Append(aChain);
			}
		
		time = User::FastCounter();
		ret = aChain.Alloc(500, allocator);
		time = User::FastCounter() - time;
		}

	if (allocIndex512 == 1)
		{
		_LIT(aLog1,"     Info: %d. Time to alloc %d 512-bytes long RMBuf: %d Average: %d");
		Log(aLog1, allocIndex512, allocCount512, allocTime512, allocTime512/allocCount512);
		}
	if (allocIndex1024 == 2)
		{
		_LIT(aLog2,"     Info: %d. Time to alloc %d 1024-bytes long RMBuf: %d Average: %d");
		Log(aLog2, allocIndex1024, allocCount1024, allocTime1024, allocTime1024/allocCount1024);
		}
	if (allocIndex2048 == 3)
		{
		_LIT(aLog3,"     Info: %d. Time to alloc %d 2048-bytes long RMBuf: %d Average: %d");
		Log(aLog3, allocIndex2048, allocCount2048, allocTime2048, allocTime2048/allocCount2048);
		}
	if (allocIndex256 == 4)
		{
		_LIT(aLog4,"     Info: %d. Time to alloc %d 256-bytes long RMBuf: %d Average: %d");
		Log(aLog4, allocIndex256, allocCount256, allocTime256, allocTime256/allocCount256);
		}
	if (allocIndex128 == 5)
		{
		_LIT(aLog5,"     Info: %d. Time to alloc %d 128-bytes long RMBuf: %d Average: %d");
		Log(aLog5, allocIndex128, allocCount128, allocTime128, allocTime128/allocCount128);
		}
	
	TBool firstSuccess = ETrue;
	TBool secondSuccess = ETrue;
	TBool thirdSuccess = ETrue;
	
	//-------------------substep 4-----------------------------
	Log(_L("  04 Allocate 750-byte long RMBuf after deallocating 1 256-byte and 2 512-byte long RMBufs:"));	
	mbuf1.TrimEnd(244);	// 256-byte mbuf
	mbuf2.Free();		// 512-byte mbuf	
	mbuf3.Free();		// 512-byte mbuf
	
	ret = aChain.Alloc(750, allocator);
	if (ret != KErrNone)
		{
		Log(_L("Error: Couldn't allocate RMBuf:"));
		firstSuccess = EFalse;
		}
	else
		{
		//Copy in Des1 into Chain
		aChain.CopyIn(aDes1->LeftTPtr(750));
		if (aChain.Length() != 750)
			{
			Log(_L("Error: Allocated RMBufChain size does not match requested:"));
			firstSuccess = EFalse;
			}
		
		// 1 512-byte long RMBuf should be free
		if (allocator.BytesAvailable(512) == 512)
			{
			Log(_L("     Info: 1 512-byte long RMBuf is free:"));
			}
		else
			{
			Log(_L("Error: 1 512-byte long RMBuf should be free, but not:"));
			firstSuccess = EFalse;
			}
		}
		
	bChain.Append(aChain);
	
	//-------------------substep 5-----------------------------
	Log(_L("  05 Allocate 750-byte long RMBuf after deallocating 2 512-byte long RMBufs:"));
	mbuf4.Free();	// 512-byte mbuf
	
	ret = aChain.Alloc(750, allocator);
	if (ret != KErrNone)
		{
		Log(_L("Error: Couldn't allocate RMBuf:"));
		secondSuccess = EFalse;
		}
	else
		{
		//Copy in Des1 into Chain
		aChain.CopyIn(aDes1->LeftTPtr(750));
		if (aChain.Length() != 750)
			{
			Log(_L("Error: Allocated RMBufChain size does not match requested:"));
			secondSuccess = EFalse;
			}
		}
		
	bChain.Append(aChain);
	
	//-------------------substep 6-----------------------------
	Log(_L("  06 Allocate 2100-byte long RMBuf after deallocating 2 2048-byte long RMBufs:"));	
	mbuf5.Free();	// 2048-byte mbuf
	mbuf6.Free();	// 2048-byte mbuf
	
	ret = aChain.Alloc(2100, allocator);
	if (ret != KErrNone)
		{
		Log(_L("Error: Couldn't allocate RMBuf:"));
		thirdSuccess = EFalse;
		}
	else
		{
		//Copy in Des1 into Chain
		aChain.CopyIn(aDes1->LeftTPtr(2100));
		if (aChain.Length() != 2100)
			{
			Log(_L("Error: Allocated RMBufChain size does not match requested:"));
			thirdSuccess = EFalse;
			}
		}
		
	bChain.Append(aChain);
	
	mbuf1.Free();
	bChain.Free();
	
	delete aDes1;
		
	//-------------- substep 7 --------------------
	Log(_L("  07 Clean up stack:"));
    CleanupStack::PopAndDestroy(iMBMngr);
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);
	
#ifdef __CFLOG_ACTIVE
	__CFLOG_CLOSE;
	__CFLOG_DELETE;
#endif
	
	if (ret != KErrNone || failedSizeCount > 0 || !firstSuccess || !secondSuccess || !thirdSuccess)
		{
		return EFail;
		}

	__UHEAP_MARKEND;
	return EPass;
	}
