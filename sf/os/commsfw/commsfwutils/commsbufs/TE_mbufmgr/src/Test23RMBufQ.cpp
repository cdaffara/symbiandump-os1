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
// Test23RMBufQ.cpp
// Contains MBufMgr Test Step 22 for Append() methods
//
//

// EPOC includes
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

#include "Test23RMBufQ.h"

// constructor
CTest23RMBufQ::CTest23RMBufQ()
	{
	SetTestStepName(_L("MBufMgrTest23"));// Store the name of this test case
	}

// destructor
CTest23RMBufQ::~CTest23RMBufQ()
	{
	}

//
enum TVerdict CTest23RMBufQ::doTestStepL(void)
	{
	SetTestStepResult(EFail);

	//-------------- substep 1 --------------------
	INFO_PRINTF1(_L("  01 Create CMBufManager and install active scheduler:"));
    CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);

	RArray<TCommsBufPoolCreateInfo> poolInfoArray;
	TCommsBufPoolCreateInfo createInfo;
	createInfo.iBufSize = 128;
	createInfo.iInitialBufs = 128;
	createInfo.iGrowByBufs = 64;
	createInfo.iMinFreeBufs = 40;
	createInfo.iCeiling = (KMBufDefaultHeapSize / 2)/createInfo.iBufSize;
	poolInfoArray.AppendL(createInfo);
	CreateInstanceMBufMgrL(poolInfoArray);
	CleanupClosePushL(iBufPond);
	poolInfoArray.Close ();

	TBool firstFail = EFalse;
	TBool secondFail = EFalse;
	TInt ret;
	RMBufChain chain,chain2;

	//-------------- substep 2 --------------------
	INFO_PRINTF1(_L("  2 Append a chain onto an RMBuqQ"));
	ret = chain.Alloc(20);
	if (ret != KErrNone)
	    {
        INFO_PRINTF1(_L("Error: Couldn't allocate initial chain:"));
        firstFail = ETrue;
	    }
    else
        {
		ret = chain2.Alloc(20);
		if (ret != KErrNone)
			{
			INFO_PRINTF1(_L("Error: Couldn't allocate second chain:"));
			firstFail = ETrue;
			}
		else
			{
			RMBuf* first2 = chain2.First();
			RMBuf* first = chain.First();

			RMBufQ aBufList;
			aBufList.Assign(chain);

			aBufList.Append(chain2);

			// Check that the buffers come back out of the queue in the right order
			if(aBufList.Remove() != first)
				{
				INFO_PRINTF1(_L("Error: chain not appended to RMBufQ correctly:"));
				secondFail = ETrue;
				}
			if(aBufList.Remove() != first2)
				{
				INFO_PRINTF1(_L("Error: chain not appended to RMBufQ correctly:"));
				secondFail = ETrue;
				}
			//-------------- substep 3 --------------------
			INFO_PRINTF1(_L("  3 Deallocating RMBufChains"));
			chain2.Free();
		    }
		chain.Free();
		}
	//-------------- substep 4 --------------------
	INFO_PRINTF1(_L("  4 Prepend a chain onto an RMBuqQ"));
	ret = chain.Alloc(20);
	if (ret != KErrNone)
	    {
        INFO_PRINTF1(_L("Error: Couldn't allocate initial chain:"));
        firstFail = ETrue;
	    }
    else
        {
		ret = chain2.Alloc(20);
		if (ret != KErrNone)
			{
			INFO_PRINTF1(_L("Error: Couldn't allocate second chain:"));
			firstFail = ETrue;
			}
		else
			{
			RMBuf* first2 = chain2.First();
			RMBuf* first = chain.First();

			RMBufQ aBufList;
			aBufList.Assign(chain);

			aBufList.Prepend(chain2);

			// Check that the buffers come back out of the queue in the right order
			if(aBufList.Remove() != first2)
				{
				INFO_PRINTF1(_L("Error: chain not prepended to RMBufQ correctly:"));
				secondFail = ETrue;
				}
			if(aBufList.Remove() != first)
				{
				INFO_PRINTF1(_L("Error: chain not prepended to RMBufQ correctly:"));
				secondFail = ETrue;
				}
			//-------------- substep 5 --------------------
			INFO_PRINTF1(_L("  5 Deallocating RMBufChains"));
			chain2.Free();
		    }
		chain.Free();
		}
	//-------------- substep 6 ---------------------
	INFO_PRINTF1(_L("  6 Clean up stack:"));
    CleanupStack::PopAndDestroy(); // pond
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);
	
	if (firstFail || secondFail)
    	{
		SetTestStepResult(EFail);
    	}
	else
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}
