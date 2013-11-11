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
// This is the main DLL entry point for the TSU_MMF_CTLFRM.dll
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_CTLFRMSuite.h"
#include "TSU_MMF_CTLFRM.h"



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
EXPORT_C CTestSuite_MMF_CTLFRM* NewTestSuiteL() 
    { 
	CTestSuite_MMF_CTLFRM* result = new (ELeave) CTestSuite_MMF_CTLFRM;
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
CTestSuite_MMF_CTLFRM::~CTestSuite_MMF_CTLFRM()
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
TPtrC CTestSuite_MMF_CTLFRM::GetVersion( void ) const
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
void CTestSuite_MMF_CTLFRM::InitialiseL( void )
	{

	// store the name of this test suite
	iSuiteName = _L("TSU_MMF_CTLFRM");

	// add test steps
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0001 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0007 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0008 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0009 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0010 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0011 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0012 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0013 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0014 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0015 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0016 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0017 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0018 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0019 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0020 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0021 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0022 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0023 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0024 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0027 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0028 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0029 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0030 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0031 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0032 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0033 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0034 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0035 );


	// Standard Custom Commands

	//------------------------------------//
	// Audio Play Device Custom Commands  //
	//------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0040 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0041 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0042 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0043 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0044 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0045 );

	//------------------------------------//
	// Audio Record Device Custom Commands//
	//------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0050 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0051 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0052 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0053 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0054 );

	//---------------------------------------//
	// Audio Play Controller Custom Commands //
	//---------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0060 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0061 );


	//-----------------------------------------//
	// Audio Record Controller Custom Commands //
	//-----------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0070 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0071 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0072 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0073 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0074 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0075 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0076 );

	//----------------------------------//
	// Audio Controller Custom Commands //
	//----------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0080 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0081 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0082 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0083 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0084 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0085 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0086 );

	//---------------------------------------//
	// Video Surface support Custom Commands //
	//---------------------------------------//
#ifdef SYMBIAN_BUILD_GCE
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0301 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0303 );
	TSurfaceId surfaceId;
	surfaceId.iInternal[0] = 11111;
	surfaceId.iInternal[1] = 22222;
	surfaceId.iInternal[2] = 33333;
	surfaceId.iInternal[3] = 44444;
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0305(surfaceId, EFalse) );
	// negative test
	surfaceId.iInternal[0] = 88888;
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0305(surfaceId, ETrue) );	
#endif // SYMBIAN_BUILD_GCE
	
	//----------------------------------//
	// Subtitle support Custom Commands //
	//----------------------------------//
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	AddTestStepL(new(ELeave) CTestStep_MMF_CTRLFRM_U_400());
	AddTestStepL(new(ELeave) CTestStep_MMF_CTRLFRM_U_401());
	AddTestStepL(new(ELeave) CTestStep_MMF_CTRLFRM_U_402());
	AddTestStepL(new(ELeave) CTestStep_MMF_CTRLFRM_U_403());
	AddTestStepL(new(ELeave) CTestStep_MMF_CTRLFRM_U_404());
	AddTestStepL(new(ELeave) CTestStep_MMF_CTRLFRM_U_405());
	AddTestStepL(new(ELeave) CTestStep_MMF_CTRLFRM_U_406());
	AddTestStepL(new(ELeave) CTestStep_MMF_CTRLFRM_U_407());
	AddTestStepL(new(ELeave) CTestStep_MMF_CTRLFRM_U_408());
	AddTestStepL(new(ELeave) CTestStep_MMF_CTRLFRM_U_409());
	AddTestStepL(new(ELeave) CTestStep_MMF_CTRLFRM_U_410());
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

	//-----------------------------------//
	// Adding Multiple Sources and Sinks //
	//-----------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0100 );

	//-------------------------------------//
	// CMMFFormatSelectionParameters       //
	//-------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0120 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0121 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0122 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0123 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0124 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0125 );

	//------------------------------------------//
	// CMMFControllerPluginSelectionParameters  //
	//------------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0130 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0131 );

	//-------------------------------------//
	// CMMFPluginImplementationInformation //
	//-------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0140 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0141 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0142 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0143 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0144 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0145 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0146 );

	//------------------------------------------//
	// CMMFControllerImplementationInformation  //
	//------------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0150 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0151 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0152 );

	//--------------------------------------//
	// CMMFFormatImplementationInformation  //
	//--------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0160 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0161 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0162 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0163 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0164 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0165 );

    //-------------------------------------//
	// CMMFFormatPluginSelectionParameters //  (Encode)
	//-------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0170 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0171 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0172 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0173 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0174 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0175 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0176 );


	//-----------------------------------------------//
	// CMMFFormatPluginSelectionParameters, Negative // 
	//-----------------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0180 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0181 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0182 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0183 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0184 );


	//-------------------------------------//
	// CMMFFormatPluginSelectionParameters //  (Decode)
	//-------------------------------------//
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0190 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0191 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0192 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0193 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0194 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0195 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0196 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0197 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0198 );
	
	
	//--------------------------------//
	// CMMFTestTerminationController  //
	//--------------------------------//
	AddTestStepL( new(ELeave) RTestStep_MMF_CTLFRM_U_0200 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0201 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0202 );
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0203 );

	//CR1655 -  MMF support for configurable controller stack size
	AddTestStepL( new(ELeave) CTestStep_MMF_CTLFRM_U_0300 );
	}


// -------------------------
