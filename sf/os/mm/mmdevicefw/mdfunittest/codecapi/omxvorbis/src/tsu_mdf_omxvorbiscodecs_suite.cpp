// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

// System includes
#include <e32base.h>

// Test system includes
#include "tsu_mdf_omxvorbiscodecs_suite.h"
#include "tsu_mdf_omxvorbiscodecs_step.h"
#include "tsu_mdf_omxvorbiscodecs_play.h"
#include "tsu_mdf_omxvorbiscodecs_record.h"
#include "tsu_mdf_omxvorbiscodecs_pu.h"
#include "tsu_mdf_omxvorbiscodecs_omx.h"

/** 
 *
 * NewTestSuiteL
 *	NewTestSuite is exported at ordinal 1
 *	this provides the interface to allow schedule test
 *	to create instances of this test suite
 * @result CTestSuiteOmxVorbisCodecs*
 *
 */
EXPORT_C CTestSuiteOmxVorbisCodecs* NewTestSuiteL() 
	{ 
	CTestSuiteOmxVorbisCodecs* result = new (ELeave) CTestSuiteOmxVorbisCodecs;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
	}

/**
 *
 * CTestSuiteOmxVorbisCodecs
 *
 */
CTestSuiteOmxVorbisCodecs::CTestSuiteOmxVorbisCodecs() 
	{
	
	}


/**
 *
 * ~CTestSuiteOmxVorbisCodecs
 *
 */
CTestSuiteOmxVorbisCodecs::~CTestSuiteOmxVorbisCodecs() 
	{}

/**
 *
 * GetVersion
 *	Get Test Suite version
 * @result TPtrC
 *
 */
TPtrC CTestSuiteOmxVorbisCodecs::GetVersion( void ) const
	{
	_LIT(KTxtVersion,"1.00");
	return KTxtVersion();
	}



/**
 *
 * InitialiseL
 *	Constructor for test suite
 *	this creates all the test steps and 
 *	stores them inside CTestSuiteOmxVorbisCodecs
 *
 */
void CTestSuiteOmxVorbisCodecs::InitialiseL(void)
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_MDF_OMXVORBISCODECS");
	// play a file
	AddTestStepL(new(ELeave) RTestStepOmxVorbisCodecPlay());
	// record a file
	AddTestStepL(new(ELeave) RTestStepOmxVorbisCodecRecord());
	
	// general tests for PU
	AddTestStepL(new(ELeave) RTestStepOmxVorbisCodecPu());
	// general tests for OMX layer
	AddTestStepL(new(ELeave) RTestStepOmxVorbisCodecOmx());
	
	}
	
	



	





