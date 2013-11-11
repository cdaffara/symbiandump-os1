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

#include "TSU_MMF_A2DPBLUETOOTH_Open.h"


CTestStep_MMF_A2DPBLUETOOTH_U_0200::CTestStep_MMF_A2DPBLUETOOTH_U_0200()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0200-HP");
	}
 
/**
Open Device
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0200::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	iA2dpBTHeadsetAudioInterface->OpenDevice(*status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNotReady)
		{
		INFO_PRINTF2(_L("Initialization error %d" ), iError);
		verdict = EFail;
		}
	if (verdict == EPass)
		{
		iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("Initialization error %d" ), iError);
			verdict = EInconclusive;
			}
		
		if (verdict == EPass)
			{
			iA2dpBTHeadsetAudioInterface->OpenDevice(*status);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Open error %d" ), iError);
				verdict = EFail;
				}
			}
		}
	return verdict;
	} 


CTestStep_MMF_A2DPBLUETOOTH_U_0201::CTestStep_MMF_A2DPBLUETOOTH_U_0201()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0201-HP");
	}

/**
Open Device with reconfigure
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0201::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	
	//change the settings to force a reconfiguration
	iError = iA2dpBTHeadsetAudioInterface->SetSampleRate(44100);
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface->SetChannels(EMMFStereo,EMMFJoint);
		}
	if (!iError)
		{
		iA2dpBTHeadsetAudioInterface->OpenDevice(*status);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		}
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("callback error %d" ), iError);
		verdict = EFail;
		}
	return verdict;
	}


CTestStep_MMF_A2DPBLUETOOTH_U_0202::CTestStep_MMF_A2DPBLUETOOTH_U_0202()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0202-HP");
	}

/**
Cancel OpenDevice
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0202::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	iA2dpBTHeadsetAudioInterface->CancelOpenDevice();
	iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Initialization error %d" ), iError);
		verdict = EInconclusive;
		}
	else
		{	
		if (verdict == EPass)
			{
			iA2dpBTHeadsetAudioInterface->OpenDevice(*status);
			//need to choose a time long enough for the device open sequence to start
			//but not long enough for the open sequence to complete
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(10000));//try 10mS
			CActiveScheduler::Current()->Start();
			if (*status != KRequestPending)
				{
				INFO_PRINTF1(_L("request status no longer pending-Open probably completed" ));
				verdict = EInconclusive;//can't really say it failed-might have been a fast init
				}
			}
		if (verdict == EPass)
			{
			iA2dpBTHeadsetAudioInterface->CancelOpenDevice();
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
			//check we really are cancelled 
			//should be able to set the sample rate if we are
			iError = iA2dpBTHeadsetAudioInterface->SetSampleRate(KDefaultTestSampleRate);
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("failed to set sample rate - OpenDevice might not really be cancelled %d" ), iError);
				verdict = EFail;
				}
			}
		}
	
	return verdict;
	}

	
CTestStep_MMF_A2DPBLUETOOTH_U_0203::CTestStep_MMF_A2DPBLUETOOTH_U_0203()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0203-HP");
	}

/**
Call OpenDevice twice
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0203::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	iA2dpBTHeadsetAudioInterface->OpenDevice(*status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("OpenDevice error %d" ), iError);
		verdict = EFail;
		}
	iA2dpBTHeadsetAudioInterface->OpenDevice(*status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("OpenDevice error %d" ), iError);
		verdict = EFail;
		}
	return verdict;
	}
	

CTestStep_MMF_A2DPBLUETOOTH_U_0204::CTestStep_MMF_A2DPBLUETOOTH_U_0204()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0204-HP");
	}

/**
Close Device
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0204::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	iA2dpBTHeadsetAudioInterface->CloseDevice(*status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	iA2dpBTHeadsetAudioInterface->OpenDevice(*status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("OpenDevice error %d" ), iError);
		verdict = EFail;
		}
	else
		{
		iA2dpBTHeadsetAudioInterface->CloseDevice(*status);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("CloseDevice error %d" ), iError);
			verdict = EFail;
			}
		else
			{//just check we can open it again
			iA2dpBTHeadsetAudioInterface->OpenDevice(*status);
			iAsyncTestStepNotifier->HandleAsyncRequest();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("OpenDevice error on reopening %d" ), iError);
				verdict = EFail;
				}
			}
		}
	return verdict;
	}

	
CTestStep_MMF_A2DPBLUETOOTH_U_0230::CTestStep_MMF_A2DPBLUETOOTH_U_0230()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0230-HP");
	}
 
/**
Reconfigure from remote headset with invald configuration
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0230::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	TAvdtpMediaTransportCapabilities* mediaTransportCaps = NULL;
	TSBCCodecCapabilities* SBCCodecCaps = NULL;
	
	iA2dpBTHeadsetAudioInterface->OpenDevice(*status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Open error %d" ), iError);
		verdict = EInconclusive;
		}
	else
		{
		//now try to perform an invalid reconfiguration
		MGavdpUser* gavdpUser = iA2dpBTHeadsetAudioInterface->TEST_MGavdpUser();
		//these TSEIDs should come from the iA2dpBTHeadsetAudioInterface
		//but this would need another TEST function to get them
		//the SEIDs below work ok on CSR and blueant but mght not work 
		//on all headsets
		TSEID symbianDevice(1,ETrue);
		TSEID headset(2,EFalse);
		iError = gavdpUser->GAVDP_SuspendIndication(symbianDevice);
		iTimer->StopAfter(TTimeIntervalMicroSeconds32(KTimeForEventsToRegister));//try 500mS
		CActiveScheduler::Current()->Start();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("error %d" ), iError);
			verdict = EFail;
			}
		else
			{
			gavdpUser->GAVDP_ConfigurationStartIndication(symbianDevice, headset);
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KTimeForEventsToRegister));//try 500mS
			CActiveScheduler::Current()->Start();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("error on GAVDP_ConfigurationStartIndication %d" ), iError);
				verdict = EFail;
				}
			else
				{
				//now send an invalid configuration
				mediaTransportCaps = new TAvdtpMediaTransportCapabilities();
				iError = gavdpUser->GAVDP_ConfigurationIndication(mediaTransportCaps);
				if (iError != KErrNone)
					{
					INFO_PRINTF1(_L("error on valid transport config "));
					verdict = EFail;
					}
				SBCCodecCaps = new TSBCCodecCapabilities();
				iError = gavdpUser->GAVDP_ConfigurationIndication(SBCCodecCaps);
				if (iError == KErrNone)
					{
					INFO_PRINTF1(_L("the invalid configuration should generate an error"));
					verdict = EFail;
					}
				iError = gavdpUser->GAVDP_StartIndication(symbianDevice);
				iTimer->StopAfter(TTimeIntervalMicroSeconds32(KTimeForEventsToRegister));//try 500mS
				CActiveScheduler::Current()->Start();
				if (iError != KErrNone)
					{
					INFO_PRINTF2(_L("error on GAVDP_StartIndication is %d" ), iError);
					verdict = EFail;
					}
				}
			}
		}
	
	return verdict;
	} 
	
	
CTestStep_MMF_A2DPBLUETOOTH_U_0231::CTestStep_MMF_A2DPBLUETOOTH_U_0231()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0231-HP");
	}
 
/**
Reconfigure from remote headset with valid configuration
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0231::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	TAvdtpMediaTransportCapabilities* mediaTransportCaps = NULL;
	TSBCCodecCapabilities* SBCCodecCaps = NULL;
	
	iA2dpBTHeadsetAudioInterface->OpenDevice(*status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Open error %d" ), iError);
		verdict = EInconclusive;
		}
	else
		{
		//now try to perform a valid reconfiguration
		MGavdpUser* gavdpUser = iA2dpBTHeadsetAudioInterface->TEST_MGavdpUser();
		//these TSEIDs should come from the iA2dpBTHeadsetAudioInterface
		//but this would need another TEST function to get them
		//the SEIDs below work ok on CSR and blueant but mght not work 
		//on all headsets
		TSEID symbianDevice(1,ETrue);
		TSEID headset(2,EFalse);
		iError = gavdpUser->GAVDP_SuspendIndication(symbianDevice);
		iTimer->StopAfter(TTimeIntervalMicroSeconds32(KTimeForEventsToRegister));//try 500mS
		CActiveScheduler::Current()->Start();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("error %d" ), iError);
			verdict = EFail;
			}
		else
			{
			//now send a valid configuration
			gavdpUser->GAVDP_ConfigurationStartIndication(symbianDevice, headset);
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KTimeForEventsToRegister));//try 500mS
			CActiveScheduler::Current()->Start();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("error on GAVDP_ConfigurationStartIndication %d" ), iError);
				verdict = EFail;
				}
			
			mediaTransportCaps = new TAvdtpMediaTransportCapabilities();
			iError = gavdpUser->GAVDP_ConfigurationIndication(mediaTransportCaps);
			if (iError != KErrNone)
				{
				INFO_PRINTF1(_L("error on valid transport config "));
				verdict = EFail;
				}
			SBCCodecCaps = new TSBCCodecCapabilities();
			SBCCodecCaps->SetSamplingFrequencies(E32kHz);
			SBCCodecCaps->SetChannelModes(EMono);
			SBCCodecCaps->SetBlockLengths(EBlockLenEight);
			SBCCodecCaps->SetSubbands(EEightSubbands);
			SBCCodecCaps->SetAllocationMethods(ELoudness);
			SBCCodecCaps->SetMinBitpoolValue(30);
			SBCCodecCaps->SetMaxBitpoolValue(44);
			iError = gavdpUser->GAVDP_ConfigurationIndication(SBCCodecCaps);
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("error on codec GAVDP_ConfigurationIndication %d" ), iError);
				verdict = EFail;
				}
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KTimeForEventsToRegister));//try 500mS
			CActiveScheduler::Current()->Start();
			iError = gavdpUser->GAVDP_ConfigurationEndIndication();
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KTimeForEventsToRegister));//try 500mS
			CActiveScheduler::Current()->Start();
			iError = gavdpUser->GAVDP_StartIndication(symbianDevice);
			iTimer->StopAfter(TTimeIntervalMicroSeconds32(KTimeForEventsToRegister));//try 500mS
			CActiveScheduler::Current()->Start();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("error on GAVDP_StartIndication is %d" ), iError);
				verdict = EFail;
				}
			}
		}
	return verdict;
	}
