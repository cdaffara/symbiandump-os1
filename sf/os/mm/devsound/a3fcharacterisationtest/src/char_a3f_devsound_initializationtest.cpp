// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// User includes
#include "char_a3f_devsound_initializationtest.h"

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0001
 */
RA3FDevSoundToneInitializationTest::RA3FDevSoundToneInitializationTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundToneInitializationTest* RA3FDevSoundToneInitializationTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneInitializationTest* self = new (ELeave) RA3FDevSoundToneInitializationTest(aTestName);
	return self;
	}

void RA3FDevSoundToneInitializationTest::DoKickoffTestL()
	{
	}

void RA3FDevSoundToneInitializationTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundToneInitializationTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("========== Test Case Successfully Completed =========="));
				StopTest();
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0002
 */
RA3FDevSoundToneCancelInitializeTest::RA3FDevSoundToneCancelInitializeTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName),iInitCompleteReceived(EFalse),
		iSampleRate(0), iChannels(0)
	{
	}

RA3FDevSoundToneCancelInitializeTest* RA3FDevSoundToneCancelInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneCancelInitializeTest* self = new (ELeave) RA3FDevSoundToneCancelInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundToneCancelInitializeTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KSampleRate, iSampleRate))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KSampleRate);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KChannel, iChannels))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KChannel);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundToneCancelInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundToneCancelInitializeTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}		
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("Calling CMMFDevSound::CancelInitialize")); 
				err = iMMFDevSound->CancelInitialize();
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("Unexpected error on CancelInitialize = %d"), err);
					StopTest(err);
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete)
				{
				iInitCompleteReceived = ETrue;
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}
void RA3FDevSoundToneCancelInitializeTest::DoTimerCallback()
	{
	if(!iInitCompleteReceived)
		{
		INFO_PRINTF1(_L("Waiting for InitializeComplete event"));
		iInitCompleteReceived = ETrue;
		}
	else
		{
		iTimer->Cancel();
		TMMFCapabilities capabilitiesSet;
		TBuf<KMaxSampleRateStringLength> stringSampleRateSet;
		TBuf<KMaxChannelsStringLength> stringChannelsSet;
		SampleRateFromTIntToTMMFSampleRate (iSampleRate, iESampleRate);
		capabilitiesSet.iRate = iESampleRate;
		capabilitiesSet.iChannels = iChannels;
		SampleRateFromTUintToString (capabilitiesSet.iRate,	stringSampleRateSet);
		ChannelsFromTUintToString (capabilitiesSet.iChannels, stringChannelsSet);
		INFO_PRINTF3(_L("Settings for Sample rate an channel mode. %S %S"), &stringSampleRateSet, &stringChannelsSet);
		INFO_PRINTF1(_L("Calling CMMFDevSound::SetConfigL"));
		TRAPD(err, iMMFDevSound->SetConfigL(capabilitiesSet));
		if (err ==  KErrNotReady)
			{
			INFO_PRINTF2(_L("CMMFDevSound::SetConfigL left with the expected error = %d"), err);
			StopTest();
			}
		else
			{
			INFO_PRINTF2(_L("CMMFDevSound::SetConfigL left with error = %d"), err);
			ERR_PRINTF2(_L("Expected error value = %d"), KErrNotReady);
			StopTest(err, EFail);
			}
		}
	}

void RA3FDevSoundToneCancelInitializeTest::InitializeComplete(TInt aError)
	{
	ERR_PRINTF1(_L("DevSound called InitializeComplete after calling CMMFDevSound::CancelInitialize"));
	StopTest(aError, EFail);
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0003
 */
RA3FDevSoundToneInitializeToPlayTonesTwiceTest::RA3FDevSoundToneInitializeToPlayTonesTwiceTest(const TDesC& aTestName) 
	: 	RA3FDevSoundTestBase(aTestName), iReinitialized(EFalse)
	{
	}

RA3FDevSoundToneInitializeToPlayTonesTwiceTest* RA3FDevSoundToneInitializeToPlayTonesTwiceTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneInitializeToPlayTonesTwiceTest* self = new (ELeave) RA3FDevSoundToneInitializeToPlayTonesTwiceTest(aTestName);
	return self;
	}

void RA3FDevSoundToneInitializeToPlayTonesTwiceTest::DoKickoffTestL()
	{
	}

void RA3FDevSoundToneInitializeToPlayTonesTwiceTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundToneInitializeTwiceToPlayTonesTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				if(!iReinitialized)
					{
					INFO_PRINTF1(_L("========== First initialization successfully completed =========="));
					iReinitialized = ETrue;
					INFO_PRINTF1(_L("Again calling CMMFDevSound::InitializeL"));
					TRAPD(err, iMMFDevSound->InitializeL(*this,EMMFStateTonePlaying));
					if(err != KErrNone)
						{
						StopTest(err);
						break;
						}
					INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
					iDevSoundState = EStateInitializing;
					}
				else
					{
					INFO_PRINTF1(_L("========== Second initialization successfully completed =========="));
					StopTest();
					}
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0004
 */
RA3FDevSoundToneInitializeToPlayTonesThenAudioTest::RA3FDevSoundToneInitializeToPlayTonesThenAudioTest(const TDesC& aTestName) 
	: 	RA3FDevSoundTestBase(aTestName), iReinitialized(EFalse)
	{
	}

RA3FDevSoundToneInitializeToPlayTonesThenAudioTest* RA3FDevSoundToneInitializeToPlayTonesThenAudioTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneInitializeToPlayTonesThenAudioTest* self = new (ELeave) RA3FDevSoundToneInitializeToPlayTonesThenAudioTest(aTestName);
	return self;
	}

void RA3FDevSoundToneInitializeToPlayTonesThenAudioTest::DoKickoffTestL()
	{
	}

