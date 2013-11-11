
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
// This main DLL entry point for the TS_WP5.dll
// 
//


// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>
#include <mmfcontrollerimplementationuids.hrh>

#include "TSI_MMFACLNT.h"
#include "TS_MMFACLNTSuite.h"
#include "TestConverter.h"
#include "TestPlayerUtils.h"
#include "TestRecorder.h"
#include "TestTone.h"
#include "TestOldCodec.h"
#include "TestDRM.h"
#include "TestPlayerCaps.h"

#include "TestPlayerCaps0012.h"
#include "TestPlayerCaps0014.h"

#include "OpenFileByHandle7803.h"
#include "OpenFileByHandle7804.h"
#include "OpenFileByHandle7805.h"
#include "OpenFileByHandle7806.h"
#include "OpenFileByHandle7901.h"
#include "OpenFileByHandle7902.h"
#include "OpenFileByHandle7904.h"
#include "OpenFileByHandle7905.h"
#include "OpenFileByHandle7906.h"
#include "OpenFileByHandle7908.h"
#include "OpenFileByHandle8801.h"
#include "OpenFileByHandle1211.h"
#include "OpenFileByHandle_PlayTone.h"

#include "TestPlayerFileSource.h"
#include "TestRecorderFileSource.h"
#include "TestAudioPlayerDRM.h"
#include "TestAudioRecorderDRM.h"

#include "TestAudioClientUtilitiesARN.h"
#include "testoggRecorder.h"
#include "tonetruepause.h"
#include "testoutputstreamtruepause.h"
#include "playtruepause.h"

/**
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuite* NewTestSuiteL()
    {
	CTestMmfAclntSuite* result = new (ELeave) CTestMmfAclntSuite;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

/**
 * destructor
 */
CTestMmfAclntSuite::~CTestMmfAclntSuite()
	{
	}

/**
 * make a version string available for test system
 * @return version string
 */
TPtrC CTestMmfAclntSuite::GetVersion( void )
	{
	_LIT(KTxtVersion,"1.0");
	return KTxtVersion();
	}

/**
 * Add a test step into the suite
 *
 * @param test step pointer to add
 */
void CTestMmfAclntSuite::AddTestStepL( CTestMmfAclntStep* aPtrTestStep )
	{
	// test steps contain a pointer back to the suite which owns them
	aPtrTestStep->SetTestSuite(this);
	// add the step using the base class method
	CTestSuite::AddTestStepL(aPtrTestStep);
	}

void CTestMmfAclntSuite::AddTestStepL( RTestStep* aTestStep )
	{
	// add the step using the base class method
	CTestSuite::AddTestStepL(aTestStep);
	}

/**
 * Constructor for test suite
 * this creates all the test steps and 
 * stores them inside CTestMMFACLNTSuite 
 */
