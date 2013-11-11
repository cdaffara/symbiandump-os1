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
//

// EPOC includes
#include <e32base.h>

#include <mdaaudiosampleplayer.h>
#include <mdaaudiosampleeditor.h>
#include <mdaaudiotoneplayer.h>

// Test system includes
#include <testframework.h>
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

#include "OneExampleUnitMMFAudClientTest.h"
#include "TestStepPlayerNew.h"
#include "TestStepPlayerManip.h"
#include "TestStepPlayerAllocFailNew.h"
#include "TestStepPlayerAllocFailOpen.h"

#include "TestStepRecorder.h"
#include "TestStepRecorderAllocFail.h"

#include "TestStepTone.h"
#include "TestStepConvert.h"
#include "TestStepAudOutStream.h"
#include "TestStepAudOutStreamGSM610.h"
#include "TestStepAudInStream.h"
#include "TestStepPlayerBitRate.h"
#include "TestStepRecorderBitRate.h"
#include "TestStepMiscAud.h"
#include "TestStepResourceNotification.h"
#include "TestStepPlayerSharedHeap.h"

#include "TestStepAudioPlayerDRM.h"
#include "TestStepAudioRecorderDRM.h"
#include "TestStepRecorderSharedHeap.h"
#include "TestStepConverterSharedHeap.h"
#include "TestStepCtrlSetRepeatsCustomCommands.h"

/**
 *
 * Create an instance of this test suite.
 *
 * NewTestSuiteUnitMMFAudClientL is exported at ordinal 1.
 * This provides the interface to allow the Test Framework
 * to create instances of this test suite.
 *
 * @return	"CTestSuiteUnitMMFAudClient*"
 *			The newly created test suite
 */
