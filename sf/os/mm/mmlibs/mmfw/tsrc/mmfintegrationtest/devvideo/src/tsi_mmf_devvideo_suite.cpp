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
// This file contains test suite class implementation for 
// DevVideo and Video Codec interface definition tests
// 
//


// Test includes
#include "tsi_mmf_devvideo_stepbase.h"
#include "tsi_mmf_devvideo_suite.h"
#include "tsi_mmf_devvideo_record.h"
#include "tsi_mmf_devvideo_play.h"

/**
 *
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 *
 */
EXPORT_C CTestSuite* NewTestSuiteL()
    {
    CTestSuiteDevVideo* result = new (ELeave) CTestSuiteDevVideo;
    CleanupStack::PushL(result);
    result->ConstructL();
    CleanupStack::Pop(result);
    return result;
    }

/**
 *
 * Destructor
 *
 */
CTestSuiteDevVideo::~CTestSuiteDevVideo()
    {
    }

/**
 *
 * Get test suite version
 *
 */
TPtrC CTestSuiteDevVideo::GetVersion() const
    {
    _LIT(KTxtVersion, "1.0");
    return KTxtVersion();
    }

/**
 *
 * Add a test step into the suite
 *
 */
void CTestSuiteDevVideo::AddTestStepL(RTestStepDevVideoBase* aPtrTestStep)
    {
    // Test steps contain a pointer back to the suite which owns them
    aPtrTestStep->SetTestSuite(this);

    // Add the step using the base class method
    CTestSuite::AddTestStepL(aPtrTestStep);
    }

/**
 *
 * Constructor for test suite
 * Creates all the test steps and stores them inside CTestSuiteDevVideo
 *
 */
