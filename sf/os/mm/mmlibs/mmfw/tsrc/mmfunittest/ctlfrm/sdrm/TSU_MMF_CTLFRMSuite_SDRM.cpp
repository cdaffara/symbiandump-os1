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
// This is the main DLL entry point for the TSU_MMF_CTLFRM_SDRM.dll
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_CTLFRMSuite_SDRM.h"
#include "TSU_MMF_CTLFRM_SDRM.h"

// Test Uids
#include "ts_testconstructcontroller/cmmftestconstructcontrolleruids.hrh"

const TUid KTstConstructController1Uid = {KMmfTestConstructController1Uid};
const TUid KTstConstructController2Uid = {KMmfTestConstructController2Uid};
const TUid KTstConstructController3Uid = {KMmfTestConstructController3Uid};
/**
 *
 * Create an instance of this test suite.
 *
 * NewTestSuiteL is exported at ordinal 1.
 * This provides the interface to allow the Test Framework
 * to create instances of this test suite.
 *
 * @return	"CTestSuite_MMF_CTLFRM*"
 *			The newly created test suite
 */
EXPORT_C CTestSuite_MMF_CTLFRM_SDRM* NewTestSuiteL() 
    { 
	CTestSuite_MMF_CTLFRM_SDRM* result = new (ELeave) CTestSuite_MMF_CTLFRM_SDRM;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

/**
 *
 * CTestSuiteCTLFRMasses destructor
 *
 * @xxxx
 * 
 */
CTestSuite_MMF_CTLFRM_SDRM::~CTestSuite_MMF_CTLFRM_SDRM()
	{
	}


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
_LIT(KTxtVersion,"0.1");
TPtrC CTestSuite_MMF_CTLFRM_SDRM::GetVersion( void ) const
	{

	return KTxtVersion();
	}

/**
 *
 * Add a test step into the suite.
 *
 * @param	"CTestStepSelfTest* aPtrTestStep"
 *			The test step to be added.
 * @xxxx
 * 
 */


/**
 *
 * Initialiser for test suite.
 * This creates all the test steps and stores them
 * inside CTestSuiteCTLFRMasses
 *
 * @xxxx
 * 
 */
void CTestSuite_MMF_CTLFRM_SDRM::InitialiseL( void )
	{

	// store the name of this test suite
	iSuiteName = _L("TSU_MMF_CTLFRM_SDRM");

	// add test steps
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0001 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0007 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0008 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0009 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0010 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0011 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0012 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0013 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0014 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0015 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0016 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0017 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0018 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0019 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0020 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0021 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0022 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0023 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0024 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0027 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0028 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0029 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0030 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0031 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0032 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0033 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0034 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0035 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0038 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0039 );


	// Standard Custom Commands

	//------------------------------------//
	// Audio Play Device Custom Commands  //
	//------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0040 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0041 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0042 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0043 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0044 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0045 );

	//------------------------------------//
	// Audio Record Device Custom Commands//
	//------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0050 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0051 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0052 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0053 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0054 );

	//---------------------------------------//
	// Audio Play Controller Custom Commands //
	//---------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0060 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0061 );


	//-----------------------------------------//
	// Audio Record Controller Custom Commands //
	//-----------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0070 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0071 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0072 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0073 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0074 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0075 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0076 );

	//----------------------------------//
	// Audio Controller Custom Commands //
	//----------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0080 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0081 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0082 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0083 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0084 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0085 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0086 );


	//-----------------------------------//
	// Adding Multiple Sources and Sinks //
	//-----------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0100 );

	//-------------------------------------//
	// CMMFFormatSelectionParameters       //
	//-------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0120 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0121 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0122 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0123 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0124 );
	
	//------------------------------------------//
	// CMMFControllerPluginSelectionParameters  //
	//------------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0130 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0131 );

	//-------------------------------------//
	// CMMFPluginImplementationInformation //
	//-------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0140 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0141 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0142 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0143 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0144 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0145 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0146 );

	//------------------------------------------//
	// CMMFControllerImplementationInformation  //
	//------------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0150 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0151 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0152 );

	//--------------------------------------//
	// CMMFFormatImplementationInformation  //
	//--------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0160 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0161 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0162 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0163 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0164 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0165 );
	
	//-------------------------------------//
	// CMMFFormatPluginSelectionParameters //  (Decode)
	//-------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0197 );
	
	// OOM test	
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0200 );
	
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_OpenController(_L("MM-MMF-CTLFRM-SDRM-U-0210"),
																		 KTstConstructController2Uid) );
    AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_OpenController(_L("MM-MMF-CTLFRM-SDRM-U-0211"),
																		 KTstConstructController3Uid) );
    AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_OpenController(_L("MM-MMF-CTLFRM-SDRM-U-0212"),
																		 KTstConstructController1Uid) );
    AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_OpenSecureController(_L("MM-MMF-CTLFRM-SDRM-U-0213"),
																		 KTstConstructController2Uid) );
    AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_OpenSecureController(_L("MM-MMF-CTLFRM-SDRM-U-0214"),
																		 KTstConstructController3Uid) );
    AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_OpenSecureController(_L("MM-MMF-CTLFRM-SDRM-U-0215"),
																		 KTstConstructController1Uid) );
    //CR1655 -  MMF support for configurable controller stack size
    AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_SDRM_U_0300());
    }


// -------------------------
