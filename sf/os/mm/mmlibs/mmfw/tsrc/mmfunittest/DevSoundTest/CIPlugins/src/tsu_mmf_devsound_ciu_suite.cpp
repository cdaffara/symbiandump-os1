// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>

//Test system includes
#include "tsu_mmf_devsound_ciu_suite.h"
#include "tsu_mmf_devsound_ciu_step.h"
#include "teststepciplaybackstatus.h"
#include "teststepcieaacplusdecoder.h"
#include "teststepcisbcencoder.h"
#include "teststepciadvancedaacencodesettings.h"
#include "teststepcisetdrmprotected.h"
#include "teststepcigettimestamps.h"
#include "teststepcibufferframesconfig.h"

#include "teststepciaacdecoderconfig.h"
#include "teststepcig729decoderconfig.h"
#include "teststepcig729encoderconfig.h"
#include "teststepciaudiobufferprefill.h"
#include "teststepciaudiovibracontrol.h"
#include "teststepciaudiodspcontrol.h"
#include "teststepcispeechencoderconfig.h"
#include "teststepcig711decoderconfig.h"
#include "teststepcig711encoderconfig.h"
#include "teststepciilbcdecoderconfig.h"
#include "teststepciilbcencoderconfig.h"
#include "teststepcierrorconcealment.h"
#include "teststepcigsmconfig.h"
#include "teststepciunderflowautostopcontrol.h"

/**
 *
 * Create an instance of this test suite.
 *
 * NewTestSuiteL() is exported at ordinal 1.
 * This provides the interface to allow the Test Framework
 * to create instances of this test suite.
 *
 * @return	"CTestSuite*"
 *		The newly created test suite
 */
EXPORT_C CTestSuite* NewTestSuiteL()
	{
	CTestSuiteCustomInterfaceUnificationUnit* result = new (ELeave) CTestSuiteCustomInterfaceUnificationUnit;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(result); 
	return result;
	}

/**
 * CTestDevSoundCiuSuite destructor
 */
CTestSuiteCustomInterfaceUnificationUnit::~CTestSuiteCustomInterfaceUnificationUnit()
	{	
	}

/**
This test suite version string
*/
_LIT(KTxtVersion,"0.1 (build 1-May-2006)");

/**
Get test suite version.

@return	TPtrC, The version string.
*/
TPtrC CTestSuiteCustomInterfaceUnificationUnit::GetVersion() const
	{
	return KTxtVersion();
	}

/**
Add a test step into the suite.

@param	CTestStepCustomInterfaceUnificationUnit* aTestStep
		The test step to be added.
*/
void CTestSuiteCustomInterfaceUnificationUnit::AddTestStepL( CTestStepCustominterfaceUnificationUnit* aTestStep )
	{
	// test steps contain a pointer back to the suite which owns them
	aTestStep->SetSuite(this);   

	// add the step using the base class method
	CTestSuite::AddTestStepL(aTestStep);
	}