void RA3FDevSoundToneInitializeToPlayTonesThenAudioTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundToneInitializeToPlayTonesThenAudioTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				if(!iReinitialized)
					{
					INFO_PRINTF1(_L("========== First initialization successfully completed =========="));
					iReinitialized = ETrue;
					INFO_PRINTF1(_L("Again calling CMMFDevSound::InitializeL"));
					TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStatePlaying));
					if(err != KErrNone)
						{
						StopTest(err);
						break;
						}
					INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
					iDevSoundState = EStateInitializing;
					}
				else
					{
					INFO_PRINTF1(_L("========== Second initialization successfully completed =========="));
					StopTest();
					}
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0005
 */
RA3FDevSoundToneInitializeToPlayTonesThenRecordAudioTest::RA3FDevSoundToneInitializeToPlayTonesThenRecordAudioTest(const TDesC& aTestName) 
	: 	RA3FDevSoundTestBase(aTestName), iReinitialized(EFalse)
	{
	}

RA3FDevSoundToneInitializeToPlayTonesThenRecordAudioTest* RA3FDevSoundToneInitializeToPlayTonesThenRecordAudioTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneInitializeToPlayTonesThenRecordAudioTest* self = new (ELeave) RA3FDevSoundToneInitializeToPlayTonesThenRecordAudioTest(aTestName);
	return self;
	}

void RA3FDevSoundToneInitializeToPlayTonesThenRecordAudioTest::DoKickoffTestL()
	{
	}

void RA3FDevSoundToneInitializeToPlayTonesThenRecordAudioTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundToneInitializeToPlayTonesThenRecordAudioTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				if(!iReinitialized)
					{
					INFO_PRINTF1(_L("========== First initialization successfully completed=========="));
					iReinitialized = ETrue;
					INFO_PRINTF1(_L("Again calling CMMFDevSound::InitializeL"));
					TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateRecording));
					if(err != KErrNone)
						{
						StopTest(err);
						break;
						}
					INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
					iDevSoundState = EStateInitializing;
					}
				else
					{
					INFO_PRINTF1(_L("========== Second initialization successfully completed =========="));
					StopTest();
					}
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0006
 */
RA3FDevSoundToneInitializeTFourCCTest::RA3FDevSoundToneInitializeTFourCCTest(const TDesC& aTestName) 
	: 	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundToneInitializeTFourCCTest* RA3FDevSoundToneInitializeTFourCCTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneInitializeTFourCCTest* self = new (ELeave) RA3FDevSoundToneInitializeTFourCCTest(aTestName);
	return self;
	}

void RA3FDevSoundToneInitializeTFourCCTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if(!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundToneInitializeTFourCCTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundToneInitializeTFourCCTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStateTonePlaying));
				if(err == KErrNotSupported)
					{
					ERR_PRINTF2(_L("Calling CMMFDevSound::InitializeL to play tones using a FourCC code left with expected error = %d"), err);
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					INFO_PRINTF2(_L("Expected error = %d"), KErrNotSupported);
					StopTest(err, EFail);
					}
				}
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0007
 */
RA3FDevSoundInitializeInvalidModeTest::RA3FDevSoundInitializeInvalidModeTest(const TDesC& aTestName) 
	: 	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundInitializeInvalidModeTest* RA3FDevSoundInitializeInvalidModeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundInitializeInvalidModeTest* self = new (ELeave) RA3FDevSoundInitializeInvalidModeTest(aTestName);
	return self;
	}

void RA3FDevSoundInitializeInvalidModeTest::DoKickoffTestL()
	{
	}

void RA3FDevSoundInitializeInvalidModeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundInitializeInvalidModeTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateConverting));
				if(err == KErrNotSupported)
					{
					ERR_PRINTF2(_L("Calling CMMFDevSound::InitializeL using an invalid TMMFState argument left with expected error code = %d"), err);
					StopTest(err, EPass);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Calling CMMFDevSound::InitializeL using an invalid TMMFState argument left with unexpected error code = %d"), err);
					INFO_PRINTF2(_L("Expected error = %d"), KErrNotSupported);
					StopTest(err, EFail);
					}
				}
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}
	

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0008
 */
RA3FDevSoundPlayInitializationTest::RA3FDevSoundPlayInitializationTest(const TDesC& aTestName) 
	: 	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundPlayInitializationTest* RA3FDevSoundPlayInitializationTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayInitializationTest* self = new (ELeave) RA3FDevSoundPlayInitializationTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayInitializationTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if(!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundPlayInitializationTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundPlayInitializationTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("CMMFDevSound::InitializeL succeeded"));
				StopTest();
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0009
 */
RA3FDevSoundPlayCancelInitializeTest::RA3FDevSoundPlayCancelInitializeTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName),iInitCompleteReceived(EFalse),
		iSampleRate(0), iChannels(0)
	{
	}