void CTestMmfAclntSuite::InitialiseL( void )
	{
	// store the name of this test suite 
	iSuiteName = _L("MMF_ACLNT");

	// varibles used within tests
	TTimeIntervalMicroSeconds position(0);
	TTimeIntervalMicroSeconds nonZeroPosition(3000000);// 3 sec from start of clip
	TTimeIntervalMicroSeconds zeroSilence(0);
	TTimeIntervalMicroSeconds silence(1000000);// silence of one sec
	TInt64 max = (TInt64)(KMaxTInt)+1;
	TTimeIntervalMicroSeconds maxSilence(max);// silence more than KMaxTInt
	TInt lessRepeatCount = 2;
	TInt moreRepeatCount = lessRepeatCount+4;
	// 
	// POSITIVE TESTS
	//

	// CAudioPlayerUtil tests
	// Open and play a number of audio clip files using different codecs
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-0001-CP"),_L("SectionOne"),_L("playerAudioFile"),EFalse,EFalse) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2001-HP"),_L("SectionOne"),_L("PCM8"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2002-HP"),_L("SectionOne"),_L("ALAW "),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2003-HP"),_L("SectionOne"),_L("PCM16"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2004-HP"),_L("SectionOne"),_L("PCMU16BE"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2005-HP"),_L("SectionOne"),_L("PCMU8"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2006-HP"),_L("SectionOne"),_L("PCMU16"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2007-HP"),_L("SectionOne"),_L("MULAW"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2008-HP"),_L("SectionOne"),_L("IMAD"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2009-HP"),_L("SectionOne"),_L("IMAS"),ETrue,KErrNotSupported) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2010-HP"),_L("SectionOne"),_L("GSM610"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2011-LP"),_L("SectionOne"),_L("PCMU8_22254Hz_ALF"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2012-HP"),_L("SectionOne"),_L("Pcm16Mono44khz"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2013-HP"),_L("SectionOne"),_L("Pcm16Mono48khz"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2014-LP"),_L("SectionOne"),_L("100khzSRate1KhzTone"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2015-LP"),_L("SectionOne"),_L("7.33khzSRate1KhzTone"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2016-HP"),_L("SectionOne"),_L("PCM8-6144bytes"),ETrue,EFalse) ); //INC23016

	AddTestStepL( CTestMmfAclntSEQNFile::NewL(_L("MM-MMF-ACLNT-I-2017-HP"),_L("SectionOne"),_L("Sequence1"),ETrue,ETrue) );
	AddTestStepL( CTestMmfAclntSEQNFile::NewL(_L("MM-MMF-ACLNT-I-2018-HP"),_L("SectionOne"),_L("Sequence2"),ETrue,EFalse) );

	// INC036267 Play a file that has an incorrect file extension
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2019-HP"),_L("SectionOne"),_L("PCM16WrongExtension1"),ETrue,KErrNone) ); //PCM16

	// INC041837 - Need 2 additional enums for 12 & 24 KHz sample rate 
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2020-CP"),_L("SectionOne"),_L("Pcm16Mono12khz"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-2021-CP"),_L("SectionOne"),_L("Pcm16Mono24khz"),ETrue,EFalse) );

	// Setup and play a number of audio clip decriptors that use different codecs
	AddTestStepL( CTestMmfAclntDesc::NewL(_L("MM-MMF-ACLNT-I-0002-CP"),_L("SectionTwo"),_L("playerAudioFile"),EFalse) );
	AddTestStepL( CTestMmfAclntDesc::NewL(_L("MM-MMF-ACLNT-I-3001-HP"),_L("SectionOne"),_L("PCM8"),ETrue) );
	AddTestStepL( CTestMmfAclntDesc::NewL(_L("MM-MMF-ACLNT-I-3002-HP"),_L("SectionOne"),_L("ALAW"),ETrue) );
	AddTestStepL( CTestMmfAclntDesc::NewL(_L("MM-MMF-ACLNT-I-3003-HP"),_L("SectionOne"),_L("PCM16"),ETrue) );
	AddTestStepL( CTestMmfAclntDesc::NewL(_L("MM-MMF-ACLNT-I-3004-HP"),_L("SectionOne"),_L("PCMU16BE"),ETrue) );
	AddTestStepL( CTestMmfAclntDesc::NewL(_L("MM-MMF-ACLNT-I-3005-HP"),_L("SectionOne"),_L("PCMU8"),ETrue) );
	AddTestStepL( CTestMmfAclntDesc::NewL(_L("MM-MMF-ACLNT-I-3006-HP"),_L("SectionOne"),_L("PCMU16"),ETrue) );
	AddTestStepL( CTestMmfAclntDesc::NewL(_L("MM-MMF-ACLNT-I-3007-HP"),_L("SectionOne"),_L("MULAW"),ETrue) );
	AddTestStepL( CTestMmfAclntDesc::NewL(_L("MM-MMF-ACLNT-I-3008-HP"),_L("SectionOne"),_L("IMAD"),ETrue) );
	AddTestStepL( CTestMmfAclntDesc::NewL(_L("MM-MMF-ACLNT-I-3009-HP"),_L("SectionOne"),_L("IMAS"),ETrue, KErrNotSupported) );
	AddTestStepL( CTestMmfAclntDesc::NewL(_L("MM-MMF-ACLNT-I-3010-HP"),_L("SectionOne"),_L("GSM610"),ETrue) );

	// ** new tests
	AddTestStepL( CTestMmfAclntDescReadOnly::NewL(_L("MM-MMF-ACLNT-I-0202-LP"),_L("SectionTwo"),_L("playerAudioFile"),EFalse) );
	AddTestStepL( CTestMmfAclntDescOpen::NewL(_L("MM-MMF-ACLNT-I-0302-LP"),_L("SectionTwo"),_L("playerAudioFile"),EFalse) );

	// Using URLs open an play a number of Audio files that use different codecs
	AddTestStepL( CTestMmfAclntUrl::NewL(_L("MM-MMF-ACLNT-I-4001-HP") ,_L(""),_L(""),EFalse) ) ;
	AddTestStepL( CTestMmfAclntUrl::NewL(_L("MM-MMF-ACLNT-I-4201-LP") ,_L(""),_L(""),ETrue) ) ;

	// Test API calls for CPlayerUtils
	AddTestStepL( CTestMmfAclntAudioPlayStopStart::NewL(_L("MM-MMF-ACLNT-I-0013-CP"),_L("SectionOne") ) );
	AddTestStepL( CTestMmfAclntAudioPlayPauseStart::NewL(_L("MM-MMF-ACLNT-I-0213-LP"),_L("SectionOne") ) );
	AddTestStepL( CTestMmfAclntAudioPlayPausePlayTest::NewL(_L("MM-MMF-ACLNT-I-0214-LP"),_L("SectionOne") ) );
	AddTestStepL( CTestMmfAclntAudioPlayVolume::NewL(_L("MM-MMF-ACLNT-I-0014-CP"),_L("SectionOne") ) );
	AddTestStepL( CTestMmfAclntAudioRepeat::NewL(_L("MM-MMF-ACLNT-I-0015-LP"),_L("SectionOne") ) );
	AddTestStepL( CTestMmfAclntAudioBalance::NewL(_L("MM-MMF-ACLNT-I-0016-LP"),_L("SectionOne"),10, KErrNone) );
	AddTestStepL( CTestMmfAclntAudioDuration::NewL(_L("MM-MMF-ACLNT-I-0018-HP"),_L("SectionOne") ) );
	AddTestStepL( CTestMmfAclntAudioPosition::NewL(_L("MM-MMF-ACLNT-I-0022-HP"),_L("SectionOne"), position) );
	AddTestStepL( CTestMmfAclntAudioPlayWindow::NewL(_L("MM-MMF-ACLNT-I-0023-LP"),_L("SectionOne") ) );
	AddTestStepL( CTestMmfAclntAudioType::NewL(_L("MM-MMF-ACLNT-I-0024-HP"),_L("SectionOne") ) );
	AddTestStepL( CTestMmfAclntAudioPlayVolume::NewL(_L("MM-MMF-ACLNT-I-0030-HP"),_L("SectionOne") ) );
	AddTestStepL( CTestMmfAclntAudioRamp::NewL(_L("MM-MMF-ACLNT-I-0031-LP"),_L("SectionOne") ) );
	AddTestStepL( CTestMmfAclntAudioMeta::NewL(_L("MM-MMF-ACLNT-I-0032-LP"),_L("SectionOne") ) );
	AddTestStepL( CTestMmfAclntAudioCloseOpen::NewL(_L("MM-MMF-ACLNT-I-0033-LP"),_L("SectionOne") ) );
	AddTestStepL( CTestMmfAclntAudioPlayWindow::NewL(_L("MM-MMF-ACLNT-I-0034-LP"),_L("SectionOne") ) );
	AddTestStepL( CTestMmfAclntAudioQuery::NewL(_L("MM-MMF-ACLNT-I-0035-LP"),_L("SectionOne") ) );


	//Sierra REQ2870 Audio Bitrate.
	AddTestStepL( CTestMmfAclntAudioBitRate::NewL(_L("MM-MMF-ACLNT-I-0036-HP"),_L("WAV128"),_L("bitrate128") ) );
	AddTestStepL( CTestMmfAclntAudioBitRate::NewL(_L("MM-MMF-ACLNT-I-0037-HP"),_L("WAV256"),_L("bitrate256")) );
	AddTestStepL( CTestMmfAclntAudioBitRate::NewL(_L("MM-MMF-ACLNT-I-0038-HP"),_L("AU128"),_L("bitrate128")) );
	AddTestStepL( CTestMmfAclntAudioBitRate::NewL(_L("MM-MMF-ACLNT-I-0039-HP"),_L("AU256"),_L("bitrate256")) );

	// Test priority settings
	AddTestStepL( CTestMmfAclntPriority::NewL( _L("MM-MMF-ACLNT-I-0019-HP"), _L("SectionOne"), EMdaPriorityMin) );
	AddTestStepL( CTestMmfAclntPriority::NewL( _L("MM-MMF-ACLNT-I-0020-HP"), _L("SectionOne"), EMdaPriorityMax) );
	AddTestStepL( CTestMmfAclntPriority::NewL( _L("MM-MMF-ACLNT-I-0021-HP"), _L("SectionOne"), EMdaPriorityNormal) );
	AddTestStepL( CTestMmfAclntPriority::NewL( _L("MM-MMF-ACLNT-I-0219-LP"), _L("SectionOne") ) );


	// Tests for CRecordUtil 


	// Record to a file
	AddTestStepL( CTestMmfAclntRecFile::NewL( _L("MM-MMF-ACLNT-I-5001-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EPcm8,ETrue) );
	AddTestStepL( CTestMmfAclntRecFile::NewL( _L("MM-MMF-ACLNT-I-5002-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EAlawWav,ETrue) );
	AddTestStepL( CTestMmfAclntRecFile::NewL( _L("MM-MMF-ACLNT-I-5003-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EPcm16Wav,ETrue) );
	AddTestStepL( CTestMmfAclntRecFile::NewL( _L("MM-MMF-ACLNT-I-5004-HP"),_L("SectionThree"),_L("outputAudioFileRaw"),CTestMmfAclntCodecTest::EPcmU16,ETrue) );
	AddTestStepL( CTestMmfAclntRecFile::NewL( _L("MM-MMF-ACLNT-I-5005-HP"),_L("SectionThree"),_L("outputAudioFileRaw"),CTestMmfAclntCodecTest::EPcmU8,ETrue) );
	AddTestStepL( CTestMmfAclntRecFile::NewL( _L("MM-MMF-ACLNT-I-5006-HP"),_L("SectionThree"),_L("outputAudioFileRaw"),CTestMmfAclntCodecTest::EMulawRaw,ETrue) );
	AddTestStepL( CTestMmfAclntRecFile::NewL( _L("MM-MMF-ACLNT-I-5007-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EImaAdpcmWav,ETrue) );
	AddTestStepL( CTestMmfAclntRecFile::NewL( _L("MM-MMF-ACLNT-I-5008-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EImasPcmWav,ETrue) );
	AddTestStepL( CTestMmfAclntRecFile::NewL( _L("MM-MMF-ACLNT-I-5009-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EGsmWav,ETrue) );


	TMdaWavClipFormat MdaWavFormat;
	TMmfAclntWavEncodeClipFormat MmfAclntWavEncodeClipFormat;
	TMmfAclntWavDecodeClipFormat MmfAclntWavDecodeClipFormat;

	AddTestStepL( CTestMmfAclntRecFileForceFormat::NewL(_L("MM-MMF-ACLNT-I-5010-HP"),_L("SectionThree"),_L("outputAudioFile"), MdaWavFormat));
	AddTestStepL( CTestMmfAclntRecFileForceFormat::NewL(_L("MM-MMF-ACLNT-I-5011-HP"),_L("SectionThree"),_L("outputAudioFile"), MmfAclntWavEncodeClipFormat));
	AddTestStepL( CTestMmfAclntRecFileForceFormat::NewL(_L("MM-MMF-ACLNT-I-5012-HP"),_L("SectionThree"),_L("outputAudioFile"), MmfAclntWavDecodeClipFormat));

	// DEF036641 record to an existing IMAD .wav file
	AddTestStepL( CTestMmfAclntRecFileAppend::NewL(_L("MM-MMF-ACLNT-I-5013-HP"),_L("SectionThree"),_L("outputAudioFile"), CTestMmfAclntRecFileAppend::EWavFormat, CTestMmfAclntRecFileAppend::EAdpcmCodec));

	// DEF036640 record to an existing .wav file with a different sample rate
	AddTestStepL( CTestMmfAclntRecFileAppend::NewL(_L("MM-MMF-ACLNT-I-5014-HP"),_L("SectionThree"),_L("outputAudioFile"), CTestMmfAclntRecFileAppend::EWavFormat, CTestMmfAclntRecFileAppend::EPcmCodec, ETrue));

	// INC036267 Play a file that has an incorrect file extension
	AddTestStepL( CTestMmfAclntRecFile::NewL(_L("MM-MMF-ACLNT-I-5015-HP"),_L("SectionOne"),_L("PCM16WrongExtension2"),CTestMmfAclntCodecTest::EPcm16Wav,EFalse) ); //PCM16

	// INC038043 Play a file with a different sample rate/number of channels
	AddTestStepL( CTestMmfAclntRecFilePlay::NewL(_L("MM-MMF-ACLNT-I-5016-HP"),_L("SectionThree"),_L("outputAudioFile"), CTestMmfAclntRecFilePlay::EWavFormat, CTestMmfAclntRecFilePlay::EPcmCodec));

	// DEF039308 - No integration test to test playback of recorded raw file
	AddTestStepL( CTestMmfAclntRecRawFilePlay::NewL(_L("MM-MMF-ACLNT-I-5017-HP"),_L("SectionThree"),_L("outputAudioFileRaw"), CTestMmfAclntRecRawFilePlay::ERawFormat, CTestMmfAclntRecRawFilePlay::EAlawCodec));

	//Tests where no playback controller is available
	AddTestStepL( CTestMmfAclntRecNoPlaybackForceFormat::NewL(_L("MM-MMF-ACLNT-I-5100-HP"),_L("SectionThree"),_L("outputAudioFile"), MmfAclntWavEncodeClipFormat));
	AddTestStepL( CTestMmfAclntRecNoPlaybackForceFormatDesc::NewL(_L("MM-MMF-ACLNT-I-5101-HP"),_L("SectionThree"),_L("outputAudioFile"), MmfAclntWavEncodeClipFormat));
	AddTestStepL( CTestMmfAclntRecNoPlaybackFile::NewL(_L("MM-MMF-ACLNT-I-5102-HP"),_L("SectionThree"),_L("outputAudioFile")));
	AddTestStepL( CTestMmfAclntRecNoPlaybackDesc::NewL(_L("MM-MMF-ACLNT-I-5103-HP"),_L("SectionThree"),_L("outputAudioFile")));

	TUid audioController ={KMmfUidControllerAudio};

	AddTestStepL( CTestMmfAclntRecNoPlaybackFileForceController::NewL(_L("MM-MMF-ACLNT-I-5104-HP"),_L("SectionThree"),_L("outputAudioFile"), audioController));
	AddTestStepL( CTestMmfAclntRecNoPlaybackDescForceController::NewL(_L("MM-MMF-ACLNT-I-5105-HP"),_L("SectionThree"),_L("outputAudioFile"), audioController));

	//Tests where no record controller is available
	AddTestStepL( CTestMmfAclntRecNoRecordForceFormat::NewL(_L("MM-MMF-ACLNT-I-5110-HP"),_L("SectionOne"),_L("playerAudioFile"), MmfAclntWavDecodeClipFormat));
	AddTestStepL( CTestMmfAclntRecNoRecordForceFormatDesc::NewL(_L("MM-MMF-ACLNT-I-5111-HP"),_L("SectionOne"),_L("playerAudioFile"), MmfAclntWavDecodeClipFormat));
	AddTestStepL( CTestMmfAclntRecNoRecordFile::NewL(_L("MM-MMF-ACLNT-I-5112-HP"),_L("SectionOne"),_L("playerAudioFile")));
	AddTestStepL( CTestMmfAclntRecNoRecordDesc::NewL(_L("MM-MMF-ACLNT-I-5113-HP"),_L("SectionOne"),_L("playerAudioFile")));
	AddTestStepL( CTestMmfAclntRecNoRecordFileForceController::NewL(_L("MM-MMF-ACLNT-I-5114-HP"),_L("SectionOne"),_L("playerAudioFile"), audioController));
	AddTestStepL( CTestMmfAclntRecNoRecordDescForceController::NewL(_L("MM-MMF-ACLNT-I-5115-HP"),_L("SectionOne"),_L("playerAudioFile"), audioController));


	// Record to a descriptor
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6001-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EPcm8,ETrue) );
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6002-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EAlawWav,ETrue) );
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6003-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EPcm16Wav,ETrue) );
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6004-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EPcmU16,ETrue) );
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6005-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EPcmU8,ETrue) );
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6006-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EMulawRaw,ETrue) );
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6007-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EImaAdpcmWav,ETrue) );
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6008-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EImasPcmWav,ETrue) );
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6009-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EGsmWav,ETrue) );


	// Record to a URL ?
	AddTestStepL( CTestMmfAclntRecUrl::NewL( _L("MM-MMF-ACLNT-I-0054-LP"), _L("") , _L("")) );
	AddTestStepL( CTestMmfAclntRecGain::NewL(_L("MM-MMF-ACLNT-I-0017-LP"),_L("SectionThree"),0, EFalse) );
	AddTestStepL( CTestMmfAclntRecStateTran::NewL( _L("MM-MMF-ACLNT-I-0025-HP"),_L("SectionSix"), EFalse) );
	AddTestStepL( CTestMmfAclntCrop::NewL(_L("MM-MMF-ACLNT-I-0027-LP"),_L("SectionOne"),ETrue, EFalse) );
	AddTestStepL( CTestMmfAclntCrop::NewL(_L("MM-MMF-ACLNT-I-0028-LP"),_L("SectionOne"),EFalse, EFalse) );
	AddTestStepL( CTestMmfAclntRecCloseOpen::NewL(_L("MM-MMF-ACLNT-I-0050-HP"),_L("SectionOne"), EFalse) );
	position = 0;
	AddTestStepL( CTestMmfAclntRecPosition::NewL( _L("MM-MMF-ACLNT-I-0055-LP"),_L("SectionOne"),position, EFalse) );
	AddTestStepL( CTestMmfAclntRecDuration::NewL( _L("MM-MMF-ACLNT-I-0056-LP"),_L("SectionOne"), EFalse) );
	AddTestStepL( CTestMmfAclntRecSetMeta::NewL( _L("MM-MMF-ACLNT-I-0057-LP"),_L("SectionOne"), EFalse) );
	AddTestStepL( new(ELeave) CTestMmfAclntRecPriority(_L("MM-MMF-ACLNT-I-0058-LP"),_L("SectionOne") ) );
	AddTestStepL( CTestMmfAclntRecBalance::NewL( _L("MM-MMF-ACLNT-I-0059-LP"),_L("SectionOne"),20, EFalse) );
	AddTestStepL( CTestMmfAclntRecLength::NewL( _L("MM-MMF-ACLNT-I-0060-LP"),_L("SectionOne"), EFalse) );
	AddTestStepL( CTestMmfAclntRecFormats::NewL( _L("MM-MMF-ACLNT-I-0061-LP"),_L("SectionOne"), EFalse) );

	// DEF039893 call GetBalance prior to SetBalance being set.
	AddTestStepL( CTestMmfAclntRecordGetBalance::NewL(_L("MM-MMF-ACLNT-I-6050-LP")));

	// Sierra REQ2870 Audio Bitrate.
	AddTestStepL( CTestMmfAclntRecSrcAuBitrate::NewL(_L("MM-MMF-ACLNT-I-0062-LP"), _L("SectionNine"), _L("PCM8bitMonoWav"), _L("bitrate64")));
	AddTestStepL( CTestMmfAclntRecSrcAuBitrate::NewL(_L("MM-MMF-ACLNT-I-0063-LP"), _L("SectionNine"), _L("PCM8bitStereoWav"), _L("bitrate128")));
	AddTestStepL( CTestMmfAclntRecSrcAuBitrate::NewL(_L("MM-MMF-ACLNT-I-0064-LP"), _L("SectionNine"), _L("PCM8bitMonoAu"), _L("bitrate64")));
	AddTestStepL( CTestMmfAclntRecSrcAuBitrate::NewL(_L("MM-MMF-ACLNT-I-0065-LP"), _L("SectionNine"), _L("PCM8bitStereoAu"), _L("bitrate128")));
	AddTestStepL( CTestMmfAclntRecBitrateRecordedFile::NewL(_L("MM-MMF-ACLNT-I-0066-LP"), _L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EPcm8,ETrue));

	//CR1566 Record-Stop-Record
	AddTestStepL( CTestMmfAclntRecStopRec::NewL( _L("MM-MMF-ACLNT-I-5018-HP"),_L("SectionSix"), EFalse) );

	// *********************** Tests for CConversionUtils ***************************


	//vm AddTestStepL( CTestMmfAclntDoConv::NewL(  _L("MM-MMF-ACLNT-I-7001-HP"), _L("SectionFour") , _L("auAudioFile") , _L("SectionFour") , _L("wavAudioFile")) ); 
	AddTestStepL( CTestMmfAclntDoConv::NewL(  _L("MM-MMF-ACLNT-I-7001-HP"), _L("SectionOne") , _L("Pcm16Mono44khz") , _L("SectionFour") , _L("PCM8_out_au"), TMdaPcmWavCodec::E8BitPcm, CTestMmfAclntDoConv::MdaFormatSpec)); 
	
	//inc21946
	AddTestStepL( CTestMmfAclntDoConv::NewL(  _L("MM-MMF-ACLNT-I-7009-HP"), _L("SectionOne") , _L("Pcm16Mono44khz") , _L("SectionFour") , _L("PCM8_out_au"), TMdaPcmWavCodec::E8BitPcm, CTestMmfAclntDoConv::MmfFormatSpec)); 

	AddTestStepL( CTestMmfAclntConCloseOpen::NewL( _L("MM-MMF-ACLNT-I-0102-HP")) );
	AddTestStepL( CTestMmfAclntConWindow::NewL( _L("MM-MMF-ACLNT-I-0103-HP")) );
	AddTestStepL( CTestMmfAclntConDuration::NewL( _L("MM-MMF-ACLNT-I-0104-LP")) );
	position = 1114536;
	AddTestStepL( CTestMmfAclntConPosition::NewL( _L("MM-MMF-ACLNT-I-0105-LP"),position) );
	AddTestStepL( CTestMmfAclntConConfig::NewL( _L("MM-MMF-ACLNT-I-0106-HP"), CTestMmfAclntCodecTest::EPcm16Wav) );
	AddTestStepL( CTestMmfAclntConQuery::NewL( _L("MM-MMF-ACLNT-I-0107-LP")) );

	// Convert to a descriptor
	AddTestStepL( CTestMmfAclntConvertDes::NewL( _L("MM-MMF-ACLNT-I-7003-LP"),_L("SectionOne"),_L("PCM16"), CTestMmfAclntCodecTest::EPcm8));
	AddTestStepL( CTestMmfAclntConvertDes::NewL( _L("MM-MMF-ACLNT-I-7004-LP"),_L("SectionOne"),_L("PCM16"), CTestMmfAclntCodecTest::EAlawWav));
	AddTestStepL( CTestMmfAclntConvertDes::NewL( _L("MM-MMF-ACLNT-I-7005-LP"),_L("SectionOne"),_L("PCM16"), CTestMmfAclntCodecTest::EPcmU16));
	AddTestStepL( CTestMmfAclntConvertDes::NewL( _L("MM-MMF-ACLNT-I-7006-LP"),_L("SectionOne"),_L("PCM16"), CTestMmfAclntCodecTest::EPcmU8));
	AddTestStepL( CTestMmfAclntConvertDes::NewL( _L("MM-MMF-ACLNT-I-7007-LP"),_L("SectionOne"),_L("PCM16"), CTestMmfAclntCodecTest::EMulawRaw));

