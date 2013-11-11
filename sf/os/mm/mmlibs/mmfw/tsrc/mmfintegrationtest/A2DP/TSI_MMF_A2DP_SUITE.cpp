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
#include "TSI_MMF_A2DP_SUITE.h"
#include "TSI_MMF_A2DP_STEP.h"
#include "TSI_MMF_A2DP_DS.h"
#include "TSI_MMF_A2DP_AC.h"


/** 
 *
 * NewTestSuiteL
 *	NewTestSuite is exported at ordinal 1
 *	this provides the interface to allow schedule test
 *	to create instances of this test suite
 * @result CTestSuiteA2dp*
 *
 */
EXPORT_C CTestSuiteA2dp* NewTestSuiteL() 
	{ 
	CTestSuiteA2dp* result = new (ELeave) CTestSuiteA2dp;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
	}

/**
 *
 * ~CTestSuiteA2dp
 *
 */
CTestSuiteA2dp::~CTestSuiteA2dp() 
	{}

/**
 *
 * GetVersion
 *	Get Test Suite version
 * @result TPtrC
 *
 */
TPtrC CTestSuiteA2dp::GetVersion() const
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
void CTestSuiteA2dp::AddTestStepL( RTestStepA2dp* aTestStep )
	{
	// test steps contain a pointer back to the suite which owns them
	aTestStep->SetTestSuite(this);
	
	// add the step using tyhe base class method
	CTestSuite::AddTestStepL(aTestStep);
	}


/**
 *
 * InitialiseL
 *	Constructor for test suite
 *	this creates all the test steps and 
 *	stores them inside CTestSuiteA2dp
 *
 */
