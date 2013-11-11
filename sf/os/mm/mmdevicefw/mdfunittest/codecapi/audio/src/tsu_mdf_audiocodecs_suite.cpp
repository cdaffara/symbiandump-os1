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

// System includes
#include <e32base.h>

// Test system includes
#include "tsu_mdf_audiocodecs_suite.h"
#include "tsu_mdf_audiocodecs_step.h"
#include "tsu_mdf_audiocodecs_load.h"
#include "tsu_mdf_audiocodecs_play.h"
#include "tsu_mdf_audiocodecs_record.h"
#include "tsu_mdf_audiocodecs_resolver.h"

/** 
 *
 * NewTestSuiteL
 *	NewTestSuite is exported at ordinal 1
 *	this provides the interface to allow schedule test
 *	to create instances of this test suite
 * @result CTestSuiteAudioCodecs*
 *
 */
EXPORT_C CTestSuiteAudioCodecs* NewTestSuiteL() 
	{ 
	CTestSuiteAudioCodecs* result = new (ELeave) CTestSuiteAudioCodecs;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
	}

/**
 *
 * CTestSuiteAudioCodecs
 *
 */
CTestSuiteAudioCodecs::CTestSuiteAudioCodecs() 
	{
	
	}


/**
 *
 * ~CTestSuiteAudioCodecs
 *
 */
CTestSuiteAudioCodecs::~CTestSuiteAudioCodecs() 
	{}

/**
 *
 * GetVersion
 *	Get Test Suite version
 * @result TPtrC
 *
 */
TPtrC CTestSuiteAudioCodecs::GetVersion( void ) const
	{
	_LIT(KTxtVersion,"1.08");
	return KTxtVersion();
	}



/**
 *
 * InitialiseL
 *	Constructor for test suite
 *	this creates all the test steps and 
 *	stores them inside CTestSuiteAudioCodecs
 *
 */
void CTestSuiteAudioCodecs::InitialiseL(void)
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_MDF_AUDIOCODECS");
	// initialize default pcm16 play
	AddTestStepL(new(ELeave) RTestStepAudioCodecLoad());
	// play a file
	AddTestStepL(new(ELeave) RTestStepAudioCodecPlay());
	// record a file
	AddTestStepL(new(ELeave) RTestStepAudioCodecRecord());
	
	// RESOLVER Test Cases
	// load a PU using the resolver
	AddTestStepL(new(ELeave) RTestStepAudioCodecLoadFourCC);
	// load a P8 PU using the resolver source data and version only
	AddTestStepL(new(ELeave) RTestStepAudioCodecFindP8InputFourCC);
	// load a PU8 PU using the resolver destination data and version only
	AddTestStepL(new(ELeave) RTestStepAudioCodecFindPU8OutputFourCC);
	// attempt to load a PU using the correct version but incorrect source info
	AddTestStepL(new(ELeave) RTestStepAudioCodecFindIncorrectInputFourCC);			
	// attempt to load a PU using the correct version, src, dest and interface UID but incorrect implementation UID
	AddTestStepL(new(ELeave) RTestStepAudioCodecFindIncorrectImpUid);			
	// attempt to load a PU using the correct version, src, dest and implementation UID, but incorrect interface UID
	AddTestStepL(new(ELeave) RTestStepAudioCodecFindIncorrectInterfaceUid);	
	// parse opaque data containing <m>, <p> and <l> tags
	AddTestStepL(new(ELeave) RTestStepVideoResolverUtils);	
	// parse opaque data missing one of the expected tags
	AddTestStepL(new(ELeave) RTestStepResolverUtils);	
	}
	
	



	





