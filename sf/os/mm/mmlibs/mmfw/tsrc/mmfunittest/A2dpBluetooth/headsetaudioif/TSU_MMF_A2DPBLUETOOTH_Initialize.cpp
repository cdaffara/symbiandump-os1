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

#include "TSU_MMF_A2DPBLUETOOTH_Initialize.h"
#include <gavdp.h>



CTestStep_MMF_A2DPBLUETOOTH_U_0001::CTestStep_MMF_A2DPBLUETOOTH_U_0001()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0001-HP");
	}
 

/**
 * Create A2DP Bluetooth Audio Interface
 */
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0001::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	 __MM_HEAP_MARK;
	
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
	
	CA2dpBTHeadsetAudioInterface* a2dpBTHeadsetAudioInterface = NULL;

	TRAP(err,a2dpBTHeadsetAudioInterface = CA2dpBTHeadsetAudioInterface::NewL())
	 if (err != KErrNone)
	 	{
	 	INFO_PRINTF2(_L("error on CA2dpBTHeadsetAudioInterface construction %d" ), err);
	 	verdict =  EFail;
	 	}
	 	
	 delete a2dpBTHeadsetAudioInterface;
	 delete activeScheduler;
	 
	 __MM_HEAP_MARKEND
	
	return verdict ;
	} 



CTestStep_MMF_A2DPBLUETOOTH_U_0002::CTestStep_MMF_A2DPBLUETOOTH_U_0002()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0002-HP");
	}
 

/**
 * Initialize
 *
 * Check Initialize completes without error
 * 
 */
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0002::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	iError = KErrGeneral; //we'll start with an error
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Initialization error %d" ), iError);
		verdict = EFail;
		}
	return verdict ;
	} 



CTestStep_MMF_A2DPBLUETOOTH_U_0003::CTestStep_MMF_A2DPBLUETOOTH_U_0003()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0003-HP");
	}


/**
 * Cancel Initialize
 *
 * first tries to cancel when no request is outstanding
 * then tries to cancel 100mS after an initialization request has been made
 * then tries to Open - which should fail with KErrNotReady
 * if the initialization was really cancelled.
 * then tries to inititialize again to check that the the cancel doesn't
 * affect a subsequent initialization
 */
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0003::DoTestStepL( void )
	{
	TVerdict verdict = EPass;		
	iError = KErrGeneral; //we'll start with an error
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	iA2dpBTHeadsetAudioInterface->CancelInitialize();
	iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
	//need to choose a time long enough for the initialize sequence to start
	//but not long enough for the initialize sequence to complete
	iTimer->StopAfter(TTimeIntervalMicroSeconds32(100000));//try 100mS
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (*status != KRequestPending)
		{
		INFO_PRINTF1(_L("request status no longer pending-initialize probably completed" ));
		verdict = EInconclusive;//can't really say it failed-might have been a fast init
		}
	else if (iError != KErrGeneral)
		{//initialize request should not have completed so error should be unchanged
		INFO_PRINTF2(_L("expected KErrGeneral got error %d" ), iError);
		verdict = EFail;
		}
		
	if (verdict == EPass)
		{
		iA2dpBTHeadsetAudioInterface->CancelInitialize();
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (*status != KErrCancel)
			{
			INFO_PRINTF1(_L("request status not still be pending" ));
			verdict = EFail;
			}
		else if (iError != KErrCancel)
			{
			INFO_PRINTF2(_L("expected KErrCancel got error %d" ), iError);
			verdict = EFail;
			}
		}
	
	if (verdict == EPass)
		{
		//now check we really did cancel the initialize
		//should not be possible to open if not initialized
		//uses default settings
		iA2dpBTHeadsetAudioInterface->OpenDevice(*status);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNotReady)
			{
			INFO_PRINTF2(_L("expected KErrNotReady got error %d" ), iError);
			verdict = EFail;
			}
		}
	
	if (verdict == EPass)
		{
		//check we can initialize again having cancelled the previous initialize
		iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("callback error %d" ), iError);
			verdict = EFail;
			}
		}
	return verdict;
	} 

	

CTestStep_MMF_A2DPBLUETOOTH_U_0004::CTestStep_MMF_A2DPBLUETOOTH_U_0004()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0004-HP");
	}
 

