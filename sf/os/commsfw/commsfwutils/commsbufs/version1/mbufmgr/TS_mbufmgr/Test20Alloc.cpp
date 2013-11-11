// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains definition of CTestStepCTMbufmgr which is the base class
// for all the Mbufmgr Test Step classes
//

#include <e32base.h>

// Test system includes
#include <networking/log.h>
#include <networking/teststep.h>
#include "TestStepCTMbufmgr.h"
#include "TestSuiteCTMbufmgr.h"

#include "Test20Alloc.h"

// constructor
CTest20Alloc::CTest20Alloc()
	{
	iTestStepName = _L("MBufMgrTest20");// Store the name of this test case
	}

// destructor
CTest20Alloc::~CTest20Alloc()
	{
	}

struct AllocTestStruct
{
	TInt min;
	TInt max;
	TInt allocSize;
	TInt expectedSize;
	TInt expectedBufCount;
	TInt expectedBufSizes[4];
};

const AllocTestStruct allocTests[] =
{
	{   64,   96,   96,    0, 0, {} },  // lower min nax range than pool range
	{ 1800, 2000,   96,    0, 0, {} },  // higher min nax range than pool range
	{ 1400, 1400,   96,    0, 0, {} },	// mid max range that doesn't match contain any pools
	{ 1600, 1600,   96,   96, 1, {1600} },  // highest size, allocate smaller
	{  128,  128,   96,   96, 1, {128} },  // lowest size, allocate smaller
	{  400,  400,   96,   96, 1, {400} },  // mid size, allocate smaller
	{ 1600, 1600, 3200, 3200, 2, {1600,1600} },  // highest size, allocate slightly higher
	{ 1600, 1600, 1604, 1604, 2, {1600,1600} },  // highest size, allocate slightly higher
	{  128,  128,  132,  132, 2, {128,128} },  // lowest size, allocate slightly higher
	{  400,  400,  260,  260, 1, {400} },  // mid size, allocate slightly higher
	{  128, 1600, 1604, 1604, 2, {1600,128} },  //
	{  128, 1600, 1728, 1728, 2, {1600,128} },  //
	{  128, 1600, 1856, 1856, 2, {1600,400} },  //
	{  128, 1600, 2000, 2000, 2, {1600,400} },  //
	{  128, 1600, 2128, 2128, 2, {1600,1600} },  //
	{  128, 1600, 2130, 2130, 2, {1600,1600} },  //
	{  128, 1600, 2398, 2398, 2, {1600,1600}, },  //
	{  128,  128,  300,  300, 3, {128,128,128} },  //
	{  128, 1600, 3208, 3208, 3, {1600,1600,128} },  // Edge case spotted in old algorithm by SMP work
    { 1600, 1600, 3208, 3208, 3, {1600,1600,1600} },  // Edge case spotted in old algorithm by SMP work

	{    0,    0,    0,    0,-1, {} }   // -1 indicates end of list
};

//
enum TVerdict CTest20Alloc::doTestStepL(void)
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
	CleanupStack::PushL(CreateInstanceMBufMgrL(KMBufDefaultHeapSize / 2, MBufSizeAllocator));

	// Create some initial pools
	MBufSizeAllocator->AddL(128,  128, 64, 48);
	MBufSizeAllocator->AddL(400,  32,  16, 12);
	MBufSizeAllocator->AddL(1600, 32,  16, 12);

	TBool accumulatedFail = EFalse;

	//-------------- substep 2 --------------------
	Log(_L("  02 Alloc Tests"));
	const AllocTestStruct* test = allocTests;
	while(test->expectedBufCount != -1)
	    {
		TBool fail = EFalse;
		RMBufChain chain;

		//-------------- substep 2 --------------------
		_LIT(aLog1, "  Min %d Max %d Alloc %d");
		Log(aLog1,test->min,test->max,test->allocSize);
		chain.Alloc(test->allocSize,test->min,test->max);

		if(chain.Length() != test->expectedSize)
			{
		    _LIT(aLog2, "Error: Wrong number of bytes allocated: received %d expected %d");
			Log(aLog2,chain.Length(),test->expectedSize);
			fail = ETrue;
			}
		if( (!chain.IsEmpty() && chain.NumBufs() != test->expectedBufCount)
		 || ( chain.IsEmpty() &&               0 != test->expectedBufCount)
		  )
			{
		    _LIT(aLog3, "Error: Wrong number of buffers allocated: received %d expected %d");
			Log(aLog3,chain.IsEmpty()?0:chain.NumBufs(),test->expectedBufCount);
			fail = ETrue;
		    }

        if(!chain.IsEmpty())
            {
			RMBuf* buf = chain.First();
			int count=0;
			while(count<chain.NumBufs() && buf)
				{
				if(buf->Size() != test->expectedBufSizes[count])
				   {
					_LIT(aLog4, "Error: Expected buffer %d in the chain to be %d rather than %d");
					Log(aLog4,count,test->expectedBufSizes[count],buf->Size());
				   fail = ETrue;
			       }

				buf = buf->Next();
				count++;
				}
			}

        accumulatedFail |= fail;
		chain.Free();

		++test;
    	}

	//-------------- substep 7 --------------------
	Log(_L("  03 Clean up stack:"));
    CleanupStack::PopAndDestroy(iMBMngr);
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);

#ifdef __CFLOG_ACTIVE
	__CFLOG_CLOSE;
	__CFLOG_DELETE;
#endif
    __UHEAP_MARKEND;

    if (accumulatedFail)
    	{
    	return EFail;
    	}

	return EPass;
	}
