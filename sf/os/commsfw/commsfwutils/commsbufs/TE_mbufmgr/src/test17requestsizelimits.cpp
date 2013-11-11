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
// Contains MBufMgr Test Step 17 for getting mbufs within request size limits
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

#include "test17requestsizelimits.h"

// RHeap information
#include <comms-infras/mbufallocator.h>
#include <comms-infras/commsbufpond.h>

CTest17RequestSizeLimits::CTest17RequestSizeLimits()
	{
	SetTestStepName( _L("MBufMgrTest17") );	// Store the name of this test case
	}

CTest17RequestSizeLimits::~CTest17RequestSizeLimits()
	{
	}

void CTest17RequestSizeLimits::FillDes(TDes8 &aBuf, TInt aStartPos, TInt anEndPos, TUint aStartChar, TUint anEndChar)
	{
	StripeDes(aBuf, aStartPos, anEndPos, aStartChar, anEndChar);
	}

enum TVerdict CTest17RequestSizeLimits::doTestStepL(void)
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
	TInt poolSize = KMBufDefaultHeapSize/totalPools;	
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
	
	//-------------- substep 2 --------------------
	INFO_PRINTF1(_L("  02 Fill in the Des1 with a pattern:"));
	TBuf8<5000> *aDes1 = NULL;
	TRAPD(ret, aDes1 = new(ELeave) TBuf8<5000>);
	if(ret != KErrNone)
		return EFail;

	aDes1->SetLength(5000);
	FillDes(*aDes1, 0, 5000, 1, 1);
	
	TBool firstAllocFail = EFalse;
	TBool secondAllocFail = EFalse;
	TBool thirdAllocFail = EFalse;
	
	TUint time;
	RMBufChain aChain;
		
	//-------------- substep 3 --------------------
	INFO_PRINTF1(_L("  03 Allocate 5000-bytes long RMBufChain with 128-bytes long RMBuf:"));

	time = User::FastCounter();
	ret = aChain.Alloc(5000, 128, 128, allocator);
	time = User::FastCounter() - time;
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Error: Couldn't allocate RMBufChain:"));
		User::Leave(EFail);
		}
	
	//Copy in Des1 into Chain
	aChain.CopyIn(aDes1->LeftTPtr(5000));
	if (aChain.Length() != 5000)
		{
		INFO_PRINTF1(_L("Error: Allocated RMBufChain size does not match requested:"));
		firstAllocFail = ETrue;
		}
	
	TInt count = 0;
	//Check RMBuf size is really 128
	RMBuf* mBuf = aChain.First();
	while (mBuf != NULL)
		{
		count++;
		if (mBuf->Size() != 128)
			{
			INFO_PRINTF1(_L("Error: RMBuf size does not match requested:"));
			firstAllocFail = ETrue;
			break;
			}
		mBuf = mBuf->Next();
		}
	
	if (firstAllocFail == EFalse)
		{
		_LIT(aLog1, "     Info: Time to allocate 5000-bytes long RMBufChain with %d 128-bytes long RMBuf: %d"); 
		INFO_PRINTF3(aLog1, count, time);
		}
	
	//-------------- substep 4 --------------------
	INFO_PRINTF1(_L("  04 Deallocating RMBufChain"));
	aChain.Free();

	//-------------- substep 5 --------------------
	INFO_PRINTF1(_L("  05 Allocate 50-bytes long RMBufChain with 1024-bytes long RMBuf:"));

	time = User::FastCounter();
	ret = aChain.Alloc(50, 1024, 1024, allocator);
	time = User::FastCounter() - time;
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Error: Couldn't allocate RMBufChain:"));
		User::Leave(EFail);
		}
	
	//Copy in Des1 into Chain
	aChain.CopyIn(aDes1->LeftTPtr(50));
	if (aChain.Length() != 50)
		{
		INFO_PRINTF1(_L("Error: Allocated RMBufChain size does not match requested:"));
		secondAllocFail = ETrue;
		}
	
	count = 0;
	//Check RMBuf size is really 1024
	mBuf = aChain.First();
	while (mBuf != NULL)
		{
		count++;
		if (mBuf->Size() != 1024)
			{
			INFO_PRINTF1(_L("Error: RMBuf size does not match requested:"));
			secondAllocFail = ETrue;
			break;
			}
		mBuf = mBuf->Next();
		}
	
	if (secondAllocFail == EFalse)
		{
		_LIT(aLog2, "     Info: Time to allocate 50-bytes long RMBufChain with %d 1024-bytes long RMBuf: %d"); 
		INFO_PRINTF3(aLog2, count, time);
		}
	
	//-------------- substep 6 --------------------
	INFO_PRINTF1(_L("  06 Deallocating RMBufChain"));
	aChain.Free();
	
	//-------------- substep 7 --------------------
	INFO_PRINTF1(_L("  07 Allocate 1500-bytes long RMBufChain with RMBuf size between 1100 and 2000:"));

	ret = aChain.Alloc(1500, 1100, 2000, allocator);
	if (ret == KErrNone)
		{
		INFO_PRINTF1(_L("Error: Allocated with false size RMBuf:"));
		thirdAllocFail = ETrue;
		aChain.Free();
		}
	else
		INFO_PRINTF1(_L("     Info: Couldn't allocate because no MBuf with size between 1100 and 2000:"));

	//-------------- substep 8 --------------------
	INFO_PRINTF1(_L("  08 Allocate 1500-bytes long RMBufChain with RMBuf min. size 2100:"));

	ret = aChain.Alloc(1500, 2100, allocator);
	if (ret == KErrNone)
		{
		INFO_PRINTF1(_L("Error: Allocated with false size RMBuf:"));
		thirdAllocFail = ETrue;
		aChain.Free();
		}
	else
		INFO_PRINTF1(_L("     Info: Couldn't allocate because no MBuf with size greater than 2100:"));
	
	//-------------- substep 9 --------------------
	INFO_PRINTF1(_L("  09 Allocate 1500-bytes long RMBufChain with RMBuf max. size 100:"));

	ret = aChain.Alloc(1500, 0, 100, allocator);
	if (ret == KErrNone)
		{
		INFO_PRINTF1(_L("Error: Allocated with false size RMBuf:"));
		thirdAllocFail = ETrue;
		aChain.Free();
		}
	else
		INFO_PRINTF1(_L("     Info: Couldn't allocate because no MBuf with size smaller than 100:"));

	//-------------- substep 10 --------------------
	INFO_PRINTF1(_L("  10 Exhaust 512-bytes long RMBufChain :"));
	
	RMBufChain bChain;
	
	while (allocator.BytesAvailable(512) / 512 > 0)
		{
		ret = aChain.Alloc(500, allocator);
		if (ret != KErrNone)
			{
			INFO_PRINTF1(_L("Error: Couldn't allocate RMBufChain:"));
			User::Leave(EFail);
			}
	
		// Now grow the chain
		bChain.Append(aChain);
		}
	
	//-------------- substep 11 --------------------
	INFO_PRINTF1(_L("  15 Allocate 500-bytes long RMBufChain with 512-bytes long RMBuf:"));

	ret = aChain.Alloc(500, 512, 512, allocator);
	if (ret == KErrNone)
		{
		INFO_PRINTF1(_L("Error: Allocated with false size RMBuf:"));
		thirdAllocFail = ETrue;
		aChain.Free();
		}
	else
		INFO_PRINTF1(_L("     Info: Couldn't allocate because no more 512-bytes long RMBuf:"));
	
	//-------------- substep 12 --------------------
	INFO_PRINTF1(_L("  12 Deallocating RMBufChain"));
	bChain.Free();

	delete aDes1;
	
	//-------------- substep 13 --------------------
	INFO_PRINTF1(_L("  13 Clean up stack:"));
   	CleanupStack::PopAndDestroy();
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);
	
	if (firstAllocFail || secondAllocFail || thirdAllocFail)
		{
        SetTestStepResult(EFail);
		return TestStepResult();
		}
		
    SetTestStepResult(EPass);
	return TestStepResult();
	}
