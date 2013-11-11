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

#include "TSU_MMF_A2DPBLUETOOTH_Settings.h"
#include "mmfRoutingSoundDevice.h"	// for TRange


CTestStep_MMF_A2DPBLUETOOTH_U_0100::CTestStep_MMF_A2DPBLUETOOTH_U_0100()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0100-HP");
	}
 

/**
 * GetSupportedDataTypes
 *
 * Gets the Supported DataTypes before the headset is initialized check this 
 * returns KErrnotReady.  Then initialize headset.  Call GetSupportedDataTypes
 * check pcm16 and SBC are listed as supported datatypes
 */
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0100::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	RArray<TFourCC> supportedDataTypes;
	iError = iA2dpBTHeadsetAudioInterface->GetSupportedDataTypes(supportedDataTypes);
	
	if (iError != KErrNotReady)
		{
		return EFail;
		} 
		
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Initialization error %d" ), iError);
		verdict = EInconclusive;
		}
	
	iError = iA2dpBTHeadsetAudioInterface->GetSupportedDataTypes(supportedDataTypes);	
	
	if (!iError)
		{
		//pcm16 and SBC should always be supported
		//although SBC is just for test purposes
		TUint i = 0;
		TBool pcm16Supported = EFalse;
		TBool SBCSupported = EFalse;
		for (i=0; i<supportedDataTypes.Count(); i++)
			{
			if (supportedDataTypes[i] == KMMFFourCCCodePCM16)
				{
				pcm16Supported = ETrue;
				}
			else if (supportedDataTypes[i] == KMMFFourCCCodeSBC)
				{
				SBCSupported = ETrue;
				}
			}
		if (!((pcm16Supported) && (SBCSupported)))
			{
			INFO_PRINTF1(_L("Failed to support pcm16 or sbc" ));
			verdict = EFail;
			}
		}
	
	if (iError)
		{
		verdict = EFail;
		}
	
	supportedDataTypes.Close();
	return verdict ;
	} 


CTestStep_MMF_A2DPBLUETOOTH_U_0101::CTestStep_MMF_A2DPBLUETOOTH_U_0101()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0101-HP");
	}

/**
 * GetSupportedSampleRates
 *
 * Gets the Supported sample rates before the headset is initialized check this 
 * returns KErrnotReady.  Then initialize headset.  Call GetSupportedSampleRates
 * check 44100 and 48000 are listed as supported sample rates
 */
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0101::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	RArray<TUint> supportedDiscreteRates;
	RArray<TRange> supportedRateRanges;
	
	//should be KErrNotReady prior to being initialised
	iError = iA2dpBTHeadsetAudioInterface->GetSupportedSampleRates(supportedDiscreteRates, supportedRateRanges);
	
	if (iError != KErrNotReady)
		{
		return EFail;
		} 
	
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Initialization error %d" ), iError);
		verdict = EInconclusive;
		}
		
	TInt err = iA2dpBTHeadsetAudioInterface->GetSupportedSampleRates(supportedDiscreteRates, supportedRateRanges);
	
		
	if (!err)
		{	
		//44100 & 48000 sample rates are mandatory for A2DP SBC devices so check
		//these sample rates are supported
		TBool sampleRate44100Supported = EFalse;
		TBool sampleRate48000Supported = EFalse;
		for (TUint i=0; i<supportedDiscreteRates.Count(); i++)
			{
			if (supportedDiscreteRates[i] == 44100)
				{
				sampleRate44100Supported = ETrue;
				}
			else if (supportedDiscreteRates[i] == 48000)
				{
				sampleRate48000Supported = ETrue;
				}
			}
		if (!((sampleRate44100Supported) && (sampleRate48000Supported)))
			{
			INFO_PRINTF1(_L("Failed to support mandatory A2DP sample rate" ));
			verdict = EFail;
			}
		if (supportedRateRanges.Count())
			{//no ranges supported for SBC
			verdict = EFail;
			}
		}
	if (err)
		{
		verdict = EFail;
		}
	supportedDiscreteRates.Close();
	supportedRateRanges.Close();
	return verdict;
	}


CTestStep_MMF_A2DPBLUETOOTH_U_0102::CTestStep_MMF_A2DPBLUETOOTH_U_0102()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0102-HP");
	}

/**
 * GetSupportedChannels
 *
 * Calls the GetSupportedChannels before the headset is initialized, check this 
 * returns KErrNotReady.  Then initialize headset.  Call GetSupportedChannels
 * check mono and stereo are listed as supported channels and that joint
 * stereo is supported.
 */
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0102::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	RArray<TUint> supportedChannels;
	TMMFStereoSupport stereoSupport;
	
	//should be KErrNotReady prior to being initialised
	iError = iA2dpBTHeadsetAudioInterface->GetSupportedChannels(supportedChannels, stereoSupport);
	
	if (iError != KErrNotReady)
		{
		return EFail;
		} 
	
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Initialization error %d" ), iError);
		verdict = EInconclusive;
		}
		
	TInt err = iA2dpBTHeadsetAudioInterface->GetSupportedChannels(supportedChannels, stereoSupport);
			
	if (!err)
		{	
		//mono , stereo and joint stereo are mandatory for the SNK
		TBool monoSupported = EFalse;
		TBool stereoSupported = EFalse;
		TBool jointStereoSupported = EFalse;
		for (TUint i=0; i<supportedChannels.Count(); i++)
			{
			if (supportedChannels[i] == EMMFMono)
				{
				monoSupported = ETrue;
				}
			else if (supportedChannels[i] == EMMFStereo)
				{
				stereoSupported = ETrue;
				if (stereoSupport == (EMMFInterleavedOnly | EMMFJoint)) 
					{
					jointStereoSupported = ETrue;
					}
				}
			}
		if (!((monoSupported) && (stereoSupported) && (jointStereoSupported)))
			{
			INFO_PRINTF1(_L("Failed to support mandatory A2DP channels and/or stereo support" ));
			verdict = EFail;
			}
		}
	if (err)
		{
		verdict = EFail;
		}
	supportedChannels.Close();
	return verdict;
	}