/**
 * Double Initialization with link going down after first initialization
 * 
 * Checks that a previous initialization does not prevent a subsequent
 * initialization from working.  The second initialization should be a full
 * initialization
 */
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0004::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	iError = KErrGeneral; //we'll start with an error
	
	//start off with a standard initialization
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Initialization error %d" ), iError);
		verdict = EFail;
		}
		
	if (verdict == EPass)
		{
		//now disable the link - note that there is a timelag between a link being
		//disabled and the lower layers picking it up	
		iBtPhysicalLink->Disconnect(*iBTheadsetAddress);
	
		//the lower level code takes around 25S to register 
		//that the BT link has gone down
		iTimer->StopAfter(TTimeIntervalMicroSeconds32(KTimeToRegisterLinkLost));
		CActiveScheduler::Current()->Start();
		if (!iBTLinkDisconnected )
			{
			//the BT link was not disconnected
			INFO_PRINTF1(_L("the BT link was not disconnected"));
			verdict = EInconclusive;
			}
		else
			{	
			//now try to initialize again
			//should do a full initialization
			iError = KErrGeneral; //we'll start with an error
			iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Initialization error %d" ), iError);
				verdict = EFail;
				}
			}
		}
	return verdict;
	} 

	
CTestStep_MMF_A2DPBLUETOOTH_U_0005::CTestStep_MMF_A2DPBLUETOOTH_U_0005()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0005-HP");
	}
 
/**
 * Double Initialization
 * 
 * Perform a first initialization which should be a full initialization (and take
 * a relatively long time) - order of magnitude seconds
 * Second initialization should be quick since it is already
 * initialized order of magnitude mS
 * @test Req. under test REQ
 */
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0005::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	iError = KErrGeneral; //we'll start with an error
	
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		verdict = EFail;
		INFO_PRINTF2(_L("Initialization error %d" ), iError);
		}
	
	if (verdict == EPass)
		{
		//now try to reinitialize
		//[TODO] should time the initialization to make sure it is quick
		iError = KErrGeneral; //we'll start with an error
		iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("Initialization error %d" ), iError);
			verdict = EFail;
			}
		}
		
	return verdict ;
	} 


CTestStep_MMF_A2DPBLUETOOTH_U_0006::CTestStep_MMF_A2DPBLUETOOTH_U_0006()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0006-HP");
	}
 

/**
 * Cancel Invalid Initialization
 * 
 * Initialize using an invalid headset address
 * Checks request status is still pending after
 * the normal initialize time - 5 seconds has been chosen
 * Then cancel the initialize
 * Check we really have cancelled the initialization
 * by Opening - should complete with KErrNotReady
 * Then check we can perform a valid initialization
 * @test Req. under test REQ
 */
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0006::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	TBTDevAddr btDevAddr(KInvalidBTAddress); //invalid address

	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	iError = KErrGeneral; //we'll start with an error
	iA2dpBTHeadsetAudioInterface->Initialize(btDevAddr, *status);
	iTimer->StopAfter(TTimeIntervalMicroSeconds32(KApproxInitializationTime));//5S should be enough
	iAsyncTestStepNotifier->HandleAsyncRequest();
	//the request status should still be pending and iError should still be KErrCompletion
	if (*status != KRequestPending)
		{
		INFO_PRINTF1(_L("request status should still be pending" ));
		verdict = EFail;
		}
	else if (iError != KErrGeneral)
		{
		INFO_PRINTF2(_L("expected KErrCompletion got error %d" ), iError);
		verdict = EFail;
		}
		
	if (verdict == EPass)
		{
		iA2dpBTHeadsetAudioInterface->CancelInitialize();
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (*status != KErrCancel)
			{
			INFO_PRINTF1(_L("request status not still be pending" ));
			verdict = EFail;
			}
		else if (iError != KErrCancel)
			{
			INFO_PRINTF2(_L("expected KErrCancel got error %d" ), iError);
			verdict = EFail;
			}
		}
	
	if (verdict == EPass)
		{
		//try Open -should give KErrNotReady if we have
		//successfully cancelled initialization
		iA2dpBTHeadsetAudioInterface->OpenDevice(*status);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNotReady)
			{
			INFO_PRINTF2(_L("expected KErrNotReady got error %d" ), iError);
			verdict = EFail;
			}
		}
	
	if (verdict == EPass)
		{
		//try reinitializing with valid headset address
		iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("Initialization error %d" ), iError);
			verdict = EFail;
			}
		}
		
	return verdict ;
	} 


