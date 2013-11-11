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
// Contains MBufMgr Test Step 11 Async. Allocate
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
#include "Test11AsyncAlloc.h"

#include <comms-infras/commsbufpond.h>
// constructor
CTest11AsyncAlloc::CTest11AsyncAlloc()
	{
	SetTestStepName(_L("MBufMgrTest11"));// Store the name of this test case
	}

// destructor
CTest11AsyncAlloc::~CTest11AsyncAlloc()	
	{
	}

class CMBufAsyncReq : public CActive
{
public:
	CMBufAsyncReq(RMBufAsyncRequest &aMBufAsyncReq)
		:CActive(EPriorityStandard)
		,iMBufAsyncReq(aMBufAsyncReq) 	{ }

	~CMBufAsyncReq() { Cancel(); }

	void RunL()      { CActiveScheduler::Stop(); }

	void DoCancel()  { iMBufAsyncReq.Cancel(); }

	TInt DoAsyncRequest(RMBufChain& aChain, TInt aSize)
		{
		iMBufAsyncReq.Alloc(aChain, aSize, iStatus);
		SetActive();
		CActiveScheduler::Start();
		return iStatus.Int();
		}
private:
	RMBufAsyncRequest& iMBufAsyncReq;
};

enum TVerdict CTest11AsyncAlloc::doTestStepL(void)
	{
	SetTestStepResult(EFail);
	//-------------- substep 1 -------------------- 
	INFO_PRINTF1(_L("  01 Create CMBufManager and install active scheduler:"));
    CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);
	CreateInstanceMBufMgrL(KMBufDefaultHeapSize);
	CleanupClosePushL(iBufPond);

	//-------------- substep 2 -------------------- 
	INFO_PRINTF1(_L("  02 Create and install active object that will do async. alloc on request:"));
	RMBufAsyncRequest async;
	CMBufAsyncReq* testAsync;
	CleanupStack::PushL(testAsync = new(ELeave) CMBufAsyncReq(async));
	CActiveScheduler::Add(testAsync);
	
	//-------------- substep 3 -------------------- 
	INFO_PRINTF1(_L("  03 Async. alloce 5000-bytes long RMBuf:"));
	RMBufChain aChain;
	TInt ret = testAsync->DoAsyncRequest(aChain, 5000);
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Error: Couldn't allocate Chain:"));
		User::Leave(EFail);
		}

	//-------------- substep 4 -------------------- 
	INFO_PRINTF1(_L("  04 Free the chain. Clean up stack:"));
	aChain.Free();
    CleanupStack::PopAndDestroy(testAsync);
    CleanupStack::PopAndDestroy();
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);
        SetTestStepResult(EPass);

	return TestStepResult();
	}
