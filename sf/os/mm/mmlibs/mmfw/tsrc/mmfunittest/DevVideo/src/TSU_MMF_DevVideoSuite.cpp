
// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <ecom/ecom.h>
// Test system includes
#include <testframework.h>

#include "TSU_MMF_DevVideo.h"
#include "TSU_MMF_DevVideoSuite.h"
#include "TestDevVideoPlay.h"
#include "TestDevVideoRecord.h"
#include "TestStepDevVideoAllocFailNew.h"
#include "test_devvideo_general.h"		    

/**
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuite* NewTestSuiteL()
    {
	CTestMmfDevVideoSuite* result = new (ELeave) CTestMmfDevVideoSuite;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

/**
 * destructor
 */
CTestMmfDevVideoSuite::~CTestMmfDevVideoSuite()
	{
	}

/**
 * make a version string available for test system
 * @return version string
 */
TPtrC CTestMmfDevVideoSuite::GetVersion()
	{
	_LIT(KTxtVersion,"1.0");
	return KTxtVersion();
	}

/**
 * Constructor for test suite
 * this creates all the test steps and 
 * stores them inside CTestMmfDevVideoSuite 
 */
void CTestMmfDevVideoSuite::InitialiseL()
	{
	// store the name of this test suite 
	iSuiteName = _L("MMF_DEVVIDEO");

// POSITIVE TEST

	// DevVideoPlay
	AddTestStepL( CTestDevVideoPlayCommonFormat::NewL( _L("MM-MMF-DEVVIDEO-U-0001-HP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayFindDecoders::NewL( _L("MM-MMF-DEVVIDEO-U-0002-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayFindPostProc::NewL( _L("MM-MMF-DEVVIDEO-U-0003-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayListDecoders::NewL( _L("MM-MMF-DEVVIDEO-U-0004-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayListPostProc::NewL( _L("MM-MMF-DEVVIDEO-U-0005-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayGetDecoderInfo::NewL( _L("MM-MMF-DEVVIDEO-U-0006-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayGetPostProcInfo::NewL( _L("MM-MMF-DEVVIDEO-U-0007-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySelectDecoder::NewL( _L("MM-MMF-DEVVIDEO-U-0008-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySelectPostProc::NewL( _L("MM-MMF-DEVVIDEO-U-0009-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayGetHeaderInfo::NewL( _L("MM-MMF-DEVVIDEO-U-0010-HP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayReturnHeader::NewL( _L("MM-MMF-DEVVIDEO-U-0011-HP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetInputFormatUC::NewL( _L("MM-MMF-DEVVIDEO-U-0012-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetInputFormat::NewL( _L("MM-MMF-DEVVIDEO-U-0013-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayGetOutputFormatList::NewL( _L("MM-MMF-DEVVIDEO-U-0014-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetOutputFormat::NewL( _L("MM-MMF-DEVVIDEO-U-0015-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetPostProcTypes::NewL( _L("MM-MMF-DEVVIDEO-U-0016-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetInputCrop::NewL( _L("MM-MMF-DEVVIDEO-U-0017-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetYuvToRgbOptionsFormat::NewL( _L("MM-MMF-DEVVIDEO-U-0018-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetYuvToRgbOptions::NewL( _L("MM-MMF-DEVVIDEO-U-0019-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetRotate::NewL( _L("MM-MMF-DEVVIDEO-U-0020-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetScale::NewL( _L("MM-MMF-DEVVIDEO-U-0021-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetCrop::NewL( _L("MM-MMF-DEVVIDEO-U-0022-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetPostProcOptions::NewL( _L("MM-MMF-DEVVIDEO-U-0023-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetClockSource::NewL( _L("MM-MMF-DEVVIDEO-U-0024-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetVideoOutput::NewL( _L("MM-MMF-DEVVIDEO-U-0025-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySynchronizeDecoding::NewL( _L("MM-MMF-DEVVIDEO-U-0026-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetBufferOptions::NewL( _L("MM-MMF-DEVVIDEO-U-0027-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayGetBufferOptions::NewL( _L("MM-MMF-DEVVIDEO-U-0028-HP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetHrdVbv::NewL( _L("MM-MMF-DEVVIDEO-U-0029-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayInit::NewL( _L("MM-MMF-DEVVIDEO-U-0030-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayStartDSA::NewL( _L("MM-MMF-DEVVIDEO-U-0031-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetClipRegion::NewL( _L("MM-MMF-DEVVIDEO-U-0032-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetPauseClipFail::NewL( _L("MM-MMF-DEVVIDEO-U-0033-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayAbortDSA::NewL( _L("MM-MMF-DEVVIDEO-U-0034-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayIsPlaying::NewL( _L("MM-MMF-DEVVIDEO-U-0035-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayRedraw::NewL( _L("MM-MMF-DEVVIDEO-U-0036-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayStart::NewL( _L("MM-MMF-DEVVIDEO-U-0037-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayStop::NewL( _L("MM-MMF-DEVVIDEO-U-0038-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayPause::NewL( _L("MM-MMF-DEVVIDEO-U-0039-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayResume::NewL( _L("MM-MMF-DEVVIDEO-U-0040-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetPosition::NewL( _L("MM-MMF-DEVVIDEO-U-0041-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayFreezePicture::NewL( _L("MM-MMF-DEVVIDEO-U-0042-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayReleaseFreeze::NewL( _L("MM-MMF-DEVVIDEO-U-0043-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayDecodingPosition::NewL( _L("MM-MMF-DEVVIDEO-U-0044-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayPlaybackPosition::NewL( _L("MM-MMF-DEVVIDEO-U-0045-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayPreDecoderBufferBytes::NewL( _L("MM-MMF-DEVVIDEO-U-0046-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayPictureBufferBytes::NewL( _L("MM-MMF-DEVVIDEO-U-0047-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayGetPictureCounters::NewL( _L("MM-MMF-DEVVIDEO-U-0048-HP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayGetBitstreamCounters::NewL( _L("MM-MMF-DEVVIDEO-U-0049-HP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayNumFreeBuffers::NewL( _L("MM-MMF-DEVVIDEO-U-0050-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlaySetComplexityLevel::NewL( _L("MM-MMF-DEVVIDEO-U-0051-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayNumComplexityLevels::NewL( _L("MM-MMF-DEVVIDEO-U-0052-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayGetComplexityLevelInfo::NewL( _L("MM-MMF-DEVVIDEO-U-0053-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayGetBuffer::NewL( _L("MM-MMF-DEVVIDEO-U-0054-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayWriteCodedData::NewL( _L("MM-MMF-DEVVIDEO-U-0055-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayWritePicture::NewL( _L("MM-MMF-DEVVIDEO-U-0056-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayInputEnd::NewL( _L("MM-MMF-DEVVIDEO-U-0057-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayGetNewPictureInfo::NewL( _L("MM-MMF-DEVVIDEO-U-0058-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayNextPicture::NewL( _L("MM-MMF-DEVVIDEO-U-0059-CP"), ETestValid));
/*	MM-MMF-DEVVIDEO-U-0060-CP - removed from test spec due to duplication of MM-MMF-DEVVIDEO-U-0059-CP*/
	AddTestStepL( CTestDevVideoPlayGetSnapshot::NewL( _L("MM-MMF-DEVVIDEO-U-0061-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayGetTimedSnapshotTimestamp::NewL( _L("MM-MMF-DEVVIDEO-U-0062-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayGetTimedSnapshotID::NewL( _L("MM-MMF-DEVVIDEO-U-0063-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayCancelTimedSnapshot::NewL( _L("MM-MMF-DEVVIDEO-U-0064-LP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayGetSupportedSnapshotFormats::NewL( _L("MM-MMF-DEVVIDEO-U-0065-HP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayClock::NewL( _L("MM-MMF-DEVVIDEO-U-0066-HP")));
	AddTestStepL( CTestDevVideoPlayCommit::NewL( _L("MM-MMF-DEVVIDEO-U-0067-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayRevert::NewL( _L("MM-MMF-DEVVIDEO-U-0068-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayCustomInterface::NewL( _L("MM-MMF-DEVVIDEO-U-0069-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayScanAndCopyCodedData::NewL( _L("MM-MMF-DEVVIDEO-U-0070-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayConfigureDecoder::NewL( _L("MM-MMF-DEVVIDEO-U-0071-CP"), ETestValid));
	AddTestStepL( CTestDevVideoPlayExtensionWriteCodedData::NewL( _L("MM-MMF-DEVVIDEO-U-0072-CP"), ETestValid));
    AddTestStepL( CTestDevVideoGetBufferWithoutInitialize::NewL( _L("MM-MMF-DEVVIDEO-U-0073-CP"), ETestValid));
    // Fix for INC093434
    AddTestStepL( CTestDevVideoPlayGetDecoderPostProcInfo::NewL(_L("MM-MMF-DEVVIDEO-U-0074-CP"), ETestValid));
    
	// DevVideoRecord
	AddTestStepL( CTestDevVideoRecordFindEncoders::NewL( _L("MM-MMF-DEVVIDEO-U-0200-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordFindPreProc::NewL( _L("MM-MMF-DEVVIDEO-U-0201-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordListEncoders::NewL( _L("MM-MMF-DEVVIDEO-U-0202-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordListPreProc::NewL( _L("MM-MMF-DEVVIDEO-U-0203-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordGetEncoderInfo::NewL( _L("MM-MMF-DEVVIDEO-U-0204-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordGetPreProcInfo::NewL( _L("MM-MMF-DEVVIDEO-U-0205-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSelectEncoder::NewL( _L("MM-MMF-DEVVIDEO-U-0206-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSelectPreProc::NewL( _L("MM-MMF-DEVVIDEO-U-0207-CP"), ETestValid)); 
	AddTestStepL( CTestDevVideoRecordSetInputFormat::NewL( _L("MM-MMF-DEVVIDEO-U-0208-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetSourceCam::NewL( _L("MM-MMF-DEVVIDEO-U-0209-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetSourceMem::NewL( _L("MM-MMF-DEVVIDEO-U-0210-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetOutputFormat::NewL( _L("MM-MMF-DEVVIDEO-U-0211-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetOutputFormatUC::NewL( _L("MM-MMF-DEVVIDEO-U-0212-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetOutputRect::NewL( _L("MM-MMF-DEVVIDEO-U-0213-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetClockSource::NewL( _L("MM-MMF-DEVVIDEO-U-0214-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetPreProcTypes::NewL( _L("MM-MMF-DEVVIDEO-U-0215-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetRgbToYuvOptions::NewL( _L("MM-MMF-DEVVIDEO-U-0216-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetYuvToYuvOptions::NewL( _L("MM-MMF-DEVVIDEO-U-0217-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetRotate::NewL( _L("MM-MMF-DEVVIDEO-U-0218-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetScale::NewL( _L("MM-MMF-DEVVIDEO-U-0219-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetInputCrop::NewL( _L("MM-MMF-DEVVIDEO-U-0220-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetOutputCrop::NewL( _L("MM-MMF-DEVVIDEO-U-0221-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetOutputPad::NewL( _L("MM-MMF-DEVVIDEO-U-0222-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetColorEnhance::NewL( _L("MM-MMF-DEVVIDEO-U-0223-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetFrameStabilisation::NewL( _L("MM-MMF-DEVVIDEO-U-0224-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetCustomISPreProc::NewL( _L("MM-MMF-DEVVIDEO-U-0225-LP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetErrors::NewL( _L("MM-MMF-DEVVIDEO-U-0226-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetMinRandomAccess::NewL( _L("MM-MMF-DEVVIDEO-U-0227-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetNumBitrateLayers::NewL( _L("MM-MMF-DEVVIDEO-U-0228-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetScalabilityType::NewL( _L("MM-MMF-DEVVIDEO-U-0229-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetGlobalReferencePic::NewL( _L("MM-MMF-DEVVIDEO-U-0230-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetLayerReference::NewL( _L("MM-MMF-DEVVIDEO-U-0231-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetBufferOptions::NewL( _L("MM-MMF-DEVVIDEO-U-0232-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetCSEncoderOptions::NewL( _L("MM-MMF-DEVVIDEO-U-0233-LP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetISEncoderOptions::NewL( _L("MM-MMF-DEVVIDEO-U-0234-LP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordInit::NewL( _L("MM-MMF-DEVVIDEO-U-0235-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordGetCSInitOutput::NewL( _L("MM-MMF-DEVVIDEO-U-0236-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordGetISInitOutput::NewL( _L("MM-MMF-DEVVIDEO-U-0237-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetErrorProtectionLevels::NewL( _L("MM-MMF-DEVVIDEO-U-0238-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetupErrorProtection::NewL( _L("MM-MMF-DEVVIDEO-U-0239-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetChannelBufferLossRate::NewL( _L("MM-MMF-DEVVIDEO-U-0240-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetChannelBitErrorRate::NewL( _L("MM-MMF-DEVVIDEO-U-0241-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetSegTargetSize::NewL( _L("MM-MMF-DEVVIDEO-U-0242-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetRateControl::NewL( _L("MM-MMF-DEVVIDEO-U-0243-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetInLayerScalability::NewL( _L("MM-MMF-DEVVIDEO-U-0244-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetLayerPromPointPeriod::NewL( _L("MM-MMF-DEVVIDEO-U-0245-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordCSSettingsOutput::NewL( _L("MM-MMF-DEVVIDEO-U-0246-LP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordISSettingsOutput::NewL( _L("MM-MMF-DEVVIDEO-U-0247-LP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordWritePicture::NewL( _L("MM-MMF-DEVVIDEO-U-0248-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSendSupplementalInfo::NewL( _L("MM-MMF-DEVVIDEO-U-0249-LP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSendSupplementalInfoTime::NewL( _L("MM-MMF-DEVVIDEO-U-0250-LP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordCancelSupplementalInfo::NewL( _L("MM-MMF-DEVVIDEO-U-0251-LP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordInputEnd::NewL( _L("MM-MMF-DEVVIDEO-U-0252-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordStart::NewL( _L("MM-MMF-DEVVIDEO-U-0253-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordStop::NewL( _L("MM-MMF-DEVVIDEO-U-0254-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordPause::NewL( _L("MM-MMF-DEVVIDEO-U-0255-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordResume::NewL( _L("MM-MMF-DEVVIDEO-U-0256-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordFreezePicture::NewL( _L("MM-MMF-DEVVIDEO-U-0257-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordReleaseFreeze::NewL( _L("MM-MMF-DEVVIDEO-U-0258-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordRecordingPosition::NewL( _L("MM-MMF-DEVVIDEO-U-0259-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordGetOutputBufferStatus::NewL( _L("MM-MMF-DEVVIDEO-U-0260-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordGetPictureCounters::NewL( _L("MM-MMF-DEVVIDEO-U-0261-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordGetFrameStabilisation::NewL( _L("MM-MMF-DEVVIDEO-U-0262-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordNumDataBuffers::NewL( _L("MM-MMF-DEVVIDEO-U-0263-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordNextBuffer::NewL( _L("MM-MMF-DEVVIDEO-U-0264-CP"), ETestValid));
	/*	MM-MMF-DEVVIDEO-U-0265-HP - removed from test spec due to duplication of MM-MMF-DEVVIDEO-U-0264-CP */
	AddTestStepL( CTestDevVideoRecordPictureLoss::NewL( _L("MM-MMF-DEVVIDEO-U-0266-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordPictureLossSpec::NewL( _L("MM-MMF-DEVVIDEO-U-0267-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSliceLoss::NewL( _L("MM-MMF-DEVVIDEO-U-0268-LP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSendRefPictureSelection::NewL( _L("MM-MMF-DEVVIDEO-U-0269-LP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordNumComplexityLevels::NewL( _L("MM-MMF-DEVVIDEO-U-0270-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordSetComplexityLevel::NewL( _L("MM-MMF-DEVVIDEO-U-0271-HP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordCommit::NewL( _L("MM-MMF-DEVVIDEO-U-0272-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordRevert::NewL( _L("MM-MMF-DEVVIDEO-U-0273-CP"), ETestValid));
	AddTestStepL( CTestDevVideoRecordCustomInterface::NewL( _L("MM-MMF-DEVVIDEO-U-0274-CP"), ETestValid));

// NEGATIVE TEST
	
	// DevVideoPlay
	AddTestStepL( CTestDevVideoPlayGetPictureCounters::NewL( _L("MM-MMF-DEVVIDEO-U-1004-LP"), ETestDecoderOnly));
	AddTestStepL( CTestDevVideoPlayGetPictureCounters::NewL( _L("MM-MMF-DEVVIDEO-U-1005-LP"), ETestPostProcOnly));
	AddTestStepL( CTestDevVideoPlayInputEnd::NewL( _L("MM-MMF-DEVVIDEO-U-1006-LP"), ETestDecoderOnly));
	AddTestStepL( CTestDevVideoPlayInputEnd::NewL( _L("MM-MMF-DEVVIDEO-U-1007-LP"), ETestPostProcOnly));
	AddTestStepL( CTestDevVideoPlayStart::NewL( _L("MM-MMF-DEVVIDEO-U-1008-LP"), ETestDecoderOnly));
	AddTestStepL( CTestDevVideoPlayStart::NewL( _L("MM-MMF-DEVVIDEO-U-1009-LP"), ETestPostProcOnly));
	AddTestStepL( CTestDevVideoPlayStop::NewL( _L("MM-MMF-DEVVIDEO-U-1010-LP"), ETestDecoderOnly));
	AddTestStepL( CTestDevVideoPlayStop::NewL( _L("MM-MMF-DEVVIDEO-U-1011-LP"), ETestPostProcOnly));
	AddTestStepL( CTestDevVideoPlayPause::NewL( _L("MM-MMF-DEVVIDEO-U-1012-LP"), ETestDecoderOnly));
	AddTestStepL( CTestDevVideoPlayPause::NewL( _L("MM-MMF-DEVVIDEO-U-1013-LP"), ETestPostProcOnly));
	AddTestStepL( CTestDevVideoPlayResume::NewL( _L("MM-MMF-DEVVIDEO-U-1014-LP"), ETestDecoderOnly));
	AddTestStepL( CTestDevVideoPlayResume::NewL( _L("MM-MMF-DEVVIDEO-U-1015-LP"), ETestPostProcOnly));
	AddTestStepL( CTestDevVideoPlaySetClockSource::NewL( _L("MM-MMF-DEVVIDEO-U-1016-LP"), ETestDecoderOnly));
	AddTestStepL( CTestDevVideoPlaySetClockSource::NewL( _L("MM-MMF-DEVVIDEO-U-1017-LP"), ETestPostProcOnly));
	AddTestStepL( CTestDevVideoPlaySetPosition::NewL( _L("MM-MMF-DEVVIDEO-U-1018-LP"), ETestDecoderOnly));
	AddTestStepL( CTestDevVideoPlaySetPosition::NewL( _L("MM-MMF-DEVVIDEO-U-1019-LP"), ETestPostProcOnly));
	AddTestStepL( CTestDevVideoPlaySetPosition::NewL( _L("MM-MMF-DEVVIDEO-U-1020-LP"), ETestDecoderFatal));
	AddTestStepL( CTestDevVideoPlaySetPosition::NewL( _L("MM-MMF-DEVVIDEO-U-1021-LP"), ETestPostProcFatal));
	AddTestStepL( CTestDevVideoPlaySetPosition::NewL( _L("MM-MMF-DEVVIDEO-U-1022-LP"), ETestBothFatal));
	AddTestStepL( CTestDevVideoPlayCommit::NewL( _L("MM-MMF-DEVVIDEO-U-1023-LP"), ETestDecoderOnly));
	AddTestStepL( CTestDevVideoPlayCommit::NewL( _L("MM-MMF-DEVVIDEO-U-1024-LP"), ETestPostProcOnly));
	AddTestStepL( CTestDevVideoPlayRevert::NewL( _L("MM-MMF-DEVVIDEO-U-1025-LP"), ETestDecoderOnly));
	AddTestStepL( CTestDevVideoPlayRevert::NewL( _L("MM-MMF-DEVVIDEO-U-1026-LP"), ETestPostProcOnly));

	// DevVideoRecord
	AddTestStepL( CTestDevVideoRecordGetPictureCounters::NewL( _L("MM-MMF-DEVVIDEO-U-1304-LP"), ETestEncoderOnly));
	AddTestStepL( CTestDevVideoRecordGetPictureCounters::NewL( _L("MM-MMF-DEVVIDEO-U-1305-LP"), ETestPreProcOnly));
	AddTestStepL( CTestDevVideoRecordInputEnd::NewL( _L("MM-MMF-DEVVIDEO-U-1306-LP"), ETestEncoderOnly));
	AddTestStepL( CTestDevVideoRecordInputEnd::NewL( _L("MM-MMF-DEVVIDEO-U-1307-LP"), ETestPreProcOnly));
	AddTestStepL( CTestDevVideoRecordStart::NewL( _L("MM-MMF-DEVVIDEO-U-1308-LP"), ETestEncoderOnly));
	AddTestStepL( CTestDevVideoRecordStart::NewL( _L("MM-MMF-DEVVIDEO-U-1309-LP"), ETestPreProcOnly));
	AddTestStepL( CTestDevVideoRecordStop::NewL( _L("MM-MMF-DEVVIDEO-U-1310-LP"), ETestEncoderOnly));
	AddTestStepL( CTestDevVideoRecordStop::NewL( _L("MM-MMF-DEVVIDEO-U-1311-LP"), ETestPreProcOnly));
	AddTestStepL( CTestDevVideoRecordPause::NewL( _L("MM-MMF-DEVVIDEO-U-1312-LP"), ETestEncoderOnly));
	AddTestStepL( CTestDevVideoRecordPause::NewL( _L("MM-MMF-DEVVIDEO-U-1313-LP"), ETestPreProcOnly));
	AddTestStepL( CTestDevVideoRecordResume::NewL( _L("MM-MMF-DEVVIDEO-U-1314-LP"), ETestEncoderOnly));
	AddTestStepL( CTestDevVideoRecordResume::NewL( _L("MM-MMF-DEVVIDEO-U-1315-LP"), ETestPreProcOnly));
	AddTestStepL( CTestDevVideoRecordSetClockSource::NewL( _L("MM-MMF-DEVVIDEO-U-1316-LP"), ETestEncoderOnly));
	AddTestStepL( CTestDevVideoRecordSetClockSource::NewL( _L("MM-MMF-DEVVIDEO-U-1317-LP"), ETestPreProcOnly));
	AddTestStepL( CTestDevVideoRecordFreezePicture::NewL( _L("MM-MMF-DEVVIDEO-U-1318-LP"), ETestEncoderOnly));
	AddTestStepL( CTestDevVideoRecordFreezePicture::NewL( _L("MM-MMF-DEVVIDEO-U-1319-LP"), ETestPreProcOnly));
	AddTestStepL( CTestDevVideoRecordReleaseFreeze::NewL( _L("MM-MMF-DEVVIDEO-U-1320-LP"), ETestEncoderOnly));
	AddTestStepL( CTestDevVideoRecordReleaseFreeze::NewL( _L("MM-MMF-DEVVIDEO-U-1321-LP"), ETestPreProcOnly));
	AddTestStepL( CTestDevVideoRecordSetSourceCam::NewL( _L("MM-MMF-DEVVIDEO-U-1322-LP"), ETestEncoderFatal));
	AddTestStepL( CTestDevVideoRecordSetSourceCam::NewL( _L("MM-MMF-DEVVIDEO-U-1323-LP"), ETestPreProcFatal));
	AddTestStepL( CTestDevVideoRecordSetSourceCam::NewL( _L("MM-MMF-DEVVIDEO-U-1324-LP"), ETestBothFatal));
	AddTestStepL( CTestDevVideoRecordCommit::NewL( _L("MM-MMF-DEVVIDEO-U-1325-LP"), ETestEncoderOnly));
	AddTestStepL( CTestDevVideoRecordCommit::NewL( _L("MM-MMF-DEVVIDEO-U-1326-LP"), ETestPreProcOnly));
	AddTestStepL( CTestDevVideoRecordRevert::NewL( _L("MM-MMF-DEVVIDEO-U-1327-LP"), ETestEncoderOnly));
	AddTestStepL( CTestDevVideoRecordRevert::NewL( _L("MM-MMF-DEVVIDEO-U-1328-LP"), ETestPreProcOnly));

// ALLOC TEST
	
	// DevVideoPlay
	AddTestStepL( CTestStepDevVideoPlayAllocFailNew::NewL( _L("MM-MMF-DEVVIDEO-U-0100-CP")));
	AddTestStepL( CTestStepDevVideoPlayAllocFailFindPlugins::NewL( _L("MM-MMF-DEVVIDEO-U-0101-CP")));
	AddTestStepL( CTestStepDevVideoPlayAllocFailInfoDecoder::NewL( _L("MM-MMF-DEVVIDEO-U-0102-CP")));
	AddTestStepL( CTestStepDevVideoPlayAllocFailInfoPostProc::NewL( _L("MM-MMF-DEVVIDEO-U-0103-CP")));
	AddTestStepL( CTestStepDevVideoPlayAllocFailGetPictures::NewL( _L("MM-MMF-DEVVIDEO-U-0104-CP")));

	// DevVideoRecord
	AddTestStepL( CTestStepDevVideoRecordAllocFailNew::NewL( _L("MM-MMF-DEVVIDEO-U-0300-CP")));
	AddTestStepL( CTestStepDevVideoRecordAllocFailFindPlugins::NewL( _L("MM-MMF-DEVVIDEO-U-0301-CP")));
	AddTestStepL( CTestStepDevVideoRecordAllocFailInfoEncoder::NewL( _L("MM-MMF-DEVVIDEO-U-0302-CP")));
	AddTestStepL( CTestStepDevVideoRecordAllocFailInfoPreProc::NewL( _L("MM-MMF-DEVVIDEO-U-0303-CP")));
	AddTestStepL( CTestStepDevVideoRecordAllocFailGetBuffers::NewL( _L("MM-MMF-DEVVIDEO-U-0304-CP")));
	
	//INC103980
	AddTestStepL(RDevVideoInfoTest::NewL(_L("MM-MMF-DEVVIDEO-U-1329-HP")));
	}