CTestStep_MMF_A2DPBLUETOOTH_U_0007::CTestStep_MMF_A2DPBLUETOOTH_U_0007()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0007-HP");
	}
 

/**
 * Break BT link during initialization
 * Initialize headset then break link
 * check Initialize completes with KErrDisconnected
 * check subsequent initialization completes ok
 * @test Req. under test REQ
 */
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0007::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	iError = KErrGeneral; //we'll start with an error
		
	//initialize and then disable BT link after 200mS
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
	iTimer->StopAfter(TTimeIntervalMicroSeconds32(KDisableBTLinkAfterTime));//break link after 200mS
	iAsyncTestStepNotifier->HandleAsyncRequest();
	//now disable the link - note that there is a timelag between a link being
	//disabled and the lower layers picking it up
	//[TODO] -this doesn't seem to work despite disconnected
	//initialization completes sucessfully
	iBtPhysicalLink->Disconnect(*iBTheadsetAddress);
	CActiveScheduler::Current()->Start();//and carry on	

			
	if (!iBTLinkDisconnected )
		{
		//the BT link was not disconnected
		INFO_PRINTF1(_L("the BT link was not disconnected"));
		verdict = EInconclusive;
		}
		

	if (iError != KErrDisconnected)
		{
		INFO_PRINTF2(_L("Expected KErrDisconnected got error %d" ), iError);
		verdict = EFail;
		}

	if (verdict == EPass)
		{
		//now try to initialize again
		//should do a full initialization
		iError = KErrGeneral; //we'll start with an error
		iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("Initialize error %d" ), iError);
			verdict = EFail;
			}
		}

	return verdict;
	} 

void CTestStep_MMF_A2DPBLUETOOTH_U_0007::AsyncNotification(TInt aError) 
	{
	iError = aError;
	iInitializeComplete = ETrue;
	iActiveScheduler->Stop();
	}
	
	
CTestStep_MMF_A2DPBLUETOOTH_U_0050::CTestStep_MMF_A2DPBLUETOOTH_U_0050()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0050-HP");
	}
 

TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0050::DoTestStepL( void )
/**
 * Manual commect from headset test
 * note this test is NOT part of the automated test sequence 
 * and should not be in the script file since it involves manual
 * intervention
 * @test Req. under test REQ
 */
	{
	TVerdict verdict = EPass;
	iError = KErrGeneral; //we'll start with an error
	
	//the lower level code takes around 25S to register 
	//that the BT link has gone down so put in a time of
	//1 minute to make sure
	//now try to get headset to manually reconnect
	INFO_PRINTF1(_L("CONNECT HEADSET HERE and press connect button on headset"));
	iTimer->StopAfter(TTimeIntervalMicroSeconds32(60000000));
	CActiveScheduler::Current()->Start();
	
	//start off with a standard initialization
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	//should already ben connected this initialize should be quick
	iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Initialization error %d" ), iError);
		verdict = EFail;
		}
		
	if (verdict == EPass)
		{
		//now manually disable the link - note that there is a timelag between a link being
		//disabled and the lower layers picking it up	
		INFO_PRINTF1(_L("DISCONNECT HEADSET HERE eg remove battery"));
	
		//the lower level code takes around 25S to register 
		//that the BT link has gone down so put in a time of
		//30s to make sure
		iTimer->StopAfter(TTimeIntervalMicroSeconds32(KTimeToRegisterLinkLost));
		CActiveScheduler::Current()->Start();
		//[TODO] could use a notify error in here to detect this
		
		//now try to get headset to manually reconnect
		INFO_PRINTF1(_L("RECONNECT HEADSET HERE and press connect button on headset"));
		iTimer->StopAfter(TTimeIntervalMicroSeconds32(60000000));
		CActiveScheduler::Current()->Start();
		//should do a full initialization
		iError = KErrGeneral; //we'll start with an error
		//the initialization below should be much quicker as we are already connected
		iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("Initialization error %d" ), iError);
			verdict = EFail;			
			}
		}
	return verdict;
	}