EXPORT_C CTestSuiteUnitMMFAudClient* NewTestSuiteUnitMMFAudClientL() 
    { 
	CTestSuiteUnitMMFAudClient* result = new (ELeave) CTestSuiteUnitMMFAudClient;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

/**
 *
 * CTestSuiteUnitMMFAudClient destructor
 *
 * @xxxx
 * 
 */
CTestSuiteUnitMMFAudClient::~CTestSuiteUnitMMFAudClient()
	{
	}

/**
 *
 * This test suite version string
 *
 * @xxxx
 *
 */
_LIT(KTxtVersion,"0.1a (build 13-Aug-2002)");

/**
 *
 * Get test suite version.
 *
 * @return	"TPtrC"
 *			The version string.
 *
 * @xxxx
 *
 */
TPtrC CTestSuiteUnitMMFAudClient::GetVersion() const
	{
	return KTxtVersion();
	}

/**
 *
 * Add a test step into the suite.
 *
 * @param	"CTestStepUnitMMFAudClient* aTestStep"
 *			The test step to be added.
 * @xxxx
 * 
 */
void CTestSuiteUnitMMFAudClient::AddTestStepL( CTestStepUnitMMFAudClient* aTestStep )
	{
	// test steps contain a pointer back to the suite which owns them
	aTestStep->SetUnitMMFAudClientSuite(this); 

	// add the step using tyhe base class method
	CTestSuite::AddTestStepL(aTestStep);
	}

void CTestSuiteUnitMMFAudClient::AddTestStepL( RTestStep* aTestStep )
	{

	// add the step using tyhe base class method
	CTestSuite::AddTestStepL(aTestStep);
	}

/**
 *
 * Initialiser for test suite.
 * This creates all the test steps and stores them
 * inside CTestSuiteUnitMMFAudClient
 *
 * @xxxx
 * 
 */
void CTestSuiteUnitMMFAudClient::InitialiseL()
	{
	InitSystemPath();
	// store the name of this test suite
	iSuiteName = _L("TSU_MMF_ACLNT_01");

	// add test steps
	AddTestStepL(CTestStepPlayerNew::NewL());
	AddTestStepL(CTestStepPlayerNewFilePlayer::NewL());
	AddTestStepL(CTestStepPlayerNewDescPlayer::NewL());
	AddTestStepL(CTestStepPlayerNewPriorPrefer::NewL());
	AddTestStepL(CTestStepPlayerNewPrefer::NewL());
	AddTestStepL(CTestStepPlayerPlay::NewL());
	AddTestStepL(CTestStepPlayerPlayTwice::NewL());
	AddTestStepL(CTestStepPlayerPlayThreeTimes::NewL());
	AddTestStepL(CTestStepPlayerClosePlay::NewL());
	AddTestStepL(CTestStepPlayerOpenUrl::NewL());

	AddTestStepL(CTestStepPlayerPlayAu::NewL());
    AddTestStepL(CTestStepCheckHeaderTag::NewL());
	AddTestStepL(CTestStepPlayerFileHandle::NewL());
	AddTestStepL(CTestStepPlayerStop::NewL());
	AddTestStepL(CTestStepPlayerDuration::NewL());
	AddTestStepL(CTestStepPlayerRepeats::NewL(_L("MM-MMF-ACLNT-U-0036-CP"), _L("SectionOne"), _L("AudioPlayFName1")));
	//Test step for tone repeats
	AddTestStepL(CTestStepPlayerRepeats::NewL(_L("MM-MMF-ACLNT-U-0067-HP"), _L("SectionOne"), _L("AudioPlayFName12")));
	AddTestStepL(CTestStepPlayerVol::NewL());
	AddTestStepL(CTestStepPlayerVolRamp::NewL());
	AddTestStepL(CTestStepPlayerPosition::NewL());
	AddTestStepL(CTestStepPlayerPlayWindow::NewL());
	AddTestStepL(CTestStepPlayerBalance::NewL());
	AddTestStepL(CTestStepPlayerClose::NewL());
	AddTestStepL(CTestStepPlayerMeta::NewL());
	AddTestStepL(CTestStepPlayerPriority::NewL());
	AddTestStepL(CTestStepPlayerPauseSetPosPlay::NewL());
	AddTestStepL(CTestStepPlayerPlayWaitSetPos::NewL());
	AddTestStepL(CTestStepMultipleSetPlayerPriority::NewL());
	AddTestStepL(CTestStepPlayerPlayPausePlay::NewL());
	
	AddTestStepL(CTestStepPlayerGetImplementationInfo::NewL());
	AddTestStepL(CTestStepPlayerSendCustomCommandSync::NewL());
	AddTestStepL(CTestStepPlayerSendCustomCommandAsync::NewL());
	AddTestStepL(CTestStepPlayerReloadingTest::NewL());
	//DEF083334 SWSV_604_R4B_Video: Real media icon still visible when changing to a non realVideo file
	AddTestStepL(CTestStepPlayerCheckImplementationInfo::NewL());

	AddTestStepL(CTestStepPlayerAllocFailNew::NewL());
	AddTestStepL(CTestStepPlayerAllocFailNewFilePlayer::NewL());
	AddTestStepL(CTestStepPlayerAllocFailPlay::NewL());
	AddTestStepL(CTestStepPlayerAllocFailNewDescPlayer::NewL());

	AddTestStepL(CTestStepPlayerAllocFailOpenFile::NewL(EFalse));
	AddTestStepL(CTestStepPlayerAllocFailOpenDesc::NewL());

	AddTestStepL(CTestStepRecorderNew::NewL());
	AddTestStepL(CTestStepRecorderOpen::NewL());
	AddTestStepL(CTestStepRecorderOpenFile::NewL(ETrue));
	AddTestStepL(CTestStepRecorderOpenFile::NewL(EFalse));
	AddTestStepL(CTestStepRecorderOpenDesc::NewL());
	AddTestStepL(CTestStepRecorderOpenUrl::NewL());
	AddTestStepL(CTestStepRecorderRecord::NewL());
	AddTestStepL(CTestStepRecorderError::NewL());

	AddTestStepL(CTestStepRecorderRecordPause::NewL(_L("MM-MMF-ACLNT-U-0210-CP")));
	//Test Step added for DEF140670
	AddTestStepL(CTestStepRecorderRecordPause::NewL(_L("MM-MMF-ACLNT-U-0211-CP")));

	AddTestStepL(CTestStepRecorderReopen::NewL());
	AddTestStepL(CTestStepRecorderRecordPlay::NewL());

	AddTestStepL(CTestStepRecorderPlayRecordPlay::NewL());
	
	AddTestStepL(CTestStepRecorderClearWindowPlay::NewL());
	AddTestStepL(CTestStepRecorderDuration::NewL());
	AddTestStepL(CTestStepRecorderType::NewL());
	AddTestStepL(CTestStepRecorderState::NewL());
	AddTestStepL(CTestStepRecorderRepeats::NewL());
	AddTestStepL(CTestStepRecorderVolRamp::NewL());
	AddTestStepL(CTestStepRecorderVol::NewL());
	AddTestStepL(CTestStepRecorderBalancePlay::NewL());
	AddTestStepL(CTestStepRecorderBalanceRecord::NewL());
	AddTestStepL(CTestStepRecorderGain::NewL());
	// some recorder tests are used for converter utility testing
	// because they tests the same API, which is implemented independently
	// both in the converter and recorder utility classes
	AddTestStepL(CTestStepRecorderWindowPlay::NewL(_L("MM-MMF-ACLNT-U-0246-HP"), EFalse));
	AddTestStepL(CTestStepRecorderWindowPlayTwice::NewL(_L("MM-MMF-ACLNT-U-0830-HP")));
	AddTestStepL(CTestStepRecorderPosPlay::NewL(_L("MM-MMF-ACLNT-U-0232-HP"), EFalse));
	AddTestStepL(CTestStepRecorderCrop::NewL(_L("MM-MMF-ACLNT-U-0240-HP"), EFalse));
	AddTestStepL(CTestStepRecorderSetPriority::NewL());
	AddTestStepL(CTestStepRecorderRecordTimeAvail::NewL());
	AddTestStepL(CTestStepRecorderSetMaxWriteLen::NewL(_L("MM-MMF-ACLNT-U-0250-HP"), EFalse));
	AddTestStepL(CTestStepRecorderGetNumMeta::NewL());
	AddTestStepL(CTestStepRecorderGetMeta::NewL());
	AddTestStepL(CTestStepRecorderAddDelMeta::NewL());
	AddTestStepL(CTestStepRecorderReplaceMeta::NewL());
	AddTestStepL(CTestStepRecorderDestinDataType::NewL(_L("MM-MMF-ACLNT-U-0264-HP"), EFalse));
	AddTestStepL(CTestStepRecorderBitRate::NewL(_L("MM-MMF-ACLNT-U-0265-HP"), EFalse));
	AddTestStepL(CTestStepRecorderSampleRate::NewL(_L("MM-MMF-ACLNT-U-0266-HP"), EFalse));
	AddTestStepL(CTestStepRecorderFormat::NewL(_L("MM-MMF-ACLNT-U-0267-HP"), EFalse));
	AddTestStepL(CTestStepRecorderChannels::NewL(_L("MM-MMF-ACLNT-U-0268-HP"), EFalse));
	AddTestStepL(CTestStepRecorderGetImplementationInfo::NewL());
	AddTestStepL(CTestStepRecorderSendCustomCommandSync::NewL());
	AddTestStepL(CTestStepRecorderSendCustomCommandAsync::NewL());
	AddTestStepL(CTestStepRecorderReloadingTest::NewL());
	//DEF083334 SWSV_604_R4B_Video: Real media icon still visible when changing to a non realVideo file
	AddTestStepL(CTestStepRecorderCheckPlayImplementationInfo::NewL());
	//DEF083334 SWSV_604_R4B_Video: Real media icon still visible when changing to a non realVideo file
	AddTestStepL(CTestStepRecorderCheckRecordImplementationInfo::NewL());

	AddTestStepL(CTestStepRecorderAllocFailNew::NewL());
	AddTestStepL(CTestStepRecorderAllocFailOpenFile::NewL(EFalse));
	AddTestStepL(CTestStepRecorderAllocFailOpenDesc::NewL());
	AddTestStepL(CTestStepRecorderAllocFailOpenUrl::NewL());
	AddTestStepL(CTestStepRecorderAllocFailOpen::NewL());
	AddTestStepL(CTestStepRecorderAllocFailPlay::NewL());
	AddTestStepL(CTestStepRecorderAllocFailRecord::NewL());
	AddTestStepL(CTestStepRecorderAllocFailCrop::NewL());

	AddTestStepL(CTestStepToneNew::NewL());
	AddTestStepL(CTestStepToneNewPriorPref::NewL());
	AddTestStepL(CTestStepTonePrepareToPlayTone::NewL());
	AddTestStepL(CTestStepTonePrepareToPlayTonePlay::NewL());
	AddTestStepL(CTestStepTonePrepareToPlayDualTonePlay::NewL());
	AddTestStepL(CTestStepTonePrepareToPlayDTMFString::NewL());
	AddTestStepL(CTestStepTonePrepareToPlayFileSeq::NewL());
	// INC087436: 3.1. DRM NRT audio files are not working properly as calender alarm tone
	AddTestStepL(CTestStepTonePrepareToPlayDRMFileSeq::NewL());
	AddTestStepL(CTestStepTonePrepareToPlayDescSeq::NewL());
	AddTestStepL(CTestStepTonePrepareToPlayFixedSeq::NewL());
	AddTestStepL(CTestStepToneCancelPlay::NewL());
	AddTestStepL(CTestStepToneCancelPrepare::NewL());
	AddTestStepL(CTestStepToneSetDTMFLengths::NewL());
	AddTestStepL(CTestStepToneSetVolume::NewL());
	AddTestStepL(CTestStepToneGetVolume::NewL());
	AddTestStepL(CTestStepToneSetPriority::NewL());
	AddTestStepL(CTestStepToneSetRepeats::NewL());
	AddTestStepL(CTestStepToneSetVolumeRamp::NewL());
	AddTestStepL(CTestStepToneGetFixedSeq::NewL());
	AddTestStepL(CTestStepToneState::NewL());

	AddTestStepL(CTestStepToneAllocFailNew::NewL());

	AddTestStepL(CTestStepConvertAllocFailNew::NewL());
	AddTestStepL(CTestStepConvertAllocFailOpen::NewL());

	AddTestStepL(CTestStepConvertNew::NewL());
	AddTestStepL(CTestStepConvertOpen::NewL());
	AddTestStepL(CTestStepConvertOpenLocFormat::NewL());
	AddTestStepL(CTestStepConvertOpenLocFormat2::NewL());
	AddTestStepL(CTestStepConvertOpenLocFormat3::NewL());

	AddTestStepL(CTestStepConvertGetImplementationInfo::NewL());
	AddTestStepL(CTestStepConvertSendCustomCommandSync::NewL());
	AddTestStepL(CTestStepConvertSendCustomCommandAsync::NewL());
	//DEF083334 SWSV_604_R4B_Video: Real media icon still visible when changing to a non realVideo file
	AddTestStepL(CTestStepConvertCheckImplementationInfo::NewL());
	
	// the following converter tests are implemented by the corresponding 
	// recorder tests since it's the same API implemented independently in both
	// recorder and converter classes.
	AddTestStepL(CTestStepRecorderDestinDataType::NewL(_L("MM-MMF-ACLNT-U-0557-HP"), ETrue));
	AddTestStepL(CTestStepRecorderBitRate::NewL(_L("MM-MMF-ACLNT-U-0558-HP"), ETrue));
	AddTestStepL(CTestStepRecorderSampleRate::NewL(_L("MM-MMF-ACLNT-U-0559-HP"), ETrue));
	AddTestStepL(CTestStepRecorderFormat::NewL(_L("MM-MMF-ACLNT-U-0560-HP"), ETrue));
	AddTestStepL(CTestStepRecorderChannels::NewL(_L("MM-MMF-ACLNT-U-0561-HP"), ETrue));
	AddTestStepL(CTestStepRecorderPosPlay::NewL(_L("MM-MMF-ACLNT-U-0562-HP"), ETrue));
	AddTestStepL(CTestStepRecorderWindowPlay::NewL(_L("MM-MMF-ACLNT-U-0564-HP"), ETrue));
	AddTestStepL(CTestStepRecorderSetMaxWriteLen::NewL(_L("MM-MMF-ACLNT-U-0565-HP"), ETrue));
	AddTestStepL(CTestStepRecorderCrop::NewL(_L("MM-MMF-ACLNT-U-0566-HP"), ETrue));

	AddTestStepL(CTestStepAudOutStream::NewL());
	AddTestStepL(CTestStepAudOutStreamNew::NewL());
	AddTestStepL(CTestStepAudOutStreamOpen::NewL());
	AddTestStepL(CTestStepAudOutStreamOpenZero::NewL());
	
	AddTestStepL(CTestStepAudOutStreamWrite::NewL());	
	AddTestStepL(CTestStepAudOutStreamSetProp::NewL());
	AddTestStepL(CTestStepAudOutStreamSetVol::NewL());
	AddTestStepL(CTestStepAudOutStreamSetPrior::NewL());
	AddTestStepL(CTestStepAudOutStreamSetBalance::NewL());
	AddTestStepL(CTestStepAudOutStreamPosition::NewL());
	AddTestStepL(CTestStepAudOutStreamStop::NewL());
	AddTestStepL(CTestStepAudOutStreamGetBytes::NewL());
	AddTestStepL(CTestStepAudOutStreamMapChannels::NewL());
	AddTestStepL(CTestStepAudOutStreamDataType::NewL());
	
	
	AddTestStepL(CTestStepAudOutStreamFormats::NewL(0));
	AddTestStepL(CTestStepAudOutStreamFormats::NewL(1));
	AddTestStepL(CTestStepAudOutStreamFormats::NewL(2));
	AddTestStepL(CTestStepAudOutStreamFormats::NewL(3));
	AddTestStepL(CTestStepAudOutStreamFormats::NewL(4));
	AddTestStepL(CTestStepAudOutStreamFormats::NewL(5));
	AddTestStepL(CTestStepAudOutStreamFormats::NewL(6));
	AddTestStepL(CTestStepAudOutStreamFormats::NewL(7));
	AddTestStepL(CTestStepAudOutStreamFormats::NewL(8));	
	AddTestStepL(CTestStepAudOutStreamFormats::NewL(9));
	AddTestStepL(CTestStepAudOutStreamFormats::NewL(10));

	// PDEF092098: DevSound fails to update policy if error occurs in StartPlayDataL
	AddTestStepL(CTestStepAudOutStreamImadPolicy::NewL(11));

	AddTestStepL(CTestStepAudOutStreamPlayError::NewL(12));
	
	AddTestStepL(CTestStepAudOutStreamGSM610Formats::NewL(0));
	
	AddTestStepL(CTestStepAudInStreamNew::NewL());
	AddTestStepL(CTestStepAudInStreamOpen::NewL());

	AddTestStepL(CTestStepAudInStreamRead::NewL(0));	
	AddTestStepL(CTestStepAudInStreamRead::NewL(1));	
	AddTestStepL(CTestStepAudInStreamRead::NewL(2));	
	AddTestStepL(CTestStepAudInStreamRead::NewL(3));
	AddTestStepL(CTestStepAudInStreamRead::NewL(4));
	AddTestStepL(CTestStepAudInStreamRead::NewL(5));
	AddTestStepL(CTestStepAudInStreamRead::NewL(6));
	
	AddTestStepL(CTestStepAudInStreamSetProp::NewL());
	AddTestStepL(CTestStepAudInStreamSetGain::NewL());
	AddTestStepL(CTestStepAudInStreamSetPrior::NewL());
	AddTestStepL(CTestStepAudInStreamSetBalance::NewL());
	AddTestStepL(CTestStepAudInStreamPosition::NewL());
	AddTestStepL(CTestStepAudInStreamStop::NewL());
	AddTestStepL(CTestStepAudInStreamGetBytes::NewL());
	AddTestStepL(CTestStepAudInStreamDataType::NewL());

	AddTestStepL(CTestStepTonePrepareToPlayIllegalDTMFString::NewL());

	// Test to check record to wav from audio input
	// with nonexistent file
	//<In Progress> 
	AddTestStepL(CTestStepRecorderRecordWav::NewL());
	
	AddTestStepL(CTestStepRecorderMaxWriteLength::NewL());

	// 
	AddTestStepL(CTestStepRecorderRecordAu::NewL());
	
	//
	AddTestStepL(CTestStepRecorderRecordRaw::NewL());

	//
	AddTestStepL(CTestStepRecorderRecordWavGsm::NewL());
	
	//
	AddTestStepL(CTestStepRecorderRecordWavGsmFail::NewL());

	// Negative test for setting invalid channels and sample rates
	AddTestStepL(CTestStepAudOutStreamSetPropNeg::NewL());

	//[ add test to test cropping a descriptor ]
	AddTestStepL( CTestStepRecCropDescriptor::NewL( _L("MM-MMF-ACLNT-U-0568-HP"), ETrue));

	AddTestStepL( CTestStepRecCropDescriptor::NewL( _L("MM-MMF-ACLNT-U-0569-HP"), EFalse));

	// DEF036477
	AddTestStepL(CTestStepRecorderSimultaneousPlay1::NewL());

	// INC0037165
	// These tests added mainly for manual verification that fix
	// for the above defect causes no regressions. Requires debugger.
	AddTestStepL(CTestStepRecorderSimultaneousPlay2::NewL());
	AddTestStepL(CTestStepRecorderSimultaneousPlay3::NewL());

	// DEF038640
	// Interrupt a low priority recorder with a higher priority recorder
	// playing the same file
	AddTestStepL(CTestStepRecorderSimultaneousPlay4::NewL());

	// DEF038653
	// Open a low priority recorder and a higher priority recorder
	// with the same file & get the duration in each case
	AddTestStepL(CTestStepRecorderSimultaneousPlay5::NewL());

	// DEF037064
	AddTestStepL(CTestStepRecorderPlayTwice::NewL());
	
	// DEF037433 Player Utility is in stopped state inbetween repeat plays (SetRepeats()) 
	AddTestStepL(CTestStepPlayerRepeatsState::NewL());
	
	// DEF038345 - MMF Audio - fast forward and rewind buttons not working correctly 
	AddTestStepL(CTestStepRecorderPlayFF::NewL(_L("MM-MMF-ACLNT-U-0822-HP")));
	

	// DEF039325 - open an MP3 file with using the test MP3 format plugin
	AddTestStepL(CTestStepRecorderOpenMp3File::NewL());

	// INC038974 - SetPlayWindow,play,pause,ClearPlayWindow,pause,stop,play
	// make sure second play starts from start of file
	AddTestStepL(CTestStepPlayerPlayAndClearWindow::NewL());
	
	// DEF040281 - Audio input streaming crashes on completion on hardware device
	AddTestStepL(CTestStepAudInStreamMemLeak::NewL());

	// INC042196 - SetPosition() on WAV files takes a long time to change position of the file (2 s 
	AddTestStepL(CTestStepRecorderSetPosition::NewL());
	
	// DEF042395 Audio recording tck failure
	AddTestStepL(CTestStepRecorderRecordStop::NewL());	
	// INC042545 - Phone freezes when playing *.wav audio with FF and Stop.
	AddTestStepL(CTestStepRecorderPlayEOF::NewL(_L("MM-MMF-ACLNT-U-0827-HP")));

	// Tests for INC044215 investigation
	AddTestStepL(CTestStepConvertOpenLocFormat4::NewL());
	
	// DEF044042 - Test playing zero length tone on CMdaAudioToneUtility 
	AddTestStepL(CTestStepTonePlayZeroLength::NewL());
	
	// PREQ 715 - Request the bit rate of the file being played.
	AddTestStepL(CTestStepPlayerBitRateWav::NewL());
	AddTestStepL(CTestStepPlayerBitRateAU::NewL());
	AddTestStepL(CTestStepRecorderBitRateWav::NewL());
	AddTestStepL(CTestStepRecorderBitRateAu::NewL());

	// INC045926- NTT-Charlie Launch-Charlie doesn't play WAV,IMAADPCM file formats.
	AddTestStepL(CTestStepPlayerPlayWav::NewL());
	
	// DEF044042 - Test playing zero length tone on CMdaAudioToneUtility 
	AddTestStepL(CTestStepTonePlayZeroLength::NewL());
	
	// PREQ 715 - Request the bit rate of the file being played.
	AddTestStepL(CTestStepPlayerBitRateWav::NewL());
	AddTestStepL(CTestStepPlayerBitRateAU::NewL());
	AddTestStepL(CTestStepRecorderBitRateWav::NewL());
	AddTestStepL(CTestStepRecorderBitRateAu::NewL());

	// INC045926- NTT-Charlie Launch-Charlie doesn't play WAV,IMAADPCM file formats.
	AddTestStepL(CTestStepPlayerPlayWav::NewL());
	// INC047838 Voice Recorder: sound is played on the newly recorded clip while mute was
	AddTestStepL(CTestStepRecorderPlayAfterRecording::NewL());
	
	AddTestStepL(CTestStepPlayerAllocFailOpenFile::NewL(ETrue));
	AddTestStepL(CTestStepRecorderAllocFailOpenFile::NewL(ETrue));
	
	//INC048184 Time bar in gallery music player does not start from zero,for the following sequence
	//SetPlayWindow,Play,Stop,fast farward,close,play and check if the position is setback to playstart
 	AddTestStepL(CTestStepPlayerPlayStopFFPlayNext::NewL());
	
	//INC048184 Time bar in gallery music player does not start from zero,for the following sequence
	//SetPlayWindow,Play,Stop,SetPosition,Stop and check if the position is setback to playstart
 	AddTestStepL(CTestStepRecorderPlayStopPosStop::NewL());
 	
 	// Test bit rate for dev CR
 	AddTestStepL(new (ELeave) RTestStepAudInStreamBitRate);
 	AddTestStepL(RTestStepAudInStreamBitRateOOM::NewL());
 	
 	// Test the actual bitrate interface itself
	AddTestStepL(CTestStepAudInStreamTestBitrate::NewL());
	
	// Test Stop procedures of InputStream
	// DEF095804 CMdaAudioInputStream loses not complete buffers from DevSound
	AddTestStepL(CTestStepAudInStreamTestReqStop::NewL(this));
	AddTestStepL(CTestStepAudInStreamTestReqStopThenStop::NewL(this));
	//PDEF121312:CMMFMdaAudioInputStream::BufferToBeEmptied() might delete an invalid pointer 
	AddTestStepL(CTestStepAudInStreamTestStopFromBufferCopied::NewL(this));

	// Add CustomInterface() tests
	CTestSuite::AddTestStepL(RTestStepAudInStreamCustIntf::NewL());
	CTestSuite::AddTestStepL(RTestStepAudOutStreamCustIntf::NewL());
	CTestSuite::AddTestStepL(RTestStepTonePlayCustIntf::NewL());

    // GT 0222 MS3.6, PREQ777 DRM Support
    // CMdaAudioPlayerUtility
    AddTestStepL(CTestMmfDRMAudioPlayFileSource::NewL());           //MM-MMF-ACLNT-U-0901-HP
    AddTestStepL(CTestMmfDRMAudioPlayFileHandleSource::NewL());     //MM-MMF-ACLNT-U-0902-HP
    AddTestStepL(CTestMmfDRMAudioPPCSFileSource::NewL());           //MM-MMF-ACLNT-U-0903-HP
    AddTestStepL(CTestMmfDRMAudioPlayAllocFileSource::NewL());      //MM-MMF-ACLNT-U-0904-HP
    AddTestStepL(CTestMmfDRMAudioPlayUniqueIdFileSource::NewL());   //MM-MMF-ACLNT-U-0905-HP
    AddTestStepL(CTestMmfDRMAudioPlayAllocFileHandleSource::NewL());//MM-MMF-ACLNT-U-0906-HP
    AddTestStepL(CTestMmfDRMAudioOpenUnknownFileSource::NewL());    //MM-MMF-ACLNT-U-0921-HP
    AddTestStepL(CTestMmfDRMAudioPlayNoRightsFileSource::NewL());   //MM-MMF-ACLNT-U-0922-HP
    AddTestStepL(CTestMmfDRMAudioPlayRightsCountFileSource::NewL());//MM-MMF-ACLNT-U-0923-HP
    AddTestStepL(CTestMmfDRMAudioPlayInvalidIdFileSource::NewL());  //MM-MMF-ACLNT-U-0924-HP
    AddTestStepL(CTestMmfDRMAudioPlayAndStopFileSource::NewL());  //MM-MMF-ACLNT-U-0907-HP
    AddTestStepL(CTestMmfDRMAudioPPCFFileSource::NewL());           //MM-MMF-ACLNT-U-0909-HP

    // CMdaAudioRecorderUtility
    AddTestStepL(CTestMmfDRMAudioRecPlayFileSource::NewL());           //MM-MMF-ACLNT-U-0911-HP
    AddTestStepL(CTestMmfDRMAudioRecPlayFileHandleSource::NewL());     //MM-MMF-ACLNT-U-0912-HP
    AddTestStepL(CTestMmfDRMAudioRecPlayStopFileSource::NewL());         //MM-MMF-ACLNT-U-0913-HP
    AddTestStepL(CTestMmfDRMAudioRecPlayAllocFileSource::NewL());      //MM-MMF-ACLNT-U-0914-HP
    AddTestStepL(CTestMmfDRMAudioRecPlayUniqueIdFileSource::NewL());   //MM-MMF-ACLNT-U-0915-HP
    AddTestStepL(CTestMmfDRMAudioRecPlayAllocFileHandleSource::NewL());//MM-MMF-ACLNT-U-0916-HP
    AddTestStepL(CTestMmfDRMAudioRecOpenUnknownFileSource::NewL());    //MM-MMF-ACLNT-U-0931-HP
    AddTestStepL(CTestMmfDRMAudioRecPlayNoRightsFileSource::NewL());   //MM-MMF-ACLNT-U-0932-HP
    AddTestStepL(CTestMmfDRMAudioRecPlayInvalidIdFileSource::NewL());  //MM-MMF-ACLNT-U-0933-HP
	// INC060658  CMdaAudioRecorderUtility::OpenDesL fails with KErrNotFound
	AddTestStepL(CTestStepRecUtilOpenDesByUid::NewL());
	// DEF061942  KERN-EXEC:15 in the AudioPolicy server
	AddTestStepL(CTestStepRecorderNotifyCheck::NewL());
	// DEF061821  - CMdaAudioRecorderUtility::PlayL() fail case when registered for notification
	AddTestStepL(CTestStepNotifyRegister::NewL());
	// DEF061822  aNotificationData in the MarncResourceAvailable() callback contains wrong data 
	AddTestStepL(CTestStepResNotificationData::NewL());		//MM-MMF-ACLNT-U-0953-HP
	// DEF068366  Using record utility, callback of PlaybackBalance APIs fail.
	AddTestStepL(CTestStepRecorderSetBalRecordGetBal::NewL());
	
	AddTestStepL(CTestStepPlaybackDurationInformation::NewL()); // MM-MMF-ACLNT-U-0960-HP
	AddTestStepL(CTestStepRecorderDurationInformation::NewL()); // MM-MMF-ACLNT-U-0961-HP
	
	AddTestStepL(CTestStepPlayerSharedHeapMultiFilePlayer::NewL( _L("MM-MMF-ACLNT-U-2000-HP"), EFalse ));
	AddTestStepL(CTestStepPlayerSharedHeapMultiFilePlayer::NewL( _L("MM-MMF-ACLNT-U-2001-HP"), ETrue ));

	AddTestStepL(CTestStepPlayerSharedHeapRepeatMultiFilePlayer::NewL( _L("MM-MMF-ACLNT-U-2002-HP"), EFalse ));
	AddTestStepL(CTestStepPlayerSharedHeapRepeatMultiFilePlayer::NewL( _L("MM-MMF-ACLNT-U-2003-HP"), ETrue ));
	
	AddTestStepL(CTestStepRecorderSharedHeapMultiFilePlayer::NewL( _L("MM-MMF-ACLNT-U-2006-HP"), EFalse ));
	AddTestStepL(CTestStepConverterSharedHeapMultiFile::NewL( _L("MM-MMF-ACLNT-U-2007-HP"), EFalse ));

	AddTestStepL(CTestStepRecorderOpenviaUriTag::NewL());
	AddTestStepL(CTestStepRecorderOpenTryNextCtrl::NewL());
	AddTestStepL(CTestStepPlayerOpenviaUriTag::NewL());
	AddTestStepL(CTestStepPlayerOpenTryNextCtrl::NewL());
	AddTestStepL(CTestStepRecorderLowMaxVol::NewL());      // MM-MMF-ACLNT-U-0965-HP
	AddTestStepL(CTestStepPlayerLowMaxVol::NewL());      // MM-MMF-ACLNT-U-0967-HP
	AddTestStepL(CTestStepPlayerRepeatsPause::NewL());      // MM-MMF-ACLNT-U-0968-HP
	AddTestStepL(CTestMmfAclntRecFileInvDesForm::NewL()); // MM-MMF-ACLNT-U-0969-HP
	AddTestStepL(RAudioPlayCompleteTest::NewL(_L("MM-MMF-ACLNT-U-0065-HP"), _L("c:\\mm\\mmf\\testfiles\\aclnt\\cdec_ff_wav_fr.wav")));
	AddTestStepL(CTestStepRecorderVolError::NewL());
	
	/*
	// Excluding these tests until _DEBUG panic in RMMFControllerProxy::Close() has been
	// removed or modified.
	// Otherwise these test steps fail when they shouldn't
	AddTestStepL(CTestStepPlayerSharedHeapPanicMultiFilePlayer::NewL( _L("MM-MMF-ACLNT-U-2004-HP"), EFalse ));
	AddTestStepL(CTestStepPlayerSharedHeapPanicMultiFilePlayer::NewL( _L("MM-MMF-ACLNT-U-2005-HP"), ETrue ));
	*/
	AddTestStepL(RTestStepConvertAudio::NewL(_L("MM-MMF-ACLNT-U-0574-HP"),
											 _L("c:\\mm\\mmf\\testfiles\\aclnt\\IMAADPCM4Bit8KhzMono512Block.wav"),
											 _L("c:\\mm\\mmf\\testfiles\\aclnt\\PCM16FromImaadpcmwith512block.wav"),
											 _L("c:\\mm\\mmf\\testfiles\\aclnt\\Refimaadpcmwith512block2pcm16.wav")));
	
	// INC107513 Startup tone is played before tone initialization is completed.
	AddTestStepL(RTestStepTonePlayBeforeInitialized::NewL());
	AddTestStepL(CTsuClientUtilityControllerLoading::NewL(_L("MM-MMF-ACLNT-U-0066-HP"),
				_L("c:\\mm\\testformat.tsa")));
	AddTestStepL(CAsyncTestStepCtrlSetRepeatsCustomCommands::NewL(_L("MM-MMF-ACLNT-U-0068-HP"),_L("SectionOne"), _L("AudioPlayFName2")));
	AddTestStepL(CTestStepAudOutStreamConcurrentOpen::NewL());
	
	//DEF121928 - Concurrently calling RecordL by audiorecorder in 3 threads make server  panics 
	AddTestStepL(RAudioConcurrentRecTest::NewL(_L("MM-MMF-ACLNT-U-0972-CP"), EMdaPriorityMin, EMdaPriorityNormal, EMdaPriorityMax));
	AddTestStepL(RAudioConcurrentRecTest::NewL(_L("MM-MMF-ACLNT-U-0973-CP"), EMdaPriorityMax, EMdaPriorityNormal, EMdaPriorityMin));
	AddTestStepL(RAudioConcurrentRecTest::NewL(_L("MM-MMF-ACLNT-U-0974-CP"), EMdaPriorityNormal, EMdaPriorityMax, EMdaPriorityMin));
	
	AddTestStepL(RTestStepTonePlayWhenBusy::NewL());
	
	//DEF120897:CMdaAudioOutputStream depending on devsound underflow events, against new behav 
	//KeepOpenAtEnd Mode and Auto RequestStop
	AddTestStepL(RAudioOPStreamRequestStop::NewL(_L("MM-MMF-ACLNT-U-0975-CP"), ETrue, ETrue, EFalse));
	//KeepOpenAtEnd Mode and Explicit RequestStop after sometime
	AddTestStepL(RAudioOPStreamRequestStop::NewL(_L("MM-MMF-ACLNT-U-0976-CP"), ETrue, EFalse, EFalse));
	//KeepOpenAtEnd Mode Negative test: Try calling WriteL after calling RequestStop
	AddTestStepL(RAudioOPStreamRequestStop::NewL(_L("MM-MMF-ACLNT-U-0977-CP"), ETrue, ETrue, ETrue));
	//Non KeepOpenAtEnd Mode
	AddTestStepL(RAudioOPStreamRequestStop::NewL(_L("MM-MMF-ACLNT-U-0978-CP"), EFalse, EFalse, EFalse));
	//Restart the play in KeepOpenAtEnd mode
	AddTestStepL(RAudioOPStreamRequestStop::NewL(_L("MM-MMF-ACLNT-U-0979-CP"), ETrue, ETrue, EFalse, ETrue));
	//Restart the play in Non KeepOpenAtEnd mode
	AddTestStepL(RAudioOPStreamRequestStop::NewL(_L("MM-MMF-ACLNT-U-0980-CP"), EFalse, EFalse, EFalse, ETrue));
	
	//DEF125377 - Test negative frequency and duration values passed to CMdaAudioToneUtility
	AddTestStepL(CTestStepTonePlayDualToneInvalidFreq::NewL(_L("MM-MMF-ACLNT-U-0981-CP")));
	AddTestStepL(CTestStepTonePlayDualToneInvalidFreq::NewL(_L("MM-MMF-ACLNT-U-0982-CP")));
	AddTestStepL(CTestStepTonePlayDualToneInvalidFreq::NewL(_L("MM-MMF-ACLNT-U-0983-CP")));

	// PDEF131534 SPPR_PERF: Music player crashes when opening corrupted wav file
	AddTestStepL(RAudioOpenCorrupt::NewL(_L("MM-MMF-ACLNT-U-0984-HP"), _L("c:\\mm\\mmf\\testfiles\\aclnt\\CorruptSampleRate.wav")));
	AddTestStepL(CTestStepAudInStreamDestroy::NewL(this));
	
	//DEF127630:Fix for INC108007 appears to close
	AddTestStepL(RAudioPlayPanicTest::NewL(_L("MM-MMF-ACLNT-U-0986-CP"), _L("c:\\mm\\mmf\\testfiles\\aclnt\\playtest.pnc")));
	AddTestStepL(RAudioRecPanicTest::NewL(_L("MM-MMF-ACLNT-U-0987-CP"), _L("c:\\mm\\mmf\\testfiles\\aclnt\\rectest.pnc")));
	AddTestStepL(RTestStepConvertPanic::NewL(_L("MM-MMF-ACLNT-U-0988-CP"), _L("c:\\mm\\mmf\\testfiles\\aclnt\\playtest.pnc"),
											 _L("c:\\mm\\mmf\\testfiles\\aclnt\\convert.pnc"), _L("")));
	}

/**
*
* InitSystemPath
* This sets up the default path
* 
*/
void CTestSuiteUnitMMFAudClient::InitSystemPath()
	{
	_LIT(KAudioStremOutSourcePathOnC,"c:\\mm\\mmf\\testfiles\\aclnt\\");

	RFs fs;
	TUint att;
	TInt ret = fs.Connect();
	__ASSERT_ALWAYS(ret == KErrNone,User::Panic(_L("Sys path not setup"),ret));

	if (fs.Att(KAudioStremOutSourcePathOnC,att) == KErrNone)
		iDefaultPath = KAudioStremOutSourcePathOnC;
	else
		User::Panic(_L("Test files not found"),KErrNotFound);

	fs.Close();
	}


/**
*
* get default path
* 
* @xxx
*
*/
const TDesC& CTestSuiteUnitMMFAudClient::DefaultPath() const
	{
	return iDefaultPath;
	}

// -------------------------
