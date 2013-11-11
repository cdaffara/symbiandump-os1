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
// Contains IPC Test 15
// 
//

// EPOC includes
#include <e32base.h>
#include <ss_std.h>
// Test system includes
//#ifdef SYMBIAN_OLD_EXPORT_LOCATION
//#include "networking/log.h"
//#include "networking/teststep.h"
//#else
//#include <networking/log.h>
//#include <networking/teststep.h>
//#endif
#include "TestStepCTMbufmgr.h"

#include "Test15Concurrency.h"
#include <comms-infras/commsbufpond.h>

// constructor
CTest15Concurrency::CTest15Concurrency()
	{
	SetTestStepName(_L("MBufMgrTest15"));// Store the name of this test case
	iMainThreadTries = 0;
	iMainThreadFails = 0;
	iHighThreadTries = 0;
	iHighThreadFails = 0;
	}

// destructor
CTest15Concurrency::~CTest15Concurrency() 
	{
	}

// 
TInt CTest15Concurrency::fHighPriorityThread(TAny* aInput)
	{
	CTest15Concurrency* pTestObject = (CTest15Concurrency*)aInput;
	// We need to introduce this new client thread to the MBufMgr
	TCommsBufPondTLSOp tls(pTestObject->iBufPond);
	tls.Set();
	
	CTrapCleanup* aCleanup = CTrapCleanup::New();

	//Install active scheduler
    CActiveScheduler* aActSch = new CActiveScheduler;
	if(aActSch==NULL)
		{
		return KErrNoMemory;
		}
	CActiveScheduler::Install(aActSch);

	RTimer aTimer; 
	TRequestStatus aTimerStatus;	// Request status associated with timer
	aTimer.CreateLocal();			// Create timer for this thread
	//-------------- substep 1 -------------------- 

	// Allocate two 500-bytes long descriptors (Des1 & Des2):"));
	TBuf8<500> *aDes1 = NULL;
	TBuf8<500> *aDes2 = NULL;

    TRAPD(ret, aDes1 = new(ELeave) TBuf8<500>);
	if(ret!=KErrNone)
		{
		return ret;
		}

    TRAP(ret, aDes2 = new(ELeave) TBuf8<500>);
	if(ret!=KErrNone)
		{
		delete aDes1;
		return ret;
		}

	aDes1->SetLength(500);
	aDes2->SetLength(500);

	//Fill in the Des1 with a pattern
	pTestObject->StripeDes(*aDes1, 0, 500, 'a', 'z');
	
	for (TInt i = 0 ; i<1000;i++)
		{
		//Allocate 500-bytes long RMBufChain
		RMBufChain aChain;
		TRAP(ret,aChain.AllocL(500));
		if (ret != KErrNone)
			{ 
			break;
			}
		//Copy in Des1 into Chain
		aChain.CopyIn(*aDes1);
		//Fill in Des2 with zeros
		pTestObject->StripeDes(*aDes2, 0, 500, 0, 0);
		//Copy out Chain into Des2;
		aChain.CopyOut(*aDes2);
		//Compare the contents of Des1 & Des2
		if(aDes1->Compare(*aDes2))	
			pTestObject->iHighThreadFails++;
		pTestObject->iHighThreadTries++;
		//Free chain
		aChain.Free();
		//Sleep for 5ms
		aTimer.After(aTimerStatus,5000);
		User::WaitForRequest(aTimerStatus); 
		}
	delete aDes1;
	delete aDes2;
	CActiveScheduler::Install(NULL);
	delete aActSch;
	delete aCleanup;
	return ret;
}


 
enum TVerdict CTest15Concurrency::doTestStepL(void)
	{
	SetTestStepResult(EFail);
	volatile TExitType aExit;

	//-------------- substep 1 -------------------- 
	INFO_PRINTF1(_L("  01 Create CMBufManager and install active scheduler:"));
    CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);
	CreateInstanceMBufMgrL(KMBufDefaultHeapSize);
	CleanupClosePushL(iBufPond);

	//-------------------substep 01-----------------------------	
	INFO_PRINTF1(_L("  02 Create high priority thread and resume it:"));
	RThread aThread;

	TInt err=aThread.Create(_L("testThreadRec"), 
		                       fHighPriorityThread, 
 							   KDefaultStackSize, 
							   KDefaultHeapSize, 
							   KDefaultHeapSize, 
							   (TAny*)this,
							   EOwnerProcess);
	if (err!=KErrNone)
		{
		_LIT(aLog,"Error:Could not create the thread. err = %d"); INFO_PRINTF2(aLog,err);
		User::Leave(EFail);
		}
	aThread.SetPriority(EPriorityAbsoluteHigh);
	aThread.Resume();

	//-------------------substep 03-----------------------------	
	INFO_PRINTF1(_L("  02 Do tests in loop end stop when the other finishes"));
	INFO_PRINTF1(_L("     ...It will take a while (about 10 sec.):"));

	// Allocate two 5000-bytes long descriptors (Des1 & Des2):"));
	TBuf8<5000> *aDes1, *aDes2;
    CleanupStack::PushL( aDes1 = new(ELeave) TBuf8<5000> );
    CleanupStack::PushL( aDes2 = new(ELeave) TBuf8<5000> );
	aDes1->SetLength(5000);
	aDes2->SetLength(5000);
	//Fill in the Des1 with a pattern:"));
	StripeDes(*aDes1, 0, 5000, '@', 'Z');

	do {
		//Allocate 5000-bytes long RMBufChain
		RMBufChain aChain;
		TRAPD(ret,aChain.AllocL(5000));
		if (ret != KErrNone)
			{
			INFO_PRINTF1(_L("Error: Couldn't allocate RMBuf:"));
			User::Leave(EFail);
			}
		//Copy in Des1 into Chain
		aChain.CopyIn(*aDes1);
		//Fill in Des2 with zeros:"));
		StripeDes(*aDes2, 0, 5000, 0, 0);
		//Copy out Chain into Des2:"));
		aChain.CopyOut(*aDes2);
		//Compare the contents of Des1 & Des2:
		if(aDes1->Compare(*aDes2))	iMainThreadFails++;
		iMainThreadTries++;
		//Free chain
		aChain.Free();

		//Check whether the other task has finished
		aExit = aThread.ExitType();
		}
	while (aExit == EExitPending);

	//Clean up stack
	CleanupStack::PopAndDestroy(aDes2);
    CleanupStack::PopAndDestroy(aDes1);
    CleanupStack::PopAndDestroy();
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);

	//-------------------substep 02-----------------------------	
	INFO_PRINTF3(_L("     Info: Number of main thread checkings: %d with %d fails"),iMainThreadTries,iMainThreadFails);
	INFO_PRINTF3(_L("     Info: Number of high priority thread checkings: %d with %d fails"),iHighThreadTries,iHighThreadFails);
	aThread.Close();
	if (iHighThreadTries <1000)
		{
		INFO_PRINTF1(_L("   Error: High priority thread ended before 1000 loops are finished"));
         SetTestStepResult(EFail);

		return TestStepResult();
		}
	if ( iHighThreadFails || iMainThreadFails)
		{
        SetTestStepResult(EFail);

		return TestStepResult();
		}
        SetTestStepResult(EPass);

	return TestStepResult();
  }