void CTestSuiteA2dp::InitialiseL()
	{
	// store the name of this test suite 
	iSuiteName = _L("TSI_MMF_A2DP");

//DevSound
	AddTestStepL(new (ELeave) RTestStepA2dpInitializeHeadset(_L("MM-MMF-A2DP-I-0001-HP")) );
	AddTestStepL(new (ELeave) RTestStepA2dpReinitialDisconnectHeadset(_L("MM-MMF-A2DP-I-0002-HP")) );
	AddTestStepL(new (ELeave) RTestStepA2dpReinitializeHeadset(_L("MM-MMF-A2DP-I-0003-HP")) );
	
	//At the moment, the headset cannot be disconnected during initialization.
	//So no implementation for this test has been done.
	//AddTestStepL(new (ELeave) RTestStepA2dpDisconnectInitializeHeadset(_L("MM-MMF-A2DP-I-0004-HP")) );
	
	//The GetSupportDataType has not been implemented yet.
	//So no implementation for this test has been done.
	//AddTestStepL(new (ELeave) RTestStepA2dpGetSupportDataType(_L("MM-MMF-A2DP-I-0006-HP")) );

	AddTestStepL(new (ELeave) RTestStepA2dpCapabilities(_L("MM-MMF-A2DP-I-0007-HP")) );
	AddTestStepL(new (ELeave) RTestStepA2dpPlayInit(_L("MM-MMF-A2DP-I-0008-HP")) );
	AddTestStepL(new (ELeave) RTestStepA2dpPlayEofPcm16(_L("MM-MMF-A2DP-I-0011-HP")) );
	AddTestStepL(new (ELeave) RTestStepA2dpPlayEofPcm8(_L("MM-MMF-A2DP-I-0014-HP")) );
	AddTestStepL(new (ELeave) RTestStepA2dpDisconnectPlay(_L("MM-MMF-A2DP-I-0016-HP")) );
	AddTestStepL(new (ELeave) RTestStepA2dpPlaySimpleTone(_L("MM-MMF-A2DP-I-0023-HP")) );
	AddTestStepL(new (ELeave) RTestStepA2dpTwoThread(_L("MM-MMF-A2DP-I-0025-HP")) );

//Aclnt
	//This test should be removed. Cos the bluetooth initialization starts in the PlayL() function,
	//not the NewL() function.
	AddTestStepL(new (ELeave) RTestStepA2dpAclPlyNew(_L("MM-MMF-A2DP-I-0200-HP")) );
	
	//PlayL() function doesn't work at the moment due to the Audio Controller problem.
	//So no implementation for this test has been done.
	//AddTestStepL(new (ELeave) RTestStepA2dpAclPlyPlay(_L("MM-MMF-A2DP-I-0201-HP")) );
	//AddTestStepL(new (ELeave) RTestStepA2dpAclPlyStopAndContPlay(_L("MM-MMF-A2DP-I-0214-HP")) );
	//AddTestStepL(new (ELeave) RTestStepA2dpAclPlyPauseAndContPlay(_L("MM-MMF-A2DP-I-0215-HP")) );

	//This test should be removed. Cos the bluetooth initialization starts in the PlayL() function,
	//not the NewL() function.
	AddTestStepL(new (ELeave) RTestStepA2dpAclPlyOpenCloseOpen(_L("MM-MMF-A2DP-I-0216-HP")) );
	
	//This test should be removed. Cos the bluetooth initialization starts in the PlayL() function,
	//not the NewL() function.
	AddTestStepL(new (ELeave) RTestStepA2dpAclPlyReInitDisconn(_L("MM-MMF-A2DP-I-0217-HP")) );
	
	//This test should be removed. Cos the bluetooth initialization starts in the PlayL() function,
	//not the NewL() function.
	AddTestStepL(new (ELeave) RTestStepA2dpAclPlyReInit(_L("MM-MMF-A2DP-I-0218-HP")) );

	//PlayL() function doesn't work at the moment due to the Audio Controller problem.
	//So no implementation for this test has been done.
	//AddTestStepL(new (ELeave) RTestStepA2dpAclPlyDisconnInit(_L("MM-MMF-A2DP-I-0219-HP")) );
	//AddTestStepL(new (ELeave) RTestStepA2dpAclPlyDisconnPlay(_L("MM-MMF-A2DP-I-0220-HP")) );

	//This test should be removed. Cos the bluetooth initialization starts in the PlayL() function,
	//not the NewL() function.
	AddTestStepL(new (ELeave) RTestStepA2dpAclRecNew(_L("MM-MMF-A2DP-I-0300-HP")) );

	//This test should be removed. Cos the bluetooth initialization starts in the PlayL() function,
	//not the NewL() function.
	AddTestStepL(new (ELeave) RTestStepA2dpAclRecOpenCloseOpen(_L("MM-MMF-A2DP-I-0301-HP")) );

	//PlayL() function doesn't work at the moment due to the Audio Controller problem.
	//So no implementation for this test has been done.
	//AddTestStepL(new (ELeave) RTestStepA2dpAclRecPlay(_L("MM-MMF-A2DP-I-0302-HP")) );

	//This test should be removed. Cos the bluetooth initialization starts in the PlayL() function,
	//So no implementation for this test has been done.
	AddTestStepL(new (ELeave) RTestStepA2dpAclToneNew(_L("MM-MMF-A2DP-I-0400-HP")) );

	//PlayL() function doesn't work at the moment due to the Audio Controller problem.
	//So no implementation for this test has been done.
	//AddTestStepL(new (ELeave) RTestStepA2dpAclTonePlayDTMF(_L("MM-MMF-A2DP-I-0402-HP")) );

	AddTestStepL(new (ELeave) RTestStepA2dpAclStrmNew(_L("MM-MMF-A2DP-I-0500-HP")) );
	AddTestStepL(new (ELeave) RTestStepA2dpAclStrmWrite(_L("MM-MMF-A2DP-I-0501-HP")) );
	AddTestStepL(new (ELeave) RTestStepA2dpAclStrmWriteStop(_L("MM-MMF-A2DP-I-0502-HP")) );	
	
//Negative tests
	AddTestStepL(new (ELeave) RTestStepA2dpInitializeHeadsetNeg(_L("MM-MMF-A2DP-I-2001-HP")) );
	AddTestStepL(new (ELeave) RTestStepA2dpAclStrmNewNeg(_L("MM-MMF-A2DP-I-2002-HP")) );
	
	//PlayL() function doesn't work at the moment due to the Audio Controller problem.
	//So no implementation for this test has been done.
	//AddTestStepL(new (ELeave) RTestStepA2dpAclPlyPlayNeg(_L("MM-MMF-A2DP-I-02003-HP")) );
	//AddTestStepL(new (ELeave) RTestStepA2dpAclRecPlayNeg(_L("MM-MMF-A2DP-I-02004-HP")) );
	//AddTestStepL(new (ELeave) RTestStepA2dpAclTonePlayNeg(_L("MM-MMF-A2DP-I-02005-HP")) );
	}
	