CTestStep_MMF_A2DPBLUETOOTH_U_0103::CTestStep_MMF_A2DPBLUETOOTH_U_0103()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0103-HP");
	}
	
/**
 * SetDataType
 *
 * Calls SetDataType before the headset is initialized, check this 
 * returns KErrNotReady.  Then initialize headset. 
 * Set datatype to pcm16 and then to sbc 
 * Note that since SBC and pcm16 are the only guaranteed datatypes
 * we cannot test for mp3,AAC & ATRAC3
 * Set datatype to an invalid datatype, check KErrNotSupported is returned.
 * @test Req
 */		
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0103::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	TFourCC pcm16(KMMFFourCCCodePCM16);
	TFourCC sbc(KMMFFourCCCodeSBC);
	TFourCC invalid(KMMFFourCCCodeNULL);
	
	iError = iA2dpBTHeadsetAudioInterface->SetDataType(pcm16);	
	if (iError != KErrNotReady)
		{
		return EFail;
		} 
		
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Initialization error %d" ), iError);
		verdict = EInconclusive;
		}
	
	iError = iA2dpBTHeadsetAudioInterface->SetDataType(pcm16);
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface->SetDataType(sbc);
		}
	
	if (iError)	
		{
		verdict = EFail;
		}
	
	//now try an invalid datatype
	if (!iError)
		{		
		iError = iA2dpBTHeadsetAudioInterface->SetDataType(invalid);
		}
		
	if (iError != KErrNotSupported)
		{
		verdict = EFail;
		}
	
	return verdict ;
	} 

	
CTestStep_MMF_A2DPBLUETOOTH_U_0104::CTestStep_MMF_A2DPBLUETOOTH_U_0104()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0104-HP");
	}
	
/**
 * SetSampleRate
 *
 * Calls SetSampleRate before the headset is initialized, check this 
 * returns KErrNotReady.  Then initialize headset. 
 * Set sample rate to 44100 and then to 48000
 * Note that since 44100 and 48000 are the only guaranteed samplerates
 * Set sample rate to an invalid sample rate, check KErrNotSupported is returned.
 */		
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0104::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	iError = iA2dpBTHeadsetAudioInterface->SetSampleRate(44100);	
	if (iError != KErrNotReady)
		{
		return EFail;
		} 
		
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Initialization error %d" ), iError);
		verdict = EInconclusive;
		}
	
	iError = iA2dpBTHeadsetAudioInterface->SetSampleRate(44100);
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface->SetSampleRate(48000);
		INFO_PRINTF1(_L("Failed to support mandatory A2DP sample rate 48000" ));
		}
	else
		{
		INFO_PRINTF1(_L("Failed to support mandatory A2DP sample rate 44100" ));
		}
	
	if (iError)	
		{
		verdict = EFail;
		}
	
	//now try an invalid sample rate
	if (!iError)
		{		
		iError = iA2dpBTHeadsetAudioInterface->SetSampleRate(1234);
		}
		
	if (iError != KErrNotSupported)
		{
		verdict = EFail;
		}
	
	return verdict ;
	} 

	
CTestStep_MMF_A2DPBLUETOOTH_U_0105::CTestStep_MMF_A2DPBLUETOOTH_U_0105()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-U-0105-HP");
	}
	
/**
 * SetChannels
 *
 * Calls SetChannels before the headset is initialized, check this 
 * returns KErrNotReady.  Then initialize headset. 
 * SetChannels to mono and then to stereo with joint stereo mode.
 * SetChannels to an invalid number of channels, check KErrNotSupported is returned.
 * SetChannels to stereo but with an invalid stereo mode, check KErrNotSupported is returned.
 */		
TVerdict CTestStep_MMF_A2DPBLUETOOTH_U_0105::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	iError = iA2dpBTHeadsetAudioInterface->SetChannels(EMMFStereo, EMMFJoint);	
	if (iError != KErrNotReady)
		{
		return EFail;
		} 
		
	TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
	iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
	iAsyncTestStepNotifier->HandleAsyncRequest();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Initialization error %d" ), iError);
		verdict = EInconclusive;
		}
	
	iError = iA2dpBTHeadsetAudioInterface->SetChannels(EMMFMono, EMMFNone);
	if (!iError)
		{
		iError = iA2dpBTHeadsetAudioInterface->SetChannels(EMMFStereo, EMMFJoint);
		}
	
	if (iError)	
		{
		verdict = EFail;
		}
	
	//now try an invalid values
	if (!iError)
		{		
		iError = iA2dpBTHeadsetAudioInterface->SetChannels(3, EMMFNone);
		}
		
	if (iError != KErrNotSupported)
		{
		verdict = EFail;
		}
	else
		{
		//test invalid stereo support	
		TUint stereoSupportInt = 1234;	
		iError = iA2dpBTHeadsetAudioInterface->SetChannels(EMMFStereo, static_cast<TMMFStereoSupport>(stereoSupportInt));
		if (iError != KErrNotSupported)
			{
			verdict = EFail;
			}
		}
			
	return verdict ;
	} 


