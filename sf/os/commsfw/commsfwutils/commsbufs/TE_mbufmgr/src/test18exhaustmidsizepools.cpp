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
//#ifdef SYMBIAN_OLD_EXPORT_LOCATION
//#include "networking/log.h"
//#include "networking/teststep.h"
//#else
//#include <networking/log.h>
//#include <networking/teststep.h>
//#endif

#include "TestStepCTMbufmgr.h"

#include "test18exhaustmidsizepools.h"

// RHeap information
#include <comms-infras/mbufallocator.h>
#include <comms-infras/commsbufpond.h>

CTest18ExhaustMidSizePools::CTest18ExhaustMidSizePools()
	{
	SetTestStepName( _L("MBufMgrTest18") );	// Store the name of this test case
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
	TInt totalPools = 5;
	TInt poolSize = (KMBufDefaultHeapSize / 2)/totalPools;	

	RArray<TCommsBufPoolCreateInfo> createInfoArray;
	
	TCommsBufPoolCreateInfo createInfo;
	createInfo.iBufSize = 128;
	createInfo.iInitialBufs = 128;
	createInfo.iGrowByBufs = 64;
	createInfo.iMinFreeBufs = 40;
	createInfo.iCeiling = poolSize/createInfo.iBufSize;

	TCommsBufPoolCreateInfo createInfo2;
	createInfo2.iBufSize = 256;
	createInfo2.iInitialBufs = 64;
	createInfo2.iGrowByBufs = 7;
	createInfo2.iMinFreeBufs = 6;
	createInfo2.iCeiling = poolSize/createInfo.iBufSize;	

	TCommsBufPoolCreateInfo createInfo3;
	createInfo3.iBufSize = 512;
	createInfo3.iInitialBufs = 32;
	createInfo3.iGrowByBufs = 6;
	createInfo3.iMinFreeBufs = 5;
	createInfo3.iCeiling = poolSize/createInfo.iBufSize;	

	TCommsBufPoolCreateInfo createInfo4;
	createInfo4.iBufSize = 1024;
	createInfo4.iInitialBufs = 16;
	createInfo4.iGrowByBufs = 5;
	createInfo4.iMinFreeBufs = 4;
	createInfo4.iCeiling = poolSize/createInfo.iBufSize;	

	TCommsBufPoolCreateInfo createInfo5;
	createInfo5.iBufSize = 2048;
	createInfo5.iInitialBufs = 8;
	createInfo5.iGrowByBufs = 4;
	createInfo5.iMinFreeBufs = 3;
	createInfo5.iCeiling = poolSize/createInfo.iBufSize;	

	createInfoArray.Append(createInfo);
	createInfoArray.Append(createInfo2);
	createInfoArray.Append(createInfo3);
	createInfoArray.Append(createInfo4);
	createInfoArray.Append(createInfo5);

	// Create 262144
	CreateInstanceMBufMgrL(createInfoArray);
	CleanupClosePushL(iBufPond);
	createInfoArray.Close ();
	
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
	INFO_PRINTF1(_L("  02 Fill in the Des1 with a pattern:"));
	TBuf8<2500> *aDes1 = NULL;
	TRAPD(ret, aDes1 = new(ELeave) TBuf8<2500>);
	if(ret != KErrNone)
		{
        SetTestStepResult(EFail);
		return TestStepResult();
		}

	aDes1->SetLength(2500);
	FillDes(*aDes1, 0, 2500, 1, 1);
	
	//-------------- substep 3 --------------------
	INFO_PRINTF1(_L("  03 Start to allocate 500-bytes long RMBufs:"));

	time = User::FastCounter();
	ret = aChain.Alloc(500, allocator);
	time = User::FastCounter() - time;
	while (ret == KErrNone)
		{
		//Copy in Des1 into Chain
		aChain.CopyIn(aDes1->LeftTPtr(500));
		if (aChain.Length() != 500)
			{
			INFO_PRINTF1(_L("Error: Allocated RMBufChain size does not match requested:"));
			failedSizeCount++;
			}
		
		switch (aChain.First()->Size())
			{
			case 128:
				if (allocator.BytesAvailable(256) / 256 > 0)
					{
					INFO_PRINTF1(_L("Error: 256-bytes long RMBuf available, but allocated 128-bytes long RMBuf:"));
					}
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
					{
					INFO_PRINTF1(_L("Error: 2048-bytes long RMBuf available, but allocated 256-bytes long RMBuf:"));
					}
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
					{
					INFO_PRINTF1(_L("Error: 512-bytes long RMBuf available, but allocated 1024-bytes long RMBuf:"));
					}
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
					{
					INFO_PRINTF1(_L("Error: 1024-bytes long RMBuf available, but allocated 2048-bytes long RMBuf:"));
					}
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
		INFO_PRINTF5(aLog1, allocIndex512, allocCount512, allocTime512, allocTime512/allocCount512);
		}
	if (allocIndex1024 == 2)
		{
		_LIT(aLog2,"     Info: %d. Time to alloc %d 1024-bytes long RMBuf: %d Average: %d");
		INFO_PRINTF5(aLog2, allocIndex1024, allocCount1024, allocTime1024, allocTime1024/allocCount1024);
		}
	if (allocIndex2048 == 3)
		{
		_LIT(aLog3,"     Info: %d. Time to alloc %d 2048-bytes long RMBuf: %d Average: %d");
		INFO_PRINTF5(aLog3, allocIndex2048, allocCount2048, allocTime2048, allocTime2048/allocCount2048);
		}
	if (allocIndex256 == 4)
		{
		_LIT(aLog4,"     Info: %d. Time to alloc %d 256-bytes long RMBuf: %d Average: %d");
		INFO_PRINTF5(aLog4, allocIndex256, allocCount256, allocTime256, allocTime256/allocCount256);
		}
	if (allocIndex128 == 5)
		{
		_LIT(aLog5,"     Info: %d. Time to alloc %d 128-bytes long RMBuf: %d Average: %d");
		INFO_PRINTF5(aLog5, allocIndex128, allocCount128, allocTime128, allocTime128/allocCount128);
		}
	
	TBool firstSuccess = ETrue;
	TBool secondSuccess = ETrue;
	TBool thirdSuccess = ETrue;
	
	//-------------------substep 4-----------------------------
	INFO_PRINTF1(_L("  04 Allocate 750-byte long RMBuf after deallocating 1 256-byte and 2 512-byte long RMBufs:"));	
	mbuf1.TrimEnd(244);	// 256-byte mbuf
	mbuf2.Free();		// 512-byte mbuf	
	mbuf3.Free();		// 512-byte mbuf
	
	ret = aChain.Alloc(750, allocator);
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Error: Couldn't allocate RMBuf:"));
		firstSuccess = EFalse;
		}
	else
		{
		//Copy in Des1 into Chain
		aChain.CopyIn(aDes1->LeftTPtr(750));
		if (aChain.Length() != 750)
			{
			INFO_PRINTF1(_L("Error: Allocated RMBufChain size does not match requested:"));
			firstSuccess = EFalse;
			}
		
		// 1 512-byte long RMBuf should be free
		if (allocator.BytesAvailable(512) == 512)
			{
			INFO_PRINTF1(_L("     Info: 1 512-byte long RMBuf is free:"));
			}
		else
			{
			INFO_PRINTF1(_L("Error: 1 512-byte long RMBuf should be free, but not:"));
			firstSuccess = EFalse;
			}
		}
		
	bChain.Append(aChain);
	
	//-------------------substep 5-----------------------------
	INFO_PRINTF1(_L("  05 Allocate 750-byte long RMBuf after deallocating 2 512-byte long RMBufs:"));
	mbuf4.Free();	// 512-byte mbuf
	
	ret = aChain.Alloc(750, allocator);
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Error: Couldn't allocate RMBuf:"));
		secondSuccess = EFalse;
		}
	else
		{
		//Copy in Des1 into Chain
		aChain.CopyIn(aDes1->LeftTPtr(750));
		if (aChain.Length() != 750)
			{
			INFO_PRINTF1(_L("Error: Allocated RMBufChain size does not match requested:"));
			secondSuccess = EFalse;
			}
		}
		
	bChain.Append(aChain);
	
	//-------------------substep 6-----------------------------
	INFO_PRINTF1(_L("  06 Allocate 2100-byte long RMBuf after deallocating 2 2048-byte long RMBufs:"));	
	mbuf5.Free();	// 2048-byte mbuf
	mbuf6.Free();	// 2048-byte mbuf
	
	ret = aChain.Alloc(2100, allocator);
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Error: Couldn't allocate RMBuf:"));
		thirdSuccess = EFalse;
		}
	else
		{
		//Copy in Des1 into Chain
		aChain.CopyIn(aDes1->LeftTPtr(2100));
		if (aChain.Length() != 2100)
			{
			INFO_PRINTF1(_L("Error: Allocated RMBufChain size does not match requested:"));
			thirdSuccess = EFalse;
			}
		}
		
	bChain.Append(aChain);
	
	mbuf1.Free();
	bChain.Free();
	
	delete aDes1;
		
	//-------------- substep 7 --------------------
	INFO_PRINTF1(_L("  07 Clean up stack:"));
    CleanupStack::PopAndDestroy();
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);
	
	if (ret != KErrNone || failedSizeCount > 0 || !firstSuccess || !secondSuccess || !thirdSuccess)
		{
        SetTestStepResult(EFail);
		return TestStepResult();
		}
	
    SetTestStepResult(EPass);
	return TestStepResult();
	}
