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
// Contains MBufMgr Test Step 03
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <networking/log.h>
#include <networking/teststep.h>
#include "TestStepCTMbufmgr.h"
#include "TestSuiteCTMbufmgr.h"

#include "Test03AllocLeave.h"


// constructor
CTest03AllocLeave::CTest03AllocLeave()
	{
	iTestStepName = _L("MBufMgrTest03");// Store the name of this test case
	}

// destructor
CTest03AllocLeave::~CTest03AllocLeave()
	{
	}

//
enum TVerdict CTest03AllocLeave::doTestStepL(void)
	{
	__UHEAP_MARK;
			
#ifdef __CFLOG_ACTIVE
	__CFLOG_CREATEL;
	__CFLOG_OPEN;
#endif
	
	TVerdict verdict(EFail);
	//-------------- substep 1 --------------------
	Log(_L("  01 Create CMBufManager (with 15K alloc memory) and install active scheduler:"));
	CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);

	CleanupStack::PushL(CreateInstanceMBufMgrL(65536));
	if (!iMBMngr)
		{
		Log(_L("Error:Could not create MBuf manager"));

#ifdef __CFLOG_ACTIVE
		__CFLOG_CLOSE;
		__CFLOG_DELETE;
#endif
		User::Leave(EFail);
		}

	//-------------- substep 2 --------------------
	RMBuf* mbufs[14];
	TInt i;
	TInt ret;
	Log(_L("  02 Allocate 42000-bytes long worth of mbufs"));
	for (i = 0; i < sizeof(mbufs)/sizeof(mbufs[0]); ++i)
		{
		TRAP(ret,mbufs[i] = iMBMngr->AllocL(3000));
		Log(_L("  Heap Size after %d allocs : %d"),i,iMBMngr->__DbgGetHeapSize());
		if (ret != KErrNone)
			{
			Log(_L("Error: Couldn't allocate RMBuf:"));

#ifdef __CFLOG_ACTIVE
			__CFLOG_CLOSE;
			__CFLOG_DELETE;
#endif
			User::Leave(EFail);
			}
		}


	//-------------- substep 3 --------------------
	Log(_L("  03 Allocate 8192-bytes long RMBuf2:"));
	RMBuf* aMBuf2=0;
	TRAP(ret,aMBuf2 = iMBMngr->AllocL(16384);)
	if (ret != KErrNone)
		{
		Log(_L("Info: Couldn't allocate RMBuf as it should be so."));
		if (ret == KErrNoMBufs)
			verdict =  EPass;
		else
			{
			Log(_L("Error: The reason AllocL has failed is %d instead of %d"),ret,KErrNoMBufs);
			verdict =  EFail;
			}
		}
	Log(_L("  Heap Size after failed alloc %d"),iMBMngr->__DbgGetHeapSize());
	// Cleanup .
	//-------------- substep 4 --------------------
	Log(_L("  04 Deallocate RMBufs (1, 2 & 3):"));
	for (i = 0; i < sizeof(mbufs)/sizeof(mbufs[0]); ++i)
		{
		iMBMngr->Free(mbufs[i]);
		}

	if (aMBuf2 != NULL)
		iMBMngr->Free(aMBuf2);

	//-------------- substep 5 --------------------
	Log(_L("  05 Clean up stack:"));
	CleanupStack::PopAndDestroy(iMBMngr);
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);

#ifdef __CFLOG_ACTIVE
	__CFLOG_CLOSE;
	__CFLOG_DELETE;
#endif
	__UHEAP_MARKEND;
	return verdict;
}