/**
Initialiser for test suite.

This creates all the test steps and stores them
inside CTestSuiteCustomInterfaceUnificationUnit
*/
void CTestSuiteCustomInterfaceUnificationUnit::InitialiseL()
	{
	// store the name of this test suite
	iSuiteName = _L("TSU_MMF_DEVSOUND_CIU");

	// add test steps
	//MSpeechEncoderConfig - Instantiating
	AddTestStepL( new(ELeave) CTestStepCISpeechEncoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0001-HP")) );
	//MSpeechEncoderConfig - SetVadMode
	AddTestStepL( new(ELeave) CTestStepCISpeechEncoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0002-HP")) );
	//MSpeechEncoderConfig - GetVadMode
	AddTestStepL( new(ELeave) CTestStepCISpeechEncoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0003-HP")) );
	//MAacDecoderConfig - Instantiating
	AddTestStepL( new(ELeave) CTestStepCIAacDecoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0004-HP")) );
	//MAacDecoderConfig - SetAudioConfig
	AddTestStepL( new(ELeave) CTestStepCIAacDecoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0005-HP")) );
	// Steps 6 - 7
	// Advanced AAC Encode Settings
	AddTestStepL( new(ELeave) CTestStepCICreateAdvancedAacEncodeSettings);
	// Test the set method
	AddTestStepL( new(ELeave) CTestStepCIAdvancedAacEncodeSettings(EFalse));	
	// Steps 8 - 19
	// EAAC+ Decoder
	AddTestStepL( new(ELeave) CTestStepCIEAacPlusDecoder );
	AddTestStepL( new(ELeave) CTestStepCIEAacPlusDecoderGetSetFrequency(ETrue) );
	AddTestStepL( new(ELeave) CTestStepCIEAacPlusDecoderGetSetObjType(ETrue) );
	AddTestStepL( new(ELeave) CTestStepCIEAacPlusDecoderGetSetNumChannels(ETrue) );
	AddTestStepL( new(ELeave) CTestStepCIEAacPlusDecoderGetSetSbr(ETrue) );
	AddTestStepL( new(ELeave) CTestStepCIEAacPlusDecoderGetSetDsm(ETrue) );
	AddTestStepL( new(ELeave) CTestStepCIEAacPlusDecoderApplyConfig );
	AddTestStepL( new(ELeave) CTestStepCIEAacPlusDecoderGetSetFrequency(EFalse) );
	AddTestStepL( new(ELeave) CTestStepCIEAacPlusDecoderGetSetObjType(EFalse) );
	AddTestStepL( new(ELeave) CTestStepCIEAacPlusDecoderGetSetNumChannels(EFalse) );
	AddTestStepL( new(ELeave) CTestStepCIEAacPlusDecoderGetSetSbr(EFalse) );
	AddTestStepL( new(ELeave) CTestStepCIEAacPlusDecoderGetSetDsm(EFalse) );
	//  Steps 20 - 39
	//
	// SBC Encoder
	AddTestStepL( new(ELeave) CTestStepCISbcEncoder );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSupportedFrequencies );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSupportedSubbands );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSupportedNumOfBlocks );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSupportedChannelModes );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSupportedAllocationMethods );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSupportedBitpoolRange );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSetFrequency(ETrue) );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSetChannelMode(ETrue) );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSetNumOfSubbands(ETrue) );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSetNumOfBlocks(ETrue) );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSetAllocationMethod(ETrue) );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSetBitpoolSize(ETrue) );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderApplyConfig );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSetFrequency(EFalse) );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSetChannelMode(EFalse) );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSetNumOfSubbands(EFalse) );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSetNumOfBlocks(EFalse) );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSetAllocationMethod(EFalse) );
	AddTestStepL( new(ELeave) CTestStepCISbcEncoderGetSetBitpoolSize(EFalse) );
	//MG711DecoderIntfc - Instantiating
	AddTestStepL( new(ELeave) CTestStepCIG711DecoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0040-HP")) );
	//MG711DecoderIntfc - SetDecoderMode
	AddTestStepL( new(ELeave) CTestStepCIG711DecoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0041-HP")) );
	//MG711DecoderIntfc - SetComfortNoiseGeneration
	AddTestStepL( new(ELeave) CTestStepCIG711DecoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0042-HP")) );
	//MG711DecoderIntfc - GetComfortNoiseGeneration
	AddTestStepL( new(ELeave) CTestStepCIG711DecoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0043-HP")) );
	//MG711DecoderIntfc - SetPacketLossConcealment
	AddTestStepL( new(ELeave) CTestStepCIG711DecoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0044-HP")) );
	//MG711EncoderIntfc - Instantiating
	AddTestStepL( new(ELeave) CTestStepCIG711EncoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0045-HP")) );
	//MG711EncoderIntfc - SetEncoderMode
	AddTestStepL( new(ELeave) CTestStepCIG711EncoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0046-HP")) );
	//MG711EncoderIntfc - SetVadMode
	AddTestStepL( new(ELeave) CTestStepCIG711EncoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0047-HP")) );
	//MG711EncoderIntfc - GetVadMode
	AddTestStepL( new(ELeave) CTestStepCIG711EncoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0048-HP")) );
	//MG729DecoderIntfc - Instantiating
	AddTestStepL( new(ELeave) CTestStepCIG729DecoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0049-HP")) );
	//MG729DecoderIntfc - BadLsfNextBuffer
	AddTestStepL( new(ELeave) CTestStepCIG729DecoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0050-HP")) );
	//MG729EncoderIntfc - Instantiating
	AddTestStepL( new(ELeave) CTestStepCIG729EncoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0051-HP")) );
	//MG729EncoderIntfc - SetVadMode
	AddTestStepL( new(ELeave) CTestStepCIG729EncoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0052-HP")) );
	//MG729EncoderIntfc - GetVadMode
	AddTestStepL( new(ELeave) CTestStepCIG729EncoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0053-HP")) );
	//MIlbcDecoderIntfc - Instantiating
	AddTestStepL( new(ELeave) CTestStepCIIlbcDecoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0054-HP")) );
	//MIlbcDecoderIntfc - SetDecoderMode
	AddTestStepL( new(ELeave) CTestStepCIIlbcDecoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0055-HP")) );
	//MIlbcDecoderIntfc - SetComfortNoiseGeneration
	AddTestStepL( new(ELeave) CTestStepCIIlbcDecoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0056-HP")) );
	//MIlbcDecoderIntfc - GetComfortNoiseGeneration
	AddTestStepL( new(ELeave) CTestStepCIIlbcDecoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0057-HP")) );
	//MIlbcEncoderIntfc - Instantiating
	AddTestStepL( new(ELeave) CTestStepCIIlbcEncoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0058-HP")) );
	//MIlbcEncoderIntfc - SetEncoderMode
	AddTestStepL( new(ELeave) CTestStepCIIlbcEncoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0059-HP")) );
	//MIlbcEncoderIntfc - SetVadMode
	AddTestStepL( new(ELeave) CTestStepCIIlbcEncoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0060-HP")) );
	//MIlbcEncoderIntfc - GetVadMode
	AddTestStepL( new(ELeave) CTestStepCIIlbcEncoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0061-HP")) );
	//MErrorConcealmentIntfc - Instantiating
	AddTestStepL( new(ELeave) CTestStepCIErrorConcealmentIntfc(_L("MM-MMF-DEVSOUNDCI-U-0062-HP")) );
	//MErrorConcealmentIntfc - ConcealErrorForNextBuffer
	AddTestStepL( new(ELeave) CTestStepCIErrorConcealmentIntfc(_L("MM-MMF-DEVSOUNDCI-U-0063-HP")) );
	//MErrorConcealmentIntfc - FrameModeRqrdForEC
	AddTestStepL( new(ELeave) CTestStepCIErrorConcealmentIntfc(_L("MM-MMF-DEVSOUNDCI-U-0064-HP")) );
	//MErrorConcealmentIntfc - SetFrameMode
	AddTestStepL( new(ELeave) CTestStepCIErrorConcealmentIntfc(_L("MM-MMF-DEVSOUNDCI-U-0065-HP")) );
	//MAudioVibraControl - Instantiating
	AddTestStepL( new(ELeave) CTestStepCIAudioVibraControl(_L("MM-MMF-DEVSOUNDCI-U-0066-HP")) );
	//MAudioVibraControl - StartVibra
	AddTestStepL( new(ELeave) CTestStepCIAudioVibraControl(_L("MM-MMF-DEVSOUNDCI-U-0067-HP")) );
	//MAudioVibraControl - StopVibra
	AddTestStepL( new(ELeave) CTestStepCIAudioVibraControl(_L("MM-MMF-DEVSOUNDCI-U-0068-HP")) );
	//MMMFSampleBuffering - Instantiating
	AddTestStepL( new(ELeave) CTestStepCISampleBuffering(_L("MM-MMF-DEVSOUNDCI-U-0069-HP")) );
	//MMMFSampleBuffering - MmsbEnableSampleBufferingBeforePlayback
	AddTestStepL( new(ELeave) CTestStepCISampleBuffering(_L("MM-MMF-DEVSOUNDCI-U-0070-HP")) );
	//MMMFSampleBuffering - MmsbDisableSampleBufferingBeforePlayback
	AddTestStepL( new(ELeave) CTestStepCISampleBuffering(_L("MM-MMF-DEVSOUNDCI-U-0071-HP")) );
	//MMMFSampleBuffering - MmsbNotifyPlayStarted
	AddTestStepL( new(ELeave) CTestStepCISampleBuffering(_L("MM-MMF-DEVSOUNDCI-U-0072-HP")) );
	//MMMFSampleBuffering - MmsbCancelNotifyPlayStarted
	AddTestStepL( new(ELeave) CTestStepCISampleBuffering(_L("MM-MMF-DEVSOUNDCI-U-0073-HP")) );
	// MMMFPlaybackStatus
	AddTestStepL( new(ELeave) CTestStepCIPlaybackStatus );
	// MMMFPlaybackStatus - MmpsGetPlaybackStatusInformation	
	AddTestStepL( new(ELeave) CTestStepCIPlaybackStatusInfo );	
	//MMMFDSPControl - Instantiating
	AddTestStepL( new(ELeave) CTestStepCIAudioDspControl(_L("MM-MMF-DEVSOUNDCI-U-0076-HP")) );
	//MMMFDSPControl - MmdspcRequestLossOfSyncNotification
	AddTestStepL( new(ELeave) CTestStepCIAudioDspControl(_L("MM-MMF-DEVSOUNDCI-U-0077-HP")) );
	//MMMFDSPControl - MmdspcCancelLossOfSyncNotification
	AddTestStepL( new(ELeave) CTestStepCIAudioDspControl(_L("MM-MMF-DEVSOUNDCI-U-0078-HP")) );
	//MMMFDSPControl - MmdspcGetAudioPlaybackInfo
	AddTestStepL( new(ELeave) CTestStepCIAudioDspControl(_L("MM-MMF-DEVSOUNDCI-U-0079-HP")) );
	// Steps 80 - 81
	// Set DRM Protected
	AddTestStepL( new(ELeave) CTestStepCICreateSetDRMProtected);
	AddTestStepL( new(ELeave) CTestStepCISetDRMProtected);
	// Steps 82 - 84
	// Buffer Frames Config
	AddTestStepL( new(ELeave) CTestStepCICreateBufferFramesConfig);
	AddTestStepL( new(ELeave) CTestStepCIBufferFramesConfigSetInput);
	AddTestStepL( new(ELeave) CTestStepCIBufferFramesConfigSetOutput);
	// Steps 85 - 87
	// Get timestamps
	AddTestStepL( new(ELeave) CTestStepCICreateGetTimestamps);
	AddTestStepL( new(ELeave) CTestStepCIGetTimestampsEnable);
	AddTestStepL( new(ELeave) CTestStepCIGetTimestampsForBuffer);
	//MMMFGsmConfig - Instantiating
	AddTestStepL( new(ELeave) CTestStepCIGsmConfig(_L("MM-MMF-DEVSOUNDCI-U-0088-HP")) );
	//MMMFGsmConfig - SetConversionFormat
	AddTestStepL( new(ELeave) CTestStepCIGsmConfig(_L("MM-MMF-DEVSOUNDCI-U-0089-HP")) );
	//MMMFGsmConfig - ConversionFormat
	AddTestStepL( new(ELeave) CTestStepCIGsmConfig(_L("MM-MMF-DEVSOUNDCI-U-0090-HP")) );
	//MMMFUnderflowAutoStopControl - Instantiation
	AddTestStepL( new(ELeave) CTestStepCIUnderflowAutoStopControl(_L("MM-MMF-DEVSOUNDCI-U-0091-HP")) );
	//MMMFUnderflowAutoStopControl - MmuascTurnOffUnderflowAutoStop
	AddTestStepL( new(ELeave) CTestStepCIUnderflowAutoStopControl(_L("MM-MMF-DEVSOUNDCI-U-0092-HP")) );
	//MAacDecoderConfig - GetSupportedAudioConfigs
	AddTestStepL( new(ELeave) CTestStepCIAacDecoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0093-HP")) );
	//MG711DecoderIntfc - GetDecoderMode
	AddTestStepL( new(ELeave) CTestStepCIG711DecoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0094-HP")) );
	//MG711DecoderIntfc - GetPacketLossConcealment
	AddTestStepL( new(ELeave) CTestStepCIG711DecoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0095-HP")) );
	//MIlbcEncoderIntfc - GetDecoderMode
	AddTestStepL( new(ELeave) CTestStepCIIlbcEncoderConfig(_L("MM-MMF-DEVSOUNDCI-U-0096-HP")) );
	// PlaybackStatus - Request Notification
	AddTestStepL( new(ELeave) CTestStepCIPlaybackStatusRequestNotification(EFalse) );
	// PlaybackStatus - Cancel request
	AddTestStepL( new(ELeave) CTestStepCIPlaybackStatusRequestNotification(ETrue) );
	// Advanced AAC Encode Settings
	// Test the get method
	AddTestStepL( new(ELeave) CTestStepCIAdvancedAacEncodeSettings(ETrue));	
	}