RA3FDevSoundPlayCancelInitializeTest* RA3FDevSoundPlayCancelInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayCancelInitializeTest* self = new (ELeave) RA3FDevSoundPlayCancelInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayCancelInitializeTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KSampleRate, iSampleRate))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KSampleRate);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KChannel, iChannels))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KChannel);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if(!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPlayCancelInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundPlayCancelInitializeTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}		
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("Calling CMMFDevSound::CancelInitialize"));
				err = iMMFDevSound->CancelInitialize();
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("Unexpected error on CancelInitialize = %d"), err);
					StopTest(err);
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete)
				{
				iInitCompleteReceived = ETrue;
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayCancelInitializeTest::DoTimerCallback()
	{
	if(!iInitCompleteReceived)
		{
		INFO_PRINTF1(_L("Waiting for InitializeComplete event"));
		iInitCompleteReceived = ETrue;
		}
	else
		{
		iTimer->Cancel();
		TMMFCapabilities capabilitiesSet;
		TBuf<KMaxSampleRateStringLength> stringSampleRateSet;
		TBuf<KMaxChannelsStringLength> stringChannelsSet;
		SampleRateFromTIntToTMMFSampleRate (iSampleRate, iESampleRate);
		capabilitiesSet.iRate = iESampleRate;
		capabilitiesSet.iChannels = iChannels;
		SampleRateFromTUintToString (capabilitiesSet.iRate,	stringSampleRateSet);
		ChannelsFromTUintToString (capabilitiesSet.iChannels, stringChannelsSet);
		INFO_PRINTF3(_L("Settings for Sample rate an channel mode. %S %S"), &stringSampleRateSet, &stringChannelsSet);
		INFO_PRINTF1(_L("Calling CMMFDevSound::SetConfigL"));
		TRAPD(err, iMMFDevSound->SetConfigL(capabilitiesSet));
		if (err ==  KErrNotReady)
			{
			INFO_PRINTF2(_L("CMMFDevSound::SetConfigL left with the expected error = %d"), err);
			StopTest();
			}
		else
			{
			INFO_PRINTF2(_L("CMMFDevSound::SetConfigL left with error = %d"), err);
			ERR_PRINTF2(_L("Expected error value = %d"), KErrNotReady);
			StopTest(err, EFail);
			}
		}
	}

void RA3FDevSoundPlayCancelInitializeTest::InitializeComplete(TInt aError)
	{
	ERR_PRINTF1(_L("DevSound called InitializeComplete after calling CMMFDevSound::CancelInitialize"));
	StopTest(aError, EFail);
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0010
 */
RA3FDevSoundPlayInitializeToPlayAudioThenTonesTest::RA3FDevSoundPlayInitializeToPlayAudioThenTonesTest(const TDesC& aTestName) 
	: 	RA3FDevSoundTestBase(aTestName), iReinitialized(EFalse)
	{
	}

RA3FDevSoundPlayInitializeToPlayAudioThenTonesTest* RA3FDevSoundPlayInitializeToPlayAudioThenTonesTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayInitializeToPlayAudioThenTonesTest* self = new (ELeave) RA3FDevSoundPlayInitializeToPlayAudioThenTonesTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayInitializeToPlayAudioThenTonesTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if(!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundPlayInitializeToPlayAudioThenTonesTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundPlayInitializeToPlayAudioThenTonesTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				if(!iReinitialized)
					{
					INFO_PRINTF1(_L("========== First initialization successfully completed =========="));
					iReinitialized = ETrue;
					INFO_PRINTF1(_L("Again calling CMMFDevSound::InitializeL"));
					TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
					if(err != KErrNone)
						{
						StopTest(err);
						break;
						}
					INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
					iDevSoundState = EStateInitializing;
					}
				else
					{
					INFO_PRINTF1(_L("========== Second initialization successfully completed =========="));
					StopTest();
					}
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0011
 */
RA3FDevSoundPlayInitializeToPlayAudioTwiceTest::RA3FDevSoundPlayInitializeToPlayAudioTwiceTest(const TDesC& aTestName) 
	: 	RA3FDevSoundTestBase(aTestName), iReinitialized(EFalse)
	{
	}

RA3FDevSoundPlayInitializeToPlayAudioTwiceTest* RA3FDevSoundPlayInitializeToPlayAudioTwiceTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayInitializeToPlayAudioTwiceTest* self = new (ELeave) RA3FDevSoundPlayInitializeToPlayAudioTwiceTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayInitializeToPlayAudioTwiceTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if(!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundPlayInitializeToPlayAudioTwiceTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundPlayInitializeTwiceToPlayAudioTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				if(!iReinitialized)
					{
					INFO_PRINTF1(_L("========== First initialization successfully completed =========="));
					iReinitialized = ETrue;
					INFO_PRINTF1(_L("Again calling CMMFDevSound::InitializeL"));
					TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
					if(err != KErrNone)
						{
						StopTest(err);
						break;
						}
					INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
					iDevSoundState = EStateInitializing;
					}
				else
					{
					INFO_PRINTF1(_L("========== Second initialization successfully completed =========="));
					StopTest();
					}
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0012
 */
RA3FDevSoundPlayInitializeToPlayAudioThenRecordTest::RA3FDevSoundPlayInitializeToPlayAudioThenRecordTest(const TDesC& aTestName) 
	: 	RA3FDevSoundTestBase(aTestName), iReinitialized(EFalse)
	{
	}

RA3FDevSoundPlayInitializeToPlayAudioThenRecordTest* RA3FDevSoundPlayInitializeToPlayAudioThenRecordTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayInitializeToPlayAudioThenRecordTest* self = new (ELeave) RA3FDevSoundPlayInitializeToPlayAudioThenRecordTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayInitializeToPlayAudioThenRecordTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if(!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundPlayInitializeToPlayAudioThenRecordTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundPlayInitializeToPlayAudioThenRecordTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				if(!iReinitialized)
					{
					INFO_PRINTF1(_L("========== First initialization successfully completed =========="));
					iReinitialized = ETrue;
					INFO_PRINTF1(_L("Again calling CMMFDevSound::InitializeL"));
					TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStateRecording));
					if(err != KErrNone)
						{
						StopTest(err);
						break;
						}
					INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
					iDevSoundState = EStateInitializing;
					}
				else
					{
					INFO_PRINTF1(_L("========== Second initialization successfully completed =========="));
					StopTest();
					}
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0013
 */
RA3FDevSoundPlayInitializeInvalidTFourCCTest::RA3FDevSoundPlayInitializeInvalidTFourCCTest(const TDesC& aTestName) 
	: 	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundPlayInitializeInvalidTFourCCTest* RA3FDevSoundPlayInitializeInvalidTFourCCTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayInitializeInvalidTFourCCTest* self = new (ELeave) RA3FDevSoundPlayInitializeInvalidTFourCCTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayInitializeInvalidTFourCCTest::DoKickoffTestL()
	{
	}

void RA3FDevSoundPlayInitializeInvalidTFourCCTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundPlayInitializeInvalidTFourCCTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, 0x00000000, EMMFStatePlaying));
				if(err == KErrNotSupported)
					{
					INFO_PRINTF2(_L("Calling CMMFDevSound::InitializeL using an invalid TFourCC argument left with expected error = %d"), err);
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("Calling CMMFDevSound::InitializeL using an invalid TFourCC argument left with expected error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotSupported);
					StopTest(err, EFail);
					}
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0014
 */
RA3FDevSoundRecordInitializationTest::RA3FDevSoundRecordInitializationTest(const TDesC& aTestName) 
	: 	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundRecordInitializationTest* RA3FDevSoundRecordInitializationTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordInitializationTest* self = new (ELeave) RA3FDevSoundRecordInitializationTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordInitializationTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if(!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundRecordInitializationTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundRecordInitializationTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStateRecording));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("========== Test Case Successfully Completed =========="));
				StopTest();
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0015
 */
