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
#include <networking/log.h>
#include <networking/teststep.h>
#include "TestStepCTMbufmgr.h"
#include "TestSuiteCTMbufmgr.h"

#include "test17requestsizelimits.h"

// RHeap information
#include <comms-infras/mbufallocator.h>

CTest17RequestSizeLimits::CTest17RequestSizeLimits()
	{
	iTestStepName = _L("MBufMgrTest17");// Store the name of this test case
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

	// Create 262144
	CleanupStack::PushL(CreateInstanceMBufMgrL(KMBufDefaultHeapSize, MBufSizeAllocator));

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
	
	//-------------- substep 2 --------------------
	Log(_L("  02 Fill in the Des1 with a pattern:"));
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
	Log(_L("  03 Allocate 5000-bytes long RMBufChain with 128-bytes long RMBuf:"));

	time = User::FastCounter();
	ret = aChain.Alloc(5000, 128, 128, allocator);
	time = User::FastCounter() - time;
	if (ret != KErrNone)
		{
		Log(_L("Error: Couldn't allocate RMBufChain:"));
		User::Leave(EFail);
		}
	
	//Copy in Des1 into Chain
	aChain.CopyIn(aDes1->LeftTPtr(5000));
	if (aChain.Length() != 5000)
		{
		Log(_L("Error: Allocated RMBufChain size does not match requested:"));
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
			Log(_L("Error: RMBuf size does not match requested:"));
			firstAllocFail = ETrue;
			break;
			}
		mBuf = mBuf->Next();
		}
	
	if (firstAllocFail == EFalse)
		{
		_LIT(aLog1, "     Info: Time to allocate 5000-bytes long RMBufChain with %d 128-bytes long RMBuf: %d"); 
		Log(aLog1, count, time);
		}
	
	//-------------- substep 4 --------------------
	Log(_L("  04 Deallocating RMBufChain"));
	aChain.Free();

	//-------------- substep 5 --------------------
	Log(_L("  05 Allocate 50-bytes long RMBufChain with 1024-bytes long RMBuf:"));

	time = User::FastCounter();
	ret = aChain.Alloc(50, 1024, 1024, allocator);
	time = User::FastCounter() - time;
	if (ret != KErrNone)
		{
		Log(_L("Error: Couldn't allocate RMBufChain:"));
		User::Leave(EFail);
		}
	
	//Copy in Des1 into Chain
	aChain.CopyIn(aDes1->LeftTPtr(50));
	if (aChain.Length() != 50)
		{
		Log(_L("Error: Allocated RMBufChain size does not match requested:"));
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
			Log(_L("Error: RMBuf size does not match requested:"));
			secondAllocFail = ETrue;
			break;
			}
		mBuf = mBuf->Next();
		}
	
	if (secondAllocFail == EFalse)
		{
		_LIT(aLog2, "     Info: Time to allocate 50-bytes long RMBufChain with %d 1024-bytes long RMBuf: %d"); 
		Log(aLog2, count, time);
		}
	
	//-------------- substep 6 --------------------
	Log(_L("  06 Deallocating RMBufChain"));
	aChain.Free();
	
	//-------------- substep 7 --------------------
	Log(_L("  07 Allocate 1500-bytes long RMBufChain with RMBuf size between 1100 and 2000:"));

	ret = aChain.Alloc(1500, 1100, 2000, allocator);
	if (ret == KErrNone)
		{
		Log(_L("Error: Allocated with false size RMBuf:"));
		thirdAllocFail = ETrue;
		aChain.Free();
		}
	else
		Log(_L("     Info: Couldn't allocate because no MBuf with size between 1100 and 2000:"));

	//-------------- substep 8 --------------------
	Log(_L("  08 Allocate 1500-bytes long RMBufChain with RMBuf min. size 2100:"));

	ret = aChain.Alloc(1500, 2100, allocator);
	if (ret == KErrNone)
		{
		Log(_L("Error: Allocated with false size RMBuf:"));
		thirdAllocFail = ETrue;
		aChain.Free();
		}
	else
		Log(_L("     Info: Couldn't allocate because no MBuf with size greater than 2100:"));
	
	//-------------- substep 9 --------------------
	Log(_L("  09 Allocate 1500-bytes long RMBufChain with RMBuf max. size 100:"));

	ret = aChain.Alloc(1500, 0, 100, allocator);
	if (ret == KErrNone)
		{
		Log(_L("Error: Allocated with false size RMBuf:"));
		thirdAllocFail = ETrue;
		aChain.Free();
		}
	else
		Log(_L("     Info: Couldn't allocate because no MBuf with size smaller than 100:"));

	//-------------- substep 10 --------------------
	Log(_L("  10 Exhaust 512-bytes long RMBufChain :"));
	
	RMBufChain bChain;
	
	while (allocator.BytesAvailable(512) / 512 > 0)
		{
		ret = aChain.Alloc(500, allocator);
		if (ret != KErrNone)
			{
			Log(_L("Error: Couldn't allocate RMBufChain:"));
			User::Leave(EFail);
			}
	
		// Now grow the chain
		bChain.Append(aChain);
		}
	
	//-------------- substep 11 --------------------
	Log(_L("  15 Allocate 500-bytes long RMBufChain with 512-bytes long RMBuf:"));

	ret = aChain.Alloc(500, 512, 512, allocator);
	if (ret == KErrNone)
		{
		Log(_L("Error: Allocated with false size RMBuf:"));
		thirdAllocFail = ETrue;
		aChain.Free();
		}
	else
		Log(_L("     Info: Couldn't allocate because no more 512-bytes long RMBuf:"));
	
	//-------------- substep 12 --------------------
	Log(_L("  12 Deallocating RMBufChain"));
	bChain.Free();

	delete aDes1;
	
	//-------------- substep 13 --------------------
	Log(_L("  13 Clean up stack:"));
    	CleanupStack::PopAndDestroy(iMBMngr);
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);
	
#ifdef __CFLOG_ACTIVE
	__CFLOG_CLOSE;
	__CFLOG_DELETE;
#endif

	if (firstAllocFail || secondAllocFail || thirdAllocFail)
		return EFail;
	
	__UHEAP_MARKEND;
	return EPass;
	}
