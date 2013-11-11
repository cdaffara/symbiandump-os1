// Copyright (c) 2008 Symbian Ltd.  All rights reserved.
//
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
//#ifdef SYMBIAN_OLD_EXPORT_LOCATION
//#include "networking/log.h"
//#include "networking/teststep.h"
//#else
//#include <networking/log.h>
//#include <networking/teststep.h>
//#endif
#include "TestStepCTMbufmgr.h"

#include "Test03AllocLeave.h"
#include <comms-infras/commsbufpond.h>

// constructor
CTest03AllocLeave::CTest03AllocLeave()
	{
	SetTestStepName(_L("MBufMgrTest03"));// Store the name of this test case
	}

// destructor
CTest03AllocLeave::~CTest03AllocLeave() 
	{
	}

// 
enum TVerdict CTest03AllocLeave::doTestStepL(void)
	{
	SetTestStepResult(EFail);
	//-------------- substep 1 -------------------- 
	INFO_PRINTF1(_L("  01 Create CMBufManager (with 15K alloc memory) and install active scheduler:"));
    CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);

	CreateInstanceMBufMgrL(65536);
	CleanupClosePushL(iBufPond);
	//-------------- substep 2 --------------------
	RMBuf* mbufs[14];
	TInt i;
	INFO_PRINTF1(_L("  02 Allocate 42000-bytes long worth of mbufs"));
	for (i = 0; i < sizeof(mbufs)/sizeof(mbufs[0]); ++i)
		{
		mbufs[i] = static_cast<RMBuf*>(iBufPond.Alloc(3000, 0, KMaxTInt));
		if (mbufs[i] == NULL)
			{
			INFO_PRINTF1(_L("Error: Couldn't allocate RMBuf:"));
			User::Leave(EFail);
			}	
		}
	

	//-------------- substep 3 -------------------- 
	INFO_PRINTF1(_L("  03 Allocate 8192-bytes long RMBuf2:"));
	RMBuf* aMBuf2 = NULL;
	RMBufChain chain;
	TInt ret;
	TRAP(ret,chain.AllocL(16384);)
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Info: Couldn't allocate RMBuf as it should be so."));
		if (ret == KErrNoMBufs)
			{
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF2(_L("Error: The reason AllocL has failed with %d"),KErrNoMBufs);
			SetTestStepResult(EFail);

			}
		}
	//INFO_PRINTF2(_L("  Heap Size after failed alloc %d"),iMBMngr->__DbgGetHeapSize());
	// Cleanup .
	//-------------- substep 4 -------------------- 
	INFO_PRINTF1(_L("  04 Deallocate RMBufs (1, 2 & 3):"));
	for (i = 0; i < sizeof(mbufs)/sizeof(mbufs[0]); ++i)
		{
		mbufs[i]->Free();
		}

	if (aMBuf2 != NULL)
		aMBuf2->Free();

	//-------------- substep 5 -------------------- 
	INFO_PRINTF1(_L("  05 Clean up stack:"));
    CleanupStack::PopAndDestroy();
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);
	return TestStepResult();
}
