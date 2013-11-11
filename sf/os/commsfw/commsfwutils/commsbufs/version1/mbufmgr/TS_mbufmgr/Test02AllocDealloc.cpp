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
#include <networking/log.h>
#include <networking/teststep.h>
#include "TestStepCTMbufmgr.h"
#include "TestSuiteCTMbufmgr.h"
#include "Test02AllocDealloc.h"

// constructor
CTest02AllocDealloc::CTest02AllocDealloc()
	{
	iTestStepName = _L("MBufMgrTest02");// Store the name of this test case
	}

// destructor
CTest02AllocDealloc::~CTest02AllocDealloc()
	{
	}

//
enum TVerdict CTest02AllocDealloc::doTestStepL(void)
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
	CleanupStack::PushL(CreateInstanceMBufMgrL(KMBufDefaultHeapSize));

	TBool firstFail = EFalse;
	TBool secondFail = EFalse;
	
	//-------------- substep 2 --------------------
	Log(_L("  02 Allocate 5000-bytes long RMBuf:"));
	RMBuf* buf=0;
	TRAPD(ret, buf = iMBMngr->AllocL(5000));
	if (ret != KErrNone)
		{
		Log(_L("Error: Couldn't allocate RMBuf:"));
		firstFail = ETrue;
		}
	else
		{
		//-------------- substep 3 --------------------
		Log(_L("  03 Deallocating RMBuf"));
		iMBMngr->Free(buf);
		}
	
	//-------------- substep 4 --------------------
	Log(_L("  04 Allocate 0-byte long RMBuf:"));
	TRAPD(ret1, buf = iMBMngr->AllocL(0));
	if (ret1 != KErrNone)
		{
		Log(_L("Error: Couldn't allocate RMBuf:"));
		secondFail = ETrue;
		}
	else
		{
		//-------------- substep 5 --------------------
		_LIT(aLog1, "  05 Allocated RMBuf size : %d");
		Log(aLog1, buf->Size());
		if (buf->Length() != 0)
			{
			_LIT(aLog2, "Error: RMBuf length : %d ");
			Log(aLog2, buf->Length());
			secondFail = ETrue;
			}
		else
			{
			_LIT(aLog3, "  Info: RMBuf length : %d ");
			Log(aLog3, buf->Length());
			}
		//-------------- substep 6 --------------------
		Log(_L("  06 Deallocating RMBuf"));
		iMBMngr->Free(buf);
		}
	//-------------- substep 7 --------------------
	Log(_L("  07 Clean up stack:"));
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
