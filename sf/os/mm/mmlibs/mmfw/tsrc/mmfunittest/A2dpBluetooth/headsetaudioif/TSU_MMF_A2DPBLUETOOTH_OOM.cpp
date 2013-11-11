// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "TSU_MMF_A2DPBLUETOOTH_OOM.h"


CTestStep_MMF_A2DPBLUETOOTH_U_0400::CTestStep_MMF_A2DPBLUETOOTH_U_0400()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0400-HP");
	}
 
/**
OOM
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0400::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	//load up the bluetooth comms driver
	TInt err = KErrNone;
	err = User::LoadPhysicalDevice(BT_COMM_PDD_NAME);
	if ((err == KErrNone) || (err == KErrAlreadyExists))
		{
		err = User::LoadLogicalDevice(BT_COMM_LDD_NAME);
		}
	if (err && (err != KErrAlreadyExists))
		{
		INFO_PRINTF2(_L("problem loading bluetooth comms driver %d" ), err);
	 	return EInconclusive;
		} 
	
	CActiveScheduler* activeScheduler = CActiveScheduler::Current();
	if (!activeScheduler) activeScheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(activeScheduler);

	CA2dpBTHeadsetAudioInterface*  a2dpBTHeadsetAudioInterface = NULL;
	 
	TInt KMaxFailRate = 100000;
	TBool complete = EFalse;
	TBool nullAlloc = EFalse;
	TInt failCount;

	 for (failCount=1; !complete && failCount<KMaxFailRate; ++failCount)
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, failCount);
		TRAPD(err, a2dpBTHeadsetAudioInterface = CA2dpBTHeadsetAudioInterface::NewL());
		if (err == KErrNone)
			{//need to check if next alloc fails
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				nullAlloc = ETrue;
				}
			else
				{
				User::Free(testAlloc);
				complete = ETrue;
				}
			//check the a2dp bt audio if has been created
			if (!a2dpBTHeadsetAudioInterface) 
				{
				verdict = EFail;
				complete = ETrue;
				}
			else 
				{
				delete a2dpBTHeadsetAudioInterface;
				a2dpBTHeadsetAudioInterface = NULL;
				}
			}
		else if (err != KErrNoMemory)
			{
			verdict = EFail; // bad error code
			complete = ETrue;
			INFO_PRINTF2(_L("OOM test error %d" ), err );
			}
		}

	 if (failCount >= KMaxFailRate-1)
		 {
		 INFO_PRINTF2(_L("fail count too high %d" ), failCount );
		 verdict = EFail;
		 }

	 if (!nullAlloc)
		 {
		 INFO_PRINTF1(_L("alloced after no OOM "));
		 verdict = EFail;
		 }


	 __UHEAP_RESET;

	 delete a2dpBTHeadsetAudioInterface;
	 delete activeScheduler;

	return verdict;
	}
