// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// System includes
#include <e32base.h>

// Test system includes
#include "TSI_MMF_SDEVSOUND_SUITE.h"
#include "TSI_MMF_SDEVSOUND_STEP.h"
#include "TSI_MMF_SDEVSOUND_0002_STEP.h"
#include "TSI_MMF_SDEVSOUND_0004_STEP.h"
#include "TSI_MMF_SDEVSOUND_0006_STEP.h"
#include "TSI_MMF_SDEVSOUND_0008_STEP.h"
#include "TSI_MMF_SDEVSOUND_0010_STEP.h"
#include "TSI_MMF_SDEVSOUND_0036_STEP.h"



/** 
 *
 * NewTestSuiteL
 *	NewTestSuite is exported at ordinal 1
 *	this provides the interface to allow schedule test
 *	to create instances of this test suite
 * @result CTestSuiteSDevSound*
 *
 */
EXPORT_C CTestSuiteSDevSound* NewTestSuiteL() 
	{ 
	CTestSuiteSDevSound* result = new (ELeave) CTestSuiteSDevSound;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
	}

/**
 *
 * ~CTestSuiteSDevSound
 *
 */
CTestSuiteSDevSound::~CTestSuiteSDevSound() 
	{}

/**
 *
 * GetVersion
 *	Get Test Suite version
 * @result TPtrC
 *
 */
TPtrC CTestSuiteSDevSound::GetVersion() const
	{
	_LIT(KTxtVersion,"1.08");
	return KTxtVersion();
	}


/**
 *
 * AddTestStepL
 *	Add a test step into the suite
 * @param aPtrTestStep
 *	Test step pointer
 *
 */
void CTestSuiteSDevSound::AddTestStepL( CTestStepSDevSound* aPtrTestStep )
	{
	// test steps contain a pointer back to the suite which owns them
	aPtrTestStep->SetTestSuite(this);

	// add the step using the base class method
	CTestSuite::AddTestStepL(aPtrTestStep);
	}

/**
 *
 * InitialiseL
 *	Constructor for test suite
 *	this creates all the test steps and 
 *	stores them inside CTestSuiteSDevSound
 *
 */
void CTestSuiteSDevSound::InitialiseL()
	{
	// store the name of this test suite 
	iSuiteName = _L("TSI_MMF_SDEVSOUND");

	//CTestStepSDevSoundPlaySimpleTone
	AddTestStepL(new(ELeave) CTestStepSDevSoundPlaySimpleTone(_L("MM-MMF-SDEVSOUND-I-0001-HP")) );
	
	//CTestStepSDevSoundPlayDTMFTones
	AddTestStepL(new(ELeave) CTestStepSDevSoundPlayDTMFTones(_L("MM-MMF-SDEVSOUND-I-0003-HP")) );
	
	//CTestStepSDevSoundPlayDualTone
	AddTestStepL(new(ELeave) CTestStepSDevSoundPlayDualTone(_L("MM-MMF-SDEVSOUND-I-0005-HP")) );
	
	//CTestStepSDevSoundPlayEOFPCM16
	AddTestStepL(new(ELeave) CTestStepSDevSoundPlayEOFPCM16(_L("MM-MMF-SDEVSOUND-I-0007-HP")) );
	
	//CTestStepSDevSoundPlayFile
	AddTestStepL(new(ELeave) CTestStepSDevSoundPlayFile (_L("MM-MMF-SDEVSOUND-I-0011-HP"),_L("PCM8")) );
	
		
	AddTestStepL(CTestStepSDevSoundPlaySimpleToneCap::NewL(_L("MM-MMF-SDEVSOUND-I-0002-HP"),_L("SectionOne"),_L("playerAudioFile") ) );
	AddTestStepL(CTestStepSDevSoundPlayDTMFToneCap::NewL(_L("MM-MMF-SDEVSOUND-I-0004-HP"),_L("SectionOne"),_L("playerAudioFile") ) );
	AddTestStepL(CTestStepSDevSoundPlayDualToneCap::NewL(_L("MM-MMF-SDEVSOUND-I-0006-HP"),_L("SectionOne"),_L("playerAudioFile") ) );
	AddTestStepL(CTestStepSDevSoundPlayDataCap::NewL(_L("MM-MMF-SDEVSOUND-I-0008-HP"),_L("SectionOne"),_L("playerAudioFile") ) );
	AddTestStepL(CTestStepSDevSoundRecordDataCap::NewL(_L("MM-MMF-SDEVSOUND-I-0010-HP"),_L("SectionOne"),_L("playerAudioFile") ) );

	//CTestStepSDevSoundRecordFile when the requesting client (testframework) has No capabilities
	AddTestStepL(new(ELeave) CTestStepSDevSoundRecordFileNoCap(_L("MM-MMF-SDEVSOUND-I-0013-HP"),_L("SectionTen"),_L("outputAudioFile"),CTestStepSDevSoundRecordFile::EPcm8,ETrue) );
	//CTestStepSDevSoundRecordFile when the requesting client (testframework) has MultimediaDD capability
	AddTestStepL(new(ELeave) CTestStepSDevSoundRecordFileNoCap(_L("MM-MMF-SDEVSOUND-I-0021-HP"),_L("SectionTen"),_L("outputAudioFile"),CTestStepSDevSoundRecordFile::EPcm8,ETrue) );
	//CTestStepSDevSoundRecordFile when the requesting client (testframework) has UserEnvironment capability
	AddTestStepL(new(ELeave) CTestStepSDevSoundRecordFile(_L("MM-MMF-SDEVSOUND-I-0022-HP"),_L("SectionTen"),_L("outputAudioFile"),CTestStepSDevSoundRecordFile::EPcm8,ETrue) );
	//CTestStepSDevSoundRecordFile when the requesting client (testframework) has ALL -TCB capabilities
	AddTestStepL(new(ELeave) CTestStepSDevSoundRecordFile(_L("MM-MMF-SDEVSOUND-I-0023-HP"),_L("SectionTen"),_L("outputAudioFile"),CTestStepSDevSoundRecordFile::EPcm8,ETrue) );
	
	//Panic Tests : Try to play/record buffer without initializing play/record at the first place
	AddTestStepL(new(ELeave) CTestStepSDevSoundPlayWithoutInitialize(_L("MM-MMF-SDevSound-I-0017-HP")) );
	AddTestStepL(new(ELeave) CTestStepSDevSoundRecordWithoutInitialize(_L("MM-MMF-SDevSound-I-0018-HP")) );	
	
	//CTestStepSDevSoundClientThreadCap
	AddTestStepL(new(ELeave) CTestStepSDevSoundClientThreadCap(_L("MM-MMF-SDEVSOUND-I-0035-HP")) );
	//CTestStepSDevSoundClientThreadNoCap
	AddTestStepL(CTestStepSDevSoundClientThreadNoCap::NewL(_L("MM-MMF-SDEVSOUND-I-0036-HP"), _L("SecDevSndTS0036"), _L("SectionOne"),_L("playerAudioFile"), EFalse ) );
	//CTestStepSDevSoundClientThreadNoCap - neg case with CI call prior to SetClientThreadInfo() - should fail
	AddTestStepL(CTestStepSDevSoundClientThreadNoCap::NewL(_L("MM-MMF-SDEVSOUND-I-1036-HP"), _L("SecDevSndTS0036"), _L("SectionOne"),_L("playerAudioFile"), ETrue ) );
	//CTestStepSDevSoundClientThreadInvalidId
	AddTestStepL(new(ELeave) CTestStepSDevSoundClientThreadInvalidId(_L("MM-MMF-SDEVSOUND-I-0037-HP")) );
	
	}
	












