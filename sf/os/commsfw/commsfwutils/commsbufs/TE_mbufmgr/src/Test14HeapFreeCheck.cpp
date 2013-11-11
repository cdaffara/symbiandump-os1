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
// Contains MBufMgr Test Step 14 that check rmbud does not use heap allocation
// 
//

// EPOC includes
#include <e32base.h>
//#include <e32cmn.h>	// maybe this is needed for eka2 ?
#include <e32std.h>

// Test system includes
//#ifdef SYMBIAN_OLD_EXPORT_LOCATION
//#include "networking/log.h"
//#include "networking/teststep.h"
//#else
//#include <networking/log.h>
//#include <networking/teststep.h>
//#endif
#include "TestStepCTMbufmgr.h"

#include "Test14HeapFreeCheck.h"
#include <comms-infras/commsbufpond.h>
// constructor
CTest14HeapFreeCheck::CTest14HeapFreeCheck()
	{
	SetTestStepName(_L("MBufMgrTest14"));// Store the name of this test case
	}

// destructor
CTest14HeapFreeCheck::~CTest14HeapFreeCheck()	
	{
	}

// 
enum TVerdict CTest14HeapFreeCheck::doTestStepL(void)
	{
	SetTestStepResult(EFail);
#if defined (_DEBUG)
	//-------------- substep 1 -------------------- 
	INFO_PRINTF1(_L("  01 Creating CMBufManager and installing active scheduler:"));
    CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);
	CreateInstanceMBufMgrL(KMBufDefaultHeapSize);
	CleanupClosePushL(iBufPond);

	//-------------- substep 2 -------------------- 
	INFO_PRINTF1(_L("  02 Allocating two descriptors (Des1 & Des2):"));
	TBuf8<1000> *aDes1, *aDes2;
    CleanupStack::PushL( aDes1 = new(ELeave) TBuf8<1000> );
    CleanupStack::PushL( aDes2 = new(ELeave) TBuf8<1000> );
	aDes1->SetLength(1000);
	aDes2->SetLength(1000);

	//-------------- substep 2 -------------------- 
	INFO_PRINTF1(_L("  02 Set User heap to fail at the next alloc.:"));
	User::__DbgSetAllocFail(RHeap::EUser, RHeap::EFailNext, 0);

	//-------------- substep 3 -------------------- 
	INFO_PRINTF1(_L("  03 Allocate Chain1:"));
	RMBufChain aChain1;
	TRAPD(ret,aChain1.AllocL(1000));
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Error: Couldn't allocate Chain1:"));
		User::__DbgSetAllocFail(RHeap::EUser, RHeap::ENone, 0);
		User::Leave(EFail);
		}

	//-------------- substep 4 -------------------- 
	INFO_PRINTF1(_L("  04 Copy in Des1 into Chain1:"));
	aChain1.CopyIn(*aDes1);

	//-------------- substep 5 -------------------- 
	INFO_PRINTF1(_L("  05 Split Chain1. The 2nd part goes to Chain2:"));
	RMBufChain aChain2;
	TRAP(ret,aChain1.SplitL(300, aChain2));
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Error: Couldn't Split"));
		aChain1.Free();
		aChain2.Free();
		User::__DbgSetAllocFail(RHeap::EUser, RHeap::ENone, 0);
		User::Leave(EFail);
		}

	//-------------- substep 6 -------------------- 
	INFO_PRINTF1(_L("  06 Copy out Chain1 into Des2:"));
	aChain1.CopyOut(*aDes2);

	//-------------- substep 7 -------------------- 
	INFO_PRINTF1(_L("  07 Copy Chain1 into Chain3:"));
    RMBufChain aChain3;
	aChain1.CopyL(aChain3,0, 200);

	//-------------- substep 8 -------------------- 
	INFO_PRINTF1(_L("  08 Free the chains. Clean up stack:"));
	aChain1.Free();
	aChain2.Free();
	aChain3.Free();
	User::__DbgSetAllocFail(RHeap::EUser, RHeap::ENone, 0);
    CleanupStack::PopAndDestroy(aDes2);
    CleanupStack::PopAndDestroy(aDes1);
    CleanupStack::PopAndDestroy();
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);
        SetTestStepResult(EPass);

	return TestStepResult();
#else
	INFO_PRINTF1(_L("Info: Test Disabled on release build"));
        SetTestStepResult(EPass);

	return TestStepResult();
#endif
	}