RA3FDevSoundRecordCancelInitializeTest::RA3FDevSoundRecordCancelInitializeTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName),iInitCompleteReceived(EFalse),
	iSampleRate(0), iChannels(0)
	{
	}

RA3FDevSoundRecordCancelInitializeTest* RA3FDevSoundRecordCancelInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordCancelInitializeTest* self = new (ELeave) RA3FDevSoundRecordCancelInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordCancelInitializeTest::DoKickoffTestL()
	{
	if (!GetIntFromConfig(iTestStepName, KSampleRate, iSampleRate))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KSampleRate);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iTestStepName, KChannel, iChannels))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KChannel);
		StopTest(KErrNotFound);
		return;
		}
	TPtrC fourccCode;
	if(!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundRecordCancelInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundRecordCancelInitializeTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStateRecording));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}		
				StartTimer(KMicroSecsTwoSec);
				INFO_PRINTF1(_L("Calling CMMFDevSound::CancelInitialize"));
				err = iMMFDevSound->CancelInitialize();
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("Unexpected error on CancelInitialize = %d"), err);
					StopTest(err);
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete)
				{
				iInitCompleteReceived = ETrue;
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundRecordCancelInitializeTest::DoTimerCallback()
	{
	if(!iInitCompleteReceived)
		{
		INFO_PRINTF1(_L("Waiting for InitializeComplete event"));
		iInitCompleteReceived = ETrue;
		}
	else
		{
		iTimer->Cancel();
		TMMFCapabilities capabilitiesSet;
		TBuf<KMaxSampleRateStringLength> stringSampleRateSet;
		TBuf<KMaxChannelsStringLength> stringChannelsSet;
		SampleRateFromTIntToTMMFSampleRate (iSampleRate, iESampleRate);
		capabilitiesSet.iRate = iESampleRate;
		capabilitiesSet.iChannels = iChannels;
		SampleRateFromTUintToString (capabilitiesSet.iRate,	stringSampleRateSet);
		ChannelsFromTUintToString (capabilitiesSet.iChannels, stringChannelsSet);
		INFO_PRINTF3(_L("Settings for Sample rate an channel mode. %S %S"), &stringSampleRateSet, &stringChannelsSet);
		INFO_PRINTF1(_L("Calling CMMFDevSound::SetConfigL"));
		TRAPD(err, iMMFDevSound->SetConfigL(capabilitiesSet));
		if (err ==  KErrNotReady)
			{
			INFO_PRINTF2(_L("CMMFDevSound::SetConfigL left with the expected error = %d"), err);
			StopTest();
			}
		else
			{
			INFO_PRINTF2(_L("CMMFDevSound::SetConfigL left with error = %d"), err);
			ERR_PRINTF2(_L("Expected error value = %d"), KErrNotReady);
			StopTest(err, EFail);
			}
		}
	}

void RA3FDevSoundRecordCancelInitializeTest::InitializeComplete(TInt aError)
	{
	ERR_PRINTF1(_L("DevSound called InitializeComplete after calling CMMFDevSound::CancelInitialize"));
	StopTest(aError, EFail);
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0016
 */
RA3FDevSoundRecordInitializeThenPlayToneInitializeTest::RA3FDevSoundRecordInitializeThenPlayToneInitializeTest(const TDesC& aTestName) 
	: 	RA3FDevSoundTestBase(aTestName), iReinitialized(EFalse)
	{
	}

RA3FDevSoundRecordInitializeThenPlayToneInitializeTest* RA3FDevSoundRecordInitializeThenPlayToneInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordInitializeThenPlayToneInitializeTest* self = new (ELeave) RA3FDevSoundRecordInitializeThenPlayToneInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordInitializeThenPlayToneInitializeTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if(!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundRecordInitializeThenPlayToneInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundRecordInitializeThenPlayToneInitializeTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStateRecording));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				if(!iReinitialized)
					{
					INFO_PRINTF1(_L("========== First initialization successfully completed =========="));
					iReinitialized = ETrue;
					INFO_PRINTF1(_L("Again calling CMMFDevSound::InitializeL"));
					TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
					if(err != KErrNone)
						{
						StopTest(err);
						break;
						}
					INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
					iDevSoundState = EStateInitializing;
					}
				else
					{
					INFO_PRINTF1(_L("========== Second initialization successfully completed =========="));
					StopTest();
					}
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0017
 */
RA3FDevSoundRecordInitializeThenPlayAudioInitializeTest::RA3FDevSoundRecordInitializeThenPlayAudioInitializeTest(const TDesC& aTestName) 
	: 	RA3FDevSoundTestBase(aTestName), iReinitialized(EFalse)
	{
	}

RA3FDevSoundRecordInitializeThenPlayAudioInitializeTest* RA3FDevSoundRecordInitializeThenPlayAudioInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordInitializeThenPlayAudioInitializeTest* self = new (ELeave) RA3FDevSoundRecordInitializeThenPlayAudioInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordInitializeThenPlayAudioInitializeTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if(!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundRecordInitializeThenPlayAudioInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundRecordInitializeThenPlayAudioInitializeTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStateRecording));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				if(!iReinitialized)
					{
					INFO_PRINTF1(_L("========== First initialization successfully completed =========="));
					iReinitialized = ETrue;
					INFO_PRINTF1(_L("Again calling CMMFDevSound::InitializeL"));
					TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
					if(err != KErrNone)
						{
						StopTest(err);
						break;
						}
					INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
					iDevSoundState = EStateInitializing;
					}
				else
					{
					INFO_PRINTF1(_L("========== Second initialization successfully completed =========="));
					StopTest();
					}
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0018
 */
