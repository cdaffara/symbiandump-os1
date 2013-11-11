// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test19Align.cpp
// Contains MBufMgr Test Step 19 for Prepend() methods
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <networking/log.h>
#include <networking/teststep.h>
#include "TestStepCTMbufmgr.h"
#include "TestSuiteCTMbufmgr.h"

#include "Test19Prepend.h"

// constructor
CTest19Prepend::CTest19Prepend()
	{
	iTestStepName = _L("MBufMgrTest19");// Store the name of this test case
	}

// destructor
CTest19Prepend::~CTest19Prepend()
	{
	}

//
enum TVerdict CTest19Prepend::doTestStepL(void)
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
	MBufSizeAllocator->AddL(128,128,64,40);

	TBool firstFail = EFalse;
	TBool secondFail = EFalse;
	TInt ret;
	RMBufChain chain;

	//-------------- substep 2 --------------------
	Log(_L("  02 Prepend 20 bytes to an empty chain"));
	ret = chain.Prepend(20);
	if (ret != KErrNone)
		{
		Log(_L("Error: Couldn't prepend:"));
		firstFail = ETrue;
		}
	else
		{
	    if(chain.NumBufs() != 1)
	        {
		    Log(_L("Error: Wrong number of MBufs allocated:"));
			secondFail = ETrue;
		    }
		if(chain.Length() != 20)
			{
		    Log(_L("Error: Wrong number of bytes prepended:"));
			secondFail = ETrue;
			}
		//-------------- substep 3 --------------------
		Log(_L("  03 Deallocating RMBufChain"));
		chain.Free();
		}

	//-------------- substep 4 --------------------
	Log(_L("  04 Prepend 200 bytes to an empty chain"));
    ret = chain.Prepend(200);
	if (ret != KErrNone)
		{
		Log(_L("Error: Couldn't prepend:"));
		firstFail = ETrue;
		}
	else
		{
	    if(chain.NumBufs() != 2)
	        {
		    Log(_L("Error: Wrong number of MBufs allocated:"));
			secondFail = ETrue;
		    }
		if(chain.Length() != 200)
			{
		    Log(_L("Error: Wrong number of bytes prepended:"));
			secondFail = ETrue;
			}
		//-------------- substep 6 --------------------
		Log(_L("  05 Deallocating RMBufChain"));
		chain.Free();
		}
	//-------------- substep 6 --------------------
	Log(_L("  06 Prepend 20 bytes to an existing chain"));
	ret = chain.Alloc(20);
	if (ret != KErrNone)
	    {
        Log(_L("Error: Couldn't allocate initial chain:"));
        firstFail = ETrue;
	    }
    else
        {
		ret = chain.Prepend(20);
		if (ret != KErrNone)
			{
			Log(_L("Error: Couldn't prepend:"));
			firstFail = ETrue;
			}
		else
			{
			if(chain.NumBufs() != 2)
				{
				Log(_L("Error: Wrong number of MBufs allocated:"));
				secondFail = ETrue;
				}
			if(chain.Length() != 40)
				{
				Log(_L("Error: Wrong number of bytes:"));
				secondFail = ETrue;
				}
		    //-------------- substep 7 --------------------
			Log(_L("  07 Deallocating RMBufChain"));
			chain.Free();
			}
		}
	//-------------- substep 6 --------------------
	Log(_L("  08 Prepend 200 bytes to an existing chain"));
	ret = chain.Alloc(20);
	if (ret != KErrNone)
	    {
        Log(_L("Error: Couldn't allocate initial chain:"));
        firstFail = ETrue;
	    }
    else
        {
		ret = chain.Prepend(200);
		if (ret != KErrNone)
			{
			Log(_L("Error: Couldn't prepend:"));
			firstFail = ETrue;
			}
		else
			{
			if(chain.NumBufs() != 3)
				{
				Log(_L("Error: Wrong number of MBufs allocated:"));
				secondFail = ETrue;
				}
			if(chain.Length() != 220)
				{
				Log(_L("Error: Wrong number of bytes:"));
				secondFail = ETrue;
				}
		    //-------------- substep 7 --------------------
			Log(_L("  09 Deallocating RMBufChain"));
			chain.Free();
			}
		}
	//-------------- substep 7 --------------------
	Log(_L("  10 Clean up stack:"));
    CleanupStack::PopAndDestroy(iMBMngr);
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);

#ifdef __CFLOG_ACTIVE
	__CFLOG_CLOSE;
	__CFLOG_DELETE;
#endif
    __UHEAP_MARKEND;

    if (firstFail || secondFail)
    	{
    	return EFail;
    	}

	return EPass;
	}

