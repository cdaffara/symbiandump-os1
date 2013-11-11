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
// Contains MBufMgr Test Step 02 Allocate/Dealocate RMBuf
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
#include "Test02AllocDealloc.h"
#include <comms-infras/commsbufpond.h>

// constructor
CTest02AllocDealloc::CTest02AllocDealloc()
	{
	SetTestStepName(_L("MBufMgrTest02"));// Store the name of this test case
	}

// destructor
CTest02AllocDealloc::~CTest02AllocDealloc() 
	{
	}

// 
enum TVerdict CTest02AllocDealloc::doTestStepL(void)
	{
	SetTestStepResult(EFail);
	//-------------- substep 1 -------------------- 
	INFO_PRINTF1(_L("  01 Create CMBufManager and install active scheduler:"));
    CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);
	CreateInstanceMBufMgrL(KMBufDefaultHeapSize);
	CleanupClosePushL(iBufPond);

	TBool firstFail = EFalse;
	TBool secondFail = EFalse;

	//-------------- substep 2 -------------------- 
	INFO_PRINTF1(_L("  02 Allocate 5000-bytes long RMBuf:"));
	RMBuf* buf=static_cast<RMBuf*>(iBufPond.Alloc(5000, 0, KMaxTInt));
	if (buf == NULL)
		{
		INFO_PRINTF1(_L("Error: Couldn't allocate RMBuf:"));
		firstFail = ETrue;
		}
	else
		{
		//-------------- substep 3 -------------------- 
		INFO_PRINTF1(_L("  03 Deallocating RMBuf"));
		buf->Free();
		}
	//-------------- substep 4 --------------------
	INFO_PRINTF1(_L("  04 Allocate 0-byte long RMBuf:"));
	buf = static_cast<RMBuf*>(iBufPond.Alloc(0, 0, KMaxTInt));
	if (buf == NULL)
		{
		INFO_PRINTF1(_L("Error: Couldn't allocate RMBuf:"));
		secondFail = ETrue;
		}
	else
		{
		//-------------- substep 5 --------------------
		_LIT(aLog1, "  05 Allocated RMBuf size : %d");
		INFO_PRINTF2(aLog1, buf->Size());
		if (buf->Length() != 0)
			{
			_LIT(aLog2, "Error: RMBuf length : %d ");
			INFO_PRINTF2(aLog2, buf->Length());
			secondFail = ETrue;
			}
		else
			{
			_LIT(aLog3, "  Info: RMBuf length : %d ");
			INFO_PRINTF2(aLog3, buf->Length());
			}
		//-------------- substep 6 --------------------
		INFO_PRINTF1(_L("  06 Deallocating RMBuf"));
		buf->Free();
		}
	//-------------- substep 7 --------------------
	INFO_PRINTF1(_L("  07 Clean up stack:"));
    CleanupStack::PopAndDestroy();  // iBufPond
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);

    if (firstFail || secondFail)
    	{
    	SetTestStepResult(EFail);
    	return TestStepResult();
    	}

	SetTestStepResult(EPass);
	return TestStepResult();
	}