RA3FDevSoundRecordInitializeToRecordAudioTwiceTest::RA3FDevSoundRecordInitializeToRecordAudioTwiceTest(const TDesC& aTestName) 
	: 	RA3FDevSoundTestBase(aTestName), iReinitialized(EFalse)
	{
	}

RA3FDevSoundRecordInitializeToRecordAudioTwiceTest* RA3FDevSoundRecordInitializeToRecordAudioTwiceTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordInitializeToRecordAudioTwiceTest* self = new (ELeave) RA3FDevSoundRecordInitializeToRecordAudioTwiceTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordInitializeToRecordAudioTwiceTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if(!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundRecordInitializeToRecordAudioTwiceTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundRecordInitializeToRecordAudioTwiceTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStateRecording));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				if(!iReinitialized)
					{
					INFO_PRINTF1(_L("========== First initialization successfully completed =========="));
					iReinitialized = ETrue;
					INFO_PRINTF1(_L("Again calling CMMFDevSound::InitializeL"));
					TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStateRecording));
					if(err != KErrNone)
						{
						StopTest(err);
						break;
						}
					INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
					iDevSoundState = EStateInitializing;
					}
				else
					{
					INFO_PRINTF1(_L("========== Second initialization successfully completed =========="));
					StopTest();
					}
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0019
 */
RA3FDevSoundRecordInitializeNoFormatSpecificationPresentedTest::RA3FDevSoundRecordInitializeNoFormatSpecificationPresentedTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundRecordInitializeNoFormatSpecificationPresentedTest* RA3FDevSoundRecordInitializeNoFormatSpecificationPresentedTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordInitializeNoFormatSpecificationPresentedTest* self = new (ELeave) RA3FDevSoundRecordInitializeNoFormatSpecificationPresentedTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordInitializeNoFormatSpecificationPresentedTest::DoKickoffTestL()
	{
	}

void RA3FDevSoundRecordInitializeNoFormatSpecificationPresentedTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundReocordCancelInitializeNoFormatSpecificationPresentedTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateRecording));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}		
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("========== Test Case Successfully Completed =========="));
				StopTest();
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0020
 */
RA3FDevSoundPlayDataWithoutInitializeTest::RA3FDevSoundPlayDataWithoutInitializeTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundPlayDataWithoutInitializeTest* RA3FDevSoundPlayDataWithoutInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayDataWithoutInitializeTest* self = new (ELeave) RA3FDevSoundPlayDataWithoutInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayDataWithoutInitializeTest::DoKickoffTestL()
	{
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundPlayDataWithoutInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundPlayDataWithoutInitializeTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Call CMMFDevSound::PlayData without calling CMMFDevSound::InitializeL..."));
				iMMFDevSound->PlayData();
				StartTimer(KMicroSecsTwoSec);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundPlayDataWithoutInitializeTest::DoTimerCallback()
	{
	iTimer->Cancel();
	ERR_PRINTF1(_L("Panic did not occur as expected when calling PlayData()."));
	StopTest(KErrNone, EFail);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0021
 */
RA3FDevSoundPlayDTMFStringWithoutInitializeTest::RA3FDevSoundPlayDTMFStringWithoutInitializeTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName), iDTMFString(KNullDesC)
	{
	}

RA3FDevSoundPlayDTMFStringWithoutInitializeTest* RA3FDevSoundPlayDTMFStringWithoutInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayDTMFStringWithoutInitializeTest* self = new (ELeave) RA3FDevSoundPlayDTMFStringWithoutInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayDTMFStringWithoutInitializeTest::DoKickoffTestL()
	{
	TPtrC DTMF;
	if(!GetStringFromConfig(iTestStepName, KDTMFString, DTMF))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDTMFString);
		StopTest(KErrNotFound);
		return;
		}
	iDTMFString.Copy(DTMF);
	}

void RA3FDevSoundPlayDTMFStringWithoutInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundPlayDTMFStringWithoutInitializeTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Call CMMFDevSound::PlayDTMFStringL..."));
				TRAPD(err, iMMFDevSound->PlayDTMFStringL(iDTMFString));
				if(err == KErrNotReady)
					{
					ERR_PRINTF2(_L("Calling CMMFDevSound::PlayDTMFStringL  without calling CMMFDevSound::InitializeL left with expected error = %d"), err);
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDTMFStringL  without calling CMMFDevSound::InitializeL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotReady);
					StopTest(err, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0022
 */
RA3FDevSoundPlayDualToneWithoutInitializeTest::RA3FDevSoundPlayDualToneWithoutInitializeTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName), iFrequencyTone1(0), iFrequencyTone2(0)
	{
	}

