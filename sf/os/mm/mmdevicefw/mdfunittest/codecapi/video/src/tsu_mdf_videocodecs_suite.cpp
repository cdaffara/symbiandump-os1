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
#include "tsu_mdf_videocodecs_suite.h"
#include "tsu_mdf_videocodecs_step.h"
#include "tsu_mdf_videocodecs_play.h"
#include "tsu_mdf_videocodecs_record.h"
#include "tsu_mdf_videocodecs_play_cov.h"
#include "tsu_mdf_videocodecs_record_cov.h"

/** 
 *
 * NewTestSuiteL
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 * @result CTestSuiteVideoCodecs*
 *
 */
EXPORT_C CTestSuiteVideoCodecs* NewTestSuiteL() 
	{ 
	CTestSuiteVideoCodecs* result = new (ELeave) CTestSuiteVideoCodecs;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(result);
	return result; 
	}

/**
 *
 * CTestSuiteVideoCodecs
 *
 */
CTestSuiteVideoCodecs::CTestSuiteVideoCodecs() 
	{	
	}


/**
 *
 * ~CTestSuiteVideoCodecs
 *
 */
CTestSuiteVideoCodecs::~CTestSuiteVideoCodecs() 
	{
	}

/**
 *
 * GetVersion
 * Get Test Suite version
 * @result TPtrC
 *
 */
TPtrC CTestSuiteVideoCodecs::GetVersion( void ) const
	{
	_LIT(KTxtVersion,"1.08");
	return KTxtVersion();
	}



/**
 *
 *  InitialiseL
 *	Constructor for test suite
 *	this creates all the test steps and 
 *	stores them inside CTestSuiteVideoCodecs
 *
 */
void CTestSuiteVideoCodecs::InitialiseL(void)
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_MDF_VIDEOCODECS");
	
	// Load Video Decoder
	AddTestStepL(new(ELeave) RTestStepVideoDecoderLoad());
	// Play a File with the Video Decoder
	AddTestStepL(new(ELeave) RTestStepVideoDecoderPlay());
	// Play a File with the Video Decoder using DSA
	AddTestStepL(new(ELeave) RTestStepVideoDecoderPlayDSA());
	// Load Video Encoder
	AddTestStepL(new(ELeave) RTestStepVideoEncoderLoad());
	// Record a File with the Video Encoder
	AddTestStepL(new(ELeave) RTestStepVideoEncoderRecord());
	// Better Code Coverage for encoder
	AddTestStepL(new(ELeave) RTestStepVideoEncoderCov());
	// Better Code Coverage for encoder
	AddTestStepL(new(ELeave) RTestStepVideoEncoderNegativeCov());
	// Better Code Coverage for decoder
	AddTestStepL(new(ELeave) RTestStepVideoDecoderPlayCov());
	// Better Code Coverage for decoder
	AddTestStepL(new(ELeave) RTestStepVideoDecoderPlayDSACov());
	// Better Code Coverage for decoder
	AddTestStepL(new(ELeave) RTestStepVideoDecoderPlayNegTestsCov(1,_L("MM-MDF-VIDEOCODECS-0207")));
	AddTestStepL(new(ELeave) RTestStepVideoDecoderPlayNegTestsCov(2,_L("MM-MDF-VIDEOCODECS-0208")));
	AddTestStepL(new(ELeave) RTestStepVideoDecoderPlayNegTestsCov(3,_L("MM-MDF-VIDEOCODECS-0209")));	
	//add the test cases to call the panic functions
	AddTestStepL(new(ELeave) RTestStepVideoDecoderPanicCov(1,_L("MM-MDF-VIDEOCODECS-0203")));
	//add the test cases to call the panic functions
	AddTestStepL(new(ELeave) RTestStepVideoDecoderPanicCov(2,_L("MM-MDF-VIDEOCODECS-0204")));
	//add the test cases to call the panic functions
	AddTestStepL(new(ELeave) RTestStepVideoDecoderPanicCov(3,_L("MM-MDF-VIDEOCODECS-0205")));
	//add the test cases to call the panic functions
	AddTestStepL(new(ELeave) RTestStepVideoDecoderPanicCov(4,_L("MM-MDF-VIDEOCODECS-0206")));

	}
	
	



	





