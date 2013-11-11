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
// DevSound and Audio Codec interface definition tests
// 
//

// Test includes
#include "tsi_mmf_devsound_stepbase.h"
#include "tsi_mmf_devsound_suite.h"
#include "tsi_mmf_devsound_record.h"
#include "tsi_mmf_devsound_play.h"

/**
 *
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 *
 */
EXPORT_C CTestSuite* NewTestSuiteL()
    {
    CTestSuiteDevSound* result = new (ELeave) CTestSuiteDevSound;
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
CTestSuiteDevSound::~CTestSuiteDevSound()
    {
    }

/**
 *
 * Get test suite version
 *
 */
TPtrC CTestSuiteDevSound::GetVersion() const
    {
    _LIT(KTxtVersion, "1.0");
    return KTxtVersion();
    }

/**
 *
 * Add a test step into the suite
 *
 */
void CTestSuiteDevSound::AddTestStepL(RTestStepDevSoundBase* aPtrTestStep)
    {
    // Test steps contain a pointer back to the suite which owns them
    aPtrTestStep->SetTestSuite(this);

    // Add the step using the base class method
    CTestSuite::AddTestStepL(aPtrTestStep);
    }

/**
 *
 * Constructor for test suite
 * Creates all the test steps and stores them inside CTestSuiteDevSound
 *
 */
void CTestSuiteDevSound::InitialiseL()
    {
    // Store the name of this test suite
    iSuiteName = _L("TSI_MMF_DEVSOUND");
    
    //TFourCC code declaration
    TFourCC KInvalidDataType('A', 'B', 'C', 'D');
    TFourCC KVORBDataType('V', 'R', 'B', '2');
    //Reuse same test but different TFourCC code for Legacy Devsound and A3f
    //e.g. MM-MMF-DEVSOUND-I-0002-HP
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
    TFourCC KVORBDataType2('V', 'O', 'R', 'B');
#else
    TFourCC KVORBDataType2('V', 'R', 'B', '2');
#endif

    //
    // POSITIVE TESTS //
    //
    
    // DevSound - Play
    AddTestStepL(RTestStepDevSoundPlayDatatype::NewL(_L("MM-MMF-DEVSOUND-I-0001-HP"), _L("SectionOgg"), _L("OggAudioPlay"), KVORBDataType));
    AddTestStepL(RTestStepDevSoundPlayConfHwDev::NewL(_L("MM-MMF-DEVSOUND-I-0002-HP"), _L("SectionOgg"), _L("OggAudioPlay"), KVORBDataType2));
    AddTestStepL(RTestStepDevSoundListInputDataTypes::NewL(_L("MM-MMF-DEVSOUND-I-0003-HP"), _L("SectionOgg"), _L("OggAudioPlay"), KVORBDataType));
    AddTestStepL(RTestStepDevSoundPositionData::NewL(_L("MM-MMF-DEVSOUND-I-0004-HP"), _L("SectionOgg"), _L("OggAudioPlay"), KVORBDataType));
    AddTestStepL(RTestStepDevSoundPlayAlloc::NewL(_L("MM-MMF-DEVSOUND-I-0005-HP"), _L("SectionOgg"), _L("OggAudioPlay"), KVORBDataType));
    AddTestStepL(RTestStepDevSoundPlayStop::NewL(_L("MM-MMF-DEVSOUND-I-0006-HP"), _L("SectionOgg"), _L("OggAudioMain"), KVORBDataType));
    AddTestStepL(RTestStepDevSoundPlayPause::NewL(_L("MM-MMF-DEVSOUND-I-0007-HP"), _L("SectionOgg"), _L("OggAudioMain"), KVORBDataType));
    AddTestStepL(RTestStepDevSoundPlayRecord::NewL(_L("MM-MMF-DEVSOUND-I-0008-HP"), _L("SectionOgg"), _L("OggAudioPlay"), _L("OggAudio0008"), KVORBDataType));

    // DevSound - Record
    AddTestStepL(RTestStepDevSoundRecordDatatype::NewL(_L("MM-MMF-DEVSOUND-I-0101-HP"), _L("SectionOgg"), _L("OggAudio0101Record"), KVORBDataType));
    AddTestStepL(RTestStepDevSoundRecordConfHwDev::NewL(_L("MM-MMF-DEVSOUND-I-0102-HP"), _L("SectionOgg"), _L("OggAudio0102Record"), KVORBDataType));
    AddTestStepL(RTestStepDevSoundRecordAlloc::NewL(_L("MM-MMF-DEVSOUND-I-0103-HP"), _L("SectionOgg"), _L("OggAudio0103Record"), KVORBDataType));
    AddTestStepL(RTestStepDevSoundRecordStop::NewL(_L("MM-MMF-DEVSOUND-I-0104-HP"), _L("SectionOgg"), _L("OggAudio0104Record"), KVORBDataType));
    AddTestStepL(RTestStepDevSoundRecordPause::NewL(_L("MM-MMF-DEVSOUND-I-0105-HP"), _L("SectionOgg"), _L("OggAudio0105Record"), KVORBDataType));
    AddTestStepL(RTestStepDevSoundListOutputDataTypes::NewL(_L("MM-MMF-DEVSOUND-I-0106-HP"), _L("SectionOgg"), _L("OggAudioRecord"), KVORBDataType));


    //
    // NEGATIVE TESTS //
    //

    // DevSound - Play
    AddTestStepL(RTestStepDevSoundPlayDatatype::NewL(_L("MM-MMF-DEVSOUND-I-0501-HP"), _L("SectionOgg"), _L("OggAudioPlay"), KInvalidDataType));
    AddTestStepL(RTestStepDevSoundPlayHwDevUid::NewL(_L("MM-MMF-DEVSOUND-I-0502-HP"), _L("SectionOgg"), _L("OggAudioPlay")));
    AddTestStepL(RTestStepDevSoundPlayHwDevUid::NewL(_L("MM-MMF-DEVSOUND-I-0503-HP"), _L("SectionOgg"), _L("OggAudioPlay")));
    AddTestStepL(RTestStepDevSoundPlayInvBitrate::NewL(_L("MM-MMF-DEVSOUND-I-0504-HP"), _L("SectionOgg"), _L("OggAudioPlay"), KVORBDataType));
    AddTestStepL(RTestStepDevSoundPlayCorruptFile::NewL(_L("MM-MMF-DEVSOUND-I-0505-HP"), _L("SectionOgg"), _L("OggCorrAudioPlay"), KVORBDataType));

    // DevSound - Record
    AddTestStepL(RTestStepDevSoundRecordDatatype::NewL(_L("MM-MMF-DEVSOUND-I-0601-HP"), _L("SectionOgg"), _L("OggAudioRecord"), KInvalidDataType));
    AddTestStepL(RTestStepDevSoundRecordHwDevUid::NewL(_L("MM-MMF-DEVSOUND-I-0602-HP"), _L("SectionOgg"), _L("OggAudioRecord")));
    AddTestStepL(RTestStepDevSoundRecordHwDevUid::NewL(_L("MM-MMF-DEVSOUND-I-0603-HP"), _L("SectionOgg"), _L("OggAudioRecord")));
    AddTestStepL(RTestStepDevSoundRecordInvBitrate::NewL(_L("MM-MMF-DEVSOUND-I-0604-HP"), _L("SectionOgg"), _L("OggAudio0604Record"), KVORBDataType));
    AddTestStepL(RTestStepDevSoundRecordCorruptFile::NewL(_L("MM-MMF-DEVSOUND-I-0605-HP"), _L("SectionOgg"), _L("OggCorrAudioRecord"), KVORBDataType));
    }