RA3FDevSoundPlayDualToneWithoutInitializeTest* RA3FDevSoundPlayDualToneWithoutInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayDualToneWithoutInitializeTest* self = new (ELeave) RA3FDevSoundPlayDualToneWithoutInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayDualToneWithoutInitializeTest::DoKickoffTestL()
	{
	if(!GetIntFromConfig(iTestStepName, KDuration, iDuration)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if(!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	if(!GetIntFromConfig(iTestStepName, KFrequencyTone2, iFrequencyTone2)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone2);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundPlayDualToneWithoutInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundPlayDualToneWithoutInitializeTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Call CMMFDevSound::PlayDualToneL..."));
				TRAPD(err, iMMFDevSound->PlayDualToneL(iFrequencyTone1,iFrequencyTone2,iDuration));
				if(err == KErrNotReady)
					{
					ERR_PRINTF2(_L("Calling CMMFDevSound::PlayDualToneL  without calling CMMFDevSound::InitializeL left with expected error = %d"), err);
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayDualToneL  without calling CMMFDevSound::InitializeL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotReady);
					StopTest(err, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0023
 */
RA3FDevSoundPlayFixedSequenceWithoutInitializeTest::RA3FDevSoundPlayFixedSequenceWithoutInitializeTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundPlayFixedSequenceWithoutInitializeTest* RA3FDevSoundPlayFixedSequenceWithoutInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayFixedSequenceWithoutInitializeTest* self = new (ELeave) RA3FDevSoundPlayFixedSequenceWithoutInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayFixedSequenceWithoutInitializeTest::DoKickoffTestL()
	{
	}

void RA3FDevSoundPlayFixedSequenceWithoutInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundPlayFixedSequenceWithoutInitializeTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Call CMMFDevSound::PlayFixedSequenceL..."));
				TRAPD(err, iMMFDevSound->PlayFixedSequenceL(0));
				if(err == KErrNotSupported)
					{
					ERR_PRINTF2(_L("Calling CMMFDevSound::PlayFixedSequenceL  without calling CMMFDevSound::InitializeL left with expected error = %d"), err);
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayFixedSequenceL  without calling CMMFDevSound::InitializeL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotReady);
					StopTest(err, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0024
 */
RA3FDevSoundPlayInitWithoutInitializeTest::RA3FDevSoundPlayInitWithoutInitializeTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundPlayInitWithoutInitializeTest* RA3FDevSoundPlayInitWithoutInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayInitWithoutInitializeTest* self = new (ELeave) RA3FDevSoundPlayInitWithoutInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayInitWithoutInitializeTest::DoKickoffTestL()
	{
	}

void RA3FDevSoundPlayInitWithoutInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundPlayInitWithoutInitializeTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Call CMMFDevSound::PlayInitL without calling CMMFDevSound::InitializeL..."));
				TRAPD(err,iMMFDevSound->PlayInitL());
				if(err == KErrNotReady)
					{
					ERR_PRINTF2(_L("Calling CMMFDevSound::PlayInitL  without calling CMMFDevSound::InitializeL left with expected error = %d"), err);
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayInitL  without calling CMMFDevSound::InitializeL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotReady);
					StopTest(err, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0025
 */
RA3FDevSoundPlayToneWithoutInitializeTest::RA3FDevSoundPlayToneWithoutInitializeTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName), iFrequencyTone1(0)
	{
	}

RA3FDevSoundPlayToneWithoutInitializeTest* RA3FDevSoundPlayToneWithoutInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayToneWithoutInitializeTest* self = new (ELeave) RA3FDevSoundPlayToneWithoutInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayToneWithoutInitializeTest::DoKickoffTestL()
	{
	if(!GetIntFromConfig(iTestStepName, KDuration, iDuration)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration);
		StopTest(KErrNotFound);
		return;
		}
	if(!GetIntFromConfig(iTestStepName, KFrequencyTone1, iFrequencyTone1)) 
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFrequencyTone1);
		StopTest(KErrNotFound);
		return;
		}
	}

void RA3FDevSoundPlayToneWithoutInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundPlayToneWithoutInitializeTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Call CMMFDevSound::PlayToneL..."));
				TRAPD(err, iMMFDevSound->PlayToneL(iFrequencyTone1, iDuration));
				if(err == KErrNotReady)
					{
					ERR_PRINTF2(_L("Calling CMMFDevSound::PlayToneL  without calling CMMFDevSound::InitializeL left with expected error = %d"), err);
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneL  without calling CMMFDevSound::InitializeL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotReady);
					StopTest(err, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0026
 */
RA3FDevSoundPlayToneSequenceWithoutInitializeTest::RA3FDevSoundPlayToneSequenceWithoutInitializeTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundPlayToneSequenceWithoutInitializeTest* RA3FDevSoundPlayToneSequenceWithoutInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayToneSequenceWithoutInitializeTest* self = new (ELeave) RA3FDevSoundPlayToneSequenceWithoutInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayToneSequenceWithoutInitializeTest::DoKickoffTestL()
	{
	}

void RA3FDevSoundPlayToneSequenceWithoutInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundPlayToneSequenceWithoutInitializeTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				TUint8* tablePointer = const_cast<TUint8*>( &(KFixedSequenceTestSequenceDataX[0] )); 
				TPtrC8 KFixedSequenceData(tablePointer,  sizeof(KFixedSequenceTestSequenceDataX));
				INFO_PRINTF1(_L("Call CMMFDevSound::PlayToneSequenceL..."));
				TRAPD(err, iMMFDevSound->PlayToneSequenceL(KFixedSequenceData));
				if(err == KErrNotReady)
					{
					ERR_PRINTF2(_L("Calling CMMFDevSound::PlayToneSequenceL  without calling CMMFDevSound::InitializeL left with expected error = %d"), err);
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::PlayToneSequenceL  without calling CMMFDevSound::InitializeL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotReady);
					StopTest(err, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0027
 */
RA3FDevSoundRecordDataWithoutInitializeTest::RA3FDevSoundRecordDataWithoutInitializeTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundRecordDataWithoutInitializeTest* RA3FDevSoundRecordDataWithoutInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordDataWithoutInitializeTest* self = new (ELeave) RA3FDevSoundRecordDataWithoutInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordDataWithoutInitializeTest::DoKickoffTestL()
	{
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

void RA3FDevSoundRecordDataWithoutInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundRecordDataWithoutInitializeTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Call CMMFDevSound::RecordData without calling CMMFDevSound::InitializeL..."));
				iMMFDevSound->RecordData();
				StartTimer(KMicroSecsTwoSec);
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}

void RA3FDevSoundRecordDataWithoutInitializeTest::DoTimerCallback()
	{
	iTimer->Cancel();
	ERR_PRINTF1(_L("Panic did not occur as expected when calling RecordData()."));
	StopTest(KErrNone, EFail);
	}

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0028
 */
RA3FDevSoundRecordInitWithoutInitializeTest::RA3FDevSoundRecordInitWithoutInitializeTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundRecordInitWithoutInitializeTest* RA3FDevSoundRecordInitWithoutInitializeTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordInitWithoutInitializeTest* self = new (ELeave) RA3FDevSoundRecordInitWithoutInitializeTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordInitWithoutInitializeTest::DoKickoffTestL()
	{
	}

void RA3FDevSoundRecordInitWithoutInitializeTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundRecordInitWithoutInitializeTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Call CMMFDevSound::RecordInitL without calling CMMFDevSound::InitializeL..."));
				TRAPD(err,iMMFDevSound->RecordInitL());
				if(err == KErrNotReady)
					{
					ERR_PRINTF2(_L("Calling CMMFDevSound::RecordInitL  without calling CMMFDevSound::InitializeL left with expected error = %d"), err);
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("CMMFDevSound::RecordInitL  without calling CMMFDevSound::InitializeL left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotReady);
					StopTest(err, EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0029
 */
RA3FDevSoundToneInitializationMoreThanOnceBeforeInitCompleteTest::RA3FDevSoundToneInitializationMoreThanOnceBeforeInitCompleteTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundToneInitializationMoreThanOnceBeforeInitCompleteTest* RA3FDevSoundToneInitializationMoreThanOnceBeforeInitCompleteTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneInitializationMoreThanOnceBeforeInitCompleteTest* self = new (ELeave) RA3FDevSoundToneInitializationMoreThanOnceBeforeInitCompleteTest(aTestName);
	return self;
	}

void RA3FDevSoundToneInitializationMoreThanOnceBeforeInitCompleteTest::DoKickoffTestL()
	{
	}

void RA3FDevSoundToneInitializationMoreThanOnceBeforeInitCompleteTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundToneInitializationMoreThanOnceBeforeInitCompleteTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("CMMFDevSound::InitializeL called twice before init complete"));
				TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if(err != KErrNotReady)
					{
					ERR_PRINTF2(_L("Calling CMMFDevSound::InitializeL more than once before init complete left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotReady);
					StopTest(err, EFail);
					}
				else
					{
					INFO_PRINTF1(_L("Second call to CMMFDevSound::InitializeL leaves with KErrNotReady as expected"));
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("========== Test Case Successfully Completed =========="));
				StopTest();
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0030
 */
RA3FDevSoundPlayInitializationMoreThanOnceBeforeInitCompleteTest::RA3FDevSoundPlayInitializationMoreThanOnceBeforeInitCompleteTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundPlayInitializationMoreThanOnceBeforeInitCompleteTest* RA3FDevSoundPlayInitializationMoreThanOnceBeforeInitCompleteTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayInitializationMoreThanOnceBeforeInitCompleteTest* self = new (ELeave) RA3FDevSoundPlayInitializationMoreThanOnceBeforeInitCompleteTest(aTestName);
	return self;
	}

void RA3FDevSoundPlayInitializationMoreThanOnceBeforeInitCompleteTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if(!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundPlayInitializationMoreThanOnceBeforeInitCompleteTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundPlayInitializationMoreThanOnceBeforeInitCompleteTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("CMMFDevSound::InitializeL called twice before init complete"));
				TRAP(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if(err != KErrNotReady)
					{
					ERR_PRINTF2(_L("Calling CMMFDevSound::InitializeL more than once before init complete left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotReady);
					StopTest(err, EFail);
					}
				else
					{
					INFO_PRINTF1(_L("Second call to CMMFDevSound::InitializeL leaves with KErrNotReady as expected"));
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("========== Test Case Successfully Completed =========="));
				StopTest();
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0031
 */
RA3FDevSoundRecordInitializationMoreThanOnceBeforeInitCompleteTest::RA3FDevSoundRecordInitializationMoreThanOnceBeforeInitCompleteTest(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundRecordInitializationMoreThanOnceBeforeInitCompleteTest* RA3FDevSoundRecordInitializationMoreThanOnceBeforeInitCompleteTest::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordInitializationMoreThanOnceBeforeInitCompleteTest* self = new (ELeave) RA3FDevSoundRecordInitializationMoreThanOnceBeforeInitCompleteTest(aTestName);
	return self;
	}

void RA3FDevSoundRecordInitializationMoreThanOnceBeforeInitCompleteTest::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if(!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundRecordInitializationMoreThanOnceBeforeInitCompleteTest::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundRecordInitializationMoreThanOnceBeforeInitCompleteTest"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStateRecording));
				if(err != KErrNone)
					{
					INFO_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("CMMFDevSound::InitializeL called twice before init complete"));
				TRAP(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStateRecording));
				if(err != KErrNotReady)
					{
					ERR_PRINTF2(_L("Calling MMFDevSound::InitializeL more than once before init complete left with error = %d"), err);
					ERR_PRINTF2(_L("Expected error = %d"), KErrNotReady);
					StopTest(err, EFail);
					}
				else
					{
					INFO_PRINTF1(_L("Second call to CMMFDevSound::InitializeL leaves with KErrNotReady as expected"));
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("========== Test Case Successfully Completed =========="));
				StopTest();
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0032
 */
RA3FDevSoundToneFirstInitializationLeavesThenSecondSucceeds::RA3FDevSoundToneFirstInitializationLeavesThenSecondSucceeds(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundToneFirstInitializationLeavesThenSecondSucceeds* RA3FDevSoundToneFirstInitializationLeavesThenSecondSucceeds::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundToneFirstInitializationLeavesThenSecondSucceeds* self = new (ELeave) RA3FDevSoundToneFirstInitializationLeavesThenSecondSucceeds(aTestName);
	return self;
	}

void RA3FDevSoundToneFirstInitializationLeavesThenSecondSucceeds::DoKickoffTestL()
	{
	}

void RA3FDevSoundToneFirstInitializationLeavesThenSecondSucceeds::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundToneFirstInitializationLeavesThenSecondSucceed"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to make it leave"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, EMMFStateConverting));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					}
				else
					{
					ERR_PRINTF1(_L("CMMFDevSound::InitializeL  did not leave as expected"));
					StopTest(err, EFail);
					}
				INFO_PRINTF1(_L("CMMFDevSound::InitializeL called twice, but now using right parameters"));
				TRAP(err, iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("========== Test Case Successfully Completed =========="));
				StopTest();
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0033
 */
RA3FDevSoundPlayFirstInitializationLeavesThenSecondSucceeds::RA3FDevSoundPlayFirstInitializationLeavesThenSecondSucceeds(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundPlayFirstInitializationLeavesThenSecondSucceeds* RA3FDevSoundPlayFirstInitializationLeavesThenSecondSucceeds::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundPlayFirstInitializationLeavesThenSecondSucceeds* self = new (ELeave) RA3FDevSoundPlayFirstInitializationLeavesThenSecondSucceeds(aTestName);
	return self;
	}

void RA3FDevSoundPlayFirstInitializationLeavesThenSecondSucceeds::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if(!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundPlayFirstInitializationLeavesThenSecondSucceeds::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundPlayFirstInitializationLeavesThenSecondSucceed"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to make it leave"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStateConverting));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					}
				else
					{
					ERR_PRINTF1(_L("CMMFDevSound::InitializeL  did not leave as expected"));
					StopTest(err, EFail);
					}
				INFO_PRINTF1(_L("CMMFDevSound::InitializeL called twice, but now using right parameters"));
				TRAP(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStatePlaying));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("========== Test Case Successfully Completed =========="));
				StopTest();
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0034
 */
RA3FDevSoundRecordFirstInitializationLeavesThenSecondSucceeds::RA3FDevSoundRecordFirstInitializationLeavesThenSecondSucceeds(const TDesC& aTestName) 
	:	RA3FDevSoundTestBase(aTestName)
	{
	}

RA3FDevSoundRecordFirstInitializationLeavesThenSecondSucceeds* RA3FDevSoundRecordFirstInitializationLeavesThenSecondSucceeds::NewL(const TDesC& aTestName)
	{
	RA3FDevSoundRecordFirstInitializationLeavesThenSecondSucceeds* self = new (ELeave) RA3FDevSoundRecordFirstInitializationLeavesThenSecondSucceeds(aTestName);
	return self;
	}

void RA3FDevSoundRecordFirstInitializationLeavesThenSecondSucceeds::DoKickoffTestL()
	{
	TPtrC fourccCode;
	if(!GetStringFromConfig(iTestStepName, KFourccCode, fourccCode))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KFourccCode);
		StopTest(KErrNotFound);
		return;
		}
	EncodingFromStringToTFourCC(fourccCode);
	}