//	AddTestStepL( CTestMmfAclntConvertDes::NewL( _L("MM-MMF-ACLNT-I-7099-LP"),_L("SectionOne"),_L("PCM16"), CTestMmfAclntCodecTest::E16BitAu));
	// Convert to a URL
	AddTestStepL( CTestMmfAclntConvertUrl::NewL( _L("MM-MMF-ACLNT-I-7008-LP"),EFalse));

	//Code coverage tests
	AddTestStepL( CTestMmfAclntConvertDestBitRate::NewL( _L("MM-MMF-ACLNT-I-0108-LP"),EFalse) );
	AddTestStepL( CTestMmfAclntConvertDestSampleRate::NewL( _L("MM-MMF-ACLNT-I-0109-LP"), EFalse) );
	AddTestStepL( CTestMmfAclntConvertDestDataType::NewL( _L("MM-MMF-ACLNT-I-0111-LP"),EFalse) );
   	AddTestStepL( CTestMmfAclntConvertDestNumberOfChannels::NewL( _L("MM-MMF-ACLNT-I-0110-LP"),EFalse) );
	AddTestStepL( CTestMmfAclntConvertDestFormat::NewL( _L("MM-MMF-ACLNT-I-0112-LP"),EFalse) );
	AddTestStepL( CTestMmfAclntConvertSourceInfo::NewL( _L("MM-MMF-ACLNT-I-0114-LP"),EFalse) );
	AddTestStepL( CTestMmfAclntConvertCrop::NewL( _L("MM-MMF-ACLNT-I-0115-LP"), EFalse, EFalse) );
	AddTestStepL( CTestMmfAclntConvertCrop::NewL( _L("MM-MMF-ACLNT-I-0116-LP"), ETrue, EFalse) );
	AddTestStepL( CTestMmfAclntConvertCrop::NewL( _L("MM-MMF-ACLNT-I-0120-LP"), EFalse, ETrue) );
	AddTestStepL( CTestMmfAclntConvertLength::NewL( _L("MM-MMF-ACLNT-I-0117-LP"), EFalse) );
	AddTestStepL( CTestMmfAclntConvertRepeat::NewL( _L("MM-MMF-ACLNT-I-0118-LP"), EFalse) );
	AddTestStepL( CTestMmfAclntConvertStop::NewL( _L("MM-MMF-ACLNT-I-0119-LP"), EFalse) );


	// Tests for CToneUtils


	AddTestStepL( CTestMmfAclntTone::NewL( _L("MM-MMF-ACLNT-I-0151-CP")) );
	AddTestStepL( CTestMmfAclntToneDtmf::NewL( _L("MM-MMF-ACLNT-I-0152-HP"),KDTMFString) );
	AddTestStepL( CTestMmfAclntToneFile::NewL( _L("MM-MMF-ACLNT-I-0153-HP"), _L("SectionSeven"), _L("toneSequence")) );
	AddTestStepL( CTestMmfAclntToneDes::NewL( _L("MM-MMF-ACLNT-I-0154-HP"),KFixedSequenceData, KErrNone) );
	AddTestStepL( CTestMmfAclntToneFixed::NewL( _L("MM-MMF-ACLNT-I-0155-CP"),1) );
	AddTestStepL( new(ELeave) CTestMmfAclntToneAudio );
	AddTestStepL( CTestMmfAclntToneCancelP::NewL( _L("MM-MMF-ACLNT-I-0157-CP")) );
	AddTestStepL( CTestMmfAclntToneCancelIni::NewL( _L("MM-MMF-ACLNT-I-0158-CP")) );
	AddTestStepL( CTestMmfAclntToneNames::NewL( _L("MM-MMF-ACLNT-I-0159-LP")) );
	AddTestStepL( CTestMmfAclntToneCount::NewL( _L("MM-MMF-ACLNT-I-0160-LP")) );
	AddTestStepL( CTestMmfAclntToneVolume::NewL( _L("MM-MMF-ACLNT-I-0161-HP"),-1) );
	AddTestStepL( CTestMmfAclntVolumeRamp::NewL( _L("MM-MMF-ACLNT-I-0162-LP"),10) );
	AddTestStepL( CTestMmfAclntToneRepeat::NewL( _L("MM-MMF-ACLNT-I-0163-LP")) );
	AddTestStepL( CTestMmfAclntToneLength::NewL( _L("MM-MMF-ACLNT-I-0164-HP")) );
	AddTestStepL( new(ELeave) CTestMmfAclntPriorityTones );
	AddTestStepL( CTestMmfAclntToneBalance::NewL( _L("MM-MMF-ACLNT-I-0166-LP"),10) );

 	//Play DualTone Test for Sirocco CR
 	AddTestStepL( CTestMmfAclntDualTone::NewL( _L("MM-MMF-ACLNT-I-0167-CP")) );


	//tests to use CMMFCodecs instead of CMMFHwDevice plugins
	AddTestStepL( CTestMmfAclntOldCodecFile::NewL(_L("MM-MMF-ACLNT-I-8002-HP"),_L("SectionOne"),_L("ALAW "),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntOldCodecFile::NewL(_L("MM-MMF-ACLNT-I-8004-HP"),_L("SectionOne"),_L("PCMU16BE"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntOldCodecFile::NewL(_L("MM-MMF-ACLNT-I-8005-HP"),_L("SectionOne"),_L("PCMU8"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntOldCodecFile::NewL(_L("MM-MMF-ACLNT-I-8006-HP"),_L("SectionOne"),_L("PCMU16"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntOldCodecFile::NewL(_L("MM-MMF-ACLNT-I-8007-HP"),_L("SectionOne"),_L("MULAW"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntOldCodecFile::NewL(_L("MM-MMF-ACLNT-I-8008-HP"),_L("SectionOne"),_L("IMAD"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntOldCodecFile::NewL(_L("MM-MMF-ACLNT-I-8010-HP"),_L("SectionOne"),_L("GSM610"),ETrue,EFalse) );
 
   

	//
	// NEGATIVE TESTS
	//

	// Tests for CAudioPlayerUtil


	AddTestStepL( CTestMmfAclntAudioPlayVolume::NewL( _L("MM-MMF-ACLNT-I-1001-LP"),_L("SectionOne"),-500) );
	AddTestStepL( CTestMmfAclntAudioPlayVolume::NewL( _L("MM-MMF-ACLNT-I-1013-LP"),_L("SectionOne"),500000000) );
	AddTestStepL( CTestMmfAclntAudioBalance::NewL( _L("MM-MMF-ACLNT-I-1002-LP"),_L("SectionOne"),-200, KErrNone) );
	AddTestStepL( CTestMmfAclntAudioBalance::NewL( _L("MM-MMF-ACLNT-I-1014-LP"),_L("SectionOne"),200, KErrNone) );
	AddTestStepL( CTestMmfAclntAudioPlayWindow::NewL( _L("MM-MMF-ACLNT-I-1004-HP"),_L("SectionOne"),ETrue) );
	AddTestStepL( CTestMmfAclntNoLoad::NewL( _L("MM-MMF-ACLNT-I-1005-LP"),_L("SectionOne") ) );
	AddTestStepL( CTestMmfFormatRead::NewL( _L("MM-MMF-ACLNT-I-6051-LP"),_L("SectionOne"),ETrue));

	// Load unsupported audio file + Load file that doesnt exist.
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-1007-HP"),_L("SectionFive"),_L("playerAudioFile "),EFalse,KErrNotSupported) );
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNT-I-1008-HP"),_L("SectionSix"),_L("playerAudioFile "),EFalse, KErrNotFound) );

	// Set invalid position
	position = -1000;
	AddTestStepL( CTestMmfAclntAudioPosition::NewL(_L("MM-MMF-ACLNT-I-1009-LP"),_L("SectionOne"),position) );
	position = 524303200;
	AddTestStepL( CTestMmfAclntAudioPosition::NewL(_L("MM-MMF-ACLNT-I-1015-LP"),_L("SectionOne"),position) );
	position = -1;
	AddTestStepL( CTestMmfAclntAudioPosition::NewL(_L("MM-MMF-ACLNT-I-1100-LP"),_L("SectionOne"),position) );

	//
	AddTestStepL( CTestMmfAclntDeletePlay::NewL(_L("MM-MMF-ACLNT-I-1010-HP"),_L("SectionOne") ) );	
	AddTestStepL( CTestMmfAclntPlayPlay::NewL( _L("MM-MMF-ACLNT-I-1011-LP"),_L("SectionOne"),_L("PCM16") ) );
	AddTestStepL( new(ELeave) CTestMmfAclntCorruptDes(_L("MM-MMF-ACLNT-I-1012-LP"), _L("SectionTwo")) );


	// Tests for CConversionUtil 

	AddTestStepL( CTestMmfAclntConConfig::NewL( _L("MM-MMF-ACLNT-I-1006-LP"), CTestMmfAclntConConfig::EPcm16Wav, ETrue) );
	AddTestStepL( CTestMmfAclntDoConv::NewL(  _L("MM-MMF-ACLNT-I-1050-LP"), _L("SectionFour") , _L("auAudioFile") , _L("SectionFive") , _L("outputAudioFile"), -1, CTestMmfAclntDoConv::MdaFormatSpec) ); 
	
	//inc21946
	AddTestStepL( CTestMmfAclntDoConv::NewL(  _L("MM-MMF-ACLNT-I-1055-LP"), _L("SectionFour") , _L("auAudioFile") , _L("SectionFive") , _L("outputAudioFile"), -1, CTestMmfAclntDoConv::MmfFormatSpec) ); 

	AddTestStepL( new(ELeave) CTestMmfAclntDelete );
	AddTestStepL( CTestMmfAclntOpenWhileConvert::NewL( _L("MM-MMF-ACLNT-I-1052-LP")) );
	position = 145235000;
	AddTestStepL( CTestMmfAclntConPosition::NewL( _L("MM-MMF-ACLNT-I-1053-LP"),position) );
	position = -10;
	AddTestStepL( CTestMmfAclntConPosition::NewL( _L("MM-MMF-ACLNT-I-1054-LP"),position) );
	

	// Tests for CRecordUtil 


	AddTestStepL( CTestMmfAclntRecFile::NewL( _L("MM-MMF-ACLNT-I-1200-CP"),_L("SectionSix"),_L("outputAudioFile")) );
	AddTestStepL( new(ELeave) CTestMmfAclntRecDelete(_L("MM-MMF-ACLNT-I-1201-HP"),_L("SectionThree") ) );
	AddTestStepL( CTestMmfAclntRecGain::NewL(_L("MM-MMF-ACLNT-I-1204-LP"),_L("SectionThree"),1234321, ETrue) );
	AddTestStepL( CTestMmfAclntRecGain::NewL(_L("MM-MMF-ACLNT-I-1208-LP"),_L("SectionThree"),-1231, ETrue) );
	position = -10202020;
	AddTestStepL( CTestMmfAclntRecPosition::NewL( _L("MM-MMF-ACLNT-I-1205-HP"),_L("SectionOne"),position, ETrue) );
	position = 10202020;
	AddTestStepL( CTestMmfAclntRecPosition::NewL( _L("MM-MMF-ACLNT-I-1209-HP"),_L("SectionOne"),position, ETrue) );
	AddTestStepL( CTestMmfAclntRecBalance::NewL( _L("MM-MMF-ACLNT-I-1206-LP"),_L("SectionOne"),-200, ETrue) );
	AddTestStepL( CTestMmfAclntRecBalance::NewL( _L("MM-MMF-ACLNT-I-1210-LP"),_L("SectionOne"),200, ETrue) );
	AddTestStepL( CTestMmfAclntRecLength::NewL( _L("MM-MMF-ACLNT-I-1207-LP"),_L("SectionOne"), ETrue) );


	// Tests for CToneUtils


	AddTestStepL( CTestMmfAclntToneDtmf::NewL( _L("MM-MMF-ACLNT-I-1150-HP"),KInvalidDTMFString, KErrArgument) );
	AddTestStepL( CTestMmfAclntToneDes::NewL( _L("MM-MMF-ACLNT-I-1151-HP"),KCorruptFixedSequenceData, KErrCorrupt) );
	AddTestStepL( CTestMmfAclntToneFixed::NewL( _L("MM-MMF-ACLNT-I-1152-HP"),1) );
	AddTestStepL( CTestMmfAclntToneVolume::NewL( _L("MM-MMF-ACLNT-I-1153-LP"),263465) );
	AddTestStepL( CTestMmfAclntToneVolume::NewL( _L("MM-MMF-ACLNT-I-1157-LP"),-2442) );
	AddTestStepL( CTestMmfAclntVolumeRamp::NewL( _L("MM-MMF-ACLNT-I-1154-HP"),-1234) );
	AddTestStepL( CTestMmfAclntVolumeRamp::NewL( _L("MM-MMF-ACLNT-I-1159-HP"),2341234) );
	AddTestStepL( new(ELeave) CTestMmfAclntOnOffPause );
	AddTestStepL( CTestMmfAclntToneBalance::NewL( _L("MM-MMF-ACLNT-I-1156-HP"),-2131) );
	AddTestStepL( CTestMmfAclntToneBalance::NewL( _L("MM-MMF-ACLNT-I-1158-HP"),1231230) );



// Additional tests for RecorderUtil (as reported by C Cover)
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6010-CP"),_L("SectionEight"),_L("playerAudioFile"),CTestMmfAclntCodecTest::EPcm8,EFalse) );
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6011-CP"),_L("SectionEight"),_L("playerAudioFile"),CTestMmfAclntCodecTest::EAlawWav,EFalse) );
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6012-CP"),_L("SectionEight"),_L("playerAudioFile"),CTestMmfAclntCodecTest::EPcm16Wav,EFalse) );
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6013-CP"),_L("SectionEight"),_L("playerAudioFile"),CTestMmfAclntCodecTest::EPcmU16,EFalse) );
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6014-CP"),_L("SectionEight"),_L("playerAudioFile"),CTestMmfAclntCodecTest::EPcmU8,EFalse) );
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6015-CP"),_L("SectionEight"),_L("playerAudioFile"),CTestMmfAclntCodecTest::EMulawRaw,EFalse) );
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6016-CP"),_L("SectionEight"),_L("playerAudioFile"),CTestMmfAclntCodecTest::EImaAdpcmWav,EFalse) );
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6017-CP"),_L("SectionEight"),_L("playerAudioFile"),CTestMmfAclntCodecTest::EImasPcmWav,EFalse) );
	AddTestStepL( CTestMmfAclntRecDes::NewL( _L("MM-MMF-ACLNT-I-6018-CP"),_L("SectionEight"),_L("playerAudioFile"),CTestMmfAclntCodecTest::EGsmWav,EFalse) );
	
	AddTestStepL( CTestMmfAclntRecordDestChannels::NewL( _L("MM-MMF-ACLNT-I-6020-CP"),_L("SectionEight"),_L("playerAudioFile"),1,CTestMmfAclntCodecTest::EPcm8,EFalse) );
	AddTestStepL( CTestMmfAclntRecordSetPriority::NewL( _L("MM-MMF-ACLNT-I-6021-CP"),_L("SectionOne"),EFalse) );
	AddTestStepL( CTestMmfAclntRecordDestSampleRate::NewL( _L("MM-MMF-ACLNT-I-6022-CP"),_L("SectionEight"),_L("playerAudioFile"),CTestMmfAclntCodecTest::EPcm8,ETrue) );
	AddTestStepL( CTestMmfAclntRecordDestBitRate::NewL( _L("MM-MMF-ACLNT-I-6023-CP"),_L("SectionOne"),EFalse) );
	AddTestStepL( CTestMmfAclntRecordDestDataType::NewL( _L("MM-MMF-ACLNT-I-6024-CP"),_L("SectionEight"),_L("playerAudioFile"),CTestMmfAclntCodecTest::EPcm8,ETrue) );
	AddTestStepL( CTestMmfAclntRecordDestFormat::NewL( _L("MM-MMF-ACLNT-I-6025-CP"),_L("SectionOne"),EFalse,TUid::Uid(0)) );//@@@
	AddTestStepL( CTestMmfAclntRecordAudioDeviceMode::NewL( _L("MM-MMF-ACLNT-I-6026-CP"),_L("SectionOne"),EFalse) );
	
	AddTestStepL( CTestMmfAclntAudioRecordVolume::NewL( _L("MM-MMF-ACLNT-I-6027-CP"),_L("SectionOne"),EFalse) );
	AddTestStepL( CTestMmfAclntAudioRecordRamp::NewL( _L("MM-MMF-ACLNT-I-6028-CP"),_L("SectionOne"),EFalse) );

	AddTestStepL( CTestMmfAclntRecordDestSampleRate::NewL( _L("MM-MMF-ACLNT-I-6029-CP"),_L("SectionEight"),_L("playerAudioFileAU"),CTestMmfAclntCodecTest::E16BitAu,ETrue) );

// Additional tests for CMdaAudioInputStream (as reported by C Cover)
	AddTestStepL(CTestStepAudInStreamSetGain::NewL(_L("MM-MMF-ACLNT-I-6030-CP")));
	AddTestStepL(CTestStepAudInStreamSetPriority::NewL(_L("MM-MMF-ACLNT-I-6031-CP")));
	AddTestStepL(CTestStepAudInStreamSetBalance::NewL(_L("MM-MMF-ACLNT-I-6032-CP")));
	AddTestStepL(CTestStepAudInStreamGetBytes::NewL(_L("MM-MMF-ACLNT-I-6033-CP")));
	AddTestStepL(CTestStepAudInStreamSetProp::NewL(_L("MM-MMF-ACLNT-I-6034-CP")));
	AddTestStepL(CTestStepAudInStreamSglBuf::NewL(_L("MM-MMF-ACLNT-I-6035-CP")));
	

// Additional tests for CMdaAudioInputStream (as reported by C Cover)
	AddTestStepL(CTestStepAudOutStreamSetProp::NewL(_L("MM-MMF-ACLNT-I-6040-CP")));
	AddTestStepL(CTestStepAudOutStreamOpen::NewL(_L("MM-MMF-ACLNT-I-6041-CP")));
	AddTestStepL(CTestStepAudOutStreamSetVol::NewL(_L("MM-MMF-ACLNT-I-6042-CP")));
	AddTestStepL(CTestStepAudOutStreamSetBalance::NewL(_L("MM-MMF-ACLNT-I-6043-CP")));
	AddTestStepL(CTestStepAudOutStreamSetPrior::NewL(_L("MM-MMF-ACLNT-I-6044-CP")));
	AddTestStepL(CTestStepAudOutStreamStop::NewL(_L("MM-MMF-ACLNT-I-6045-CP")));
	AddTestStepL(CTestStepAudOutStreamPosition::NewL(_L("MM-MMF-ACLNT-I-6046-CP")));
	AddTestStepL(CTestStepAudOutStreamGetBytes::NewL(_L("MM-MMF-ACLNT-I-6047-CP")));

	// DRM test Steps
	AddTestStepL(CTestMmfAclntDRMPlayerNoRights::NewL());		// MM-MMF-ACLNT-I-9000-LP
	AddTestStepL(CTestMmfAclntDRMPlayerRightsCount::NewL());	// MM-MMF-ACLNT-I-9001-LP
	AddTestStepL(CTestMmfAclntDRMRecorderNoRights::NewL());		// MM-MMF-ACLNT-I-9002-LP
	AddTestStepL(CTestMmfAclntDRMRecorderRightsCount::NewL());	// MM-MMF-ACLNT-I-9003-LP
	AddTestStepL(CTestMmfAclntDRMPlayerOpen::NewL());			// MM-MMF-ACLNT-I-9004-LP
	AddTestStepL(CTestMmfAclntDRMRecorderPlay::NewL());			// MM-MMF-ACLNT-I-9005-LP
	AddTestStepL(CTestMmfAclntDRMConverterFail::NewL());		// MM-MMF-ACLNT-I-9006-LP
	
	// Capabilities Test
	AddTestStepL(CTestMmfAclntCaps0012::NewL(_L("MM-MMF-SDEVSOUND-I-0012-HP"),_L("SectionOne"),_L("playerAudioFile")));
	AddTestStepL(CTestMmfAclntCaps0014::NewL(_L("MM-MMF-SDEVSOUND-I-0014-HP"),_L("SectionOne"),_L("playerAudioFile")));
	//The requesting client has No capabilities
	AddTestStepL(CTestStepAudInStreamNoUECap::NewL(_L("MM-MMF-SDEVSOUND-I-0031-HP")));	
	//The requesting client has a MultimediaDD capability
	AddTestStepL(CTestStepAudInStreamNoUECap::NewL(_L("MM-MMF-SDEVSOUND-I-0032-HP")));
	//The requesting client has UserEnvironment cpability
	AddTestStepL(CTestStepAudInStreamWithUECap::NewL(_L("MM-MMF-SDEVSOUND-I-0033-HP")));
	//The requesting client has ALL -TCB capabilities
	AddTestStepL(CTestStepAudInStreamWithUECap::NewL(_L("MM-MMF-SDEVSOUND-I-0034-HP")));
	

	// Media Access by File Handle
	AddTestStepL( CTestMmfAclntOpenFile7803::NewL(_L("MM-MMF-ACLNT-I-7803-HP"),_L("SectionOne"),_L("PCM16")/*,ETrue,EFalse*/) );
	AddTestStepL( CTestMmfAclntOpenFile7804::NewL(_L("MM-MMF-ACLNT-I-7804-HP"),_L("SectionOne")/*,_L("PCM16"),ETrue,EFalse*/) );
	AddTestStepL( CTestMmfAclntOpenFile7805::NewL(_L("MM-MMF-ACLNT-I-7805-HP"),_L("SectionOne"),_L("PCM16")/*,ETrue,EFalse*/) );
	AddTestStepL( CTestMmfAclntOpenFile7806::NewL(_L("MM-MMF-ACLNT-I-7806-HP"),_L("SectionOne")/*,_L("PCM16"),ETrue,EFalse*/) );
	AddTestStepL( CTestMmfAclntOpenFile7901::NewL(_L("MM-MMF-ACLNT-I-7901-HP"),_L("SectionOne"), EFalse) );

	AddTestStepL( CTestMmfAclntOpenFile7902::NewL( _L("MM-MMF-ACLNT-I-7902-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EPcm16Wav,ETrue) );
	AddTestStepL( CTestMmfAclntOpenFile7904::NewL( _L("MM-MMF-ACLNT-I-7904-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EPcm16Wav,ETrue) );
	AddTestStepL( CTestMmfAclntOpenFile7905::NewL( _L("MM-MMF-ACLNT-I-7905-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EPcm16Wav,ETrue) );
	AddTestStepL( CTestMmfAclntOpenFile7906::NewL( _L("MM-MMF-ACLNT-I-7906-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EPcm16Wav,ETrue) );
	AddTestStepL( CTestMmfAclntOpenFile7908::NewL( _L("MM-MMF-ACLNT-I-7908-HP"),_L("SectionThree"),_L("outputAudioFile"),CTestMmfAclntCodecTest::EPcm16Wav,ETrue) );

	AddTestStepL( CTestMmfAclntOpenFile8801::NewL(_L("MM-MMF-ACLNT-I-8801-HP"),_L("SectionOne")/*,_L("PCM16"),ETrue,EFalse*/) );
	AddTestStepL( CTestMmfAclntOpenFile1211::NewL( _L("MM-MMF-ACLNT-I-1211-HP"),_L("SectionThree"),_L("outputTextFile"),CTestMmfAclntCodecTest::EPcm16Wav,ETrue) );

	AddTestStepL( CTestMmfAclntOpenToneFile::NewL( _L("MM-MMF-ACLNT-I-1212-HP"), _L("SectionSeven"), _L("toneSequence")) );

    // Tests for TMMFileSource and TMMFileHandleSource
    AddTestStepL( CTestMmfAclntFileSource::NewL(_L("MM-MMF-ACLNT-I-2031-HP"),_L("SectionOne"),_L("PCMU16BE"),ETrue,EFalse) );
    AddTestStepL( CTestMmfAclntFileHandleSource::NewL(_L("MM-MMF-ACLNT-I-2032-HP"),_L("SectionOne"),_L("PCMU16BE"),ETrue,EFalse) );
    AddTestStepL( CTestMmfAclntRecFileSource::NewL(_L("MM-MMF-ACLNT-I-5021-HP"),_L("SectionThree"),_L("outputAudioFile"), EFileName, CTestMmfAclntCodecTest::EPcm16Wav,EFalse) );
    AddTestStepL( CTestMmfAclntRecFileSource::NewL(_L("MM-MMF-ACLNT-I-5022-HP"),_L("SectionThree"),_L("outputAudioFile"), EFileHandle, CTestMmfAclntCodecTest::EPcm16Wav,EFalse) );
    AddTestStepL( CTestMmfAclntRecPlayFileSource::NewL(_L("MM-MMF-ACLNT-I-5031-HP"),_L("SectionThree"),_L("outputAudioFile"), CTestMmfAclntCodecTest::EPcm16Wav, EFileName));
    AddTestStepL( CTestMmfAclntRecPlayFileSource::NewL(_L("MM-MMF-ACLNT-I-5032-HP"),_L("SectionThree"),_L("outputAudioFile"), CTestMmfAclntCodecTest::EPcm16Wav, EFileHandle));


    // GT 0222 MS3.6, PREQ777 DRM Support
    AddTestStepL( CTestMmfAudioPlayDRMEnableAgentUI::NewL(_L("MM-MMF-ACLNT-I-9011-HP"), _L("SectionDRM"), _L("AudioContent"), _L("AudioTestmainTstId001")) );
    AddTestStepL( CTestMmfAudioPlayDRMDisableAutoIntent::NewL(_L("MM-MMF-ACLNT-I-9012-HP"), _L("SectionDRM"), _L("AudioContent"), _L("AudioTestmainTstId001")) );

    AddTestStepL( CTestMmfAudioRecPlayDRMEnableAgentUI::NewL(_L("MM-MMF-ACLNT-I-9021-HP"), _L("SectionDRM"), _L("AudioContent"), _L("AudioTestmainTstId001")) );
    AddTestStepL( CTestMmfAudioRecPlayDRMDisableAutoIntent::NewL(_L("MM-MMF-ACLNT-I-9022-HP"), _L("SectionDRM"), _L("AudioContent"), _L("AudioTestmainTstId001")) );

	//**************PREQ 797 - Audio Resource Notification**************
	// Positive Cases
	AddTestStepL( CTestMmfAclntARN8101::NewL(_L("MM-MMF-ACLNT-I-8101-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN8102::NewL(_L("MM-MMF-ACLNT-I-8102-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN8103::NewL(_L("MM-MMF-ACLNT-I-8103-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN8104::NewL(_L("MM-MMF-ACLNT-I-8104-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN8105::NewL(_L("MM-MMF-ACLNT-I-8105-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN8106::NewL(_L("MM-MMF-ACLNT-I-8106-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN8107::NewL(_L("MM-MMF-ACLNT-I-8107-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN8108::NewL(_L("MM-MMF-ACLNT-I-8108-HP"),_L("SectionARN") ) );
	//************ CR1424:  high priority client 
	//************ plays, low priority client does not get the resources when the high priority client is playing silence for every repeat.
	AddTestStepL( CTestMmfAclntARN8109::NewL(_L("MM-MMF-ACLNT-I-8109-HP"),_L("SectionARN") ) );
	//************ CR :Dont register for ARN, get pre-empted and then resume from same position */
    AddTestStepL( CTestMmfAclntARN8110::NewL(_L("MM-MMF-ACLNT-I-8110-HP"),_L("SectionARN") ) );
    AddTestStepL( CTestMmfAclntARN8111::NewL(_L("MM-MMF-ACLNT-I-8111-HP"),_L("SectionARN") ) );
    AddTestStepL( CTestMmfAclntARN8112::NewL(_L("MM-MMF-ACLNT-I-8112-HP"),_L("SectionARN") ) );
	
	
	AddTestStepL( CTestMmfAclntARN8011::NewL(_L("MM-MMF-ACLNT-I-8011-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN8012::NewL(_L("MM-MMF-ACLNT-I-8012-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN8013::NewL(_L("MM-MMF-ACLNT-I-8013-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN8014::NewL(_L("MM-MMF-ACLNT-I-8014-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN8015::NewL(_L("MM-MMF-ACLNT-I-8015-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN8016::NewL(_L("MM-MMF-ACLNT-I-8016-HP"),_L("SectionARN") ) );
	
	AddTestStepL( CTestMmfAclntARN8021::NewL(_L("MM-MMF-ACLNT-I-8021-HP") ) );
	AddTestStepL( CTestMmfAclntARN8022::NewL(_L("MM-MMF-ACLNT-I-8022-HP") ) );
	AddTestStepL( CTestMmfAclntARN8023::NewL(_L("MM-MMF-ACLNT-I-8023-HP") ) );
	AddTestStepL( CTestMmfAclntARN8024::NewL(_L("MM-MMF-ACLNT-I-8024-HP") ) );
	AddTestStepL( CTestMmfAclntARN8025::NewL(_L("MM-MMF-ACLNT-I-8025-HP") ) );
	AddTestStepL( CTestMmfAclntARN8026::NewL(_L("MM-MMF-ACLNT-I-8026-HP") ) );
	
	AddTestStepL( CTestMmfAclntARN9001::NewL(_L("MM-MMF-ACLNT-I-9001-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN9002::NewL(_L("MM-MMF-ACLNT-I-9002-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN9003::NewL(_L("MM-MMF-ACLNT-I-9003-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN9004::NewL(_L("MM-MMF-ACLNT-I-9004-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN9005::NewL(_L("MM-MMF-ACLNT-I-9005-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN9006::NewL(_L("MM-MMF-ACLNT-I-9006-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN9007::NewL(_L("MM-MMF-ACLNT-I-9007-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN9008::NewL(_L("MM-MMF-ACLNT-I-9008-HP"),_L("SectionARN") ) );
	
	// Negative Cases
	AddTestStepL( CTestMmfAclntARN1301::NewL(_L("MM-MMF-ACLNT-I-1301-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN1302::NewL(_L("MM-MMF-ACLNT-I-1302-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN1303::NewL(_L("MM-MMF-ACLNT-I-1303-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN1304::NewL(_L("MM-MMF-ACLNT-I-1304-HP"),_L("SectionARN") ) );
	
	AddTestStepL( CTestMmfAclntARN1311::NewL(_L("MM-MMF-ACLNT-I-1311-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN1312::NewL(_L("MM-MMF-ACLNT-I-1312-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN1313::NewL(_L("MM-MMF-ACLNT-I-1313-HP"),_L("SectionARN") ) );
	AddTestStepL( CTestMmfAclntARN1314::NewL(_L("MM-MMF-ACLNT-I-1314-HP"),_L("SectionARN") ) );
	
	AddTestStepL( CTestMmfAclntARN1321::NewL(_L("MM-MMF-ACLNT-I-1321-HP") ) );
	AddTestStepL( CTestMmfAclntARN1322::NewL(_L("MM-MMF-ACLNT-I-1322-HP") ) );
	AddTestStepL( CTestMmfAclntARN1323::NewL(_L("MM-MMF-ACLNT-I-1323-HP") ) );
	AddTestStepL( CTestMmfAclntARN1324::NewL(_L("MM-MMF-ACLNT-I-1324-HP") ) );
	
#ifdef SYMBIAN_VARIABLE_BITRATE_CODEC // use system wide flag instead of SYMBIAN_MMF_MODEL_CONTROLLERS for ease
	//
	// Ogg-Vorbis tests //
	//
	
	// POSITIVE TESTS :: Audio Player (Ogg-Vorbis)
	AddTestStepL( CTestMmfAclntFile::NewL(_L("MM-MMF-ACLNTOGG-I-0001-CP"),_L("SectionOgg"),_L("playerAudioFile"),ETrue,EFalse) );
	AddTestStepL( CTestMmfAclntDesc::NewL(_L("MM-MMF-ACLNTOGG-I-0002-CP"),_L("SectionOggSmall"),_L("playerAudioFile"),ETrue) );
	AddTestStepL( CTestMmfAclntAudioPlayStopStart::NewL(_L("MM-MMF-ACLNTOGG-I-0003-CP"),_L("SectionOgg") ) );
	AddTestStepL( CTestMmfAclntAudioPlayPauseStart::NewL(_L("MM-MMF-ACLNTOGG-I-0004-CP"),_L("SectionOgg") ) );
	AddTestStepL( CTestMmfAclntAudioPlayVolume::NewL(_L("MM-MMF-ACLNTOGG-I-0005-CP"),_L("SectionOgg") ) );
	AddTestStepL( CTestMmfAclntAudioRepeat::NewL(_L("MM-MMF-ACLNTOGG-I-0006-CP"),_L("SectionOgg") ) );
	AddTestStepL( CTestMmfAclntAudioBalance::NewL(_L("MM-MMF-ACLNTOGG-I-0007-CP"),_L("SectionOgg"),10, KErrNone) );
	AddTestStepL( CTestMmfAclntAudioDuration::NewL(_L("MM-MMF-ACLNTOGG-I-0008-HP"),_L("SectionOgg") ) );
	AddTestStepL( CTestMmfAclntPriority::NewL( _L("MM-MMF-ACLNTOGG-I-0009-HP"), _L("SectionOgg"), EMdaPriorityMin) );
	AddTestStepL( CTestMmfAclntPriority::NewL( _L("MM-MMF-ACLNTOGG-I-0010-HP"), _L("SectionOgg") ) );
	AddTestStepL( CTestMmfAclntPriority::NewL( _L("MM-MMF-ACLNTOGG-I-0011-HP"), _L("SectionOgg"), EMdaPriorityMax) );
	AddTestStepL( CTestMmfAclntPriority::NewL( _L("MM-MMF-ACLNTOGG-I-0012-HP"), _L("SectionOgg"), EMdaPriorityNormal) );
	position = 0;
	AddTestStepL( CTestMmfAclntAudioPosition::NewL(_L("MM-MMF-ACLNTOGG-I-0013-HP"),_L("SectionOgg"), position) );
	AddTestStepL( CTestMmfAclntAudioMeta::NewL(_L("MM-MMF-ACLNTOGG-I-0014-HP"),_L("SectionOgg") ) );
	AddTestStepL( CTestMmfAclntAudioPlayVolume::NewL(_L("MM-MMF-ACLNTOGG-I-0015-HP"),_L("SectionOgg") ) );
	AddTestStepL( CTestMmfAclntAudioRamp::NewL(_L("MM-MMF-ACLNTOGG-I-0016-HP"),_L("SectionOgg") ) );
	AddTestStepL( CTestMmfAclntAudioCloseOpen::NewL(_L("MM-MMF-ACLNTOGG-I-0017-HP"),_L("SectionOgg") ) );
	AddTestStepL( CTestMmfAclntAudioPlayWindow::NewL(_L("MM-MMF-ACLNTOGG-I-0018-HP"),_L("SectionOgg") ) );
	AddTestStepL( CTestMmfAclntOpenFile7805::NewL(_L("MM-MMF-ACLNTOGG-I-0019-HP"),_L("SectionOgg"),_L("playerAudioFile")/*,ETrue,EFalse*/) );
	
	// POSITIVE TESTS :: Audio Player (Ogg-Vorbis) APR
	AddTestStepL( CTestMmfAclntARN8101::NewL(_L("MM-MMF-ACLNTOGG-I-0020-CP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN8102::NewL(_L("MM-MMF-ACLNTOGG-I-0021-CP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN8103::NewL(_L("MM-MMF-ACLNTOGG-I-0022-CP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN8104::NewL(_L("MM-MMF-ACLNTOGG-I-0023-CP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN8105::NewL(_L("MM-MMF-ACLNTOGG-I-0024-CP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN8106::NewL(_L("MM-MMF-ACLNTOGG-I-0025-HP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN8107::NewL(_L("MM-MMF-ACLNTOGG-I-0026-HP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN8108::NewL(_L("MM-MMF-ACLNTOGG-I-0027-HP"),_L("SectionOggAPR") ) );
	//************ CR1424:  high priority client 
	//************ plays, low priority client does not get the resources when the high priority client is playing silence for every repeat.
	AddTestStepL( CTestMmfAclntARN8109::NewL(_L("MM-MMF-ACLNTOGG-I-0030-HP"),_L("SectionOggAPR") ) );
	// POSITIVE TESTS :: Audio Player DRM (Ogg-Vorbis)
	AddTestStepL( CTestMmfAudioPlayDRMEnableAgentUI::NewL(_L("MM-MMF-ACLNTOGG-I-0028-HP"), _L("SectionOggDRM"), _L("AudioContent"), _L("AudioOggTestmainTstId001")) );
	AddTestStepL( CTestMmfAudioPlayDRMDisableAutoIntent::NewL(_L("MM-MMF-ACLNTOGG-I-0029-HP"), _L("SectionOggDRM"), _L("AudioContent"), _L("AudioOggTestmainTstId001")) );


	// POSITIVE TESTS :: Audio Recorder (Ogg-Vorbis)
	// No Need to Pass params as in CTestMmfAclntCodecTest, to support Ogg-Vorbis.. This is deprecated so passing in ENone...
	AddTestStepL( CTestMmfAclntRecFile::NewL(_L("MM-MMF-ACLNTOGG-I-0101-CP"),_L("SectionOggRec"),_L("outputAudioFile"),CTestMmfAclntCodecTest::ENone, ETrue) );
	AddTestStepL( CTestMmfAclntRecFile::NewL(_L("MM-MMF-ACLNTOGG-I-0102-CP"),_L("SectionOggRec"),_L("cropAudioFile"),CTestMmfAclntCodecTest::ENone, EFalse) );
	AddTestStepL( CTestMmfAclntRecDes::NewL(_L("MM-MMF-ACLNTOGG-I-0103-CP"),_L("SectionOggRec"),_L("playerAudioFile"),CTestMmfAclntCodecTest::ENone, EFalse) );
	AddTestStepL( CTestMmfAclntRecGain::NewL(_L("MM-MMF-ACLNTOGG-I-0104-CP"),_L("SectionOggRec"),0, EFalse) );
	AddTestStepL( CTestMmfAclntRecStateTran::NewL(_L("MM-MMF-ACLNTOGG-I-0105-CP"),_L("SectionOggRecState"), EFalse) );
	AddTestStepL( CTestMmfAclntCrop::NewL(_L("MM-MMF-ACLNTOGG-I-0106-HP"),_L("SectionOgg"),ETrue, EFalse) );
	AddTestStepL( CTestMmfAclntCrop::NewL(_L("MM-MMF-ACLNTOGG-I-0107-HP"),_L("SectionOgg"),EFalse, EFalse) );
	position = 0;
	AddTestStepL( CTestMmfAclntRecPosition::NewL(_L("MM-MMF-ACLNTOGG-I-0108-HP"),_L("SectionOgg"),position, EFalse) );
	AddTestStepL( CTestMmfAclntRecDuration::NewL(_L("MM-MMF-ACLNTOGG-I-0109-HP"),_L("SectionOgg"), EFalse) );
	AddTestStepL( CTestMmfAclntRecSetMeta::NewL(_L("MM-MMF-ACLNTOGG-I-0110-HP"),_L("SectionOgg"), EFalse) );
	AddTestStepL( new(ELeave) CTestMmfAclntRecPriority(_L("MM-MMF-ACLNTOGG-I-0111-HP"), _L("SectionOgg")) );
	AddTestStepL( CTestMmfAclntRecBalance::NewL( _L("MM-MMF-ACLNTOGG-I-0112-HP"),_L("SectionOgg"),20, EFalse) );
	AddTestStepL( CTestMmfAclntRecLength::NewL(_L("MM-MMF-ACLNTOGG-I-0113-HP"),_L("SectionOgg"), EFalse) );
	AddTestStepL( CTestMmfAclntRecFormats::NewL(_L("MM-MMF-ACLNTOGG-I-0114-HP"),_L("SectionOgg"), EFalse) );
	AddTestStepL( CTestMmfAclntRecordDestChannels::NewL(_L("MM-MMF-ACLNTOGG-I-0115-HP"),_L("SectionOggRec"),_L("outputAudioFile"),1,CTestMmfAclntCodecTest::ENone,EFalse) );
	AddTestStepL( CTestMmfAclntRecordSetPriority::NewL(_L("MM-MMF-ACLNTOGG-I-0116-HP"),_L("SectionOgg"),EFalse) );
	AddTestStepL( CTestMmfAclntRecordDestSampleRate::NewL(_L("MM-MMF-ACLNTOGG-I-0117-HP"),_L("SectionOggRec"),_L("outputAudioFile"),CTestMmfAclntCodecTest::ENone,EFalse) );
	AddTestStepL( CTestMmfAclntRecordDestBitRate::NewL(_L("MM-MMF-ACLNTOGG-I-0118-HP"),_L("SectionOggRec"),EFalse) );
	AddTestStepL( CTestMmfAclntRecordDestDataType::NewL(_L("MM-MMF-ACLNTOGG-I-0119-HP"),_L("SectionOggRec"),_L("outputAudioFile"),CTestMmfAclntCodecTest::ENone,EFalse) );
	AddTestStepL( CTestMmfAclntRecordDestFormat::NewL(_L("MM-MMF-ACLNTOGG-I-0120-HP"),_L("SectionOgg"),EFalse,TUid::Uid(0)) );//@@@
	AddTestStepL( CTestMmfAclntRecordAudioDeviceMode::NewL(_L("MM-MMF-ACLNTOGG-I-0121-HP"),_L("SectionOgg"),EFalse) );
	AddTestStepL( CTestMmfAclntAudioRecordVolume::NewL(_L("MM-MMF-ACLNTOGG-I-0122-HP"),_L("SectionOgg"),EFalse) );
	AddTestStepL( CTestMmfAclntAudioRecordRamp::NewL(_L("MM-MMF-ACLNTOGG-I-0123-HP"),_L("SectionOgg"),EFalse) );
	AddTestStepL( CTestMmfAclntOpenFile7908::NewL(_L("MM-MMF-ACLNTOGG-I-0124-HP"),_L("SectionOggRec"),_L("outputAudioFile"),CTestMmfAclntCodecTest::ENone,ETrue) ); // EPcm16Wav
	
	// POSITIVE TESTS :: Audio Recorder (Ogg-Vorbis) APR
	AddTestStepL( CTestMmfAclntARN8011::NewL(_L("MM-MMF-ACLNTOGG-I-0125-CP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN8012::NewL(_L("MM-MMF-ACLNTOGG-I-0126-CP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN8013::NewL(_L("MM-MMF-ACLNTOGG-I-0127-HP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN8014::NewL(_L("MM-MMF-ACLNTOGG-I-0128-HP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN8015::NewL(_L("MM-MMF-ACLNTOGG-I-0129-HP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN8016::NewL(_L("MM-MMF-ACLNTOGG-I-0130-HP"),_L("SectionOggAPR") ) );
	
	// POSITIVE TESTS :: Ogg-Vorbis APR with 3 clients (COMBINED)
	AddTestStepL( CTestMmfAclntARN9001::NewL(_L("MM-MMF-ACLNTOGG-I-0201-HP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN9002::NewL(_L("MM-MMF-ACLNTOGG-I-0202-HP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN9003::NewL(_L("MM-MMF-ACLNTOGG-I-0203-HP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN9004::NewL(_L("MM-MMF-ACLNTOGG-I-0204-HP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN9005::NewL(_L("MM-MMF-ACLNTOGG-I-0205-HP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN9006::NewL(_L("MM-MMF-ACLNTOGG-I-0206-HP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN9007::NewL(_L("MM-MMF-ACLNTOGG-I-0207-HP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN9008::NewL(_L("MM-MMF-ACLNTOGG-I-0208-HP"),_L("SectionOggAPR") ) );

	// POSITIVE TESTS :: Audio Recorder DRM (Ogg-Vorbis)
	AddTestStepL( CTestMmfAudioRecPlayDRMEnableAgentUI::NewL(_L("MM-MMF-ACLNTOGG-I-0131-HP"), _L("SectionOggDRM"), _L("AudioContent"), _L("AudioOggTestmainTstId001")) );
	AddTestStepL( CTestMmfAudioRecPlayDRMDisableAutoIntent::NewL(_L("MM-MMF-ACLNTOGG-I-0132-HP"), _L("SectionOggDRM"), _L("AudioContent"), _L("AudioOggTestmainTstId001")) );
	
	// POSITIVE TESTS :: Audio Recorder Simple test with SetMaxFileSize()
	AddTestStepL( CTestMmfOggAudioRecorderBase::NewL(_L("MM-MMF-ACLNTOGG-I-0133-HP"),_L("SectionOggRec"), _L("outputAudioFile")) );
	
	// NEGATIVE TESTS :: Audio Player (Ogg-Vorbis)
	AddTestStepL( CTestMmfAclntAudioPlayVolume::NewL(_L("MM-MMF-ACLNTOGG-I-1001-LP"),_L("SectionOgg"), -500) );
	AddTestStepL( CTestMmfAclntAudioPlayVolume::NewL(_L("MM-MMF-ACLNTOGG-I-1002-LP"),_L("SectionOgg"),500000000) );
	AddTestStepL( CTestMmfAclntAudioBalance::NewL(_L("MM-MMF-ACLNTOGG-I-1003-LP"),_L("SectionOgg"),-200, KErrNone) );
	AddTestStepL( CTestMmfAclntAudioBalance::NewL(_L("MM-MMF-ACLNTOGG-I-1004-LP"),_L("SectionOgg"),200, KErrNone) );
	AddTestStepL( CTestMmfAclntAudioPlayWindow::NewL(_L("MM-MMF-ACLNTOGG-I-1005-LP"),_L("SectionOgg"),ETrue) );
	position = 524303200;
	AddTestStepL( CTestMmfAclntAudioPosition::NewL(_L("MM-MMF-ACLNTOGG-I-1006-LP"),_L("SectionOgg"),position) );
	position = -1000;
	AddTestStepL( CTestMmfAclntAudioPosition::NewL(_L("MM-MMF-ACLNTOGG-I-1007-LP"),_L("SectionOgg"),position) );
	AddTestStepL( CTestMmfAclntDeletePlay::NewL(_L("MM-MMF-ACLNTOGG-I-1008-LP"),_L("SectionOgg") ) );
	AddTestStepL( CTestMmfAclntPlayPlay::NewL(_L("MM-MMF-ACLNTOGG-I-1009-LP"),_L("SectionOggSmall"),_L("playerAudioFile") ) );
	AddTestStepL( new(ELeave) CTestMmfAclntCorruptDes(_L("MM-MMF-ACLNTOGG-I-1010-LP"), _L("SectionOggSmall") ) );
	position = -1;
	AddTestStepL( CTestMmfAclntAudioPosition::NewL(_L("MM-MMF-ACLNTOGG-I-1011-LP"),_L("SectionOgg"),position) );
	
	// NEGATIVE TESTS :: Audio Player (Ogg-Vorbis) APR
	AddTestStepL( CTestMmfAclntARN1301::NewL(_L("MM-MMF-ACLNTOGG-I-1012-LP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN1302::NewL(_L("MM-MMF-ACLNTOGG-I-1013-LP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN1303::NewL(_L("MM-MMF-ACLNTOGG-I-1014-LP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN1304::NewL(_L("MM-MMF-ACLNTOGG-I-1015-LP"),_L("SectionOggAPR") ) );
	
	// NEGATIVE TESTS :: Audio Recorder (Ogg-Vorbis)
	AddTestStepL( new(ELeave) CTestMmfAclntRecDelete(_L("MM-MMF-ACLNTOGG-I-1101-LP"),_L("SectionOggRec") ) );
	AddTestStepL( CTestMmfAclntRecGain::NewL(_L("MM-MMF-ACLNTOGG-I-1102-LP"),_L("SectionOggRec"),1234321, ETrue) );
	AddTestStepL( CTestMmfAclntRecGain::NewL(_L("MM-MMF-ACLNTOGG-I-1103-LP"),_L("SectionOggRec"),-1231, ETrue) );
	position = -10202020;
	AddTestStepL( CTestMmfAclntRecPosition::NewL(_L("MM-MMF-ACLNTOGG-I-1104-LP"),_L("SectionOgg"),position, ETrue) );
	position = 10202020;
	AddTestStepL( CTestMmfAclntRecPosition::NewL(_L("MM-MMF-ACLNTOGG-I-1105-LP"),_L("SectionOgg"),position, ETrue) );
	AddTestStepL( CTestMmfAclntRecBalance::NewL(_L("MM-MMF-ACLNTOGG-I-1106-LP"),_L("SectionOgg"),-200, ETrue) );
	AddTestStepL( CTestMmfAclntRecBalance::NewL(_L("MM-MMF-ACLNTOGG-I-1107-LP"),_L("SectionOgg"),200, ETrue) );
	AddTestStepL( CTestMmfAclntRecLength::NewL(_L("MM-MMF-ACLNTOGG-I-1108-LP"),_L("SectionOgg"), ETrue) );
		
	// NEGATIVE TESTS :: Audio Recorder (Ogg-Vorbis) APR
	AddTestStepL( CTestMmfAclntARN1311::NewL(_L("MM-MMF-ACLNTOGG-I-1109-LP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN1312::NewL(_L("MM-MMF-ACLNTOGG-I-1110-LP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN1313::NewL(_L("MM-MMF-ACLNTOGG-I-1111-LP"),_L("SectionOggAPR") ) );
	AddTestStepL( CTestMmfAclntARN1314::NewL(_L("MM-MMF-ACLNTOGG-I-1112-LP"),_L("SectionOggAPR") ) );

#endif	
	//CR1424 tests(AudioController)
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatPlayPausePlay::NewL(_L("MM-MMF-ACLNT-I-0070-HP"),_L("SectionOne"),_L("playerAudioFile") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatPlayPauseRepeatPlay::NewL(_L("MM-MMF-ACLNT-I-0071-HP"),_L("SectionOne"),_L("playerAudioFile") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayPauseRepeatPlay::NewL(_L("MM-MMF-ACLNT-I-0072-HP"),_L("SectionOne"),_L("playerAudioFile") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayRepeat::NewL(_L("MM-MMF-ACLNT-I-0073-HP"),_L("SectionOne"),_L("playerAudioFile") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayRepeatPauseRepeatPlay::NewL(_L("MM-MMF-ACLNT-I-0074-HP"),_L("SectionOne"),_L("playerAudioFile") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatPlayStopPlay::NewL(_L("MM-MMF-ACLNT-I-0075-HP"),_L("SectionOne"),_L("playerAudioFile") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayRepeatRepeatPlay::NewL(_L("MM-MMF-ACLNT-I-0076-HP"),_L("SectionOne") ,_L("playerAudioFile")) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayRepeatRamp::NewL(_L("MM-MMF-ACLNT-I-0077-HP"),_L("SectionOne"),_L("playerAudioFile") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatForeverPlay::NewL(_L("MM-MMF-ACLNT-I-0078-HP"),_L("SectionOne"),_L("playerAudioFile"),zeroSilence ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatForeverPlay::NewL(_L("MM-MMF-ACLNT-I-0079-HP"),_L("SectionOne"),_L("playerAudioFile"),silence ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatBeforeOpen::NewL(_L("MM-MMF-ACLNT-I-0080-HP"),_L("SectionOne"),_L("playerAudioFile") ) );
	position = 0;
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatPlayRepositionPlay::NewL(_L("MM-MMF-ACLNT-I-0081-HP"),_L("SectionOne"),_L("playerAudioFile"), position ) );
	AddTestStepL( CAsyncTestMmfAclntRepeatMultipleFiles::NewL(_L("MM-MMF-ACLNT-I-0082-HP"),_L("SectionOne")) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatForeverPlay::NewL(_L("MM-MMF-ACLNT-I-0083-HP"),_L("SectionOne"),_L("playerAudioFile"),maxSilence ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatPlayGetPosition::NewL(_L("MM-MMF-ACLNT-I-0084-HP"),_L("SectionOne"),_L("playerAudioFile") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioSetPlayWindowRepeatPlay::NewL(_L("MM-MMF-ACLNT-I-0087-HP"),_L("SectionOne"),_L("playerAudioFile")));
		// reposition to 1.5 sec frm start of clip
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatPlayRepositionPlay::NewL(_L("MM-MMF-ACLNT-I-0088-HP"),_L("SectionOne"),_L("playerAudioFile"), nonZeroPosition ) );
	//Negative tests
	AddTestStepL( CAsyncTestMmfAclntAudioNegativeRepeatPlay::NewL(_L("MM-MMF-ACLNT-I-0085-HP"),_L("SectionOne") ,_L("playerAudioFile")) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayNegativeRepeat::NewL(_L("MM-MMF-ACLNT-I-0086-HP"),_L("SectionOne"),_L("playerAudioFile") ) );
	//Test for DRM loop playback without rights expire
	AddTestStepL( CAsyncTestMmfAclntDRMRepeatPlay::NewL(_L("MM-MMF-ACLNT-I-9023-HP"),_L("SectionDRM"),_L("AudioContent"),_L("AudioTestmainTst3Id003"), lessRepeatCount) );
	//Test for DRM loop playback with rights expire
	AddTestStepL( CAsyncTestMmfAclntDRMRepeatPlay::NewL(_L("MM-MMF-ACLNT-I-9024-HP"),_L("SectionDRM"),_L("AudioContent"),_L("AudioTestmainTst3Id003"), moreRepeatCount ) );
	
	// Test API calls for CMdaAudioPlayerUtility for testing repeats for .sqn file(ToneController)
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatPlayPausePlay::NewL(_L("MM-MMF-ACLNT-I-0140-HP"),_L("SectionOne"),_L("Sequence1") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatPlayPauseRepeatPlay::NewL(_L("MM-MMF-ACLNT-I-0141-HP"),_L("SectionOne"),_L("Sequence2") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayPauseRepeatPlay::NewL(_L("MM-MMF-ACLNT-I-0142-HP"),_L("SectionOne"),_L("Sequence2") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayRepeat::NewL(_L("MM-MMF-ACLNT-I-0143-HP"),_L("SectionOne"),_L("Sequence2") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayRepeatPauseRepeatPlay::NewL(_L("MM-MMF-ACLNT-I-0144-HP"),_L("SectionOne"),_L("Sequence2") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatPlayStopPlay::NewL(_L("MM-MMF-ACLNT-I-0145-HP"),_L("SectionOne"),_L("Sequence2") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayRepeatRepeatPlay::NewL(_L("MM-MMF-ACLNT-I-0146-HP"),_L("SectionOne") ,_L("Sequence2")) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayRepeatRamp::NewL(_L("MM-MMF-ACLNT-I-0147-HP"),_L("SectionOne"),_L("Sequence2") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioNegativeRepeatPlay::NewL(_L("MM-MMF-ACLNT-I-0148-HP"),_L("SectionOne") ,_L("Sequence2")) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayNegativeRepeat::NewL(_L("MM-MMF-ACLNT-I-0149-HP"),_L("SectionOne"),_L("Sequence2") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatForeverPlay::NewL(_L("MM-MMF-ACLNT-I-0150-HP"),_L("SectionOne"),_L("Sequence2"),zeroSilence ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatForeverPlay::NewL(_L("MM-MMF-ACLNT-I-0151-HP"),_L("SectionOne"),_L("Sequence2"),silence ) );
	
	//Alloc test
	AddTestStepL( CTestMmfAclntRepeatPlayAlloc::NewL(_L("MM-MMF-ACLNT-I-7807-HP"),_L("SectionOne"),_L("PCM16")) );
	//OggController
#ifdef SYMBIAN_VARIABLE_BITRATE_CODEC
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatPlayPausePlay::NewL(_L("MM-MMF-ACLNTOGG-I-0030-HP"),_L("SectionOgg"), _L("playerAudioFile") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatPlayPauseRepeatPlay::NewL(_L("MM-MMF-ACLNTOGG-I-0031-HP"),_L("SectionOgg"), _L("playerAudioFile")  ) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayPauseRepeatPlay::NewL(_L("MM-MMF-ACLNTOGG-I-0032-HP"),_L("SectionOgg"), _L("playerAudioFile")  ) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayRepeat::NewL(_L("MM-MMF-ACLNTOGG-I-0033-HP"),_L("SectionOgg"), _L("playerAudioFile")  ) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayRepeatPauseRepeatPlay::NewL(_L("MM-MMF-ACLNTOGG-I-0034-HP"),_L("SectionOgg") , _L("playerAudioFile") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatPlayStopPlay::NewL(_L("MM-MMF-ACLNTOGG-I-0035-HP"),_L("SectionOgg"), _L("playerAudioFile")  ) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayRepeatRepeatPlay::NewL(_L("MM-MMF-ACLNTOGG-I-0036-HP"),_L("SectionOgg"), _L("playerAudioFile")  ) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayRepeatRamp::NewL(_L("MM-MMF-ACLNTOGG-I-0037-HP"),_L("SectionOgg"), _L("playerAudioFile")  ) );
	AddTestStepL( CAsyncTestMmfAclntAudioNegativeRepeatPlay::NewL(_L("MM-MMF-ACLNTOGG-I-0038-HP"),_L("SectionOgg"), _L("playerAudioFile")  ) );
	AddTestStepL( CAsyncTestMmfAclntAudioPlayNegativeRepeat::NewL(_L("MM-MMF-ACLNTOGG-I-0039-HP"),_L("SectionOgg"), _L("playerAudioFile") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatForeverPlay::NewL(_L("MM-MMF-ACLNTOGG-I-0040-HP"),_L("SectionOgg"),_L("playerAudioFile"),zeroSilence ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatForeverPlay::NewL(_L("MM-MMF-ACLNTOGG-I-0041-HP"),_L("SectionOgg"),_L("playerAudioFile"),silence ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatBeforeOpen::NewL(_L("MM-MMF-ACLNTOGG-I-0042-HP"),_L("SectionOgg"),_L("playerAudioFile") ) );
	// reposition to start
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatPlayRepositionPlay::NewL(_L("MM-MMF-ACLNTOGG-I-0043-HP"),_L("SectionOgg"),_L("playerAudioFile"), position ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatForeverPlay::NewL(_L("MM-MMF-ACLNTOGG-I-0044-HP"),_L("SectionOgg"),_L("playerAudioFile"),maxSilence ) );
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatPlayGetPosition::NewL(_L("MM-MMF-ACLNTOGG-I-0045-HP"),_L("SectionOgg"),_L("playerAudioFile") ) );
	AddTestStepL( CAsyncTestMmfAclntAudioSetPlayWindowRepeatPlay::NewL(_L("MM-MMF-ACLNTOGG-I-0046-HP"),_L("SectionOgg"),_L("playerAudioFile") ) );
		// reposition to 1.5  sec frm start of clip for ogg file
	nonZeroPosition = 1500000;
	AddTestStepL( CAsyncTestMmfAclntAudioRepeatPlayRepositionPlay::NewL(_L("MM-MMF-ACLNTOGG-I-0047-HP"),_L("SectionOgg"),_L("playerAudioFile"), nonZeroPosition ) );
	AddTestStepL( CTestMmfAclntRepeatPlayAlloc::NewL(_L("MM-MMF-ACLNTOGG-I-0048-HP"),_L("SectionOgg"),_L("playerAudioFile")) );
	//Test for DRM loop playback without rights expire
	AddTestStepL( CAsyncTestMmfAclntDRMRepeatPlay::NewL(_L("MM-MMF-ACLNTOGG-I-0134-HP"),_L("SectionOggDRM"),_L("AudioContent"), _L("AudioOggTestmainTst3Id003"), lessRepeatCount ));
	//Test for DRM loop playback with rights expire
	AddTestStepL( CAsyncTestMmfAclntDRMRepeatPlay::NewL(_L("MM-MMF-ACLNTOGG-I-0135-HP"),_L("SectionOggDRM"),_L("AudioContent"), _L("AudioOggTestmainTst3Id003"), moreRepeatCount ));
#endif

	//TruePause
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	AddTestStepL( RMdaToneUtilityResumeTest::NewL(_L("MM-MMF-ACLNT-I-0168-HP"),_L("SectionTruePause")));
	AddTestStepL( RMdaToneUtilityResumeThreeTest::NewL(_L("MM-MMF-ACLNT-I-0169-HP"),_L("SectionTruePause")));
	AddTestStepL( RMdaToneUtilityPlayPauseStopPlayTest::NewL(_L("MM-MMF-ACLNT-I-0170-HP"),_L("SectionTruePause")));
	AddTestStepL( RMdaToneUtilityConfigResumeTest::NewL(_L("MM-MMF-ACLNT-I-0171-HP"),_L("SectionTruePause")));
	AddTestStepL( RMdaToneUtilityPrepareInPauseTest::NewL(_L("MM-MMF-ACLNT-I-0172-HP"),_L("SectionTruePause")));
	AddTestStepL( RMdaToneUtilityPauseBeforePlayTest::NewL(_L("MM-MMF-ACLNT-I-0173-HP"),_L("SectionTruePause")));
	AddTestStepL( RMdaToneUtilityNegPauseResumeTest::NewL(_L("MM-MMF-ACLNT-I-0174-HP"),_L("SectionTruePause")));
	AddTestStepL( RMdaOutputStreamPauseResumeBeforeWriteTest::NewL(_L("MM-MMF-ACLNT-I-0175-HP"),_L("SectionTruePauseAOS")));
	AddTestStepL( RMdaOutputStreamResumeTest::NewL(_L("MM-MMF-ACLNT-I-0176-HP"),_L("SectionTruePauseAOS")));
	AddTestStepL( RMdaOutputStreamResumeThriceTest::NewL(_L("MM-MMF-ACLNT-I-0177-HP"),_L("SectionTruePauseAOS")));
	AddTestStepL( RMdaOutputStreamConfigInPauseTest::NewL(_L("MM-MMF-ACLNT-I-0178-HP"),_L("SectionTruePauseAOS")));
	AddTestStepL( RMdaOutputStreamInvalidConfigInPauseTest::NewL(_L("MM-MMF-ACLNT-I-0179-HP"),_L("SectionTruePauseAOS")));
	AddTestStepL( RMdaOutputStreamGetBytesInPauseTest::NewL(_L("MM-MMF-ACLNT-I-0180-HP"),_L("SectionTruePauseAOS")));
	AddTestStepL( RMdaOutputStreamPlayPauseStopPlayTest::NewL(_L("MM-MMF-ACLNT-I-0181-HP"),_L("SectionTruePauseAOS")));
	AddTestStepL( RMdaOutputStreamInvalidFormatsTest::NewL(_L("MM-MMF-ACLNT-I-0182-HP"),_L("SectionTruePauseAOS")));
	AddTestStepL(RMdaPlayerUtiliyPauseStopAndPlayWavTest::NewL(_L("MM-MMF-ACLNT-I-0185-HP"), _L("SectionPlayTrue")));
	AddTestStepL(RMdaPlayerUtilityQueryPauseAndPlayWavTest::NewL(_L("MM-MMF-ACLNT-I-0186-HP"), _L("SectionPlayTrue")));
	AddTestStepL(RMdaPlayerUtilityPauseAndPlaySqnTest::NewL(_L("MM-MMF-ACLNT-I-0187-HP") ,_L("SectionPlayTrue")));
	AddTestStepL(RMdaPlayerUtilityPauseAndPlayThreeTimesSqnTest::NewL(_L("MM-MMF-ACLNT-I-0188-HP") ,_L("SectionPlayTrue")));
	AddTestStepL(RMdaPlayerUtilityPauseStopAndPlaySqnTest::NewL(_L("MM-MMF-ACLNT-I-0189-HP") ,_L("SectionPlayTrue")));
	AddTestStepL(RMdaPlayerUtilityGetVolumeAndBalancePauseAndPlaySqnTest::NewL(_L("MM-MMF-ACLNT-I-0190-HP") ,_L("SectionPlayTrue")));
	AddTestStepL(RMdaPlayerUtilityPauseAndPlayFormatsTest::NewL(_L("MM-MMF-ACLNT-I-0191-HP") ,_L("SectionPlayTrue")));
#else
	AddTestStepL( RMdaToneUtilityPauseNonA3fTest::NewL(_L("MM-MMF-ACLNT-I-0183-HP"),_L("SectionTruePause")));
	AddTestStepL( RMdaOutputStreamPauseNonA3FTest::NewL(_L("MM-MMF-ACLNT-I-0184-HP"),_L("SectionTruePauseAOS")));
#endif

	}