void CTestSuiteDevVideo::InitialiseL()
    {
    // Store the name of this test suite
    iSuiteName = _L("TSI_MMF_DEVVIDEO");

    //
    // DevVideo Common TESTS //
    //
    
    // DevVideoPlay
    AddTestStepL(RTestDevVideoListVideoDecoders::NewL(_L("MM-MMF-DEVVIDEO-I-0001-HP"), _L("XviDCodecPU"), _L("PlayFile"), KUidDevVideoTestDecodeHwDevice1));
    AddTestStepL(RTestDevVideoListVideoPostProcessors::NewL(_L("MM-MMF-DEVVIDEO-I-0002-HP"), _L("XviDCodecPU"), _L("PlayFile"), KUidTestVideoDecodeHwDeviceAdapter));
    // DevVideoRecord
    AddTestStepL(RTestDevVideoListVideoEncoders::NewL(_L("MM-MMF-DEVVIDEO-I-0003-HP"), _L("XviDCodecPU"), _L("RecordFile"), KUidDevVideoTestEncodeHwDevice1));
    AddTestStepL(RTestDevVideoListVideoPreProcessors::NewL(_L("MM-MMF-DEVVIDEO-I-0004-HP"), _L("XviDCodecPU"), _L("RecordFile"), KUidTestVideoEncodeHwDeviceAdapter));

    //
    // XviD PU TESTS //
    //
    
    // POSITIVE TESTS //

    // DevVideoPlay
    AddTestStepL(RTestDevVideoPlaySimple::NewL(_L("MM-MMF-DEVVIDEO-I-1001-HP"), _L("XviDCodecPU"), _L("PlayFile"), KUidTestVideoPuXvidDecoderDevice));
    AddTestStepL(RTestDevVideoPlayConfHwDev::NewL(_L("MM-MMF-DEVVIDEO-I-1002-HP"), _L("XviDCodecPU"), _L("PlayFile"), KUidTestVideoPuXvidDecoderDevice));
    AddTestStepL(RTestDevVideoPlayStop::NewL(_L("MM-MMF-DEVVIDEO-I-1003-HP"), _L("XviDCodecPU"), _L("PlayFile"), KUidTestVideoPuXvidDecoderDevice));
    AddTestStepL(RTestDevVideoPlayPause::NewL(_L("MM-MMF-DEVVIDEO-I-1004-HP"), _L("XviDCodecPU"), _L("PlayFile"), KUidTestVideoPuXvidDecoderDevice));
    AddTestStepL(RTestDevVideoPlayDecodePlaybackPos::NewL(_L("MM-MMF-DEVVIDEO-I-1005-HP"), _L("XviDCodecPU"), _L("PlayFile"), KUidTestVideoPuXvidDecoderDevice));
    AddTestStepL(RTestDevVideoPlayDecoderInfo::NewL(_L("MM-MMF-DEVVIDEO-I-1006-HP"), _L("XviDCodecPU"), _L("PlayFile"), KUidTestVideoPuXvidDecoderDevice));
    AddTestStepL(RTestDevVideoPlayAlloc::NewL(_L("MM-MMF-DEVVIDEO-I-1007-HP"), _L("XviDCodecPU"), _L("PlayFile"), KUidTestVideoPuXvidDecoderDevice));
    // DevVideoRecord
    AddTestStepL(RTestDevVideoRecordSimple::NewL(_L("MM-MMF-DEVVIDEO-I-1101-HP"), _L("XviDCodecPU"), _L("RecordFile"), KUidTestVideoPuXvidEncoderDevice));
    AddTestStepL(RTestDevVideoRecordUidHwDev::NewL(_L("MM-MMF-DEVVIDEO-I-1102-HP"), _L("XviDCodecPU"), _L("RecordFile"), EUidTest, KUidTestVideoPuXvidEncoderDevice));
    AddTestStepL(RTestDevVideoRecordConfHwDev::NewL(_L("MM-MMF-DEVVIDEO-I-1103-HP"), _L("XviDCodecPU"), _L("RecordFile"), KUidTestVideoPuXvidEncoderDevice));
    AddTestStepL(RTestDevVideoRecordStop::NewL(_L("MM-MMF-DEVVIDEO-I-1104-HP"), _L("XviDCodecPU"), _L("RecordFile"), KUidTestVideoPuXvidEncoderDevice));
    AddTestStepL(RTestDevVideoRecordPause::NewL(_L("MM-MMF-DEVVIDEO-I-1105-HP"), _L("XviDCodecPU"), _L("RecordFile"), KUidTestVideoPuXvidEncoderDevice));
    AddTestStepL(RTestDevVideoRecordRecordingPos::NewL(_L("MM-MMF-DEVVIDEO-I-1106-HP"), _L("XviDCodecPU"), _L("RecordFile"), KUidTestVideoPuXvidEncoderDevice));
    AddTestStepL(RTestDevVideoRecordEncoderInfo::NewL(_L("MM-MMF-DEVVIDEO-I-1107-HP"), _L("XviDCodecPU"), _L("RecordFile"), KUidTestVideoPuXvidEncoderDevice));
    AddTestStepL(RTestDevVideoRecordAlloc::NewL(_L("MM-MMF-DEVVIDEO-I-1108-HP"), _L("XviDCodecPU"), _L("RecordFile"), KUidTestVideoPuXvidEncoderDevice));

    // NEGATIVE TESTS //

    // DevVideoPlay
    AddTestStepL(RTestDevVideoPlayUidHwDev::NewL(_L("MM-MMF-DEVVIDEO-I-1501-HP"), _L("XviDCodecPU"), _L("PlayFile"), KUidTestVideoPuXvidDecoderDevice));
    AddTestStepL(RTestDevVideoPlayCorruptFile::NewL(_L("MM-MMF-DEVVIDEO-I-1502-HP"), _L("XviDCodecPU"), _L("CorruptPlayFile"), KUidTestVideoPuXvidDecoderDevice));
    AddTestStepL(RTestDevVideoPlayWithoutVideoType::NewL(_L("MM-MMF-DEVVIDEO-I-1503-HP"), _L("XviDCodecPU"), _L("PlayFile"), KUidTestVideoPuXvidDecoderDevice));
    // DevVideoRecord
    AddTestStepL(RTestDevVideoRecordUidHwDev::NewL(_L("MM-MMF-DEVVIDEO-I-1601-HP"), _L("XviDCodecPU"), _L("RecordFile"), EInvalidUidTest, KUidTestVideoPuXvidEncoderDevice));
    AddTestStepL(RTestDevVideoRecordWithoutVideoType::NewL(_L("MM-MMF-DEVVIDEO-I-1602-HP"), _L("XviDCodecPU"), _L("RecordFile"), KUidTestVideoPuXvidEncoderDevice));

    //
    // Video test PU TESTS //
    //
    
    // POSITIVE TESTS //

    // DevVideoPlay
    AddTestStepL(RTestDevVideoPlaySimple::NewL(_L("MM-MMF-DEVVIDEO-I-2001-HP"), _L("VideoStubPU"), _L("PlayFile"), KUidTestVideoDecodeStubPU));
    AddTestStepL(RTestDevVideoPlayConfHwDev::NewL(_L("MM-MMF-DEVVIDEO-I-2002-HP"), _L("VideoStubPU"), _L("PlayFile"), KUidTestVideoDecodeStubPU));
    AddTestStepL(RTestDevVideoPlayStop::NewL(_L("MM-MMF-DEVVIDEO-I-2003-HP"), _L("VideoStubPU"), _L("PlayFile"), KUidTestVideoDecodeStubPU));
    AddTestStepL(RTestDevVideoPlayPause::NewL(_L("MM-MMF-DEVVIDEO-I-2004-HP"), _L("VideoStubPU"), _L("PlayFile"), KUidTestVideoDecodeStubPU));
    AddTestStepL(RTestDevVideoPlayDecodePlaybackPos::NewL(_L("MM-MMF-DEVVIDEO-I-2005-HP"), _L("VideoStubPU"), _L("PlayFile"), KUidTestVideoDecodeStubPU));
    AddTestStepL(RTestDevVideoPlayDecoderInfo::NewL(_L("MM-MMF-DEVVIDEO-I-2006-HP"), _L("VideoStubPU"), _L("PlayFile"), KUidTestVideoDecodeStubPU));
    AddTestStepL(RTestDevVideoPlayAlloc::NewL(_L("MM-MMF-DEVVIDEO-I-2007-HP"), _L("VideoStubPU"), _L("PlayFile"), KUidTestVideoDecodeStubPU));
    // DevVideoRecord
    AddTestStepL(RTestDevVideoRecordSimple::NewL(_L("MM-MMF-DEVVIDEO-I-2101-HP"), _L("VideoStubPU"), _L("RecordFile"), KUidTestVideoEncodeStubPU));
    AddTestStepL(RTestDevVideoRecordUidHwDev::NewL(_L("MM-MMF-DEVVIDEO-I-2102-HP"), _L("VideoStubPU"), _L("RecordFile"), EUidTest, KUidTestVideoEncodeStubPU));
    AddTestStepL(RTestDevVideoRecordConfHwDev::NewL(_L("MM-MMF-DEVVIDEO-I-2103-HP"), _L("VideoStubPU"), _L("RecordFile"), KUidTestVideoEncodeStubPU));
    AddTestStepL(RTestDevVideoRecordStop::NewL(_L("MM-MMF-DEVVIDEO-I-2104-HP"), _L("VideoStubPU"), _L("RecordFile"), KUidTestVideoEncodeStubPU));
    AddTestStepL(RTestDevVideoRecordPause::NewL(_L("MM-MMF-DEVVIDEO-I-2105-HP"), _L("VideoStubPU"), _L("RecordFile"), KUidTestVideoEncodeStubPU));
    AddTestStepL(RTestDevVideoRecordRecordingPos::NewL(_L("MM-MMF-DEVVIDEO-I-2106-HP"), _L("VideoStubPU"), _L("RecordFile"), KUidTestVideoEncodeStubPU));
    AddTestStepL(RTestDevVideoRecordEncoderInfo::NewL(_L("MM-MMF-DEVVIDEO-I-2107-HP"), _L("VideoStubPU"), _L("RecordFile"), KUidTestVideoEncodeStubPU));
    AddTestStepL(RTestDevVideoRecordAlloc::NewL(_L("MM-MMF-DEVVIDEO-I-2108-HP"), _L("VideoStubPU"), _L("RecordFile"), KUidTestVideoEncodeStubPU));
    
    // NEGATIVE TESTS //

    // DevVideoPlay
    AddTestStepL(RTestDevVideoPlayUidHwDev::NewL(_L("MM-MMF-DEVVIDEO-I-2501-HP"), _L("VideoStubPU"), _L("PlayFile"), KUidTestVideoDecodeStubPU));
    AddTestStepL(RTestDevVideoPlayCorruptFile::NewL(_L("MM-MMF-DEVVIDEO-I-2502-HP"), _L("VideoStubPU"), _L("CorruptPlayFile"), KUidTestVideoDecodeStubPU));
    AddTestStepL(RTestDevVideoPlayWithoutVideoType::NewL(_L("MM-MMF-DEVVIDEO-I-2503-HP"), _L("VideoStubPU"), _L("PlayFile"), KUidTestVideoDecodeStubPU));
    // DevVideoRecord
    AddTestStepL(RTestDevVideoRecordUidHwDev::NewL(_L("MM-MMF-DEVVIDEO-I-2601-HP"), _L("VideoStubPU"), _L("RecordFile"), EInvalidUidTest, KUidTestVideoEncodeStubPU));
    AddTestStepL(RTestDevVideoRecordWithoutVideoType::NewL(_L("MM-MMF-DEVVIDEO-I-2602-HP"), _L("VideoStubPU"), _L("RecordFile"), KUidTestVideoEncodeStubPU));

    }