void RA3FDevSoundRecordFirstInitializationLeavesThenSecondSucceeds::Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError)
	{
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone),Panic(_L("RA3FDevSoundRecordFirstInitializationLeavesThenSecondSucceed"),EFsmIncorrectErrorPassed));
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL to make it leave"));
				TRAPD(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStateConverting));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					}
				else
					{
					ERR_PRINTF1(_L("CMMFDevSound::InitializeL  did not leave as expected"));
					StopTest(err, EFail);
					}
				INFO_PRINTF1(_L("CMMFDevSound::InitializeL called twice, but now using right parameters"));
				TRAP(err, iMMFDevSound->InitializeL(*this, iFourCCCode, EMMFStateRecording));
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("CMMFDevSound::InitializeL left with error = %d"), err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
				iDevSoundState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitialize not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("========== Test Case Successfully Completed =========="));
				StopTest();
				}
			else if(aDevSoundEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("DevSound called CMMFDevSound::InitializeComplete with error = %d"), aError);
				StopTest(aError);		
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventInitComplete not received as expected. Received event: %d"), aDevSoundEvent);
				StopTest(aError, EFail);
				}
			break;
			}	
		default:
			{
			ERR_PRINTF2(_L("Invalid DevSound state received: %d"), iDevSoundState);
			StopTest(aError, EFail);
			}
		}
	}
