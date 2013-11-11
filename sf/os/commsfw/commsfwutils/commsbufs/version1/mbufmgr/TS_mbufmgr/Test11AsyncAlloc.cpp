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
#include <networking/log.h>
#include <networking/teststep.h>
#include "TestStepCTMbufmgr.h"
#include "TestSuiteCTMbufmgr.h"
#include "Test11AsyncAlloc.h"


// constructor
CTest11AsyncAlloc::CTest11AsyncAlloc()
	{
	iTestStepName = _L("MBufMgrTest11");// Store the name of this test case
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

	//-------------- substep 2 --------------------
	Log(_L("  02 Create and install active object that will do async. alloc on request:"));
	RMBufAsyncRequest async;
	CMBufAsyncReq* testAsync;
	CleanupStack::PushL(testAsync = new(ELeave) CMBufAsyncReq(async));
	CActiveScheduler::Add(testAsync);

	//-------------- substep 3 --------------------
	Log(_L("  03 Async. alloce 5000-bytes long RMBuf:"));
	RMBufChain aChain;
	TInt ret = testAsync->DoAsyncRequest(aChain, 5000);
	if (ret != KErrNone)
		{
		Log(_L("Error: Couldn't allocate Chain:"));

#ifdef __CFLOG_ACTIVE
		__CFLOG_CLOSE;
		__CFLOG_DELETE;
#endif
		User::Leave(EFail);
		}

	//-------------- substep 4 --------------------
	Log(_L("  04 Free the chain. Clean up stack:"));
	aChain.Free();
    CleanupStack::PopAndDestroy(testAsync);
    CleanupStack::PopAndDestroy(iMBMngr);
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);

#ifdef __CFLOG_ACTIVE
	__CFLOG_CLOSE;
	__CFLOG_DELETE;
#endif
   	__UHEAP_MARKEND;
	return EPass;
	}
